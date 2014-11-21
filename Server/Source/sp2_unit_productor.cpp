/**************************************************************
*
* sp2_unit_productor.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_unit_productor.h"

// Uncomment to log information about shipped units
//#define LOG_SHIP_UNITS






CWSTR c_sMaxProductionPerDayMoney = L"MaxProductionMoneyPerDay";
const REAL32 c_fDefaultMaxProductionPerDayMoney = 2000000.f;

GProductionQueue::GProductionQueue()
{
   m_fProductionCapacity = 0;
   m_bProductionStatusIsDirty = false;
}

GUnitProductionRequest::GUnitProductionRequest()
{
      m_iID                      = 0;
      m_iQtyWanted               = 0;
      m_iQtyShipped              = 0;
      m_iDesignID                = 0;
      m_iBuilderCountryID        = 0;
      m_iDestinationCountryID    = 0;
      m_fGameTimeInProduction    = 0;
      m_fQtyShippedAccumulated   = 0;
      m_fBonusFromTech           = 1;
}

GUnitProductionRequest::~GUnitProductionRequest()
{
}



/*!
 * Constructor
 **/
GUnitProductor::GUnitProductor()
{
   m_iNextProductionRequestID   = 1;
   m_iLastIterationGameTime     = 0;
}

/*!
 * Destructor
 **/
GUnitProductor::~GUnitProductor()
{
}

/*!
 * Initializes the unit productor
 **/
bool GUnitProductor::Initialize()
{
   g_ServerDAL.DataManager()->RegisterNode(this);
   return true;
}

void GUnitProductor::Shutdown()
{
   m_vProductionQueues.clear();
   m_iNextProductionRequestID   = 1;
   m_iLastIterationGameTime     = g_Joshua.GameTime();
}

/*!
 * Iterate the unit production for every country
 **/
void GUnitProductor::Iterate()
{
   //Game Time Span since last iteration 
   REAL64 l_fActualClock = g_Joshua.GameTime();
   REAL32 l_fTimeSpan      = (REAL32)(l_fActualClock - m_iLastIterationGameTime);
   gassert(l_fTimeSpan >= 0,"Time should be positive");

   if(l_fTimeSpan < 0.0001f)
      return;

   // Clean up , all production status
   CleanProductionStatus();

   //Process the production queues for every country
   UINT16 l_iCountryID = 1;
   for(vector<GProductionQueue>::iterator l_It = m_vProductionQueues.begin() ; 
      l_It != m_vProductionQueues.end() ; 
      l_It++,l_iCountryID++)
   {

      // Calculate number of production point available for this country (Production Capacity)
      REAL32 l_fTotalProductionPoint = (REAL32)(log10(g_ServerDAL.CountryData(l_iCountryID)->GDPValue() / 1000000000.0));
      REAL32 l_fBonusForInfrastructure = -1 + 3 * g_ServerDAL.CountryData(l_iCountryID)->Infrastructure();
      if(l_fTotalProductionPoint > 0.5)
      {
         l_fTotalProductionPoint = max(0.5f, l_fTotalProductionPoint + l_fBonusForInfrastructure);
      }
      else if(l_fBonusForInfrastructure > 0)
      {
         l_fTotalProductionPoint += l_fBonusForInfrastructure;
      }



      REAL32 l_fRemainingProductionPoint = l_fTotalProductionPoint;
      REAL32 l_fProductionPointToUse = l_fRemainingProductionPoint;

      //Now, for every request in the production list until we don't have any production point or request.
      list<GUnitProductionRequest>::iterator l_RequestIt = (*l_It).m_ActiveQueue.begin();
      while(l_RequestIt != (*l_It).m_ActiveQueue.end())
      {

         GUnitProductionRequest *l_pRequest = &(*l_RequestIt);

         // Update this production time
         l_pRequest->m_fGameTimeInProduction += l_fTimeSpan;


         bool l_bThisProductionRequestNeedRefresh  = false;
         bool l_bRequestHasBeenDestroy             = false;

         // Don't process request that are OnHold.
         if( l_pRequest->m_ePriority == EUnitProductionPriority::OnHold)
         {
            // If this unit was build before, its not anymore, better tell the client
            if(l_pRequest->m_bUnitIsBeingBuilt == true)
               l_bThisProductionRequestNeedRefresh = true;

            // This unit is not going to be build, thats for sure.
            l_pRequest->m_bUnitIsBeingBuilt = false;
         }
         else
         // If we still have produciton point, let spend them
         if(l_fRemainingProductionPoint>0) 
         {
            // If we change status about this request , update client
            if(l_pRequest->m_bUnitIsBeingBuilt == false)
               l_bThisProductionRequestNeedRefresh = true;
            l_pRequest->m_bUnitIsBeingBuilt = true;


            // Get Design 
            SP2::GUnitDesign* l_pRequestedDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pRequest->m_iDesignID);
            gassert(l_pRequestedDesign ,"Tried to produce a unit whose design does not exist: ");

            // What unit type are we building
            EUnitType::Enum l_eUnitType = (EUnitType::Enum) l_pRequestedDesign->Type()->Id();

            // Are we in production ?
            if(l_pRequest->m_fGameTimeInProduction > g_ServerDAL.ProductionInformation(l_eUnitType).m_fInitialTimeInDays)
            {
               // Yes, we can ship units, initial time is ok.
               // Calculate production point to use on that request
               // High priority takes all prod point, med, .75 low .5
               REAL32 l_fProductionSpeed = l_pRequest->m_iProductionSpeed + 1.0f ;
               
               if(l_fRemainingProductionPoint > 1 * l_fProductionSpeed)
                  l_fProductionPointToUse = l_fProductionSpeed;
               else
                  l_fProductionPointToUse = l_fRemainingProductionPoint;

               // Remove from available production
               l_fRemainingProductionPoint -= l_fProductionPointToUse;

               // Now calculated how much unit we have produce for this time spawn
               REAL32 l_fProducedUnitsForTimeSpawn = (REAL32)(l_fTimeSpan * l_fProductionPointToUse * g_ServerDAL.ProductionInformation(l_eUnitType).m_fUnitsPerDays * l_pRequest->m_fBonusFromTech);
               l_pRequest->m_fQtyShippedAccumulated+= l_fProducedUnitsForTimeSpawn;

               REAL32 l_fProducedUnits = l_pRequest->m_fQtyShippedAccumulated  - l_pRequest->m_iQtyShipped *1.0f;
               


               // Did we ships some units ?
               if(l_fProducedUnits > 1)
               {
                  // Update client
                  l_bThisProductionRequestNeedRefresh = true;


                  bool l_bRequestIsFinish = false;
                  UINT32 l_iQtyToShip; 
                  
                  // Are we done witht this request ? 
                  if((l_pRequest->m_fQtyShippedAccumulated + l_fProducedUnits) > (REAL32)l_pRequest->m_iQtyWanted)
                  {
                     // we have produced the last portion of the last units.
                     l_fProducedUnits = l_pRequest->m_iQtyWanted - l_pRequest->m_fQtyShippedAccumulated;
                     l_iQtyToShip = l_pRequest->m_iQtyWanted - l_pRequest->m_iQtyShipped;
                     l_bRequestIsFinish = true;
                  }
                  else
                  {
                     // We have produced more than one units , ship
                     l_iQtyToShip = (UINT32)floor(l_fProducedUnits);
                  }

                  //Ship the new units
                  ShipUnits(l_pRequest->m_iBuilderCountryID,                        //builder
                           l_pRequest->m_iDestinationCountryID,                    //destination
                           l_iQtyToShip,  //qty
                           l_pRequest->m_iDesignID);                               //Design Id

                  // Add produced units 
                  l_pRequest->m_iQtyShipped += l_iQtyToShip;


                  if(l_bRequestIsFinish)
                  {

                     l_bRequestHasBeenDestroy = true;
                     m_vProductionQueues[l_pRequest->m_iBuilderCountryID-1].m_bProductionStatusIsDirty = true;
                     m_vProductionQueues[l_pRequest->m_iDestinationCountryID-1].m_bProductionStatusIsDirty = true;

                     list<GUnitProductionRequest>::iterator l_2BeRemovedIt = l_RequestIt;
                     l_RequestIt++;
                     //Remove the units that have been shipped
                     (*l_It).m_ActiveQueue.erase(l_2BeRemovedIt);

                     continue;
                  }
               }
            }
            else
            {
               if( l_pRequest->m_fGameTimeInProduction - l_pRequest->m_fLastUpdateTime  > 1)
               {
                  l_bThisProductionRequestNeedRefresh = true;
                  l_pRequest->m_fLastUpdateTime = l_pRequest->m_fGameTimeInProduction;
               }
            }
         }
         else
         {
            
            // If this unit was build before, its not anymore, better tell the client
            if(l_pRequest->m_bUnitIsBeingBuilt == true)
            {
               l_pRequest->m_bUnitIsBeingBuilt  = false;
               l_bThisProductionRequestNeedRefresh = true;
            }

         }
         
         // If the request still exist, fill out the update section for the production update to the client
         if(l_bRequestHasBeenDestroy == false)
         {
            UpdateProductionStatus(l_iCountryID,*l_pRequest,l_fProductionPointToUse); 

            if(l_pRequest->m_iDestinationCountryID != l_iCountryID)
            {
               UpdateProductionStatus((UINT16)l_pRequest->m_iDestinationCountryID,*l_pRequest,l_fProductionPointToUse); 
            }
         }

         if(l_bThisProductionRequestNeedRefresh)
         {
            m_vProductionQueues[l_pRequest->m_iBuilderCountryID-1].m_bProductionStatusIsDirty = true;
            m_vProductionQueues[l_pRequest->m_iDestinationCountryID-1].m_bProductionStatusIsDirty = true;
         }

         l_RequestIt++;
      }


      // Calculate production capacity for this country
      if(l_fTotalProductionPoint > 0)
         (*l_It).m_fProductionCapacity = (REAL32)(l_fRemainingProductionPoint / l_fTotalProductionPoint);
      else
         (*l_It).m_fProductionCapacity = 0;

   }

   // Send Production Queue to people that need update
   l_iCountryID = 1;
   for(vector<GProductionQueue>::iterator l_It = m_vProductionQueues.begin() ; 
      l_It != m_vProductionQueues.end() ; 
      l_It++)
   {
      if((*l_It).m_bProductionStatusIsDirty)
            SendProductionQueueToCountry(l_iCountryID);
      l_iCountryID++;
   }

   // Get last update(time)
   m_iLastIterationGameTime = g_Joshua.GameTime();
}

/*!
 * Get the country military production value
 **/
REAL64 GUnitProductor::CountryMilitaryProduction(UINT32 in_iCountryID) const
{
   //Military production  = [Country’s GNP] x [Country’s Economic Health] x [0.002]
   GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	return l_pCountryData->GDPValue() * 0.002f * (REAL64)l_pCountryData->EconomicHealth() ;   
}


/*!
 * Actually ship the units by adding them to the deployment list of a country
 **/
bool GUnitProductor::ShipUnits(UINT32 in_iBuilderCountryID,
                               UINT32 in_iDestinationCountryID,
                               UINT32 in_iQty,
                               UINT32 in_iDesignID) const
{
   SP2::GUnitGroupEx* l_pGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[in_iDestinationCountryID-1];
   IF_RETURN(!l_pGroup, false);

   g_ServerDAL.DirtyCountryUnitsServer(in_iDestinationCountryID);

   //Check if there is not already a unit with the same design ID 
   //simply add the qty to the exiting unit
   for(UINT32 i = 0 ;i  < l_pGroup->Units().size() ; i++)
   {
      SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pGroup->Units()[i]->Design();
      gassert(l_pDesign,"Design should exist, invalid NULL pointer");
      //If the design ID is the same, add the units
      UINT32 l_iUnitID = l_pGroup->Units()[i]->Id();
      if(l_pDesign->Id() == in_iDesignID && 
         g_ServerDCL.UnitMover().IsUnitInTraining(l_iUnitID) == false && 
         g_ServerDCL.IsUnitForSale(l_iUnitID) == false &&
         ((SP2::GUnit*)(l_pGroup->Units()[i]))->Training() == ETrainingLevel::Recruit)
      {  
         //Add the units to this existing unit
         SP2::GUnit* l_pUnit = (SP2::GUnit*)l_pGroup->Units()[i];
         UINT32 l_iNewHP = l_pUnit->HP() + l_pDesign->Type()->HP()*in_iQty;
         gassert(l_iNewHP,"HP should not be NULL");
         l_pUnit->HP(l_iNewHP);
         g_Joshua.UnitManager().ModifyUnit(l_pUnit);
         g_Joshua.UnitManager().ModifyUnitGroup(l_pUnit->Group());
#ifdef LOG_SHIP_UNITS
   g_Joshua.Log(GString(in_iBuilderCountryID)+ " Shipped : " + GString(in_iQty) + " " + l_pDesign->Name() + " to " + GString(in_iDestinationCountryID));
#endif

      return true;
      }   
   }

   //If we reach that point, means unit do not already exist, create one
   //Create the unit
   SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().CreateUnit();
   l_pUnit->Id(g_Joshua.UnitManager().NewUnitID());   
   SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(in_iDesignID);
   gassert(l_pDesign,"Unit Productor, Cannot create a unit with an unknown design");
   l_pUnit->Design(l_pDesign);
   gassert(in_iQty,"Qty should not be NULL");
   l_pUnit->HP(in_iQty*l_pDesign->Type()->HP());
   g_Joshua.UnitManager().AddNewUnit(l_pUnit);
   g_Joshua.UnitManager().AddUnit2Group(l_pUnit,l_pGroup);
#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(in_iBuilderCountryID)+ " Shipped : " + GString(in_iQty) + " " + l_pDesign->Name() + " to " + GString(in_iDestinationCountryID));
#endif

   return true;
}

/*!
 * Gives a unique identifier for a production request
 * @return Unique ID
 **/
UINT32 GUnitProductor::NextProductionRequestID()
{
   return m_iNextProductionRequestID++;
}

/*!
 * Insert the production Request at the correct place in the production queue 
 * and returns the ID of the request
 * @param in_Request: Production request
 * @return INT32: Id of the production request, -1 if an error
 **/
INT32 GUnitProductor::InsertProductionRequest(GUnitProductionRequest& in_Request)
{
   if(in_Request.m_iDesignID <= 0)
   {
      return -1;
   }

   gassert(g_Joshua.UnitManager().UnitDesign(in_Request.m_iDesignID),"Invalid unit design requested for a unit production");

   //Assign a unique ID to the request
   in_Request.m_iID = NextProductionRequestID();

   //Get a reference to the country's production queue
   list<GUnitProductionRequest>& l_CountryProductionQueue = m_vProductionQueues[in_Request.m_iBuilderCountryID-1].m_ActiveQueue;
   m_vProductionQueues[in_Request.m_iBuilderCountryID-1].m_bProductionStatusIsDirty = true;
   m_vProductionQueues[in_Request.m_iDestinationCountryID-1].m_bProductionStatusIsDirty = true;

   //If the production is for another country, it is always on the top of the line, but after the 
   //production for another countries
   if(in_Request.m_iBuilderCountryID != in_Request.m_iDestinationCountryID)
   {
      list<GUnitProductionRequest>::iterator l_It;
      //Find where must be inserted
      for(l_It = l_CountryProductionQueue.begin() ;
          l_It != l_CountryProductionQueue.end() ;
          l_It++)
      {
         if((*l_It).m_iBuilderCountryID != (*l_It).m_iDestinationCountryID)
         {
            continue;
         }
         else
         {
            //Insert here
            l_CountryProductionQueue.insert(l_It,in_Request);
            break;
         }
      }
      if(l_It == l_CountryProductionQueue.end())
      {
         l_CountryProductionQueue.push_back(in_Request);
      }
      
   }
   else
   {//Production is for the country itself
      //Insert after production for other countries, and dependent on the priority
      //Find where must be inserted
      bool l_bInserted = false;
      for(list<GUnitProductionRequest>::iterator l_It = l_CountryProductionQueue.begin() ;
          l_It != l_CountryProductionQueue.end() ;
          l_It++)
      {
         //If production request for another country, skip the entry
         if((*l_It).m_iBuilderCountryID != (*l_It).m_iDestinationCountryID)
         {
            continue;
         }

         //If the queued request priority is < than our request priority
         if((*l_It).m_ePriority < in_Request.m_ePriority)
         {
            //Insert the value
            l_bInserted = true;
            l_CountryProductionQueue.insert(l_It,in_Request);
            break;
         }       
      }   
      //If not inserted yet, insert at the end
      if(!l_bInserted)
      {
         l_CountryProductionQueue.insert(l_CountryProductionQueue.end(),in_Request);
      }
   }

   return in_Request.m_iID;
}

void GUnitProductor::ClearProductionForCountry(UINT32 in_iCountryID)
{
	m_vProductionQueues[in_iCountryID-1].m_ProductionStatusList.clear();
	m_vProductionQueues[in_iCountryID-1].m_ActiveQueue.clear();
	m_vProductionQueues[in_iCountryID-1].m_fProductionCapacity = 1.f;
	m_vProductionQueues[in_iCountryID-1].m_bProductionStatusIsDirty = true;

   for(UINT32 i = 0;i < g_ServerDAL.NbCountry();i ++)
   {
      list<GUnitProductionRequest>::iterator it = m_vProductionQueues[i].m_ActiveQueue.begin();
      while(it != m_vProductionQueues[i].m_ActiveQueue.end() )
      {
         if(it->m_iDestinationCountryID == in_iCountryID)
         {
            list<GUnitProductionRequest>::iterator l_ItToRemove = it;
            ++ it;
            m_vProductionQueues[i].m_ActiveQueue.erase(l_ItToRemove);
         }
         else
         {
            ++ it;
         }
      }
   }
}

bool GUnitProductor::CancelUnitProduction(UINT32 in_iCountryID, UINT32 in_iProductionRequestID,GUnitProductionRequest& out_RemovedProductionRequest)
{
   //ProductionRequest(
   //Find the unit production ID in the country queue
   list<GUnitProductionRequest>& l_CountryProductionQueue = m_vProductionQueues[in_iCountryID-1].m_ActiveQueue;

   for(list<GUnitProductionRequest>::iterator l_It = l_CountryProductionQueue.begin() ;
         l_It != l_CountryProductionQueue.end() ;
         l_It++)
   {
      if((*l_It).m_iID == in_iProductionRequestID)
      {
         out_RemovedProductionRequest = *l_It;
         m_vProductionQueues[in_iCountryID-1].m_ActiveQueue.erase(l_It);
         return true;         
      }
   }


   return false;
}


/*!
 * Updates the position of a unit production request in a production queue
 **/
bool GUnitProductor::UpdateProductionPriority(UINT32 in_iCountryID,
                                              UINT32 in_iRequestID,
                                              UINT32 in_iNewPositionFromTop)
{
   //Find the unit production ID in the country queue
   list<GUnitProductionRequest>& l_CountryProductionQueue = m_vProductionQueues[in_iCountryID-1].m_ActiveQueue;

   list<GUnitProductionRequest>::iterator l_ActualPositionIt = l_CountryProductionQueue.end();

   //Find the actual position of the unit in the queue
   for(list<GUnitProductionRequest>::iterator l_It = l_CountryProductionQueue.begin() ;
         l_It != l_CountryProductionQueue.end() ;
         l_It++)
   {
      if((*l_It).m_iID == in_iRequestID)
      {
         l_ActualPositionIt = l_It;
      }
   }

   //Didnt find
   if(l_ActualPositionIt == l_CountryProductionQueue.end())
      return false;

   //Find the new position in the list
   {
      //If the request is for another country, it can go in top of foreign countries request
      //else, the request cannot go above the foreign countries production requests
      bool l_bCanGoTop = false;
      if(l_ActualPositionIt->m_iBuilderCountryID != l_ActualPositionIt->m_iDestinationCountryID)
      {
         l_bCanGoTop = true;
      }

      UINT32 l_iIndex = 0;
      for(list<GUnitProductionRequest>::iterator l_It = l_CountryProductionQueue.begin() ;
         l_It != l_CountryProductionQueue.end() ;
         l_It++)
      {
         //If there is a possible swap
         if(l_iIndex >= in_iNewPositionFromTop)
         {     
            //Check if we can do the swap
            if(
                //If a foreign production request and we can go above it, swap
                ((l_It->m_iBuilderCountryID != l_It->m_iDestinationCountryID) && l_bCanGoTop)  ||
                //If a production request for us, we can go above it
                (l_It->m_iBuilderCountryID == l_It->m_iDestinationCountryID)              
               )
            {
               l_CountryProductionQueue.insert(l_It,*l_ActualPositionIt);
               l_CountryProductionQueue.erase(l_ActualPositionIt);
               return true;
            }
         }
         l_iIndex++;
      }
   }
   return false;
}

// Call when a the prod queue of a country has changed.
void GUnitProductor::SendProductionQueueToCountry(UINT32 in_iCountryID) 
{
   // Create Game Event and send it to client (the country).
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GUpdateUnitOrderStatus); 

    // Send to appropriate client
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iCountryID);
   // Don't update AI controlled player
   if(l_pPlayer->AIControlled())
      return;
   l_pEvent->m_iTarget= l_pPlayer->Client()->Id();
   l_pEvent->m_iSource= SDK::Event::ESpecialTargets::Server;

   SP2::Event::GUpdateUnitOrderStatus* l_pUpdateOrderStatus = (SP2::Event::GUpdateUnitOrderStatus*)l_pEvent.get();
   l_pUpdateOrderStatus->m_fProductionCapacity = m_vProductionQueues[in_iCountryID-1].m_fProductionCapacity;
   l_pUpdateOrderStatus->m_ProductionStatusList = m_vProductionQueues[in_iCountryID-1].m_ProductionStatusList;

	gassert(l_pUpdateOrderStatus->m_fProductionCapacity >= 0.f, "GUnitProductor::SendProductionQueueToCountry() Production Capacity invalid");

   m_vProductionQueues[in_iCountryID-1].m_bProductionStatusIsDirty = false;
 
   // Send Event
   g_Joshua.RaiseEvent(l_pEvent);
}


bool  GUnitProductor::GetProductionRequest(UINT32 in_iCountryId, UINT32 in_iProductionRequestId,GUnitProductionRequest& out_ProductionRequest)
{
   //Find the unit production ID in the country queue
   list<GUnitProductionRequest>& l_CountryProductionQueue = m_vProductionQueues[in_iCountryId-1].m_ActiveQueue;

   for(list<GUnitProductionRequest>::iterator l_It = l_CountryProductionQueue.begin() ;
         l_It != l_CountryProductionQueue.end() ;
         l_It++)
   {
      if((*l_It).m_iID == in_iProductionRequestId)
      {
         out_ProductionRequest = *l_It;
         return  true;
      }
   }
   return false;
}


/*! 
* Production Percentage 
* Gets the production percentage of the given unit production request ID 
* @return The production percentage, between 0 and 1,  -1 if error 
**/ 
REAL32 GUnitProductor::ProductionProgress(UINT32 in_iCountryID,UINT32 in_iRequestID) const 
{ 
  //Find the unit production ID in the country queue 
  const list<GUnitProductionRequest>& l_CountryProductionQueue = m_vProductionQueues[in_iCountryID-1].m_ActiveQueue; 

  for(list<GUnitProductionRequest>::const_iterator l_It = l_CountryProductionQueue.begin() ; 
        l_It != l_CountryProductionQueue.end() ; 
        l_It++) 
  { 
     if((*l_It).m_iID == in_iRequestID) 
     { 
        //SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_It->m_iDesignID); 
        
        return (*l_It).m_iQtyShipped / (*l_It).m_iQtyWanted * 1.0f;
     } 
  } 
  return -1.f; 
}

void GUnitProductor::CleanProductionStatus(void)
{
   //Process the production queues for every country
   for(vector<GProductionQueue>::iterator l_It = m_vProductionQueues.begin() ; 
      l_It != m_vProductionQueues.end() ; 
      l_It++)
   {
      (*l_It).m_ProductionStatusList.clear();
   }
}

void  GUnitProductor::UpdateProductionStatus(UINT16 in_iCountryId, GUnitProductionRequest &in_ProductionRequest,REAL32 in_fAvailableProductionPoint)
{
   // Get Production Status list for this country
   list<GUnitProductionStatus>& l_ProductionStatusList = m_vProductionQueues[in_iCountryId-1].m_ProductionStatusList; 
   
   // Build Production Status
   GUnitProductionStatus l_ProductionStatus;

   // Production progress is between 0 and 2 . 0 - 1 when in pre-prod and 1-2 in production
   SP2::GUnitDesign* l_pRequestedDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(in_ProductionRequest.m_iDesignID);
   gassert(l_pRequestedDesign ,"Tried to produce a unit whose design does not exist: ");
   
   l_ProductionStatus.m_fProgress = (REAL32)min(1,in_ProductionRequest.m_fGameTimeInProduction / g_ServerDAL.ProductionInformation((EUnitType::Enum)l_pRequestedDesign->Type()->Id()).m_fInitialTimeInDays);
   l_ProductionStatus.m_fProgress += (float)in_ProductionRequest.m_iQtyShipped / (float)in_ProductionRequest.m_iQtyWanted;
   l_ProductionStatus.m_ePriority = in_ProductionRequest.m_ePriority;

      
   if(in_ProductionRequest.m_ePriority == EUnitProductionPriority::OnHold)
   {
     l_ProductionStatus.m_iDaysLeft = 0xFFFFFFFF; // Hold means infinte time.
   }
   else
   {
      
      REAL32 l_fRemainingTimeForPreparation = (REAL32)max(0, g_ServerDAL.ProductionInformation((EUnitType::Enum)l_pRequestedDesign->Type()->Id()).m_fInitialTimeInDays - in_ProductionRequest.m_fGameTimeInProduction);
      REAL32 l_fTimeInProduction = (REAL32)( (in_ProductionRequest.m_iQtyWanted - in_ProductionRequest.m_iQtyShipped) / 
                                             ( g_ServerDAL.ProductionInformation((EUnitType::Enum)l_pRequestedDesign->Type()->Id()).m_fUnitsPerDays * 
                                               in_ProductionRequest.m_fBonusFromTech * 
                                               (in_ProductionRequest.m_iProductionSpeed + 1)
                                              )
                                            );

      if(in_fAvailableProductionPoint < 1)
      {
         if(l_ProductionStatusList.size() > 0)
         {
            l_fTimeInProduction += l_ProductionStatusList.back().m_iDaysLeft;
         }
         else
         {
            l_fTimeInProduction = (REAL32)(l_fTimeInProduction / max(0.001,min(1,in_fAvailableProductionPoint)));
         }
      }

      l_ProductionStatus.m_iDaysLeft =(UINT32) ceil(l_fRemainingTimeForPreparation + l_fTimeInProduction);
    
   }
   l_ProductionStatus.m_iQuantity = in_ProductionRequest.m_iQtyWanted;
   l_ProductionStatus.m_iDesignID = in_ProductionRequest.m_iDesignID;
   l_ProductionStatus.m_iRequestID= in_ProductionRequest.m_iID;
   l_ProductionStatus.m_bIsUnitBeingBuilt = in_ProductionRequest.m_bUnitIsBeingBuilt;
   l_ProductionStatus.m_iBuildingCountry  = (UINT16) in_ProductionRequest.m_iBuilderCountryID;
   l_ProductionStatus.m_iPayingCountry    = (UINT16) in_ProductionRequest.m_iDestinationCountryID;

   // Insert new status
   l_ProductionStatusList.push_back(l_ProductionStatus);
}

bool GUnitProductor::OnSave(GIBuffer& io_Buffer)
{
   g_Joshua.Log(L"Saving unit production...");

   io_Buffer << m_iLastIterationGameTime
             << m_iNextProductionRequestID;

   io_Buffer << (UINT32) m_vProductionQueues.size();
   for(UINT32 i = 0;i < m_vProductionQueues.size();i ++)
   {
      io_Buffer << m_vProductionQueues[i].m_ActiveQueue
                << m_vProductionQueues[i].m_bProductionStatusIsDirty
                << m_vProductionQueues[i].m_ProductionStatusList
                << m_vProductionQueues[i].m_fProductionCapacity;
   }

   return true;
}

bool GUnitProductor::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_iLastIterationGameTime
             >> m_iNextProductionRequestID;

   UINT32 l_iCount;
   io_Buffer >> l_iCount;
   m_vProductionQueues.resize(l_iCount);
   for(UINT32 i = 0;i < l_iCount;i ++)
   {
      io_Buffer >> m_vProductionQueues[i].m_ActiveQueue
                >> m_vProductionQueues[i].m_bProductionStatusIsDirty
                >> m_vProductionQueues[i].m_ProductionStatusList
                >> m_vProductionQueues[i].m_fProductionCapacity;
   }

   return true;
}

bool GUnitProductor::OnNew(GDatabase* in_pDatabase)
{
   m_vProductionQueues.resize(g_ServerDAL.NbCountry());
   m_iNextProductionRequestID = 1;
   m_iLastIterationGameTime = 0;

   return true;
}

void GUnitProductor::OnClean()
{
   m_vProductionQueues.clear();
}
