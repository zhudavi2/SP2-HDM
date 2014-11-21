/**************************************************************
*
* golem_manager.h
*
* Description
* ===========
*  Describes the interface of class GManager
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_MANAGER_H_
#define _GOLEM_MANAGER_H_

namespace SDK
{
   namespace Combat
   {
      class GUnit;
      class GArena;

      typedef SDK::Combat::GArena* (*ARENA_CONSTR_FUNCTION)(void);

      /*!
      * This class is the link between the EHE and the clients, it will manage
      * the combats, create and make sure that the plans are executed or stopped 
      * assign units to plans, etc.
      **/
      class GOLEM_SDK_API GManager
      {
         GManager();
         ~GManager();
      public:
         //! Gets the unique Combat manager instance
         static GManager& Instance();

         //! Iterate all the battleplans, using specified game time
         void Iterate(const REAL64 in_fTime);

         void Shutdown();

         //! Create a combat Arena
         Combat::GArena* CreateArena(const UINT32 in_iID,const GVector2D<REAL32>& in_Coord);

         //! Create a combat Arena using incoming stream
         Combat::GArena* CreateArena(GOBuffer&);

         //! Get the arena with the given Id
         Combat::GArena* Arena(const UINT32 in_iID);

         //! Get a random number
         REAL32 RandomReal();

         //! Register a handler for the on combat over event
         bool RegisterOnCombatOverHandler(CALLBACK_HANDLER_void_int_pvoid in_pFunc, void* in_pHandlerInstance);
         //! Actually notify that a combat is over
         void NotifyCombatOver(Combat::EBattleStatus::Enum in_Status,GArena* in_pArena);

         //! Register the Arena Creation Function
         void RegisterArenaCreationFunc(ARENA_CONSTR_FUNCTION in_pFunc);

         //! Get the combat manager clock
         REAL64 Clock();

			//! Get the arenas currently in the world
			const vector<GArena*>& Arenas();

         bool            DeleteArena(Combat::GArena* in_pArena);
      private:
         vector<GArena*>                m_vArenas;
         Random::GQuick                 m_QuickRandom;
         ARENA_CONSTR_FUNCTION          m_pArenaConstrFunc;

         REAL64                         m_fClock;

         CALLBACK_HANDLER_void_int_pvoid     m_pCombatOverHandlerFunc;//!<Handling when combat is over
         void*                m_pCombatOverHandlerInstance;//!<Combat over handler instance.


      };//End class GManager

   };//end namespace combat
}
#define g_CombatManager SDK::Combat::GManager::Instance()

#endif //_GOLEM_MANAGER_H_
