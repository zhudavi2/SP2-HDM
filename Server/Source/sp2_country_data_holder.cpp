/**************************************************************
*
* sp2_country_data_holder.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

REAL64 GCountryDataItf::m_pResourceMarketAvailability[EResources::ItemCount];

//! Default constructor
GCountryData::GCountryData() : GCountryDataItf()
{
   m_fReloadNuclearSubmarinesClock  = -1.f;   
	m_pAdvisor                       = NULL;


   m_pResearchInformation = new GResearchInformation();

   m_fPopulationGrowth = 0.f;

   //Unused
   m_fGDPValueBase     = 0.f;
   m_fEconomicActivity = 0.f;
}

//! destructor
GCountryData::~GCountryData()
{
	SAFE_DELETE(m_pAdvisor);
   SAFE_DELETE(m_pResearchInformation);
}

GCountryDataItf* GCountryData::New()
{
   return new GCountryData();
}

void GCountryData::InitExportsImports()
{
	Memory::Fill<REAL64>(m_pResourceExport,0.0,EResources::ItemCount);
	Memory::Fill<REAL64>(m_pResourceImport,0.0,EResources::ItemCount);
	Memory::Fill<REAL32>(m_pResourceMarketShare,0.0,EResources::ItemCount);
	m_fBudgetExpenseImports = 0.f;
	m_fBudgetRevenueTrade = 0.f;
   m_bResourceDataDirty = true;
   m_bEconomicDataDirty = true;
}

void GCountryData::ReadjustDesiredExportsImports(REAL64 l_fChangeRatio)
{
	//Also readjust Desired Exports / Imports for gvt controlled resources
	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		if(m_pResourceGvtControl[i])
		{
         m_bResourceDataDirty = true;
			m_pResourceExportDesired[i] *= l_fChangeRatio;
			m_pResourceImportDesired[i] *= l_fChangeRatio;
		}
	}
}

void GCountryData::SynchronizeWithRegions()
{
	const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(m_iCountryID);
		
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		m_pResourceProduction[i] = 0.0f;
		m_pResourceDemand[i] = 0.0f;
	}

   m_bResourceDataDirty = true;
   m_bEconomicDataDirty = true;
   m_bOtherDataDirty    = true;
   m_bBudgetDataDirty   = true;
	
	m_fDemandTotal = 0.f;

	
	//Adjust the production modifier
	if(m_iGvtType == EGovernmentType::Anarchy)
		m_fResourceProductionModifier = m_fGvtApproval / 2.f;

	m_iHighestPoint = 0;
	m_iLowestPoint = 999999;
	m_iStandardElevation = 0;	
	m_iCoastlines = 0;
	m_fAreaTotal = 0.0f;
	m_fAreaLandTotal = 0.0f;
	m_fAreaWaterTotal = 0.0f;	
	m_fBudgetRevenueTourism = 0.f;
	m_iPop15 = 0;
	m_iPop1565 = 0;
	m_iPop65 = 0;	
	m_fGDPValue = 0.f;

	m_iPopulationPoliticalControl = 0;
	REAL32 l_fTempElevation = 0.0f;
	REAL32 l_fArableLand = 0.0f;
	REAL32 l_fParksLand = 0.0f;
	REAL32 l_fForestLand = 0.0f;
	REAL32 l_fNotUsedLand = 0.0f;
	
	m_Religions.clear();
	m_Languages.clear();

	GReligionList l_Religions;
	GLanguageList l_Languages;


	GRegion* l_pRegion = NULL;

	for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
			l_RegionItr != l_vRegionsPolitical.end();
			l_RegionItr++)
	{
		l_pRegion = g_ServerDAL.GetGRegion(*l_RegionItr);

        l_fArableLand += l_pRegion->ArableLand();
        l_fParksLand += l_pRegion->ParksLand();
        l_fForestLand += l_pRegion->ForestLand();
        l_fNotUsedLand += l_pRegion->NotUsedLand();

		m_fAreaTotal += l_pRegion->AreaTotal();
		m_fAreaLandTotal += l_pRegion->AreaLand();
		m_fAreaWaterTotal += l_pRegion->AreaWater();

		m_iPop15 += (INT64)l_pRegion->Population15();
		m_iPop1565 += (INT64)l_pRegion->Population1565();
		m_iPop65 += (INT64)l_pRegion->Population65();

        if(l_pRegion->OwnerId() == (INT64)l_pRegion->OwnerMilitaryId())
        {
            m_iPopulationPoliticalControl += (INT64)l_pRegion->Population();
            m_fBudgetRevenueTourism += l_pRegion->TourismIncome();
        }
			
        //Calculate the production total of a country
        for(UINT32 j=0; j < EResources::ItemCount; j++)
        {
            m_pResourceProduction[j]	+= l_pRegion->ResourceProduction((SP2::EResources::Enum)j);
            gassert(!_isnan(m_pResourceProduction[j]), "Production is NAN");
            //m_pResourceDemand[j]			+= l_pRegion->ResourceDemand((SP2::EResources::Enum)j);
            m_fGDPValue += l_pRegion->ResourceProduction((SP2::EResources::Enum)j) * m_fResourceProductionModifier;
        }

		m_iHighestPoint = max(m_iHighestPoint,l_pRegion->HighestPoint());
		m_iLowestPoint = min(m_iLowestPoint,l_pRegion->LowestPoint());
		m_iCoastlines += l_pRegion->Coastlines();
		l_fTempElevation += (REAL32)l_pRegion->StandardElevation() * l_pRegion->AreaTotal();		

		//cycle through this region's religions
		l_pRegion->GetReligions(l_Religions);
		l_pRegion->GetLanguages(l_Languages);
		for(GReligionList::iterator l_Itr = l_Religions.begin();
				l_Itr != l_Religions.end();
				l_Itr++)
		{
			m_Religions[l_Itr->first] += l_Itr->second;
		}
		for(GLanguageList::iterator l_Itr = l_Languages.begin();
				l_Itr != l_Languages.end();
				l_Itr++)
		{
			m_Languages[l_Itr->first] += l_Itr->second;
		}		
	}

	IterateDemand();
	
	//Cap exports based on production, and calculate the new desired import and exports
	for(UINT32 j=0; j<EResources::ItemCount; j++)
	{						
        m_pResourceExport[j] = min(m_pResourceExport[j], m_pResourceProduction[j]*m_fResourceProductionModifier);

		m_fDemandTotal += m_pResourceDemand[j] * m_fResourceDemandModifier;
		if(!m_pResourceGvtControl[j] || m_pResourceMeetDomesticConsumption[j])
		{
			if((m_pResourceProduction[j]*m_fResourceProductionModifier) > (m_pResourceDemand[j]*m_fResourceDemandModifier))
			{
				m_pResourceExportDesired[j] = (m_pResourceProduction[j]*m_fResourceProductionModifier) - (m_pResourceDemand[j]*m_fResourceDemandModifier);
				m_pResourceImportDesired[j] = 0.f;
			}
			else
			{
				m_pResourceExportDesired[j] = 0.f;
				m_pResourceImportDesired[j] = (m_pResourceDemand[j]*m_fResourceDemandModifier) - (m_pResourceProduction[j]*m_fResourceProductionModifier);
			}			
		}
        else
            m_pResourceExportDesired[j] = min(m_pResourceExportDesired[j], m_pResourceProduction[j]*m_fResourceProductionModifier);
	}

	m_iPopulation = m_iPop15 + m_iPop1565 + m_iPop65;

    if(m_fAreaLandTotal != 0.0f)
    {
        m_fArableLandLevel = l_fArableLand / m_fAreaLandTotal;
        m_fParksLandLevel = l_fParksLand / m_fAreaLandTotal;
        m_fForestLandLevel = l_fForestLand / m_fAreaLandTotal;
        m_fNotUsedLandLevel = l_fNotUsedLand / m_fAreaLandTotal;
    }
    else
    {
        m_fArableLandLevel = 0.0f;
        m_fParksLandLevel = 0.0f;
        m_fForestLandLevel = 0.0f;
        m_fNotUsedLandLevel = 0.0f;
	}

    m_iStandardElevation = (m_fAreaLandTotal != 0.0f) ? (INT32)(l_fTempElevation / m_fAreaLandTotal) : m_iStandardElevation;

	m_fTelecomLevel = 0.f;
	m_fInfrastructure = 0.f;

	//Recalculate the importance of regions 
	for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
			l_RegionItr != l_vRegionsPolitical.end();
			l_RegionItr++)
	{
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(*l_RegionItr);
		REAL32 l_fImportance = 0.f;
		if(m_iPopulation != 0)
			l_fImportance = (REAL32)l_pRegion->Population() / (REAL32)m_iPopulation;
		else
			l_fImportance = 0.f;

		l_pRegion->PercentageValue(l_fImportance);
		
		m_fTelecomLevel += l_pRegion->TelecomLevel() * l_fImportance;		
		m_fInfrastructure += l_pRegion->Infrastructure() * l_fImportance;		
	}
	
	//Calculate the change on research expenses
	UpdateResearchExpenses();

}

void GCountryData::IterateDemand()
{
    GDZLOG(EDZLogLevel::Entry, NameAndIDForLog());

	//Find new demand
    REAL64 l_fUncappedHdi = GCountryData::FindHumanDevelopment(m_fLifeExpectancy,
                                                               m_fMeanYearsSchooling,
                                                               m_fExpectedYearsSchooling,
                                                               (0 < m_iPopulation) ? GDPPerCapita() : 0,
                                                               false);

    //When this is ultimately called from FetchCountryData() at the start of the game, LE, MYS, and EYS may not be filled in.
    //This ensures that a sane value will be used for the first IterateDemand(). After the game starts, uncapped HDI would never be lower than capped HDI.
    l_fUncappedHdi = max(m_fHumanDevelopment, l_fUncappedHdi);

	REAL64 l_fTemp = m_iPopulation * (REAL64)((1.f-m_fPopInPoverty) * l_fUncappedHdi);
	m_pResourceDemand[EResources::Meat] = (l_fTemp * 498.2f) + ((REAL64)m_iPopulation * 30.f);
	m_pResourceDemand[EResources::Vegetable_Fruits] = (l_fTemp * 764.3f) + ((REAL64)m_iPopulation * 40.f);
	m_pResourceDemand[EResources::Dairy] = (l_fTemp * 830.9f) + ((REAL64)m_iPopulation * 25.f);
	m_pResourceDemand[EResources::Tobacco] = (l_fTemp * 143.49f) + ((REAL64)m_iPopulation * 4.f);
	m_pResourceDemand[EResources::Drugs] = (l_fTemp * 95.18f) + ((REAL64)m_iPopulation * 5.f);
	m_pResourceDemand[EResources::Minerals] = (l_fTemp * 567.25f) + ((REAL64)m_iPopulation * 30.f);
	m_pResourceDemand[EResources::Fossile_Fuels] = (l_fTemp * 1142.7f) + ((REAL64)m_iPopulation * 40.f);
	m_pResourceDemand[EResources::Precious_Stones] = (l_fTemp * 35.2f) + ((REAL64)m_iPopulation * 0.8f);
	m_pResourceDemand[EResources::Commodities] = (l_fTemp * 1198.f) + ((REAL64)m_iPopulation * 10.f);
	m_pResourceDemand[EResources::Appliances] = (l_fTemp * 685.05f) + ((REAL64)m_iPopulation * 7.f);
	m_pResourceDemand[EResources::Vehicules] = (l_fTemp * 826.15f) + ((REAL64)m_iPopulation * 15.f);
	m_pResourceDemand[EResources::Luxury_Commodities] = (l_fTemp * 247.4f) + ((REAL64)m_iPopulation * 0.8f);
	m_pResourceDemand[EResources::Construction] = (l_fTemp * 6473.3f) + ((REAL64)m_iPopulation * 130.f);
	m_pResourceDemand[EResources::Health_Care] = (l_fTemp * 11158.28f) + ((REAL64)m_iPopulation * 30.f);
	m_pResourceDemand[EResources::Retail] = (l_fTemp * 1456.18f) + ((REAL64)m_iPopulation * 260.f);
	m_pResourceDemand[EResources::Legal_Services] = (l_fTemp * 2739.6f) + ((REAL64)m_iPopulation * 25.f);
	m_pResourceDemand[EResources::Cereals] = (((l_fTemp * 592.46f) + ((REAL64)m_iPopulation * 75.f))*0.94f) +
		(0.01f * m_pResourceDemand[EResources::Dairy]) + (0.05f * m_pResourceDemand[EResources::Meat]);
	m_pResourceDemand[EResources::Wood_Paper] = (((l_fTemp * 183.88f) + ((REAL64)m_iPopulation * 10.f))*0.96f) +
		(0.04f * m_pResourceDemand[EResources::Construction]);
	m_pResourceDemand[EResources::Plastics] = (((l_fTemp * 615.9f) + ((REAL64)m_iPopulation * 10.f))*0.97f) +
		(0.01f * m_pResourceDemand[EResources::Luxury_Commodities]) + (0.02f * m_pResourceDemand[EResources::Commodities]);
	m_pResourceDemand[EResources::Fabrics] = (((l_fTemp * 1338.8f) + ((REAL64)m_iPopulation * 75.f))*0.94f) +
		(0.01f * m_pResourceDemand[EResources::Commodities]) + (0.05f * m_pResourceDemand[EResources::Luxury_Commodities]);
	m_pResourceDemand[EResources::Pharmaceuticals] = (((l_fTemp * 128.97f) + ((REAL64)m_iPopulation * 5.f))*0.96f) +
		(0.04f * m_pResourceDemand[EResources::Health_Care]);
	m_pResourceDemand[EResources::Chemicals] = (((l_fTemp * 1517.f) + ((REAL64)m_iPopulation * 40.f))*0.95f) +
		(0.01f * m_pResourceDemand[EResources::Luxury_Commodities]) + (0.02f * m_pResourceDemand[EResources::Commodities]) +
		(0.01f * m_pResourceDemand[EResources::Plastics]) + (0.01f * m_pResourceDemand[EResources::Pharmaceuticals]);	
	m_pResourceDemand[EResources::Machinery] = (((l_fTemp * 413.81f) + ((REAL64)m_iPopulation * 12.f))*0.84f) +
		(0.05f * m_pResourceDemand[EResources::Construction]) + (0.03f * m_pResourceDemand[EResources::Fossile_Fuels]) +
		(0.02f * m_pResourceDemand[EResources::Wood_Paper]) + (0.05f * m_pResourceDemand[EResources::Minerals]) +
		(0.01f * m_pResourceDemand[EResources::Precious_Stones]);	
	m_pResourceDemand[EResources::Electricity] = (((l_fTemp * 1401.75f) + ((REAL64)m_iPopulation * 35.f))*0.78f) +
		(0.02f * m_pResourceDemand[EResources::Wood_Paper]) + (0.04f * m_pResourceDemand[EResources::Plastics]) +
		(0.02f * m_pResourceDemand[EResources::Fabrics]) + (0.01f * m_pResourceDemand[EResources::Chemicals]) +
		(0.02f * m_pResourceDemand[EResources::Pharmaceuticals]) + 
		(0.04f * m_pResourceDemand[EResources::Appliances]) + (0.02f * m_pResourceDemand[EResources::Vehicules]) +
		(0.02f * m_pResourceDemand[EResources::Machinery]) + (0.01f * m_pResourceDemand[EResources::Commodities]) +
		(0.02f * m_pResourceDemand[EResources::Luxury_Commodities]);
	m_pResourceDemand[EResources::Iron_Steel] = (((l_fTemp * 1078.88f) + ((REAL64)m_iPopulation * 50.f))*0.82f) +
		(0.04f * m_pResourceDemand[EResources::Appliances]) + (0.04f * m_pResourceDemand[EResources::Vehicules]) +
		(0.04f * m_pResourceDemand[EResources::Machinery]) + (0.06f * m_pResourceDemand[EResources::Construction]);	
	m_pResourceDemand[EResources::Engineering] = (((l_fTemp * 2687.87f) + ((REAL64)m_iPopulation * 20.f))*0.98f) +
		(0.02f * m_pResourceDemand[EResources::Construction]);
	m_pResourceDemand[EResources::Marketing_Advertising] = (((l_fTemp * 1349.f) + ((REAL64)m_iPopulation * 8.f))*0.98f) +
		(0.02f * m_pResourceDemand[EResources::Retail]);

    GDZLOG(EDZLogLevel::Exit, L"");
}

bool GCountryData::FetchCountryData(const ENTITY_ID in_iCountryID)
{
	m_iCountryID = (INT16)in_iCountryID;	

	//The huge select statement;
	GTable l_Table;
	GSString l_sSelectQuery(
                "SELECT interest_level" \
	                  ", personal_income_tax" \
							", available_money" \
							", mili_manpower_avai" \
							", martial_law" \
							", suffrage" \
							", emigration_closed" \
							", immigration_closed" \
							", gvt_type" \
							", propaganda_level" \
							", budget_expense_security" \
							", next_election" \
							", inflation_level" \
							", inflation_level_expected" \
							", budget_revenue_tax" \
							", budget_revenue_exports" \
							", budget_revenue_imf" \
							", budget_revenue_tourism" \
							", economic_activity" \
							", budget_expense_ratio_telecom" \
							", budget_expense_ratio_imf" \
							", budget_expense_ratio_government" \
							", budget_expense_diplomacy" \
							", budget_expense_imports" \
							", budget_expense_ratio_healthcare" \
							", budget_expense_ratio_education" \
							", budget_expense_unit_upkeep" \
							", budget_expense_ratio_environ" \
							", budget_expense_ratio_tourism" \
							", budget_expense_ratio_infra" \
							", budget_expense_ratio_propaganda" \
							", pop_political_ideology" \
							", birth_rate" \
							", gvt_approval" \
							", death_rate" \
							", human_development" \
							", pop_15_65_unemployed" \
							", pop_in_poverty" \
							", gvt_stability" \
							", corruption_level" \
                     ", amds_level" \
							", pop_economic_model" \
							", political_party" \
							", last_iteration" \
							", groups" \
							", emigration_level" \
							", immigration_level" \
							", name_stid" \
							", global_tax_mod" \
							", activated" \
							", law_freedom_speech" \
							", law_freedom_demonstration" \
							", law_women_suffrage" \
							", law_nb_children" \
							", law_contraception" \
							", law_abortion" \
							", law_child_labor" \
							", law_polygamy" \
							", law_same_sex_marriage" \
							", budget_expense_ratio_research" \
							", indiv_collec" \
							", hier_distance" \
							", male_female" \
							", gvt_stability_ex" \
							", gvt_approval_ex" \
							", budget_expense_corruption" \
							", climate" \
							", natural_hazards" \
							", corruption_level_ex" \
							", telecom_level_ex" \
							", infrastructure_level_ex" \
							", birth_rate_ex" \
							", death_rate_ex" \
							", emigration_level_ex" \
							", accept_foreign_order" \
							", economic_health" \
							", nuclear_ready" \
							" FROM country WHERE Id = " + GSString(in_iCountryID));	
   
   DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_Table);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the country table. SP2::country_data_holder",MSGTYPE_ERROR);
		return false;
	}      		

	//Fill up the member variables	
	//(the number is their respective data number. See AI Data relations document)
	m_fInterestLevel = *((REAL32*)l_Table.Row(0)->Cell(0)->Data());	//113
	m_fPersonalIncomeTax = *((REAL64*)l_Table.Row(0)->Cell(1)->Data());	//114
	m_fMonetarySupplyAmount = *((REAL64*)l_Table.Row(0)->Cell(2)->Data());	//126
	m_iMiliManpowerAvailable = (INT64)(*((INT32*)l_Table.Row(0)->Cell(3)->Data()));	//205
	if (*((char*)l_Table.Row(0)->Cell(4)->Data()) == 'T')	//107
		m_bMartialLaw = true;
	else
		m_bMartialLaw = false;
	m_iSuffrage = *((INT32*)l_Table.Row(0)->Cell(5)->Data());	//88-89
	if (*((char*)l_Table.Row(0)->Cell(6)->Data()) == 'T')	//94
		m_bEmigrationClosed = true;
	else
		m_bEmigrationClosed = false;
	if (*((char*)l_Table.Row(0)->Cell(7)->Data()) == 'T')	//95
		m_bImmigrationClosed = true;
	else
		m_bImmigrationClosed = false;

    //Limit income tax according to gov't type
	m_iGvtType = *((INT32*)l_Table.Row(0)->Cell(8)->Data());	//81
    m_fPersonalIncomeTax = min(m_fPersonalIncomeTax, g_SP2Server->IncomeTaxLimit(static_cast<EGovernmentType::Enum>(m_iGvtType)));

	m_fPropagandaLevel = *((REAL64*)l_Table.Row(0)->Cell(9)->Data());	//200
	m_fBudgetExpenseSecurity = *((REAL64*)l_Table.Row(0)->Cell(10)->Data());	//167
	if((INT64*)l_Table.Row(0)->Cell(11)->Data() != NULL) 
	{	
		GDateTime l_NextElection(*(INT64*)l_Table.Row(0)->Cell(11)->Data());	//97
		m_NextElection = l_NextElection;	
	}
	else 
		m_NextElection = NULL;	
	m_fInflationLevel = *((REAL32*)l_Table.Row(0)->Cell(12)->Data());	//111
	m_fInflationLevelExpected = *((REAL32*)l_Table.Row(0)->Cell(13)->Data());	//196
	m_fBudgetRevenueTax = *((REAL64*)l_Table.Row(0)->Cell(14)->Data());	//191
	m_fBudgetRevenueTrade = *((REAL64*)l_Table.Row(0)->Cell(15)->Data());	//192
	m_fBudgetRevenueIMF = *((REAL64*)l_Table.Row(0)->Cell(16)->Data());	//193
	m_fBudgetRevenueTourism = *((REAL64*)l_Table.Row(0)->Cell(17)->Data());	//194
	m_fBudgetExpenseRatioTelecom = *((REAL64*)l_Table.Row(0)->Cell(19)->Data());	
	m_fBudgetExpenseRatioIMF = *((REAL64*)l_Table.Row(0)->Cell(20)->Data());	//179
	m_fBudgetExpenseRatioGovernment = *((REAL64*)l_Table.Row(0)->Cell(21)->Data());	//166
	m_fBudgetExpenseDiplomacy = *((REAL64*)l_Table.Row(0)->Cell(22)->Data());	//168
	m_fBudgetExpenseImports = *((REAL64*)l_Table.Row(0)->Cell(23)->Data());	//170
	m_fBudgetExpenseRatioHealthcare = *((REAL64*)l_Table.Row(0)->Cell(24)->Data());	//160
	m_fBudgetExpenseRatioEducation = *((REAL64*)l_Table.Row(0)->Cell(25)->Data());	//161
	m_fBudgetExpenseUnitUpkeep = *((REAL64*)l_Table.Row(0)->Cell(26)->Data());	//186
	m_fBudgetExpenseRatioEnvironment = *((REAL64*)l_Table.Row(0)->Cell(27)->Data());
	m_fBudgetExpenseRatioTourism = *((REAL64*)l_Table.Row(0)->Cell(28)->Data());
	m_fBudgetExpenseRatioInfrastructure = *((REAL64*)l_Table.Row(0)->Cell(29)->Data());
	m_fBudgetExpenseRatioPropaganda = *((REAL64*)l_Table.Row(0)->Cell(30)->Data());
	m_fPopPoliticalIdeology = *((REAL32*)l_Table.Row(0)->Cell(31)->Data());		
	m_fBirthRate = *((REAL32*)l_Table.Row(0)->Cell(32)->Data());	//38	
	m_fGvtApproval = *((REAL32*)l_Table.Row(0)->Cell(33)->Data());	//86
	m_fDeathRate = *((REAL32*)l_Table.Row(0)->Cell(34)->Data());	//37
	m_fHumanDevelopment = *((REAL32*)l_Table.Row(0)->Cell(35)->Data());
	m_fPop1565Unemployed = *((REAL32*)l_Table.Row(0)->Cell(36)->Data());	//33
	m_fPopInPoverty = *((REAL32*)l_Table.Row(0)->Cell(37)->Data());	//109
	m_fGvtStability = *((REAL32*)l_Table.Row(0)->Cell(38)->Data());	//84
	m_fCorruptionLevel = *((REAL32*)l_Table.Row(0)->Cell(39)->Data());
	l_Table.Row(0)->Cell(40)->GetData(m_fAMDSLevel);
	m_fPopEconomicModel = *((REAL32*)l_Table.Row(0)->Cell(41)->Data());
	m_iPoliticalParty = *((INT32*)l_Table.Row(0)->Cell(42)->Data());
	m_fLastIteration = *((REAL64*)l_Table.Row(0)->Cell(43)->Data());
	m_iCountryGroup = *((INT32*)l_Table.Row(0)->Cell(44)->Data());
	m_fEmigrationLevel = *((REAL32*)l_Table.Row(0)->Cell(45)->Data());
	gassert(!_isnan(m_fEmigrationLevel) && m_fEmigrationLevel >= 0.f,"Emigration isnan");
	m_fImmigrationLevel = *((REAL32*)l_Table.Row(0)->Cell(46)->Data());
	m_iNameID = *((INT32*)l_Table.Row(0)->Cell(47)->Data());
    m_fGlobalTaxMod = min( *((REAL32*)l_Table.Row(0)->Cell(48)->Data()), g_SP2Server->GlobalTaxLimit());

    //Always activate if country not marked inactive in database
    //Also activate if countries can survive without regions, and server is set to activate every country in database
    //\todo #144 Resolving crash might allow activateAllDatabaseCountries to be removed.
    m_bActivated = !(*((char*)l_Table.Row(0)->Cell(49)->Data()) == 'F') || (!g_SP2Server->CountryNeedsRegions() && g_SP2Server->ActivateAllDatabaseCountries());

	if(*((char*)l_Table.Row(0)->Cell(50)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::FreedomOfSpeech] = false;
	else
		m_pInternalLaws[EInternalLaws::FreedomOfSpeech] = true;
	if(*((char*)l_Table.Row(0)->Cell(51)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::FreedomOfDemonstration] = false;
	else
		m_pInternalLaws[EInternalLaws::FreedomOfDemonstration] = true;
	if(*((char*)l_Table.Row(0)->Cell(52)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::WomenSuffrage] = false;
	else
		m_pInternalLaws[EInternalLaws::WomenSuffrage] = true;
	if(*((char*)l_Table.Row(0)->Cell(53)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::NumberOfChildren] = false;
	else
		m_pInternalLaws[EInternalLaws::NumberOfChildren] = true;
	if(*((char*)l_Table.Row(0)->Cell(54)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::Contraception] = false;
	else
		m_pInternalLaws[EInternalLaws::Contraception] = true;
	if(*((char*)l_Table.Row(0)->Cell(55)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::Abortion] = false;
	else
		m_pInternalLaws[EInternalLaws::Abortion] = true;
	if(*((char*)l_Table.Row(0)->Cell(56)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::ChildLabor] = false;
	else
		m_pInternalLaws[EInternalLaws::ChildLabor] = true;
	if(*((char*)l_Table.Row(0)->Cell(57)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::Polygamy] = false;
	else
		m_pInternalLaws[EInternalLaws::Polygamy] = true;
	if(*((char*)l_Table.Row(0)->Cell(58)->Data()) == 'F' )
		m_pInternalLaws[EInternalLaws::SameSexMarriage] = false;
	else
		m_pInternalLaws[EInternalLaws::SameSexMarriage] = true;
	m_fBudgetExpenseRatioResearch = *((REAL64*)l_Table.Row(0)->Cell(59)->Data());	
	m_fAIIndividualism = *((REAL32*)l_Table.Row(0)->Cell(60)->Data());	
	m_fAIHierarchicalDistance = *((REAL32*)l_Table.Row(0)->Cell(61)->Data());	
	m_fAIMaleFemale = *((REAL32*)l_Table.Row(0)->Cell(62)->Data());	
	m_fGvtStabilityExpected = *((REAL32*)l_Table.Row(0)->Cell(63)->Data());	//84
	m_fGvtApprovalExpected = *((REAL32*)l_Table.Row(0)->Cell(64)->Data());	//86
	m_fBudgetExpenseCorruption = *((REAL64*)l_Table.Row(0)->Cell(65)->Data());	
	m_iClimateNameID = *((INT32*)l_Table.Row(0)->Cell(66)->Data());
	m_fCorruptionLevelExpected = *((REAL32*)l_Table.Row(0)->Cell(68)->Data());
	m_fTelecomLevelExpected = *((REAL32*)l_Table.Row(0)->Cell(69)->Data());
	m_fInfrastructureExpected = *((REAL32*)l_Table.Row(0)->Cell(70)->Data());
	m_fBirthRateExpected = *((REAL32*)l_Table.Row(0)->Cell(71)->Data());
	m_fDeathRateExpected = *((REAL32*)l_Table.Row(0)->Cell(72)->Data());
	m_fEmigrationLevelExpected = *((REAL32*)l_Table.Row(0)->Cell(73)->Data());
	if (*((char*)l_Table.Row(0)->Cell(74)->Data()) == 'T')
		m_bAcceptForeignOrder = true;
	else
		m_bAcceptForeignOrder = false;
	m_fEconomicHealth = *((REAL32*)l_Table.Row(0)->Cell(75)->Data()); 
	m_fNuclearReady = *((REAL32*)l_Table.Row(0)->Cell(76)->Data()); 
		
	m_fResourceProductionModifier = g_ServerDAL.ResourceProductionModifier(in_iCountryID,(EGovernmentType::Enum)m_iGvtType);
	m_fResourceDemandModifier = SP2::c_pGvtTypeDemandModifier[m_iGvtType];	

    m_sName = g_ServerDAL.GetString(m_iNameID);

    m_Master = pair<ENTITY_ID, UINT32>(0, 0);

    m_iPopulation = 0;

	//Fill information that comes from the regions
	SynchronizeWithRegions();

    {
        //Determine life expectancy, mean years schooling, and expected years schooling, given just HDI and GDP per capita.
        const REAL32 l_fIndexProduct = powf(m_fHumanDevelopment, 3.f);

        const REAL32 l_fIncomeIndex = FindIncomeIndex(m_fGDPValue / m_iPopulation, true);
        REAL32 l_fHIEIProduct = l_fIndexProduct / ((l_fIncomeIndex > 0) ? l_fIncomeIndex : 1.f);

        REAL32 l_fHI = 1.f;
        REAL32 l_fEI = 1.f;

        if(l_fHIEIProduct <= 1.f)
        {
            //Estimate health and education indices using quadratic
            static const REAL32 l_fA = 0.996339245527081f;
            static const REAL32 l_fB = -0.166249106711782f;
            const REAL32 l_fC = -l_fHIEIProduct;

            l_fHI = (-l_fB + sqrtf(powf(l_fB, 2.f) - (4 * l_fA * l_fC))) / (2 * l_fA);
            gassert(l_fHI >= 0.f, L"HI (" + GString(l_fHI) + L") less than 0.000");

            l_fEI = l_fA * l_fHI + l_fB;
            gassert(l_fHI >= l_fEI, L"HI (" + GString(l_fHI) + L") less than EI (" + GString(l_fEI) +L")");

            if(1.f < l_fHI || l_fEI < 0.f || 1.f < l_fEI)
            {
                l_fHI = sqrtf(l_fHIEIProduct);
                l_fEI = sqrtf(l_fHIEIProduct);
            }
        }
        else
        {
            GDZLOG(EDZLogLevel::Warning, NameAndIDForLog() + L" has nonincome HDI > 1.000 (" + GString(l_fHIEIProduct) + L"), with HDI " + GString::FormatNumber(m_fHumanDevelopment) + L" and II " + GString(l_fIncomeIndex) + L"; capping HI and EI at 1.000 and recalculating HDI");
            m_fHumanDevelopment = powf(l_fIncomeIndex, 1.f / 3.f);
        }

        GDZLOG(EDZLogLevel::Info1, L"HI: " + GString(l_fHI) + L", EI: " + GString(l_fEI));

        m_fLifeExpectancy = (l_fHI * 65.f) + 20.f;

        {
            //Estimate MYS and EYS using linear
            static const REAL32 l_fM = 1.49263332994651f;
            m_fMeanYearsSchooling = (180.f * l_fEI) / ((5.f * l_fM) + 6.f);
            m_fExpectedYearsSchooling = l_fM * m_fMeanYearsSchooling;

            if(c_fMaxMeanYearsSchooling < m_fMeanYearsSchooling || c_fMaxExpectedYearsSchooling < m_fExpectedYearsSchooling)
            {
                m_fMeanYearsSchooling = l_fEI * c_fMaxMeanYearsSchooling;
                m_fExpectedYearsSchooling = l_fEI * c_fMaxExpectedYearsSchooling;
            }
        }

        //Log out some country data right as the game starts.
        REAL64 l_fGDPPerCapita = (m_iPopulation > 0) ? (m_fGDPValue / m_iPopulation) : 0;
        g_Joshua.Log(
            NameAndIDForLog() + L": " +
            L"Population " + GString::FormatNumber(static_cast<REAL64>(m_iPopulation), L",", L".", L"", L"", 3, 0) + L"; " +
            L"GDP " + GString::FormatNumber(m_fGDPValue, L",", L".", L"$", L"", 3) + L"; " +
            L"LE " + GString::FormatNumber(m_fLifeExpectancy, 1) + L"; " +
            L"MYS " + GString::FormatNumber(m_fMeanYearsSchooling, 1) + L"; " +
            L"EYS " + GString::FormatNumber(m_fExpectedYearsSchooling, 1) + L"; " +
            L"GDP per capita " + GString::FormatNumber(l_fGDPPerCapita, L",", L".", L"$", L"", 3) + L"; " +
            L"HDI " + GString::FormatNumber(m_fHumanDevelopment, 3));
    }

	m_fBudgetRevenues = m_fBudgetRevenueTax + m_fBudgetRevenueTrade + m_fBudgetRevenueIMF +
								m_fBudgetRevenueTourism;

	m_fBudgetExpenses = BudgetExpenseTelecom() + BudgetExpensePropaganda() +
								BudgetExpenseResearch() + BudgetExpenseTourism() + 
								BudgetExpenseIMF() + m_fBudgetExpenseUnitUpkeep + 
								BudgetExpenseInfrastructure() + BudgetExpenseEnvironment() + 
								BudgetExpenseHealthcare() + BudgetExpenseEducation() + 
								BudgetExpenseGovernment() + m_fBudgetExpenseSecurity + 
								m_fBudgetExpenseDiplomacy + m_fBudgetExpenseImports + 
								m_fBudgetExpenseCorruption;

	if(m_fMonetarySupplyAmount < 0)
		m_fBudgetExpenses += -m_fMonetarySupplyAmount * DEBT_INTEREST_RATE;




	//Select statement on the party table;	
   GTable l_TableParty;
	GSString l_sSelectQueryParty("SELECT id, status, perc, ideology, name_stid, gvt_type, in_power FROM parties WHERE COUNTRY_ID = " + GSString(in_iCountryID));

	e_Error = g_Joshua.QueryDB(l_sSelectQueryParty, l_TableParty);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the parties table. SP2::country_data_holder",MSGTYPE_ERROR);
      return false;		
	}
	
	//fill up the party vector
	for(UINT32 i = 0; i< l_TableParty.RowCount(); i++) 
	{ 
		GPoliticalParty l_newParty;
		l_newParty.Id(*(INT32*)l_TableParty.Row(i)->Cell(0)->Data());
		l_newParty.Status(*(INT32*)l_TableParty.Row(i)->Cell(1)->Data());
		l_newParty.PercentageValue(*(REAL32*)l_TableParty.Row(i)->Cell(2)->Data());
		l_newParty.PoliticalIdeology(*(REAL32*)l_TableParty.Row(i)->Cell(3)->Data());
		l_newParty.NameId(*(INT32*)l_TableParty.Row(i)->Cell(4)->Data());
		l_newParty.CountryId(in_iCountryID);
		l_newParty.GvtType((EGovernmentType::Enum)(*(INT32*)l_TableParty.Row(i)->Cell(5)->Data()));
		if (*((char*)l_TableParty.Row(i)->Cell(6)->Data()) == 'T')
			l_newParty.InPower(true);
		else
			l_newParty.InPower(false);

		m_vPoliticalParty.push_back(l_newParty);
	}


	//Official/Illegal Religions and Languages
   GTable l_TableReligionStatus;
	GSString l_sSelectQueryReligionsStatus("SELECT religion_id, status FROM religions_status WHERE country_id = " + GSString(in_iCountryID));

	e_Error = g_Joshua.QueryDB(l_sSelectQueryReligionsStatus, l_TableReligionStatus);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the religions_status. SP2::country_data_holder",MSGTYPE_ERROR);
      return false;		
	}
	
	//fill up the religion status set
	for(UINT32 i = 0; i< l_TableReligionStatus.RowCount(); i++) 
	{ 
		INT32 l_iStatus = *(INT32*)l_TableReligionStatus.Row(i)->Cell(1)->Data();
		if(l_iStatus == 2)
			m_ReligionIllegalStatus.insert(*(INT32*)l_TableReligionStatus.Row(i)->Cell(0)->Data());
		else if (l_iStatus == 3)
			m_ReligionOfficialStatus.insert(*(INT32*)l_TableReligionStatus.Row(i)->Cell(0)->Data());
	}	

	GTable l_TableLanguageStatus;
	GSString l_sSelectQueryLanguagesStatus("SELECT language_id, status FROM languages_status WHERE country_id = " + GSString(in_iCountryID));

	e_Error = g_Joshua.QueryDB(l_sSelectQueryLanguagesStatus, l_TableLanguageStatus);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the languages_status. SP2::country_data_holder",MSGTYPE_ERROR);
      return false;		
	}
	
	//fill up the Language status set
	for(UINT32 i = 0; i< l_TableLanguageStatus.RowCount(); i++) 
	{ 
		INT32 l_iStatus = *(INT32*)l_TableLanguageStatus.Row(i)->Cell(1)->Data();
		if(l_iStatus == 2)
			m_LanguageIllegalStatus.insert(*(INT32*)l_TableLanguageStatus.Row(i)->Cell(0)->Data());
		else if (l_iStatus == 3)
			m_LanguageOfficialStatus.insert(*(INT32*)l_TableLanguageStatus.Row(i)->Cell(0)->Data());
	}

	//Resources statement
	GTable l_TableResources;
	GSString l_sSelectQueryResources("SELECT * FROM resources WHERE Id = " + GSString(in_iCountryID));   

	e_Error = g_Joshua.QueryDB(l_sSelectQueryResources, l_TableResources);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the resources table. SP2::country_data_holder",MSGTYPE_ERROR);
      return false;	
	}

	for(UINT32 i = 0; i < EResources::ItemCount; i++) 
	{
			m_pResourceMarketShare[i]				= 0.f;
			m_pResourceMarketAvailability[i]		= 0.f;
			m_pResourceImport[i]				= *((REAL64*)l_TableResources.Row(0)->Cell((i*9)+1)->Data());
			m_pResourceImportDesired[i]	= *((REAL64*)l_TableResources.Row(0)->Cell((i*9)+2)->Data());
			m_pResourceExport[i]				= *((REAL64*)l_TableResources.Row(0)->Cell((i*9)+3)->Data());
			m_pResourceExportDesired[i]	= *((REAL64*)l_TableResources.Row(0)->Cell((i*9)+4)->Data());
			if (*((char*)l_TableResources.Row(0)->Cell((i*9)+5)->Data()) == 'T')
				m_pResourceGvtControl[i] = true;
			else
				m_pResourceGvtControl[i] = false;
			if (*((char*)l_TableResources.Row(0)->Cell((i*9)+6)->Data()) == 'T')
				m_pResourceLegal[i] = true;
			else
				m_pResourceLegal[i] = false;
			if (*((char*)l_TableResources.Row(0)->Cell((i*9)+7)->Data()) == 'T')
				m_pResourceMeetDomesticConsumption[i] = true;
			else
				m_pResourceMeetDomesticConsumption[i] = false;
			m_pResourceTaxes[i]			= min(*((REAL32*)l_TableResources.Row(0)->Cell((i*9)+8)->Data()) / 100.f, g_SP2Server->ResourceTaxLimit());
			m_pResourceGDP[i]	= *((REAL32*)l_TableResources.Row(0)->Cell((i*9)+9)->Data()) / 100.f;			
	}
	
   //Fetch the list of nuclear missiles
   {
      GString l_sQuery = L"select * from missile where country_id = " + GString(m_iCountryID);
      DB::GTable l_Table;
      DB::EError l_Error = g_Joshua.QueryDB(l_sQuery,l_Table);
      IF_RETURN_LOG(l_Error != DB::DB_NO_ERROR,false,L"Couldnt load the nuclear missiles for the country");
      GNuclearMissile l_Missile;
      REAL64 l_fLongitude;
      REAL64 l_fLatitude;


      for(UINT32 i = 0 ; i < l_Table.RowCount() ; i++)
      {         
         l_Table.Row(i)->Cell(1)->GetData(l_Missile.m_iID);
         l_Table.Row(i)->Cell(2)->GetData(l_Missile.m_iOwnerID);
         l_Table.Row(i)->Cell(3)->GetData(l_Missile.m_iDesignID);
         l_Table.Row(i)->Cell(4)->GetData(l_Missile.m_iQty);
         l_Table.Row(i)->Cell(5)->GetData(l_fLatitude);
         l_Table.Row(i)->Cell(6)->GetData(l_fLongitude);
         UINT32 l_iFromSubmarine;
         l_Table.Row(i)->Cell(7)->GetData(l_iFromSubmarine);
         l_Missile.FromSubmarine(l_iFromSubmarine != 0);
         UINT32 l_iAttachedSubmarineUnit;
         l_Table.Row(i)->Cell(8)->GetData(l_iAttachedSubmarineUnit);         
         l_Missile.AttachedSubmarine(l_iAttachedSubmarineUnit);
         l_Missile.Position(GVector2D<REAL32>((REAL32)l_fLongitude,(REAL32)l_fLatitude));
         if(l_Missile.FromSubmarine())
         {
            m_vSubmarineBasedNuclearMissiles.push_back(l_Missile);
         }
         else
         {
            m_vGroundBasedNuclearMissiles.push_back(l_Missile);
         }
      }
   }

   //Fetch Its research informatoin
   ((SP2::GResearchInformation*)m_pResearchInformation)->Load(in_iCountryID);
	
	//Fetch the economic health data
	m_fEconomicHealth = GWorldBehavior::FindEconomicHealth(in_iCountryID);


   //Fetch the Covert Action Cells information
   {

      GString l_sQuery = L"select * from cov_ops_cell where owner_id = " + GString(m_iCountryID);
      DB::GTable l_Table;
      DB::EError l_Error = g_Joshua.QueryDB(l_sQuery,l_Table);
      IF_RETURN_LOG(l_Error != DB::DB_NO_ERROR,false,L"Couldnt load the covert action cells for the country");

      for(UINT32 i = 0 ; i < l_Table.RowCount() ; i++)
      {
         GCovertActionCell                         l_Cell;
         UINT32                                    l_iID;
			GString												l_sName;
			INT32													l_iNameID;
         UINT32                                    l_iOwnerID;
         SP2::EUnitCategory::Enum                  l_eUnitCategory;
         SP2::ECovertActionsTargetSector::Enum     l_eTargetSector;
         REAL32                                    l_fTraining;
         UINT32                                    l_iCountryToFrame;
         UINT32                                    l_iAssignedCountry;
         ECovertActionsMissionType::Enum           l_eMissionType;
         ECovertActionsMissionComplexity::Enum     l_eMissionComplexity;
         INT32                                     l_iActualState;


         l_Table.Row(i)->Cell(1)->GetData(l_iID);
			l_Table.Row(i)->Cell(2)->GetData(l_iNameID);
         l_Table.Row(i)->Cell(3)->GetData(l_iOwnerID);
         l_Table.Row(i)->Cell(4)->GetData(l_eUnitCategory);
         l_Table.Row(i)->Cell(5)->GetData(l_eTargetSector);
         l_Table.Row(i)->Cell(6)->GetData(l_fTraining);
         l_Table.Row(i)->Cell(7)->GetData(l_iCountryToFrame);
         l_Table.Row(i)->Cell(8)->GetData(l_iAssignedCountry);
         l_Table.Row(i)->Cell(9)->GetData(l_eMissionType);
         l_Table.Row(i)->Cell(10)->GetData(l_eMissionComplexity);
         l_Table.Row(i)->Cell(11)->GetData(l_iActualState);

			l_sName = g_ServerDAL.GetString(l_iNameID);

         l_Cell.ID(l_iID);		
			l_Cell.Name(l_sName);
			l_Cell.NameID(l_iNameID);
         l_Cell.OwnerID(l_iOwnerID);
         l_Cell.UnitCategory(l_eUnitCategory);
         l_Cell.TargetSector(l_eTargetSector);
         l_Cell.ExperienceLevel(l_fTraining);
         l_Cell.CountryToFrame(l_iCountryToFrame);
         l_Cell.AssignedCountry(l_iAssignedCountry);
         l_Cell.MissionType(l_eMissionType);
         l_Cell.MissionComplexity(l_eMissionComplexity);

         AddCovertActionCell(l_Cell, true);
         m_vCovertActionCells[m_vCovertActionCells.size()-1].Initialize((ECovertActionsCellState::Enum)l_iActualState);
         m_bCovertActionCellsDirty = true;
      }
   }//end load covert action cells



   //Load the name of designs
   {
      GString l_sQuery = L"select unit_type, next_id, name from design_format where country_id = " + GString(m_iCountryID);
      DB::GTable l_Table;      
		DB::EError l_Error = g_Joshua.QueryDB(l_sQuery,l_Table);
      IF_RETURN_LOG(l_Error != DB::DB_NO_ERROR,false,L"Couldnt load the design formats for the country");

      m_vDesignFormat.resize(EUnitType::TotalCount);

      for(UINT32 i = 0 ; i < l_Table.RowCount() ; i++)
      {
         GDesignFormat                             l_Format;
         UINT32                                    l_iType;
         UINT32                                    l_iNextID;
			GString												l_sName;
			INT32													l_iNameID;						

         l_Table.Row(i)->Cell(0)->GetData(l_iType);
			l_Table.Row(i)->Cell(1)->GetData(l_iNextID);
         l_Table.Row(i)->Cell(2)->GetData(l_iNameID);
         l_sName = g_ServerDAL.GetString(l_iNameID);
         l_Format.m_eType = (EUnitType::Enum)l_iType;
         l_Format.m_sFormat = l_sName;
         l_Format.m_iNextID = l_iNextID;
         m_vDesignFormat[l_iType].push_back(l_Format);         
      }
      
      

   }//end fetching design type names


   m_bOtherDataDirty           = true;
   m_bNuclearMissilesDataDirty = true;
   m_bEconomicDataDirty        = true;
   m_bBudgetDataDirty          = true;
   m_bResourceDataDirty        = true;
	return true;

}

/*!
 * Commits the to the database 
 * @return  true if success
 **/
bool GCountryData::CommitCountryData(IndexedData::GWriter& in_StringTable, vector<GSString>& in_vQueries, UINT32& in_iLanguageStatusIterator, UINT32& in_iReligionStatusIterator, UINT32& in_iCovertOpsStatusIterator)
{		
					
	//UPDATE country TABLE
	//! TODO SAVE CAPITAL CORRECTLY
	GSString l_sUpdateQuery("UPDATE country SET interest_level = '" + GSString(m_fInterestLevel) + "'");
	
	l_sUpdateQuery += ", personal_income_tax = '" + GSString(m_fPersonalIncomeTax) + "'" \
				", available_money = '" + GSString(m_fMonetarySupplyAmount) + "'" \
				", mili_manpower_avai = '" + GSString(m_iMiliManpowerAvailable) + "'" ;
	if (m_bAcceptForeignOrder == true)
		l_sUpdateQuery += ", accept_foreign_order = 'T'";
	else
		l_sUpdateQuery += ", accept_foreign_order = 'F'";
	if (m_bMartialLaw == true)
		l_sUpdateQuery += ", martial_law = 'T'";
	else
		l_sUpdateQuery += ", martial_law = 'F'";
	l_sUpdateQuery += ", suffrage = '" + GSString(m_iSuffrage) + "'" ;
	if (m_bEmigrationClosed == true)
		l_sUpdateQuery += ", emigration_closed = 'T'";
	else
		l_sUpdateQuery += ", emigration_closed = 'F'";
	if (m_bImmigrationClosed == true)
		l_sUpdateQuery += ", immigration_closed = 'T'";
	else
		l_sUpdateQuery += ", immigration_closed = 'F'";
	if (m_bActivated == true)
		l_sUpdateQuery += ", activated = 'T'";
	else
		l_sUpdateQuery += ", activated = 'F'";
	if (m_pInternalLaws[EInternalLaws::FreedomOfSpeech] == true)
		l_sUpdateQuery += ", law_freedom_speech = 'T'";
	else
		l_sUpdateQuery += ", law_freedom_speech = 'F'";
	if (m_pInternalLaws[EInternalLaws::FreedomOfDemonstration] == true)
		l_sUpdateQuery += ", law_freedom_demonstration = 'T'";
	else
		l_sUpdateQuery += ", law_freedom_demonstration = 'F'";
	if (m_pInternalLaws[EInternalLaws::NumberOfChildren] == true)
		l_sUpdateQuery += ", law_nb_children = 'T'";
	else
		l_sUpdateQuery += ", law_nb_children = 'F'";
	if (m_pInternalLaws[EInternalLaws::Polygamy] == true)
		l_sUpdateQuery += ", law_polygamy = 'T'";
	else
		l_sUpdateQuery += ", law_polygamy = 'F'";
	if (m_pInternalLaws[EInternalLaws::SameSexMarriage] == true)
		l_sUpdateQuery += ", law_same_sex_marriage = 'T'";
	else
		l_sUpdateQuery += ", law_same_sex_marriage = 'F'";
	if (m_pInternalLaws[EInternalLaws::WomenSuffrage] == true)
		l_sUpdateQuery += ", law_women_suffrage = 'T'";
	else
		l_sUpdateQuery += ", law_women_suffrage = 'F'";
	if (m_pInternalLaws[EInternalLaws::Abortion] == true)
		l_sUpdateQuery += ", law_abortion = 'T'";
	else
		l_sUpdateQuery += ", law_abortion = 'F'";
	if (m_pInternalLaws[EInternalLaws::ChildLabor] == true)
		l_sUpdateQuery += ", law_child_labor = 'T'";
	else
		l_sUpdateQuery += ", law_child_labor = 'F'";
	if (m_pInternalLaws[EInternalLaws::Contraception] == true)
		l_sUpdateQuery += ", law_contraception = 'T'";
	else
		l_sUpdateQuery += ", law_contraception = 'F'";
			
	l_sUpdateQuery += ", gvt_type = '" + GSString(m_iGvtType) + "'" \
				", inflation_level = '" + GSString(m_fInflationLevel) + "'" \
				", inflation_level_expected = '" + GSString(m_fInflationLevelExpected) + "'" \
				", budget_revenue_tax = '" + GSString(m_fBudgetRevenueTax) + "'" \
				", budget_revenue_exports = '" + GSString(m_fBudgetRevenueTrade) + "'" \
				", budget_revenue_imf = '" + GSString(m_fBudgetRevenueIMF) + "'" \
				", budget_revenue_tourism = '" + GSString(m_fBudgetRevenueTourism) + "'" \
				", budget_expense_security = '" + GSString(m_fBudgetExpenseSecurity) + "'" \
				", budget_expense_ratio_research = '" + GSString(m_fBudgetExpenseRatioResearch) + "'" \
				", budget_expense_ratio_tourism = '" + GSString(m_fBudgetExpenseRatioTourism) + "'" \
				", budget_expense_ratio_infra = '" + GSString(m_fBudgetExpenseRatioInfrastructure) + "'" \
				", budget_expense_ratio_propaganda = '" + GSString(m_fBudgetExpenseRatioPropaganda) + "'" \
				", budget_expense_ratio_imf = '" + GSString(m_fBudgetExpenseRatioIMF) + "'" \
				", budget_expense_ratio_government = '" + GSString(m_fBudgetExpenseRatioGovernment) + "'" \
				", budget_expense_diplomacy = '" + GSString(m_fBudgetExpenseDiplomacy) + "'" \
				", budget_expense_imports = '" + GSString(m_fBudgetExpenseImports) + "'" \
				", budget_expense_ratio_healthcare = '" + GSString(m_fBudgetExpenseRatioHealthcare) + "'" \
				", budget_expense_ratio_education = '" + GSString(m_fBudgetExpenseRatioEducation) + "'" \
				", budget_expense_unit_upkeep = '" + GSString(m_fBudgetExpenseUnitUpkeep) + "'" \
				", budget_expense_corruption = '" + GSString(m_fBudgetExpenseCorruption) + "'" \
				", budget_expense_ratio_environ = '" + GSString(m_fBudgetExpenseRatioEnvironment) + "'" \
				", budget_expense_ratio_telecom = '" + GSString(m_fBudgetExpenseRatioTelecom) + "'" \
				", pop_political_ideology = '" + GSString(m_fPopPoliticalIdeology) + "'" \
				", telecom_level_ex = '" + GSString(m_fTelecomLevelExpected) + "'" \
				", gvt_approval = '" + GSString(m_fGvtApproval) + "'" \
				", infrastructure_level_ex = '" + GSString(m_fInfrastructureExpected) + "'" \
				", birth_rate = '" + GSString(m_fBirthRate) + "'" \
				", death_rate = '" + GSString(m_fDeathRate) + "'" \
				", pop_15_65_unemployed = '" + GSString(m_fPop1565Unemployed) + "'" \
				", pop_in_poverty = '" + GSString(m_fPopInPoverty) + "'" \
				", gvt_stability = '" + GSString(m_fGvtStability) + "'" \
				", gvt_stability_ex = '" + GSString(m_fGvtStabilityExpected) + "'" \
				", human_development = '" + GSString(m_fHumanDevelopment) + "'" \
				", corruption_level = '" + GSString(m_fCorruptionLevel) + "'" \
				", corruption_level_ex = '" + GSString(m_fCorruptionLevelExpected) + "'" \
				", gvt_type = '" + GSString(m_iGvtType) + "'" \
            ", amds_level = '" + GSString(m_fAMDSLevel) + "'" \
				", economic_activity = '" + GSString(m_fEconomicActivity) + "'" \
				", pop_economic_model = '" + GSString(m_fPopEconomicModel) + "'" \
				", political_party = '" + GSString(m_iPoliticalParty) + "'" \
				", last_iteration = '" + GSString(m_fLastIteration) + "'" \
				", emigration_level = '" + GSString(m_fEmigrationLevel) + "'" \
				", emigration_level_ex = '" + GSString(m_fEmigrationLevelExpected) + "'" \
				", immigration_level = '" + GSString(m_fImmigrationLevel) + "'" \
				", global_tax_mod = '" + GSString(m_fGlobalTaxMod) + "'" \
				", hier_distance = '" + GSString(m_fAIHierarchicalDistance) + "'" \
				", male_female = '" + GSString(m_fAIMaleFemale) + "'" \
				", indiv_collec = '" + GSString(m_fAIIndividualism) + "'" \
				", groups = '" + GSString(m_iCountryGroup) + "'" \
				", birth_rate_ex = '" + GSString(m_fBirthRateExpected) + "'" \
				", death_rate_ex = '" + GSString(m_fDeathRateExpected) + "'" \
				", telecom_level_ex = '" + GSString(m_fTelecomLevelExpected) + "'" \
				", infrastructure_level_ex = '" + GSString(m_fInfrastructureExpected) + "'" \
				", economic_health = '" + GSString(m_fEconomicHealth) + "'" \
				", corruption_level_ex = '" + GSString(m_fCorruptionLevelExpected) + "'" ;
	if (m_NextElection != NULL)
	{
		l_sUpdateQuery += ", next_election = '" + GSString(m_NextElection.ToString(GDateTime::etfMMDDYYYY)) + "'";
	}

	l_sUpdateQuery += " WHERE Id = " + GSString(m_iCountryID);	

	in_vQueries.push_back(l_sUpdateQuery);
	
	//Insert the languages status for that country
	for(set<INT32>::iterator it = m_LanguageIllegalStatus.begin();
		 it != m_LanguageIllegalStatus.end(); it++)
	{
		GSString l_sInsertQuery = "INSERT INTO languages_status VALUES(" +
               GSString(in_iLanguageStatusIterator++)         + " , " +
               GSString(m_iCountryID)				 + " , " +
               GSString(*it)    + " , " +
               GSString(2)		  + ")";		
		in_vQueries.push_back(l_sInsertQuery); 
	}
	for(set<INT32>::iterator it = m_LanguageOfficialStatus.begin();
		 it != m_LanguageOfficialStatus.end(); it++)
	{
		GSString l_sInsertQuery = "INSERT INTO languages_status VALUES(" +
               GSString(in_iLanguageStatusIterator++)         + " , " +
               GSString(m_iCountryID)				 + " , " +
               GSString(*it)    + " , " +
               GSString(3)		  + ")";		
		in_vQueries.push_back(l_sInsertQuery); 
	}
	

	//Insert the religions status for that country	
	for(set<INT32>::iterator it = m_ReligionIllegalStatus.begin();
		 it != m_ReligionIllegalStatus.end(); it++)
	{
		GSString l_sInsertQuery = "INSERT INTO religions_status VALUES(" +
               GSString(in_iReligionStatusIterator++)         + " , " +
               GSString(m_iCountryID)				 + " , " +
               GSString(*it)    + " , " +
               GSString(2)		  + ")";		
		in_vQueries.push_back(l_sInsertQuery); 
	}
	for(set<INT32>::iterator it = m_ReligionOfficialStatus.begin();
		 it != m_ReligionOfficialStatus.end(); it++)
	{
		GSString l_sInsertQuery = "INSERT INTO religions_status VALUES(" +
               GSString(in_iReligionStatusIterator++)         + " , " +
               GSString(m_iCountryID)				 + " , " +
               GSString(*it)    + " , " +
               GSString(3)		  + ")";		
		in_vQueries.push_back(l_sInsertQuery); 
	}
	
	//Insert the political parties of that country
	for(UINT32 i=0; i<m_vPoliticalParty.size(); i++)
	{
				
		GSString l_sInsertQuery = "INSERT INTO parties VALUES(" +
               GSString(m_vPoliticalParty[i].Id())          + " , " +
               GSString(m_iCountryID)								+ " , " +
               GSString(m_vPoliticalParty[i].NameId())      + " , " +
					GSString(m_vPoliticalParty[i].Status())      + " , " +
					GSString(m_vPoliticalParty[i].PercentageValue())      + " , " +
					GSString(m_vPoliticalParty[i].PoliticalIdeology())      + " , " +
					GSString(m_vPoliticalParty[i].GvtType())      + " , ";
		if(m_vPoliticalParty[i].InPower())
			l_sInsertQuery += "'T')";
		else
			l_sInsertQuery += "'F')";

		in_vQueries.push_back(l_sInsertQuery); 
	}

	//Don't forget to multiply the resource taxes and gdp by 100, since I'm dividing them by 100 at loading
	 
	GSString l_ResourceStrings[EResources::ItemCount] = {"cereals",
																	"veg_fruits",
																	"meat",
																	"dairy",                            
																	"tobacco",
																	"drugs",
																	"electricity",
																	"fossile_fuels",
																	"wood_paper",
																	"minerals",
																	"iron_steel",
																	"prec_stones",                            
																	"fabrics",
																	"plastics",                            
																	"chemicals",
																	"pharma",
																	"hh_appl",
																	"vehicules",
																	"machinery",
																	"hh_comm",                            
																	"luxury_comm",
																	"construction",
																	"engineering",
																	"health_care",
																	"retail",
																	"legal_serv",
																	"market_ad"
																};
	GSString l_sUpdateQueryResources("UPDATE resources SET ");
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
	l_sUpdateQueryResources += l_ResourceStrings[i] + "_gdp = '" + GSString(m_pResourceGDP[i] * 100.f) + "', " + \
	l_ResourceStrings[i] + "_taxes = '" + GSString(m_pResourceTaxes[i] * 100.f) + "', " + \
	l_ResourceStrings[i] + "_import = '" + GSString(m_pResourceImport[i]) + "', " + \
	l_ResourceStrings[i] + "_import_desired = '" + GSString(m_pResourceImportDesired[i]) + "', " + \
	l_ResourceStrings[i] + "_export = '" + GSString(m_pResourceExport[i]) + "', " + \
	l_ResourceStrings[i] + "_export_desired = '" + GSString(m_pResourceExportDesired[i]) + "', " + \
	l_ResourceStrings[i] + "_gvt_ctrl = '";
	if(m_pResourceGvtControl[i])
		l_sUpdateQueryResources += "T', ";
	else
		l_sUpdateQueryResources += "F', ";
	l_sUpdateQueryResources += l_ResourceStrings[i] + "_legal = '";
	if(m_pResourceLegal[i])
		l_sUpdateQueryResources += "T', ";
	else
		l_sUpdateQueryResources += "F', ";
	l_sUpdateQueryResources += l_ResourceStrings[i] + "_meet = '";
	if(m_pResourceMeetDomesticConsumption[i])
		l_sUpdateQueryResources += "T'";
	else
		l_sUpdateQueryResources += "F'";
	if(i != ((UINT32)EResources::ItemCount - 1))
		l_sUpdateQueryResources += ", ";
	}
	l_sUpdateQueryResources += " WHERE id = " + GSString(m_iCountryID);
	in_vQueries.push_back(l_sUpdateQueryResources);
	
   // Save the nuclear missile list
   {		      
      for(UINT32 i = 0 ; i < m_vGroundBasedNuclearMissiles.size() ; i++)
      {
         
			GSString l_sInsertQuery = "insert into missile values(" +
                                    GSString(m_vGroundBasedNuclearMissiles[i].m_iID)         + " , " +
                                    GSString(m_vGroundBasedNuclearMissiles[i].m_iID)         + " , " +
                                    GSString(m_vGroundBasedNuclearMissiles[i].m_iOwnerID)    + " , " +
                                    GSString(m_vGroundBasedNuclearMissiles[i].m_iDesignID)   + " , " +
                                    GSString(m_vGroundBasedNuclearMissiles[i].m_iQty)        + " , " +
                                    GSString(m_vGroundBasedNuclearMissiles[i].Position().x)  + " , " + 
												GSString(m_vGroundBasedNuclearMissiles[i].Position().y)  + " , " + 
												GSString(0)															  + " , " + 
                                    GSString(0)  + ")";		
			in_vQueries.push_back(l_sInsertQuery);         
      }
      for(UINT32 i = 0 ; i < m_vSubmarineBasedNuclearMissiles.size() ; i++)
      {
         GSString l_sInsertQuery = "insert into missile values(" +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].m_iID)         + " , " +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].m_iID)         + " , " +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].m_iOwnerID)    + " , " +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].m_iDesignID)   + " , " +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].m_iQty)        + " , " +
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].Position().x)  + " , " + 
												GSString(m_vSubmarineBasedNuclearMissiles[i].Position().y)  + " , " + 
												GSString(1)																  + " , " + 
                                    GSString(m_vSubmarineBasedNuclearMissiles[i].AttachedSubmarine())  + ")";
         in_vQueries.push_back(l_sInsertQuery);
      }
   }

	 //Save its country data
   ((SP2::GResearchInformation*)m_pResearchInformation)->Save(m_iCountryID,in_vQueries);
	
	//Commit the covert action cells information
   {      
		INT32 l_iNameID = 0;
      for(UINT32 i = 0 ; i < m_vCovertActionCells.size() ; i++)
      {			
         l_iNameID = m_vCovertActionCells[i].NameID();
			if(l_iNameID== -1)
			{
				l_iNameID = in_StringTable.AddData(m_vCovertActionCells[i].Name().c_str(),m_vCovertActionCells[i].Name().size()*2);
			}
			
			GSString l_sInsertQuery = "insert into cov_ops_cell values(" +
													GSString(in_iCovertOpsStatusIterator++) + " , " +
                                       GSString(m_vCovertActionCells[i].ID()) + " , " +
                                       GSString(l_iNameID) + " , " +
                                       GSString(m_vCovertActionCells[i].OwnerID()) + " , " +
                                       GSString(m_vCovertActionCells[i].UnitCategory()) + " , " +
                                       GSString(m_vCovertActionCells[i].TargetSector()) + " , " +
                                       GSString(m_vCovertActionCells[i].ExperienceLevel()) + " , " +
                                       GSString(m_vCovertActionCells[i].CountryToFrame()) + " , " +
                                       GSString(m_vCovertActionCells[i].AssignedCountry()) + " , " +
                                       GSString(m_vCovertActionCells[i].MissionType()) + " , " +
                                       GSString(m_vCovertActionCells[i].MissionComplexity()) + " , " +
                                       GSString(m_vCovertActionCells[i].ActualState()) + ")";
         in_vQueries.push_back(l_sInsertQuery);
      }
   }//end savecovert action cells

	return true;
}



GResearchInformation::GResearchInformation()
{
   GDZLOG(EDZLogLevel::Entry, L"");

   //Base class members
   m_fBudgetGround  = 0.0;
   m_fBudgetAir     = 0.0;
   m_fBudgetNaval   = 0.0;
   m_fBudgetNuclear = 0.0;

   for(INT32 i = EUnitCategory::Ground; i < EUnitCategory::ItemCount; i++)
   {
       for(INT32 j = EUnitDesignCharacteristics::GunRange; j < EUnitDesignCharacteristics::ItemCount; j++)
       {
           m_fBudget[i][j]    = 0.0;
           m_fMaxValues[i][j] = 0.f;
       }
   }

   //Child class members
   m_fLastCheckClock = 0;

   GDZLOG(EDZLogLevel::Exit, L"");
}
GResearchInformation::~GResearchInformation()
{}


/*!
 * Updates the research Information
 **/
void GResearchInformation::Update(const REAL64& in_fClock, REAL64 in_fBudgetExpenseResearch, REAL64 in_fBonusToResearch)
{
	gassert(!_isnan(in_fBonusToResearch),"Research Info NAN");

	if(in_fBudgetExpenseResearch <= 0.f)
		in_fBudgetExpenseResearch = 1.f;

   REAL64 l_fBonusModificator = 1.f + (in_fBonusToResearch / in_fBudgetExpenseResearch);

   //! \todo : Maintaining this code is a REAL pain in the ass, using arrays with an enum would be so wonderful...
   // This code is far from being DRY...
   for(UINT32 i = EUnitCategory::Ground ; i < EUnitCategory::ItemCount ; i++)
   {
      for(UINT32 j =0 ; j < EUnitDesignCharacteristics::ItemCount ; j++)
      {
         m_fMaxValues[i][j] = (REAL32)ComputeProgression((m_fBudget[i][j])* l_fBonusModificator,m_fMaxValues[i][j], in_fClock );
      }
   }

   m_fLastCheckClock = in_fClock;
}

/*!
 * Returns the new progression in the given 
 * The clock received in argument corresponds to the game time (in days)
 **/
REAL64 GResearchInformation::ComputeProgression(REAL64 in_fBudget,REAL64 in_fActualLevel,const REAL64& in_fClock)
{
   //If no budget, nothing advances
   if(!in_fBudget)
   { 
      return in_fActualLevel;
   }
   
   //Dont go further the last technology level
   if(in_fActualLevel >= (REAL32)c_iMaxResearchLevel)
   {
      return (REAL32)c_iMaxResearchLevel;
   }

   gassert(in_fActualLevel < c_iMaxResearchLevel,"Invalid level");

   REAL64 l_fTimeToCompletionDays = TimeToCompletion(in_fActualLevel,in_fBudget);
   
   //TimeDelay since last check
   REAL64 l_fNbDaysSinceLastCheck   = in_fClock - m_fLastCheckClock;

   REAL64 l_fProgressionRatioUntilEnd = l_fNbDaysSinceLastCheck/l_fTimeToCompletionDays;
   //If we reached the next level
   if(l_fProgressionRatioUntilEnd >= 1)
   {
      return (REAL64)(UINT32)in_fActualLevel+1; //we cast into UINT32 to remove the decimals
   }
   //Compute the progression
   else
   {
      REAL64 l_fActualProgressionIntoLvl = in_fActualLevel - (UINT32)in_fActualLevel;
      return in_fActualLevel + (1-l_fActualProgressionIntoLvl)*l_fProgressionRatioUntilEnd;
   }

/*

   //---------WARNING ----------------
   //IF YOU CHANGE THIS, MAKE SURE YOU REFLECT THE CHANGES INTO THE GResearchInformationHolder::TimeToCompletion METHOD
   
   //If no budget, nothing advances
   if(!in_fBudget)
   { 
      return in_fActualLevel;
   }
   
   //Dont go further the last technology level
   if(in_fActualLevel >= (REAL32)c_iMaxResearchLevel)
   {
      return (REAL32)c_iMaxResearchLevel;
   }

   gassert(in_fActualLevel < c_iMaxResearchLevel,"Invalid level");


   //Budget ratio, if developping at normal speed, it will take the std number of days.
   REAL32 l_fBudgetRatio = (REAL32)(in_fBudget / g_ServerDAL.MilitaryResearchStdBudget());


   //Using an equation found in GraphCalc [dont ask how, just know that max speed ratio is about 2]
   //compute the impact that the budget ratio has on the progression
   REAL32 l_fSpeedRatio = (REAL32)log10((l_fBudgetRatio+0.1f)*0.08f)+2.12f;
   
   UINT32 l_iActualLevel            = (UINT32)in_fActualLevel;
   UINT32 l_iNbDayToFullyLevelUp    = g_ServerDAL.MilitaryResearchTimeInDays()[l_iActualLevel];

   //TimeDelay since last check
   REAL64 l_fNbDaysSinceLastCheck   = in_fClock - m_fLastCheckClock;

   REAL32 l_fNewProgressionDone = (REAL32)(l_fNbDaysSinceLastCheck/l_iNbDayToFullyLevelUp*l_fSpeedRatio);

   //Check if we lvl up, if so, dont allow to go further than the beginning of the next level
   UINT32 l_iNewLevel = (UINT32)(in_fActualLevel + l_fNewProgressionDone);   //cast into int32 to remove the decimals
   if(l_iNewLevel > in_fActualLevel)
   {
      return (REAL64)l_iNewLevel;
   }
   else
   {
      //We did not pass the level
      return (in_fActualLevel + l_fNewProgressionDone);
   }
   */
}

bool GResearchInformation::Load(UINT32 in_iCountryID)
{
   DB::GTable l_Table;
   GSString l_sSelectQuery("SELECT * from research WHERE ID = " + GSString(in_iCountryID));	
   DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_Table);
	if(e_Error != DB_NO_ERROR)
	{
      g_Joshua.Log(L"Error during SELECT query on the research table SP2::GResearchInformation::Load",MSGTYPE_ERROR);
		return false;
	}      		

   l_Table.Row(0)->Cell( 1)->GetData(m_fBudgetAir);
   l_Table.Row(0)->Cell( 2)->GetData(m_fBudgetGround);
   l_Table.Row(0)->Cell( 3)->GetData(m_fBudgetNaval);
   l_Table.Row(0)->Cell( 4)->GetData(m_fBudgetNuclear);
   l_Table.Row(0)->Cell( 5)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor ]);
   l_Table.Row(0)->Cell( 6)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures ]);
   l_Table.Row(0)->Cell( 7)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage ]);
   l_Table.Row(0)->Cell( 8)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision ]);
   l_Table.Row(0)->Cell( 9)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange ]);
   l_Table.Row(0)->Cell(10)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage ]);
   l_Table.Row(0)->Cell(11)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload ]);
   l_Table.Row(0)->Cell(12)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision ]);
   l_Table.Row(0)->Cell(13)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange ]);
   l_Table.Row(0)->Cell(14)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors ]);
   l_Table.Row(0)->Cell(15)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed ]);
   l_Table.Row(0)->Cell(16)->GetData(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth ]);
   l_Table.Row(0)->Cell(17)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor ]);
   l_Table.Row(0)->Cell(18)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::CounterMesures ]);
   l_Table.Row(0)->Cell(19)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage ]);
   l_Table.Row(0)->Cell(20)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision ]);
   l_Table.Row(0)->Cell(21)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange ]);
   l_Table.Row(0)->Cell(22)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage ]);
   l_Table.Row(0)->Cell(23)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload ]);
   l_Table.Row(0)->Cell(24)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision ]);
   l_Table.Row(0)->Cell(25)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange ]);
   l_Table.Row(0)->Cell(26)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors ]);
   l_Table.Row(0)->Cell(27)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed ]);
   l_Table.Row(0)->Cell(28)->GetData(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Stealth ]);   
   l_Table.Row(0)->Cell(29)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor ]);
   l_Table.Row(0)->Cell(30)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures ]);
   l_Table.Row(0)->Cell(31)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage ]);
   l_Table.Row(0)->Cell(32)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision ]);
   l_Table.Row(0)->Cell(33)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange ]);
   l_Table.Row(0)->Cell(34)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage ]);
   l_Table.Row(0)->Cell(35)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload ]);
   l_Table.Row(0)->Cell(36)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision ]);
   l_Table.Row(0)->Cell(37)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange ]);
   l_Table.Row(0)->Cell(38)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors ]);
   l_Table.Row(0)->Cell(39)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed ]);
   l_Table.Row(0)->Cell(40)->GetData(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth ]);
   l_Table.Row(0)->Cell(41)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Armor ]);
   l_Table.Row(0)->Cell(42)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::CounterMesures ]);
   l_Table.Row(0)->Cell(43)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunDamage ]);
   l_Table.Row(0)->Cell(44)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunPrecision ]);
   l_Table.Row(0)->Cell(45)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunRange ]);
   l_Table.Row(0)->Cell(46)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage ]);
   l_Table.Row(0)->Cell(47)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePayload ]);
   l_Table.Row(0)->Cell(48)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision ]);
   l_Table.Row(0)->Cell(49)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange ]);
   l_Table.Row(0)->Cell(50)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Sensors ]);
   l_Table.Row(0)->Cell(51)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Speed ]);
   l_Table.Row(0)->Cell(52)->GetData(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Stealth ]);
   REAL64 l_fTemp;
   l_Table.Row(0)->Cell(53)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(54)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(55)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(56)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(57)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(58)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(59)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(60)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(61)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(62)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(63)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(64)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(65)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(66)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::CounterMesures ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(67)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(68)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(69)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(70)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(71)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(72)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(73)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(74)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(75)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(76)->GetData(l_fTemp);
   m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Stealth ]  = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(77)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(78)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(79)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(80)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(81)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(82)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(83)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(84)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(85)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(86)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(87)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(88)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(89)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Armor ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(90)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::CounterMesures ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(91)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(92)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunPrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(93)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(94)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(95)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePayload ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(96)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(97)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(98)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Sensors ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(99)->GetData(l_fTemp    );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Speed ] = (REAL32)l_fTemp;
   l_Table.Row(0)->Cell(100)->GetData(l_fTemp   );
   m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Stealth ] = (REAL32)l_fTemp;

   return true;
}

bool GResearchInformation::Save(UINT32 in_iCountryID, vector<GSString>& in_vQueries)
{	
	
   GSString l_sDeleteQuery("delete from research where id = " + GSString(in_iCountryID));
	in_vQueries.push_back(l_sDeleteQuery);
   
   GSString l_sUpdateQuery("insert into research values(" +
                           GSString(in_iCountryID) + " , " +
                           GSString(m_fBudgetAir) + " , "  +
                           GSString(m_fBudgetGround) + " , "  +
                           GSString(m_fBudgetNaval) + " , "  +
                           GSString(m_fBudgetNuclear) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Stealth ]) + " , "  +   
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Stealth ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Stealth ]) + " , "  +   
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Armor ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::CounterMesures ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunPrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::GunRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePayload ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Sensors ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Speed ]) + " , "  +
                           GSString(m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Stealth ]) + ") ");

	in_vQueries.push_back(l_sUpdateQuery);
   
   return true;
}

bool GResearchInformation::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_fBudgetAir
             << m_fBudgetGround
             << m_fBudgetNaval
             << m_fBudgetNuclear;

   for(UINT32 i = 0;i < EUnitCategory::ItemCount;i ++)
   {
      for(UINT32 j = 0;j < EUnitDesignCharacteristics::ItemCount;j ++)
      {
         io_Buffer << m_fBudget[i][j]
                   << m_fMaxValues[i][j];
      }
   }

   io_Buffer << m_fLastCheckClock;

   return true;
}

bool GResearchInformation::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_fBudgetAir
             >> m_fBudgetGround
             >> m_fBudgetNaval
             >> m_fBudgetNuclear;

   for(UINT32 i = 0;i < EUnitCategory::ItemCount;i ++)
   {
      for(UINT32 j = 0;j < EUnitDesignCharacteristics::ItemCount;j ++)
      {
         io_Buffer >> m_fBudget[i][j]
                   >> m_fMaxValues[i][j];
      }
   }

   io_Buffer >> m_fLastCheckClock;

   return true;
}

bool GResearchInformation::OnNew(GDatabase* in_pDatabase)
{
   m_fLastCheckClock = 0;

   return true;
}


/*!
 * AddNewMissile        : Add a new missile to the nuclear missile list
 * @param in_newMissile : The new missile to be added to the list
 **/
void GCountryData::AddNewMissile(GNuclearMissile &in_NewMissile)
{
   if(in_NewMissile.FromSubmarine())
      m_vSubmarineBasedNuclearMissiles.push_back(in_NewMissile);
   else
      m_vGroundBasedNuclearMissiles.push_back(in_NewMissile);

   m_bNuclearMissilesDataDirty = true;

}

GAdvisor* GCountryData::Advisor()
{
	return m_pAdvisor;
}

/*!
 * @param in_eEconomic: Advisor status for the economic side
 * @param in_ePolitic: Advisor status for the politic side
 * @param in_eMilitary: Advisor status for the military side
 **/

void GCountryData::AddAdvisor(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary)
{
	if(!m_pAdvisor)
	{
		m_pAdvisor = new GAdvisor(in_eEconomic,in_ePolitic,in_eMilitary);		
      m_eAdvisorEconomicStatus = m_pAdvisor->EconomicStatus();
      m_eAdvisorMilitaryStatus = m_pAdvisor->MilitaryStatus();
      m_eAdvisorPoliticStatus = m_pAdvisor->PoliticStatus();
      m_bOtherDataDirty = true;
	}
}

void GCountryData::RemoveAdvisor()
{
	SAFE_DELETE(m_pAdvisor);
}



/*!
 * Adds the nuclear missiles that are in that submarine to the list of nuclear missiles for that country
 **/
void GCountryData::CreateMissilesForNuclearSubmarine(const UINT32 in_iNuclearSubmarineID)
{
   SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_iNuclearSubmarineID);
   gassert(l_pUnit, "GCountryData::CreateMissilesForNuclearSubmarine Unit with the given ID should exist");
   SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_pUnit->Design();
   gassert(l_pUnitDesign && l_pUnitDesign->Type()->Id() == c_iNuclearSubmarineUnitTypeID,"GCountryData::CreateMissilesForNuclearSubmarine: Invalid unit");

   //Get the missile payload
   UINT32 l_iMissilePayload = (UINT32)l_pUnitDesign->Type()->MissilePayloadByLevel()[l_pUnitDesign->MissilePayload()-1];
   for(UINT32 i = 0 ; i < l_iMissilePayload * l_pUnit->Qty() ; i++)
   {
      GNuclearMissile   l_Missile;
      l_Missile.m_iID       = g_Joshua.UnitManager().NewUnitID();
      l_Missile.m_iOwnerID  = l_pUnit->Group()->OwnerId();
      l_Missile.m_iDesignID = l_pUnitDesign->Id();
      // JMercier TODO : When qty at same position will be supported  change this for lmissilepayload and remove the variable from for loop
      l_Missile.m_iQty      = 1;
      l_Missile.AttachedSubmarine(in_iNuclearSubmarineID);
      l_Missile.FromSubmarine(true);
      m_vSubmarineBasedNuclearMissiles.push_back(l_Missile);
   }   
   m_bNuclearMissilesDataDirty = true;
}

/*!
 * Removes all the nuclear missiles that were in that submarine
 **/
void GCountryData::RemoveMissilesForNuclearSubmarine(const UINT32 in_iNuclearSubmarineID)
{
   //List all the missiles that should be removed
   vector<UINT32> l_vMissileIDToRemove;
   {
      for(UINT32 i = 0; i < m_vSubmarineBasedNuclearMissiles.size() ; i++)
      {
         if(m_vSubmarineBasedNuclearMissiles[i].FromSubmarine() && in_iNuclearSubmarineID == m_vSubmarineBasedNuclearMissiles[i].AttachedSubmarine())
         {
            l_vMissileIDToRemove.push_back(i);
         }
      }
      reverse(l_vMissileIDToRemove.begin(),l_vMissileIDToRemove.end());
   }

   //Remove all the missiles
   for(vector<UINT32>::iterator l_It = l_vMissileIDToRemove.begin();
       l_It != l_vMissileIDToRemove.end();
       l_It++)
   {
      m_vSubmarineBasedNuclearMissiles.erase(m_vSubmarineBasedNuclearMissiles.begin()+*l_It);
   }
   m_bNuclearMissilesDataDirty = true;
}


void GCountryData::StartNuclearSubmarineReloadClock()
{
   if(m_fReloadNuclearSubmarinesClock == -1.f)
      m_fReloadNuclearSubmarinesClock = (REAL32)g_Joshua.GameTime();
}
/*!
 * Reload all the nuclear submarine missiles for this country
 **/
void GCountryData::ReloadSubmarinesNuclearMissiles()
{
   if(!m_bActivated)
      return;

   //If the reload clock is not started, dont reload
   if(m_fReloadNuclearSubmarinesClock == -1.f)
   {
      return;
	}
   //If it is not yet the time to reload
   else if( (g_Joshua.GameTime() - m_fReloadNuclearSubmarinesClock) < g_ServerDCL.MissileRearmingDelay(g_ServerDAL.UnitTypes()[c_iNuclearSubmarineUnitTypeID-1]))
   {
      return;
   }

   //Ignore the production queue group for the reload
   gassert(g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[m_iCountryID - 1], "No production group for an active country");
   UINT32 l_iProductionQueueGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[m_iCountryID - 1]->Id();

   //Generate a list of attached submarines and existing missiles attached
   //Key = attached submarine id, value = nb of missiles existing for that sub
   hash_map<UINT32,UINT32> l_hActualMissiles;
   {
      for(UINT32 i = 0; i < m_vSubmarineBasedNuclearMissiles.size() ; i++)
      {
         if(l_hActualMissiles.find(m_vSubmarineBasedNuclearMissiles[i].AttachedSubmarine()) != l_hActualMissiles.end())
         {
            l_hActualMissiles[m_vSubmarineBasedNuclearMissiles[i].AttachedSubmarine()]++;
         }
         else
         {
            l_hActualMissiles[m_vSubmarineBasedNuclearMissiles[i].AttachedSubmarine()] = 1;
         }
      }
   }
   //Get the units of the country
   const set<UINT32>& l_UnitGroupsId = g_Joshua.UnitManager().CountryUnitGroups(m_iCountryID);
   for(set<UINT32>::const_iterator l_UnitGroupIt = l_UnitGroupsId.begin();
       l_UnitGroupIt != l_UnitGroupsId.end();
       l_UnitGroupIt++ )
   {
      //Only check the group if it is naval and is not the production queue group
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIt);
      if(l_pGroup->IsNaval() && (l_pGroup->Id() != l_iProductionQueueGroup))
      {
         const vector<SDK::Combat::GUnit*>& l_vUnits = l_pGroup->Units();
         //for each unit
         for(vector<SDK::Combat::GUnit*>::const_iterator l_UnitIt = l_vUnits.begin();
             l_UnitIt != l_vUnits.end();
             l_UnitIt++)
         {
            SP2::GUnit* l_pUnit = (SP2::GUnit*)*l_UnitIt;
            //Check if the unit is a nuclear submarine
            SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();
            if(l_pDesign->Type()->Id() == c_iNuclearSubmarineUnitTypeID)
            {
               //Check if all the missiles for that unit are present, if there are missing missiles, create some
               UINT32 l_iMissilePayload = 0;
               UINT32 l_iMissilePayloadLevel = l_pDesign->MissilePayload(); 
               if(l_iMissilePayloadLevel)
               {                  
                  SP2::GUnitType* l_pType = l_pDesign->Type();
                  l_iMissilePayload = (UINT32)l_pType->MissilePayloadByLevel()[l_iMissilePayloadLevel-1];
               }

               UINT32 l_iAlreadyPresentMissilesForThatUnit = 0;
               if(l_hActualMissiles.find(l_pUnit->Id()) != l_hActualMissiles.end())
               {
                  l_iAlreadyPresentMissilesForThatUnit = l_hActualMissiles[l_pUnit->Id()];
               }
               
					l_iAlreadyPresentMissilesForThatUnit = min(l_iAlreadyPresentMissilesForThatUnit,l_iMissilePayload);

               for(UINT32 i = l_iAlreadyPresentMissilesForThatUnit ; i <= l_iMissilePayload ; i++)
               {
                  //Create a missile
                  GNuclearMissile   l_Missile;
                  l_Missile.m_iID       = g_Joshua.UnitManager().NewUnitID();
                  l_Missile.m_iOwnerID  = l_pUnit->Group()->OwnerId();
                  l_Missile.m_iDesignID = l_pDesign->Id();
                  l_Missile.m_iQty      = 1;
                  l_Missile.AttachedSubmarine(l_pUnit->Id());
                  l_Missile.FromSubmarine(true);
                  m_vSubmarineBasedNuclearMissiles.push_back(l_Missile);
               }
            }
         }//end of for each unit of the group
      }
   }//end of for each group of the country
   m_fReloadNuclearSubmarinesClock = -1.f;
}

void		GCountryData::LastIteration(REAL64	in_fLastIteration)
{
   m_fLastIteration = in_fLastIteration;
}
REAL64	GCountryData::LastIteration()
{
   return m_fLastIteration;
}

REAL32 GCountryData::IndividualismCollectivism() const {return m_fAIIndividualism;}
REAL32 GCountryData::MaleFemale() const {return m_fAIMaleFemale;}
REAL32 GCountryData::HierarchicalDistance() const {return m_fAIHierarchicalDistance;}

void  GCountryData::IterateCovertActionCells()
{
	bool l_bCellsHaveChanged = false;

    SDK::GPlayer* const l_pPlayer = g_Joshua.ActivePlayerByModID(m_iCountryID);
    const bool l_bIsHumanPlayer = l_pPlayer != nullptr && !l_pPlayer->AIControlled();
    const bool l_bAutoCovertMissions = (l_bIsHumanPlayer && g_SP2Server->AutoCovertMissions());

    set<UINT32> l_vCellsToRemove;

    for(auto it = m_vCovertActionCells.begin(); it < m_vCovertActionCells.end(); ++it)
    {
        const INT32 l_iOldState = it->ActualState();
        it->Iterate();
        l_bCellsHaveChanged = (l_bCellsHaveChanged || l_iOldState != it->ActualState());

        if(l_bAutoCovertMissions)
        {
            //Auto execute and repeat mission
            if(it->ActualState() == ECovertActionsCellState::ReadyToExecute &&
               it->MissionType() != SP2::ECovertActionsMissionType::CoupEtat)
            {
                const ENTITY_ID l_iTargetID = it->AssignedCountry();
                const UINT32 l_iCellID = it->ID();
                if(g_ServerDAL.CountryValidityArray(l_iTargetID))
                {
                    if(!g_ServerDCL.ExecuteMission(*it))
                    {
                        //Set the cell back to active before preparing the mission again
                        //This is needed to force the cell to spend time preparing its next mission, rather than execute the next mission instantly
                        it->ChangeState(ECovertActionsCellState::Active);

                        GDZLOG(EDZLogLevel::Info2, g_ServerDAL.CovertCellInfoForLog(m_iCountryID, l_iCellID) + L" wasn't captured after executing auto mission; restarting mission");
                        it->ChangeState(ECovertActionsCellState::PreparingMission);
                    }
                    else
                    {
                        GDZLOG(EDZLogLevel::Info2, g_ServerDAL.CovertCellInfoForLog(m_iCountryID, l_iCellID) + L" was captured after executing auto mission; marking cell for removal");
                        l_vCellsToRemove.insert(l_iCellID);
                    }
                }
                else
                {
                    GDZLOG(EDZLogLevel::Info1, g_ServerDAL.CovertCellInfoForLog(m_iCountryID, l_iCellID) + L" is cancelling an automatic mission against " + GString(l_iTargetID) + L" due to target inactivity");
                    it->CancelAction();
                    it->AssignedCountry(m_iCountryID);
                }

                m_bCovertActionCellsDirty = true;
            }
        }
    }

    gassert(l_vCellsToRemove.empty() || l_bAutoCovertMissions, L"Not possible for cells to require removal here, without auto covert missions");

    for(auto it = l_vCellsToRemove.cbegin(); it != l_vCellsToRemove.cend(); ++it)
    {
        GDZLOG(EDZLogLevel::Info2, L"Removing captured cell " + g_ServerDAL.CovertCellInfoForLog(m_iCountryID, *it));
        RemoveCovertActionCell(CovertActionCell(*it));
    }

    //m_bCovertActionCellsDirty seems to get reset upon calling RemoveCovertActionCell, and it needs to be set after any RemoveCovertActionCell calls
    m_bCovertActionCellsDirty = (m_bCovertActionCellsDirty || !l_vCellsToRemove.empty());

    if(m_bCovertActionCellsDirty)
        GDZLOG(EDZLogLevel::Info2, L"m_bCovertActionCellsDirty TRUE");
    
    if(l_bCellsHaveChanged || m_bCovertActionCellsDirty)
	{
		g_ServerDAL.UpdateAccuracyOfInformation(m_iCountryID);
		m_fNationalSecurity = g_ServerDCL.FindNationalSecurity(m_iCountryID);
	}
}

vector<GDesignFormat>& GCountryData::DesignFormat(EUnitType::Enum in_eType)
{
   return m_vDesignFormat[in_eType];
}

REAL32 GCountryData::LifeExpectancy() const
{
    return m_fLifeExpectancy;
}

void GCountryData::LifeExpectancy(REAL32 in_fLifeExpectancy)
{
    m_fLifeExpectancy = in_fLifeExpectancy;
}

REAL32 GCountryData::MeanYearsSchooling() const
{
    return m_fMeanYearsSchooling;
}

void GCountryData::MeanYearsSchooling(REAL32 in_fMeanYearsSchooling)
{
    m_fMeanYearsSchooling = in_fMeanYearsSchooling;
}

REAL32 GCountryData::ExpectedYearsSchooling() const
{
    return m_fExpectedYearsSchooling;
}

void GCountryData::ExpectedYearsSchooling(REAL32 in_fExpectedYearsSchooling)
{
    m_fExpectedYearsSchooling = in_fExpectedYearsSchooling;
}

REAL32 GCountryData::FindIncomeIndex(const REAL64 in_fGDPPerCapita, const bool in_bCap)
{
    static const REAL64 c_fMinIncome = 100;
    static const REAL64 c_fMaxIncome = 60000;

    REAL32 l_fIncomeIndex = 0.f;

    if(100 < in_fGDPPerCapita)
        l_fIncomeIndex = static_cast<REAL32>(log(in_fGDPPerCapita/c_fMinIncome) / log(c_fMaxIncome/c_fMinIncome));

    l_fIncomeIndex = in_bCap ? min(l_fIncomeIndex, 1) : l_fIncomeIndex;
    return l_fIncomeIndex;
}

REAL32 GCountryData::FindHumanDevelopment(const REAL32 in_fLifeExpectancy, const REAL32 in_fMeanYearsSchooling, const REAL32 in_fExpectedYearsSchooling, const REAL64 in_fGDPPerCapita, const bool in_bCap)
{
    static const REAL32 c_fMinLE = 20;
    static const REAL32 c_fMaxLE = 85;

    static const REAL32 c_fMaxMys = 15;
    static const REAL32 c_fMaxEys = 18;

    REAL32 l_fHI = max(0, (in_fLifeExpectancy - c_fMinLE) / (c_fMaxLE - c_fMinLE));
    l_fHI = in_bCap ? min(l_fHI, 1) : l_fHI;

    REAL32 l_fMeanYearsIndex = in_fMeanYearsSchooling / c_fMaxMys;
    l_fMeanYearsIndex = in_bCap ? min(l_fMeanYearsIndex, 1) : l_fMeanYearsIndex;

    REAL32 l_fExpectedYearsIndex = in_fExpectedYearsSchooling / c_fMaxEys;
    l_fExpectedYearsIndex = in_bCap ? min(l_fExpectedYearsIndex, 1) : l_fExpectedYearsIndex;

    const REAL32 l_fEI = (l_fMeanYearsIndex + l_fExpectedYearsIndex) / 2;

    const REAL32 l_fHdi = powf(l_fHI * l_fEI * GCountryData::FindIncomeIndex(in_fGDPPerCapita, in_bCap), 1.f/3.f);

    gassert((l_fHdi <= 1) || !in_bCap, L"Calculated an HDI greater than 1.000 when capped: " + GString(l_fHdi));

    return l_fHdi;
}

INT32 GCountryData::NumberOfPoliticallyControlledRegions() const
{
    INT32 l_iNumberOfPoliticallyControlledRegions = 0;

    const set<UINT32>& l_vPoliticalRegions = g_ServerDAL.CountryPoliticalControl(m_iCountryID);
    for(set<UINT32>::const_iterator l_It = l_vPoliticalRegions.cbegin();
        l_It != l_vPoliticalRegions.cend();
        ++l_It)
    {
        l_iNumberOfPoliticallyControlledRegions += (g_ServerDAL.RegionControl(*l_It).m_iMilitary == m_iCountryID);
    }

    return l_iNumberOfPoliticallyControlledRegions;
}

bool GCountryData::EligibleToBeClientOf(ENTITY_ID in_iMaster) const
{
    const GCountryData* const l_pMasterData = g_ServerDAL.CountryData(in_iMaster);
    GDZLOG(EDZLogLevel::Info1, L"Testing if " + NameAndIDForLog() + L" can be a client of " +
           l_pMasterData->NameAndIDForLog());

    //Check if the server allows human-controlled countries to become client states
    if(!g_SP2Server->AllowHumanClientStates())
    {
        SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(m_iCountryID);
        if(l_pPlayer == nullptr || !l_pPlayer->AIControlled())
        {
            GDZLOG(EDZLogLevel::Info1, NameAndIDForLog() + L" is ineligible to be a client because it is not AI-controlled, and the server disallows human-controlled countries from becoming client states");
            return false;
        }
    }

    //Master can't be a client itself
    if(l_pMasterData->Master().first != 0)
        return false;

    //Client state can't have an economic score higher than half the master's
    const REAL64 l_fClientOptimalEconomicScore = OptimalGDPValue() * m_fEconomicHealth;
    const REAL64 l_fMasterEconomicScore = l_pMasterData->GDPValue() * l_pMasterData->EconomicHealth();
    GDZLOG(EDZLogLevel::Info2, NameAndIDForLog() + L" economic score: " +
           GString::FormatNumber(l_fClientOptimalEconomicScore, L",", L".", L"$", L"") +
           L"; " +
           l_pMasterData->NameAndIDForLog() + L" economic score: " +
           GString::FormatNumber(l_fMasterEconomicScore, L",", L".", L"$", L""));
    if(l_fMasterEconomicScore / 2.0 < l_fClientOptimalEconomicScore)
        return false;

    //Client must have less than or equal to 1/10 of master's military strength
    const REAL32 l_fMasterMilitaryStrength = l_pMasterData->MilitaryStrength();
    GDZLOG(EDZLogLevel::Info2, NameAndIDForLog() + L" military: " +
           GString::FormatNumber(m_fMilitaryStrength, L",", L".", L"$", L"") +
           L"; " +
           l_pMasterData->NameAndIDForLog() + L" military: " +
           GString::FormatNumber(l_fMasterMilitaryStrength, L",", L".", L"$", L""));
    if(m_fMilitaryStrength > l_fMasterMilitaryStrength / 10.f)
        return false;

    //Client must be occupied sufficiently
    const REAL32 l_fPercentageOccupied = PercentageOfPopulationOccupiedByCountry(in_iMaster);
    GDZLOG(EDZLogLevel::Info2, NameAndIDForLog() + L" percentage occupied: " +
           GString(l_fPercentageOccupied * 100.f));
    if(l_fPercentageOccupied < 0.8f)
        return false;

    GDZLOG(EDZLogLevel::Info2, NameAndIDForLog() + L" is eligible");
    return true;
}

REAL64 GCountryData::OptimalGDPValue() const
{
    REAL64 l_fOptimalGDP = 0.f;

    const auto& l_vRegions = g_ServerDAL.CountryPoliticalControl(m_iCountryID);
	for(auto it = l_vRegions.begin(); it != l_vRegions.end(); it++)
	{
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
		if(l_pRegion == nullptr)
			continue;

		for(INT32 i = 0; i < EResources::ItemCount; i++)
        {
            auto l_eResource = static_cast<EResources::Enum>(i);
			l_fOptimalGDP += l_pRegion->ResourceProduction(l_eResource);
        }
    }

    return max(l_fOptimalGDP, m_fGDPValue);
}

void GCountryData::CheckForCivilWar()
{
    GDZLOG(EDZLogLevel::Entry, NameAndIDForLog());

    const GCivilWarConfig l_CivilWarConfig = g_SP2Server->CivilWarConfig();
    const ENTITY_ID l_iRebelsId = l_CivilWarConfig.m_iRebelsId;
    GDZLOG(EDZLogLevel::Info1, L"Rebels ID: " + GString(l_iRebelsId));

    if(static_cast<ENTITY_ID>(m_iCountryID) != l_iRebelsId && g_ServerDAL.CountryValidityArray(l_iRebelsId))
    {
        GCountryData* const l_pRebelsData = g_ServerDAL.CountryData(l_iRebelsId);
        const GString l_sRebelsName = l_pRebelsData->NameAndIDForLog();
        GDZLOG(EDZLogLevel::Info1, L"Rebels, " + l_sRebelsName + L", are active");

        Random::GQuick l_Rand;
        l_Rand.Seed(static_cast<UINT32>(g_Joshua.GameTime() * time(nullptr)));
        const REAL32 l_fCivilWarRandom = l_Rand.RandomReal();
        GDZLOG(EDZLogLevel::Info1, L"Civil war chance " + GString(l_CivilWarConfig.m_fChance) + L", random number " + GString(l_fCivilWarRandom));
        if(l_fCivilWarRandom < l_CivilWarConfig.m_fChance)
        {
            //Check if rebels take over any regions
            const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(m_iCountryID);
            bool l_bDirtyUnitGroups = false;
            bool l_bDeclareWar = false;
            bool l_bAnnexRegions = false;
            SDK::Combat::GUnitManager& l_UnitManager = g_Joshua.UnitManager();
            for(auto l_RegionIt = l_vRegions.cbegin(); l_RegionIt != l_vRegions.cend(); ++l_RegionIt)
            {
                GRegionEx* const l_pRegion = g_ServerDAL.GetGRegion(*l_RegionIt);
                const GString l_sRegionName = l_pRegion->NameForLog();
                GDZLOG(EDZLogLevel::Info1, L"Checking " + l_sRegionName + L" for occupation");

                const REAL32 l_fControlRandom = l_Rand.RandomReal();
                GDZLOG(EDZLogLevel::Info1, L"Rebel control chance " + GString(l_CivilWarConfig.m_fControlChance) + L", random number " + GString(l_fControlRandom));
                if(l_fControlRandom < l_CivilWarConfig.m_fControlChance)
                {
                    //Rebel military control; military units shift to rebels
                    GUnitMover& l_UnitMover = g_ServerDCL.UnitMover();
                    const set<UINT32> l_vUnitGroups = l_UnitMover.UnitGroupsInsideRegion(*l_RegionIt);
                    GDZLOG(EDZLogLevel::Info1, L"Number of unit groups that will switch sides: " + GString(l_vUnitGroups.size()));

                    for(auto l_UnitGroupIt = l_vUnitGroups.cbegin(); l_UnitGroupIt != l_vUnitGroups.cend(); ++l_UnitGroupIt)
                    {
                        SP2::GUnitGroup* const l_pGroup = dynamic_cast<SP2::GUnitGroup*>(g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIt));
                        if(l_pGroup->OwnerId() == static_cast<ENTITY_ID>(m_iCountryID))
                        {
                            switch(l_pGroup->Status())
                            {
                            case EMilitaryStatus::Moving:
                                GDZLOG(EDZLogLevel::Warning, L"Unit group ID " + GString(*l_UnitGroupIt) + L" is moving, won't switch sides");
                                break;

                            case EMilitaryStatus::InDeployement:
                                GDZLOG(EDZLogLevel::Warning, L"Unit group ID " + GString(*l_UnitGroupIt) + L" is in deployment, won't switch sides");
                                break;

                            case EMilitaryStatus::Attacking:
                                GDZLOG(EDZLogLevel::Warning, L"Unit group ID " + GString(*l_UnitGroupIt) + L" is in battle, won't switch sides");
                                break;

                            default:
                            {
                                GDZLOG(EDZLogLevel::Info1, L"Unit group ID " + GString(*l_UnitGroupIt) + L" switches sides");

                                //Need to create brand-new unit group; for some reason, trying to change an existing group's ownership would cause client-side crashes
                                SP2::GUnitGroup* const l_pNewGroup = dynamic_cast<SP2::GUnitGroup*>(l_UnitManager.CreateUnitGroup());
                                l_pNewGroup->Id(l_UnitManager.NewUnitGroupID());
                                l_pNewGroup->OwnerId(l_iRebelsId);
                                l_UnitManager.SetGroupPosition(l_pNewGroup, l_pGroup->Position());

                                vector<SDK::Combat::GUnit*> l_vUnits = l_pGroup->Units();
                                for(auto l_UnitIt = l_vUnits.begin(); l_UnitIt < l_vUnits.end(); ++l_UnitIt)
                                    l_UnitManager.AddUnit2Group(*l_UnitIt, l_pNewGroup);

                                l_UnitManager.AddUnitGroup(l_pNewGroup);
                                l_UnitMover.AddGroup(l_pNewGroup);

                                l_UnitManager.RemoveUnitGroup(*l_UnitGroupIt);

                                l_bDirtyUnitGroups = true;
                                break;
                            }
                            }
                        }
                    }

                    l_bDeclareWar = true;

                    g_ServerDCL.ChangeRegionMilitaryControl(*l_RegionIt, l_iRebelsId);
                    GDZLOG(EDZLogLevel::Info1, l_sRebelsName + L" has occupied " + l_sRegionName);

                    //Check rebel annexation, if military control changed
                    const REAL32 l_fAnnexRandom = l_Rand.RandomReal();
                    GDZLOG(EDZLogLevel::Info1, L"Rebel annex chance " + GString(l_CivilWarConfig.m_fAnnexChance) + L", random number " + GString(l_fAnnexRandom));
                    if(l_fAnnexRandom < l_CivilWarConfig.m_fAnnexChance)
                    {
                        g_ServerDCL.AnnexRegion(l_iRebelsId, *l_RegionIt);
                        GDZLOG(EDZLogLevel::Info1, l_sRebelsName + L" is annexing " + l_sRegionName);
                        l_bAnnexRegions = true;
                    }
                }
            }

            gassert(l_bDeclareWar || !l_bDirtyUnitGroups, NameAndIDForLog() + L" isn't declaring war against rebels in civil war, but unit groups have defected");
            gassert(l_bDeclareWar || !l_bAnnexRegions, NameAndIDForLog() + L" isn't declaring war against rebels in civil war, but at least one region is being annexed");

            if(l_bDeclareWar)
            {
                if(l_bDirtyUnitGroups)
                {
                    g_ServerDAL.DirtyCountryUnitsServer(l_iRebelsId);
                    g_ServerDAL.DirtyCountryUnitsServer(m_iCountryID);
                }

                set<ENTITY_ID> l_vAttackers;
                l_vAttackers.insert(l_iRebelsId);
                g_ServerDCL.DeclareNewWar(l_vAttackers, l_iRebelsId, m_iCountryID);

                if(l_bAnnexRegions)
                {
                    const GString l_sChatMessage = GString(L"Rebels have begun annexing regions from ") + m_sName;
                    g_SP2Server->SendChatMessage(SDK::Event::ESpecialTargets::Server, SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers, l_sChatMessage);
                }
            }
        }
    }

    GDZLOG(EDZLogLevel::Exit, L"");
}

bool GCountryData::OnSave(GIBuffer& io_Buffer)
{
   GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Size() = " + GString(io_Buffer.Size()));

   GDZLOG(EDZLogLevel::Info1, L"m_iCountryID = " + GString(m_iCountryID));
   io_Buffer << m_iCountryID;

   // If country is conquered, only save that it is actualy conquered
   gassert(m_bActivated || !g_SP2Server->CountryNeedsRegions(), L"Country not activated when it should be");
   io_Buffer << (UINT8) (m_bActivated ? 1 : 0);
   if(!m_bActivated)
   {
      GDZLOG(EDZLogLevel::Exit, L"Returning true");
      return true;
   }

   // Save simple variables
   {
      io_Buffer << m_fInterestLevel
                << m_fPersonalIncomeTax
                << m_fMonetarySupplyAmount
                << m_iMiliManpowerAvailable
                << (UINT8) (m_bAcceptForeignOrder ? 1 : 0)
                << m_fNuclearReady
                << (UINT8) (m_bMartialLaw ? 1 : 0)
                << m_iSuffrage
                << (UINT8) (m_bEmigrationClosed ? 1 : 0)
                << (UINT8) (m_bImmigrationClosed ? 1 : 0)
                << m_iGvtType
                << m_fInflationLevel
                << m_fInflationLevelExpected
                << m_fBudgetRevenueTax
                << m_fBudgetRevenueTrade
                << m_fBudgetRevenueIMF
                << m_fBudgetRevenueTourism
                << m_fBudgetExpenseSecurity
                << m_fBudgetExpenseRatioResearch
                << m_fBudgetExpenseRatioTourism
                << m_fBudgetExpenseRatioInfrastructure
                << m_fBudgetExpenseRatioPropaganda
                << m_fBudgetExpenseRatioIMF
                << m_fBudgetExpenseRatioGovernment
                << m_fBudgetExpenseDiplomacy
                << m_fBudgetExpenseImports
                << m_fBudgetExpenseRatioHealthcare
                << m_fBudgetExpenseRatioEducation
                << m_fBudgetExpenseUnitUpkeep
                << m_fBudgetExpenseCorruption
                << m_fBudgetExpenseRatioEnvironment
                << m_fBudgetExpenseRatioTelecom
                << m_fPopPoliticalIdeology
                << m_fTelecomLevelExpected
                << m_fGvtApproval
                << m_fInfrastructureExpected
                << m_fBirthRate
                << m_fDeathRate
                << m_fPop1565Unemployed
                << m_fPopInPoverty
                << m_fGvtStability
                << m_fGvtStabilityExpected
                << m_fHumanDevelopment
                << m_fLifeExpectancy
                << m_fMeanYearsSchooling
                << m_fExpectedYearsSchooling
                << m_fCorruptionLevel
                << m_fCorruptionLevelExpected
                << m_fAMDSLevel
                << m_fEconomicActivity
                << m_fPopEconomicModel
                << m_iPoliticalParty
                << m_fEmigrationLevel
                << m_fEmigrationLevelExpected
                << m_fImmigrationLevel
                << m_fGlobalTaxMod
                << m_fAIHierarchicalDistance
                << m_fAIMaleFemale
                << m_fAIIndividualism
                << m_iCountryGroup
                << m_fBirthRateExpected
                << m_fDeathRateExpected
                << m_fEconomicHealth
                << m_NextElection.Serialize()
                << m_fReloadNuclearSubmarinesClock
                << m_fLastIteration
                << m_fPressure
                << m_iClimateNameID
                << m_iNameID
                << m_sName
                << m_iPoliticRank
                << m_iEconomicRank
                << m_iMilitaryRank
                << m_fPoliticStrength
                << m_fEconomicStrength
                << m_fMilitaryStrength
                << m_iPop15
                << m_iPop1565
                << m_iPop65
                << m_fTelecomLevel
                << m_fGvtApprovalExpected
                << m_fInfrastructure
                << m_fGDPValue
                << m_fDemandTotal
                << m_fGDPValueBase
                << m_fOverallDiplomaticRelations
                << m_fNationalSecurity
                << m_fGDPGrowth
                << m_fBudgetRevenues
                << m_fBudgetExpenses
                << m_iPopulation
                << m_iPopulationPoliticalControl
                << m_fPropagandaLevel
                << m_fAreaTotal
                << m_fAreaWaterTotal
                << m_fAreaLandTotal
                << m_fArableLandLevel
                << m_fForestLandLevel
                << m_fParksLandLevel
                << m_fNotUsedLandLevel
                << m_iHighestPoint
                << m_iLowestPoint
                << m_iCoastlines
                << m_iStandardElevation
                << m_fCountryPoliticalIdeology
                << m_fCountryEconomicModel
                << m_Religions
                << m_Languages
                << m_AccuracyOfInformation;
   }

   // Save internal laws
   {
      for(UINT32 i = 0;i < EInternalLaws::ItemCount;i ++)
      {
         io_Buffer << (UINT8) (m_pInternalLaws[i] ? 1 : 0);
      }
   }

	// Save the languages & religions status
   {
      // Illegal languages
      io_Buffer << (UINT32) m_LanguageIllegalStatus.size();
	   for(set<INT32>::iterator it = m_LanguageIllegalStatus.begin();
		   it != m_LanguageIllegalStatus.end(); it++)
	   {
         io_Buffer << *it;
	   }

      // Official languages
      io_Buffer << (UINT32) m_LanguageOfficialStatus.size();
	   for(set<INT32>::iterator it = m_LanguageOfficialStatus.begin();
		   it != m_LanguageOfficialStatus.end(); it++)
	   {
         io_Buffer << *it;
	   }

      // Illegal religions
      io_Buffer << (UINT32) m_ReligionIllegalStatus.size();
	   for(set<INT32>::iterator it = m_ReligionIllegalStatus.begin();
		   it != m_ReligionIllegalStatus.end(); it++)
	   {
         io_Buffer << *it;
	   }

      // Official religions
      io_Buffer << (UINT32) m_ReligionOfficialStatus.size();
	   for(set<INT32>::iterator it = m_ReligionOfficialStatus.begin();
		   it != m_ReligionOfficialStatus.end(); it++)
	   {
         io_Buffer << *it;
	   }
   }

   // Save the political parties
   {
      io_Buffer << (UINT32) m_vPoliticalParty.size();

	   for(UINT32 i=0; i<m_vPoliticalParty.size(); i++)
	   {
         m_vPoliticalParty[i].Serialize(io_Buffer);
      }
	}

   // Save the resources
   {
      io_Buffer << m_fResourceProductionModifier << m_fResourceDemandModifier;

	   for(UINT32 i = 0;i < EResources::ItemCount;i ++)
	   {
         io_Buffer << m_pResourceProduction[i]
                   << m_pResourceDemand[i]
                   << m_pResourceMarketShare[i]
                   << m_pResourceGDP[i]
                   << m_pResourceTaxes[i]
                   << m_pResourceImport[i]
                   << m_pResourceImportDesired[i]
                   << m_pResourceExport[i]
                   << m_pResourceExportDesired[i]
                   << (UINT8) ( (m_pResourceGvtControl[i] ? c_iResourceGvtControled : 0) | 
                                (m_pResourceLegal[i] ? c_iResourceLegal : 0) | 
                                (m_pResourceMeetDomesticConsumption[i] ? c_iResourceMeetsComsuption : 0) );
      }
   }

   // Save the nuclear missile list
   {
      // Ground based missiles
      io_Buffer << (UINT32) m_vGroundBasedNuclearMissiles.size();
      for(UINT32 i = 0;i < m_vGroundBasedNuclearMissiles.size();i ++)
      {
         m_vGroundBasedNuclearMissiles[i].Serialize(io_Buffer);
      }

      // Submarine missiles
      io_Buffer << (UINT32) m_vSubmarineBasedNuclearMissiles.size();
      for(UINT32 i = 0;i < m_vSubmarineBasedNuclearMissiles.size();i ++)
      {
         m_vSubmarineBasedNuclearMissiles[i].Serialize(io_Buffer);
      }
   }

	// Save research
   ((SP2::GResearchInformation*)m_pResearchInformation)->OnSave(io_Buffer);
   io_Buffer << m_AccuracyOfInformation;

	// Save the covert action cells
   {
      io_Buffer << (UINT32) m_vCovertActionCells.size();
      for(UINT32 i = 0;i < m_vCovertActionCells.size();i ++)
      {
         m_vCovertActionCells[i].Serialize(io_Buffer);
      }
   }

   // Adivsor
   {
      io_Buffer << m_eAdvisorMilitaryStatus
                << m_eAdvisorPoliticStatus
                << m_eAdvisorEconomicStatus;

      if(m_pAdvisor)
      {
         io_Buffer << (UINT8) 1
                   << m_pAdvisor->MilitaryStatus()
                   << m_pAdvisor->EconomicStatus()
                   << m_pAdvisor->PoliticStatus();
      }
      else
      {
         io_Buffer << (UINT8) 0;
      }
   }

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;
}

bool GCountryData::OnLoad(GOBuffer& io_Buffer)
{
   GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()));

   io_Buffer >> m_iCountryID;
   GDZLOG(EDZLogLevel::Info1, L"m_iCountryID = " + GString(m_iCountryID));

   UINT8 l_iBooleanVal;

   // If country is conquered, only load this info
   io_Buffer >> l_iBooleanVal;
   m_bActivated = (l_iBooleanVal == 1);
   GDZLOG(EDZLogLevel::Info1, L"m_bActivated from save file = " + GString(m_bActivated));
   if(!m_bActivated)
   {
      if(!g_SP2Server->CountryNeedsRegions() && g_SP2Server->ActivateAllDatabaseCountries())
      {
          if(FetchCountryData(m_iCountryID))
          {
              gassert(m_bActivated, NameAndIDForLog() + L" wasn't activated properly");
              GDZLOG(EDZLogLevel::Warning, L"Activated " + NameAndIDForLog() + L" based on config");
          }
          else
          {
              GDZLOG(EDZLogLevel::Error, L"Couldn't fetch country data for country ID " + GString(m_iCountryID));
              m_bActivated = false;
          }
      }
      GDZLOG(EDZLogLevel::Exit, L"Returning true");
      return true;
   }

   // Load simple variables
   {
      INT64 l_iNextElection;
      io_Buffer >> m_fInterestLevel
                >> m_fPersonalIncomeTax
                >> m_fMonetarySupplyAmount
                >> m_iMiliManpowerAvailable
                >> l_iBooleanVal;
      m_bAcceptForeignOrder = (l_iBooleanVal == 1);

      io_Buffer >> m_fNuclearReady
                >> l_iBooleanVal;
      m_bMartialLaw = (l_iBooleanVal == 1);

      io_Buffer >> m_iSuffrage
                >> l_iBooleanVal;
      m_bEmigrationClosed = (l_iBooleanVal == 1);

      io_Buffer >> l_iBooleanVal;
      m_bImmigrationClosed = (l_iBooleanVal == 1);

      io_Buffer >> m_iGvtType
                >> m_fInflationLevel
                >> m_fInflationLevelExpected
                >> m_fBudgetRevenueTax
                >> m_fBudgetRevenueTrade
                >> m_fBudgetRevenueIMF
                >> m_fBudgetRevenueTourism
                >> m_fBudgetExpenseSecurity
                >> m_fBudgetExpenseRatioResearch
                >> m_fBudgetExpenseRatioTourism
                >> m_fBudgetExpenseRatioInfrastructure
                >> m_fBudgetExpenseRatioPropaganda
                >> m_fBudgetExpenseRatioIMF
                >> m_fBudgetExpenseRatioGovernment
                >> m_fBudgetExpenseDiplomacy
                >> m_fBudgetExpenseImports
                >> m_fBudgetExpenseRatioHealthcare
                >> m_fBudgetExpenseRatioEducation
                >> m_fBudgetExpenseUnitUpkeep
                >> m_fBudgetExpenseCorruption
                >> m_fBudgetExpenseRatioEnvironment
                >> m_fBudgetExpenseRatioTelecom
                >> m_fPopPoliticalIdeology
                >> m_fTelecomLevelExpected
                >> m_fGvtApproval
                >> m_fInfrastructureExpected
                >> m_fBirthRate
                >> m_fDeathRate
                >> m_fPop1565Unemployed
                >> m_fPopInPoverty
                >> m_fGvtStability
                >> m_fGvtStabilityExpected
                >> m_fHumanDevelopment
                >> m_fLifeExpectancy
                >> m_fMeanYearsSchooling
                >> m_fExpectedYearsSchooling
                >> m_fCorruptionLevel
                >> m_fCorruptionLevelExpected
                >> m_fAMDSLevel
                >> m_fEconomicActivity
                >> m_fPopEconomicModel
                >> m_iPoliticalParty
                >> m_fEmigrationLevel
                >> m_fEmigrationLevelExpected
                >> m_fImmigrationLevel
                >> m_fGlobalTaxMod
                >> m_fAIHierarchicalDistance
                >> m_fAIMaleFemale
                >> m_fAIIndividualism
                >> m_iCountryGroup
                >> m_fBirthRateExpected
                >> m_fDeathRateExpected
                >> m_fEconomicHealth
                >> l_iNextElection
                >> m_fReloadNuclearSubmarinesClock
                >> m_fLastIteration
                >> m_fPressure
                >> m_iClimateNameID
                >> m_iNameID
                >> m_sName
                >> m_iPoliticRank
                >> m_iEconomicRank
                >> m_iMilitaryRank
                >> m_fPoliticStrength
                >> m_fEconomicStrength
                >> m_fMilitaryStrength
                >> m_iPop15
                >> m_iPop1565
                >> m_iPop65
                >> m_fTelecomLevel
                >> m_fGvtApprovalExpected
                >> m_fInfrastructure
                >> m_fGDPValue
                >> m_fDemandTotal
                >> m_fGDPValueBase
                >> m_fOverallDiplomaticRelations
                >> m_fNationalSecurity
                >> m_fGDPGrowth
                >> m_fBudgetRevenues
                >> m_fBudgetExpenses
                >> m_iPopulation
                >> m_iPopulationPoliticalControl
                >> m_fPropagandaLevel
                >> m_fAreaTotal
                >> m_fAreaWaterTotal
                >> m_fAreaLandTotal
                >> m_fArableLandLevel
                >> m_fForestLandLevel
                >> m_fParksLandLevel
                >> m_fNotUsedLandLevel
                >> m_iHighestPoint
                >> m_iLowestPoint
                >> m_iCoastlines
                >> m_iStandardElevation
                >> m_fCountryPoliticalIdeology
                >> m_fCountryEconomicModel
                >> m_Religions
                >> m_Languages
                >> m_AccuracyOfInformation;

      m_NextElection = GDateTime(l_iNextElection);
   }

   // Load internal laws
   {
      for(UINT32 i = 0;i < EInternalLaws::ItemCount;i ++)
      {
         io_Buffer >> l_iBooleanVal;
         m_pInternalLaws[i] = (l_iBooleanVal == 1);
      }
   }

	// Load the languages & religions status
   {
      UINT32 l_iCount;
      INT32 l_iValue;

      // Illegal languages
      io_Buffer >> l_iCount;
	   for(UINT32 i = 0;i < l_iCount;i ++)
	   {
         io_Buffer >> l_iValue;
         m_LanguageIllegalStatus.insert(l_iValue);
	   }

      // Official languages
      io_Buffer >> l_iCount;
	   for(UINT32 i = 0;i < l_iCount;i ++)
	   {
         io_Buffer >> l_iValue;
         m_LanguageOfficialStatus.insert(l_iValue);
	   }

      // Illegal religions
      io_Buffer >> l_iCount;
	   for(UINT32 i = 0;i < l_iCount;i ++)
	   {
         io_Buffer >> l_iValue;
         m_ReligionIllegalStatus.insert(l_iValue);
	   }

      // Official religions
      io_Buffer >> l_iCount;
	   for(UINT32 i = 0;i < l_iCount;i ++)
	   {
         io_Buffer >> l_iValue;
         m_ReligionOfficialStatus.insert(l_iValue);
	   }
   }

   // Load the political parties
   {
      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      m_vPoliticalParty.resize(l_iCount);

	   for(UINT32 i = 0;i < l_iCount;i ++)
	   {
         m_vPoliticalParty[i].Unserialize(io_Buffer);
      }
	}

   // Load the resources
   {
      io_Buffer >> m_fResourceProductionModifier >> m_fResourceDemandModifier;

	   for(UINT32 i = 0;i < EResources::ItemCount;i ++)
	   {
         UINT8 l_iFlags;
         io_Buffer >> m_pResourceProduction[i]
                   >> m_pResourceDemand[i]
                   >> m_pResourceMarketShare[i]
                   >> m_pResourceGDP[i]
                   >> m_pResourceTaxes[i]
                   >> m_pResourceImport[i]
                   >> m_pResourceImportDesired[i]
                   >> m_pResourceExport[i]
                   >> m_pResourceExportDesired[i]
                   >> l_iFlags;

          m_pResourceGvtControl[i] = ( (c_iResourceGvtControled & l_iFlags) != 0);
          m_pResourceLegal[i] = ( (c_iResourceLegal & l_iFlags) != 0);
          m_pResourceMeetDomesticConsumption[i] = ( (c_iResourceMeetsComsuption & l_iFlags) != 0);
      }
   }

   // Load the nuclear missile list
   {
      UINT32 l_iCount;

      // Ground based missiles
      io_Buffer >> l_iCount;
      m_vGroundBasedNuclearMissiles.resize(l_iCount);
      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         m_vGroundBasedNuclearMissiles[i].Unserialize(io_Buffer);
      }

      // Submarine missiles
      io_Buffer >> l_iCount;
      m_vSubmarineBasedNuclearMissiles.resize(l_iCount);
      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         m_vSubmarineBasedNuclearMissiles[i].Unserialize(io_Buffer);
      }
   }

	// Load research
   IF_RETURN( !((SP2::GResearchInformation*)m_pResearchInformation)->OnLoad(io_Buffer), false);
   io_Buffer >> m_AccuracyOfInformation;
	
	// Load the covert action cells
   {
      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      m_vCovertActionCells.resize(l_iCount);
      for(UINT32 i = 0;i < m_vCovertActionCells.size();i ++)
      {
         m_vCovertActionCells[i].Unserialize(io_Buffer);
         m_vCovertActionCells[i].Dirty(true);

         GCovertActionCell::m_iNextId = max(GCovertActionCell::m_iNextId, m_vCovertActionCells[i].ID() + 1);
      }
   }

   // Adivsor
   {
      io_Buffer >> m_eAdvisorMilitaryStatus
                >> m_eAdvisorPoliticStatus
                >> m_eAdvisorEconomicStatus;

      io_Buffer >> l_iBooleanVal;
      if(l_iBooleanVal)
      {
         EAdvisorStatus::Enum l_eMilStatus, l_eEconoStatus, l_ePolStatus;
         io_Buffer >> l_eMilStatus
                   >> l_eEconoStatus
                   >> l_ePolStatus;

         AddAdvisor(l_eEconoStatus, l_ePolStatus, l_eMilStatus);
      }
      else
      {
         RemoveAdvisor();
      }
   }

   // Clear master and client data; GDataAccessLayerServer::OnLoad() will take care of it
   m_Master = pair<ENTITY_ID, UINT32>(0, 0);
   m_mClients.clear();

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;
}

bool GCountryData::OnNew(GDatabase* in_pDatabase)
{
   m_fReloadNuclearSubmarinesClock  = -1.f;

   GString l_sQuery = L"SELECT unit_type, next_id, name FROM design_format WHERE country_id = " + GString(m_iCountryID);
   DB::GTable l_Table;

   DB::EError l_Error = in_pDatabase->ExecuteQuery(l_sQuery, l_Table);
   IF_RETURN(l_Error != DB_NO_ERROR, false);

   m_vDesignFormat.resize(EUnitType::TotalCount);

   for(UINT32 i = 0 ; i < l_Table.RowCount() ; i++)
   {
      GDesignFormat                             l_Format;
      UINT32                                    l_iType;
      UINT32                                    l_iNextID;
		GString												l_sName;
		INT32													l_iNameID;						

      l_Table.Row(i)->Cell(0)->GetData(l_iType);
		l_Table.Row(i)->Cell(1)->GetData(l_iNextID);
      l_Table.Row(i)->Cell(2)->GetData(l_iNameID);
      l_sName = g_ServerDAL.GetString(l_iNameID);
      l_Format.m_eType = (EUnitType::Enum)l_iType;
      l_Format.m_sFormat = l_sName;
      l_Format.m_iNextID = l_iNextID;
      m_vDesignFormat[l_iType].push_back(l_Format);         
   }

   ( (SP2::GResearchInformation*) m_pResearchInformation)->OnNew(in_pDatabase);

   return true;
}

void GCountryData::OnClean()
{
	SAFE_DELETE(m_pAdvisor);

   m_vPoliticalParty.clear();
   m_vGroundBasedNuclearMissiles.clear();
   m_vSubmarineBasedNuclearMissiles.clear();
   m_vCovertActionCells.clear();

	m_ReligionOfficialStatus.clear();
	m_ReligionIllegalStatus.clear();
	m_LanguageOfficialStatus.clear();
	m_LanguageIllegalStatus.clear();
}

void GCountryData::AddClient(ENTITY_ID in_iCountryID, UINT32 in_iTreatyID)
{
    gassert(m_Master.first == 0,
            NameAndIDForLog() + L" is already a client and " +
            L" can't have a client of its own");
    m_mClients.insert(m_mClients.cend(),
                      pair<ENTITY_ID, UINT32>(in_iCountryID, in_iTreatyID));
}

void GCountryData::RemoveClient(ENTITY_ID in_iCountryID)
{
    gassert(m_mClients.count(in_iCountryID) == 1,
            NameAndIDForLog() + L" doesn't have " +
            g_ServerDAL.CountryData(in_iCountryID)->NameAndIDForLog() +
            L" as a client");

    //For debugging
    const UINT32 l_iTreatyID = m_mClients[in_iCountryID];

    m_mClients.erase(in_iCountryID);

    GCountryData* const l_pClient = g_ServerDAL.CountryData(in_iCountryID);
    const UINT32 l_iClientTreatyID = l_pClient->Master().second;
    gassert(l_iClientTreatyID == l_iTreatyID,
            NameAndIDForLog() + L"-" + l_pClient->NameAndIDForLog() + L" " +
            L"client treaty inconsistency; " +
            GString(l_iClientTreatyID) + L" client vs. " +
            GString(l_iTreatyID) + L" master");
    l_pClient->Master(0, 0);

    GDZLOG(EDZLogLevel::Info2, l_pClient->NameAndIDForLog() + L" is no longer a client of " + NameAndIDForLog());

    //Send country list to update the former client's name
    g_ServerDCL.SendCountryList();
}

REAL32 GCountryData::PercentageOfPopulationOccupiedByCountry(ENTITY_ID in_iCountryID) const
{
    const auto& l_vRegions = g_ServerDAL.CountryPoliticalControl(m_iCountryID);
    REAL32 l_fPercentageOccupied = 0.f;
    for(auto it = l_vRegions.cbegin(); it != l_vRegions.cend(); ++it)
    {
        l_fPercentageOccupied += g_ServerDAL.RegionControlArray()[*it].m_iMilitary == in_iCountryID ?
                                 g_ServerDAL.GetGRegion(*it)->PercentageValue() :
                                 0.f;
    }
    return l_fPercentageOccupied;
}

REAL32 GCountryData::PopulationGrowth() const
{
    return m_fPopulationGrowth;
}

void GCountryData::PopulationGrowth(REAL32 in_fPopulationGrowth)
{
    m_fPopulationGrowth = in_fPopulationGrowth;
}

REAL32 GCountryData::Population1565Growth() const
{
    return m_fPopulation1565Growth;
}

void GCountryData::Population1565Growth(REAL32 in_fPopulation1565Growth)
{
    m_fPopulation1565Growth = in_fPopulation1565Growth;
}

void GCountryData::SetResourceDemand(const EResources::Enum in_eResource, const REAL64 in_fDemand)
{
    m_pResourceDemand[in_eResource] = in_fDemand / m_fResourceDemandModifier;
}

REAL64 GCountryData::ExpectedResourceDemand(const EResources::Enum in_eResource) const
{
    GDZLOG(EDZLogLevel::Entry, L"in_eResource = " + GString(in_eResource) + L" (" + g_ServerDAL.GetString(g_ServerDAL.StringIdResource(in_eResource)) + L")");

    const REAL64 l_fDemographicFactor = ((GWorldBehavior::FindDemographicFinancingFactor(m_iCountryID) * 3.0) + m_fBudgetExpenseRatioInfrastructure) / 4.0;
    GDZLOG(EDZLogLevel::Info2, L"m_fPersonalIncomeTax = " + GString(m_fPersonalIncomeTax) + L", l_fDemographicFactor = " + GString(l_fDemographicFactor));

    //Modify demand, per resource, based on PIT
    //This is PIT needed for no effect on demand; if PIT exceeds this, demand will decrease
    REAL64 l_fPitForNormalDemand = 1.0;

    //Trend that's solely based on PIT, as if 0 budget
    REAL64 l_fBaseTrend = 0.0;

    //Trend that factors in a maxed budget, should always equal or exceed l_fBaseTrend
    REAL64 l_fMaxBudgetTrend = l_fBaseTrend;

    //(PIT, demand)
    if(in_eResource <= EResources::Drugs)
    {
        if(in_eResource <= EResources::Dairy)
        {
            //(0.8, 1) linear decrease to (1, 0.5)
            l_fPitForNormalDemand = 0.8;
            l_fBaseTrend = (-2.5 * m_fPersonalIncomeTax) + 3.0;
            l_fMaxBudgetTrend = l_fBaseTrend;
        }
        else
        {
            //(0, 1) parabolic decrease to (1, 0)
            l_fPitForNormalDemand = 0.0;
            l_fBaseTrend = -pow(m_fPersonalIncomeTax, 2.0) + 1.0;
            l_fMaxBudgetTrend = l_fBaseTrend;
        }
    }
    else if(in_eResource >= EResources::Electricity && in_eResource <= EResources::Fossile_Fuels)
    {
        //Base: (2/3, 1) linear decrease to (1, 0)
        //Max budget: (2/3, 1) linear decrease to (1, 0.5)

        l_fPitForNormalDemand = 2.0 / 3.0;
        l_fBaseTrend = (-3.0 * m_fPersonalIncomeTax) + 3.0;
        l_fMaxBudgetTrend = (-1.5 * m_fPersonalIncomeTax) + 2.0;
    }
    else if(in_eResource >= EResources::Wood_Paper && in_eResource <= EResources::Luxury_Commodities)
    {
        //Base: (0.5, 1) linear decrease to (1, 0)
        //Max budget: (0.5, 1) linear decrease to (1, 0.5)

        l_fPitForNormalDemand = 0.5;
        l_fBaseTrend = (-2.0 * m_fPersonalIncomeTax) + 2.0;
        l_fMaxBudgetTrend = -m_fPersonalIncomeTax + 1.5;
    }
    else
    {
        //Base: (0.4, 1) linear decrease to (1, 0)
        //Max budget: (0.4, 1) linear decrease to (1, 0.25)

        l_fPitForNormalDemand = 0.4;
        l_fBaseTrend = ((-5.0 / 3.0) * m_fPersonalIncomeTax) + (5.0 / 3.0);
        l_fMaxBudgetTrend = (-1.25 * m_fPersonalIncomeTax) + 1.5;
    }

    const REAL64 l_fDifference = l_fMaxBudgetTrend - l_fBaseTrend;
    const REAL64 l_fDemandMultiplier = (m_fPersonalIncomeTax < l_fPitForNormalDemand) ? 1.0 : (l_fBaseTrend + (l_fDifference * l_fDemographicFactor));
    gassert(l_fDemandMultiplier >= 0.0 && l_fDemandMultiplier <= 1.0, L"l_fDemandMultiplier out of range");

    const REAL64 l_fDemand = ResourceDemand(in_eResource);
    GDZLOG(EDZLogLevel::Info2, L"l_fDemand = " + GString::FormatNumber(l_fDemand, L",", L".", L"$", L"", 3, 2, true) + L", l_fDemandMultiplier = " + GString(l_fDemandMultiplier));

    const REAL64 l_fExpectedDemand = l_fDemand * l_fDemandMultiplier;
    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString::FormatNumber(l_fExpectedDemand, L",", L".", L"$", L"", 3, 2, true));
    return l_fExpectedDemand;
}
