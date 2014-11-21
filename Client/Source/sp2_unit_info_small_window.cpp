/**************************************************************
*
* sp2_unit_info_small_window.cpp
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

#include "../include/sp2_unit_info_small_window.h"
#include "../include/sp2_background_earth.h"

const GString GUnitInfoSmallWindow::TypeName(L"Small_Unit_Info_Window");

GUI::GBaseObject* GUnitInfoSmallWindow::New()
{
   return new GUnitInfoSmallWindow();
}

GUnitInfoSmallWindow::GUnitInfoSmallWindow() : SDK::GUI::GFrame()
{
   m_iContainerWidth = 0;

   m_pInfantryValue = NULL;
   m_pGroundValue = NULL;
   m_pNavalValue = NULL;
   m_pAirValue = NULL;
}

bool GUnitInfoSmallWindow::OnCreate()
{
   __super::OnCreate();

   Focusable(false);
   m_iContainerWidth = Child(L"frmI")->Child(L"frmInfantry")->Width();

   gassert(Child(L"frmG")->Child(L"frmGround")->Width() == (INT32) m_iContainerWidth, "Invalid ground width" );
   gassert(Child(L"frmN")->Child(L"frmNaval")->Width() == (INT32) m_iContainerWidth, "Invalid naval width" );
   gassert(Child(L"frmA")->Child(L"frmAir")->Width() == (INT32) m_iContainerWidth, "Invalid air width" );

   m_iContainerWidth -= 1;

   m_pInfantryValue     = (GUI::GFrame*) Child(L"frmI")->Child(L"frmInfantry")->Child(L"frmValue");
   m_pGroundValue       = (GUI::GFrame*) Child(L"frmG")->Child(L"frmGround")->Child(L"frmValue");
   m_pNavalValue        = (GUI::GFrame*) Child(L"frmN")->Child(L"frmNaval")->Child(L"frmValue");
   m_pAirValue          = (GUI::GFrame*) Child(L"frmA")->Child(L"frmAir")->Child(L"frmValue");

   m_pInfantryTxt    = (GUI::GLabel*)Child(L"txtI");
   m_pGroundTxt      = (GUI::GLabel*)Child(L"txtG");
   m_pNavalTxt       = (GUI::GLabel*)Child(L"txtN");
   m_pAirTxt         = (GUI::GLabel*)Child(L"txtA");

   return true;
}

void GUnitInfoSmallWindow::DescribeGroup(SP2::GUnitGroup* in_pGroup)
{
	UINT32 l_iOwnerID = in_pGroup->OwnerId();
	REAL32 l_fAccuracyOfInformation = g_ClientDAL.m_PlayerCountryData.AccuracyInformation(l_iOwnerID);
   
	UINT32 l_iRegionID = g_ClientDCL.EarthLocateRegion(in_pGroup->Position().x,in_pGroup->Position().y);
	UINT32 l_iCountryWhereTheUnitsAre = g_ClientDAL.RegionControl(l_iRegionID).m_iMilitary;
	if(g_ClientDAL.DiplomaticStatus(l_iCountryWhereTheUnitsAre,g_ClientDAL.m_PlayerCountryData.CountryID()) == EDiplomaticStatus::Allied)
		l_fAccuracyOfInformation += SP2::c_fSameRegionAccuracyBonus; 

	l_fAccuracyOfInformation = min(1.f,l_fAccuracyOfInformation);

	UINT32 l_iNbInfantry = 0;
   UINT32 l_iNbGround = 0;
   UINT32 l_iNbNaval = 0;
   UINT32 l_iNbAir = 0;

   UINT64 l_iTotInfantry = 0;
   UINT64 l_iTotGround = 0;
   UINT64 l_iTotNaval = 0;
   UINT64 l_iTotAir = 0;

	Random::GQuick l_Rand;

   const vector<SDK::Combat::GUnit*>& l_vUnits = in_pGroup->Units();
   for(UINT32 i = 0;i < l_vUnits.size();i ++)
   {
		SP2::GUnitType* l_pUnitType = ((SP2::GUnitType*)((SP2::GUnitDesign*)l_vUnits[i]->Design())->Type());
		UINT32 l_iQtyToAdd = 0;
		l_Rand.Seed(l_iOwnerID * (UINT32)floor(l_fAccuracyOfInformation*100.f) * l_pUnitType->Id() * l_vUnits[i]->Id());
		if(l_Rand.RandomReal() > l_fAccuracyOfInformation)
			continue;
		l_Rand.RandomReal(); //To keep the same order as the unit list window
		if(l_Rand.RandomReal() <= l_fAccuracyOfInformation)
			l_iQtyToAdd = (UINT32)(((REAL32)(((SP2::GUnit*)l_vUnits[i])->Qty())) * (1.f + ( (1.f-l_fAccuracyOfInformation) * (l_Rand.RandomReal(2.f)-1.f)  )));

		if(l_iQtyToAdd == 0)
			continue;

      switch(l_pUnitType->Category() )
      {
      case EUnitCategory::Infantry:
         l_iNbInfantry ++;
         l_iTotInfantry += l_iQtyToAdd;   
         break;
      case EUnitCategory::Ground:
         l_iNbGround ++;
         l_iTotGround += l_iQtyToAdd;   
         break;
      case EUnitCategory::Naval:
         l_iNbNaval ++;
         l_iTotNaval += l_iQtyToAdd;   
         break;
      case EUnitCategory::Air:
         l_iNbAir ++;
         l_iTotAir += l_iQtyToAdd;   
         break;
      }
   }

   UINT32 l_iTotal = l_iNbInfantry + l_iNbGround + l_iNbNaval + l_iNbAir;

   REAL32 l_fInfantryWidth = (REAL32) (m_iContainerWidth * l_iNbInfantry) / (REAL32) l_iTotal;
   REAL32 l_fGroundWidth = (REAL32) (m_iContainerWidth * l_iNbGround) / (REAL32) l_iTotal;
   REAL32 l_fNavalWidth = (REAL32) (m_iContainerWidth * l_iNbNaval) / (REAL32) l_iTotal;
   REAL32 l_fAirWidth = (REAL32) (m_iContainerWidth * l_iNbAir) / (REAL32) l_iTotal;

   m_pInfantryValue->Width( (INT16) ceilf(l_fInfantryWidth) );
   m_pGroundValue->Width( (INT16) ceilf(l_fGroundWidth) );
   m_pNavalValue->Width( (INT16) ceilf(l_fNavalWidth) );
   m_pAirValue->Width( (INT16) ceilf(l_fAirWidth) );

	if(l_iTotInfantry == 0 && l_fAccuracyOfInformation != 1.f)
		m_pInfantryTxt->Text(L"?");
	else
		m_pInfantryTxt->Text(g_ClientDDL.FormatNumberContracted((REAL64)l_iTotInfantry));
	if(l_iTotGround == 0 && l_fAccuracyOfInformation != 1.f)
		m_pGroundTxt->Text(L"?");
	else
		m_pGroundTxt->Text(g_ClientDDL.FormatNumberContracted((REAL64)l_iTotGround));
	if(l_iTotNaval == 0 && l_fAccuracyOfInformation != 1.f)
		m_pNavalTxt->Text(L"?");
	else
		m_pNavalTxt->Text(g_ClientDDL.FormatNumberContracted((REAL64)l_iTotNaval));
	if(l_iTotAir == 0 && l_fAccuracyOfInformation != 1.f)
		m_pAirTxt->Text(L"?");
	else
		m_pAirTxt->Text(g_ClientDDL.FormatNumberContracted((REAL64)l_iTotAir));
   
   
   
}

void GUnitInfoSmallWindow::OnShow()
{
   __super::OnShow();
   SendToBack();
   g_ClientDDL.BackgroundEarth()->SendToBackUnits();
}

void GUnitInfoSmallWindow::OnHide()
{
   __super::OnHide();
}
