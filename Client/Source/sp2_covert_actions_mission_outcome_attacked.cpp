/**************************************************************
*
* sp2_covert_actions_mission_outcome_attacked.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_mission_outcome_attacked.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_covert_actions_new_mission.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCovertActionsMissionOutcomeAttackedWindow::TypeName(L"Covert_Actions_Mission_Outcome_Attacked");

GUI::GBaseObject* GCovertActionsMissionOutcomeAttackedWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsMissionOutcomeAttackedWindow();
}


GCovertActionsMissionOutcomeAttackedWindow::GCovertActionsMissionOutcomeAttackedWindow() : GUI::GFrame()
{
}

GCovertActionsMissionOutcomeAttackedWindow::~GCovertActionsMissionOutcomeAttackedWindow()
{
}

bool GCovertActionsMissionOutcomeAttackedWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjExitBtn            = (GUI::GButton*)Child(L"btnExit");
   m_pObjExitBtn->BringToFront();

   m_pObjTitleLbl           = (GUI::GLabel*)Child(L"lblTitle");

   m_pObjMissionTypeLbl     = (GUI::GLabel*)Child(L"lblMission");
   m_pObjAttackerLbl        = (GUI::GLabel*)Child(L"lblCountry");
   m_pObjOutcomeLbl         = (GUI::GLabel*)Child(L"lblOutcome");
   m_pObjEffectLbl          = (GUI::GLabel*)Child(L"lblEffect");

   m_pObjMissionTypeTxt     = (GUI::GLabel*)Child(L"txtMission");
   m_pObjAttackerTxt        = (GUI::GLabel*)Child(L"txtCountry");
   m_pObjOutcomeTxt         = (GUI::GLabel*)Child(L"txtOutcome");
   m_pObjEffectTxt          = (GUI::GLabel*)Child(L"txtEffect");
   m_pObjFlagPic            = (GUI::GPictureBox*)Child(L"picFlag");

   m_pObjFlagPic->BringToFront();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsMissionOutcomeAttackedWindow::OnShow()
{
   __super::OnShow();

   BringToFront();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GCovertActionsMissionOutcomeAttackedWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsMissionOutcomeAttackedWindow::Update(SDataMissionOver in_Data)
{
   bool l_bFramed     = in_Data.m_iFramedCountry == (UINT32)g_ClientDAL.ControlledCountryID();
   bool l_bSuccessful = in_Data.m_bSuccessful;
	bool l_bCaptured	 = in_Data.m_bCaptured;

   if(l_bFramed)
   {
      m_pObjTitleLbl->Text( g_ClientDAL.GetString(EStrId::CountryFramed) );
      m_pObjAttackerLbl->Text( g_ClientDAL.GetString(EStrId::AttackedCountry) );

      m_pObjOutcomeLbl->Hide();
      m_pObjOutcomeTxt->Hide();
      
      m_pObjEffectLbl->Top( m_pObjOutcomeLbl->Top() );
      m_pObjEffectTxt->Top( m_pObjOutcomeTxt->Top() );

      Height( 88 );
   }
   else
   {
      m_pObjTitleLbl->Text( g_ClientDAL.GetString(EStrId::CountryTargetOfCovertAction) );
      m_pObjAttackerLbl->Text( g_ClientDAL.GetString(EStrId::AttackingCountry) );

      m_pObjOutcomeLbl->Show();
      m_pObjOutcomeTxt->Show();
      
      m_pObjEffectLbl->Top( 73 );
      m_pObjEffectTxt->Top( 73 );

      Height( 105 );
   }

   m_pObjMissionTypeTxt->Text( g_ClientDAL.ConvertMissionTypeToString(in_Data.m_eType) );

   if(l_bSuccessful)
   {
      m_pObjOutcomeTxt->Text( g_ClientDAL.GetString(EStrId::CovertActionSuccessful) );

      if(!l_bFramed)
      {
         GString l_sText;
         switch(in_Data.m_eType)
         {
         case ECovertActionsMissionType::Assassination:
            l_sText =  g_ClientDAL.GetString(102256);
            break;
         case ECovertActionsMissionType::CoupEtat:
            l_sText =  g_ClientDAL.GetString(102257);
            break;
         case ECovertActionsMissionType::Sabotage:
            switch(in_Data.m_eTargetSector)
            {
            case ECovertActionsTargetSector::Civilian:    
					if(((EResources::Enum)in_Data.m_iResourceOrUnitType) != EResources::ItemCount &&
						((EResources::Enum)in_Data.m_iResourceOrUnitType) != -1)
					{
						l_sText =  g_ClientDAL.GetString(102258); 
						l_sText = l_sText.ReplaceNextPattern(g_ClientDAL.ConvertResourcesToString((EResources::Enum)in_Data.m_iResourceOrUnitType), L"[nom de resource]");
					}
               break;
            case ECovertActionsTargetSector::Military:               
					if( ((EUnitCategory::Enum)in_Data.m_iResourceOrUnitType) != EUnitCategory::ItemCount && 
						 ((EUnitCategory::Enum)in_Data.m_iResourceOrUnitType) != -1)
					{
						l_sText =  g_ClientDAL.GetString(102259); 
						l_sText = l_sText.ReplaceNextPattern(g_ClientDAL.ConvertUnitCategoryToString((EUnitCategory::Enum)in_Data.m_iResourceOrUnitType), L"[nom de resource]");
					}
               break;
            }
            break;
         case ECovertActionsMissionType::Terrorism:
            l_sText =  g_ClientDAL.GetString(102261);
            break;
         }

         m_pObjEffectTxt->Text(l_sText);
      }
   }
   else
   {
		if(l_bCaptured)
			m_pObjOutcomeTxt->Text( g_ClientDAL.GetString(EStrId::CovertActionFailedCaptured) );
		else
			m_pObjOutcomeTxt->Text( g_ClientDAL.GetString(EStrId::CovertActionFailedNotCaptured) );
      m_pObjEffectTxt->Text( g_ClientDAL.GetString(102262) );
   }

   if(l_bFramed && l_bSuccessful)
   {
      m_pObjAttackerTxt->Text( g_ClientDAL.Country( in_Data.m_iTargetCountry ).Name() );
      m_pObjFlagPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[g_ClientDAL.Country(in_Data.m_iTargetCountry).Id()]);
   }
   else if(l_bFramed && !l_bSuccessful)
   {
      m_pObjAttackerTxt->Text( g_ClientDAL.Country( in_Data.m_iTargetCountry ).Name() );
      m_pObjFlagPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[g_ClientDAL.Country(in_Data.m_iTargetCountry).Id()]);

   }
   else if(!l_bFramed && l_bSuccessful)
   {
      m_pObjAttackerTxt->Text( g_ClientDAL.GetString(EStrId::UnknownInSymbol) );
      m_pObjFlagPic->PictureNormal( GString(L"000159") );

   }
   else // !l_bFramed && !l_bSuccessful
   {
      m_pObjAttackerTxt->Text( g_ClientDAL.Country( in_Data.m_iCellSourceCountry ).Name() );
      m_pObjFlagPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[g_ClientDAL.Country(in_Data.m_iCellSourceCountry).Id()]);
      
   }
}