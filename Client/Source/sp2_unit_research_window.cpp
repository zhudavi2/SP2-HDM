/**************************************************************
*
* sp2_unit_research_window.cpp
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

#include "../include/sp2_unit_research_window.h"

#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_military_window.h"
#include "../include/sp2_unit_view_scene.h"

const GString GUnitResearchWindow::TypeName(L"Unit_Research_Window");

//static members
SP2::EUnitCategory::Enum GUnitResearchWindow::m_Filter;
GUnitResearchWindow* GUnitResearchWindow::m_pThis;

GUI::GBaseObject* GUnitResearchWindow::New()
{
   return new GUnitResearchWindow();
}

GUnitResearchWindow::GUnitResearchWindow() : GBaseWindow()
{
   m_pThis     = this;
   m_bDragging = false;
   NeedCountryCheckToSave(false);
}

GUnitResearchWindow::~GUnitResearchWindow()
{

}

bool GUnitResearchWindow::OnCreate()
{
   __super::OnCreate();

   m_pOriginalInfo = &(g_ClientDAL.m_ResearchInfo);

   Child(L"frmTitle")->Child(L"picRedBar")->BringToFront();
   Child(L"frmMain")->BringToFront();
   Child(L"frmView")->BringToFront();

   m_pObjApplyBtn->BringToFront();

   //Create the 3D Model Scene
   m_pModelFrm = (GUI::GFrame*)Child("frmView")->Child("frmModel");

   //Initialize pointer to GUI objects
   m_pObjGroundTog      = (GUI::GToggle*)Child(L"frmMain")->Child(L"togGround");
   m_pObjAirTog         = (GUI::GToggle*)Child(L"frmMain")->Child(L"togAir");
   m_pObjNavalTog       = (GUI::GToggle*)Child(L"frmMain")->Child(L"togNaval");
   m_pObjStrategicTog   = (GUI::GToggle*)Child(L"frmMain")->Child(L"togStrategic");
   m_pObjResearchTab    = (GUI::GTabbed*)Child(L"frmDetails")->Child(L"tabResearch");
   m_pObjExitBtn        = (GUI::GButton*)Child(L"frmTitle")->Child(L"btnExit");
  
   m_pObjResearchTab->m_vTabPage[0]->ColorNormal(GColorRGBInt(0,0,0,255));
   m_pObjResearchTab->m_vTabPage[0]->PictureNormal(L"256");
   m_pObjResearchTab->m_vTabPage[0]->Borders( GBorders(1,0,1,1) );
   m_pObjResearchTab->m_vTabPage[0]->Height(m_pObjResearchTab->m_vTabPage[0]->Height() + 1);
   GPictureBox* l_Pic = CREATE_GUI_OBJECT_NAMED_STATIC(GPictureBox, L"TopPicture1",m_pObjResearchTab->m_vTabPage[0], true);
   l_Pic->PictureNormal(L"258");
   l_Pic->Position(100,0);
   l_Pic->Size(464,1);

   m_pObjResearchTab->m_vTabPage[1]->ColorNormal(GColorRGBInt(0,0,0,255));
   m_pObjResearchTab->m_vTabPage[1]->PictureNormal(L"256");
   m_pObjResearchTab->m_vTabPage[1]->Borders( GBorders(1,0,1,1) );
   m_pObjResearchTab->m_vTabPage[1]->Height(m_pObjResearchTab->m_vTabPage[1]->Height() + 1);
   l_Pic = CREATE_GUI_OBJECT_NAMED_STATIC(GPictureBox, L"TopPicture2",m_pObjResearchTab->m_vTabPage[1],true);
   l_Pic->PictureNormal(L"258");
   l_Pic->Position(201,0);
   l_Pic->Size(363,1);

   m_pObjResearchTab->m_vTabPage[2]->ColorNormal(GColorRGBInt(0,0,0,255));
   m_pObjResearchTab->m_vTabPage[2]->PictureNormal(L"256");
   m_pObjResearchTab->m_vTabPage[2]->Borders( GBorders(1,0,1,1) );
   m_pObjResearchTab->m_vTabPage[2]->Height(m_pObjResearchTab->m_vTabPage[2]->Height() + 1);
   l_Pic = CREATE_GUI_OBJECT_NAMED_STATIC(GPictureBox,L"TopPicture3",m_pObjResearchTab->m_vTabPage[2],true);
   l_Pic->PictureNormal(L"258");
   l_Pic->Position(302,0);
   l_Pic->Size(262,1);

   m_pObjResearchTab->m_vTabPage[0]->TooltipText( g_ClientDAL.GetString(102241) );
   m_pObjResearchTab->m_vTabPage[1]->TooltipText( g_ClientDAL.GetString(102242) );
   m_pObjResearchTab->m_vTabPage[2]->TooltipText( g_ClientDAL.GetString(102243) );
   m_pObjResearchTab->m_vTabButtons[0]->TooltipText( g_ClientDAL.GetString(102241) );
   m_pObjResearchTab->m_vTabButtons[1]->TooltipText( g_ClientDAL.GetString(102242) );
   m_pObjResearchTab->m_vTabButtons[2]->TooltipText( g_ClientDAL.GetString(102243) );

   m_vMainFinancing[0] = (SP2::GFinancingMeter*)Child(L"frmMain")->Child(L"frmGroundFinancing")->Child(L"frmFinancing");
   m_vMainFinancing[1] = (SP2::GFinancingMeter*)Child(L"frmMain")->Child(L"frmAirFinancing")->Child(L"frmFinancing");
   m_vMainFinancing[2] = (SP2::GFinancingMeter*)Child(L"frmMain")->Child(L"frmNavalFinancing")->Child(L"frmFinancing");
   m_vMainFinancing[3] = (SP2::GFinancingMeter*)Child(L"frmMain")->Child(L"frmStrategicFinancing")->Child(L"frmFinancing");

   GUI::GFrame* l_pFrame = (GUI::GFrame*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner");
   m_vSpecTechnology[0]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmGunRange");
   m_vSpecTechnology[1]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmGunPrecision");
   m_vSpecTechnology[2]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmGunDamage");
   m_vSpecTechnology[3]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmMissileRange");
   m_vSpecTechnology[4]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmMissilePrecision");
   m_vSpecTechnology[5]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmMissileDamage");
   m_vSpecTechnology[6]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmMissilePayload");
   m_vSpecTechnology[7]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmSpeed");
   m_vSpecTechnology[8]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmSensors");
   m_vSpecTechnology[9]  = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmArmor");
   m_vSpecTechnology[10] = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmStealth");
   m_vSpecTechnology[11] = (SP2::GTechnologyMeterBlue*)l_pFrame->Child(L"frmCountermeasures");

   l_pFrame = (GUI::GFrame*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner");
   m_vSpecProgress[0]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmGunRange");
   m_vSpecProgress[1]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmGunPrecision");
   m_vSpecProgress[2]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmGunDamage");
   m_vSpecProgress[3]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmMissileRange");
   m_vSpecProgress[4]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmMissilePrecision");
   m_vSpecProgress[5]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmMissileDamage");
   m_vSpecProgress[6]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmMissilePayload");
   m_vSpecProgress[7]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmSpeed");
   m_vSpecProgress[8]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmSensors");
   m_vSpecProgress[9]  = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmArmor");
   m_vSpecProgress[10] = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmStealth");
   m_vSpecProgress[11] = (SP2::GProgressMeterYellowValueSmall*)l_pFrame->Child(L"frmCountermeasures");

   l_pFrame = (GUI::GFrame*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner");
   m_vSpecFinancing[0]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmGunRange");
   m_vSpecFinancing[1]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmGunPrecision");
   m_vSpecFinancing[2]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmGunDamage");
   m_vSpecFinancing[3]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmMissileRange");
   m_vSpecFinancing[4]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmMissilePrecision");
   m_vSpecFinancing[5]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmMissileDamage");
   m_vSpecFinancing[6]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmMissilePayload");
   m_vSpecFinancing[7]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmSpeed");
   m_vSpecFinancing[8]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmSensors");
   m_vSpecFinancing[9]  = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmArmor");
   m_vSpecFinancing[10] = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmStealth");
   m_vSpecFinancing[11] = (SP2::GFinancingMeter*)l_pFrame->Child(L"frmCountermeasures");
   
   m_vTechnologyLbl[0] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblGunRange");
   m_vTechnologyLbl[1] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblGunPrecision");
   m_vTechnologyLbl[2] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblGunDamage");
   m_vTechnologyLbl[3] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblMissileRange");
   m_vTechnologyLbl[4] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblMissilePrecision");
   m_vTechnologyLbl[5] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblMissileDamage");
   m_vTechnologyLbl[6] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblMissilePayload");
   m_vTechnologyLbl[7] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblSpeed");
   m_vTechnologyLbl[8] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblSensors");
   m_vTechnologyLbl[9] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblArmor");
   m_vTechnologyLbl[10] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblStealth");
   m_vTechnologyLbl[11] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[0]->Child(L"frmInner")->Child(L"lblCountermeasures");

   m_vFinancingLbl[0] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblGunRange");
   m_vFinancingLbl[1] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblGunPrecision");
   m_vFinancingLbl[2] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblGunDamage");
   m_vFinancingLbl[3] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblMissileRange");
   m_vFinancingLbl[4] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblMissilePrecision");
   m_vFinancingLbl[5] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblMissileDamage");
   m_vFinancingLbl[6] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblMissilePayload");
   m_vFinancingLbl[7] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblSpeed");
   m_vFinancingLbl[8] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblSensors");
   m_vFinancingLbl[9] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblArmor");
   m_vFinancingLbl[10] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblStealth");
   m_vFinancingLbl[11] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[1]->Child(L"frmInner")->Child(L"lblCountermeasures");

   m_vProgressLbl[0] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblGunRange");
   m_vProgressLbl[1] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblGunPrecision");
   m_vProgressLbl[2] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblGunDamage");
   m_vProgressLbl[3] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblMissileRange");
   m_vProgressLbl[4] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblMissilePrecision");
   m_vProgressLbl[5] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblMissileDamage");
   m_vProgressLbl[6] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblMissilePayload");
   m_vProgressLbl[7] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblSpeed");
   m_vProgressLbl[8] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblSensors");
   m_vProgressLbl[9] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblArmor");
   m_vProgressLbl[10] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblStealth");
   m_vProgressLbl[11] = (GUI::GLabel*)m_pObjResearchTab->m_vTabPage[2]->Child(L"frmInner")->Child(L"lblCountermeasures");

   for(INT8 i=1; i<3; i++)
   {
      m_vMainFinancing[0]->AddLinkedCursorToGroup(m_vMainFinancing[i]);				
   }
	if(g_ClientDAL.m_PlayerCountryData.NuclearReady() >= 1.f)
		m_vMainFinancing[0]->AddLinkedCursorToGroup(m_vMainFinancing[3]);
   
   for(INT8 i=1; i<12; i++)
   {
      m_vSpecFinancing[0]->AddLinkedCursorToGroup(m_vSpecFinancing[i]);
   }

   for(INT8 i=0; i<12; i++)
   {
      m_vSpecProgress[i]->TextValueObj((GUI::GLabel*)m_vSpecProgress[i]->Child(L"lblValue"));
      m_vSpecFinancing[i]->Owner(this);
      m_vSpecTechnology[i]->CursorDragable()[0] = false;
      m_vSpecTechnology[i]->MeterBtn()[1]->SendToBack();

      if(i < 4)
         m_vMainFinancing[i]->Owner(this);
   }

   m_pObjGroundTog->ToggleState(true);
   m_Filter = EUnitCategory::Ground;

   m_pObjGroundTog->BringToFront();
   m_pObjAirTog->BringToFront();
   m_pObjNavalTog->BringToFront();
   m_pObjStrategicTog->BringToFront();

   g_ClientDAL.m_ResearchInfo.Update();

   return true;
}

bool GUnitResearchWindow::OnDestroy()
{
   __super::OnDestroy();
   return true;
}

void GUnitResearchWindow::OnShow()
{
   __super::OnShow();

   m_pOriginalInfo = &(g_ClientDAL.m_ResearchInfo);

   // Update our window data
   UpdateBudget();   
   UpdateMaxValues();
   Update(m_Filter);

   // Set up timer for window refresh procedure
   m_iUpdateTimerId = SetTimer(NULL, 10001, 5000, UpdateTimerProc);
}

void GUnitResearchWindow::OnHide()
{
   KillTimer(NULL, m_iUpdateTimerId);
   // remove the request associated with our ressource laws window (if any)
   __super::OnHide();
}

GUI::EEventStatus::Enum GUnitResearchWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return __super::OnMouseDown(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GUnitResearchWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitResearchWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitResearchWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjGroundTog)
         {
            Update(SP2::EUnitCategory::Ground);
            m_pModelFrm->PictureNormal(L"000884");
         }

         if(in_pCaller == m_pObjAirTog)
         {
            Update(SP2::EUnitCategory::Air);
            m_pModelFrm->PictureNormal(L"000883");
         }

         if(in_pCaller == m_pObjNavalTog)
         {
            Update(SP2::EUnitCategory::Naval);
            m_pModelFrm->PictureNormal(L"000885");
         }

         if(in_pCaller == m_pObjStrategicTog)
         {
            Update(SP2::EUnitCategory::Nuclear);
            m_pModelFrm->PictureNormal(L"000886");
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitResearchWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch(in_EventId)
   {
   // On Meter Change event
   case c_OnMeterChangeEvent:
      {
         Dirty(true);

         bool l_bFound = false;
         
         // One of the four main financing meter ?
         INT32 i;
         for(i=0; i<4; i++)
         {
            if(m_vMainFinancing[i] == in_pCaller)
            {
               l_bFound = true;
               break;
            }
         }

         if(l_bFound)
         {
            REAL64 l_fTotal = 0;
            l_fTotal += m_ModifiedInfo.m_fBudgetAir;
            l_fTotal += m_ModifiedInfo.m_fBudgetGround;
            l_fTotal += m_ModifiedInfo.m_fBudgetNaval;
            l_fTotal += m_ModifiedInfo.m_fBudgetNuclear;

            if(!l_fTotal)
               return EEventStatus::Handled;

            REAL64 l_fOldAirValue      = m_ModifiedInfo.m_fBudgetAir;
            REAL64 l_fOldGroundValue   = m_ModifiedInfo.m_fBudgetGround;
            REAL64 l_fOldNavalValue    = m_ModifiedInfo.m_fBudgetNaval;
            REAL64 l_fOldNuclearValue  = m_ModifiedInfo.m_fBudgetNuclear;

            m_ModifiedInfo.m_fBudgetGround   = l_fTotal * (m_vMainFinancing[0]->Value(0) / 100);
            gassert(!_isnan(m_ModifiedInfo.m_fBudgetGround),"isnan");

            m_ModifiedInfo.m_fBudgetAir      = l_fTotal * (m_vMainFinancing[1]->Value(0) / 100);
            gassert(!_isnan(m_ModifiedInfo.m_fBudgetAir),"isnan");            

            m_ModifiedInfo.m_fBudgetNaval    = l_fTotal * (m_vMainFinancing[2]->Value(0) / 100);
            gassert(!_isnan(m_ModifiedInfo.m_fBudgetNaval),"isnan");

            m_ModifiedInfo.m_fBudgetNuclear  = l_fTotal * (m_vMainFinancing[3]->Value(0) / 100);
            gassert(!_isnan(m_ModifiedInfo.m_fBudgetNuclear),"isnan");

            // Ground main financing 
            REAL64 l_fRatio = (m_ModifiedInfo.m_fBudgetGround / l_fTotal) / (l_fOldGroundValue / l_fTotal);
            if(l_fOldGroundValue)
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]           *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]          *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]   *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]      *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]     *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]              *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]            *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]              *= l_fRatio;
            }
            else
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]         = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]     = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]        = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]     = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision] = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]    = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]   = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]            = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]          = m_ModifiedInfo.m_fBudgetGround / 10;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]            = m_ModifiedInfo.m_fBudgetGround / 10;
            }

            // Air main financing 
            l_fRatio = (m_ModifiedInfo.m_fBudgetAir / l_fTotal) / (l_fOldAirValue / l_fTotal);
            if(l_fOldAirValue)
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]           *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]          *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]   *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]      *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]     *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]              *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]            *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]              *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]            *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]     *= l_fRatio;
            }
            else
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]           = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]       = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]          = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]       = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]   = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]      = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]     = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]              = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]            = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]              = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]            = m_ModifiedInfo.m_fBudgetAir / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]     = m_ModifiedInfo.m_fBudgetAir / 12;
            }

            // Naval main financing 
            l_fRatio = (m_ModifiedInfo.m_fBudgetNaval / l_fTotal) / (l_fOldNavalValue / l_fTotal);
            if(l_fOldNavalValue)
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]           *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]          *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]       *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]   *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]      *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]     *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]              *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]            *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]              *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]            *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]     *= l_fRatio;
            }
            else
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]           = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]       = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]          = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]       = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]   = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]      = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]     = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]              = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]            = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]              = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]            = m_ModifiedInfo.m_fBudgetNaval / 12;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]     = m_ModifiedInfo.m_fBudgetNaval / 12;
            }

            // Strategic main financing 
            l_fRatio = (m_ModifiedInfo.m_fBudgetNuclear / l_fTotal) / (l_fOldNuclearValue / l_fTotal);
            if(l_fOldNuclearValue)
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Speed]             *= l_fRatio;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::Sensors]           *= l_fRatio;
            }
            else
            {
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      = m_ModifiedInfo.m_fBudgetNuclear / 3;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  = m_ModifiedInfo.m_fBudgetNuclear / 3;
               m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     = m_ModifiedInfo.m_fBudgetNuclear / 3;
            }
            return GUI::EEventStatus::Handled;
         }

         // One of the specific financing meter ?
         for(i=0; i<12; i++)
         {
            if(m_vSpecFinancing[i] == in_pCaller)
            {
               l_bFound = true;
               break;
            }
         }

         if(l_bFound)
         {
            switch(m_Filter)
            {
               case SP2::EUnitCategory::Ground:
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]         = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[0]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]     = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[1]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]        = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[2]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]     = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[3]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision] = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[4]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]    = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[5]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]   = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[6]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]            = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[7]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]          = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[8]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]            = m_ModifiedInfo.m_fBudgetGround * (m_vSpecFinancing[9]->Value(0) / 100);
                  break;

               case SP2::EUnitCategory::Air:
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]          = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[0]->Value(0) / 100);                  
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]      = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[1]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]         = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[2]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]      = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[3]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]  = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[4]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]     = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[5]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]    = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[6]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]             = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[7]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]           = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[8]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]             = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[9]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]           = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[10]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]    = m_ModifiedInfo.m_fBudgetAir * (m_vSpecFinancing[11]->Value(0) / 100);
                  break;

               case SP2::EUnitCategory::Naval:
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]         = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[0]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]     = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[1]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]        = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[2]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]     = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[3]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision] = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[4]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]    = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[5]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]   = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[6]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]            = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[7]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]          = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[8]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]            = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[9]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]          = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[10]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]   = m_ModifiedInfo.m_fBudgetNaval * (m_vSpecFinancing[11]->Value(0) / 100);
                  break;

               case SP2::EUnitCategory::Nuclear:
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      = m_ModifiedInfo.m_fBudgetNuclear * (m_vSpecFinancing[3]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  = m_ModifiedInfo.m_fBudgetNuclear * (m_vSpecFinancing[4]->Value(0) / 100);
                  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     = m_ModifiedInfo.m_fBudgetNuclear * (m_vSpecFinancing[5]->Value(0) / 100);
                  break;
            }

            return GUI::EEventStatus::Handled;
         }
      }
      break;

   default:
      return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);
   }

   return GUI::EEventStatus::Handled;
}

void GUnitResearchWindow::SetRemainingDays(GUI::GLabel* in_pLabel, INT32 in_iNbDays)
{
   if(in_iNbDays != -1)
      in_pLabel->Text(GString(in_iNbDays) + L" " + g_ClientDAL.GetString(EStrId::Days));
   else
      in_pLabel->Text(g_ClientDAL.GetString(EStrId::NA));
}

void GUnitResearchWindow::Update(REAL32 in_fNewBudget)
{	
   if(Dirty())
      return;

   if(in_fNewBudget)
   {
      REAL64 l_fOldTotal = m_pOriginalInfo->m_fBudgetAir + m_pOriginalInfo->m_fBudgetGround + m_pOriginalInfo->m_fBudgetNaval + m_pOriginalInfo->m_fBudgetNuclear;
      
      if(l_fOldTotal)
      {
         BudgetAir(    in_fNewBudget * (m_pOriginalInfo->m_fBudgetAir     / l_fOldTotal));
         BudgetGround( in_fNewBudget * (m_pOriginalInfo->m_fBudgetGround  / l_fOldTotal));
         BudgetNaval(  in_fNewBudget * (m_pOriginalInfo->m_fBudgetNaval   / l_fOldTotal));
         BudgetNuclear(in_fNewBudget * (m_pOriginalInfo->m_fBudgetNuclear / l_fOldTotal));
      }
      else
      {
         BudgetAir(    in_fNewBudget * 0.25f);
         BudgetGround( in_fNewBudget * 0.25f);
         BudgetNaval(  in_fNewBudget * 0.25f);
         BudgetNuclear(in_fNewBudget * 0.25f);
      }
   }
   else
   {
      BudgetAir(0);
      BudgetGround(0);
      BudgetNaval(0);
      BudgetNuclear(0);
   }
   UpdateBudget();
   Update(m_Filter);
}

void GUnitResearchWindow::Update(SP2::EUnitCategory::Enum in_Filter)
{
   if(g_ClientDAL.m_PlayerCountryData.NuclearReady() < 1.f)
	{
		m_pObjStrategicTog->Enabled(false);
		m_vMainFinancing[3]->Enabled(false);
		m_vMainFinancing[3]->Value(0,0,false,false);
	}
	else
	{
		if(m_vMainFinancing[0]->NumberOfLinks() == 3)
			m_vMainFinancing[0]->AddLinkedCursorToGroup(m_vMainFinancing[3]);
	}

   gassert(!_isnan(m_ModifiedInfo.m_fBudgetAir),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetGround),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetNaval),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetNuclear),"isnan");

   m_Filter = in_Filter;

   // Retrieve total budget
   REAL64 l_fTotal = 0;
   l_fTotal += m_ModifiedInfo.m_fBudgetAir;
   l_fTotal += m_ModifiedInfo.m_fBudgetGround;
   l_fTotal += m_ModifiedInfo.m_fBudgetNaval;
   l_fTotal += m_ModifiedInfo.m_fBudgetNuclear;

   // Disable everything
   for(INT8 i=0; i<12; i++)
   {
      m_vSpecTechnology[i]->Enabled(true);
      m_vSpecFinancing[i] ->Enabled(true);
      m_vSpecProgress[i]  ->Enabled(true);
      m_vTechnologyLbl[i] ->Enabled(true);
      m_vFinancingLbl[i]  ->Enabled(true);
      m_vProgressLbl[i]   ->Enabled(true);
      m_vSpecTechnology[i]->Value(0.0f,0);
      m_vSpecFinancing[i] ->Value(0.0f,0,false,false);
      m_vSpecProgress[i]  ->Value(0.0f,0);
      m_vSpecProgress[i]  ->TextValueObj()->Text(L"");
      m_vSpecFinancing[i] ->Locked()[0] = false;
   }

   // Set up the main financing meters
   if(l_fTotal)
   {
      m_vMainFinancing[0]->Value(m_ModifiedInfo.m_fBudgetGround  / l_fTotal * 100,0,false,false);
      m_vMainFinancing[1]->Value(m_ModifiedInfo.m_fBudgetAir     / l_fTotal * 100,0,false,false);
      m_vMainFinancing[2]->Value(m_ModifiedInfo.m_fBudgetNaval   / l_fTotal * 100,0,false,false);
      m_vMainFinancing[3]->Value(m_ModifiedInfo.m_fBudgetNuclear / l_fTotal * 100,0,false,false);
   }
   else
   {
      m_vMainFinancing[0]->Value(0,0,false,false);
      m_vMainFinancing[1]->Value(0,0,false,false);
      m_vMainFinancing[2]->Value(0,0,false,false);
      m_vMainFinancing[3]->Value(0,0,false,false);
   }

   switch(in_Filter)
   {
   // -------------- GROUND ---------------
   case SP2::EUnitCategory::Ground:

      // ------------- TECHNOLOGY ------------
      m_vSpecTechnology[0] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange],        0);
      m_vSpecTechnology[1] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision],    0);
      m_vSpecTechnology[2] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage],       0);
      m_vSpecTechnology[3] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange],    0);
      m_vSpecTechnology[4] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision],0);
      m_vSpecTechnology[5] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage],   0);
      m_vSpecTechnology[6] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload],  0);
      m_vSpecTechnology[7] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed],           0);
      m_vSpecTechnology[8] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors],         0);
      m_vSpecTechnology[9] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor],           0);
      m_vSpecTechnology[10]->Enabled(false);
      m_vSpecTechnology[11]->Enabled(false);

      m_vTechnologyLbl[10] ->Enabled(false);
      m_vFinancingLbl[10]  ->Enabled(false);
      m_vProgressLbl[10]   ->Enabled(false);
      m_vTechnologyLbl[11] ->Enabled(false);
      m_vFinancingLbl[11]  ->Enabled(false);
      m_vProgressLbl[11]   ->Enabled(false);


      // ------------- FINANCING ------------
      if(m_ModifiedInfo.m_fBudgetGround)
      {
         m_vSpecFinancing[0] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]         / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[1] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]     / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[2] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]        / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[3] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]     / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[4] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision] / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[5] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]    / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[6] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]   / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[7] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]            / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[8] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]          / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[9] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]            / m_ModifiedInfo.m_fBudgetGround * 100,0,false,false);
         m_vSpecFinancing[10]->Enabled(false);
         m_vSpecFinancing[10]->Locked()[0] = true;
         m_vSpecFinancing[11]->Enabled(false);
         m_vSpecFinancing[11]->Locked()[0] = true;
      }

      // ------------ PROGRESSION ------------
      m_vSpecProgress[0] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]        - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]        ),0);
      m_vSpecProgress[1] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]    - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]    ),0);
      m_vSpecProgress[2] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]       - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]       ),0);
      m_vSpecProgress[3] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]    - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]    ),0);
      m_vSpecProgress[4] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]- (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]),0);
      m_vSpecProgress[5] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]   - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]   ),0);
      m_vSpecProgress[6] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]  - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]  ),0);
      m_vSpecProgress[7] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]           - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]           ),0);
      m_vSpecProgress[8] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]         - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]         ),0);
      m_vSpecProgress[9] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]           - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]           ),0);
      m_vSpecProgress[10]->Enabled(false);
      m_vSpecProgress[11]->Enabled(false);

      SetRemainingDays(m_vSpecProgress[0]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]         ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]          ));
      SetRemainingDays(m_vSpecProgress[1]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]      ));
      SetRemainingDays(m_vSpecProgress[2]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]        ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]         ));
      SetRemainingDays(m_vSpecProgress[3]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]      ));
      SetRemainingDays(m_vSpecProgress[4]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision] ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]  ));
      SetRemainingDays(m_vSpecProgress[5]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]    ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]     ));
      SetRemainingDays(m_vSpecProgress[6]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]   ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]    ));
      SetRemainingDays(m_vSpecProgress[7]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]             ));
      SetRemainingDays(m_vSpecProgress[8]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]          ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]           ));
      SetRemainingDays(m_vSpecProgress[9]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]             ));
         
      break;

   // -------------- AIR ---------------
   case SP2::EUnitCategory::Air:

      // ------------- TECHNOLOGY ------------
      m_vSpecTechnology[0] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange],        0);
      m_vSpecTechnology[1] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision],    0);
      m_vSpecTechnology[2] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage],       0);
      m_vSpecTechnology[3] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange],    0);
      m_vSpecTechnology[4] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision],0);
      m_vSpecTechnology[5] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage],   0);
      m_vSpecTechnology[6] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload],  0);
      m_vSpecTechnology[7] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed],           0);
      m_vSpecTechnology[8] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors],         0);
      m_vSpecTechnology[9] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor],           0);
      m_vSpecTechnology[10]->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth],         0);
      m_vSpecTechnology[11]->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures],  0);

      // ------------- FINANCING ------------
      if(m_ModifiedInfo.m_fBudgetAir)
      {         
         m_vSpecFinancing[0]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]        / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[1]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]    / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[2]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]       / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[3]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]    / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[4]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]/ m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[5]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]   / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[6]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]  / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[7]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]           / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[8]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]         / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[9]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]           / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[10]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]        / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
         m_vSpecFinancing[11]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures] / m_ModifiedInfo.m_fBudgetAir * 100,0,false,false);
      }

      // ------------ PROGRESSION ------------
      m_vSpecProgress[0] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]           - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]        ),0);
      m_vSpecProgress[1] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]       - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]    ),0);
      m_vSpecProgress[2] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]          - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]       ),0);
      m_vSpecProgress[3] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]       - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]    ),0);
      m_vSpecProgress[4] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]   - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]),0);
      m_vSpecProgress[5] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]      - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]   ),0);
      m_vSpecProgress[6] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]     - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]  ),0);
      m_vSpecProgress[7] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]              - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]           ),0);
      m_vSpecProgress[8] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]            - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]         ),0);
      m_vSpecProgress[9] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]              - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]           ),0);
      m_vSpecProgress[10]->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]            - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]         ),0);
      m_vSpecProgress[11]->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]     - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]  ),0);

      SetRemainingDays(m_vSpecProgress[0] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]         ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]          ));
      SetRemainingDays(m_vSpecProgress[1] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]      ));
      SetRemainingDays(m_vSpecProgress[2] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]        ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]         ));
      SetRemainingDays(m_vSpecProgress[3] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]      ));
      SetRemainingDays(m_vSpecProgress[4] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision] ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]  ));
      SetRemainingDays(m_vSpecProgress[5] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]    ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]     ));
      SetRemainingDays(m_vSpecProgress[6] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]   ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]    ));
      SetRemainingDays(m_vSpecProgress[7] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]             ));
      SetRemainingDays(m_vSpecProgress[8] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]          ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]           ));
      SetRemainingDays(m_vSpecProgress[9] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]             ));
      SetRemainingDays(m_vSpecProgress[10]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]          ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]           ));
      SetRemainingDays(m_vSpecProgress[11]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]   ,m_ModifiedInfo.m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]    ));

      break;

   // -------------- NAVAL ---------------
   case SP2::EUnitCategory::Naval:

       // ------------- TECHNOLOGY ------------
      m_vSpecTechnology[0] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange],        0);
      m_vSpecTechnology[1] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision],    0);
      m_vSpecTechnology[2] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage],       0);
      m_vSpecTechnology[3] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange],    0);
      m_vSpecTechnology[4] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision],0);
      m_vSpecTechnology[5] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage],   0);
      m_vSpecTechnology[6] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload],  0);
      m_vSpecTechnology[7] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed],           0);
      m_vSpecTechnology[8] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors],         0);
      m_vSpecTechnology[9] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor],           0);
      m_vSpecTechnology[10]->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth],         0);
      m_vSpecTechnology[11]->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures],  0);

      // ------------- FINANCING ------------
      if(m_ModifiedInfo.m_fBudgetNaval)
      {         
         m_vSpecFinancing[0] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]        / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[1] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]    / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[2] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]       / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[3] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]    / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[4] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]/ m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[5] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]   / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[6] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]  / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[7] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]           / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[8] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]         / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[9] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]           / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[10]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]        / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
         m_vSpecFinancing[11]->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures] / m_ModifiedInfo.m_fBudgetNaval * 100,0,false,false);
      }

      // ------------ PROGRESSION ------------
      m_vSpecProgress[0] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]           - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]        ),0);
      m_vSpecProgress[1] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]       - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]    ),0);
      m_vSpecProgress[2] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]          - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]       ),0);
      m_vSpecProgress[3] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]       - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]    ),0);
      m_vSpecProgress[4] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]   - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]),0);
      m_vSpecProgress[5] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]      - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]   ),0);
      m_vSpecProgress[6] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]     - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]  ),0);
      m_vSpecProgress[7] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]              - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]           ),0);
      m_vSpecProgress[8] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]            - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]         ),0);
      m_vSpecProgress[9] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]              - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]           ),0);
      m_vSpecProgress[10]->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]            - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]         ),0);
      m_vSpecProgress[11]->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]     - (INT32)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]  ),0);

      SetRemainingDays(m_vSpecProgress[0] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]         ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]          ));
      SetRemainingDays(m_vSpecProgress[1] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]      ));
      SetRemainingDays(m_vSpecProgress[2] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]        ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]         ));
      SetRemainingDays(m_vSpecProgress[3] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]     ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]      ));
      SetRemainingDays(m_vSpecProgress[4] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision] ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]  ));
      SetRemainingDays(m_vSpecProgress[5] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]    ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]     ));
      SetRemainingDays(m_vSpecProgress[6] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]   ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]    ));
      SetRemainingDays(m_vSpecProgress[7] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]             ));
      SetRemainingDays(m_vSpecProgress[8] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]          ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]           ));
      SetRemainingDays(m_vSpecProgress[9] ->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]            ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]             ));
      SetRemainingDays(m_vSpecProgress[10]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]          ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]           ));
      SetRemainingDays(m_vSpecProgress[11]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]   ,m_ModifiedInfo.m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]    ));

      break;

   // -------------- NUCLEAR ---------------
   case SP2::EUnitCategory::Nuclear:

      if(g_ClientDAL.m_PlayerCountryData.NuclearReady() >= 1.f)
      {
         // ------------- TECHNOLOGY ------------
         m_vSpecTechnology[0] ->Enabled(false);
         m_vSpecTechnology[1] ->Enabled(false);
         m_vSpecTechnology[2] ->Enabled(false);
         m_vSpecTechnology[3] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]     ,0);
         m_vSpecTechnology[4] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision] ,0);
         m_vSpecTechnology[5] ->Value((INT8)m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]    ,0);
         m_vSpecTechnology[6] ->Enabled(false);
         m_vSpecTechnology[7] ->Enabled(false);
         m_vSpecTechnology[8] ->Enabled(false);
         m_vSpecTechnology[9] ->Enabled(false);
         m_vSpecTechnology[10]->Enabled(false);
         m_vSpecTechnology[11]->Enabled(false);

         m_vTechnologyLbl[0] ->Enabled(false);
         m_vFinancingLbl[0]  ->Enabled(false);
         m_vProgressLbl[0]   ->Enabled(false);

         m_vTechnologyLbl[1] ->Enabled(false);
         m_vFinancingLbl[1]  ->Enabled(false);
         m_vProgressLbl[1]   ->Enabled(false);

         m_vTechnologyLbl[2] ->Enabled(false);
         m_vFinancingLbl[2]  ->Enabled(false);
         m_vProgressLbl[2]   ->Enabled(false);

         m_vTechnologyLbl[6] ->Enabled(false);
         m_vFinancingLbl[6]  ->Enabled(false);
         m_vProgressLbl[6]   ->Enabled(false);

         m_vTechnologyLbl[7] ->Enabled(false);
         m_vFinancingLbl[7]  ->Enabled(false);
         m_vProgressLbl[7]   ->Enabled(false);

         m_vTechnologyLbl[8] ->Enabled(false);
         m_vFinancingLbl[8]  ->Enabled(false);
         m_vProgressLbl[8]   ->Enabled(false);

         m_vTechnologyLbl[9] ->Enabled(false);
         m_vFinancingLbl[9]  ->Enabled(false);
         m_vProgressLbl[9]   ->Enabled(false);

         m_vTechnologyLbl[10] ->Enabled(false);
         m_vFinancingLbl[10]  ->Enabled(false);
         m_vProgressLbl[10]   ->Enabled(false);

         m_vTechnologyLbl[11] ->Enabled(false);
         m_vFinancingLbl[11]  ->Enabled(false);
         m_vProgressLbl[11]   ->Enabled(false);

         // ------------- FINANCING ------------
         if(m_ModifiedInfo.m_fBudgetNuclear)
         {         
            m_vSpecFinancing[0] ->Enabled(false);
            m_vSpecFinancing[0] ->Locked()[0] = true;
            m_vSpecFinancing[1] ->Enabled(false);
            m_vSpecFinancing[1] ->Locked()[0] = true;
            m_vSpecFinancing[2] ->Enabled(false);
            m_vSpecFinancing[2] ->Locked()[0] = true;
            m_vSpecFinancing[3] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      / m_ModifiedInfo.m_fBudgetNuclear * 100,0,false,false);
            m_vSpecFinancing[4] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  / m_ModifiedInfo.m_fBudgetNuclear * 100,0,false,false);
            m_vSpecFinancing[5] ->Value(m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     / m_ModifiedInfo.m_fBudgetNuclear * 100,0,false,false);
            m_vSpecFinancing[6] ->Enabled(false);
            m_vSpecFinancing[6] ->Locked()[0] = true;
            m_vSpecFinancing[7] ->Enabled(false);
            m_vSpecFinancing[7] ->Locked()[0] = true;
            m_vSpecFinancing[8] ->Enabled(false);
            m_vSpecFinancing[8] ->Locked()[0] = true;
            m_vSpecFinancing[9] ->Enabled(false);
            m_vSpecFinancing[9] ->Locked()[0] = true;
            m_vSpecFinancing[10]->Enabled(false);
            m_vSpecFinancing[10]->Locked()[0] = true;
            m_vSpecFinancing[11]->Enabled(false);
            m_vSpecFinancing[11]->Locked()[0] = true;
         }

         // ------------ PROGRESSION ------------
         m_vSpecProgress[0] ->Enabled(false);
         m_vSpecProgress[1] ->Enabled(false);
         m_vSpecProgress[2] ->Enabled(false);
         m_vSpecProgress[3] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]         - (INT32) m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]    ),0);
         m_vSpecProgress[4] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]     - (INT32) m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]),0);
         m_vSpecProgress[5] ->Value((m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]        - (INT32) m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]   ),0);
         m_vSpecProgress[6] ->Enabled(false);                                                                                                        
         m_vSpecProgress[7] ->Enabled(false);                                                                                                        
         m_vSpecProgress[8] ->Enabled(false);                                                                                                        
         m_vSpecProgress[9] ->Enabled(false);                                                                                                        
         m_vSpecProgress[10]->Enabled(false);
         m_vSpecProgress[11]->Enabled(false);

         SetRemainingDays(m_vSpecProgress[3]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]    ,  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]    ));
         SetRemainingDays(m_vSpecProgress[4]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision],  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]));                  
         SetRemainingDays(m_vSpecProgress[5]->TextValueObj(), (INT32)GResearchInformationItf::TimeToCompletion(m_ModifiedInfo.m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]   ,  m_ModifiedInfo.m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]   ));
      }
      else
      {
         for(INT32 i=0; i<12; i++)
         {
            m_vSpecProgress[i]  ->Enabled(false);
            m_vSpecFinancing[i] ->Enabled(false);
            m_vSpecTechnology[i]->Enabled(false);
            m_vTechnologyLbl[i] ->Enabled(false);
            m_vFinancingLbl[i]  ->Enabled(false);
            m_vProgressLbl[i]   ->Enabled(false);
            m_vSpecProgress[i]  ->Value(0,0);
            m_vSpecTechnology[i]->Value(0,0);
            m_vSpecFinancing[i] ->Value(0,0);
         }
      }

      break;
   }

   gassert(!_isnan(m_ModifiedInfo.m_fBudgetAir),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetGround),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetNaval),"isnan");
   gassert(!_isnan(m_ModifiedInfo.m_fBudgetNuclear),"isnan");
}

void GUnitResearchWindow::UpdateMaxValues()
{
   memcpy(m_ModifiedInfo.m_fMaxValues, m_pOriginalInfo->m_fMaxValues,
          sizeof(REAL32)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
}

void GUnitResearchWindow::UpdateBudget()
{
   m_ModifiedInfo.m_fBudgetAir      = m_pOriginalInfo->m_fBudgetAir;
   m_ModifiedInfo.m_fBudgetGround   = m_pOriginalInfo->m_fBudgetGround;
   m_ModifiedInfo.m_fBudgetNaval    = m_pOriginalInfo->m_fBudgetNaval;
   m_ModifiedInfo.m_fBudgetNuclear  = m_pOriginalInfo->m_fBudgetNuclear;
   memcpy(m_ModifiedInfo.m_fBudget, m_pOriginalInfo->m_fBudget, 
          sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
}

void GUnitResearchWindow::SaveBudget()
{
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GUnitResearchUpdate);   

   // Set up event info
   SP2::Event::GUnitResearchUpdate* l_pUpdate = (SP2::Event::GUnitResearchUpdate*)l_pEvent.get();
   l_pUpdate->m_iSource    = g_SP2Client->Id();
   l_pUpdate->m_iTarget    = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iCountryId = g_ClientDAL.ControlledCountryID();
   l_pUpdate->SetData      = true;

   // set up new budget info
   l_pUpdate->m_fBudgetAir     = m_ModifiedInfo.m_fBudgetAir;
   l_pUpdate->m_fBudgetNuclear = m_ModifiedInfo.m_fBudgetNuclear;
   l_pUpdate->m_fBudgetNaval   = m_ModifiedInfo.m_fBudgetNaval;
   l_pUpdate->m_fBudgetGround  = m_ModifiedInfo.m_fBudgetGround;
   memcpy(l_pUpdate->m_fBudget, m_ModifiedInfo.m_fBudget, 
          sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);

   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdate);  

   g_Joshua.RaiseEvent(l_pEvent);
}

VOID CALLBACK GUnitResearchWindow::UpdateTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
   // only update if no request pending
   if(!m_pThis->RequestPending())
   {
      // Retrieve the info from the server
      g_ClientDAL.m_ResearchInfo.Update();
      // if not dirty .. display this info
      if(!m_pThis->Dirty())
      {
         // Refresh screen values with server values
         m_pThis->UpdateMaxValues();
         m_pThis->UpdateBudget();
         m_pThis->Update(m_pThis->m_Filter);
      }
   }
}

void GUnitResearchWindow::Save()
{
   if(Dirty())
   {
      SaveBudget();
      Dirty(false);
      g_ClientDAL.m_ResearchInfo.Update();
   }
}

void GUnitResearchWindow::BudgetGround(REAL64 in_fNewValue)
{
   if(in_fNewValue)
   {   
      if(m_pOriginalInfo->m_fBudgetGround)
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]                 / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]           = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]               / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]          = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]              / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]          / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]         = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]             / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]        / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]          / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]  = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]      / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]     = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]         / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]                 / m_pOriginalInfo->m_fBudgetGround * in_fNewValue;
      }
      else
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]             = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]           = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]          = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]      = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]         = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]    = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]      = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]  = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]     = in_fNewValue / 10;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]             = in_fNewValue / 10;
      }
   }
   else
   {
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Speed]             = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Sensors]           = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunRange]          = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunPrecision]      = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::GunDamage]         = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePayload]    = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileRange]      = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissilePrecision]  = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::MissileDamage]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Ground][EUnitDesignCharacteristics::Armor]             = 0;
   }

   m_pOriginalInfo->m_fBudgetGround = in_fNewValue;
}

void GUnitResearchWindow::BudgetAir(REAL64 in_fNewValue)
{
   if(in_fNewValue)
   {   
      if(m_pOriginalInfo->m_fBudgetAir)
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]                 / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]           = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]               / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]          = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]              / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]          / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]         = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]             / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]        / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]          / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]  = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision]      / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]     = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]         / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]                 / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]        / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]           = m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]               / m_pOriginalInfo->m_fBudgetAir * in_fNewValue;
      }
      else
      {

         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]            = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]          = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]         = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]     = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]        = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]   = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]     = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision] = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]    = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]            = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]   = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]          = in_fNewValue / 12;
      }
   }
   else
   {
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Speed]            = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Sensors]          = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunRange]         = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunPrecision]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::GunDamage]        = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePayload]   = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileRange]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissilePrecision] = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::MissileDamage]    = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Armor]            = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::CounterMesures]   = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Air][EUnitDesignCharacteristics::Stealth]          = 0;
   }

   m_pOriginalInfo->m_fBudgetAir = in_fNewValue;
}

void GUnitResearchWindow::BudgetNaval(REAL64 in_fNewValue)
{
   if(in_fNewValue)
   {   
      if(m_pOriginalInfo->m_fBudgetNaval)
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]                 / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]           = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]               / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]          = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]              / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]          / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]         = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]             / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]        / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]      = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]          / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]  = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision]      / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]     = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]         / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]             = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]                 / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]        / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]           = m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]               / m_pOriginalInfo->m_fBudgetNaval * in_fNewValue;
      }
      else
      {

         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]            = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]          = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]         = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]     = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]        = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]   = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]     = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision] = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]    = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]            = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]   = in_fNewValue / 12;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]          = in_fNewValue / 12;
      }
   }
   else
   {
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Speed]            = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Sensors]          = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunRange]         = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunPrecision]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::GunDamage]        = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePayload]   = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileRange]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissilePrecision] = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::MissileDamage]    = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Armor]            = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::CounterMesures]   = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Naval][EUnitDesignCharacteristics::Stealth]          = 0;
   }

   m_pOriginalInfo->m_fBudgetNaval = in_fNewValue;
}

void GUnitResearchWindow::BudgetNuclear(REAL64 in_fNewValue)
{
   if(in_fNewValue)
   {   
      if(m_pOriginalInfo->m_fBudgetNuclear)
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]     = m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]     / m_pOriginalInfo->m_fBudgetNuclear * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision] = m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision] / m_pOriginalInfo->m_fBudgetNuclear * in_fNewValue;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]    = m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]    / m_pOriginalInfo->m_fBudgetNuclear * in_fNewValue;
      }
      else
      {
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]     = in_fNewValue / 3;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision] = in_fNewValue / 3;
         m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]    = in_fNewValue / 3;
      }
   }
   else
   {
      m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]     = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision] = 0;
      m_pOriginalInfo->m_fBudget[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]    = 0;
   }

   m_pOriginalInfo->m_fBudgetNuclear = in_fNewValue;
}