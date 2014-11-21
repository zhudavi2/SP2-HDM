/**************************************************************
*
* golem_ehe_objective.h
*
* Description
* ===========
*  An Objective has a current level and an optimal level. 
*	Every entity will try to reach the optimal level by taking different actions, 
*	based on its experience. 
*	Different entities might have different priorities level for the same objective.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_OBJECTIVE
#define _EHE_OBJECTIVE


#include "golem_ehe_globals.h"


namespace SDK
{
   namespace EHE
   {


      class GOLEM_SDK_API GObjective
      {
         friend class GManager;
         friend class GEntity;
         friend class GAction;

      public:
         // constructors / destructors
         GObjective();
         GObjective( OBJECTIVE_ID in_iID, const GString& in_sName, bool in_bConditionOnly );
         virtual ~GObjective();

         // object must be cloneable (deep copy) for registration in the manager
         virtual GObjective* Clone() const = 0;

         // clear contents
         virtual void Clear();

         // get Id number
         OBJECTIVE_ID Id() const;

         // set Id number
         void Id( OBJECTIVE_ID in_iID );

         // get name
         const GString& Name() const;

         // set name
         void Name( const GString& in_sName );

			// set if an objective is a condition only
			void IsConditionOnly( bool in_bConditionOnly);

			// get if an objective is a condition only
			bool IsConditionOnly() const;

         // get string that describes the objective (for debugging purposes of course)
         virtual GString Description() const;

         // get current level for given entity
         virtual OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const = 0;

         // get the optimal level for given entity
         virtual OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const = 0;

         // formula that determines if an objective, with given initial and final levels and entity's priority,
         // requires immediate action
         static bool ActionIsRequired( OBJECTIVE_LEVEL in_fCurrentObjectiveLevel,
            OBJECTIVE_LEVEL in_fOptimalObjectiveLevel,
            PRIORITY_LEVEL in_fPriorityLevel );

      protected:
         OBJECTIVE_ID   m_iId;         // id number
         GString        m_sName;       // name
			bool				m_bConditionOnly;	//Is a condition only, or not
      };
   }
};

#endif