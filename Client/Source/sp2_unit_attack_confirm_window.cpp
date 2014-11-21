/**************************************************************
*
* sp2_unit_attack_confirm_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_unit_attack_confirm_window.h"
#include "../include/sp2_sound.h"

const GString GUnitAttackConfirmWindow::TypeName(L"Unit_Attack_Confirm_Window");

GUI::GBaseObject* SP2::GUnitAttackConfirmWindow::New()
{
	return (GUI::GBaseObject*) new GUnitAttackConfirmWindow();
}


GUnitAttackConfirmWindow::GUnitAttackConfirmWindow() : GFrame()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Unit Attack Confirm Window");
   m_iTargetCountry = 0xFFFFFFFF;
   m_vMoveEvents.reserve(16);
}

GUnitAttackConfirmWindow::~GUnitAttackConfirmWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Unit Attack Confirm Window");
}

bool GUnitAttackConfirmWindow::OnCreate()
{
   __super::OnCreate();

   m_pBtnYes   = (GUI::GButton*)Child(L"btnYes");
   m_pBtnNo    = (GUI::GButton*)Child(L"btnNo");
	m_pCountryName = (GUI::GLabel*)Child(L"txtCountry");

	return true;
}

EEventStatus::Enum GUnitAttackConfirmWindow::OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         bool l_bDestroyMe = false;
         if(in_pCaller == m_pBtnYes)
         {
            // Play a sound
            g_SP2Client->PlaySound(ESoundEventID::ConfirmOrder);
            l_bDestroyMe = true;
            for(vector<SDK::GGameEventSPtr>::const_iterator it = m_vMoveEvents.begin();
               it != m_vMoveEvents.end();++ it)
            {
               g_Joshua.RaiseEvent(*it);
            }
            Hide();
         }
         else if(in_pCaller == m_pBtnNo)
         {
            l_bDestroyMe = true;
            Hide();
         }

         if(l_bDestroyMe)
         {
            g_ClientDDL.UnitAttackConfirmWindowKill(m_iTargetCountry);
         }
      }
   }
   return EEventStatus::Handled;
}

void GUnitAttackConfirmWindow::TargetCountry(UINT32 in_iTargetCountry)
{
   if(m_iTargetCountry == 0xFFFFFFFF)
   {
      m_iTargetCountry = in_iTargetCountry;
		//Modify Country Name
		const GCountry& l_Country = g_ClientDAL.Country(m_iTargetCountry);
		m_pCountryName->Text(l_Country.Name());
   }
}

UINT32 GUnitAttackConfirmWindow::TargetCountry() const
{
   return m_iTargetCountry;
}

void GUnitAttackConfirmWindow::AddMoveEvent(SDK::GGameEventSPtr in_MoveEvent)
{
   m_vMoveEvents.push_back(in_MoveEvent);
}

//Needed by the tutorials
GUI::GButton* GUnitAttackConfirmWindow::ButtonYes()
{
   return m_pBtnYes;
}
GUI::GButton* GUnitAttackConfirmWindow::ButtonNo()
{
   return m_pBtnNo;
}
//---------
