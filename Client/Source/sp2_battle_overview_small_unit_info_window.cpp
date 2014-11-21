/**************************************************************
*
* sp2_battle_overview_small_unit_info.cpp
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

#include "../include/sp2_battle_overview_small_unit_info_window.h"

#include "../include/sp2_battle_overview_window.h"

const GString GBattleOverviewSmallUnitInfoWindow::TypeName(L"Battle_Overview_Small_Unit_Info");

GUI::GBaseObject* GBattleOverviewSmallUnitInfoWindow::New()
{
   return new GBattleOverviewSmallUnitInfoWindow();
}

GBattleOverviewSmallUnitInfoWindow::GBattleOverviewSmallUnitInfoWindow()
{
}

GBattleOverviewSmallUnitInfoWindow::~GBattleOverviewSmallUnitInfoWindow()
{
}

bool GBattleOverviewSmallUnitInfoWindow::OnCreate()
{
   __super::OnCreate();

   m_pUnitLst  = (GUI::GListBox*)Child(L"lstUnits");

   return true;
}

bool GBattleOverviewSmallUnitInfoWindow::OnDestroy()
{
   __super::OnDestroy();

   return true;
}

GUI::EEventStatus::Enum GBattleOverviewSmallUnitInfoWindow::OnMouseEnter(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller && in_pCaller->Type() == L"lstBattleOverviewSmallUnitInfoItem")
   {
      DebugBreak();
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBattleOverviewSmallUnitInfoWindow::OnMouseLeave(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller && in_pCaller->Type() == L"lstBattleOverviewSmallUnitInfoItem")
   {
      DebugBreak();
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBattleOverviewSmallUnitInfoWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBattleOverviewSmallUnitInfoWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if (in_EventId != ECustomEvents::OnListItemSelected)
      return GUI::EEventStatus::Handled;

   if(in_pCaller)
   {
      const vector<INT32>& l_viItem = m_pUnitLst->Get_Selected_Item();
      if(l_viItem.size() > 0)
      {
         //INT32 l_iIndex = l_viItem[0];
         //((GBattleOverviewWindow*)Owner())->OnSmallUnitInfoSelection(m_viCountryList[l_iIndex]);
      }
   }
   
   return GUI::EEventStatus::Handled;
}

void GBattleOverviewSmallUnitInfoWindow::AddItem(UINT64 in_iQty, INT32 in_iCountryId)
{
   m_iUnitQty = 0;

   if(m_pUnitLst->Get_Nb_Rows() == 0)
   {
      m_viCountryList.clear();
      m_viCountryList.push_back(0);
      GUI::GListBox::GRow* l_pTotalRow = m_pUnitLst->Insert_New_Row();
      l_pTotalRow->Object()->Child(L"picFlag")->Hide();
      ((GUI::GFrame*)l_pTotalRow->Object())->ColorNormal(GColorRGBInt(0,0,0,0));
      m_pObjTotalTxt = (GUI::GLabel*)l_pTotalRow->Object()->Child(L"lblQty");
      m_pObjTotalTxt->Left(2);
      m_pObjTotalTxt->Width(l_pTotalRow->Object()->Width() - 4);
   }

   GUI::GListBox::GRow* l_pRow = m_pUnitLst->Insert_New_Row();
   ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(0,0,0,0));
   ((GUI::GLabel*)l_pRow->Object()->Child(L"lblQty"))->Text(GString::FormatNumber((REAL64)in_iQty,L" ",L"",L"",L"",3,0));
   ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDAL.CountryFlagSmallPath(in_iCountryId));
   m_viCountryList.push_back(in_iCountryId);
   m_iUnitQty += in_iQty;
   m_pObjTotalTxt->Text(g_ClientDAL.GetString(100305) + L"" + GString((INT64)m_iUnitQty));
}

void GBattleOverviewSmallUnitInfoWindow::Clear()
{
   m_pUnitLst->Clear();
   m_viCountryList.clear();
}

bool GBattleOverviewSmallUnitInfoWindow::Update()
{
   if(m_pUnitLst->Get_Nb_Rows() < 3)
   {
      m_pUnitLst->Height(m_pUnitLst->Property(LIST_ITEM_HEIGHT).ToINT32() * m_pUnitLst->Get_Nb_Rows());
      Height(m_pUnitLst->Height() + 4);
   }
   else
   {
      m_pUnitLst->Height(m_pUnitLst->Property(LIST_ITEM_HEIGHT).ToINT32() * 3);
      Height(m_pUnitLst->Height() + 4);
   }

   m_pUnitLst->Update();

   return true;
}