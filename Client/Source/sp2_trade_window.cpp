/**************************************************************
*
* sp2_trade_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_trade_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GTradeWindow::TypeName(L"Trade_Window");

/*!
 * Window allocator
 **/
GUI::GBaseObject* GTradeWindow::New()
{
	return (GUI::GBaseObject*) new GTradeWindow();
}

/*!
 * Constructor
 **/
GTradeWindow::GTradeWindow() : GBaseWindow()
{
   // Init our objects data
   m_pObjInnerFrm = NULL;
}

/*!
 * Destructor
 **/
GTradeWindow::~GTradeWindow()
{
}

/*!
 * Called when the object is created
 **/
bool GTradeWindow::OnCreate()
{
	__super::OnCreate();

   // Frames
   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

	return true;
}

/*!
 * Called when the object is destroyed
 **/
bool GTradeWindow::OnDestroy()
{
   return __super::OnDestroy();
}

/*!
 * Called when the object is displayed
 **/
void GTradeWindow::OnShow()
{
   __super::OnShow();

   // Bring to front main bar and mini map
   if(g_ClientDDL.MainBar())
      g_ClientDDL.MainBar()->BringToFront();
   if(g_ClientDDL.MiniMap())
      g_ClientDDL.MiniMap()->BringToFront();
}

/*!
 * Called when the object is hidden
 **/
void GTradeWindow::OnHide()
{
   __super::OnHide();
}

/*!
 * Called when the mouse is pressed down
 **/
EEventStatus::Enum GTradeWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the mouse is moved
 **/
EEventStatus::Enum GTradeWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the mouse is up
 **/
EEventStatus::Enum GTradeWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the object is clicked
 **/
EEventStatus::Enum GTradeWindow::OnMouseClick(const GEventData &in_EventData, GUI::GBaseObject *in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when a custom event happens
 **/
EEventStatus::Enum GTradeWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   return EEventStatus::Handled;
}