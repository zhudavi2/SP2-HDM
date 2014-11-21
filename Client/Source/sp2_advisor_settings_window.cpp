/**************************************************************
*
* sp2_advisor_settings_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_advisor_settings_window.h"
#include "../include/sp2_save_confirmation_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GAdvisorSettingsWindow::TypeName(L"Advisor_Settings_Window");

GUI::GBaseObject* GAdvisorSettingsWindow::New()
{
	return (GUI::GBaseObject*) new GAdvisorSettingsWindow();
}


GAdvisorSettingsWindow::GAdvisorSettingsWindow() : SP2::GBaseWindow()
{
   m_fWhenSaved = -1;
}

GAdvisorSettingsWindow::~GAdvisorSettingsWindow()
{
}

bool GAdvisorSettingsWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjTitleLbl             = (GUI::GLabel*)Child(L"txtTitle");
   m_pObjInnerFrm             = (GUI::GFrame*)Child(L"frmInner");

   m_pObjEconManualTog        = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togEconManual");
   m_pObjEconAutomaticTog     = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togEconAutomatic");
   m_pObjEconConfirmationTog  = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togEconConfirmations");
   m_pObjPoliManualTog        = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togPoliManual");
   m_pObjPoliAutomaticTog     = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togPoliAutomatic");
   m_pObjPoliConfirmationTog  = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togPoliConfirmations");
   m_pObjMiliManualTog        = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togMiliManual");
   m_pObjMiliAutomaticTog     = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togMiliAutomatic");
   m_pObjMiliConfirmationTog  = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSettings")->Child(L"togMiliConfirmations");

   m_pObjApplyBtn->Left( m_pObjLockTog->Left() );
   m_pObjLockTog->Hide();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GAdvisorSettingsWindow::OnShow()
{
   __super::OnShow();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Advisor);

   Update();
}

void GAdvisorSettingsWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Advisor);
   // remove the request associated with our ressource laws window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);

   __super::OnHide();

   g_ClientDDL.MainBar()->CloseAdvisor();
}

GUI::EEventStatus::Enum GAdvisorSettingsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjEconManualTog       ||
            in_pCaller == m_pObjEconAutomaticTog    ||
            in_pCaller == m_pObjEconConfirmationTog ||
            in_pCaller == m_pObjPoliManualTog       ||
            in_pCaller == m_pObjPoliAutomaticTog    ||
            in_pCaller == m_pObjPoliConfirmationTog ||
            in_pCaller == m_pObjMiliManualTog       ||
            in_pCaller == m_pObjMiliAutomaticTog    ||
            in_pCaller == m_pObjMiliConfirmationTog)
         {
            bool l_bDirty = false;

            if( m_pObjEconManualTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorEconomicStatus() != EAdvisorStatus::Manual) )
               l_bDirty = true;
            else if( m_pObjEconAutomaticTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorEconomicStatus() != EAdvisorStatus::Automatic) )
               l_bDirty = true;
            else if( m_pObjEconConfirmationTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorEconomicStatus() != EAdvisorStatus::Confirmation) )
               l_bDirty = true;

            if( m_pObjPoliManualTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorPoliticStatus() != EAdvisorStatus::Manual) )
               l_bDirty = true;
            else if( m_pObjPoliAutomaticTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorPoliticStatus() != EAdvisorStatus::Automatic) )
               l_bDirty = true;
            else if( m_pObjPoliConfirmationTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorPoliticStatus() != EAdvisorStatus::Confirmation) )
               l_bDirty = true;

            if( m_pObjMiliManualTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorMilitaryStatus() != EAdvisorStatus::Manual) )
               l_bDirty = true;
            else if( m_pObjMiliAutomaticTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorMilitaryStatus() != EAdvisorStatus::Automatic) )
               l_bDirty = true;
            else if( m_pObjMiliConfirmationTog->ToggleState() && (g_ClientDAL.m_PlayerCountryData.AdvisorMilitaryStatus() != EAdvisorStatus::Confirmation) )
               l_bDirty = true;

            Dirty(l_bDirty);
         }
			if(in_pCaller == m_pObjExitBtn)
			{
				if(Dirty())
            {
               if(!g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow)
               {
                  g_ClientDDL.SaveConfirmationWindow()->Caller(this);
                  g_ClientDDL.SaveConfirmationWindow()->Show();
               }
               else
               {
                  if(g_ClientDAL.GenericDataHolder().m_bSaveConfirmationAlwaysApply)
                  {
                     Save();
                     Hide();
                  }
                  else
                  {
                     Hide();
                  }
               }
            }
            else
            {
               Hide();
            }
			}
			else if(in_pCaller == m_pObjApplyBtn)
			{				
				Save();
			}
      }
   }

	return EEventStatus::Handled;
   //return __super::OnMouseClick(in_EventData, in_pCaller);
}

bool GAdvisorSettingsWindow::Update()
{
   /*
   // TO DO : REMOVE LATER, REQUEST MANAGER WILL DO THE JOB
   if( ((m_fWhenSaved + g_Joshua.Latency()) > (g_ClientDAL.m_PlayerCountryData.m_fWhenSent + (g_Joshua.Latency() / 2))) || Dirty() )
      return;
   */
   if(Dirty())
      return true;

   m_eEconomic = g_ClientDAL.m_PlayerCountryData.AdvisorEconomicStatus();
   m_ePolitic = g_ClientDAL.m_PlayerCountryData.AdvisorPoliticStatus();
   m_eMilitary = g_ClientDAL.m_PlayerCountryData.AdvisorMilitaryStatus();

   switch(m_eEconomic)
   {
   case EAdvisorStatus::Automatic:
      m_pObjEconManualTog->ToggleState(false);
      m_pObjEconAutomaticTog->ToggleState(true);
      m_pObjEconConfirmationTog->ToggleState(false);
      break;
   case EAdvisorStatus::Confirmation:
      m_pObjEconManualTog->ToggleState(false);
      m_pObjEconAutomaticTog->ToggleState(false);
      m_pObjEconConfirmationTog->ToggleState(true);
      break;
   case EAdvisorStatus::Manual:
      m_pObjEconManualTog->ToggleState(true);
      m_pObjEconAutomaticTog->ToggleState(false);
      m_pObjEconConfirmationTog->ToggleState(false);
      break;
   }

   switch(m_ePolitic)
   {
   case EAdvisorStatus::Automatic:
      m_pObjPoliManualTog->ToggleState(false);
      m_pObjPoliAutomaticTog->ToggleState(true);
      m_pObjPoliConfirmationTog->ToggleState(false);
      break;
   case EAdvisorStatus::Confirmation:
      m_pObjPoliManualTog->ToggleState(false);
      m_pObjPoliAutomaticTog->ToggleState(false);
      m_pObjPoliConfirmationTog->ToggleState(true);
      break;
   case EAdvisorStatus::Manual:
      m_pObjPoliManualTog->ToggleState(true);
      m_pObjPoliAutomaticTog->ToggleState(false);
      m_pObjPoliConfirmationTog->ToggleState(false);
      break;
   }

   switch(m_eMilitary)
   {
   case EAdvisorStatus::Automatic:
      m_pObjMiliManualTog->ToggleState(false);
      m_pObjMiliAutomaticTog->ToggleState(true);
      m_pObjMiliConfirmationTog->ToggleState(false);
      break;
   case EAdvisorStatus::Confirmation:
      m_pObjMiliManualTog->ToggleState(false);
      m_pObjMiliAutomaticTog->ToggleState(false);
      m_pObjMiliConfirmationTog->ToggleState(true);
      break;
   case EAdvisorStatus::Manual:
      m_pObjMiliManualTog->ToggleState(true);
      m_pObjMiliAutomaticTog->ToggleState(false);
      m_pObjMiliConfirmationTog->ToggleState(false);
      break;
   }

   Dirty(false);

   return true;
}

void GAdvisorSettingsWindow::Save()
{
   m_fWhenSaved = g_Joshua.GameTime();

   if(m_pObjEconManualTog->ToggleState())
      m_eEconomic = EAdvisorStatus::Manual;
   else if(m_pObjEconAutomaticTog->ToggleState())
      m_eEconomic = EAdvisorStatus::Automatic;
   else
      m_eEconomic = EAdvisorStatus::Confirmation;

   if(m_pObjPoliManualTog->ToggleState())
      m_ePolitic = EAdvisorStatus::Manual;
   else if(m_pObjPoliAutomaticTog->ToggleState())
      m_ePolitic = EAdvisorStatus::Automatic;
   else
      m_ePolitic = EAdvisorStatus::Confirmation;

   if(m_pObjMiliManualTog->ToggleState())
      m_eMilitary = EAdvisorStatus::Manual;
   else if(m_pObjMiliAutomaticTog->ToggleState())
      m_eMilitary = EAdvisorStatus::Automatic;
   else
      m_eMilitary = EAdvisorStatus::Confirmation;

   g_ClientDAL.SendAdvisorSettings(m_eEconomic, m_ePolitic, m_eMilitary, this);

   Dirty(false);
}

void GAdvisorSettingsWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   // is there any request pending for our advisor settings window, if yes do not notify
   if(!RequestPending())
      if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Advisor)
         Update();
}