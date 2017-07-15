/**************************************************************
*
* sp2_chat.cpp
*
* Description
* ===========
*  See corresponding .h file
*
*
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


#include "golem_pch.hpp"

#include "../include/sp2_chat.h"


bool GChat::GChatPeer::operator < (const GChat::GChatPeer & rsh) const
{
   return this->PlayerName < rsh.PlayerName;   
}

GChat::GChat()
{
}

GChat::~GChat()
{
}

const vector<GChat::GChatPeer> & GChat::PlayersList()
{
   return this->m_PlayersList;   
}

int GChat::Admin()
{
   return m_Admin;
}

void GChat::ClearMessageHistory()
{
   m_History.clear();
}

const deque<GChat::GChatMessage> & GChat::MessageHistory()
{
   return m_History;
}


bool GChat::SendToSelected(const GString &msg,const list<UINT32> &in_iSelectedPlayer)
{

   SDK::GGameEventSPtr l_event = CREATE_GAME_EVENT(SDK::Event::GChatEvent);
   l_event->m_iSource = g_Joshua.Client()->Id();
   ((SDK::Event::GChatEvent *)l_event.get())->Message(msg);   
  
   // Private Message
   if(in_iSelectedPlayer.size() > 0)
   {
      ((SDK::Event::GChatEvent *)l_event.get())->Private(true);
      list<UINT32>::const_iterator l_playerIt = in_iSelectedPlayer.begin();
      while(l_playerIt != in_iSelectedPlayer.end())
      {
         if(*l_playerIt != (UINT32)g_Joshua.Client()->Id())
         {
            l_event->m_iTarget = *l_playerIt;
            g_Joshua.RaiseEvent(l_event); 
         }
         l_playerIt++;
      }

      // See your own message
      AddSentMessage(g_Joshua.Client()->Id(), msg,true);

   }
   // Broadcast to all
   else
   {
      ((SDK::Event::GChatEvent *)l_event.get())->Private(false);
	   l_event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastHumanPlayers;
	   g_Joshua.RaiseEvent(l_event);
      AddSentMessage(g_Joshua.Client()->Id(), msg,false);
   }

   
   return true;
}

void GChat::AddSentMessage(int in_Sender, const GString & in_msg, bool in_bPrivate)
{
   GString l_PlayerName(in_Sender);
   INT32  l_countryID = 0;
   vector<GChatPeer>::const_iterator itr;

   for (itr = m_PlayersList.begin(); itr != m_PlayersList.end(); itr++)
   {
      if (itr->ClientID == in_Sender)
      {
         l_PlayerName = itr->PlayerName;
         l_countryID  = itr->CountryID;
         break;
      }
   }
  
   if(itr == m_PlayersList.end())
   {
       l_PlayerName = "Server";
       l_countryID  = 0;
      
   }

   GChatMessage l_message;
   l_message.m_bPrivate    = in_bPrivate;
   l_message.m_countryID   = l_countryID;
   l_message.m_message     = in_msg;
   l_message.m_sender      = l_PlayerName;

   m_History.push_back(l_message);
   while (m_History.size() > MAX_MESSAGE_HISTORY_LENGTH)
      m_History.pop_front();

   OnNewMessageReceived(*(m_History.end() - 1));
}

void GChat::PlayersList(const vector<GChat::GChatPeer> & in_Players)
{
   hash_map<int, int> mapping;
   
   for (vector<GChatPeer>::const_iterator itr1 = m_PlayersList.begin(); itr1 != m_PlayersList.end(); itr1++)
      for (vector<GChatPeer>::const_iterator itr2 = in_Players.begin(); itr2 != in_Players.end(); itr2++)
      {
         if (itr1->ClientID == itr2->ClientID)
         {
            mapping.insert(pair<int,int>(itr1->ClientID, itr2->ClientID));
            break;
         }
      }

   m_PlayersList = in_Players;
   
   sort(m_PlayersList.begin(), m_PlayersList.end());
   for (vector<GChatPeer>::iterator i = m_PlayersList.begin(); i != m_PlayersList.end(); i++)
      if (i->CountryID == g_Joshua.AdminPlayerID())
      {
         m_Admin = i - m_PlayersList.begin();
         break;
      }

  
   OnPlayerListChanged();
}

void GChat::RefreshPlayersList(bool l_Inactive)
{
   SDK::GGameEventSPtr l_Event =  CREATE_GAME_EVENT(SP2::Event::GGetPlayersList);
   l_Event->m_iSource = g_Joshua.Client()->Id();
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
   g_Joshua.RaiseEvent(l_Event);
}
