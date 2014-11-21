/**************************************************************
*
* sp2_history_marker_info_window.cpp
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

#include "../include/sp2_history_marker_info_window.h"

const GString GHistoryMarkerInfoWindow::TypeName = L"History_Tooltip_Window";

GUI::GBaseObject* GHistoryMarkerInfoWindow::New()
{
   return new GHistoryMarkerInfoWindow();
}

GHistoryMarkerInfoWindow::GHistoryMarkerInfoWindow()
   : m_pLabel(NULL)
{
}

bool GHistoryMarkerInfoWindow::OnCreate()
{
   __super::OnCreate();

   m_pLabel = (GLabel*) Child("txtText");
   assert(m_pLabel);

   m_pLabel->Multiline(true);

   return true;
}

void GHistoryMarkerInfoWindow::OnResize()
{
}

GLabel* GHistoryMarkerInfoWindow::Label()
{
   return m_pLabel;
}
