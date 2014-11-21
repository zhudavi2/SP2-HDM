/**************************************************************
*
* sp2_event_update_war.h
*
* Description
* ===========
*  Communicate between client and server to bring updates of wars
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_UPDATE_WAR_H_
#define _GOLEM_UPDATE_WAR_H_

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
      class GUpdateWar :  public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         //! ID of a removed war or 0xFFFFFFFF if there is no war to remove
         UINT32         m_iRemovedWar;

         //! Content of an added and/or modified war
         GWar        m_ModifiedWar;

      protected:
         static SDK::GGameEventSPtr New();

         GUpdateWar();

         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };
   }
}

#endif // #ifndef _GOLEM_UPDATE_WAR_H_