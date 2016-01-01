/**************************************************************
*
* sp2_ai_general_event_handler.cpp
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

GAIGeneralEventHandler::GAIGeneralEventHandler()
{}
GAIGeneralEventHandler::~GAIGeneralEventHandler()
{}

bool GAIGeneralEventHandler::HandleNukeLaunchNotify(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GStrategicWarfareNotify* l_pNotify = (SP2::Event::GStrategicWarfareNotify*)in_Event.get();
   
   for(UINT32 l_iNukeLaunchIndex = 0; l_iNukeLaunchIndex < l_pNotify->m_vLaunchedNukes.size(); l_iNukeLaunchIndex++)
   {
      GLaunchedNuke& l_LaunchedNuke = l_pNotify->m_vLaunchedNukes[l_iNukeLaunchIndex];

      ENTITY_ID		l_iSource       = l_LaunchedNuke.m_Missile.m_iOwnerID;
      //Find the TargetID
      ENTITY_ID		l_iTarget       = 0;
      {
         const Map::GMultiface* l_pRegion = g_ServerDAL.CountryView()->Locate(l_LaunchedNuke.m_TargetPosition.x,l_LaunchedNuke.m_TargetPosition.y);
         if(l_pRegion)
         {
            UINT32 l_iRegionID = l_pRegion->Id();
            l_iTarget = g_ServerDAL.RegionControl(l_iRegionID).m_iPolitical;
         }
      }

      //If the nuke was sent in the water, dont do anything
      if(!l_iTarget)
      {
         continue;
      }

         
      ENTITY_ID		l_iAI		       = g_Joshua.ActivePlayer( l_pNotify->m_iSpecificTarget )->ModID();
      UINT32			l_iMegatonsSent = l_LaunchedNuke.m_Missile.Megatons();
   	
	   bool				l_bSendingNukes      = false;
	   UINT32			l_iMegatonsSending   = 0;
	   ENTITY_ID		l_iNewTarget         = 0;
	   ENuclearTargetType::Enum l_Type     = ENuclearTargetType::Civil;
	   REAL32 l_fRatio = 2.f;	
   	
	   if(l_iAI == l_iSource || l_iSource == l_iTarget)
		   return true;

	   REAL32 l_fRelationsAttacker = g_ServerDAL.RelationBetweenCountries(l_iSource,l_iAI);
	   REAL32 l_fRelationsTarget	 = g_ServerDAL.RelationBetweenCountries(l_iTarget,l_iAI);

	   REAL32 l_fUnitsValueSource = g_ServerDAL.TotalUnitsValue(l_iSource);
	   REAL32 l_fUnitsValueTarget = g_ServerDAL.TotalUnitsValue(l_iTarget);
	   REAL32 l_fUnitsValueAI		= g_ServerDAL.TotalUnitsValue(l_iAI);

	   set<UINT32> l_ListOfAllies;
	   g_ServerDAL.CountryAllies(l_iAI,l_ListOfAllies);
   	

	   //! \todo Check if countries are neighbors, same continent, belong to same country group, or have same government type, before nuking a country

	   if(l_iTarget == l_iAI)
	   {
		   //AI is the target

		   //It will send nukes
		   l_bSendingNukes =  true;
		   l_iNewTarget = l_iSource;

		   if(l_fUnitsValueSource != 0.f)
			   l_fRatio = l_fUnitsValueAI / l_fUnitsValueSource;

		   //If the target has as much forces than the source, he launches as many missiles.
		   //If the target has more forces, he launches less missiles
		   //If the target has less forces, he launches more missiles
		   l_iMegatonsSending = (UINT32)((REAL32)l_iMegatonsSent * (1.f + (max(2.f - l_fRatio,0.1f))));

		   if(l_fRatio <= 2.f)
			   l_Type = ENuclearTargetType::Military;
		   else
			   l_Type = ENuclearTargetType::Civil;
   		
	   }
	   else if(l_ListOfAllies.find(l_iTarget) != l_ListOfAllies.end())
	   {
		   //AI is allied to the target

		   if(l_fRelationsTarget >= SP2::c_fRelationsLove)
		   {
			   //Will nuke the attacker, 
			   //! \todo: Or one of its allies
   			
			   l_bSendingNukes =  true;
			   l_iNewTarget = l_iSource;

			   if(l_fUnitsValueSource != 0.f)
				   l_fRatio = l_fUnitsValueAI / l_fUnitsValueSource;

			   l_iMegatonsSending = (UINT32)((REAL32)l_iMegatonsSent * (1.f + (max(2.f - l_fRatio,0.1f))));

			   if(l_fRatio <= 2.f)
				   l_Type = ENuclearTargetType::Military;
			   else
				   l_Type = ENuclearTargetType::Civil;

		   }
		   else if (l_fRelationsTarget <= SP2::c_fRelationsHate)
		   {
			   //Break its alliance with the target country
			   const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
			   for(hash_map<UINT32,GTreaty>::const_iterator l_Itr = l_Treaties.begin();
				   l_Itr != l_Treaties.end();
				   l_Itr++)
			   {
				   const GTreaty& l_Treaty = l_Itr->second;
				   if(l_Treaty.Type() == ETreatyType::Alliance && 
					   l_Treaty.CountrySide(l_iAI) == 1 &&
					   l_Treaty.CountrySide(l_iTarget) == 1)
				   {
					   g_ServerDCL.LeaveTreaty(l_iAI,l_Treaty.ID());
				   }
			   }
		   }
	   }
	   else if(l_ListOfAllies.find(l_iSource) != l_ListOfAllies.end())
	   {
		   //AI is allied to the attacker

		   if(l_fRelationsAttacker >= SP2::c_fRelationsLove)
		   {
			   //Will nuke the target, 
			   //! \todo: Or one of its allies
   			
			   l_bSendingNukes =  true;
			   l_iNewTarget = l_iTarget;

			   if(l_fUnitsValueSource != 0.f)
				   l_fRatio = l_fUnitsValueAI / l_fUnitsValueTarget;

			   l_iMegatonsSending = (UINT32)((REAL32)l_iMegatonsSent * (1.f + (max(2.f - l_fRatio,0.1f))));

			   if(l_fRatio <= 2.f)
				   l_Type = ENuclearTargetType::Military;
			   else
				   l_Type = ENuclearTargetType::Civil;

		   }
		   else if (l_fRelationsAttacker <= SP2::c_fRelationsHate)
		   {
			   //Break its alliance with the attacker country
			   const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
			   for(hash_map<UINT32,GTreaty>::const_iterator l_Itr = l_Treaties.begin();
				   l_Itr != l_Treaties.end();
				   l_Itr++)
			   {
				   const GTreaty& l_Treaty = l_Itr->second;
				   if(l_Treaty.Type() == ETreatyType::Alliance && 
					   l_Treaty.CountrySide(l_iAI) == 1 &&
					   l_Treaty.CountrySide(l_iSource) == 1)
				   {
					   g_ServerDCL.LeaveTreaty(l_iAI,l_Treaty.ID());
				   }
			   }
		   }
	   }
	   else if(l_fRelationsAttacker >= SP2::c_fRelationsLove && l_fRelationsTarget < SP2::c_fRelationsHate)
	   {
		   //AI is a friend of the attacker
		   //AI might send nuke towards the target
		   //! \todo : Do the "might" part

		   l_bSendingNukes =  true;
		   l_iNewTarget = l_iTarget;

		   if(l_fUnitsValueSource != 0.f)
			   l_fRatio = l_fUnitsValueAI / l_fUnitsValueTarget;

		   l_iMegatonsSending = (UINT32)((REAL32)l_iMegatonsSent * (1.f + (max(2.f - l_fRatio,0.1f))));		

		   if(l_fRatio <= 2.f)
			   l_Type = ENuclearTargetType::Military;
		   else
			   l_Type = ENuclearTargetType::Civil;
   		
	   }
	   else if(l_fRelationsTarget >= SP2::c_fRelationsLove && l_fRelationsAttacker < SP2::c_fRelationsHate)
	   {
		   //AI is a friend of the target
		   //AI might send nuke towards the attacker
		   //! \todo : Do the "might" part

		   l_bSendingNukes =  true;
		   l_iNewTarget = l_iSource;

		   if(l_fUnitsValueSource != 0.f)
			   l_fRatio = l_fUnitsValueAI / l_fUnitsValueSource;

		   l_iMegatonsSending = (UINT32)((REAL32)l_iMegatonsSent * (1.f + (max(2.f - l_fRatio,0.1f))));

		   if(l_fRatio <= 2.f)
			   l_Type = ENuclearTargetType::Military;
		   else
			   l_Type = ENuclearTargetType::Civil;
	   }
	   else
	   {
		   //AI is neutral, does nothing
	   }

   			
	   if(l_bSendingNukes)
		   g_CombatPlanner.AddNukeToWaitingList(l_iAI,l_iNewTarget,l_iMegatonsSending,l_Type);
   }

   return true;
}

bool GAIGeneralEventHandler::HandleTreatyOffer(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GTreatyOffer* l_pNotify = (SP2::Event::GTreatyOffer*)in_Event.get();

	//Go through the treaty list and find the respective ID treaty

	UINT32		l_iTreatyID	= l_pNotify->TreatyID;
	ENTITY_ID	l_iAI			= l_pNotify->m_iTarget;

   l_iAI = g_Joshua.ActivePlayer(l_iAI)->ModID();

	GTreaty* l_pTreaty = g_ServerDAL.Treaty(l_iTreatyID);

	if(!l_pTreaty)
		return false;

	UINT32 l_iSide = l_pTreaty->CountrySide(l_iAI);

	if(!l_iSide)
	{
		//Treaty already left that treaty
		return true;
	}

	gassert( l_iSide >= 1 || l_iSide <= 3, "Can not be offered a treaty where country has no original side");

	ETreatyRefusal::Enum l_eDecision = GAI::CountryShouldJoinTreaty(l_iAI,l_iTreatyID,l_iSide);
	if(l_eDecision == ETreatyRefusal::Accepted)
	{
		g_ServerDCL.JoinTreaty(l_iAI,l_iTreatyID,l_iSide);
	}
	else
	{
		g_ServerDCL.LeaveTreaty(l_iAI,l_iTreatyID);

		//Store that decision to show the player		
		g_ServerDAL.TreatyRefusalHistory()[l_iTreatyID][l_iAI] = l_eDecision;
		
	}

	return true;
}

bool GAIGeneralEventHandler::HandleTreatyJoin(SDK::GGameEventSPtr in_Event)
{
   //SP2::Event::GTreatyJoin* l_pNotify = (SP2::Event::GTreatyJoin*)in_Event.get();
	return true;
}

bool GAIGeneralEventHandler::HandleTreatyLeave(SDK::GGameEventSPtr in_Event)
{
   //SP2::Event::GTreatyLeave* l_pNotify = (SP2::Event::GTreatyLeave*)in_Event.get();
	return true;
}

bool GAIGeneralEventHandler::HandleTreatyPeaceSigned(SDK::GGameEventSPtr in_Event)
{
	SP2::Event::GTreatyPeaceSigned* l_pNotify = (SP2::Event::GTreatyPeaceSigned*)in_Event.get();
	UINT32 l_iFirstCountry = l_pNotify->CountryA;
	UINT32 l_iSecondCountry = l_pNotify->CountryB;
	UINT32 l_iAI = g_Joshua.ActivePlayer( l_pNotify->m_iSpecificTarget )->ModID();

	if(l_iAI == l_iFirstCountry)
	{
		g_ServerDCL.UnitMover().MilitaryRemoval(l_iAI,l_iSecondCountry);
	}
	else if(l_iAI == l_iSecondCountry)
	{
		g_ServerDCL.UnitMover().MilitaryRemoval(l_iAI,l_iFirstCountry);
	}
	else
	{
		//Peace is signed by 2 other countries. Does it affect that AI?

	}

	return true;
}



bool GAIGeneralEventHandler::HandleWarDeclaration(SDK::GGameEventSPtr in_Event)
{
	SP2::Event::GWarDeclarationNotify* l_pNotify = (SP2::Event::GWarDeclarationNotify*)in_Event.get();	
	ENTITY_ID		l_iAI		 = g_Joshua.ActivePlayer( l_pNotify->m_iSpecificTarget )->ModID();

	GAI::ExamineWarAndPeacePossibilities(l_iAI);
	return true;
}