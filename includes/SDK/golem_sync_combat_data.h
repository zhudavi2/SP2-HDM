/**************************************************************
*
* golem_sync_combat_data.h
*
* Description
* ===========
*  Describes the interface of class GSyncCombatData
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_sync_combat_data_H_
#define _GOLEM_sync_combat_data_H_

namespace SDK
{
   namespace Event
   {
      /*!
       * Event sent from the server to the client to give
       * information on units.
       **/
      class GSyncCombatData : public GGameEvent
      {
      protected:          
         GSyncCombatData();
         virtual ~GSyncCombatData();

      public:

         //Serialization functions
         virtual bool Serialize(GIBuffer&)  const;
         virtual bool Unserialize(GOBuffer&);

         //! GSyncCombatData construction function
         static GGameEventSPtr New();

         //! GSynchronized buffer that is used by other system to unserialized
         //! Note : The GOBuffer is exposed for efficiency reasons.
         GOBuffer*   m_pSynchronizedGOBuffer;  

         //Data 2 that must be transferred
         hash_map<UINT32, Combat::Design::GUnit*>  m_vAddedModifiedDesigns;
         hash_map<UINT32, Combat::GUnit*>          m_vAddedModifiedUnits;
         set<UINT32>                               m_vDeletedUnits;
         hash_map<UINT32, Combat::GUnitGroup*>     m_vAddedModifiedUnitGroups;
         set<UINT32>                               m_vDeletedUnitGroups;
         hash_map<UINT32, Combat::GArenaInfo*>     m_vAddedModifiedArenas;
         set<UINT32>                               m_vDeletedArenas;
      };

      /*!
       * Event sent from the server to the client to give
       * information on units for the first time.
       **/
      class GSyncCombatDataFirst : public GSyncCombatData
      {
      protected:
         GSyncCombatDataFirst();

      public:
         UINT32                        m_iNbCountries;

         //! Construction function
         static GGameEventSPtr New();

         //Serialization functions
         virtual bool Serialize(GIBuffer&)  const;
         virtual bool Unserialize(GOBuffer&);
      };
   }
}

#endif //_GOLEM_TransferCombatData_H_
