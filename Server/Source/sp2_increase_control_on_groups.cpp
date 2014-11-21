#include "golem_pch.hpp"


using namespace EHEActions;


GIncreaseControlOnGroups* GIncreaseControlOnGroups::Clone() const
{
   return new GIncreaseControlOnGroups( *this ); 
}


void GIncreaseControlOnGroups::Execute()
{
	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );
	if ( ! pEntity ) return;	

	//A country can do 1 thing here. 
	/*
		1: Make political parties illegal

		-- To be added later on
		-Make pressure groups illegal
	*/

	//First, let's give a number between -1 and 1 to every possibilites. The one with the lowest
	//value will be executed.
	REAL32 l_Values[1];

	//Sets the political parties status level
	REAL32 l_fPartyStatus = 0.0f;
	vector< GPoliticalParty > l_vParty = g_ServerDAL.CountryData(m_iSource)->PoliticalParty();
	//For each party in a country, multiply its pourcentage by a modifier.
	// 1 if Legal
	// 5 if Illegal
	// -2 if Official
	EGovernmentType::Enum l_iType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_iType == EGovernmentType::MultiPartyDemocracy) //only in multi party democracy can we choose the political party status
	{			
		for(UINT32 i=0; i < l_vParty.size(); i++)
		{
			switch(l_vParty[i].Status())
			{
				case 1:	//Legal
					l_fPartyStatus += 1;
					break;
				case 2:	//Illegal
					l_fPartyStatus += 5;
					break;
				case 3:	//Official
					l_fPartyStatus += -2;
					break;
				default:	//Should not happen
					l_fPartyStatus += 0;
					break;
			}			
		}
	}
	l_Values[0] = GAI::VerifyValues(l_fPartyStatus);

	//now, we go through the array, and take the lowest value. 
	REAL32 l_fMin = l_Values[0];
	UINT32 l_iMinPosition = 0;
	for(UINT32 i=0; i < 1; i++)
	{
		if (l_Values[i] < l_fMin)
		{
			l_fMin = l_Values[i];		
			l_iMinPosition = i;
		}
	}

	//now that we know the action to execute, we do it
	switch(l_iMinPosition)
	{
		case 0:	//Open the immigration
			ExecutePartyStatus(pEntity);			
			break;		
		default:	//Should not happen
			break;
	}	

}

void GIncreaseControlOnGroups::ExecutePartyStatus(const GEntity* in_pEntity) const
{
	//select the party
	INT32 l_iID = 0;
	REAL32 l_fMinPourc = 9999.f;
	UINT32 l_iTotalParties = 0;
	if(g_ServerDAL.CountryData(in_pEntity->Id())->GvtType() == EGovernmentType::MultiPartyDemocracy)
	{
		for(UINT32 i=0; i < g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty().size(); i++)
		{
			if(g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty()[i].GvtType() != 9)
				continue;
			if(g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty()[i].Status() == 1)
			{
				l_iTotalParties++;
				if(g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty()[i].PercentageValue() < l_fMinPourc)
				{
					l_fMinPourc = g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty()[i].PercentageValue();
					l_iID = g_ServerDAL.CountryData(in_pEntity->Id())->PoliticalParty()[i].Id();
				}
			}
		}
		
		//At least 1 party must remain legal, so the number of results must be <= 1
		if (l_iTotalParties <= 1)
		{
			//We can't do anything on parties, we quit
			return;
		}
		else
		{
			g_ServerDCL.ChangePoliticalPartyStatus(in_pEntity->Id(),l_iID,1,2);				
		}
	}
	
}

bool GIncreaseControlOnGroups::CheckHardConditions() const
{
	//select the party
	UINT32 l_iTotalParties = 0;
	for(UINT32 i=0; i < g_ServerDAL.CountryData(m_iSource)->PoliticalParty().size(); i++)
	{
		if(g_ServerDAL.CountryData(m_iSource)->PoliticalParty()[i].Status() == 1)
		{			
			//If there is one group legal, then we can increase control on groups
			l_iTotalParties++;
		}
	}

	if (l_iTotalParties <= 1)
	{
		//We can't do anything on parties, return false
		return false;
	}

	if(g_ServerDCL.CalculateProbabilityOfAction(g_ServerDAL.CountryData(m_iSource),
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionNeutral) > 0.5f)
		return true;
	else
		return false;
	

}