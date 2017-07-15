/**************************************************************
*
* sp2_advisor_advise_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_advisor_advise_window.h"

const GString GAdvisorAdviseWindow::TypeName(L"Advisor_Advise_Window");

GUI::GBaseObject* GAdvisorAdviseWindow::New()
{
   return (GUI::GBaseObject*) new GAdvisorAdviseWindow();
}

GAdvisorAdviseWindow::GAdvisorAdviseWindow()
{
}

GAdvisorAdviseWindow::~GAdvisorAdviseWindow()
{
}

GUI::EEventStatus::Enum GAdvisorAdviseWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   g_ClientDDL.AdvisorAdviseWindowKill();

   return GUI::EEventStatus::Handled;
}

void GAdvisorAdviseWindow::Set(GString in_sText, GUI::GBaseObject* in_pObject)
{
   SetText(in_sText);
   AttachTo(in_pObject);
}

void GAdvisorAdviseWindow::SetText(GString in_sText)
{
   ((GLabel*)Child(L"lblText"))->Text(in_sText);
   UINT32 l_iWidth = ((GLabel*)Child(L"lblText"))->Text2D()->Width();
   UINT32 l_iHeight = ((GLabel*)Child(L"lblText"))->Text2D()->Height();

   Size((INT16)l_iWidth + 10,(INT16)l_iHeight + 10);
   Child(L"lblText")->Size(Width() - 6, Height() - 6);
}

void GAdvisorAdviseWindow::AttachTo(GUI::GBaseObject* in_pObject)
{
   GVector2D<INT32> l_iCoord(in_pObject->RelativeToAbsolute(GVector2D<INT32>(0,0)));
   Left((INT16)l_iCoord.x + in_pObject->Left());
   Top((INT16)l_iCoord.y - Height() - 3);
}
