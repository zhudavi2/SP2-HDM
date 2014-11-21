#include "golem_pch.hpp"


using namespace EHEActions;


GDecreaseControlOnPopulation* GDecreaseControlOnPopulation::Clone() const
{
   return new GDecreaseControlOnPopulation( *this ); 
}


void GDecreaseControlOnPopulation::Execute()
{
	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );
	if ( ! pEntity ) return;	

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	const INT32 l_iNumberOfOptions = 6;
	//A country can do 5 things here. 
	/*
		1: Lower the suffrage age
		2: Open the immigration		
		3: Open the emigration		
		4: Change internal laws
		5: Change the status of a religion from legal to official, or from illegal to legal
		6: Change the status of a language from legal to official, or from illegal to legal
	*/
	
	//First, let's give a number between -1 and 1 to every possibilites. The one with the lowest
	//value will be executed.
	REAL32 l_Values[l_iNumberOfOptions];

	//Sets suffrage age
	//If the suffrage age is <= 18, level is 1
	//If the suffrage age is >= 25, level is -1
	INT32 l_iSuffrage = l_pCountryData->Suffrage();
	REAL32 l_fSuffrage = 0.0f;
	if (l_iSuffrage <= 18)
		l_fSuffrage = 1.0f;
	else if (l_iSuffrage >= 25)
		l_fSuffrage = -1.0f;
	else
	{
		switch(l_iSuffrage)
		{
			case 19:	
				l_fSuffrage = 0.66f;
				break;
			case 20:	
				l_fSuffrage = 0.33f;
				break;
			case 21:	
				l_fSuffrage = 0.0f;
				break;
			case 22:	
				l_fSuffrage = -0.25f;
				break;
			case 23:	
				l_fSuffrage = -0.5f;
				break;
			case 24:	
				l_fSuffrage = -0.75f;
				break;
			default:	//Should not happen
				l_fSuffrage = 0.0f;
				break;
		}	
	}
	l_Values[0] = l_fSuffrage;

	//Sets immigration level
	if (l_pCountryData->ImmigrationClosed())
	{
		l_Values[1] = -1.0f;
	}
	else
	{
		l_Values[1] = 1.0f;
	}
	
	//Sets emigration level
	if (l_pCountryData->EmigrationClosed())
	{
		l_Values[2] = -1.0f;
	}	
	else
	{
		l_Values[2] = 1.0f;
	}		

	l_Values[3] = 1.f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::Abortion))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::Contraception))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::SameSexMarriage))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::WomenSuffrage))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::Polygamy))
		l_Values[3] -= 0.25f;
	if(l_pCountryData->InternalLaw(EInternalLaws::ChildLabor))
		l_Values[3] -= 0.25f;

	if(g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionPlus) <= 0.5f)
	{
		//That country will not change its languages nor religions statuses
		l_Values[4] = 1.f;
		l_Values[5] = 1.f;
	}
	else
	{
		//Sets religion level
		REAL32 l_fReligionStatus = 0.0f;
		const GReligionList& l_Religion = l_pCountryData->GetReligions();
		//For each religion in a country, multiply its pourcentage by a modifier.
		// 1 if Legal
		// -5 if Illegal
		// 2 if Official
		for(GReligionList::const_iterator l_Itr = l_Religion.begin();
				l_Itr != l_Religion.end();
				l_Itr++)
		{
			switch(l_pCountryData->ReligionGetStatus(l_Itr->first))
			{
				case 1:	//Legal
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first);
					break;
				case 2:	//Illegal
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first) * -5.0f;
					break;
				case 3:	//Official
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first) * 2.0f;
					break;
				default:	//Should not happen
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first);
					break;
			}			
		}
		l_Values[4] = GAI::VerifyValues(l_fReligionStatus);

		//Sets the language level
		REAL32 l_fLanguageStatus = 0.0f;	
		const GLanguageList& l_Language = l_pCountryData->GetLanguages();
		//For each language in a country, multiply its pourcentage by a modifier.
		// 1 if Legal
		// -5 if Illegal
		// 2 if Official
		for(GLanguageList::const_iterator l_Itr = l_Language.begin();
				l_Itr != l_Language.end();
				l_Itr++)
		{
			switch(l_pCountryData->LanguageGetStatus(l_Itr->first))
			{
				case 1:	//Legal
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first);
					break;
				case 2:	//Illegal
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first) * -5.0f;
					break;
				case 3:	//Official
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first) * 2.0f;
					break;
				default:	//Should not happen
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first);
					break;
			}			
		}
		l_Values[5] = GAI::VerifyValues(l_fLanguageStatus);
	}
	//now, we go through the array, and take the lowest value. 
	REAL32 l_fMin = l_Values[0];
	UINT32 l_iMinPosition = 0;
	for(UINT32 i=0; i < l_iNumberOfOptions; i++)
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
		case 0:	//Lower Suffrage Age			
			ExecuteLowerSuffrage(pEntity);			
			break;
		case 1:	//Open the immigration
			ExecuteOpenImmigration(pEntity);			
			break;
		case 2:	//Open the emigration			
			ExecuteOpenEmigration(pEntity);			
			break;
		case 3:	//Open the emigration			
			ExecuteChangeLaws(pEntity);			
			break;
		case 4:	//Change the status of a religion from legal to official, or from illegal to legal
			ExecuteStatusReligion(pEntity);
			break;
		case 5:	//Change the status of a language from legal to official, or from illegal to legal
			ExecuteStatusLanguage(pEntity);
			break;
		default:	//Should not happen
			break;
	}	
}

void GDecreaseControlOnPopulation::ExecuteChangeLaws(const GEntity* in_pEntity) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pEntity->Id());

	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionDoublePlus,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionMinus) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::FreedomOfSpeech,true);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::WomenSuffrage) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionDoubleMinus,
																	  SP2::c_fAIActionMinus) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::WomenSuffrage,true);
	}	
	else if(l_pCountryData->InternalLaw(EInternalLaws::ChildLabor) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::ChildLabor,false);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::NumberOfChildren,false);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::Polygamy) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Polygamy,true);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::Contraception) && 
			  l_pCountryData->LeaderParty()->PoliticalIdeology() <= 0.6f &&
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Contraception,true);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::Abortion) && 
			  l_pCountryData->LeaderParty()->PoliticalIdeology() <= 0.5f &&
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Abortion,true);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::SameSexMarriage) && 
			  l_pCountryData->LeaderParty()->PoliticalIdeology() <= 0.5f &&
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionDoubleMinus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::SameSexMarriage,true);
	}
	else
		ExecuteStatusReligion(in_pEntity);
}


void GDecreaseControlOnPopulation::ExecuteLowerSuffrage(const GEntity* in_pEntity) const
{
	INT32 l_iSuffrage = g_ServerDAL.CountryData(in_pEntity->Id())->Suffrage();
	INT32 l_iNewAge = l_iSuffrage;
	if(m_fStrength >= 2.5f)
		l_iNewAge -= 2;
	else
		l_iNewAge -= 1;

	//g_ServerDCL.ChangeSuffrageAge(in_pEntity->Id(),l_iSuffrage,l_iNewAge);
	
}
void GDecreaseControlOnPopulation::ExecuteOpenEmigration(const GEntity* in_pEntity) const
{
	g_ServerDCL.ChangeEmigration(in_pEntity->Id(),false);	
}
void GDecreaseControlOnPopulation::ExecuteOpenImmigration(const GEntity* in_pEntity) const
{
	g_ServerDCL.ChangeImmigration(in_pEntity->Id(),false);	
}
void GDecreaseControlOnPopulation::ExecuteStatusReligion(const GEntity* in_pEntity) const
{
	//Check the maximum pourcentage it will affect
	REAL32 l_fMaxPourc = m_fStrength * 0.2f;
	if (l_fMaxPourc > 1.0f)
		l_fMaxPourc = 1.0f;
	else if (l_fMaxPourc < 0.0f)
		l_fMaxPourc = 0.0f;
	
	//select the religion
	INT32 l_iID = 0;
	INT32 l_iStatus = 0;
	REAL64 l_fTempMaxPourc = 0.f;
	UINT32 l_iTotalReligions = 0;
	REAL32 l_fPourc = 0.0f;

	const GReligionList& l_Religion = g_ServerDAL.CountryData(in_pEntity->Id())->GetReligions();

	for(GReligionList::const_iterator l_Itr = l_Religion.begin();
			l_Itr != l_Religion.end();
			l_Itr++)
	{
		l_fPourc = g_ServerDAL.CountryData(in_pEntity->Id())->ReligionGetPourcentage(l_Itr->first);
		if(g_ServerDAL.CountryData(in_pEntity->Id())->ReligionGetStatus(l_Itr->first) != 3 && l_fPourc < l_fMaxPourc)
		{			
			l_iTotalReligions++;
			if (l_fPourc >= l_fTempMaxPourc)
			{
				l_iID = l_Itr->first;
				l_fTempMaxPourc = l_fPourc;
				l_iStatus = g_ServerDAL.CountryData(in_pEntity->Id())->ReligionGetStatus(l_Itr->first);
			}
				
		}
	}

	if (l_iTotalReligions == 0)
	{
		//We can't do anything on religons, so let's try languages
		ExecuteStatusLanguage(in_pEntity);
	}
	else
	{
		INT32 l_iNewStatus = 1;
		if (l_iStatus == 2)
			l_iNewStatus = 1;
		else if (l_iStatus == 1)
			l_iNewStatus = 3;

		g_ServerDCL.ChangeReligionStatus(in_pEntity->Id(),l_iID,l_iStatus,l_iNewStatus);	
		

	}
	
}
void GDecreaseControlOnPopulation::ExecuteStatusLanguage(const GEntity* in_pEntity) const
{
	//Check the maximum pourcentage it will affect
	REAL32 l_fMaxPourc = m_fStrength * 0.2f;
	if (l_fMaxPourc > 1.0f)
		l_fMaxPourc = 1.0f;
	else if (l_fMaxPourc < 0.0f)
		l_fMaxPourc = 0.0f;
		
	//select the language
	INT32 l_iID = 0;
	INT32 l_iStatus = 0;
	REAL64 l_fTempMaxPourc = 0.f;
	UINT32 l_iTotalLanguages = 0;
	REAL32 l_fPourc = 0.0f;
	
	const GLanguageList& l_Language = g_ServerDAL.CountryData(in_pEntity->Id())->GetLanguages();

	for(GLanguageList::const_iterator l_Itr = l_Language.begin();
			l_Itr != l_Language.end();
			l_Itr++)
	{
		l_fPourc = g_ServerDAL.CountryData(in_pEntity->Id())->LanguageGetPourcentage(l_Itr->first);
		if(g_ServerDAL.CountryData(in_pEntity->Id())->LanguageGetStatus(l_Itr->first) != 3 && l_fPourc < l_fMaxPourc)
		{			
			l_iTotalLanguages++;
			if (l_fPourc >= l_fTempMaxPourc)
			{
				l_iID = l_Itr->first;
				l_fTempMaxPourc = l_fPourc;
				l_iStatus = g_ServerDAL.CountryData(in_pEntity->Id())->LanguageGetStatus(l_Itr->first);
			}
				
		}
	}

	if (l_iTotalLanguages == 0)
	{
		//We can't do anything on religons, so let's try lowering suffrage age
		ExecuteLowerSuffrage(in_pEntity);
	}
	else
	{
		INT32 l_iNewStatus = 1;
		if (l_iStatus == 2)
			l_iNewStatus = 1;
		else if (l_iStatus == 1)
			l_iNewStatus = 3;

		g_ServerDCL.ChangeLanguageStatus(in_pEntity->Id(),l_iID,l_iStatus,l_iNewStatus);	
		
	}
}

bool GDecreaseControlOnPopulation::CheckHardConditions() const
{
	//It checks if there's one thing it can do
	
	if (g_ServerDAL.CountryData(m_iSource)->Suffrage() <= 18
			&& g_ServerDAL.CountryData(m_iSource)->ImmigrationClosed() == false
			&& g_ServerDAL.CountryData(m_iSource)->EmigrationClosed() == false)
	{
		bool l_bContinue = false;

		const GReligionList& l_Religion = g_ServerDAL.CountryData(m_iSource)->GetReligions();

		for(GReligionList::const_iterator l_Itr = l_Religion.begin();
				l_Itr != l_Religion.end();
				l_Itr++)
		{
			if(g_ServerDAL.CountryData(m_iSource)->ReligionGetStatus(l_Itr->first) != 3)
			{						
				l_bContinue = true;
				break;
			}
		}

		if (!l_bContinue)
		{	
			const GLanguageList& l_Language = g_ServerDAL.CountryData(m_iSource)->GetLanguages();

			for(GLanguageList::const_iterator l_Itr = l_Language.begin();
					l_Itr != l_Language.end();
					l_Itr++)
			{
				if(g_ServerDAL.CountryData(m_iSource)->LanguageGetStatus(l_Itr->first) != 3)
				{			
					return true;						
				}
			}

			return false;
		}
	}
	return true;

}