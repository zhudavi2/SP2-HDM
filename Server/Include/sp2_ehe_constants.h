/**************************************************************
*
* sp2_ehe_constants.h
*
* Description
* ===========
*  Contains Constants used by EHE Actions and Objectives
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_CONSTANTS_H_
#define _EHE_CONSTANTS_H_

namespace EHEConstants
{	
	const INT32	      Suffrage_Min                     =  14;
	const INT32	      Suffrage_Max                     =  25;

	// modification of relations based from propaganda & foreign aid changes
	// a strength of 1 == a modification of 1
	const REAL32		RelationPropaganda					= 15.f;
	const REAL32		RelationImf								= 15.f;

	// modification is capped between 0.01% and 5%
	// a strength of 1 == a modification of 1%
	const REAL32		InterestRate_Min						= 0.0001f;
	const REAL32		InterestRate_Max						= 0.05f;
	const REAL32		InterestRate_Normal					= 0.01f;

	// modification is capped between 0.1$ and 5$
	// a strength of 1 == a modification of 1$
	const REAL32		MinimumWage_Min							= 0.1f;
	const REAL32		MinimumWage_Max							= 5.0f;
	const REAL32		MinimumWage_Normal						= 1.0f;

	// modification is capped between 0.1% and 25%
	// a strength of 1 == a modification of 5%
	const REAL32		PersonalTaxes_Min							= 0.01f;
	const REAL32		PersonalTaxes_Max							= 0.25f;
	const REAL32		PersonalTaxes_Normal						= 0.05f;

	//Modification is bound between 1 % and 25 %
	//A strength of 1 == 5%
	const REAL32		NationalSecurity_Min						= 0.001f;
	const REAL32		NationalSecurity_Max						= 0.25f;
	const REAL32		NationalSecurity_Normal					= 0.05f;

	//Modification is bound between 1 % and 25 %
	//A strength of 1 == 5%
	const REAL32		Propaganda_Min								= 0.001f;
	const REAL32		Propaganda_Max								= 0.25f;
	const REAL32		Propaganda_Normal							= 0.05f;

	//Modification is bound between 1 % and 25 %
	//A strength of 1 == 5%
	const REAL32		Corruption_Min								= 0.001f;
	const REAL32		Corruption_Max								= 0.25f;
	const REAL32		Corruption_Normal							= 0.05f;

	//Objectives constants
	const REAL32		InterestLevel_Desired					= 0.02f; //Must be lower than InterestLevel_Desired
	const REAL32		InterestLevel_Max							= 0.20f;
	const REAL32		PersonalIncomeTax_Norm					= 0.52f;
   const REAL32		PersonalIncomeTax_Max					= 0.9f;
	const REAL32		MinimumWage_Norm							= 6.0f;  //Must be lower than MinimumWage_Max
	const REAL32		MinimumWage_MaximumValue				= 20.0f;
	const REAL32		InflationLevel_Desired					= 0.02f;	//Must be higher than InflationLevel_Lower && InflationLevel_LowerBound
	const REAL32		InflationLevel_Lower						= 0.00f;
	const REAL32		InflationLevel_LowerBound				= 0.037f; //Must be higher than InflationLevel_Desired
	const REAL32		InflationLevel_HigherBound				= 0.20f;	//Must be higher than InflationLevel_LowerBound
	const REAL32		InflationGrowth_Desired					= 0.00f;
	const REAL32		InflationGrowth_Lower					= 0.08f;
	const REAL32		InflationGrowth_Higher					= 0.20f; //Must be lower than InflationGrowth_Lower
	const REAL32		Military_Norm								= 0.427f; 
	const REAL32		Military_Best								= 0.3f; //Must be lower than Military_Worst & lower than Military_Norm
	const REAL32		Military_Worst								= 0.6f; //Must be higher than Military_Best & higher than Military_Norm
	const REAL32		IMF_Norm										= 0.0029f;
	const REAL32		IMF_Best										= 0.00069f;	//Must be lower than IMF_Norm & lower than IMF_Worst
	const REAL32		IMF_Worst									= 0.01196f; //Must be higher than IMF_Best & higher than IMF_Norm 


};

#endif