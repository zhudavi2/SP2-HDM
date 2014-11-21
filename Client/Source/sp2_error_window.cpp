/**************************************************************
*
* sp2_error_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_error_window.h"

const GString GErrorWindow::TypeName(L"Error_Window");

/*!
 * Creation function
 **/
GUI::GBaseObject* GErrorWindow::New()
{
   return new GErrorWindow();
}
GErrorWindow::GErrorWindow()
{
}

GErrorWindow::~GErrorWindow()
{
}

GUI::EEventStatus::Enum GErrorWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   return GUI::EEventStatus::Handled;
}



/*!
 * Set the content of the window
 * @param in_sText: The text to display
 * @return true if the text was displayed successfully, false otherwise
 **/
bool GErrorWindow::Set(GString in_sText)
{
   //Get the text box and set the text in it
   GUI::GLabel* l_pLabel = (GUI::GLabel*)Child(L"txtText");
   if(!l_pLabel)
      return false;

   //Set the text
   l_pLabel->Text(in_sText);

   //Make sure that the window is large enough to display the text
   if(this->Width() < (INT32)l_pLabel->Text2D()->Width())
   {
      Width((INT16)(l_pLabel->Text2D()->Width()+10));
   }
   return true;
}

GUI::EEventStatus::Enum GErrorWindow::OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Key() == VK_ESCAPE)
   {
      g_Joshua.GUIManager()->ReleaseObjectAsync(this);
   }
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GErrorWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)  
   {
      if(in_pCaller && in_pCaller->Name() == L"btnOk")
      {
         g_Joshua.GUIManager()->ReleaseObjectAsync(this);
      }
   }

   return GUI::EEventStatus::Handled;
}

