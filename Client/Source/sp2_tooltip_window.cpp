/**************************************************************
*
* sp2_tooltip_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_tooltip_window.h"

const GString GTooltipWindow::TypeName(L"Tooltip_Window");

/*!
 * MAIN MENU construction function
 **/
GUI::GBaseObject* GTooltipWindow::New()
{
	return (GUI::GBaseObject*) new GTooltipWindow();
}


GTooltipWindow::GTooltipWindow() : GUI::GFrame()
{
   m_pObjText = NULL;
}

GTooltipWindow::~GTooltipWindow()
{
}

bool GTooltipWindow::OnCreate()
{
	__super::OnCreate();
   BringToFront();

   m_pObjText = (GUI::GLabel*)Child(L"lblTooltip");

	return true;
}

void GTooltipWindow::OnShow()
{
   Size(200, 100);

   if (m_pObjText)
   {
      if((INT32)m_pObjText->Text2D()->Width() < m_pObjText->Width())
         Width(m_pObjText->Text2D()->Width() + 10);

      Height(m_pObjText->Text2D()->Height() + 10);

      while( Height() > Width() )
      {
         Width( Width() + 50 );
         Height( m_pObjText->Text2D()->Height() + 10 );
      }

      Position(g_Joshua.GUIManager()->MousePosition().x - Width() - 3, g_Joshua.GUIManager()->MousePosition().y - Height() - 3);

      __super::OnShow();
      BringToFront();   
   }
}

void GTooltipWindow::OnMove()
{
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if(Left() < 0)
      Left(g_Joshua.GUIManager()->MousePosition().x + 10);

   if(Top() < 0)
      Top(g_Joshua.GUIManager()->MousePosition().y + 10);
}

void GTooltipWindow::OnResize()
{
   m_pObjText->Size(Width() - 10, Height() - 10);
}

GUI::GLabel* GTooltipWindow::Label()
{
   return m_pObjText;
}