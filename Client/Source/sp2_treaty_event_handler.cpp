/**************************************************************
*
* sp2_treaty_event_handler.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_treaties_detailed_window.h"

using namespace SP2::Event;
using namespace SDK::Event;


SP2::GTreatyEventHandler::GTreatyEventHandler()
{
}

SP2::GTreatyEventHandler::~GTreatyEventHandler()
{
}

bool SP2::GTreatyEventHandler::HandleTreatyOffer(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GTreatyOffer* l_pEvent = (SP2::Event::GTreatyOffer*)in_pEvent.get();
   
   if(!g_ClientDDL.TreatyDetailsWindow())
      g_ClientDDL.TreatyDetailsWindowSpawn();

   gassert(g_ClientDAL.Treaties().find(l_pEvent->TreatyID) != g_ClientDAL.Treaties().end(),"Treaty wasn't synch properly");
   if(g_ClientDAL.Treaties().find(l_pEvent->TreatyID) != g_ClientDAL.Treaties().end())
   {
      INT32 l_iMailID = g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.GetString(EStrId::AskedToJoinTreaty) + g_ClientDAL.Treaties().find(l_pEvent->TreatyID)->second.Name(), GDataControlLayer::ShowTreaty , (void*)l_pEvent->TreatyID);
      g_ClientDAL.m_TreatiesMail.insert( make_pair( l_pEvent->TreatyID, l_iMailID ) );
   }
   
   return true;
}

bool SP2::GTreatyEventHandler::HandleConditionsCountries(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GRequestTreatyConditionsCountries* l_pEvent = (SP2::Event::GRequestTreatyConditionsCountries*)in_pEvent.get();

   if(g_ClientDDL.TreatyDetailsWindow() && g_ClientDDL.TreatyDetailsWindow()->Visible())
   {
      g_ClientDDL.TreatyDetailsWindow()->ApplyCountryConditions( l_pEvent->m_CountryList );
   }

   return true;
}
