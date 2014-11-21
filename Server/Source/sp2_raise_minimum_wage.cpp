#include "golem_pch.hpp"


using namespace EHEActions;


GRaiseMinimumWage* GRaiseMinimumWage::Clone() const
{
   return new GRaiseMinimumWage( *this ); 
}


void GRaiseMinimumWage::Execute()
{
   // modification is capped between 0.1$ and 5$
   // a strenght of 1 == a modification of 1$
	GEntity* pEntity = GManager::Entity( m_iSource );
	if ( ! pEntity ) return;

   REAL32 l_fWage = 0.0f;
   REAL32 l_fWageModif = 0.0f;   

	l_fWage = g_ServerDAL.CountryData(m_iSource)->MinimumWageAmount();

	if(l_fWage < SP2::MinimumWage_UpperCap) 
   {
      l_fWageModif = m_fStrength*EHEConstants::MinimumWage_Normal;

      if(l_fWageModif < EHEConstants::MinimumWage_Min)
         l_fWageModif = EHEConstants::MinimumWage_Min;
      if(l_fWageModif > EHEConstants::MinimumWage_Max)
         l_fWageModif = EHEConstants::MinimumWage_Max;

      REAL32 l_fNewWage = l_fWage + l_fWageModif;

      g_ServerDCL.ChangeMinimumWage(m_iSource,l_fWage,l_fNewWage);
   }
	else
		g_Joshua.Log(pEntity->Description() + L" cannot raise its Minimum Wage any more.", MSGTYPE_MSG);
   
}

bool GRaiseMinimumWage::CheckHardConditions() const
{
	if (g_ServerDAL.CountryData(m_iSource)->MinimumWageAmount() >= SP2::MinimumWage_UpperCap)
		return false;

	return true;
}