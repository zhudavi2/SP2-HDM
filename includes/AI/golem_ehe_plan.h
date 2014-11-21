/**************************************************************
*
* golem_ehe_plan.h
*
* Description
* ===========
*  A plan is a list of actions to be done in the future for every objectives. 
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_PLAN
#define _EHE_PLAN


#include "golem_ehe_globals.h"
#include "golem_ehe_entity.h"


namespace SDK
{
   namespace EHE
   {

      // forward declarations	
		class GAction;

      class GOLEM_SDK_API GPlan
      {
      public:
         // constructors / destructors
         GPlan();
         GPlan( const GPlan& in_Plan );   // needed for deep copy of members
         virtual ~GPlan();

			virtual EPlanStatus::Enum GoOnWithPlan();

         // overloaded assignment operator
         GPlan& operator = ( const GPlan& in_Plan );

         // clear contents
         void Clear();

         // check if plan is empty
         bool Empty() const;

			//! Returns the owner of that plan
			ENTITY_ID Entity() const;

			//! Sets the owner of that plan
			void Entity(ENTITY_ID);

			//! Returns the objective of that plan
			OBJECTIVE_ID Objective() const;

			//! Sets the objective of that plan
			void Objective(OBJECTIVE_ID);

         // get string that describes the plan (for debugging purposes of course)
         GString Description() const;

         // add an action at the back of waiting list
         void PushAction( const GAction& in_Action );

         // add an entire plan at the end of this plan
         void PushPlan( const GPlan& in_OtherPlan );

         // remove action from head of waiting list and make it the current action
         void StepForward( TIME in_iCurrentTime, const GEntity::GState& in_EntityState );

         // tells if current action is through executing
         bool ReadyToStepForward() const;

         // get pointer to current action
         GAction* CurrentAction() const;

         // get number of scheduled actions in the waiting list
         UINT32 NbScheduledActions() const;

         // read-only access to waiting list
         const list<GAction*>& ScheduledActions() const;

         // get read-only access to entity's state at the start of current action
         const GEntity::GState& EntityState() const;

			//! Returns the ACTION_ID of the last scheduled action. 
			ACTION_ID LastActionScheduled();						

      private:
         list<GAction*>    m_ScheduledActions;       // waiting list of actions
			
			ENTITY_ID			m_iEntityID;
			OBJECTIVE_ID		m_iObjectiveID;
         GAction*          m_pCurrentAction;         // action currently executing
         TIME              m_iStartTime;           // when the current action has started
         GEntity::GState   m_EntityState;          // state of the entity at the start of the current action
         
      };
   }
};

#endif