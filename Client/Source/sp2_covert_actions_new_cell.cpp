/**************************************************************
*
* sp2_covert_actions_new_cell.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_new_cell.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_generic_message_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

#include <sstream>

const GString GCovertActionsNewCellWindow::TypeName(L"Covert_Actions_Cell_Creation");

GUI::GBaseObject* GCovertActionsNewCellWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsNewCellWindow();
}


GCovertActionsNewCellWindow::GCovertActionsNewCellWindow() : GBaseWindow()
{
}

GCovertActionsNewCellWindow::~GCovertActionsNewCellWindow()
{
}

bool GCovertActionsNewCellWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   m_pObjInnerFrm       = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjNameEdt        = (GUI::GEditBox*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"edtName");
   m_pObjTraining       = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"cboTraining");
   m_pObjSetupCostLbl   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtCost");
   m_pObjUpkeepLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtUpkeep");
   m_pObjUpdateTimeLbl  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtTime");
   m_pObjCountryCbo     = (SP2::GComboListBoxLess*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"cboCountry");

   m_pObjConfirmBtn     = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancelBtn      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

   m_pObjCountryCbo->SetListBox(g_ClientDDL.CountriesListBox());
     
   m_pObjTraining->Add_Item(g_ClientDAL.GetString(EStrId::TrainingRecruit));
   m_pObjTraining->Add_Item(g_ClientDAL.GetString(EStrId::TrainingRegular));
   m_pObjTraining->Add_Item(g_ClientDAL.GetString(EStrId::TrainingVeteran));
   m_pObjTraining->Add_Item(g_ClientDAL.GetString(EStrId::TrainingElite));
   
	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsNewCellWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());

   m_pObjNameEdt->Text(L"");
   m_pObjCountryCbo->Selected_Content( g_ClientDAL.Country(g_ClientDAL.ControlledCountryID()).Name() );

   // Have Elite selected by default, instead of Recruit.
   m_pObjTraining->Selected_Content(g_ClientDAL.GetString(EStrId::TrainingElite));

   m_pObjConfirmBtn->Enabled(false);

   CalculateCost();
}

void GCovertActionsNewCellWindow::OnHide()
{
   RegisteredModalWindow.clear();
   
   __super::OnHide();
}

GUI::EEventStatus::Enum GCovertActionsNewCellWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjConfirmBtn)
         {
            if(m_pObjNameEdt->Text().Trim() == L"")
            {
               if(!g_ClientDDL.GenericMessageWindow())
                  g_ClientDDL.GenericMessageWindowSpawn();

               g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::MustEnterName), this );
            }
            else
            {
               INT32 l_iStartingCellName = -1;
               if(m_pObjNameEdt->Text().find('-') == 0)
               {
                   // If the name contains a dash at the front, then create 10 cells at a time.
				   wstringstream l_Conversion(m_pObjNameEdt->Text().substr(1));
				   l_Conversion >> l_iStartingCellName;
               }

               for(INT32 i=l_iStartingCellName; i<l_iStartingCellName+10; i++)
               {
                SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellCreation);
                SP2::Event::GEventCellCreation* l_pEvent = (SP2::Event::GEventCellCreation*)l_Event.get();

                l_pEvent->m_iAssignedCountryID = g_ClientDAL.Country( m_pObjCountryCbo->Selected_Content() ).Id();

                if(l_iStartingCellName == -1)
                {
                    l_pEvent->m_sName = m_pObjNameEdt->Text();
                }
                else
                {
                    l_pEvent->m_sName = GSString(i);
                }

                l_pEvent->m_eTraining = g_ClientDAL.ConvertCellTrainingToEnum( m_pObjTraining->Selected_Content() );

                l_pEvent->m_iSource = g_SP2Client->Id();
                l_pEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;


                g_ClientDCL.ClientAdvisor().RaiseEvent(l_Event);

                if(l_iStartingCellName == -1)
                {
                    break;
                }
               }

               Hide();
            }
         }
         else if(in_pCaller == m_pObjCancelBtn)
         {
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GCovertActionsNewCellWindow::OnCustomEvent(UINT32 in_iType, const SDK::GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_iType == ECustomEvents::OnComboBoxSelectionEvent)
   {
      if(((GUI::GComboBox*)in_EventData.Data) == m_pObjTraining)
      {
         CalculateCost();
      }
   }
   else if(in_iType == c_iOnEditBoxEditEnd)
   {
      if( ((GUI::GEditBox*)in_EventData.Data) == m_pObjNameEdt )
      {
         if(m_pObjNameEdt->Text().Trim() == L"")
            m_pObjConfirmBtn->Enabled(false);
         else
            m_pObjConfirmBtn->Enabled(true);
      }
   }
   else if(in_iType == ECustomEvents::OnComboBoxBeforeOpen)
   {
      if(in_EventData.Data == m_pObjCountryCbo)
         g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), (GUI::GComboBox*)in_EventData.Data, true);

      return GUI::EEventStatus::Handled;
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsNewCellWindow::CalculateCost()
{
   ECovertActionCellTraining::Enum l_eLevel = g_ClientDAL.ConvertCellTrainingToEnum( m_pObjTraining->Selected_Content() );
   
   REAL64 l_fCost  = GDALInterface::CovertActionCellSetupCost(l_eLevel);
   UINT32 l_iMonth = 0;
   REAL64 l_fUpkp  = 0.f;

   switch(l_eLevel)
   {
   case ECovertActionCellTraining::Regular:
      l_iMonth = SP2::c_iCellCreationTimeMonthRegular;
      l_fUpkp  = c_fCellUpkeepBase * c_fCellUpkeepModifierRegular;
      break;
   case ECovertActionCellTraining::Recruit:
      l_iMonth = SP2::c_iCellCreationTimeMonthRecruit;
      l_fUpkp  = c_fCellUpkeepBase;
      break;
   case ECovertActionCellTraining::Veteran:
      l_iMonth = SP2::c_iCellCreationTimeMonthVeteran;
      l_fUpkp  = c_fCellUpkeepBase * c_fCellUpkeepModifierVeteran;
      break;
   case ECovertActionCellTraining::Elite:
      l_iMonth = SP2::c_iCellCreationTimeMonthElite;
      l_fUpkp  = c_fCellUpkeepBase * c_fCellUpkeepModifierElite;
      break;
   }

   m_pObjSetupCostLbl->Text( GString::FormatNumber(l_fCost, L" ", L"", L"$ ", L"", 3, 0, false) );
   m_pObjUpkeepLbl->Text( GString::FormatNumber(l_fUpkp, L" ", L"", L"$ ", L"", 3, 0, false) ); 
   m_pObjUpdateTimeLbl->Text( GString(l_iMonth) + GString(L" ") + g_ClientDAL.GetString(EStrId::Month) );
}