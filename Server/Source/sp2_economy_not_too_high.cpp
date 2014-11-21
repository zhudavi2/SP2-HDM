#include "golem_pch.hpp"


using namespace EHEObjectives;


GEconomyTooHigh* GEconomyTooHigh::Clone() const
{
   return new GEconomyTooHigh( *this );
}

//OBJECTIVE: ECONOMY_TOO_HIGH
OBJECTIVE_LEVEL GEconomyTooHigh::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   
	//The sum of the 5 values must be 1
	#define InterestLevel_Imp			0.25f
	#define PersonalIncomeTax_Imp    0.25f
	#define InflationLevel_Imp		   0.50f

	REAL32 l_fInterestLevel = GAI::AdjustToEHEObjective(SP2::GWorldBehavior::FindInterestRateFactor(in_iEntity));
	REAL32 l_fPersonalIncomeTax = GAI::AdjustToEHEObjective(1.f - SP2::GWorldBehavior::FindPersonalIncomeTaxFactor(in_iEntity));
	
	REAL32 l_fInflation = g_ServerDAL.CountryData(in_iEntity)->InflationLevel();
	REAL32 l_fInflationLevel = 0.f;
	if(l_fInflation >= 0.1f)
		l_fInflationLevel = -1.f;
	else if(l_fInflation <= 0.02f)
		l_fInflationLevel = 1.f;
	else if(l_fInflation >= 0.05f)
		l_fInflationLevel = (-20.f * l_fInflation) + 1.f;
	else
		l_fInflationLevel = (-33.333333333f * l_fInflation) + 1.66666666667f;
	
	return (l_fInterestLevel*InterestLevel_Imp) + (l_fPersonalIncomeTax*PersonalIncomeTax_Imp) 
			+ (l_fInflationLevel*InflationLevel_Imp);
}


OBJECTIVE_LEVEL GEconomyTooHigh::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0.75f;
}