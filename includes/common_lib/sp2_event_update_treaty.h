/**************************************************************
*
* sp2_event_update_treaty.h
*
* Description
* ===========
*  Describes the interface of class GUpdateTreaty, which is 
*  sent when a treaty is added, modified or deleted.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_UPDATE_TREATY_H_
#define _GOLEM_UPDATE_TREATY_H_

namespace SP2
{
   class GClient;
   class GServer;

   namespace Event
   {
      /*!
       * Game event used to send updates on data from the server to the
       * client so the client will be able to cache the data instead of
       * always request it.
       **/
      class GUpdateTreaty :  public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         //! ID of a removed treaty or 0xFFFFFFFF if there is no treaty to remove
         UINT32         m_iRemovedTreaty;

         //! Content of an added and/or modified treaty
         GTreaty        m_ModifiedTreaty;

      protected:
         static SDK::GGameEventSPtr New();

         GUpdateTreaty();

         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };
   }
}

#endif // #ifndef _GOLEM_UPDATE_TREATY_H_
