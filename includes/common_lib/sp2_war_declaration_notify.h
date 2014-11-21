/**************************************************************
*
* sp2_war_declaration_notify.h
*
* Description
* ===========
*  Event sent to the AI when a war is declared
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


#ifndef _GOLEM_EVENT_WAR_NOTIFY_H_
#define _GOLEM_EVENT_WAR_NOTIFY_H_

namespace SP2
{   
   namespace Event
   {
      /*!
      * Event Sent from the server to the AI player that notifies a war has been declared
      **/
      class GWarDeclarationNotify: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
			UINT32 WarId;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_WAR_NOTIFY_H_
