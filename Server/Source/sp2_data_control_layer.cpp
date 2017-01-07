/**************************************************************
*
* sp2_data_control_layer.cpp
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  Alain Bellehumeur
** Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

// Uncomment to enable logging of control operation
//#define LOG_DCL

const INT32 c_iMeaningfulUnitQty = 25;

const GString GDataControlLayer::c_sClientStateTreatyPrefix = L"CLIENT";

GDataControlLayer::GDataControlLayer(GDALInterface* in_pDAL) : GDCLInterface(in_pDAL)
{
   // Set vulnerability of group status
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Parked]       = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Ready]        = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Fortified]    = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Moving]       = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::InDeployement]= false;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Attacking]    = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Training]     = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::Upgrading]    = true;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::ForSale]      = false;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::InTransit]    = false;
   m_bNuclearVulnerabilityByStatus[EMilitaryStatus::ReadyToDeploy]= false;	
}

GDataControlLayer::~GDataControlLayer()
{
}

void GDataControlLayer::LogNewAction(GString in_sNewAction)
{
#ifdef LOG_DCL
	g_Joshua.Log(in_sNewAction, MSGTYPE_MSG);
#endif
}

void GDataControlLayer::Initialize()
{
   IF_RETURN_LOG(!m_UnitMover.Init(),/*void*/,"Error while initializing unit movement manager");
   IF_RETURN_LOG(!m_UnitProductor.Initialize(),/*void*/,L"Error while initializing the Unit Productor");

   g_ServerDAL.DataManager()->RegisterNode(&m_GameObjectiveEvaluator);
   g_ServerDAL.DataManager()->RegisterNode(this);
}

void GDataControlLayer::Shutdown()
{
   m_UnitMover.Shutdown();
   m_UnitProductor.Shutdown();
}

void GDataControlLayer::IterateCountryControl()
{
   REAL64 l_fTime = g_Joshua.GameTime();

   // Verify each ongoing annexion to see if it is complete
   hash_map<UINT32, GRegionAnnex>::const_iterator l_It = g_ServerDAL.Annexes().begin();
   vector<UINT32> l_vCompleteAnnex;
   while(l_It != g_ServerDAL.Annexes().end() )
   {
      const GRegionAnnex& l_Annex = l_It->second;
      if(l_Annex.m_iCompletionTime < l_fTime)
      {
         // Annex is complete, update political control
         ChangeRegionPoliticalControl(l_Annex.m_iAnnexedRegion, l_Annex.m_iAnnexingCountry);
         l_vCompleteAnnex.push_back(l_Annex.m_iAnnexedRegion);
      }
      ++ l_It;
   }

   // Remove all completed annexion
   for(UINT32 i = 0;i < l_vCompleteAnnex.size();i ++)
   {
      g_ServerDAL.RemoveAnnexOfRegion(l_vCompleteAnnex[i] );
   }
}

void GDataControlLayer::ConquerCountry(GRegionControl& in_PreviousControl, GRegion* in_pRegion)
{
   // Make sure region is "qualified" to be conquered
   if(in_PreviousControl.m_iMilitary == in_PreviousControl.m_iPolitical)
   {
      if(in_pRegion->OwnerId() != in_pRegion->OwnerMilitaryId() )
      {
         // To be conquered, a country must have no remaining military units
         // (in fact, there is one group remaining : the build queue)
         if(g_Joshua.UnitManager().CountryUnitGroups(in_PreviousControl.m_iPolitical).size() == 1)
         {
            UINT32 l_iConqueror = in_pRegion->OwnerMilitaryId();
            const vector<GRegionControl>& l_vRegions = g_ServerDAL.RegionControlArray();

            // Also, 80% of the country population must be controled by the conqueror
            REAL32 l_fConquerRatio = 1.f;

            const set<UINT32>& l_vPoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_PreviousControl.m_iPolitical);
            for(set<UINT32>::const_iterator l_It = l_vPoliticalRegions.begin();
                (l_It != l_vPoliticalRegions.end() ) && l_fConquerRatio >= c_fConquerThreshold;
                ++ l_It)
            {
               if(l_vRegions[*l_It].m_iMilitary != l_iConqueror)
               {
                  l_fConquerRatio -= g_ServerDAL.GetGRegion(*l_It)->PercentageValue();
               }
            }

            if(l_fConquerRatio >= c_fConquerThreshold)
            {
               // Conquer what remains of the country military control
               for(set<UINT32>::const_iterator l_It = l_vPoliticalRegions.begin();
                  l_It != l_vPoliticalRegions.end(); ++ l_It)
               {
                  if(l_vRegions[*l_It].m_iMilitary == in_PreviousControl.m_iPolitical)
                  {
                     ChangeRegionMilitaryControl(*l_It, l_iConqueror, false);
                  }
               }
            }
         }
      }
   }
}

void GDataControlLayer::UpdateMilitaryControl(UINT32 in_iRegionID, UINT32 in_iCountryID)
{
   if(in_iRegionID == 0)
		return;
	// Make sure unit does not already own the region
   GRegionControl l_Control = g_ServerDAL.RegionControl(in_iRegionID);
   const UINT8* l_pDiplStatus = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);
   if(in_iCountryID != l_Control.m_iMilitary)
   {
      const set<UINT32>& l_RegionGroups = m_UnitMover.UnitGroupsInsideRegion(in_iRegionID);

      // Make sure current unit is alone in the region
      set<UINT32>::const_iterator l_It = l_RegionGroups.begin();
      bool l_bHostilePresence = false;
      bool l_bNewOwnerPresence = false;
      bool l_bOtherPresence = false;
      while( (!l_bHostilePresence) && (l_It != l_RegionGroups.end() ) )
      {
         SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_It);
         if(l_pGroup->OwnerId() == in_iCountryID)
         {
            l_bNewOwnerPresence = true;
         }
         else if(l_pDiplStatus[l_pGroup->OwnerId() ] == (UINT8) EDiplomaticStatus::Hostile)
         {
            l_bHostilePresence = true;
         }
         else
         {
            l_bOtherPresence = true;
         }

         ++ l_It;
      }

      // Country is alone inside region, update military control
      if(!l_bHostilePresence && l_bNewOwnerPresence)
      {
         // The country takes military control only if the current political control is hostile
         if(g_ServerDAL.DiplomaticStatus(in_iCountryID, l_Control.m_iPolitical) == EDiplomaticStatus::Hostile)
         {
            g_ServerDCL.ChangeRegionMilitaryControl(in_iRegionID, in_iCountryID);
         }
			else if(g_ServerDAL.DiplomaticStatus(in_iCountryID,l_Control.m_iMilitary) == EDiplomaticStatus::Hostile)
         {
            g_ServerDCL.ChangeRegionMilitaryControl(in_iRegionID, l_Control.m_iPolitical);
         }
      }
      else if(!l_bHostilePresence && l_bOtherPresence && !l_bNewOwnerPresence)
      {
         if(g_ServerDAL.DiplomaticStatus(in_iCountryID,l_Control.m_iMilitary) == EDiplomaticStatus::Hostile)
         {
            g_ServerDCL.ChangeRegionMilitaryControl(in_iRegionID, l_Control.m_iPolitical);
         }
      }
   }
}

bool GDataControlLayer::ChangeRegionMilitaryControl(const UINT32 in_iRegionID,
                                                    const UINT32 in_iNewControl,
                                                    bool in_bTestConquer)
{
    GDZLOG(L"in_iRegionID: " + GString(in_iRegionID) + L", " +
           L"in_iNewControl: " + GString(in_iNewControl) + L", " +
           L"in_bTestConquer: " + GString(in_bTestConquer),
           EDZLogCat::Regions);

   if(in_iRegionID == 0)
		return true;
   gassert(in_iRegionID <= g_ServerDAL.NbRegion(), "Region ID: " + GString(in_iRegionID));
   gassert(in_iNewControl <= g_ServerDAL.NbCountry(), "Military control country ID: " + GString(in_iNewControl));

   GRegionControl l_CurrentControl = g_ServerDAL.RegionControl(in_iRegionID);

	IF_RETURN(!ConfirmChangeMilitaryControl(in_iRegionID, in_iNewControl), false);

   m_UnitMover.UpdateRegionMilitaryControl(in_iRegionID, in_iNewControl, l_CurrentControl.m_iMilitary);

	GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
   assert(l_pRegion);

	g_ServerDAL.AddCountryToSynchronize(l_CurrentControl.m_iPolitical);
	g_ServerDAL.AddCountryToSynchronize(l_CurrentControl.m_iMilitary);

   if(l_CurrentControl.m_iPolitical == l_CurrentControl.m_iMilitary)
   {
       const auto l_iCountryID = l_CurrentControl.m_iMilitary;
	   GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_iCountryID);
      assert(l_pCountryData);

		if(l_pCountryData->Population() > 0)
		{
			//Remove some % of budget allocated, in ratio of the region now lost.
			REAL32 l_fRatioForLosingCountry = (REAL32)l_pRegion->Population() / (REAL32) l_pCountryData->Population();


			gassert(l_fRatioForLosingCountry <= 1.f, 
					"Region/Country poop ratio should not exceed 1");


			l_pCountryData->ReadjustDesiredExportsImports((REAL64)(1.f-l_fRatioForLosingCountry));
		}

      // Change capital if it was in the region where military control has just been taken
      ChangeCountryCapital(in_iRegionID);

      // Test if the region change provoques a complete conquer
      if(in_bTestConquer)
      {
         ConquerCountry(l_CurrentControl, l_pRegion);

         // Disband nuclear missiles if country is now fully occupied
         if(l_pCountryData->NumberOfPoliticallyControlledRegions() == 0)
         {
             GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" is fully occupied; " +
                           L"disbanding nuclear missiles and AMDS",
                           EDZLogCat::Nuclear,
                           __FUNCTION__, __LINE__);

             // remove nuclear missiles of our country
             vector<GNuclearMissile> l_vEmptyList; l_vEmptyList.clear();
             l_pCountryData->NuclearMissilesSet(ENuclearMissileType::Standard,     l_vEmptyList);
             l_pCountryData->NuclearMissilesSet(ENuclearMissileType::OnSubmarines, l_vEmptyList);

             if(g_SP2Server->DisableNuclearOnOccupy() &&
                l_pCountryData->NuclearReady() >= 0.f)
             {
                 auto& l_UnitManager = g_Joshua.UnitManager();

                 // Cancel nuclear units in production
                 const auto& l_vTargetProductionQueue = m_UnitProductor.m_vProductionQueues.at(l_iCountryID - 1).m_ActiveQueue;
                 map<UINT32, ENTITY_ID> l_mRequestsToCancel;
                 for(auto l_RequestIt = l_vTargetProductionQueue.cbegin();
                     l_RequestIt != l_vTargetProductionQueue.cend();
                     ++l_RequestIt)
                 {
                     const auto l_pDesign = dynamic_cast<GUnitDesign*>(l_UnitManager.UnitDesign(l_RequestIt->m_iDesignID));
                     if(l_pDesign->Type()->Category() == EUnitCategory::Nuclear)
                         l_mRequestsToCancel[l_RequestIt->m_iID] = l_RequestIt->m_iDestinationCountryID;
                 }

                 GDZDebug::Log(L"Number of requests to cancel: " +
                               GString(l_mRequestsToCancel.size()),
                               EDZLogCat::Nuclear,
                               __FUNCTION__, __LINE__);
                 for(auto it = l_mRequestsToCancel.cbegin();
                     it != l_mRequestsToCancel.cend();
                     ++it)
                 {
                     const auto l_iRequestID = it->first;
                     const auto l_iDestinationID = it->second;
                     GDZDebug::Log(L"Cancelling units in production: Request ID " + GString(l_iRequestID) + L": " +
                                   L"Built by " + l_pCountryData->NameAndIDForLog() +
                                   L" and destined for " + g_ServerDAL.CountryData(l_iDestinationID)->NameAndIDForLog(),
                                   EDZLogCat::Nuclear | EDZLogCat::ObtainUnits,
                                   __FUNCTION__, __LINE__);
                     CancelUnitProduction(l_iCountryID, l_iDestinationID, l_iRequestID);
                 }

                 // Drop nuclear tech by 1
                 REAL32* const l_pNuclearValues = l_pCountryData->ResearchInfo()->m_fMaxValues[EUnitCategory::Nuclear];
                 REAL32 l_fHighestTechLevel = 0.f;
                 for(INT32 i = EUnitDesignCharacteristics::MissileRange;
                     i <= EUnitDesignCharacteristics::MissileDamage;
                     i++)
                 {
                     GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": " +
                         g_ServerDAL.GetString(c_iUnitDesignCharacteristicsStringID[i]) + L" " + GString(l_pNuclearValues[i]),
                         EDZLogCat::Nuclear,
                         __FUNCTION__, __LINE__);
                     l_pNuclearValues[i] = max(0.f, l_pNuclearValues[i] - 1.f);
                     l_fHighestTechLevel = max(l_pNuclearValues[i], l_fHighestTechLevel);
                 }

                 // If all nuclear research would go below 0, then require country to restart nuclear research
                 if(fabs(l_fHighestTechLevel) <= 0.0000005f)
                 {
                     GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": " +
                                   L"Removing nuclear readiness",
                                   EDZLogCat::Nuclear,
                                   __FUNCTION__, __LINE__);

                     l_pCountryData->NuclearReady(-1.f);
                 }
             }

             // Cancel or disband AMDS
             if(g_SP2Server->DisbandAMDSOnOccupy())
                 l_pCountryData->AMDSLevel(0);
         }
      }
   }
   else if(l_CurrentControl.m_iPolitical == in_iNewControl)
   {
	   GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iNewControl);
      assert(l_pCountryData);

      if(l_pCountryData->Population() > 0)
      {
	      //Add some % of budget allocated, in ratio of the region now regained.
	      REAL32 l_fRatioForGainingCountry = (REAL32)l_pRegion->Population() / (REAL32) l_pCountryData->Population();

	      l_pCountryData->ReadjustDesiredExportsImports((REAL64)(1.f+l_fRatioForGainingCountry));
      }

      // Test if gaining this region creates a capital 
      // (this can happen when country was previously completely controled by another)
      if(g_ServerDAL.CapitalID(l_CurrentControl.m_iPolitical) == 0x80000000)
      {
         // Capital was unexistant, find a new one in this region
         const vector<UINT32>& l_vRegionCities = g_ServerDAL.CitiesInRegion(in_iRegionID);
         if(l_vRegionCities.empty() )
         {
            ConfirmChangeCapitalID(l_CurrentControl.m_iPolitical, in_iRegionID | 0x80000000);
         }
         else
         {
            UINT32 l_iBestCityID = 0xFFFFFFFF;
            UINT32 l_iBestCityScore = 0;
            for(UINT32 j = 0;j < l_vRegionCities.size();j ++)
            {
               const SCityInfo& l_City = g_ServerDAL.Cities().at(l_vRegionCities[j] );
               UINT32 l_iCityScore = l_City.m_iPopulation;

               if(l_iCityScore >= l_iBestCityScore)
               {
                  l_iBestCityScore = l_iCityScore;
                  l_iBestCityID = l_City.m_iId;
               }
            }
            ConfirmChangeCapitalID(l_CurrentControl.m_iPolitical, l_iBestCityID);
         }
         m_UnitMover.UpdateCapitalLocation(l_CurrentControl.m_iPolitical);
      }
   }

   if( (in_bTestConquer) && (l_CurrentControl.m_iMilitary != in_iNewControl) )
   {
      // Test if gaining this region cancels a region annex
      CancelRegionAnnex(in_iRegionID);
   }

   return true;
}

bool GDataControlLayer::ChangeRegionPoliticalControl(const UINT32 in_iRegionID,
                                                     const UINT32 in_iNewControl,
																	  bool in_bForceChange)
{
   assert(in_iRegionID <= g_ServerDAL.NbRegion() );
   assert(in_iNewControl <= (UINT32) g_ServerDAL.NbCountry() );

   GRegionControl l_CurrentControl = g_ServerDAL.RegionControl(in_iRegionID);

	if(!in_bForceChange)
	{
		// Can only change political control to the current military control
		IF_RETURN(l_CurrentControl.m_iMilitary != in_iNewControl, false);
	}

   // Check if we need to change capital after political control has been taken.
   //  This is a one case scenario with Palestine capital being in Israel military
   //  controlled territory
   ChangeCountryCapital(in_iRegionID);

   GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iNewControl);
   assert(l_pCountryData);

   //Save the stats of the country with new control, because we'll need them to calculate the new country's LE, MYS, and EYS.
   REAL32 l_fCurrentCountryBirthRate = l_pCountryData->BirthRate();
   REAL32 l_fCurrentCountryDeathRate = l_pCountryData->DeathRate();
   INT64 l_iCurrentCountryPop15 = l_pCountryData->Pop15();
   INT64 l_iCurrentCountryPopOver15 = l_pCountryData->Population() - l_pCountryData->Pop15();
   
   IF_RETURN(!ConfirmChangePoliticalControl(in_iRegionID, in_iNewControl), false);

   GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
   assert(l_pRegion);

   //Remove some production from the region if it was gained through force
   if(!in_bForceChange)
   {
       for(UINT32 j=0; j<EResources::ItemCount; j++)
       {
           const EResources::Enum l_eResource = static_cast<EResources::Enum>(j);
           REAL64 l_fResourceProduction = l_pRegion->ResourceProduction(l_eResource);
           l_fResourceProduction *= 1 - g_SP2Server->ProductionLossOnAnnex();
           l_pRegion->ResourceProduction(l_eResource, l_fResourceProduction);
       }
   }

   g_ServerDAL.CountryData(l_CurrentControl.m_iPolitical)->SynchronizeWithRegions();
   l_pCountryData->SynchronizeWithRegions();

   //Now l_pCountryData has been updated with the new population.

   if(l_pCountryData->Population() > 0)
   {
	   //Add some % of budget allocated, in ratio of the region now regained.
	   REAL32 l_fRatioForGainingCountry = (REAL32)l_pRegion->Population() / (REAL32) l_pCountryData->Population();

	   l_pCountryData->ReadjustDesiredExportsImports((REAL64)(1.f+l_fRatioForGainingCountry));

       //Update HDI and other stats
       {
           const GCountryData* l_pFormerCountryData = g_ServerDAL.CountryData(l_CurrentControl.m_iPolitical);
           gassert(l_pFormerCountryData,"Country data is NULL");

           GDZDebug::Log(l_pCountryData->Name() + L": Under-15 population " +
                         GString::FormatNumber(static_cast<REAL64>(l_iCurrentCountryPop15), L",", L".", L"", L"", 3, 0) + L", " +
                         L"over-15 population " +
                         GString::FormatNumber(static_cast<REAL64>(l_iCurrentCountryPopOver15), L",", L".", L"", L"", 3, 0) + L"; " +
                         L"gaining region " + g_ServerDAL.GetString(l_pRegion->NameId()) + L" with under-15 population " +
                         GString::FormatNumber(static_cast<REAL64>(l_pRegion->Population15()), L",", L".", L"", L"", 3, 0) + L", " +
                         L"over-15 population " +
                         GString::FormatNumber(static_cast<REAL64>(l_pRegion->Population() - l_pRegion->Population15()), L",", L".", L"", L"", 3, 0),
                         EDZLogCat::HDI,
                         __FUNCTION__, __LINE__);

           REAL32 l_fFormerBirths = l_pFormerCountryData->BirthRate() * l_pRegion->Population();
           REAL32 l_fCurrentBirths = l_fCurrentCountryBirthRate * (l_iCurrentCountryPop15 + l_iCurrentCountryPopOver15);
           REAL32 l_fNewBirthRate = (l_fFormerBirths + l_fCurrentBirths) / l_pCountryData->Population();
           //g_Joshua.Log(L"Former and current births: " + GString(l_fFormerBirths) + ", " + GString(l_fCurrentBirths) + "; " + GString(l_pCountryData->Population()));
           l_pCountryData->BirthRate(l_fNewBirthRate);

           REAL32 l_fNewDeathRate = ((l_pFormerCountryData->DeathRate() * l_pRegion->Population()) + (l_fCurrentCountryDeathRate * (l_iCurrentCountryPop15 + l_iCurrentCountryPopOver15))) / l_pCountryData->Population();
           l_pCountryData->DeathRate(l_fNewDeathRate);

           REAL32 l_fNewLifeExpectancy =
               ((l_pFormerCountryData->LifeExpectancy() * l_fFormerBirths) +
                (l_pCountryData->LifeExpectancy() * l_fCurrentBirths)) / 
               (l_pCountryData->BirthRate() * l_pCountryData->Population());
           //g_Joshua.Log(L"BRs: " + GString::FormatNumber(l_pFormerCountryData->BirthRate(), 3) + ", " + GString::FormatNumber(l_fCurrentCountryBirthRate, 3) + ", " + GString::FormatNumber(l_pCountryData->BirthRate(), 3));
           //g_Joshua.Log(L"Old LEs: " + GString::FormatNumber(l_pFormerCountryData->LifeExpectancy(), 1) + " and " + GString::FormatNumber(l_pCountryData->LifeExpectancy(), 1) + "; new LE: " + GString::FormatNumber(l_fNewLifeExpectancy, 1));
           l_pCountryData->LifeExpectancy(l_fNewLifeExpectancy);

           REAL32 l_fNewMeanYearsSchooling =
               ((l_pFormerCountryData->MeanYearsSchooling() * l_pRegion->Population15()) +
                (l_pCountryData->MeanYearsSchooling() * l_iCurrentCountryPop15)) / l_pCountryData->Pop15();
           //g_Joshua.Log(L"Old MYSs: " + GString::FormatNumber(l_pFormerCountryData->MeanYearsSchooling(), 1) + " and " + GString::FormatNumber(l_pCountryData->MeanYearsSchooling(), 1) + "; new MYS: " + GString::FormatNumber(l_fNewMeanYearsSchooling, 1));
           l_pCountryData->MeanYearsSchooling(l_fNewMeanYearsSchooling);

           REAL32 l_fNewExpectedYearsSchooling =
               ((l_pFormerCountryData->ExpectedYearsSchooling() * (l_pRegion->Population() - l_pRegion->Population15())) +
                (l_pCountryData->ExpectedYearsSchooling() * l_iCurrentCountryPopOver15)) / 
               (l_pCountryData->Population() - l_pCountryData->Pop15());
           //g_Joshua.Log(L"Old EYSs: " + GString::FormatNumber(l_pFormerCountryData->ExpectedYearsSchooling(), 1) + " and " + GString::FormatNumber(l_pCountryData->ExpectedYearsSchooling(), 1) + "; new EYS: " + GString::FormatNumber(l_fNewExpectedYearsSchooling, 1));
           l_pCountryData->ExpectedYearsSchooling(l_fNewExpectedYearsSchooling);

           REAL32 l_fNewHumanDevelopment = GCountryData::FindHumanDevelopment(l_fNewLifeExpectancy, l_fNewMeanYearsSchooling, l_fNewExpectedYearsSchooling, l_pCountryData->GDPPerCapita(), true);
           l_pCountryData->HumanDevelopment(l_fNewHumanDevelopment);
           //g_Joshua.Log(L"New HDI: " + GString::FormatNumber(l_fNewHumanDevelopment, 3));

           if(g_SP2Server->ShowHDIComponents())
           {
               l_pCountryData->ArableLandLevel(l_fNewHumanDevelopment);
               l_pCountryData->ForestLandLevel(l_fNewLifeExpectancy / 100.f);
               l_pCountryData->ParksLandLevel(l_fNewMeanYearsSchooling / 100.f);
               l_pCountryData->NotUsedLandLevel(l_fNewExpectedYearsSchooling / 100.f);
           }

           //Lose stability, lose 0.25f of usual if region gained through trade.
           REAL32 l_fStabilityLoss = static_cast<REAL32>(l_pRegion->Population()) / static_cast<REAL32>(l_iCurrentCountryPop15 + l_iCurrentCountryPopOver15);
           l_fStabilityLoss *= in_bForceChange ? 0.25f : 1;
           GDZDebug::Log(L"Country " + GString(in_iNewControl) + L", " +
                         g_SP2Server->Countries().at(in_iNewControl - 1).Name() + L" gains region " +
                         g_ServerDAL.GetString(g_ServerDAL.StringIdRegion(in_iRegionID)) +
                         L" with population " + GString::FormatNumber(static_cast<const REAL64>(l_pRegion->Population()), L",", L".", L"", L"", 3, 0) + L"; " +
                         L"loses " + GString::FormatNumber(l_fStabilityLoss, 3) + L" stability",
                         EDZLogCat::Regions,
                         __FUNCTION__, __LINE__);
           ChangeCountryStability(in_iNewControl, -l_fStabilityLoss, false);
       }
   }

   // Test if region owner change has destroyed a country
   if(g_ServerDAL.CountryPoliticalControl(l_CurrentControl.m_iPolitical).empty() )
   {
      g_ServerDAL.DestroyCountryEntity(l_CurrentControl.m_iPolitical, in_iNewControl);

      //Send the news
      {
         News::GInfo l_News;
         l_News.m_iCountryA = l_CurrentControl.m_iPolitical;
         l_News.m_eType = News::EType::Conquered;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(News::EType::Conquered);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(News::EType::Conquered);
		   SendNews(l_News);
      }
   }

   return true;
}

void GDataControlLayer::ChangeCountryCapital(const UINT32 in_iRegionID)
{
	const REAL32 c_fPopulationImportance = 8.f;
   GRegionControl l_CurrentControl = g_ServerDAL.RegionControl(in_iRegionID);

   if(g_ServerDAL.CapitalRegion(l_CurrentControl.m_iPolitical) == in_iRegionID)
   {
      // Capital is located in this region, find a new one
      GVector2D<REAL32> l_PrevLocation = g_ServerDAL.CapitalPosition(l_CurrentControl.m_iPolitical);
      UINT32 l_iBestCityID = 0xFFFFFFFF;
      REAL32 l_fBestCityScore = 0;
      UINT32 l_iBestRegionID = 0xFFFFFFFF;
      REAL32 l_fBestRegionScore = 0;
      const set<UINT32>& l_vPoliticalRegions = g_ServerDAL.CountryPoliticalControl(l_CurrentControl.m_iPolitical);
      for(set<UINT32>::const_iterator i = l_vPoliticalRegions.begin();
         i != l_vPoliticalRegions.end();i ++)
      {
         UINT32 l_iCurRegionId = *i;
         if(g_ServerDAL.RegionControl(l_iCurRegionId).m_iMilitary == l_CurrentControl.m_iPolitical)
         {
            const vector<UINT32>& l_vRegionCities = g_ServerDAL.CitiesInRegion(l_iCurRegionId);
            if(l_vRegionCities.empty() )
            {
               REAL32 l_fRegionScore = 1.f * RSqrt(g_ServerDAL.DistanceBetween2PointsLatLong(g_ServerDAL.RegionGraph().Graph()[l_iCurRegionId].m_Position, l_PrevLocation) );
               if(l_fRegionScore >= l_fBestRegionScore)
               {
                  l_fBestRegionScore = l_fRegionScore;
                  l_iBestRegionID = l_iCurRegionId;
               }
            }
            else
            {
               for(UINT32 j = 0;j < l_vRegionCities.size();j ++)
               {
                  const SCityInfo& l_City = g_ServerDAL.Cities().at(l_vRegionCities[j] );
                  REAL32 l_fCityScore = (REAL32) l_City.m_iPopulation * c_fPopulationImportance *
                     RSqrt(g_ServerDAL.DistanceBetween2PointsLatLong(l_City.m_Position, l_PrevLocation) );

                  if(l_fCityScore >= l_fBestCityScore)
                  {
                     l_fBestCityScore = l_fCityScore;
                     l_iBestCityID = l_City.m_iId;
                  }
               }
            }
         }
      }

      if(l_iBestCityID != 0xFFFFFFFF)
      {
         ConfirmChangeCapitalID(l_CurrentControl.m_iPolitical, l_iBestCityID);
      }
      else if(l_iBestRegionID != 0xFFFFFFFF)
      {
         ConfirmChangeCapitalID(l_CurrentControl.m_iPolitical, l_iBestRegionID | 0x80000000);
      }
      else
      {
         ConfirmChangeCapitalID(l_CurrentControl.m_iPolitical, 0x80000000);
      }
      m_UnitMover.UpdateCapitalLocation(l_CurrentControl.m_iPolitical);
   }
}

void GDataControlLayer::AnnexRegion(UINT32 in_iCountryID, UINT32 in_iRegionID)
{
   gassert(g_ServerDAL.RegionControl(in_iRegionID).m_iMilitary == (UINT16) in_iCountryID, 
           "Trying to annex a region without having military control");

	gassert(g_ServerDAL.RegionControl(in_iRegionID).m_iPolitical != in_iCountryID,
              "Trying to annex a region that is already annexed");

   // If region already is under annexion, make sure it is still valid
   if(g_ServerDAL.RegionIsAnnexed(in_iRegionID) )
   {
      gassert(g_ServerDAL.RegionAnnex(in_iRegionID)->m_iAnnexingCountry == in_iCountryID,
              "Trying to annex a region that is already annexing");
   }
   else
   {      
		g_ServerDAL.AddAnnexToRegion(in_iCountryID, in_iRegionID, c_fDaysForRegionAnnex);
   }
}

void GDataControlLayer::CancelRegionAnnex(UINT32 in_iRegionID)
{
   // Make sure region is currently under annexion
   const GRegionAnnex* l_pAnnex = g_ServerDAL.RegionAnnex(in_iRegionID);
   if(l_pAnnex)
   {
      const vector<GRegionControl>& l_vControl = g_ServerDAL.RegionControlArray();
      GRegionControl l_RegionControl = l_vControl[in_iRegionID];
      if(l_RegionControl.m_iMilitary != l_pAnnex->m_iAnnexingCountry)
      {
         // Iterate through all neighboring regions and remove annexion
         vector<UINT32>       l_vRegionsToVisit;

         l_vRegionsToVisit.reserve(20);
         l_vRegionsToVisit.push_back(in_iRegionID);

         // This is a depth first search of all regions 
         // currently annexed with the same political/military control
         const GRegionGraphNode* l_pGraph = g_ServerDAL.RegionGraph().Graph();
         while(!l_vRegionsToVisit.empty() )
         {
            UINT32 l_iCurRegion = l_vRegionsToVisit.back();
            l_vRegionsToVisit.pop_back();
            GRegionControl l_CurControl = l_vControl[l_iCurRegion];
            const GRegionGraphNode& l_pCurNode = l_pGraph[l_iCurRegion];
            if( (l_RegionControl == l_CurControl) && 
               g_ServerDAL.RegionIsAnnexed(in_iRegionID) )
            {
               g_ServerDAL.RemoveAnnexOfRegion(in_iRegionID);
               for(UINT32 n = 0;n < l_pCurNode.m_iNbNeighbors;n ++)
               {
                  l_vRegionsToVisit.push_back(l_pCurNode.m_pNeighborIDs[n] );
               }
            }
         }
      }
   }
}

bool GDataControlLayer::ChangeMartialLaw(ENTITY_ID in_iCountryID, bool in_bMartialLawStatus)
{
	if(in_bMartialLawStatus == g_ServerDAL.CountryData(in_iCountryID)->MartialLaw())
		return true;

	g_ServerDAL.CountryData(in_iCountryID)->MartialLaw(in_bMartialLawStatus);

	if (in_bMartialLawStatus)
	{
		//A small gain in stability (4%)
		ChangeCountryStability(in_iCountryID,SP2::c_fStabilityChangeMartialLaw,false);
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") declares Martial Law.");

      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::MartialLawOn;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }


	}
	else
	{
		//A small loss in stability (4%)
		ChangeCountryStability(in_iCountryID,-SP2::c_fStabilityChangeMartialLaw,false);
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") removes Martial Law.");

      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType     = News::EType::MartialLawOff;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   in_bMartialLawStatus ? EHistoryMarkerType::DeclareMartialLaw : EHistoryMarkerType::RemoveMartialLaw, 
                                   0);

	return true;
}

bool GDataControlLayer::ChangeGovernmentType(ENTITY_ID in_iCountryID,
															EGovernmentType::Enum in_PreviousGvtType,
															EGovernmentType::Enum in_NewGvtType)
{
	if(in_PreviousGvtType == in_NewGvtType)
		return true;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	const vector<GPoliticalParty>& l_vParties = l_pCountryData->PoliticalParty();
	if(in_NewGvtType == EGovernmentType::MultiPartyDemocracy)
	{
		UINT32 l_iNumberDem = 0;
		for(UINT32 j=0; j<l_vParties.size(); j++)
		{
			if(l_vParties[j].GvtType() == 9)
				l_iNumberDem++;
		}
		//Cannot change to multi-party if there is only 1 (or less) democratic party
		if(l_iNumberDem <= 1)
		{
			in_NewGvtType = EGovernmentType::SinglePartyDemocracy;
			if(in_PreviousGvtType == EGovernmentType::SinglePartyDemocracy)
				return false;
		}
		
	}

	l_pCountryData->GvtType(in_NewGvtType);	

	if( (in_NewGvtType == EGovernmentType::MultiPartyDemocracy) ||
		 (in_NewGvtType == EGovernmentType::SinglePartyDemocracy))
	{
		//Put the election date in 1 year
		Hector::GDateTime l_CurrentDate = g_ServerDAL.ActualDate();
		GDateTime l_NextElection(l_CurrentDate.Year() + 4,l_CurrentDate.Month(),
			l_CurrentDate.Day(),l_CurrentDate.Hours(),l_CurrentDate.Minutes(),
			l_CurrentDate.Seconds(),l_CurrentDate.Tenths());
		l_pCountryData->NextElection(l_NextElection);
	}
	if(in_NewGvtType != EGovernmentType::Anarchy)
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") changes government type from " + g_ServerDAL.GetString(g_ServerDAL.StringIdGvtType(in_PreviousGvtType)) + L" to " + g_ServerDAL.GetString(g_ServerDAL.StringIdGvtType(in_NewGvtType)) + L".");

		vector<INT32> l_vNumbers;
		vector<INT32> l_vStrings;
		l_vStrings.push_back(g_ServerDAL.StringIdGvtType(in_PreviousGvtType));
		l_vStrings.push_back(g_ServerDAL.StringIdGvtType(in_NewGvtType));

      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::ChangedGovernmentType;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         l_News.m_vNumbers = l_vNumbers;
         l_News.m_vStrings = l_vStrings;
         SendNews(l_News);
      }

		//Put back every party legal
		for(UINT32 i=0; i<l_vParties.size(); i++)
		{	
			l_pCountryData->PoliticalPartyChangeStatus(l_vParties[i].Id(),1);
		}
		
		//Country lose some stability (20%)
		if(in_PreviousGvtType != EGovernmentType::Anarchy)
			ChangeCountryStability(in_iCountryID,SP2::c_fStabilityLossGvtType,false);

		//Country will change its political party
		l_pCountryData->LeadingPoliticalPartyID(l_pCountryData->LeadingPoliticalPartyOfType(in_NewGvtType));
				
		INT32 l_iLeadingPartyId = l_pCountryData->LeadingPoliticalPartyID();		
		for(UINT32 i=0; i<l_vParties.size(); i++)
		{		
			INT32 l_iCurrentId = l_vParties[i].Id();
			//If the new type is a democracy, change the in power status of political parties
			if(in_NewGvtType == EGovernmentType::MultiPartyDemocracy ||
				in_NewGvtType == EGovernmentType::SinglePartyDemocracy)
			{
				if(l_vParties[i].GvtType() == EGovernmentType::MultiPartyDemocracy ||
					l_vParties[i].GvtType() == EGovernmentType::SinglePartyDemocracy)
				{
					if(l_iCurrentId == l_iLeadingPartyId)
						l_pCountryData->ChangePoliticalPartyInPower(l_iCurrentId,true);
					else
						l_pCountryData->ChangePoliticalPartyInPower(l_iCurrentId,false);
				}
			}
			//Make every other parties illegal, except when in a Multi-party democraty or when that party is the leading one
			if(l_iCurrentId == l_iLeadingPartyId ||
				in_NewGvtType == EGovernmentType::MultiPartyDemocracy)
				l_pCountryData->PoliticalPartyChangeStatus(l_iCurrentId,1);
			else
				l_pCountryData->PoliticalPartyChangeStatus(l_iCurrentId,2);
		}

		//Change relations with other countries in the world
		UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();		
		for(UINT32 i=1; i<= l_iNbCountry; i++)
		{
			GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);			
			if(i == in_iCountryID || !l_pCountryData->Activated())
				continue;
			REAL32 l_fRelationsOld = g_ServerDAL.RelationBetweenGovernmentType((EGovernmentType::Enum)l_pCountryData->GvtType(),in_PreviousGvtType);
			REAL32 l_fRelationsNew = g_ServerDAL.RelationBetweenGovernmentType((EGovernmentType::Enum)l_pCountryData->GvtType(),in_NewGvtType);
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,
				g_ServerDAL.RelationBetweenCountries(i,in_iCountryID) - l_fRelationsOld + l_fRelationsNew);  			
		}
	}
	else
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") has fallen into anarchy.");

      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::FallenAnarchy;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }
	}

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   EHistoryMarkerType::ChangeGovermentType, 
                                   (REAL32) g_ServerDAL.StringIdGvtType(in_NewGvtType) );

	return true;
}

bool GDataControlLayer::ChangeLanguageStatus(ENTITY_ID in_iCountryID,
															INT32 in_iLanguageID,
															INT32 in_iOldStatus,
															INT32 in_iNewStatus)
{
   static EHistoryMarkerType::Enum s_eStatusToMarker[] = 
   {
      EHistoryMarkerType::TotalCount,
      EHistoryMarkerType::ReligionLanguageLegal,
      EHistoryMarkerType::ReligionLanguageIllegal,
      EHistoryMarkerType::ReligionLanguageOfficial,
   };

	if(in_iOldStatus == in_iNewStatus)
		return true;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	//check if it's the last legal or official status
	if(in_iNewStatus == 2)
	{
		UINT32 l_iNbLegalOfficial = 0;		
		const GLanguageList& l_vLanguages = l_pCountryData->GetLanguages();
		for(GLanguageList::const_iterator l_Itr = l_vLanguages.begin();
			 l_Itr != l_vLanguages.end();
			 l_Itr++)
		{
			if(l_pCountryData->LanguageGetStatus(l_Itr->first) != 2)
				l_iNbLegalOfficial++;
		}
		if(l_iNbLegalOfficial <= 1)
			return false;
	}
	
	l_pCountryData->LanguageChangeStatus(in_iLanguageID,in_iNewStatus);

	//Whenever you change a language status, every country that has this language and has a different status towards it will react good, or bad
	for(UINT32 i=1; (INT16)i <= g_ServerDAL.NbCountry(); i++)
	{
		if(i != in_iCountryID && g_ServerDAL.CountryData(i)->HasLanguage(in_iLanguageID) == true)
		{
			//If a country makes a Language illegal, every other country where this Language is legal drops relation by 5
			//If a country makes a Language illegal, every other country where this Language is official drops relation by 10
			//If a country makes a Language illegal, every other country where this Language is illegal raises relation by 5
			INT32 l_iLanguageStatus = g_ServerDAL.CountryData(i)->LanguageGetStatus(in_iLanguageID);
			if(in_iNewStatus == 2)
			{
				if (l_iLanguageStatus == 1)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
				else if (l_iLanguageStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-10.f);
				else if (l_iLanguageStatus == 2)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
			}
			//If the Language status was from illegal to legal, it will raise relatios from 5 or 10, depending if the other country has this Language legal or official
			else if (in_iNewStatus == 1 && in_iOldStatus == 2)
			{
				if (l_iLanguageStatus == 1)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
				else if (l_iLanguageStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+10.f);
			}
			//If the Language was official, and now is legal, it will drop relations by 5 from countries where it is official
			else if (in_iNewStatus == 1 && in_iOldStatus == 3)
			{
				if (l_iLanguageStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
			}
			//If the Language is now official, it will raise relations with other countries where that Language is official
			//If the Language is now official, it will lower relations with other countries where that Language is illegal
			else if (in_iNewStatus == 3)
			{
				if (l_iLanguageStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
				else if (l_iLanguageStatus == 2)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
			}
		}
	}//Ends relation changes

	REAL32 l_fMod = l_pCountryData->LanguageGetPourcentage(in_iLanguageID);
	//Country lose, or gain,  some stability and approval
	if (in_iOldStatus == 3 && in_iNewStatus == 1)	//Official to legal
	{
		ChangeCountryStability(in_iCountryID,-SP2::c_fStabilityChangeOfficialToLegal * l_fMod);
		ChangeCountryApproval(in_iCountryID,-SP2::c_fApprovalChangeOfficialToLegal * l_fMod);
	}
	else if (in_iOldStatus == 3 && in_iNewStatus == 2)	//Official to illegal
	{
		ChangeCountryStability(in_iCountryID,-(SP2::c_fStabilityChangeOfficialToLegal + SP2::c_fStabilityChangeLegalToIllegal) * l_fMod);
		ChangeCountryApproval(in_iCountryID,-(SP2::c_fApprovalChangeOfficialToLegal + SP2::c_fApprovalChangeLegalToIllegal) * l_fMod);
	}
	else if (in_iOldStatus == 1 && in_iNewStatus == 2)	//Legal to illegal
	{
		ChangeCountryStability(in_iCountryID,-SP2::c_fStabilityChangeLegalToIllegal * l_fMod);
		ChangeCountryApproval(in_iCountryID,-SP2::c_fApprovalChangeLegalToIllegal * l_fMod);
	}
	else if (in_iOldStatus == 1 && in_iNewStatus == 3)	//Legal to official
	{
		ChangeCountryApproval(in_iCountryID,SP2::c_fApprovalChangeOfficialToLegal * l_fMod);
	}
	else if (in_iOldStatus == 2 && in_iNewStatus == 1)	//Illegal to legal
	{
		ChangeCountryApproval(in_iCountryID,SP2::c_fApprovalChangeLegalToIllegal * l_fMod);
	}
	else if (in_iOldStatus == 2 && in_iNewStatus == 3)	//Illegal to official
	{
		ChangeCountryApproval(in_iCountryID,(SP2::c_fApprovalChangeOfficialToLegal + SP2::c_fApprovalChangeLegalToIllegal) * l_fMod);
	}

	LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") changes the status of language: " + g_ServerDAL.GetString(g_ServerDAL.StringIdLanguage(in_iLanguageID)) + L" from " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iOldStatus)) + L" to " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iNewStatus)) + L".");


	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vStrings.push_back(g_ServerDAL.StringIdLanguage(in_iLanguageID));
	l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iOldStatus));
	l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iNewStatus));

   //Send a news ticker
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = News::EType::ChangeStatusLanguage;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   s_eStatusToMarker[in_iNewStatus], 
                                   (REAL32) g_ServerDAL.StringIdLanguage(in_iLanguageID) );

	return true;
}

//! Change how a resource is controlled, public or private
bool GDataControlLayer::ChangeResourceGvtControlled(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, bool in_bControlled)
{
	if(g_ServerDAL.CountryData(in_iCountryID)->ResourceGvtCtrl(in_iResource) == in_bControlled)
		return true;

	g_ServerDAL.CountryData(in_iCountryID)->ResourceGvtCtrl(in_iResource,in_bControlled);

	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vStrings.push_back(g_ServerDAL.StringIdResource(in_iResource));

	News::EType::Enum l_eType = News::EType::PrivateControlRes;
	if(in_bControlled)
		l_eType = News::EType::PublicControlRes;
		
   //Send a news ticker
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = l_eType;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   in_bControlled ? EHistoryMarkerType::ResourceIsPublic : EHistoryMarkerType::ResourceIsPrivate, 
                                   100.f / (REAL32) EResources::ItemCount);

	return true;
}

//! Change global tax mod
bool GDataControlLayer::ChangeGlobalModTax(ENTITY_ID in_iCountryID, REAL32 in_fNewTax)
{
	REAL32 l_fOldTax = g_ServerDAL.CountryData(in_iCountryID)->GlobalTaxMod();
    in_fNewTax = min(in_fNewTax, g_SP2Server->GlobalTaxLimit());

	News::EType::Enum l_eType;
   EHistoryMarkerType::Enum l_eMarker;
	if(l_fOldTax < in_fNewTax)
   {
		l_eType = News::EType::RaiseGlobalModTax;
      l_eMarker = EHistoryMarkerType::IncreaseGlobalResourceTax;
   }
	else if (l_fOldTax > in_fNewTax)
   {
		l_eType = News::EType::LowerGlobalModTax;
      l_eMarker = EHistoryMarkerType::DecreaseGlobalResourceTax;
   }
	else
   {
      return true;
   }

   //Send a news ticker
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = l_eType;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      SendNews(l_News);
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   l_eMarker, 
                                   fabsf(l_fOldTax - in_fNewTax) * 100.f);

	g_ServerDAL.CountryData(in_iCountryID)->GlobalTaxMod(in_fNewTax);
	return true;
}

//! Change resource tax
bool GDataControlLayer::ChangeResourceTax(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL32 in_fNewTax)
{
	REAL32 l_fOldTax = g_ServerDAL.CountryData(in_iCountryID)->ResourceTaxes(in_iResource);
    in_fNewTax = min(in_fNewTax, g_SP2Server->ResourceTaxLimit());

	News::EType::Enum l_eType;
   EHistoryMarkerType::Enum l_eMarker;
	if(l_fOldTax < in_fNewTax)
   {
      l_eType = News::EType::RaiseTaxRes;
      l_eMarker = EHistoryMarkerType::IncreaseResourceTax;
   }
	else if (l_fOldTax > in_fNewTax)
   {
      l_eType = News::EType::LowerTaxRes;
      l_eMarker = EHistoryMarkerType::DecreaseResourceTax;
   }
	else
   {
      return true;
   }

	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vStrings.push_back(g_ServerDAL.StringIdResource(in_iResource));

   //Send a news ticker
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = l_eType;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   l_eMarker, 
                                   fabsf(l_fOldTax - in_fNewTax) * 100.f);

	g_ServerDAL.CountryData(in_iCountryID)->ResourceTaxes(in_iResource,in_fNewTax);
	return true;
}

//! Change resource import desired
bool GDataControlLayer::ChangeResourceImportDesired(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL64 in_fNewDesired)
{
	if(g_ServerDAL.CountryData(in_iCountryID)->ResourceGvtCtrl(in_iResource))
		g_ServerDAL.CountryData(in_iCountryID)->ResourceImportDesired(in_iResource,in_fNewDesired);
	return true;
}

//! Change resource export desired
bool GDataControlLayer::ChangeResourceExportDesired(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL64 in_fNewDesired)
{
	if(g_ServerDAL.CountryData(in_iCountryID)->ResourceGvtCtrl(in_iResource))
	{

		g_ServerDAL.CountryData(in_iCountryID)->ResourceExportDesired(in_iResource,
			min(in_fNewDesired, g_ServerDAL.CountryData(in_iCountryID)->ResourceProduction(in_iResource)));
	}
	return true;
}

//! Change resource status
bool GDataControlLayer::ChangeResourceStatus(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, bool in_bNewStatus)
{
	
	if(g_ServerDAL.CountryData(in_iCountryID)->ResourceLegal(in_iResource) == in_bNewStatus)
		return true;
	
	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vStrings.push_back(g_ServerDAL.StringIdResource(in_iResource));

   News::EType::Enum l_eType = News::EType::IllegalizedRes;
	if(in_bNewStatus)
      l_eType = News::EType::LegalizedRes;
	
   //Send a news ticker
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = l_eType;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }

	g_ServerDAL.CountryData(in_iCountryID)->ResourceLegal(in_iResource,in_bNewStatus);
	return true;
}

bool GDataControlLayer::ChangeReligionStatus(ENTITY_ID in_iCountryID,
															INT32 in_iReligionID,
															INT32 in_iOldStatus,
															INT32 in_iNewStatus)
{
   static EHistoryMarkerType::Enum s_eStatusToMarker[] = 
   {
      EHistoryMarkerType::TotalCount,
      EHistoryMarkerType::ReligionLanguageLegal,
      EHistoryMarkerType::ReligionLanguageIllegal,
      EHistoryMarkerType::ReligionLanguageOfficial,
   };

	if(in_iOldStatus == in_iNewStatus)
		return true;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	//check if it's the last legal or official status
	if(in_iNewStatus == 2)
	{
		UINT32 l_iNbLegalOfficial = 0;
		
		const GReligionList& l_vReligions = l_pCountryData->GetReligions();
		for(GReligionList::const_iterator l_Itr = l_vReligions.begin();
			 l_Itr != l_vReligions.end();
			 l_Itr++)
		{
			if(l_pCountryData->ReligionGetStatus(l_Itr->first) != 2)
				l_iNbLegalOfficial++;
		}
		if(l_iNbLegalOfficial <= 1)
			return false;
	}

	l_pCountryData->ReligionChangeStatus(in_iReligionID,in_iNewStatus);

	//Whenever you change a religion status, every country that has this religion and has a different status towards it will react good, or bad
	for(UINT32 i=1; (INT16)i <= g_ServerDAL.NbCountry(); i++)
	{
		if(i != in_iCountryID && g_ServerDAL.CountryData(i)->HasReligion(in_iReligionID) == true)
		{
			//If a country makes a religion illegal, every other country where this religion is legal drops relation by 5
			//If a country makes a religion illegal, every other country where this religion is official drops relation by 10
			//If a country makes a religion illegal, every other country where this religion is illegal raises relation by 5
			INT32 l_iReligionStatus = g_ServerDAL.CountryData(i)->ReligionGetStatus(in_iReligionID);
			if(in_iNewStatus == 2)
			{
				if (l_iReligionStatus == 1)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
				else if (l_iReligionStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-10.f);
				else if (l_iReligionStatus == 2)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
			}
			//If the religion status was from illegal to legal, it will raise relatios from 5 or 10, depending if the other country has this religion legal or official
			else if (in_iNewStatus == 1 && in_iOldStatus == 2)
			{
				if (l_iReligionStatus == 1)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
				else if (l_iReligionStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+10.f);
			}
			//If the religion was official, and now is legal, it will drop relations by 5 from countries where it is official
			else if (in_iNewStatus == 1 && in_iOldStatus == 3)
			{
				if (l_iReligionStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
			}
			//If the religion is now official, it will raise relations with other countries where that religion is official
			//If the religion is now official, it will lower relations with other countries where that religion is illegal
			else if (in_iNewStatus == 3)
			{
				if (l_iReligionStatus == 3)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)+5.f);
				else if (l_iReligionStatus == 2)
					g_ServerDAL.RelationBetweenCountries(i,
					in_iCountryID,
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID)-5.f);
			}
		}
	}//Ends relation changes

	REAL32 l_fMod = l_pCountryData->ReligionGetPourcentage(in_iReligionID);
	//Country lose, or gain,  some stability and approval
	if (in_iOldStatus == 3 && in_iNewStatus == 1)	//Official to legal
	{
		ChangeCountryStability(in_iCountryID,-SP2::c_fStabilityChangeOfficialToLegal * l_fMod);
		ChangeCountryApproval(in_iCountryID,-SP2::c_fApprovalChangeOfficialToLegal * l_fMod);
	}
	else if (in_iOldStatus == 3 && in_iNewStatus == 2)	//Official to illegal
	{
		ChangeCountryStability(in_iCountryID,-(SP2::c_fStabilityChangeOfficialToLegal + SP2::c_fStabilityChangeLegalToIllegal) * l_fMod);
		ChangeCountryApproval(in_iCountryID,-(SP2::c_fApprovalChangeOfficialToLegal + SP2::c_fApprovalChangeLegalToIllegal) * l_fMod);
	}
	else if (in_iOldStatus == 1 && in_iNewStatus == 2)	//Legal to illegal
	{
		ChangeCountryStability(in_iCountryID,-SP2::c_fStabilityChangeLegalToIllegal * l_fMod);
		ChangeCountryApproval(in_iCountryID,-SP2::c_fApprovalChangeLegalToIllegal * l_fMod);
	}
	else if (in_iOldStatus == 1 && in_iNewStatus == 3)	//Legal to official
	{
		ChangeCountryApproval(in_iCountryID,SP2::c_fApprovalChangeOfficialToLegal * l_fMod);
	}
	else if (in_iOldStatus == 2 && in_iNewStatus == 1)	//Illegal to legal
	{
		ChangeCountryApproval(in_iCountryID,SP2::c_fApprovalChangeLegalToIllegal * l_fMod);
	}
	else if (in_iOldStatus == 2 && in_iNewStatus == 3)	//Illegal to official
	{
		ChangeCountryApproval(in_iCountryID,(SP2::c_fApprovalChangeOfficialToLegal + SP2::c_fApprovalChangeLegalToIllegal) * l_fMod);
	}

	LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") changes the status of religion: " + g_ServerDAL.GetString(g_ServerDAL.StringIdReligion(in_iReligionID)) + L" from " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iOldStatus)) + L" to " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iNewStatus)) + L".");



   //Send a news ticker
   {
	   vector<INT32> l_vStrings;
	   l_vStrings.push_back(g_ServerDAL.StringIdReligion(in_iReligionID));
	   l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iOldStatus));
	   l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iNewStatus));

      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = News::EType::ChangeStatusReligion;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   s_eStatusToMarker[in_iNewStatus], 
                                   (REAL32) g_ServerDAL.StringIdReligion(in_iReligionID) );

	return true;
}

bool GDataControlLayer::ChangePoliticalPartyStatus(ENTITY_ID in_iCountryID,
																	INT32 in_iPartyID,
																	INT32 in_iOldStatus,
																	INT32 in_iNewStatus)
{
	if(in_iOldStatus == in_iNewStatus || in_iNewStatus == 3)
		return true;
	
	g_ServerDAL.CountryData(in_iCountryID)->PoliticalPartyChangeStatus(in_iPartyID,in_iNewStatus);
   
	REAL32 l_fPopularity = 0.f;
	const vector<GPoliticalParty>& l_vParties = g_ServerDAL.CountryData(in_iCountryID)->PoliticalParty();
	for(UINT32 i=0; i<l_vParties.size(); i++)
	{
		if(in_iPartyID == l_vParties[i].Id())
		{
			l_fPopularity = l_vParties[i].PercentageValue();
			break;
		}
	}
   
	LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") changes the status of party: " + g_ServerDAL.GetString(g_ServerDAL.StringIdPoliticalParty(in_iPartyID)) + L" from " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iOldStatus)) + L" to " + g_ServerDAL.GetString(g_ServerDAL.StringIdStatus(in_iNewStatus)) + L".");
   
   //Send a news ticker
   
   {
	   vector<INT32> l_vStrings;
	   l_vStrings.push_back(g_ServerDAL.StringIdPoliticalParty(in_iPartyID));
	   l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iOldStatus));
	   l_vStrings.push_back(g_ServerDAL.StringIdStatus(in_iNewStatus));

      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = News::EType::ChangeStatusPoliticalParties;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }
   
   
	//Check if the current government type is Single-Party. If it is and we're making a party legal, switch to multi-party democraty
	if(in_iOldStatus == 2 && 
		in_iNewStatus == 1 && 
		(EGovernmentType::Enum)g_ServerDAL.CountryData(in_iCountryID)->GvtType() == EGovernmentType::SinglePartyDemocracy)
	{
		ChangeGovernmentType(in_iCountryID,EGovernmentType::SinglePartyDemocracy,EGovernmentType::MultiPartyDemocracy);
	}
	else if(in_iOldStatus == 1 &&
			  in_iNewStatus == 2 &&
			  (EGovernmentType::Enum)g_ServerDAL.CountryData(in_iCountryID)->GvtType() == EGovernmentType::MultiPartyDemocracy)
	{
		//Check if there's only 1 government type left
		UINT32 l_iNumberOfLegalDemocraticParties = 0;
		const vector<GPoliticalParty>& l_vParties = g_ServerDAL.CountryData(in_iCountryID)->PoliticalParty();
		for(UINT32 i=0; i < l_vParties.size(); i++)
		{
			if(l_vParties[i].Status() == 1 && l_vParties[i].GvtType() == 9)
				l_iNumberOfLegalDemocraticParties++;
		}
		if(l_iNumberOfLegalDemocraticParties <= 1)
			ChangeGovernmentType(in_iCountryID,EGovernmentType::MultiPartyDemocracy,EGovernmentType::SinglePartyDemocracy);
	}	
	
	if(in_iNewStatus == 2) //Illegal
	{
		//Country loses approval ratings
		ChangeCountryApproval(in_iCountryID,-(l_fPopularity*0.5f));
		ChangeCountryStability(in_iCountryID,-(l_fPopularity*0.5f));

      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::PoliticPartyIllegal, 
                                      (REAL32) g_ServerDAL.StringIdPoliticalParty(in_iPartyID) );
   }	
	else //legal
	{
		ChangeCountryApproval(in_iCountryID,(l_fPopularity*0.5f));
		ChangeCountryStability(in_iCountryID,(l_fPopularity*0.5f));

      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::PoliticPartyLegal, 
                                      (REAL32) g_ServerDAL.StringIdPoliticalParty(in_iPartyID) );
   }	
   
	return true;
}

bool GDataControlLayer::ChangeEmigration(ENTITY_ID in_iCountryID, bool in_bEmigrationStatus)
{
	REAL32 l_fRelation = 0.f;
	for(UINT32 i=1; (INT16)i<=g_ServerDAL.NbCountry(); i++)
	{
		if(i==in_iCountryID)
			continue;
		//If your emigration is different from another country, the relations go down by 5.
		//If they are the same, relations go up by 5
		l_fRelation = g_ServerDAL.RelationBetweenCountries(i,in_iCountryID);
		if(g_ServerDAL.CountryData(i)->EmigrationClosed() == in_bEmigrationStatus)
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,(l_fRelation+5.f));
		else
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,(l_fRelation-5.f));
	}

	g_ServerDAL.CountryData(in_iCountryID)->EmigrationClosed(in_bEmigrationStatus);


	if(in_bEmigrationStatus)
	{
		//Country lose some stability (5%)
		ChangeCountryStability(in_iCountryID,-0.05f);
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") closes Emigration.");



      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::ClosedEmigration;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
     }
	}
	else
	{
		//Country lose some stability (1%)
		ChangeCountryStability(in_iCountryID,-0.01f);
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") opens Emigration.");

      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::OpenedEmigration;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }

	}


	return true;
}

bool GDataControlLayer::ChangeImmigration(ENTITY_ID in_iCountryID, bool in_bImmigrationStatus)
{
	REAL32 l_fRelation = 0.f;

	for(UINT32 i=1; (INT16)i<=g_ServerDAL.NbCountry(); i++)
	{
		if(i==in_iCountryID)
			continue;
		//If your immigration is different from another country, the relations go down by 0.05.
		//If they are the same, relations go up by 0.05
		l_fRelation = g_ServerDAL.RelationBetweenCountries(i,in_iCountryID);
		if(g_ServerDAL.CountryData(i)->ImmigrationClosed() == in_bImmigrationStatus)
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,(l_fRelation+5.f));
		else
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,(l_fRelation-5.f));
	}

	g_ServerDAL.CountryData(in_iCountryID)->ImmigrationClosed(in_bImmigrationStatus);

	if(in_bImmigrationStatus)
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") closes Immigration.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType     = News::EType::ClosedImmigration;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }
	}
	else
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") opens Immigration.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::OpenedImmigration;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }
	}

	//Country lose some stability (2%)
	ChangeCountryStability(in_iCountryID,-0.02f);

	return true;
}

bool GDataControlLayer::ChangeBudgetExpenseRatio(UINT32 in_iCountryID, 
																 EBudgetExpenses::Enum in_Expense, 
																 REAL64 in_fNewRatio)
{
	if(in_fNewRatio > 1.f)
		in_fNewRatio = 1.f;
	else if(in_fNewRatio < 0.f)
		in_fNewRatio = 0.f;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL64 l_fOldRatio = 0.f;
	REAL64 l_fDifference = 0.f;

	REAL32 l_fMod = 1.f;

	switch(in_Expense)
	{
	case EBudgetExpenses::Education:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseEducationRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseEducationRatio(in_fNewRatio);

			l_fMod = 0.5f + g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
																						SP2::c_fAIActionNeutral,
																						SP2::c_fAIActionMinus,
																						SP2::c_fAIActionDoubleMinus,
																						SP2::c_fAIActionNeutral);			
		}
		break;
	case EBudgetExpenses::Environment:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseEnvironmentRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseEnvironmentRatio(in_fNewRatio);
			l_fMod = 0.5f + g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
																						SP2::c_fAIActionNeutral,
																						SP2::c_fAIActionMinus,
																						SP2::c_fAIActionDoubleMinus,
																						SP2::c_fAIActionDoubleMinus);	
		}
		break;
	case EBudgetExpenses::Government:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseGovernmentRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseGovernmentRatio(in_fNewRatio);
			l_fMod = 0.f;
		}
		break;
	case EBudgetExpenses::Healthcare:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseHealthcareRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseHealthcareRatio(in_fNewRatio);
			l_fMod = 0.5f + g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
																						SP2::c_fAIActionNeutral,
																						SP2::c_fAIActionMinus,
																						SP2::c_fAIActionDoubleMinus,
																						SP2::c_fAIActionNeutral);	
		}
		break;
	case EBudgetExpenses::IMF:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseIMFRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseIMFRatio(in_fNewRatio);
			l_fMod = 0.f;
			//Raising IMF raises relations over the world
			for(UINT32 i=1; (INT16)i <= g_ServerDAL.NbCountry(); i++)
			{
				REAL32 l_fRelation = g_ServerDAL.RelationBetweenCountries(i,in_iCountryID);
				if (i != in_iCountryID)
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,l_fRelation + ((REAL32)l_fDifference * EHEConstants::RelationPropaganda));
			}
		}
		break;
	case EBudgetExpenses::Infrastructure:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseInfrastructureRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseInfrastructureRatio(in_fNewRatio);			
		}
		break;
	case EBudgetExpenses::Propaganda:
		{
			l_fOldRatio = l_pCountryData->BudgetExpensePropagandaRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpensePropagandaRatio(in_fNewRatio);
			l_fMod = 0.f;
			//Raising propaganda lowers relations over the world
			for(UINT32 i=1; (INT16)i <= g_ServerDAL.NbCountry(); i++)
			{
				REAL32 l_fRelation = g_ServerDAL.RelationBetweenCountries(i,in_iCountryID);
				if (i != in_iCountryID)
					g_ServerDAL.RelationBetweenCountries(i,in_iCountryID,l_fRelation + -((REAL32)l_fDifference * EHEConstants::RelationPropaganda));
			}
		}
		break;
	case EBudgetExpenses::Research:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseResearchRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseResearchRatio(in_fNewRatio);
			l_fMod = 0.f;
		}
		break;
	case EBudgetExpenses::Telecom:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseTelecomRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseTelecomRatio(in_fNewRatio);
			l_fMod = 0.5f + g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
																						SP2::c_fAIActionDoublePlus,
																						SP2::c_fAIActionMinus,
																						SP2::c_fAIActionNeutral,
																						SP2::c_fAIActionNeutral);	
		}
		break;
	case EBudgetExpenses::Tourism:
		{
			l_fOldRatio = l_pCountryData->BudgetExpenseTourismRatio();
			l_fDifference = in_fNewRatio - l_fOldRatio;
			l_pCountryData->BudgetExpenseTourismRatio(in_fNewRatio);
		}
		break;
	default:
		gassert(false,"GDataControlLayer::ChangeBudgetExpenseRatio: Non existing expense");
		return false;
	}	

	g_ServerDCL.ChangeCountryApproval(in_iCountryID, 
		(REAL32)l_fDifference * SP2::c_fApprovalRatioChangeBudgetExpense * l_fMod);

	return true;
}

//! Change the interest rate of a country
bool GDataControlLayer::ChangeInterestRate(ENTITY_ID in_iCountryID,
														 REAL32 l_fOldInterestRate,
														 REAL32 l_fNewInterestRate)
{
	if(l_fNewInterestRate < SP2::InterestRate_LowerCap)
		l_fNewInterestRate = SP2::InterestRate_LowerCap;
	else if (l_fNewInterestRate > SP2::InterestRate_UpperCap)
		l_fNewInterestRate = SP2::InterestRate_UpperCap;

	g_ServerDAL.CountryData(in_iCountryID)->InterestLevel(l_fNewInterestRate);

	if (l_fNewInterestRate > l_fOldInterestRate)
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") raises Interest Rate.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::RaiseInterestRate;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }

      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::IncreaseInterestLevel, 
                                      (REAL32) (l_fNewInterestRate - l_fOldInterestRate) * 100.f);
	}
	else if (l_fNewInterestRate < l_fOldInterestRate)
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") lowers Interest Rate.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::LowerInterestRate;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }

      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::DecreaseInterestLevel, 
                                      (REAL32) (l_fOldInterestRate - l_fNewInterestRate) * 100.f);
   }

	return true;
}

//! Change the Personal Income Tax of a country
bool GDataControlLayer::ChangePersonalIncomeTax(ENTITY_ID in_iCountryID,
																REAL64 l_fOldPersonalIncomeTax,
																REAL64 l_fNewPersonalIncomeTax)
{
   // Nothing to do when value is identical
   if(l_fOldPersonalIncomeTax == l_fNewPersonalIncomeTax)
      return true;

   l_fNewPersonalIncomeTax = min(l_fNewPersonalIncomeTax, g_SP2Server->IncomeTaxLimit(static_cast<EGovernmentType::Enum>(g_ServerDAL.CountryData(in_iCountryID)->GvtType())));
	if(l_fNewPersonalIncomeTax < SP2::PersonalTaxes_LowerCap)
		l_fNewPersonalIncomeTax = SP2::PersonalTaxes_LowerCap;
	else if (l_fNewPersonalIncomeTax > SP2::PersonalTaxes_UpperCap)
		l_fNewPersonalIncomeTax = SP2::PersonalTaxes_UpperCap;

	REAL32 l_fRange = SP2::PersonalTaxes_UpperCap - SP2::PersonalTaxes_LowerCap;
	REAL32 l_fDifference = -(REAL32)(l_fNewPersonalIncomeTax - l_fOldPersonalIncomeTax);
   // are we lowering taxes ?
   if(l_fDifference > 0)
	   l_fDifference = (l_fDifference / l_fRange) * SP2::c_fApprovalRatioLowerPersonalTax;
   else
      l_fDifference = (l_fDifference / l_fRange) * SP2::c_fApprovalRatioRaisePersonalTax;

	ChangeCountryApproval(in_iCountryID,l_fDifference);

	g_ServerDAL.CountryData(in_iCountryID)->PersonalIncomeTax(l_fNewPersonalIncomeTax);

	if (l_fNewPersonalIncomeTax > l_fOldPersonalIncomeTax)
	{
		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") raises Personal Income Tax.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::RaisePersonalIncomeTax;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }
      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::IncreaseTaxes, 
                                      (REAL32) (l_fNewPersonalIncomeTax - l_fOldPersonalIncomeTax) * 100.f);
	}
	else
	{
      gassert(l_fNewPersonalIncomeTax < l_fOldPersonalIncomeTax, "New tax rate should be lower");

		LogNewAction(L"Country ID: (" + GString(in_iCountryID) + L") lowers Personal Income Tax.");
      //Send a news ticker
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iCountryID;
         l_News.m_eType = News::EType::LowerPersonalIncomeTax;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         SendNews(l_News);
      }

      g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                      EHistoryMarkerType::DecreaseTaxes, 
                                      (REAL32) (l_fOldPersonalIncomeTax - l_fNewPersonalIncomeTax) * 100.f);
	}	
	
	IterateBudgetRevenueTax(in_iCountryID);

	return true;
}

bool GDataControlLayer::JoinAWar(UINT32 in_iCountryID, UINT32 in_iWarToJoin, UINT32 in_iSide, bool in_bWantToContinueWar)
{

	GWar* l_pWar = g_ServerDAL.War(in_iWarToJoin);	

   IF_RETURN(!l_pWar,false);
   

	if(in_iSide == 1)
	{
        gassert(l_pWar->MasterDefending() != in_iCountryID        &&
                l_pWar->DefendingSide().count(in_iCountryID) == 0,
                g_ServerDAL.CountryData(in_iCountryID)->NameAndIDForLog() +
                L" is trying to join a war against itself, " +
                g_ServerDAL.CountryData(l_pWar->MasterAttacking())->NameAndIDForLog() +
                L" vs. " +
                g_ServerDAL.CountryData(l_pWar->MasterDefending())->NameAndIDForLog());
		if(!l_pWar->AttackingSide().count(in_iCountryID))
		{
			l_pWar->AddCountryToAttackingSide(in_iCountryID);
			//Declare war to every countries on defending side
			for(set<UINT32>::const_iterator it = l_pWar->DefendingSide().begin(); 
				 it != l_pWar->DefendingSide().end(); it++)
			{
				if(*it == l_pWar->MasterDefending())
					DeclareWar(in_iCountryID,*it,l_pWar->AttackingSide(),true, true);
				else
					DeclareWar(in_iCountryID,*it,l_pWar->AttackingSide(),false, true);
			}			
		}
	}
	else if(in_iSide == 2)
	{
        gassert(l_pWar->MasterAttacking() != in_iCountryID        &&
                l_pWar->AttackingSide().count(in_iCountryID) == 0,
                g_ServerDAL.CountryData(in_iCountryID)->NameAndIDForLog() +
                L" is trying to join a war against itself, " +
                g_ServerDAL.CountryData(l_pWar->MasterAttacking())->NameAndIDForLog() +
                L" vs. " +
                g_ServerDAL.CountryData(l_pWar->MasterDefending())->NameAndIDForLog());
		if(!l_pWar->DefendingSide().count(in_iCountryID))
		{
			l_pWar->AddCountryToDefendingSide(in_iCountryID);
			//Declare war to every countries on attacking side
			for(set<UINT32>::const_iterator it = l_pWar->AttackingSide().begin(); 
				 it != l_pWar->AttackingSide().end(); it++)
			{
				DeclareWar(in_iCountryID,*it,l_pWar->AttackingSide(),false, false);				
			}	
		}
	}

    //All client states join; can recursively call JoinAWar because clients can't have clients of their own
    const auto l_vClients = g_ServerDAL.CountryData(in_iCountryID)->Clients();
    for(auto it = l_vClients.cbegin(); it != l_vClients.cend(); ++it)
        JoinAWar(it->first, in_iWarToJoin, in_iSide, in_bWantToContinueWar);

	g_ServerDAL.ModifyWar(in_iWarToJoin);
	set<UINT32> l_NullTreaties;
	if(VerifyPeaceStatus(l_pWar,l_NullTreaties))
		g_ServerDAL.RemoveWar(in_iWarToJoin);
	
	return true;
}

bool GDataControlLayer::DeclareNewWar(const set<UINT32>& in_AttackingCountries, UINT32 in_iMasterAttacking, UINT32 in_iDefendingCountry)
{
    //Add client states to attackers
    set<ENTITY_ID> l_vAttackers = in_AttackingCountries;
    for(auto it = in_AttackingCountries.cbegin();
        it != in_AttackingCountries.cend();
        ++it)
    {
        const GCountryData* const l_pCountryData = g_ServerDAL.CountryData(*it);
        const auto l_vClients = l_pCountryData->Clients();
        for(auto cit = l_vClients.cbegin(); cit != l_vClients.cend(); ++cit)
        {
            if(cit->first != in_iDefendingCountry && l_vAttackers.count(cit->first) == 0)
            {
                GDZLOG(g_ServerDAL.CountryData(cit->first)->NameAndIDForLog() +
                       L" joins war on the same side as its master, " +
                       l_pCountryData->NameAndIDForLog(),
                       EDZLogCat::ClientStates);
                l_vAttackers.insert(l_vAttackers.cend(), cit->first);
            }
        }
    }

	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();

	//Check if an identical war already exist, if so, do not create a new one
	for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
			it != l_Wars.end(); it++)
	{
		const GWar& l_CurWar = it->second;
		if(l_CurWar.MasterAttacking() == in_iMasterAttacking &&
			l_CurWar.MasterDefending() == in_iDefendingCountry)
		{
			//compare the members
			bool l_bAllAttackersAreThere = true;
			for(set<UINT32>::const_iterator it = l_vAttackers.begin(); 
				it != l_vAttackers.end(); it++)
			{
				if(l_CurWar.AttackingSide().count(*it) == 0)
				{
					l_bAllAttackersAreThere = false;
					break;
				}
			}
			if(l_bAllAttackersAreThere)
				return true;

		}
	}

	//If we are here, no similiar war has been found, so create a new one
	set<UINT32> l_CountriesAlreadyInSamePosition;
	GWar l_NewWar;
	for(set<UINT32>::const_iterator it = l_vAttackers.begin(); 
		 it != l_vAttackers.end(); it++)
	{
		l_NewWar.AddCountryToAttackingSide(*it);		
		DeclareWar(*it,in_iDefendingCountry,l_vAttackers,true, true);		
	}	
	l_NewWar.AddCountryToDefendingSide(in_iDefendingCountry);
	l_NewWar.MasterAttacking(in_iMasterAttacking);
	l_NewWar.MasterDefending(in_iDefendingCountry);
	l_NewWar.MasterAttackingWantsPeace(false);
	l_NewWar.MasterDefendingWantsPeace(true);
	l_NewWar.Date(g_ServerDAL.ActualDate());	
   g_ServerDAL.AddWar(l_NewWar);	
	UINT32 l_iWarID = l_NewWar.ID();

    //Add client states to defenders
    const auto l_vClients = g_ServerDAL.CountryData(in_iDefendingCountry)->Clients();
    for(auto it = l_vClients.cbegin(); it != l_vClients.cend(); ++it)
    {
        if(it->first != in_iMasterAttacking && l_vAttackers.count(it->first) == 0)
            JoinAWar(it->first, l_iWarID, 2, false);
    }

	//Alert AI that a war has been declared
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GWarDeclarationNotify);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastAIPlayers;
	SP2::Event::GWarDeclarationNotify* l_pWarNotify = (SP2::Event::GWarDeclarationNotify*)l_Event.get();
	l_pWarNotify->WarId = l_iWarID;
	g_Joshua.RaiseEvent(l_Event);

	return true;
}

void GDataControlLayer::ChangeOpinionOnWar(UINT32 in_iCountryID, UINT32 in_iWarID, bool in_bWantsPeace)
{
	GWar* l_pWar = g_ServerDAL.War(in_iWarID);
	if(!l_pWar)
		return;

	set<UINT32> l_NullTreaties;

	if(in_iCountryID == l_pWar->MasterAttacking())
	{
		if(l_pWar->MasterAttackingWantsPeace() != in_bWantsPeace)
		{
			l_pWar->MasterAttackingWantsPeace(in_bWantsPeace);
			g_ServerDAL.ModifyWar(in_iWarID);
			if(VerifyPeaceStatus(l_pWar,l_NullTreaties))
				g_ServerDAL.RemoveWar(in_iWarID);
		}
	}
	else if (in_iCountryID == l_pWar->MasterDefending())
	{
		if(l_pWar->MasterDefendingWantsPeace() != in_bWantsPeace)
		{
			l_pWar->MasterDefendingWantsPeace(in_bWantsPeace);
			g_ServerDAL.ModifyWar(in_iWarID);
			if(VerifyPeaceStatus(l_pWar,l_NullTreaties))
				g_ServerDAL.RemoveWar(in_iWarID);
		}
	}	
}

bool GDataControlLayer::VerifyPeaceStatus(const GWar* in_pWar, const set<UINT32>& in_WarsToRemove)
{
	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();	

	if( in_pWar->MasterAttackingWantsPeace() && in_pWar->MasterDefendingWantsPeace() )
	{
		//The two countries want to have peace, or are forced to do so
		const set<UINT32>& l_Attackers = in_pWar->AttackingSide();
		const set<UINT32>& l_Defenders = in_pWar->DefendingSide();
		for(set<UINT32>::const_iterator itA = l_Attackers.begin();
			 itA != l_Attackers.end(); itA++)
		{
			for(set<UINT32>::const_iterator itD = l_Defenders.begin();
				 itD != l_Defenders.end(); itD++)
			{
				UINT32 in_iCountryA = *itA;
				UINT32 in_iCountryB = *itD;
				bool l_bSameWarFound = false;

				//Country A and Country B must not be in the same war somewhere else to have peace
				for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
					 it != l_Wars.end(); it++)
				{
					const GWar& l_CurWar = it->second;
					if(  it->first != in_pWar->ID() &&
						  in_WarsToRemove.count(it->first) == 0 &&
						((l_CurWar.AttackingSide().count(in_iCountryA) > 0 && l_CurWar.DefendingSide().count(in_iCountryB) > 0) ||
						 (l_CurWar.AttackingSide().count(in_iCountryB) > 0 && l_CurWar.DefendingSide().count(in_iCountryA) > 0)))
					{
						l_bSameWarFound = true;
						break;
					}						
				}

				if(!l_bSameWarFound)
				{
					if(( in_pWar->MasterAttacking() == in_iCountryA && 
						  in_pWar->MasterDefending() == in_iCountryB ) ||
					   (in_pWar->MasterAttacking() == in_iCountryB && 
						  in_pWar->MasterDefending() == in_iCountryA ))
					{
						//Print News of the peace
						DeclarePeace(*itA,*itD,true);
					}
					else
					{
						//Do not print news
						DeclarePeace(*itA,*itD,false);
					}

				}
			}
		}

		//Remove the war
		return true;		
	}	

	return false;
}

/*!
* War declaration
* @param in_iAttackingCountry: ID of the country that wants to declare a war
* @param in_iDefendingCountry: ID of the country that receives the declaration
* @return true if success
*/
bool GDataControlLayer::DeclareWar(ENTITY_ID in_iAttackingCountry, ENTITY_ID in_iDefendingCountry,const set<UINT32>& in_AttackingCountries, bool in_bOffensiveWar, bool in_bLossOfRelations)
{
	if (in_iAttackingCountry == in_iDefendingCountry)
		return false;	

	bool l_bSameOffensiveStatus = g_CombatPlanner.WarOffensiveStatus(in_iAttackingCountry,in_iDefendingCountry) == in_bOffensiveWar;

	//If the 2 countries are already at war, do not declare another war
	if(l_bSameOffensiveStatus && g_ServerDAL.DiplomaticStatus(in_iAttackingCountry,in_iDefendingCountry) == EDiplomaticStatus::Hostile)
		return true;
	
	//Lose relations on other countries of the world, except those on the same side
	if(in_bLossOfRelations)
	{
		LoseRelationsWarDeclaration(in_iAttackingCountry,in_iDefendingCountry, in_AttackingCountries);	

		//Cycle through alliances, if they are allied
		if(g_ServerDAL.DiplomaticStatus(in_iAttackingCountry,in_iDefendingCountry) == EDiplomaticStatus::Allied)
		{
			hash_set<UINT32> l_TreatiesToLeave;
			const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();
			for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
				it != l_Treaties.end();it++)
			{
				const GTreaty& l_CurTreaty = it->second;
				if(l_CurTreaty.Type() == ETreatyType::Alliance  ||
                   l_CurTreaty.Type() == ETreatyType::WeaponTrade)
				{
					if(l_CurTreaty.CountrySide(in_iAttackingCountry) == 1 &&
						l_CurTreaty.CountrySide(in_iDefendingCountry) == 1)
					{
						l_TreatiesToLeave.insert(l_CurTreaty.ID());
					}
				}
			}
			for(hash_set<UINT32>::iterator it = l_TreatiesToLeave.begin();
				 it != l_TreatiesToLeave.end(); it++)
			{
				g_ServerDCL.LeaveTreaty(in_iAttackingCountry,*it);
			}
		}

		//Send the news to the clients
		{
			News::GInfo l_News;
			l_News.m_iCountryA = in_iAttackingCountry;
			l_News.m_iCountryB = in_iDefendingCountry;
			l_News.m_eType = News::EType::DeclareWar;
			l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
			l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
			SendNews(l_News);
		}
	}

        //Cancel any military units currently in production between the two countries
    {
        vector<pair<pair<ENTITY_ID, ENTITY_ID>, UINT32>> l_vUnitsToCancel;

        const auto& l_vDeclarerProductionQueue = m_UnitProductor.m_vProductionQueues.at(in_iAttackingCountry - 1).m_ActiveQueue;
        for(auto l_RequestIt = l_vDeclarerProductionQueue.cbegin();
            l_RequestIt != l_vDeclarerProductionQueue.cend();
            ++l_RequestIt)
        {
            if(l_RequestIt->m_iBuilderCountryID == in_iAttackingCountry &&
               l_RequestIt->m_iDestinationCountryID == in_iDefendingCountry)
               l_vUnitsToCancel.push_back(pair<pair<ENTITY_ID, ENTITY_ID>, UINT32>(pair<ENTITY_ID, ENTITY_ID>(in_iAttackingCountry, in_iDefendingCountry), l_RequestIt->m_iID));
        }

        const auto& l_vTargetProductionQueue = m_UnitProductor.m_vProductionQueues.at(in_iDefendingCountry - 1).m_ActiveQueue;
        for(auto l_RequestIt = l_vTargetProductionQueue.cbegin();
            l_RequestIt != l_vTargetProductionQueue.cend();
            ++l_RequestIt)
        {
            if(l_RequestIt->m_iBuilderCountryID == in_iDefendingCountry &&
               l_RequestIt->m_iDestinationCountryID == in_iAttackingCountry)
                l_vUnitsToCancel.push_back(pair<pair<ENTITY_ID, ENTITY_ID>, UINT32>(pair<ENTITY_ID, ENTITY_ID>(in_iDefendingCountry, in_iAttackingCountry), l_RequestIt->m_iID));
        }

        for(auto l_UnitIt = l_vUnitsToCancel.cbegin();
            l_UnitIt != l_vUnitsToCancel.cend();
            ++l_UnitIt)
        {
            const auto l_iBuilderID = l_UnitIt->first.first;
            const auto l_iDestinationID = l_UnitIt->first.second;
            const auto l_iRequestID = l_UnitIt->second;
            GDZDebug::Log(L"Cancelling units in production: Request ID " + GString(l_iRequestID) + L": " +
                          L"Built by " + g_ServerDAL.CountryData(l_iBuilderID)->NameAndIDForLog() +
                          L" and destined for " + g_ServerDAL.CountryData(l_iDestinationID)->NameAndIDForLog(),
                          EDZLogCat::ObtainUnits | EDZLogCat::War,
                          __FUNCTION__, __LINE__);
            CancelUnitProduction(l_iBuilderID, l_iDestinationID, l_iRequestID);
        }

        SendProductionQueueToCountry(in_iAttackingCountry);
        SendProductionQueueToCountry(in_iDefendingCountry);
    }

	//Add the war status between the 2 countries.	
	g_ServerDAL.AddWarStatus(in_iAttackingCountry,in_iDefendingCountry);			

	//Relations between the 2 countries drop at -100
	g_ServerDAL.RelationBetweenCountries(in_iAttackingCountry,in_iDefendingCountry,-100.f);			

	//Log the war declaration
	LogNewAction(L"Country ID: (" + GString(in_iAttackingCountry) + L") has declared war to Country ID: (" + GString(in_iDefendingCountry) + L") !!");	

	//Calculate the total of enemies this country has
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	m_TotalEnemiesByCountries[in_iAttackingCountry] = 0.f;
	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		if(!g_ServerDAL.CountryIsValid(i))
			continue;
		if(g_ServerDAL.RelationBetweenCountries(i,in_iAttackingCountry) < SP2::c_fRelationsHate)
			m_TotalEnemiesByCountries[in_iAttackingCountry] += g_ServerDAL.TotalUnitsValue(i);
	}	

   g_ServerDAL.AddHistoricalMarker(in_iAttackingCountry, 
                                   EHistoryMarkerType::DeclareWar, 
                                   1);

   g_ServerDAL.AddHistoricalMarker(in_iDefendingCountry, 
                                   EHistoryMarkerType::DeclareWar, 
                                   1);

	//Set the war defensive or offensive status of the attacking country
	g_CombatPlanner.WarOffensiveStatus(in_iAttackingCountry,in_iDefendingCountry,in_bOffensiveWar);	
	
	//Tell the AI it needs to build a military plan. 
	//Also tell every country already at war with any of those two countries to do their military plan once again
	set<UINT32> l_vAttackingEnnemies;
	set<UINT32> l_vDefendingEnnemies;
	g_ServerDAL.IsAtWarWith(in_iAttackingCountry,l_vAttackingEnnemies);
	g_ServerDAL.IsAtWarWith(in_iDefendingCountry,l_vDefendingEnnemies);
	for(set<UINT32>::const_iterator l_Itr = l_vAttackingEnnemies.begin();
		 l_Itr != l_vAttackingEnnemies.end();
		 l_Itr++)
	{
		g_CombatPlanner.MilitaryPlanExist(false,in_iAttackingCountry);
		g_CombatPlanner.MilitaryPlanExist(false,*l_Itr);
	}
	for(set<UINT32>::const_iterator l_Itr = l_vDefendingEnnemies.begin();
		 l_Itr != l_vDefendingEnnemies.end();
		 l_Itr++)
	{
		g_CombatPlanner.MilitaryPlanExist(false,in_iDefendingCountry);	
		g_CombatPlanner.MilitaryPlanExist(false,*l_Itr);
	}
	
	//Both countries lose some stability
	//The country that declares the war loses 10%
	//The country that receives that declaration loses 5%
	if(in_bOffensiveWar)
		ChangeCountryStability(in_iAttackingCountry, 
			(SP2::c_fStabilityLossWarDeclarationOffensive * c_pGvtTypeApprovalWarModifier[g_ServerDAL.CountryData(in_iAttackingCountry)->GvtType()]),true);
	
	REAL32 l_fTotalUnitsAttacking = g_ServerDAL.TotalUnitsValue(in_iAttackingCountry);
	REAL32 l_fTotalUnitsDefending = g_ServerDAL.TotalUnitsValue(in_iDefendingCountry);
	if(l_fTotalUnitsDefending == 0.f || l_fTotalUnitsAttacking >= l_fTotalUnitsDefending)
		ChangeCountryStability(in_iDefendingCountry, SP2::c_fStabilityLossWarDeclarationDefensive,true);
	else
		ChangeCountryStability(in_iDefendingCountry, 
		(SP2::c_fStabilityLossWarDeclarationDefensive * (l_fTotalUnitsAttacking/l_fTotalUnitsDefending)),true);	

	return true;
}

/*!
* Peace declaration
* @param in_iFirstCountry, in_iSecondCountry: ID of the countries that will sign a peace treaty
* @return true if success
*/
bool GDataControlLayer::DeclarePeace(ENTITY_ID in_iFirstCountry, ENTITY_ID in_iSecondCountry, bool in_bPrintNews)
{	
	if (in_iFirstCountry == in_iSecondCountry)
		return false;
	if(g_ServerDAL.DiplomaticStatus(in_iFirstCountry,in_iSecondCountry) != EDiplomaticStatus::Hostile)
		return false;	

	if(in_bPrintNews)
	{
		News::GInfo l_News;
		l_News.m_iCountryA = in_iFirstCountry;
		l_News.m_iCountryB = in_iSecondCountry;
		l_News.m_eType = News::EType::SignedCeaseFire;
		l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
		l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
		SendNews(l_News);	
	}

   g_ServerDAL.AddHistoricalMarker(in_iFirstCountry, 
                                   EHistoryMarkerType::DeclarePeace, 
                                   1);

   g_ServerDAL.AddHistoricalMarker(in_iSecondCountry, 
                                   EHistoryMarkerType::DeclarePeace, 
                                   1);

	//Add the war status between the 2 countries.
	g_ServerDAL.RemoveWarStatus(in_iFirstCountry,in_iSecondCountry);		

	//Resets to defensive mode between those 2 countries
	g_CombatPlanner.WarOffensiveStatus(in_iFirstCountry,in_iSecondCountry,false);	
	g_CombatPlanner.WarOffensiveStatus(in_iSecondCountry,in_iFirstCountry,false);	


	//Tell the AI that a peace treaty has been signed
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GTreatyPeaceSigned);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
	l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastAIPlayers;
	SP2::Event::GTreatyPeaceSigned* l_pPeaceNotify = (SP2::Event::GTreatyPeaceSigned*)l_Event.get();
	l_pPeaceNotify->CountryA = in_iFirstCountry;
	l_pPeaceNotify->CountryB = in_iSecondCountry;
	g_Joshua.RaiseEvent(l_Event);  	

	LogNewAction(L"Country ID: (" + GString(in_iFirstCountry) + L") and Country ID: (" + GString(in_iSecondCountry) + L") have signed a peace treaty!");

   // Removed army from battles (attaker between those two countries)
   hash_map<UINT32, SDK::Combat::GArenaInfo*>::const_iterator l_It = g_Joshua.UnitManager().ArenaInfos().begin();
   while(l_It !=  g_Joshua.UnitManager().ArenaInfos().end())
   {
      GArenaInfo* l_pArenaInfo = (GArenaInfo*)l_It->second;
      
      GActorInfo* l_pActorCountry1 = l_pArenaInfo->Actor(in_iFirstCountry);
      GActorInfo* l_pActorCountry2 = l_pArenaInfo->Actor(in_iSecondCountry);

      if(l_pActorCountry1 && l_pActorCountry2)
      {
         if(l_pActorCountry1->m_Side != l_pActorCountry2->m_Side)
         {
            if(l_pActorCountry1->m_Side == ECombatSide::Attacker)
            {
               // Do a Retreat
               g_ServerDCL.RetreatFromCombat(l_pArenaInfo,in_iFirstCountry);
            }
            else
            {
               // Do a Retreat for  second country
               g_ServerDCL.RetreatFromCombat(l_pArenaInfo,in_iSecondCountry);
            }
         }
      }
      l_It++;
   }

	return true;
}



void GDataControlLayer::RetreatFromCombat(SP2::GArenaInfo* in_pArenaInfo,UINT32 in_iCountryId)
{
   gassert(in_pArenaInfo,"Invalid arena pointer");
   GArena *l_pArena = (GArena *)g_CombatManager.Arena(in_pArenaInfo->m_iId);
   gassert(l_pArena,"Invalid arena id");

   // Get capital
   UINT32 l_iCombatRegionId  = in_pArenaInfo->m_iRegionID;

   // If we are on water, just try to move 
   if(l_iCombatRegionId == 0)
   {
      g_ServerDCL.RetreatCountryFromCombat(l_pArena,in_iCountryId);
   }
   else
   {
      GVector2D<REAL32> l_newPosition = g_ServerDAL.CapitalPosition(in_iCountryId);
      g_ServerDCL.RetreatCountryFromCombat(l_pArena,in_iCountryId); //MultiMOD ,l_newPosition);
   }
}

/*!
* Disband some units
* @param in_iOwnerID: ID of the country that wants to disband that units
* @param in_pUnit: pointer to that unit
* @return true if success
*/
bool GDataControlLayer::DisbandUnits(ENTITY_ID in_iOwnerID, SDK::Combat::GUnit* in_pUnit)
{
	gassert(in_pUnit,"Invalid null pointer");

   GUnitGroup* l_pOriginalGroup = (GUnitGroup *)in_pUnit->Group();
   
	//A country can only disband its own units
	if (l_pOriginalGroup->OwnerId() != in_iOwnerID)
		return false;	

   // from now on, no matter what happends .. unit will be dirty
   g_ServerDAL.DirtyCountryUnitsServer(in_iOwnerID);

   // Cant disband if the group is in deployment or is currently attacking
   if(l_pOriginalGroup->Status() == EMilitaryStatus::InDeployement || 
      l_pOriginalGroup->Status() == EMilitaryStatus::Attacking)
   {
      // mark it has modified
      g_Joshua.UnitManager().ModifyUnit(in_pUnit);
      return false;
   }   

	//Remove from sell list, just in case
   if(IsUnitForSale(in_pUnit->Id()))
      m_UnitListForSell.erase(in_pUnit->Id());

	if(m_UnitMover.IsUnitInTraining(in_pUnit->Id()))
		m_UnitMover.CancelTraining(in_pUnit->Id());

	//Add population if unit was infantry
	if( (EUnitType::Enum)((SP2::GUnitDesign*)in_pUnit->Design())->Type()->Id() == EUnitType::Infantry )
		g_ServerDCL.AddPopulationAfterDisbandInfantry(in_iOwnerID,((SP2::GUnit*)in_pUnit)->Qty());

	//Send the news to the clients, only if the qty is meaningful
   /* //News's disable, reenable when it will be significant enough
   if(((SP2::GUnit*)in_pUnit)->Qty() > c_iMeaningfulUnitQty)
	{
	   vector<INT32> l_vNumbers;
	   vector<INT32> l_vStrings;
	   l_vNumbers.push_back((INT32)((SP2::GUnit*)in_pUnit)->Qty());
	   l_vStrings.push_back(in_pUnit->Design()->Id());

      News::GInfo l_News;
      l_News.m_iCountryA = in_iOwnerID;
      l_News.m_eType = News::EType::DisbandUnits;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }
   */
   g_ServerDAL.AddHistoricalMarker(in_iOwnerID, 
                                   EHistoryMarkerType::DecreaseMilitaryArsenal, 
                                   ( ((SP2::GUnit*)in_pUnit)->Qty()*((SP2::GUnitDesign*)in_pUnit->Design())->Cost()) );

	GString l_sDisbandMessage(L"Country ID: (" + GString(in_iOwnerID) + L") has disbanded " + GString(((SP2::GUnit*)in_pUnit)->Qty()) + L" " + in_pUnit->Design()->Name() + L".");
   // Don't kill group, just in case unit was in deployement group
	g_Joshua.UnitManager().KillUnit(in_pUnit->Id(),false);
	LogNewAction(l_sDisbandMessage);

   // Purge group only if its not the deployement group
   if(l_pOriginalGroup->Units().size() == 0 && l_pOriginalGroup->Status() != EMilitaryStatus::ReadyToDeploy)
      g_Joshua.UnitManager().RemoveUnitGroup(l_pOriginalGroup->Id());


	return true;
}

const hash_set<UINT32>& GDataControlLayer::ForSaleUnitList(void)
{
   return m_UnitListForSell;
}

void GDataControlLayer::SellUnit(UINT32 in_iUnitIdToSell)
{
	// If this unit was already for sale, remove it from the selling list
   if(IsUnitForSale(in_iUnitIdToSell) == true)
   {
		GUnit* l_pUnit = (GUnit* )g_Joshua.UnitManager().Unit(in_iUnitIdToSell);
      gassert(l_pUnit,"Unit to sell should exist");
		NotifyObserver(SDK::Combat::c_notificationUnitGroupModified, (void*)l_pUnit->Group()->OwnerId());

      // Add this unit to the global market unit list
#     ifdef LOG_DCL
	   g_Joshua.Log(L"*$*$* Removing selling unit " + GString(in_iUnitIdToSell) + L" *$*$*");
#     endif		
      m_UnitListForSell.erase(in_iUnitIdToSell);

      m_UnitMover.TryDeployGroupMerge(l_pUnit);
   }
   else
   {
      GUnit* l_pUnit = (GUnit* )g_Joshua.UnitManager().Unit(in_iUnitIdToSell);

      // unit really exists ? might be a sync problem
      if(l_pUnit)
      {
#        ifdef LOG_DCL
		   if(g_Joshua.GameTime() > 0.f)
			   g_Joshua.Log(L"*$*$* Selling unit (" + GString(((SP2::GUnit*)(l_pUnit))->Qty()) + L") " + GString(in_iUnitIdToSell) + L" from: " + GString(l_pUnit->Group()->OwnerId()) + L" *$*$*");
#        endif
   				
         // Get Deployement group
         SP2::GUnitGroupEx* l_pGroup = UnitMover().ProductionQueueUnitGroups()[l_pUnit->Group()->OwnerId()-1];
         gassert(l_pGroup,"Deployement group should exist");

         // Get Original Group
         GUnitGroup* l_pUnitToRemoveFromGroup = (GUnitGroup*)l_pUnit->Group();

         // Don't sell units that are in deployement or attacking
         if(l_pUnitToRemoveFromGroup->Status() == EMilitaryStatus::InDeployement || 
            l_pUnitToRemoveFromGroup->Status() == EMilitaryStatus::Attacking)
            return;

         NotifyObserver(SDK::Combat::c_notificationUnitGroupModified, (void*)l_pUnit->Group()->OwnerId());

         // Move this unit to the 'magic' group
         g_Joshua.UnitManager().AddUnit2Group(l_pUnit,l_pGroup);

         if(l_pUnitToRemoveFromGroup->Units().size() == 0)
            g_Joshua.UnitManager().RemoveUnitGroup(l_pUnitToRemoveFromGroup->Id());

         // Add this unit to the global market unit list
         m_UnitListForSell.insert(in_iUnitIdToSell);
      }
   }
}

bool GDataControlLayer::IsUnitForSale(UINT32 in_iUnitIdToTrain)
{
   list<UINT32>::const_iterator l_UnitIterator = find(m_UnitListForSell.begin(),m_UnitListForSell.end(),in_iUnitIdToTrain);

   return l_UnitIterator != m_UnitListForSell.end();
}

bool GDataControlLayer::BuyUnit(UINT32 in_iBuyingCountryId,UINT32 in_iUnitID,UINT32 in_iQty)
{
#ifdef LOG_DCL
	g_Joshua.Log(L"*$*$* Country " + GString(in_iBuyingCountryId) + L" is buying " + GString(in_iQty) + L" of unit id: " + GString(in_iUnitID));
#endif

	// Get Unit
   if(IsUnitForSale(in_iUnitID) == true)
   {
      // Get Unit 
      GUnit* l_pUnit         = (GUnit*)g_Joshua.UnitManager().Unit(in_iUnitID);
      GUnitDesign* l_pDesign = (GUnitDesign*)l_pUnit->Design();
      UINT16 l_iSellingCountry = (UINT16)l_pUnit->Group()->OwnerId();

      // If its a nuclear units, can we buy it ?
      if(l_pDesign->Type()->Category() == SP2::EUnitCategory::Nuclear && !g_ServerDAL.GameOptions().NuclearAllowed())
         return false;

      if(l_pUnit->Group()->OwnerId() == in_iBuyingCountryId)
         return false;

		if(g_ServerDAL.DiplomaticStatus(in_iBuyingCountryId,l_iSellingCountry) == EDiplomaticStatus::Hostile)
			return false;

		if(g_ServerDAL.WeaponTradeEmbargo(l_iSellingCountry,in_iBuyingCountryId))
			return false;

        // Can't be more than 10% as militarily powerful as master
        const GCountryData* const l_pBuyingCountryData = g_ServerDAL.CountryData(in_iBuyingCountryId);
        const auto l_iMaster = l_pBuyingCountryData->Master().first;
        if(l_iMaster != 0)
        {
            const REAL32 l_fFutureClientMilitaryValue = l_pBuyingCountryData->MilitaryStrength() +
                                                        in_iQty * l_pDesign->Cost();
            if(l_fFutureClientMilitaryValue >
                g_ServerDAL.CountryData(l_iMaster)->MilitaryStrength() / 10.f)
                return false;
        }
		
      g_ServerDAL.DirtyCountryUnitsServer(in_iBuyingCountryId);
      g_ServerDAL.DirtyCountryUnitsServer(l_iSellingCountry);

      // Move unit to the buying Country Deployement group
      SP2::GUnitGroupEx* l_pBuyingDepGroup = UnitMover().ProductionQueueUnitGroups()[in_iBuyingCountryId-1];
      gassert(l_pBuyingDepGroup,"Deployement group should exist");      

      SP2::GUnit* l_pBuyedUnit = NULL;
      if(in_iQty >= l_pUnit->Qty())
      {
         // Make sure any damage is repaired
         l_pUnit->MaximizeHP();

         // All units are transfert
         g_Joshua.UnitManager().AddUnit2Group(l_pUnit,l_pBuyingDepGroup);
         l_pBuyedUnit = l_pUnit;

         // Remove this one, its not for sale anymores
          m_UnitListForSell.erase(in_iUnitID);
      }
      else
      {
         // Split this unit and don't remove the current one
         l_pBuyedUnit = SplitUnit(l_pUnit, in_iQty, l_pBuyingDepGroup);
      }

      // The buyed units could already be in the deployment group
      m_UnitMover.TryDeployGroupMerge(l_pBuyedUnit);

      // Execute the monetary transaction
      REAL32 l_fCost = in_iQty * l_pDesign->Cost();

      //Multiply the cost by the selling ratio
      REAL32 l_fCostWithSellingRatio = l_fCost * c_fUsedUnitsRebates;

      //Take the money from the paying country
      TakeMoney(in_iBuyingCountryId,l_fCostWithSellingRatio);

      //Give some money to the seller country ID
      GiveMoney(l_iSellingCountry,l_fCostWithSellingRatio);

      g_ServerDAL.AddHistoricalMarker(in_iBuyingCountryId, 
                                      EHistoryMarkerType::IncreaseMilitaryArsenal, 
                                      l_fCost);

      g_ServerDAL.AddHistoricalMarker(l_iSellingCountry, 
                                      EHistoryMarkerType::DecreaseMilitaryArsenal, 
                                      l_fCost);
   }
   else
   {
		return false;
   }
	return true;
}

/*!
* Build a new unit
* @param in_iBuildingCountryID: ID of the producting country
* @param in_iDestinationCountryID: ID of the country that will receive the units
* @param in_iDesignID: ID of the design to be built
* @param in_iQty: Qty of units to build
* @param in_ePriority: Priority of the unit production request
* @return Unique Identifier of the request in the production queue, -1 if error
**/
INT32 GDataControlLayer::BuildUnits(UINT32 in_iBuildingCountryID,
												UINT32 in_iDestinationCountryID,
												UINT32 in_iDesignID,
												UINT32 in_iQty,
                                    UINT8  in_iProductionSpeed,
                                    EUnitProductionPriority::Enum in_ePriority)
{
	gassert(in_iBuildingCountryID,"Building Country ID should be specified");
	gassert(in_iDestinationCountryID,"Destination Country ID should be specified");
	gassert(in_iQty,"Qty of units to build should be specified");

	SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(in_iDesignID);
   gassert(l_pDesign,"Build units: Asking for an unknown design");

   const GCountryData* const l_pDestinationCountryData = g_ServerDAL.CountryData(in_iDestinationCountryID);
   gassert(l_pDestinationCountryData != NULL,"Destination Country data is NULL");

   // Can we really start the production ?
   if(!l_pDestinationCountryData->Activated()                       ||
      !g_ServerDAL.CountryData(in_iBuildingCountryID)->Activated()  ||
      (l_pDestinationCountryData->NumberOfPoliticallyControlledRegions() == 0) ||
      ((!g_ServerDAL.GameOptions().NuclearAllowed()) && 
       (l_pDesign->Type()->Category() == SP2::EUnitCategory::Nuclear)))
   {
      return 0;
   }

   // Can't be more than 10% as militarily powerful as master
   const auto l_iMaster = l_pDestinationCountryData->Master().first;
   if(l_iMaster != 0)
   {
       const REAL32 l_fFutureClientMilitaryValue = l_pDestinationCountryData->MilitaryStrength() +
                                                   in_iQty * l_pDesign->Cost();
       if(l_fFutureClientMilitaryValue >
          g_ServerDAL.CountryData(l_iMaster)->MilitaryStrength() / 10.f)
           return 0;
   }

   // order from another country ?
   if(in_iBuildingCountryID != in_iDestinationCountryID)
   {
      // retrieve the countries wich the player can get units from
      set<UINT32> l_PossibleCountries;
	   g_ServerDCL.CountryCanOrderNewUnitsFrom(in_iDestinationCountryID, l_PossibleCountries);
      // order from a valid country ?
      if(l_PossibleCountries.find(in_iBuildingCountryID) == l_PossibleCountries.end())
         // no, then chow bye
         return 0;
   }

    if(l_pDesign->Type()->Category() == EUnitCategory::Infantry)
    {
        //Try removing population from country
        //Check available manpower
        const INT64 l_iMiliManpowerAvailable = l_pDestinationCountryData->MiliManpowerAvailable();
        GDZDebug::Log(l_pDestinationCountryData->NameAndIDForLog() +
                      L" is requesting " + GString::FormatNumber(static_cast<REAL64>(in_iQty), L",", L".", L"", L"", 3, 0) +
                      L" infantry; has " + GString::FormatNumber(static_cast<REAL64>(l_iMiliManpowerAvailable), L",", L".", L"", L"", 3, 0) +
                      L" people available",
                      EDZLogCat::ObtainUnits,
                      __FUNCTION__, __LINE__);

        in_iQty = min(static_cast<UINT32>(l_iMiliManpowerAvailable), in_iQty);
        if(in_iQty == 0)
            return 0;

        //Check if we actually succeed in removing population
        GDZDebug::Log(L"Will try to remove " +
                      GString::FormatNumber(static_cast<REAL64>(in_iQty), L",", L".", L"", L"", 3, 0) +
                      L" people from " + l_pDestinationCountryData->NameAndIDForLog() +
                      L" due to enrolling infantry",
                      EDZLogCat::ObtainUnits,
                      __FUNCTION__, __LINE__);
        const INT64 l_iRemovedPeople = g_ServerDCL.RemovePopulationFromCountry(in_iDestinationCountryID,in_iQty,true);
        in_iQty = min(static_cast<UINT32>(l_iRemovedPeople), in_iQty);
        if(in_iQty == 0)
            return 0;
    }

	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vNumbers.push_back((INT32)in_iQty);
	l_vStrings.push_back((INT32)in_iDesignID);

   

   //Check if the countries can pay, if so, take the money and go johnny go    
   REAL32 l_fCost = in_iQty*l_pDesign->Cost()*g_ServerDAL.ProductionSpeedCostModificator(in_iProductionSpeed);
   {      
      //If the Building country is not the destination country
      if(in_iBuildingCountryID != in_iDestinationCountryID)
      {
         //Multiply the cost by the selling ratio
         REAL32 l_fCostWithMoneyForBuildingCountry = l_fCost * g_ServerDAL.SellingUnitCostRatio();

         //Take the money from the paying country
         TakeMoney(in_iDestinationCountryID,l_fCostWithMoneyForBuildingCountry);

         //Give some money to the building country ID
         GiveMoney(in_iBuildingCountryID,l_fCostWithMoneyForBuildingCountry-l_fCost);

         //Send the news to the clients, only if the qty is meaningful
         if(in_iQty > c_iMeaningfulUnitQty)	      
         {
            News::GInfo l_News;
            l_News.m_iCountryA = in_iDestinationCountryID;
            l_News.m_iCountryB = in_iBuildingCountryID;
            l_News.m_eType = News::EType::OrderedUnits;
            l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
            l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
            l_News.m_vNumbers = l_vNumbers;
            l_News.m_vStrings = l_vStrings;
            SendNews(l_News);
         }
      }
      else
      {
         //Simply take the money
         TakeMoney(in_iBuildingCountryID,l_fCost);			

         //Send the news to the clients, only if the qty is meaningful
         if(in_iQty > c_iMeaningfulUnitQty)	      
         {
            News::GInfo l_News;
            l_News.m_iCountryA = in_iBuildingCountryID;
            l_News.m_eType = News::EType::StartProductionUnits;
            l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
            l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
            l_News.m_vNumbers = l_vNumbers;
            l_News.m_vStrings = l_vStrings;
            SendNews(l_News);
         }
      }

      g_ServerDAL.AddHistoricalMarker(in_iBuildingCountryID, 
                                      EHistoryMarkerType::IncreaseMilitaryArsenal, 
                                      in_iQty*l_pDesign->Cost() );
   }

 
   REAL32 l_fTechLevelFromDesign = 0;
   REAL32 l_fCountryTechLevel = 0;
   GCountryData *l_pCountryData = g_ServerDAL.CountryData(in_iBuildingCountryID);
   REAL32 *l_techValue = l_pCountryData->ResearchInfo()->m_fMaxValues[l_pDesign->Type()->Category()];


   if(l_pDesign->Type()->Category() == EUnitCategory::Infantry)
   {
      l_fCountryTechLevel = 1;
      l_fTechLevelFromDesign = 1;

   }
   else
   {

      

      if( l_pDesign->GunDamage() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->GunDamage();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::GunDamage];
      }
      if( l_pDesign->GunPrecision() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->GunPrecision();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::GunPrecision];
      }

      if( l_pDesign->GunRange() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->GunRange();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::GunRange];
      }

      if( l_pDesign->MissileDamage() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->MissileDamage();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::MissileDamage];
      }

      if( l_pDesign->MissilePayload() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->MissilePayload();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::MissilePayload];
      }

      if( l_pDesign->MissilePrecision() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->MissilePrecision();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::MissilePrecision];
      }

      if( l_pDesign->MissileRange() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->MissileRange();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::MissileRange];
      }


      if( l_pDesign->Sensors() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->Sensors();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::Sensors];
      }

      if( l_pDesign->SpeedAndManeuverability() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->SpeedAndManeuverability();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::Speed];
      }

      if( l_pDesign->Stealth() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->Stealth();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::Stealth];
      }

      if( l_pDesign->Armor() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->Armor();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::Armor];
      }

      if( l_pDesign->CounterMesures() > 0)
      {
         l_fTechLevelFromDesign += l_pDesign->CounterMesures();
         l_fCountryTechLevel    += l_techValue[EUnitDesignCharacteristics::CounterMesures];
      }
   }

   REAL32 l_fTechBonus = min(l_fCountryTechLevel / l_fTechLevelFromDesign,5);



   //Create the unit request
   GUnitProductionRequest l_Request;
   {
      l_Request.m_iBuilderCountryID       = in_iBuildingCountryID;
      l_Request.m_iDestinationCountryID   = in_iDestinationCountryID;
      l_Request.m_iDesignID               = in_iDesignID;
      l_Request.m_iQtyShipped             = 0;
      l_Request.m_fQtyShippedAccumulated  = 0;
      l_Request.m_iQtyWanted              = in_iQty;
      l_Request.m_fGameTimeInProduction   = 0;
      l_Request.m_iProductionSpeed        = in_iProductionSpeed;
      l_Request.m_ePriority               = in_ePriority;
      l_Request.m_fBonusFromTech          = l_fTechBonus;
      l_Request.m_bUnitIsBeingBuilt       = false;
   }

   //Insert the request into the production queue
   UINT32 l_requestID =  m_UnitProductor.InsertProductionRequest(l_Request);

   return l_requestID;
}

/*!
* Change the priority of a production
* @param in_iCountryID: Id of the productor
* @param in_iUnitProductionQueueID: ID of the entry in the unit production queue
* @return true if successfully changed, false otherwise
**/
bool GDataControlLayer::ChangeUnitProductionPriority(UINT32 in_iBuildingCountryID, 
                                UINT32 in_iProductionRequestID,
                                EUnitProductionPriority::Enum in_eNewPriority)
{
   GUnitProductionRequest l_Request;

   // If the priority is the same don't do anything
   m_UnitProductor.GetProductionRequest(in_iBuildingCountryID,in_iProductionRequestID,l_Request);
   if(l_Request.m_ePriority == in_eNewPriority)
      return true;

   // Get prod request and cancel it
   
   IF_RETURN(!m_UnitProductor.CancelUnitProduction(in_iBuildingCountryID,in_iProductionRequestID,l_Request), false);

   // Change priority on this request
   l_Request.m_ePriority = in_eNewPriority;

   //re-insert the request into the production queue
   m_UnitProductor.InsertProductionRequest(l_Request);

   return true;
}

void GDataControlLayer::ClearUnitProductionForCountry(UINT32 in_iCountryID)
{
	m_UnitProductor.ClearProductionForCountry(in_iCountryID);
}

/*!
* Cancel a unit production
* @param in_iCountryID: Id of the productor
* @param in_iProductionRequestID: ID of the production request
* @return true if successfully cancelled, false otherwise
**/
bool GDataControlLayer::CancelUnitProduction(UINT32 in_iBuildingCountryID, 
                                             UINT32 in_iCancellingCountryID, 
                                             UINT32 in_iProductionRequestID,
															bool in_bSabotage)
{
   //Cancel the production
   GUnitProductionRequest l_Request;
   IF_RETURN(!m_UnitProductor.CancelUnitProduction(in_iBuildingCountryID,in_iProductionRequestID,l_Request),false);

   SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_Request.m_iDesignID);

   gassert(l_pDesign != NULL,"Trying to cancel a unit with invalid design");

   const GCountryData* const l_pCancellingCountry = g_ServerDAL.CountryData(in_iCancellingCountryID);

   GDZDebug::Log(L"Cancelling units in production: Request ID " + GString(in_iProductionRequestID) + L": " +
                 L"Built by " + g_ServerDAL.CountryData(in_iBuildingCountryID)->NameAndIDForLog() + L" " +
                 L"and destined for " + l_pCancellingCountry->NameAndIDForLog() + L": " +
                 GString(l_Request.m_iQtyWanted) + L" units of " +
                 l_pDesign->Name(),
                 EDZLogCat::ObtainUnits,
                 __FUNCTION__, __LINE__);

   // Decrease population if we just ship some infantry
   SP2::GUnitDesign* l_pUnitToShipDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_Request.m_iDesignID);
   if(l_pUnitToShipDesign->Type()->Category() == EUnitCategory::Infantry)
   {
      const UINT32 l_iPopulationToAdd = l_Request.m_iQtyWanted - l_Request.m_iQtyShipped;
      g_ServerDCL.AddPopulationAfterDisbandInfantry(l_Request.m_iDestinationCountryID,(UINT32)(l_iPopulationToAdd));
      GDZDebug::Log(l_pCancellingCountry->NameAndIDForLog() + L" will gain " +
                    GString(l_iPopulationToAdd) + L" population",
                    EDZLogCat::ObtainUnits,
                    __FUNCTION__, __LINE__);
   }



	if(!in_bSabotage)
	{
		//3 Cases: 
		// 1 - Production is by a country, for the same country:
		//       Country receives back the non invested money + 10% of invested money
		// 2 - Production is by Country A for Country B and A cancels
		//       Country A refund completely country B (total value * g_ServerDAL.m_fSellingUnitCostRatio)
		//       Country receives back the non invested money + c_fProductionRefundRate of invested money
		// 3 - Production is by Country A for Country B and B cancels
		//       Country A keeps the g_ServerDAL.m_fSellingUnitCostRatio*totalValue
		//       Country B Receives back the non invested money

		//Case 1:
		if(l_Request.m_iBuilderCountryID == l_Request.m_iDestinationCountryID)
		{
			//Give back the non invested money + c_fProductionRefundRate of the invested money
         REAL32 l_fMoneyToGive = (l_Request.m_iQtyWanted - l_Request.m_iQtyShipped )*l_pDesign->Cost()*c_fProductionRefundRate * g_ServerDAL.ProductionSpeedCostModificator(l_Request.m_iProductionSpeed);
	      
			//Give the money back
			GiveMoney(in_iBuildingCountryID,l_fMoneyToGive);
		}
		else
		{

         
			//Compute the total cost
			//REAL32 l_fTotalValue = l_pDesign->Cost()*l_Request.m_iQtyWanted*g_ServerDAL.SellingUnitCostRatio();
         REAL32 l_fTotalValue = l_pDesign->Cost()*(l_Request.m_iQtyWanted -l_Request.m_iQtyShipped) * g_ServerDAL.ProductionSpeedCostModificator(l_Request.m_iProductionSpeed);
         REAL32 l_fReimbersumentForBuilder = ( l_fTotalValue *  g_ServerDAL.SellingUnitCostRatio()) - l_fTotalValue;

         if(in_iCancellingCountryID == l_Request.m_iBuilderCountryID)
            TakeMoney(l_Request.m_iBuilderCountryID,l_fReimbersumentForBuilder);

         GiveMoney(l_Request.m_iDestinationCountryID,l_fTotalValue * c_fProductionRefundRate);


		}
	}


   if(l_Request.m_iBuilderCountryID == in_iCancellingCountryID)
   {
      if(l_Request.m_iBuilderCountryID == l_Request.m_iDestinationCountryID)
      {
      }
   }
   else
   {
      
   }	

   //Send the news to the clients, only if the qty is meaningful
   if((INT32)l_Request.m_iQtyWanted > c_iMeaningfulUnitQty)	      
   {
	   vector<INT32> l_vNumbers;
	   vector<INT32> l_vStrings;
	   l_vNumbers.push_back((INT32)l_Request.m_iQtyWanted);
	   l_vStrings.push_back((INT32)l_Request.m_iDesignID);
      News::GInfo l_News;
      l_News.m_iCountryA = in_iBuildingCountryID;
      l_News.m_eType = News::EType::CancelProductionUnits;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }


   return true;
}


/*!
 * Gets the production percentage
 * @return between 0 and 1, the production progression, -1 if error
 **/
REAL32 GDataControlLayer::UnitProductionProgress(UINT32 in_iCountryID,UINT32 in_iProductionRequestID)
{
   return m_UnitProductor.ProductionProgress(in_iCountryID,in_iProductionRequestID);
}


/*!
* Return the production queue to a clientID
* @param in_iCountryID: country Id of the productor
**/
void GDataControlLayer::SendProductionQueueToCountry(UINT32  in_iCountryID)
{
   m_UnitProductor.m_vProductionQueues[in_iCountryID-1].m_bProductionStatusIsDirty = true;
}

/*!
* Deploy a Units
* @param in_iCountryID        : country Id of the unit to deploy
* @param in_iUnitIdToDeploy   : Id of the unit to be deployed
* @param in_DeployementCoordinate : coordinate to drop the unit. 
**/
void GDataControlLayer::DeployUnits(UINT32  in_iCountryID,list<UINT32>& in_UnitListToDeploy, GVector2D<REAL32> in_DeployementCoordinate)
{
   SP2::GUnitGroupEx* l_pGroup = UnitMover().ProductionQueueUnitGroups()[in_iCountryID-1];
   gassert(l_pGroup,"There should be a unit production queue group where the units are to be placed when built");
	if(!l_pGroup)
		return;

   // Validate tat units belongs to deployement group
   // Go through all IDs
   list<UINT32>::iterator l_UnitIdIterator = in_UnitListToDeploy.begin();
   while(l_UnitIdIterator != in_UnitListToDeploy.end())
   {
      // Get Unit from unit manager
      GUnit* l_pUnitToDeploy = (GUnit*)g_Joshua.UnitManager().Unit(*l_UnitIdIterator); 
      if(l_pUnitToDeploy->Group() != l_pGroup)
      {
         // One units doesnt belong to the unit deployement queue, this action is not possible
         return;
      }
      // Iterate over IDs
      l_UnitIdIterator++;
   }


   // Validate Type of unit with the first unit
   UINT32 l_iUnitIdToDeploy = *(in_UnitListToDeploy.begin());

   // Find units 
   GUnit* l_pUnitToDeploy = (GUnit*)g_Joshua.UnitManager().Unit(l_iUnitIdToDeploy); 

   // It might be a sync problem,just return.
   if(l_pUnitToDeploy == NULL)
      return;

   // Look at what kind of units we are deploying.
   GUnitDesign*  l_pUnitDesign= (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pUnitToDeploy->Design()->Id());

   // If its a nuclear category, add it to the missile list
   if(l_pUnitDesign->Type()->Category() == EUnitCategory::Nuclear)
   {
      // Can we really deploy a nuclear missile in this game ?
      if(g_ServerDAL.GameOptions().NuclearAllowed())
      {
         DeployMissileFromUnitGroup(l_pGroup,in_UnitListToDeploy,in_DeployementCoordinate);
      }
   }
   else
   {
      // Ask the unit mover to do it.
	   g_ServerDCL.UnitMover().DeployUnit((SP2::GUnitGroup*)l_pGroup,in_UnitListToDeploy,in_DeployementCoordinate);
   }
}

void GDataControlLayer::DeployMissileFromUnitGroup(SP2::GUnitGroup* in_pGroup,list<UINT32>& in_UnitListToDeploy, GVector2D<REAL32> in_DeployementCoordinate)
{
   // Validate few pointers
   gassert(in_pGroup,"Invalid group pointer");
   gassert(in_UnitListToDeploy.size() > 0,"Invalid unit pointer");

   // Add it to missile group.
   GCountryData* l_pAttackerCountryData = g_ServerDAL.CountryData(in_pGroup->OwnerId());

   list<UINT32>::iterator l_UnitIdIterator = in_UnitListToDeploy.begin();

   while(l_UnitIdIterator != in_UnitListToDeploy.end())
   {

      // Get Unit pointer
      SP2::GUnit* l_pUnitToDeploy = (GUnit*)g_Joshua.UnitManager().Unit(*l_UnitIdIterator);

      // Add qty n time to the list
      for(UINT32 i=0;i<l_pUnitToDeploy->Qty();i++)
      {
         

         // Build a new missile
         GNuclearMissile l_NewNuclearMissible;
         l_NewNuclearMissible.m_iID       = g_Joshua.UnitManager().NewUnitID();
         l_NewNuclearMissible.m_iOwnerID  = in_pGroup->OwnerId();
         l_NewNuclearMissible.m_iDesignID = l_pUnitToDeploy->Design()->Id();
         l_NewNuclearMissible.m_iQty      = 1; 
         l_NewNuclearMissible.Position(in_DeployementCoordinate);

         l_pAttackerCountryData->AddNewMissile(l_NewNuclearMissible);
      }

      // Kill this 'bad' unit.
      g_Joshua.UnitManager().KillUnit(l_pUnitToDeploy->Id(),false);

      // Next one.
      l_UnitIdIterator++;
   }
}


/*!
* Advances the unit production, when units are finished building
* they are added to the unit list
**/
void GDataControlLayer::IterateUnitProduction()
{
   return m_UnitProductor.Iterate();
}

/*!
* Removes a certain amount of money to a country
**/
bool GDataControlLayer::TakeMoney(ENTITY_ID in_iCountryID, REAL64 in_fAmountToRemove)
{
	REAL64 l_fAvailableMoney = g_ServerDAL.CountryData(in_iCountryID)->MonetarySupplyAmount();
	l_fAvailableMoney -= in_fAmountToRemove;
	g_ServerDAL.CountryData(in_iCountryID)->MonetarySupplyAmount(l_fAvailableMoney);
	
	return true;
}

/*!
* Gives a certain amount of money to a country
**/
bool GDataControlLayer::GiveMoney(ENTITY_ID in_iCountryID, REAL64 in_fAmountToGive)
{
	REAL64 l_fAvailableMoney = g_ServerDAL.CountryData(in_iCountryID)->MonetarySupplyAmount();
	l_fAvailableMoney += in_fAmountToGive;
	g_ServerDAL.CountryData(in_iCountryID)->MonetarySupplyAmount(l_fAvailableMoney);
	
	return true;
}

/*!
* Creates a new Unit Design
**/
bool GDataControlLayer::CreateUnitDesign(UINT32         in_iID,
													  UINT32         in_iDesignerID,
													  GUnitType*     in_pType,
													  const GString& in_sName,
													  INT32          in_iNameID,
													  UINT8          in_iSpeedAndManeuverability,
													  UINT8          in_iSensors,
													  UINT8          in_iGunRange,
													  UINT8          in_iGunPrecision,
													  UINT8          in_iGunDamage,
													  UINT8          in_iMissilePayload,
													  UINT8          in_iMissileRange,
													  UINT8          in_iMissilePrecision,
													  UINT8          in_iMissileDamage,
													  UINT8          in_iStealth,
													  UINT8          in_iCounterMesures,
													  UINT8          in_iArmor,
													  INT16          * in_iPiece,
													  INT16          in_iColor)
{
   // Dont create missile design if not allowed make it crashed on loading if not commented
   // TO DO : should not load the nuclear missiles in the game options not allowed
   /*
   if(!g_ServerDAL.GameOptions().NuclearAllowed() && in_pType->Category() == SP2::EUnitCategory::Nuclear)
   {
      return false;
   }
   */

	SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().CreateUnitDesign();
	l_pDesign->Name(in_sName);
	l_pDesign->NameId(in_iNameID);
	l_pDesign->DesignerID(in_iDesignerID);
	l_pDesign->Type(in_pType);
	l_pDesign->Id(in_iID);
	l_pDesign->Armor(in_iArmor);
	l_pDesign->CounterMesures(in_iCounterMesures);
	l_pDesign->MissileDamage(in_iMissileDamage);
	l_pDesign->MissilePrecision(in_iMissilePrecision);
	l_pDesign->MissileRange(in_iMissileRange);
	l_pDesign->MissilePayload(in_iMissilePayload);
	l_pDesign->GunDamage(in_iGunDamage);
	l_pDesign->GunPrecision(in_iGunPrecision);
	l_pDesign->GunRange(in_iGunRange);
	l_pDesign->Sensors(in_iSensors);
	l_pDesign->SpeedAndManeuverability(in_iSpeedAndManeuverability);
	l_pDesign->Stealth(in_iStealth);
	if (in_iPiece)
	{
      for(UINT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
      {
		   l_pDesign->m_DesignVariations.PieceIndex[i] = in_iPiece[i];
      }
	}
	l_pDesign->m_DesignVariations.ColorIndex = in_iColor;
	g_Joshua.UnitManager().RegisterUnitDesign(l_pDesign);
	return true;
}

/*!
* Iterate the research for all the countries
* @param in_fClock: Game Time in days
**/
void GDataControlLayer::IterateResearch(REAL64 in_fClock)
{
	//Check the bonus to budget, based on treaties, for every countries
	vector<REAL64> l_vBonusToResearch;
	l_vBonusToResearch.resize(g_ServerDAL.NbCountry()+1,0.f);
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;		
		if(l_Treaty.Active() && l_Treaty.Type() == ETreatyType::ResearchPartnership)
		{
			REAL64 l_fTotalBudgetResearch = 0.f;
			const set<UINT32>& l_SideA = l_Treaty.MembersSideA(true);
			//Add the total of research for that treaty
			for(set<UINT32>::const_iterator itCountry = l_SideA.begin();
				 itCountry != l_SideA.end(); itCountry++)
			{
				l_fTotalBudgetResearch += g_ServerDAL.CountryData(*itCountry)->BudgetExpenseResearch();
			}
			//10% of that will go to the country inside that treaty (10% minus their own research funding)
			for(set<UINT32>::const_iterator itCountry = l_SideA.begin();
				 itCountry != l_SideA.end(); itCountry++)
			{
				l_vBonusToResearch[*itCountry] += 
					SP2::c_fResearchBonusInResearchPartnerships * (l_fTotalBudgetResearch - g_ServerDAL.CountryData(*itCountry)->BudgetExpenseResearch());
			}			
		}
	}

	for(INT32 i = 1 ; i <= g_ServerDAL.NbCountry() ; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);

      if(l_pCountryData->Activated() )
      {
		   //Iterate the AMDS research
		   {
			   //Compute the delay of time since last check
			   REAL64 l_fTimeDelay = in_fClock - g_ServerDAL.m_fAMDSResearchLastCheckClock;

			   //If the AMDS research was started
			   if((l_pCountryData->AMDSLevel() > 0) && (l_pCountryData->AMDSLevel() < 1))
			   {
				   //Calculate the progression and set it
               REAL32 l_fProgression = (REAL32)(l_fTimeDelay/g_ServerDAL.AMDSResearchTimeDays());
				   l_pCountryData->m_fAMDSLevel = min(1,l_pCountryData->m_fAMDSLevel+l_fProgression);
			   }
		   }
		   //Update the other research

           //Keep track of nuclear research for logging purposes
         REAL32* const l_pNuclearValues = l_pCountryData->ResearchInfo()->m_fMaxValues[EUnitCategory::Nuclear];
         const vector<REAL32> l_vOldNuclearValues(l_pNuclearValues, l_pNuclearValues + EUnitDesignCharacteristics::ItemCount);

         if(l_pCountryData->NuclearReady() >= 0.f)
         {
             GString l_sOldNuclearValuesString;
             for(INT32 i = EUnitDesignCharacteristics::MissileRange; i <= EUnitDesignCharacteristics::MissileDamage; i++)
             {
                 l_sOldNuclearValuesString += GString(l_vOldNuclearValues[i]) +
                                              (i < EUnitDesignCharacteristics::MissileDamage ? L", " : L"");
             }

             /*GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": "
                           L"NuclearReady " + GString(l_pCountryData->NuclearReady()) + L"; " +
                           L"nuclear values " + l_sOldNuclearValuesString,
                           EDZLogCat::Nuclear,
                           __FUNCTION__, __LINE__);*/
         }

         ((SP2::GResearchInformation*)l_pCountryData->ResearchInfo())->Update(in_fClock,l_pCountryData->BudgetExpenseResearch(), l_vBonusToResearch[i]);

         if(l_pCountryData->NuclearReady() >= 0.f)
         {
             for(INT32 i = EUnitDesignCharacteristics::MissileRange; i <= EUnitDesignCharacteristics::MissileDamage; i++)
             {
                 if(g_SP2Server->DisableNuclearOnOccupy()                       &&
                    l_pCountryData->NumberOfPoliticallyControlledRegions() == 0)
                 {
                     GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" is fully occupied; " +
                                   L"halting nuclear " + g_ServerDAL.GetString(c_iUnitDesignCharacteristicsStringID[i]) + L" research",
                                   EDZLogCat::Nuclear,
                                   __FUNCTION__, __LINE__);
                     l_pNuclearValues[i] = l_vOldNuclearValues[i];
                 }
             }
         }
      }
	}

	g_ServerDAL.m_fAMDSResearchLastCheckClock = in_fClock;
}

/*!
* Starts the AMDS research
* @param in_iCountryID: ID of the country to start the research on
* @return true if started successfully or already started, false otherwise
**/
bool GDataControlLayer::StartAMDSResearch(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	gassert(l_pCountryData, "There should be a country data for that country");

   // do not research if not allowed
   if(!g_ServerDAL.GameOptions().NuclearAllowed())
      return false;

   // don't research if occupied
   if(g_SP2Server->DisbandAMDSOnOccupy() && l_pCountryData->NumberOfPoliticallyControlledRegions() == 0)
   {
       gassert(l_pCountryData->AMDSLevel() == 0,"Occupied country has AMDS research");
       GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" can't research AMDS due to being fully occupied",
                     EDZLogCat::Nuclear,
                     __FUNCTION__, __LINE__);
       return false;
   }

	//If AMDS research is already started
	if(l_pCountryData->AMDSLevel())
    {
        GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" can't research AMDS " +
                      L"due to already having an AMDS level of " + GString::FormatNumber(l_pCountryData->AMDSLevel(), 3),
                      EDZLogCat::Nuclear,
                      __FUNCTION__, __LINE__);
		return true;
    }

	TakeMoney(in_iCountryID, g_ServerDAL.AMDSResearchCost());
	l_pCountryData->m_fAMDSLevel = 0.000000001f;

    GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" has started AMDS research",
                  EDZLogCat::Nuclear,
                  __FUNCTION__, __LINE__);

	//Lose Relations
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		if( i != in_iCountryID )
		{
			g_ServerDAL.RelationBetweenCountries(in_iCountryID, i, 
				g_ServerDAL.RelationBetweenCountries(in_iCountryID, i) - SP2::c_fRelationsLossInAMDSstart);
		}
	}

	//Send the news to the clients
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountryID;
      l_News.m_eType = News::EType::AMDSResearch;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      SendNews(l_News);
   }

	return true;
}


/*!
* Launches a nuclear attack on the given country ID, the given coordinates
*
* @param in_iAttackerCountryID: ID of the attacker country
* @param in_TargetCoordinates: Coordinates where the nuke should be launched
* @param in_iTargetCountryID:  ID of the target country
* @param in_AttackType:        Type of the attack
* @param in_iMissileID:        ID Of the Missile
**/
UINT32 GDataControlLayer::LaunchNuclearAttacks(const vector<GNukeLaunchRequest>& in_vNukeRequest)
{
   UINT32 l_iNbSuccess = 0;

   if(in_vNukeRequest.size() > 0 && g_ServerDAL.GameOptions().NuclearAllowed())
   {
      UINT32 l_iNbSuccess = 0;

      //Check if the country is allowed to nuke based on its occupied regions
      {
            const set<UINT32>& l_PoliticallyControlledRegions = g_ServerDAL.CountryPoliticalControl(in_vNukeRequest[0].m_iAttackerID);
	        const vector<GRegionControl>& l_vAllRegions = g_ServerDAL.RegionControlArray();

	        REAL32 l_fTotalRegions    = 0.f;
	        REAL32 l_fOccupiedRegions = 0.f;

	        for(set<UINT32>::const_iterator it = l_PoliticallyControlledRegions.begin();
		        it != l_PoliticallyControlledRegions.end(); it++)
	        {
		        if(l_vAllRegions[*it].m_iMilitary != l_vAllRegions[*it].m_iPolitical)
			        l_fOccupiedRegions += 1.f;
		        l_fTotalRegions += 1.f;
	        }

	        if(l_fTotalRegions == 0.f)
		        return 0;
	
	        REAL32 l_fOccupiedRegionsPercentage = l_fOccupiedRegions / l_fTotalRegions;

            //Not enough regions occupied; disallow nuking
            if(l_fOccupiedRegionsPercentage < g_SP2Server->OccupiedRegionPercentageForNuclear())
		        return 0;
      }

      //Prepare the game event that will notify the nukes to the Active Human Players
		SDK::GGameEventSPtr                  l_Event       = CREATE_GAME_EVENT(SP2::Event::GStrategicWarfareNotify);
      SP2::Event::GStrategicWarfareNotify* l_pNukeNotify = (SP2::Event::GStrategicWarfareNotify*)l_Event.get();
      {
		   l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActivePlayers;
      }
		
		if(!g_ServerDAL.LastCountryThatNuked())
		{
			UINT32 l_iCountryThatJustNuked = in_vNukeRequest[0].m_iAttackerID;
			g_ServerDAL.LastCountryThatNuked(l_iCountryThatJustNuked);
			g_ServerDAL.DateOfLastNuke(g_ServerDAL.ActualDate());

			//This Country loses relations with the rest of the world			
			UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
			for(UINT32 i=1; i<= l_iNbCountries; i++)
			{
				g_ServerDAL.RelationBetweenCountries(l_iCountryThatJustNuked,i,
					g_ServerDAL.RelationBetweenCountries(l_iCountryThatJustNuked,i) - SP2::c_fLossOfRelationsWhenFirstUsingNukes);
			}
		}

      GLaunchedNuke l_LaunchedNuke; 
      for(UINT32 i = 0;i < in_vNukeRequest.size();i ++)
      {
			if(!g_ServerDAL.CountryData(in_vNukeRequest[i].m_iAttackerID)->NuclearReady())
				continue;
         //The launched nuke will be updated during the method execution
         if(LaunchNuclearAttack(in_vNukeRequest[i],l_LaunchedNuke))
         {
            l_pNukeNotify->m_vLaunchedNukes.push_back(l_LaunchedNuke);
            l_iNbSuccess ++;
         }
      }		

      //Add an historical marker
      g_ServerDAL.AddHistoricalMarker(in_vNukeRequest[0].m_iAttackerID, 
                                    EHistoryMarkerType::LaunchNuke, 
                                    (REAL32) l_iNbSuccess);

      //If there were nukes successfully launched, notify the nukes were launched
      if(l_pNukeNotify->m_vLaunchedNukes.size())
      {
         NotifyObserver(c_iNotificationOnNukeLaunched,&l_pNukeNotify->m_vLaunchedNukes[0]);
   		g_Joshua.RaiseEvent(l_Event); 
      }
   }
   return l_iNbSuccess;
}

/*!
 * Launchs a sigle nuclear attack, returns the launched nuke
 **/
bool GDataControlLayer::LaunchNuclearAttack(const GNukeLaunchRequest& in_NukeRequest,GLaunchedNuke& out_LaunchedNuke)
{
   if(!g_SP2Server->GameStarted() || !g_ServerDAL.GameOptions().NuclearAllowed())
      return false;

   gassert(g_Joshua.ActivePlayerByModID(in_NukeRequest.m_iTargetID),"LaunchNuclearAttack, on an unexiting active player: ");

	UINT32		  l_iMegatonsSent = 0;
	GCountryData* l_pAttackerCountryData = g_ServerDAL.CountryData(in_NukeRequest.m_iAttackerID);
	GCountryData* l_pTargetCountryData   = g_ServerDAL.CountryData(in_NukeRequest.m_iTargetID);
	gassert(l_pAttackerCountryData && l_pTargetCountryData,"Should be country data for the given country");	

   vector<GNuclearMissile>::iterator l_SentMissileIterator = l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.end();

	//Check if the attacker really have a missile with this ID
	{     
      for(vector<GNuclearMissile>::iterator l_MissileIt = l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.begin();
          l_MissileIt != l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.end();
          l_MissileIt++)
		{
         if((*l_MissileIt).m_iID == in_NukeRequest.m_iMissileID)
			{
				l_SentMissileIterator = l_MissileIt;
				l_iMegatonsSent = (*l_MissileIt).Megatons();
				break;
			}
		}

      if(l_SentMissileIterator == l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.end())
      {
         //Check in the sub based nuke list
         for(vector<GNuclearMissile>::iterator l_MissileIt = l_pAttackerCountryData->m_vSubmarineBasedNuclearMissiles.begin();
            l_MissileIt != l_pAttackerCountryData->m_vSubmarineBasedNuclearMissiles.end();
            l_MissileIt++)
		   {
            if((*l_MissileIt).m_iID == in_NukeRequest.m_iMissileID)
			   {
				   l_SentMissileIterator = l_MissileIt;
				   l_iMegatonsSent = (*l_MissileIt).Megatons();
				   break;
			   }
		   }
      }

		IF_RETURN_LOG(l_SentMissileIterator == l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.end(),false,L"Tried to send unexisting nuke id "+ GString(in_NukeRequest.m_iMissileID) +  L" of country " + GString(in_NukeRequest.m_iAttackerID));
	}

   //If the nuke was launched from a Submarine : Start the submarine nuclear missile reload clock
   //(if already started will not be resetted)
   if((*l_SentMissileIterator).FromSubmarine())
      g_ServerDAL.CountryData(in_NukeRequest.m_iAttackerID)->StartNuclearSubmarineReloadClock();

	//Nuking a country represents a war declaration
	set<UINT32> l_Attackers;
	l_Attackers.insert(in_NukeRequest.m_iAttackerID);
	DeclareNewWar(l_Attackers,in_NukeRequest.m_iAttackerID,in_NukeRequest.m_iTargetID);

	//The target drops in stability
	ChangeCountryApproval(in_NukeRequest.m_iAttackerID,-0.5f);
	ChangeCountryStability(in_NukeRequest.m_iTargetID,-1.f,false);

	//If the target is the source, drops approval too
	if(in_NukeRequest.m_iAttackerID == in_NukeRequest.m_iTargetID)
	{
		ChangeCountryApproval(in_NukeRequest.m_iTargetID,-1.f);

		//This Country loses relations with the rest of the world, because it's so wrong to nuke its own people			
		UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
		for(UINT32 i=1; i<= l_iNbCountries; i++)
		{
			g_ServerDAL.RelationBetweenCountries(in_NukeRequest.m_iTargetID,i,
				g_ServerDAL.RelationBetweenCountries(in_NukeRequest.m_iTargetID,i) - 50.f);
		}
	}

	//Compute the data about the missile
	REAL64 l_fGameTimeAtDestination = 0.f;
	UINT8    l_iAMDSIntercepted = 0;
	UINT8    l_iAMDSLauched = 0;
	REAL64   l_fInterceptionClock = 0.f;
	{
		//1st compute the time to reach the destination
		{
         REAL32 l_fDistanceBetweenLaunchAndDestination = GDALInterface::DistanceBetween2PointsLatLong((*l_SentMissileIterator).Position(),in_NukeRequest.m_TargetCoordinates);
			//Rule of three, half of the Earth Circonference = g_ServerDAL::m_fNuclearMissileSpeed seconds to traverse
			REAL32 l_fTimeToReachDestionation = l_fDistanceBetweenLaunchAndDestination*g_ServerDAL.m_fNuclearMissileSpeed*2/EARTH_CIRCONFERENCE_AT_EQUATOR;
			l_fGameTimeAtDestination = g_Joshua.GameTime()  + l_fTimeToReachDestionation;
		}
		//2nd check if the target country AMDS System successfully intercepted the nuke
		if(l_pTargetCountryData->AMDSLevel() >= 1)
		{
			l_iAMDSLauched |= 1;
			REAL32 l_fLongRangeValue = g_CombatManager.RandomReal();
			REAL32 l_fShortRangeValue = g_CombatManager.RandomReal();

			//Long range interceptions
			if(l_fLongRangeValue <= g_ServerDAL.m_fAMDSLongRangeInterceptionSuccess)
			{
				//Long range interception is done at 3/4 of the run
				l_fInterceptionClock = g_Joshua.GameTime() + (l_fGameTimeAtDestination-g_Joshua.GameTime())*0.75;
				l_iAMDSIntercepted = 1;
			}
			//Short range interceptions
			else 
			{
				l_iAMDSLauched |= 2;
				if(l_fShortRangeValue <= g_ServerDAL.m_fAMDSShortRangeInterceptionSuccess)
				{
					//Short range interception is done at 7/8 of the run
					l_fInterceptionClock = g_Joshua.GameTime() + (l_fGameTimeAtDestination-g_Joshua.GameTime())*0.875;
					l_iAMDSIntercepted = 2;
				}
			}
		}
	}


   //Delay the nuke launch time by x secs
   REAL32 l_fNukeLaunchDelay = (REAL32) g_Joshua.GameSpeed() * c_fNukeLaunchDelaySeconds; //where the number is x
   {   
      l_fGameTimeAtDestination += l_fNukeLaunchDelay;
      l_fInterceptionClock     += l_fNukeLaunchDelay;
   }


	//Create the GLaunchedNuke
   {
	   out_LaunchedNuke.m_fHitClock          = l_fGameTimeAtDestination;
	   out_LaunchedNuke.m_fInterceptionClock = l_fInterceptionClock;
	   out_LaunchedNuke.m_fLaunchClock       = g_Joshua.GameTime() + l_fNukeLaunchDelay;
	   out_LaunchedNuke.m_TargetPosition     = in_NukeRequest.m_TargetCoordinates;
	   out_LaunchedNuke.m_iAMDSIntercepted   = l_iAMDSIntercepted;
	   out_LaunchedNuke.m_iAMDSLaunched      = l_iAMDSLauched;
	   out_LaunchedNuke.m_Missile            = *l_SentMissileIterator;
   }

	//Remove the missile from the country missile list
   //Do not set the missile list as dirty, instead, we will use the GStrategicWarfareNotify
   //event to remove the missiles from the list on the client side
	{
      if(!l_SentMissileIterator->FromSubmarine())
      {
		   l_pAttackerCountryData->m_vGroundBasedNuclearMissiles.erase(l_SentMissileIterator);
      }
      else
      {
         l_pAttackerCountryData->m_vSubmarineBasedNuclearMissiles.erase(l_SentMissileIterator);
      }
	}

	//If the missile was stopped by the AMDS, we dont need to keep it in the list, just mention it to
	//the clients
	if(!l_iAMDSIntercepted)
	{
		g_ServerDAL.m_LaunchedNukes.insert(make_pair<REAL64,GLaunchedNuke>(l_fGameTimeAtDestination,out_LaunchedNuke));
	}


	//Add launch nuked to the list of pending nukes news
	AddNewsNukeLaunched(in_NukeRequest.m_iAttackerID);
	return true;
}

/*!
* Iterates the nukes and if a nuke has hitted its target, calculate the impact
* of that explosion
**/
void GDataControlLayer::IterateNukes()
{
   if(!g_ServerDAL.GameOptions().NuclearAllowed()) return;

	REAL64 l_fActualGameTime =  g_Joshua.GameTime();

   //Missiles, grouped by target, key = target, value = pair<damagerange,missile>
   hash_map<GNukeTarget,multimap<REAL32,GNuclearMissile> > l_MissilesGroupedByTarget;
	
   multimap<REAL64,GLaunchedNuke>::iterator l_It = g_ServerDAL.m_LaunchedNukes.begin();

   hash_map<UINT32, UINT32> l_NukeCountPerTarget;

   const vector<GRegionControl>& l_vRegionControlArray = g_ServerDAL.RegionControlArray();

	//Group nukes that have hit the ground
	while((l_It->first <= l_fActualGameTime) &&(l_It != g_ServerDAL.m_LaunchedNukes.end()))
	{
      //Insert this entry into the grouped missiles by target
      {
         GNukeTarget l_Target;
         l_Target.m_Position = l_It->second.m_TargetPosition;

         hash_map<GNukeTarget,multimap<REAL32,GNuclearMissile> >::iterator l_MGroupIt = 
            l_MissilesGroupedByTarget.find(l_Target);

         const GRegionControl* l_pRegionCtrl = NULL;

         if(l_MGroupIt == l_MissilesGroupedByTarget.end() )
         {
            l_Target.m_iTargetRegionID = EarthLocateRegion(l_Target.m_Position.x, l_Target.m_Position.y);
            l_pRegionCtrl = &(l_vRegionControlArray[l_Target.m_iTargetRegionID] );

            multimap<REAL32,GNuclearMissile> l_Missiles;
            l_Missiles.insert(make_pair(l_It->second.m_Missile.FullDamageRange(),l_It->second.m_Missile));
            l_MissilesGroupedByTarget[l_Target] = l_Missiles;

            l_NukeCountPerTarget[l_pRegionCtrl->m_iMilitary] = 0;
            l_NukeCountPerTarget[l_pRegionCtrl->m_iPolitical] = 0;
         }
         else
         {
            l_MGroupIt->second.insert(make_pair(l_It->second.m_Missile.FullDamageRange(),l_It->second.m_Missile));
            l_pRegionCtrl = &(l_vRegionControlArray[l_MGroupIt->first.m_iTargetRegionID] );
         }

         if(l_pRegionCtrl->m_iMilitary != 0)
         {
            l_NukeCountPerTarget[l_pRegionCtrl->m_iMilitary] += l_It->second.m_Missile.m_iQty;
            if(l_pRegionCtrl->m_iMilitary != l_pRegionCtrl->m_iPolitical)
            {
               l_NukeCountPerTarget[l_pRegionCtrl->m_iPolitical] += l_It->second.m_Missile.m_iQty;
            }
         }
      }
      l_It++;
   }//end grouping nukes

   //For every entry (in the grouped nuke list)
   for(hash_map<GNukeTarget,multimap<REAL32,GNuclearMissile> >::const_iterator l_OptimizedListIt = l_MissilesGroupedByTarget.begin();
       l_OptimizedListIt != l_MissilesGroupedByTarget.end() ;
       l_OptimizedListIt++)
   {
#     ifdef LOG_DCL
      UINT32 l_iPopulationCasualties   = 
#     endif      
         NuclearCasualtiesCivilExecute(l_OptimizedListIt->first, l_OptimizedListIt->second);

#     ifdef LOG_DCL
      UINT32 l_iMilitaryDamage         = 
#     endif      
         NuclearCasualtiesMilitaryExecute(l_OptimizedListIt->first, l_OptimizedListIt->second);

#     ifdef LOG_DCL
      if(l_OptimizedListIt->second.size() > 1)
         g_Joshua.Log("!!Nuke optimisation was useful - Iterations saved: " + GString(l_OptimizedListIt->second.size()-1));
      if(l_iPopulationCasualties)
			g_Joshua.Log(L"Nuclear missile killed " + GString(l_iPopulationCasualties) + L" civil.");
      if(l_iMilitaryDamage)
			g_Joshua.Log(L"Nuclear missile killed " + GString(l_iMilitaryDamage) + L" military units.");
#     endif      
   }

   //Remove the processed entries
	if(l_It != g_ServerDAL.m_LaunchedNukes.begin())
		g_ServerDAL.m_LaunchedNukes.erase(g_ServerDAL.m_LaunchedNukes.begin(),l_It);   

   // Add markers to players
   hash_map<UINT32, UINT32>::iterator l_TargetCountIt = l_NukeCountPerTarget.begin();

   while(l_TargetCountIt != l_NukeCountPerTarget.end() )
   {
      if(l_TargetCountIt->second > 0)
      {
         g_ServerDAL.AddHistoricalMarker(l_TargetCountIt->first, 
                                       EHistoryMarkerType::ReceiveNuke, 
                                       (REAL32) l_TargetCountIt->second);
      }
      ++ l_TargetCountIt;
   }
}

/*!
* @param in_iCountryID: The ID of the country
* @param in_fStabilityChanges:   The pourcentage of stability gained or lossed (ex: 0.02, -0.03,...)
**/

bool GDataControlLayer::ChangeCountryStability(ENTITY_ID in_iCountryID, REAL32 in_fStabilityChanges, bool in_bProportional)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fStability = l_pCountryData->GvtStability();
	
	if(in_bProportional)
		l_fStability *= (1.f + in_fStabilityChanges);
	else
		l_fStability += in_fStabilityChanges;

	if (l_fStability > 1.f)
		l_fStability = 1.f;
	else if (l_fStability < 0.f)
		l_fStability = 0.f;

	REAL32 l_fBonus = 0.f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		l_fBonus += SP2::c_fFreedomOfSpeechStabilityBonus;
	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
		l_fBonus += SP2::c_fFreedomOfDemonstrationStabilityBonus;

    const REAL32 l_fServerStabilityAnarchyLowerLimit = g_SP2Server->StabilityAnarchyLowerLimit();
    const REAL32 l_fServerStabilityAnarchyUpperLimit = g_SP2Server->StabilityAnarchyUpperLimit();

	if(l_fStability < (l_fServerStabilityAnarchyLowerLimit-l_fBonus) && 
		l_pCountryData->GvtStabilityExpected() < (l_fServerStabilityAnarchyLowerLimit-l_fBonus) &&
		l_pCountryData->GvtType() != EGovernmentType::Anarchy)
	{
		ChangeGovernmentType(in_iCountryID, (EGovernmentType::Enum)l_pCountryData->GvtType(), EGovernmentType::Anarchy);
	}
	else if(l_fStability > (l_fServerStabilityAnarchyUpperLimit-l_fBonus) && 
		l_pCountryData->GvtStabilityExpected() > (l_fServerStabilityAnarchyUpperLimit-l_fBonus) &&
		l_pCountryData->GvtType() == EGovernmentType::Anarchy)
	{
		ChangeGovernmentType(in_iCountryID, EGovernmentType::Anarchy, l_pCountryData->LeaderParty()->GvtType());
	}	

	g_ServerDAL.CountryData(in_iCountryID)->GvtStability(l_fStability);

	return true;
}

/*!
* Broadcast the given packet to all the human players
**/
void GDataControlLayer::BroadcastUDPPacket(void* in_pPacket, short in_iSize)
{
   for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin() ;
       l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
       l_HumanPlayersIt++)
   {
      SDK::GPlayer* l_pPlayer = l_HumanPlayersIt->second;
      gassert(l_pPlayer->Client(),"Invalid client pointer");
      SDK::GGenericNetworkClient* l_pNetworkClient = (SDK::GGenericNetworkClient*)l_pPlayer->Client();
      g_Joshua.SendPacketUDP(in_pPacket, in_iSize,l_pNetworkClient->InternetAddress());
   }
}
/*!
* @param in_iCountryID:	The ID of the country
* @param in_fApprovalChanges:	The pourcentage of stability gained or lossed (ex: 0.02, -0.03,...)
**/
bool GDataControlLayer::ChangeCountryApproval(ENTITY_ID in_iCountryID, REAL32 in_fApprovalChanges)
{
	REAL32 l_fApproval = g_ServerDAL.CountryData(in_iCountryID)->GvtApproval();
	l_fApproval *= (1.f + in_fApprovalChanges);
	if (l_fApproval > 1.f)
		l_fApproval = 1.f;
	else if (l_fApproval < 0.f)
		l_fApproval = 0.f;
	g_ServerDAL.CountryData(in_iCountryID)->GvtApproval(l_fApproval);

	return true;
}

/*!
 * Compute the civilian casualties of a nuke (and perform modifications)
 * @param in_Target: Nuclear target, only the position is supposed to be valid
 * @param in_Missiles: List of missiles sent to that target, key = full damage range, value = missile
 * @return number of civilian units dead.
 **/
UINT32 GDataControlLayer::NuclearCasualtiesCivilExecute(const GNukeTarget& in_Target,
                                                        const multimap<REAL32,GNuclearMissile>& in_Missiles)
{
	set<UINT32> l_vCountriesHit;
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
   UINT32 l_iTotalCivilianCasualties = 0;

   gassert(in_Missiles.size(),"Should be some missiles specified");

   //Get the highest missile damage range of the biggest missile
   REAL32 l_fHighestDamageRangeDegrees = 0 ;
   {
      REAL32 l_fHighestDamageRangeKM = 0;
      multimap<REAL32,GNuclearMissile>::const_iterator l_BiggestMissileIt = in_Missiles.end();
      l_BiggestMissileIt--;
      l_fHighestDamageRangeKM = l_BiggestMissileIt->first;
      l_fHighestDamageRangeDegrees = GDCLInterface::KMLengthInDegrees(in_Target.m_Position,l_fHighestDamageRangeKM);
   }

   //List all the cities that are within range of the largest damage, key = distance from center
   multimap<REAL32,const SCityInfo*> l_CitiesInRange;

   //Keep track of the regions that will be affected, and their initial populations for casualty calculation
   map<UINT32, INT64> l_mRegionsInRange;

   {
	   const vector<SCityInfo>& l_vCityList = GDALInterface::Instance->Cities();
	   for(UINT32 i = 0 ; i  < l_vCityList.size() ; i++)
	   {
         const SCityInfo& l_City = l_vCityList[i];
         if(l_City.m_iId != 0xFFFFFFFF)
         {
            REAL32 l_fPopDensity   = GDCLInterface::PopulationDensityBySqKM(l_City.m_iPopulation);
				REAL32 l_fCityArea = 0.f;
				if(l_fPopDensity > 0.f)
					l_fCityArea = l_City.m_iPopulation / l_fPopDensity;
				REAL32 l_fCityRadiusKM   = sqrt(l_fCityArea/(REAL32)Math::PI);				
				REAL32 l_fCityRadiusDegree = GDCLInterface::KMLengthInDegrees(l_City.m_Position,l_fCityRadiusKM);

				//Check how much area is superposed between the city and the nuke explosion full range
            REAL32 l_fTouchesIt = GDCLInterface::AreaOf2IntersectingCircles(l_City.m_Position,l_fCityRadiusDegree,in_Target.m_Position,l_fHighestDamageRangeDegrees);
            if(l_fTouchesIt)
            {
                if(l_City.m_iPopulation > 0)
                    l_CitiesInRange.insert(make_pair(l_City.m_Position.Distance(in_Target.m_Position),&l_City));

                const UINT32 l_iRegionId = l_City.m_iRegionId;
                const GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_iRegionId);
                if(l_pRegion != NULL && l_pRegion->Population() > 0 && l_mRegionsInRange.find(l_iRegionId) == l_mRegionsInRange.cend())
                {
                    l_mRegionsInRange[l_iRegionId] = l_pRegion->Population();
                    GDZDebug::Log(L"Adding affected region " +
                                  g_ServerDAL.GetString(l_pRegion->NameId()) + L" of " +
                                  L"country " + g_SP2Server->Countries().at(l_pRegion->OwnerId() - 1).Name() + L" with " +
                                  L"population " + GString::FormatNumber(static_cast<const REAL64>(l_pRegion->Population()), L",", L".", L"", L"", 3, 0),
                                  EDZLogCat::Nuclear | EDZLogCat::War,
                                  __FUNCTION__, __LINE__);
                }
            }
         }//end if city has a valid id
      }//end of for each city
   }//end of list all the cities that are in range of the biggest missile
   
   //
   for(multimap<REAL32,GNuclearMissile>::const_iterator l_MissileIt = in_Missiles.cbegin();
       l_MissileIt != in_Missiles.cend();
       ++l_MissileIt)
   {
        const UINT32 l_iRegionId = GDCLInterface::EarthLocateRegion(in_Target.m_Position.x, in_Target.m_Position.y);
        const GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_iRegionId);
        if(l_pRegion != NULL && l_pRegion->Population() > 0 && l_mRegionsInRange.find(l_iRegionId) == l_mRegionsInRange.cend())
        {
            l_mRegionsInRange[l_iRegionId] = l_pRegion->Population();
            GDZDebug::Log(L"Adding affected region (not from city) " +
                          g_ServerDAL.GetString(l_pRegion->NameId()) + L" of " +
                          L"country " + g_SP2Server->Countries().at(l_pRegion->OwnerId() - 1).Name() + L" with " +
                          L"population " + GString::FormatNumber(static_cast<const REAL64>(l_pRegion->Population()), L",", L".", L"", L"", 3, 0),
                          EDZLogCat::Nuclear | EDZLogCat::War,
                          __FUNCTION__, __LINE__);
        }
   }

   //For each city of the city in range, check if the missile touches it
   for(multimap<REAL32,const SCityInfo*>::iterator l_CitiesInRangeIt = l_CitiesInRange.begin();
       l_CitiesInRangeIt != l_CitiesInRange.end();
       l_CitiesInRangeIt++)
   {
      UINT32 l_iCityCasualties  = 0;
      //For each missile, check if it touched the city
      for(multimap<REAL32,GNuclearMissile>::const_iterator l_MissileIt = in_Missiles.begin();
          l_MissileIt != in_Missiles.end();
          l_MissileIt++)
      {
	      REAL32 l_fFullAreaKmHit = 0.f;

      	//Given the strength of the nuke
	      REAL32 l_fFullDamageRange = l_MissileIt->second.FullDamageRange() * g_SP2Server->NuclearMissileRangePercentage();

	      REAL32 l_fFullDamageRangeDegrees = GDCLInterface::KMLengthInDegrees(in_Target.m_Position,l_fFullDamageRange);

			REAL32 l_fPopDensity   = GDCLInterface::PopulationDensityBySqKM(l_CitiesInRangeIt->second->m_iPopulation);
			REAL32 l_fCityArea = 0.f;
			if(l_fPopDensity > 0.f)
				l_fCityArea = l_CitiesInRangeIt->second->m_iPopulation / l_fPopDensity;
			REAL32 l_fCityRadiusKM   = sqrt(l_fCityArea/(REAL32)Math::PI);				
			REAL32 l_fCityRadiusDegree = GDCLInterface::KMLengthInDegrees(l_CitiesInRangeIt->second->m_Position,l_fCityRadiusKM);


			REAL32 l_fFullBlastIntersectedArea = GDCLInterface::AreaOf2IntersectingCircles(l_CitiesInRangeIt->second->m_Position,l_fCityRadiusDegree,in_Target.m_Position,l_fFullDamageRangeDegrees);
			if(l_fFullBlastIntersectedArea >= 0)
			{
				REAL32 l_fMaxFullBlastAreaDegrees = min((REAL32)(Math::PI * powf(l_fCityRadiusDegree,2.f)),(REAL32)(Math::PI * powf(l_fFullDamageRangeDegrees,2.f)));
				REAL32 l_fAreaPourcentage = l_fFullBlastIntersectedArea / l_fMaxFullBlastAreaDegrees;
				REAL32 l_fMaxFullBlastAreaKm = min((REAL32)(Math::PI * powf(l_fCityRadiusKM,2.f)),(REAL32)(Math::PI * powf(l_fFullDamageRange,2.f)));
				l_fFullAreaKmHit = l_fAreaPourcentage * l_fMaxFullBlastAreaKm;

				//City was hit by the full nuke blast, compute the casualties
				l_iCityCasualties = (UINT32)(l_fFullAreaKmHit*l_fPopDensity);
			}

			//Cant be more casualties than the city's population
			l_iCityCasualties = min(l_iCityCasualties,(UINT32)l_CitiesInRangeIt->second->m_iPopulation);
			l_iCityCasualties = g_ServerDAL.RemovePopulationFromCity(l_iCityCasualties,l_CitiesInRangeIt->second->m_iId,l_CitiesInRangeIt->second->m_iRegionId);
         AddCasualtiesToCountry(l_vControls[l_CitiesInRangeIt->second->m_iRegionId].m_iPolitical,l_iCityCasualties);
			l_vCountriesHit.insert(l_vControls[l_CitiesInRangeIt->second->m_iRegionId].m_iPolitical);
         l_iTotalCivilianCasualties += l_iCityCasualties;
      }
   }

   //Now do a rough calculation for region casualties
   for(map<UINT32, INT64>::const_iterator l_RegionIt = l_mRegionsInRange.cbegin();
       l_RegionIt != l_mRegionsInRange.cend();
       ++l_RegionIt)
   {
        GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_RegionIt->first);
        const REAL32 l_fRegionAreaTotal = l_pRegion->AreaTotal();

        GDZDebug::Log(L"Region " + g_ServerDAL.GetString(l_pRegion->NameId()) + L": " +
                      L"Current population is " + GString::FormatNumber(static_cast<REAL64>(l_pRegion->Population()), L",", L".", L"", L"", 3, 0),
                      EDZLogCat::Nuclear | EDZLogCat::War,
                      __FUNCTION__, __LINE__);

        INT64 l_iRegionCasualtiesTotal = 0;

        for(multimap<REAL32,GNuclearMissile>::const_iterator l_MissileIt = in_Missiles.cbegin();
           l_MissileIt != in_Missiles.cend();
           ++l_MissileIt)
        {
            if(l_pRegion->Population() > 0)
            {
                const REAL64 l_fMissileAreaKm = Math::PI * pow(l_MissileIt->second.FullDamageRange() * g_SP2Server->NuclearMissileRangePercentage(), 2);
                REAL64 l_fRegionLandAreaPercentage = (l_fMissileAreaKm / l_fRegionAreaTotal) *
                    (l_pRegion->AreaLand() / l_fRegionAreaTotal);
                l_fRegionLandAreaPercentage = min(max(0, l_fRegionLandAreaPercentage), 1);
                GDZDebug::Log(L"Region " + g_ServerDAL.GetString(l_pRegion->NameId()) + L": " +
                              L"Missile area: " + GString::FormatNumber(l_fMissileAreaKm, L",", L".", L"", L"", 3, 2) + L"; " +
                              L"total region area: " + GString::FormatNumber(l_fRegionAreaTotal, L",", L".", L"", L"", 3, 2) + L"; " +
                              L"percentage: " + GString::FormatNumber(l_fRegionLandAreaPercentage * 100, 2),
                              EDZLogCat::Nuclear | EDZLogCat::War,
                              __FUNCTION__, __LINE__);

                INT64 l_iRegionCasualtiesFromMissile = static_cast<INT64>(l_RegionIt->second * l_fRegionLandAreaPercentage);
                l_iRegionCasualtiesFromMissile = min(max(0, l_iRegionCasualtiesFromMissile), l_pRegion->Population());

                RemovePopulationFromRegion(l_RegionIt->first, l_iRegionCasualtiesFromMissile, false);
                AddCasualtiesToCountry(l_vControls[l_RegionIt->first].m_iPolitical, static_cast<UINT32>(l_iRegionCasualtiesFromMissile));
                l_iTotalCivilianCasualties += static_cast<UINT32>(l_iRegionCasualtiesFromMissile);
                l_iRegionCasualtiesTotal += l_iRegionCasualtiesFromMissile;
            }

            //Drop infrastructure and telecom in the region
            l_pRegion->Infrastructure( l_pRegion->Infrastructure() * 0.9f );
            l_pRegion->TelecomLevel( l_pRegion->TelecomLevel() * 0.9f );
        }

        GDZDebug::Log(L"Removed " +
                      GString::FormatNumber(static_cast<REAL64>(l_iRegionCasualtiesTotal), L",", L".", L"", L"", 3, 0) + L" population " +
                      L"from region " + g_ServerDAL.GetString(l_pRegion->NameId()),
                      EDZLogCat::Nuclear | EDZLogCat::War,
                      __FUNCTION__, __LINE__);
   }

	for(set<UINT32>::const_iterator l_Itr = l_vCountriesHit.begin();
		 l_Itr != l_vCountriesHit.end(); l_Itr++)
	{
		g_ServerDAL.AddCountryToSynchronize(*l_Itr);
	}
   return l_iTotalCivilianCasualties;
}

/*!
 * Compute the military casualties of a nuke (and perform modifications)
 * @return Number of military units dead
 **/
UINT32 GDataControlLayer::NuclearCasualtiesMilitaryExecute(const GNukeTarget& in_Target,
                                                           const multimap<REAL32,GNuclearMissile>& in_Missiles)
{
	UINT32 l_iCasualtiesTotal = 0;
	REAL32 l_fCasualtiesValue = 0;
	set<UINT32> l_GroupToBeRemove;

   //Get the highest missile damage range of the biggest missile
   REAL32 l_fHighestDamageRangeDegrees = 0 ;
   {
      REAL32 l_fHighestDamageRangeKM = 0;
      multimap<REAL32,GNuclearMissile>::const_iterator l_BiggestMissileIt = in_Missiles.end();
      l_BiggestMissileIt--;
      l_fHighestDamageRangeKM = l_BiggestMissileIt->first;
      l_fHighestDamageRangeDegrees = GDCLInterface::KMLengthInDegrees(in_Target.m_Position,l_fHighestDamageRangeKM);
   }

   //List all the unit groups that are in range of the blast
   multimap<REAL32,const SP2::GUnitGroupEx*> l_GroupsInRange;
   {
	   hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator l_GroupsIt = g_Joshua.UnitManager().UnitGroups().begin();
	   while(l_GroupsIt != g_Joshua.UnitManager().UnitGroups().end())
	   {
		   const SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)l_GroupsIt->second;

         //Skip the units groups that are the Military production queue
         if(m_bNuclearVulnerabilityByStatus[l_pGroup->Status()] == false)
         {
            l_GroupsIt++;
            continue;
         }

         REAL32 l_fDistance = l_pGroup->Position().Distance(in_Target.m_Position);
         if(l_fDistance <= l_fHighestDamageRangeDegrees)
         {
            l_GroupsInRange.insert(make_pair(l_fDistance,l_pGroup));
         }
         l_GroupsIt++;
      }
   }//End of list all the unit groups that were hit



   //Apply the damage to the groups in range
   for(multimap<REAL32,const SP2::GUnitGroupEx*>::iterator l_GroupsInRangeIt = l_GroupsInRange.begin();
       l_GroupsInRangeIt != l_GroupsInRange.end();
       l_GroupsInRangeIt++)
   {
      g_ServerDAL.DirtyCountryUnitsServer(l_GroupsInRangeIt->second->OwnerId() );

      //For each missile, check if it touched the group
      for(multimap<REAL32,GNuclearMissile>::const_iterator l_MissileIt = in_Missiles.begin();
          l_MissileIt != in_Missiles.end();
          l_MissileIt++)
      {
         //Given the strength of the nuke
          REAL32 l_fFullDamageRange = l_MissileIt->second.FullDamageRange() * g_SP2Server->NuclearMissileRangePercentage();

         bool l_bFullBlast = false;
         
		   REAL32 l_fDistance = GDALInterface::DistanceBetween2PointsLatLong(l_GroupsInRangeIt->second->Position(),in_Target.m_Position);
		   //! \todo: A unit group should have a radius. Larger groups == larger radius.
		   //If the group was within blast radius
		   if(l_fDistance <= l_fFullDamageRange)
		   {//Within full blast range
			   l_bFullBlast = true;
		   }
         //Compute the casualties for that group
		   if(l_bFullBlast)
		   {
            //If the group is in a combat, remove it from there
            if(l_GroupsInRangeIt->second->ArenaInfo())
				{
					SP2::GUnitGroupEx* l_pGroup     = (SP2::GUnitGroupEx*)l_GroupsInRangeIt->second;
					gassert(l_pGroup,"Should be a group");
					SP2::GArenaInfo*   l_pArenaInfo = (SP2::GArenaInfo*)l_pGroup->ArenaInfo();
					gassert(l_pArenaInfo,"Should be an arena info");
					SP2::GArena*       l_pArena     = (SP2::GArena*)g_CombatManager.Arena(l_pArenaInfo->m_iId);
					gassert(l_pArena, "Should be an arena");
					l_pArena->RemoveGroup(l_pGroup);
				}
            l_GroupToBeRemove.insert(l_GroupsInRangeIt->second->Id());
      		
			   //For each unit, compute the casualties and add it to the correct category
			   for(UINT32 i = 0 ; i  < l_GroupsInRangeIt->second->Units().size() ; i++)
			   {				
				   SP2::GUnit*       l_pUnit       = (SP2::GUnit*)l_GroupsInRangeIt->second->Units()[i];
				   SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_pUnit->Design();
				   UINT32 l_iCasualties = l_pUnit->Qty();     			
				   l_fCasualtiesValue += l_iCasualties*l_pUnitDesign->Cost();
				   l_iCasualtiesTotal += l_iCasualties;                  
			   }//end of for each unit of the group
         }
      }
   }

	//now remove unit groups
   for(set<UINT32>::iterator l_GroupsToRemoveIt = l_GroupToBeRemove.begin(); 
       l_GroupsToRemoveIt != l_GroupToBeRemove.end();
       l_GroupsToRemoveIt++)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_GroupsToRemoveIt);
      vector<SDK::Combat::GUnit*> l_vUnitsToRemove = l_pGroup->Units();
      //Remove all the units of the group
      for(UINT32 j = 0 ; j < l_vUnitsToRemove.size() ; j++)
      {
         g_Joshua.UnitManager().KillUnit(l_vUnitsToRemove[j]->Id(),true);
      }
   }
	return l_iCasualtiesTotal;
}

void GDataControlLayer::ChangeCapitalID(UINT32 in_iCountryID, UINT32 in_iCapitalID)
{   
	// Make sure city exists
   const SCityInfo* l_pCity = g_ServerDAL.City(in_iCapitalID);
   if(l_pCity)
   {
      // Make sure city can be used as capital for country
      const GRegionControl& l_RegionControl = g_ServerDAL.RegionControl(l_pCity->m_iRegionId);
      if( (l_RegionControl.m_iMilitary == in_iCountryID) && 
            (l_RegionControl.m_iPolitical == in_iCountryID) )
         ConfirmChangeCapitalID(in_iCountryID, in_iCapitalID);
   }
   // Try to set a region as capital instead
   else if(in_iCapitalID & 0x80000000)
   {
      UINT32 l_iRegionId = in_iCapitalID & 0x7FFFFFFF;
      if(l_iRegionId == 0)
         ConfirmChangeCapitalID(in_iCountryID, in_iCapitalID);
      else if(l_iRegionId <= g_ServerDAL.NbRegion() )
      {
         // Make sure region is controlled by country
         const GRegionControl& l_RegionControl = g_ServerDAL.RegionControl(l_iRegionId);
         if( (l_RegionControl.m_iMilitary == in_iCountryID) && 
            (l_RegionControl.m_iPolitical == in_iCountryID) )
         {
            // Make sure the region does not have cities
            if(g_ServerDAL.CitiesInRegion(l_iRegionId).empty() )
               ConfirmChangeCapitalID(in_iCountryID, in_iCapitalID);
         }
      }
   }	

}

UINT32 GDataControlLayer::CreateNewTreaty(ENTITY_ID in_iCountryCreator,
													 const set<ENTITY_ID>& in_vSideA,
													 const set<ENTITY_ID>& in_vSideB,
													 const set<ENTITY_ID>& in_vPressure,
													 ETreatyType::Enum in_iType,
													 bool in_bPrivate,
													 GString& in_sName,
													 const UINT32* in_pConditions)
{
	//Can't create treaties when in Anarchy
	if(g_ServerDAL.CountryData(in_iCountryCreator)->GvtType() == EGovernmentType::Anarchy)
		return 0;

    //Can't create treaties as a client
    if(g_ServerDAL.CountryData(in_iCountryCreator)->Master().first != 0)
        return 0;
	
	//Can't create an alliance if the condition isn't set at no limit
	if(in_iType == ETreatyType::Alliance && in_pConditions[ETreatyConditions::RelationsNotAtWar] == EConditionStatus::NoLimit)
		return 0;

    const bool l_bIsClientStateTreaty = in_iType == ETreatyType::MilitaryAccess &&
                                        in_sName.find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0;
    const GCountryData* l_pClientData = nullptr;
    const ENTITY_ID l_iMasterID = l_bIsClientStateTreaty ? *in_vSideA.cbegin() : 0;

    //Don't go through with creating a client state treaty if the client isn't eligible, or if the master-client relationship already exists between the 2 parties
    if(l_bIsClientStateTreaty)
    {
        gassert(in_vSideA.size() == 1 && in_vSideB.size() == 1,
                "Client state treaty has more than 1 master or client");

        l_pClientData = g_ServerDAL.CountryData(*in_vSideB.cbegin());
        if(!l_pClientData->EligibleToBeClientOf(l_iMasterID))
            return 0;

        if(l_pClientData->Master().first == l_iMasterID)
        {
            gassert(l_pClientData->Master().second != 0, g_ServerDAL.CountryData(l_iMasterID)->NameAndIDForLog() + L"-" + l_pClientData->NameAndIDForLog() + L" doesn't have a valid treaty");
            return 0;
        }
    }

	//Check if a treaty already exist with that name
	GString l_sBaseName = in_sName;
	UINT32 l_iNbTreaties = 1;
	//Now check if existing treaties have the same name.
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end(); it++)
   {
      const GTreaty& l_Treaty = it->second;
		GString l_sTreatyName = l_Treaty.Name();
		if(in_sName == l_sTreatyName)
		{
			//Treaties have the same name, add number at the end
			in_sName = l_sBaseName + L" - " + GString(l_iNbTreaties);
			l_iNbTreaties++;
			it = l_Treaties.begin();
		}
	}	
	
	/*
	vector<INT32> l_vString;
	switch(in_iType)
	{
	case ETreatyType::Alliance:
		l_vString.push_back(SP2::EStrId::TreatyAlliance); break;
	case ETreatyType::Annexation:
		l_vString.push_back(SP2::EStrId::TreatyAnnexation); break;
	case ETreatyType::AssumeForeignDebt:
		l_vString.push_back(SP2::EStrId::TreatyAssumeForeignDebt); break;
	case ETreatyType::CommonMarket:
		l_vString.push_back(SP2::EStrId::TreatyCommonMarket); break;
	case ETreatyType::CulturalExchanges:
		l_vString.push_back(SP2::EStrId::TreatyCulturalExchanges); break;
	case ETreatyType::EconomicAid:
		l_vString.push_back(SP2::EStrId::TreatyEconomicAid); break;
	case ETreatyType::EconomicEmbargo:
		l_vString.push_back(SP2::EStrId::TreatyEconomicEmbargo); break;
	case ETreatyType::EconomicPartnership:
		l_vString.push_back(SP2::EStrId::TreatyEconomicPartnership); break;
	case ETreatyType::FreeRegion:
		l_vString.push_back(SP2::EStrId::TreatyFreeRegion); break;
	case ETreatyType::HumanDevelopmentCollaboration:
		l_vString.push_back(SP2::EStrId::TreatyHumanDevelopmentCollaboration); break;
	case ETreatyType::MilitaryAccess:
		l_vString.push_back(SP2::EStrId::TreatyMilitaryAccess); break;
	case ETreatyType::NobleCause:
		l_vString.push_back(SP2::EStrId::TreatyNobleCause); break;
	case ETreatyType::RequestMilitaryPresenceRemoval:
		l_vString.push_back(SP2::EStrId::TreatyRequestMilitaryPresenceRemoval); break;
	case ETreatyType::ResearchPartnership:
		l_vString.push_back(SP2::EStrId::TreatyResearchPartnership); break;	
	case ETreatyType::War:
		l_vString.push_back(SP2::EStrId::TreatyWar); break;
	case ETreatyType::WeaponTrade:
		l_vString.push_back(SP2::EStrId::TreatyWeaponTrade); break;
	case ETreatyType::WeaponTradeEmbargo:
		l_vString.push_back(SP2::EStrId::TreatyWeaponTradeEmbargo); break;
	default:
		gassert(false,"GDataControlLayer::CreateNewTreaty: Type of treaty is missing");
		break;
	}	
	
	News::GInfo l_News;
	l_News.m_iCountryA = in_iCountryCreator;
	l_News.m_fGameTime = g_Joshua.GameTime();
	l_News.m_eType = News::EType::ProposedTreaty;
	l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
	l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
	l_News.m_vStrings = l_vString;
	SendNews(l_News);
	*/

	GTreaty l_NewTreaty;
	l_NewTreaty.Type(in_iType);
	l_NewTreaty.Private(in_bPrivate || l_bIsClientStateTreaty);
	l_NewTreaty.Name(in_sName);	
	l_NewTreaty.Creator(in_iCountryCreator);

	for(set<ENTITY_ID>::const_iterator l_Itr = in_vSideA.begin();
		 l_Itr != in_vSideA.end();
		 l_Itr++)
	{
		if(*l_Itr == in_iCountryCreator)
			l_NewTreaty.AddMemberSideA(*l_Itr,true,true);			
		else
			l_NewTreaty.AddMemberSideA(*l_Itr,false,true);
	}

	for(set<ENTITY_ID>::const_iterator l_Itr = in_vSideB.begin();
		 l_Itr != in_vSideB.end();
		 l_Itr++)
	{
		if(*l_Itr == in_iCountryCreator)
			l_NewTreaty.AddMemberSideB(*l_Itr,true,true);
		else
		{
			if(!l_NewTreaty.SideBCanLeave() || l_bIsClientStateTreaty) 
				l_NewTreaty.AddMemberSideB(*l_Itr,true,true);
			else
				l_NewTreaty.AddMemberSideB(*l_Itr,false,true);
		}
	}
	
	//If the creator isn't on side A or B, join it on side pressure
	if(in_vSideA.find(in_iCountryCreator) == in_vSideA.end() &&
		in_vSideB.find(in_iCountryCreator) == in_vSideB.end())
	{
		l_NewTreaty.AddMemberPressure(in_iCountryCreator);
	}
	for(set<ENTITY_ID>::const_iterator l_Itr = in_vPressure.begin();
		 l_Itr != in_vPressure.end();
		 l_Itr++)
	{
		l_NewTreaty.AddMemberPressure(*l_Itr);		
	}	
	
	//Add the conditions
	for(UINT32 i=0; i<ETreatyConditions::ItemCount; i++)
	{
		if(in_pConditions[i])
		{
			l_NewTreaty.AddCondition((ETreatyConditions::Enum)i,in_pConditions[i]);
		}
	}

   g_ServerDAL.AddTreaty(l_NewTreaty);	

	UINT32 l_iTreatyID = l_NewTreaty.ID();
	SetEligibleCountries(l_iTreatyID, true);

    GDZLOG(g_ServerDAL.CountryData(in_iCountryCreator)->NameAndIDForLog() +
           L" has created treaty: " + in_sName +
           L" with ID: " + GString(l_iTreatyID),
           EDZLogCat::Treaties);

	//Then, send the game events
	for(set<ENTITY_ID>::const_iterator l_Itr = in_vSideA.begin();
		 l_Itr != in_vSideA.end();
		 l_Itr++)
	{
		if(*l_Itr != in_iCountryCreator)			
		{		
			g_ServerDAL.AddFutureGameEventCreateNewTreaty(*l_Itr,l_iTreatyID);
			/*
			//Send GTreatyOfferGameEvent notice this country he has been added to a treaty		
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GTreatyOffer);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(*l_Itr)->Id();
			SP2::Event::GTreatyOffer* l_pTreatyOffer = (SP2::Event::GTreatyOffer*)l_Event.get();
			l_pTreatyOffer->TreatyID = l_iTreatyID;				
			g_Joshua.RaiseEvent(l_Event);
			*/
		}
	}
	for(set<ENTITY_ID>::const_iterator l_Itr = in_vSideB.begin();
		 l_Itr != in_vSideB.end();
		 l_Itr++)
	{
		if(*l_Itr != in_iCountryCreator                            &&
           (l_NewTreaty.SideBCanLeave() && !l_bIsClientStateTreaty))		
		{		
			g_ServerDAL.AddFutureGameEventCreateNewTreaty(*l_Itr,l_iTreatyID);
			/*
			//Send GTreatyOfferGameEvent notice this country he has been added to a treaty		
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GTreatyOffer);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(*l_Itr)->Id();
			SP2::Event::GTreatyOffer* l_pTreatyOffer = (SP2::Event::GTreatyOffer*)l_Event.get();
			l_pTreatyOffer->TreatyID = l_iTreatyID;				
			g_Joshua.RaiseEvent(l_Event);
			*/
		}
	}
	
	//Execute treaty
    GDZLOG(L"Treaty ID " + GString(l_iTreatyID) + L": " +
           L"Active " + GString(l_NewTreaty.Active()) + L", " +
           L"should be active " + GString(l_NewTreaty.ShouldBeActive()),
           EDZLogCat::Treaties);
	if(!l_NewTreaty.Active() && l_NewTreaty.ShouldBeActive())
	{
        gassert(l_bIsClientStateTreaty != (l_pClientData == nullptr),
                "Client treaty doesn't have a client");
        gassert(l_bIsClientStateTreaty != (l_iMasterID == 0),
                "Client treaty doesn't have a master, or non-client treaty has master");
        gassert(!l_bIsClientStateTreaty || l_pClientData->EligibleToBeClientOf(l_iMasterID),
                "Client treaty with ineligible client went through");

		GTreaty* l_pTreaty = g_ServerDAL.Treaty(l_iTreatyID);
        gassert(!l_bIsClientStateTreaty || l_pTreaty->Private(),
                "Non-private client state treaty");

		l_pTreaty->Active(true);

		//Treaty is active, let's notify clients
		g_ServerDAL.ModifyTreaty(l_iTreatyID);

		ExecuteTreaty(l_iTreatyID);
	}

	return l_iTreatyID;
}

void GDataControlLayer::JoinTreaty(ENTITY_ID in_iCountry, UINT32 in_iTreatyID, UINT32 in_iSide)
{
	if(in_iSide < 1 || in_iSide > 3)
		return;

	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	if(!l_pTreaty)
		return;

	if(!l_pTreaty->Ponctual() && l_pTreaty->EligibleCountries().count((INT16)in_iCountry) == 0) 
		return;

	if(in_iSide == 1)
	{		
		//Is already an active member
		if(l_pTreaty->MembersSideA(true).find(in_iCountry) != l_pTreaty->MembersSideA(true).end())
			return;
		//Is a member on side B, and cannot change side
		if(!l_pTreaty->SideBCanLeave() && (l_pTreaty->MembersSideB(true).count(in_iCountry) > 0))
			return;
		if( (!l_pTreaty->Private()) || (l_pTreaty->Private() && l_pTreaty->CountrySide(in_iCountry) == 1) )
		{
			l_pTreaty->RemoveMemberSideB(in_iCountry); 
			l_pTreaty->RemoveMemberPressure(in_iCountry);
			l_pTreaty->AddMemberSideA(in_iCountry,true);
		}
		else
			return;
	}
	else if (in_iSide == 2)
	{		
		//If it's an economic aid treaty, a country cannot join on side B if it was not an original member
		if(l_pTreaty->Type() == ETreatyType::EconomicAid && !l_pTreaty->IsAnOriginalB(in_iCountry))
			return;
		//Is already an active member
		if(l_pTreaty->MembersSideB(true).count(in_iCountry) > 0)
			return;
		if( (!l_pTreaty->Private()) || (l_pTreaty->Private() && l_pTreaty->CountrySide(in_iCountry) == 2) )
		{
			l_pTreaty->RemoveMemberSideA(in_iCountry); 
			l_pTreaty->RemoveMemberPressure(in_iCountry);
			l_pTreaty->AddMemberSideB(in_iCountry,true);
		}
		else
			return;
	}
	else
	{
		l_pTreaty->RemoveMemberSideA(in_iCountry); 
		l_pTreaty->RemoveMemberSideB(in_iCountry); 
		l_pTreaty->AddMemberPressure(in_iCountry);
	}

#ifdef LOG_DCL
   g_Joshua.Log(L"Country " + GString(in_iCountry) + L" has joined treaty: " + l_pTreaty->Name() + L" With ID: " + GString(l_pTreaty->ID()));
#endif

	SetEligibleCountries(in_iTreatyID, true);
	g_ServerDAL.ModifyTreaty(in_iTreatyID);

	//Execute treaty
	if(!l_pTreaty->Active() && l_pTreaty->ShouldBeActive())
	{
		l_pTreaty->Active(true);
		
		l_pTreaty->ClearPressure();

		//Treaty is active, let's notify clients
		g_ServerDAL.ModifyTreaty(l_pTreaty->ID());

		ExecuteTreaty(l_pTreaty->ID());
	}
   else if(l_pTreaty->Active() )
   {
      g_ServerDAL.AddHistoricalMarker(in_iCountry, 
                                      EHistoryMarkerType::TreatySigned, 
                                      (REAL32) EStrId::c_TreatyTypes[l_pTreaty->Type() ] );
   }
}

void GDataControlLayer::LeaveTreaty(ENTITY_ID in_iCountry, UINT32 in_iTreatyID, bool in_bClearOriginal)
{
	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	if(!l_pTreaty)
		return;

   if(l_pTreaty->Active() )
   {
      g_ServerDAL.AddHistoricalMarker(in_iCountry, 
                                      EHistoryMarkerType::TreatyCanceled, 
                                      (REAL32) EStrId::c_TreatyTypes[l_pTreaty->Type() ] );
   }

	UINT32 l_iSide = l_pTreaty->CountrySide(in_iCountry);
	if(l_iSide < 1 || l_iSide > 3)
		return;

   const bool l_bIsClientStateTreaty = l_pTreaty->Type() == ETreatyType::MilitaryAccess &&
                                       l_pTreaty->Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0;

	switch(l_iSide)
	{
		case 1:
            if(l_bIsClientStateTreaty && !l_pTreaty->MembersSideA(true).empty())
                g_ServerDAL.CountryData(in_iCountry)->RemoveClient(*l_pTreaty->MembersSideB(true).cbegin());
			l_pTreaty->RemoveMemberSideA(in_iCountry);
			break;
		case 2:
			if(!l_pTreaty->SideBCanLeave() && !in_bClearOriginal)
                return;
            else if(l_bIsClientStateTreaty)
            {
                //Check if the client treaty has an active master. If not, then cancel the treaty.
                const set<ENTITY_ID>& l_vMasters = l_pTreaty->MembersSideA(true);
                switch(l_vMasters.size())
                {
                case 1:
                {
                    const GCountryData* const l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
                    if(l_pCountryData->MilitaryStrength() <
                       g_ServerDAL.CountryData(*l_vMasters.cbegin())->MilitaryStrength())
                    {
                        GDZLOG(g_ServerDAL.CountryData(in_iCountry)->NameAndIDForLog() +
                               L" is trying to leave a client state treaty, " +
                               L"but can't because it's the client",
                               EDZLogCat::ClientStates);
                        return;
                    }
                    else
                        g_ServerDAL.CountryData(in_iCountry)->RemoveClient(*l_pTreaty->MembersSideB(true).cbegin());
                    break;
                }
                default:
                    gassert(l_vMasters.size() == 0,
                            L"Client treaty has " + GString(l_vMasters.size()) + L" masters");
                    break;
                }
            }
			l_pTreaty->RemoveMemberSideB(in_iCountry);
			break;
		case 3:
			l_pTreaty->RemoveMemberPressure(in_iCountry);
			break;
	}	

	if(in_bClearOriginal)
	{
		l_pTreaty->RemoveOriginal(in_iCountry);
	}

#ifdef LOG_DCL
   g_Joshua.Log(L"Country " + GString(in_iCountry) + L" has left treaty: " + l_pTreaty->Name() + L" With ID: " + GString(l_pTreaty->ID()));
#endif
	
	if(
		//Treaty is empty
		 (l_pTreaty->MembersSideA(false).size() == 0 &&
		  l_pTreaty->MembersSideB(false).size() == 0 &&
		  l_pTreaty->MembersPressure().size() == 0)
		  ||		 
	   //Treaty is ponctual, and everyone has refused
	    (l_pTreaty->Ponctual() && 
		  l_pTreaty->MembersPressure().size() == 0 &&
		 (l_pTreaty->MembersSideA(false).size() != l_pTreaty->MembersOriginalsA().size() ||
		  l_pTreaty->MembersSideB(false).size() != l_pTreaty->MembersOriginalsB().size()))			
		  ||
	  //Treaty has no one on side A, but some on side B
	     (l_pTreaty->MembersOriginalsA().size() == 0 &&
		   l_pTreaty->MembersOriginalsB().size() > 0)
		  ||
	  //Treaty is active, has only one member left, and is long term. It is useless
		 (!l_pTreaty->Ponctual() && 
		   l_pTreaty->Active() && 
			l_pTreaty->MembersOriginalsA().size() && 
		  (((((REAL32)(l_pTreaty->MembersSideA(true).size() + l_pTreaty->MembersSideA(false).size()) / (REAL32)l_pTreaty->MembersOriginalsA().size())  < SP2::c_fTreatyActivationRatio)) ||
		   (l_pTreaty->MembersOriginalsB().size() > 1 &&  ((((REAL32)(l_pTreaty->MembersSideB(true).size() + l_pTreaty->MembersSideB(false).size()) / (REAL32)l_pTreaty->MembersOriginalsB().size())  < SP2::c_fTreatyActivationRatio)))) &&
		   l_pTreaty->MembersPressure().size() == 0)
		  ||
		  //Treaty is not active yet, and enough people have refused
		 (!l_pTreaty->Ponctual() && 
		 ( (l_pTreaty->MembersOriginalsA().size() > 0 && ((REAL32)(l_pTreaty->MembersOriginalsA().size() - l_pTreaty->MembersSideA(false).size()) / (REAL32)l_pTreaty->MembersOriginalsA().size()) > (1.f - SP2::c_fTreatyActivationRatio)) ||
		   (l_pTreaty->MembersOriginalsB().size() > 0 && ((REAL32)(l_pTreaty->MembersOriginalsB().size() - l_pTreaty->MembersSideB(false).size()) / (REAL32)l_pTreaty->MembersOriginalsB().size()) > (1.f - SP2::c_fTreatyActivationRatio)) )&&
		   l_pTreaty->MembersPressure().size() == 0))
			
	{
		//Remove treaty
		g_ServerDAL.RemoveTreaty(in_iTreatyID);		
		return;
	}

	SetEligibleCountries(in_iTreatyID, true);
	g_ServerDAL.ModifyTreaty(in_iTreatyID);

}

void GDataControlLayer::ExecuteTreaty(UINT32 in_iTreatyID)
{
	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	if(!l_pTreaty)
		return;

	gassert(l_pTreaty->Active(), "Treaty should be active");

	//Clear the decisions made by those who rejected it
	g_ServerDAL.TreatyRefusalHistory().erase(in_iTreatyID);

   const set<UINT32>& l_vSideA = l_pTreaty->MembersSideA(true);
	const set<UINT32>& l_vSideB = l_pTreaty->MembersSideB(true);

	vector<INT32> l_vStrings;

	switch(l_pTreaty->Type())
	{
	case ETreatyType::War:
		//Side A declares war to side B
        //Treaty creator is the attacker master, unless the creator isn't a participant.
		DeclareNewWar(l_vSideA,
                      (l_pTreaty->CountrySide(l_pTreaty->Creator()) == 1) ? l_pTreaty->Creator() : *(l_vSideA.begin()),
                      *(l_vSideB.begin()));		
		break;
	case ETreatyType::RequestMilitaryPresenceRemoval:
		{
			//Remove second country's units from first country
			UINT32 l_iFirstCountry = *l_vSideB.begin();
			UINT32 l_iSecondCountry = *l_vSideA.begin();
			if(l_iFirstCountry > 0 && l_iSecondCountry > 0)
			{
                if(m_UnitMover.MilitaryRemoval(l_iFirstCountry, l_iSecondCountry))
                {
                    //Improve relations with country from which military was removed
                    g_ServerDAL.RelationBetweenCountries(l_iFirstCountry, l_iSecondCountry,
                                                         g_ServerDAL.RelationBetweenCountries(l_iFirstCountry, l_iSecondCountry) + 10);
                    
                    //Send the news
                    vector<INT32> l_vStrings;
                    l_vStrings.push_back(SP2::EStrId::TreatyRequestMilitaryPresenceRemoval);
                    News::GInfo l_News;
                    l_News.m_iCountryA = l_iFirstCountry;
                    l_News.m_iCountryB = l_iSecondCountry;
                    l_News.m_eType = News::EType::SignedTreatyTwo;
                    l_News.m_fGameTime = g_Joshua.GameTime();
                    l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
                    l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
                    l_News.m_vStrings = l_vStrings;
                    SendNews(l_News);
                }
                else
                {
                    GDZLOG(L"Military removal treaty " + GString(in_iTreatyID) + L" has no effect",
                           EDZLogCat::Treaties);
                }
			}
		}
		break;
	case ETreatyType::Annexation:
		//Annex every regions that first country has military control, and second country has political control
		{
			const vector<GRegionControl>& l_vRegions = g_ServerDAL.RegionControlArray();
			UINT32 l_iMilitaryControl = *l_vSideA.begin();
			UINT32 l_iPoliticControl = *l_vSideB.begin();
			bool l_bRegionsFound = false;
			for(UINT32 i=0; i<l_vRegions.size(); i++)
			{
				if(l_vRegions[i].m_iMilitary == l_iMilitaryControl &&
					l_vRegions[i].m_iPolitical == l_iPoliticControl)
				{
					//Annex that region
					g_ServerDCL.AnnexRegion(l_iMilitaryControl,i);
					l_bRegionsFound = true;
				}
			}
			if(l_bRegionsFound)
			{
				LoseRelationsAnnexation(l_iMilitaryControl,l_iPoliticControl);
				//Send news
				News::GInfo l_News;
				l_News.m_iCountryA = l_iMilitaryControl;
				l_News.m_iCountryB = l_iPoliticControl;
				l_News.m_eType = News::EType::DeclareIntentionToAnnexRegions;
				l_News.m_fGameTime = g_Joshua.GameTime();
				l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
				l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
				SendNews(l_News);
			}			
		}
		break;
	case ETreatyType::FreeRegion:
		//Free every region that first country has military control, and second country has political control
		{
			const vector<GRegionControl>& l_vRegions = g_ServerDAL.RegionControlArray();
			UINT32 l_iMilitaryControl = *l_vSideA.begin();
			UINT32 l_iPoliticControl = *l_vSideB.begin();
            const GCountryData* const l_pPoliticalControlData = g_ServerDAL.CountryData(l_iPoliticControl);

            //If country is sufficiently occupied, and it's a client, then it may be freed from its master
            const REAL32 l_fPercentageOccupied = l_pTreaty->Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0 ?
                                                 l_pPoliticalControlData->PercentageOfPopulationOccupiedByCountry(l_iMilitaryControl) :
                                                 0.f;
            if(l_fPercentageOccupied >= 0.8f)
            {
                const auto l_MasterPair = l_pPoliticalControlData->Master();
                if(l_MasterPair.first != 0)
                {
                    gassert(g_ServerDAL.Treaty(l_MasterPair.second) != nullptr,
                            l_pPoliticalControlData->NameAndIDForLog() +
                            L" is a client but doesn't have an associated treaty");
                    LeaveTreaty(l_MasterPair.first, l_MasterPair.second, true);
                }
            }

			for(UINT32 i=0; i<l_vRegions.size(); i++)
			{
				if(l_vRegions[i].m_iMilitary == l_iMilitaryControl &&
					l_vRegions[i].m_iPolitical == l_iPoliticControl)
				{
					//Liberate that region
					g_ServerDCL.ChangeRegionMilitaryControl(i,l_iPoliticControl);

					//Liberating gives +2 relations between the owner and former occupier for every region freed
					g_ServerDAL.RelationBetweenCountries(l_iPoliticControl, l_iMilitaryControl,
						g_ServerDAL.RelationBetweenCountries(l_iPoliticControl, l_iMilitaryControl) + 2);
				}
			}

			//Synchronize the 2 countries
			g_ServerDAL.CountryData(l_iMilitaryControl)->SynchronizeWithRegions();
			g_ServerDAL.CountryData(l_iPoliticControl)->SynchronizeWithRegions();

			//MultiMOD
			//This also gives a +5 to country stability due to regions being governed by the correct gov again
			ChangeCountryStability(l_iPoliticControl, +5, false);

			//Send the news
			vector<INT32> l_vStrings;
			l_vStrings.push_back(SP2::EStrId::TreatyFreeRegion);
			News::GInfo l_News;
			l_News.m_iCountryA = l_iMilitaryControl;
			l_News.m_iCountryB = l_iPoliticControl;
			l_News.m_eType = News::EType::SignedTreatyTwo;
			l_News.m_fGameTime = g_Joshua.GameTime();
			l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
			l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
			l_News.m_vStrings = l_vStrings;
			SendNews(l_News);

		}
		break;
	case ETreatyType::AssumeForeignDebt:
	{
		REAL32 l_fEconomicStrength = 0.f;
		//first we need to calculate the economic stength of the whole treaty
		for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
			 l_Itr != l_vSideA.end();
			 l_Itr++)
		{
			l_fEconomicStrength += g_ServerDAL.CountryData(*l_Itr)->EconomicStrength();

			//Raise relations with countries on side B
			for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			 l_ItrB != l_vSideB.end();
			 l_ItrB++)
			{
				GCountryData* l_pCountryDataB = g_ServerDAL.CountryData(*l_ItrB);
				if(l_pCountryDataB && l_pCountryDataB->MonetarySupplyAmount() < 0.f)				
				{
					g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB,
						g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB) + SP2::c_fBonusRelationsWhenSigningAssumeForeignDebt);
				}
			}

		}
		if(l_fEconomicStrength == 0.f)
			break;
		//then calculate the total debt of countries on side B
		REAL64 l_fTotalDebt = 0.f;
		UINT32 l_iCountryReceivingMoney = 0;
		for(set<UINT32>::const_iterator l_Itr = l_vSideB.begin();
			 l_Itr != l_vSideB.end();
			 l_Itr++)
		{
			l_iCountryReceivingMoney = *l_Itr;
			if(g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount() < 0.f)
			{
				l_fTotalDebt += abs(g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount());
				g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount(0.f);
			}
		}
		//finally, share the debt to countries on Side A
		REAL32 l_fDebtRatio = 0.f;		
		for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
			 l_Itr != l_vSideA.end();
			 l_Itr++)
		{			
			l_fDebtRatio = g_ServerDAL.CountryData(*l_Itr)->EconomicStrength() / l_fEconomicStrength;
			g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount(
				g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount() - ( (REAL64)l_fDebtRatio * l_fTotalDebt ) );
		}

		if(l_iCountryReceivingMoney)
		{
			//Send the news
			vector<INT32> l_vStrings;
			l_vStrings.push_back(SP2::EStrId::TreatyAssumeForeignDebt);
			News::GInfo l_News;
			l_News.m_iCountryA = l_iCountryReceivingMoney;
			l_News.m_eType = News::EType::SignedTreatySingle;
			l_News.m_fGameTime = g_Joshua.GameTime();
			l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
			l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
			l_News.m_vStrings = l_vStrings;
			SendNews(l_News);
		}
	}
	break;
   
	case ETreatyType::Alliance:
		l_vStrings.push_back(EStrId::TreatyAlliance); break;
	case ETreatyType::CommonMarket:
		l_vStrings.push_back(EStrId::TreatyCommonMarket); break;
	case ETreatyType::CulturalExchanges:
		l_vStrings.push_back(EStrId::TreatyCulturalExchanges); break;
	case ETreatyType::EconomicAid:
		l_vStrings.push_back(EStrId::TreatyEconomicAid); break;
	case ETreatyType::EconomicEmbargo:
		l_vStrings.push_back(EStrId::TreatyEconomicEmbargo);

		//MultiMOD
		for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
			 l_Itr != l_vSideA.end();
			 l_Itr++)
		{			
			//Lower relations with countries on side B
			for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			 l_ItrB != l_vSideB.end();
			 l_ItrB++)
			{
				g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB,
					g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB) + -50);
			}
		}		
		break;
	case ETreatyType::EconomicPartnership:
		l_vStrings.push_back(EStrId::TreatyEconomicPartnership); break;
	case ETreatyType::HumanDevelopmentCollaboration:
		l_vStrings.push_back(EStrId::TreatyHumanDevelopmentCollaboration); break;
	case ETreatyType::NobleCause:
		l_vStrings.push_back(EStrId::TreatyNobleCause); break;
	case ETreatyType::ResearchPartnership:
		l_vStrings.push_back(EStrId::TreatyResearchPartnership); break;
	case ETreatyType::WeaponTrade:
		l_vStrings.push_back(EStrId::TreatyWeaponTrade); break;
	case ETreatyType::WeaponTradeEmbargo:
	   l_vStrings.push_back(EStrId::TreatyWeaponTradeEmbargo); 
	   //MultiMOD
	   //MultiMOD2
		for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
			 l_Itr != l_vSideA.end();
			 l_Itr++)
		{
			
			//Lower relations with countries on side B
			for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			 l_ItrB != l_vSideB.end();
			 l_ItrB++)
			{
				g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB,
					g_ServerDAL.RelationBetweenCountries(*l_Itr,*l_ItrB) + -25);
			}
		}
	   
		break;

    //Client state
    case ETreatyType::MilitaryAccess:
    {
        if(l_pTreaty->Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0)
        {
            gassert(l_vSideA.size() == 1 && l_vSideB.size() == 1,
                    L"Client state treaty has " + GString(l_vSideA.size()) +
                    L" members on side A and " +
                    GString(l_vSideB.size()) + L" on side B");

            const ENTITY_ID l_iMaster = *l_vSideA.cbegin();
            const ENTITY_ID l_iClient = *l_vSideB.cbegin();
            MakeClientState(l_iMaster, l_iClient, in_iTreatyID);
        }
    }
        break;
	}

	if(l_vStrings.size())
	{
		//Send the news				
		News::GInfo l_News;
		l_News.m_iCountryA = l_pTreaty->Creator();
		l_News.m_eType = News::EType::LongTermTreatySigned;
		l_News.m_fGameTime = g_Joshua.GameTime();
		l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
		l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
		l_News.m_vStrings = l_vStrings;
		SendNews(l_News);
	}

   for(set<UINT32>::const_iterator i = l_pTreaty->MembersSideA(true).begin();i != l_pTreaty->MembersSideA(true).end();++ i)
   {
      g_ServerDAL.AddHistoricalMarker(*i, 
                                      EHistoryMarkerType::TreatySigned, 
                                      (REAL32) EStrId::c_TreatyTypes[l_pTreaty->Type() ] );
   }

   for(set<UINT32>::const_iterator i = l_pTreaty->MembersSideB(true).begin();i != l_pTreaty->MembersSideB(true).end();++ i)
   {
      g_ServerDAL.AddHistoricalMarker(*i, 
                                      EHistoryMarkerType::TreatySigned, 
                                      (REAL32) EStrId::c_TreatyTypes[l_pTreaty->Type() ] );
   }

	if(l_pTreaty->Ponctual())
		g_ServerDAL.RemoveTreaty(l_pTreaty->ID());
}


void GDataControlLayer::SendNews(const News::GInfo& in_News)
{	
	UINT32      l_iCountry;
	UINT32      l_iTarget;
   UINT8       l_iOriginalPriority  = in_News.m_iPriority;
   News::GInfo l_News               = in_News;


   //Send the news to all the Human players
   const SDK::GPlayers& l_HumanPlayers = g_Joshua.HumanPlayers();
   for(SDK::GPlayers::const_iterator l_PlayerIt = l_HumanPlayers.begin();
       l_PlayerIt != l_HumanPlayers.end(); 
       l_PlayerIt++)
	{
      if(l_PlayerIt->second->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
      {
		   l_iCountry = l_PlayerIt->second->ModID();
		   l_iTarget = l_PlayerIt->second->Id();

         //Compute a priority
         UINT8 l_iPriority = l_iOriginalPriority;
         {
		      //! \todo Adjust the priority level based on treaties
            if( (l_News.m_iCountryA == l_iCountry) || (l_News.m_iCountryB == l_iCountry) )
   			   l_iPriority += 2;
		      else if(g_ServerDAL.InsideTheSameCountryGroup(l_iCountry,l_News.m_iCountryA) || g_ServerDAL.InsideTheSameCountryGroup(l_iCountry,l_News.m_iCountryB))
   			   l_iPriority += 1;

            l_News.m_iPriority = l_iPriority;
         }
      
         //Create, fill and raise the event
         {
		      SDK::GGameEventSPtr l_Event        = CREATE_GAME_EVENT(SP2::Event::GSendNews);
		      l_Event->m_iSource                 = SDK::Event::ESpecialTargets::Server;
		      l_Event->m_iTarget                 = l_iTarget;
		      SP2::Event::GSendNews* l_pSendNews = (SP2::Event::GSendNews*)l_Event.get();      
            l_pSendNews->m_News                = l_News;
		      g_Joshua.RaiseEvent(l_Event); 
         }
      }
	}//end of for each player
}

void GDataControlLayer::ConfirmChangeCapitalID(UINT32 in_iCountry,UINT32 in_iCapital)
{	
	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
		
	if(g_ServerDAL.CapitalExists(in_iCountry))
	{
		if(g_ServerDAL.CapitalCity(in_iCountry))
			l_vStrings.push_back(g_ServerDAL.CapitalCity(in_iCountry)->m_iNameStringID);
		else
			l_vStrings.push_back(g_ServerDAL.StringIdRegion(g_ServerDAL.CapitalID(in_iCountry)));		
	}

	g_ServerDAL.UpdateCapitalID(in_iCountry,in_iCapital);

	//If a capital changed has been made, let's drop the stability of that country by 5%
	ChangeCountryStability(in_iCountry,-0.05f);	

	if(g_ServerDAL.CapitalExists(in_iCountry))
	{
		if(g_ServerDAL.CapitalCity(in_iCountry))
			l_vStrings.push_back(g_ServerDAL.CapitalCity(in_iCountry)->m_iNameStringID);
		else
			l_vStrings.push_back(g_ServerDAL.StringIdRegion(g_ServerDAL.CapitalID(in_iCountry)));		
	}

	//Send the news to the clients
   {
      News::GInfo l_News;
      l_News.m_iCountryA = in_iCountry;
      l_News.m_eType = News::EType::MoveCapital;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      l_News.m_vNumbers = l_vNumbers;
      l_News.m_vStrings = l_vStrings;
      SendNews(l_News);
   }
}

bool GDataControlLayer::ConfirmChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
	UINT32 l_iOldOwner = g_ServerDAL.RegionControl(in_iRegionID).m_iMilitary;
	UINT32 l_iPoliticalOwner = g_ServerDAL.RegionControl(in_iRegionID).m_iPolitical;

	if(l_iOldOwner == in_iNewControlID)
		return false;

	bool l_bResult = g_ServerDAL.ChangeMilitaryControl(in_iRegionID,in_iNewControlID);
    GDZLOG(L"g_ServerDAL.ChangeMilitaryControl() returned " + GString(l_bResult) +
           L" for " + g_ServerDAL.CountryData(in_iNewControlID)->NameAndIDForLog() + 
           L" to occupy region " + GString(in_iRegionID),
           EDZLogCat::Regions);

	if(l_bResult)
	{
		if(l_iPoliticalOwner == in_iNewControlID ||
			l_iOldOwner == l_iPoliticalOwner)
		{
			vector<INT32> l_vStrings;
			l_vStrings.push_back(g_ServerDAL.StringIdRegion(in_iRegionID));

			//Send the news to the clients
			{
				News::GInfo l_News;
				l_News.m_iCountryA = in_iNewControlID;
				l_News.m_iCountryB = l_iOldOwner;
				l_News.m_eType = News::EType::TakenMilitaryControl;
				l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
				l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
				l_News.m_vStrings = l_vStrings;
				SendNews(l_News);
			}
		}
      g_ServerDAL.AddHistoricalMarker(l_iOldOwner, 
                                      EHistoryMarkerType::LooseMilitaryControl, 
                                      1);

      g_ServerDAL.AddHistoricalMarker(in_iNewControlID, 
                                      EHistoryMarkerType::GainMilitaryControl, 
                                      1);

		//If a country loses one of its own region, it will refuse war against that attacker
		if(l_iPoliticalOwner != in_iNewControlID)
		{
			const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();
			for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
					it != l_Wars.end(); it++)
			{
				const GWar& l_CurWar = it->second;
				if(l_CurWar.MasterAttacking() == in_iNewControlID &&
					l_CurWar.MasterDefending() == l_iPoliticalOwner)
				{
					ChangeOpinionOnWar(l_iPoliticalOwner,it->first,false);
				}
			}
		}

   }

	return l_bResult;
}

bool GDataControlLayer::ConfirmChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
	UINT32 l_iOldOwner = g_ServerDAL.RegionControl(in_iRegionID).m_iPolitical;
	
	bool l_bResult = g_ServerDAL.ChangePoliticalControl(in_iRegionID,in_iNewControlID);

	if(l_bResult)
	{
		vector<INT32> l_vStrings;
		l_vStrings.push_back(g_ServerDAL.StringIdRegion(in_iRegionID));
	   //Send the news to the clients
      {
         News::GInfo l_News;
         l_News.m_iCountryA = in_iNewControlID;
         l_News.m_iCountryB = l_iOldOwner;
         l_News.m_eType = News::EType::AnnexedRegions;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         l_News.m_vStrings = l_vStrings;
         SendNews(l_News);
      }

      g_ServerDAL.AddHistoricalMarker(l_iOldOwner, 
                                      EHistoryMarkerType::LoosePoliticalControl, 
                                      1);

      g_ServerDAL.AddHistoricalMarker(in_iNewControlID, 
                                      EHistoryMarkerType::GainPoliticalControl, 
                                      1);
	}

	return l_bResult;
}

void GDataControlLayer::SendNewsNukes()
{
	REAL64 l_fGameTime = g_Joshua.GameTime();

   //Send the news to the clients
	for(set<UINT32>::const_iterator l_Country = m_vNewsNukesAttacks.begin();
		 l_Country != m_vNewsNukesAttacks.end();
		 l_Country++)
	{		
      News::GInfo l_News;
      l_News.m_fGameTime = l_fGameTime;
      l_News.m_iCountryA = *l_Country;
      l_News.m_eType = News::EType::LaunchNuclearAttack;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
      SendNews(l_News);
	}
	m_vNewsNukesAttacks.clear();
}

void GDataControlLayer::SendNewsCasualties()
{
	vector<INT32> l_vNumbers;
	vector<INT32> l_vStrings;
	l_vNumbers.push_back(0);
	l_vStrings.push_back(0);
	REAL64 l_fGameTime = g_Joshua.GameTime();	

	for(hash_map<UINT32,INT64>::iterator it = m_NumberOfDeadByCountry.begin();
		 it != m_NumberOfDeadByCountry.end(); it++)
	{
		l_vNumbers[0] = (INT32)it->second;
		l_vStrings[0] = g_ServerDAL.CountryData(it->first)->NameID();
		if(l_vNumbers[0] > 0)
		{
	      //Send the news to the clients
         News::GInfo l_News;
         l_News.m_iCountryA = it->first;
         l_News.m_fGameTime = l_fGameTime;
         l_News.m_eType = News::EType::NuclearAttacks;
         l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
         l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
         l_News.m_vNumbers = l_vNumbers;
         l_News.m_vStrings = l_vStrings;
         SendNews(l_News);
		}
	}

	m_NumberOfDeadByCountry.clear();	
}

void GDataControlLayer::AddNewsNukeLaunched(ENTITY_ID in_iCountry)
{
	m_vNewsNukesAttacks.insert(in_iCountry);
}

void GDataControlLayer::AddCasualtiesToCountry(UINT32 in_iCountryID, UINT32 in_iCasualties)
{
	if(m_NumberOfDeadByCountry.find(in_iCountryID) == m_NumberOfDeadByCountry.end())
		m_NumberOfDeadByCountry[in_iCountryID] = in_iCasualties;
	else
		m_NumberOfDeadByCountry[in_iCountryID] += in_iCasualties;
}

void GDataControlLayer::RemoveProductionFromCountry(UINT32 in_iCountryID, REAL64 in_fLossOfProduction, EResources::Enum in_iResource)
{
	REAL64 l_fCountryProduction = g_ServerDAL.CountryData(in_iCountryID)->ResourceProduction(in_iResource);
	if(l_fCountryProduction <= 0.f)
		return;
	const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	GRegion* l_pRegion;
	REAL64 l_fRatio = 0.f;
	for(set<UINT32>::const_iterator l_Itr = l_vRegions.begin();
		 l_Itr != l_vRegions.end();
		 l_Itr++)
	{
		l_pRegion = g_ServerDAL.GetGRegion(*l_Itr);
		l_fRatio = l_pRegion->ResourceProduction(in_iResource) / l_fCountryProduction;
		l_pRegion->ResourceProduction(in_iResource,max(1.f,l_pRegion->ResourceProduction(in_iResource) - (l_fRatio * in_fLossOfProduction)) );
	}

}

void GDataControlLayer::AddProductionToCountry(UINT32 in_iCountryID, REAL64 in_fGainOfProduction, EResources::Enum in_iResource)
{
	REAL64 l_fCountryProduction = g_ServerDAL.CountryData(in_iCountryID)->ResourceProduction(in_iResource);
	if(l_fCountryProduction <= 0.f)
		return;
	const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	GRegion* l_pRegion;
	REAL64 l_fRatio = 0.f;
	for(set<UINT32>::const_iterator l_Itr = l_vRegions.begin();
		 l_Itr != l_vRegions.end();
		 l_Itr++)
	{
		l_pRegion = g_ServerDAL.GetGRegion(*l_Itr);
		l_fRatio = l_pRegion->ResourceProduction(in_iResource) / l_fCountryProduction;
		l_pRegion->ResourceProduction(in_iResource,l_pRegion->ResourceProduction(in_iResource) + (l_fRatio * in_fGainOfProduction));
	}

}

bool GDataControlLayer::CountryCanJoinThoseConditions(UINT32 in_iCountryID, const UINT32* in_pConditions, const set<UINT32>& in_vListOfCountries, bool in_bCheckForRelations)
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
   if(!l_pCountryData->Activated() )
      return false;

   //Checking for 2nd condition, the military strength similitude
	{
		INT16	l_iMinRank = g_ServerDAL.NbCountry() + 1;
		//Find the maximum and minimum army values required
		switch(in_pConditions[ETreatyConditions::MilitaryStrengthSimilitude])
		{
		case EConditionStatusRank::Top10:
			l_iMinRank = 10;
			break;
		case EConditionStatusRank::Top25:
			l_iMinRank = 25;
			break;
		case EConditionStatusRank::Top50:
			l_iMinRank = 50;
			break;
		case EConditionStatusRank::Top100:
			l_iMinRank = 100;
			break;
		}
		if(l_pCountryData->MilitaryRank() > l_iMinRank)
			return false;
	}//2nd check done

	//Checking for 3rd condition, the economical strength similitude
	{
		INT16	l_iMinRank = g_ServerDAL.NbCountry() + 1;
		//Find the maximum and minimum army values required
		switch(in_pConditions[ETreatyConditions::EconomicStrengthSimilitude])
		{
		case EConditionStatusRank::Top10:
			l_iMinRank = 10;
			break;
		case EConditionStatusRank::Top25:
			l_iMinRank = 25;
			break;
		case EConditionStatusRank::Top50:
			l_iMinRank = 50;
			break;
		case EConditionStatusRank::Top100:
			l_iMinRank = 100;
			break;
		}
		if(l_pCountryData->EconomicRank() > l_iMinRank)
			return false;
	}//3rd check done

	//checking for 4th condition, the political similitude
	{
		EGovernmentType::Enum l_eType = (EGovernmentType::Enum)l_pCountryData->GvtType();
		switch(in_pConditions[ETreatyConditions::PoliticalSimilitude])
		{
		case EConditionStatusPolitical::AllDemocracies:
			if( (l_eType != EGovernmentType::MultiPartyDemocracy) && 
				 (l_eType != EGovernmentType::SinglePartyDemocracy) )
				return false;
			break;
		case EConditionStatusPolitical::Communism:
			if(l_eType != EGovernmentType::Communist)
				return false;
			break;
		case EConditionStatusPolitical::MilitaryDictatorship:
			if(l_eType != EGovernmentType::MilitaryDictatorship)
				return false;
			break;
		case EConditionStatusPolitical::Monarchy:
			if(l_eType != EGovernmentType::Monarchy)
				return false;
			break;
		case EConditionStatusPolitical::MultiParty:
			if(l_eType != EGovernmentType::MultiPartyDemocracy)
				return false;
			break;
		case EConditionStatusPolitical::SingleParty:
			if(l_eType != EGovernmentType::SinglePartyDemocracy)
				return false;
			break;
		case EConditionStatusPolitical::Theocracy:
			if(l_eType != EGovernmentType::Theocracy)
				return false;
			break;
		}
	}//4th check done

	{
		REAL64 l_fMinimumResearchRatio = 0.f;
		switch(in_pConditions[ETreatyConditions::ResearchLevelSimilitude])
		{
		case EConditionStatusResearch::Low:
			l_fMinimumResearchRatio = SP2::c_fConditionsResearchLow;
			break;
		case EConditionStatusResearch::Average:
			l_fMinimumResearchRatio = SP2::c_fConditionsResearchAverage;
			break;
		case EConditionStatusResearch::High:
			l_fMinimumResearchRatio = SP2::c_fConditionsResearchHigh;
			break;
		}
		if(l_pCountryData->BudgetExpenseResearchRatio() < l_fMinimumResearchRatio)
			return false;
	}

	//checking for the 6th condition, the relations closeness
	{
		if(in_bCheckForRelations && in_pConditions[ETreatyConditions::RelationsNotAtWar] == EConditionStatusRelation::NotAtWar)
		{
			const UINT8* l_pStatuses = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);
			for(set<UINT32>::const_iterator l_Itr = in_vListOfCountries.begin();
				l_Itr != in_vListOfCountries.end(); l_Itr++)
			{
				if((EDiplomaticStatus::Enum)l_pStatuses[*l_Itr] == EDiplomaticStatus::Hostile)
					return false;
			}
		}		
	}

	//Checking for first condition, the geographic proximity
	{
		UINT32 l_iFindThatGeoGroup = 0;
		UINT32 l_iFindThatContinent = 0;
		switch(in_pConditions[ETreatyConditions::GeographicProximity])
		{
		case EConditionStatusGeo::Africa:
			l_iFindThatContinent = SP2::EContinent::Africa; break;
		case EConditionStatusGeo::Europe:
			l_iFindThatContinent = SP2::EContinent::Europe; break;
		case EConditionStatusGeo::America:
			l_iFindThatContinent = SP2::EContinent::America; break;
		case EConditionStatusGeo::Oceania:
			l_iFindThatContinent = SP2::EContinent::Oceania; break;
		case EConditionStatusGeo::Asia:
			l_iFindThatContinent = SP2::EContinent::Asia; break;
		case EConditionStatusGeo::CentralAmerica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::CentralAmerica; break;
		case EConditionStatusGeo::EasternAfrica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::EasternAfrica; break;
		case EConditionStatusGeo::EasternAsia:
			l_iFindThatGeoGroup = SP2::EGeoGroups::EasternAsia; break;
		case EConditionStatusGeo::EasternEurope:
			l_iFindThatGeoGroup = SP2::EGeoGroups::EasternEurope; break;
		case EConditionStatusGeo::FarEast:
			l_iFindThatGeoGroup = SP2::EGeoGroups::FarEast; break;
		case EConditionStatusGeo::MiddleEast:
			l_iFindThatGeoGroup = SP2::EGeoGroups::MiddleEast; break;
		case EConditionStatusGeo::NorthernAfrica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::NorthernAfrica; break;
		case EConditionStatusGeo::NorthernAmerica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::NorthernAmerica; break;
		case EConditionStatusGeo::PacificIslands:
			l_iFindThatGeoGroup = SP2::EGeoGroups::PacificIslands; break;
		case EConditionStatusGeo::SouthernAfrica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::SouthernAfrica; break;
		case EConditionStatusGeo::SouthernAmerica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::SouthernAmerica; break;
		case EConditionStatusGeo::SouthernOceania:
			l_iFindThatGeoGroup = SP2::EGeoGroups::SouthernOceania; break;
		case EConditionStatusGeo::WesternAfrica:
			l_iFindThatGeoGroup = SP2::EGeoGroups::WesternAfrica; break;
		case EConditionStatusGeo::WesternEurope:
			l_iFindThatGeoGroup = SP2::EGeoGroups::WesternEurope; break;
		}
		if(l_iFindThatGeoGroup > 0)
		{
			//search if that country has at least 1 region belonging to that geogroup
			bool l_bFound = false;		
			const set<UINT32>& l_Regions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
			for(set<UINT32>::const_iterator l_Itr = l_Regions.begin();
				l_Itr != l_Regions.end();
				l_Itr++)
			{
				if((UINT32)g_ServerDAL.RegionGeoGroup(*l_Itr) == l_iFindThatGeoGroup)
				{
					l_bFound = true;
					break;
				}				
			}	
			if(!l_bFound)
				return false;
		}
		if(l_iFindThatContinent > 0)
		{
			//search if that country has at least 1 region belonging to that geogroup
			bool l_bFound = false;		
			const set<UINT32>& l_Regions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
			for(set<UINT32>::const_iterator l_Itr = l_Regions.begin();
				l_Itr != l_Regions.end();
				l_Itr++)
			{
				if((UINT32)g_ServerDAL.RegionContinent(*l_Itr) == l_iFindThatContinent)
				{
					l_bFound = true;
					break;
				}				
			}	
			if(!l_bFound)
					return false;
		}
	}//1st check done

	return true;
}

void GDataControlLayer::ExecuteSuspension(UINT32 in_iTreatyID)
{
	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	gassert(l_pTreaty,"SP2_DCL::ExecuteSuspension: Treaty should exist");

	if(l_pTreaty->Ponctual())
		return;

	bool l_bChangesMadeInsideTreaty = false;

	set<UINT32> l_vListOfCountriesActive;
	set<UINT32> l_vListOfCountriesSuspended;
	const set<UINT32>& l_vSideA = l_pTreaty->MembersSideA(false);
	
	for(set<UINT32>::const_iterator l_ItrA = l_vSideA.begin();
		 l_ItrA != l_vSideA.end(); l_ItrA++)
	{
		if(l_pTreaty->IsSuspended(*l_ItrA))
			l_vListOfCountriesSuspended.insert(*l_ItrA);
		else
			l_vListOfCountriesActive.insert(*l_ItrA);
	}

	if(l_pTreaty->SideBCanLeave())
	{
		const set<UINT32>& l_vSideB = l_pTreaty->MembersSideB(false);
		
		for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			l_ItrB != l_vSideB.end(); l_ItrB++)
		{
			if(l_pTreaty->IsSuspended(*l_ItrB))
				l_vListOfCountriesSuspended.insert(*l_ItrB);
			else
				l_vListOfCountriesActive.insert(*l_ItrB);
		}
	}		

	//Run through the countries
	for(set<UINT32>::const_iterator l_Itr = l_vListOfCountriesActive.begin();
		 l_Itr != l_vListOfCountriesActive.end(); l_Itr++)
	{
		if(l_pTreaty->EligibleCountries().count((INT16)*l_Itr) == 0)
		{
			l_pTreaty->SuspendCountry(*l_Itr);
			l_bChangesMadeInsideTreaty = true;
		}
	}
	for(set<UINT32>::const_iterator l_Itr = l_vListOfCountriesSuspended.begin();
		 l_Itr != l_vListOfCountriesSuspended.end(); l_Itr++)
	{
		if(l_pTreaty->EligibleCountries().count((INT16)*l_Itr))
		{
			l_pTreaty->CancelSuspension(*l_Itr);
			l_bChangesMadeInsideTreaty = true;
		}
	}

	if(l_bChangesMadeInsideTreaty)
		g_ServerDAL.ModifyTreaty(in_iTreatyID);
}

bool GDataControlLayer::CountryCanJoinTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID, bool in_bCheckForRelations)
{

	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	gassert(l_pTreaty,"SP2_DCL::CountryCanJoinTreaty: Treaty should exist");

	if(l_pTreaty->Ponctual())
		return true;

	const UINT32* l_pConditions = l_pTreaty->Conditions();

	set<UINT32> l_vListOfCountries;
	l_vListOfCountries.insert(l_pTreaty->MembersSideA(false).begin(),l_pTreaty->MembersSideA(false).end());
	if(l_pTreaty->SideBCanLeave())
		l_vListOfCountries.insert(l_pTreaty->MembersSideB(false).begin(),l_pTreaty->MembersSideB(false).end());

	return CountryCanJoinThoseConditions(in_iCountryID,l_pConditions,l_vListOfCountries,in_bCheckForRelations);

}

void GDataControlLayer::LoseRelationsAnnexation(UINT32 in_iCountryAnnexing, UINT32 in_iCountryBeingAnnexed)
{
	//Lose relations with other countries			
	const UINT8* l_pDiplomaticAggressed = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryBeingAnnexed);
	REAL32 l_fRelationsTowardAggressor = 0.f;
	REAL32 l_fRelationsTowardAggressed = 0.f;
	REAL32 l_fLossOfRelations = 0.f;						
	for(UINT32 i=1; i<=g_ServerDAL.NbCountry(); i++)
	{
		if(i==in_iCountryAnnexing || i==in_iCountryBeingAnnexed || !g_ServerDAL.CountryIsValid(i))
			continue;
		l_fRelationsTowardAggressor = g_ServerDAL.RelationBetweenCountries(i,in_iCountryAnnexing);
		l_fRelationsTowardAggressed = g_ServerDAL.RelationBetweenCountries(i,in_iCountryBeingAnnexed);
		l_fLossOfRelations = (100.f + l_fRelationsTowardAggressor) - (100.f + l_fRelationsTowardAggressed)
			- SP2::c_fLossRelations;
		if(l_pDiplomaticAggressed[i] == EDiplomaticStatus::Allied)
			l_fLossOfRelations -= SP2::c_fLossRelationsAllies;
		l_fLossOfRelations = min(SP2::c_fLossRelationsMaximum,l_fLossOfRelations);
		if(l_fLossOfRelations != 0.f)
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryAnnexing,
				g_ServerDAL.RelationBetweenCountries(i,in_iCountryAnnexing) + l_fLossOfRelations);
	}
}

void GDataControlLayer::LoseRelationsWarDeclaration(UINT32 in_iCountryDeclaringWar, UINT32 in_iCountryBeingDeclaredWarOn, const set<UINT32>& in_CountriesAttackingWithAttacker)
{	
	const REAL32 c_fRelationsAdjustDefendingSide = 2.f;
	const REAL32 c_fRelationsAdjustMultiplierGoodBonus = 0.1f;
	const REAL32 c_fRelationsBonusAllied = 10.f;
	const REAL32 c_fRelationsAdjustWhenNotInSameGeoGroup = 0.5f;

	//Lose relations with other countries			
	const UINT8* l_pDiplomaticTarget = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryBeingDeclaredWarOn);
	INT16 l_iNbCountries = g_ServerDAL.NbCountry();
	GCountryData* l_pTarget		= g_ServerDAL.CountryData(in_iCountryBeingDeclaredWarOn);
	GCountryData* l_pObserver		= NULL;
	
	REAL32 l_fEconomicStrengthTarget = ((REAL32)(l_iNbCountries - l_pTarget->EconomicRank()) / (REAL32)(l_iNbCountries - 1));
	l_fEconomicStrengthTarget *= l_fEconomicStrengthTarget;
	REAL32 l_fEconomicStrengthObserver = 0.f;

	hash_set<INT32> l_iGeoGroupTarget;
	//Fill the geogroup of the target
	const set<UINT32>& l_RegionsTarget = g_ServerDAL.CountryPoliticalControl(in_iCountryBeingDeclaredWarOn);
	for(set<UINT32>::const_iterator it = l_RegionsTarget.begin();
		 it != l_RegionsTarget.end(); it++)
	{
		l_iGeoGroupTarget.insert((UINT32)g_ServerDAL.RegionGeoGroup(*it));
	}
	REAL32 l_fRelationsAttackerTarget = g_ServerDAL.RelationBetweenCountries(in_iCountryDeclaringWar,in_iCountryBeingDeclaredWarOn) + 100.f;
	
	REAL32 l_fRelationsTowardAttacker = 0.f;
	REAL32 l_fRelationsTowardTarget = 0.f;	
	REAL32 l_fChangeOfRelations = 0.f;
	REAL32 l_fRelationBonus = 0.f;

	for(UINT32 i=1; i<=g_ServerDAL.NbCountry(); i++)
	{		
		if(i==in_iCountryDeclaringWar || i==in_iCountryBeingDeclaredWarOn || !g_ServerDAL.CountryIsValid(i) || in_CountriesAttackingWithAttacker.count(i) > 0)
			continue;
		l_pObserver = g_ServerDAL.CountryData(i);

		bool l_bSameGeoGroup = false;
		const set<UINT32>& l_RegionsObserver = g_ServerDAL.CountryPoliticalControl(i); 
		for(set<UINT32>::const_iterator it = l_RegionsObserver.begin();
			it != l_RegionsObserver.end(); it++)
		{
			if(l_iGeoGroupTarget.count((UINT32)g_ServerDAL.RegionGeoGroup(*it)) > 0)
			{
				l_bSameGeoGroup = true;
				break;
			}
		}

		l_fEconomicStrengthObserver = ((REAL32)(l_iNbCountries - l_pObserver->EconomicRank()) / (REAL32)(l_iNbCountries - 1));
		l_fEconomicStrengthObserver *= l_fEconomicStrengthObserver;

		if(l_pDiplomaticTarget[i] == EDiplomaticStatus::Allied)
			l_fRelationBonus = c_fRelationsBonusAllied;
		else
			l_fRelationBonus = 0.f;			

		l_fRelationsTowardAttacker = g_ServerDAL.RelationBetweenCountries(i,in_iCountryDeclaringWar) + 100.f;
		l_fRelationsTowardTarget = (g_ServerDAL.RelationBetweenCountries(i,in_iCountryBeingDeclaredWarOn) + l_fRelationBonus) + 100.f;
		//Adjust the relations on the target side, to give greater importance to that side
		if(l_fRelationsTowardTarget > 0.f)
			l_fRelationsTowardTarget *= c_fRelationsAdjustDefendingSide;

		l_fChangeOfRelations = l_fRelationsTowardAttacker - l_fRelationsTowardTarget;

		REAL32 l_fPoliticIdeology = l_pObserver->LeaderParty()->PoliticalIdeology();
		if(l_fPoliticIdeology >= 0.5f)
			l_fChangeOfRelations *= (l_fPoliticIdeology * 2.f);
		else
			l_fChangeOfRelations *= (l_fPoliticIdeology + 0.5f);
		

		//Then, we must use the relations between the 2 countries attacking each other
		l_fChangeOfRelations -= (l_fRelationsAttackerTarget * l_fEconomicStrengthObserver) ;

		l_fChangeOfRelations *= l_fEconomicStrengthTarget;

		//If the relations is positive, give small bonuses towards the attacker
		if(l_fChangeOfRelations > 0.f)
			l_fChangeOfRelations *= c_fRelationsAdjustMultiplierGoodBonus;

		if(!l_bSameGeoGroup)
			l_fChangeOfRelations *= c_fRelationsAdjustWhenNotInSameGeoGroup;

		//! Adjust the aggressiveness to this factor
		l_fChangeOfRelations *= (0.5f + g_SP2Server->AIAggressiveness());

		g_ServerDAL.RelationBetweenCountries(i,in_iCountryDeclaringWar,
			g_ServerDAL.RelationBetweenCountries(i,in_iCountryDeclaringWar) + l_fChangeOfRelations);
	}
}

bool GDataControlLayer::AddPopulationToRegion(UINT32 in_iRegionID, INT64 in_iNbPopulation)
{
	GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_pRegion->OwnerId());

	INT64 l_iPop15 = l_pRegion->Population15();
	INT64 l_iPop1565 = l_pRegion->Population1565();
	INT64 l_iPop65 = l_pRegion->Population65();
	INT64 l_iPop = l_pRegion->Population();

	//Take care of military manpower available portion
	INT64 l_iNbOfEnrollToAdd =	(INT64)(((REAL32)in_iNbPopulation * ((REAL32)l_iPop1565/(REAL32)l_iPop)) * SP2::c_fPourcPopulationAbleToEnroll);	
	l_pCountryData->MiliManpowerAvailable(l_pCountryData->MiliManpowerAvailable() + l_iNbOfEnrollToAdd);
	//military manpower available portion done

	/*
		We can now start to add some population from group ages
	*/
	INT64 l_iTempLeftToAdd = in_iNbPopulation;		
	INT64 l_iTempAdding = 0;
	//add equally from age 15
	l_iTempAdding = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop15/(REAL32)l_iPop));		
	l_iPop15 += l_iTempAdding;
	l_iTempLeftToAdd -= l_iTempAdding;
	//add equally from age 15-65
	l_iTempAdding = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop1565/(REAL32)l_iPop));		
	l_iPop1565 += l_iTempAdding;
	l_iTempLeftToAdd -= l_iTempAdding;
	//add equally from age 65
	l_iTempAdding = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop65/(REAL32)l_iPop));		
	l_iPop65 += l_iTempAdding;
	l_iTempLeftToAdd -= l_iTempAdding;
	//Add the remaining
	if(l_iTempLeftToAdd != 0)
	{
		gassert(l_iTempLeftToAdd >= 0,"GDataControlLayer::AddPopulationToRegion(): Should be bigger than 0");
		l_iPop15 += l_iTempLeftToAdd;	
	}

	l_pRegion->Population15(l_iPop15);
	l_pRegion->Population1565(l_iPop1565);
	l_pRegion->Population65(l_iPop65);

	/*
		Now add people from religions
	*/
	l_iTempLeftToAdd = in_iNbPopulation;
	GReligionList l_Religions;
	l_pRegion->GetReligions(l_Religions);	
	for(GReligionList::iterator l_Itr = l_Religions.begin();
			l_Itr != l_Religions.end();
			l_Itr++)
	{
		l_iTempAdding = (INT64)(((REAL32)l_Itr->second / (REAL32)l_iPop) * (REAL32)in_iNbPopulation);
		l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
		l_iTempLeftToAdd -= l_iTempAdding;
	}
	if(l_iTempLeftToAdd != 0)
	{		
		gassert(l_iTempLeftToAdd >= 0,"GDataControlLayer::AddPopulationToRegion(): Should be bigger than 0");
		l_pRegion->GetReligions(l_Religions);	
		for(GReligionList::iterator l_Itr = l_Religions.begin();
				l_Itr != l_Religions.end();
				l_Itr++)
			{
				l_iTempAdding = l_iTempLeftToAdd;
				l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
				l_iTempLeftToAdd -= l_iTempAdding;
				if(l_iTempLeftToAdd == 0)
					break;
			}
			if(l_iTempLeftToAdd != 0)
			{
				g_Joshua.Log(L"Error while adding population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}

	}
	/*
		Now add people from languages
	*/
	l_iTempLeftToAdd = in_iNbPopulation;
	GLanguageList l_Languages;
	l_pRegion->GetLanguages(l_Languages);
	for(GLanguageList::iterator l_Itr = l_Languages.begin();
			l_Itr != l_Languages.end();
			l_Itr++)
	{
		l_iTempAdding = (INT64)(((REAL32)l_Itr->second / (REAL32)l_iPop) * (REAL32)in_iNbPopulation);
		l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
		l_iTempLeftToAdd -= l_iTempAdding;
	}
	if(l_iTempLeftToAdd != 0)
	{		
		gassert(l_iTempLeftToAdd >= 0,"GDataControlLayer::AddPopulationToRegion(): Should be bigger than 0");
		l_pRegion->GetLanguages(l_Languages);
		for(GLanguageList::iterator l_Itr = l_Languages.begin();
				l_Itr != l_Languages.end();
				l_Itr++)
			{
				l_iTempAdding = l_iTempLeftToAdd;
				l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
				l_iTempLeftToAdd -= l_iTempAdding;
				if(l_iTempLeftToAdd == 0)
					break;
			}
			if(l_iTempLeftToAdd != 0)
			{
				g_Joshua.Log(L"Error while adding population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}

	}

	//Now add production, for that region;
	REAL32 l_fPopRatio = ((REAL32)in_iNbPopulation / (REAL32)max(l_iPop,1)) + 1.f;
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		//If the country is gaining people, it will gain production
		l_pRegion->ResourceProduction((EResources::Enum)i,l_pRegion->ResourceProduction((EResources::Enum)i)*l_fPopRatio);
	}
	return true;
}

bool GDataControlLayer::AddPopulationAfterDisbandInfantry(UINT32 in_iCountryID, INT64 in_iNbPopulation)
{

	INT64 l_iTotalPopulation = g_ServerDAL.CountryData(in_iCountryID)->Population();

	//Adjust the manpower available
	g_ServerDAL.CountryData(in_iCountryID)->MiliManpowerAvailable( g_ServerDAL.CountryData(in_iCountryID)->MiliManpowerAvailable() + in_iNbPopulation );

	const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	for(set<UINT32>::const_iterator it = l_vRegions.begin();
		 it != l_vRegions.end(); it++)
	{
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
		INT64 l_iPop = l_pRegion->Population();
		INT64 l_iPeopleToAdd = 0;
		if(l_iTotalPopulation > 0)
			l_iPeopleToAdd = (INT64)(((REAL32)l_pRegion->Population() / (REAL32)l_iTotalPopulation) * (REAL32)in_iNbPopulation);
		l_pRegion->Population1565(l_pRegion->Population1565() + l_iPeopleToAdd);		
		//Add that population to religions & languages

		INT64 l_iTempAdding = 0;
		
		//Religions
		INT64 l_iTempTotalAdd = l_iPeopleToAdd;		
		GReligionList l_Religions;
		l_pRegion->GetReligions(l_Religions);	
		for(GReligionList::iterator l_Itr = l_Religions.begin();
				l_Itr != l_Religions.end();
				l_Itr++)
		{
			l_iTempAdding = (INT64)(((REAL32)l_Itr->second / (REAL32)max(l_iPop,1)) * (REAL32)l_iPeopleToAdd);
			l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
			l_iTempTotalAdd -= l_iTempAdding;
		}
		if(l_iTempTotalAdd != 0)
		{
			l_pRegion->GetReligions(l_Religions);	
			for(GReligionList::iterator l_Itr = l_Religions.begin();
				l_Itr != l_Religions.end();
				l_Itr++)
			{
				l_iTempAdding = l_iTempTotalAdd;
				l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
				l_iTempTotalAdd -= l_iTempAdding;
				if(l_iTempTotalAdd == 0)
					break;
			}
			if(l_iTempTotalAdd != 0)
			{
				g_Joshua.Log(L"GDataControlLayer::AddPopulationAfterDisbandInfantry: Error while adding population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}
		}

		//Languages
		l_iTempTotalAdd = l_iPeopleToAdd;
		GLanguageList l_Languages;
		l_pRegion->GetLanguages(l_Languages);	
		for(GLanguageList::iterator l_Itr = l_Languages.begin();
				l_Itr != l_Languages.end();
				l_Itr++)
		{
			l_iTempAdding = (INT64)(((REAL32)l_Itr->second / (REAL32)max(l_iPop,1)) * (REAL32)l_iPeopleToAdd);
			l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
			l_iTempTotalAdd -= l_iTempAdding;
		}
		if(l_iTempTotalAdd != 0)
		{
			l_pRegion->GetLanguages(l_Languages);	
			for(GLanguageList::iterator l_Itr = l_Languages.begin();
				l_Itr != l_Languages.end();
				l_Itr++)
			{
				l_iTempAdding = l_iTempTotalAdd;
				l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second + l_iTempAdding);
				l_iTempTotalAdd -= l_iTempAdding;
				if(l_iTempTotalAdd == 0)
					break;
			}
			if(l_iTempTotalAdd != 0)
			{
				g_Joshua.Log(L"GDataControlLayer::AddPopulationAfterDisbandInfantry: Error while adding population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}
		}

		if(l_iPop > 0)
		{
			//Now remove production, and demand, for that region;
			REAL32 l_fPopRatio = ((REAL32)l_iPeopleToAdd / (REAL32)l_iPop) + 1.f;
			for(UINT32 i=0; i<EResources::ItemCount; i++)
			{
				//Adjust demand for that region
				//l_pRegion->ResourceDemand((EResources::Enum)i,l_pRegion->ResourceDemand((EResources::Enum)i)*l_fPopRatio);
				//If the country is losing people, it will lose production
				l_pRegion->ResourceProduction((EResources::Enum)i,l_pRegion->ResourceProduction((EResources::Enum)i)*l_fPopRatio);
			}
		}		
	}

	g_ServerDAL.AddCountryToSynchronize(in_iCountryID);

	return true;
}

INT64 GDataControlLayer::RemovePopulationFromCountry(UINT32 in_iCountryID, INT64 in_iNbPopulation, bool in_bEnroll)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(in_bEnroll && l_pCountryData->MiliManpowerAvailable() < in_iNbPopulation)
		return 0;

	INT64 l_iTotalPopulation = l_pCountryData->Population();
	if(in_iNbPopulation > l_iTotalPopulation)
		return 0;

	if(l_iTotalPopulation == 0)
		return 0;	

	//Go through the regions, and remove population
    INT64 l_iRemovedPeople = 0;
	const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	for(set<UINT32>::const_iterator it = l_vRegions.begin();
		 it != l_vRegions.end(); it++)
	{
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
		INT64 l_iPeopleRemovedFromThatRegion = 0;
		if(l_iTotalPopulation > 0)
			l_iPeopleRemovedFromThatRegion = (INT64)(((REAL32)l_pRegion->Population() / (REAL32)l_iTotalPopulation) * (REAL32)in_iNbPopulation);
		RemovePopulationFromRegion(*it,l_iPeopleRemovedFromThatRegion,in_bEnroll);
        l_iRemovedPeople += l_iPeopleRemovedFromThatRegion;
	}

	g_ServerDAL.AddCountryToSynchronize(in_iCountryID);

	return l_iRemovedPeople;
}

bool GDataControlLayer::RemovePopulationFromRegion(UINT32 in_iRegionID, INT64 in_iNbPopulation, bool in_bEnroll)
{	  
	GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_pRegion->OwnerId());

	INT64 l_iPop15 = l_pRegion->Population15();
	INT64 l_iPop1565 = l_pRegion->Population1565();
	INT64 l_iPop65 = l_pRegion->Population65();
	INT64 l_iPop = l_pRegion->Population();
   if(l_iPop == 0)
      return false;

	gassert(in_iNbPopulation <= l_iPop,"GDataControlLayer::RemovePopulationFromRegion: Removing more population than there is.");

	//Take care of military manpower available portion
	INT64 l_iNbOfEnrollToRemove = 0;
	if(in_bEnroll)
		l_iNbOfEnrollToRemove = in_iNbPopulation;
	else
	{
		if(l_iPop1565 == 1 && in_iNbPopulation > 0)
			l_iNbOfEnrollToRemove = 1;
		else
			l_iNbOfEnrollToRemove =	(INT64)(((REAL32)in_iNbPopulation * ((REAL32)l_iPop1565/(REAL32)l_iPop)) * SP2::c_fPourcPopulationAbleToEnroll);
	}

	l_pCountryData->MiliManpowerAvailable(l_pCountryData->MiliManpowerAvailable() - l_iNbOfEnrollToRemove);
    GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": " +
                  L"Removing " + GString::FormatNumber(static_cast<REAL64>(in_iNbPopulation), L",", L".", L"", L"", 3, 0) + L" population; " +
                  L"new manpower available " + GString::FormatNumber(static_cast<REAL64>(l_pCountryData->MiliManpowerAvailable()), L",", L".", L"", L"", 3, 0),
                  EDZLogCat::ObtainUnits,
                  __FUNCTION__, __LINE__);

	//military manpower available portion done

	/*
		We can now start to remove some population from group ages
	*/
	INT64 l_iTempCasualties = in_iNbPopulation;		
	INT64 l_iTempRemoval = 0;
	//remove equally from age 15
	if(!in_bEnroll)
	{
		l_iTempRemoval = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop15/(REAL32)l_iPop));
		if(l_iTempRemoval > l_iPop15)
			l_iTempRemoval = l_iPop15;
		l_iPop15 -= l_iTempRemoval;
		l_iTempCasualties -= l_iTempRemoval;
	}
	//remove equally from age 15-65
	if(!in_bEnroll)
		l_iTempRemoval = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop1565/(REAL32)l_iPop));		
	else
		l_iTempRemoval = in_iNbPopulation;
	if(l_iTempRemoval > l_iPop1565)
		l_iTempRemoval = l_iPop1565;
	l_iPop1565 -= l_iTempRemoval;
	l_iTempCasualties -= l_iTempRemoval;
	//remove equally from age 65
	if(!in_bEnroll)
	{
		l_iTempRemoval = (INT64)((REAL32)in_iNbPopulation * ((REAL32)l_iPop65/(REAL32)l_iPop));
		if(l_iTempRemoval > l_iPop65)
			l_iTempRemoval = l_iPop65;
		l_iPop65 -= l_iTempRemoval;
		l_iTempCasualties -= l_iTempRemoval;
	}
	//Kill the remaining
	while(l_iTempCasualties > 0)
	{
		if(l_iPop15 != 0)
		{
			l_iPop15--;
			l_iTempCasualties--;
			continue;
		}
		if(l_iPop1565 != 0)
		{
			l_iPop1565--;
			l_iTempCasualties--;
			continue;
		}
		if(l_iPop65 != 0)
		{
			l_iPop65--;
			l_iTempCasualties--;
			continue;
		}
		g_Joshua.Log(L"Error while removing population. Should never happen!",MSGTYPE_FATAL_ERROR);
		return false;
	}

	l_pRegion->Population15(l_iPop15);
	l_pRegion->Population1565(l_iPop1565);
	l_pRegion->Population65(l_iPop65);

	/*
		Now remove people from religions
	*/
	l_iTempCasualties = in_iNbPopulation;
	GReligionList l_Religions;
	l_pRegion->GetReligions(l_Religions);	
	for(GReligionList::iterator l_Itr = l_Religions.begin();
			l_Itr != l_Religions.end();
			l_Itr++)
	{
		l_iTempRemoval = (INT64)(((REAL32)l_Itr->second / (REAL32)max(l_iPop,1)) * (REAL32)in_iNbPopulation);
		if(l_iTempRemoval > l_Itr->second)
			l_iTempRemoval = l_Itr->second;
		l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second - l_iTempRemoval);
		l_iTempCasualties -= l_iTempRemoval;
	}
	if(l_iTempCasualties != 0)
	{		
		l_pRegion->GetReligions(l_Religions);	
		for(GReligionList::iterator l_Itr = l_Religions.begin();
				l_Itr != l_Religions.end();
				l_Itr++)
			{
				l_iTempRemoval = l_iTempCasualties;
				if(l_iTempRemoval > l_Itr->second)
					l_iTempRemoval = l_Itr->second;
				l_pRegion->ReligionChangePopulation(l_Itr->first,l_Itr->second - l_iTempRemoval);
				l_iTempCasualties -= l_iTempRemoval;
				if(l_iTempCasualties == 0)
					break;
			}
			if(l_iTempCasualties != 0)
			{
				g_Joshua.Log(L"Error while removing population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}

	}
	/*
		Now remove people from languages
	*/
	l_iTempCasualties = in_iNbPopulation;
	GLanguageList l_Languages;
	l_pRegion->GetLanguages(l_Languages);
	for(GLanguageList::iterator l_Itr = l_Languages.begin();
			l_Itr != l_Languages.end();
			l_Itr++)
	{
		l_iTempRemoval = (INT64)(((REAL32)l_Itr->second / (REAL32)max(l_iPop,1)) * (REAL32)in_iNbPopulation);
		if(l_iTempRemoval > l_Itr->second)
			l_iTempRemoval = l_Itr->second;
		l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second - l_iTempRemoval);
		l_iTempCasualties -= l_iTempRemoval;
	}
	if(l_iTempCasualties != 0)
	{		
		l_pRegion->GetLanguages(l_Languages);
		for(GLanguageList::iterator l_Itr = l_Languages.begin();
				l_Itr != l_Languages.end();
				l_Itr++)
			{
				l_iTempRemoval = l_iTempCasualties;
				if(l_iTempRemoval > l_Itr->second)
					l_iTempRemoval = l_Itr->second;
				l_pRegion->LanguageChangePopulation(l_Itr->first,l_Itr->second - l_iTempRemoval);
				l_iTempCasualties -= l_iTempRemoval;
				if(l_iTempCasualties == 0)
					break;
			}
			if(l_iTempCasualties != 0)
			{
				g_Joshua.Log(L"Error while removing population. Should never happen!",MSGTYPE_FATAL_ERROR);
				return false;
			}

	}

	//Now remove production, and demand, for that region;
	REAL32 l_fPopRatio = -((REAL32)in_iNbPopulation / (REAL32)max(l_iPop,1)) + 1.f;
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		//Adjust demand for that region
		//l_pRegion->ResourceDemand((EResources::Enum)i,l_pRegion->ResourceDemand((EResources::Enum)i)*l_fPopRatio);
		//If the country is losing people, it will lose production
		l_pRegion->ResourceProduction((EResources::Enum)i,l_pRegion->ResourceProduction((EResources::Enum)i)*l_fPopRatio);
	}
	return true;
}

void GDataControlLayer::ChangeCountryInternalLaw(UINT32 in_iCountryID, EInternalLaws::Enum in_eLaw, bool in_bStatus)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	
	//If the law is already the same, return immediately
	if(l_pCountryData->InternalLaw(in_eLaw) == in_bStatus)
		return;

	l_pCountryData->InternalLaw(in_eLaw,in_bStatus);

	vector<INT32> l_vStrings;
	News::GInfo l_News;
   l_News.m_iCountryA = in_iCountryID;

	if(in_bStatus)
	{
		if(in_eLaw == EInternalLaws::NumberOfChildren)
			l_News.m_eType = News::EType::NbChildrenIllimited;
		else
			l_News.m_eType = News::EType::LegalizedLaw;
	}
	else
	{
		if(in_eLaw == EInternalLaws::NumberOfChildren)
			l_News.m_eType = News::EType::NbChildrenLimited;
		else
			l_News.m_eType = News::EType::IllegalizedLaw;
	}
   
   l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
   l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);   

	//Different effects for different laws
   UINT32 l_iStrID = 0;
	switch(in_eLaw)
	{
	case EInternalLaws::Abortion:		
		ExecuteInternalLawAbortion(in_iCountryID,in_bStatus);
      l_iStrID = EStrId::LawAbortion;
		break;
	case EInternalLaws::ChildLabor:
		ExecuteInternalLawChildLabor(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawChildLabor;
		break;
	case EInternalLaws::Contraception:
		ExecuteInternalLawContraception(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawContraception;
		break;
	case EInternalLaws::FreedomOfDemonstration:
		ExecuteInternalLawFreedomOfDemonstration(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawFreedomOfDemonstration;
		break;
	case EInternalLaws::FreedomOfSpeech:
		ExecuteInternalLawFreedomOfSpeech(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawFreedomSpeech;
		break;
	case EInternalLaws::NumberOfChildren:		
		ExecuteInternalLawNumberOfChildren(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawNumberOfChildren;
		break;
	case EInternalLaws::Polygamy:
		ExecuteInternalLawPolygamy(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawPolygamy;
		break;
	case EInternalLaws::SameSexMarriage:
		ExecuteInternalLawSameSexMarriage(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawSameSexMarriage;
		break;
	case EInternalLaws::WomenSuffrage:
		ExecuteInternalLawWomenSuffrage(in_iCountryID,in_bStatus);
		l_iStrID = EStrId::LawWomenSuffrage;
		break;
	default:
		gassert(false,"GDataControlLayer::ChangeCountryInternalLaw: Invalid Internal law.");
		return;
	}
   l_vStrings.push_back(l_iStrID);
	l_News.m_vStrings = l_vStrings;
	SendNews(l_News);

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   EHistoryMarkerType::ModifyLaw, 
                                   (REAL32) l_iStrID);
}

void GDataControlLayer::ExecuteInternalLawWomenSuffrage(UINT32 in_iCountryID,bool in_bStatus)
{
	RemoveRelationsFromInternalLaws(in_iCountryID,in_bStatus,EInternalLaws::WomenSuffrage);
	REAL32 l_fChange = 0.f;

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionDoubleMinus,
														SP2::c_fAIActionMinus);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
	{
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
		l_fChange = -SP2::c_fLawIdeologyWomenSuffrage;
	}
	else
	{
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
		l_fChange = SP2::c_fLawIdeologyWomenSuffrage;
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);	
}
void GDataControlLayer::ExecuteInternalLawSameSexMarriage(UINT32 in_iCountryID,bool in_bStatus)
{
	RemoveRelationsFromInternalLaws(in_iCountryID,in_bStatus,EInternalLaws::SameSexMarriage);
	REAL32 l_fChange = 0.f;
	
	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionDoubleMinus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
	{
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
		l_fChange = -SP2::c_fLawIdeologySameSexMarriage;
	}
	else
	{
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
		l_fChange = SP2::c_fLawIdeologySameSexMarriage;
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);

}
void GDataControlLayer::ExecuteInternalLawPolygamy(UINT32 in_iCountryID,bool in_bStatus)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fChange = 0.f;
	if(in_bStatus)
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * SP2::c_fPolygamyBirthRateChange );
		l_fChange = -SP2::c_fLawIdeologyPolygamy;
	}
	else
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * (1.f/SP2::c_fPolygamyBirthRateChange) );
		l_fChange = SP2::c_fLawIdeologyPolygamy;
	}

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
	else
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);

	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}
void GDataControlLayer::ExecuteInternalLawNumberOfChildren(UINT32 in_iCountryID,bool in_bStatus)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fChange = 0.f;
	if(in_bStatus)
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * (1.f/SP2::c_fNbChildrenBirthRateChange) );
		l_fChange = -SP2::c_fLawIdeologyNbOfChildren;
	}
	else
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * SP2::c_fNbChildrenBirthRateChange );
		l_fChange = SP2::c_fLawIdeologyNbOfChildren;
	}
	
	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
	else
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);

	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}
void GDataControlLayer::ExecuteInternalLawFreedomOfSpeech(UINT32 in_iCountryID,bool in_bStatus)
{
	RemoveRelationsFromInternalLaws(in_iCountryID,in_bStatus,EInternalLaws::FreedomOfSpeech);
	REAL32 l_fChange = 0.f;

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionDoublePlus,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionMinus);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
	{
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
		l_fChange = -SP2::c_fLawIdeologyFreedomOfSpeech;
	}
	else
	{
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
		l_fChange = SP2::c_fLawIdeologyFreedomOfSpeech;
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}
void GDataControlLayer::ExecuteInternalLawFreedomOfDemonstration(UINT32 in_iCountryID,bool in_bStatus)
{
	RemoveRelationsFromInternalLaws(in_iCountryID,in_bStatus,EInternalLaws::FreedomOfDemonstration);
	REAL32 l_fChange = 0.f;

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionDoublePlus,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionDoubleMinus);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
	{
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
		l_fChange = -SP2::c_fLawIdeologyFreedomOfDemonstration;
	}
	else
	{
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
		l_fChange = SP2::c_fLawIdeologyFreedomOfDemonstration;
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}
void GDataControlLayer::ExecuteInternalLawContraception(UINT32 in_iCountryID,bool in_bStatus)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fChange = 0.f;
	if(in_bStatus)
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * (1.f/SP2::c_fContraceptionBirthRateChange) );
		l_fChange = -SP2::c_fLawIdeologyContraception;
	}
	else
	{		
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * SP2::c_fContraceptionBirthRateChange );
		l_fChange = SP2::c_fLawIdeologyContraception;
	}
	
	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
	else
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
	
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}
void GDataControlLayer::ExecuteInternalLawChildLabor(UINT32 in_iCountryID,bool in_bStatus)
{
	RemoveRelationsFromInternalLaws(in_iCountryID,in_bStatus,EInternalLaws::ChildLabor);

	REAL32 l_fChange = 0.f;

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
	{
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
		l_fChange = SP2::c_fLawIdeologyChildLabor;
	}
	else
	{
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
		l_fChange = -SP2::c_fLawIdeologyChildLabor;
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);
}

void GDataControlLayer::ExecuteInternalLawAbortion(UINT32 in_iCountryID,bool in_bStatus)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fChange = 0.f;
	if(in_bStatus)
	{
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * SP2::c_fAbortionBirthRateChange );
		l_fChange = -SP2::c_fLawIdeologyAbortion;
	}
	else
	{		
		l_pCountryData->BirthRate( l_pCountryData->BirthRate() * (1.f/SP2::c_fAbortionBirthRateChange) );	
		l_fChange = SP2::c_fLawIdeologyAbortion;
	}

	l_pCountryData->CountryPoliticalIdeology(
		l_pCountryData->CountryPoliticalIdeology() + l_fChange);

	REAL32 l_fApprovalRating = g_ServerDCL.CalculateProbabilityOfAction(
														g_ServerDAL.CountryData(in_iCountryID),
														SP2::c_fAIActionPlus,
														SP2::c_fAIActionNeutral,
														SP2::c_fAIActionMinus,
														SP2::c_fAIActionNeutral);
	REAL32 l_fApprovalDifference = l_fApprovalRating - 0.5f;
	if(in_bStatus)
		ChangeCountryApproval(in_iCountryID,l_fApprovalDifference);
	else
		ChangeCountryApproval(in_iCountryID,-l_fApprovalDifference);
}

void GDataControlLayer::RemoveRelationsFromInternalLaws(UINT32 in_iCountryID, bool in_bStatus, EInternalLaws::Enum in_Law)
{
	REAL32 l_fChange = 0.f;
	switch(in_Law)
	{
	case EInternalLaws::WomenSuffrage:
		l_fChange = c_fRelationsChangeWomenSuffrage; break;
	case EInternalLaws::ChildLabor:
		l_fChange = c_fRelationsChangeChildLabour; break;
	case EInternalLaws::SameSexMarriage:
		l_fChange = c_fRelationsChangeSameSexMarriage; break;
	case EInternalLaws::FreedomOfDemonstration:
		l_fChange = c_fRelationsChangeFreedomOfDemonstration; break;
	case EInternalLaws::FreedomOfSpeech:
		l_fChange = c_fRelationsChangeFreedomOfSpeech; break;
	}
	//Change relations with other countries
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<=l_iNbCountry; i++)
	{
		if(i == in_iCountryID || !g_ServerDAL.CountryIsValid(i))
			continue;
		if(g_ServerDAL.CountryData(i)->InternalLaw(in_Law) == in_bStatus)
      {
			g_ServerDAL.RelationBetweenCountries(in_iCountryID,i,
				g_ServerDAL.RelationBetweenCountries(in_iCountryID,i) + l_fChange);
      }
		else
      {
			g_ServerDAL.RelationBetweenCountries(in_iCountryID,i,
				g_ServerDAL.RelationBetweenCountries(in_iCountryID,i) - l_fChange);
      }
	}
}

void GDataControlLayer::IncreaseProduction(UINT32 in_iCountryID, EResources::Enum in_eResource, REAL64 in_fPourcOfIncrease)
{
	REAL64 l_fCost = g_ServerDCL.IncreaseProductionApproxCost(in_fPourcOfIncrease,g_ServerDAL.CountryData(in_iCountryID)->ResourceProduction(in_eResource));

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);

	//REAL64 l_fTotalResourceProductionAdded = g_ServerDAL.CountryData(in_iCountryID)->ResourceProduction(in_eResource) * in_fPourcOfIncrease; 
	//Adjust the resource production for that specified resource
	GRegion* l_pRegion;
	const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	REAL64 l_fProductionChanges = 1.f + in_fPourcOfIncrease;
   REAL64 l_fIncrease = 0;
	for(set<UINT32>::const_iterator l_Itr = l_PoliticalRegions.begin();
		 l_Itr != l_PoliticalRegions.end(); l_Itr++)
	{
		l_pRegion = g_ServerDAL.GetGRegion(*l_Itr);
      REAL64 l_fResProd = l_pRegion->ResourceProduction(in_eResource);
		l_pRegion->ResourceProduction(in_eResource, l_fResProd * l_fProductionChanges);
      l_fIncrease += l_fResProd * in_fPourcOfIncrease;
	}

	TakeMoney(in_iCountryID,l_fCost);
	l_pCountryData->SynchronizeWithRegions();

	//Calculate the new market share for every countries
	const REAL64* l_pWorldProductionFinal = g_ServerDAL.MarketProduction();	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		for(UINT32 k=0; k< EResources::ItemCount; k++)
		{
			l_pCountryData->ResourceMarketShare((EResources::Enum)k, min(1.f,(REAL32)(l_pCountryData->ResourceProduction((EResources::Enum)k) / (l_pWorldProductionFinal[k] + 1.f))) ); 
		}		
	}

   g_ServerDAL.AddHistoricalMarker(in_iCountryID, 
                                   EHistoryMarkerType::IncreaseResourceProduction, 
                                   (REAL32) l_fIncrease);

	return;
}

/*!
 * Declares war to all military units that are in the combat 
 * treshold range at the given position
 * @param in_iCountryID: ID of the country that is declaring war
 * @param in_Position: Position to check for neutral units
 * @param in_fSquareRange: (Range*Range) in which we want to check.
 **/
void GDataControlLayer::DeclareWarToNeutralUnitsWithinRange(UINT32 in_iCountryID,
                                                            const GVector2D<REAL32>& in_Position,
                                                            REAL32 in_fSquareRange)
{
   //List all the countries that are neutral with us
   set<UINT32> l_CountriesToDeclareWar;
   for(UINT32 i = 1 ; i <= g_ServerDAL.NbCountry() ; i++)
   {
      if(i == in_iCountryID)
         continue;

      for(set<UINT32>::const_iterator l_CountryGroupsIt = g_Joshua.UnitManager().CountryUnitGroups(i).begin();
          l_CountryGroupsIt != g_Joshua.UnitManager().CountryUnitGroups(i).end();
          l_CountryGroupsIt++)
      {
         SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_CountryGroupsIt);
         if(in_Position.DistanceSquared(l_pGroup->Position()) <= in_fSquareRange )
         {
            l_CountriesToDeclareWar.insert(i);
         }
      }      
   }

   //For each unit of those countries, check if it is in the range
   //If in range, declare war to that country

   //Declare war to the countries
   for(set<UINT32>::const_iterator l_WarDeclarationIt = l_CountriesToDeclareWar.begin();
      l_WarDeclarationIt != l_CountriesToDeclareWar.end();
      l_WarDeclarationIt++)
   {
		set<UINT32> l_Attackers;
		l_Attackers.insert(in_iCountryID);
      g_ServerDCL.DeclareNewWar(l_Attackers,in_iCountryID,*l_WarDeclarationIt);
   }
}

/*!
 * Calculate the missile Rearming delay in days
 * @return REAL32: Missile Rearming Delay in real time days (time before unit gets new missiles)
 **/
REAL32 GDataControlLayer::MissileRearmingDelay(const SP2::GUnitType* in_pUnitType) const
{
   return (REAL32) c_fSP2NormalTimeSpeed * (atanf(0.002f * in_pUnitType->MissileRearmingDelay() ) * 300.0f + 5.f);
}

REAL64 GDataControlLayer::CountryYearlyUnitProduction(UINT32 in_iCountryID) const
{
	return m_UnitProductor.CountryMilitaryProduction(in_iCountryID);
}	

bool GDataControlLayer::CanBuildUnitDesign(UINT32 in_iCountryID, EUnitCategory::Enum in_eCategory) const
{
	GResearchInformation* l_pInfo = (GResearchInformation*)g_ServerDAL.CountryData(in_iCountryID)->ResearchInfo();
	switch(in_eCategory)
	{
	case EUnitCategory::Air:
	case EUnitCategory::Naval:
		{
         for(UINT32 i = 0  ; i < EUnitDesignCharacteristics::ItemCount ; i++)
         {
            if(l_pInfo->m_fMaxValues[in_eCategory][i] < 1.f)
            {
               return false;
            }
         }
         return true;
		}
	case EUnitCategory::Ground:
		{
         for(UINT32 i = 0  ; i < EUnitDesignCharacteristics::ItemCount ; i++)
         {
            //Skip over the 2 non used characteristics
            if(i == EUnitDesignCharacteristics::Stealth || i == EUnitDesignCharacteristics::CounterMesures)
               continue;
            if(l_pInfo->m_fMaxValues[EUnitCategory::Air][i] < 1.f)
            {
               return false;
            }
         }
         return true;
		}
	case EUnitCategory::Nuclear:
		{
         if(l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     < 1.f || 
            l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  < 1.f || 
				l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      < 1.f)
				return false;
			else
				return true;
		}
	default:
		gassert(false,"GDataControlLayer::CanBuildUnitDesign(): Invalid Unit Category");
	}
	return false;
}

void GDataControlLayer::FindNavalWaypointsAdjacentToCountry(UINT32 in_iCountryID, vector<Hector::GVector2D<REAL32> >& out_Positions) const
{
	out_Positions.clear();
	const set<UINT32>& l_vMilitary = g_ServerDAL.CountryMilitaryControl(in_iCountryID);

	Hector::GVector2D<REAL32> l_Location;
	for(set<UINT32>::const_iterator it = l_vMilitary.begin(); 
		it != l_vMilitary.end(); it++)
	{
		if(m_UnitMover.FindNavalWaypointAdjacentToRegion(*it,l_Location))
			out_Positions.push_back(l_Location);
	}
}

REAL32 GDataControlLayer::CalculateProbabilityOfAction(const GCountryData* in_pCountryData, 
																	REAL32 in_fIndividualism, 
																	REAL32 in_fHierarchicalDistance, 
																	REAL32 in_fMaleFemale, 
																	REAL32 in_fIncertitudeControl)
{
	UINT32 l_iNbParameters = 0;
	if(in_fIndividualism != SP2::c_fAIActionNeutral)
		l_iNbParameters++;
	if(in_fHierarchicalDistance != SP2::c_fAIActionNeutral)
		l_iNbParameters++;
	if(in_fMaleFemale != SP2::c_fAIActionNeutral)
		l_iNbParameters++;
	if(in_fIncertitudeControl != SP2::c_fAIActionNeutral)
		l_iNbParameters++;
	if(!l_iNbParameters)
		return 0.5f;

	REAL32 l_fProbTotal = 0.f;

	REAL32 l_fIndividualismRating = in_pCountryData->IndividualismCollectivism();
	REAL32 l_fHierarchicalDistanceRating = in_pCountryData->HierarchicalDistance();
	REAL32 l_fMaleFemaleRating = in_pCountryData->MaleFemale();
	REAL32 l_fIncertitudeControlRating = 1.f - in_pCountryData->GvtStability();

	if(in_fIndividualism < SP2::c_fAIActionNeutral)
	{
		if(l_fIndividualismRating <= in_fIndividualism)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((l_fIndividualismRating - in_fIndividualism)/(1.f - in_fIndividualism))) / (REAL32)l_iNbParameters;
	}
	else if(in_fIndividualism > SP2::c_fAIActionNeutral)
	{
		if(l_fIndividualismRating >= in_fIndividualism)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((in_fIndividualism - l_fIndividualismRating)/in_fIndividualism)) / (REAL32)l_iNbParameters;
	}
	if(in_fHierarchicalDistance < SP2::c_fAIActionNeutral)
	{
		if(l_fHierarchicalDistanceRating <= in_fHierarchicalDistance)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((l_fHierarchicalDistanceRating - in_fHierarchicalDistance)/(1.f - in_fHierarchicalDistance))) / (REAL32)l_iNbParameters;
	}
	else if(in_fHierarchicalDistance > SP2::c_fAIActionNeutral)
	{
		if(l_fHierarchicalDistanceRating >= in_fHierarchicalDistance)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((in_fHierarchicalDistance - l_fHierarchicalDistanceRating)/in_fHierarchicalDistance)) / (REAL32)l_iNbParameters;
	}
	if(in_fMaleFemale < SP2::c_fAIActionNeutral)
	{
		if(l_fMaleFemaleRating <= in_fMaleFemale)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((l_fMaleFemaleRating - in_fMaleFemale)/(1.f - in_fMaleFemale))) / (REAL32)l_iNbParameters;
	}
	else if(in_fMaleFemale > SP2::c_fAIActionNeutral)
	{
		if(l_fMaleFemaleRating >= in_fMaleFemale)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((in_fMaleFemale - l_fMaleFemaleRating)/in_fMaleFemale)) / (REAL32)l_iNbParameters;
	}
	if(in_fIncertitudeControl < SP2::c_fAIActionNeutral)
	{
		if(l_fIncertitudeControlRating <= in_fIncertitudeControl)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((l_fIncertitudeControlRating - in_fIncertitudeControl)/(1.f - in_fIncertitudeControl))) / (REAL32)l_iNbParameters;
	}
	else if(in_fIncertitudeControl > SP2::c_fAIActionNeutral)
	{
		if(l_fIncertitudeControlRating >= in_fIncertitudeControl)
			l_fProbTotal += 1.f / (REAL32)l_iNbParameters;
		else
			l_fProbTotal += (1.f - ((in_fIncertitudeControl - l_fIncertitudeControlRating)/in_fIncertitudeControl)) / (REAL32)l_iNbParameters;
	}
	return l_fProbTotal;	
}

void GDataControlLayer::ChangeAllUnitGroupStatus(UINT32 in_iCountry, EMilitaryStatus::Enum in_NewStatus)
{
	if(!(in_NewStatus == EMilitaryStatus::Fortified || 
		  in_NewStatus == EMilitaryStatus::Ready || 
		  in_NewStatus == EMilitaryStatus::Parked))
		return;

	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountry);
	for(set<UINT32>::const_iterator it = l_UnitGroups.begin();
		 it != l_UnitGroups.end(); it++)
	{
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		if(l_pUnitGroup->Status() == EMilitaryStatus::Fortified ||
			l_pUnitGroup->Status() == EMilitaryStatus::Ready ||
			l_pUnitGroup->Status() == EMilitaryStatus::Parked)
		{
			m_UnitMover.ChangeUnitState((SDK::Combat::GUnitGroup*)l_pUnitGroup,in_NewStatus);
		}
	}
}

/*!
 * Debug method to log military data incoherences
 **/
void GDataControlLayer::LogMilitaryDataIncoherences(bool l_bFixIncoherences)
{

   FILE* l_pFile;
   fopen_s(&l_pFile,"mildataincoherences.log","a");

   const REAL32** l_pGunEfficiencyTable      = g_ServerDAL.GunCombatEfficiencyTable();
   const REAL32** l_pMissileEfficiencyTable  = g_ServerDAL.MissileCombatEfficiencyTable();

   //For every unit design
   for(stdext::hash_map<UINT32, SDK::Combat::Design::GUnit*>::const_iterator l_DesignIt = g_Joshua.UnitManager().UnitDesigns().begin();
      l_DesignIt != g_Joshua.UnitManager().UnitDesigns().end();
      l_DesignIt++)
   {
      SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_DesignIt->second;
      SP2::GUnitType*   l_pType   = l_pDesign->Type();
      REAL32 l_fMissileRange = -1.f;
      REAL32 l_fGunRange     = -1.f;
      if(l_pDesign->MissileRange())
      {
         l_fMissileRange = l_pType->MissileRangeByLevel()[l_pDesign->MissileRange()-1];
      }
      if(l_pDesign->GunRange())
      {
         l_fGunRange = l_pType->GunRangeByLevel()[l_pDesign->GunRange()-1];
      }

      //Check efficiency against all other unit types
      for(vector<GUnitType*>::const_iterator l_It = g_ServerDAL.UnitTypes().begin();
         l_It != g_ServerDAL.UnitTypes().end();
         l_It++)
      {
         //Check if the target can be hit using Missiles
         REAL32 l_fMissileEfficiency = l_pMissileEfficiencyTable[l_pType->Id()-1][(*l_It)->Id()-1];
         //Check if the target can be hit using Gun
         REAL32 l_fGunEfficiency     = l_pGunEfficiencyTable[l_pType->Id()-1][(*l_It)->Id()-1];

         bool l_bGunRangeProblem = false;
         bool l_bMissileRangeProblem = false;
         if(l_fGunRange <= 0 && l_fGunEfficiency > 0)
         {
            l_bGunRangeProblem = true;
         }
         if(l_fMissileRange <= 0 && l_fMissileEfficiency > 0)
         {
            l_bMissileRangeProblem = true;
         }
         //Write to the file
         {
            
            if(l_bGunRangeProblem || l_bMissileRangeProblem)
            {
               GString l_sMsg = L"";            
               if(l_bGunRangeProblem)
                  l_sMsg += L" Gun efficiency but no range ";
               if(l_bMissileRangeProblem)
                  l_sMsg += L" Missile efficiency but no range ";
               l_sMsg += GString("\r\n");
               l_sMsg += "\tAttacker Design: " ;
               l_sMsg += GString(GString(l_pDesign->Id()) + GString(L" ") + l_pDesign->Name());
               l_sMsg += " Type: ";
               l_sMsg += l_pType->Name();
               l_sMsg += " Target: ";
               l_sMsg += (*l_It)->Name();
               l_sMsg += "\r\n\r\n";
               fputws(l_sMsg.c_str(),l_pFile);
            }
         }

         //If must fix incoherences
         if(l_bFixIncoherences)
         {
            if(l_bGunRangeProblem)
            {
               if(l_pDesign->GunRange() <= 0)
               {
                  l_pDesign->GunRange(1);
               }
               else if(l_pType->GunRangeByLevel()[l_pDesign->GunRange()-1] == 0)
               {
                  REAL32* l_pGunRangeByLevel = (REAL32*)l_pType->GunRangeByLevel();
                  l_pGunRangeByLevel[l_pDesign->GunRange()-1] = 5;
               }
            }
            if(l_bMissileRangeProblem)
            {
               if(l_pDesign->MissileRange() <= 0)
               {
                  l_pDesign->MissileRange(1);
               }
               else if(l_pType->MissileRangeByLevel()[l_pDesign->MissileRange()-1] == 0)
               {
                  REAL32* l_pRangeByLevel = (REAL32*)l_pType->MissileRangeByLevel();                  
                  l_pRangeByLevel[l_pDesign->MissileRange()-1] = 5;
               }
            }
         }
      }
   }
   fclose(l_pFile);
}

/*!
 * Perform the retreat of all the units of a country
 **/
bool GDataControlLayer::RetreatCountryFromCombat(SP2::GArena* in_pArena, UINT32 in_iCountry,GVector2D<REAL32> &in_MoveToPosition)
{
   gassert(in_pArena,"GDataControlLayer::RetreatCountryFromCombat: Invalid parameter, NULL Arena pointer");
   gassert(in_iCountry <= g_ServerDAL.NbCountry(),"GDataControlLayer::RetreatCountryFromCombat: Invalid parameter (country id)");

   vector<SDK::Combat::GUnitGroup*> l_pGroupToMove;
   list<SDK::Combat::GUnitGroup*> l_FightingGroupsCopy =  in_pArena->FightingGroups();
   for(list<SDK::Combat::GUnitGroup*>::iterator l_It = l_FightingGroupsCopy.begin();
         l_It != l_FightingGroupsCopy.end();
         l_It++
      )
   {
      if( (*l_It)->OwnerId() == in_iCountry)
      {
         in_pArena->RemoveGroup(*l_It);

         //Move the group away
         l_pGroupToMove.push_back(*l_It);
      }
   }

   // Move all groups
   g_ServerDCL.UnitMover().MoveUnits(l_pGroupToMove, in_MoveToPosition);

   return true;
}

bool GDataControlLayer::RetreatCountryFromCombat(SP2::GArena* in_pArena, UINT32 in_iCountry)
{
   gassert(in_pArena,"GDataControlLayer::RetreatCountryFromCombat: Invalid parameter, NULL Arena pointer");
   gassert(in_iCountry <= g_ServerDAL.NbCountry(),"GDataControlLayer::RetreatCountryFromCombat: Invalid parameter (country id)");

   list<SDK::Combat::GUnitGroup*> l_FightingGroupsCopy =  in_pArena->FightingGroups();
   for(list<SDK::Combat::GUnitGroup*>::iterator l_It = l_FightingGroupsCopy.begin();
         l_It != l_FightingGroupsCopy.end();
         l_It++
      )
   {
      if( (*l_It)->OwnerId() == in_iCountry)
      {
         in_pArena->RemoveGroup(*l_It);

         //Move the group away
         const REAL32 l_fCombatThresholdSquare = g_SP2Server->CombatThresholdSquare();
         GVector2D<REAL32> l_fActualPosition = (*l_It)->Position();
         GVector2D<REAL32> l_fNewPosition    = l_fActualPosition;
         l_fNewPosition.x += l_fCombatThresholdSquare * 1.1f;
         l_fNewPosition.y += l_fCombatThresholdSquare * 1.1f;
         g_Joshua.UnitManager().SetGroupPosition(*l_It,l_fNewPosition);
      }
   }
   return true;
}

GGameObjectiveEvaluator& GDataControlLayer::GameObjectiveEvaluator()
{
   return m_GameObjectiveEvaluator;
}
/*!
 * Updates the objectives for the players with the newly setted objectives
 **/
void GDataControlLayer::UpdateGameObjectives()
{
   //Apply the objectives to the players

   for(SDK::GPlayers::const_iterator l_PlayersIt = g_Joshua.HumanPlayers().begin() ;
      l_PlayersIt != g_Joshua.HumanPlayers().end();
      l_PlayersIt++)
   {
      if(l_PlayersIt->second->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
      {
         g_Joshua.Log("Game objectives updated for player ");
         g_ServerDAL.PlayersObjectives(l_PlayersIt->second,g_ServerDAL.GameObjectives());
      }
   }
}

/*!
 * Execute the covert action mission for the cell (all values are contained in it)
 **/
bool GDataControlLayer::ExecuteMission(GCovertActionCell& in_Cell)
{
	bool l_bSuccess = false;
	bool l_bCaptured = false;
	bool l_bKnowAttacker = false;
	
	UINT32 l_iCellOwnerID = in_Cell.OwnerID();
	ECovertActionsMissionType::Enum l_eMissionType = in_Cell.MissionType();

	UINT32 l_iCountryTarget = in_Cell.AssignedCountry();
	UINT32 l_iCountryBeingFramed = in_Cell.CountryToFrame();

	UINT32 l_iCountryA = l_iCountryTarget;
	UINT32 l_iCountryB = 0;
	INT32 l_iRessourceOrUnitType = -1;
	News::EType::Enum l_eType = News::EType::Undefined;
	News::EType::Enum l_eTypeSuccess = News::EType::Undefined;
	News::EType::Enum l_eTypeFailed = News::EType::Undefined;
	News::EType::Enum l_eTypeFramed = News::EType::Undefined;
	
	INT32 l_iSpecificSector = -1;
	REAL32 l_fRelationLoss = 0.f;	
	REAL32 l_fDifficultyOfKnowing = 0.f;
	{		
		//Possible relation loss		
		REAL32 l_fStabilityRate = 0.f;
		switch(l_eMissionType)
		{
		case ECovertActionsMissionType::Assassination:
			l_fDifficultyOfKnowing = SP2::c_fMissionTypeAssassinationKwnowingModifier;
			l_fRelationLoss = SP2::c_fRelationLossAssassination; 
			l_eTypeSuccess = News::EType::Assassination;
			l_eTypeFailed = News::EType::AssassinationFailed;
			l_eTypeFramed = News::EType::AssassinationFramed;
			break;
		case ECovertActionsMissionType::CoupEtat:
			l_fDifficultyOfKnowing = SP2::c_fMissionTypeCoupEtatKwnowingModifier;
			l_fRelationLoss = SP2::c_fRelationLossCoupEtat; 
			l_eTypeSuccess = News::EType::CoupEtat;
			l_eTypeFailed = News::EType::CoupEtatFailed;
			l_eTypeFramed = News::EType::CoupEtatFramed;
			l_fStabilityRate = g_ServerDAL.CountryData(l_iCountryTarget)->GvtStability();
			break;
		case ECovertActionsMissionType::Espionage:
			l_fDifficultyOfKnowing = SP2::c_fMissionTypeEspionageKwnowingModifier;
			l_fRelationLoss = SP2::c_fRelationLossEspionage; 
			if(in_Cell.TargetSector() == ECovertActionsTargetSector::Military)
			{
				l_iSpecificSector = in_Cell.UnitCategory();
				l_eTypeSuccess = News::EType::EspionageMilitary;
				l_eTypeFailed = News::EType::EspionageMilitaryFailed;
				l_eTypeFramed = News::EType::EspionageMilitaryFramed;
			}
			else
			{
				l_iSpecificSector = in_Cell.ResourceType();
				l_eTypeSuccess = News::EType::EspionageCivilian;
				l_eTypeFailed = News::EType::EspionageCivilianFailed;
				l_eTypeFramed = News::EType::EspionageCivilianFramed;
			}			
			break;
		case ECovertActionsMissionType::Sabotage:
			l_fDifficultyOfKnowing = SP2::c_fMissionTypeSabotageKwnowingModifier;
			l_fRelationLoss = SP2::c_fRelationLossSabotage; 			
			if(in_Cell.TargetSector() == ECovertActionsTargetSector::Military)
			{
				l_iSpecificSector = in_Cell.UnitCategory();
				l_eTypeSuccess = News::EType::SabotageMilitary;
				l_eTypeFailed = News::EType::SabotageMilitaryFailed;
				l_eTypeFramed = News::EType::SabotageMilitaryFramed;
			}
			else
			{
				l_iSpecificSector = in_Cell.ResourceType();
				l_eTypeSuccess = News::EType::SabotageCivilian;
				l_eTypeFailed = News::EType::SabotageCivilianFailed;
				l_eTypeFramed = News::EType::SabotageCivilianFramed;
			}			
			break;
		case ECovertActionsMissionType::SocialUpheaval:
			l_fRelationLoss = SP2::c_fRelationLossSocialUpheaval; 
			l_eTypeSuccess = News::EType::SocialUpheaval;
			l_eTypeFailed = News::EType::SocialUpheavalFailed;
			l_eTypeFramed = News::EType::SocialUpheavalFramed;
			break;
		case ECovertActionsMissionType::Terrorism:
			l_fDifficultyOfKnowing = SP2::c_fMissionTypeTerrorismKwnowingModifier;
			l_fRelationLoss = SP2::c_fRelationLossTerrorism; 
			l_eTypeSuccess = News::EType::Terrorism;
			l_eTypeFailed = News::EType::TerrorismFailed;
			l_eTypeFramed = News::EType::TerrorismFramed;
			break;
		default:
			gassert(false,"GDataControlLayer::ExecuteMission(): Invalid mission type");
			return false;
		}

		bool l_bFraming = false;
		if(l_iCountryBeingFramed)
			l_bFraming = true;

		//Now calculate the success rate:
		REAL32 l_fSuccessRate = 0.f;
		if(l_iCountryTarget == l_iCellOwnerID)
		{
			if(l_iCellOwnerID != l_iCountryBeingFramed && l_iCountryBeingFramed > 0)
				l_fSuccessRate = 0.4f;
			else l_fSuccessRate = 0.8f;

		}
		else
        {
            //More covert cells in the target country should result in a better success rate
            GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_iCellOwnerID);
            const vector<GCovertActionCell>& l_vCells = l_pCountryData->CovertActionCells();
            REAL32 l_fTotalTrainingOfAdditionalCells = 0.f;
            for (vector<GCovertActionCell>::const_iterator l_CellIt = l_vCells.cbegin();
                 l_CellIt < l_vCells.cend();
                 ++l_CellIt)
            {
                if ((l_CellIt->AssignedCountry() == l_iCountryTarget) &&
                    (l_CellIt->ActualState() == ECovertActionsCellState::Active) &&
                    (l_CellIt->ID() != in_Cell.ID())) //Executing cell has already been set to Active by this point, but of course it shouldn't count as an extra cell contributing to the base success rate
                {
                    switch(l_CellIt->ExperienceLevelType())
                    {
                    case ECovertActionCellTraining::Recruit:
                        l_fTotalTrainingOfAdditionalCells += SP2::c_fCovertActionsCellsRecruitSuccessRate; break;
                    case ECovertActionCellTraining::Regular:
                        l_fTotalTrainingOfAdditionalCells += SP2::c_fCovertActionsCellsRegularSuccessRate; break;
                    case ECovertActionCellTraining::Veteran:
                        l_fTotalTrainingOfAdditionalCells += SP2::c_fCovertActionsCellsVeteranSuccessRate; break;
                    case ECovertActionCellTraining::Elite:
                        l_fTotalTrainingOfAdditionalCells += SP2::c_fCovertActionsCellsEliteSuccessRate; break;
                    default:
                        gassert(false,"GDataControlLayer::ExecuteMission(): Invalid training level for multiple cells");
                        return 0.f;
                    }
                }
            }

			l_fSuccessRate = FindCovertOpsSuccessRate(in_Cell.MissionComplexity(),
																	in_Cell.ExperienceLevelType(),
																	l_eMissionType,
																	in_Cell.TargetSector(),
																	l_bFraming,
																	l_iCountryTarget,
																	l_iSpecificSector,
                                                                    l_fTotalTrainingOfAdditionalCells);	
        }

		//Success?
		Random::GQuick l_Rand;
		l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * (REAL64)(l_fSuccessRate*100.f) * time(NULL)) );
		if(l_Rand.RandomReal() < l_fSuccessRate)
			l_bSuccess = true;
	}
	
   if(!l_bSuccess)
	{
		if(l_iCountryTarget == l_iCellOwnerID)
		{
			g_ServerDAL.CountryData(l_iCountryTarget)->GvtApproval(
				max(0.f,g_ServerDAL.CountryData(l_iCountryTarget)->GvtApproval() - 0.5f));
		}
		else
		{
			//Loss of relations between source and target
			g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCellOwnerID,
				g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCellOwnerID) - l_fRelationLoss);
		}
		
		if(l_iCountryBeingFramed > 0)
		{
			//Loss of relations between source and framed country
			g_ServerDAL.RelationBetweenCountries(l_iCellOwnerID,l_iCountryBeingFramed,
				g_ServerDAL.RelationBetweenCountries(l_iCellOwnerID,l_iCountryBeingFramed) - l_fRelationLoss);
			
		}

		l_iCountryB = l_iCellOwnerID;
		l_eType = l_eTypeFailed;

		//Check if the cell has been captured
		Random::GQuick l_Rand;
		l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryB * time(NULL)) );
		if(l_Rand.RandomReal() < SP2::c_fChanceOfCapture)
		{
			l_bCaptured = true;
		}
	}
	else
	{	
		l_eType = l_eTypeSuccess;

		//Check if the attacker will be uncovered
		{
			REAL32 l_fTrainingModifier = 0.f;
			switch(in_Cell.ExperienceLevelType())
			{
			case ECovertActionCellTraining::Recruit:
				l_fTrainingModifier = SP2::c_fCovertActionsCellsRecruitSuccessRate; break;
			case ECovertActionCellTraining::Regular:
				l_fTrainingModifier = SP2::c_fCovertActionsCellsRegularSuccessRate; break;
			case ECovertActionCellTraining::Veteran:
				l_fTrainingModifier = SP2::c_fCovertActionsCellsVeteranSuccessRate; break;
			case ECovertActionCellTraining::Elite:
				l_fTrainingModifier = SP2::c_fCovertActionsCellsEliteSuccessRate; break;
			}
			REAL32 l_fChanceOfKnowingAttacker = max(0.f,l_fDifficultyOfKnowing * ((FindNationalSecurity(l_iCountryTarget) * 0.75f) - (l_fTrainingModifier * 0.2f)));

			//Success?
			Random::GQuick l_Rand;
			l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * time(NULL)) );
			if(l_Rand.RandomReal() < l_fChanceOfKnowingAttacker)
			{
				l_bKnowAttacker = true;
			}
			if(l_bKnowAttacker)
			{
				l_iCountryBeingFramed = l_iCellOwnerID;				
			}
		}
		
		if(l_iCountryBeingFramed > 0)
		{
			if(l_iCountryTarget == l_iCellOwnerID && l_iCountryTarget == l_iCountryBeingFramed)
			{
                //Successfully targeted self, but also framed self
				g_ServerDAL.CountryData(l_iCountryTarget)->GvtApproval(
					max(0.f,g_ServerDAL.CountryData(l_iCountryTarget)->GvtApproval() - 0.5f));
			}
			else
			{
				//Loss of relations between framed country and target
				g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCountryBeingFramed,
					g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCountryBeingFramed) - l_fRelationLoss);
			}
			l_iCountryB = l_iCountryBeingFramed;
			l_eType = l_eTypeFramed;
		}

		REAL32 l_fEffectModifier = 0.f;
		switch(in_Cell.MissionComplexity())
		{
		case ECovertActionsMissionComplexity::Low:
			l_fEffectModifier = SP2::c_fMissionComplexityModifierLow;
			break;
		case ECovertActionsMissionComplexity::Medium:
			l_fEffectModifier = SP2::c_fMissionComplexityModifierMedium;
			break;
		case ECovertActionsMissionComplexity::High:
			l_fEffectModifier = SP2::c_fMissionComplexityModifierHigh;
			break;
		case ECovertActionsMissionComplexity::Undefined:
			gassert(false,"GDataControlLayer::ExecuteMission(): Undefined target importance");
			return false;
		}
		
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_iCountryTarget);

		switch(l_eMissionType)
		{
			case SP2::ECovertActionsMissionType::Assassination:
            {
				g_ServerDCL.ChangeCountryStability(l_iCountryTarget,SP2::c_fStabilityChangeAssassination * l_fEffectModifier);

                if(l_iCountryTarget != l_iCellOwnerID)
                {
                    //If performed against a foreign country, we may remove (assassinate) a covert cell from the target country at random
                    //Higher the experience level and complexity, higher the chance
                    REAL32 l_fDeterminer = SP2::c_fCovertActionsCellsRecruitSuccessRate;

                    switch(in_Cell.ExperienceLevelType())
			        {
			        case ECovertActionCellTraining::Recruit:
				        l_fDeterminer = SP2::c_fCovertActionsCellsRecruitSuccessRate; break;
			        case ECovertActionCellTraining::Regular:
				        l_fDeterminer = SP2::c_fCovertActionsCellsRegularSuccessRate; break;
			        case ECovertActionCellTraining::Veteran:
				        l_fDeterminer = SP2::c_fCovertActionsCellsVeteranSuccessRate; break;
			        case ECovertActionCellTraining::Elite:
				        l_fDeterminer = SP2::c_fCovertActionsCellsEliteSuccessRate; break;
			        }

                    switch(in_Cell.MissionComplexity())
		            {
		            case ECovertActionsMissionComplexity::Low:
			            l_fDeterminer *= SP2::c_fMissionComplexityModifierLow;
			            break;
		            case ECovertActionsMissionComplexity::Medium:
			            l_fDeterminer *= SP2::c_fMissionComplexityModifierMedium;
			            break;
		            case ECovertActionsMissionComplexity::High:
			            l_fDeterminer *= SP2::c_fMissionComplexityModifierHigh;
			            break;
		            case ECovertActionsMissionComplexity::Undefined:
			            gassert(false,"GDataControlLayer::ExecuteMission(): Undefined target importance");
			            return false;
		            }

                    //0.125f <= l_fDeterminer <= 4.f
                    Random::GQuick l_Rand;
                    l_Rand.Seed(static_cast<UINT32>(g_Joshua.GameTime() * l_iCountryTarget * time(NULL)));
                    if(l_fDeterminer > l_Rand.RandomReal(10.f))
                    {
                        const vector<GCovertActionCell>& l_vTotalCells = l_pCountryData->CovertActionCells();

                        //Get a list of cells that are actually in the target country
                        vector<vector<GCovertActionCell>::const_iterator> l_vEligibleCells;
                        for(vector<GCovertActionCell>::const_iterator l_CellIt = l_vTotalCells.cbegin();
                            l_CellIt < l_vTotalCells.cend();
                            ++l_CellIt)
                        {
                            if(l_CellIt->AssignedCountry() == l_iCountryTarget &&
                                (l_CellIt->ActualState() == ECovertActionsCellState::Active ||
                                l_CellIt->ActualState() == ECovertActionsCellState::Dormant ||
                                l_CellIt->ActualState() == ECovertActionsCellState::PreparingMission ||
                                l_CellIt->ActualState() == ECovertActionsCellState::ReadyToExecute))
                                l_vEligibleCells.push_back(l_CellIt);
                        }

                        if(!l_vEligibleCells.empty())
                        {
                            //There exist eligible cells to remove, so remove one at random
                            Random::GQuick l_Rand;
                            l_Rand.Seed(static_cast<UINT32>(g_Joshua.GameTime() * l_iCountryTarget * l_vEligibleCells.size() * time(NULL)));
                            INT32 l_iEligibleCellToRemove = static_cast<INT32>(l_Rand.RandomReal(static_cast<REAL32>(l_vEligibleCells.size())));
                            l_pCountryData->RemoveCovertActionCell(*l_vEligibleCells.at(l_iEligibleCellToRemove));
                            l_pCountryData->m_bCovertActionCellsDirty = true;
                        }
                    }
                }
            }
			break;
			case SP2::ECovertActionsMissionType::CoupEtat:
			{
				if(l_iCountryTarget == l_iCellOwnerID)
					break;
				//What happens when 
				if(l_pCountryData->GvtType() == EGovernmentType::MultiPartyDemocracy ||
					l_pCountryData->GvtType() == EGovernmentType::SinglePartyDemocracy)
				{
					//Change political party in power
					INT32 l_iOldLeaderParty = l_pCountryData->LeaderParty()->Id();
					REAL32 l_fMaxPerc = 0.f;
					INT32 l_iMaxLeaderParty = -1;
					const vector<GPoliticalParty>& l_vParties = l_pCountryData->PoliticalParty();
					for(UINT32 i=0; i<l_vParties.size(); i++)
					{
						if(l_vParties[i].GvtType() == EGovernmentType::MultiPartyDemocracy)
						{
							if(l_vParties[i].InPower())
							{
								l_pCountryData->ChangePoliticalPartyInPower(l_vParties[i].Id(),false);
								if(l_pCountryData->GvtType() == EGovernmentType::SinglePartyDemocracy)
									l_pCountryData->PoliticalPartyChangeStatus(l_vParties[i].Id(),2);
							}
							else
							{
								if(l_vParties[i].PercentageValue() >= l_fMaxPerc)
								{
									l_fMaxPerc = l_vParties[i].PercentageValue();
									l_iMaxLeaderParty = l_vParties[i].Id();
								}
							}
						}
					}
					//If no party has been found, restored the old one
					if(l_iMaxLeaderParty == -1)
					{
						l_pCountryData->ChangePoliticalPartyInPower(l_iOldLeaderParty,true);
						l_pCountryData->PoliticalPartyChangeStatus(l_iOldLeaderParty,1);
					}
					else
					{
						l_pCountryData->ChangePoliticalPartyInPower(l_iMaxLeaderParty,true);
						l_pCountryData->PoliticalPartyChangeStatus(l_iMaxLeaderParty,1);
					}
				}
				else
				{
					//no political party changes
				}

				g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCellOwnerID,g_ServerDAL.RelationBetweenCountries(l_iCountryTarget,l_iCellOwnerID) + SP2::c_fRelationBonusCoupEtat);
				
            //If the player is human, warn the game objective evaluator that a country was a victim of a coup d'etat
            NotifyObserver(c_iNotificationOnSuccessfulCoupEtat,(void*)l_iCountryTarget);
			}
			break;
			case SP2::ECovertActionsMissionType::Espionage:
			{
                if(l_iCountryTarget == l_iCellOwnerID)
					break;
				if(in_Cell.TargetSector() == ECovertActionsTargetSector::Civilian)
				{
					//Increase the resource for the country doing the action
					EResources::Enum l_TargetType = in_Cell.ResourceType();
					if(l_TargetType == EResources::ItemCount || l_TargetType == -1)
					{
						l_TargetType = RandomResource((UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * (REAL64)l_pCountryData->Population()));					
					}				
					l_iRessourceOrUnitType = (INT32)l_TargetType;
					REAL64 l_fStolen = l_pCountryData->ResourceProduction(l_TargetType) * SP2::c_fPercentageResourceStolenByEspionnage * (REAL64)l_fEffectModifier;
					AddProductionToCountry(l_iCellOwnerID,l_fStolen,l_TargetType);
					g_ServerDAL.CountryData(l_iCellOwnerID)->SynchronizeWithRegions();

				}
				else if(in_Cell.TargetSector() == ECovertActionsTargetSector::Military)
				{
					//A technology possessed by the target country is acquired
					EUnitCategory::Enum l_TargetType = in_Cell.UnitCategory();
					SP2::GResearchInformation* l_pResearchInfoTarget = (SP2::GResearchInformation*)l_pCountryData->ResearchInfo();
					SP2::GResearchInformation* l_pResearchInfoSource = (SP2::GResearchInformation*)g_ServerDAL.CountryData(l_iCellOwnerID)->ResearchInfo();
					if(l_TargetType == EUnitCategory::ItemCount || l_TargetType == -1)
					{					
						Random::GQuick l_Rand;
						l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * (REAL64)l_pCountryData->Population()) );
						l_TargetType = (EUnitCategory::Enum)(1 + (UINT32)(l_Rand.RandomReal() * 4.f));
					}

               //Create and initialize the 2 research arrays
               UINT8 l_TargetResearch[EUnitDesignCharacteristics::ItemCount];
               UINT8 l_SourceResearch[EUnitDesignCharacteristics::ItemCount];
               {
                  memset(l_SourceResearch,0,EUnitDesignCharacteristics::ItemCount);
                  memset(l_TargetResearch,0,EUnitDesignCharacteristics::ItemCount);
               }

					l_iRessourceOrUnitType = (INT32)l_TargetType;



               //Fill the arrays with the values
               for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
               {
                  l_TargetResearch[i] = (UINT8)floor(l_pResearchInfoTarget->m_fMaxValues[l_TargetType][i]);
                  l_SourceResearch[i] = (UINT8)floor(l_pResearchInfoSource->m_fMaxValues[l_TargetType][i]);
               }



					switch(l_TargetType)
					{
					   case EUnitCategory::Air:
                  case EUnitCategory::Naval:
						{
                     break;
						}
					case EUnitCategory::Ground:
						{
                     l_TargetResearch[EUnitDesignCharacteristics::Stealth]        = 0;                                  
                     l_TargetResearch[EUnitDesignCharacteristics::CounterMesures] = 0;                                  
                     l_SourceResearch[EUnitDesignCharacteristics::Stealth]        = 0;                                  
                     l_SourceResearch[EUnitDesignCharacteristics::CounterMesures] = 0;                                  
						}
						break;
					case EUnitCategory::Nuclear:
						{

                     l_TargetResearch[EUnitDesignCharacteristics::GunRange]         = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::GunPrecision]     = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::GunDamage]        = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::MissilePayload]   = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::Stealth]          = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::CounterMesures]   = 0;
                     l_TargetResearch[EUnitDesignCharacteristics::Armor]            = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::GunRange]         = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::GunPrecision]     = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::GunDamage]        = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::MissilePayload]   = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::Stealth]          = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::CounterMesures]   = 0;
                     l_SourceResearch[EUnitDesignCharacteristics::Armor]            = 0;
						}
						break;
					default:
						gassert(false,"GDataControlLayer::ExecuteMission(): Undefined unit category");
						return false;
					}

               for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
               {
                  if(l_TargetResearch[i] > l_SourceResearch[i])
   						l_pResearchInfoSource->m_fMaxValues[l_TargetType][i] = (REAL32)(l_SourceResearch[i] + 1);
                  
               }
				}
				else
				{
					gassert(false,"GDataControlLayer::ExecuteMission(): Undefined Target sector");
					return false;
				}
			}
			break;
			case SP2::ECovertActionsMissionType::Sabotage:
			{
				if(in_Cell.TargetSector() == ECovertActionsTargetSector::Civilian)
				{
					//Increase the resource for the country doing the action
					EResources::Enum l_TargetType = in_Cell.ResourceType();
					if(l_TargetType == EResources::ItemCount || l_TargetType == -1)
					{
						l_TargetType = RandomResource((UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * (REAL64)l_pCountryData->Population()));
					}				

					l_iRessourceOrUnitType = (INT32)l_TargetType;

					REAL64 l_fLoss = l_pCountryData->ResourceProduction(l_TargetType) * SP2::c_fPercentageResourceLostBySabotage * (REAL64)l_fEffectModifier;
					RemoveProductionFromCountry(l_iCountryTarget,l_fLoss,l_TargetType);
					g_ServerDAL.CountryData(l_iCountryTarget)->SynchronizeWithRegions();
				}
				else if(in_Cell.TargetSector() == ECovertActionsTargetSector::Military)
				{
					//A technology possessed by the target country is acquired
					EUnitCategory::Enum l_TargetType = in_Cell.UnitCategory();
					SP2::GResearchInformation* l_pResearchInfoTarget = (SP2::GResearchInformation*)l_pCountryData->ResearchInfo();
					if(l_TargetType == EUnitCategory::ItemCount || l_TargetType == -1)
					{					
						Random::GQuick l_Rand;
						l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)l_iCountryTarget * (REAL64)l_pCountryData->Population()) );
						l_TargetType = (EUnitCategory::Enum)(1 + (UINT32)(l_Rand.RandomReal() * 4.f));
					}

					UINT32 l_iIndex = 0;
					REAL32 l_fMaxValue = 0.f;
					//Find unit characterestic to sabotage
					for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
               {
						if(l_pResearchInfoTarget->m_fMaxValues[l_TargetType][i] > l_fMaxValue)
						{
							l_iIndex = i;
							l_fMaxValue = l_pResearchInfoTarget->m_fMaxValues[l_TargetType][i];
						}
					}

					//Cut for that resource
					REAL32 l_fDifference = l_pResearchInfoTarget->m_fMaxValues[l_TargetType][l_iIndex] - floor(l_pResearchInfoTarget->m_fMaxValues[l_TargetType][l_iIndex]);
					
					switch(in_Cell.MissionComplexity())
					{
					case ECovertActionsMissionComplexity::Low:
						l_fDifference = min(l_fDifference,0.25f); break;
					case ECovertActionsMissionComplexity::Medium:
						l_fDifference = min(l_fDifference,0.5f); break;
					case ECovertActionsMissionComplexity::High:
						l_fDifference = min(l_fDifference,1.f); break;
					}

					l_pResearchInfoTarget->m_fMaxValues[l_TargetType][l_iIndex] -= l_fDifference;					
				}
				else
				{
					gassert(false,"GDataControlLayer::ExecuteMission(): Undefined Target sector in sabotage mission");
					return false;
				}
			}
			break;
			case SP2::ECovertActionsMissionType::SocialUpheaval:
				g_ServerDCL.ChangeCountryStability(l_iCountryTarget,SP2::c_fStabilityChangeSocialUpheaval * l_fEffectModifier);
				break;
			case SP2::ECovertActionsMissionType::Terrorism:
				g_ServerDCL.ChangeCountryStability(l_iCountryTarget,SP2::c_fStabilityChangeTerrorism * l_fEffectModifier);
				break;
			default:
				gassert(false,"GDataControlLayer::ExecuteMission(): Undefined mission type");
		}
	}
	
	//send event noticing players an action has occured
	
	//Notify source
	SDK::GGameEventSPtr l_EventSource = CREATE_GAME_EVENT(SP2::Event::GSendCovertOpsMissionDone);
	l_EventSource->m_iSource = SDK::Event::ESpecialTargets::Server;
	l_EventSource->m_iTarget = g_Joshua.ActivePlayerByModID(l_iCellOwnerID)->Id();
	SP2::Event::GSendCovertOpsMissionDone* l_pCovertOpsSource = (SP2::Event::GSendCovertOpsMissionDone*)l_EventSource.get();
	l_pCovertOpsSource->m_iCellID						= in_Cell.ID();
	l_pCovertOpsSource->m_iCellSourceCountry		= l_iCellOwnerID;
	l_pCovertOpsSource->m_sCellName					= in_Cell.Name();
	l_pCovertOpsSource->m_eType						= l_eMissionType;
	l_pCovertOpsSource->m_eTargetSector				= in_Cell.TargetSector();
	l_pCovertOpsSource->m_eComplexity				= in_Cell.MissionComplexity();
	l_pCovertOpsSource->m_iResourceOrUnitType		= l_iRessourceOrUnitType;
	l_pCovertOpsSource->m_iTargetCountry			= l_iCountryTarget;
	l_pCovertOpsSource->m_iFramedCountry			= l_iCountryBeingFramed;
	l_pCovertOpsSource->m_bSuccessful				= l_bSuccess;
	l_pCovertOpsSource->m_bCaptured					= l_bCaptured;
	l_pCovertOpsSource->m_bKnowAttacker				= l_bKnowAttacker;
	g_Joshua.RaiseEvent(l_EventSource); 

	//Notify target
	if(!(( (l_eType == News::EType::EspionageCivilian && l_iCountryBeingFramed == 0 && l_bSuccess) ||
		(l_eType == News::EType::EspionageMilitary && l_iCountryBeingFramed == 0 && l_bSuccess) )))
	{
		SDK::GGameEventSPtr l_EventTarget = CREATE_GAME_EVENT(SP2::Event::GSendCovertOpsMissionDone);
		l_EventTarget->m_iSource = SDK::Event::ESpecialTargets::Server;
		l_EventTarget->m_iTarget = g_Joshua.ActivePlayerByModID(l_iCountryTarget)->Id();
		SP2::Event::GSendCovertOpsMissionDone* l_pCovertOpsTarget = (SP2::Event::GSendCovertOpsMissionDone*)l_EventTarget.get();
		l_pCovertOpsTarget->m_iCellID						= in_Cell.ID();
		l_pCovertOpsTarget->m_iCellSourceCountry		= l_iCellOwnerID;
		l_pCovertOpsTarget->m_sCellName					= in_Cell.Name();
		l_pCovertOpsTarget->m_eType						= l_eMissionType;
		l_pCovertOpsTarget->m_eTargetSector				= in_Cell.TargetSector();
		l_pCovertOpsTarget->m_eComplexity				= in_Cell.MissionComplexity();
		l_pCovertOpsTarget->m_iResourceOrUnitType		= l_iRessourceOrUnitType;
		l_pCovertOpsTarget->m_iTargetCountry			= l_iCountryTarget;
		l_pCovertOpsTarget->m_iFramedCountry			= l_iCountryBeingFramed;
		l_pCovertOpsTarget->m_bSuccessful				= l_bSuccess;
		l_pCovertOpsTarget->m_bCaptured					= l_bCaptured;
		l_pCovertOpsTarget->m_bKnowAttacker				= l_bKnowAttacker;
		g_Joshua.RaiseEvent(l_EventTarget); 
	}

	if(l_iCountryBeingFramed > 0)
	{
		SDK::GGameEventSPtr l_EventFramed = CREATE_GAME_EVENT(SP2::Event::GSendCovertOpsMissionDone);
		l_EventFramed->m_iSource = SDK::Event::ESpecialTargets::Server;
		l_EventFramed->m_iTarget = g_Joshua.ActivePlayerByModID(l_iCountryBeingFramed)->Id();
		SP2::Event::GSendCovertOpsMissionDone* l_pCovertOpsFramed = (SP2::Event::GSendCovertOpsMissionDone*)l_EventFramed.get();
		l_pCovertOpsFramed->m_iCellID						= in_Cell.ID();
		l_pCovertOpsFramed->m_iCellSourceCountry		= l_iCellOwnerID;
		l_pCovertOpsFramed->m_sCellName					= in_Cell.Name();
		l_pCovertOpsFramed->m_eType						= l_eMissionType;
		l_pCovertOpsFramed->m_eTargetSector				= in_Cell.TargetSector();
		l_pCovertOpsFramed->m_eComplexity				= in_Cell.MissionComplexity();
		l_pCovertOpsFramed->m_iResourceOrUnitType		= l_iRessourceOrUnitType;	
		l_pCovertOpsFramed->m_iTargetCountry			= l_iCountryTarget;
		l_pCovertOpsFramed->m_iFramedCountry			= l_iCountryBeingFramed;
		l_pCovertOpsFramed->m_bSuccessful				= l_bSuccess;
		l_pCovertOpsFramed->m_bCaptured					= l_bCaptured;
		l_pCovertOpsFramed->m_bKnowAttacker				= l_bKnowAttacker;
		g_Joshua.RaiseEvent(l_EventFramed); 
	}		

	//Send the news, except when it's a succesful espionnage mission, without framed
	if(l_eType == News::EType::EspionageCivilian && l_iCountryBeingFramed == 0 && l_bSuccess)
		return false;
	if(l_eType == News::EType::EspionageMilitary && l_iCountryBeingFramed == 0 && l_bSuccess)
		return false;
   
	{
		//Send news
      News::GInfo l_News;
      l_News.m_iCountryA = l_iCountryA;
		l_News.m_iCountryB = l_iCountryB;
      l_News.m_eType = l_eType;
      l_News.m_iPriority = g_ServerDAL.NewsPriority(l_eType);
      l_News.m_eCategory = g_ServerDAL.NewsCategory(l_eType);
		SendNews(l_News);
   }

   if(l_bSuccess)
   {
      g_ServerDAL.AddHistoricalMarker(l_iCellOwnerID, 
                                      EHistoryMarkerType::CovertOpSuccess, 
                                      (REAL32) g_ServerDAL.ConvertMissionTypeStrID(l_eMissionType ) );

      g_ServerDAL.AddHistoricalMarker(l_iCountryTarget, 
                                      EHistoryMarkerType::CovertOpVictim, 
                                      (REAL32) g_ServerDAL.ConvertMissionTypeStrID(l_eMissionType ) );
   }
   else
   {
      g_ServerDAL.AddHistoricalMarker(l_iCellOwnerID, 
                                      EHistoryMarkerType::CovertOpFailure, 
                                      (REAL32) g_ServerDAL.ConvertMissionTypeStrID(l_eMissionType ) );
   }

	if(l_bCaptured)
	{
		//The cell has been captured, tortured, then killed by horrible mutant monkeys
		return true;
	}

   return false;
}

EResources::Enum GDataControlLayer::RandomResource(UINT32 in_iSeed) const
{
	Random::GQuick l_Rand;
	l_Rand.Seed( in_iSeed );
	UINT32 l_iNbResources = (UINT32)EResources::ItemCount;
	return (EResources::Enum)((UINT32)(l_Rand.RandomReal() * (REAL32)l_iNbResources));	

}

/*!
 * Iterate the covert actions cells
 **/
void GDataControlLayer::IterateCovertActionCells()
{
   for(UINT32 i = 1 ; i <= g_ServerDAL.NbCountry() ; i++)
   {
      SP2::GCountryData* l_pData = g_ServerDAL.CountryData(i);
      gassert(l_pData,"GDataControlLayer::IterateCovertActionCells invalid country data");
      l_pData->IterateCovertActionCells();
   }
}

void GDataControlLayer::IterateBudgetExpenseSecurity(UINT32 in_iCountryID)
{
	REAL64 l_fUpkeepFee = 0.f;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	const vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(vector<GCovertActionCell>::const_iterator it = l_vCells.begin();
		it != l_vCells.end(); it++)
	{
		const GCovertActionCell& l_Cell = *it;
		l_fUpkeepFee += l_Cell.UpkeepFee();		
	}

	l_pCountryData->BudgetExpenseSecurity(l_fUpkeepFee);
}

REAL32 GDataControlLayer::FindNationalSecurity(UINT32 in_iCountryID) const
{
	REAL32 l_fPopulationLevel = g_ServerDAL.PopulationLevel(in_iCountryID);
	UINT32 l_iNbCellsInDefense = 0;
	REAL32 l_fAverageCell = 0.f;
	
	//Iterate through the target country, and find how many cells it has inside, in defense
	const vector<GCovertActionCell>& l_vCells = g_ServerDAL.CountryData(in_iCountryID)->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].AssignedCountry() == in_iCountryID && 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
		{
			l_iNbCellsInDefense++;
			switch(l_vCells[i].ExperienceLevelType())
			{
			case ECovertActionCellTraining::Recruit:
				l_fAverageCell += SP2::c_fCovertActionsCellsRecruitSuccessRate; break;
			case ECovertActionCellTraining::Regular:
				l_fAverageCell += SP2::c_fCovertActionsCellsRegularSuccessRate; break;
			case ECovertActionCellTraining::Veteran:
				l_fAverageCell += SP2::c_fCovertActionsCellsVeteranSuccessRate; break;
			case ECovertActionCellTraining::Elite:
				l_fAverageCell += SP2::c_fCovertActionsCellsEliteSuccessRate; break;
			default:
				gassert(false,"GDataControlLayer::ExecuteMission(): Invalid training level");
			}
			
		}
	}

	if(l_iNbCellsInDefense > 0)
		l_fAverageCell /= (REAL32)l_iNbCellsInDefense;
	return FindNationalSecurity(l_iNbCellsInDefense,l_fPopulationLevel,l_fAverageCell);
}

REAL32 GDataControlLayer::FindNationalSecurity(UINT32 in_iNbCellsInDefense,			
																REAL32 in_fPopulationLevel,
																REAL32 in_fAverageTrainingCell) const
{
	if(in_iNbCellsInDefense == 1)
	{
		return (SP2::c_fMaximumProtectionCovertActions * in_fAverageTrainingCell) / (in_fPopulationLevel + 1.f);
	}
	else if(in_iNbCellsInDefense == 0)
	{
		return 0.f;
	}
	else
	{
		REAL32 l_fPreceedingTotal = FindNationalSecurity(in_iNbCellsInDefense-1,
																		 in_fPopulationLevel,
																		 in_fAverageTrainingCell);
		return l_fPreceedingTotal + 
			((in_fAverageTrainingCell * (SP2::c_fMaximumProtectionCovertActions - l_fPreceedingTotal)) 
			/ (in_fPopulationLevel + 1.f));
	}
}

void GDataControlLayer::UpdateCostByCountryInsideTreaty(UINT32 in_iTreatyID)
{
	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	if(!l_pTreaty)
		return;

	l_pTreaty->ClearCosts();

	//Find the total cost for that treaty
	//First, find the cost of that treaty
	REAL64 l_fTotalCost = 0.f;
	
	if(l_pTreaty->Active())
	{
		l_fTotalCost = SP2::c_fFixedCostForTreaties;
		l_fTotalCost += (l_pTreaty->MembersSideA(false).size() + l_pTreaty->MembersSideB(false).size()) * SP2::c_fAdditionalCostForTreatiesByCountry;
	}

	//Find the list of countries that will pay this treaty
	set<UINT32> l_CountriesThatPay;
	l_CountriesThatPay.insert(l_pTreaty->MembersSideA(true).begin(),l_pTreaty->MembersSideA(true).end());
	if(l_pTreaty->SideBCanLeave())
		l_CountriesThatPay.insert(l_pTreaty->MembersSideB(true).begin(),l_pTreaty->MembersSideB(true).end());
	l_CountriesThatPay.insert(l_pTreaty->MembersPressure().begin(),l_pTreaty->MembersPressure().end());

	//Every country inside that treaty
	set<UINT32> l_CountryList;
	l_CountryList.insert(l_pTreaty->MembersSideA(false).begin(),l_pTreaty->MembersSideA(false).end());
	l_CountryList.insert(l_pTreaty->MembersSideB(false).begin(),l_pTreaty->MembersSideB(false).end());
	l_CountryList.insert(l_pTreaty->MembersPressure().begin(),l_pTreaty->MembersPressure().end());

	//Find the total economic weight of that treaty
	REAL32 l_fEconomicStrength = 0.f;
	for(set<UINT32>::iterator it = l_CountriesThatPay.begin();
		 it != l_CountriesThatPay.end(); it++)
	{
		l_fEconomicStrength += g_ServerDAL.CountryData(*it)->EconomicStrength();
	}
	
	if(l_fEconomicStrength == 0.f)
		return;

	for(set<UINT32>::iterator it = l_CountryList.begin(); 
		 it != l_CountryList.end(); it++)
	{
		if(l_CountriesThatPay.find(*it) == l_CountriesThatPay.end())
		{
			l_pTreaty->CostForCountry(*it,0.f);
		}
		else
		{
			REAL64 l_fRatio = (REAL64)(g_ServerDAL.CountryData(*it)->EconomicStrength() / l_fEconomicStrength);
			l_pTreaty->CostForCountry(*it,l_fTotalCost * l_fRatio);
		}
	}

}

bool GDataControlLayer::RegionTouchesOtherCountryOrWater(UINT32 in_iCountryID, UINT32 in_iRegionID) const
{	
	if(in_iRegionID == 0)
		return true;
	const GRegionGraphNode* l_pGraph = g_ServerDAL.RegionGraph().Graph();	
	const GRegionGraphNode& l_pCurNode = l_pGraph[in_iRegionID];	

	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	for(UINT32 n = 0;n < l_pCurNode.m_iNbNeighbors;n ++)
	{
		if(l_vControls[l_pCurNode.m_pNeighborIDs[n]].m_iPolitical != in_iCountryID ||
			l_pCurNode.m_pNeighborIDs[n] == 0)
			return true;
	}
	return false;
}

void GDataControlLayer::OccupyCountry(UINT32 in_iCountryAttacking, UINT32 in_iCountryTarget,const set<UINT32>& in_vAdditionalEnnemies)
{
   //Check if it is a defensive country occupation
	bool l_bDefensive = true;
   {
	   if(in_iCountryAttacking != in_iCountryTarget)
	   {
   		set<UINT32> l_Attackers;
			l_Attackers.insert(in_iCountryAttacking);
			DeclareNewWar(l_Attackers,in_iCountryAttacking,in_iCountryTarget);
		   l_bDefensive = false;
      }
   }

   //Declare war to all countries that we occupy
	for(set<UINT32>::const_iterator it = in_vAdditionalEnnemies.begin();
		 it != in_vAdditionalEnnemies.end(); it++)
	{
		set<UINT32> l_Attackers;
		l_Attackers.insert(in_iCountryAttacking);
		DeclareNewWar(l_Attackers,in_iCountryAttacking,*it);
	}

   //Fetch all the enemies of the attacking country
	set<UINT32> l_Ennemies;
	g_ServerDAL.IsAtWarWith(in_iCountryAttacking,l_Ennemies);

   const UINT8*                  l_pDiplomatic  = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryAttacking);
   const vector<GRegionControl>& l_vControl     = g_ServerDAL.RegionControlArray();

   //Check if country Attacking has any unit groups inside country Target
   set<UINT32> l_PossibleUnitGroups;
   {
	   const set<UINT32>&            l_Attacking    = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryAttacking);      
   	//Check if group is in target country
   	for(set<UINT32>::const_iterator it = l_Attacking.begin(); 
		   it != l_Attacking.end(); it++)
	   {
   		SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		   if(!l_pGroup->IsNaval() && 
   			(l_pGroup->Status() == EMilitaryStatus::Fortified ||
			   l_pGroup->Status() == EMilitaryStatus::Ready ||
			   l_pGroup->Status() == EMilitaryStatus::Moving ||
			   l_pGroup->Status() == EMilitaryStatus::Parked) &&
			   l_vControl[((SP2::GUnitGroupEx*)l_pGroup)->m_iRegionID].m_iPolitical == in_iCountryTarget)
		   {
   			l_PossibleUnitGroups.insert(*it);
		   }
	   }
   }

   //Fetch the target regions (where we will put units) and ad
	vector<UINT32>          l_vDefenseUnitGroups;
	multimap<REAL32,UINT32> l_InvadingRegions;
   {
	   const set<UINT32>&      l_Politic   = g_ServerDAL.CountryPoliticalControl(in_iCountryTarget);
	
      //Insert target regions controlled politically by the target, and controled military by the target and ennemies		
	   if(l_bDefensive)
	   {
         //If a defensive mode, only add the region that touch water.
		   for(set<UINT32>::const_iterator it = l_Politic.begin();
			   it != l_Politic.end(); it++)
		   {
			   GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
			   m_UnitMover.UnitGroupsInsideRegion(*it,l_vDefenseUnitGroups,in_iCountryTarget);
			   if(((EDiplomaticStatus::Enum)(l_pDiplomatic[l_vControl[*it].m_iMilitary]) == EDiplomaticStatus::Hostile) ||
				   RegionTouchesOtherCountryOrWater(in_iCountryTarget,*it)
				   )
			   {
				   l_InvadingRegions.insert(make_pair<REAL32,UINT32>(-l_pRegion->PercentageValue(),l_pRegion->Id()));		
			   }
		   }
	   }
	   else
	   {
         //add the regions
		   for(set<UINT32>::const_iterator it = l_Politic.begin();
			   it != l_Politic.end(); it++)
		   {
			   GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
			   if((EDiplomaticStatus::Enum)(l_pDiplomatic[l_vControl[*it].m_iMilitary]) == 
				   EDiplomaticStatus::Hostile)
			   {
				   l_InvadingRegions.insert(make_pair<REAL32,UINT32>(-l_pRegion->PercentageValue(),l_pRegion->Id()));		
			   }
		   }
	   }
   }

	if(l_bDefensive)
	{
		//We must have as many units as we have regions to cover up
		//To do so, split up the biggest unit group
		UINT32 l_iNbRegionsToProtect       = l_InvadingRegions.size();
		UINT32 l_iNbUnitGroups             = l_vDefenseUnitGroups.size();
		bool   l_bNoMoreUnitGroupAvailable = false;		

      //While we do not have enough units groups or we cant split any further, split unit groups.
		while(l_iNbUnitGroups < l_iNbRegionsToProtect && !l_bNoMoreUnitGroupAvailable)
		{
			bool l_bFoundInvalidGroup = false;
			
         //Find strongest unit group
			REAL64           l_fMaxCost      = 0.f;
			SP2::GUnitGroup* l_pMaxUnitGroup = NULL;
         {
			   for(vector<UINT32>::iterator it = l_vDefenseUnitGroups.begin();
				   it != l_vDefenseUnitGroups.end(); it++)
			   {
				   SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
				   //Check if the unit group can split, if not remove it
				   const vector<SDK::Combat::GUnit*>& l_vUnits = l_pUnitGroup->Units();
				   if( (l_pUnitGroup->Status() == EMilitaryStatus::Attacking) ||
					   (l_vUnits.size() == 1 && ((SP2::GUnit*)l_vUnits[0])->Qty() == 1)
				   )
				   {
					   l_vDefenseUnitGroups.erase(it);
					   l_iNbUnitGroups--;
					   l_bFoundInvalidGroup = true;
					   break;
				   }
				   if(l_pUnitGroup->Value() > l_fMaxCost)
				   {
					   l_fMaxCost = l_pUnitGroup->Value();
					   l_pMaxUnitGroup = l_pUnitGroup;
				   }
			   }
         }

         //There is a group that we must split.
			if(l_pMaxUnitGroup)
			{				
				vector<SP2::GUnit>                 l_vUnitsToSplit;
				const vector<SDK::Combat::GUnit*>& l_vUnits        = l_pMaxUnitGroup->Units();
				UINT32                              l_iIndex       = 0;

            //Decide which units we must split.
				for(UINT32 i=0; i<l_vUnits.size(); i++)
				{
					SP2::GUnit* l_pTempUnit = (SP2::GUnit*)l_vUnits[i];
					UINT32 l_iQty           = l_pTempUnit->Qty();

               //If we cant split it
               if(l_iQty <= 1)
               {
                  gassert(l_iQty, "Should not be a quantity of 0");
						continue;
               }
               //If an Odd number, get an even nb (so it can be divided by 2)
					if(l_iQty % 2 != 0)
						l_iQty -= 1;
               //Determine the quantity to split
					UINT32 l_iQtyToSplit              = l_iQty / 2;
               gassert(l_iQtyToSplit,"Should not be null");
              
               //Insert the value into the vector of the units to split
               {
   					const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) l_pTempUnit->Design();					
					   l_vUnitsToSplit.resize(l_vUnitsToSplit.size() + 1);
					   l_vUnitsToSplit[l_iIndex].HP(l_pDesign->Type()->HP() * l_iQtyToSplit);
					   l_vUnitsToSplit[l_iIndex].Design(l_pTempUnit->Design());
                  gassert(l_pTempUnit->Training() <= ETrainingLevel::Elite,"Invalid training");
					   l_vUnitsToSplit[l_iIndex].Training(l_pTempUnit->Training());
               }
					l_iIndex++;
				}

            //If there are no units to split, take only the 1st unit and send it into the other group
				if(l_vUnitsToSplit.size() == 0)
				{
					SP2::GUnit* l_pTempUnit = (SP2::GUnit*)l_vUnits[0];
					l_vUnitsToSplit.resize(l_vUnitsToSplit.size() + 1);
					l_vUnitsToSplit[l_iIndex].HP(l_pTempUnit->HP());
					l_vUnitsToSplit[l_iIndex].Design(l_pTempUnit->Design());
               gassert(l_pTempUnit->Training() <= ETrainingLevel::Elite,"Invalid training");
					l_vUnitsToSplit[l_iIndex].Training(l_pTempUnit->Training());					
				}


            //Split the unit group
				UINT32 l_iNewUnitGroup                   = SplitUnitGroup(l_pMaxUnitGroup,l_vUnitsToSplit);
				SDK::Combat::GUnitGroup* l_pNewUnitGroup = g_Joshua.UnitManager().UnitGroup(l_iNewUnitGroup);
				gassert(l_pNewUnitGroup,"GDataControlLayer::OccupyCountry(): New unit group should exist");								
				l_iNbUnitGroups++;
				l_vDefenseUnitGroups.push_back(l_iNewUnitGroup);
			}
			else if(!l_bFoundInvalidGroup)
				l_bNoMoreUnitGroupAvailable = true;

		}
	}




   
   //Make the units move to their correct position
	bool l_bDone = false;
	if(l_bDefensive)
	{
		//First, sort unit groups
		multimap<REAL64,SP2::GUnitGroup*> l_DefenseUnitGroups;
		for(UINT32 i=0; i<l_vDefenseUnitGroups.size(); i++)
		{
			SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_vDefenseUnitGroups[i]);
			gassert(l_pUnitGroup,"GDataControlLayer::OccupyCountry(): Unit group should exist");
			l_DefenseUnitGroups.insert(make_pair<REAL64,SP2::GUnitGroup*>(-l_pUnitGroup->Value(),l_pUnitGroup));
		}
		multimap<REAL64,SP2::GUnitGroup*>::iterator l_Itr    = l_DefenseUnitGroups.begin();
		multimap<REAL64,SP2::GUnitGroup*>::iterator l_ItrEnd = l_DefenseUnitGroups.end();
		
      //For every regions, move unit group there	
		for(multimap<REAL32,UINT32>::iterator it = l_InvadingRegions.begin();
			it != l_InvadingRegions.end() && l_Itr != l_ItrEnd; it++)
		{
			UINT32 l_iRegionID = it->second;
			vector<SDK::Combat::GUnitGroup*> l_vUnitsToMove;

			vector<UINT32> l_vEnnemyUnitGroups;
			GVector2D<REAL32> l_Position(0.f,0.f);
			//Check if any enemies are there
			for(set<UINT32>::iterator EnnemyItr = l_Ennemies.begin();
				EnnemyItr != l_Ennemies.end(); EnnemyItr++)
			{
				m_UnitMover.UnitGroupsInsideRegion(l_iRegionID,l_vEnnemyUnitGroups,*EnnemyItr);
			}

			if(l_vEnnemyUnitGroups.size() > 0)
			{				
				REAL64 l_fStrengthEnnemies = 0.f;
				REAL64 l_fMaxEnnemyStrength = 0.f;
				for(UINT32 i=0; i<l_vEnnemyUnitGroups.size(); i++)
				{
					SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_vEnnemyUnitGroups[i]);
					if(l_pUnitGroup->Value() > l_fMaxEnnemyStrength)
					{
						l_Position = l_pUnitGroup->Position();
						l_fMaxEnnemyStrength = l_pUnitGroup->Value();
					}
					l_fStrengthEnnemies += l_pUnitGroup->Value();
				}
				REAL64 l_fDefenseSent = 0.f;
				//Send as many troups as we can to beat ennemy
				for(l_Itr; l_Itr != l_ItrEnd && l_fDefenseSent < l_fStrengthEnnemies; l_Itr++)
				{
					l_vUnitsToMove.push_back(l_Itr->second);
					l_fDefenseSent += -(l_Itr->first);
				}
			}
			else
			{
				l_Position = m_UnitMover.RegionLocation(l_iRegionID);
				l_vUnitsToMove.push_back(l_Itr->second);
				l_Itr++;
			}
			if(l_vUnitsToMove.size() > 0)
			{
				m_UnitMover.MoveUnits(l_vUnitsToMove,l_Position,EMilitaryAction::Default);
			}
		}
	}
	else
	{
		//For every regions, move unit group there	
		for(multimap<REAL32,UINT32>::iterator it = l_InvadingRegions.begin();
			it != l_InvadingRegions.end() && !l_bDone; it++)
		{
			UINT32 l_iRegionID    = it->second;
			vector<UINT32> l_vEnnemyUnitGroups;
			//Check if any enemies are there
			
         for(set<UINT32>::iterator EnnemyItr = l_Ennemies.begin();
				EnnemyItr != l_Ennemies.end(); EnnemyItr++)
			{
				m_UnitMover.UnitGroupsInsideRegion(l_iRegionID,l_vEnnemyUnitGroups,*EnnemyItr);
			}
			if(l_vEnnemyUnitGroups.size() > 0)
			{
				//Ennemies are present
				vector<SDK::Combat::GUnitGroup*> l_vUnitsToMove;
				REAL64 l_fStrengthSent             = 0.f;
				REAL64 l_fStrengthEnnemies         = 0.f;
				REAL64 l_fMaxEnnemyStrength        = 0.f;
				GVector2D<REAL32> l_EnnemyPosition = g_Joshua.UnitManager().UnitGroup(l_vEnnemyUnitGroups[0])->Position();
				for(UINT32 i=0; i<l_vEnnemyUnitGroups.size(); i++)
				{
					SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_vEnnemyUnitGroups[i]);
					if(l_pUnitGroup->Value() > l_fMaxEnnemyStrength)
					{
						l_EnnemyPosition = l_pUnitGroup->Position();
						l_fMaxEnnemyStrength = l_pUnitGroup->Value();
					}
					l_fStrengthEnnemies += l_pUnitGroup->Value();
				}
				
				while(l_fStrengthSent <= l_fStrengthEnnemies && !l_bDone)
				{
					//Find closest unit group, and move it
					REAL32 l_fMinDistance = FLT_MAX;
					SDK::Combat::GUnitGroup* l_pGroupToMove = NULL;
					for(set<UINT32>::iterator ItrUnitGroup = l_PossibleUnitGroups.begin();
						ItrUnitGroup != l_PossibleUnitGroups.end(); 
						ItrUnitGroup++)
					{
						SDK::Combat::GUnitGroup* l_pTempGroup = g_Joshua.UnitManager().UnitGroup(*ItrUnitGroup);
						gassert(l_pTempGroup,"GDataControlLayer::OccupyCountry(): Invalid Unit Group");
						REAL32 l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pTempGroup->Position(),l_EnnemyPosition);
						if(l_fDistance < l_fMinDistance && MoveHostilePossible(in_iCountryAttacking,((SP2::GUnitGroupEx*)l_pTempGroup)->m_iRegionID,l_iRegionID))
						{
							l_fMinDistance = l_fDistance;
							l_pGroupToMove = l_pTempGroup;
						}
					}
					if(l_pGroupToMove)
					{
						l_fStrengthSent += ((SP2::GUnitGroup*)l_pGroupToMove)->Value();
						l_PossibleUnitGroups.erase(l_pGroupToMove->Id());
						l_vUnitsToMove.push_back(l_pGroupToMove);
					}
					else
						l_bDone = true;
				}					
				if(l_vUnitsToMove.size() > 0)
					m_UnitMover.MoveUnits(l_vUnitsToMove,l_EnnemyPosition,EMilitaryAction::Attack);
			}
			else
			{
				//No enemy, move only one unit		
				GVector2D<REAL32> l_Location = m_UnitMover.RegionLocation(l_iRegionID);
				REAL32 l_fMinDistance = FLT_MAX;
				SDK::Combat::GUnitGroup* l_pGroupToMove = NULL;
				for(set<UINT32>::iterator ItrUnitGroup = l_PossibleUnitGroups.begin();
					ItrUnitGroup != l_PossibleUnitGroups.end(); 
					ItrUnitGroup++)
				{
					SDK::Combat::GUnitGroup* l_pTempGroup = g_Joshua.UnitManager().UnitGroup(*ItrUnitGroup);
					gassert(l_pTempGroup,"GDataControlLayer::OccupyCountry(): Invalid Unit Group");
					REAL32 l_fDistance                   = g_ServerDAL.DistanceBetween2PointsLatLong(l_pTempGroup->Position(),l_Location);
					if(l_fDistance < l_fMinDistance && MoveHostilePossible(in_iCountryAttacking,((SP2::GUnitGroupEx*)l_pTempGroup)->m_iRegionID,l_iRegionID))
					{
						l_fMinDistance = l_fDistance;
						l_pGroupToMove = l_pTempGroup;
					}
				}
				if(l_pGroupToMove)
				{
					vector<SDK::Combat::GUnitGroup*> l_vUnitsToMove;				
					//Find the weakest unit from the unit group, split it, and move it towards the destination
					const vector<SDK::Combat::GUnit*>& l_vUnits     = l_pGroupToMove->Units();
					bool l_bHaveToSplit = true;
					if(l_vUnits.size() == 1)
					{
						//Possibly move whole unit group in that case
						SP2::GUnit* l_pUnitToSplit = (SP2::GUnit*)l_vUnits[0];
						gassert(l_pUnitToSplit,"GDataControlLayer::OccupyCountry(): Invalid Unit");
						if(l_pUnitToSplit->Qty() == 1)
						{
							//move whole unit group
							l_vUnitsToMove.push_back(l_pGroupToMove);
							l_PossibleUnitGroups.erase(l_pGroupToMove->Id());
							l_bHaveToSplit = false;
						}
					}
					if(l_bHaveToSplit)
					{
						REAL32      l_fMinCost     = FLT_MAX;
						SP2::GUnit* l_pWeakestUnit = NULL;
						//Find the weakest unit, and move it
						for(UINT32 i=0; i<l_vUnits.size(); i++)
						{
							SP2::GUnit* l_pTempUnit = (SP2::GUnit*)l_vUnits[i];
							const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) l_pTempUnit->Design();
							if(l_pDesign->Cost() < l_fMinCost)
							{
								l_fMinCost = l_pDesign->Cost();
								l_pWeakestUnit = l_pTempUnit;
							}
						}
						gassert(l_pWeakestUnit,"GDataControlLayer::OccupyCountry(): We should have found a weak unit, unless that unit group has no unit in it.");
						const SP2::GUnitDesign* l_pDesignSplitting = (const SP2::GUnitDesign*) l_pWeakestUnit->Design();					
						//Create that temporary unit we are moving
						vector<SP2::GUnit> l_vUnitsToMoveAfterSplit;
						l_vUnitsToMoveAfterSplit.resize(1);
						l_vUnitsToMoveAfterSplit[0].HP(l_pDesignSplitting->Type()->HP());
						l_vUnitsToMoveAfterSplit[0].Design(l_pWeakestUnit->Design());
                  gassert(l_pWeakestUnit->Training() <= ETrainingLevel::Elite,"Invalid training");
						l_vUnitsToMoveAfterSplit[0].Training(l_pWeakestUnit->Training());					
						UINT32 l_iNewUnitGroup                    = SplitUnitGroup((SP2::GUnitGroup*)l_pGroupToMove,l_vUnitsToMoveAfterSplit);
						SDK::Combat::GUnitGroup* l_pNewUnitGroup  = g_Joshua.UnitManager().UnitGroup(l_iNewUnitGroup);
						gassert(l_pNewUnitGroup,"GDataControlLayer::OccupyCountry(): New unit group should exist");
						l_vUnitsToMove.push_back(l_pNewUnitGroup);
					}
					
					if(l_vUnitsToMove.size() > 0)
						m_UnitMover.MoveUnits(l_vUnitsToMove,l_Location,EMilitaryAction::Attack);
					
				}
				else
					l_bDone = true;
			}
			
		}
	}
}

/*!
 * Split the given unit group, the in_vUnitsToMove vector only gives an indication of 
 * what should be moved in a newly created group and does not actually represent any unit
 * that is in the UnitManager(), only used to hold data (that's why it's not a pointer)
 **/
UINT32 GDataControlLayer::SplitUnitGroup(SP2::GUnitGroup* in_pUnitGroup,
													  vector<SP2::GUnit>& in_vUnitsToMove)
{
	hash_map<UINT32,UINT32> l_HPAbleToRemoveByDesign;
	hash_map<UINT32,UINT32> l_HPToRemoveFromOriginalUnitGroup;
	const vector<SDK::Combat::GUnit*>& l_vUnits = in_pUnitGroup->Units();
	
   //Check which and how many units it can remove
	for(UINT32 i=0; i<l_vUnits.size(); i++)
	{
		SDK::Combat::GUnit*     l_pUnit   = l_vUnits[i];
		const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) l_pUnit->Design();
		
		if(l_HPAbleToRemoveByDesign.find(l_pDesign->Id()) == l_HPAbleToRemoveByDesign.end())
			l_HPAbleToRemoveByDesign[l_pDesign->Id()] = 0;

		l_HPAbleToRemoveByDesign[l_pDesign->Id()] += l_pUnit->HP();
	}	


   //Create a new military unit group.
   SP2::GUnitGroup*  l_pNewGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().CreateUnitGroup();
   {
	   gassert(l_pNewGroup,"GDataControlLayer::SplitUnitGroup(): New group couldn't be created. Invalid null pointer");
      l_pNewGroup->Id(g_Joshua.UnitManager().NewUnitGroupID());
      l_pNewGroup->OwnerId(in_pUnitGroup->OwnerId());
	   g_Joshua.UnitManager().SetGroupPosition(l_pNewGroup,in_pUnitGroup->Position());  
      l_pNewGroup->ChangeStatus(EMilitaryStatus::Ready);   
      g_Joshua.UnitManager().AddUnitGroup(l_pNewGroup);// Add this group to unit manager        
      m_UnitMover.AddGroup(l_pNewGroup);// Add this group to the unit mover
   }
	//For each of the units to split
	for(UINT32 i=0; i < in_vUnitsToMove.size(); i++)
	{
		// Split this unit
		SP2::GUnit* l_pUnitToSplit        = &in_vUnitsToMove[i];
		const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) l_pUnitToSplit->Design();
		if(l_HPAbleToRemoveByDesign.find(l_pDesign->Id()) != l_HPAbleToRemoveByDesign.end())
		{
			//That design exists in the old unit group, compute the HP that will have to be removed
         //from the original group
			UINT32 l_iHPNewUnit = min(l_pUnitToSplit->HP(),l_HPAbleToRemoveByDesign[l_pDesign->Id()]);
			if(l_HPToRemoveFromOriginalUnitGroup.count(l_pDesign->Id()) == 0)
				l_HPToRemoveFromOriginalUnitGroup[l_pDesign->Id()] = 0;
			l_HPToRemoveFromOriginalUnitGroup[l_pDesign->Id()] += l_iHPNewUnit;

			// Create new Unit and add it to the group
			SP2::GUnit* l_pNewUnit = (SP2::GUnit*)g_Joshua.UnitManager().CreateUnit();
         {
			   l_pNewUnit->Id(g_Joshua.UnitManager().NewUnitID());
			   l_pNewUnit->Design(l_pUnitToSplit->Design());
            gassert(l_iHPNewUnit,"HP should not be NULL");
			   l_pNewUnit->HP(l_iHPNewUnit);
            gassert(l_pUnitToSplit->Training() <= ETrainingLevel::Elite,"Invalid training");
			   l_pNewUnit->Training(l_pUnitToSplit->Training());
   			g_Joshua.UnitManager().AddNewUnit(l_pNewUnit);
			   g_Joshua.UnitManager().AddUnit2Group(l_pNewUnit,l_pNewGroup);
         }
		}		
	}
	
	//Remove HP from original unit group (cause we had some units out)
	for(UINT32 i=0; i<l_vUnits.size(); i++)
	{
		SDK::Combat::GUnit*     l_pUnit   = l_vUnits[i];
		const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) l_pUnit->Design();
		if(l_HPToRemoveFromOriginalUnitGroup.count(l_pDesign->Id()) > 0)
		{
			INT32 l_HpToRemove    = min(l_pUnit->HP(),l_HPToRemoveFromOriginalUnitGroup[l_pDesign->Id()]);
         //Unit is marked as modified in the SetUnitHP method
         g_Joshua.UnitManager().SetUnitHP(l_pUnit,l_pUnit->HP() - l_HpToRemove,true);
			l_HPToRemoveFromOriginalUnitGroup[l_pDesign->Id()] -= l_HpToRemove;
		}
	}	
	return l_pNewGroup->Id();
}

GUnit* GDataControlLayer::SplitUnit(GUnit* in_pUnitToSplit, INT32 in_iQuantityToGive, GUnitGroup* in_pDestinationGroup)
{   
   gassert(in_pUnitToSplit, "Invalid null pointer");
   gassert(in_pDestinationGroup,"Invalid null pointer");

   if(((SP2::GUnitGroup*)in_pUnitToSplit->Group())->Status() == EMilitaryStatus::Attacking ||
      ((SP2::GUnitGroup*)in_pUnitToSplit->Group())->Status() == EMilitaryStatus::Upgrading ||
      ((SP2::GUnitGroup*)in_pUnitToSplit->Group())->Status() == EMilitaryStatus::Training)
      return NULL; 

   // Split this unit
   const SP2::GUnitDesign* l_pDesign = (const SP2::GUnitDesign*) in_pUnitToSplit->Design();
   
   UINT32 l_iDesignHP     = l_pDesign->Type()->HP();
   UINT32 l_iOldUnitHP    = in_pUnitToSplit->HP();
   UINT32 l_iSecondUnitHP = in_iQuantityToGive * l_iDesignHP;
   l_iOldUnitHP -= l_iSecondUnitHP;
  
   gassert(l_iSecondUnitHP,"Should be HP");
   
   // Create new Unit (will be marked as modified in AddUnit2Group)
   SP2::GUnit* l_pNewUnit = (SP2::GUnit*)g_Joshua.UnitManager().CreateUnit();
   l_pNewUnit->Id(g_Joshua.UnitManager().NewUnitID());
   l_pNewUnit->Design(in_pUnitToSplit->Design());
   l_pNewUnit->HP(l_iSecondUnitHP);
   gassert(in_pUnitToSplit->Training() <= ETrainingLevel::Elite,"Invalid training");
   l_pNewUnit->Training(in_pUnitToSplit->Training());
   g_Joshua.UnitManager().AddNewUnit(l_pNewUnit);
   g_Joshua.UnitManager().AddUnit2Group(l_pNewUnit,in_pDestinationGroup);

   // split missiles between our units 
   REAL32 l_fRatioToGive = 1.f;
   if(in_pUnitToSplit->HP())
      l_fRatioToGive = ((REAL32)l_iSecondUnitHP / (REAL32)in_pUnitToSplit->HP());
   GiveMissilesFromUnit(in_pUnitToSplit, l_pNewUnit, l_fRatioToGive);

   //Set the HP of the original unit, (unit will be marked as modified in SetUnitHP)
   g_Joshua.UnitManager().SetUnitHP(in_pUnitToSplit, l_iOldUnitHP, ((SP2::GUnitGroup*)in_pUnitToSplit->Group())->Status() != EMilitaryStatus::ReadyToDeploy);

   return l_pNewUnit;
}

REAL32 GDataControlLayer::AverageHumanDevelopment() const
{
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
    REAL32 l_fTotalLE = 0.f;
    REAL32 l_fTotalMYS = 0.f;
    REAL32 l_fTotalEYS = 0.f;
    REAL64 l_fWorldGDP = 0;
    INT64 l_iWorldBabyPopulation = 0;
    INT64 l_iWorldUnder15Population = 0;
    INT64 l_iWorldOver15Population = 0;
    INT64 l_iWorldPopulation = 0;

	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		if(!g_ServerDAL.CountryIsValid(i))
			continue;

        GCountryData* l_pData = g_ServerDAL.CountryData(i);
        l_fTotalLE += l_pData->Population() * l_pData->BirthRate() * l_pData->LifeExpectancy();

        //Approximate MYS with population over 15, and EYS with population under 15 (should be 25 and 5 respectively)
        l_fTotalMYS += (l_pData->Pop1565() + l_pData->Pop65()) * l_pData->MeanYearsSchooling();
        l_fTotalEYS += l_pData->Pop15() * l_pData->ExpectedYearsSchooling();

        l_fWorldGDP += l_pData->GDPValue();

        l_iWorldBabyPopulation += static_cast<INT64>(l_pData->Population() * l_pData->BirthRate());
        l_iWorldUnder15Population += l_pData->Pop1565() + l_pData->Pop65();
        l_iWorldOver15Population += l_pData->Pop15();
        l_iWorldPopulation += l_pData->Population();
	}
    REAL32 l_fWorldLE = l_fTotalLE / l_iWorldBabyPopulation;
    REAL32 l_fWorldMYS = l_fTotalMYS / l_iWorldUnder15Population;
    REAL32 l_fWorldEYS = l_fTotalEYS / l_iWorldOver15Population;
    REAL64 l_fWorldGDPPerCapita = l_fWorldGDP / l_iWorldPopulation;
        
    return GCountryData::FindHumanDevelopment(l_fWorldLE, l_fWorldMYS, l_fWorldEYS, l_fWorldGDPPerCapita, true);
}

void GDataControlLayer::SendRelationHistory(UINT32 in_iPlayerID,UINT32 in_iCountry1,UINT32 in_iCountry2)
{
      GSymmetricMatrix<REAL32>* l_pHistoryTable = g_ServerDAL.RelationHistory();
      gassert(l_pHistoryTable,"SP2::GGeneralEventHandler::HandleRelationHistoryFetch History table shouldnt be null");


      SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GRelationHistoryFetch);
      l_Evt->m_iTarget          = in_iPlayerID;
      l_Evt->m_iSource          = SDK::Event::ESpecialTargets::Server;
      SP2::Event::GRelationHistoryFetch* l_pEvt = (SP2::Event::GRelationHistoryFetch*)l_Evt.get();


      //Resize the vector and fill with the requested values
      l_pEvt->m_vRelationsHistoryValues.resize(c_iRelationHistoryNumberOfValuesToKeep);
      for(UINT32 i = 0 ; i < c_iRelationHistoryNumberOfValuesToKeep ; i++)
      {
         l_pEvt->m_vRelationsHistoryValues[i] = l_pHistoryTable[i].Get(in_iCountry1-1,in_iCountry2-1);     
      }

      //Send the event
      l_pEvt->m_iCountryA = in_iCountry1;
      l_pEvt->m_iCountryB = in_iCountry2;
      g_Joshua.RaiseEvent(l_Evt);
}

/*
 * Send the building country list to a list of country
 */ 
void GDataControlLayer::SendBuildingCountriesList(const set<UINT32> &in_sCountries)
{
   // Notify list of building countries changes
	for(set<UINT32>::const_iterator l_It = in_sCountries.begin();
		l_It != in_sCountries.end();
		l_It++)
	{
      SendBuildingCountriesList(*l_It);
   }
}

/*
 * Send the building countries list
 */ 
bool GDataControlLayer::SendBuildingCountriesList(UINT32 in_iCountryId)
{
	// Get player to get country ID
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iCountryId);
   if(!l_pPlayer)
      return false;

   // create our event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GUpdateForeignBuildingCountryList);
   SP2::Event::GUpdateForeignBuildingCountryList* l_pUpdateEvent = (SP2::Event::GUpdateForeignBuildingCountryList*) (l_pEvent.get() );

   // retrieve the building countries
   set<UINT32> l_PossibleCountries;
	CountryCanOrderNewUnitsFrom(in_iCountryId, l_PossibleCountries);

	// Set Target and Source for this event
   l_pUpdateEvent->m_iTarget = l_pPlayer->Id();
   l_pUpdateEvent->m_iSource = SDK::Event::ESpecialTargets::Server;	
   l_pUpdateEvent->m_iForeignBuildingCountryList = l_PossibleCountries;

   // Raise It
   g_Joshua.RaiseEvent(l_pEvent);	

   // ok
   return true;
}

void GDataControlLayer::SetEligibleCountries(UINT32 in_iTreatyID, bool in_bCheckForNotAtWar)
{
	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	gassert(l_pTreaty,"GDataControlLayer::FindElligibleCountries() Treaty is null");

	set<UINT32>& l_EligibleCountries = l_pTreaty->EligibleCountries();
	l_EligibleCountries.clear();

	if(l_pTreaty->Ponctual())
		return;

	const UINT32* l_pConditions = l_pTreaty->Conditions();

	set<UINT32> l_vListOfCountries;
	l_vListOfCountries.insert(l_pTreaty->MembersSideA(false).begin(),l_pTreaty->MembersSideA(false).end());
	if(l_pTreaty->SideBCanLeave())
		l_vListOfCountries.insert(l_pTreaty->MembersSideB(false).begin(),l_pTreaty->MembersSideB(false).end());

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i <= l_iNbCountry; i++)
	{
		if(CountryCanJoinThoseConditions(i,l_pConditions,l_vListOfCountries,in_bCheckForNotAtWar))
			l_EligibleCountries.insert(i);
	}
	l_pTreaty->m_bEligibleDirty = true;	
}

void GDataControlLayer::CalculateMarketAvailability()
{
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	REAL64 l_MarketAvailability[EResources::ItemCount];
	Memory::Fill<REAL64>(l_MarketAvailability,0,EResources::ItemCount);

	GCountryData* l_pCountryData = NULL;
	for(UINT32 i=1; i<=l_iNbCountries; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;

		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			l_MarketAvailability[k] += max(0.f,l_pCountryData->ResourceProduction((EResources::Enum)k) - ( l_pCountryData->ResourceDemand((EResources::Enum)k) + l_pCountryData->ResourceExport((EResources::Enum)k) ) );
		}
	}
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		GCountryDataItf::m_pResourceMarketAvailability[i] = l_MarketAvailability[i];
	}
}

void GDataControlLayer::IterateDesiredExportsImports(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		if(!l_pCountryData->ResourceGvtCtrl((EResources::Enum)i) || 
			l_pCountryData->ResourceMeetConsumption((EResources::Enum)i))
		{
			if(l_pCountryData->ResourceProduction((EResources::Enum)i) > 
				l_pCountryData->ResourceDemand((EResources::Enum)i))
			{
				l_pCountryData->ResourceExportDesired((EResources::Enum)i,
					l_pCountryData->ResourceProduction((EResources::Enum)i) - 
					l_pCountryData->ResourceDemand((EResources::Enum)i));
				l_pCountryData->ResourceImportDesired((EResources::Enum)i,0.f);
			}
			else
			{
				l_pCountryData->ResourceImportDesired((EResources::Enum)i,
					l_pCountryData->ResourceDemand((EResources::Enum)i) - 
					l_pCountryData->ResourceProduction((EResources::Enum)i));
				l_pCountryData->ResourceExportDesired((EResources::Enum)i,0.f);
			}

		}
	}
	GAdvisor* l_pAdvisor = l_pCountryData->Advisor();
	if(!l_pAdvisor || l_pAdvisor->EconomicStatus() != EAdvisorStatus::Manual)
	{
		REAL64 l_fRevenuesWithoutTrade = l_pCountryData->BudgetRevenues() - l_pCountryData->BudgetRevenueTrade();
		REAL64 l_fExpensesWithoutTrade = l_pCountryData->BudgetExpenses() - l_pCountryData->BudgetExpenseImports();

		//Execute exports first
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			EResources::Enum l_Resource = (EResources::Enum)k;		
			if(!l_pCountryData->ResourceGvtCtrl(l_Resource))
			{
				l_fRevenuesWithoutTrade += (l_pCountryData->ResourceExport(l_Resource) + l_pCountryData->ResourceImport(l_Resource)) * (l_pCountryData->ResourceTaxes(l_Resource) + l_pCountryData->GlobalTaxMod());
			}
			else
			{
				REAL64 l_fBalance = l_pCountryData->ResourceProduction(l_Resource) - l_pCountryData->ResourceDemand(l_Resource);
				l_pCountryData->ResourceMeetConsumption(l_Resource,false);
				if(l_fBalance >= 0.f)
				{
					l_pCountryData->ResourceExportDesired(l_Resource,l_fBalance);
					l_fRevenuesWithoutTrade += l_fBalance;
					l_pCountryData->ResourceImportDesired(l_Resource,0.f);
				}
			}
		}

		//Then execute imports
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			EResources::Enum l_Resource = (EResources::Enum)k;		
			if(l_pCountryData->ResourceGvtCtrl(l_Resource))
			{
				REAL64 l_fBalance = l_pCountryData->ResourceProduction(l_Resource) - l_pCountryData->ResourceDemand(l_Resource);
				if(l_fBalance < 0.f)
				{
					l_pCountryData->ResourceExportDesired(l_Resource,0.f);
					if(l_fRevenuesWithoutTrade - l_fExpensesWithoutTrade > 0.f)
					{
						REAL64 l_fDesiredImport = min(abs(l_fBalance),l_fRevenuesWithoutTrade-l_fExpensesWithoutTrade);
						l_pCountryData->ResourceImportDesired(l_Resource,l_fDesiredImport);
						l_fExpensesWithoutTrade += l_fDesiredImport;
					}
					else
						l_pCountryData->ResourceImportDesired(l_Resource,0.f);
				}
			}
		}
	}
}

void GDataControlLayer::IterateBudgetRevenueTax(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);

	//(GDP * HumanDev (0..1) * Corruption (0..1) * taxes (0..1)) / constante
	REAL64 l_fRevenueTax = l_pCountryData->GDPValue() * (REAL64)l_pCountryData->HumanDevelopment()
		* l_pCountryData->PersonalIncomeTax() / SP2::c_fPersonalIncomeTaxConstant;

	l_pCountryData->BudgetRevenueTax(l_fRevenueTax);
}

void GDataControlLayer::IterateBudgetUnitUpkeep(UINT32 in_iCountryID)
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);

   if(!l_pCountryData->Activated() )
   {
      return;
   }

	REAL32 l_fTotalValue = 0.0f;

    REAL32 l_fMissileValue = g_ServerDAL.TotalMissileValue(l_pCountryData->CountryID());
    REAL32 l_fNuclearModifier = g_SP2Server->MilitaryUpkeepPercentages(EUnitCategory::Nuclear);
    if(l_fMissileValue > 0.f)
        GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": " +
                      L" Missile value " + GString::FormatNumber(l_fMissileValue/1000000.f, L",", L".", L"$", L"M", 3, 3) + L"; " +
                      L" modified value " + GString::FormatNumber(l_fMissileValue*l_fNuclearModifier/1000000.f, L",", L".", L"$", L"M", 3, 3),
                      EDZLogCat::Upkeep,
                      __FUNCTION__, __LINE__);
    l_fMissileValue *= l_fNuclearModifier;

	const set<UINT32>& l_vUnitGroupsID = g_Joshua.UnitManager().CountryUnitGroups(l_pCountryData->CountryID());
	for(set<UINT32>::const_iterator l_It = l_vUnitGroupsID.begin();
		l_It != l_vUnitGroupsID.end();
		l_It++)
	{
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)(g_Joshua.UnitManager().UnitGroup((*l_It)));

		const vector<SDK::Combat::GUnit*>& l_vUnits = l_pUnitGroup->Units();
		for(vector<SDK::Combat::GUnit*>::const_iterator l_It2 = l_vUnits.begin();
			l_It2 != l_vUnits.end();
			l_It2++)
		{									
			l_fTotalValue += UpkeepFeeForSingleUnit((SP2::GUnit*)(*l_It2));
		}		
	}

	l_fTotalValue += (PARKED * l_fMissileValue);
	l_pCountryData->BudgetExpenseUnitUpkeep(l_fTotalValue);
}

REAL32 GDataControlLayer::UpkeepFeeForSingleUnit(const SP2::GUnit* in_pUnit) const
{
	SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)in_pUnit->Group();
	REAL32 l_fCountryModifier = 0.f;		
	switch(l_pUnitGroup->Status())
	{
	case SP2::EMilitaryStatus::Ready:
	case SP2::EMilitaryStatus::Attacking:
	case SP2::EMilitaryStatus::Moving:
		l_fCountryModifier = SP2::READY;
		break;
	case SP2::EMilitaryStatus::Fortified:
		l_fCountryModifier = SP2::FORTIFIED;
		break;
	case SP2::EMilitaryStatus::Parked:
		l_fCountryModifier = SP2::PARKED;
		break;
	default:
		l_fCountryModifier = SP2::PARKED;
		break;
	}		

    const GCountryData* const l_pCountryData = g_ServerDAL.CountryData(l_pUnitGroup->OwnerId());
	l_fCountryModifier *= (l_pCountryData->HumanDevelopment() + SP2::HUMANDEVMOD);

	if(l_pCountryData->GvtType() == EGovernmentType::MilitaryDictatorship)
			l_fCountryModifier *= SP2::c_fGvtTypeMilitaryUpkeepRebates;

	REAL32 l_fUnitModifier = 1.f;
	switch(in_pUnit->Training())
	{
	case ETrainingLevel::Recruit:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierRecruit; break;
	case ETrainingLevel::Regular:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierRegular; break;
	case ETrainingLevel::Veteran:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierVeteran; break;
	case ETrainingLevel::Elite:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierElite; break;
   default:
      gassert(0,"Invalid unit training");
	}
	if(g_ServerDCL.IsUnitForSale(in_pUnit->Id()))
		l_fUnitModifier *= SP2::FORSALE;
	else if(g_ServerDCL.UnitMover().IsUnitInTraining(in_pUnit->Id()))
		l_fUnitModifier *= SP2::TRAINING;

    const GUnitDesign* const l_pDesign = dynamic_cast<const GUnitDesign*>(in_pUnit->Design());
    REAL32 l_fUpkeep = l_pDesign->Cost() * in_pUnit->Qty() * (l_fUnitModifier * l_fCountryModifier);
    REAL32 l_fUpkeepModifier = g_SP2Server->MilitaryUpkeepPercentages(l_pDesign->Type()->Category());

    GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L": " +
                  GString(in_pUnit->Qty()) + L" " + l_pDesign->Name() + L" " + g_ServerDAL.GetString(c_iUnitCategoryStringID[l_pDesign->Type()->Category()]) + L": " +
                  L" Units value " + GString::FormatNumber(l_fUpkeep/1000000.f, L",", L".", L"$", L"M", 3, 3) + L"; " +
                  L" modified value " + GString::FormatNumber(l_fUpkeep*l_fUpkeepModifier/1000000.f, L",", L".", L"$", L"M", 3, 3),
                  EDZLogCat::Upkeep,
                  __FUNCTION__, __LINE__);

	return l_fUpkeep * l_fUpkeepModifier;
}

void GDataControlLayer::CountryCanOrderNewUnitsFrom(UINT32 in_iOrderingCountry, set<UINT32>& out_PossibleCountries) const
{
	set<UINT32> l_Allies;
	g_ServerDAL.CountryAllies(in_iOrderingCountry,l_Allies);
	for(set<UINT32>::iterator it = l_Allies.begin(); 
		 it != l_Allies.end(); it++)
	{
		if(g_ServerDAL.CountryData(*it)->AcceptForeignOrder())
			out_PossibleCountries.insert(*it);
	}
	const UINT8* l_pDiplomatic = g_ServerDAL.CountryDiplomaticStatuses(in_iOrderingCountry);
	const hash_map<UINT32,SP2::GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	for(hash_map<UINT32,SP2::GTreaty>::const_iterator it = l_Treaties.begin();
		 it != l_Treaties.end(); it++)
	{
		const GTreaty& l_Treaty = it->second;
		if(l_Treaty.Active() && 
			l_Treaty.Type() == ETreatyType::WeaponTrade &&
			l_Treaty.CountrySide(in_iOrderingCountry) == 1 &&
			!l_Treaty.IsSuspended(in_iOrderingCountry))
		{
			for(set<UINT32>::const_iterator itr = l_Treaty.MembersSideA(true).begin();
					itr != l_Treaty.MembersSideA(true).end(); itr++)
			{
				if((EDiplomaticStatus::Enum)l_pDiplomatic[*itr] != EDiplomaticStatus::Hostile &&
					g_ServerDAL.CountryData(*itr)->AcceptForeignOrder())
					out_PossibleCountries.insert(*itr);
			}
		}
	}
	out_PossibleCountries.erase(in_iOrderingCountry);
}

REAL32 GDataControlLayer::TotalUnitsValueThatHateCountry(UINT32 in_iCountryID)
{
	if(m_TotalEnemiesByCountries.count(in_iCountryID))
		return m_TotalEnemiesByCountries[in_iCountryID];
	return 0.f;
}

void GDataControlLayer::IterateWars()
{
	set<UINT32> l_WarsToRemove;
	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();
	for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
			it != l_Wars.end(); it++)
	{
		const GWar& l_CurWar = it->second;
		if(VerifyPeaceStatus(&l_CurWar,l_WarsToRemove))
			l_WarsToRemove.insert(it->first);
	}
	for(set<UINT32>::iterator it = l_WarsToRemove.begin();
		 it != l_WarsToRemove.end(); it++)
	{
		g_ServerDAL.RemoveWar(*it);	
	}
}
void GDataControlLayer::LeaveWar(UINT32 in_iCountryID, UINT32 in_iWarID)
{
	GWar* l_pWar = g_ServerDAL.War(in_iWarID);
	if(!l_pWar)
		return;

	l_pWar->RemoveCountryFromAttackingSide(in_iCountryID);
	l_pWar->RemoveCountryFromDefendingSide(in_iCountryID);

	g_ServerDAL.ModifyWar(in_iWarID);

}

REAL32 GDataControlLayer::FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Enum in_eComplexity,
																	ECovertActionCellTraining::Enum in_eTraining,
																	ECovertActionsMissionType::Enum in_eMissionType,
																	ECovertActionsTargetSector::Enum in_eTargetSector,
																	bool in_bFraming,
																	UINT32 in_iTarget,
																	INT32 in_iSpecificSector,
                                                                    REAL32 in_fTotalTrainingOfAdditionalCells) const
{
	// Mission complexity modifier
	REAL32 l_fDifficultyModifier = 0.f;
	switch(in_eComplexity)
	{
	case ECovertActionsMissionComplexity::Low:
		l_fDifficultyModifier = SP2::c_fMissionComplexitySuccessModifierLow;
		break;
	case ECovertActionsMissionComplexity::Medium:
		l_fDifficultyModifier = SP2::c_fMissionComplexitySuccessModifierMedium;
		break;
	case ECovertActionsMissionComplexity::High:
		l_fDifficultyModifier = SP2::c_fMissionComplexitySuccessModifierHigh;
		break;
	case ECovertActionsMissionComplexity::Undefined:
		gassert(false,"Undefined target importance");
		return 0.f;
	}

	// Cell training modifier
	REAL32 l_fTrainingModifier = 0.f;
   switch(in_eTraining)
	{
   case ECovertActionCellTraining::Recruit:
		l_fTrainingModifier = SP2::c_fCovertActionsCellsRecruitSuccessRate; break;
	case ECovertActionCellTraining::Regular:
		l_fTrainingModifier = SP2::c_fCovertActionsCellsRegularSuccessRate; break;
	case ECovertActionCellTraining::Veteran:
		l_fTrainingModifier = SP2::c_fCovertActionsCellsVeteranSuccessRate; break;
	case ECovertActionCellTraining::Elite:
		l_fTrainingModifier = SP2::c_fCovertActionsCellsEliteSuccessRate; break;
	default:
		gassert(false,"GDataControlLayer::ExecuteMission(): Invalid training level");
		return 0.f;
	}

    // Additional cells
    l_fTrainingModifier += (in_eTraining < ECovertActionCellTraining::Elite) ? 
                           (0.5f - (0.5f / sqrt(in_fTotalTrainingOfAdditionalCells + 1.f))) :
                           log((in_fTotalTrainingOfAdditionalCells / 5.f) + 1.f);

	// Mission type modifier && Stability rate
	REAL32 l_fMissionBaseRate = 0.f;
	REAL32 l_fStabilityRate = 0.f;
	switch(in_eMissionType)
	{
	case ECovertActionsMissionType::Assassination:
		l_fMissionBaseRate = SP2::c_fMissionTypeAssassinationSuccessModifier; 
		break;
	case ECovertActionsMissionType::CoupEtat:
		l_fMissionBaseRate = SP2::c_fMissionTypeCoupEtatSuccessModifier; 
		l_fStabilityRate = g_ServerDAL.CountryData(in_iTarget)->GvtStability();
		break;
	case ECovertActionsMissionType::Espionage:
		l_fMissionBaseRate = SP2::c_fMissionTypeEspionageSuccessModifier; 
		if(in_eTargetSector == ECovertActionsTargetSector::Military)
		{
			if(!((EUnitCategory::Enum)in_iSpecificSector == EUnitCategory::ItemCount || in_iSpecificSector == -1))
				l_fMissionBaseRate *= SP2::c_fMissionTypeSpecificTargetModifier;
		}
		else
		{
			if(!((EResources::Enum)in_iSpecificSector == EResources::ItemCount || in_iSpecificSector == -1))
				l_fMissionBaseRate *= SP2::c_fMissionTypeSpecificTargetModifier;
		}			
		break;
	case ECovertActionsMissionType::Sabotage:
		l_fMissionBaseRate = SP2::c_fMissionTypeSabotageSuccessModifier; 
		if(in_eTargetSector == ECovertActionsTargetSector::Military)
		{
			if(!((EUnitCategory::Enum)in_iSpecificSector == EUnitCategory::ItemCount || in_iSpecificSector == -1))
				l_fMissionBaseRate *= SP2::c_fMissionTypeSpecificTargetModifier;
		}
		else
		{
			if(!((EResources::Enum)in_iSpecificSector == EResources::ItemCount || in_iSpecificSector == -1))
				l_fMissionBaseRate *= SP2::c_fMissionTypeSpecificTargetModifier;
		}			
		break;
	case ECovertActionsMissionType::SocialUpheaval:
		l_fMissionBaseRate = SP2::c_fMissionTypeSocialUpheavalSuccessModifier; 	
		break;
	case ECovertActionsMissionType::Terrorism:
		l_fMissionBaseRate = SP2::c_fMissionTypeTerrorismSuccessModifier; 
		break;
	default:
		gassert(false,"Invalid mission type");
		return 0.f;
	}
		

	REAL32 l_fSecurity = FindNationalSecurity(in_iTarget);	

	//Framing modifier	
	REAL32 l_fFrameModifier = 1.f;
	if(in_bFraming)
		l_fFrameModifier = SP2::c_fCountryToFrameModifier;

	return min(l_fDifficultyModifier * l_fFrameModifier * l_fTrainingModifier * l_fMissionBaseRate * (1.f - l_fSecurity) * (1.f - l_fStabilityRate), SP2::c_fMaximumSuccessRateAllowed);
}

void GDataControlLayer::StartNuclearResearch(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);

	REAL32 l_fNuclearLevel = l_pCountryData->NuclearReady();
    GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" will start nuclear research " +
                  L"with current nuclear level " + GString(l_fNuclearLevel),
                  EDZLogCat::Nuclear,
                  __FUNCTION__, __LINE__);

	if(l_fNuclearLevel >= 0.f)
	{
		//Nuclear research has already been started
		return;
	}

    if(g_SP2Server->DisableNuclearOnOccupy()                        &&
       l_pCountryData->NumberOfPoliticallyControlledRegions() == 0)
    {
        //Nuclear research cannot be started when fully occupied, and we're configured to disable nuclear research on occupy
        GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" can't start nuclear research due to being fully occupied",
                      EDZLogCat::Nuclear,
                      __FUNCTION__, __LINE__);
        return;
    }

	//Take money from country
	TakeMoney(in_iCountryID,SP2::c_fNuclearResearchCost);

	//Lose Relations
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		if( i != in_iCountryID )
		{
			g_ServerDAL.RelationBetweenCountries(in_iCountryID, i, 
				g_ServerDAL.RelationBetweenCountries(in_iCountryID, i) - SP2::c_fRelationsLossInNuclearStart);
		}
	}

	//Start research
	l_pCountryData->NuclearReady(0.f);

    GDZDebug::Log(l_pCountryData->NameAndIDForLog() + L" has started nuclear research",
                  EDZLogCat::Nuclear,
                  __FUNCTION__, __LINE__);
}


void GDataControlLayer::AcknowledgeRequest(UINT64 in_iRID, INT32 in_iTarget)
{
   // create the acknowledge event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GAnswerAckGameEvent);   
   // assign acknowledgement data
   l_pEvent->m_iTarget          = in_iTarget;
   l_pEvent->m_iSource          = SDK::Event::ESpecialTargets::Server;
   // retrieve a ptr to its struct
   SP2::Event::GAnswerAckGameEvent* l_pAckEvent = (SP2::Event::GAnswerAckGameEvent*)l_pEvent.get();
   l_pAckEvent->m_iRequestID                    = in_iRID;
   // raise it!
   g_Joshua.RaiseEvent(l_pEvent);
}

/*!
 * Request naval units (in parameter) to bombard the given region
 **/
bool GDataControlLayer::NavalUnitsBombardRegion(const vector<SDK::Combat::GUnitGroup*>& in_vGroups,
                                                UINT32 in_iRegionId)
{
   gassert(in_vGroups.size(),"Asked to bombard region without specifying units");

   // Can't bombard Antartica or water
   if(g_ServerDAL.RegionControl(in_iRegionId).m_iPolitical == 0)
   {
      return false;
   }

   const vector<GVector2D<REAL32> >& l_vBombardmentPoints = g_ServerDAL.RegionBombardmentPoints(in_iRegionId);
   if(!l_vBombardmentPoints.size())
   {
      return false;
   }

   //Build a list of the groups that can bombard, only Destroyers and Aircraft Carriers can 
   vector<SDK::Combat::GUnitGroup*> l_vValidUnitGroupsToBombard;
   {
      for(UINT32 i = 0 ; i < in_vGroups.size(); i++)
      {
         SP2::GUnitGroup* l_pSP2Group = (SP2::GUnitGroup*)in_vGroups[i];

         if(l_pSP2Group->CanPerformOffShoreBombardment())
         {
            // We check if the current group is already bombarding another region, if so we
            //  stop it.  This fixes the problem of bombardments that continue even when
            //  no groups are bombing a given region
            if(((SP2::GUnitGroup*)in_vGroups[i])->RegionToBombard())
               g_ServerDCL.UnitMover().RemoveBombardingGroup(((SP2::GUnitGroup*)in_vGroups[i])->Id());

            //Set the region to bombard
            ((SP2::GUnitGroup*)in_vGroups[i])->RegionToBombard((UINT16)in_iRegionId);

            l_vValidUnitGroupsToBombard.push_back(in_vGroups[i]);
         }
      }
   }


   //If there are units that can bombard
   if(l_vValidUnitGroupsToBombard.size())
   {
      //Declare war between the region owner and the attacker
      {
         GRegionControl l_RegionControl = g_ServerDAL.RegionControl(in_iRegionId);
         set<UINT32> l_Attackers;
         l_Attackers.insert(l_vValidUnitGroupsToBombard[0]->OwnerId());
			DeclareNewWar(l_Attackers,l_vValidUnitGroupsToBombard[0]->OwnerId(),l_RegionControl.m_iPolitical);
      }
   }


   //Find the closest bombardment points for every unit group, and make the units move to the point
   for(UINT32 i = 0 ; i < l_vValidUnitGroupsToBombard.size() ; i++)
   {
      GVector2D<REAL32> l_vClosestPoint;
      g_ServerDAL.RandomBombardmentPoint(in_iRegionId,l_vClosestPoint);

      

      //Ask the unit to move to that position
      vector<SDK::Combat::GUnitGroup*> l_vGroupToMove;
      l_vGroupToMove.push_back(l_vValidUnitGroupsToBombard[i]);

      gassert(!g_ServerDCL.EarthLocateRegion(l_vClosestPoint.x,l_vClosestPoint.y),"Should be nULL");

      g_ServerDCL.UnitMover().MoveUnits(l_vGroupToMove,l_vClosestPoint,EMilitaryAction::BombardRegion);
   }

   return false;
}

bool GDataControlLayer::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_vNewsNukesAttacks
             << m_NumberOfDeadByCountry
             << m_UnitListForSell;

   return true;
}

bool GDataControlLayer::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_vNewsNukesAttacks
             >> m_NumberOfDeadByCountry
             >> m_UnitListForSell;

   return true;
}

bool GDataControlLayer::OnNew(GDatabase* in_pDatabase)
{
   if(!g_SP2Server->UpdateStaticOnly() )
   {
      //Calculate the initial treaty cost
      const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
      for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
         it != l_Treaties.end();++ it)
      {
         UpdateCostByCountryInsideTreaty(it->first);
         SetEligibleCountries(it->first, false);
      }

      UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
      for(UINT32 i=1; i<=l_iNbCountry; i++)
      {
         IterateBudgetUnitUpkeep(i);
      }
   }

   return true;
}

void GDataControlLayer::OnClean()
{
   m_TotalEnemiesByCountries.clear();
   m_vNewsNukesAttacks.clear();
   m_NumberOfDeadByCountry.clear();
   m_UnitListForSell.clear();
}

REAL32 GDataControlLayer::PressureStrengthTreaty(const GTreaty* in_pTreaty, UINT32 in_iCountryID) const
{
	const set<UINT32>& l_Pressure = in_pTreaty->MembersPressure();
	REAL32 l_fPressureStrength = 0.f;
	for(set<UINT32>::const_iterator it = l_Pressure.begin();
		 it != l_Pressure.end(); it++)
	{
		l_fPressureStrength += g_ServerDAL.TotalUnitsValue(*it);
	}
	l_fPressureStrength /= (g_ServerDAL.TotalUnitsValue(in_iCountryID) + 1.f);
	l_fPressureStrength = min(l_fPressureStrength,1.f);
	return l_fPressureStrength;
}


/*!
 * Sum up the characteristics needed to perform (attack) offshore bombardment
 **/
UINT32 GDataControlLayer::BombardmentAttackSumCharacteristics(SP2::GUnitGroup* in_pGroup)
{
   UINT32 l_iSum = 0;
   UINT32 l_iPrecisionTotal      = 0;
   UINT32 l_iAverageDenominator  = 0;
   gassert(in_pGroup->CanPerformOffShoreBombardment(),"Group should be able to perform offshore bombardments");

   //Make the total of the gunrange for the attacking group
   for(UINT32 i = 0 ;i < in_pGroup->Units().size() ; i++)
   {
      const SP2::GUnit*       l_pUnit   = (const SP2::GUnit*)in_pGroup->Units()[i];
      SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();

      switch(l_pDesign->Type()->Id())
      {
         case EUnitType::AircraftCarrier:
         case EUnitType::Destroyer:
            l_iSum += l_pDesign->Sensors();
            l_iSum += l_pDesign->GunDamage();
            l_iSum += l_pDesign->GunRange();
            l_iSum += l_pDesign->MissileDamage();
            l_iSum += l_pDesign->MissileRange();
            l_iPrecisionTotal += l_pDesign->GunPrecision();
            l_iPrecisionTotal += l_pDesign->MissilePrecision();
            l_iAverageDenominator += 2;
         default:
            break;
      }
   }

   REAL32 l_fPrecisionAverage = max(1,l_iPrecisionTotal * 1.f / l_iAverageDenominator);
   return (UINT32)(l_fPrecisionAverage*l_iSum);
}

/*!
 * Sum up the characteristics needed to defend from offshore bombardment
 **/
UINT32 GDataControlLayer::BombardmentDefendSumCharacteristics(SP2::GUnitGroup* in_pGroup)
{
   UINT32 l_iSum = 0;

   //Make the total of the gunrange for the attacking group
   for(UINT32 i = 0 ;i < in_pGroup->Units().size() ; i++)
   {
      const SP2::GUnit*       l_pUnit   = (const SP2::GUnit*)in_pGroup->Units()[i];
      SP2::GUnitDesign*       l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();

      l_iSum += l_pDesign->SpeedAndManeuverability();
      l_iSum += l_pDesign->Armor();
   }

   //Affect the result with the status modifier (ready, parked, fortified)
   REAL32 l_fModifier = 1.f;
   switch(in_pGroup->CombatStatus())
   {
      case EMilitaryStatus::Parked:
      case EMilitaryStatus::Ready:
      case EMilitaryStatus::Fortified:
         l_fModifier = c_GroupMilitaryStatusModifiers[in_pGroup->CombatStatus()];
         break;
      default:
         break;
   }

   return (UINT32)(l_iSum*l_fModifier);
}

/*!
 * Lists points in the region where explosions can be
 **/
void GDataControlLayer::BombardmentExplosionsPointsInRegion(UINT32 in_iRegionId,vector<GVector2D<REAL32> >& out_vPoints) const
{
   //Fetch all the unit groups in that region
   const set<UINT32>& l_GroupsInRegion = m_UnitMover.m_pRegionGroups[in_iRegionId];
   for(set<UINT32>::const_iterator l_It = l_GroupsInRegion.begin();
                                   l_It != l_GroupsInRegion.end();
                                   l_It++)
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_It);
      out_vPoints.push_back(l_pGroup->Position());      
   }

   //Fetch the waypoints in that region
   const vector<vector<UINT32> >& l_vRegionVertices = m_UnitMover.m_vRegionVertices;

   //Populate the list of points where bombardments can appear
   for(UINT32 i = 0 ; i < l_vRegionVertices[in_iRegionId].size(); i++)
   {
      UINT32 l_iVerticeId = l_vRegionVertices[in_iRegionId][i];
      out_vPoints.push_back(m_UnitMover.m_vRegionGraph[l_iVerticeId].m_Center);
   }   
}

void GDataControlLayer::TradeProposal(GTradeData& in_Trade)
{
	in_Trade.Status(SP2::ETradeStatus::Pending);
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
	SP2::Event::GEventTradeProposal* l_pTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
	l_pTrade->m_trdData = in_Trade;
	g_Joshua.RaiseEvent(l_Event);
}

void GDataControlLayer::TradeAccept(GTradeData& in_TradeToAccept)
{	
	in_TradeToAccept.Status(SP2::ETradeStatus::Accepted);
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
	SP2::Event::GEventTradeProposal* l_pTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
	l_pTrade->m_trdData = in_TradeToAccept;
	g_Joshua.RaiseEvent(l_Event);
}

void GDataControlLayer::TradeRefuse(GTradeData& in_TradeToRefuse)
{
	in_TradeToRefuse.Status(SP2::ETradeStatus::Refused);
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
	SP2::Event::GEventTradeProposal* l_pTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
	l_pTrade->m_trdData = in_TradeToRefuse;
	g_Joshua.RaiseEvent(l_Event);	
}

void GDataControlLayer::UpdateWorldPeaceStatus(bool in_bWorldAtPeace)
{
   NotifyObserver(c_iWorldPeaceStatusChanged, (void*)&in_bWorldAtPeace);
}

/*!
 * Split the nuclear missiles from the in_pUnitSrc between the in_pUnitSrc and in_pUnitDest
 **/
void GDataControlLayer::GiveMissilesFromUnit(GUnit* in_pUnitSrc, GUnit* in_pUnitDest, REAL32 in_fRatioToGive)
{
   // if we have a source, a dest and a valid dest group
   if(in_pUnitSrc && in_pUnitDest && in_pUnitDest->Group())
   {
      // retrive our design and unit group info
      const SP2::GUnitDesign* l_pDesignSrc  = (const SP2::GUnitDesign*)in_pUnitSrc->Design();
      const SP2::GUnitDesign* l_pDesignDest = (const SP2::GUnitDesign*)in_pUnitDest->Design();
      const SP2::GUnitGroup*  l_pGroupDest  = (const SP2::GUnitGroup*)in_pUnitDest->Group();

      // are we splitting between 2 nuclears subs 
      if(l_pDesignSrc->Type()->Id()  == c_iNuclearSubmarineUnitTypeID &&
         l_pDesignDest->Type()->Id() == c_iNuclearSubmarineUnitTypeID)
      {    
         // get a ref to our country submarines missiles array
         vector<SP2::GNuclearMissile> &l_vTotalSubsMissiles = g_ServerDAL.CountryData(l_pGroupDest->OwnerId())->m_vSubmarineBasedNuclearMissiles;
         // the missiles of our original sub
         vector<SP2::GNuclearMissile*> l_vOriginalSubMissiles;

         // fetch the missiles which belong to our original sub
         for(UINT32 i = 0; i < l_vTotalSubsMissiles.size(); i++)
         {
            if(l_vTotalSubsMissiles[i].AttachedSubmarine() == in_pUnitSrc->Id())
            {
               l_vOriginalSubMissiles.push_back(&l_vTotalSubsMissiles[i]);
            }
         }

         // do we have missiles to split ?
         if(l_vOriginalSubMissiles.size() && in_fRatioToGive)
         {
            // missiles count to give
            UINT32 l_iMissilesCountToGive = (UINT32)(l_vOriginalSubMissiles.size() * in_fRatioToGive);
            l_iMissilesCountToGive        = min(l_iMissilesCountToGive, l_vOriginalSubMissiles.size());
            // attach requested missiles count to our new sub
            for(UINT32 i = 0; i < l_iMissilesCountToGive; i++)
            {
               l_vOriginalSubMissiles[i]->AttachedSubmarine(in_pUnitDest->Id());
            }
         }
      }
   }
}

REAL64 GDataControlLayer::GetRevenuesForEconomicFailure(UINT32 in_iCountryID) const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_pCountryData)
		return 0.f;

	REAL64 l_fRevenues     = l_pCountryData->BudgetRevenueTrade() + 
		l_pCountryData->BudgetRevenueIMF() + 
		l_pCountryData->BudgetRevenueTourism();

    REAL32 l_fHI = max(0, min((l_pCountryData->LifeExpectancy() - 20) / 65, 1));
    REAL32 l_fMeanYearsIndex = max(0, min(l_pCountryData->MeanYearsSchooling() / 15, 1));
    REAL32 l_fExpectedYearsIndex = max(0, min(l_pCountryData->ExpectedYearsSchooling() / 18, 1));
    REAL32 l_fEI = (l_fMeanYearsIndex + l_fExpectedYearsIndex) / 2;

    REAL32 l_fHumanDevelopmentForTest = powf(l_fHI * l_fEI, 0.5f);
    l_fHumanDevelopmentForTest = max(l_fHumanDevelopmentForTest, l_pCountryData->HumanDevelopment());

	l_fRevenues += l_pCountryData->OptimalGDPValue() * l_fHumanDevelopmentForTest
      * l_pCountryData->PersonalIncomeTax() / SP2::c_fPersonalIncomeTaxConstant;

	return l_fRevenues;

}

void GDataControlLayer::LiberateRegions(UINT32 in_iCountryLiberating,
													 UINT32 in_iCountryLiberated)
{
	//Check if the country really has 75%...
	if(g_ServerDAL.ProportionGainedByCountryFromCountry(in_iCountryLiberating, in_iCountryLiberated) >= SP2::c_fPercentageNecessaryToChangeGovernmentType  &&
		g_ServerDAL.DiplomaticStatus(in_iCountryLiberating,in_iCountryLiberated) != EDiplomaticStatus::Hostile)
	{
		//Player has asked for a country liberation, so free regions
		const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryLiberated);
		for(set<UINT32>::const_iterator it = l_PoliticalRegions.begin();
			it != l_PoliticalRegions.end(); it++)
		{
			if(g_ServerDAL.RegionControl(*it).m_iMilitary == in_iCountryLiberating)
				g_ServerDCL.ChangeRegionMilitaryControl(*it,in_iCountryLiberated);
		}

		//MultiMOD
		//Adding tresspassing treaty & raising relationships
        GString l_sNewTreatyName = g_ServerDAL.TreatyName(ETreatyType::MilitaryAccess,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_iCountryLiberated)->NameID()));
		UINT32 l_Conditions[ETreatyConditions::ItemCount];
		Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);
		l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
		set<UINT32> l_SideAMembers;
		set<UINT32> l_SideBMembers;
		set<UINT32> l_SidePressure;
		l_SideAMembers.insert(in_iCountryLiberated);
		l_SideBMembers.insert(in_iCountryLiberating);
		g_ServerDCL.CreateNewTreaty(in_iCountryLiberated,l_SideAMembers,l_SideBMembers,l_SidePressure,ETreatyType::MilitaryAccess,true,l_sNewTreatyName,l_Conditions);

		g_ServerDAL.RelationBetweenCountries(in_iCountryLiberating, in_iCountryLiberated, 
				g_ServerDAL.RelationBetweenCountries(in_iCountryLiberating, in_iCountryLiberated) + 100 );
	}
}

void GDataControlLayer::ChangeCountryName(ENTITY_ID in_iCountryID, const GString& in_sNewName)
{
    if(g_SP2Server->CountryNameChangeMode() != ECountryNameChangeMode::Off &&
       !in_sNewName.empty())
    {
        gassert(in_iCountryID >= 1,"Invalid country ID, name change won't work");

        //See if the new country's name is taken already. If yes, then the name change will not occur
        const UINT16 l_iNbCountry = g_ServerDAL.NbCountry();
        bool l_bNameTakenAlready = false;
        for(ENTITY_ID i = 1; i < l_iNbCountry; i++)
        {
            if(g_ServerDAL.CountryData(i)->Name() == in_sNewName)
            {
                l_bNameTakenAlready = true;
                break;
            }
        }

        if(!l_bNameTakenAlready)
        {
            GCountryData* const l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
            const GString l_sOldName = l_pCountryData->Name();

            //Change GCountryData name to allow saving the new name to save files
            l_pCountryData->Name(in_sNewName);

            //Change GCountry name to allow client display of the new name
            //g_SP2Server->Countries() is 0-based
            g_SP2Server->Countries().at(in_iCountryID - 1).Name(in_sNewName);

            SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iCountryID);
            g_Joshua.Log(L"Country ID " + GString(in_iCountryID) +
                ((l_pPlayer != NULL) ?
                (L", played by player ID " + GString(l_pPlayer->Id()) + L", " + l_pPlayer->Name() + L", ") :
            L" ") +
                L"has changed its name from " +
                l_sOldName + L" to " + in_sNewName);

            SendCountryList();
        }
    }
}

void GDataControlLayer::ChangePlayerName(SDK::GPlayer* in_pPlayer, const GString& in_sNewName)
{
    if(!in_sNewName.empty() && in_pPlayer->Name() != in_sNewName)
    {
        GDZDebug::Log(L"Player ID " + GString(in_pPlayer->Id()) + L", " +
                      in_pPlayer->Name() + L", " +
                      L"is changing name to " +
                      in_sNewName,
                      EDZLogCat::Player,
                      __FUNCTION__, __LINE__);

        in_pPlayer->Name(in_sNewName);

        //Not sure why GGameClientItf::PlayerName() needs a non-const GString
        GString l_sNewName(in_sNewName);
        in_pPlayer->Client()->PlayerName(l_sNewName);

        g_SP2Server->SendPlayersList();
    }
}

void GDataControlLayer::SendCountryList(INT32 in_iTarget) const
{
    SDK::GGameEventSPtr l_ReceiveCountryListEvent = CREATE_GAME_EVENT(Event::GReceiveCountryList);
    l_ReceiveCountryListEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
    l_ReceiveCountryListEvent->m_iTarget = in_iTarget;

    Event::GReceiveCountryList* l_pCntrListEvent = reinterpret_cast<Event::GReceiveCountryList*>(l_ReceiveCountryListEvent.get());
    l_pCntrListEvent->m_vCountries = g_SP2Server->Countries();

    //Display client states' names with whom they're a client of
    //Won't affect their server-side name
    for(ENTITY_ID i = 0; i < l_pCntrListEvent->m_vCountries.size(); i++)
    {
        const ENTITY_ID l_iMaster = g_ServerDAL.CountryData(i + 1)->Master().first;
        if(g_ServerDAL.CountryValidityArray(i + 1) &&
           l_iMaster != 0)
        {
            GCountry& l_Country = l_pCntrListEvent->m_vCountries[i];
            GString l_sNewName = l_Country.Name();
            l_sNewName.append(L", client of " +
                              g_ServerDAL.CountryData(l_iMaster)->Name());
            l_Country.Name(l_sNewName);
        }
    }

    g_Joshua.RaiseEvent(l_ReceiveCountryListEvent);

    //Libraries automatically change all GCountry objects to active for GReceiveCountryList event, so we need to correct for that
    for(INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
    {
        if(!g_ServerDAL.CountryValidityArray(i))
        {
            SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GConquerCountry);
            Event::GConquerCountry* l_ConquerEvent = reinterpret_cast<Event::GConquerCountry*>(l_Event.get());
            l_ConquerEvent->m_iConqeredID = i;
            l_ConquerEvent->m_iConqueringID = 0;
            l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
            l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
            g_Joshua.RaiseEvent(l_Event);
        }
    }
}

void GDataControlLayer::MakeClientState(ENTITY_ID in_iMaster, ENTITY_ID in_iClient, UINT32 in_iTreaty, bool in_bLoadingGame)
{
    GCountryData* const l_pClientData = g_ServerDAL.CountryData(in_iClient);
    GCountryData* const l_pMasterData = g_ServerDAL.CountryData(in_iMaster);
    gassert(in_iMaster != in_iClient,
            l_pClientData->NameAndIDForLog() + L" can't be its own client!");

    l_pClientData->Master(in_iMaster, in_iTreaty);
    l_pMasterData->AddClient(in_iClient, in_iTreaty);

    if(!in_bLoadingGame)
    {
        GDZLOG(l_pClientData->NameAndIDForLog() + L" is now a client of " +
               l_pMasterData->NameAndIDForLog(),
               EDZLogCat::ClientStates);

        //Leave all wars, but join wars on same side as master
        auto& l_mWars = g_ServerDAL.m_CurrentWars;
        for(auto it = l_mWars.begin(); it != l_mWars.end(); ++it)
        {
            GWar& l_War = it->second;

            if(l_War.MasterAttacking() == in_iClient ||
               l_War.MasterDefending() == in_iClient)
            {
                GDZLOG(l_pClientData->NameAndIDForLog() + L" is leaving the war " +
                       g_ServerDAL.WarInfoForLog(it->first, true) +
                       L" in which it's a master",
                       EDZLogCat::ClientStates);

                //Call ChangeOpinionOnWar instead of DeclarePeace, as DeclarePeace alone isn't sufficient to clear war status, and ChangeOpinionOnWar calls down to DeclarePeace via VerifyPeaceStatus anyway
                ChangeOpinionOnWar(l_War.MasterAttacking(), it->first, true);
                ChangeOpinionOnWar(l_War.MasterDefending(), it->first, true);
            }
            else
            {
                if(l_War.AttackingSide().count(in_iClient) == 1)
                {
                    GDZLOG(l_pClientData->NameAndIDForLog() + L" is leaving the war " +
                           g_ServerDAL.WarInfoForLog(it->first, true),
                           EDZLogCat::ClientStates);

                    l_War.RemoveCountryFromAttackingSide(in_iClient);
                    g_ServerDAL.ModifyWar(it->first);
                }
                else if(l_War.DefendingSide().count(in_iClient) == 1)
                {
                    GDZLOG(l_pClientData->NameAndIDForLog() + L" is leaving the war " +
                           g_ServerDAL.WarInfoForLog(it->first, true),
                           EDZLogCat::ClientStates);

                    l_War.RemoveCountryFromDefendingSide(in_iClient);
                    g_ServerDAL.ModifyWar(it->first);
                }

                if(l_War.AttackingSide().count(in_iMaster) == 1)
                    JoinAWar(in_iClient, it->first, 1, !l_War.MasterAttackingWantsPeace());
                else if(l_War.DefendingSide().count(in_iMaster) == 1)
                    JoinAWar(in_iClient, it->first, 2, !l_War.MasterDefendingWantsPeace());
            }
        }

        GDZLOG(l_pClientData->NameAndIDForLog() + L" has left all previous wars and joined all wars on " +
               l_pMasterData->NameAndIDForLog() + L"'s side",
               EDZLogCat::ClientStates);

        //Free all regions of the client state
        const auto& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iClient);
        for(auto l_It = l_vRegions.cbegin(); l_It != l_vRegions.cend(); ++l_It)
        {
            if(g_ServerDAL.RegionControlArray()[*l_It].m_iMilitary == in_iMaster)
                ChangeRegionMilitaryControl(*l_It, in_iClient, false);
        }

        GDZLOG(l_pClientData->NameAndIDForLog() + L" has regained its regions occupied by " + l_pMasterData->NameAndIDForLog(),
               EDZLogCat::ClientStates);

        //Synchronize the 2 countries
        l_pClientData->SynchronizeWithRegions();
        l_pMasterData->SynchronizeWithRegions();

        GDZLOG(L"Synchronized regions of " + l_pClientData->NameAndIDForLog() + L" and " + l_pMasterData->NameAndIDForLog(),
               EDZLogCat::ClientStates);

        //Client state will start off with same relations
        for(ENTITY_ID i = 1; i <= g_ServerDAL.NbCountry(); i++)
            g_ServerDAL.RelationBetweenCountries(in_iClient, i, g_ServerDAL.RelationBetweenCountries(in_iMaster, i));

        GDZLOG(L"Synchronized relations of " + l_pClientData->NameAndIDForLog() + L" and " + l_pMasterData->NameAndIDForLog(),
               EDZLogCat::ClientStates);
    }

    //Send country list to update the client state's name (with "client of (name)")
    if(g_SP2Server->CountryNameChangeMode() != ECountryNameChangeMode::Off)
        SendCountryList();

    GDZLOG(L"<<<",
           EDZLogCat::ClientStates);
}
