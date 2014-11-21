#include "golem_pch.hpp"


using namespace EHEActions;


GDecreaseStatusRelLang* GDecreaseStatusRelLang::Clone() const
{
   return new GDecreaseStatusRelLang( *this ); 
}


void GDecreaseStatusRelLang::Execute()
{

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	//Check if any Languages are legal. Put the lowest percentage Language illegal
	multimap<REAL32,INT32> l_LanguageToMakeIllegal;
	const SP2::GLanguageList& l_Languages = l_pCountryData->GetLanguages();
	for(GLanguageList::const_iterator it = l_Languages.begin();
		 it != l_Languages.end(); it++)
	{
		if(l_pCountryData->LanguageGetStatus(it->first) == 1)
		{
			l_LanguageToMakeIllegal.insert(make_pair<REAL32,INT32>(l_pCountryData->LanguageGetPourcentage(it->first),it->first));
		}
	}

	if(l_LanguageToMakeIllegal.size() > 1)
	{
		g_ServerDCL.ChangeLanguageStatus(m_iSource,l_LanguageToMakeIllegal.begin()->second,1,2);
		return;
	}

	//Check if any religions are legal. Put the lowest percentage religion illegal
	multimap<REAL32,INT32> l_ReligionToMakeIllegal;
	const SP2::GReligionList& l_Religions = l_pCountryData->GetReligions();
	for(GReligionList::const_iterator it = l_Religions.begin();
		 it != l_Religions.end(); it++)
	{
		if(l_pCountryData->ReligionGetStatus(it->first) == 1)
		{
			l_ReligionToMakeIllegal.insert(make_pair<REAL32,INT32>(l_pCountryData->ReligionGetPourcentage(it->first),it->first));
		}
	}

	if(l_ReligionToMakeIllegal.size() > 1)
	{
		g_ServerDCL.ChangeLanguageStatus(m_iSource,l_ReligionToMakeIllegal.begin()->second,1,2);
		return;
	}

}

bool GDecreaseStatusRelLang::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	//Check if any religions are legal
	const SP2::GReligionList& l_Religions = l_pCountryData->GetReligions();
	for(GReligionList::const_iterator it = l_Religions.begin();
		 it != l_Religions.end(); it++)
	{
		if(l_pCountryData->ReligionGetStatus(it->first) == 1)
		{
			return true;
		}
	}
	
	//Check if any languages are legal
	const SP2::GLanguageList& l_Languages = l_pCountryData->GetLanguages();
	for(GLanguageList::const_iterator it = l_Languages.begin();
		 it != l_Languages.end(); it++)
	{
		if(l_pCountryData->LanguageGetStatus(it->first) == 1)
		{			
			return true;
		}
	}

	return false;
}