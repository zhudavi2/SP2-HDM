/**************************************************************
*
* sp2_event_update_advisor_settings.h
*
* Description
* ===========
*  Update advisor settings
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_UPDATE_ADVISOR_SETTINGS_H_
#define _GOLEM_EVENT_UPDATE_ADVISOR_SETTINGS_H_

namespace SP2
{   	
   namespace Event
   {
      /*!
      * Event Sent from the server to the client, to tell him a news has been posted
      **/
      class GUpdateAdvisorSettings: public SP2::Event::GRequestAckGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
         			
         EAdvisorStatus::Enum m_eEconAdvisorStatus;
         EAdvisorStatus::Enum m_ePoliAdvisorStatus;
         EAdvisorStatus::Enum m_eMiliAdvisorStatus;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_UPDATE_ADVISOR_SETTINGS_H_
