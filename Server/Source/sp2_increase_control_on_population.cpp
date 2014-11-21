#include "golem_pch.hpp"
#include "../../../../AI/EHE/include/golem_ehe_manager.h"
#include "../../../../AI/EHE/include/golem_ehe_entity.h"
#include "../../../../AI/EHE/include/golem_ehe_objective.h"

using namespace EHEActions;


GIncreaseControlOnPopulation* GIncreaseControlOnPopulation::Clone() const
{
   return new GIncreaseControlOnPopulation( *this ); 
}


void GIncreaseControlOnPopulation::Execute()
{

	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );
	if ( ! pEntity ) return;	

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	const INT32 l_iNumberOfOptions = 6;
	//A country can do 5 things here. 
	/*
		1: Raise Suffrage Age
		2: Close the immigration
		3: Close the emigration
		4: Change laws
		5: Change the status of a religion from official to legal, or from legal to illegal
		6: Change the status of a language to from official to legal, or from legal to illegal						
	*/
	
	//First, let's give a number between -1 and 1 to every possibilites. The one with the lowest
	//value will be executed.
	REAL32 l_Values[l_iNumberOfOptions];

	//Sets suffrage age
	//If the suffrage age is <= 18, level is -1
	//If the suffrage age is >= 25, level is 1
	INT32 l_iSuffrage = l_pCountryData->Suffrage();
	REAL32 l_fSuffrage = 0.0f;
	if (l_iSuffrage <= 18)
		l_fSuffrage = -1.0f;
	else if (l_iSuffrage >= 25)
		l_fSuffrage = 1.0f;
	else
	{
		switch(l_iSuffrage)
		{
			case 19:	
				l_fSuffrage = -0.66f;
				break;
			case 20:	
				l_fSuffrage = -0.33f;
				break;
			case 21:	
				l_fSuffrage = 0.0f;
				break;
			case 22:	
				l_fSuffrage = 0.25f;
				break;
			case 23:	
				l_fSuffrage = 0.5f;
				break;
			case 24:	
				l_fSuffrage = 0.75f;
				break;
			default:	//Should not happen
				l_fSuffrage = 0.0f;
				break;
		}	
	}
	l_Values[0] = l_fSuffrage;


	//Immigration/Emigration
	REAL32 l_fCurrentLevel = 0.0f;
	REAL32 l_fOptimalLevel = 0.0f;

	hash_map<OBJECTIVE_ID, GObjective*> l_Obj =GManager::Objectives();	 
	GObjective* l_pHaveSpace = l_Obj[EHEObjectives::Ids::HAVE_SPACE];

	l_fCurrentLevel = l_pHaveSpace->CurrentLevel(m_iSource);
	l_fOptimalLevel = l_pHaveSpace->OptimalLevel(m_iSource);

	//Sets immigration level
	if (l_pCountryData->ImmigrationClosed())
		l_Values[1] = 1.0f;
	else
	{
		if (l_fCurrentLevel < l_fOptimalLevel)
			l_Values[1] = -1.0f;
		else
			l_Values[1] = 1.0f;
	}

	//Sets emigration level
	if (l_pCountryData->EmigrationClosed())
	{
		l_Values[2] = 1.0f;
	}	
	else
	{
		if (l_fCurrentLevel < l_fOptimalLevel)
			l_Values[2] = 1.0f;
		else
			l_Values[2] = -1.0f;

	}

	l_Values[3] = 1.f;
	if(l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		l_Values[3] -= 0.25f;
	if(l_pCountryData->InternalLaw(EInternalLaws::Abortion))
		l_Values[3] -= 0.25f;
	if(l_pCountryData->InternalLaw(EInternalLaws::Contraception))
		l_Values[3] -= 0.25f;
	if(l_pCountryData->InternalLaw(EInternalLaws::SameSexMarriage))
		l_Values[3] -= 0.25f;
	if(l_pCountryData->InternalLaw(EInternalLaws::WomenSuffrage))
		l_Values[3] -= 0.25f;	
	if(l_pCountryData->InternalLaw(EInternalLaws::Polygamy))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren))
		l_Values[3] -= 0.25f;
	if(!l_pCountryData->InternalLaw(EInternalLaws::ChildLabor))
		l_Values[3] -= 0.25f;	

	if(g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionPlus,
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
		// 5 if Illegal
		// -2 if Official
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
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first) * 5.0f;
					break;
				case 3:	//Official
					l_fReligionStatus += l_pCountryData->ReligionGetPourcentage(l_Itr->first) * -2.0f;
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
		// 5 if Illegal
		// -2 if Official
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
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first) * 5.0f;
					break;
				case 3:	//Official
					l_fLanguageStatus += l_pCountryData->LanguageGetPourcentage(l_Itr->first) * -2.0f;
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
		case 0:	//Raise Suffrage Age
			ExecuteRaiseSuffrage(pEntity);						
			break;
		case 1:	//Close the immigration
			ExecuteCloseImmigration(pEntity);						
			break;
		case 2:	//Close the emigration
			ExecuteCloseEmigration(pEntity);			
			break;
		case 3:	//Change laws
			ExecuteChangeLaws(pEntity);			
			break;
		case 4:	//Change the status of a religion from official to legal, or from legal to illegal
			ExecuteStatusReligion(pEntity);
			break;
		case 5:	//Change the status of a language to from official to legal, or from legal to illegal
			ExecuteStatusLanguage(pEntity);
			break;
		default:	//Should not happen
			break;
	}	

}

void GIncreaseControlOnPopulation::ExecuteChangeLaws(const GEntity* in_pEntity) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pEntity->Id());

	if(l_pCountryData->InternalLaw(EInternalLaws::SameSexMarriage) && 
		l_pCountryData->LeaderParty()->PoliticalIdeology() >= 0.5f &&
		g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																SP2::c_fAIActionPlus,
																SP2::c_fAIActionNeutral,
																SP2::c_fAIActionDoubleMinus,
																SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::SameSexMarriage,false);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::Abortion) && 
			  l_pCountryData->LeaderParty()->PoliticalIdeology() >= 0.5f &&
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Abortion,false);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::Contraception) && 
			  l_pCountryData->LeaderParty()->PoliticalIdeology() >= 0.6f &&
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Contraception,false);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::Polygamy) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::Polygamy,false);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::NumberOfChildren,true);
	}
	else if(!l_pCountryData->InternalLaw(EInternalLaws::ChildLabor) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral) >= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::ChildLabor,true);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::WomenSuffrage) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionPlus,
																	  SP2::c_fAIActionNeutral,
																	  SP2::c_fAIActionDoubleMinus,
																	  SP2::c_fAIActionMinus) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::WomenSuffrage,false);
	}
	else if(l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech) && 
			  g_ServerDCL.CalculateProbabilityOfAction( l_pCountryData,
																	  SP2::c_fAIActionDoublePlus,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionMinus,
																	  SP2::c_fAIActionMinus) <= 0.5f)
	{
		g_ServerDCL.ChangeCountryInternalLaw(l_pCountryData->CountryID(),EInternalLaws::FreedomOfSpeech,false);
	}
	else
		ExecuteStatusReligion(in_pEntity);

}

void GIncreaseControlOnPopulation::ExecuteRaiseSuffrage(const GEntity* in_pEntity) const
{
	INT32 l_iSuffrage = g_ServerDAL.CountryData(in_pEntity->Id())->Suffrage();
	INT32 l_iNewAge = l_iSuffrage;
	if(m_fStrength >= 2.5f)
		l_iNewAge += 2;
	else
		l_iNewAge += 1;

	//g_ServerDCL.ChangeSuffrageAge(in_pEntity->Id(),l_iSuffrage,l_iNewAge);
}
void GIncreaseControlOnPopulation::ExecuteCloseEmigration(const GEntity* in_pEntity) const
{
	g_ServerDCL.ChangeEmigration(in_pEntity->Id(),true);	

}
void GIncreaseControlOnPopulation::ExecuteCloseImmigration(const GEntity* in_pEntity) const
{
	g_ServerDCL.ChangeImmigration(in_pEntity->Id(),true);	
	
}
void GIncreaseControlOnPopulation::ExecuteStatusReligion(const GEntity* in_pEntity) const
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
	REAL64 l_fTempMaxPourc = 99.0f;
	UINT32 l_iTotalReligions = 0;
	REAL32 l_fPourc = 0.0f;
	INT32 l_iTempStatus = 0;

	const GReligionList& l_Religion = g_ServerDAL.CountryData(in_pEntity->Id())->GetReligions();

	for(GReligionList::const_iterator l_Itr = l_Religion.begin();
			l_Itr != l_Religion.end();
			l_Itr++)
	{
		l_fPourc = g_ServerDAL.CountryData(in_pEntity->Id())->ReligionGetPourcentage(l_Itr->first);
		l_iTempStatus = g_ServerDAL.CountryData(in_pEntity->Id())->ReligionGetStatus(l_Itr->first);
		if(l_iTempStatus != 2 && l_fPourc < l_fMaxPourc)
		{			
			l_iTotalReligions++;
			if (l_fPourc < l_fTempMaxPourc)
			{
				l_iID = l_Itr->first;
				l_fTempMaxPourc = l_fPourc;
				l_iStatus = l_iTempStatus;
			}
				
		}
	}

	//If there is only 1 religion left, do not make it illegal (since all religons would be now illegal... not a good idea)
	if (l_iTotalReligions <= 1)
	{
		//We can't do anything on religons, so let's try languages
		ExecuteStatusLanguage(in_pEntity);
	}
	else
	{
		INT32 l_iNewStatus = 2;
		if (l_iStatus == 3)
			l_iNewStatus = 1;
		else if (l_iStatus == 1)
			l_iNewStatus = 2;

		g_ServerDCL.ChangeReligionStatus(in_pEntity->Id(),l_iID,l_iStatus,l_iNewStatus);				

	}
	
}
void GIncreaseControlOnPopulation::ExecuteStatusLanguage(const GEntity* in_pEntity) const
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
	REAL64 l_fTempMaxPourc = 99.0f;
	UINT32 l_iTotalLanguages = 0;
	REAL32 l_fPourc = 0.0f;
	INT32 l_iTempStatus = 0;
	
	const GLanguageList& l_Language = g_ServerDAL.CountryData(in_pEntity->Id())->GetLanguages();

	for(GLanguageList::const_iterator l_Itr = l_Language.begin();
			l_Itr != l_Language.end();
			l_Itr++)
	{
		l_fPourc = g_ServerDAL.CountryData(in_pEntity->Id())->LanguageGetPourcentage(l_Itr->first);
		l_iTempStatus = g_ServerDAL.CountryData(in_pEntity->Id())->LanguageGetStatus(l_Itr->first);
		if(l_iTempStatus != 2 && l_fPourc < l_fMaxPourc)
		{			
			l_iTotalLanguages++;
			if (l_fPourc < l_fTempMaxPourc)
			{
				l_iID = l_Itr->first;
				l_fTempMaxPourc = l_fPourc;
				l_iStatus = l_iTempStatus;
			}
				
		}
	}
	
	//If there is only 1 language left, do not make it illegal (since every languages would be now illegal... not a good idea)
	if (l_iTotalLanguages <= 1)
	{
		//We can't do anything on religons, so let's try Suffrage
		ExecuteRaiseSuffrage(in_pEntity);
	}
	else
	{
		INT32 l_iNewStatus = 2;
		if (l_iStatus == 3)
			l_iNewStatus = 1;
		else if (l_iStatus == 1)
			l_iNewStatus = 2;

		g_ServerDCL.ChangeLanguageStatus(in_pEntity->Id(),l_iID,l_iStatus,l_iNewStatus);				
		

	}
}

bool GIncreaseControlOnPopulation::CheckHardConditions() const
{
	//It checks if there's one thing it can do
	
	if (g_ServerDAL.CountryData(m_iSource)->Suffrage() >= 18
			&& g_ServerDAL.CountryData(m_iSource)->ImmigrationClosed() == true
			&& g_ServerDAL.CountryData(m_iSource)->EmigrationClosed() == true)
	{
		UINT32 l_iTotalReligions = 0;

		const GReligionList& l_Religion = g_ServerDAL.CountryData(m_iSource)->GetReligions();

		for(GReligionList::const_iterator l_Itr = l_Religion.begin();
				l_Itr != l_Religion.end();
				l_Itr++)
		{
			if(g_ServerDAL.CountryData(m_iSource)->ReligionGetStatus(l_Itr->first) != 2)
			{						
				l_iTotalReligions++;
			}
		}

		if (l_iTotalReligions <= 1)
		{	
			UINT32 l_iTotalLanguages = 0;
			
			const GLanguageList& l_Language = g_ServerDAL.CountryData(m_iSource)->GetLanguages();

			for(GLanguageList::const_iterator l_Itr = l_Language.begin();
					l_Itr != l_Language.end();
					l_Itr++)
			{
				if(g_ServerDAL.CountryData(m_iSource)->LanguageGetStatus(l_Itr->first) != 3)
				{			
					l_iTotalLanguages++;						
				}
			}

			if (l_iTotalLanguages <= 1)
				return false;
		}
	}
	return true;

}