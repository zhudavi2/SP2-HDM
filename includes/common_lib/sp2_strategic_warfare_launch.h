/**************************************************************
*
* sp2_window_economic_health_update.h
*
* Description
* ===========
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_STRATEGIC_WARFARE_LAUNCH_H_
#define _GOLEM_EVENT_STRATEGIC_WARFARE_LAUNCH_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GStrategicWarfareLaunch: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         //Launch nuke requests
         vector<GNukeLaunchRequest> m_vNukeLaunchRequests;

      protected:
      private:
         GStrategicWarfareLaunch();
         ~GStrategicWarfareLaunch();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_STRATEGIC_WARFARE_LAUNCH_H_
