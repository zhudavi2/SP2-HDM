/**************************************************************
*
* sp2_battle_abstract_window.cpp
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
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_sound.h"

using namespace SP2;

const GString GBattleSummaryWindow::TypeName(L"Battle_Summary_Window");

GUI::GBaseObject* GBattleSummaryWindow::New()
{
	return (GUI::GBaseObject*) new GBattleSummaryWindow();
}

/*!
 * Constructor
 **/
GBattleSummaryWindow::GBattleSummaryWindow() : SP2::GBaseWindow()
{
   m_pListBoxSideA         = NULL;
   m_pListBoxSideB         = NULL;
   m_pLossesInfantrySideA  = NULL;
   m_pLossesInfantrySideB  = NULL;
   m_pLossesGroundSideA    = NULL;
   m_pLossesGroundSideB    = NULL;
   m_pLossesAirSideA       = NULL;
   m_pLossesAirSideB       = NULL;
   m_pLossesNavalSideA     = NULL;
   m_pLossesNavalSideB     = NULL;
   m_pStatusSideA          = NULL;
   m_pStatusSideB          = NULL;
   m_pBattleName           = NULL;
}

/*!
 * Destructor
 **/
GBattleSummaryWindow::~GBattleSummaryWindow()
{
}

bool GBattleSummaryWindow::OnCreate()
{
   IF_RETURN(!__super::OnCreate(),false);
   m_pBattleName           = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmBattleInfo")->Child("txtBattleName");
   m_pListBoxSideA         = (SDK::GUI::GListBox*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child("lstCountries");
   m_pListBoxSideB         = (SDK::GUI::GListBox*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child("lstCountries");;
   m_pLossesInfantrySideA  = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child(L"txtI");
   m_pLossesInfantrySideB  = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child(L"txtI");
   m_pLossesGroundSideA    = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child(L"txtG");
   m_pLossesGroundSideB    = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child(L"txtG");
   m_pLossesAirSideA       = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child(L"txtA");
   m_pLossesAirSideB       = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child(L"txtA");
   m_pLossesNavalSideA     = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child(L"txtN");
   m_pLossesNavalSideB     = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child(L"txtN");
   m_pStatusSideA          = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideA")->Child(L"lblStatus");
   m_pStatusSideB          = (SDK::GUI::GLabel*)Child(L"frmInner")->Child(L"frmInner")->Child(L"frmSideB")->Child(L"lblStatus");
   return true;
}

bool GBattleSummaryWindow::OnDestroy()
{
   __super::OnDestroy();
   if(Owner() ==  (GBaseObject*)g_ClientDDL.BattleOverviewWindow())
   {
      g_ClientDDL.BattleOverviewWindowKill();
   }
   return true;
}


void GBattleSummaryWindow::OnHide()
{
   __super::OnHide();
   //g_ClientDDL.BattleSummaryWindowKill();
}

GUI::EEventStatus::Enum GBattleSummaryWindow::OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.BattleSummaryWindowKill();
         }
      }
   }

   return __super::OnMouseClick(in_EventData, in_pCaller);
}

bool GBattleSummaryWindow::SetArenaInformation(const SP2::GArenaInfo& in_ArenaInformation)
{

   //Display the losses
   m_pLossesInfantrySideA->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Defender][EUnitCategory::Infantry  ]));
   m_pLossesGroundSideA  ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Defender][EUnitCategory::Ground    ]));
   m_pLossesAirSideA     ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Defender][EUnitCategory::Air       ]));
   m_pLossesNavalSideA   ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Defender][EUnitCategory::Naval     ]));
   m_pLossesInfantrySideB->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Attacker][EUnitCategory::Infantry  ]));
   m_pLossesGroundSideB  ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Attacker][EUnitCategory::Ground    ]));
   m_pLossesAirSideB     ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Attacker][EUnitCategory::Air       ]));
   m_pLossesNavalSideB   ->Text(GString(in_ArenaInformation.m_pArenaLosses[ECombatSide::Attacker][EUnitCategory::Naval     ]));

   //Get the ID of the region where the combat is located
   UINT32 in_iRegionID = g_ClientDCL.EarthLocateRegion(in_ArenaInformation.m_Coordinate.x,in_ArenaInformation.m_Coordinate.y);
   // append region in which battle happened
   if(in_iRegionID == 0)
      m_pBattleName->Text(g_ClientDAL.GetString(102255));
   else
      m_pBattleName->Text(g_ClientDAL.GetString(102254) + L" " + g_ClientDAL.RegionName(in_iRegionID));

   //Display the flags of the countries: Defending
   m_pListBoxSideA->Clear();
   for(vector<UINT16>::const_iterator l_It = in_ArenaInformation.m_vDefenderCountriesList.begin();
      l_It != in_ArenaInformation.m_vDefenderCountriesList.end();
      l_It++)
   {
      SDK::GUI::GListBox::GRow* l_pRow = m_pListBoxSideA->Insert_New_Row();
      SDK::GUI::GFrame* l_pFrame = (SDK::GUI::GFrame*)l_pRow->m_pObject;
      l_pFrame->ColorBackground(GColorRGBInt(0,0,0,0));
      SDK::GUI::GPictureBox* l_pFlagPicture = (SDK::GUI::GPictureBox*)l_pFrame->Child(L"frmInfo")->Child(L"picFlag");
      SDK::GUI::GLabel*      l_pCountryCode = (SDK::GUI::GLabel*)l_pFrame->Child(L"frmInfo")->Child("txtCode");

      //Get the country
      const GCountry& l_Country = g_ClientDAL.Country(*l_It); 
      l_pFlagPicture->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);  
      l_pFrame->Child(L"frmInfo")->Width(81);
      l_pCountryCode->Text(GString(l_Country.Flag()).substr(0,3));

      l_pFrame->TooltipText( l_Country.Name() );
   }
   m_pListBoxSideA->Update();

   //Display the flags of the countries: Defending
   m_pListBoxSideB->Clear();
   for(vector<UINT16>::const_iterator  l_It = in_ArenaInformation.m_vAttackerCountriesList.begin();
      l_It != in_ArenaInformation.m_vAttackerCountriesList.end();
      l_It++)
   {
      SDK::GUI::GListBox::GRow* l_pRow = m_pListBoxSideB->Insert_New_Row();
      SDK::GUI::GFrame* l_pFrame = (SDK::GUI::GFrame*)l_pRow->m_pObject;
      l_pFrame->ColorBackground(GColorRGBInt(0,0,0,0));
      SDK::GUI::GPictureBox* l_pFlagPicture = (SDK::GUI::GPictureBox*)l_pFrame->Child(L"frmInfo")->Child(L"picFlag");
      SDK::GUI::GLabel*      l_pCountryCode = (SDK::GUI::GLabel*)l_pFrame->Child(L"frmInfo")->Child("txtCode");

      //Get the country
      const GCountry& l_Country = g_ClientDAL.Country(*l_It);
      l_pFlagPicture->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
      l_pFrame->Child(L"frmInfo")->Width(81);
      l_pCountryCode->Text(GString(l_Country.Flag()).substr(0,3));

      l_pFrame->TooltipText( l_Country.Name() );
   }
   m_pListBoxSideB->Update();

   //Display which side is victorious
   ECombatSide::Enum l_WinnerSide = in_ArenaInformation.m_eWinnerSide;

   switch(l_WinnerSide)
   {
      case ECombatSide::Attacker:
         m_pStatusSideA->Text(g_ClientDAL.GetString(100402));
         m_pStatusSideB->Text(g_ClientDAL.GetString(100401));
         break;
      case ECombatSide::Defender:
         m_pStatusSideA->Text(g_ClientDAL.GetString(100401));
         m_pStatusSideB->Text(g_ClientDAL.GetString(100402));
         break;
   }


   return true;
}

void GBattleSummaryWindow::PlayResultMusic(const SP2::GArenaInfo& in_ArenaInformation)
{
   // If we are on the winner side, play a win sound.
   ECombatSide::Enum l_OurSide;

   vector<UINT16>::const_iterator l_countryIt = find(in_ArenaInformation.m_vAttackerCountriesList.begin(),in_ArenaInformation.m_vAttackerCountriesList.end(),g_ClientDAL.ControlledCountryID());
   if(l_countryIt == in_ArenaInformation.m_vAttackerCountriesList.end())
   {
      l_OurSide= ECombatSide::Defender;
   }
   else
   {
     l_OurSide= ECombatSide::Attacker;
   }
   
   
   ECombatSide::Enum l_WinnerSide = in_ArenaInformation.m_eWinnerSide;

   if(l_WinnerSide == ECombatSide::CantDetermine)
   {
      //combat was probably nuked, dont play a sound
   }
   else if(l_WinnerSide == l_OurSide)
   {
      g_SP2Client->PlaySound(ESoundEventID::CombatWin1);
   }
   else
   {
      g_SP2Client->PlaySound(ESoundEventID::CombatLost1);
   }
}