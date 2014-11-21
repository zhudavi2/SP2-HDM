#include "golem_pch.hpp"


using namespace EHEActions;


GDecreaseControlOnCountry* GDecreaseControlOnCountry::Clone() const
{
   return new GDecreaseControlOnCountry( *this ); 
}


void GDecreaseControlOnCountry::Execute()
{
	//A country can do 2 things here. 
	/*
		1: Removing Martial Law
		2: Give freedom of demonstration
		3: Change the government type

		-- To be added later on
	*/
	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	//If a country has the martial law ON, it turns it OFF now.
	if (l_pCountryData->MartialLaw())
	{
		g_ServerDCL.ChangeMartialLaw(m_iSource,false);		
		return;
	}

	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
	{
		g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::FreedomOfDemonstration,true);
			return;
	}

	//Else, the only thing left to do is change the government type.
	EGovernmentType::Enum l_GvtType = (EGovernmentType::Enum)l_pCountryData->GvtType();
	
	if(l_GvtType == EGovernmentType::Anarchy)
		return;	
	if(g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionDoubleMinus) <= 0.5f)
		return;

	REAL32 l_fPoliticalParties[EGovernmentType::ItemCount];
	Memory::Fill<REAL32>(l_fPoliticalParties,0,EGovernmentType::ItemCount);

	l_fPoliticalParties[EGovernmentType::MultiPartyDemocracy] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionNeutral);
	l_fPoliticalParties[EGovernmentType::SinglePartyDemocracy] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionNeutral);
	l_fPoliticalParties[EGovernmentType::Communist] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral);
	l_fPoliticalParties[EGovernmentType::Monarchy] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral);
	l_fPoliticalParties[EGovernmentType::Theocracy] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus);
	l_fPoliticalParties[EGovernmentType::MilitaryDictatorship] = 
		g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionNeutral);


	REAL32 l_fMax = 0.f;
	EGovernmentType::Enum l_MaxType = (EGovernmentType::Enum)0;
	for(UINT32 i=0; i<EGovernmentType::ItemCount; i++)
	{
		if(l_fPoliticalParties[i] > l_fMax)
		{
			l_fMax = l_fPoliticalParties[i];
			l_MaxType = (EGovernmentType::Enum)i;
		}
	}

	if(l_MaxType > 0)
		g_ServerDCL.ChangeGovernmentType(m_iSource, l_GvtType, l_MaxType);			
}

bool GDecreaseControlOnCountry::CheckHardConditions() const
{
	if (g_ServerDAL.CountryData(m_iSource)->MartialLaw() == false)
	{
		INT32 l_iGvtType = g_ServerDAL.CountryData(m_iSource)->GvtType();
		
		if(l_iGvtType == 1 || l_iGvtType == 2 || l_iGvtType == 9)
		{
			//If the country is in Anarchy, Conquered, or Already at the top of the government type, do nothing
			return false;
		}
		else
		{
			if(g_ServerDCL.CalculateProbabilityOfAction(g_ServerDAL.CountryData(m_iSource),
																		SP2::c_fAIActionNeutral,
																		SP2::c_fAIActionNeutral,
																		SP2::c_fAIActionNeutral,
																		SP2::c_fAIActionDoubleMinus) > 0.5f)
				return true;
			else
				return false;
		}
	}

	return true;
}