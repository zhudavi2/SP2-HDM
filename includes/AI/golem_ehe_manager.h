/**************************************************************
*
* golem_ehe_manager.h
*
* Description
* ===========
*  Takes care of every entities, and the iteration of the AI. 
*	It also loads and saves entities at the beginning of the game, or whenever we save a game.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_MANAGER
#define _EHE_MANAGER

#include "golem_ehe_globals.h"


namespace SDK
{
   namespace EHE
   {		

      // forward declarations
      class GEntity;
      class GObjective;
      class GAction;
		class GPlan;		

		typedef GPlan* (*PLAN_CONSTR_FUNCTION)();
      typedef bool (GEventHandler::*EHE_ITERATION_FUNC_PTR)(const UINT32 );


      //---------------------------------------------------------
      //    Manager class for the generic EHE. It is not instantiable,
      //    and all of its members are static. That way, it behaves
      //    just like a singleton.
      //---------------------------------------------------------
      class GOLEM_SDK_API GManager
      {
      public:
         //! clear contents
         static void Clear();

         //! set message logger
         static void Logger( GLogger* in_pLogger );

         //! get messager logger
         static GLogger* Logger();

         //! write message to log
         static void Log( const GString& in_sMessage, EMessageType in_iMessageType = MSGTYPE_MSG );

         //! load entities from XML config file
         static bool LoadEntities( const GString& in_sFileName );

         //! write entities in XML config file
         static void WriteEntities( const GString& in_sFileName );

         //! manage an entity's tasks at the given time
         static void Iterate( ENTITY_ID in_iEntity, TIME in_iCurrentTime );

         //! register entity
         static void RegisterEntity( const GEntity& in_Entity );
         static void RegisterEntity( ENTITY_ID in_iID,
            const GString& in_sName,
            const GEntity& in_Entity );  // override entity's id and name
         //! register objective
         static void RegisterObjective( const GObjective& in_Objective );
         static void RegisterObjective( OBJECTIVE_ID in_iID,
            const GString& in_sName,
				bool in_bConditionOnly,
            const GObjective& in_Objective );  // override objective's id and name        			
         //! register action
         static void RegisterAction( const GAction& in_Action );
         static void RegisterAction( ACTION_ID in_iID,
            const GString& in_sName,
            const GAction& in_Action );  // override action's id and name

			//! Register the Plan Creation Function
         static void RegisterPlanCreationFunc(PLAN_CONSTR_FUNCTION in_pFunc);

         //! get pointer to given registered entity
         static GEntity* Entity( ENTITY_ID in_iEntity );

         //! get pointer to given registered action
         static GAction* Action( ACTION_ID in_iAction );

         //! get pointer to given registered objective
         static GObjective* Objective( OBJECTIVE_ID in_iObjective );

         //! read-only access to entity map
         static const hash_map<ENTITY_ID, GEntity*>& Entities();

         //! read-only access to objective map
         static const hash_map<OBJECTIVE_ID, GObjective*>& Objectives();

         //! read-only access to action map
         static const hash_map<ACTION_ID, GAction*>& Actions();

         //!Execute the function pointer stored into p_NewIteration. Return true if success
         static bool NewAIIteration(ENTITY_ID in_iEntity);

         //!Execute the function pointer stored into p_EndIteration. Return true if success
         static bool EndAIIteration(ENTITY_ID in_iEntity);

         static void RegisterStartIterationHandler(GEventHandler* in_pHandler,
            EHE_ITERATION_FUNC_PTR in_pFunc);
         static void RegisterEndIterationHandler(GEventHandler* in_pHandler,
            EHE_ITERATION_FUNC_PTR in_pFunc);

			static GPlan* CreateNewPlan();

      private:
         //! private constructor and destructor: the class cannot be instantiated from outside
         GManager();
         ~GManager();

         //! sole instance, whose only purpose is to clear the manager at the beginning and end of execution
         static GManager m_Manager;

         //! message logger
         static GLogger* m_pLogger;

         //! maps of registered objects
         static hash_map<ENTITY_ID, GEntity*>         m_Entities;
         static hash_map<OBJECTIVE_ID, GObjective*>   m_Objectives;
         static hash_map<ACTION_ID, GAction*>         m_Actions;

         //! remove specified registered objects
         static void RemoveEntity( ENTITY_ID in_iEntity );
         static void RemoveObjective( OBJECTIVE_ID in_iObjective );
         static void RemoveAction( ACTION_ID in_iAction );

         //! remove all registered objects
         static void ClearRegisteredObjects();

         static EHE_ITERATION_FUNC_PTR   m_pStartIterationHandlerFunc;
         static GEventHandler*			  m_pStartIterationHandlerInstance;
         static EHE_ITERATION_FUNC_PTR   m_pEndIterationHandlerFunc;
         static GEventHandler*			  m_pEndIterationHandlerInstance;

			static PLAN_CONSTR_FUNCTION     m_pPlanConstrFunc;

      };
   }
};

#endif