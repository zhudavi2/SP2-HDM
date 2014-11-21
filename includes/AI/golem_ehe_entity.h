/**************************************************************
*
* golem_ehe_entity.h
*
* Description
* ===========
*  An entity represents a country. Each entity has a plan, its own experience and priorities.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_ENTITY
#define _EHE_ENTITY


#include "golem_ehe_globals.h"
#include "golem_ehe_experience.h"


namespace SDK
{
   namespace EHE
   {

      // forward declarations
      class GPlan;


      class GOLEM_SDK_API GEntity
      {
         friend class GManager;
         friend class GObjective;
         friend class GAction;

      public:
         // container type for the status of objectives
         typedef hash_map<OBJECTIVE_ID, OBJECTIVE_LEVEL> GState;

         // constructors / destructors
         GEntity();
         GEntity( ENTITY_ID in_iID, const GString& in_sName );
         GEntity( const GEntity& in_Entity );   // needed for deep copy of members
         ~GEntity();

         // overloaded assignment operator
         GEntity& operator = ( const GEntity& in_Entity );

         // clear contents
         void Clear();

         // read in XML tree node
         void ReadInXMLNode( GTreeNode<GXMLNode>& in_Node );

         // write contents into XML tree node
         void WriteToXMLNode( GTreeNode<GXMLNode>& out_Node ) const;

         // get Id number
         ENTITY_ID Id() const;

         // set Id number
         void Id( ENTITY_ID in_iID );

         // get name
         const GString& Name() const;

         // set name
         void Name( const GString& in_sName );

         // get string that describes the entity (for debugging purposes of course)
         GString Description() const;

         // get objective review period
         TIME_SPAN ObjectiveReviewPeriod() const;

         // set objective review period
         void ObjectiveReviewPeriod( TIME_SPAN in_iPeriod );

         // get max number of recursions when building a plan
         UINT32 MaxPlanDepth() const;

         // set max number of recursions when building a plan
         void MaxPlanDepth( UINT32 in_iMaxSteps );

         // get priority rating for given objective
         PRIORITY_LEVEL Priority( OBJECTIVE_ID in_iObjective ) const;

         // set priority rating for given objective
         void Priority( OBJECTIVE_ID in_iObjective, PRIORITY_LEVEL in_fPriorityLevel );

         // delete an entry in the priority list
         void RemovePriority( OBJECTIVE_ID in_iObjective );

         // access experience data
         GExperience& Experience();

			// predict the entity's state after the specified action
         void PredictState( const GState& in_InitialState, ACTION_ID in_iAction, STRENGTH in_fStrength, GState& out_ProjectedState ) const;

         // predict the entity's state after the execution of a plan
         void PredictState( const GState& in_InitialState, const GPlan& in_Plan, GState& out_ProjectedState ) const;

			// compute the entity's current state
         void ComputeState( GState& out_State ) const;

      private:
         ENTITY_ID                              m_iId;                     // id number
         GString                                m_sName;                   // name
         GExperience                            m_Experience;              // experience data
         hash_map<OBJECTIVE_ID, PRIORITY_LEVEL> m_Priorities;              // list of objectives and their priorities
         TIME_SPAN                              m_iObjectiveReviewPeriod;  // determines the frequency of objective reviews
         TIME                                   m_iNextObjectiveReview;    // when the next objective review should occur
         UINT32                                 m_iMaxPlanDepth;           // maximum number of recursions when building a plan
         hash_map<OBJECTIVE_ID, GPlan*>         m_Plans;                   // plans currently being carried through, indexed by objective
			hash_map<OBJECTIVE_ID, ACTION_ID>		m_History;						// Contains the action id that was last used to solve that objective

			// !Fill the history of objective id by this action id;
			void FillHistory(OBJECTIVE_ID in_iObjectiveID, ACTION_ID in_iActionID);

         // is it time to review objectives yet ?
         bool ObjectiveReviewIsDue( TIME in_iCurrentTime );

         // schedule next objective review
         void ScheduleNextObjectiveReview( TIME in_iCurrentTime );

         // review objectives and build new plans accordingly
         void ReviewObjectives();

         // build plan to bring objective's level from current to optimal, returns NULL no plan could be built
         GPlan* BuildPlan( OBJECTIVE_ID in_iObjective, OBJECTIVE_LEVEL in_fDesiredLevel, const GState& in_CurrentState, GState& out_ProjectedState );

         // recursive function used to build plans.
         bool ReachObjective( OBJECTIVE_ID in_iObjective,
            OBJECTIVE_LEVEL in_fDesiredLevel, 
            const GState& in_InitialState, 
            GState& out_ProjectedState,
            GPlan& out_Plan,
            UINT32& io_iRecursionLevel,
				OBJECTIVE_ID in_iObjectiveWarning = 0);

         // execute the due steps in the entity's current plans
         void FollowPlans( TIME in_iCurrentTime );         

         // get text description of a state transition (for debugging purposes)
         GString DescribeStateTransition( const GState& in_InitialState, const GState& in_FinalState ) const;

         // update experience data after an action was executed
         void AcquireExperience( ACTION_ID in_iAction, STRENGTH in_fStrength, const GState& in_InitialState, const GState& in_FinalState );

         // verify if the specified conditions are met in the given state
         static bool CheckConditions( const vector<GCondition>& in_Conditions, const GState& in_State );

         // compute experience level times action strength, initial and final objective levels
         static REAL32 ExperienceStrengthProduct( OBJECTIVE_LEVEL in_fInitialObjectiveLevel,
            OBJECTIVE_LEVEL in_fFinalObjectiveLevel );

         // compute experience, given objective transition and action strength
         static EXPERIENCE_LEVEL ComputeExperienceLevel( OBJECTIVE_LEVEL in_fInitialObjectiveLevel,
            OBJECTIVE_LEVEL in_fFinalObjectiveLevel,
            STRENGTH in_fActionStrength );

         // compute action strength, given objective transition and experience value
         static STRENGTH ComputeStrength( OBJECTIVE_LEVEL in_fInitialObjectiveLevel,
            OBJECTIVE_LEVEL in_fFinalObjectiveLevel,
            EXPERIENCE_LEVEL in_fExperienceLevel );

         // predict new objective level, given initial value, action strength and experience level
         static OBJECTIVE_LEVEL PredictObjectiveLevel( OBJECTIVE_LEVEL in_fInitialObjectiveLevel,
            STRENGTH in_fActionStrength,
            EXPERIENCE_LEVEL in_fExperienceLevel );
      };
   }
};

#endif