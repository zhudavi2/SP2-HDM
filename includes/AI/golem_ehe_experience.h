/**************************************************************
*
* golem_ehe_experience.h
*
* Description
* ===========
*  Contains the changes of an action to the list of objectives. 
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_EXPERIENCE
#define _EHE_EXPERIENCE


#include "golem_ehe_globals.h"


namespace SDK
{
   namespace EHE
   {

      class GExperience
      {
      public:
         // constructors / destructors
         GExperience();
         ~GExperience();

         // overloaded assignment operator
         GExperience& operator = ( const GExperience& in_Experience );

         // clear contents
         void Clear();

         // get inertia
         INERTIA_LEVEL Inertia() const;

         // set inertia
         void Inertia( INERTIA_LEVEL in_fInertia );

         // set experience level for given action and objective
         void Level( ACTION_ID in_iAction, OBJECTIVE_ID in_iObjective, EXPERIENCE_LEVEL in_fLevel );

         // get experience level for given action and objective
         EXPERIENCE_LEVEL Level( ACTION_ID in_iAction, OBJECTIVE_ID in_iObjective ) const;

         // update experience using supplied value and considering inertia
         void Update( ACTION_ID in_iAction, 
            OBJECTIVE_ID in_iObjective, 
            EXPERIENCE_LEVEL in_fNewLevel );

         // structure that represents the action-objective relationship
         struct GExperienceNode
         {
            GExperienceNode( ACTION_ID in_iAction, OBJECTIVE_ID in_iObjective, EXPERIENCE_LEVEL in_fLevel );

            ACTION_ID         Action;
            OBJECTIVE_ID      Objective;
            EXPERIENCE_LEVEL  Level; 
         };

         // structure used to uniquely identify experience nodes
         struct GExperienceKey
         {
            GExperienceKey( ACTION_ID in_iAction, OBJECTIVE_ID in_iObjective );

            ACTION_ID      Action;
            OBJECTIVE_ID   Objective;

            bool operator < ( const GExperienceKey& in_Other ) const;
            operator size_t() const;
         };

         struct GExperiencePredicate
         {
            // comparison function for two node pointers, used in the decreasing ordering of nodes
            bool operator() ( const GExperienceNode* in_pLeft, const GExperienceNode* in_pRight ) const;
         };

         // Type for an ordered list of experience nodes. Nodes are ordered in decreasing ratio.
         typedef set<GExperienceNode*, GExperiencePredicate> GNodeSet;

         // find the most effective action for the given objective
         bool MostEffectiveAction( OBJECTIVE_ID in_iObjective, ACTION_ID& out_iAction, EXPERIENCE_LEVEL& out_fLevel ) const;

         // read-only access to ordered set of nodes pertaining to given action
         const GNodeSet* ActionNodes( ACTION_ID in_iAction ) const;

         // read-only access to ordered set of nodes pertaining to given objective
         const GNodeSet* ObjectiveNodes( OBJECTIVE_ID in_iObjective ) const;

         // read-only access to hash_map of experience nodes
         const hash_map<GExperienceKey, GExperienceNode*>& Nodes() const;

      private:
         // measure of the entity's long-term memory
         INERTIA_LEVEL m_fInertia;

         // list of all experience nodes
         hash_map<GExperienceKey, GExperienceNode*> m_ExperienceNodes;

         // list of all actions and ordered set of pointers to experience nodes
         hash_map<ACTION_ID, GNodeSet> m_Actions;

         // list of all objectives and ordered set of pointers to experience nodes
         hash_map<OBJECTIVE_ID, GNodeSet> m_Objectives;
      };
   }
};

#endif