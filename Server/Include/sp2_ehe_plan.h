/**************************************************************
*
* sp2_ehe_plan.h
*
* Description
* ===========
*  Describes the interface of class GPlan
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_PLAN_H_
#define _GOLEM_SP2_PLAN_H_

namespace SP2
{
   /*!
   * SP2 representation of a EHE Plan
   **/
	class GPlan : public ::SDK::EHE::GPlan
   {  
		friend class SP2::GServer;
   public:
      GPlan();		
		
		EPlanStatus::Enum GoOnWithPlan();

   private:
      static SDK::EHE::GPlan* New();

   };

};//End namespace SP2
#endif //_GOLEM_SP2_PLAN_H_
