/**************************************************************
*
* sp2_design_window2.cpp
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
#include "../include/sp2_design_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_create_design_window.h"
using namespace SP2;

const GString GDesignWindow::TypeName(L"Unit_Design_New");

GUI::GBaseObject* GDesignWindow::New()
{
   return new GDesignWindow();
}

GDesignWindow::GDesignWindow()
{
   m_pObjUnitTypeCbo       = NULL;
   m_pObjUnitNameCbo       = NULL;
   m_pObjDescTxtLbl        = NULL;
   m_pObjTabbed            = NULL;
   m_pObjTab1Container     = NULL;
   m_pUnitSceneViewPort    = NULL;
   m_pSelectedUnitDesign   = NULL;
   m_pObjCreateDesignBtn   = NULL;
   m_bDragView             = false;
   memset(m_pObjTechnologyMeters,0,4*EUnitDesignCharacteristics::ItemCount);
}

GDesignWindow::~GDesignWindow()
{
}

/*!
 * Creation method
 **/
bool GDesignWindow::OnCreate()
{
   __super::OnCreate();

   //Fetch the pointer to the objects
   {
      SDK::GUI::GFrame*   l_pInnerFrame = (GUI::GFrame*)Child(L"frmInner");
      SDK::GUI::GFrame* l_pUnitInfoFrame =(GUI::GFrame*)l_pInnerFrame->Child(L"frmUnitInfo");
      m_pObjUnitTypeCbo   = (GUI::GComboBox*)l_pUnitInfoFrame->Child(L"cboUnitType");
      m_pObjUnitNameCbo   = (GUI::GComboBox*)l_pUnitInfoFrame->Child(L"cboUnitName");
      m_pObjDescTxtLbl    = (GUI::GLabel*)   l_pUnitInfoFrame->Child(L"frmDescription")->Child(L"txtText");
      m_pObjTabbed        = (GUI::GTabbed*)  l_pInnerFrame->Child(L"tabGroups");
      m_pObjUnitCostLbl   = (GUI::GLabel*)   l_pInnerFrame->Child(L"frmUnitInfo1")->Child(L"txtCost");

      m_pObjCreateDesignBtn = (GUI::GButton*)l_pInnerFrame->Child(L"btnNewDesign");

      gassert(m_pObjTabbed->m_vTabPage.size() == 2,L"should be 2 pages, model and stats");
      m_pObjTab1Container     = (GUI::GUIContainer*) m_pObjTabbed->m_vTabPage[0]->Child(L"Container1"); 
      GUI::GFrame* l_pFrmList   = (GUI::GFrame*) m_pObjTabbed->m_vTabPage[1]->Child(L"frmList");
      GUI::GFrame* l_pFrmSpecs  = (GUI::GFrame*) l_pFrmList->Child(L"frmSpecs");

      m_pObjSpecsScr = (GUI::GVScrollBar*)m_pObjTabbed->m_vTabPage[1]->Child(L"scrList");
      m_pObjDescScr  = (GUI::GVScrollBar*)l_pInnerFrame->Child(L"frmUnitInfo")->Child(L"frmDescription")->Child(L"scrText");

      m_pObjSpecsScr->Initialize( this, 0, max(0, l_pFrmSpecs->Height() - l_pFrmList->Height()), 15, 45, 0 );
      m_pObjDescScr->Initialize( this, 0, max(0, m_pObjDescTxtLbl->Height() - m_pObjDescTxtLbl->Owner()->Height()), 15, 45, 0 );
      ((GFX::GWindow*)m_pObjDescTxtLbl->Owner()->Model()->Get_Primitive())->ClipChildren(true);

      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]        = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmGunRange");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]    = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmGunPrecision");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]       = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmGunDamage");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]    = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmMissileRange");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]= (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmMissilePrecision");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]   = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmMissileDamage");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]  = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmMissilePayload");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]           = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmSpeed");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]         = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmStealth");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors]         = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmSensors");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]  = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmCountermeasures");
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]           = (GTechnologyMeterBlue*)l_pFrmSpecs->Child(L"frmArmor");
      
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]            ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]         ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]     ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]        ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]            ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors]          ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]   ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]            ->Enabled(false);

      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]        = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblGunRange");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision]    = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblGunPrecision");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]       = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblGunDamage");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileRange]    = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblMissileRange");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePrecision]= (GUI::GLabel*)l_pFrmSpecs->Child(L"lblMissilePrecision");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileDamage]   = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblMissileDamage");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePayload]  = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblMissilePayload");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Speed]           = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblSpeed");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]         = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblStealth");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Sensors]         = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblSensors");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::CounterMesures]  = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblCountermeasures");
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Armor]           = (GUI::GLabel*)l_pFrmSpecs->Child(L"lblArmor");

      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Armor]            ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]         ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision]     ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]        ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Speed]            ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Sensors]          ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::CounterMesures]   ->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Armor]            ->Enabled(false);

      //Initialize the scrollbar
      ((GFX::GWindow*)l_pFrmList->Model()->Get_Primitive())->ClipChildren(true);

      if(m_pObjApplyBtn)
         m_pObjApplyBtn->Hide();
   }

   //Set the unit view scene
   {
      m_pUnitSceneViewPort = g_Joshua.Renderer()->Create_Viewport();
      m_pUnitSceneViewPort->Set_Scene(m_UnitViewScene.Scene());
      m_pObjTab1Container->Model()->Set_Primitive(m_pUnitSceneViewPort);
      m_UnitViewScene.CameraControl().SetPerspectiveFromViewportSize(m_pObjTab1Container->Size());
   }

   //Remove the magenta
   m_pObjTabbed->m_vTabPage[0]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjTabbed->m_vTabPage[1]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjTabbed->m_vTabPage[0]->PictureNormal(GString(L"000647"));
   m_pObjTabbed->m_vTabPage[1]->PictureNormal(GString(L"000647"));

   return true;
}

/*!
 * Unit Design Destroy handler
 **/
bool GDesignWindow::OnDestroy()
{
   SAFE_RELEASE(m_pUnitSceneViewPort);
   __super::OnDestroy();
   return true;
}
 
/*!
 * Init the display of the window
 **/
void GDesignWindow::InitDisplayedData()
{
   m_pObjUnitTypeCbo->Clear();
   //Fill the unit type combo with the correct values
   const vector<GUnitType*>& l_vUnitTypes = g_ClientDAL.UnitTypes();
   for(UINT32 i = 1 ; i < l_vUnitTypes.size() ; i++)
   {
      //Add the item in the combo box
      m_pObjUnitTypeCbo->Add_Item(l_vUnitTypes[i]->Name());
      //Set the ID of the item in the combo box to be the ID of the unit type
      m_pObjUnitTypeCbo->GetListBox()->Get_Row_At(i-1)->m_iAdditionalId = i+1;
   }
   
   //Set the initial content with the 1st unit for the country   
   if(g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID()).size())
   {
      bool l_bAllSet = false;

      //Find the 1st non infantry unit and display it
      for(set<UINT32>::const_iterator l_UnitGroupIt = g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID()).begin();
                                      l_UnitGroupIt != g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID()).end();
                                      l_UnitGroupIt++)
      {
         //For all the units in the group
         SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIt);
         for(UINT32 i = 0 ; i < l_pGroup->Units().size() ; i++)
         {
            SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pGroup->Units()[i]->Design();
            //If the design is not infantry
            if(l_pDesign->Type()->Id() != EUnitType::Infantry)
            {
               FillNamesComboBox((EUnitType::Enum)l_pDesign->Type()->Id());
               UpdateAvailableSpecs((EUnitType::Enum)l_pDesign->Type()->Id());
               SelectedDesign(l_pDesign);    
               l_bAllSet = true;
               break;
            }
         }
         if(l_bAllSet == true)
            break;
      }
   }
}

void GDesignWindow::OnShow()
{
   __super::OnShow();
   // Init our window displayed data
   InitDisplayedData();
   // be notify when new design
   g_Joshua.UnitManager().Attach(this, SDK::Combat::c_notificationNewUnitDesign);
}

void GDesignWindow::OnHide()
{
   // stop being notify when new design
   g_Joshua.UnitManager().Detach(this, SDK::Combat::c_notificationNewUnitDesign);
   return __super::OnHide();
}

void GDesignWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   // On new design
   if(in_Notification == SDK::Combat::c_notificationNewUnitDesign)
   {
      // retrieve ou current row
      GUI::GListBox::GRow* l_pRow = m_pObjUnitTypeCbo->SelectedRow();
      // valid row ?
      if(l_pRow)
      {
         // Init our window displayed data
         FillNamesComboBox((EUnitType::Enum)l_pRow->m_iAdditionalId);
         // refresh
         m_pObjUnitNameCbo->Update();
      }
   }
}

/*!
 * Fills the combo box that gives the name of the units
 **/
void GDesignWindow::FillNamesComboBox(EUnitType::Enum in_eType)
{
   //Display the text description for the unit type
   {
      UINT32 l_iDescStrId = 0;
      switch(in_eType)
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
      m_pObjDescTxtLbl->Text(l_sUnitTypeDesc);  
      m_pObjDescTxtLbl->Height( m_pObjDescTxtLbl->Text2D()->Height() );
      m_pObjDescScr->Initialize( this, 0, max(0, m_pObjDescTxtLbl->Height() - m_pObjDescTxtLbl->Owner()->Height()), 15, 45, 0 );
   }

   UINT32 l_iRowId = 0;

   //Clear the combo names
   m_pObjUnitNameCbo->Clear();
   //Fetch all the unit designs for the country that correspond to that type and add them
   //to the combo box
   const stdext::hash_map<UINT32, SDK::Combat::Design::GUnit*>& l_Designs = g_Joshua.UnitManager().UnitDesigns();
   for(hash_map<UINT32, SDK::Combat::Design::GUnit*>::const_iterator l_It = l_Designs.begin();
       l_It != l_Designs.end();
       l_It++)
   {
      SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_It->second;
      //If the design is for everybody or for us
      if(l_pDesign->DesignerID() == 0 || l_pDesign->DesignerID() == (UINT32)g_ClientDAL.ControlledCountryID())
      {
         //If the design is of the wanted type
         if(l_pDesign->Type()->Id() == (UINT32)in_eType)
         {
            m_pObjUnitNameCbo->Add_Item(l_pDesign->Name());
            //Set the additional ID on the item
            m_pObjUnitNameCbo->GetListBox()->Get_Row_At(l_iRowId++)->m_iAdditionalId = l_pDesign->Id();
         }
      }
   }

   if( !m_pObjUnitNameCbo->NbItems() )
   {
      m_pObjUnitCostLbl->Text(L"");
      m_UnitViewScene.HideUnit();
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]           ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]        ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]    ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]       ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]    ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]   ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]  ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]           ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]         ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors]         ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]  ->Value(0,0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]           ->Value(0,0,false,false);
   }
   else
   {
      m_pObjUnitNameCbo->Selected_Content(0, false);
      GUI::GListBox::GRow* l_pRow = m_pObjUnitNameCbo->SelectedRow();
      SelectedDesign((const SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pRow->m_iAdditionalId));
   }

   m_pObjUnitNameCbo->DropSize( (m_pObjUnitNameCbo->NbItems() * 14) );
   if( m_pObjUnitNameCbo->DropSize() )
   {
      m_pObjUnitNameCbo->DropSize( m_pObjUnitNameCbo->DropSize() + 2 );

      if( m_pObjUnitNameCbo->DropSize() > 100 )
         m_pObjUnitNameCbo->DropSize( 100 );
   }
   else
   {
      m_pObjUnitNameCbo->DropSize( 16 );
   }
}

void GDesignWindow::UpdateAvailableSpecs(EUnitType::Enum in_eActuallySelectedUnitType)
{
   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      //m_pObjTechnologyMeters[i]->Enabled(true);
		m_pObjTechnologyLbl[i]->Enabled(true);
   }

   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      REAL32 l_fMaxValue = g_ClientDAL.m_PlayerCountryData.ResearchInfo()->m_fMaxValues[c_eUnitCategoryByType[in_eActuallySelectedUnitType]][i];
      m_pObjTechnologyMeters[i]->Maximum()[0]         = floor(l_fMaxValue);
      m_pObjTechnologyMeters[i]->Maximum()[1]         = floor(l_fMaxValue);
      m_pObjTechnologyMeters[i]->RightBound             ((INT16)l_fMaxValue * 8 );
      m_pObjTechnologyMeters[i]->Value                  (l_fMaxValue,       1 );
   }


   switch(c_eUnitCategoryByType[in_eActuallySelectedUnitType])
   {
   case SP2::EUnitCategory::Ground:      
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Maximum()[0]  = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum()[0]  = 0;		
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Maximum()[1]  = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum()[1]  = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->RightBound(0);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->RightBound(0);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Value(0 , 1);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(0 , 1);
      /*
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
		m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]			->Enabled(false);
		m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
*/
      switch(in_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::MobileLauncher:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
/*
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         */
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      case SP2::EUnitType::Tank:
      case SP2::EUnitType::ArtilleryGun:
      case SP2::EUnitType::InfantryVehicle:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Value(0 , 1);
/*
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
*/
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
         break;
      }

      break;
   
   case SP2::EUnitCategory::Air:
      switch(in_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::TransportHelicopter:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileRange]     ->Enabled(false);

         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);

         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);

         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePayload]   ->Enabled(false);

         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         break;
      case SP2::EUnitType::Bomber:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      }
      break;
   case SP2::EUnitCategory::Naval:
      switch(in_eActuallySelectedUnitType)
      {
      case SP2::EUnitType::ASWHelicopter:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         break;
      case SP2::EUnitType::PatrolCraft:
      case SP2::EUnitType::Corvette:
      case SP2::EUnitType::Frigate:
      case SP2::EUnitType::Destroyer:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         break;
      case SP2::EUnitType::AttackSubmarine:
      case SP2::EUnitType::BallisticMissileSubmarine:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         /*
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         */
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         break;
      case SP2::EUnitType::AircraftCarrier:
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Value(0 , 1);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Value(0 , 1);
         /*
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         */
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]    ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision] ->Enabled(false);
         m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]     ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]          ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissileDamage]    ->Enabled(false);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[0]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Maximum()[1]  = 0;
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->RightBound(0);
         m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Value(0 , 1);
         //m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
			m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePrecision] ->Enabled(false);
         break;
      }
      break;
  
   case SP2::EUnitCategory::Nuclear:
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Maximum()       [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Maximum()   [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Maximum()      [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Maximum() [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Maximum()        [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum() [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]->Maximum()          [0]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]->Maximum()          [0]     = 0;

      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Maximum()       [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Maximum()   [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Maximum()      [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Maximum() [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Maximum()        [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Maximum() [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]->Maximum()          [1]     = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]->Maximum()          [1]     = 0;

      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]->RightBound( 0 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]->RightBound( 0 );

      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]->Value( 0, 1 );
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]->Value( 0, 1 );
/*
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]->Enabled(false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]->Enabled(false);
*/
		m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunRange]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunPrecision]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::GunDamage]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::MissilePayload]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Stealth]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::CounterMesures]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Armor]->Enabled(false);
      m_pObjTechnologyLbl[EUnitDesignCharacteristics::Speed]->Enabled(false);

      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Maximum()[0]  = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Maximum()[1]  = 0;
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->RightBound(0);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Value(0 , 1);
      //m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors] ->Enabled(false);
		m_pObjTechnologyLbl[EUnitDesignCharacteristics::Sensors] ->Enabled(false);

      break;
   }
}


/*!
 * Set the selected design
 **/
void GDesignWindow::SelectedDesign(const SP2::GUnitDesign* in_pDesign)
{
   // reassign if different unit
   if(m_pSelectedUnitDesign != in_pDesign)
   {
      m_pSelectedUnitDesign = in_pDesign;
      FillNamesComboBox((EUnitType::Enum)in_pDesign->Type()->Id()); 
      UpdateAvailableSpecs((EUnitType::Enum)in_pDesign->Type()->Id());
      // select the type
      m_pObjUnitTypeCbo->Selected_Content(in_pDesign->Type()->Name());
   }
   // Select the new name
   m_pObjUnitNameCbo->Selected_Content(in_pDesign->Name()); 

   //Set the unit viewScene
   m_UnitViewScene.DisplayUnit(in_pDesign->Type()->Id(),in_pDesign->m_DesignVariations);   

   //Set the meters with the correct dots
   {
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]         ->Value(in_pDesign->Armor(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunRange]      ->Value(in_pDesign->GunRange(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunPrecision]  ->Value(in_pDesign->GunPrecision(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::GunDamage]     ->Value(in_pDesign->GunDamage(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileRange]  ->Value(in_pDesign->MissileRange(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePrecision]->Value(in_pDesign->MissilePrecision(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissileDamage] ->Value(in_pDesign->MissileDamage(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::MissilePayload]->Value(in_pDesign->MissilePayload(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Speed]         ->Value(in_pDesign->SpeedAndManeuverability(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Stealth]       ->Value(in_pDesign->Stealth(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Sensors]       ->Value(in_pDesign->Sensors(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::CounterMesures]->Value(in_pDesign->CounterMesures(),0,false,false);
      m_pObjTechnologyMeters[EUnitDesignCharacteristics::Armor]         ->Value(in_pDesign->Armor(),0,false,false);
   }

   //Display the unit cost
   m_pObjUnitCostLbl->Text(GString::FormatNumber(in_pDesign->Cost(),L" ",L".",L"",L" $",3,0));
}


const SP2::GUnitDesign*    GDesignWindow::SelectedDesign() const
{
   return m_pSelectedUnitDesign;
}


GUI::EEventStatus::Enum GDesignWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         if(in_EventData.Data == m_pObjUnitTypeCbo)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjUnitTypeCbo->SelectedRow();
            FillNamesComboBox((EUnitType::Enum)l_pRow->m_iAdditionalId);
            UpdateAvailableSpecs((EUnitType::Enum)l_pRow->m_iAdditionalId);
         }
         if(in_EventData.Data == m_pObjUnitNameCbo)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjUnitNameCbo->SelectedRow();
            SelectedDesign((const SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pRow->m_iAdditionalId));
         }
         break;
      }
      case ECustomEvents::OnScrollVertical:
      {
         if( in_pCaller == m_pObjDescScr )
         {
            m_pObjDescTxtLbl->Top(-((INT16)in_EventData.Data));
         }
         else if( in_pCaller == m_pObjSpecsScr )
         {
            GUI::GFrame* l_pFrmList   = (GUI::GFrame*) m_pObjTabbed->m_vTabPage[1]->Child(L"frmList");
            GUI::GFrame* l_pFrmSpecs  = (GUI::GFrame*) l_pFrmList->Child(L"frmSpecs");
            l_pFrmSpecs->Top(-((INT16)in_EventData.Data));
            break;
         }
      }
      default:
      {
         break;
      }
      
   }
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GDesignWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   //If the create new design button was pressed
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjCreateDesignBtn)
      {
         if(!g_ClientDDL.CreateDesignWindow())
         {
            g_ClientDDL.CreateDesignWindowSpawn();
         }
         if(!g_ClientDDL.CreateDesignWindow()->Visible())
         {
            g_ClientDDL.CreateDesignWindow()->Show();
         }
         return GUI::EEventStatus::Handled;
      }
   }

   return __super::OnMouseClick(in_EventData,in_pCaller);
}

void GDesignWindow::OnDrag(const GEventData& in_EventData, const GVector2D<INT32>& in_MouseMove, const GVector2D<INT32>& in_3rdParam)
{

   if(m_bDragView)
   {
      m_UnitViewScene.CameraControl().OffsetOrientation( (REAL32) in_MouseMove.x * -0.025f, (REAL32) in_MouseMove.y * 0.025f);
   }
   else
   {
      return __super::OnDrag(in_EventData,in_MouseMove,in_3rdParam);
   }
}

GUI::EEventStatus::Enum GDesignWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   m_bDragView = false;
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if( in_EventData.InitialRecipient == m_pObjTab1Container)  
      {
         if(in_EventData.Mouse.Down.Bits.Left)
         {
            m_bDragView = true;
            Drag();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}


GUI::EEventStatus::Enum GDesignWindow::OnMouseWheel(const GEventData& in_Data, GBaseObject*)
{
   if(in_Data.InitialRecipient == m_pObjTab1Container)
   {
      m_UnitViewScene.CameraControl().OffsetDistanceRelative( (REAL32) in_Data.Mouse.WheelDelta * -0.1f);
   }
   else if(in_Data.InitialRecipient == m_pObjDescTxtLbl)
   {
      m_pObjDescScr->OnMouseWheel(in_Data, NULL); 
   }
   else if(m_pObjTabbed->ActiveTab() == 1)
   {
      m_UnitViewScene.CameraControl().OffsetDistanceRelative( (REAL32) in_Data.Mouse.WheelDelta * -0.1f);
   }
   else if(m_pObjTabbed->ActiveTab() == 2)
   {
      m_pObjSpecsScr->OnMouseWheel(in_Data, NULL); 
   }

   return GUI::EEventStatus::Handled;
}