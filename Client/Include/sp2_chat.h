/**************************************************************
*
* sp2_chat.h
*
* Description
* ===========
*  This class implements all the functionalities of the chat 
*  window, without implementing anything of the window itself
*  It manages the players list, allows grouping and send to
*  group, and it contains the message history
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_GCHAT_H_
#define _GOLEM_SP2_GCHAT_H_

#define MAX_MESSAGE_HISTORY_LENGTH 300

namespace SP2
{
   class Client;
   class GGeneralEventHandler;

   class GChat
   {      
   public:
      enum EChatMessageType
      {
         ECMT_BROADCAST,
         ECMT_MUTLIPLETARGETS,
         ECMT_SINGLETARGET
      };

      struct GChatMessage
      {
         GString m_sender;
         GString m_message;
         INT32 m_countryID;
         bool m_bPrivate;
      };

      struct GChatPeer
      {
         INT32 ClientID;
         INT32 CountryID;
         GString PlayerName;
         SDK::GEPlayerStatus PlayerStatus;

         bool operator < (const GChatPeer &) const;         
      };

      GChat();
      ~GChat();


      void RefreshPlayersList(bool = false);

      const vector<GChatPeer> & PlayersList();
      
      int Admin();
      
      vector<int> & Selected();      
            
      void GroupMembers(int, const vector<int> &);
      bool SendToSelected(const GString &,const list<UINT32> &in_iSelectedPlayer);

      const deque<GChatMessage> & MessageHistory();
      void ClearMessageHistory();

   protected:
      virtual void OnNewMessageReceived(const GChatMessage &) = 0;
      virtual void OnPlayerListChanged() = 0;
      vector<GChatPeer> m_PlayersList;
   
   private:
      friend class GClient;
      friend class GGeneralEventHandler;
      int m_Admin;
      void AddSentMessage(int, const GString &,bool);
      void PlayersList(const vector<GChatPeer> &);
      deque<GChatMessage> m_History;
   };
}

#endif