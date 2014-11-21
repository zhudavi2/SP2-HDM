/**************************************************************
*
* sp2_event_acknowledgement.h
*
* Description
* ===========
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_ACKNOWLEDGEMENT_H_
#define _GOLEM_EVENT_ACKNOWLEDGEMENT_H_

namespace SP2
{
   class GEconomicEventHandler;
   namespace Event
   {
      typedef UINT64 GRequestEventType;
      const GRequestEventType NO_REQUEST = 0;

      /*!
      * Base class for game events wich will requires request
      **/
      class GRequestGameEvent: public SDK::Event::GGameEvent
      {
         static SDK::GGameEventSPtr New();
      public:
         // Network serialization functions
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
         // request id
         UINT64 m_iRequestID;
      protected:      
         GRequestGameEvent(){m_iRequestID = NO_REQUEST;};
         ~GRequestGameEvent(){};
      private:         
      };

      /*!
      * Event Sent from the server to the player to inform
      * him that an event has been received and acknowledged
      **/
      class GAnswerAckGameEvent: public GRequestGameEvent
      {
         friend class GServer;
         friend class GClient;
         static SDK::GGameEventSPtr New();
         friend class SP2::GEconomicEventHandler;
      public:
      protected:      
         GAnswerAckGameEvent(){};
         ~GAnswerAckGameEvent(){};
      private:
      };

     /*!
      * Special event to support an event wich will request an acknowledgement
      **/
      class GRequestAckGameEvent: public GRequestGameEvent
      {
         static SDK::GGameEventSPtr New();
      public:         
      protected:
         GRequestAckGameEvent(){};
         ~GRequestAckGameEvent(){};
      private:
      };

   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_ACKNOWLEDGEMENT_H_