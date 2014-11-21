/**************************************************************
*
* sp2_strategic_warfare.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_strategic_warfare.h"

#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GStrategicWarfareWindow::TypeName(L"Strategic_Warfare");

GUI::GBaseObject* SP2::GStrategicWarfareWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicWarfareWindow();
}


GStrategicWarfareWindow::GStrategicWarfareWindow() : GBaseWindow()
{
}

GStrategicWarfareWindow::~GStrategicWarfareWindow()
{
}

GUI::EEventStatus::Enum GStrategicWarfareWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.StrategicWarfareWindow()->Hide();
         }
         
         if(in_pCaller->Name() == L"btnLaunch")
         {
            INT32 l_iAttackType = -1;
            for(INT32 i=100087; i<=100089; i++)
            {
               GString l_sAttackType = g_ClientDAL.GetString(i);
               if(l_sAttackType == m_pObjAttackType->Selected_Content())
                  l_iAttackType = i - 100087;
               
            }
                        
            /*       
            REAL32 l_iAttackStrength = m_pObjAttackCursor->Value();
            const GCountry& l_Country = g_ClientDAL.Country(m_pObjCountry->Selected_Content() );
            
            SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GStrategicWarfareLaunch); 
            SP2::Event::GStrategicWarfareLaunch* l_pLaunchEvent = (SP2::Event::GStrategicWarfareLaunch*)l_pEvent.get();

            //Create the launch request and add it to the game event
            GNukeLaunchRequest l_NukeLaunchRequest;
            {
               l_NukeLaunchRequest.m_iAttackerID = g_SP2Client->Id();
               l_NukeLaunchRequest.m_iTargetID   = l_Country.Id();
               l_NukeLaunchRequest.m_iMissileID  = 0;
               l_NukeLaunchRequest.m_TargetCoordinates = GVector2D<REAL32>(0,0);
               l_NukeLaunchRequest.m_TargetType = ENuclearTargetType::Military;
            }
            l_pLaunchEvent->m_vNukeLaunchRequests.push_back(l_NukeLaunchRequest);
            l_pLaunchEvent->m_iSource = g_SP2Client->Id();
            l_pLaunchEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
         
            g_Joshua.RaiseEvent(l_pEvent);
            */
         }
      }
   }

   return GUI::EEventStatus::Handled;
}


bool GStrategicWarfareWindow::OnCreate()
{
	__super::OnCreate();
/*
   m_pObjInnerFrame   = (GFrame*)Child(L"frmInner");
   m_pObjAttackCursor = (GProgressMeterYellowValue*)m_pObjInnerFrame->Child("frmAttackStrength")->Child("frmAttackStrengthValue");
   m_pObjCountry      = (GComboBox*)m_pObjInnerFrame->Child("frmCountry")->Child(L"cboCountry");
   m_pObjAttackType   = (GComboBox*)m_pObjInnerFrame->Child("frmAttackType")->Child(L"cboAttackType");

   m_pObjAttackStrengthValue = m_pObjAttackCursor->TextValueObj();

   m_pObjAttackCursor->Minimum(0,0);
   m_pObjAttackCursor->Maximum(100,0);
   m_pObjAttackCursor->Value(50,0);
   m_pObjAttackStrengthValue->Text(L"50%");

   for(INT32 i=1; i<=g_ClientDAL.NbCountry(); i++)
   {
      GCountry l_Country;
      g_ClientDAL.Country(i,l_Country);
      m_pObjCountry->Add_Item(l_Country.Name());
      if(i == 1)
         m_pObjCountry->Selected_Content(l_Country.Name());
   }

   for(INT32 i=100087; i<=100089; i++)
   {
      GString l_sAttackType = g_ClientDAL.GetString(i);
      m_pObjAttackType->Add_Item(l_sAttackType);
      m_vAttackType.push_back(l_sAttackType);
      if(i == 100087)
         m_pObjAttackType->Selected_Content(l_sAttackType);
   }
*/
	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GStrategicWarfareWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GStrategicWarfareWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseMove(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_pCaller->Name() == L"btnFill" || in_pCaller->Name() == L"btnUnfilled")
      {
         REAL32  l_fValue = (REAL32) m_pObjAttackCursor->Value(0);
         GString l_sValue = GString::FormatNumber((REAL64)l_fValue,L"",L"",L"",L"%",3,0);
         m_pObjAttackStrengthValue->Text(l_sValue);
      }
   }

   return GUI::EEventStatus::Handled;
}