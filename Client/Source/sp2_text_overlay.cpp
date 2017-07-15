/**************************************************************
*
* sp2_text_overlay.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_text_overlay.h"

const GString GTextOverlay::TypeName(L"txtOverlay");

GUI::GBaseObject* GTextOverlay::New()
{
	return (GUI::GBaseObject*) new GTextOverlay();
}


GTextOverlay::GTextOverlay() : GUI::GLabel()
{
}

GTextOverlay::~GTextOverlay()
{
}

GUI::EEventStatus::Enum GTextOverlay::OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller)
{

   return GUI::EEventStatus::Callback;
}

GUI::EEventStatus::Enum GTextOverlay::OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   Hide(); 
   return GUI::EEventStatus::Callback;
}

void GTextOverlay::Update(const GString & in_sText, GUI::GLabel* in_pCaller)
{
   if(!in_pCaller)
      return;

   Owner(in_pCaller);

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Text( in_sText );
   Width( (INT32)in_pCaller->Text2D()->Width() + 6 );
   Height( in_pCaller->Height() );
   
   FontColorNormal( in_pCaller->FontColorNormal() );
   FontSize( in_pCaller->FontSize() );
   TextOffset( in_pCaller->TextOffset() );

   GVector2D<INT32> l_AbsPos = in_pCaller->Owner()->RelativeToAbsolute( GVector2D<INT32>(in_pCaller->Left(), in_pCaller->Top()) );
   
   //If the box is too much at the bottom of the screen to be shown set it's top accordingly
   if( (l_AbsPos.y + Height()) > mode.m_Resolution.y )
   {
      Top( l_AbsPos.y - Height() );
   }
   else if( l_AbsPos.y < 0 )
   {
      Top( 0 );
   }
   else
   {
      Top( l_AbsPos.y );
   }

   if( (l_AbsPos.x + Width()) > mode.m_Resolution.x )
   {
      Left( l_AbsPos.x - (Width() - in_pCaller->Width()) );
   }
   else if( l_AbsPos.x < 0 )
   {
      Left( 0 );
   }
   else
   {
      Left( l_AbsPos.x );
   }
}