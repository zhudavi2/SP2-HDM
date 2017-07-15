/**************************************************************
*
* sp2_save_game_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_save_game_window.h"
#include "../../includes/common_lib//sp2_game_scenario.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GSaveGameWindow::TypeName(L"Save_Game_Window");

GUI::GBaseObject* GSaveGameWindow::New()
{
	return (GUI::GBaseObject*) new GSaveGameWindow();
}


GSaveGameWindow::GSaveGameWindow() : GBaseWindow()
{
   m_iLastSelectedId = -1;
}

GSaveGameWindow::~GSaveGameWindow()
{
}

bool GSaveGameWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   m_pObjGamesFrm = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmSave");
   m_pObjSaveGameBtn = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnSaveGame");

   m_pObjSaveLst  = (GUI::GListBox*)m_pObjGamesFrm->Child(L"lstSaves");
   m_pObjNameEdt  = (GUI::GEditBox*)m_pObjGamesFrm->Child(L"edtSaveName");

   m_pObjApplyBtn->Hide();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GSaveGameWindow::OnShow()
{
   __super::OnShow();

   m_pObjSaveGameBtn->Enabled(false);

   Update();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

void GSaveGameWindow::OnHide()
{
   __super::OnHide();
   m_pObjSaveLst->Clear();
}

EEventStatus::Enum GSaveGameWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller == m_pObjExitBtn)
   {
      Hide();
   }
   else if(in_pCaller == m_pObjSaveGameBtn)
   {
      g_ClientDCL.SaveGame( m_pObjNameEdt->Text() );
      Hide();
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GSaveGameWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == GUI::ECustomEvents::OnListItemSelected)
   {
      if(in_pCaller == m_pObjSaveLst)
      {
         vector<INT32> l_iSel = m_pObjSaveLst->Get_Selected_Item();
         if(l_iSel.size() && l_iSel[0] >= 0)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjSaveLst->Get_Row_At(l_iSel[0]);
            m_pObjNameEdt->Text( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );

            m_pObjSaveGameBtn->Enabled(true);

            if(m_iLastSelectedId >= 0)
               UnselectRow( m_pObjSaveLst->Get_Row_At( m_iLastSelectedId ) );

            SelectRow(l_pRow);
            m_iLastSelectedId = l_iSel[0];
         }
      }
   }
   else if(in_iEventID == c_iOnEditBoxEditEnd)
   {
      if(m_pObjNameEdt->Text().size())
      {
         m_pObjSaveGameBtn->Enabled(true);
      }
		else
			m_pObjSaveGameBtn->Enabled(false);

   }

   return GUI::EEventStatus::Handled;
}

bool GSaveGameWindow::Update()
{
   m_pObjSaveLst->Clear();

   SP2::EGameTypes::Enum l_GameType = g_SP2Client->CurrentGameType();

   bool l_bShowSinglePlayerGames = false;
   if( l_GameType == EGameTypes::SinglePlayer )
   {
      l_bShowSinglePlayerGames = true;
   }
   else if( l_GameType == EGameTypes::Scenario )
   {
      l_bShowSinglePlayerGames = true;
   }
   else if( l_GameType == EGameTypes::HostMultiPlayer )
   {
      l_bShowSinglePlayerGames = false;
   }
   else
   {
      //Can't save anyway !
      return true;
   }

   g_ClientDCL.BuildSavedGameList();
   const vector< pair< GString, GSaveGameData > > & l_Saves = g_ClientDCL.SavedGameList();

   for(UINT32 i=0; i<l_Saves.size(); i++)
   {
      if( (l_Saves[i].second.m_iGameIsMp != 0)== !l_bShowSinglePlayerGames )
      {
         GUI::GListBox::GRow* l_pRow = m_pObjSaveLst->Insert_New_Row();
         FillSaveRow( l_pRow, l_Saves[i] );
      }
   }

   m_pObjSaveLst->Update();

   return true;
}

void GSaveGameWindow::FillSaveRow( GUI::GListBox::GRow* in_pRow, const pair< GString, GSaveGameData > & in_SaveData )
{
   if(!in_pRow)
      return;

   GUI::GLabel* l_pName       = (GUI::GLabel*)in_pRow->Object()->Child(L"txtName");
   GUI::GLabel* l_pDate       = (GUI::GLabel*)in_pRow->Object()->Child(L"txtDate");
   GUI::GLabel* l_pScenario   = (GUI::GLabel*)in_pRow->Object()->Child(L"txtScenario");
   GUI::GLabel* l_pGametime   = (GUI::GLabel*)in_pRow->Object()->Child(L"txtGametime");
   GUI::GLabel* l_pCountry    = (GUI::GLabel*)in_pRow->Object()->Child(L"txtCountry");
   GUI::GPictureBox* l_pFlag  = (GUI::GPictureBox*)in_pRow->Object()->Child(L"picFlag");
   l_pFlag->BringToFront();

   l_pName->Text( in_SaveData.first );
   l_pDate->Text( in_SaveData.second.m_SaveDate.ToString(GDateTime::etfYYYYMMDDhhmmss) );
   if( in_SaveData.second.m_pScenario )
   {
      const GString & l_sScen = in_SaveData.second.m_pScenario->Name();
      l_pScenario->Text( l_sScen );
   }
   l_pCountry->Text( in_SaveData.second.m_sCountryName );
   l_pGametime->Text( in_SaveData.second.m_GameDate.ToString(GDateTime::etfYYYYMMDDhhmmss) );
   l_pFlag->PictureNormal( in_SaveData.second.m_sCountryFlagPath );

}

void GSaveGameWindow::SelectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtDate"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenario"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtGametime"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtCountry"))->PictureNormal(L"000647");
}

void GSaveGameWindow::UnselectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtDate"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenario"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtGametime"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtCountry"))->PictureNormal(L"000587");
}