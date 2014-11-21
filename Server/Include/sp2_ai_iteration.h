/**************************************************************
*
* sp2_ai_iteration.h
*
* Description
* ===========
* Hold GCountryData information for the country being iterated
* This class will be used to control everything the AI should do during each iteration
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_SP2_AIITERATION_H_
#define _GOLEM_SP2_AIITERATION_H_

#include "sp2_country_data_holder.h"

namespace SP2
{
   class GAIIteration
   {
   protected:
   public:
      GAIIteration();
      ~GAIIteration();		

		//Execute all SP2 functions relative to AI iteration
		//Before the country is iterated
		//Is used through a function pointer inside golem_joshua
		void ExecuteNewAIIteration(const INT16 l_iCountryID);

		//Execute all SP2 functions relative to AI iteration
		//After the country is iterated
		//Is used through a function pointer inside golem_joshua
		void ExecuteFinishAIIteration(const INT16 li_iCountryID);

		//Utility function, used by iterations to make sure a data is between 1 and -1.
		//Used by objective levels
		static REAL32 VerifyValues(REAL32 l_fLevel);

   };
};

#endif //_GOLEM_SP2_AIITERATION_H_