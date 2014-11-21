/**************************************************************
*
* sp2_load_game_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_load_game_window.h"
#include "../../includes/common_lib//sp2_game_scenario.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GLoadGameWindow::TypeName(L"Load_Saved_Game");

GUI::GBaseObject* GLoadGameWindow::New()
{
	return (GUI::GBaseObject*) new GLoadGameWindow();
}


GLoadGameWindow::GLoadGameWindow() : GBaseWindow()
{
   m_iLastSelectedId  = -1;
   m_bShowSingleGames = false;
}

GLoadGameWindow::~GLoadGameWindow()
{
}

bool GLoadGameWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   m_pObjGamesLst    = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmList")->Child(L"lstGames");
   m_pObjLoadGameBtn = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnLoadGame");

   m_pObjApplyBtn->Hide();
   
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

	return true;
}

void GLoadGameWindow::OnHide()
{
   __super::OnHide();
   m_pObjGamesLst->Clear();
}

void GLoadGameWindow::Show(bool in_bShowSingleGames)
{
   m_bShowSingleGames = in_bShowSingleGames;
   __super::Show();
}

void GLoadGameWindow::OnShow()
{
   __super::OnShow();

   Update();
   
   m_pObjLoadGameBtn->Enabled(false);

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GLoadGameWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if( in_pCaller == m_pObjExitBtn )
   {
      Hide();
   }
   else if( in_pCaller == m_pObjLoadGameBtn )
   {
      GString l_sGame(L"");

      const vector<INT32> & l_Games = m_pObjGamesLst->Get_Selected_Item();
      if( l_Games.size() && l_Games[0] >= 0 )
      {
         l_sGame = ((GUI::GLabel*)m_pObjGamesLst->Get_Row_At(l_Games[0])->Object()->Child(L"txtName"))->Text();
      }

      if( l_sGame != L"" )
      {
         g_ClientDCL.LoadGame(l_sGame);
         Hide();
      }
      else
      {
         m_pObjLoadGameBtn->Enabled(false);
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GLoadGameWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnListItemSelected)
   {
      m_pObjLoadGameBtn->Enabled(true);
      
      if(m_iLastSelectedId >= 0)
         UnselectRow( m_pObjGamesLst->Get_Row_At( m_iLastSelectedId ) );
      
      GUI::GListBox::GRow* l_pRow = m_pObjGamesLst->Get_Row_At( m_pObjGamesLst->Get_Selected_Item()[0] );
      SelectRow(l_pRow);
      m_iLastSelectedId = m_pObjGamesLst->Get_Selected_Item()[0];
   }

   return GUI::EEventStatus::Handled;
}

bool GLoadGameWindow::Update()
{
   m_pObjGamesLst->Clear();

   g_ClientDCL.BuildSavedGameList();
   const vector< pair< GString, GSaveGameData > > & l_Saves = g_ClientDCL.SavedGameList();

   for(UINT32 i=0; i<l_Saves.size(); i++)
   {
      GUI::GListBox::GRow* l_pRow;

      UINT32 l_iSingle = 0;
      if(m_bShowSingleGames)
         l_iSingle = 1;

      if( l_Saves[i].second.m_iGameIsMp != l_iSingle )
      {
         l_pRow = m_pObjGamesLst->Insert_New_Row();
         FillSaveRow( l_pRow, l_Saves[i] );
      }
   }

   m_pObjGamesLst->Update();

   return true;
}

void GLoadGameWindow::FillSaveRow( GUI::GListBox::GRow* in_pRow, const pair< GString, GSaveGameData > & in_SaveData )
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

void GLoadGameWindow::SelectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtDate"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenario"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtGametime"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtCountry"))->PictureNormal(L"000647");
}

void GLoadGameWindow::UnselectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtDate"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenario"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtGametime"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtCountry"))->PictureNormal(L"000587");
}