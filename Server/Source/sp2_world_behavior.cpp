/**************************************************************
*
* sp2_world_behavior.cpp
*
* Description
* ===========
*  This is the manager of effects of actions on data, and the effects of data on
*  other data.
*
* Owner
* =====
*  Jean-René Couture, Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

//Main categories
const REAL32 FoodAgricultureImp = 0.45f;
const REAL32 EnergyImp = 0.2f;
const REAL32 RawMaterialsImp = 0.1f;
const REAL32 IndustrialMaterialsImp = 0.1f;
const REAL32 FinishedGoodsImp = 0.1f;
const REAL32 ServicesImp = 0.05f;

//food & agriculture
const REAL32 ResImpCereals = 0.25f;
const REAL32 ResImpVegetablesFruits = 0.2f;
const REAL32 ResImpMeat = 0.2f;
const REAL32 ResImpDairy = 0.15f;
const REAL32 ResImpTobacco = 0.1f;
const REAL32 ResImpDrugs = 0.1f;
//energy
const REAL32 ResImpElectricity = 0.5f;
const REAL32 ResImpFossileFuels = 0.5f;
//raw materials
const REAL32 ResImpWoodPaper = 0.25f;
const REAL32 ResImpMinerals = 0.25f;
const REAL32 ResImpIronSteel = 0.4f;
const REAL32 ResImpPreciousStones = 0.1f;
//industrial materials
const REAL32 ResImpFabrics = 0.20f;
const REAL32 ResImpPlastics = 0.25f;
const REAL32 ResImpChemicals = 0.4f;
const REAL32 ResImpPharmaceuticals = 0.15f;
//finished goods
const REAL32 ResImpAppliances = 0.1f;
const REAL32 ResImpVehicles = 0.2f;
const REAL32 ResImpMachinery = 0.2f;
const REAL32 ResImpComm = 0.4f;
const REAL32 ResImpLuxuryComm = 0.1f;
//services
const REAL32 ResImpConstruction = 0.25f;
const REAL32 ResImpEngineering = 0.1f;
const REAL32 ResImpHealth = 0.4f;
const REAL32 ResImpRetail = 0.1f;
const REAL32 ResImpLegal = 0.1f;
const REAL32 ResImpMarket = 0.05f;


//////////////////////////////////////////////////////////////////////////
// WORLD BEHAVIOR CODE
//////////////////////////////////////////////////////////////////////////
GWorldBehavior::GWorldBehavior()
{
	Reset();

#ifdef GOLEM_DEBUG
	m_pLogger = new GLogger("World_Behavior.log");
	m_pLoggerResources = new GLogger("Resources.log");
	GString l_sTitle;
	l_sTitle += L"country_id,last_iteration,amds_level,budget_balance,budget_expense_debt," \
		L"budget_expense_education, budget_expense_environment, budget_expense_government," \
		L"budget_expense_healthcare,budget_expense_imf,budget_expense_imports,budget_expense_infrastructure," \
		L"budget_expense_diplomacy," \
		L"budget_expense_research,budget_expense_security,budget_expense_telecom,budget_expense_tourism," \
		L"budget_expense_unit_upkeep,budget_expense_propaganda,budget_expense_corruption," \
		L"budget_revenue_exports,budget_revenue_imf,budget_revenue_tax," \
		L"budget_revenue_tourism,corruption_level,economic_rank,emigration_level,gvt_type," \
		L"gdp_value,gdp_growth,growth_rate,birth_rate,death_rate,gvt_approval,gvt_stability,human_development," \
		L"immigration_level,inflation_level,inflation_growth,infrastructure,interest_level,mili_manpower_available,military_rank," \
		L"personal_income_tax,political_rank,population," \
		L"pop_15,pop_1565,pop_65,pop_1565_unemployed,pop_in_poverty,pop_political_ideology,pop_economic_model," \
		L"economic_health,telecom_level,demand_cereals,production_cereals,demand_vegetables,production_vegetables,demand_meat,production_meat,demand_dairy,production_dairy" \
		L",demand_tobacco,production_tobacco,demand_drugs,production_drugs,demand_electricity,production_electricity,demand_fossile,production_fossile,demand_wood,production_wood,demand_minerals,production_minerals,demand_iron,production_iron" \
		L",demand_precious_stones,production_precious_stones,demand_fabrics,production_fabrics,demand_plastics,production_plastics,demand_chemicals,production_chemicals,demand_pharma,production_pharma" \
		L",demand_appliances,production_appliances,demand_vehicules,production_vehicules,demand_machinery,production_machinery,demand_commodities,production_commodities,demand_lux_comm,production_lux_comm" \
		L",demand_construction,production_construction,demand_engineering,production_engineering,demand_health,production_health,demand_retail,production_retail,demand_legal,production_legal,demand_market,production_market";
	m_pLogger->Log(MSGTYPE_CSV,l_sTitle);
#endif	
}

GWorldBehavior::~GWorldBehavior()
{
#ifdef GOLEM_DEBUG
	SAFE_DELETE(m_pLogger);
#endif
}

bool GWorldBehavior::Reset()
{
	m_iInternalCounter = 1;
	m_fFrequency = 0.0f;
	m_fNextNewsIteration = 0.f;
	m_fNextRelationsTreatiesIteration = 0.f;
	m_fLastStabilityApprovalIteration = 0.f;
	m_fLastNukeReadyIteration = 0.f;
	m_fNextIteration = 0.f;
	m_fLastBudgetIteration = 0.f;
	m_QuickRandom.Seed(GetTickCount() );

    m_iHDILogStartingCountryID = 0;

	return true;
}

//! actively inits the class
//! in_sPath/in_sFilename -> xml file to load
bool GWorldBehavior::LoadConfigFile(const GString &in_sFilename, const GString &in_sPath)
{
	m_bInit = true;

   g_ServerDAL.DataManager()->RegisterNode(this);

	GFile          l_XMLFile;
	GString        l_sTempName;
	GXMLParser		l_XmlParser;
	GTree<GXMLNode>* l_XMLData;

	GTreeNode<GXMLNode>* objectNode  = NULL;

	GString				  elementName;
	GString				  elementValue;
	GString				  SubelementName;
	GString				  SubelementValue;

	if(in_sFilename == L"")
	{
		return false;
	}

	//Request the file to the file manager
	if(!((SP2::GServer*)g_Joshua.Server())->DAL().File(in_sPath+in_sFilename,l_XMLFile))
	{
		g_Joshua.Log(SP2::ERROR_CANT_FIND_FILE + in_sPath + in_sFilename,MSGTYPE_FATAL_ERROR);
		return false;
	} 

	//Extract the file to a temp file on the disk so it can be parsed (xml)
	if(!l_XMLFile.Extract(l_sTempName))
	{
		g_Joshua.Log(SP2::ERROR_EXTRACT_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
		return false;
	}

	l_XMLData = l_XmlParser.Parse(l_sTempName);
	l_XMLFile.Unextract(); //free the temp file

	if(!l_XMLData)
	{
		g_Joshua.Log(SP2::ERROR_PARSING_XML_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
		return false;
	}

	// parse the file
	for(UINT32 i = 0 ; (INT32)i<l_XMLData->Root()->NbChilds() ; i++)
	{
		//Look for OBJECT nodes
		objectNode = l_XMLData->Root()->Child(i);

		elementName  = objectNode->Data().m_sName;

		// data modifiers
		if(elementName == FREQUENCY)
		{
			SubelementValue = objectNode->Data().m_value;
			m_fFrequency = SubelementValue.ToREAL32();
		}
	}

	return true;
}

bool GWorldBehavior::Iterate_Population(GRegion* in_pRegion)
{	
    //const UINT32 l_iRegionId = in_pRegion->Id();
    /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
                m_CountryData->NameAndIDForLog() + L": " +
                L"Population15() "   + GString(in_pRegion->Population15()) + L", " +
                L"Population1565() " + GString(in_pRegion->Population1565()) + L", " +
                L"Population65() "   + GString(in_pRegion->Population65()) + L", " +
                L"Population() "     + GString(in_pRegion->Population()));*/

	GReligionList l_Religions;
	GLanguageList l_Languages;
	INT64 l_iPopulationReligion = 0;
	INT64 l_iPopulationLanguage = 0;
	INT64 l_iTempPopulationAll = 0;

	const REAL32 PourcConvertsConstReligion = 0.05f;
	const REAL32 PourcConvertsConstLanguage = 0.1f;

	INT64 l_iInitialPopulation = in_pRegion->Population15() + in_pRegion->Population1565() + in_pRegion->Population65();
	INT64 l_iTemp15 = 0;
	INT64 l_iTemp1565 = 0;
	INT64 l_iTemp65 = 0;
	INT64 l_iNew1565 = 0;
	INT64	l_iNew65 = 0;
	INT64 l_iDeaths15 = 0;
	INT64 l_iDeaths1565 = 0;
	INT64 l_iDeaths65 = 0;
	INT64 l_iTotalDeaths = 0;
	INT64 l_iBabies = 0;
	INT64 l_iDifference = 0;

	INT64 l_iTemp = 0;
	INT64 l_iTempLeftToArrive = 0;
	INT64 l_iTempLeftToLeave = 0;	
	INT64 l_iTempCalc = 0;
	INT64 l_iVarManPower = 0;

	l_iBabies = ReturnInteger64(m_fFrequency * in_pRegion->Population() * m_CountryData->BirthRate());
	l_iTotalDeaths = ReturnInteger64(m_fFrequency * in_pRegion->Population() * m_CountryData->DeathRate());

	l_iDeaths65 = ReturnInteger64(m_fFrequency * in_pRegion->Population65() * m_CountryData->DeathRate() * 5.f);
	if(l_iDeaths65 > in_pRegion->Population65())
		l_iDeaths65 = in_pRegion->Population65();
	if(l_iDeaths65 < l_iTotalDeaths)
	{
		l_iDeaths1565 = ReturnInteger64(m_fFrequency * in_pRegion->Population1565() * m_CountryData->DeathRate() * 1.5f);
		if(l_iDeaths1565 > in_pRegion->Population1565())
			l_iDeaths1565 = in_pRegion->Population1565();
		if(l_iDeaths1565 < (l_iTotalDeaths - l_iDeaths65))
			l_iDeaths15 = l_iTotalDeaths - l_iDeaths65 - l_iDeaths1565;
		else
			l_iDeaths1565 = l_iTotalDeaths - l_iDeaths65;
	}
	else
		l_iDeaths65 = l_iTotalDeaths;

	// Remove 1/15 of the population under 15, and add them to the population of 15-65
	l_iNew1565 = ReturnInteger64((1.0f/15.0f) * m_fFrequency * (REAL32)in_pRegion->Population15());
	l_iTemp15 = in_pRegion->Population15() - l_iNew1565;
	//add new born babies
	l_iTemp15 += l_iBabies - l_iDeaths15;
	if(l_iTemp15 < 0)
		l_iTemp15 = 0;
	in_pRegion->Population15(l_iTemp15);   


	// Remove 1/50 of the population between 15-65, and add them to the population of over 65
	l_iNew65 = ReturnInteger64((1.0f/50.f) * m_fFrequency * in_pRegion->Population1565());
	l_iTemp1565 = in_pRegion->Population1565() - l_iNew65 - l_iDeaths1565;
	//add teenagers coming from the under 15 category
	l_iTemp1565 += l_iNew1565;
	if(l_iTemp1565 < 0)
	{
		l_iTemp1565 = 0;
	}
	in_pRegion->Population1565(l_iTemp1565);

	l_iVarManPower = l_iNew1565 - l_iDeaths1565 - l_iNew65;
	l_iVarManPower = (INT64)((REAL32)l_iVarManPower * SP2::c_fPourcPopulationAbleToEnroll);
	m_CountryData->MiliManpowerAvailable(m_CountryData->MiliManpowerAvailable() + l_iVarManPower);

	l_iTemp65 = in_pRegion->Population65() - l_iDeaths65;
	//add adults coming from the 15-65 category
	l_iTemp65 += l_iNew65;
	if(l_iTemp65 < 0)
	{
		l_iTemp65 = 0;		
	}
	in_pRegion->Population65(l_iTemp65);


	l_iDifference = in_pRegion->Population15() + in_pRegion->Population1565() + in_pRegion->Population65() - l_iInitialPopulation;
    /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
                m_CountryData->NameAndIDForLog() + L": " +
                L"Birth and death difference " + GString(l_iDifference));*/

	//Number of people that leave and enter the country
	INT64 l_iLeavingPopulation15 = ReturnInteger64((REAL32)in_pRegion->Population15() * m_CountryData->EmigrationLevel() * m_fFrequency);
	INT64 l_iLeavingPopulation1565 = ReturnInteger64((REAL32)in_pRegion->Population1565() * m_CountryData->EmigrationLevel() * m_fFrequency);
	INT64 l_iLeavingPopulation65 = ReturnInteger64((REAL32)in_pRegion->Population65() * m_CountryData->EmigrationLevel() * m_fFrequency);

	INT64 l_iArrivingPopulation15 = ReturnInteger64((REAL32)in_pRegion->Population15() * m_CountryData->ImmigrationLevel() * m_fFrequency);
	INT64 l_iArrivingPopulation1565 = ReturnInteger64((REAL32)in_pRegion->Population1565() * m_CountryData->ImmigrationLevel() * m_fFrequency);
	INT64 l_iArrivingPopulation65 = ReturnInteger64((REAL32)in_pRegion->Population65() * m_CountryData->ImmigrationLevel() * m_fFrequency);

	if(in_pRegion->Population15() == 0)
		l_iArrivingPopulation15 = ReturnInteger64(m_CountryData->ImmigrationLevel() * m_fFrequency);
	if(in_pRegion->Population1565() == 0)
		l_iArrivingPopulation1565 = ReturnInteger64(m_CountryData->ImmigrationLevel() * m_fFrequency);
	if(in_pRegion->Population65() == 0)
		l_iArrivingPopulation65 = ReturnInteger64(m_CountryData->ImmigrationLevel() * m_fFrequency);


	INT64 l_iLeavingPopulation = 0; 
	INT64 l_iArrivingPopulation = 0;

	if(!m_CountryData->EmigrationClosed())
		l_iLeavingPopulation = l_iLeavingPopulation15 + l_iLeavingPopulation1565 + l_iLeavingPopulation65;
	else
	{
		l_iLeavingPopulation15 = 0;
		l_iLeavingPopulation1565 = 0;
		l_iLeavingPopulation65 = 0;
	}
	if(!m_CountryData->ImmigrationClosed())
		l_iArrivingPopulation = l_iArrivingPopulation15 + l_iArrivingPopulation1565 + l_iArrivingPopulation65;
	else
	{
		l_iArrivingPopulation15 = 0;
		l_iArrivingPopulation1565 = 0;
		l_iArrivingPopulation65 = 0;
	}

    /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
                m_CountryData->NameAndIDForLog() + L": " +
                L"Leaving populations: " + GString(l_iLeavingPopulation15) + L" " + GString(l_iLeavingPopulation1565) + L" " + GString(l_iLeavingPopulation65) + L", " +
                L"Arriving populations: " + GString(l_iArrivingPopulation15) + L" " + GString(l_iArrivingPopulation1565) + L" " + GString(l_iArrivingPopulation65));*/

	in_pRegion->Population15(in_pRegion->Population15() + l_iArrivingPopulation15 - l_iLeavingPopulation15);
	in_pRegion->Population1565(in_pRegion->Population1565() + l_iArrivingPopulation1565 - l_iLeavingPopulation1565);
	in_pRegion->Population65(in_pRegion->Population65() + l_iArrivingPopulation65 - l_iLeavingPopulation65);	

	/*
	now, we must adjust the religions/languages population based on babies/deaths and emigration/immigration
	*/
	l_iTemp = l_iDifference;	
	l_iTempLeftToArrive = l_iArrivingPopulation;
	l_iTempLeftToLeave = l_iLeavingPopulation;
	in_pRegion->GetReligions(l_Religions);	

    INT64 l_iInitialPopulationOfReligions = 0;
    for(GReligionList::iterator l_Itr = l_Religions.begin();
		l_Itr != l_Religions.end();
		l_Itr++)
	{
        l_iInitialPopulationOfReligions += in_pRegion->ReligionGetPopulation(l_Itr->first);
        /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
        l_iInitialPopulationOfReligions +=
            in_pRegion->ReligionGetPopulation(l_Itr->first);
            if(m_CountryData->CountryID() == 142)
                    m_CountryData->NameAndIDForLog() + L": Population of " +
                    g_ServerDAL.GetString(g_ServerDAL.StringIdReligion(l_Itr->first)) +
                    L": " +
                    GString(m_CountryData->ReligionGetPourcentage(l_Itr->first)) + L" " +
                    GString(in_pRegion->ReligionGetPopulation(l_Itr->first)));*/
    }

    /*gassert(l_iInitialPopulation == l_iInitialPopulationOfReligions,
            L"Initial total population of religions should be equal to initial total population: " +
            g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
            m_CountryData->NameAndIDForLog() + L": " +
            L"Total population " + GString(l_iInitialPopulation) + L" vs. " +
            L"population of religions " + GString(l_iInitialPopulationOfReligions));*/

	INT64 l_iMaxPopulationReligion = -1;
	INT32 l_iMaxReligion = 0;
	for(GReligionList::iterator l_Itr = l_Religions.begin();
		l_Itr != l_Religions.end();
		l_Itr++)
	{
		if(l_Itr->second > l_iMaxPopulationReligion)
		{
			l_iMaxPopulationReligion = l_Itr->second;
			l_iMaxReligion = l_Itr->first;
		}
		if(l_iTemp != 0)
		{
			l_iTempCalc = (INT64)(m_CountryData->ReligionGetPourcentage(l_Itr->first) * l_iDifference);
			in_pRegion->ReligionChangePopulation(l_Itr->first,l_iTempCalc + l_Itr->second);
			l_iTemp -= l_iTempCalc;
		}
		if (l_iTempLeftToArrive != 0)
		{
			l_iTempCalc = (INT64)(m_CountryData->ReligionGetPourcentage(l_Itr->first) * l_iArrivingPopulation);
			if (m_CountryData->ReligionGetStatus(l_Itr->first) == 2)
				l_iTempCalc = 0;
			in_pRegion->ReligionChangePopulation(l_Itr->first,l_iTempCalc + in_pRegion->ReligionGetPopulation(l_Itr->first));
			l_iTempLeftToArrive -= l_iTempCalc;
		}
		if (l_iTempLeftToLeave != 0)
		{			
			l_iTempCalc = (INT64)(m_CountryData->ReligionGetPourcentage(l_Itr->first) * l_iLeavingPopulation);
			if (m_CountryData->ReligionGetStatus(l_Itr->first) == 2)
				l_iTempCalc *= 10;			
			if (l_iTempCalc > l_iTempLeftToLeave)
				l_iTempCalc = l_iTempLeftToLeave;
			if (l_iTempCalc > in_pRegion->ReligionGetPopulation(l_Itr->first))
				l_iTempCalc = in_pRegion->ReligionGetPopulation(l_Itr->first);
			in_pRegion->ReligionChangePopulation(l_Itr->first,in_pRegion->ReligionGetPopulation(l_Itr->first) - l_iTempCalc );
			l_iTempLeftToLeave -= l_iTempCalc;
		}

        /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
                    m_CountryData->NameAndIDForLog() + L": New population of " +
                    g_ServerDAL.GetString(g_ServerDAL.StringIdReligion(l_Itr->first)) +
                    L": " +
                    GString(in_pRegion->ReligionGetPopulation(l_Itr->first)));*/
		l_iPopulationReligion += in_pRegion->ReligionGetPopulation(l_Itr->first);

	}	
	//If there are no religion selected, it means that region had none before. We must then check the most popular religion for that country.
	if(l_iMaxReligion == 0 && (l_iTemp != 0 || l_iTempLeftToArrive != 0 || l_iTempLeftToLeave != 0))
	{
		l_iMaxPopulationReligion = -1;

		const GReligionList& l_vTempReligions = m_CountryData->GetReligions();
		for(GReligionList::const_iterator l_Itr = l_vTempReligions.begin();
			l_Itr != l_vTempReligions.end();
			l_Itr++)
		{
			if(l_Itr->second > l_iMaxPopulationReligion)
			{
				l_iMaxPopulationReligion = l_Itr->second;
				l_iMaxReligion = l_Itr->first;
			}
		}
		if(l_iMaxReligion == -1)
		{
			if(l_vTempReligions.size())
			{
				l_iMaxReligion = l_vTempReligions.begin()->first;
			}
			else
			{
				//! \todo 53 is the ID for the "Other" religion.
				l_iMaxReligion = 53;
			}
		}

	}

    /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
                m_CountryData->NameAndIDForLog() + L": " +
                L"Remaining population " + GString(l_iTemp) + L", " +
                L"Remaining immigrants " + GString(l_iTempLeftToArrive) + L", " +
                L"Remaining emigrants " + GString(l_iTempLeftToLeave));*/

	if (l_iTemp != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest religion population
		in_pRegion->ReligionChangePopulation(l_iMaxReligion,in_pRegion->ReligionGetPopulation(l_iMaxReligion) + l_iTemp);
		l_iPopulationReligion += l_iTemp;
	}
	if (l_iTempLeftToArrive != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest religion population
		in_pRegion->ReligionChangePopulation(l_iMaxReligion,in_pRegion->ReligionGetPopulation(l_iMaxReligion) + l_iTempLeftToArrive);
		l_iPopulationReligion += l_iTempLeftToArrive;
	}
	if (l_iTempLeftToLeave != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest religion population
		in_pRegion->ReligionChangePopulation(l_iMaxReligion,in_pRegion->ReligionGetPopulation(l_iMaxReligion) - l_iTempLeftToLeave);
		l_iPopulationReligion -= l_iTempLeftToLeave;
	}

	l_iTemp = l_iDifference;	
	in_pRegion->GetLanguages(l_Languages);	
	l_iTempLeftToArrive = l_iArrivingPopulation;
	l_iTempLeftToLeave = l_iLeavingPopulation;
	INT64 l_iMaxPopulationLanguage = -1;
	INT32 l_iMaxLanguage = 0;
	for(GLanguageList::iterator l_Itr = l_Languages.begin();
		l_Itr != l_Languages.end();
		l_Itr++)
	{
		if(l_Itr->second > l_iMaxPopulationLanguage)
		{
			l_iMaxPopulationLanguage = l_Itr->second;
			l_iMaxLanguage = l_Itr->first;
		}
		if(l_iTemp != 0)
		{
			l_iTempCalc = (INT64)(m_CountryData->LanguageGetPourcentage(l_Itr->first) * l_iDifference);
			in_pRegion->LanguageChangePopulation(l_Itr->first,l_iTempCalc + l_Itr->second);
			l_iTemp -= l_iTempCalc;
		}
		if (l_iTempLeftToArrive != 0)
		{
			l_iTempCalc = (INT64)(m_CountryData->LanguageGetPourcentage(l_Itr->first) * l_iArrivingPopulation);
			if (m_CountryData->LanguageGetStatus(l_Itr->first) == 2)
				l_iTempCalc = 0;
			in_pRegion->LanguageChangePopulation(l_Itr->first,l_iTempCalc + in_pRegion->LanguageGetPopulation(l_Itr->first));
			l_iTempLeftToArrive -= l_iTempCalc;
		}
		if (l_iTempLeftToLeave != 0)
		{
			l_iTempCalc = (INT64)(m_CountryData->LanguageGetPourcentage(l_Itr->first) * l_iLeavingPopulation);
			if (m_CountryData->LanguageGetStatus(l_Itr->first) == 2)
				l_iTempCalc *= 10;	
			if (l_iTempCalc > l_iTempLeftToLeave)
				l_iTempCalc = l_iTempLeftToLeave;
			if (l_iTempCalc > in_pRegion->LanguageGetPopulation(l_Itr->first))
				l_iTempCalc = in_pRegion->LanguageGetPopulation(l_Itr->first);
			in_pRegion->LanguageChangePopulation(l_Itr->first,in_pRegion->LanguageGetPopulation(l_Itr->first) - l_iTempCalc );
			l_iTempLeftToLeave -= l_iTempCalc;
		}

		l_iPopulationLanguage += in_pRegion->LanguageGetPopulation(l_Itr->first);
	}	
	//If there are no language selected, it means that region had none before. We must then check the most popular language for that country.
	if(l_iMaxLanguage == 0 && (l_iTemp != 0 || l_iTempLeftToArrive != 0 || l_iTempLeftToLeave != 0))
	{
		l_iMaxPopulationLanguage = -1;

		const GLanguageList& l_vTempLanguages = m_CountryData->GetLanguages();
		for(GLanguageList::const_iterator l_Itr = l_vTempLanguages.begin();
			l_Itr != l_vTempLanguages.end();
			l_Itr++)
		{
			if(l_Itr->second > l_iMaxPopulationLanguage)
			{
				l_iMaxPopulationLanguage = l_Itr->second;
				l_iMaxLanguage = l_Itr->first;
			}
		}
		if(l_iMaxLanguage == 0)
		{
			if(l_vTempLanguages.size())
			{
				l_iMaxLanguage = l_vTempLanguages.begin()->first;
			}
			else
			{
				//! \todo 170 is the ID for the "Other" language.
				l_iMaxLanguage = 170;
			}
		}

	}
	if (l_iTemp != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest Language population
		in_pRegion->LanguageChangePopulation(l_iMaxLanguage,in_pRegion->LanguageGetPopulation(l_iMaxLanguage) + l_iTemp);
		l_iPopulationLanguage += l_iTemp;
	}
	if (l_iTempLeftToArrive != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest Language population
		in_pRegion->LanguageChangePopulation(l_iMaxLanguage,in_pRegion->LanguageGetPopulation(l_iMaxLanguage) + l_iTempLeftToArrive);
		l_iPopulationLanguage += l_iTempLeftToArrive;
	}
	if (l_iTempLeftToLeave != 0)
	{
		//we have not put every new people in, so we add the remaining at the largest Language population
		in_pRegion->LanguageChangePopulation(l_iMaxLanguage,in_pRegion->LanguageGetPopulation(l_iMaxLanguage) - l_iTempLeftToLeave);
		l_iPopulationLanguage -= l_iTempLeftToLeave;
	}

	//Calculate the total population, for debugging purposes
	l_iTempPopulationAll = in_pRegion->Population15() + in_pRegion->Population1565() + in_pRegion->Population65();	
    /*GDZDEBUGLOG(g_ServerDAL.RegionNameAndIDForLog(in_pRegion->Id()) + L" of " +
                m_CountryData->NameAndIDForLog() + L": " +
                L"Population15() " + GString(in_pRegion->Population15()) + L", " +
                L"Population1565() " + GString(in_pRegion->Population1565()) + L", " +
                L"Population65() " + GString(in_pRegion->Population65()));*/

	INT64 l_iNbConverts = 0;
	INT64 l_iTempConverts = 0;
	l_iMaxPopulationReligion = -1;
	l_iMaxReligion = 0;
	in_pRegion->GetReligions(l_Religions);

	//We go through the religion list, and remove some from the illegal religions, and convert them
	//to the most popular religion
	for(GReligionList::iterator l_Itr = l_Religions.begin();
		l_Itr != l_Religions.end();
		l_Itr++)
	{
		if(l_Itr->second > l_iMaxPopulationReligion
			&& m_CountryData->ReligionGetStatus(l_Itr->first) != 2)
		{
			l_iMaxPopulationReligion = l_Itr->second;
			l_iMaxReligion = l_Itr->first;
		}
		if (m_CountryData->ReligionGetStatus(l_Itr->first) == 2) //if illegal, then let's remove some
		{
			l_iTempConverts = ReturnInteger64(m_fFrequency * PourcConvertsConstReligion * (REAL32)l_Itr->second);
			in_pRegion->ReligionChangePopulation(l_Itr->first
				,l_Itr->second-l_iTempConverts);
			l_iNbConverts += l_iTempConverts;

		}
	}
	//Now that we have gone through the list of religions, let's add the converts to the most populous, and legal, religion;
	in_pRegion->ReligionChangePopulation(l_iMaxReligion,
		in_pRegion->ReligionGetPopulation(l_iMaxReligion) + l_iNbConverts);

	l_iNbConverts = 0;
	l_iTempConverts = 0;
	l_iMaxPopulationLanguage = -1;
	l_iMaxLanguage = 0;
	in_pRegion->GetLanguages(l_Languages);

	//We go through the Language list, and remove some from the illegal Languages, and convert them
	//to the most popular Language
	for(GLanguageList::iterator l_Itr = l_Languages.begin();
		l_Itr != l_Languages.end();
		l_Itr++)
	{
		if(l_Itr->second > l_iMaxPopulationLanguage
			&& m_CountryData->LanguageGetStatus(l_Itr->first) != 2)
		{
			l_iMaxPopulationLanguage = l_Itr->second;
			l_iMaxLanguage = l_Itr->first;
		}
		if (m_CountryData->LanguageGetStatus(l_Itr->first) == 2) //if illegal, then let's remove some
		{
			l_iTempConverts = ReturnInteger64(m_fFrequency * PourcConvertsConstLanguage * (REAL32)l_Itr->second);
			in_pRegion->LanguageChangePopulation(l_Itr->first
				,l_Itr->second-l_iTempConverts);
			l_iNbConverts += l_iTempConverts;

		}
	}
	//Now that we have gone through the list of Languages, let's add the converts to the most populous, and legal, Language;
	in_pRegion->LanguageChangePopulation(l_iMaxLanguage,
		in_pRegion->LanguageGetPopulation(l_iMaxLanguage) + l_iNbConverts);


	//Check if our population of languages and religions are equal to the total population
//	gassert(l_iTempPopulationAll == l_iPopulationLanguage,"Population of language should be equal to total population");
	/*gassert(l_iTempPopulationAll == l_iPopulationReligion,
            "Population of religion should be equal to total population: " +
            g_ServerDAL.RegionNameAndIDForLog(l_iRegionId) + L" of " +
            m_CountryData->NameAndIDForLog() + L": " +
            L"Total population " + GString(l_iTempPopulationAll) + L" vs. " +
            L"population of all religions " + GString(l_iPopulationReligion));*/

	return true;
}

void GWorldBehavior::Iterate_Gvt_Approval_Expected()
{
   const REAL32 PourcObjTaxesRaw		   = -0.15f;
	const REAL32 PourcObjTaxes				= 0.2f;
	const REAL32 PourcObjUnemployment	= 0.10f;
	const REAL32 PourcObjStability		= 0.15f;
	const REAL32 PourcObjPhilosophy		= 0.05f;	
	const REAL32 PourcBudgetExpenses		= 0.35f;
	const REAL32 PourcFreedomOfSpeech	= 0.04f;
	const REAL32 PourcFreedomOfDemonstration	= 0.06f;
	const REAL32 PourcDensityOfPopulation = 0.05f;

	REAL32 l_fSpeech = 1.f;
	REAL32 l_fDemonstration = 1.f;
	REAL32 l_fResults = 0.0f;
	REAL32 l_fPersonalIncomeTax    = FindTaxBudgetFactor(m_CountryData->CountryID());
   REAL32 l_fPersonalIncomeTaxRaw = (REAL32)m_CountryData->PersonalIncomeTax();
	REAL32 l_fStability = m_CountryData->GvtStability();
	REAL32 l_fUnemployment = FindUnemploymentFactor(m_CountryData->CountryID());
	REAL32 l_fPhilosophy = abs(m_CountryData->CountryPoliticalIdeology() - m_CountryData->PopPoliticalIdeology());
	REAL32 l_fExpenses = 0.f;
	REAL32 l_fDensity = FindPopulationDensityFactor(m_CountryData->CountryID());

	if(!m_CountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		l_fSpeech = 0.f;
	if(!m_CountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
		l_fDemonstration = 0.f;

	/*!
	* Expenses having an impact on approval rating (6):
	*	infrastructure, environment, healthcare, education, telecom, propaganda
	**/
	REAL32 l_fBestValueTelecom = 0.5f * (1.f + g_ServerDCL.CalculateProbabilityOfAction(
		m_CountryData,
		SP2::c_fAIActionDoublePlus,
		SP2::c_fAIActionMinus,
		SP2::c_fAIActionNeutral,
		SP2::c_fAIActionNeutral));
	REAL32 l_fBestValueInfrastructure = 0.5f * (1.f + m_CountryData->HumanDevelopment());
	REAL32 l_fBestValueEnvironment = 0.5f * (1.f + g_ServerDCL.CalculateProbabilityOfAction(
		m_CountryData,
		SP2::c_fAIActionNeutral,
		SP2::c_fAIActionMinus,
		SP2::c_fAIActionDoubleMinus,
		SP2::c_fAIActionDoubleMinus));
	REAL32 l_fBestValueHealthcare = 0.5f * (1.f + g_ServerDCL.CalculateProbabilityOfAction(
		m_CountryData,
		SP2::c_fAIActionNeutral,
		SP2::c_fAIActionMinus,
		SP2::c_fAIActionDoubleMinus,
		SP2::c_fAIActionNeutral));
	REAL32 l_fBestValueEducation = 0.5f * (1.f + g_ServerDCL.CalculateProbabilityOfAction(
		m_CountryData,
		SP2::c_fAIActionNeutral,
		SP2::c_fAIActionMinus,
		SP2::c_fAIActionDoubleMinus,
		SP2::c_fAIActionNeutral));
	REAL32 l_fBestValuePropaganda = 0.5f * (1.f + m_CountryData->HumanDevelopment());

	if((REAL32)m_CountryData->BudgetExpenseTelecomRatio() <= l_fBestValueTelecom)
	{
		l_fExpenses += (REAL32)m_CountryData->BudgetExpenseTelecomRatio() / l_fBestValueTelecom;
	}
	else
	{
		l_fExpenses += 1.f + 
			((((REAL32)m_CountryData->BudgetExpenseTelecomRatio() - l_fBestValueTelecom) /
			(1.f - l_fBestValueTelecom)) * 0.5f);
	}
	if((REAL32)m_CountryData->BudgetExpenseInfrastructureRatio() <= l_fBestValueInfrastructure)
	{
		l_fExpenses += (REAL32)m_CountryData->BudgetExpenseInfrastructureRatio() / l_fBestValueInfrastructure;
	}
	else
	{
		l_fExpenses += 1.f + 
			((((REAL32)m_CountryData->BudgetExpenseInfrastructureRatio() - l_fBestValueInfrastructure) /
			(1.f - l_fBestValueInfrastructure)) * 0.5f);
	}
	if((REAL32)m_CountryData->BudgetExpenseEnvironmentRatio() <= l_fBestValueEnvironment)
	{
		l_fExpenses += (REAL32)m_CountryData->BudgetExpenseEnvironmentRatio() / l_fBestValueEnvironment;
	}
	else
	{
		l_fExpenses += 1.f + 
			((((REAL32)m_CountryData->BudgetExpenseEnvironmentRatio() - l_fBestValueEnvironment) /
			(1.f - l_fBestValueEnvironment)) * 0.5f);
	}
	if((REAL32)m_CountryData->BudgetExpenseHealthcareRatio() <= l_fBestValueHealthcare)
	{
		l_fExpenses += (REAL32)m_CountryData->BudgetExpenseHealthcareRatio() / l_fBestValueHealthcare;
	}
	else
	{
		l_fExpenses += 1.f + 
			((((REAL32)m_CountryData->BudgetExpenseHealthcareRatio() - l_fBestValueHealthcare) /
			(1.f - l_fBestValueHealthcare)) * 0.5f);
	}
	if((REAL32)m_CountryData->BudgetExpenseEducationRatio() <= l_fBestValueEducation)
	{
		l_fExpenses += (REAL32)m_CountryData->BudgetExpenseEducationRatio() / l_fBestValueEducation;
	}
	else
	{
		l_fExpenses += 1.f + 
			((((REAL32)m_CountryData->BudgetExpenseEducationRatio() - l_fBestValueEducation) /
			(1.f - l_fBestValueEducation)) * 0.5f);
	}
	REAL32 l_fBonusFreedoms = 0.f;
	if(!m_CountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
		l_fBonusFreedoms += SP2::c_fPropagandaEffectFreedomOfDemonstration;
	if(!m_CountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		l_fBonusFreedoms += SP2::c_fPropagandaEffectFreedomOfSpeech;
	if(l_fBonusFreedoms == 0.f)
		l_fBonusFreedoms = 1.f;
	if((REAL32)m_CountryData->BudgetExpensePropagandaRatio() <= l_fBestValuePropaganda)
	{
		l_fExpenses += l_fBonusFreedoms * ((REAL32)m_CountryData->BudgetExpensePropagandaRatio() / l_fBestValuePropaganda);

	}
	else
	{
		l_fExpenses += l_fBonusFreedoms * (1.f + 
			((((REAL32)m_CountryData->BudgetExpensePropagandaRatio() - l_fBestValuePropaganda) /
			(1.f - l_fBestValuePropaganda)) * 0.5f));
	}


	l_fExpenses /= 6.f; //6 budget expenses
	if(l_fExpenses > 1.f)
		l_fExpenses = 1.f;

	/*!
	* Philosophy
	* If the difference between the 2 government is over .5, its a complete negative
	* If the difference between the 2 government is lower than 0.02, its a complete positive
	* Anything between the two differs between 0 and 1. 
	* A difference of 0.25 is considered average
	**/
	if(l_fPhilosophy > 0.5f)
		l_fPhilosophy = 0.0f;
	else if (l_fPhilosophy < 0.02f)
		l_fPhilosophy = 1.0f;
	else if (l_fPhilosophy > 0.25f)
		l_fPhilosophy = (-2.f * l_fPhilosophy) + 1.f;
	else
		l_fPhilosophy = (-2.17391304347f * l_fPhilosophy) + 1.0434782608694f;

	l_fResults = (PourcObjTaxes * l_fPersonalIncomeTax) 
      + (PourcObjUnemployment * l_fUnemployment)
		+ (PourcObjStability * l_fStability) 
      + (PourcObjPhilosophy * l_fPhilosophy)
		+ (PourcBudgetExpenses * l_fExpenses)
		+ (PourcFreedomOfDemonstration * l_fDemonstration) 
      + (PourcFreedomOfSpeech * l_fSpeech)
		+ (PourcDensityOfPopulation * l_fDensity)
      + (PourcObjTaxesRaw * l_fPersonalIncomeTaxRaw);

	//If the martial law is on, there is a fix penalty.
	if(m_CountryData->MartialLaw())
		l_fResults -= 0.05f;

	if(l_fResults > 1.f)
		l_fResults = 1.f;
	else if (l_fResults < 0.f)
		l_fResults = 0.f;

    gassert(l_fResults >= 0.f && l_fResults <= 1.f,
            "Invalid expected approval, " + GString(l_fResults) + L", " +
            L"for " + m_CountryData->NameAndIDForLog());
	m_CountryData->GvtApprovalExpected(l_fResults);

}

bool GWorldBehavior::Iterate_Inflation_Level_Expected()
{
	REAL32 l_fEconomicHealth = m_CountryData->EconomicHealth();
	REAL32 l_fInflationExpected = 0.f;
	if(l_fEconomicHealth > 0.5f)
		l_fInflationExpected = (0.16f * l_fEconomicHealth) - 0.06f;
	else
		l_fInflationExpected = 0.04f * l_fEconomicHealth;

	m_CountryData->InflationLevelExpected(l_fInflationExpected);

	return true;
}

void GWorldBehavior::Iterate_Infrastructure_Expected()
{
	m_CountryData->InfrastructureExpected((REAL32)m_CountryData->BudgetExpenseInfrastructureRatio());
}
void GWorldBehavior::Iterate_Telecom_Expected()
{
	m_CountryData->TelecomLevelExpected((REAL32)m_CountryData->BudgetExpenseTelecomRatio());
}

bool GWorldBehavior::Iterate_Telecom_Level(GRegion* in_pRegion, REAL64 in_fGameTime)
{
	REAL32 l_fTimeDifference = (REAL32)(in_fGameTime - m_CountryData->LastIteration());
	REAL32 l_fModTelecom = l_fTimeDifference / (REAL32)SP2::c_fTelecomIterationPeriod;

	//Change corruption
	REAL32 l_fTelecom = in_pRegion->TelecomLevel();
	REAL32 l_fTelecomEx = m_CountryData->TelecomLevelExpected();
	in_pRegion->TelecomLevel( l_fTelecom + ((l_fTelecomEx - l_fTelecom)*l_fModTelecom) );

	return true;
}

bool GWorldBehavior::Iterate_Infrastructure(GRegion* in_pRegion, REAL64 in_fGameTime)
{
	REAL32 l_fTimeDifference = (REAL32)(in_fGameTime - m_CountryData->LastIteration());
	REAL32 l_fModInfrastructure = l_fTimeDifference / (REAL32)SP2::c_fInfrastrucutureIterationPeriod;

	//Change corruption
	REAL32 l_fInfrastrucuture = in_pRegion->Infrastructure();
	REAL32 l_fInfrastrucutureEx = m_CountryData->InfrastructureExpected();
	in_pRegion->Infrastructure( l_fInfrastrucuture + ((l_fInfrastrucutureEx - l_fInfrastrucuture)*l_fModInfrastructure) );

	return true;
}

bool GWorldBehavior::Iterate_Tourism(GRegion* in_pRegion)
{
	const REAL64 PourcInfrastrucuture	= 0.15f;
	const REAL64 PourcTourism				= 0.5f;
	const REAL64 PourcStability			= 0.35f;

	REAL64 l_fTourism = m_CountryData->BudgetExpenseTourismRatio();
	REAL64 l_fInfrastrucutre = m_CountryData->BudgetExpenseInfrastructureRatio();
	REAL64 l_fStability = (REAL64)m_CountryData->GvtStability();

	REAL64 l_fResults = (PourcInfrastrucuture * l_fInfrastrucutre) + (PourcTourism * l_fTourism)
		+ (PourcStability * l_fStability);
	//Tourism income can change from +/- 20% per year
	l_fResults = (l_fResults * 0.4f) + 0.8f;	

	// If battles are occurring in the country, then reduce tourism income
	REAL32 l_fBattleMultiplier = 1.f;

    // Militarily occupied by someone else - 0.5
    if(in_pRegion->OwnerMilitaryId() != in_pRegion->OwnerId())
    {
        l_fBattleMultiplier = 0.5f;
    }

    // Battles in our region - 0.25
    const vector<SDK::Combat::GArena*> l_vArenas = g_CombatManager.Arenas();
    for(UINT32 i = 0 ; i < l_vArenas.size() ; i++)
    {
         SP2::GArena*     l_pArena      = (SP2::GArena*)l_vArenas[i];
         SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)l_pArena->Info();
         if (l_pArenaInfo->m_iRegionID == in_pRegion->Id())
         {
             //g_Joshua.Log("Region is under attack");
             l_fBattleMultiplier = 0.25f;
             break;
         }
    }

    // Bombardment?
    if(l_fBattleMultiplier > 0.25f)
    {
        bool bombarded = false;
        for (UINT32 i=1; i<=(UINT32)g_ServerDAL.NbCountry(); i++)
	    {
		    if(!g_ServerDAL.CountryIsValid(i))
            {
			    continue;
            }

            const set<UINT32>& l_UnitGroup = g_Joshua.UnitManager().CountryUnitGroups(i);
            set<UINT32>::const_iterator l_UnitGroupIterator = l_UnitGroup.begin();
   
            while(l_UnitGroupIterator != l_UnitGroup.end())
            {
                SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIterator);
                if ( l_pUnitGroup->RegionToBombard() == in_pRegion->OwnerId() )
                {
                    bombarded = true;
                    break;
                }

                ++l_UnitGroupIterator;
            }

            if (bombarded)
            {
                l_fBattleMultiplier = 0.25f;
                break;
            }
        }
    }

	l_fResults *= l_fBattleMultiplier;

	REAL64 l_fNewTourism = in_pRegion->TourismIncome();

	if(l_fNewTourism == 0.f)
	{
		if(l_fResults > 1.f)
			l_fNewTourism = l_fResults * m_fFrequency / 10.f;
	}
	else
		l_fNewTourism += (((l_fNewTourism * l_fResults) - l_fNewTourism) * m_fFrequency);	

	if (l_fNewTourism < 0.f)
		l_fNewTourism = 0.f;	

	//Check Production of that region
	REAL64 l_fProduction = 0.f;
	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		l_fProduction += in_pRegion->ResourceProduction((EResources::Enum)i);
	}

	if(l_fNewTourism > (l_fProduction * 0.01f))
		l_fNewTourism = l_fProduction * 0.05f * m_CountryData->BudgetExpenseTourismRatio();

	in_pRegion->TourismIncome(l_fNewTourism);

	return true;
}

bool GWorldBehavior::Iterate_Production(GRegion* in_pRegion)
{
	REAL64 l_fTempProduction = 0.f;

	const REAL64* l_pMarketProduction = g_ServerDAL.MarketProduction();
	const REAL64* l_pMarketDemand = g_ServerDAL.MarketDemand();
	const REAL64* l_pResourceBonus = g_ServerDAL.CountryEconomicPartnership(m_CountryData->CountryID());

	REAL64 l_fProductionBonusChildLabour = 1.f;
	if(m_CountryData->InternalLaw(EInternalLaws::ChildLabor))
		l_fProductionBonusChildLabour = SP2::c_fProductionBonusChildLabour;

	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{		
		REAL64 l_fBudgetModifier = 1.f;
		if(i <= EResources::Drugs)
			l_fBudgetModifier = 0.5f + m_CountryData->BudgetExpenseEnvironmentRatio();
		else if( i >= EResources::Construction && i <= EResources::Marketing_Advertising )
			l_fBudgetModifier = 0.5f + m_CountryData->BudgetExpenseEducationRatio();

		l_fTempProduction = in_pRegion->ResourceProduction((EResources::Enum)i);
		if(l_fTempProduction >= 1.f && 
			( m_CountryData->ResourceProduction((EResources::Enum)i) < m_CountryData->ResourceDemand((EResources::Enum)i) || l_pMarketProduction[i] < l_pMarketDemand[i] ))
		{
			REAL64 l_fTaxLevel = (REAL64)(m_CountryData->ResourceTaxes((EResources::Enum)i) + m_CountryData->GlobalTaxMod());
			REAL64 l_fTaxMod = 0.15 * pow(l_fTaxLevel - 2.25, 2) - 0.009375;
			REAL64 l_fProductionGrowth = SP2::c_pResourcesYearlyGain[i] * (m_CountryData->EconomicHealth() * 2.f) * l_fTaxMod * l_fBudgetModifier;

			gassert(!_isnan(l_fProductionGrowth),"Production growth is NAN");

			//Bonus from the economic partnership
			l_fProductionGrowth *= 1.f + l_pResourceBonus[i];

			l_fProductionGrowth *= l_fProductionBonusChildLabour;

			if(m_CountryData->ResourceGvtCtrl((EResources::Enum)i))
				l_fProductionGrowth *= SP2::c_fPublicResourcePenalty;

			l_fTempProduction += (l_fTempProduction * l_fProductionGrowth * (REAL64)m_fFrequency);

			if(l_fTempProduction < 1.f)
				l_fTempProduction = 1.f;

			in_pRegion->ResourceProduction((EResources::Enum)i,l_fTempProduction);
		}				
	}

	return true;
}

/*
bool GWorldBehavior::Iterate_Demand(GRegion* in_pRegion)
{
REAL64 l_fGDPGrowth = (REAL64)m_CountryData->GDPGrowth() + 1.f;
REAL64 l_fInflationGrowth = (REAL64)m_CountryData->InflationGrowth() + 1.f;
REAL64 l_fTempNewDemand = 0.0f;
REAL64 l_fFrequency = (REAL64)m_fFrequency;
REAL64 l_NewDemand[EResources::ItemCount];
//Cereals
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Cereals)  
* l_fGDPGrowth * l_fInflationGrowth) + (0.05f * in_pRegion->ResourceDemand(EResources::Meat) )
+ (0.01f * in_pRegion->ResourceDemand(EResources::Dairy));
l_NewDemand[EResources::Cereals] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Cereals)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Cereals) ;
//Vegetables
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Vegetable_Fruits)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Vegetable_Fruits] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Vegetable_Fruits)) * l_fFrequency)
+ in_pRegion->ResourceDemand(EResources::Vegetable_Fruits);
//Meat
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Meat) 
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Meat] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Meat)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Meat);
//Dairy
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Dairy) 
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Dairy] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Dairy)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Dairy);
//Tobacco
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Tobacco) 
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Tobacco] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Tobacco)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Tobacco);
//Drugs
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Drugs)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Drugs] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Drugs)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Drugs);
//Electricity
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Electricity)  
* l_fGDPGrowth * l_fInflationGrowth) + (0.02f * in_pRegion->ResourceDemand(EResources::Wood_Paper) )
+ (0.04f * in_pRegion->ResourceDemand(EResources::Plastics))
+ (0.02f * in_pRegion->ResourceDemand(EResources::Fabrics))
+ (0.01f * in_pRegion->ResourceDemand(EResources::Chemicals))
+ (0.02f * in_pRegion->ResourceDemand(EResources::Pharmaceuticals))
+ (0.04f * in_pRegion->ResourceDemand(EResources::Appliances))
+ (0.02f * in_pRegion->ResourceDemand(EResources::Vehicules))
+ (0.02f * in_pRegion->ResourceDemand(EResources::Machinery))
+ (0.01f * in_pRegion->ResourceDemand(EResources::Commodities))
+ (0.02f * in_pRegion->ResourceDemand(EResources::Luxury_Commodities));
l_NewDemand[EResources::Electricity] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Electricity)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Electricity);
//Fossile Fuels
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Fossile_Fuels)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Fossile_Fuels] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Fossile_Fuels)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Fossile_Fuels);
//Wood Paper
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Wood_Paper) * l_fGDPGrowth
* l_fInflationGrowth) + (0.04f * in_pRegion->ResourceDemand(EResources::Construction) );
l_NewDemand[EResources::Wood_Paper] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Wood_Paper)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Wood_Paper);
//Minerals
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Minerals) * l_fGDPGrowth
* l_fInflationGrowth);
l_NewDemand[EResources::Minerals] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Minerals)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Minerals);
//Iron
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Iron_Steel) * l_fGDPGrowth
* l_fInflationGrowth) + (0.04f * in_pRegion->ResourceDemand(EResources::Appliances) )
+ (0.04f * in_pRegion->ResourceDemand(EResources::Vehicules) )
+ (0.04f * in_pRegion->ResourceDemand(EResources::Machinery) )
+ (0.06f * in_pRegion->ResourceDemand(EResources::Construction) );
l_NewDemand[EResources::Iron_Steel] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Iron_Steel)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Iron_Steel);
//Precious Stones
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Precious_Stones)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Precious_Stones] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Precious_Stones)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Precious_Stones);
//Plastics
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Plastics) * l_fGDPGrowth
* l_fInflationGrowth) + (0.01f * in_pRegion->ResourceDemand(EResources::Luxury_Commodities) )
+ (0.02f * in_pRegion->ResourceDemand(EResources::Commodities) );
l_NewDemand[EResources::Plastics] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Plastics)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Plastics);
//Fabrics
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Fabrics) * l_fGDPGrowth
* l_fInflationGrowth) + (0.05f * in_pRegion->ResourceDemand(EResources::Luxury_Commodities) )
+ (0.01f * in_pRegion->ResourceDemand(EResources::Commodities) );
l_NewDemand[EResources::Fabrics] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Fabrics)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Fabrics);
//Chemicals
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Chemicals) * l_fGDPGrowth
* l_fInflationGrowth) + (0.1f * in_pRegion->ResourceDemand(EResources::Luxury_Commodities) )
+ (0.2f * in_pRegion->ResourceDemand(EResources::Commodities) )
+ (0.1f * in_pRegion->ResourceDemand(EResources::Plastics) )
+ (0.1f * in_pRegion->ResourceDemand(EResources::Pharmaceuticals) );
l_NewDemand[EResources::Chemicals] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Chemicals)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Chemicals);
//Pharmaceutical
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Pharmaceuticals)  
* l_fGDPGrowth * l_fInflationGrowth) + (0.25f * in_pRegion->ResourceDemand(EResources::Health_Care) );		
l_NewDemand[EResources::Pharmaceuticals] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Pharmaceuticals)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Pharmaceuticals);
//Appliances
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Appliances)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Appliances] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Appliances)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Appliances);
//Vehicules
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Vehicules)  
* l_fGDPGrowth * l_fInflationGrowth);
l_NewDemand[EResources::Vehicules] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Vehicules)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Vehicules);
//Machinery
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Machinery) * l_fGDPGrowth) 
+ (0.2f * in_pRegion->ResourceDemand(EResources::Construction) )
+ (0.03f * in_pRegion->ResourceDemand(EResources::Fossile_Fuels) )
+ (0.02f * in_pRegion->ResourceDemand(EResources::Wood_Paper) )
+ (0.05f * in_pRegion->ResourceDemand(EResources::Minerals) )
+ (0.01f * in_pRegion->ResourceDemand(EResources::Precious_Stones) );
l_NewDemand[EResources::Machinery] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Machinery)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Machinery);
//Commodities
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Commodities) 
* (((l_fGDPGrowth-1.f)*2.5f)+1.f));
l_NewDemand[EResources::Commodities] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Commodities)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Commodities);
//Luxury Commodities
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Luxury_Commodities) 
* (((l_fGDPGrowth-1.f)*2.f)+1.f));
l_NewDemand[EResources::Luxury_Commodities] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Luxury_Commodities)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Luxury_Commodities);
//Construction
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Construction) * l_fGDPGrowth);
l_NewDemand[EResources::Construction] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Construction)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Construction);
//Engineering
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Engineering) * l_fGDPGrowth)
+ (0.01f * in_pRegion->ResourceDemand(EResources::Construction) );
l_NewDemand[EResources::Engineering] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Engineering)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Engineering);
//Health Care
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Health_Care) * l_fGDPGrowth
* l_fInflationGrowth);
l_NewDemand[EResources::Health_Care] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Health_Care)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Health_Care);
//Retail
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Retail) * l_fGDPGrowth
* l_fInflationGrowth);
l_NewDemand[EResources::Retail] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Retail)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Retail);
//Legal Service
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Legal_Services) * l_fGDPGrowth);
l_NewDemand[EResources::Legal_Services] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Legal_Services)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Legal_Services);
//Market
l_fTempNewDemand = (in_pRegion->ResourceDemand(EResources::Marketing_Advertising) * (((l_fGDPGrowth-1.f)*0.5f)+1.f))
+ (0.5f * in_pRegion->ResourceDemand(EResources::Retail) );
l_NewDemand[EResources::Marketing_Advertising] = ((l_fTempNewDemand - in_pRegion->ResourceDemand(EResources::Marketing_Advertising)) * l_fFrequency) 
+ in_pRegion->ResourceDemand(EResources::Marketing_Advertising);

for(UINT32 i=0; i < EResources::ItemCount; i++)
{
in_pRegion->ResourceDemand((EResources::Enum)i,l_NewDemand[i]);
}

return true;
}

*/

bool GWorldBehavior::Iterate_Resources_Gdp()
{	
	REAL64 l_fTvalue2 = 0.0f;
	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		l_fTvalue2 += m_CountryData->ResourceProduction((SP2::EResources::Enum)i);
	}	

	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		m_CountryData->ResourceGDP((SP2::EResources::Enum)i,  (REAL32)(m_CountryData->ResourceProduction((SP2::EResources::Enum)i) / (l_fTvalue2 + 1.f)) );
	}	

	return true;
}

bool GWorldBehavior::Iterate_Political_Ideology()
{
	return true;
}

bool GWorldBehavior::Iterate_Economic_Model()
{
	return true;
}

bool GWorldBehavior::Iterate_Political_Party_Popularity()
{
	return true;
}

bool GWorldBehavior::Iterate_Pop_15_65_Unemployed()
{
	REAL32 l_fEconomicHealth = m_CountryData->EconomicHealth();
	REAL32 l_fNewUnemployement = 0.f;
	if(l_fEconomicHealth >= 0.5f)
		l_fNewUnemployement = (-0.04f*l_fEconomicHealth) + 0.06f;
	else
		l_fNewUnemployement = (-0.32f*l_fEconomicHealth) + 0.2f;

    const REAL32 l_fMaximumEmploymentChangePerIteration = 0.005f * m_fFrequency;
    const REAL32 l_fOldUnemployment = m_CountryData->Pop1565Unemployed();
    if(l_fNewUnemployement - l_fOldUnemployment < -l_fMaximumEmploymentChangePerIteration)
        l_fNewUnemployement = l_fOldUnemployment - l_fMaximumEmploymentChangePerIteration;
    else if(l_fNewUnemployement - l_fOldUnemployment > l_fMaximumEmploymentChangePerIteration)
        l_fNewUnemployement = l_fOldUnemployment + l_fMaximumEmploymentChangePerIteration;

	m_CountryData->Pop1565Unemployed(l_fNewUnemployement);	

	return true;
}

void GWorldBehavior::Iterate_Birth_Rate_Expected()
{
    static const REAL32 c_fRelativeEysContrib    = 0.641159635450596f;
    static const REAL32 c_fRelativeOver65Contrib = 0.746064534199632f;

    static const REAL32 c_fMinStabilityForNormalBirth = 0.5f;
    static const REAL32 c_fMaxEffectiveTaxForNormalBirth = 0.4f;

    GDZLOG(L"Calculating for " + m_CountryData->NameAndIDForLog(),
           EDZLogLevel::Info2);

    const REAL32 l_fEys = m_CountryData->ExpectedYearsSchooling();
    const REAL32 l_fExpectedFromEys = max(0.f, (-0.002655445944747f * l_fEys) + 0.043519905465958f);
    GDZLOG(L"EYS " + GString::FormatNumber(l_fEys, 1) + L", expected BR " + GString::FormatNumber(l_fExpectedFromEys * 100.f, L"", L".", L"", L"%", 3, 1, false),
           EDZLogLevel::Info2);

    const REAL32 l_fPopPercentOver65 = static_cast<REAL32>(m_CountryData->Pop65()) / static_cast<REAL32>(m_CountryData->Population());
    const REAL32 l_fExpectedFromOver65 = min(0.00309363345249f * powf(l_fPopPercentOver65, -0.715877580330996f), 0.1f);
    GDZLOG(L"% population over 65 " + GString::FormatNumber(l_fPopPercentOver65 * 100.f, L"", L".", L"", L"%", 3, 1, false) + L", expected BR " + GString::FormatNumber(l_fExpectedFromOver65 * 100.f, L"", L".", L"", L"%", 3, 1, false),
           EDZLogLevel::Info2);

    REAL32 l_fExpected = ((l_fExpectedFromEys * c_fRelativeEysContrib) + (l_fExpectedFromOver65 * c_fRelativeOver65Contrib)) / (c_fRelativeEysContrib + c_fRelativeOver65Contrib);
    GDZLOG(L"Raw BR from EYS and over 65 " + GString::FormatNumber(l_fExpected * 100.f, L"", L".", L"", L"%", 3, 1, false),
           EDZLogLevel::Info2);

    static const REAL32 c_fStabilityParabolaAmplitude = -(1.f / powf(c_fMinStabilityForNormalBirth, 2.f));
    const REAL32 l_fGvtStability = m_CountryData->GvtStability();
    const REAL32 l_fStabilityScaling = (l_fGvtStability < c_fMinStabilityForNormalBirth) ? (c_fStabilityParabolaAmplitude * powf(l_fGvtStability - c_fMinStabilityForNormalBirth, 2.f) + 1.f) : 1.f;
    GDZLOG(L"Stability " + GString::FormatNumber(l_fGvtStability * 100.f, L"", L".", L"", L"%", 3, 1, false) + L", stability scaling " + GString::FormatNumber(l_fStabilityScaling, 3),
           EDZLogLevel::Info2);
    l_fExpected *= l_fStabilityScaling;

    static const REAL32 c_fIncomeParabolaAmplitude = -(1.f / powf(1.f - c_fMaxEffectiveTaxForNormalBirth, 2.f));
    const REAL64 l_fIncomeTax = m_CountryData->PersonalIncomeTax();
    const REAL32 l_fHdi = m_CountryData->HumanDevelopment();
    const REAL64 l_fEffectiveTaxRate = l_fIncomeTax * l_fHdi;
    const REAL64 l_fTaxScaling = (c_fMaxEffectiveTaxForNormalBirth < l_fEffectiveTaxRate) ? (c_fIncomeParabolaAmplitude * pow(l_fEffectiveTaxRate - c_fMaxEffectiveTaxForNormalBirth, 2.0) + 1.0) : 1.0;
    GDZLOG(L"Tax rate " + GString::FormatNumber(l_fIncomeTax * 100.f, L"", L".", L"", L"%", 3, 1, false) + L", HDI " + GString::FormatNumber(l_fHdi, 3) + L", tax scaling " + GString::FormatNumber(l_fTaxScaling, 3),
           EDZLogLevel::Info2);
    l_fExpected *= static_cast<REAL32>(l_fTaxScaling);

    gassert(0.f <= l_fExpected, L"Expected birth rate " + GString::FormatNumber(l_fExpected * 100.f, L"", L".", L"", L"%", 3, 1, false) + L" below 0.0%");

    GDZLOG(L"Expected BR " + GString::FormatNumber(l_fExpected * 100.f, L"", L".", L"", L"%", 3, 1, false),
           EDZLogLevel::Info2);

	m_CountryData->BirthRateExpected(l_fExpected);
}
void GWorldBehavior::Iterate_Death_Rate_Expected()
{
	const REAL32 c_fMaxDeathRate = 0.035f;
	const REAL32 c_fMinDeathRate = 0.0015f;

	const REAL32 PourcHealthCare			= 0.4f;
	const REAL32 PourcHumanDevelopment	= 0.3f;
	const REAL32 PourcGdpPop				= 0.3f;

	REAL32 l_fHealthCare = 1.f - (REAL32)m_CountryData->BudgetExpenseHealthcareRatio();
	REAL32 l_fHumanDevelopment = 1.f - FindHumanDevelopmentFactor(m_CountryData->CountryID());
	REAL32 l_fGdpPop = 1.f - FindGDPPopulationFactor(m_CountryData->CountryID());

	REAL32 l_fResults = (PourcHealthCare * l_fHealthCare) 
		+ (PourcHumanDevelopment * l_fHumanDevelopment)
		+ (PourcGdpPop * l_fGdpPop);

	//Now, bring the value between 0 and 1 to between MinBirthRate & MaxBirthRate
	REAL32 l_fExpected = c_fMinDeathRate + ((c_fMaxDeathRate - c_fMinDeathRate) * l_fResults);
	m_CountryData->DeathRateExpected(l_fExpected);
}

bool GWorldBehavior::Iterate_Human_Development()
{
	/*!
	* Humand Development depends on 3 things
	*	Demographic financing, gvt stability, and primary & tertiary resources
	**/	
	//! \todo Do the secondary factor method

    // Now find out how much HI and EI are rising or falling.
	const REAL32 PourcDemographic			= 0.4f;
	const REAL32 PourcResources			= 0.35f;
	const REAL32 PourcStability			= 0.25f;

	REAL32 l_fDemographic = FindDemographicFinancingFactor(m_CountryData->CountryID());	
	REAL32 l_fStability = m_CountryData->GvtStability(); 
	REAL32 l_fResources = FindResourceFactor(m_CountryData->CountryID());	

	REAL32 l_fResults = (PourcDemographic * l_fDemographic) + (PourcResources * l_fResources)
		+ (PourcStability * l_fStability);

	if(m_CountryData->InternalLaw(EInternalLaws::ChildLabor))
		l_fResults -= 0.05f;

	//Bonus from treaties
	l_fResults += g_ServerDAL.HumanDevelopmentBonus(m_CountryData->CountryID());

    //Apply!
    REAL32 l_fLifeExpectancy = m_CountryData->LifeExpectancy();
    REAL32 l_fMeanYearsSchooling = m_CountryData->MeanYearsSchooling();
    REAL32 l_fExpectedYearsSchooling = m_CountryData->ExpectedYearsSchooling();

    {
        REAL32 l_fLEGain = -0.6f * powf(min(l_fResults, 13.f/12.f) - 13.f/12.f, 2) + 97.f/240.f;
        l_fLEGain += 0.1f * (1.f - FindHumanDevelopmentFactor(m_CountryData->CountryID()));
        l_fLifeExpectancy += l_fLEGain * m_fFrequency;
    }

    l_fMeanYearsSchooling += (l_fResults - 0.5f) * 0.4f * m_fFrequency;

    {
        REAL32 l_fEYSGain = (l_fResults - 0.5f) * 0.6f;
        l_fEYSGain += 0.1f * (1.f - FindHumanDevelopmentFactor(m_CountryData->CountryID()));
        l_fExpectedYearsSchooling += l_fEYSGain * m_fFrequency;
    }

    l_fLifeExpectancy = max(0.f, l_fLifeExpectancy);
    l_fMeanYearsSchooling = max(0.f, l_fMeanYearsSchooling);
    l_fExpectedYearsSchooling = max(0.f, l_fExpectedYearsSchooling);

    m_CountryData->LifeExpectancy(l_fLifeExpectancy);
    m_CountryData->MeanYearsSchooling(l_fMeanYearsSchooling);
    m_CountryData->ExpectedYearsSchooling(l_fExpectedYearsSchooling);

    REAL32 l_fNewHDLevel = GCountryData::FindHumanDevelopment(l_fLifeExpectancy, l_fMeanYearsSchooling, l_fExpectedYearsSchooling, m_CountryData->GDPPerCapita(), true);
	m_CountryData->HumanDevelopment(l_fNewHDLevel);

    if(g_SP2Server->ShowHDIComponents())
    {
        m_CountryData->ArableLandLevel(l_fNewHDLevel);
        m_CountryData->ForestLandLevel(l_fLifeExpectancy / 100.f);
        m_CountryData->ParksLandLevel(l_fMeanYearsSchooling / 100.f);
        m_CountryData->NotUsedLandLevel(l_fExpectedYearsSchooling / 100.f);
    }

	return true;
}

void GWorldBehavior::Iterate_Gvt_Stability_Expected()
{
	/*!
	*	Gvt Stability depends on 5 things:
	*	human development, current wars, resources, and gdp/population
	**/

	const REAL32 PourResources				= 0.30f;
	const REAL32 PourcWars					= 0.20f;	
	const REAL32 PourcHumanDevelopment	= 0.30f;	
	const REAL32 PourcGdpPop				= 0.2f;
	const REAL32 PourcReligions			= 0.1f;
	const REAL32 PourcLanguages			= 0.1f;

	REAL32 l_fResources						= 0.f;
	REAL32 l_fWars								= 0.f;
	REAL32 l_fHumanDevelopment				= 0.f;	
	REAL32 l_fGdpPop							= 0.f;
	REAL32 l_fReligions						= 0.f;
	REAL32 l_fLanguages						= 0.f;

	if(!g_ServerDAL.IsAtWar(m_CountryData->CountryID()))
		l_fWars = 1.f;

	l_fResources = FindResourceFactor(m_CountryData->CountryID());

	l_fGdpPop = FindGDPPopulationFactor(m_CountryData->CountryID());

	l_fHumanDevelopment = FindHumanDevelopmentFactor(m_CountryData->CountryID());	

	//For religions, find the most popular religion in the country;
	REAL32 l_fMaxReligionPourc = -1.f;
	const GReligionList& l_Religions = m_CountryData->GetReligions();
	for(GReligionList::const_iterator l_Itr = l_Religions.begin();
		l_Itr != l_Religions.end();
		l_Itr++)
	{
		if(m_CountryData->ReligionGetPourcentage(l_Itr->first) > l_fMaxReligionPourc)
			l_fMaxReligionPourc = m_CountryData->ReligionGetPourcentage(l_Itr->first);
	}
	//then, calculate its value between 0 and 1
	if(m_CountryData->GvtType() == EGovernmentType::Theocracy)
		l_fReligions = ((exp(3.5f * l_fMaxReligionPourc) - 30.f) /
		(exp(3.5f * l_fMaxReligionPourc) + 30.f))  + 0.95f;
	else
		l_fReligions = l_fHumanDevelopment + ( (1.f-l_fHumanDevelopment) * l_fMaxReligionPourc );

	//For languages, find the most popular Language in the country;
	REAL32 l_fMaxLanguagePourc = -1.f;
	const GLanguageList& l_Languages = m_CountryData->GetLanguages();
	for(GLanguageList::const_iterator l_Itr = l_Languages.begin();
		l_Itr != l_Languages.end();
		l_Itr++)
	{
		if(m_CountryData->LanguageGetPourcentage(l_Itr->first) > l_fMaxLanguagePourc)
			l_fMaxLanguagePourc = m_CountryData->LanguageGetPourcentage(l_Itr->first);
	}
	//then, calculate its value between 0 and 1
	l_fLanguages = l_fHumanDevelopment + ( (1.f-l_fHumanDevelopment) * l_fMaxLanguagePourc );

	REAL32 l_fModReligionsPlus = 1.f;
	REAL32 l_fModReligionsMinus = 1.f;
	if(m_CountryData->GvtType() == EGovernmentType::Theocracy)
	{
		l_fModReligionsPlus = 2.f;
		l_fModReligionsMinus = 0.5f;
	}

	REAL32 l_fResults = (l_fResources * PourResources) + (l_fHumanDevelopment * PourcHumanDevelopment) 
		+ (l_fGdpPop * PourcGdpPop * l_fModReligionsMinus) + (l_fReligions * PourcReligions * l_fModReligionsPlus)
		+ (l_fLanguages * PourcLanguages) + (l_fWars * PourcWars);

	l_fResults -= 0.1f;

	if (l_fResults > 1.f)
		l_fResults = 1.f;
	else if (l_fResults < 0.f)
		l_fResults = 0.f;

	if (m_CountryData->MartialLaw())
	{
		l_fResults *= 0.9f;
	}

	//Adjust with the personal income tax
	if(m_CountryData->PersonalIncomeTax() > 0.4f)
	{
		REAL64 l_fAdjustment = 1.f - ((m_CountryData->PersonalIncomeTax() * m_CountryData->PersonalIncomeTax()) - 0.16f);
		l_fResults *= (REAL32)l_fAdjustment;
	}
	

	m_CountryData->GvtStabilityExpected(l_fResults);

}

void GWorldBehavior::Iterate_Emigration_Level_Expected()
{
	const REAL32 LowestEmigration = 0.0005f;
	const REAL32 HighestEmigration = 0.02f;

	const REAL32 PourcStability			= 0.3f;
	const REAL32 PourcApproval	         = 0.3f;	
	const REAL32 PourcStabilityAvg		= 0.2f;
	const REAL32 PourcApprovalAvg 		= 0.2f;

	REAL32 l_fExpected = 0.f;

	INT32  l_iGroup = m_CountryData->CountryGroup();
	REAL32 l_fGvtStability = m_CountryData->GvtStability();
	REAL32 l_fGvtApproval = m_CountryData->GvtApproval();
	REAL32 l_fAverageStability = 0.0f;
	REAL32 l_fAverageApproval = 0.0f;
	REAL32 l_fAverageStabilityResult = 0.0f;
	REAL32 l_fAverageApprovalResult = 0.0f;	

	//now, we calculate the average gvt stability and gvt approval for countries of this group
	INT32 l_iNbCountries = 0;	
	for(UINT32 i=1; (INT16)i <= g_ServerDAL.NbCountry(); i++)
	{
		if (g_ServerDAL.CountryData(i)->CountryGroup() == l_iGroup)
		{
			l_fAverageStability += g_ServerDAL.CountryData(i)->GvtStability();
			l_fAverageApproval += g_ServerDAL.CountryData(i)->GvtApproval();
			l_iNbCountries++;
		}
	}

	if (l_iNbCountries > 0)
	{
		l_fAverageApproval /= l_iNbCountries;
		l_fAverageStability /= l_iNbCountries;
	}

	//If the average group stability/approval is over .5, 0
	//If the average group stability/approval is below -.5, 1
	//Else between 0 and 1
	if( (l_fAverageApproval-l_fGvtApproval) < -0.5f )
		l_fAverageApprovalResult = 1.f;
	else if( (l_fAverageApproval - l_fGvtApproval) > 0.5f )
		l_fAverageApprovalResult = 0.f;
	else
		l_fAverageApprovalResult =  - ((l_fAverageApproval-l_fGvtApproval) - 0.5f);

	if( (l_fAverageStability-l_fGvtStability) < -0.5f )
		l_fAverageStabilityResult = 1.f;
	else if( (l_fAverageStability - l_fGvtStability) > 0.5f )
		l_fAverageStabilityResult = 0.f;
	else
		l_fAverageStabilityResult =  - ((l_fAverageStability-l_fGvtStability) - 0.5f);


	REAL32 l_fResults = (PourcStability * l_fGvtStability) + (PourcApproval * l_fGvtApproval)
		+ (PourcStabilityAvg * l_fAverageStabilityResult) + (PourcApprovalAvg * l_fAverageApprovalResult);

	if(l_fResults > 1.f)
		l_fResults = 1.f;
	else if (l_fResults < 0.f)
		l_fResults = 0.f;


	// Higher l_fResults means lower emigration
	l_fExpected = LowestEmigration + ((1.f - l_fResults) * (HighestEmigration - LowestEmigration));

    // Clamp between 0.f and 1.f once more, or else GCountryDataItf::EmigrationLevelExpected() may hit an assertion
    l_fExpected = max(0.f, min(l_fExpected, 1.f));

	m_CountryData->EmigrationLevelExpected(l_fExpected);

}

void GWorldBehavior::Iterate_Corruption_Expected()
{
	/*!
	*	Corruption level depends on 4 things:
	*	human development, financing to cancel corruption, imf/gdp, and minimum wage
	**/

	const REAL32 PourcCorruptionFunding	= 0.6f;
	const REAL32 PourcHumanDevelopment	= 0.35f;	
	const REAL32 PourcImfGdp				= 0.1f;
	const REAL32 PourcIncomeTax			= 0.45f;

	REAL32 l_fCorruptionFunding = 1.f - (REAL32)m_CountryData->BudgetExpenseGovernmentRatio();;
	REAL32 l_fHumanDevelopment = 1.f - FindHumanDevelopmentFactor(m_CountryData->CountryID());
	REAL32 l_fImfGdp = 1.f - FindIMFReceivingFactor(m_CountryData->CountryID());
	REAL32 l_fPersonalIncomeTax = 1.f - FindTaxBudgetFactor(m_CountryData->CountryID());

	REAL32 l_fResults = (l_fCorruptionFunding * PourcCorruptionFunding) + (l_fImfGdp * PourcImfGdp)
		+ (l_fHumanDevelopment * PourcHumanDevelopment)
		+ (l_fPersonalIncomeTax * PourcIncomeTax);
	
	if(l_fResults <= 0.25f)
		l_fResults = 0.f;
	else if(l_fResults > 1.25f)
		l_fResults = 1.f;
	else
		l_fResults = l_fResults - 25.f / 100.f;

	m_CountryData->CorruptionLevelExpected(l_fResults);

}

bool GWorldBehavior::Iterate_Pop_In_Poverty()
{
	const REAL32 PourcAverageEconomy	= 0.3f;
	const REAL32 PourcEconomicHealth	= 0.7f;	

	REAL32 l_fAverageEconomy = 0.f;
	REAL32 l_fEconomicHealth = 0.f;
	REAL32 l_fResults = 0.f;
	
	//The poverty is calculated compared to the stronger economy, and with the economic health
	REAL64 l_fAverageGDPPerCapita = 0.f;
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	UINT32 l_iNbActiveCountries = 0;
	//Find strongest economy
	for(UINT32 i=1; i<= l_iNbCountries; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;

		l_fAverageGDPPerCapita += l_pCountryData->GDPValue() / (REAL64)( max(1,l_pCountryData->Pop1565()) );
		l_iNbActiveCountries++;
	}

	if(l_iNbActiveCountries == 0)
		return false;

	l_fAverageGDPPerCapita /= (REAL64)l_iNbActiveCountries;

	if(( m_CountryData->GDPValue() / (REAL64)( max(1,m_CountryData->Pop1565()) ) ) > l_fAverageGDPPerCapita)
		l_fAverageEconomy = 0.f;
	else
		l_fAverageEconomy = 1.f;

	l_fEconomicHealth = (((1.f-m_CountryData->EconomicHealth()) * m_CountryData->CountryEconomicModel()) * 2.f ) - 1.f;
	
	l_fResults = (l_fEconomicHealth * PourcEconomicHealth) + (l_fAverageEconomy * PourcAverageEconomy);

	l_fResults = 1.f + ( ((l_fResults * 2.f) - 1.f) / 100.f);

	REAL32 l_fNewPoverty = m_CountryData->PopInPoverty();
	l_fNewPoverty += (((l_fNewPoverty * l_fResults) - l_fNewPoverty) * m_fFrequency);	
	
	if(l_fNewPoverty > 0.9f)
		l_fNewPoverty = 0.9f;
	else if(l_fNewPoverty < 0.02f)
		l_fNewPoverty = 0.02f;

	m_CountryData->PopInPoverty(l_fNewPoverty);

	return true;
}

bool GWorldBehavior::Iterate_Pop_Economic_Model()
{
	/*!
	*	We will move towards the economic ideology 2% closer per year.
	**/

	const REAL32 PopEconomicIdeologyAdv = 0.02f;

	REAL32 l_fPopEconomicIdeology = m_CountryData->PopEconomicModel();
	REAL32 l_fGvtEconomicIdeology = m_CountryData->CountryEconomicModel();

	if (l_fPopEconomicIdeology < l_fGvtEconomicIdeology)
		l_fPopEconomicIdeology += PopEconomicIdeologyAdv * m_fFrequency;
	else if (l_fPopEconomicIdeology > l_fGvtEconomicIdeology)
		l_fPopEconomicIdeology -= PopEconomicIdeologyAdv * m_fFrequency;

	if(l_fPopEconomicIdeology > 1.f)
		l_fPopEconomicIdeology = 1.f;
	else if (l_fPopEconomicIdeology < 0.f)
		l_fPopEconomicIdeology = 0.f;

	m_CountryData->PopEconomicModel(l_fPopEconomicIdeology);

	return true;
}

bool GWorldBehavior::Iterate_Pop_Political_Philosophy()
{
	/*!
	*	We will move towards the political ideology 2% closer per year.
	**/

	const REAL32 PopPoliticalIdeologyAdv = 0.02f;

	REAL32 l_fPopPoliticalIdeology = m_CountryData->PopPoliticalIdeology();
	REAL32 l_fGvtPoliticalIdeology = m_CountryData->CountryPoliticalIdeology();

	if (l_fPopPoliticalIdeology < l_fGvtPoliticalIdeology)
		l_fPopPoliticalIdeology += PopPoliticalIdeologyAdv * m_fFrequency;
	else if (l_fPopPoliticalIdeology > l_fGvtPoliticalIdeology)
		l_fPopPoliticalIdeology -= PopPoliticalIdeologyAdv * m_fFrequency;

	if(l_fPopPoliticalIdeology > 1.f)
		l_fPopPoliticalIdeology = 1.f;
	else if (l_fPopPoliticalIdeology < 0.f)
		l_fPopPoliticalIdeology = 0.f;

	m_CountryData->PopPoliticalIdeology(l_fPopPoliticalIdeology);

	return true;
}

/*!
* Go through the covert actions cells list of a country, and calculate its unit upkeep
**/
bool GWorldBehavior::Iterate_Budget_Expense_Security()
{	
	g_ServerDCL.IterateBudgetExpenseSecurity(m_CountryData->CountryID());	
	return true;
}

/*!
* Go through the unit group list of a country, and calculate its unit upkeep
* Fortified and Ready units: 50% of value per year
* Parked units: 10% of value per year
**/
bool GWorldBehavior::Iterate_Budget_Expense_Unit_Upkeep()
{	
	g_ServerDCL.IterateBudgetUnitUpkeep(m_CountryData->CountryID());
	return true;
}

bool GWorldBehavior::Iterate_Budget_Revenue_Tax()
{
	g_ServerDCL.IterateBudgetRevenueTax(m_CountryData->CountryID());
	return true;
}

//! iterates a country and changes all data that are affected normally by other data
bool GWorldBehavior::CountryIterate(INT16 in_iCountryID)
{	
	if(!m_bInit)
		return false;	

	if(in_iCountryID < 1)
		return false;   				

	m_CountryData = g_ServerDAL.CountryData(in_iCountryID);		

	if(!m_CountryData->Activated()) 
		return false;
	else
	{
		//Check if the countr is really activated
		const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
		const set<UINT32>& l_MilitaryRegions = g_ServerDAL.CountryMilitaryControl(in_iCountryID);
		if(l_PoliticalRegions.size() == 0 &&
			l_MilitaryRegions.size() == 0)
		{
			g_ServerDAL.DestroyCountryEntity(in_iCountryID,0);
			return false;
		}
	}
	

	REAL64 l_fGameTime = g_Joshua.GameTime();
	m_fFrequency = (REAL32)((l_fGameTime - m_CountryData->LastIteration()) / 365.0f);	

	//log the current data for this country
	GString l_sCSV(in_iCountryID);		

	l_sCSV += L"," + GString(m_CountryData->LastIteration()) + \
		L"," + GString(m_CountryData->AMDSLevel()) + \
		L"," + GString(m_CountryData->BudgetRevenues() - m_CountryData->BudgetExpenses()) + \
		L"," + GString(m_CountryData->BudgetExpenseDebt()) + \
		L"," + GString(m_CountryData->BudgetExpenseEducation()) + \
		L"," + GString(m_CountryData->BudgetExpenseEnvironment()) + \
		L"," + GString(m_CountryData->BudgetExpenseGovernment()) + \
		L"," + GString(m_CountryData->BudgetExpenseHealthcare()) + \
		L"," + GString(m_CountryData->BudgetExpenseIMF()) + \
		L"," + GString(m_CountryData->BudgetExpenseImports()) + \
		L"," + GString(m_CountryData->BudgetExpenseInfrastructure()) + \
		L"," + GString(m_CountryData->BudgetExpenseDiplomacy()) + \
		L"," + GString(m_CountryData->BudgetExpenseResearch()) + \
		L"," + GString(m_CountryData->BudgetExpenseSecurity()) + \
		L"," + GString(m_CountryData->BudgetExpenseTelecom()) + \
		L"," + GString(m_CountryData->BudgetExpenseTourism()) + \
		L"," + GString(m_CountryData->BudgetExpenseUnitUpkeep()) + \
		L"," + GString(m_CountryData->BudgetExpensePropaganda()) + \
		L"," + GString(m_CountryData->BudgetExpenseCorruption()) + \
		L"," + GString(m_CountryData->BudgetRevenueTrade()) + \
		L"," + GString(m_CountryData->BudgetRevenueIMF()) + \
		L"," + GString(m_CountryData->BudgetRevenueTax()) + \
		L"," + GString(m_CountryData->BudgetRevenueTourism()) + \
		L"," + GString(m_CountryData->CorruptionLevel()) + \
		L"," + GString(m_CountryData->EconomicRank()) + \
		L"," + GString(m_CountryData->EmigrationLevel()) + \
		L"," + GString(m_CountryData->GvtType()) + \
		L"," + GString(m_CountryData->GDPValue()) + \
		L"," + GString(m_CountryData->GDPGrowth()) + \
		L"," + GString(m_CountryData->GrowthRate()) + \
		L"," + GString(m_CountryData->BirthRate()) + \
		L"," + GString(m_CountryData->DeathRate()) + \
		L"," + GString(m_CountryData->GvtApproval()) + \
		L"," + GString(m_CountryData->GvtStability()) + \
		L"," + GString(m_CountryData->HumanDevelopment()) + \
		L"," + GString(m_CountryData->ImmigrationLevel()) + \
		L"," + GString(m_CountryData->InflationLevel()) + \
		L"," + GString(m_CountryData->InflationLevelExpected()) + \
		L"," + GString(m_CountryData->Infrastructure()) + \
		L"," + GString(m_CountryData->InterestLevel()) + \
		L"," + GString(m_CountryData->MiliManpowerAvailable()) + \
		L"," + GString(m_CountryData->MilitaryRank()) + \
		L"," + GString(m_CountryData->PersonalIncomeTax()) + \
		L"," + GString(m_CountryData->PoliticRank()) + \
		L"," + GString(m_CountryData->Population()) + \
		L"," + GString(m_CountryData->Pop15()) + \
		L"," + GString(m_CountryData->Pop1565()) + \
		L"," + GString(m_CountryData->Pop65()) + \
		L"," + GString(m_CountryData->Pop1565Unemployed()) + \
		L"," + GString(m_CountryData->PopInPoverty()) + \
		L"," + GString(m_CountryData->PopPoliticalIdeology()) + \
		L"," + GString(m_CountryData->PopEconomicModel()) + \
		L"," + GString(m_CountryData->EconomicHealth()) + \
		L"," + GString(m_CountryData->TelecomLevel());
	for(UINT32 i=0; i < EResources::ItemCount; i++)
	{
		l_sCSV += L"," + GString(m_CountryData->ResourceDemand((EResources::Enum)i));
		l_sCSV += L"," + GString(m_CountryData->ResourceProduction((EResources::Enum)i));
	}

#ifdef GOLEM_DEBUG
	m_pLogger->Log(MSGTYPE_CSV,l_sCSV);	
#endif

	//Iterate new value for regions
	const set<UINT32>& l_vRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	GRegion* l_pRegion;	

	REAL64 l_fOldGDP = m_CountryData->GDPValue();

	Iterate_Infrastructure_Expected();
	Iterate_Telecom_Expected();
	INT64 l_iFinalPopulation = 0;

	for(set<UINT32>::const_iterator l_RegionItr = l_vRegions.begin();
		l_RegionItr != l_vRegions.end();
		l_RegionItr++)
	{
		l_pRegion = g_ServerDAL.GetGRegion(*l_RegionItr);
		if (! l_pRegion) 
		{
			g_Joshua.Log(L"Invalid region!",MSGTYPE_ERROR);
			return false;
		}		

		//! \todo Regions under military control don't grow as well as ordinary regions
		INT64 l_iOldPopulation = l_pRegion->Population();
		Iterate_Population(l_pRegion);
		REAL32 l_fPopRatio = 0.f;
		if(l_iOldPopulation > 0)
			l_fPopRatio = 1.f + ((REAL32)(l_pRegion->Population() - l_iOldPopulation) / (REAL32)l_iOldPopulation);


		for(UINT32 i=0; i<EResources::ItemCount; i++)
		{
			//Adjust demand for that region
			//l_pRegion->ResourceDemand((EResources::Enum)i,l_pRegion->ResourceDemand((EResources::Enum)i)*l_fPopRatio);
			//If the country is losing people, it will lose production
			if(l_fPopRatio < 1.f)
				l_pRegion->ResourceProduction((EResources::Enum)i,l_pRegion->ResourceProduction((EResources::Enum)i)*l_fPopRatio);
		}

		Iterate_Telecom_Level(l_pRegion,l_fGameTime);
		Iterate_Infrastructure(l_pRegion,l_fGameTime);
		//Iterate_Demand(l_pRegion);		
		Iterate_Tourism(l_pRegion);

		if(l_pRegion->OwnerId() == l_pRegion->OwnerMilitaryId())
			l_iFinalPopulation += l_pRegion->Population();

	}

	m_CountryData->PopulationPoliticalControl(l_iFinalPopulation);
	m_CountryData->IterateDemand();

	for(set<UINT32>::const_iterator l_RegionItr = l_vRegions.begin();
		l_RegionItr != l_vRegions.end();
		l_RegionItr++)
	{
		Iterate_Production(g_ServerDAL.GetGRegion(*l_RegionItr));
	}

	m_CountryData->SynchronizeWithRegions();	

	REAL64 l_fNewGDP = m_CountryData->GDPValue();
	REAL32 l_fNewGDPGrowth = 0.f;
	if(l_fOldGDP > 0.f && m_fFrequency > 0.f)
		l_fNewGDPGrowth = (REAL32)( (l_fNewGDP - l_fOldGDP) / l_fOldGDP ) * (1.f/m_fFrequency);

	m_CountryData->GDPGrowth(l_fNewGDPGrowth);

	// figures out by themselves
	//Iterate_Available_Money();      	 
	Iterate_Resources_Gdp();

	// complex
	Iterate_Political_Ideology();
	Iterate_Economic_Model();
	Iterate_Political_Party_Popularity();

	// behavior data
	Iterate_Pop_15_65_Unemployed();
	Iterate_Human_Development();   
	Iterate_Pop_In_Poverty();

	Iterate_Pop_Economic_Model();
	Iterate_Pop_Political_Philosophy();

	Iterate_Corruption_Expected();
	Iterate_Gvt_Stability_Expected();
	Iterate_Gvt_Approval_Expected();	
	Iterate_Birth_Rate_Expected();
	Iterate_Death_Rate_Expected();
	Iterate_Emigration_Level_Expected();
	Iterate_Inflation_Level_Expected();  

	// budget & resource
	Iterate_Budget_Expense_Security();
	Iterate_Budget_Expense_Unit_Upkeep();
	Iterate_Budget_Revenue_Tax();

	//Iterate economic health
	m_CountryData->EconomicHealth(FindEconomicHealth(m_CountryData->CountryID()));

	m_CountryData->LastIteration(l_fGameTime);

	//Calculate the overall relations of that country
	m_CountryData->OverallDiplomaticRelations( g_ServerDAL.OverallRelations(m_CountryData->CountryID()) );	

   // Verify economic failure, in a economic failure if, non human controlled country, this country is bankrupt.
   if(VerifyEconomicFailure(in_iCountryID) == true)
   {

      //SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iCountryID);
      //if(l_pPlayer && l_pPlayer->AIControlled())
      {
         BankruptCountry(in_iCountryID);
      }
   }

	return true;
}

bool GWorldBehavior::VerifyEconomicFailure(INT16 in_iCountryID)
{

   //Fetch country data
   GCountryData* l_pData = g_ServerDAL.CountryData(in_iCountryID);
   gassert(l_pData,"Invalid country data (NULL)");

   //Economic failure is when your expenses on your debt are higher than your revenues
   REAL64 l_fDebtExpenses = l_pData->BudgetExpenseDebt();

	SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iCountryID);
   if(l_pPlayer && l_pPlayer->AIControlled())
		l_fDebtExpenses /= 10.f;

   REAL64 l_fRevenues = g_ServerDCL.GetRevenuesForEconomicFailure(in_iCountryID);
   if(l_fDebtExpenses > l_fRevenues)
   {
        if(g_SP2Server->LogBankruptcies())
            g_Joshua.Log(L"The economy of " + l_pData->NameAndIDForLog() +
                         L" has failed with GDP " +
                         GString::FormatNumber(l_pData->GDPValue(), L",", L".", L"$", L"", 3) +
                         L" and debt " +
                         GString::FormatNumber(l_pData->BudgetExpenseDebt(), L",", L".", L"$", L"", 3));
        return true;
   }
   else
      return false;
}

void GWorldBehavior::BankruptCountry(INT16 in_iCountryID)
{

   GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
   gassert(l_pCountryData,"Invalid country data (NULL)");

   // From Design , when a country is bankrupt do the following :
   // 1. Reduce all the Budget Spendings to 25% (half of the needed spendings), except for Research and IMF Funding which should receive no funding at all.
   l_pCountryData->BudgetExpenseEducationRatio(0.25f);
   l_pCountryData->BudgetExpenseEnvironmentRatio(0.25f);
   l_pCountryData->BudgetExpenseGovernmentRatio(0.25f);
   l_pCountryData->BudgetExpenseHealthcareRatio(0.25f);
   l_pCountryData->BudgetExpenseIMFRatio(0.0f);
   l_pCountryData->BudgetExpenseInfrastructureRatio(0.25f);
   l_pCountryData->BudgetExpensePropagandaRatio(0.25f);
   l_pCountryData->BudgetExpenseResearchRatio(0.0f);
   l_pCountryData->BudgetExpenseTelecomRatio(0.25f);
   l_pCountryData->BudgetExpenseTourismRatio(0.25f);

   // 2. Disband all the military arsenal but infantry
   set<UINT32> l_UnitIDToDisband;
   const set<UINT32>& l_UnitGroup = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);
   set<UINT32>::const_iterator l_UnitGroupIterator = l_UnitGroup.begin();
   
   while(l_UnitGroupIterator != l_UnitGroup.end())
   {
      SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIterator);
      for(UINT i=0;i<l_pUnitGroup->Units().size();i++)
      {
         SDK::Combat::GUnit* l_pUnit = l_pUnitGroup->Units()[i];
         if(l_pUnit->Design()->Id() != 1)
         {
            l_UnitIDToDisband.insert(l_pUnit->Id());
         }

      }

      l_UnitGroupIterator++;
   }

   // Disband!!!
   set<UINT32>::iterator l_UnitIDIterator = l_UnitIDToDisband.begin();
   while(l_UnitIDIterator != l_UnitIDToDisband.end())
   {
      g_ServerDCL.DisbandUnits(in_iCountryID,g_Joshua.UnitManager().Unit(*l_UnitIDIterator));
      l_UnitIDIterator++;
   }

   // 3. Remove all covert action cells
   // For now, disband everything
   l_pCountryData->CovertActionCells().clear();

   // 4. Reduce Resource Production:
   // Iterate over regions
   const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
   set<UINT32>::const_iterator l_iRegionIterator =l_PoliticalRegions.begin();
   while(l_iRegionIterator != l_PoliticalRegions.end())
   {
      // for each region, change production
      GRegion *l_pRegion = g_ServerDAL.GetGRegion(*l_iRegionIterator);

      // Industrial Materials and Finished Goods : 0.20 of original value;
      l_pRegion->ResourceProduction(EResources::Appliances,l_pRegion->ResourceProduction(EResources::Appliances) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Chemicals,l_pRegion->ResourceProduction(EResources::Chemicals) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Drugs,l_pRegion->ResourceProduction(EResources::Drugs) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Fabrics,l_pRegion->ResourceProduction(EResources::Fabrics) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Luxury_Commodities,l_pRegion->ResourceProduction(EResources::Luxury_Commodities) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Machinery,l_pRegion->ResourceProduction(EResources::Machinery) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Pharmaceuticals,l_pRegion->ResourceProduction(EResources::Pharmaceuticals) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Plastics,l_pRegion->ResourceProduction(EResources::Plastics) * 0.20f);
      l_pRegion->ResourceProduction(EResources::Vehicules,l_pRegion->ResourceProduction(EResources::Vehicules) * 0.20f);
      
      // Food & Agriculture, Raw Materials and Energy : 0.50 of the original value;
      l_pRegion->ResourceProduction(EResources::Cereals,l_pRegion->ResourceProduction(EResources::Cereals) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Dairy,l_pRegion->ResourceProduction(EResources::Dairy) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Fossile_Fuels,l_pRegion->ResourceProduction(EResources::Fossile_Fuels) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Iron_Steel,l_pRegion->ResourceProduction(EResources::Iron_Steel) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Meat,l_pRegion->ResourceProduction(EResources::Meat) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Minerals,l_pRegion->ResourceProduction(EResources::Minerals) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Precious_Stones,l_pRegion->ResourceProduction(EResources::Precious_Stones) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Tobacco,l_pRegion->ResourceProduction(EResources::Tobacco) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Vegetable_Fruits,l_pRegion->ResourceProduction(EResources::Vegetable_Fruits) * 0.50f);
      l_pRegion->ResourceProduction(EResources::Wood_Paper,l_pRegion->ResourceProduction(EResources::Wood_Paper) * 0.50f);
   
      // Services : 0.10 of original value.
      l_pRegion->ResourceProduction(EResources::Commodities,l_pRegion->ResourceProduction(EResources::Commodities) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Construction,l_pRegion->ResourceProduction(EResources::Construction) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Electricity,l_pRegion->ResourceProduction(EResources::Electricity) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Engineering,l_pRegion->ResourceProduction(EResources::Engineering) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Health_Care,l_pRegion->ResourceProduction(EResources::Health_Care) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Legal_Services,l_pRegion->ResourceProduction(EResources::Legal_Services) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Marketing_Advertising,l_pRegion->ResourceProduction(EResources::Marketing_Advertising) * 0.10f);
      l_pRegion->ResourceProduction(EResources::Retail,l_pRegion->ResourceProduction(EResources::Retail) * 0.10f);

      l_iRegionIterator++;
   }

   // 5. Quit all Economic Aid treaties in which the country pays to help others
   //    Also free all clients
   const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();
   hash_map<UINT32, GTreaty>::const_iterator l_treatyIt = l_Treaties.begin();
   while(l_treatyIt != l_Treaties.end())
   {
      const GTreaty& l_Treaty = l_treatyIt->second;
      if(l_Treaty.CountrySide(in_iCountryID) == 1)
      {
          switch(l_Treaty.Type())
          {
          case ETreatyType::EconomicAid:
              g_ServerDCL.LeaveTreaty(in_iCountryID,l_treatyIt->first);
              break;
          case ETreatyType::MilitaryAccess:
              if(l_Treaty.Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0)
                  g_ServerDCL.LeaveTreaty(in_iCountryID,l_treatyIt->first,true);
              break;
          default:
              break;
          }
      }

      l_treatyIt++;
   }

   gassert(l_pCountryData->Clients().size() == 0,
           L"About-to-be-bankrupt " + l_pCountryData->NameAndIDForLog() +
           L" still has clients");

   // 6. Set the Personal Income Tax to 40%, interest rate to 2%, inflation to 2%
   l_pCountryData->PersonalIncomeTax(0.40f);
   l_pCountryData->InflationLevel(0.02f);
   l_pCountryData->InterestLevel(0.02f);

   // 7. Cancel Unit Production
   g_ServerDCL.ClearUnitProductionForCountry(in_iCountryID);

   // And the last, but not the least, the country's debt is then reduced to 0.
   l_pCountryData->MonetarySupplyAmount(0);
}

//! todo - parameter
bool GWorldBehavior::PerformActionConsequence()
{
	if(!m_bInit)
		return false;

	return true;
}

bool GWorldBehavior::GlobalIterate(REAL64 in_fGameTime)
{
	if(!m_bInit)
		return false;

	//Check for elections
	IterateForElections();
	

	if(in_fGameTime > m_fNextIteration)   
	{			

		IterateChangingVariables(in_fGameTime);

		//Iterate the verification of wars
		g_ServerDCL.IterateWars();

		//Send news of nukes and casualties every 2 days. 
		if (in_fGameTime >= m_fNextNewsIteration)
		{			
			g_ServerDCL.SendNewsNukes();
			g_ServerDCL.SendNewsCasualties();		

			//Also, iterate the loss of relations from pressure on treaties
			IterateLossRelationsFromPressure();

			//Iterate treaties suspension
			IterateTreatiesSuspension();

			//Iterate the research level of every country
			IterateNuclearResearchLevel(in_fGameTime);

			m_fNextNewsIteration = in_fGameTime + SP2::c_fIterationPeriodNukeNews;
			
		}	

		//we calculate again the economic and political and military ranks
		g_ServerDAL.CalculateCountryInfo();
		RandomEvents(m_iInternalCounter);

		if(m_iInternalCounter == 1)
		{
			//Check if we must clear the nuke history clock
			if(g_ServerDAL.LastCountryThatNuked())
			{
				
				GTimeSpan l_Span = g_ServerDAL.ActualDate() - g_ServerDAL.DateOfLastNuke();
				if(l_Span.Days() > SP2::c_fDaysBeforeResettingNukeHistory)
				{
					g_ServerDAL.LastCountryThatNuked(0);
				}
			}

			//Calculate the average human development level
			GCountryDataItf::m_fHumanDevelopmentAvg = g_ServerDCL.AverageHumanDevelopment();			

			REAL64 l_fLength = g_Joshua.TimeCounter().GetPrecisionTime();
			IterateGlobalMarket();
			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime() - l_fLength;
#ifdef GOLEM_DEBUG
			g_Joshua.Log(L"Length of global market is: " + GString(l_fLength));
#endif			

			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime();
			IterateIMF(in_fGameTime);
			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime() - l_fLength;
#ifdef GOLEM_DEBUG
			g_Joshua.Log(L"Length of IMF iteration: " + GString(l_fLength));
#endif
			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime();
			IterateBudget(in_fGameTime);
			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime() - l_fLength;
#ifdef GOLEM_DEBUG
			g_Joshua.Log(L"Length of Budget iteration: " + GString(l_fLength));
#endif			

			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime();
			//Iterate the new eligible countries for treaties
			const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
			for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
				it != l_Treaties.end();++ it)
			{
				g_ServerDCL.SetEligibleCountries(it->first, false);
			}
			l_fLength = g_Joshua.TimeCounter().GetPrecisionTime() - l_fLength;
#ifdef GOLEM_DEBUG
			g_Joshua.Log(L"Length of Finding Eligible countries: " + GString(l_fLength));
#endif			

			IterateEmigrationImmigration();

			g_ServerDAL.CalculateExpectedRelations();

			m_fLastBudgetIteration = in_fGameTime;
		}
		
		//Iterate budget upkeep for countries that have modified their units (Human controlled countries only)
		for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin() ;
       l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
       l_HumanPlayersIt++)
		{
			SDK::GPlayer* l_pPlayer = l_HumanPlayersIt->second;
			gassert(l_pPlayer->Client(),"Invalid client pointer");
			if(l_pPlayer->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE && 
				l_pPlayer->ModID() > 0)
				g_ServerDCL.IterateBudgetUnitUpkeep(l_pPlayer->ModID());
		}

      CountryIterate(m_iInternalCounter);				
		m_iInternalCounter++;

		if(m_iInternalCounter > g_ServerDAL.NbCountry() )
		{										
			m_iInternalCounter = 1;					
		}
		
		m_fNextIteration = in_fGameTime + 0.0705f;	//this number assures us that 1 iteration every 2 hours.
	}
	return true;
}

void GWorldBehavior::IterateNuclearResearchLevel(REAL64 in_fGameTime)
{
	REAL32 l_fTimeDifference = (REAL32)(in_fGameTime - m_fLastNukeReadyIteration);
	REAL32 l_fPointsToAdd = (REAL32)(l_fTimeDifference / SP2::c_fNuclearResearchTime);
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountries; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
        if(g_SP2Server->DisableNuclearOnOccupy()                       &&
           l_pCountryData->NumberOfPoliticallyControlledRegions() == 0)
            continue;
		REAL32 l_fLevel = l_pCountryData->NuclearReady();
		if(l_fLevel >= 0.f && l_fLevel < 1.f)
		{
			l_fLevel += l_fPointsToAdd;

			l_fLevel = min(1.f,l_fLevel);
			l_pCountryData->NuclearReady(l_fLevel);
		}
	}
	m_fLastNukeReadyIteration = in_fGameTime;
}

void GWorldBehavior::IterateEmigrationImmigration()
{
	const REAL32 MaxImmigrationLevel = 0.02f;

	//Countries with bad political rank will emigrate first 
	//AND
	//Calculate the nb of possible immigrants
	multimap<UINT32,GCountryData*> l_EmigrationCountries;
	multimap<UINT32,GCountryData*> l_ImmigrationCountries;
	hash_map<UINT32,INT64> l_EmigrantsByCountry;
	hash_map<UINT32,INT64> l_ImmigrantsByCountry;
	INT64 l_iNbOfImmigrantsPossible = 0;
	INT64 l_iEmigrants = 0;
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<=l_iNbCountries; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);

		if(l_pCountryData->Activated())
		{
			if(!l_pCountryData->ImmigrationClosed())
			{
				l_ImmigrationCountries.insert(make_pair<UINT32,GCountryData*>(l_pCountryData->PoliticRank(),l_pCountryData));
				l_iNbOfImmigrantsPossible += (INT64)((REAL32)l_pCountryData->Population() * MaxImmigrationLevel);
			}
			
			l_EmigrationCountries.insert(make_pair<UINT32,GCountryData*>(l_iNbCountries-l_pCountryData->PoliticRank(),l_pCountryData));			
		}

		
	}

	for(multimap<UINT32,GCountryData*>::iterator it = l_EmigrationCountries.begin();
		 it != l_EmigrationCountries.end() && l_iNbOfImmigrantsPossible > l_iEmigrants; it++)
	{
		INT64 l_iEmmigrantsWantedForThatCountry = (INT64)((REAL32)it->second->Population() * it->second->EmigrationLevel());
		INT64 l_iPossibleEmmigrants = l_iNbOfImmigrantsPossible - l_iEmigrants;
		INT64 l_iPeopleMovingToCountry = min(l_iPossibleEmmigrants,l_iEmmigrantsWantedForThatCountry);
		l_iEmigrants += l_iPeopleMovingToCountry;
	}
	
	for(multimap<UINT32,GCountryData*>::iterator it = l_ImmigrationCountries.begin();
		 it != l_ImmigrationCountries.end(); it++)
	{
		if(l_iEmigrants == 0)
		{
			it->second->ImmigrationLevel(0.f);
			continue;
		}
		INT64 l_iEmigrantsThatCountryCanHave = (INT64)((REAL32)it->second->Population() * MaxImmigrationLevel);
		INT64 l_iEmigrantsGoingToThatCountry = min(l_iEmigrantsThatCountryCanHave,l_iEmigrants);
		if(it->second->Population() > 0)
		{
			it->second->ImmigrationLevel( (REAL32)l_iEmigrantsGoingToThatCountry / (REAL32)it->second->Population() ); 
			l_iEmigrants -= l_iEmigrantsGoingToThatCountry;
		}		
	}
		
}

void GWorldBehavior::RandomEvents(UINT32 in_iCountryID)
{
	if(!g_ServerDAL.CountryIsValid(in_iCountryID))
		return;		

	REAL32 l_fDaysSpent = (REAL32)(g_Joshua.GameTime() - g_ServerDAL.CountryData(in_iCountryID)->LastIteration());

	REAL32 l_fWorldEnvironmentFunding = 1.f;
	//Find the average of environment funding in the world
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	REAL64 l_fAverageRatio = 0.f;
	UINT32 l_iNbCountryValid = 0;
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		if(!g_ServerDAL.CountryIsValid(i))
			continue;
		l_fAverageRatio += g_ServerDAL.CountryData(i)->BudgetExpenseEnvironmentRatio();
		l_iNbCountryValid++;
	}
	l_fAverageRatio = l_fAverageRatio / (REAL64)l_iNbCountryValid;
	l_fWorldEnvironmentFunding = (REAL32)((-1.9 * l_fAverageRatio) + 2.f);

	REAL32 l_fCountryEnvironmentFundingMod = 0.f;
	REAL64 l_fFundingEnvironment = g_ServerDAL.CountryData(in_iCountryID)->BudgetExpenseEnvironmentRatio();
	if(l_fFundingEnvironment >= 0.5f)
		l_fCountryEnvironmentFundingMod = (REAL32)(1.f - (((l_fFundingEnvironment-0.5f)/0.5f)*0.9));
	else
		l_fCountryEnvironmentFundingMod = (REAL32)((-18.f * l_fFundingEnvironment) + 10.f);


	const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	for(set<UINT32>::const_iterator it = l_PoliticalRegions.begin();
		it != l_PoliticalRegions.end(); it++)
	{
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
		for(UINT32 i=0; i<ERandomEvents::ItemCount; i++)
		{
			ERandomEvents::Enum l_Event = (ERandomEvents::Enum)i;
			if(i > ERandomEvents::ActsOfNatureBeginning && i < ERandomEvents::ActsOfNatureEnd)
			{
				if(GetRandom(1.f) < (l_pRegion->RandomEvent(l_Event) / 30.f * l_pRegion->PercentageValue() * l_fWorldEnvironmentFunding * l_fDaysSpent))
				{
					ExecuteRandomEvent(l_pRegion->Id(),l_Event);
					break;
				}
			}
			else if(i > ERandomEvents::EconomyBeginning && i < ERandomEvents::EconomyEnd)
			{
				if(GetRandom(1.f) < (l_pRegion->RandomEvent(l_Event) / 30.f * l_pRegion->PercentageValue() * l_fDaysSpent))
				{
					ExecuteRandomEvent(l_pRegion->Id(),l_Event);
					break;
				}
			}
			else if(i > ERandomEvents::DemographyBeginning && i < ERandomEvents::DemographyEnd)
			{
				if(GetRandom(1.f) < (l_pRegion->RandomEvent(l_Event) / 30.f * l_pRegion->PercentageValue() * l_fDaysSpent))
				{
					ExecuteRandomEvent(l_pRegion->Id(),l_Event);
					break;
				}
			}
			else if(i > ERandomEvents::EnvironmentBeginning && i < ERandomEvents::EnvironmentEnd)
			{
				if(GetRandom(1.f) < (l_pRegion->RandomEvent(l_Event) / 30.f * l_pRegion->PercentageValue() * l_fCountryEnvironmentFundingMod * l_fDaysSpent))
				{
					ExecuteRandomEvent(l_pRegion->Id(),l_Event);
					break;
				}
			}
			else if(i > ERandomEvents::PoliticsBeginning && i < ERandomEvents::PoliticsEnd)
			{
				if(GetRandom(1.f) < (l_pRegion->RandomEvent(l_Event) / 30.f * l_pRegion->PercentageValue() * l_fDaysSpent))
				{
					ExecuteRandomEvent(l_pRegion->Id(),l_Event);
					break;
				}
			}
		}		
	}
}

void GWorldBehavior::ExecuteRandomEvent(UINT32 in_iRegionID, ERandomEvents::Enum in_Event)
{
	GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);	

	REAL64 l_pResourceChange[EResources::ItemCount];	
	Memory::Fill<REAL64>(l_pResourceChange,0.f,EResources::ItemCount);
	INT64 l_iPopulationChange = 0;
	REAL32 l_fInfrastructureChange = 0.f;
	REAL64 l_fMoneyChange = 0.f;
	REAL64 l_fTotalResourceChange = 0.f;

	set<UINT32> l_EconomicFriends;
	REAL64 l_pResourcePercFriends[EResources::ItemCount];
	REAL64 l_pResourcePerc[EResources::ItemCount];
	REAL64 l_pResourceFix[EResources::ItemCount];
	Memory::Fill<REAL64>(l_pResourcePerc,0.f,EResources::ItemCount);
	Memory::Fill<REAL64>(l_pResourceFix,0.f,EResources::ItemCount);
	Memory::Fill<REAL64>(l_pResourcePercFriends,0.f,EResources::ItemCount);	
	INT64 l_iMinimumPeopleDead = 0;
	REAL32 l_fPopulationBoom = 0.f;
	REAL32 l_fVariationPeopleDead = 0.f;
	REAL64 l_fAmountToRemovePerDead = 0.f;
	REAL64 l_fFixAmountChange = 0.f;
	REAL64 l_fAmountChangeBasedIncome = 0.f;
	REAL64 l_fAmountChangeBasedIncomeFriend = 0.f;
	REAL32 l_fPercInfrastructureChange = 0.f;
	REAL32 l_fApprovalChange = 0.f;
	REAL32 l_fStabilityChange = 0.f;

	bool l_bProductionCountry = false;

	switch(in_Event)
	{
	case ERandomEvents::Avalanche:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInAvalanche;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInAvalanche;			
		}
		break;
	case ERandomEvents::Blizzard:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInBlizzard;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInBlizzard;						
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInBlizzard;			
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInBlizzard;			
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInBlizzard;
		}
		break;
	case ERandomEvents::ColdWave:
		{
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInColdWave;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInColdWave;
		}
		break;
	case ERandomEvents::Cyclone:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInCyclone;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInCyclone;			
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInCyclone;
			//Remove infrastructure
			l_fPercInfrastructureChange = SP2::c_fPercInfrastructureChangeInCyclone;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInCyclone;
		}
		break;
	case ERandomEvents::Drought:
		{
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInDrought;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInDrought;
			//Remove money
			l_fFixAmountChange = SP2::c_fAmountChangeInDrought;
		}
		break;
	case ERandomEvents::Earthquake:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInEarthquake;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInEarthquake;
			//Remove all resources			
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePerc[k] = SP2::c_fPercAllResourceChangeInEarthquake;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInEarthquake;
		}
		break;
	case ERandomEvents::Flood:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInFlood;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInFlood;
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInFlood;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInFlood;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInFlood;
		}
		break;
	case ERandomEvents::HeathWave:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInHeatWave;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInHeatWave;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInHeatWave;
		}
		break;
	case ERandomEvents::Hurricane:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInHurricane;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInHurricane;			
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInHurricane;
			//Remove infrastructure
			l_fPercInfrastructureChange = SP2::c_fPercInfrastructureChangeInHurricane;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInHurricane;
		}
		break;
	case ERandomEvents::InsectInfestation:
		{
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInInsects;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInInsects;
			//Remove money
			l_fFixAmountChange = SP2::c_fAmountChangeInInsects;
		}
		break;
	case ERandomEvents::Landslide:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInLandslide;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInLandslide;			
			//Remove infrastructure
			l_fPercInfrastructureChange = SP2::c_fPercInfrastructureChangeInLandslide;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInLandslide;
		}
		break;
	case ERandomEvents::Storm:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInStorm;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInStorm;			
			//Remove infrastructure
			l_fPercInfrastructureChange = SP2::c_fPercInfrastructureChangeInStorm;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInStorm;
		}
		break;
	case ERandomEvents::TidalWave:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInTidalWave;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInTidalWave;			
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInTidalWave;
		}
		break;
	case ERandomEvents::Tornado:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInTornado;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInTornado;			
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInTornado;
		}
		break;
	case ERandomEvents::Tsunami:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInTsunami;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInTsunami;			
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInTsunami;
		}
		break;
	case ERandomEvents::Typhoon:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInTyphoon;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInTyphoon;			
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInTyphoon;
			//Remove infrastructure
			l_fPercInfrastructureChange = SP2::c_fPercInfrastructureChangeInTyphoon;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInTyphoon;
		}
		break;
	case ERandomEvents::VolcanicEruption:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInVolcanic;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInVolcanic;						
		}
		break;
	case ERandomEvents::WildFire:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInWildfire;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInWildfire;			
			//Remove cereal production
			l_pResourcePerc[EResources::Wood_Paper] = SP2::c_fPercWoodChangeInWildfire;
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInWildfire;
		}
		break;
	case ERandomEvents::StockMarketCrash:
		{
			l_bProductionCountry = true;
			//Remove all resources			
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePerc[k] = SP2::c_fPercAllResourceChangeInStockMarketCrash;
			//Remove all resources for friends
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePercFriends[k] = SP2::c_fPercAllResourceChangeInStockMarketCrashFriend;			
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInStockMarketCrash;
			//List of friends
			EconomicFriends(l_pRegion->OwnerId(),l_EconomicFriends);			
		}
		break;
	case ERandomEvents::EconomicDepression:
		{
			l_bProductionCountry = true;
			//Remove all resources			
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePerc[k] = SP2::c_fPercAllResourceChangeInEconomicDepression;
			//Remove all resources for friends
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePercFriends[k] = SP2::c_fPercAllResourceChangeInEconomicDepressionFriend;			
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInEconomicDepression;
			//List of friends
			EconomicFriends(l_pRegion->OwnerId(),l_EconomicFriends);			
			//Remove specific amount of money based on income
			l_fAmountChangeBasedIncome = SP2::c_fPercWealthChangeInEconomicDepression;
			//Remove money to friends base on their income
			l_fAmountChangeBasedIncomeFriend = SP2::c_fPercWealthChangeInEconomicDepressionFriend;
		}
		break;
	case ERandomEvents::EconomicBoom:
		{
			l_bProductionCountry = true;
			//Remove all resources			
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePerc[k] = SP2::c_fPercAllResourceChangeInEconomicBoom;
			//Remove all resources for friends
			for(UINT32 k=0; k < EResources::ItemCount; k++)
				l_pResourcePercFriends[k] = SP2::c_fPercAllResourceChangeInEconomicBoomFriend;
			//Add gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInEconomicBoom;
			//List of friends
			EconomicFriends(l_pRegion->OwnerId(),l_EconomicFriends);
			//Remove specific amount of money based on income
			l_fAmountChangeBasedIncome = SP2::c_fPercWealthChangeInEconomicBoom;
			//Remove money to friends base on their income
			l_fAmountChangeBasedIncomeFriend = SP2::c_fPercWealthChangeInEconomicBoomFriend;
		}
		break;
	case ERandomEvents::PoorHarvest:
		{
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInPoorHarvest;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInPoorHarvest;
			//Remove dairy production
			l_pResourcePerc[EResources::Dairy] = SP2::c_fPercDairyChangeInPoorHarvest;
			//Remove vegetables production
			l_pResourcePerc[EResources::Vegetable_Fruits] = SP2::c_fPercVegetablesChangeInPoorHarvest;
		}
		break;
	case ERandomEvents::BountifulHarvest:
		{
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInBountifulHarvest;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInBountifulHarvest;
			//Remove dairy production
			l_pResourcePerc[EResources::Dairy] = SP2::c_fPercDairyChangeInBountifulHarvest;
		}
		break;
	case ERandomEvents::LivestockEpidemic:
		{
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInLivestockEpidemic;
			//Remove dairy production
			l_pResourcePerc[EResources::Dairy] = SP2::c_fPercDairyChangeInLivestockEpidemic;
			//Remove specific amount of money based on income
			l_fAmountChangeBasedIncome = SP2::c_fPercWealthChangeInLivestockEpidemic;
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInLivestockEpidemic;
		}
		break;
	case ERandomEvents::ResourceDiscoveryEnergy:
		{
			//Add fix energy production
			REAL32 l_fRandom = GetRandom(10.f);
			if(l_fRandom < 6.f)
				l_pResourceFix[EResources::Electricity] = SP2::c_fFixChangeInResourceProductionEnergyElectricity;
			else
				l_pResourceFix[EResources::Fossile_Fuels] = SP2::c_fFixChangeInResourceProductionEnergyFossileFuels;
			//Add gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInResourceEnergy;
		}
		break;
	case ERandomEvents::ResourceDiscoveryOre:
		{
			//Add fix energy production
			REAL32 l_fRandom = GetRandom(10.f);
			if(l_fRandom < 4.f)
				l_pResourceFix[EResources::Minerals] = SP2::c_fFixChangeInResourceProductionOreMinerals;
			else if(l_fRandom < 8.f)
				l_pResourceFix[EResources::Iron_Steel] = SP2::c_fFixChangeInResourceProductionOreIronSteel;
			else
				l_pResourceFix[EResources::Precious_Stones] = SP2::c_fFixChangeInResourceProductionOrePrecStones;

			//Add gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInResourceOre;
		}
		break;
	case ERandomEvents::ResourceDiscoveryWealth:
		{
			//Add fix wealth
			l_fFixAmountChange = SP2::c_fFixChangeInResourceProductionWealth;
			//Add gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInResourceWealth;
		}
		break;
	case ERandomEvents::Epidemic:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInEpidemic;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInEpidemic;			
			//Remove money
			l_fAmountToRemovePerDead = SP2::c_fAmountChangePerDeadInEpidemic;
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInEpidemic;
		}
		break;
	case ERandomEvents::PopulationBoom:
		{
			//Add population based on actual population
			l_fPopulationBoom = SP2::c_fPopulationBoostInPopulationBoom;
		}
		break;
	case ERandomEvents::Famine:
		{
			//Remove Population
			l_iMinimumPeopleDead = SP2::c_iMinimumPeopleKilledInFamine;
			l_fVariationPeopleDead = SP2::c_fVariationPeopleKilledInFamine;
			//Remove cereal production
			l_pResourcePerc[EResources::Cereals] = SP2::c_fPercCerealsChangeInFamine;
			//Remove meat production
			l_pResourcePerc[EResources::Meat] = SP2::c_fPercMeatChangeInFamine;
			//Remove dairy production
			l_pResourcePerc[EResources::Dairy] = SP2::c_fPercDairyChangeInFamine;
			//Remove vegetables production
			l_pResourcePerc[EResources::Vegetable_Fruits] = SP2::c_fPercVegetablesChangeInFamine;
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInFamine;
		}
		break;
	case ERandomEvents::NuclearMeltdown:
		{
			//Remove electricity production
			l_pResourcePerc[EResources::Electricity] = SP2::c_fPercElectricityChangeInNuclearMeltdown;
			//Remove money
			l_fFixAmountChange = SP2::c_fAmountChangeInNuclearMeltdown;
		}
		break;
	case ERandomEvents::ChemicalSpill:
		{
			//Remove electricity production
			l_pResourcePerc[EResources::Electricity] = SP2::c_fPercElectricityChangeInChemicalSpill;
			//Remove chemicals production
			l_pResourcePerc[EResources::Chemicals] = SP2::c_fPercChemicalsChangeInChemicalSpill;
			//Remove money
			l_fFixAmountChange = SP2::c_fAmountChangeInChemicalSpill;
		}
		break;
	case ERandomEvents::IndustrialAccident:
		{
			//Remove electricity production
			l_pResourcePerc[EResources::Electricity] = SP2::c_fPercElectricityChangeInIndustrialAccident;
			//Remove chemicals production
			l_pResourcePerc[EResources::Chemicals] = SP2::c_fPercChemicalsChangeInIndustrialAccident;
			//Remove money
			l_fFixAmountChange = SP2::c_fAmountChangeInIndustrialAccident;
		}
		break;
	case ERandomEvents::AntiGlobalizationProtest:
		{
			//Remove gvt approval
			l_fApprovalChange = SP2::c_fPopulationApprovalChangeInAntiGlobalProtest;
			//Remove gvt stability
			l_fStabilityChange = SP2::c_fPopulationStabilityChangeInAntiGlobalProtest;
		}
		break;
	}		

	UINT32 l_iCountry = l_pRegion->OwnerId();
	//Remove Population
	l_iPopulationChange = -(l_iMinimumPeopleDead + (INT64)GetRandom(l_fVariationPeopleDead));
	l_iPopulationChange += (INT64)(l_fPopulationBoom * (REAL32)l_pRegion->Population());
	if(l_iPopulationChange < 0)
		g_ServerDCL.RemovePopulationFromRegion(in_iRegionID,min(-l_iPopulationChange,l_pRegion->Population()));
	else if(l_iPopulationChange > 0)
		g_ServerDCL.AddPopulationToRegion(in_iRegionID,l_iPopulationChange);


	//Change resource production		
	for(UINT32 k=0; k < EResources::ItemCount; k++)
	{	
		if(l_bProductionCountry)
		{	
			l_pResourceChange[k] = g_ServerDAL.CountryData(l_iCountry)->ResourceProduction((EResources::Enum)k) * l_pResourcePerc[k];
			l_fTotalResourceChange += l_pResourceChange[k];
			if(l_pResourceChange[k] > 0.f)
				g_ServerDCL.AddProductionToCountry(l_iCountry,l_pResourceChange[k],(EResources::Enum)k);
			else if(l_pResourceChange[k] < 0.f)
				g_ServerDCL.RemoveProductionFromCountry(l_iCountry,abs(l_pResourceChange[k]),(EResources::Enum)k);
		}
		else
		{
			l_pResourceChange[k] = l_pRegion->ResourceProduction((EResources::Enum)k) * l_pResourcePerc[k];
			l_fTotalResourceChange += l_pResourceChange[k];			
			l_pRegion->ResourceProduction((EResources::Enum)k,
				l_pRegion->ResourceProduction((EResources::Enum)k) + l_pResourceChange[k]);
		}

		l_pRegion->ResourceProduction((EResources::Enum)k,
			max(0.f,l_pRegion->ResourceProduction((EResources::Enum)k) + l_pResourceFix[k]));
	}

	//Remove/Add resource production, and wealth, for friends
	for(set<UINT32>::iterator it = l_EconomicFriends.begin();
		it != l_EconomicFriends.end(); it++)
	{		
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(*it);
		for(UINT32 k=0; k < EResources::ItemCount; k++)
		{	
			REAL64 l_fResourceChange = l_pCountryData->ResourceProduction((EResources::Enum)k) * l_pResourcePercFriends[k];
			if(l_pResourceChange[k] > 0.f)
				g_ServerDCL.AddProductionToCountry(*it,l_fResourceChange,(EResources::Enum)k);
			else if(l_pResourceChange[k] < 0.f)
				g_ServerDCL.RemoveProductionFromCountry(*it,abs(l_fResourceChange),(EResources::Enum)k);			
		}
		if(l_fAmountChangeBasedIncomeFriend > 0.f)		
			g_ServerDCL.GiveMoney(*it,l_pCountryData->BudgetRevenues() * l_fAmountChangeBasedIncomeFriend);
		else if(l_fAmountChangeBasedIncomeFriend < 0.f)
			g_ServerDCL.TakeMoney(*it,l_pCountryData->BudgetRevenues() * abs(l_fAmountChangeBasedIncomeFriend));
	}


	//Remove infrastructure
	l_fInfrastructureChange = l_pRegion->Infrastructure() * l_fPercInfrastructureChange;
	l_pRegion->Infrastructure(l_pRegion->Infrastructure() + l_fInfrastructureChange);

	//Change money, fix amount
	l_fMoneyChange = l_fFixAmountChange;
	//Remove money, variable amount

	if(l_iPopulationChange < 0)
		l_fMoneyChange += (REAL64)(-l_iPopulationChange) * l_fAmountToRemovePerDead;	

	//Change money, amount based on income
	l_fMoneyChange += g_ServerDAL.CountryData(l_iCountry)->BudgetRevenues() * l_fAmountChangeBasedIncome;

	if(l_fMoneyChange > 0.f)
		g_ServerDCL.GiveMoney(l_pRegion->OwnerId(),l_fMoneyChange);
	else if(l_fMoneyChange < 0.f)
		g_ServerDCL.TakeMoney(l_pRegion->OwnerId(),abs(l_fMoneyChange));

	//Change Gvt Approval
	REAL32 l_fOldApproval = g_ServerDAL.CountryData(l_iCountry)->GvtApproval();
	g_ServerDCL.ChangeCountryApproval(l_iCountry,l_fApprovalChange);
	l_fApprovalChange = g_ServerDAL.CountryData(l_iCountry)->GvtApproval() - l_fOldApproval;

	//Change Gvt Stability
	REAL32 l_fOldStability = g_ServerDAL.CountryData(l_iCountry)->GvtStability();
	g_ServerDCL.ChangeCountryStability(l_iCountry,l_fStabilityChange,true);
	l_fStabilityChange = g_ServerDAL.CountryData(l_iCountry)->GvtStability() - l_fOldStability;

	if(in_Event == ERandomEvents::NuclearMeltdown)
	{
		//Lose relationships with neighbours
		set<UINT32> l_Neighbours;
		const GRegionGraphNode* l_pGraph = g_ServerDAL.RegionGraph().Graph();
		const GRegionGraphNode& l_pCurNode = l_pGraph[l_pRegion->Id()];
		for(UINT32 n = 0;n < l_pCurNode.m_iNbNeighbors;n ++)
		{
			if(l_pCurNode.m_pNeighborIDs[n] != 0)
			{
				l_Neighbours.insert(g_ServerDAL.GetGRegion(l_pCurNode.m_pNeighborIDs[n])->OwnerId());
			}			
		}
		l_Neighbours.erase(l_iCountry);
		//Iterate neighbours country
		for(set<UINT32>::iterator it = l_Neighbours.begin();
			it != l_Neighbours.end(); it++)
		{
			g_ServerDAL.RelationBetweenCountries(l_iCountry,*it,
				g_ServerDAL.RelationBetweenCountries(l_iCountry,*it) + SP2::c_fRelationsLossInNuclearMeltdown); 
		}
	}

	REAL32 l_fLongitude = g_ServerDCL.UnitMover().RegionLocation(l_pRegion->Id()).x;
	REAL32 l_fLatitude = g_ServerDCL.UnitMover().RegionLocation(l_pRegion->Id()).y;


	//Notify clients a random event has happened
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GRandomEvent);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
	l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
	SP2::Event::GRandomEvent* l_pEventNotify = (SP2::Event::GRandomEvent*)l_Event.get();
	l_pEventNotify->m_RandomEvent = in_Event;
	l_pEventNotify->m_iCountryID = l_iCountry;
	l_pEventNotify->m_iRegionID = l_pRegion->Id();
	l_pEventNotify->m_fLongitude = l_fLongitude;
	l_pEventNotify->m_fLatitude = l_fLatitude;
	l_pEventNotify->m_iPopulationChange = l_iPopulationChange;
	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		l_pEventNotify->m_pResourceProductionChange[i] = l_pResourceChange[i] + l_pResourceFix[i];
	}	
	l_pEventNotify->m_fInfrastructureChange = l_fInfrastructureChange;
	l_pEventNotify->m_fMoneyChange = l_fMoneyChange;
	l_pEventNotify->m_fApprovalChange = l_fApprovalChange;
	l_pEventNotify->m_fStabilityChange = l_fStabilityChange;
	g_Joshua.RaiseEvent(l_Event); 

   if( (in_Event == ERandomEvents::Earthquake) ||
       (in_Event == ERandomEvents::PopulationBoom) ||
       (in_Event == ERandomEvents::StockMarketCrash) ||
       (in_Event == ERandomEvents::EconomicDepression) ||
       (in_Event == ERandomEvents::EconomicBoom) )
   {
      g_ServerDAL.AddHistoricalMarker(l_pRegion->OwnerId(), 
                                      EHistoryMarkerType::RandomEvent, 
                                      (REAL32) g_ServerDAL.StringIdRandomEvent(in_Event) );
   }
}

void GWorldBehavior::IterateChangingVariables(REAL64 in_fGameTime)
{
	REAL32 l_fTimeDifference = (REAL32)(in_fGameTime - m_fLastStabilityApprovalIteration);
	REAL32 l_fModStability = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fStabilityIterationPeriod);
	REAL32 l_fModApproval = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fApprovalIterationPeriod);	
	REAL32 l_fModBirthRate = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fBirthRateIterationPeriod);
	REAL32 l_fModDeathRate = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fDeathRateIterationPeriod);
	REAL32 l_fModEmigrationRate = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fEmigrationRateIterationPeriod);
	REAL32 l_fModInflationRate = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fInflationRateIterationPeriod);
	REAL32 l_fModRelationRate = min(1.f,l_fTimeDifference / (REAL32)SP2::c_fRelationRateIterationPeriod);

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		REAL32 l_fStability = l_pCountryData->GvtStability();
		REAL32 l_fStabilityEx = l_pCountryData->GvtStabilityExpected();
		REAL32 l_fApproval = l_pCountryData->GvtApproval();
		REAL32 l_fApprovalEx = l_pCountryData->GvtApprovalExpected();
		REAL32 l_fCorruption = l_pCountryData->CorruptionLevel();
		REAL32 l_fCorruptionEx = l_pCountryData->CorruptionLevelExpected();
		REAL32 l_fBirthRate = l_pCountryData->BirthRate();
		REAL32 l_fBirthRateEx = l_pCountryData->BirthRateExpected();
		REAL32 l_fDeathRate = l_pCountryData->DeathRate();
		REAL32 l_fDeathRateEx = l_pCountryData->DeathRateExpected();
		REAL32 l_fEmigration = l_pCountryData->EmigrationLevel();
		REAL32 l_fEmigrationEx = l_pCountryData->EmigrationLevelExpected();
		REAL32 l_fInflation = l_pCountryData->InflationLevel();
		REAL32 l_fInflationEx = l_pCountryData->InflationLevelExpected();

		gassert(l_fStability >= 0.f && l_fStability <= 1.f,"GWorldBehavior::IterateChangingVariables(): Stability is negative");
		gassert(l_fStabilityEx >= 0.f && l_fStabilityEx <= 1.f,"GWorldBehavior::IterateChangingVariables(): Stability expected is negative");
		gassert(l_fApproval >= 0.f && l_fApproval <= 1.f,"GWorldBehavior::IterateChangingVariables(): Approval is negative");

        if(l_fApprovalEx < 0.f || l_fApprovalEx > 1.f)
        {
            GDZLOG("Warning: Invalid expected approval, " + GString(l_fApprovalEx) + L", " +
                   L"for " + l_pCountryData->NameAndIDForLog(),
                   EDZLogLevel::Warning);
            l_pCountryData->GvtApprovalExpected(min(max(0.f, l_fApprovalEx), 1.f));
            GDZLOG("Changed expected approval to " + GString(l_pCountryData->GvtApprovalExpected()),
                   EDZLogLevel::Info2);
        }

		gassert(l_fCorruption >= 0.f && l_fCorruption <= 1.f,"GWorldBehavior::IterateChangingVariables(): Corruption is negative");
		gassert(l_fCorruptionEx >= 0.f && l_fCorruptionEx <= 1.f,"GWorldBehavior::IterateChangingVariables(): Corruption expected is negative");
		gassert(l_fBirthRate >= 0.f && l_fBirthRate <= 1.f,"GWorldBehavior::IterateChangingVariables(): Birth rate is negative");
		gassert(l_fBirthRateEx >= 0.f && l_fBirthRateEx <= 1.f,"GWorldBehavior::IterateChangingVariables(): Birth rate expected is negative");
		gassert(l_fDeathRate >= 0.f,"GWorldBehavior::IterateChangingVariables(): Death rate is negative");
		gassert(l_fDeathRateEx >= 0.f,"GWorldBehavior::IterateChangingVariables(): Death rate expected is negative");		
		gassert(l_fEmigration >= 0.f,"GWorldBehavior::IterateChangingVariables(): Emigration is negative");
		gassert(l_fEmigrationEx >= 0.f,"GWorldBehavior::IterateChangingVariables(): Emigration expected is negative");		
		
		//Change stability
		l_pCountryData->GvtStability(l_fStability + ((l_fStabilityEx - l_fStability)*l_fModStability));

		REAL32 l_fBonus = 0.f;
		if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
			l_fBonus += SP2::c_fFreedomOfSpeechStabilityBonus;
		if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
			l_fBonus += SP2::c_fFreedomOfDemonstrationStabilityBonus;

        const REAL32 l_fServerStabilityAnarchyLowerLimit = g_SP2Server->StabilityAnarchyLowerLimit();
        const REAL32 l_fServerStabilityAnarchyUpperLimit = g_SP2Server->StabilityAnarchyUpperLimit();

		if(l_pCountryData->GvtType() == EGovernmentType::Anarchy &&
			l_pCountryData->GvtStability() > (l_fServerStabilityAnarchyUpperLimit-l_fBonus))
		{
			g_ServerDCL.ChangeGovernmentType(i, EGovernmentType::Anarchy, l_pCountryData->LeaderParty()->GvtType());
		}
		else if(l_fStability < (l_fServerStabilityAnarchyLowerLimit-l_fBonus) && 
			l_pCountryData->GvtStabilityExpected() < (l_fServerStabilityAnarchyLowerLimit-l_fBonus) &&
			l_pCountryData->GvtType() != EGovernmentType::Anarchy)
		{
			g_ServerDCL.ChangeGovernmentType(i, (EGovernmentType::Enum)l_pCountryData->GvtType(), EGovernmentType::Anarchy);
		}

		//Change approval
		l_pCountryData->GvtApproval(l_fApproval + ((l_fApprovalEx - l_fApproval)*l_fModApproval));
		gassert(l_pCountryData->GvtApproval() >= 0.f && l_pCountryData->GvtApproval() <= 1.f,"GWorldBehavior::IterateChangingVariables(): Approval is correctly iterated");
		gassert(!_isnan(l_pCountryData->GvtApproval()),"GWorldBehavior::IterateChangingVariables(): NAN in approval");

		//Change corruption	
		REAL32 l_fModCorruption = l_fTimeDifference;
		if(l_fCorruptionEx < l_fCorruption)
			l_fModCorruption /= ((REAL32)SP2::c_fCorruptionIterationPeriod * SP2::c_pGvtTypeCorruptionModifier[l_pCountryData->GvtType()] + 1.f);
		else
			l_fModCorruption /= (REAL32)SP2::c_fCorruptionIterationPeriod;
		l_fModCorruption = min(1.f,l_fModCorruption);
		l_pCountryData->CorruptionLevel(l_fCorruption + ((l_fCorruptionEx - l_fCorruption)*l_fModCorruption));

		//Change birth rate
		l_pCountryData->BirthRate(l_fBirthRate + ((l_fBirthRateEx - l_fBirthRate)*l_fModBirthRate));

		//Change Death rate
		l_pCountryData->DeathRate(l_fDeathRate + ((l_fDeathRateEx - l_fDeathRate)*l_fModDeathRate));

		//Change emigration level 
		l_pCountryData->EmigrationLevel(l_fEmigration + ((l_fEmigrationEx - l_fEmigration)*l_fModEmigrationRate));

		//Change inflation level
		l_pCountryData->InflationLevel(l_fInflation + ((l_fInflationEx - l_fInflation)*l_fModInflationRate));
	}

	//Change relations
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		for(UINT32 j=i; j <= l_iNbCountries; j++)
		{
			if(i==j)
				continue;
			g_ServerDAL.RelationBetweenCountries(i,j,
				g_ServerDAL.RelationBetweenCountries(i,j) + ((g_ServerDAL.RelationBetweenCountriesExpected(i,j) - g_ServerDAL.RelationBetweenCountries(i,j))*l_fModRelationRate));
		}
	}

	m_fLastStabilityApprovalIteration = in_fGameTime;
}

void GWorldBehavior::IterateForElections()
{
	Hector::GDateTime l_CurrentDate = g_ServerDAL.ActualDate();
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	GCountryData* l_pCountryData = NULL;	
	for(UINT32 i=1; i <= l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(l_pCountryData->Activated() &&
			(l_pCountryData->GvtType() == EGovernmentType::MultiPartyDemocracy || 
			l_pCountryData->GvtType() == EGovernmentType::SinglePartyDemocracy ) &&
			l_pCountryData->NextElection() <= l_CurrentDate)
		{
			//execute elections for that country

			ExecuteElections(l_pCountryData->CountryID());

			//Set next election in 4 years					
			GDateTime l_NextElection(l_CurrentDate.Year() + 4,l_CurrentDate.Month(),
				l_CurrentDate.Day(),l_CurrentDate.Hours(),l_CurrentDate.Minutes(),
				l_CurrentDate.Seconds(),l_CurrentDate.Tenths());
			l_pCountryData->NextElection(l_NextElection);
		}
	}	
}

void GWorldBehavior::IterateTreatiesSuspension()
{
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();	
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
		it != l_Treaties.end();++ it)
	{
		g_ServerDCL.ExecuteSuspension(it->first);
	}

	return;
}

void GWorldBehavior::IterateLossRelationsFromPressure()
{
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();	
	REAL32 l_fModifier = (REAL32)SP2::c_fIterationPeriodNukeNews / 365.f;
	vector<hash_set<UINT32> > l_vPressureOnCountries;
	l_vPressureOnCountries.resize(g_ServerDAL.NbCountry() + 1);

	//Insert the list of treaties for every country.
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
		it != l_Treaties.end();++ it)
	{
		const GTreaty& l_Treaty = it->second;

		const set<UINT32>& l_vMembersPressure = l_Treaty.MembersPressure();
		if(l_vMembersPressure.size())
		{
			//There are members applying pressure. We must give relations loss to countries not active
			for(set<UINT32>::const_iterator l_Pres = l_vMembersPressure.begin();
				l_Pres != l_vMembersPressure.end();
				l_Pres++)
			{
				const set<UINT32>& l_vMembersAActive = l_Treaty.MembersSideA(true);
				const set<UINT32>& l_vMembersAOriginal = l_Treaty.MembersOriginalsA();
				const set<UINT32>& l_vMembersBActive = l_Treaty.MembersSideB(true);
				const set<UINT32>& l_vMembersBOriginal = l_Treaty.MembersOriginalsB();

				for(set<UINT32>::const_iterator l_ItrA = l_vMembersAOriginal.begin();
					l_ItrA != l_vMembersAOriginal.end();
					l_ItrA++)
				{
					if(l_vMembersAActive.find(*l_ItrA) == l_vMembersAActive.end())
					{
						//Member is not active yet, let's penalize him!
						g_ServerDAL.RelationBetweenCountries(*l_Pres,*l_ItrA,
							g_ServerDAL.RelationBetweenCountries(*l_Pres,*l_ItrA) - (l_fModifier * SP2::c_fRelationLossAnnualyFromPressure));

						l_vPressureOnCountries[*l_ItrA].insert(*l_Pres);
					}
				}

				for(set<UINT32>::const_iterator l_ItrB = l_vMembersBOriginal.begin();
					l_ItrB != l_vMembersBOriginal.end();
					l_ItrB++)
				{
					if(l_vMembersBActive.find(*l_ItrB) == l_vMembersBActive.end())
					{
						//Member is not active yet, let's penalize him!
						g_ServerDAL.RelationBetweenCountries(*l_Pres,*l_ItrB,
							g_ServerDAL.RelationBetweenCountries(*l_Pres,*l_ItrB) - (l_fModifier * SP2::c_fRelationLossAnnualyFromPressure));
						
						l_vPressureOnCountries[*l_ItrB].insert(*l_Pres);
					}
				}

			}
		}
		//Pressure from refusing military presence removal
		else if (l_Treaty.Type() == ETreatyType::RequestMilitaryPresenceRemoval &&
					l_Treaty.MembersOriginalsA().size() > 0 &&
					l_Treaty.MembersOriginalsB().size() > 0)
		{
			UINT32 l_iCountryA = *(l_Treaty.MembersOriginalsA().begin());
			UINT32 l_iCountryB = *(l_Treaty.MembersOriginalsB().begin());
			g_ServerDAL.RelationBetweenCountries(l_iCountryA,l_iCountryB,
				g_ServerDAL.RelationBetweenCountries(l_iCountryA,l_iCountryB) - (l_fModifier * SP2::c_fRelationLossAnnualyFromMilitaryRemoval));
		}
	}

	//Update pressure on every countries
	for(UINT32 i=1; i< l_vPressureOnCountries.size(); i++)
	{
		REAL32 l_fTotalValuePressuringCountry = 0.f;
		for(hash_set<UINT32>::iterator it = l_vPressureOnCountries[i].begin();
			 it != l_vPressureOnCountries[i].end(); it++)
		{
			l_fTotalValuePressuringCountry += g_ServerDAL.TotalUnitsValue(*it);
		}
		g_ServerDAL.CountryData(i)->Pressure(min(1.f,l_fTotalValuePressuringCountry / (g_ServerDAL.TotalUnitsValue(i) + 1.f)));
	}
}

void GWorldBehavior::IterateGlobalMarket()
{
	const REAL64 c_fGDPEconomicActivityRatio = 0.5f;

	INT16 l_iNbCountry = g_ServerDAL.NbCountry();		

#ifdef GOLEM_DEBUG
	//m_pLoggerResources->Log(MSGTYPE_CSV,L"---------------------------------------------------------");
	//m_pLoggerResources->Log(MSGTYPE_CSV,L"Pool for imports:");
#endif GOLEM_DEBUG

	//Calculate the preceding amount of economic activity, to have the pool of possible imports for every country
	vector<REAL64> l_vPoolOfImports;
	l_vPoolOfImports.resize(l_iNbCountry+1);
	for(UINT32 i=1; i<= (UINT32)l_iNbCountry; i++)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		l_pCountryData->IterateDemand();
		l_vPoolOfImports[i] = l_pCountryData->GDPValue() * c_fGDPEconomicActivityRatio;
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			l_vPoolOfImports[i] += l_pCountryData->ResourceExport((EResources::Enum)k);
		}
		
		//let's put all exports and imports to 0
		l_pCountryData->InitExportsImports();

#ifdef GOLEM_DEBUG
		//m_pLoggerResources->Log(MSGTYPE_CSV,GString(i) + L", " + GString(l_vPoolOfEA[i]));
#endif GOLEM_DEBUG

	}

	//Calculate the production/demand of every country, used before the final one, to calculate the loss of production
	g_ServerDAL.CalculateWorldProductionDemand();	

	//If there's only 1 country left, everything below is not important
	if(l_iNbCountry == 1)
		return;
	const REAL64* l_pWorldProduction = g_ServerDAL.MarketProduction();
	const REAL64* l_pWorldDemand = g_ServerDAL.MarketDemand();	

#ifdef GOLEM_DEBUG
	//m_pLoggerResources->Log(MSGTYPE_CSV,L"Market availability: ");
#endif GOLEM_DEBUG

	REAL64 l_pLossFromMarket[EResources::ItemCount];
	Memory::Fill<REAL64>(l_pLossFromMarket,0.f,EResources::ItemCount);
	for(UINT32 k = 0; k < EResources::ItemCount; k++)
	{		
		if(l_pWorldProduction[k] > l_pWorldDemand[k])
			l_pLossFromMarket[k] = l_pWorldProduction[k] - l_pWorldDemand[k];

#ifdef GOLEM_DEBUG
		//m_pLoggerResources->Log(MSGTYPE_CSV,GString(l_pWorldProduction[k] - l_pWorldDemand[k]) + L", " + g_ServerDAL.GetString(g_ServerDAL.StringIdResource((EResources::Enum)k)));
#endif GOLEM_DEBUG

	}


	/*
	*	First of all, we calculate the loss of production for every country. 
	*	Then, adjust the production loss of countries, for those with too little market share
	**/
	REAL64 l_pSumOfShare[EResources::ItemCount];
	REAL64 l_pLossForEachCountry[EResources::ItemCount];	
	Memory::Fill<REAL64>(l_pSumOfShare,0.f,EResources::ItemCount);
	Memory::Fill<REAL64>(l_pLossForEachCountry,0.f,EResources::ItemCount);	
	GCountryData* l_pCountryData;
	for(UINT32 i=1; i<= (UINT32)l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		REAL64 l_fEconomicHealth = (REAL64)l_pCountryData->EconomicHealth();
		for(UINT32 k = 0; k < EResources::ItemCount; k++)
		{
			if(l_pCountryData->ResourceProduction((EResources::Enum)k) > l_pCountryData->ResourceDemand((EResources::Enum)k))
				l_pSumOfShare[k] += (l_pCountryData->ResourceProduction((EResources::Enum)k) / (l_pWorldProduction[k] + 1.f)) * l_fEconomicHealth;
		}
	}


#ifdef GOLEM_DEBUG
	//m_pLoggerResources->Log(MSGTYPE_CSV,L"Market loss: ");
#endif GOLEM_DEBUG
	for(UINT32 i=1; i<= (UINT32)l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		bool l_bChanged = false;
		REAL64 l_fEconomicHealth = (REAL64)l_pCountryData->EconomicHealth();
		REAL64 l_fLoss = 0.f;
		REAL64 l_fWhatCountryCanLose = 0.f;
		for(UINT32 k = 0; k < EResources::ItemCount; k++)
		{
			l_fWhatCountryCanLose = max(0.f,l_pCountryData->ResourceProduction((EResources::Enum)k)-l_pCountryData->ResourceDemand((EResources::Enum)k));
			if(l_fWhatCountryCanLose > 0.f)
			{
				l_fLoss = ( 1.f - ( ((l_pCountryData->ResourceProduction((EResources::Enum)k) / (l_pWorldProduction[k] + 1.f)) * l_fEconomicHealth) / l_pSumOfShare[k] ) ) / ((REAL64)(l_iNbCountry - 1));
				l_fLoss = min(l_fLoss * l_pLossFromMarket[k],l_fWhatCountryCanLose);
				//Remove production from active regions
				g_ServerDCL.RemoveProductionFromCountry(i,l_fLoss,(EResources::Enum)k);
#ifdef GOLEM_DEBUG
				//m_pLoggerResources->Log(MSGTYPE_CSV,GString(i) + L"," + GString(l_fLoss) + L", " + g_ServerDAL.GetString(g_ServerDAL.StringIdResource((EResources::Enum)k)));
#endif GOLEM_DEBUG
				l_bChanged = true;
			}
		}
		if(l_bChanged)
			l_pCountryData->SynchronizeWithRegions();
	}

	// FINAL calcul of the world production and demand
	g_ServerDAL.CalculateWorldProductionDemand();	

	//Once we have the final world production and demand, we can calculate the resource market share for every country
	const REAL64* l_pWorldProductionFinal = g_ServerDAL.MarketProduction();	
	for(UINT32 i=1; i<= (UINT32)l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		for(UINT32 k=0; k< EResources::ItemCount; k++)
		{
			l_pCountryData->ResourceMarketShare((EResources::Enum)k,(REAL32)(l_pCountryData->ResourceProduction((EResources::Enum)k) / (l_pWorldProductionFinal[k] + 1.f)) ); 
		}		

		//Also, we need to Recalculate the desired exports/imports
		g_ServerDCL.IterateDesiredExportsImports(i);
	}

	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();	

	UINT32 l_iNbCommonMarket = 0;
	
	//Insert the list of treaties for every country.
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
		it != l_Treaties.end();++ it)
	{
		const GTreaty& l_Treaty = it->second;

		if(!l_Treaty.Active()  || l_Treaty.Type() != ETreatyType::CommonMarket)
			continue;
		ExecuteMarket(l_Treaty.ID(),false,l_vPoolOfImports);
		l_iNbCommonMarket++;
	}

	#ifdef GOLEM_DEBUG
			g_Joshua.Log(L"Number of global market treaties: " + GString(l_iNbCommonMarket));
	#endif

	ExecuteMarket(0,true,l_vPoolOfImports);	

	//Drop production
	for(UINT32 i=1; i<= (UINT32)l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(!l_pCountryData->Activated())
			continue;
		bool l_bChanged = false;
		for(UINT32 k=0; k< EResources::ItemCount; k++)
		{
			REAL64 l_fBalance = l_pCountryData->ResourceBalance((EResources::Enum)k);
			if(l_fBalance > 0.f) 
			{
				g_ServerDCL.RemoveProductionFromCountry(i,l_fBalance * 0.8f,(EResources::Enum)k);
				l_bChanged = true;
			}	
		}
		if(l_bChanged)
			l_pCountryData->SynchronizeWithRegions();
	}

	g_ServerDCL.CalculateMarketAvailability();
}

void GWorldBehavior::ExecuteMarket(UINT32 in_iTreatyID, bool in_bWorldMarket, vector<REAL64>& in_vPool)
{

	//Fill the economic rank hash map
	multimap<INT16,GCountryData*> l_EconomicRanks;			

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	if(in_bWorldMarket)
	{
		for(UINT32 i = 1; i<= l_iNbCountry; i++)
		{
			if(g_ServerDAL.CountryIsValid(i))
				l_EconomicRanks.insert(make_pair<INT16,GCountryData*>(g_ServerDAL.CountryData(i)->EconomicRank(),g_ServerDAL.CountryData(i)));
		}
	}
	else
	{
		GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
		for(set<UINT32>::const_iterator it =  l_pTreaty->MembersSideA(true).begin();
			it != l_pTreaty->MembersSideA(true).end(); it++)
		{			
			l_EconomicRanks.insert(make_pair<INT16,GCountryData*>(g_ServerDAL.CountryData(*it)->EconomicRank(),g_ServerDAL.CountryData(*it)));
		}
	}

	const bool* l_pTradeEmbargos = g_ServerDAL.TradeEmbargos();

	REAL64 l_fImportersImport = 0.f;
	REAL64 l_fExporterExport = 0.f;
	REAL64 l_fNeededImport = 0.f;
	REAL64 l_fRealImport = 0.f;	
	EResources::Enum l_iResource;	
	GCountryData* l_pImporter;
	GCountryData* l_pExporter;

	//Go through the list of countries that want to import resources
	for(multimap<INT16,GCountryData*>::iterator i = l_EconomicRanks.begin();
		i != l_EconomicRanks.end(); i++)
	{
		l_pImporter = i->second;
		gassert(l_pImporter != NULL,"WORLD_BEHAVIOR::ExecuteMarket(): No country has that rank!");

		if(in_vPool[l_pImporter->CountryID()] <= 0.f)
			continue;

		const UINT8* l_pStatus = g_ServerDAL.CountryDiplomaticStatuses(l_pImporter->CountryID());

		for(UINT32 k = 0; k < EResources::ItemCount; k++)
		{
			if(in_vPool[l_pImporter->CountryID()] <= 0.f)
				break;
			
			l_iResource = (EResources::Enum)k;

			for(multimap<INT16,GCountryData*>::iterator j = l_EconomicRanks.begin();
				j != l_EconomicRanks.end(); j++)
			{		
				l_pExporter = j->second;					

				if(l_pExporter->CountryID() == l_pImporter->CountryID())
					continue;								

				//Check if the country is activated, if it's not at war against that other country, and that they have no trade embargos
				if((EDiplomaticStatus::Enum)l_pStatus[l_pExporter->CountryID()] == EDiplomaticStatus::Hostile ||
					l_pTradeEmbargos[l_pExporter->CountryID() * (l_iNbCountry + 1) + l_pImporter->CountryID() ])
					continue;

				l_fImportersImport = l_pImporter->ResourceImport(l_iResource) * (1.f + l_pImporter->ResourceTaxes(l_iResource) + l_pImporter->GlobalTaxMod());

				l_fExporterExport = l_pExporter->ResourceExport(l_iResource);

				l_fNeededImport = l_pImporter->ResourceImportDesired(l_iResource) - l_fImportersImport;

				if(!l_pImporter->ResourceGvtCtrl(l_iResource) && l_pImporter->ResourceLegal(l_iResource))
					l_fNeededImport = min(l_fNeededImport,in_vPool[l_pImporter->CountryID()]);

				if(l_fNeededImport > 0.f)
				{
					l_fRealImport = min(l_fNeededImport, min(l_pExporter->ResourceExportDesired(l_iResource), l_pExporter->ResourceProduction(l_iResource))- l_fExporterExport);

					if(l_fRealImport > 0.f)
					{
						if(!l_pImporter->ResourceGvtCtrl(l_iResource) && l_pImporter->ResourceLegal(l_iResource))
							in_vPool[l_pImporter->CountryID()] -= l_fRealImport;
						//g_Joshua.Log(g_ServerDAL.GetString(l_pImporter->NameID()) + L", imports, " + GString(l_fRealImport) + L", " + g_ServerDAL.GetString(g_ServerDAL.StringIdResource(l_iResource)) + L", -" + GString(l_fRealImport));

						l_pImporter->ResourceImport(l_iResource, l_fImportersImport + (l_fRealImport * (1.f - (REAL64)(l_pImporter->ResourceTaxes(l_iResource) + l_pImporter->GlobalTaxMod())) ));
						if(l_pImporter->ResourceLegal(l_iResource))
						{
							if(l_pImporter->ResourceGvtCtrl(l_iResource))
								l_pImporter->BudgetExpenseImports(l_pImporter->BudgetExpenseImports()+(l_fRealImport*SP2::c_fGvtControlledResourceMod));
							else
							{
								l_pImporter->BudgetRevenueTrade(l_pImporter->BudgetRevenueTrade() + 
									(l_fRealImport * (REAL64)(l_pImporter->ResourceTaxes(l_iResource) + l_pImporter->GlobalTaxMod())));
							}
						}

						//g_Joshua.Log(g_ServerDAL.GetString(l_pExporter->NameID()) + L", exports, " + GString(l_fRealImport) + L", " + g_ServerDAL.GetString(g_ServerDAL.StringIdResource(l_iResource)) + L", " + GString(l_fRealImport * (1.f - (REAL64)(l_pExporter->ResourceTaxes(l_iResource) + l_pExporter->GlobalTaxMod() ))));

						l_pExporter->ResourceExport(l_iResource,l_fExporterExport + l_fRealImport);
						if(l_pExporter->ResourceLegal(l_iResource))
						{
							if(l_pExporter->ResourceGvtCtrl(l_iResource))
								l_pExporter->BudgetRevenueTrade(l_pExporter->BudgetRevenueTrade()+(l_fRealImport*SP2::c_fGvtControlledResourceMod));
							else
							{
								l_pExporter->BudgetRevenueTrade(l_pExporter->BudgetRevenueTrade()
									+ (l_fRealImport * (REAL64)(l_pExporter->ResourceTaxes(l_iResource) + l_pExporter->GlobalTaxMod() )));
							}
						}	
					}
				}
				else
					break;
			}
		}				
	}

}

void GWorldBehavior::IterateIMF(REAL64 in_fGameTime)
{
	GCountryData* l_pCountryData;
	m_fIMFfunds = 0.f;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	//Fill the economic rank hash map
	multimap<INT16,GCountryData*> l_EconomicRanks;
	for(UINT32 i = 1; i <= l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		if(l_pCountryData->Activated())
		{
			m_fIMFfunds += l_pCountryData->BudgetExpenseIMF();
			l_EconomicRanks.insert(make_pair<INT16,GCountryData*>(-l_pCountryData->EconomicRank(),l_pCountryData));
		}
	}	
	
	if(m_fIMFfunds == 0.f)
		return;

	REAL64 l_fTotalNeed = 0.f;
	REAL32 l_fTotalOverallRelations = 0.f;
	multimap<REAL32,GCountryData*> l_CountriesSortedByOverallRelations;
	hash_map<UINT32,REAL64> l_NeedsByCountry;

	//Give IMF to most deserving countries
	REAL64 l_fIMFPart = m_fIMFfunds;
	for(multimap<INT16,GCountryData*>::iterator it = l_EconomicRanks.begin();
		 it != l_EconomicRanks.end() && l_fIMFPart > 0.f; it++)
	{	
		l_pCountryData = it->second;
		//Calculate the Revenues
		REAL64 l_fRevenues = l_pCountryData->BudgetRevenues();
		REAL64 l_fFixedExpenses = l_pCountryData->BudgetExpenseEducationOnAverage()
		+ l_pCountryData->BudgetExpenseGovernmentOnAverage() + l_pCountryData->BudgetExpenseHealthcareOnAverage()
		+ l_pCountryData->BudgetExpenseIMFOnAverage() + l_pCountryData->BudgetExpenseResearchOnAverage()
		+ l_pCountryData->BudgetExpenseEnvironmentOnAverage() + l_pCountryData->BudgetExpenseInfrastructureOnAverage()
		+ l_pCountryData->BudgetExpensePropagandaOnAverage() + l_pCountryData->BudgetExpenseTelecomOnAverage()
		+ l_pCountryData->BudgetExpenseTourismOnAverage();

		g_ServerDAL.CountryData(l_pCountryData->CountryID())->BudgetRevenueIMF(0.f);
		REAL64 l_fDifference = l_fRevenues - l_fFixedExpenses;
		if(l_fDifference < 0.f)
		{
			l_fTotalNeed += abs(l_fDifference);
			l_fTotalOverallRelations += l_pCountryData->OverallDiplomaticRelations() + 100.f;
			l_NeedsByCountry[l_pCountryData->CountryID()] = abs(l_fDifference);
			l_CountriesSortedByOverallRelations.insert(make_pair<REAL32,GCountryData*>(-(l_pCountryData->OverallDiplomaticRelations() + 100.f),l_pCountryData));
		}			
	}

	//Give the first 50% of money based on needs
	REAL64 l_fHalf = m_fIMFfunds / 2.f;
	for(hash_map<UINT32,REAL64>::iterator it = l_NeedsByCountry.begin();
		 it != l_NeedsByCountry.end(); it++)
	{
		REAL64 l_fPartToGive = (it->second / l_fTotalNeed) * l_fHalf;
		g_ServerDAL.CountryData(it->first)->BudgetRevenueIMF(l_fPartToGive);
		it->second -= l_fPartToGive;
	}
	
	if(l_fTotalOverallRelations == 0.f)
		return;

	//Then, give second half to most loved countries
	for(multimap<REAL32,GCountryData*>::iterator it = l_CountriesSortedByOverallRelations.begin();
		 it != l_CountriesSortedByOverallRelations.end(); it++)
	{
		REAL32 l_fRelations = -(it->first);
		REAL64 l_fPartToReceive = (REAL64)(l_fRelations / l_fTotalOverallRelations) * l_fHalf;
		REAL64 l_fLeftToReceive = l_NeedsByCountry[it->second->CountryID()];
		g_ServerDAL.CountryData(it->second->CountryID())->BudgetRevenueIMF( 
			g_ServerDAL.CountryData(it->second->CountryID())->BudgetRevenueIMF() + min(l_fLeftToReceive,l_fPartToReceive));
	}
}

void GWorldBehavior::IterateBudget(REAL64 in_fGameTime)
{
	const REAL64 TwoWeekShare = (in_fGameTime - m_fLastBudgetIteration) / 365.0f;

	GCountryData* l_pCountryData = NULL;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	vector<REAL64> l_vDiplomacyExpenses;
	vector<REAL64> l_vForeignAid;
	vector<REAL64> l_vForeignAidExpenses;
	vector<UINT32> l_vNbOfTreatiesReceivingByCountry;
	vector<UINT32> l_vNbOfTreatiesGivingByCountry;
	l_vDiplomacyExpenses.resize(l_iNbCountry+1,0.f);	
	l_vForeignAidExpenses.resize(l_iNbCountry+1,0.f);	
	l_vForeignAid.resize(l_iNbCountry+1,0.f);	
	l_vForeignAid.resize(l_iNbCountry+1,0);	
	l_vNbOfTreatiesGivingByCountry.resize(l_iNbCountry+1,0);
	l_vNbOfTreatiesReceivingByCountry.resize(l_iNbCountry+1);
	set<UINT32> l_vListOfEconomicAidTreaties;	
	REAL64 l_fAmountToShareByTreaties = 0.f;
	REAL64 l_fAmountGiven = 0.f;
	REAL64 l_fTempAmount = 0.f;
	hash_map<UINT32,REAL64> l_hAmountByTreaty;	
	
	//Also iterate the diplomacy cost, for treaties

	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();	

	//Insert the list of treaties for every country.
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
		it != l_Treaties.end();++ it)
	{
		const GTreaty& l_Treaty = it->second;

		const set<UINT32>& l_MembersSideA = l_Treaty.MembersSideA(true);
		const set<UINT32>& l_MembersSideB = l_Treaty.MembersSideB(true);
		const set<UINT32>& l_MembersPressure = l_Treaty.MembersPressure();
		for(set<UINT32>::const_iterator l_ItrA = l_MembersSideA.begin();
			l_ItrA != l_MembersSideA.end();
			l_ItrA++)
		{
			l_vDiplomacyExpenses[*l_ItrA] += l_Treaty.CostForCountry(*l_ItrA);			
		}
		for(set<UINT32>::const_iterator l_ItrB = l_MembersSideB.begin();
			l_ItrB != l_MembersSideB.end();
			l_ItrB++)
		{
			l_vDiplomacyExpenses[*l_ItrB] += l_Treaty.CostForCountry(*l_ItrB);			
		}
		for(set<UINT32>::const_iterator l_Itr = l_MembersPressure.begin();
			l_Itr != l_MembersPressure.end();
			l_Itr++)
		{
			l_vDiplomacyExpenses[*l_Itr] += l_Treaty.CostForCountry(*l_Itr);						
		}

		//Check if the treaty is an economic aid treaty
		{
			if(l_Treaty.Active() && l_Treaty.Type() == ETreatyType::EconomicAid)
			{
				l_vListOfEconomicAidTreaties.insert(l_Treaty.ID());

				for(set<UINT32>::const_iterator l_ItrB = l_MembersSideB.begin();
	 				 l_ItrB != l_MembersSideB.end();
				    l_ItrB++)
				{
					l_vNbOfTreatiesReceivingByCountry[*l_ItrB] += 1;
				}
				for(set<UINT32>::const_iterator l_ItrA = l_MembersSideA.begin();
	 				 l_ItrA != l_MembersSideA.end();
				    l_ItrA++)
				{
					l_vNbOfTreatiesGivingByCountry[*l_ItrA] += 1;
				}
			}
		}
	}

	//Calculate the sum of money needed by every treaty, for economic aid
	for(set<UINT32>::const_iterator it = l_vListOfEconomicAidTreaties.begin();
		 it != l_vListOfEconomicAidTreaties.end();++ it)
	{
		const GTreaty& l_Treaty  = l_Treaties.find(*it)->second;
		const set<UINT32>& l_MembersSideB = l_Treaty.MembersSideB(true);
		const set<UINT32>& l_MembersSideA = l_Treaty.MembersSideA(true);
		REAL64 l_fAmountForThatTreaty = 0.f;
		for(set<UINT32>::const_iterator l_ItrB = l_MembersSideB.begin();
			 l_ItrB != l_MembersSideB.end();
			 l_ItrB++)
		{
			UINT32 l_iNbTreaties = l_vNbOfTreatiesReceivingByCountry[*l_ItrB];
			if(l_iNbTreaties > 9)
				l_fAmountToShareByTreaties = 90.f / (REAL64)l_iNbTreaties / 100.f;
			else
				l_fAmountToShareByTreaties = .10f;
			l_fAmountForThatTreaty += (g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenses() - g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenseIMF() - g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenseDiplomacy()) * l_fAmountToShareByTreaties;
		}
		
		REAL64 l_fSliceForEveryCountry = 0.f; 
		if(l_MembersSideA.size() > 0)
			l_fSliceForEveryCountry = l_fAmountForThatTreaty / l_MembersSideA.size();
		for(set<UINT32>::const_iterator l_ItrA = l_MembersSideA.begin();
			 l_ItrA != l_MembersSideA.end();
			 l_ItrA++)
		{			
			l_vForeignAidExpenses[*l_ItrA] += l_fSliceForEveryCountry;
		}
		l_hAmountByTreaty[*it] = l_fAmountForThatTreaty;
	}

	//Compute money given by contribuing countries
	for(hash_map<UINT32,REAL64>::iterator l_Itr = l_hAmountByTreaty.begin();
		l_Itr != l_hAmountByTreaty.end();
		l_Itr++)
	{
		const GTreaty& l_Treaty  = l_Treaties.find(l_Itr->first)->second;
		REAL64 l_fMoneyNeeded = l_Itr->second;

		if(l_fMoneyNeeded <= 0.f)
		continue;

		l_fAmountGiven = 0.f;
		const set<UINT32>& l_MembersSideA = l_Treaty.MembersSideA(true);
		const set<UINT32>& l_MembersSideB = l_Treaty.MembersSideB(true);
		for(set<UINT32>::const_iterator l_ItrA = l_MembersSideA.begin();
			l_ItrA != l_MembersSideA.end();
			l_ItrA++)
		{
			UINT32 l_iNbTreatiesTheGivingCountryIsIn = l_vNbOfTreatiesGivingByCountry[*l_ItrA];
			l_fTempAmount = min(((g_ServerDAL.CountryData(*l_ItrA)->BudgetExpenses() - g_ServerDAL.CountryData(*l_ItrA)->BudgetExpenseIMF() - g_ServerDAL.CountryData(*l_ItrA)->BudgetExpenseDiplomacy()) * SP2::c_fMaximumPourcPaidByEconomicAid) / (REAL64)l_iNbTreatiesTheGivingCountryIsIn, 
									  l_fMoneyNeeded / (REAL64)l_MembersSideA.size() );
			l_fAmountGiven += l_fTempAmount;
			l_vDiplomacyExpenses[*l_ItrA] += l_fTempAmount;
		}
		
		REAL64 l_fRatioGiven = l_fAmountGiven / l_fMoneyNeeded;

		//Give money
		for(set<UINT32>::const_iterator l_ItrB = l_MembersSideB.begin();
			l_ItrB != l_MembersSideB.end();
			l_ItrB++)
		{
			UINT32 l_iNbTreaties = l_vNbOfTreatiesReceivingByCountry[*l_ItrB];
			if(l_iNbTreaties > 9)
				l_fAmountToShareByTreaties = 90.f / (REAL64)l_iNbTreaties / 100.f;
			else
				l_fAmountToShareByTreaties = .10f;
			//??
			REAL64 l_fAmountToReceive = (g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenses() - g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenseIMF() - g_ServerDAL.CountryData(*l_ItrB)->BudgetExpenseDiplomacy()) * l_fAmountToShareByTreaties * l_fRatioGiven;
			l_vForeignAid[*l_ItrB] += l_fAmountToReceive;
		}
	}

    //Iterate client state tributes
    for(ENTITY_ID i = 1; i <= l_iNbCountry; i++)
    {
        const GCountryData* const l_pClientData = g_ServerDAL.CountryData(i);
        const auto l_MasterPair = l_pClientData->Master();
        const ENTITY_ID l_iMaster = l_MasterPair.first;
        if(l_iMaster != 0)
        {
            const GCountryData* const l_pMasterData = g_ServerDAL.CountryData(l_iMaster);
            gassert(g_ServerDAL.Treaties().count(l_MasterPair.second) == 1,
                    l_pClientData->NameAndIDForLog() + L" doesn't have a treaty " +
                    L"for its client status with " +
                    l_pMasterData->NameAndIDForLog());

            //Diplomacy expense for client, foreign aid revenue for master
            const REAL64 l_fClientGDP = l_pClientData->GDPValue();
            const REAL64 l_fTribute   = l_fClientGDP * g_SP2Server->TributePercent();
            GDZLOG(l_pClientData->NameAndIDForLog() + L" with GDP " +
                   GString::FormatNumber(l_fClientGDP / 1000000, L",", L".", L"$", L"M", 3, 3) + L" pays " +
                   GString::FormatNumber(l_fTribute / 1000000, L",", L".", L"$", L"M", 3, 3) +
                   L" tribute to " + l_pMasterData->NameAndIDForLog(),
                   EDZLogLevel::Info2);
            l_vDiplomacyExpenses[i] += l_fTribute;
            l_vForeignAid[l_iMaster] += l_fTribute;
        }
    }

	//Remove money from budget, or add expenses
	for(UINT32 i=1; i <= l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);
		l_pCountryData->BudgetRevenueIMF( l_pCountryData->BudgetRevenueIMF() + l_vForeignAid[i] );
		l_pCountryData->BudgetExpenseDiplomacy( l_vDiplomacyExpenses[i] );
	}

	//Now iterate budget
	for(UINT32 i = 1; i <= l_iNbCountry; i++)
	{
		l_pCountryData = g_ServerDAL.CountryData(i);

		if(!l_pCountryData->Activated())
			continue;		

		REAL64 l_fYearlyRevenues = l_pCountryData->BudgetRevenueTax() +
			l_pCountryData->BudgetRevenueTrade() +
			l_pCountryData->BudgetRevenueTourism() +
			l_pCountryData->BudgetRevenueIMF();

		//Iterate Corruption
		l_pCountryData->BudgetExpenseCorruption(l_fYearlyRevenues * 
			(l_pCountryData->CorruptionLevel() * SP2::c_fCorruptionStrength));

		REAL64 l_fTempExpenses = TwoWeekShare * 
			(l_pCountryData->BudgetExpenseInfrastructure() +
			l_pCountryData->BudgetExpenseEnvironment() +
			l_pCountryData->BudgetExpenseHealthcare() +
			l_pCountryData->BudgetExpenseEducation() +
			l_pCountryData->BudgetExpenseTelecom() +
			l_pCountryData->BudgetExpenseGovernment() +
			l_pCountryData->BudgetExpenseSecurity() +
			l_pCountryData->BudgetExpenseDiplomacy() +
			l_pCountryData->BudgetExpenseDebt() +
			l_pCountryData->BudgetExpenseUnitUpkeep() +
			l_pCountryData->BudgetExpenseImports() +
			l_pCountryData->BudgetExpenseIMF() +
			l_pCountryData->BudgetExpensePropaganda() +
			l_pCountryData->BudgetExpenseResearch() +
			l_pCountryData->BudgetExpenseTourism() + 
			l_pCountryData->BudgetExpenseCorruption());		

		REAL64 l_fTempRevenues = TwoWeekShare * l_fYearlyRevenues;

		REAL64 l_fTotal = l_fTempRevenues - l_fTempExpenses;

		//Set the available money
		l_pCountryData->MonetarySupplyAmount(l_pCountryData->MonetarySupplyAmount() + l_fTotal);
	}	
}

/*!
* Returns a value between 0.0f and in_fMaximumValue (excluded)
**/

REAL32 GWorldBehavior::GetRandom(REAL32 in_fMaximumValue)
{
	return m_QuickRandom.RandomReal(in_fMaximumValue);
}

/*!
* Calculate the primary resource factor, which gives a number between 0 and 1.
* It is not adjusted by the rich factor of the country
**/
REAL32 GWorldBehavior::FindPrimaryResourceFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fTotalPrimary = 0.0f;	

	l_fTotalPrimary = ( ResImpCereals * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Cereals)
		+ l_CountryData->ResourceImport(SP2::EResources::Cereals)
		- l_CountryData->ResourceExport(SP2::EResources::Cereals) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Cereals) + 1.f)) ) 
		+ ( ResImpVegetablesFruits * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Vegetable_Fruits)
		+ l_CountryData->ResourceImport(SP2::EResources::Vegetable_Fruits)
		- l_CountryData->ResourceExport(SP2::EResources::Vegetable_Fruits) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Vegetable_Fruits) + 1.f)) )
		+ ( ResImpDairy * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Dairy)
		+ l_CountryData->ResourceImport(SP2::EResources::Dairy)
		- l_CountryData->ResourceExport(SP2::EResources::Dairy) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Dairy) + 1.f)) )
		+ ( ResImpMeat * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Meat)
		+ l_CountryData->ResourceImport(SP2::EResources::Meat)
		- l_CountryData->ResourceExport(SP2::EResources::Meat) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Meat) + 1.f)) )
		+ ( ResImpTobacco * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Tobacco)
		+ l_CountryData->ResourceImport(SP2::EResources::Tobacco)
		- l_CountryData->ResourceExport(SP2::EResources::Tobacco) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Tobacco) + 1.f)) )
		+ ( ResImpDrugs * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Drugs)
		+ l_CountryData->ResourceImport(SP2::EResources::Drugs)
		- l_CountryData->ResourceExport(SP2::EResources::Drugs) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Drugs) + 1.f)) );

	if (l_fTotalPrimary > 1.f)
		return 1.f;
	if (l_fTotalPrimary < 0.f)
		return 0.f;

	return l_fTotalPrimary;
}

/*!
* Calculate the tertiary resource factor, which gives a number between 0 and 1.
* It is not adjusted by the rich factor of the country
**/
REAL32 GWorldBehavior::FindTertiaryResourceFactor(ENTITY_ID in_iCountryID)
{

	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fTotalTertiary = 0.0f;

	l_fTotalTertiary = ( ResImpConstruction * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Construction)
		+ l_CountryData->ResourceImport(SP2::EResources::Construction)
		- l_CountryData->ResourceExport(SP2::EResources::Construction) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Construction) + 1.f)) ) 
		+ ( ResImpEngineering * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Engineering)
		+ l_CountryData->ResourceImport(SP2::EResources::Engineering)
		- l_CountryData->ResourceExport(SP2::EResources::Engineering) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Engineering) + 1.f)) )
		+ ( ResImpHealth * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Health_Care)
		+ l_CountryData->ResourceImport(SP2::EResources::Health_Care)
		- l_CountryData->ResourceExport(SP2::EResources::Health_Care) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Health_Care) + 1.f)) )
		+ ( ResImpRetail * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Retail)
		+ l_CountryData->ResourceImport(SP2::EResources::Retail)
		- l_CountryData->ResourceExport(SP2::EResources::Retail) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Retail) + 1.f)) )
		+ ( ResImpLegal * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Legal_Services)
		+ l_CountryData->ResourceImport(SP2::EResources::Legal_Services)
		- l_CountryData->ResourceExport(SP2::EResources::Legal_Services) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Legal_Services) + 1.f)) )
		+ ( ResImpMarket * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Marketing_Advertising)
		+ l_CountryData->ResourceImport(SP2::EResources::Marketing_Advertising)
		- l_CountryData->ResourceExport(SP2::EResources::Marketing_Advertising) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Marketing_Advertising) + 1.f)) );	

	if (l_fTotalTertiary > 1.f)
		return 1.f;
	if (l_fTotalTertiary < 0.f)
		return 0.f;

	return l_fTotalTertiary;
}

/*!
* Calculate the resource factor, which gives a number between 0 and 1.
* Then, adjust it with the poverty factor of the country 
* (A poor country doesn't need the same resources than a rich country)
**/
REAL32 GWorldBehavior::FindResourceFactor(ENTITY_ID in_iCountryID)
{		
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	//! \todo Find a realy money factor based on that country, between 0 and 1
	REAL32 l_fMoneyFactor = 0.5;

	REAL32 l_fTotalFood = 0.0f;
	REAL32 l_fTotalEnergy = 0.0f;
	REAL32 l_fTotalRawMaterials = 0.0f;
	REAL32 l_fTotalIndustrialMaterials = 0.0f;
	REAL32 l_fTotalFinishedGoods = 0.0f;
	REAL32 l_fTotalServices = 0.0f;

	l_fTotalFood = ( ResImpCereals * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Cereals)
		+ l_CountryData->ResourceImport(SP2::EResources::Cereals)
		- l_CountryData->ResourceExport(SP2::EResources::Cereals) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Cereals) + 1.f)) ) 
		+ ( ResImpVegetablesFruits * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Vegetable_Fruits)
		+ l_CountryData->ResourceImport(SP2::EResources::Vegetable_Fruits)
		- l_CountryData->ResourceExport(SP2::EResources::Vegetable_Fruits) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Vegetable_Fruits) + 1.f)) )
		+ ( ResImpDairy * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Dairy)
		+ l_CountryData->ResourceImport(SP2::EResources::Dairy)
		- l_CountryData->ResourceExport(SP2::EResources::Dairy) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Dairy) + 1.f)) )
		+ ( ResImpMeat * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Meat)
		+ l_CountryData->ResourceImport(SP2::EResources::Meat)
		- l_CountryData->ResourceExport(SP2::EResources::Meat) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Meat) + 1.f)) )
		+ ( ResImpTobacco * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Tobacco)
		+ l_CountryData->ResourceImport(SP2::EResources::Tobacco)
		- l_CountryData->ResourceExport(SP2::EResources::Tobacco) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Tobacco) + 1.f)) )
		+ ( ResImpDrugs * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Drugs)
		+ l_CountryData->ResourceImport(SP2::EResources::Drugs)
		- l_CountryData->ResourceExport(SP2::EResources::Drugs) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Drugs) + 1.f)) );

	l_fTotalEnergy = ( ResImpElectricity * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Electricity)
		+ l_CountryData->ResourceImport(SP2::EResources::Electricity)
		- l_CountryData->ResourceExport(SP2::EResources::Electricity) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Electricity) + 1.f)) ) 
		+ ( ResImpFossileFuels * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Fossile_Fuels)
		+ l_CountryData->ResourceImport(SP2::EResources::Fossile_Fuels)
		- l_CountryData->ResourceExport(SP2::EResources::Fossile_Fuels) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Fossile_Fuels) + 1.f)) );

	l_fTotalRawMaterials = ( ResImpWoodPaper * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Wood_Paper)
		+ l_CountryData->ResourceImport(SP2::EResources::Wood_Paper)
		- l_CountryData->ResourceExport(SP2::EResources::Wood_Paper) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Wood_Paper) + 1.f)) ) 
		+ ( ResImpMinerals * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Minerals)
		+ l_CountryData->ResourceImport(SP2::EResources::Minerals)
		- l_CountryData->ResourceExport(SP2::EResources::Minerals) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Minerals) + 1.f)) )
		+ ( ResImpIronSteel * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Iron_Steel)
		+ l_CountryData->ResourceImport(SP2::EResources::Iron_Steel)
		- l_CountryData->ResourceExport(SP2::EResources::Iron_Steel) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Iron_Steel) + 1.f)) )
		+ ( ResImpPreciousStones * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Precious_Stones)
		+ l_CountryData->ResourceImport(SP2::EResources::Precious_Stones)
		- l_CountryData->ResourceExport(SP2::EResources::Precious_Stones) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Precious_Stones) + 1.f)) );

	l_fTotalIndustrialMaterials = ( ResImpFabrics * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Fabrics)
		+ l_CountryData->ResourceImport(SP2::EResources::Fabrics)
		- l_CountryData->ResourceExport(SP2::EResources::Fabrics) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Fabrics) + 1.f)) ) 
		+ ( ResImpPlastics * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Plastics)
		+ l_CountryData->ResourceImport(SP2::EResources::Plastics)
		- l_CountryData->ResourceExport(SP2::EResources::Plastics) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Plastics) + 1.f)) )
		+ ( ResImpChemicals * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Chemicals)
		+ l_CountryData->ResourceImport(SP2::EResources::Chemicals)
		- l_CountryData->ResourceExport(SP2::EResources::Chemicals) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Chemicals) + 1.f)) )
		+ ( ResImpPharmaceuticals * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Pharmaceuticals)
		+ l_CountryData->ResourceImport(SP2::EResources::Pharmaceuticals)
		- l_CountryData->ResourceExport(SP2::EResources::Pharmaceuticals) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Pharmaceuticals) + 1.f)) );

	l_fTotalFinishedGoods = ( ResImpAppliances * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Appliances)
		+ l_CountryData->ResourceImport(SP2::EResources::Appliances)
		- l_CountryData->ResourceExport(SP2::EResources::Appliances) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Appliances) + 1.f)) ) 
		+ ( ResImpVehicles * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Vehicules)
		+ l_CountryData->ResourceImport(SP2::EResources::Vehicules)
		- l_CountryData->ResourceExport(SP2::EResources::Vehicules) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Vehicules) + 1.f)) )
		+ ( ResImpMachinery * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Machinery)
		+ l_CountryData->ResourceImport(SP2::EResources::Machinery)
		- l_CountryData->ResourceExport(SP2::EResources::Machinery) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Machinery) + 1.f)) )
		+ ( ResImpComm * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Commodities)
		+ l_CountryData->ResourceImport(SP2::EResources::Commodities)
		- l_CountryData->ResourceExport(SP2::EResources::Commodities) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Commodities) + 1.f)) )
		+ ( ResImpLuxuryComm * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Luxury_Commodities)
		+ l_CountryData->ResourceImport(SP2::EResources::Luxury_Commodities)
		- l_CountryData->ResourceExport(SP2::EResources::Luxury_Commodities) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Luxury_Commodities) + 1.f)) );

	l_fTotalServices = ( ResImpConstruction * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Construction)
		+ l_CountryData->ResourceImport(SP2::EResources::Construction)
		- l_CountryData->ResourceExport(SP2::EResources::Construction) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Construction) + 1.f)) ) 
		+ ( ResImpEngineering * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Engineering)
		+ l_CountryData->ResourceImport(SP2::EResources::Engineering)
		- l_CountryData->ResourceExport(SP2::EResources::Engineering) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Engineering) + 1.f)) )
		+ ( ResImpHealth * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Health_Care)
		+ l_CountryData->ResourceImport(SP2::EResources::Health_Care)
		- l_CountryData->ResourceExport(SP2::EResources::Health_Care) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Health_Care) + 1.f)) )
		+ ( ResImpRetail * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Retail)
		+ l_CountryData->ResourceImport(SP2::EResources::Retail)
		- l_CountryData->ResourceExport(SP2::EResources::Retail) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Retail) + 1.f)) )
		+ ( ResImpLegal * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Legal_Services)
		+ l_CountryData->ResourceImport(SP2::EResources::Legal_Services)
		- l_CountryData->ResourceExport(SP2::EResources::Legal_Services) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Legal_Services) + 1.f)) )
		+ ( ResImpMarket * AdjustMod( ( l_CountryData->ResourceProduction(SP2::EResources::Marketing_Advertising)
		+ l_CountryData->ResourceImport(SP2::EResources::Marketing_Advertising)
		- l_CountryData->ResourceExport(SP2::EResources::Marketing_Advertising) )
		/(l_CountryData->ResourceDemand(SP2::EResources::Marketing_Advertising) + 1.f)) );	

	REAL32 l_fResults = (l_fTotalFood * FoodAgricultureImp) + (l_fTotalEnergy * EnergyImp) 
		+ (l_fTotalRawMaterials * RawMaterialsImp) + (l_fTotalIndustrialMaterials * IndustrialMaterialsImp) 
		+ (l_fTotalFinishedGoods * FinishedGoodsImp) + (l_fTotalServices * ServicesImp);

	//Now, readjust the result for rich/poor countries.
	REAL32 l_fResMod = (4.f * l_fMoneyFactor) - 2.f;	//will adjust the resource rating

	REAL32 l_fAdjustment = (1.f - l_fResults) * l_fResMod;
	if(l_fAdjustment > 0.2f)
		l_fAdjustment = 0.2f;
	else if (l_fAdjustment < -0.2f)
		l_fAdjustment = -0.2f;

	l_fResults -= l_fAdjustment;

	if (l_fResults > 1.f)
		return 1.f;
	if (l_fResults < 0.f)
		return 0.f;

	return l_fResults;

}

REAL32 GWorldBehavior::AdjustMod(REAL64 in_fMod)
{
	if (in_fMod > 1.f)
		return 1.f;
	if (in_fMod < 0.f)
		return 0.f;
	return (REAL32)in_fMod;
}

/*! Gets modifier for human development
*	Over 0.9:				   1
*	Between 0.7 and 0.9: [0.5 , 1]
*	Between 0.3 and 0.7:  [0, 0.5]
*	Below 0.3:              0
**/
REAL32 GWorldBehavior::FindHumanDevelopmentFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

    REAL32 l_fHDI = l_CountryData->HumanDevelopment();
	REAL32 l_fHumanDevelopmentFactor = 0.f;	

	if(l_fHDI > 0.9f)
		l_fHumanDevelopmentFactor = 1.f;
	else if (l_fHDI > 0.7f)
		l_fHumanDevelopmentFactor = ((l_fHDI-0.7f)/(0.9f-0.7f) * 0.5f) + 0.5f;	
	else if (l_fHDI > 0.3f)
		l_fHumanDevelopmentFactor = (l_fHDI-0.3f)/(0.7f-0.3f) * 0.5f;
	else
		l_fHumanDevelopmentFactor = 0.f;

	return l_fHumanDevelopmentFactor;
}

/*! Gets modifier for GDP / Population
*	Same as income index
**/
REAL32 GWorldBehavior::FindGDPPopulationFactor(ENTITY_ID in_iCountryID)
{
    return GCountryData::FindIncomeIndex(g_ServerDAL.CountryData(in_iCountryID)->GDPPerCapita(), true);
}

/*!
* National Security Funding / Population
*	Over 300 :		       1
*	Between 50 and 300:  [0.5 , 1]
*	Between 2 and 50:    [0, 0.5]
*	Below 2:              0
**/
REAL32 GWorldBehavior::FindNationalSecurityFundingFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fNatSecurityFunding = 0.0f;
	REAL32 l_fRatio = 0.0f;

	if (l_CountryData->PopulationPoliticalControl() != 0)
		l_fRatio = (REAL32)(l_CountryData->BudgetExpenseSecurity() / (REAL64)l_CountryData->PopulationPoliticalControl());

	if(l_fRatio > 300.f)
		l_fNatSecurityFunding = 1.f;
	else if (l_fRatio > 50.f)
		l_fNatSecurityFunding = ((l_fRatio-50.f)/(300.f-50.f) * 0.5f) + 0.5f;	
	else if (l_fRatio > 2.f)
		l_fNatSecurityFunding = ((l_fRatio-2.f)/(50.f-2.f) * 0.5f);
	else
		l_fNatSecurityFunding = 0.0f;

	return l_fNatSecurityFunding;
}

/*!
* Imf Receiving / Gdp
*	Over 1 :				   0
*	Between 1 and 0.01:  [0 , 0.5]
*	Between 0.01 and >0: [0.5, 1]
*	0:                   1
**/
REAL32 GWorldBehavior::FindIMFReceivingFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fIMF = 0.0f;
	REAL32 l_fRatio = 0.0f;

	if (l_CountryData->GDPValue() != 0)
		l_fRatio = (REAL32)(l_CountryData->BudgetRevenueIMF() / l_CountryData->GDPValue());

	if(l_fRatio > 1.f)
		l_fIMF = 0.f;
	else if (l_fRatio > 0.01f)
		l_fIMF = 0.5f - ((l_fRatio-0.01f)/(1.f-0.01f) * 0.5f);
	else if (l_fRatio > 0.f)
		l_fIMF = (0.5f - ((l_fRatio-0.f)/(0.01f-0.f) * 0.5f) ) + 0.5f;
	else
		l_fIMF = 1.f;

	return l_fIMF;
}

/*!
* Interest Rate
*	Over 0.2 :		       1
*	Between 0.05 and 0.2: [0.5 , 1]
*	Between 0 and 0.05:   [0, 0.5]
*	0:                    0
**/
REAL32 GWorldBehavior::FindInterestRateFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fInterestRate = 0.0f;
	REAL32 l_fRatio = l_CountryData->InterestLevel();

	if(l_fRatio > 0.2f)
		l_fInterestRate = 1.f;
	else if (l_fRatio > 0.05f)
		l_fInterestRate = ((l_fRatio-0.05f)/(0.2f-0.05f) * 0.5f) + 0.5f;	
	else if (l_fRatio > 0.f)
		l_fInterestRate = ((l_fRatio-0.f)/(0.05f-0.f) * 0.5f);
	else
		l_fInterestRate = 0.0f;

	return l_fInterestRate;
}

/*!
* Inflation Level
*	Over 0.2 :		         0
*	Between 0.05 and 0.2:   [0.5 , 0]
*	Between 0.02 and 0.05:  [1, 0.5]
*	Below 0.02:             1
**/
REAL32 GWorldBehavior::FindInflationLevelFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fInflationLevel = 0.0f;
	REAL32 l_fRatio = l_CountryData->InflationLevel();

	if(l_fRatio > 0.2f)
		l_fInflationLevel = 0.f;
	else if (l_fRatio > 0.05f)
		l_fInflationLevel = 0.5f - ((l_fRatio-0.05f)/(0.2f-0.05f) * 0.5f);
	else if (l_fRatio > 0.02f)
		l_fInflationLevel = (0.5f - ((l_fRatio-0.02f)/(0.05f-0.02f) * 0.5f) ) + 0.5f;
	else
		l_fInflationLevel = 1.f;

	return l_fInflationLevel;
}

/*!
* Unemployment
*	Over 0.4 :				 0
*	Between 0.4 and 0.1:  [0 , 0.5]
*	Between 0.1 and >0:   [0.5, 1]
*	0:                    1
**/
REAL32 GWorldBehavior::FindUnemploymentFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fUnemployment = 0.0f;
	REAL32 l_fRatio = l_CountryData->Pop1565Unemployed();

	if(l_fRatio > 0.4f)
		l_fUnemployment = 0.f;
	else if (l_fRatio > 0.1f)
		l_fUnemployment = 0.5f - ((l_fRatio-0.1f)/(0.4f-0.1f) * 0.5f);
	else if (l_fRatio > 0.f)
		l_fUnemployment = (0.5f - ((l_fRatio-0.f)/(0.1f-0.f) * 0.5f) ) + 0.5f;
	else
		l_fUnemployment = 1.f;

	return l_fUnemployment;
}

/*!
* Unemployment
*	Best 0.04				  0.5
*	Between 0 and 0.04:    [1, 0.5]
*	Between 0.04 and 0.15: [0.5, 0.1]
*	Over 0.15:             [0.1, 0]
**/
REAL32 GWorldBehavior::FindUnemploymentFactorForInflation(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fUnemployment = 0.5f;
	REAL32 l_fRatio = l_CountryData->Pop1565Unemployed();

	if(l_fRatio < 0.04f)
		l_fUnemployment = (-12.5f * l_fRatio) + 1.f;
	else if (l_fRatio < 0.15f)
		l_fUnemployment = (-3.636363f * l_fRatio) + 0.6454545454f;
	else
		l_fUnemployment = (-0.117647058f * l_fRatio) + 0.117647058f;


	return l_fUnemployment;
}

/*!
* GDP Growth
*	Over 0.1 :		       1
*	Between 0.05 and 0.1: [0.5 , 1]
*	Between 0 and 0.05:   [0, 0.5]
*	Below 0:              0
**/
REAL32 GWorldBehavior::FindGDPGrowthFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fGDPGrowth = 0.0f;
	REAL32 l_fRatio = l_CountryData->GDPGrowth();

	if(l_fRatio > 0.1f)
		l_fGDPGrowth = 1.f;
	else if (l_fRatio > 0.05f)
		l_fGDPGrowth = ((l_fRatio-0.05f)/(0.1f-0.05f) * 0.5f) + 0.5f;	
	else if (l_fRatio > 0.f)
		l_fGDPGrowth = ((l_fRatio-0.f)/(0.05f-0.f) * 0.5f);
	else
		l_fGDPGrowth = 0.0f;

	return l_fGDPGrowth;
}

/*!
* Demographic Financing Factor
*	Returns a value between 0 and 1, which describes the country's strength on those 3 points:
* - Health Care
* - Education
*	- Telecom
**/
REAL32 GWorldBehavior::FindDemographicFinancingFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_pCountryData)
		return 0.f;

	return (REAL32)(l_pCountryData->BudgetExpenseHealthcareRatio() + 
		l_pCountryData->BudgetExpenseEducationRatio() + 
		l_pCountryData->BudgetExpenseTelecomRatio()) / 3.f;
}

/*!
* Personal Income Tax
*	Over 0.6 :				 0
*	Between 0.6 and 0.35:  [0 , 0.5]
*	Between 0.35 and 0.05: [0.5, 1]
*	Below 0.05:           1
**/
REAL32 GWorldBehavior::FindPersonalIncomeTaxFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fPersonalIncomeTax = 0.0f;
	REAL64 l_fRatio = l_CountryData->PersonalIncomeTax();

	if(l_fRatio > 0.6f)
		l_fPersonalIncomeTax = 0.f;
	else if (l_fRatio > 0.35f)
		l_fPersonalIncomeTax = (REAL32)(0.5f - ((l_fRatio-0.35f)/(0.6f-0.35f) * 0.5f));
	else if (l_fRatio > 0.05f)
		l_fPersonalIncomeTax = (REAL32)((0.5f - ((l_fRatio-0.05f)/(0.35f-0.05f) * 0.5f) ) + 0.5f);
	else
		l_fPersonalIncomeTax = 1.f;

	return l_fPersonalIncomeTax;
}

REAL32 GWorldBehavior::FindTaxBudgetFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL64 l_fPersonalIncomeTax = l_CountryData->PersonalIncomeTax();
	REAL64 l_fExpectedAverageExpenses =  max(0.f, (2.f * l_fPersonalIncomeTax) - 0.2f);
	REAL64 l_fAverageExpenses = (l_CountryData->BudgetExpenseEducationRatio() + 
										 l_CountryData->BudgetExpenseEnvironmentRatio() + 
										 l_CountryData->BudgetExpenseHealthcareRatio() + 
										 l_CountryData->BudgetExpenseTelecomRatio() + 
										 l_CountryData->BudgetExpenseInfrastructureRatio()) / 5.f;
	if(l_fExpectedAverageExpenses == 0.f)
		return 1.f;
	REAL32 l_fRatio = min(1.f,(REAL32)(l_fAverageExpenses / l_fExpectedAverageExpenses));
	
	return l_fRatio;
}

/*!
* Military Force
* - GDP Ratio 
*	Over 50 :		       1
*	Between 5 and 50:    [0.5 , 1]
*	Between 0.2 and 5:   [0, 0.5]
*	Below 0.2:            0
* - Area Land Ratio
*	Over 10000000 :						1
*	Between 5000000 and 10000000:   [0.5 , 1]
*	Between 50000 and 5000000:      [0, 0.5]
*	Below 50000:		               0
**/
REAL32 GWorldBehavior::FindMilitaryForceFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	const REAL32 GDP_IMPORTANCE	= 0.4f;
	const REAL32 LAND_IMPORTANCE	= 0.6f;

	REAL32 l_fMilitaryLevel = 0.f;
	REAL32 l_fGDPRatio = 0.f;
	REAL32 l_fLandRatio = 0.f;
	REAL32 l_fTotalUnitsValue = g_ServerDAL.TotalUnitsValue(in_iCountryID);
	REAL32 l_fGDP = 0.f;
	REAL32 l_fLand = 0.f;

	if(l_CountryData->GDPValue() != 0.f)
		l_fGDPRatio = (REAL32)((REAL64)l_fTotalUnitsValue / l_CountryData->GDPValue());

	if(l_CountryData->AreaLandTotal() != 0.f)
		l_fLandRatio = l_fTotalUnitsValue / l_CountryData->AreaLandTotal();

	if(l_fGDPRatio > 50.f)
		l_fGDP = 1.f;
	else if (l_fGDPRatio > 5.f)
		l_fGDP = ((l_fGDPRatio-5.f)/(50.f-5.f) * 0.5f) + 0.5f;	
	else if (l_fGDPRatio > 0.2f)
		l_fGDP = ((l_fGDPRatio-0.2f)/(5.f-0.2f) * 0.5f);
	else
		l_fGDP = 0.0f;

	if(l_fLandRatio > 10000000.f)
		l_fLand = 1.f;
	else if (l_fLandRatio > 5000000.f)
		l_fLand = ((l_fLandRatio-5000000.f)/(10000000.f-5000000.f) * 0.5f) + 0.5f;	
	else if (l_fLandRatio > 50000.f)
		l_fLand = ((l_fLandRatio-50000.f)/(5000000.f-50000.f) * 0.5f);
	else
		l_fLand = 0.0f;

	l_fMilitaryLevel = (GDP_IMPORTANCE * l_fGDP) + (LAND_IMPORTANCE * l_fLand);

	return l_fMilitaryLevel;
}

REAL32 GWorldBehavior::FindEconomicHealth(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	const REAL32 PourcInfrastructure		= 0.4f;
	const REAL32 PourcHumanDevelopment	= 0.35f;
	const REAL32 PourcInterestRate		= 0.25f;

	/*
	What you need to have a perfect economic health!:
	- Infrastructure of 100%
	- Human Development level of 100%
	- Interest rate at 0%
	*/

	REAL32 l_fInterestRate = l_CountryData->InterestLevel();
	REAL32 l_fInterestRateMod = 0.f;
	if(l_fInterestRate > 0.02f)
		l_fInterestRateMod = (((0.2f-l_fInterestRate))/(0.2f-0.02f)) * 0.5f;
	else
		l_fInterestRateMod = (-25.f * l_fInterestRate) + 1.f;
	REAL32 l_fHD = FindHumanDevelopmentFactor(in_iCountryID);

	REAL32 l_fInfrastructure = l_CountryData->Infrastructure();	

	REAL32 l_fEconomicHealth = (l_fInfrastructure * PourcInfrastructure) 
		+ (l_fInterestRateMod * PourcInterestRate)	
		+ (l_fHD * PourcHumanDevelopment);

	return l_fEconomicHealth;
}

/*!
* Population in poverty
*	Over 0.7 :				 0
*	Between 0.7 and 0.2:  [0 , 0.5]
*	Between 0.2 and 0:    [0.5, 1]
*	Below 0:              1
**/
REAL32 GWorldBehavior::FindPopulationInPovertyFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fPopInPoverty = 0.0f;
	REAL32 l_fRatio = l_CountryData->PopInPoverty();

	if(l_fRatio > 0.7f)
		l_fPopInPoverty = 0.f;
	else if (l_fRatio > 0.2f)
		l_fPopInPoverty = 0.5f - ((l_fRatio-0.2f)/(0.7f-0.2f) * 0.5f);
	else if (l_fRatio > 0.f)
		l_fPopInPoverty = (0.5f - ((l_fRatio-0.f)/(0.2f-0.f) * 0.5f) ) + 0.5f;
	else
		l_fPopInPoverty = 1.f;

	return l_fPopInPoverty;
}

/*!
* Population in poverty
*	Over 1000/km :					0
*	Between 114/km and 1000/km:[0 , 0.5]
*	Between 1/km and 114/km:   [0.5, 1]
*	Below 1/km:						1
**/
REAL32 GWorldBehavior::FindPopulationDensityFactor(ENTITY_ID in_iCountryID)
{
	GCountryData* l_CountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(!l_CountryData)
		return 0.f;

	REAL32 l_fPopDensity = 0.0f;
	REAL32 l_fRatio = l_CountryData->Population() / l_CountryData->AreaTotal();

	if(l_fRatio > 1000.f)
		l_fPopDensity = 0.f;
	else if (l_fRatio > 114.f)
		l_fPopDensity = 0.5f - ((l_fRatio-114.f)/(1000.f-114.f) * 0.5f);
	else if (l_fRatio > 1.f)
		l_fPopDensity = (0.5f - ((l_fRatio-1.f)/(114.f-1.f) * 0.5f) ) + 0.5f;
	else
		l_fPopDensity = 1.f;

	return l_fPopDensity;
}

INT64 GWorldBehavior::ReturnInteger64(REAL32 in_fValue)
{
	if(in_fValue >= 1.f)
		return (INT64)in_fValue;
	REAL32 in_fRandom = GetRandom(1.f);
	if(in_fRandom < in_fValue)
		return 1;
	return 0;
}

INT32 GWorldBehavior::ReturnInteger32(REAL32 in_fValue)
{
	if(in_fValue >= 1.f)
		return (INT32)in_fValue;
	REAL32 in_fRandom = GetRandom(1.f);
	if(in_fRandom < in_fValue)
		return 1;
	return 0;
}

void GWorldBehavior::ExecuteElections(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	REAL32 l_fApproval = l_pCountryData->GvtApproval();

	//Modify approval based on government funding
	REAL32 l_fApprovalModifier = -0.05f + (0.1f * (REAL32)l_pCountryData->BudgetExpenseGovernmentRatio());
	l_fApproval += l_fApprovalModifier;
	if(l_fApproval < 0.f)
		l_fApproval = 0.f;
	else if(l_fApproval > 1.f)
		l_fApproval = 1.f;

	REAL32 l_fPopulationPolIdeology = l_pCountryData->PopPoliticalIdeology();
	INT32 l_iFormerLeaderId = l_pCountryData->LeaderParty()->Id();	
	const vector<SP2::GPoliticalParty>& l_vParties = l_pCountryData->PoliticalParty();

	REAL32 l_fSumOfVotes = 0.f;
	REAL32 l_fTempVote = 0.f;

	hash_map<INT32,REAL32> l_PartiesVotes;

	for(UINT32 i=0; i<l_vParties.size(); i++)
	{
		if(l_vParties[i].GvtType() == EGovernmentType::MultiPartyDemocracy)
		{
			if(l_vParties[i].Status() == 2)
			{
				l_fTempVote = 0.f;
			}
			else
			{
				if(l_iFormerLeaderId == l_vParties[i].Id())
					l_fTempVote = (l_fApproval)*
					(1 - fabs(l_vParties[i].PoliticalIdeology()-l_fPopulationPolIdeology));
				else
					l_fTempVote = (1.f - l_fApproval)*
					(1 - fabs(l_vParties[i].PoliticalIdeology()-l_fPopulationPolIdeology));				
			}
			l_fSumOfVotes += l_fTempVote;	
			l_PartiesVotes[l_vParties[i].Id()] = l_fTempVote;
		}
	}

	if(l_fSumOfVotes == 0.f)
	{
		//nothing changes
		return;
	}

	REAL32 l_fMaxPerc = 0.f;
	INT32 l_iNewLeaderId = 0;
	//Change the percentage values of political parties
	for(UINT32 i=0; i<l_vParties.size(); i++)
	{
		if(l_vParties[i].GvtType() == EGovernmentType::MultiPartyDemocracy)
		{
			l_pCountryData->ChangePoliticalPartyPercentageValue(l_vParties[i].Id(),
				l_PartiesVotes[l_vParties[i].Id()] / l_fSumOfVotes);
			if(l_vParties[i].PercentageValue() > l_fMaxPerc)
			{
				l_fMaxPerc = l_vParties[i].PercentageValue();
				l_iNewLeaderId = l_vParties[i].Id();
			}

		}
	}

	//Set the InPower variable
	for(UINT32 i=0; i<l_vParties.size(); i++)
	{
		if(l_vParties[i].GvtType() == EGovernmentType::MultiPartyDemocracy)
		{
			if(l_vParties[i].Id() == l_iNewLeaderId)
				l_pCountryData->ChangePoliticalPartyInPower(l_vParties[i].Id(),true);
			else
				l_pCountryData->ChangePoliticalPartyInPower(l_vParties[i].Id(),false);
		}
	}

	//Change the leading party
	l_pCountryData->LeadingPoliticalPartyID(l_iNewLeaderId);

	News::GInfo l_News;
	vector<INT32> l_vStrings;
	l_vStrings.push_back(l_pCountryData->LeaderParty()->NameId());
	l_News.m_iCountryA = in_iCountryID;
	l_News.m_eType = News::EType::Elections;
	l_News.m_fGameTime = g_Joshua.GameTime();
	l_News.m_iPriority = g_ServerDAL.NewsPriority(l_News.m_eType);
	l_News.m_eCategory = g_ServerDAL.NewsCategory(l_News.m_eType);
	l_News.m_vStrings = l_vStrings;
	g_ServerDCL.SendNews(l_News);

	if(l_iNewLeaderId != l_iFormerLeaderId)
		NotifyObserver(c_iNotificationLeadingPoliticalPartyChangeAfterElections,(void*)in_iCountryID);
}

void GWorldBehavior::EconomicFriends(UINT32 in_iCountryID, set<UINT32>& out_Friends) const
{
	out_Friends.clear();
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	for(hash_map<UINT32,GTreaty>::const_iterator l_Itr = l_Treaties.begin();
		l_Itr != l_Treaties.end();
		l_Itr++)
	{
		const GTreaty& l_Treaty = l_Itr->second;
		if(l_Treaty.Active() && 
			(l_Treaty.Type() == ETreatyType::ResearchPartnership ||
			l_Treaty.Type() == ETreatyType::EconomicPartnership ||
			l_Treaty.Type() == ETreatyType::CommonMarket) &&
			l_Treaty.CountrySide(in_iCountryID) == 1)
		{
			//add to the friends every country, except the main country
			out_Friends.insert(l_Treaty.MembersSideA(true).begin(),
				l_Treaty.MembersSideA(true).end());
		}
		out_Friends.erase(in_iCountryID);
	}
}

bool GWorldBehavior::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_iInternalCounter
             << m_fNextIteration
             << m_fNextNewsIteration
             << m_fNextRelationsTreatiesIteration
             << m_fLastBudgetIteration
             << m_fLastStabilityApprovalIteration
				 << m_fLastNukeReadyIteration
             << m_QuickRandom;

   return true;
}

bool GWorldBehavior::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_iInternalCounter
             >> m_fNextIteration
             >> m_fNextNewsIteration
             >> m_fNextRelationsTreatiesIteration
             >> m_fLastBudgetIteration
             >> m_fLastStabilityApprovalIteration
				 >> m_fLastNukeReadyIteration
             >> m_QuickRandom;

   return true;
}

bool GWorldBehavior::OnNew(GDatabase* in_pDatabase)
{
   Reset();

   return true;
}

void GWorldBehavior::OnClean()
{
}
