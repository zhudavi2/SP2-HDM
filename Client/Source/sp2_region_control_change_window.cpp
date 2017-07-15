/**************************************************************
*
* sp2_region_control_change_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_region_control_change_window.h"

const GString GRegionControlChangeWindow::TypeName(L"Region_Control_Change_Window");

GUI::GBaseObject* GRegionControlChangeWindow::New()
{
	return (GUI::GBaseObject*) new GRegionControlChangeWindow();
}


GRegionControlChangeWindow::GRegionControlChangeWindow() : GFrame()
{
}

GRegionControlChangeWindow::~GRegionControlChangeWindow()
{
}

bool GRegionControlChangeWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjOkBtn   = (GUI::GButton*)Child(L"btnOk");
   m_pObjTextFrm = (GUI::GFrame*)Child(L"frmClip");
   m_pObjTextLbl = (GUI::GLabel*)m_pObjTextFrm->Child(L"lblMessage");
   m_pObjTextScr = (GUI::GVScrollBar*)Child(L"scrMessage");

   // Activate text clipping for the label
   ( (GFX::GWindow*) m_pObjTextFrm->Model()->Get_Primitive() )->ClipChildren(true);

	return true;
}

GUI::EEventStatus::Enum GRegionControlChangeWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller == m_pObjOkBtn)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         Hide();
      }
   }
   
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GRegionControlChangeWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_EventId == ECustomEvents::OnScrollVertical)
   {
      m_pObjTextLbl->Top(-(INT16)in_EventData.Data);
   }

   return GUI::EEventStatus::Handled;
}

void GRegionControlChangeWindow::Show(GString in_sMessage)
{
   m_pObjTextLbl->Text(Hector::Util::ReplaceEndLinesByRealEndLines(in_sMessage));
	m_pObjTextLbl->Top(0);

   if(m_pObjTextLbl->Height() != (INT32)m_pObjTextLbl->Text2D()->Height())
   {
      m_pObjTextLbl->Height(m_pObjTextLbl->Text2D()->Height());
      m_pObjTextScr->Initialize(this, 0, max(0, m_pObjTextLbl->Height() - m_pObjTextFrm->Height()), 15, 45);
   }

   __super::Show();
}