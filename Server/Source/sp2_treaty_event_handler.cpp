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
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

using namespace SP2::Event;
using namespace SDK::Event;

SP2::GTreatyEventHandler::GTreatyEventHandler()
{
}

SP2::GTreatyEventHandler::~GTreatyEventHandler()
{
}

bool SP2::GTreatyEventHandler::HandleTreatyJoin(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GTreatyJoin* l_pUpdate = (SP2::Event::GTreatyJoin*)in_pEvent.get();

   g_ServerDCL.JoinTreaty(l_pUpdate->NewMemberID, l_pUpdate->TreatyID, l_pUpdate->Side);

   return true;
}

bool SP2::GTreatyEventHandler::HandleTreatyLeave(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GTreatyLeave* l_pUpdate = (SP2::Event::GTreatyLeave*)in_pEvent.get();

   g_ServerDCL.LeaveTreaty(l_pUpdate->OldMemberID, l_pUpdate->TreatyID);

   return true;
}

bool SP2::GTreatyEventHandler::HandleConditionsCountries(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GRequestTreatyConditionsCountries* l_pUpdate = (SP2::Event::GRequestTreatyConditionsCountries*)in_pEvent.get();

   l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
   l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

   set<UINT32> l_New;

   for(UINT16 i=1; i<=g_ServerDAL.NbCountry(); i++)
   {
      if( g_ServerDCL.CountryCanJoinThoseConditions(i, l_pUpdate->m_iConditions, l_pUpdate->m_CountryList, true) )
         l_New.insert((UINT32)i);
   }

   l_pUpdate->m_CountryList = l_New;
   
   g_Joshua.RaiseEvent(in_pEvent);

   return true;
}
