#include "golem_pch.hpp"
//#include "../../AI/EHE/include/golem_ehe_entity.h"
//#include "../../AI/EHE/include/golem_ehe_manager.h"

using namespace EHEActions;


GLowerTaxes* GLowerTaxes::Clone() const
{
   return new GLowerTaxes( *this ); 
}


void GLowerTaxes::Execute()
{
   // modification is capped between 0.1% and 25%
   // a strenght of 1 == a modification of 5%

	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );

	if ( ! pEntity ) return;

   REAL64 l_fIncomeTax = 0.0f;
   REAL64 l_fTaxModif = 0.0f;
   
   l_fIncomeTax = g_ServerDAL.CountryData(m_iSource)->PersonalIncomeTax();

	if(l_fIncomeTax > SP2::PersonalTaxes_LowerCap) 
   {
      l_fTaxModif = m_fStrength*EHEConstants::PersonalTaxes_Normal;

      if(l_fTaxModif < EHEConstants::PersonalTaxes_Min)
         l_fTaxModif = EHEConstants::PersonalTaxes_Min;
      if(l_fTaxModif > EHEConstants::PersonalTaxes_Max)
         l_fTaxModif = EHEConstants::PersonalTaxes_Max;

      REAL64 l_fNewIncomeTax = l_fIncomeTax - l_fTaxModif;

		if(l_fNewIncomeTax < 0.05f)
			l_fNewIncomeTax = 0.05f;
		g_ServerDCL.ChangePersonalIncomeTax(m_iSource,l_fIncomeTax,l_fNewIncomeTax);
   }
	else
		g_Joshua.Log(pEntity->Description() + L" cannot lower its Personal Tax any more.", MSGTYPE_MSG);
  
	m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;
}

bool GLowerTaxes::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GLowerTaxes::CheckHardConditions() const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if (l_pCountryData->PersonalIncomeTax() <= SP2::PersonalTaxes_LowerCap)
		return false;

	if(l_pCountryData->BudgetRevenues() < l_pCountryData->BudgetExpenses())
		return false;

	return true;
}