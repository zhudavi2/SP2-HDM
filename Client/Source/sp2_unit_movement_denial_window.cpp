/**************************************************************
*
* sp2_unit_movement_denial_window.cpp
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

#include "../include/sp2_unit_movement_denial_window.h"

const GString GUnitMovementDenialWindow::TypeName(L"Unit_Movement_Denial");
UINT32 GUnitMovementDenialWindow::m_iNextOffset = 0;

GUI::GBaseObject* SP2::GUnitMovementDenialWindow::New()
{
	return (GUI::GBaseObject*) new GUnitMovementDenialWindow();
}

GUnitMovementDenialWindow::GUnitMovementDenialWindow() : GFrame()
{
   const INT32 c_iOffsetInc = 10;
   const INT32 c_iOffsetMax = 60;

   m_pBtnClose = NULL;
   m_pLstDenials = NULL;

   m_iID = 0xFFFFFFFF;
   m_vMoveEvents.reserve(16);
   m_iOffset = m_iNextOffset;
   m_iNextOffset += c_iOffsetInc;
   if(m_iNextOffset >= c_iOffsetMax)
   {
      m_iNextOffset = 0;
   }
}

GUnitMovementDenialWindow::~GUnitMovementDenialWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Unit Attack Confirm Window");
}

bool GUnitMovementDenialWindow::OnCreate()
{
   __super::OnCreate();

   m_pBtnClose    = (GUI::GButton*)Child(L"btnClose");
   m_pLstDenials  = (GUI::GListBox*)Child(L"lstDenialList");

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   const INT32 c_iOffsetX = 10;
   const INT32 c_iOffsetY = 10;

   Position(mode.m_Resolution.x - Width() - c_iOffsetX, m_iOffset + c_iOffsetY);

	return true;
}

EEventStatus::Enum GUnitMovementDenialWindow::OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pBtnClose)
         {
            g_ClientDDL.UnitMovementDenialWindowKill(m_iID);
         }
      }
   }
   return EEventStatus::Handled;
}

void GUnitMovementDenialWindow::ID(UINT32 in_iNewID)
{
   if(m_iID == 0xFFFFFFFF)
   {
      m_iID = in_iNewID;
   }
}

UINT32 GUnitMovementDenialWindow::ID() const
{
   return m_iID;
}
