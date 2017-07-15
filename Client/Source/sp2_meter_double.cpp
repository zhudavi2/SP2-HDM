/**************************************************************
*
* sp2_meter_double.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_meter_double.h"
#include "../include/sp2_data_display_layer.h"

const GString GMeterDouble::TypeName(L"frmMeterDouble");

GUI::GBaseObject* GMeterDouble::New()
{
   return new GMeterDouble();
}

GMeterDouble::GMeterDouble()
{
   m_bDragging = false;
}

GMeterDouble::~GMeterDouble()
{
}

bool GMeterDouble::OnCreate()
{
   __super::OnCreate();

   m_pObjRedBg       = (GUI::GPictureBox*)Child(L"picRed");
   m_pObjGreenBg     = (GUI::GPictureBox*)Child(L"picGreen");
   m_pObjRedMeter    = (GUI::GPictureBox*)Child(L"picRedMeter");
   m_pObjGreenMeter  = (GUI::GPictureBox*)Child(L"picGreenMeter");
   m_pObjMiddle      = (GUI::GPictureBox*)Child(L"picMiddle");

   m_pObjRedMeter->BringToFront();
   m_pObjGreenMeter->BringToFront();

   return true;
}

bool GMeterDouble::OnDestroy()
{
   __super::OnDestroy();

   return true;
}

GUI::EEventStatus::Enum GMeterDouble::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      m_bDragging = true;

      OnMouseMove(in_EventData, in_pCaller);
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMeterDouble::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      OnMouseMove(in_EventData, in_pCaller);
      
      m_bDragging = false;
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMeterDouble::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(m_bDragging)
   {
      INT32 x = in_EventData.X();

      bool l_bSkipSetValue = false;
      if(x == -1)
      {
         x = (INT32)((152.f * m_fValue) + 4);
         l_bSkipSetValue = true;
      }

      if(x > 82)
      {
         m_pObjRedMeter->Hide();
         m_pObjGreenMeter->Show();

         if(x > 156)
            x = 156;

         m_pObjGreenMeter->Width(x - 80);
         m_pObjGreenMeter->Left(82);
      }

      if(x <= 80 && x >= 76)
      {
         m_pObjRedMeter->Hide();
         m_pObjGreenMeter->Show();

         m_pObjGreenMeter->Left(x);
         m_pObjGreenMeter->Width(4);
      }

      if(x < 76)
      {
         m_pObjGreenMeter->Hide();
         m_pObjRedMeter->Show();

         if(x < 4)
            x = 4;

         m_pObjRedMeter->Left(x);
         m_pObjRedMeter->Width(80 - x);
      }

      if(!l_bSkipSetValue)
         m_fValue = ((REAL64)x - 4.f) / 152.f;

      if(x >= 78 && x <= 82)
      {
         if(!l_bSkipSetValue)
            m_fValue = 0.5f;

         if(!m_pObjGreenMeter->Visible())
         {
            m_pObjRedMeter->Hide();
            m_pObjGreenMeter->Show();
         }
         
         m_pObjGreenMeter->Left(79);
         m_pObjGreenMeter->Width(4);
      }

      if(m_bCallEvent)
      {
         GUI::GEventData l_Data;
         l_Data.Data = (void*)this;
         Owner()->FireCustomEvent(c_OnMeterDoubleChange, l_Data, true, this);
      }
      else
         m_bCallEvent = true;
   }

   return GUI::EEventStatus::Handled;
}

REAL64 GMeterDouble::Value()
{
   return m_fValue;
}

void   GMeterDouble::Value(REAL64 in_fValue, bool in_bCallEvent)
{
   m_bCallEvent = in_bCallEvent;

   if(in_fValue < 0)
      in_fValue = 0;

   if(in_fValue > 1)
      in_fValue = 1;

   m_fValue = in_fValue;

   GEventData l_Data;
   l_Data.Mouse.Position.x = -1;

   m_bDragging = true;
   OnMouseMove(l_Data, NULL);
   m_bDragging = false;
}