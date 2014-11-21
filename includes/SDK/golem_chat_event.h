/**************************************************************
*
* golem_chatgameevent.h
*
* Description
* ===========
*  Describes the interface of class GChatHameEvent
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CHATHAMEEVENT_H_
#define _GOLEM_CHATHAMEEVENT_H_

namespace SDK
{
   namespace Event
   {

      class GOLEM_SDK_API GChatEvent : GGameEvent
      {
      private:
         friend bool GDefaultHandler::HandleChatEvents(GGameEventSPtr); 
         GString        m_Message;
         bool           m_bPrivate;
         GChatEvent();
      public:
         static GGameEventSPtr New();

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         void Message(const GString &);
         void Private(bool in_bPrivateMessage);
         bool Private(void);
         const GString & Message() {return m_Message;}
      };
   }
}

#endif //_GOLEM_CHATGAMEEVENT_H_
