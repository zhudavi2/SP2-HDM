#include "golem_pch.hpp"


using namespace EHEActions;


GLowerInterestRate* GLowerInterestRate::Clone() const
{
   return new GLowerInterestRate( *this ); 
}


void GLowerInterestRate::Execute()
{
	GEntity* pEntity = SDK::EHE::GManager::Entity( m_iSource );

	if ( ! pEntity ) return;

   REAL32 l_fLevel = 0.0f;
   REAL32 l_fWageModif = 0.0f;

   l_fLevel = g_ServerDAL.CountryData(m_iSource)->InterestLevel();

	if(l_fLevel > SP2::InterestRate_LowerCap) 
   {
      l_fWageModif = m_fStrength*EHEConstants::InterestRate_Normal;

      if(l_fWageModif < EHEConstants::InterestRate_Min)
         l_fWageModif = EHEConstants::InterestRate_Min;
      if(l_fWageModif > EHEConstants::InterestRate_Max)
         l_fWageModif = EHEConstants::InterestRate_Max;

      REAL32 l_fNewWage = l_fLevel - l_fWageModif;

      g_ServerDCL.ChangeInterestRate(m_iSource,l_fLevel,l_fNewWage);
   }
	else
   	g_Joshua.Log(pEntity->Description() + L" cannot lower its Interest Rate any more.", MSGTYPE_MSG);

	m_fWhenToStepForward = g_Joshua.GameTime() + 120.f;
   
}

bool GLowerInterestRate::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GLowerInterestRate::CheckHardConditions() const
{
	if (g_ServerDAL.CountryData(m_iSource)->InterestLevel() <= SP2::InterestRate_LowerCap)
		return false;

	return true;
}

GRaiseInterestRate* GRaiseInterestRate::Clone() const
{
   return new GRaiseInterestRate( *this ); 
}


void GRaiseInterestRate::Execute()
{
   // modification is capped between 0.1$ and 5$
   // a strenght of 1 == a modification of 1$
	GEntity* pEntity = GManager::Entity( m_iSource );
	if ( ! pEntity ) return;

   REAL32 l_fLevel = 0.0f;
   REAL32 l_fWageModif = 0.0f;   

	l_fLevel = g_ServerDAL.CountryData(m_iSource)->InterestLevel();

	if(l_fLevel < SP2::InterestRate_UpperCap) 
   {
      l_fWageModif = m_fStrength*EHEConstants::InterestRate_Normal;

      if(l_fWageModif < EHEConstants::InterestRate_Min)
         l_fWageModif = EHEConstants::InterestRate_Min;
      if(l_fWageModif > EHEConstants::InterestRate_Max)
         l_fWageModif = EHEConstants::InterestRate_Max;

      REAL32 l_fNewWage = l_fLevel + l_fWageModif;

      g_ServerDCL.ChangeInterestRate(m_iSource,l_fLevel,l_fNewWage);
   }
	else
		g_Joshua.Log(pEntity->Description() + L" cannot raise its Interest Rate any more.", MSGTYPE_MSG);

	m_fWhenToStepForward = g_Joshua.GameTime() + 120.f;
   
}

bool GRaiseInterestRate::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GRaiseInterestRate::CheckHardConditions() const
{
	if (g_ServerDAL.CountryData(m_iSource)->InterestLevel() >= SP2::InterestRate_UpperCap)
		return false;

	return true;
}