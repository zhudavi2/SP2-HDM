/**************************************************************
*
* sp2_credits_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_credits_window.h"

const GString GCreditsWindow::TypeName(L"Credits_Window");

GUI::GBaseObject* GCreditsWindow::New()
{
	return (GUI::GBaseObject*) new GCreditsWindow();
}


GCreditsWindow::GCreditsWindow() : GBaseWindow()
{
}

GCreditsWindow::~GCreditsWindow()
{
}

bool GCreditsWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");
   m_pObjScrl     = (GUI::GVScrollBar*)m_pObjInnerFrm->Child(L"scrFrm");
   m_pObjCreditsFrm = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmCredits")->Child(L"frmInner");

   //SP2 Steam, HACK create space for Nordic Games
   m_pObjCreditsFrm->Height(m_pObjCreditsFrm->Height() + 95);         
   for (GGUIObjects::iterator it = m_pObjCreditsFrm->GetChildren().begin(); it != m_pObjCreditsFrm->GetChildren().end(); it++)
   {
      std::string::size_type found = it->second->Name().find(L"lblTitle");
      if(found!=std::string::npos &&  it->second->Top() >= 636)
      {
         it->second->Top(it->second->Top() + 140);
      }
   }   

   ((GFX::GWindow*)m_pObjCreditsFrm->Owner()->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

   m_pObjScrl->Initialize(this, 0, max(m_pObjCreditsFrm->Height() - m_pObjCreditsFrm->Owner()->Height(),0) ,15,500);

	return true;
}

GUI::EEventStatus::Enum GCreditsWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData & in_Data, GUI::GBaseObject * in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnScrollVertical)
   {
      m_pObjCreditsFrm->Top(-(INT16)in_Data.Data);
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GCreditsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.CreditsWindow()->Hide();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GCreditsWindow::OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
	if(m_pObjScrl)
	{
		return m_pObjScrl->OnMouseWheel(in_EventData, in_pCaller);
	}	

	return __super::OnMouseWheel(in_EventData, in_pCaller);;
}