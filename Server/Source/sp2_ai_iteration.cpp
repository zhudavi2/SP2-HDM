/**************************************************************
*
* sp2_ai_iteration.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

//! Default constructor
GAIIteration::GAIIteration()
{
}

//! destructor
GAIIteration::~GAIIteration()
{
}

//Execute all SP2 functions relative to AI iteration
//Before the country is iterated
//Is used through a function pointer inside golem_joshua
void GAIIteration::ExecuteNewAIIteration(const INT16 i_iCountryID)
{
	return;	
}

//Execute all SP2 functions relative to AI iteration
//After the country is iterated
//Is used through a function pointer inside golem_joshua
void GAIIteration::ExecuteFinishAIIteration(const INT16 i_iCountryID)
{
	return;
}


REAL32 GAIIteration::VerifyValues(REAL32 l_fLevel)
{
	if (l_fLevel > 1.0f)
		return 1.0f;
	
	if (l_fLevel < -1.0f)
		return -1.0f;
	
	return l_fLevel;
}