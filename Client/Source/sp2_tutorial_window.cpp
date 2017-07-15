/**************************************************************
*
* sp2_tutorial_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_tutorial_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_battle_overview_unit.h"

using namespace SP2;

const GString GTutorialWindow::TypeName(L"Tutorial_Window");

void (*func)(void);

GBaseObject * GTutorialWindow::New()
{      
   return new GTutorialWindow();
}

GTutorialWindow::GTutorialWindow(void)
{
}

GTutorialWindow::~GTutorialWindow(void)
{
}

void GTutorialWindow::OnShow()
{
   //Resize the window
   SetupStandardSize();
}

bool GTutorialWindow::OnCreate()
{
   __super::OnCreate();

   //Reposition the tutorial window
   Size(400, 400);
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
   Position((l_Mode.m_Resolution.x - Width()) / 2, (INT32)((l_Mode.m_Resolution.x - Height()) * 0.4));

   m_pLabel = (GLabel*)Child(L"lblTooltip");
   m_pLabel->OnShow();
   m_pLabel->Size(400, 400);
   m_pLabel->FontSize(12);
   m_pArrow = (GFrame*)CREATE_GUI_OBJECT(L"Tutorial_Arrow_Window", L"Tutorial_Arrow_Window", NULL, false, false);      
   m_pArrow->Owner(NULL);

   return true;
}

GUI::EEventStatus::Enum GTutorialWindow::OnMouseClick(const GUI::GEventData & in_EventData, GUI::GBaseObject *)
{
   return EEventStatus::Handled;
}

void GTutorialWindow::PointTo(GBaseObject * in_BaseObject)
{
   GVector2D<INT32> l_Position = in_BaseObject->RelativeToAbsolute(GVector2D<INT32>(0,0));
   l_Position.x += in_BaseObject->Width() / 2;
   PointTo(l_Position);
}

void GTutorialWindow::PointTo(GVector2D<INT32> & in_Position)
{
   m_pArrow->Position(in_Position.x - 25, in_Position.y - 50);
   m_pArrow->Show();
   m_pArrow->BringToFront();
}

void GTutorialWindow::HideArrow()
{
   if (m_pArrow)
      m_pArrow->Hide();
}

void GTutorialWindow::OnHide()
{
   HideArrow();
}

bool GTutorialWindow::OnDestroy()
{
   m_pArrow = NULL;
   __super::OnDestroy();
   return true;
}

void GTutorialWindow::PointToLatLong(const GVector2D<REAL32>& in_Coord)
{
   GVector2D<INT32> l_Position2D;
   GVector3D<REAL32> l_SphericalCoord = g_ClientDDL.BackgroundEarth()->ConvertSphericalToRectangular(in_Coord);
   g_ClientDDL.BackgroundEarth()->ProjectAndClipOnEarth3DPointIn2D(l_SphericalCoord,l_Position2D);
   //Dont let the arrow hide the icon
   l_Position2D.y -= 10;
   PointTo(l_Position2D);
   return;
}

void GTutorialWindow::PointTo(GVector2D<REAL32> in_Position,GBaseObject* in_pParent)
{
   GVector2D<INT32>  l_iRelativePos((INT32)in_Position.x,(INT32)in_Position.y);
   GVector2D<INT32>  l_AbsolutePosition = in_pParent->RelativeToAbsolute(l_iRelativePos);
   PointTo(l_AbsolutePosition);
}

void GTutorialWindow::CenterAndResize()
{
   //Reposition the tutorial window
   Size(400, 400);
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
   Position((l_Mode.m_Resolution.x - Width()) / 2, (INT32)((l_Mode.m_Resolution.x - Height()) * 0.4));
   SetupStandardSize();
}

void GTutorialWindow::SetupStandardSize()
{
   //Resize the window
   m_pObjText->Size(Width() - 10, Height() - 10);
   Height(m_pLabel->Text2D()->Height() + 10);
   BringToFront();
}

void GTutorialWindow::MoveToTop()
{
   //Reposition the tutorial window
   Size(400, 400);
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
   Position((l_Mode.m_Resolution.x - Width()) / 2, (INT32)((l_Mode.m_Resolution.x - Height()) * 0.05));
   SetupStandardSize();
}

void GTutorialWindow::MoveToBottom()
{
	//Reposition the tutorial window
	Size(400, 400);
	CGFX_Renderer_Init_Mode l_Mode;
	g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
	Position((l_Mode.m_Resolution.x - Width()) / 4, (INT32)((l_Mode.m_Resolution.y - Height()) ));
	SetupStandardSize();
}