/**************************************************************
*
* golem_combat_unit_manager.h
*
* Description
* ===========
*  Describes interface of class Combat::GUnitManager which 
*  keeps track of all unit, group of units, design and arena
*  of combat.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_COMBAT_UNIT_MANAGER_H_
#define _GOLEM_COMBAT_UNIT_MANAGER_H_

typedef SDK::Combat::GUnit* (*UNIT_CONSTR_FUNCTION)();
typedef SDK::Combat::GUnitGroup* (*UNIT_GROUP_CONSTR_FUNCTION)();
typedef SDK::Combat::Design::GUnit* (*UNIT_DESIGN_CONSTR_FUNCTION)();
typedef SDK::Combat::GArenaInfo* (*ARENA_INFO_CONSTR_FUNCTION)();

//! This callback function is called when a unit is automatically killed
//! by the unit manager. It receives in parameter a pointer to the object
//! that is to be killed. The object pointer will be invalid after this call.
//! The unit manager must not be updated during the call to this function to 
//! ensure its validity 
typedef void (GCallbackHandlerItf::*CALLBACK_HANDLER_pUnit)(SDK::Combat::GUnit*);

//! This callback function is called when a group is automatically destroyed
//! by the unit manager. It receives in parameter a pointer to the object
//! that is to be destroyed. The object pointer will be invalid after this call.
//! The unit manager must not be updated during the call to this function to 
//! ensure its validity 
typedef void (GCallbackHandlerItf::*CALLBACK_HANDLER_pGroup)(SDK::Combat::GUnitGroup*);

typedef void (GCallbackHandlerItf::*CALLBACK_HANDLER_Sync)(SDK::Event::GSyncCombatData&);


namespace SDK
{
   namespace Combat
   {
      const Hector::DesignPattern::GSubjectNotification  c_notificationUnitGroupRemoved      = 0x01;
      const Hector::DesignPattern::GSubjectNotification  c_notificationUnitGroupModified     = 0x02;
      const Hector::DesignPattern::GSubjectNotification  c_notificationBeforeUnitKilled      = 0x03;
      const Hector::DesignPattern::GSubjectNotification  c_notificationFirstSynchro          = 0x04;
      const Hector::DesignPattern::GSubjectNotification  c_notificationUnitRemoved           = 0x05;
      const Hector::DesignPattern::GSubjectNotification  c_notificationUnitModified          = 0x06;
      const Hector::DesignPattern::GSubjectNotification  c_notificationNewUnitDesign         = 0x07;

      /*!
       * Keeps track of all unit, group of units, design and arena
       * of combat.
       **/
      class GOLEM_SDK_API GUnitManager : public DesignPattern::GSubject
      {
      public:
         GUnitManager(bool in_bWriter);
         ~GUnitManager();

         //! Reset the unit manager ID generation
         void ResetNextIDs();

         //! Set number of countries
         bool NbCountries(UINT32 in_iCount);

         //! Return a unique Id for a new unit design
         UINT32 NewUnitDesignID();

         //! Register a new unit design
         bool RegisterUnitDesign(Design::GUnit* in_pUnitDesign);

         //! Get the unit design associated with the Id
         Design::GUnit* UnitDesign(UINT32 in_iID) const;

			const stdext::hash_map<UINT32, Design::GUnit*>& UnitDesigns() const;

         //! Get all registered unit designs
         const stdext::hash_map<UINT32, Design::GUnit*> & RegisteredUnitDesigns() const;

         //! Return a unique Id for a new unit
         UINT32 NewUnitID();

         //! Add a new unit
         bool AddNewUnit(GUnit* in_Unit);

         //! Kill (remove) a unit
         bool KillUnit(UINT32 in_iID,bool in_bPurgeGroup = true);

         //! Add a unit group
         bool AddUnitGroup(GUnitGroup* in_UnitGroup);

         //! Adds a unit into a group
         bool AddUnit2Group(GUnit* in_pUnit,GUnitGroup* in_pGroup);

         //! Retrieve the specified unit
         GUnit* Unit(UINT32 in_iID) const;

         //! Retrieve all units
         const stdext::hash_map<UINT32, GUnit*> & Units() const;


         //! Return a unique Id for a new unit group
         UINT32 NewUnitGroupID();

         //! Remove a unit group
         bool RemoveUnitGroup(UINT32 in_iID);

         //! Retrieve the specified unit group
         GUnitGroup* UnitGroup(UINT32 in_iID) const;

         //! Retrieve all unit groups
         const stdext::hash_map<UINT32, GUnitGroup*> & UnitGroups() const;

         //! Retrieve all unit group IDs of a country
         const set<UINT32> & CountryUnitGroups(UINT32 in_iCountryID) const;			

         //! Return a unique Id for a new arena information
         UINT32 NewArenaInfoID();

         //! Add a new combat arena information
         bool AddNewArenaInfo(GArenaInfo* in_ArenaInfo);

         //! Remove a combat arena information
         bool RemoveArenaInfo(UINT32 in_iID);

         //! Retrieve the specified combat arena information
         GArenaInfo* ArenaInfo(UINT32 in_iID) const;

         //! Get all registered combat arena infos
         const stdext::hash_map<UINT32, GArenaInfo*> & ArenaInfos() const;

         //! Get if manager must keep track of all modifications
         bool ModificationTracking();

         //! Reset modification tracking state
         bool ClearModifications();

         //! Set the health of the unit
         bool SetUnitHP(GUnit* in_pUnit,const INT32 in_iNewHP, bool in_bPurgeGroup);

         //! Set the group position
         bool SetGroupPosition(GUnitGroup* in_pGroup,const GVector2D<REAL32>& in_Pos);

         //! Get list of modified unit designs
         const hash_map<UINT32, Design::GUnit*>& ModifiedUnitDesigns() const;

         //! Get list of modified units
         const hash_map<UINT32, GUnit*>& ModifiedUnits() const;

         //! Get list of modified unit groups
         const hash_map<UINT32, GUnitGroup*>& ModifiedUnitGroups() const;

         //! Get list of modified arena infos
         const hash_map<UINT32, GArenaInfo*>& ModifiedArenaInfos() const;



         //! Get list of removed units
         const set<UINT32> & RemovedUnits() const;

         //! Get list of removed unit groups
         const set<UINT32> & RemovedUnitGroups() const;

         //! Get list of removed arena infos
         const set<UINT32> & RemovedArenaInfos() const;

         //! Set unit design as modified
         void ModifyUnitDesign(Design::GUnit* in_pUnitDesign);

         //! Set unit as modified
         void ModifyUnit(GUnit* in_pUnit);

         //! Set unit group as modified
         void ModifyUnitGroup(GUnitGroup* in_pUnitGroup);

         //! Set arena info as modified
         void ModifyArenaInfo(GArenaInfo* in_pArenaInfo);


         //! Register the Arena Information creation function
         void RegisterArenaInfoCreationFunc(ARENA_INFO_CONSTR_FUNCTION in_pFunc);
         SDK::Combat::GArenaInfo* CreateArenaInfo();

         //! Register the Unit Creation Function
         void RegisterUnitCreationFunc(UNIT_CONSTR_FUNCTION in_pFunc);

         //! Creates a unit using the construction function
         SDK::Combat::GUnit* CreateUnit();         

         //! Register the Unit Group Creation Function
         void RegisterUnitGroupCreationFunc(UNIT_GROUP_CONSTR_FUNCTION in_pFunc);

         //! Creates a unit group using the construction function
         SDK::Combat::GUnitGroup* CreateUnitGroup();

         //! Deletes a unit group
         bool DeleteUnitGroup(SDK::Combat::GUnitGroup* in_pUnitGroup);


         //! Register the Unit design creation function
         void RegisterUnitDesignCreationFunc(UNIT_DESIGN_CONSTR_FUNCTION in_pFunc);

         //! Creates a unit design using the construction function
         SDK::Combat::Design::GUnit* CreateUnitDesign();

         //! Deletes a unit design
         bool DeleteUnitDesign(SDK::Combat::Design::GUnit* in_pUnitDesign);


         //! Register a destroy group calback, which is called each time a unit
         //! group is destroyed automatically (group is emptied by a killed unit).
         void RegistedDestroyGroupCallback(CALLBACK_HANDLER_pGroup in_pFunc, void* in_pInstance);


         bool SynchronizeData(GGameEventSPtr in_Event);
         bool SynchronizeDataFirst(GGameEventSPtr in_Event);

         bool Shutdown();

      private:
         // Sub Services
         void SynchronizeUnitDesign(Event::GSyncCombatData& in_SynchCombatData);
         void SynchronizeUnits(Event::GSyncCombatData& in_SynchCombatData);
         void SynchronizeGroup(Event::GSyncCombatData& in_SynchCombatData);
         void SynchronizeArena(Event::GSyncCombatData& in_SynchCombatData);

         bool RemoveUnitGroupNoCallback(UINT32 in_iID);

			//! Deletes a unit
         bool DeleteUnit(SDK::Combat::GUnit* in_pUnit);

         stdext::hash_map<UINT32, Design::GUnit*>  m_UnitDesigns;
         stdext::hash_map<UINT32, GUnit*>          m_Units;

         stdext::hash_map<UINT32, GUnitGroup*>     m_UnitGroups;
         vector<set<UINT32> >                      m_CountryUnitGroups;

         stdext::hash_map<UINT32, GArenaInfo*>     m_ArenaInfos;

         stdext::hash_map<UINT32, Design::GUnit*>           m_ModUnitDesigns;
         stdext::hash_map<UINT32, GUnit*>                   m_ModUnits;
         stdext::hash_map<UINT32, GUnitGroup*>              m_ModUnitGroups;
         stdext::hash_map<UINT32, GArenaInfo*>              m_ModArenaInfos;

         set<UINT32>                m_DelUnits;
         set<UINT32>                m_DelUnitGroups;
         set<UINT32>                m_DelArenaInfos;

         UINT32                        m_iNextUnitDesignID;
         UINT32                        m_iNextUnitID;
         UINT32                        m_iNextUnitGroupID;
         UINT32                        m_iNextArenaInfoID;

         UNIT_DESIGN_CONSTR_FUNCTION   m_pUnitDesignConstrFunc;
         UNIT_CONSTR_FUNCTION          m_pUnitConstrFunc;
         ARENA_INFO_CONSTR_FUNCTION    m_pArenaInfoConstrFunc;
         UNIT_GROUP_CONSTR_FUNCTION    m_pUnitGroupConstrFunc;

         GCallbackHandlerItf*          m_pDestroyGroupHandlerInstance;
         CALLBACK_HANDLER_pGroup       m_pDestroyGroupHandlerFunc;

         bool                          m_bWriter;

         bool                          m_bFirstSyncReceived;

#ifdef GOLEM_DEBUG
         //! Only track created units when in Debug
         hash_set<SDK::Combat::GUnit*>         m_vCreatedUnits;
         //! Only track created units groups when in Debug
         hash_set<SDK::Combat::GUnitGroup*>    m_vCreatedUnitGroups;
         //! Only track created units designs when in Debug
         hash_set<SDK::Combat::Design::GUnit*> m_vCreatedUnitDesigns;
#endif 
      };
   }
}

#endif //_GOLEM_COMBAT_UNIT_MANAGER_H_