/**************************************************************
*
* sp2_unit_design_window.cpp
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

#include "../include/sp2_create_design_window.h"

#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_military_window.h"
#include "../include/sp2_unit_view_scene.h"
#include "../include/sp2_gui_helper.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCreateDesignWindow::TypeName(L"Unit_New_Design");

GUI::GBaseObject* GCreateDesignWindow::New()
{
   return new GCreateDesignWindow();
}

GCreateDesignWindow::GCreateDesignWindow() : GFrame()
{
   m_pUnitScene = NULL;
   m_SelectedDisplay.PieceIndex[0] = -1;
   m_SelectedDisplay.PieceIndex[1] = -1;
   m_SelectedDisplay.PieceIndex[2] = -1;
   m_SelectedDisplay.ColorIndex = 0;

   m_iCurrentPiece = 0;
   m_iCurrentVariation = 0;
}

GCreateDesignWindow::~GCreateDesignWindow()
{
}

bool GCreateDesignWindow::OnCreate()
{
   __super::OnCreate();


   GUI::GFrame* l_pFrmDesignSpecs = (GUI::GFrame*)    Child(L"frmDesignSpecs");
   GUI::GFrame* l_pFrmUnitInfo    = (GUI::GFrame*)    l_pFrmDesignSpecs->Child(L"frmUnitInfo");

   m_pDesignTypeCbo               = (GUI::GComboBox*)  l_pFrmUnitInfo->Child(L"cboUnitType");
   m_pObjNameEdt                  = (GUI::GEditBox*)   l_pFrmUnitInfo->Child(L"edtName");
   m_pObjDescFrm                  = (GUI::GFrame*)     l_pFrmUnitInfo->Child(L"frmDescription");
   m_pObjDescTxt                  = (GUI::GLabel*)     m_pObjDescFrm->Child(L"txtText");
   m_pObjDescScr                  = (GUI::GVScrollBar*)m_pObjDescFrm->Child(L"scrText");
   m_pObjCostTxt                  = (GUI::GLabel*)    (GUI::GFrame*)l_pFrmDesignSpecs->Child(L"frm1stLvlInner1")->Child(L"frmUnitInfo1")->Child(L"txtCost");

   ((GFX::GWindow*)m_pObjDescFrm->Model()->Get_Primitive())->ClipChildren(true);

   //Technology meters
   GUI::GFrame* l_pMetersFrm = (GUI::GFrame*)l_pFrmDesignSpecs->Child(L"frm1stLvlInner1")->Child(L"frmInner");
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]        = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmGunRange");
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]    = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmGunPrecision");
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]       = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmGunDamage");
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]    = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmMissileRange");
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]= (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmMissilePrecision");
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]   = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmMissileDamage");
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]  = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmMissilePayload");
   m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]           = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmSpeed");
   m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]         = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmStealth");
   m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors]         = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmSensors");
   m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]  = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmCountermeasures");
   m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]           = (GTechnologyMeterBlue*)l_pMetersFrm->Child(L"frmArmor");

	m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]        = (GLabel*)l_pMetersFrm->Child(L"lblGunRange");
   m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision]    = (GLabel*)l_pMetersFrm->Child(L"lblGunPrecision");
   m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]       = (GLabel*)l_pMetersFrm->Child(L"lblGunDamage");
   m_pTechnologyLabels[EUnitDesignCharacteristics::MissileRange]    = (GLabel*)l_pMetersFrm->Child(L"lblMissileRange");
   m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePrecision]= (GLabel*)l_pMetersFrm->Child(L"lblMissilePrecision");
   m_pTechnologyLabels[EUnitDesignCharacteristics::MissileDamage]   = (GLabel*)l_pMetersFrm->Child(L"lblMissileDamage");
   m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePayload]  = (GLabel*)l_pMetersFrm->Child(L"lblMissilePayload");
   m_pTechnologyLabels[EUnitDesignCharacteristics::Speed]           = (GLabel*)l_pMetersFrm->Child(L"lblSpeed");
   m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]         = (GLabel*)l_pMetersFrm->Child(L"lblStealth");
   m_pTechnologyLabels[EUnitDesignCharacteristics::Sensors]         = (GLabel*)l_pMetersFrm->Child(L"lblSensors");
   m_pTechnologyLabels[EUnitDesignCharacteristics::CounterMesures]  = (GLabel*)l_pMetersFrm->Child(L"lblCountermeasures");
   m_pTechnologyLabels[EUnitDesignCharacteristics::Armor]           = (GLabel*)l_pMetersFrm->Child(L"lblArmor");

   m_pModelFrm       = (SDK::GUI::GUIContainer*)Child(L"frmDesignView")->Child(L"Model");
   m_pPartLeftFrm    = (SDK::GUI::GUIContainer*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"LeftModel");
   m_pPartMiddleFrm  = (SDK::GUI::GUIContainer*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"MiddleModel");
   m_pPartRightFrm   = (SDK::GUI::GUIContainer*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"RightModel");


   m_pObjLeftBtn     = (SDK::GUI::GButton*)Child(L"frmBuild")->Child(L"frmPage")->Child(L"btnLeft");
   m_pObjRightBtn    = (SDK::GUI::GButton*)Child(L"frmBuild")->Child(L"frmPage")->Child(L"btnRight");

   m_pObjBtnExit     = (SDK::GUI::GButton*)Child(L"frmTitle")->Child(L"btnExit");
   m_pObjLockTog     = (SDK::GUI::GToggle*)Child(L"frmTitle")->Child(L"togLockWindow");

   m_pObjConfirmCreationBtn = (SDK::GUI::GButton*) Child(L"frmAction")->Child(L"btnConfirm");
	m_pObjCancelBtn			 = (SDK::GUI::GButton*) Child(L"frmAction")->Child(L"btnCancel");
   m_pObjPieceTypeCbo       = (SDK::GUI::GComboBox*) Child(L"frmBuild")->Child(L"frmPage")->Child(L"cboType");

  //Fill the Unit Type Combo with the possible type names
   {     
      const vector<GUnitType*>& l_vUnitTypes = g_ClientDAL.UnitTypes();
      for(UINT32 i = 1 ; i < l_vUnitTypes.size() ; i++)
      {
         // dont add nuclear categories if not allowed
         if(!g_ClientDAL.GameOptions().NuclearAllowed() && l_vUnitTypes[i]->Category() == SP2::EUnitCategory::Nuclear)
            continue;
         //Add the item in the combo box
         m_pDesignTypeCbo->Add_Item(l_vUnitTypes[i]->Name());
         //Set the ID of the item in the combo box to be the ID of the unit type
         m_pDesignTypeCbo->GetListBox()->Get_Row_At(i-1)->m_iAdditionalId = i+1;
      }
   }

   for(INT32 i=1; i<=c_iNbMilitaryColors; i++)
   {
      m_vpObjColorBtn.push_back((GUI::GButton*)Child(L"frmBuild")->Child(L"frmTextures")->Child(L"btn" + GString(i)));
   }

   m_vpObjPartBtn.push_back((GUI::GButton*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"btnLeft"));
   m_vpObjPartBtn.push_back((GUI::GButton*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"btnMiddle"));
   m_vpObjPartBtn.push_back((GUI::GButton*)Child(L"frmBuild")->Child(L"frmParts")->Child(L"btnRight"));

   for(UINT32 i = 0;i < m_vpObjColorBtn.size();i ++)
   {
      m_vpObjColorBtn[i]->BringToFront();
   }
   
   for(UINT32 i = 0;i < m_vpObjPartBtn.size();i ++)
   {
      m_vpObjPartBtn[i]->BringToFront();
   }

   // Setup the unit design display
   m_pUnitScene = new GUnitViewScene;
   m_pUnitScene->ChangeAspectRatio( (REAL32) m_pModelFrm->Width(), (REAL32) m_pModelFrm->Height() );

   CGFX_Viewport_Itf* l_pViewport = g_Joshua.Renderer()->Create_Viewport();
   l_pViewport->Set_Scene(m_pUnitScene->Scene() );
   m_pModelFrm->Model()->Set_Primitive(l_pViewport);

   // Setup the unit parts display
   GVector2D<UINT32> l_PartSize = m_pPartLeftFrm->Size();
   gassert(l_PartSize == m_pPartMiddleFrm->Size(), "Part window sizes are unequal (middle)");
   gassert(l_PartSize == m_pPartRightFrm->Size(), "Part window sizes are unequal (right)");

   m_pUnitParts = new GUnitViewParts(l_PartSize.x, l_PartSize.y);

   m_vPartWindows.resize(3);

   for(UINT32 i = 0;i < m_vPartWindows.size();i ++)
   {
      m_vPartWindows[i] = g_Joshua.Renderer()->CreateWindow();
   }

   m_pPartLeftFrm->Model()->Set_Primitive(m_vPartWindows[0]);
   m_pPartMiddleFrm->Model()->Set_Primitive(m_vPartWindows[1]);
   m_pPartRightFrm->Model()->Set_Primitive(m_vPartWindows[2]);

   for(UINT32 i = 0;i < m_vPartWindows.size();i ++)
   {
      m_vPartWindows[i]->Color().R = 0.01f;
      m_vPartWindows[i]->Color().G = 0.01f;
      m_vPartWindows[i]->Color().B = 0.01f;

      m_vPartWindows[i]->ColorOperation(GFX::OPERATOR_BLEND_TEXTURE_ALPHA);
      m_vPartWindows[i]->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);
   }

   l_pFrmDesignSpecs->BringToFront();

   m_pDesignTypeCbo->Selected_Content(0);
   OnUnitTypeChange(EUnitType::InfantryVehicle);

	if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());

   return true;
}

void GCreateDesignWindow::OnSubjectChange(DesignPattern::GSubject &in_pSubject, const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if( in_Notification == ENotifyWindowMsg::ResearchWindow )
   {
      UpdateLevelFilter();
   }
}

bool GCreateDesignWindow::OnDestroy()
{
   RegisteredModalWindow.clear();

   ReleaseUnitParts();
   SAFE_DELETE(m_pUnitScene);

   for(UINT32 i = 0;i < m_vPartWindows.size();i ++)
   {
      SAFE_RELEASE(m_vPartWindows[i] )
   }

   SAFE_DELETE(m_pUnitParts);

   __super::OnDestroy();

   return true;
}

void GCreateDesignWindow::OnShow()
{
   __super::OnShow();

   UpdateLevelFilter();
   ResetSpecs();
   //m_pUnitScene->HideUnit();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::ResearchWindow);
}

void GCreateDesignWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::ResearchWindow);
   
   __super::OnHide();
}


void GCreateDesignWindow::UpdateLevelFilter()
{
   //Fetch the actually selected unit type
   EUnitType::Enum l_eActuallySelectedUnitType = ActuallySelectedUnitType();
   //If no unit type actually selected
   if(l_eActuallySelectedUnitType < 0)
   {
      ResetSpecs(L""); 
      return;
   }

   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      m_pTechnologyMeters[i]->Enabled(true);
		m_pTechnologyLabels[i]->Enabled(true);
   }

   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      REAL32 l_fMaxValue = g_ClientDAL.m_PlayerCountryData.ResearchInfo()->m_fMaxValues[c_eUnitCategoryByType[l_eActuallySelectedUnitType]][i];
      m_pTechnologyMeters[i]->Maximum()[0]         = floor(l_fMaxValue);
      m_pTechnologyMeters[i]->Maximum()[1]         = floor(l_fMaxValue);
      m_pTechnologyMeters[i]->RightBound             ((INT16)l_fMaxValue * 8 );
      m_pTechnologyMeters[i]->Value                  (l_fMaxValue,       1 );
   }


   switch(c_eUnitCategoryByType[l_eActuallySelectedUnitType])
   {
   case SP2::EUnitCategory::Ground:      
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Maximum()[0]  = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum()[0]  = 0;		
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Maximum()[1]  = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum()[1]  = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->RightBound(0);
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->RightBound(0);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Value(0 , 1);
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0 , 1);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
		m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]			->Enabled(false);
		m_pTechnologyLabels[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);

      switch(l_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::MobileLauncher:
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      case SP2::EUnitType::Tank:
      case SP2::EUnitType::ArtilleryGun:
      case SP2::EUnitType::InfantryVehicle:
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
         break;
      }

      break;
   
   case SP2::EUnitCategory::Air:
      switch(l_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::TransportHelicopter:
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);

         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);

         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);

         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);

         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         break;
      case SP2::EUnitType::Bomber:
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      }
      break;
   case SP2::EUnitCategory::Naval:
      switch(l_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::ASWHelicopter:
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]      ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]      ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]      ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]      ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]      ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]      ->Enabled(false);
         break;
      case SP2::EUnitType::PatrolCraft:
      case SP2::EUnitType::Corvette:
      case SP2::EUnitType::Frigate:
      case SP2::EUnitType::Destroyer:
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         break;
      case SP2::EUnitType::AttackSubmarine:
      case SP2::EUnitType::BallisticMissileSubmarine:
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      case SP2::EUnitType::AircraftCarrier:
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
			m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         break;
      }
      break;
  
   case SP2::EUnitCategory::Nuclear:
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Maximum()       [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Maximum()   [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Maximum()      [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Maximum() [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Maximum()        [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum() [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Maximum()          [0]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Maximum()          [0]     = 0;

      m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Maximum()       [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Maximum()   [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Maximum()      [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Maximum() [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Maximum()        [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum() [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Maximum()          [1]     = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Maximum()          [1]     = 0;

      m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->RightBound( 0 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->RightBound( 0 );

      m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value( 0, 1 );
      m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value( 0, 1 );

      m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Enabled(false);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Enabled(false);

		m_pTechnologyLabels[EUnitDesignCharacteristics::GunRange]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::GunPrecision]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::GunDamage]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::MissilePayload]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::Stealth]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::Armor]->Enabled(false);
      m_pTechnologyLabels[EUnitDesignCharacteristics::Speed]->Enabled(false);

      m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Maximum()[0]  = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Maximum()[1]  = 0;
      m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->RightBound(0);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Value(0 , 1);
      m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Enabled(false);
		m_pTechnologyLabels[EUnitDesignCharacteristics::Sensors] ->Enabled(false);

      break;
   }
}

GUI::EEventStatus::Enum GCreateDesignWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjBtnExit)
         {
            g_ClientDDL.CreateDesignWindowKill();
         }

			if(in_pCaller == m_pObjCancelBtn)
         {
            g_ClientDDL.CreateDesignWindowKill();
         }

         if(in_pCaller == m_pObjLeftBtn)
            OnLeftButton();

         if(in_pCaller == m_pObjRightBtn)
            OnRightButton();

         if(in_pCaller == m_pObjConfirmCreationBtn)
         {				
            if(!g_ClientDDL.GenericMessageWindow())
               g_ClientDDL.GenericMessageWindowSpawn();

				if(m_pObjNameEdt->Text().size())
				{
					// if we can save or if we are in tutorial mode
					if(SaveDesign() || GTutorials::Instance().StartedTutorial() != ETutorials::None)
						g_ClientDDL.CreateDesignWindowKill();
					else
						g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(102245), this );
				}
				else
					g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(102263), this );
         }


         for(UINT32 i=0; i<m_vpObjPartBtn.size(); i++)
         {
            if(in_pCaller == m_vpObjPartBtn[i])
            {
               OnPartClick(i);
               break;
            }
         }

         for(UINT32 i=0; i<m_vpObjColorBtn.size(); i++)
         {
            if(in_pCaller == m_vpObjColorBtn[i])
            {
               OnColorButton(i);
               break;
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GCreateDesignWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if( (in_EventData.InitialRecipient == m_pModelFrm) || 
       (in_EventData.InitialRecipient == m_pObjDescTxt) )
   {
      if(in_EventData.Mouse.Down.Bits.Left)
      {
         Drag();
         return GUI::EEventStatus::Handled;
      }
   }
   else
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         GBaseObject* l_pClicked = g_Joshua.GUIManager()->MouseDownTarget();
         if(l_pClicked->Name() == L"lblTitle" || l_pClicked->Name() == L"frmTitle")         
         {
            if(l_pClicked->Name() == L"lblTitle" && l_pClicked->Owner()->Name() != L"frmTitle")
            {
               return EEventStatus::Handled;
            }
            else if(l_pClicked->Name() == L"frmTitle" && l_pClicked->Owner() != this)
            {
               return EEventStatus::Handled;
            }

            m_MouseClick = Position();
            Drag();
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

bool GCreateDesignWindow::OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller)
{
   BringToFront();  

   if(g_ClientDDL.MainBar() && g_ClientDDL.MainBar()->Visible())
      g_ClientDDL.MainBar()->BringToFront();

   if(g_ClientDDL.MiniMap() && g_ClientDDL.MiniMap()->Visible())      
      g_ClientDDL.MiniMap()->BringToFront();
   
   return false;
}


GUI::EEventStatus::Enum GCreateDesignWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller->Owner() && 
            (in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::Speed] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures] ||
            in_pCaller->Owner() == m_pTechnologyMeters[EUnitDesignCharacteristics::Armor] 
            )
         )
         {
            EUnitType::Enum l_eActuallySelectedUnitType = ActuallySelectedUnitType();
            if(l_eActuallySelectedUnitType >= 0)
            {
               REAL32 l_fUnitCost = g_ClientDCL.CalculateUnitCost(ActuallySelectedUnitType(),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0),
                                                                  (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value(0));

               m_pObjCostTxt->Text(GString::FormatNumber(l_fUnitCost,L" ",L".",L"",L" $",3,0));
               }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GCreateDesignWindow::OnMouseWheel(const GEventData& in_Data, GBaseObject*)
{
   if( (in_Data.InitialRecipient == m_pModelFrm) )
   {
      m_pUnitScene->CameraControl().OffsetDistanceRelative( (REAL32) in_Data.Mouse.WheelDelta * -0.1f);
   }
   else if( in_Data.InitialRecipient == m_pObjDescTxt )
   {
      m_pObjDescScr->OnMouseWheel(in_Data, NULL);
   }

   return GUI::EEventStatus::Handled;
}

void GCreateDesignWindow::OnDrag(const GEventData& in_EventData, const GVector2D<INT32>& in_MouseMove, const GVector2D<INT32>& in_MouseRelative)
{
   if( (in_EventData.InitialRecipient == m_pModelFrm) || 
       (in_EventData.InitialRecipient == m_pObjDescTxt) )
   {
      m_pUnitScene->CameraControl().OffsetOrientation( (REAL32) in_MouseMove.x * -0.025f, (REAL32) in_MouseMove.y * 0.025f);
   }
   else
   {
      GVector2D<INT32> l_NewPos = m_MouseClick + in_MouseRelative;
      
      CGFX_Renderer_Init_Mode mode;
      g_Joshua.Renderer()->Get_Current_Mode(&mode); 

      if(l_NewPos.x < 0)
         l_NewPos.x = 0;
      
      if(l_NewPos.y < 0)
         l_NewPos.y = 0;
      
      if(l_NewPos.x + Width() > mode.m_Resolution.x)
         l_NewPos.x = mode.m_Resolution.x - Width();

      if(l_NewPos.y + Height() > mode.m_Resolution.y)
         l_NewPos.y = mode.m_Resolution.y - Height();

      Position(l_NewPos);
   }
}

GUI::EEventStatus::Enum GCreateDesignWindow::OnCustomEvent(UINT32 in_iEvent, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch(in_iEvent)
   {
      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         GUI::GComboBox* l_pCombo = (GUI::GComboBox*)in_EventData.Data;
         //Unit type combo selected
         if(l_pCombo == m_pDesignTypeCbo)
         {
            OnUnitTypeChange(ActuallySelectedUnitType());
            return GUI::EEventStatus::Handled;
         }
         if(l_pCombo == m_pObjPieceTypeCbo)
         {
            //Update the pieces to display
            UINT32 l_iNewPieceID = m_pObjPieceTypeCbo->SelectedRow()->m_iAdditionalId;
            OnUnitPieceChange(l_iNewPieceID);
         }
         break;
      }
      case ECustomEvents::OnScrollVertical:
         {
            m_pObjDescTxt->Top(-((INT16)in_EventData.Data));
         }
         break;
      default:
         break;      
   }
   return GUI::EEventStatus::NotHandled;
}

void GCreateDesignWindow::ResetSpecs(GString in_sName)
{
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value(0,0);
   m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors]->Value(0,0);

   m_pObjNameEdt->Text(in_sName);
   m_pObjCostTxt->Text(L"");
}

bool GCreateDesignWindow::SaveDesign()
{
   //create game event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GAddUnitDesign);
   SP2::Event::GAddUnitDesign* l_pUpdate = (SP2::Event::GAddUnitDesign*)l_pEvent.get();

   //set the specs
   l_pUpdate->m_iGunRange           = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value(0);
   l_pUpdate->m_iGunPrecision       = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value(0);
   l_pUpdate->m_iGunDamage          = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value(0);
   l_pUpdate->m_iMissileRange       = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]->Value(0);
   l_pUpdate->m_iMissilePrecision   = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(0);
   l_pUpdate->m_iMissileDamage      = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]->Value(0);
   l_pUpdate->m_iMissilePayload     = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value(0);
   l_pUpdate->m_iSpeed              = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value(0);
   l_pUpdate->m_iStealth            = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value(0);
   l_pUpdate->m_iCountermeasures    = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0);
   l_pUpdate->m_iArmor              = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value(0);
   l_pUpdate->m_iSensors            = (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors]->Value(0);

	EUnitType::Enum l_eType = ActuallySelectedUnitType();
	//Check if the design is correct
	switch(l_eType)
	{
	case EUnitType::PatrolCraft:
	case EUnitType::Corvette:
	case EUnitType::Frigate:
	case EUnitType::Destroyer:
		if(l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		if( (l_pUpdate->m_iGunRange == 0 || l_pUpdate->m_iGunPrecision == 0 || l_pUpdate->m_iGunDamage == 0) &&
			 (l_pUpdate->m_iMissileRange == 0 || l_pUpdate->m_iMissilePrecision == 0 || l_pUpdate->m_iMissileDamage == 0 || l_pUpdate->m_iMissilePayload == 0) )
			 return false;
		break;
	case EUnitType::AttackSubmarine:
	case EUnitType::BallisticMissileSubmarine:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::AircraftCarrier:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1)
			return false;
		break;
	case EUnitType::BallisticMissile:
		if( l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1)
			return false;
		break;
	case EUnitType::Bomber:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::AttackAircraft:
	case EUnitType::FighterAircraft:
	case EUnitType::AttackHelicopter:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iGunRange < 1 || l_pUpdate->m_iGunPrecision < 1 || l_pUpdate->m_iGunDamage < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::ASWHelicopter:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::TransportHelicopter:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iGunRange < 1 || l_pUpdate->m_iGunPrecision < 1 || l_pUpdate->m_iGunDamage < 1 || l_pUpdate->m_iCountermeasures < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;	
	case EUnitType::ArtilleryGun:
	case EUnitType::Tank:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iGunRange < 1 || l_pUpdate->m_iGunPrecision < 1 || l_pUpdate->m_iGunDamage < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::MobileLauncher:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iMissilePayload < 1 || l_pUpdate->m_iMissileRange < 1 || l_pUpdate->m_iMissilePrecision < 1 || l_pUpdate->m_iMissileDamage < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	case EUnitType::AirDefense:
		if(l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		if( (l_pUpdate->m_iGunRange == 0 || l_pUpdate->m_iGunPrecision == 0 || l_pUpdate->m_iGunDamage == 0) &&
			 (l_pUpdate->m_iMissileRange == 0 || l_pUpdate->m_iMissilePrecision == 0 || l_pUpdate->m_iMissileDamage == 0 || l_pUpdate->m_iMissilePayload == 0) )
			 return false;
		break;
	case EUnitType::InfantryVehicle:
		if( l_pUpdate->m_iSpeed < 1 || l_pUpdate->m_iSensors < 1 || l_pUpdate->m_iGunRange < 1 || l_pUpdate->m_iGunPrecision < 1 || l_pUpdate->m_iGunDamage < 1 || l_pUpdate->m_iArmor < 1)
			return false;
		break;
	}

   //set display
   l_pUpdate->m_Variations          = m_SelectedDisplay;

   //set name
   l_pUpdate->m_sName = m_pObjNameEdt->Text();
   
   //set the designer as our country
   l_pUpdate->m_iDesignerId = (INT16)g_ClientDAL.ControlledCountryID();
   
   l_pUpdate->m_iUnitType = (UINT8)ActuallySelectedUnitType();
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();

   g_Joshua.RaiseEvent(l_pEvent);
   g_Joshua.PassSteamAchievement(0);

	return true;
}

//Events for arrow buttons
void GCreateDesignWindow::OnLeftButton()
{
   if(m_UnitPartPictures[m_iCurrentPiece].size() > m_vpObjPartBtn.size() )
   {
      m_iCurrentVariation --;
      if(m_iCurrentVariation < 0)
      {
         m_iCurrentVariation = m_UnitPartPictures[m_iCurrentPiece].size() - 1;
      }

      UpdateUnitParts();
   }
}

void GCreateDesignWindow::OnRightButton()
{
   if(m_UnitPartPictures[m_iCurrentPiece].size() > m_vpObjPartBtn.size() )
   {
      m_iCurrentVariation ++;
      if(m_iCurrentVariation >= (INT32) m_UnitPartPictures[m_iCurrentPiece].size() )
      {
         m_iCurrentVariation = 0;
      }

      UpdateUnitParts();
   }
}


//Event for on color change, for now there is no color disabled because filter on 
//naval, ground and air are not implemented yet
void GCreateDesignWindow::OnColorButton(INT32 in_iColor)
{
   if(ActuallySelectedUnitType() >= 0)
   {
      m_SelectedDisplay.ColorIndex = (INT16) in_iColor;
      m_pUnitScene->DisplayUnit(ActuallySelectedUnitType(), m_SelectedDisplay);
      g_Joshua.GUIManager()->SetDirty();
      UpdateUnitParts();
   }
}

//Event for click on the wanted part
void GCreateDesignWindow::OnPartClick(INT32 in_iPart)
{
   if(in_iPart < (INT32) m_UnitPartPictures[m_iCurrentPiece].size() )
   {
      INT32 l_iNewPart = in_iPart + m_iCurrentVariation;
      if(l_iNewPart >= (INT32) m_UnitPartPictures[m_iCurrentPiece].size() )
      {
         l_iNewPart -= m_UnitPartPictures[m_iCurrentPiece].size();
      }

      INT16 l_iNewPieceID = (INT16) m_UnitPartPictures[m_iCurrentPiece][l_iNewPart].m_iVariation;

      // Make sure the selected part is available for its parent
      GDesignVariationHolder::GType::GPiece* l_Piece = 
         g_ClientDAL.DesignVariationHolder.Type(ActuallySelectedUnitType())->Pieces[m_iCurrentPiece];
      gassert(l_Piece, "A piece part should exist for this window");
      if(l_Piece->ParentID != c_iRootParentId)
      {
         GDesignVariationHolder::GType::GPiece* l_ParentPiece = g_ClientDAL.DesignVariationHolder.Type(ActuallySelectedUnitType())->Pieces[l_Piece->ParentID];
         const vector<UINT32>& l_vConnectableArray = 
            l_ParentPiece->Variations[m_SelectedDisplay.PieceIndex[l_Piece->ParentID] ].Connectable;
         if(!l_vConnectableArray.empty() )
         {
            if(find(l_vConnectableArray.begin(), l_vConnectableArray.end(), l_iNewPieceID) == 
               l_vConnectableArray.end() )
            {
               return;
            }
         }
      }

      m_SelectedDisplay.PieceIndex[m_iCurrentPiece] = l_iNewPieceID;
      m_pUnitScene->DisplayUnit(ActuallySelectedUnitType(), m_SelectedDisplay);
      g_Joshua.GUIManager()->SetDirty();
   }
}

void GCreateDesignWindow::UpdateUnitParts()
{
   if( (!g_ClientDAL.DesignVariationHolder.Type(ActuallySelectedUnitType()) ) || 
       (m_iCurrentNbPieces == 0) )
   {
      return;
   }

   // Make sure the selected part is available for its parent
   GDesignVariationHolder::GType::GPiece* l_Piece = 
      g_ClientDAL.DesignVariationHolder.Type(ActuallySelectedUnitType())->Pieces[m_iCurrentPiece];
   gassert(l_Piece, "A piece part should exist for this window");
   GDesignVariationHolder::GType::GPiece* l_ParentPiece = NULL;

   if(l_Piece->ParentID != c_iRootParentId)
   {
      l_ParentPiece = g_ClientDAL.DesignVariationHolder.Type(ActuallySelectedUnitType())->Pieces[l_Piece->ParentID];
   }

   vector<GUnitViewParts::GPicture>& l_vPieceParts = m_UnitPartPictures[m_iCurrentPiece];
   UINT32 i;
   for(i = 0;(i < m_vPartWindows.size() ) && (i < l_vPieceParts.size() );i ++)
   {
      INT32 l_iVariationIndex = (i + m_iCurrentVariation);
      if(l_iVariationIndex >= (INT32) l_vPieceParts.size() )
      {
         l_iVariationIndex -= l_vPieceParts.size();
      }

      m_vPartWindows[i]->SetImage(l_vPieceParts[l_iVariationIndex].m_pTxtr);
      m_vPartWindows[i]->Color() = c_MilitaryColorPalette[m_SelectedDisplay.ColorIndex];

      // Make sure the current part is selectable
      if(l_ParentPiece)
      {
         const vector<UINT32>& l_vConnectableArray = 
            l_ParentPiece->Variations[m_SelectedDisplay.PieceIndex[l_Piece->ParentID] ].Connectable;
         if(!l_vConnectableArray.empty() )
         {
            if(find(l_vConnectableArray.begin(), 
                    l_vConnectableArray.end(), 
                    l_vPieceParts[l_iVariationIndex].m_iVariation) == 
               l_vConnectableArray.end() )
            {
               m_vPartWindows[i]->Color().A = 0.25f;
            }
         }
      }
   }

   for(;i < m_vPartWindows.size();i ++)
   {
      m_vPartWindows[i]->SetImage(NULL);
      m_vPartWindows[i]->Color() = GColorRGBReal(0.01f, 0.01f, 0.01f, 1.f);
   }
}

void GCreateDesignWindow::ReleaseUnitParts()
{
   for(UINT32 i = 0;i < m_vPartWindows.size();i ++)
   {
      m_vPartWindows[i]->SetImage(NULL);
   }

   for(UINT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
   {
      for(UINT32 j = 0;j < m_UnitPartPictures[i].size();j ++)
      {
         SAFE_RELEASE(m_UnitPartPictures[i][j].m_pTxtr);
      }
      m_UnitPartPictures[i].clear();
   }
}


EUnitType::Enum GCreateDesignWindow::ActuallySelectedUnitType() const
{
   GListBox::GRow* l_pRow = m_pDesignTypeCbo->SelectedRow();
   if(l_pRow)
      return (EUnitType::Enum)l_pRow->m_iAdditionalId;
   else
      return (EUnitType::Enum)-1;   
}

void GCreateDesignWindow::DisplayInitialUnitModel(EUnitType::Enum in_eType)
{
   m_SelectedDisplay.ColorIndex = 7;
   m_SelectedDisplay.PieceIndex[0] = 0;
   m_SelectedDisplay.PieceIndex[1] = 0;
   m_SelectedDisplay.PieceIndex[2] = 0;
   m_pUnitScene->DisplayUnit(ActuallySelectedUnitType(), m_SelectedDisplay);
}

void GCreateDesignWindow::OnUnitTypeChange(EUnitType::Enum in_eNewType)
{
   // Release old textures
   ReleaseUnitParts();

   if(in_eNewType >= 0)
   {
      m_iCurrentNbPieces = m_pUnitParts->LoadUnitPictures(in_eNewType, m_UnitPartPictures);
   }
   m_iCurrentPiece = 0;
   m_iCurrentVariation = 0; 

   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      m_pTechnologyMeters[i]->Value(0 , 0);
   }

   REAL32 l_fUnitCost = g_ClientDCL.CalculateUnitCost(ActuallySelectedUnitType(),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Sensors]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileRange]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0),
                                                      (UINT8)m_pTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value(0));

   m_pObjCostTxt->Text(GString::FormatNumber(l_fUnitCost,L" ",L".",L"",L" $",3,0));

   UINT32 l_iDescStrId = 0;
   switch(in_eNewType)
   {         
      case EUnitType::InfantryVehicle:
         l_iDescStrId = EStrId::DescriptionInfantryVehicle;
         break;
      case EUnitType::AirDefense:
         l_iDescStrId = EStrId::DescriptionAirDefense;
         break;
      case EUnitType::MobileLauncher:
         l_iDescStrId = EStrId::DescriptionMobileLauncher;
         break;
      case EUnitType::Tank:
         l_iDescStrId = EStrId::DescriptionTank;
         break;
      case EUnitType::ArtilleryGun:
         l_iDescStrId = EStrId::DescriptionArtilleryGun;
         break;
      case EUnitType::AttackHelicopter:
         l_iDescStrId = EStrId::DescriptionAttackHelicopter;
         break;
      case EUnitType::TransportHelicopter:
         l_iDescStrId = EStrId::DescriptionTransportHelicopter;
         break;
      case EUnitType::ASWHelicopter:
         l_iDescStrId = EStrId::DescriptionASWHelicopter;
         break;
      case EUnitType::FighterAircraft:
         l_iDescStrId = EStrId::DescriptionFighterAircraft;
         break;
      case EUnitType::AttackAircraft:
         l_iDescStrId = EStrId::DescriptionAttackAircraft;
         break;
      case EUnitType::Bomber:
         l_iDescStrId = EStrId::DescriptionBomber;
         break;
      case EUnitType::PatrolCraft:
         l_iDescStrId = EStrId::DescriptionPatrolCraft;
         break;
      case EUnitType::Corvette:
         l_iDescStrId = EStrId::DescriptionCorvette;
         break;
      case EUnitType::Frigate:
         l_iDescStrId = EStrId::DescriptionFrigate;
         break;
      case EUnitType::Destroyer:
         l_iDescStrId = EStrId::DescriptionDestroyer;
         break;
      case EUnitType::AttackSubmarine:
         l_iDescStrId = EStrId::DescriptionAttackSubmarine;
         break;
      case EUnitType::BallisticMissileSubmarine:
         l_iDescStrId = EStrId::DescriptionBallisticMissileSubmarine;
         break;
      case EUnitType::AircraftCarrier:
         l_iDescStrId = EStrId::DescriptionAircraftCarrier;
         break;
      case EUnitType::BallisticMissile:
         l_iDescStrId = EStrId::DescriptionBallisticMissile;
         break;
      case EUnitType::Infantry:
      default:
         gassert(0,"Unhandled case");
   }
   GString l_sUnitTypeDesc = g_ClientDAL.GetString(l_iDescStrId);
   m_pObjDescTxt->Text(l_sUnitTypeDesc);  
   m_pObjDescTxt->Height( m_pObjDescTxt->Text2D()->Height() );
   m_pObjDescScr->Initialize( this, 0, max(0, m_pObjDescTxt->Height() - m_pObjDescTxt->Owner()->Height()), 15, 45, 0 );

   UpdateUnitParts();
   UpdateLevelFilter();
   DisplayInitialUnitModel(in_eNewType);   
   FillPieceTypeCombo(in_eNewType);
}

/*!
 * Fill the piece type combo box (cannon, hull, turrets ...)
 **/
void GCreateDesignWindow::FillPieceTypeCombo(EUnitType::Enum in_eType)
{
   m_pObjPieceTypeCbo->Clear();

   //List the types of pieces
   UINT32 l_iPieceCount = 0;
   for(UINT32 i = 0 ; i < c_iMaxDesignPieceCount ; i++)
   {
      GDesignVariationHolder::GType::GPiece* l_pPiece = g_ClientDAL.DesignVariationHolder.Type(in_eType)->Pieces[i];
      if(l_pPiece)
      {
         m_pObjPieceTypeCbo->Add_Item(l_pPiece->Name,i);      
         l_iPieceCount++;
      }
      else
         break;
   }

   //If there is just one piece, disable the piece selection combo
   m_pObjPieceTypeCbo->Enabled(l_iPieceCount > 1);
   m_pObjPieceTypeCbo->DropSize(l_iPieceCount * 14);
   m_pObjPieceTypeCbo->Selected_Content(m_iCurrentPiece);
}

void GCreateDesignWindow::OnUnitPieceChange(UINT32 in_iNewPieceId)
{
   m_iCurrentPiece = in_iNewPieceId;
   m_iCurrentVariation = 0;

   UpdateUnitParts();
}