/**************************************************************
*
* sp2_frame_cursor_meter.cpp
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

#include "../include/sp2_frame_cursor_meter.h"

const GString GCursorMeter::TypeName(L"frmCursorMeter");
const GString GGradientBar::TypeName(L"frmGradientBar");
const GString GProgressMeterYellowValue::TypeName(L"frmProgressMeterYellowValue");
const GString GProgressMeterYellowValueSmall::TypeName(L"frmProgressMeterYellowValueSmall");
const GString GTechnologyMeterBlue::TypeName(L"frmTechnologyMeterBlue");
const GString GBattleOverviewMeter::TypeName(L"frmBattleOverviewMeter");
const GString GFinancingMeter::TypeName(L"frmFinancingMeter");
const GString GIdeologyMeter::TypeName(L"frmIdeologyMeter");
const GString GSplitMeter::TypeName(L"frmSplitMeter");

GUI::GBaseObject* GCursorMeter::New()
{
   return new GCursorMeter();
}

GUI::GBaseObject* GGradientBar::New()
{
   return new GGradientBar();
}

GUI::GBaseObject* GProgressMeterYellowValue::New()
{
   return new GProgressMeterYellowValue();
}

GUI::GBaseObject* GProgressMeterYellowValueSmall::New()
{
   return new GProgressMeterYellowValueSmall();
}

GUI::GBaseObject* GTechnologyMeterBlue::New()
{
   return new GTechnologyMeterBlue();
}

GUI::GBaseObject* GBattleOverviewMeter::New()
{
   return new GBattleOverviewMeter();
}

GUI::GBaseObject* GFinancingMeter::New()
{
   return new GFinancingMeter();
}

GUI::GBaseObject* GIdeologyMeter::New()
{
   return new GIdeologyMeter();
}

GUI::GBaseObject* GSplitMeter::New()
{
   return new GSplitMeter();
}

//---------------------------------------------------
// GProgressMeterYellowValue Stuff
//---------------------------------------------------

bool GProgressMeterYellowValue::OnCreate()
{
   if(!__super::OnCreate())
      return false;
   
   NbCursor(2);
   StepWidth(1);
   LeftBound(0);
   RightBound(161);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   CursorDragable()[0] = false;
   Locked()[0] = false;
   Lockable()[0] = false;
   BtnWidth()[0] = 0;
   Value(0,0);

   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);
   
   TextValueObj((GUI::GLabel*)Child(L"lblValue"));
   Initialize();

   return true;
}

//---------------------------------------------------
// GProgressMeterYellowValueSmall Stuff
//---------------------------------------------------

bool GProgressMeterYellowValueSmall::OnCreate()
{
   if(!__super::OnCreate())
      return false;
   
   NbCursor(2);
   
   StepWidth(1);
   LeftBound(0);
   RightBound(81);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   CursorDragable()[0] = false;
   Locked()[0] = false;
   Lockable()[0] = false;
   BtnWidth()[0] = 0;
   Value(0,0);


   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);



   TextValueObj((GUI::GLabel*)Child(L"lblValue"));
   Initialize();

   return true;
}

bool GGradientBar::OnCreate()
{
   if(!__super::OnCreate())
      return false;

   NbCursor(2);

   StepWidth(1);
   LeftBound(0);
   RightBound(143);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   CursorDragable()[0] = false;
   Locked()[0] = false;
   Lockable()[0] = true;
   BtnWidth()[0] = 0;
   Value(0.5f,0);


   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);

   Initialize();

   return true;
}

//---------------------------------------------------
// GTechnologyMeterBlue Stuff
//---------------------------------------------------

GTechnologyMeterBlue::GTechnologyMeterBlue() : GCursorMeter()
{
}

bool GTechnologyMeterBlue::OnCreate()
{
   if(!__super::OnCreate())
      return false;

   NbCursor(2);

   StepWidth(8);
   LeftBound(0);
   RightBound(81);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 10;
   Minimum()[0] = 0;
   CursorDragable()[0] = true;
   Locked()[0] = false;
   Lockable()[0] = true;
   BtnWidth()[0] = 0;
   Value(5,0);

   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 10;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(10,1);
   
   ((GFX::GWindow*)Child(L"frmBackground")->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);
   Initialize();
   Child(L"frmBackground")->SendToBack();

   return true;
}

GIdeologyMeter::GIdeologyMeter() : GCursorMeter()
{
}

bool GIdeologyMeter::OnCreate()
{
   __super::OnCreate();

   NbCursor(2);
   StepWidth(8);
   LeftBound(4);
   RightBound(180);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1.04;
   Minimum()[0] = 0;
   CursorDragable()[0] = false;
   Locked()[0] = false;
   Lockable()[0] = false;
   BtnWidth()[0] = 7;
   Value(0.5,0);

   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);

   Initialize();
   
   ((GFX::GWindow*)MeterBtn()[1]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);

   return true;
}

GSplitMeter::GSplitMeter() : GCursorMeter()
{
}

bool GSplitMeter::OnCreate()
{
   __super::OnCreate();

   NbCursor(2);
   StepWidth(1);
   LeftBound(4);
   RightBound(83);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   CursorDragable()[0] = true;
   Locked()[0] = false;
   Lockable()[0] = false;
   BtnWidth()[0] = 0;
   Value(0.5,0);

   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);

   Initialize();
   ((GFX::GWindow*)MeterBtn()[0]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Border);

   return true;
}

GUI::EEventStatus::Enum GCursorMeter::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(__super::OnMouseUp(in_EventData, in_pCaller) == GUI::EEventStatus::Handled)
      return GUI::EEventStatus::Handled;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(m_bDragging)
      {
         OnMouseMove(in_EventData,in_pCaller);
         m_bDragging = false;
      }
   }

   return GUI::EEventStatus::Callback;
}

//---------------------------------------------------
// GBattleOverviewMeter Stuff
//---------------------------------------------------

GBattleOverviewMeter::GBattleOverviewMeter() : GCursorMeter()
{
}

bool GBattleOverviewMeter::OnCreate()
{
   if(!__super::OnCreate())
      return false;

   NbCursor(2);
   StepWidth(1);
   LeftBound(4);
   RightBound(438);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   BtnWidth()[0] = 0;
   Locked()[0]   = true;
   Lockable()[0] = true;
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   Value(0.5,0);
   
   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   BtnWidth()[1] = 0;
   Locked()[1]   = true;
   Lockable()[1] = true;
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   Value(1,1);

   Initialize();
   ((GFX::GWindow*)m_vpMeterBtn[0]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Stretch);
   ((GFX::GWindow*)m_vpMeterBtn[1]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Stretch);

   if(Child(L"picBar"))
   {
      Child(L"picBar")->Hide();
   }

   return true;
}

GFinancingMeter::GFinancingMeter() : GCursorMeter()
{
}

bool GFinancingMeter::OnCreate()
{
   if(!__super::OnCreate())
      return false;

   NbCursor(2);
   StepWidth(1);
   LeftBound(3);
   RightBound(94);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 100;
   Minimum()[0] = 0;
   CursorDragable()[0] = true;
   Locked()[0] = false;
   Lockable()[0] = true;
   BtnWidth()[0] = 0;
   Value(50,0);

   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 100;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(100,1);

   Initialize();
   ((GFX::GWindow*)MeterBtn()[0]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Border);

   return true;
}


//----------------------------------
GCursorMeter::GCursorMeter()
{
   m_pObjTextValue = NULL;
   m_bDragging     = false;
}

GCursorMeter::~GCursorMeter()
{}

bool GCursorMeter::OnCreate()
{
   if(!__super::OnCreate())
      return false;

   NbCursor(2);

   StepWidth(1);
   LeftBound(4);
   RightBound(188);

   MeterBtn()[0] = (GUI::GButton*)Child(L"btnFill");
   Maximum()[0] = 1;
   Minimum()[0] = 0;
   CursorDragable()[0] = true;
   Locked()[0] = false;
   Lockable()[0] = true;
   BtnWidth()[0] = 8;
   Value(0.5f,0);


   MeterBtn()[1] = (GUI::GButton*)Child(L"btnUnfilled");
   Maximum()[1] = 1;
   Minimum()[1] = 0;
   CursorDragable()[1] = false;
   Locked()[1] = true;
   Lockable()[1] = false;
   BtnWidth()[1] = 0;
   Value(1,1);
//   ((GFX::GWindow*)MeterBtn()[0]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Border);
//   ((GFX::GWindow*)MeterBtn()[1]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Border);

   Initialize();

   if(Child(L"picBar"))
   {
      MeterBtn()[1]->PictureNormal(L"000159");
      Child(L"picBar")->SendToBack();
      ((GUI::GPictureBox*)Child(L"picBar"))->Borders(GBorders(5,0,5,0));
   }

   return true;
}

GUI::EEventStatus::Enum GCursorMeter::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(__super::OnMouseDown(in_EventData, in_pCaller) == GUI::EEventStatus::Handled)
      return GUI::EEventStatus::Handled;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller)
      {

         //If the meter is enabled, allow to drag
         if(Enabled())
         {
            for(m_iMeterId=0; m_iMeterId<(INT32)m_vpMeterBtn.size(); m_iMeterId++)
            {
               if(in_pCaller == m_vpMeterBtn[m_iMeterId])
                  break;
            }

            if((m_iMeterId + 1) == (INT32)m_vpMeterBtn.size())
               m_iMeterId--;

            if(!m_vbCursorDragable[m_iMeterId] || m_vbLocked[m_iMeterId])
               return EEventStatus::Handled;

            m_bDragging = true;

            if(in_pCaller->Name() == L"btnUnfilled" || in_pCaller->Name() == L"btnFill")
               in_pCaller->Owner()->Drag();
            else
               in_pCaller->Drag();

            OnDrag(in_EventData, GVector2D<INT32>(0,0), GVector2D<INT32>(0,0));
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);   
}

GUI::EEventStatus::Enum GCursorMeter::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(__super::OnMouseMove(in_EventData, in_pCaller) == GUI::EEventStatus::Handled)
      return GUI::EEventStatus::Handled;

   return GUI::EEventStatus::Callback;
}

void GCursorMeter::OnDrag(const GUI::GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative)
{
   GVector2D<INT32> l_Position = AbsoluteToRelative(g_Joshua.GUIManager()->MousePosition());
   
   INT32 l_iPosition = l_Position.x;

   INT32 l_iLowerBound;
   INT32 l_iUpperBound;

   //Set the lower bound of the drag also check if it's the first
   //meter part that has been clicked to take good bound
   if(m_iMeterId == 0)
      l_iLowerBound = m_iLeftBound;
   else
      l_iLowerBound = m_vpMeterBtn[0]->Left() + 1;

   
   //If it's the last meter part we can go to the highest bound possible
   //if not, it's the next part bound - 1 pixel (so the next remain clickable)
   if( (m_iMeterId == ( (INT32)m_vpMeterBtn.size() - 1) ) || (m_iMeterId + 1 == ((INT32)m_vpMeterBtn.size() - 1) ) )
   {
      if( m_iMeterId + 1 == (INT32)m_vpMeterBtn.size() )
         l_iUpperBound = /*m_vpMeterBtn[m_iMeterId + 1]->Left() + */m_vpMeterBtn[1]->Width();
      else
         l_iUpperBound = /*m_vpMeterBtn[m_iMeterId + 1]->Left() + */m_vpMeterBtn[1]->Width();
   }
   else
   {
      if( m_iMeterId + 1 == (INT32)m_vpMeterBtn.size() )
         l_iUpperBound = /*m_vpMeterBtn[m_iMeterId + 1]->Left() + */m_vpMeterBtn[1]->Width() - 1;
      else
         l_iUpperBound = /*m_vpMeterBtn[m_iMeterId + 1]->Left() + */m_vpMeterBtn[1]->Width() - 1;
   }

   //Check if position is in the bounds
   if(l_iPosition < l_iLowerBound)
   {
      l_iPosition = l_iLowerBound;
   }
   else if(l_iPosition > l_iUpperBound)
   {
      l_iPosition = l_iUpperBound;
   }

   REAL64 l_fValueRatio = (REAL64)((REAL64)l_iPosition - (REAL64)m_iLeftBound) / ((REAL64)m_iRightBound - (REAL64) m_iLeftBound);      
   Value((l_fValueRatio * (m_vfMaximum[1] - m_vfMinimum[0])) ,0,true);
}

void GCursorMeter::UpdateCursors()
{
   REAL64 l_fTotalValue = 0.0f;

   for(INT32 i=0; i<(INT32)m_vpLinkedCursors.size(); i++)
      if(!m_vpLinkedCursors[i]->Locked()[m_iMeterId])
         l_fTotalValue += m_vpLinkedCursors[i]->Value(m_iMeterId);

   REAL64 l_fToDistribute = m_fMeterValue - m_vfValue[m_iMeterId];

   if(l_fToDistribute == 0)
      return;

   for(INT32 i=0; i<(INT32)m_vpLinkedCursors.size(); i++)
   {
      if(!m_vpLinkedCursors[i]->Locked()[m_iMeterId])
      {
         REAL64 l_fValue = 0.f;
         if(l_fTotalValue != 0)
         {
            l_fValue = (m_vpLinkedCursors[i]->Value(m_iMeterId) / l_fTotalValue) * l_fToDistribute;
            //gassert(l_fValue != 0, "l_fValue != 0");
         }
         else
         {
            gassert(m_vpLinkedCursors.size() != 0, "m_vpLinkedCursors.size() == 0");
            l_fValue = l_fToDistribute / (REAL64)m_vpLinkedCursors.size();
         }

         if((m_vpLinkedCursors[i]->m_vfValue[m_iMeterId] + l_fValue) < m_vfMinimum[m_iMeterId])
            m_vpLinkedCursors[i]->m_vfValue[m_iMeterId] = m_vfMinimum[m_iMeterId];
         else
         {
            if((m_vpLinkedCursors[i]->m_vfValue[m_iMeterId] + l_fValue) > m_vfMaximum[m_iMeterId])
               m_vpLinkedCursors[i]->m_vfValue[m_iMeterId] = m_vfMaximum[m_iMeterId];
            else
               m_vpLinkedCursors[i]->m_vfValue[m_iMeterId] += l_fValue;
         }
         
         gassert(m_vpLinkedCursors[i]->Maximum()[m_iMeterId] - m_vpLinkedCursors[i]->Minimum()[m_iMeterId] != 0, "m_vpLinkedCursors[i]->Maximum()[m_iMeterId] - m_vpLinkedCursors[i]->Minimum()[m_iMeterId] == 0");
         REAL64 l_fRatio = (m_vpLinkedCursors[i]->Value(m_iMeterId) - m_vpLinkedCursors[i]->Minimum()[m_iMeterId]) / (m_vpLinkedCursors[i]->Maximum()[m_iMeterId] - m_vpLinkedCursors[i]->Minimum()[m_iMeterId]);
         l_fValue = (l_fRatio * (m_iRightBound - m_iLeftBound)) + m_iLeftBound;

         gassert(l_fValue >= 0,"Trying to put value smaller than 0 in a cursor");
         
         m_vpLinkedCursors[i]->m_vpMeterBtn[m_iMeterId]->Width((INT16)(l_fValue) );
      }
   }

}

void  GCursorMeter::Value(const REAL64 & in_fValue, const INT32 & in_iBtnId, bool in_bCallEvent, bool in_bUpdateLinked)
{
   m_iMeterId = (INT16)in_iBtnId;
   REAL64 l_Value = in_fValue;

   gassert(!_isnan(l_Value),"isnan");
   
   //if the value is not between min. and max. then return
   if(l_Value < m_vfMinimum[in_iBtnId])
      l_Value = m_vfMinimum[in_iBtnId];
   
   if(l_Value > m_vfMaximum[in_iBtnId])
      l_Value = m_vfMaximum[in_iBtnId];

   //set the old value
   m_fMeterValue = m_vfValue[in_iBtnId];

   //set the new value
   m_vfValue[in_iBtnId] = l_Value;



   REAL64 l_fDeltaMeter = Maximum()[m_vpMeterBtn.size() - 1] - Minimum()[0];
   //with the value, calculate the new position
   if(l_fDeltaMeter <= 0.000001f)
      m_iMeterNewPosition = (INT16) m_iLeftBound; 
   else
      m_iMeterNewPosition = (INT16)(((l_Value - Minimum()[0]) / (Maximum()[m_vpMeterBtn.size() - 1] - Minimum()[0])) * (m_iRightBound - m_iLeftBound)) + m_iLeftBound;

   //calculate the ratio of the calue 
   //REAL64 l_fValueRatio = (REAL64)((REAL64)m_iMeterNewPosition - (REAL64)m_iLeftBound) / ((REAL64)m_iRightBound - (REAL64) m_iLeftBound);

   if(m_iStepWidth > 1)
   {
      m_iMeterNewPosition = (m_iMeterNewPosition - ((m_iMeterNewPosition - m_iLeftBound) % m_iStepWidth));
      //m_iMeterNewPosition = (m_iMeterNewPosition - ((m_iMeterNewPosition - m_iLeftBound) % m_iStepWidth)) + m_iLeftBound;
   }

   if(m_viBtnWidth[m_iMeterId] > 0)
   {
      m_vpMeterBtn[m_iMeterId]->Left(m_iMeterNewPosition);
   }
   else
   {
      m_vpMeterBtn[m_iMeterId]->Width(m_iMeterNewPosition/* - m_iLeftBound*/);
   }

   if(in_bUpdateLinked)
      UpdateCursors();

   if(in_bCallEvent)
   {
      GOnMeterChangeData l_Data;
      l_Data.m_iBtnId    = m_iMeterId;
      l_Data.m_fBtnValue = (REAL32)m_vfValue[m_iMeterId];

      gassert(!_isnan(l_Data.m_fBtnValue),"isnan");
      gassert(!_isnan(m_vfValue[m_iMeterId]),"isnan");
      gassert(!_isnan((REAL32)m_vfValue[m_iMeterId]),"isnan");

      GEventData l_EventData;
      l_EventData.Data = &l_Data;

      Owner()->FireCustomEvent(c_OnMeterChangeEvent, l_EventData, false, this);
   }
}

const REAL64 & GCursorMeter::Value(const INT32 & in_iBtnId)
{
   return m_vfValue[in_iBtnId];
}

GUI::EEventStatus::Enum GCursorMeter::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return __super::OnMouseClick(in_EventData, in_pCaller);
}

void GCursorMeter::Initialize()
{
   for(INT32 i=0; i<(INT32)m_vpMeterBtn.size(); i++)
   {
      m_vpMeterBtn[i]->SendToBack();
      ((GFX::GWindow*)m_vpMeterBtn[i]->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);
   }
}

vector<GUI::GButton*> & GCursorMeter::MeterBtn()
{
   return m_vpMeterBtn;
}

vector<bool> & GCursorMeter::CursorDragable()
{
   return m_vbCursorDragable;
}

void GCursorMeter::TextValueObj(GUI::GLabel* in_pTextObj)
{
   m_pObjTextValue = in_pTextObj;
}

GUI::GLabel* GCursorMeter::TextValueObj()
{
   return m_pObjTextValue;
}

vector<REAL64> & GCursorMeter::Minimum()
{
   return m_vfMinimum;
}

vector<REAL64> & GCursorMeter::Maximum()
{
   return m_vfMaximum;
}

vector<bool>   & GCursorMeter::Locked()
{
   return m_vbLocked;
}

vector<bool>   & GCursorMeter::Lockable()
{
   return m_vbLockable;
}

void GCursorMeter::StepWidth(INT16 in_iPixelValue)
{
   m_iStepWidth = in_iPixelValue;
}

void GCursorMeter::LeftBound(INT16 in_iPixelValue)
{
   m_iLeftBound = in_iPixelValue;
}

void GCursorMeter::RightBound(INT16 in_iPixelValue)
{
   m_iRightBound = in_iPixelValue;
}

void GCursorMeter::NbCursor(const INT32 & in_iValue)
{
   m_iNbCursor = (INT16)in_iValue;

   //Resize the vectors so it now fits the wanted value
   m_vpMeterBtn.resize(m_iNbCursor);
   m_vbCursorDragable.resize(m_iNbCursor);
   m_vfMinimum.resize(m_iNbCursor);
   m_vfMaximum.resize(m_iNbCursor);
   m_vfValue.resize(m_iNbCursor);
   m_vbLocked.resize(m_iNbCursor);
   m_vbLockable.resize(m_iNbCursor);
   m_viBtnWidth.resize(m_iNbCursor);
}

UINT32 GCursorMeter::NumberOfLinks() const
{
	return m_vpLinkedCursors.size();
}

void GCursorMeter::AddLinkedCursorToGroup(GCursorMeter* in_pLinkedCursor)
{
   //Add new linked to this list
   m_vpLinkedCursors.push_back(in_pLinkedCursor);

   //For each known linked cursor add the new linked one
   for(UINT32 i=0; i<(UINT32)m_vpLinkedCursors.size(); i++)
   {
      m_vpLinkedCursors[i]->m_vpLinkedCursors.clear();

      for(UINT32 j=0; j<(UINT32)m_vpLinkedCursors.size(); j++)
      {
         if(m_vpLinkedCursors[i] != m_vpLinkedCursors[j])
            m_vpLinkedCursors[i]->m_vpLinkedCursors.push_back(m_vpLinkedCursors[j]);
         else
            m_vpLinkedCursors[i]->m_vpLinkedCursors.push_back(this);
      }
   }
}

vector<INT16>  & GCursorMeter::BtnWidth()
{
   return m_viBtnWidth;
}