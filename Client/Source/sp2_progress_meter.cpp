/**************************************************************
*
* sp2_progress_meter.cpp
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

#include "../include/sp2_progress_meter.h"

const GString GProgressMeter::TypeName(L"Progress_Meter");

CWSTR sFRAME_PROGRESS = L"frmProgress";

GUI::GBaseObject* GProgressMeter::New()
{
   return new GProgressMeter();
}

GProgressMeter::GProgressMeter() : GUI::GFrame()
{
   m_iActualPercentage = 0;

   GSplitInfo l_FullSplit;

   l_FullSplit.m_fOffset = 0;
   l_FullSplit.m_fScale = 1;
   l_FullSplit.m_iLength = 100;
   l_FullSplit.m_iStart = 0;

   // We can surely predict that 8 levels of split will be enough
   m_vSplits.reserve(8);
   m_vSplits.push_back(l_FullSplit);
}

/*!
 * GProgressMeter creation handler.
 **/
bool GProgressMeter::OnCreate()
{
   __super::OnCreate();

   m_pTextLbl = (GUI::GLabel*)Child(L"txtText");

   //Update the render list
   g_Joshua.GUIManager()->Update();

   //Gets the length that corresponds to 100 percent
   GUI::GBaseObject* l_pFrame = Child(sFRAME_PROGRESS);

   //There should be a frame named sFRAME_PROGRESS
   assert(l_pFrame);

   m_iHundredPercentLength = l_pFrame->Width();

   //Update to reflect the actual percentage.
   Update();

   //Move the progress meter to the bottom of the screen
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
   this->Top((INT32)(0.9f*l_Mode.m_Resolution.y));


   return true;
}

void GProgressMeter::SetText(const GString & in_sText)
{
   m_pTextLbl->Text( in_sText );
}

void  GProgressMeter::Percentage(UINT8 in_iPercentage)
{
   m_iActualPercentage = in_iPercentage;
}

void  GProgressMeter::PercentageUpdate(UINT8 in_iPercentage)
{
   Percentage(in_iPercentage);
   Update();
}

UINT8 GProgressMeter::Percentage()
{
   const GSplitInfo& l_CurSplit = m_vSplits.back();

   return (UINT8) ( (REAL32) m_iActualPercentage * l_CurSplit.m_fScale + l_CurSplit.m_fOffset );
}


void  GProgressMeter::Split(UINT8 in_iPercentage)
{
   INT32 l_iActualSplitAmount = max(0, min(in_iPercentage - m_iActualPercentage, 100 - m_iActualPercentage) );

   if(l_iActualSplitAmount > 0)
   {
      const GSplitInfo& l_CurSplit = m_vSplits.back();
      GSplitInfo l_NewSplit;

      l_NewSplit.m_fOffset = l_CurSplit.m_fOffset + m_iActualPercentage * l_CurSplit.m_fScale;
      l_NewSplit.m_fScale = (REAL32) l_iActualSplitAmount * 0.01f * l_CurSplit.m_fScale;
      l_NewSplit.m_iLength = l_iActualSplitAmount;
      l_NewSplit.m_iStart = m_iActualPercentage;

      m_vSplits.push_back(l_NewSplit);
   }
}

UINT8 GProgressMeter::SplitPercentage()
{
   return (UINT8) m_vSplits.back().m_iLength;
}

void  GProgressMeter::EndSplit()
{
   if(m_vSplits.size() > 1)
   {
      const GSplitInfo& l_CurSplit = m_vSplits.back();

      m_iActualPercentage = l_CurSplit.m_iStart + l_CurSplit.m_iLength;;

      m_vSplits.pop_back();
   }
}

/*!
 * Update the Progress meter to reflect the correct percentage (%)
 **/
bool GProgressMeter::Update()
{
   this->BringToFront();
   INT32 l_iNewWidth = (Percentage() * m_iHundredPercentLength) / 100;
   GUI::GBaseObject* l_pFrame = Child(sFRAME_PROGRESS);
   //There should be a frame named sFRAME_PROGRESS
   assert(l_pFrame);
   l_pFrame->Width(l_iNewWidth);
   g_Joshua.GUIManager()->Update();
   g_Joshua.Renderer()->Render();

   return true;
}

