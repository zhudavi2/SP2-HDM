#include "golem_pch.hpp"
#include "sp2_common.h"
#include "../include/sp2_server.h"


using namespace EHEActions;


GRaiseTaxes* GRaiseTaxes::Clone() const
{
   return new GRaiseTaxes( *this ); 
}


void GRaiseTaxes::Execute()
{

   // modification is capped between 0.1% and 25%
   // a strenght of 1 == a modification of 5%

	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );

	if ( ! pEntity ) return;

   REAL64 l_fIncomeTax = 0.0f;
   REAL64 l_fTaxModif = 0.0f;

   l_fIncomeTax = g_ServerDAL.CountryData(m_iSource)->PersonalIncomeTax();

	if(l_fIncomeTax < SP2::PersonalTaxes_UpperCap)
   {
      l_fTaxModif = m_fStrength*EHEConstants::PersonalTaxes_Normal;

      if(l_fTaxModif < EHEConstants::PersonalTaxes_Min)
         l_fTaxModif = EHEConstants::PersonalTaxes_Min;
      if(l_fTaxModif > EHEConstants::PersonalTaxes_Max)
         l_fTaxModif = EHEConstants::PersonalTaxes_Max;

      REAL64 l_fNewIncomeTax = l_fIncomeTax + l_fTaxModif;

      // cant raise taxes highier than this for ai
      if(l_fNewIncomeTax > EHEConstants::PersonalIncomeTax_Max)
         l_fNewIncomeTax = EHEConstants::PersonalIncomeTax_Max;

      g_ServerDCL.ChangePersonalIncomeTax(m_iSource,l_fIncomeTax,l_fNewIncomeTax);
   }
	else
		g_Joshua.Log(pEntity->Description() + L" cannot raise its Personal Tax any more.", MSGTYPE_MSG);

	m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;
}

bool GRaiseTaxes::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GRaiseTaxes::CheckHardConditions() const
{
	if (g_ServerDAL.CountryData(m_iSource)->PersonalIncomeTax() >= 0.8)
		return false;

	return true;
}