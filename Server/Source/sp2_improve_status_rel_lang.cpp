#include "golem_pch.hpp"


using namespace EHEActions;


GImproveStatusRelLang* GImproveStatusRelLang::Clone() const
{
   return new GImproveStatusRelLang( *this ); 
}


void GImproveStatusRelLang::Execute()
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	//Check if any religions are illegal
	const SP2::GReligionList& l_Religions = l_pCountryData->GetReligions();
	for(GReligionList::const_iterator it = l_Religions.begin();
		 it != l_Religions.end(); it++)
	{
		if(l_pCountryData->ReligionGetStatus(it->first) == 2)
		{
			g_ServerDCL.ChangeReligionStatus(m_iSource,it->first,2,1);
			return;
		}
	}

	//Check if any languages are illegal
	const SP2::GLanguageList& l_Languages = l_pCountryData->GetLanguages();
	for(GLanguageList::const_iterator it = l_Languages.begin();
		 it != l_Languages.end(); it++)
	{
		if(l_pCountryData->LanguageGetStatus(it->first) == 2)
		{
			g_ServerDCL.ChangeLanguageStatus(m_iSource,it->first,2,1);
			return;
		}
	}	
}

bool GImproveStatusRelLang::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	//Check if any religions are illegal
	const SP2::GReligionList& l_Religions = l_pCountryData->GetReligions();
	for(GReligionList::const_iterator it = l_Religions.begin();
		 it != l_Religions.end(); it++)
	{
		if(l_pCountryData->ReligionGetStatus(it->first) == 2)
		{
			return true;
		}
	}
	
	//Check if any languages are illegal or legal
	const SP2::GLanguageList& l_Languages = l_pCountryData->GetLanguages();
	for(GLanguageList::const_iterator it = l_Languages.begin();
		 it != l_Languages.end(); it++)
	{
		if(l_pCountryData->LanguageGetStatus(it->first) == 2)
		{			
			return true;
		}
	}

	return false;
}