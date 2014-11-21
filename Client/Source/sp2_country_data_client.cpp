/**************************************************************
*
* sp2_country_data_client.cpp
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

REAL64 GCountryDataItf::m_pResourceMarketAvailability[EResources::ItemCount];

GCountryDataClient::GCountryDataClient()
{
   m_pResearchInformation     = new GResearchInformationItf();
   m_bConstitutionalFormDirty = false;
   m_bReceivedAtLeastOneSync  = false;
}
GCountryDataClient::~GCountryDataClient()
{
   SAFE_DELETE(m_pResearchInformation);
}

GCountryDataItf* GCountryDataClient::New()
{
   return new GCountryDataClient();
}


bool GCountryDataClient::ReceivedAtLeastOneSync() const
{
   return m_bReceivedAtLeastOneSync;
}
void GCountryDataClient::ReceivedAtLeastOneSync(bool in_bVal)
{
   m_bReceivedAtLeastOneSync = in_bVal;
}


/*!
 * Updates the player country data with the new synchronized data from the network
 * Make sure only the modified data is copied
 **/
void GCountryDataClient::CopyModifiedData(const GCountryDataClient& in_Original)
{
   m_fWhenSent                      = in_Original.m_fWhenSent;
   
   m_bNuclearMissilesDataDirty      = in_Original.m_bNuclearMissilesDataDirty;
   m_bEconomicDataDirty             = in_Original.m_bEconomicDataDirty ;
   m_bBudgetDataDirty               = in_Original.m_bBudgetDataDirty;
   m_bOtherDataDirty                = in_Original.m_bOtherDataDirty ;
   m_bResourceDataDirty             = in_Original.m_bResourceDataDirty ;
   m_bCovertActionCellsDirty        = in_Original.m_bCovertActionCellsDirty;
   m_fResourceDemandModifier        = in_Original.m_fResourceDemandModifier;
   m_fResourceProductionModifier    = in_Original.m_fResourceProductionModifier;

   if(in_Original.m_bEconomicDataDirty)
   {
      m_iEconomicRank                     = in_Original.m_iEconomicRank;
      m_fEconomicStrength                 = in_Original.m_fEconomicStrength;
      m_fInterestLevel                    = in_Original.m_fInterestLevel ;
      m_fPersonalIncomeTax                = in_Original.m_fPersonalIncomeTax;
      m_fInflationLevel                   = in_Original.m_fInflationLevel;
      m_fInflationLevelExpected           = in_Original.m_fInflationLevelExpected ;
      m_fGDPValue                         = in_Original.m_fGDPValue ;
		m_fDemandTotal								= in_Original.m_fDemandTotal;
		m_fGDPValueBase							= in_Original.m_fGDPValueBase;
      m_fGDPGrowth                        = in_Original.m_fGDPGrowth ;
      m_fPopEconomicModel                 = in_Original.m_fPopEconomicModel  ;
      m_fCountryEconomicModel             = in_Original.m_fCountryEconomicModel ;
      m_fGlobalTaxMod                     = in_Original.m_fGlobalTaxMod;
   }

   if(in_Original.m_bBudgetDataDirty)
   {
      m_fBudgetExpenseSecurity            = in_Original.m_fBudgetExpenseSecurity ;
      m_fBudgetExpenseDiplomacy           = in_Original.m_fBudgetExpenseDiplomacy ;
      m_fBudgetExpenseImports             = in_Original.m_fBudgetExpenseImports ;
      m_fBudgetExpenseUnitUpkeep          = in_Original.m_fBudgetExpenseUnitUpkeep ;
		m_fBudgetExpenseCorruption				= in_Original.m_fBudgetExpenseCorruption;
      m_fBudgetExpenseRatioTelecom        = in_Original.m_fBudgetExpenseRatioTelecom;
      m_fBudgetExpenseRatioResearch       = in_Original.m_fBudgetExpenseRatioResearch ;
      m_fBudgetExpenseRatioInfrastructure = in_Original.m_fBudgetExpenseRatioInfrastructure ;
      m_fBudgetExpenseRatioIMF            = in_Original.m_fBudgetExpenseRatioIMF ;
      m_fBudgetExpenseRatioGovernment     = in_Original.m_fBudgetExpenseRatioGovernment ;
      m_fBudgetExpenseRatioHealthcare     = in_Original.m_fBudgetExpenseRatioHealthcare ;
      m_fBudgetExpenseRatioEducation      = in_Original.m_fBudgetExpenseRatioEducation ;
      m_fBudgetExpenseRatioEnvironment    = in_Original.m_fBudgetExpenseRatioEnvironment ;
      m_fBudgetExpenseRatioPropaganda     = in_Original.m_fBudgetExpenseRatioPropaganda ;
      m_fBudgetExpenseRatioTourism        = in_Original.m_fBudgetExpenseRatioTourism;
      m_fBudgetRevenueTax                 = in_Original.m_fBudgetRevenueTax ;
      m_fBudgetRevenueTrade               = in_Original.m_fBudgetRevenueTrade ;
      m_fBudgetRevenueIMF                 = in_Original.m_fBudgetRevenueIMF ;
      m_fBudgetRevenueTourism             = in_Original.m_fBudgetRevenueTourism;
      m_fMonetarySupplyAmount             = in_Original.m_fMonetarySupplyAmount;
   }

   if(in_Original.m_bNuclearMissilesDataDirty)
   {
      m_vGroundBasedNuclearMissiles    = in_Original.m_vGroundBasedNuclearMissiles;
      m_vSubmarineBasedNuclearMissiles = in_Original.m_vSubmarineBasedNuclearMissiles;
   }

   if(in_Original.m_bOtherDataDirty)
   {

		m_AccuracyOfInformation    = in_Original.m_AccuracyOfInformation;
		m_iClimateNameID				= in_Original.m_iClimateNameID;
      m_iCountryID               = in_Original.m_iCountryID ;
      m_iNameID                  = in_Original.m_iNameID ;
      m_iPoliticRank             = in_Original.m_iPoliticRank  ;
      m_iMilitaryRank            = in_Original.m_iMilitaryRank ;
      m_fPoliticStrength         = in_Original.m_fPoliticStrength ;
      m_fMilitaryStrength        = in_Original.m_fMilitaryStrength;
      m_iPop15                   = in_Original.m_iPop15 ;
      m_iPop1565                 = in_Original.m_iPop1565 ;
      m_iPop65                   = in_Original.m_iPop65 ;   
      m_fTelecomLevel            = in_Original.m_fTelecomLevel ;
      m_fGvtApproval             = in_Original.m_fGvtApproval ;
      m_fGvtApprovalExpected     = in_Original.m_fGvtApprovalExpected;
      m_fInfrastructure          = in_Original.m_fInfrastructure ;
      m_fBirthRate               = in_Original.m_fBirthRate ;
		m_fBirthRateExpected			= in_Original.m_fBirthRateExpected;
      m_fDeathRate               = in_Original.m_fDeathRate ;
		m_fDeathRateExpected       = in_Original.m_fDeathRateExpected ;
      m_fHumanDevelopment        = in_Original.m_fHumanDevelopment ;
      m_fHumanDevelopmentAvg     = in_Original.m_fHumanDevelopmentAvg ;
      m_iMiliManpowerAvailable   = in_Original.m_iMiliManpowerAvailable ;
      m_fPop1565Unemployed       = in_Original.m_fPop1565Unemployed;
      m_fPopInPoverty            = in_Original.m_fPopInPoverty ;
      m_fGvtStability            = in_Original.m_fGvtStability ;
      m_fGvtStabilityExpected    = in_Original.m_fGvtStabilityExpected ;
      m_iSuffrage                = in_Original.m_iSuffrage ;
      
      if(m_iGvtType != in_Original.m_iGvtType)
      {
         m_iGvtType                 = in_Original.m_iGvtType ;
         m_bConstitutionalFormDirty = true;
      }
      
      if(m_NextElection != in_Original.m_NextElection)
      {
         m_NextElection             = in_Original.m_NextElection;
         m_bConstitutionalFormDirty = true;
      }
      
      m_fPopPoliticalIdeology    = in_Original.m_fPopPoliticalIdeology ;
      m_iPopulation              = in_Original.m_iPopulation ;
      m_iPopulationPoliticalControl = in_Original.m_iPopulationPoliticalControl;
      m_fEmigrationLevel         = in_Original.m_fEmigrationLevel ;
		gassert(!_isnan(m_fEmigrationLevel) && m_fEmigrationLevel >= 0.f,"Emigration is NAN");
		m_fEmigrationLevelExpected	= in_Original.m_fEmigrationLevelExpected;
      m_fImmigrationLevel        = in_Original.m_fImmigrationLevel ;
      m_fPropagandaLevel         = in_Original.m_fPropagandaLevel;
      m_fCorruptionLevel         = in_Original.m_fCorruptionLevel ;
		m_fCorruptionLevelExpected	= in_Original.m_fCorruptionLevelExpected;
      m_fAreaTotal               = in_Original.m_fAreaTotal ;
      m_fAreaWaterTotal          = in_Original.m_fAreaWaterTotal ;
      m_fAreaLandTotal           = in_Original.m_fAreaLandTotal ;
      m_fArableLandLevel         = in_Original.m_fArableLandLevel ;
      m_fForestLandLevel         = in_Original.m_fForestLandLevel ;
      m_fParksLandLevel          = in_Original.m_fParksLandLevel ;
      m_fNotUsedLandLevel        = in_Original.m_fNotUsedLandLevel ;
      m_fEconomicHealth          = in_Original.m_fEconomicHealth ;
		m_fNuclearReady				= in_Original.m_fNuclearReady;
      m_iHighestPoint            = in_Original.m_iHighestPoint ;
      m_iLowestPoint             = in_Original.m_iLowestPoint ;
      m_iCoastlines              = in_Original.m_iCoastlines ;
      m_iStandardElevation       = in_Original.m_iStandardElevation ;
      m_iPoliticalParty          = in_Original.m_iPoliticalParty ;
      m_fCountryPoliticalIdeology= in_Original.m_fCountryPoliticalIdeology ;
      m_fAMDSLevel               = in_Original.m_fAMDSLevel ;
      m_iCountryGroup            = in_Original.m_iCountryGroup;
      m_eAdvisorEconomicStatus   = in_Original.m_eAdvisorEconomicStatus;
      m_eAdvisorMilitaryStatus   = in_Original.m_eAdvisorMilitaryStatus;
      m_eAdvisorPoliticStatus    = in_Original.m_eAdvisorPoliticStatus;
      memcpy(m_pInternalLaws,in_Original.m_pInternalLaws,SP2::EInternalLaws::ItemCount*sizeof(bool));
      m_Religions                = in_Original.m_Religions;
      m_Languages                = in_Original.m_Languages;
      m_ReligionOfficialStatus   = in_Original.m_ReligionOfficialStatus;
      m_ReligionIllegalStatus    = in_Original.m_ReligionIllegalStatus;
      m_LanguageOfficialStatus   = in_Original.m_LanguageOfficialStatus;
      m_LanguageIllegalStatus    = in_Original.m_LanguageIllegalStatus;
      m_vPoliticalParty          = in_Original.m_vPoliticalParty;
      *m_pResearchInformation    = *in_Original.m_pResearchInformation;
      m_bEmigrationClosed        = in_Original.m_bEmigrationClosed ;
      m_bImmigrationClosed       = in_Original.m_bImmigrationClosed;
      m_bMartialLaw              = in_Original.m_bMartialLaw;
      m_bActivated               = in_Original.m_bActivated;
		m_fOverallDiplomaticRelations          = in_Original.m_fOverallDiplomaticRelations;
		m_fPressure										= in_Original.m_fPressure;
		m_fNationalSecurity							= in_Original.m_fNationalSecurity;
		m_bAcceptForeignOrder		= in_Original.m_bAcceptForeignOrder;
   }


   if(in_Original.m_bResourceDataDirty)
   {
      memcpy(m_pResourceGDP,in_Original.m_pResourceGDP,sizeof(REAL32)*SP2::EResources::ItemCount);
      memcpy(m_pResourceTaxes,in_Original.m_pResourceTaxes,sizeof(REAL32)*SP2::EResources::ItemCount);
      memcpy(m_pResourceMeetDomesticConsumption,in_Original.m_pResourceMeetDomesticConsumption,sizeof(bool)*SP2::EResources::ItemCount);
      memcpy(m_pResourceGvtControl,in_Original.m_pResourceGvtControl,sizeof(bool)*SP2::EResources::ItemCount);
      memcpy(m_pResourceLegal,in_Original.m_pResourceLegal,sizeof(bool)*SP2::EResources::ItemCount);
      memcpy(m_pResourceExportDesired,in_Original.m_pResourceExportDesired,sizeof(REAL64)*SP2::EResources::ItemCount);
      memcpy(m_pResourceExport,in_Original.m_pResourceExport,sizeof(REAL64)*SP2::EResources::ItemCount);
		memcpy(m_pResourceMarketShare,in_Original.m_pResourceMarketShare,sizeof(REAL64)*SP2::EResources::ItemCount);
		memcpy(m_pResourceMarketAvailability,in_Original.m_pResourceMarketAvailability,sizeof(REAL64)*SP2::EResources::ItemCount);
      memcpy(m_pResourceImportDesired,in_Original.m_pResourceImportDesired,sizeof(REAL64)*SP2::EResources::ItemCount);
      memcpy(m_pResourceImport,in_Original.m_pResourceImport,sizeof(REAL64)*SP2::EResources::ItemCount);
      memcpy(m_pResourceDemand,in_Original.m_pResourceDemand,sizeof(REAL64)*SP2::EResources::ItemCount);
      memcpy(m_pResourceProduction,in_Original.m_pResourceProduction,sizeof(REAL64)*SP2::EResources::ItemCount);     
   }

   if(in_Original.m_bCovertActionCellsDirty)
   {
      m_vCovertActionCells = in_Original.m_vCovertActionCells;
   }
}

bool GCountryDataClient::RemoveMissileClientSide(SP2::GNuclearMissile& in_Missile)
{
   for(vector<SP2::GNuclearMissile>::iterator l_GroundBasedIt = m_vGroundBasedNuclearMissiles.begin();
       l_GroundBasedIt != m_vGroundBasedNuclearMissiles.end();
       l_GroundBasedIt++)
   {
      if(l_GroundBasedIt->m_iID == in_Missile.m_iID)
      {
         m_vGroundBasedNuclearMissiles.erase(l_GroundBasedIt);
         return true;
      }
   }
   for(vector<SP2::GNuclearMissile>::iterator l_SubBasedIt  = m_vSubmarineBasedNuclearMissiles.begin();
       l_SubBasedIt != m_vSubmarineBasedNuclearMissiles.end();
       l_SubBasedIt++)
   {
      if(l_SubBasedIt->m_iID == in_Missile.m_iID)
      {
         m_vSubmarineBasedNuclearMissiles.erase(l_SubBasedIt);
         return true;
      }
   }
   return false;
}

