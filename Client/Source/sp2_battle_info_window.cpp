/**************************************************************
*
* sp2_battle_info_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_battle_info_window.h"

const GString GBattleInfoWindow::TypeName(L"Battle_Info_Window");

GUI::GBaseObject* GBattleInfoWindow::New()
{
   return new GBattleInfoWindow();
}

GBattleInfoWindow::GBattleInfoWindow() : GBaseWindow()
{
   m_pObjCountryListA = NULL;
   m_pObjCountryListB = NULL;

   for(UINT32 i = 0;i < c_iNbSides;i ++)
   {
      for(UINT32 j = 0;j < EUnitCategory::ItemCount;j ++)
      {
         m_pObjCountTxt[i][j] = NULL;
      }
   }

   m_iArenaID = 0;
}

GBattleInfoWindow::~GBattleInfoWindow()
{
}

bool GBattleInfoWindow::OnCreate()
{
   __super::OnCreate();

   GUI::GFrame* l_pInfoFrm = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmBattleInfo");

   m_pObjCountryListA = (GUI::GListBox*)l_pInfoFrm->Child(L"lstCountriesA");
   m_pObjCountryListB = (GUI::GListBox*)l_pInfoFrm->Child(L"lstCountriesB");

   m_pObjCountTxt[0][0] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtIPercA");
   m_pObjCountTxt[0][1] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtGPercA");
   m_pObjCountTxt[0][2] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtAPercA");
   m_pObjCountTxt[0][3] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtNPercA");
   m_pObjCountTxt[1][0] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtIPercB");
   m_pObjCountTxt[1][1] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtGPercB");
   m_pObjCountTxt[1][2] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtAPercB");
   m_pObjCountTxt[1][3] = (GUI::GLabel*)l_pInfoFrm->Child(L"txtNPercB");

   m_iArenaID = 0;

   return true;
}

bool GBattleInfoWindow::OnDestroy()
{
   m_iArenaID = 0;

   return  __super::OnDestroy();;
}

void GBattleInfoWindow::OnShow()
{
   __super::OnShow();

   m_iArenaID = 0;
}

void GBattleInfoWindow::OnHide()
{
   __super::OnHide();

   m_iArenaID = 0;
}

bool GBattleInfoWindow::SetArena(SP2::GArenaInfo* in_pArena)
{
   m_iArenaID = in_pArena->m_iId;

   return true;
}

UINT32 GBattleInfoWindow::ArenaId()
{
   return m_iArenaID;
}

void GBattleInfoWindow::UpdateContent()
{
   m_pObjCountryListA->Clear();
   m_pObjCountryListB->Clear();

   if(m_iArenaID)
   {
      SP2::GArenaInfo* l_pArena = (SP2::GArenaInfo*) g_Joshua.UnitManager().ArenaInfo(m_iArenaID);

		if(!l_pArena)
			return;

      UINT32 l_iCounts[c_iNbSides][EUnitCategory::ItemCount];
      Memory::Clear( (UINT32*) l_iCounts, c_iNbSides * EUnitCategory::ItemCount);

      const hash_map<UINT32,GActorInfo>& l_Actors = l_pArena->Actors();
      for(hash_map<UINT32,GActorInfo>::const_iterator l_It =  l_Actors.begin();
                                                      l_It != l_Actors.end() ;
                                                      l_It++)
      {
         GUI::GListBox::GRow* l_pRow = NULL;
         const GActorInfo& l_Actor = l_It->second;
         switch(l_Actor.m_Side)
         {
         case ECombatSide::Attacker:
            l_pRow = m_pObjCountryListB->Insert_New_Row();
            l_Actor.CountUnitTypes(l_iCounts[1] );
            break;
         case ECombatSide::Defender:
            l_pRow = m_pObjCountryListA->Insert_New_Row();
            l_Actor.CountUnitTypes(l_iCounts[0] );
            break;
         default:
            gassert(0, "Actor has an invalid combat side");
            break;
         }

         if(l_pRow)
         {
            GUI::GBaseObject* l_pObj = l_pRow->Object();

		      const GCountry& l_Country = g_ClientDAL.Country(l_Actor.m_iCountryID);

		      GString l_sCode(l_Country.Flag());

            l_pObj->TooltipText( l_Country.Name() );
            
            ((GUI::GFrame*) l_pObj)->ColorNormal(GColorRGBInt(0, 0, 0, 0) );
            ((GUI::GLabel*)l_pObj->Child(L"frmInfo")->Child(L"txtCode"))->Text(l_sCode.substr(0,3));
		      ((GUI::GPictureBox*)l_pObj->Child(L"frmInfo")->Child(L"picFlag"))->PictureNormal(g_ClientDAL.CountryFlagSmallPath(l_Actor.m_iCountryID));
         }
         else
         {
            g_Joshua.Log(GString(L"Country has no side in combat : ") + GString(l_Actor.m_iCountryID) );
            gassert(0, "No side for country");
         }
      }

      for(UINT32 i = 0;i < c_iNbSides;i ++)
      {
         for(UINT32 j = 0;j < EUnitCategory::ItemCount;j ++)
         {
            if(m_pObjCountTxt[i][j] )
            {
               m_pObjCountTxt[i][j]->Text(GString(l_iCounts[i][j] ) );
            }
         }
      }
   }
   else
   {
      for(UINT32 i = 0;i < c_iNbSides;i ++)
      {
         for(UINT32 j = 0;j < EUnitCategory::ItemCount;j ++)
         {
            if(m_pObjCountTxt[i][j] )
            {
               m_pObjCountTxt[i][j]->Text(L"");
            }
         }
      }
   }
   m_pObjCountryListA->Update();
   m_pObjCountryListB->Update();
}
