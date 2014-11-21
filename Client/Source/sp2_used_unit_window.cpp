 /**************************************************************
*
* sp2_used_unit_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes
#include "golem_pch.hpp"
#include "../include/sp2_used_unit_window.h"
#include "../include/sp2_gui_helper.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_unit_view_scene.h"

// Constants
const GString GUsedUnitWindow::TypeName(L"Unit_Buy_Used_Window");

// Constructors and Destructors
GUI::GBaseObject* GUsedUnitWindow::New()
{
   return new GUsedUnitWindow();
}

GUsedUnitWindow::GUsedUnitWindow() : SP2::GBaseWindow()
{
   m_iSelectedUnitID = -1;
   m_iLastSelectedRowID = -1;
   m_bDragView             = false;
}

GUsedUnitWindow::~GUsedUnitWindow()
{
   SAFE_DELETE(m_pUnitScene);
}

bool GUsedUnitWindow::OnCreate()
{
   __super::OnCreate();

   Child(L"frmDesignView")->BringToFront();

   // Collect gui items
   m_pObjExitBtn             = (GUI::GButton*) Child("frmBorder")->Child("frmTitle")->Child("btnExit");
   //m_pObjApplyBtn->Hide();

   m_pObjInnerFrm             = (GUI::GFrame*)Child(L"frmBorder")->Child("frmInner");

   SDK::GUI::GBaseObject* l_pUnitInfo = Child("frmBorder")->Child("frmInner")->Child("frmDesignList");
   m_pObjBuyBtn               = (GUI::GButton*)  l_pUnitInfo->Child("btnBuy");
   m_pObjProducerLabel        = (GUI::GLabel*) l_pUnitInfo->Child("edtUnitName");
   m_pObjUnitCostLabel        = (GUI::GLabel*) l_pUnitInfo->Child("edtUnitName1");
   m_pObjTotalUnitCostLabel   = (GUI::GLabel*) l_pUnitInfo->Child("edtUnitName2");
   m_pObjQuantity             = (SP2::GUpdatableValue*) l_pUnitInfo->Child("frmUpdatableValue1");
   m_pObjQuantity->TextValueObj()->TextAlignment(L"MiddleRight");

   m_pObjDesignFrm            = (GUI::GFrame*)Child("frmBorder")->Child("frmInner")->Child(L"frmDesignList")->Child(L"frmList");
   m_pObjDesignScr            = (GUI::GVScrollBar*)Child(L"frmDesignView")->Child(L"frmUnitSpecs")->Child(L"scrList");
   m_pObjSpecListFrm          = (GUI::GFrame*)Child(L"frmDesignView")->Child(L"frmUnitSpecs")->Child(L"frmList");
   m_pUnitTypeCbo             = (GUI::GComboBox*)Child("frmBorder")->Child("frmInner")->Child(L"frmDesignList")->Child(L"cboUnitType");
   m_pUnitListLst             = (GUI::GListBox*)Child("frmBorder")->Child("frmInner")->Child(L"frmDesignList")->Child(L"lstUnits");

   // Design info part
   m_pObjDesignNameLabel          = (GUI::GLabel*)( Child("frmDesignView")->Child("frmUnitName")->Child("edtUnitName"));
   GUI::GFrame* l_pObjSpecListFrm = (GUI::GFrame*)Child("frmDesignView")->Child(L"frmUnitSpecs")->Child(L"frmList")->Child(L"frmInner");
   m_pObjGunRangeMeter            = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmGunRange");
   m_pObjGunPrecisionMeter        = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmGunPrecision");
   m_pObjGunDamageMeter           = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmGunDamage");
   m_pObjMissileRangeMeter        = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmMissileRange");
   m_pObjMissileDamageMeter       = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmMissileDamage");
   m_pObjMissilePayloadMeter      = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmMissilePayload");
   m_pObjMissilePrecisionMeter    = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmMissilePrecision");
   m_pObjSpeedMeter               = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmSpeed");
   m_pObjSensorsMeter             = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmSensors");
   m_pObjArmorMeter               = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmArmor");
   m_pObjStealthMeter             = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmStealth");
   m_pObjCountermeasuresMeter     = (SP2::GTechnologyMeterBlue*)l_pObjSpecListFrm->Child(L"frmCountermeasures");

   ((GUI::GPictureBox*)Child("frmDesignView")->Child(L"picBg"))->SendToBack();

   m_pObjGunRangeMeter->Maximum()[1]         = 10;
   m_pObjGunPrecisionMeter->Maximum()[1]     = 10;
   m_pObjGunDamageMeter->Maximum()[1]        = 10;
   m_pObjMissileRangeMeter->Maximum()[1]     = 10;
   m_pObjMissileDamageMeter->Maximum()[1]    = 10;
   m_pObjMissilePrecisionMeter->Maximum()[1] = 10;
   m_pObjMissilePayloadMeter->Maximum()[1]   = 10;
   m_pObjSpeedMeter->Maximum()[1]            = 10;
   m_pObjStealthMeter->Maximum()[1]          = 10;
   m_pObjCountermeasuresMeter->Maximum()[1]  = 10;
   m_pObjArmorMeter->Maximum()[1]            = 10;
   m_pObjSensorsMeter->Maximum()[1]          = 10;
   EnableMeters(false);

   ((GFX::GWindow*)m_pObjSpecListFrm->Model()->Get_Primitive())->ClipChildren(true);

   vector<UINT32> m_viStep;
   vector<REAL64> m_vfStep;
   m_viStep.push_back(1);
   m_viStep.push_back(9);
   m_viStep.push_back(99);
   m_vfStep.push_back(1);
   m_vfStep.push_back(10);
   m_vfStep.push_back(100);
   m_pObjQuantity->Initialize(m_viStep, m_vfStep, L"", L"", 0, 3, 0, 1000000);
   m_pObjDesignScr->Initialize(this, 0, max(0, l_pObjSpecListFrm->Height() - m_pObjSpecListFrm->Height()), 15, 45, 0);

   // Setup the unit design display
   m_pUnitScene = new GUnitViewScene;
   m_pModelFrm  = (GUI::GUIContainer*)Child("frmDesignView")->Child(L"frm3dModel")->Child(L"Model");
   m_pUnitScene->ChangeAspectRatio( (REAL32) m_pModelFrm->Width(), (REAL32) m_pModelFrm->Height() );

   CGFX_Viewport_Itf* l_pViewport = g_Joshua.Renderer()->Create_Viewport();
   l_pViewport->Set_Scene(m_pUnitScene->Scene() );
   m_pModelFrm->Model()->Set_Primitive(l_pViewport);

   m_bFirstTimeToDisplayUnit = true;

   m_bRequestedUsedUnit = false;

   return true;
}


void GUsedUnitWindow::GenerateUnitDesignList(list<UINT32>& in_ListOfUnitForSale)
{
   // clear Design list
   m_UnitDesignList.clear();

	UINT32 l_iControlledCountry = (UINT32)g_ClientDAL.ControlledCountryID();
	const UINT8* l_pStatuses = g_ClientDAL.CountryDiplomaticStatuses(l_iControlledCountry);

   // Iterate over unit that can be sold
   list<UINT32>::iterator l_UnitIterator = in_ListOfUnitForSale.begin();
   while(l_UnitIterator != in_ListOfUnitForSale.end())
   {
      SP2::GUnit* l_pUnit                 = (SP2::GUnit*)g_Joshua.UnitManager().Unit(*l_UnitIterator);
      gassert(l_pUnit,"Unit should never be NULL");		

      if(l_pUnit)
      {
			if((EDiplomaticStatus::Enum)l_pStatuses[l_pUnit->Group()->OwnerId()] == EDiplomaticStatus::Hostile)
			{
				l_UnitIterator++;
				continue;
			}
			else if(g_ClientDAL.WeaponTradeEmbargo(l_pUnit->Group()->OwnerId(),l_iControlledCountry))
			{
				l_UnitIterator++;
				continue;
			}

         SP2::GUnitDesign* l_pUnitDesign  = (SP2::GUnitDesign*)l_pUnit->Design();
         UINT32 l_iTypeId                 = l_pUnitDesign->Type()->Id();

         m_UnitDesignList.insert(multimap<UINT32,UINT32>::value_type(l_iTypeId,*l_UnitIterator));
      }
      l_UnitIterator++;
   }
}

void GUsedUnitWindow::BuildUnitDesignCbo(UINT32 in_iLastSelectedUnitType)
{
   // keep the last id
   INT32 l_iLastId = -1;

   // empty the cbo first
   m_pUnitTypeCbo->Clear();

	UINT32 l_iControlledCountry = (UINT32)g_ClientDAL.ControlledCountryID();
	const UINT8* l_pStatuses = g_ClientDAL.CountryDiplomaticStatuses(l_iControlledCountry);

   // Feed the combobox from the multimap
   for(multimap<UINT32,UINT32>::iterator l_ItUnit = m_UnitDesignList.begin(); 
      l_ItUnit != m_UnitDesignList.end();
      l_ItUnit++)
   {
      // references to the unit and the unit desgin informations
      INT32             l_iUnitTypeId = (INT32)l_ItUnit->first;
      SP2::GUnit*       l_pUnit       = (SP2::GUnit*)g_Joshua.UnitManager().Unit(l_ItUnit->second);
      gassert(l_pUnit,"Unit should never be NULL");     		
      
      if(l_pUnit)
      {
			if((EDiplomaticStatus::Enum)l_pStatuses[l_pUnit->Group()->OwnerId()] == EDiplomaticStatus::Hostile)
				continue;
			if(g_ClientDAL.WeaponTradeEmbargo(l_pUnit->Group()->OwnerId(),l_iControlledCountry))
				continue;

         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_pUnit->Design();

         // id already added ?
         if(l_iUnitTypeId != l_iLastId)
         {
            // if not, add it into the cbo
            m_pUnitTypeCbo->Add_Item(l_pUnitDesign->Type()->Name(), l_iUnitTypeId);
            l_iLastId = l_iUnitTypeId;
         }
      }
   }

   // check if we have something in it
   if(m_pUnitTypeCbo->NbItems() > 0)
   {
      // last selected unit type still valid ?
      if(m_pUnitTypeCbo->NbItems() <= in_iLastSelectedUnitType)
      {
         in_iLastSelectedUnitType = 0;
      } 

      // select the first item by default if yes
      m_pUnitTypeCbo->Enabled(true);
      m_pUnitTypeCbo->Selected_Content(in_iLastSelectedUnitType, false);

      // Update the List
      // retrieve our row 
      GUI::GListBox::GRow* l_pSelectedUnit = m_pUnitTypeCbo->SelectedRow();
      INT32 l_iUnitTypeId  = (l_pSelectedUnit != NULL ? l_pSelectedUnit->m_iAdditionalId : -1);
      // feed our list box
      BuildUnitDesignLstFromId(l_iUnitTypeId);


      if(m_pUnitTypeCbo->NbItems() >= 10)
         m_pUnitTypeCbo->DropSize( 142 );
      else
         m_pUnitTypeCbo->DropSize( (m_pUnitTypeCbo->NbItems() * 14) + 2 );
   }
   else
   {
      // no need to activate it, no unit !
      m_pUnitTypeCbo->Enabled(false);
      BuildUnitDesignLstFromId(-1);
   }

   // display it properly
   m_pUnitTypeCbo->Update();
}

void GUsedUnitWindow::BuildUnitDesignLstFromId(INT32 in_iUnitId)
{
   // Use old listbox item
   INT32 l_iListBoxItemCount = 0;
   
	UINT32 l_iControlledCountry = (UINT32)g_ClientDAL.ControlledCountryID();
	const UINT8* l_pStatuses = g_ClientDAL.CountryDiplomaticStatuses(l_iControlledCountry);

   // Feed the listbox from the multimap
   for(multimap<UINT32,UINT32>::iterator l_ItUnit = m_UnitDesignList.lower_bound(in_iUnitId); 
      l_ItUnit != m_UnitDesignList.upper_bound(in_iUnitId);
      l_ItUnit++)
   {  
      // references to the unit and the unit desgin informations
      SP2::GUnit*       l_pUnit       = (SP2::GUnit*)g_Joshua.UnitManager().Unit(l_ItUnit->second);     		

		if(l_pUnit)
      {
			if((EDiplomaticStatus::Enum)l_pStatuses[l_pUnit->Group()->OwnerId()] == EDiplomaticStatus::Hostile)
				continue;
			if(g_ClientDAL.WeaponTradeEmbargo(l_pUnit->Group()->OwnerId(),l_iControlledCountry))
				continue;
         if(l_pUnit->Group()->OwnerId() == l_iControlledCountry)
            continue;

         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_pUnit->Design();

         // add into the listbox
         GUI::GListBox::GRow* l_pRow;
         if(l_iListBoxItemCount < m_pUnitListLst->Get_Nb_Rows())
         {
            l_pRow = m_pUnitListLst->Get_Row_At(l_iListBoxItemCount);
         }
         else
         {
            l_pRow = m_pUnitListLst->Insert_New_Row();
         }
         // Set additionnal ID
         l_pRow->m_iAdditionalId     = l_pUnit->Id();
         GString l_UnitInfo          = GString(l_pUnit->Qty()) + L" " + l_pUnitDesign->Name();
         ((GUI::GLabel*)l_pRow->Object())->FontSize(9);
         ((GUI::GLabel*)l_pRow->Object())->Text(l_UnitInfo);

         l_iListBoxItemCount++;
      }
   }


   // Remove highlight
   if(m_iLastSelectedRowID >= 0)
   {
        GUI::GListBox::GRow* l_pRow = m_pUnitListLst->Get_Row_At(m_iLastSelectedRowID);
        if(l_pRow)
         ((GUI::GFrame*)l_pRow->Object())->PictureNormal(GString(L"000587"));
   }


   while(l_iListBoxItemCount  <  m_pUnitListLst->Get_Nb_Rows())
   {
      m_pUnitListLst->RemoveRow(m_pUnitListLst->Get_Nb_Rows() - 1);
   }

   // update selection
   m_pUnitListLst->ClearSelection(); 

   if(m_iLastSelectedRowID >=  m_pUnitListLst->Get_Nb_Rows())
   {
      m_iLastSelectedRowID = m_pUnitListLst->Get_Nb_Rows()-1;
   }

   if(m_iLastSelectedRowID > 0)
   {
      GUI::GListBox::GRow* l_pRow = m_pUnitListLst->Get_Row_At(m_iLastSelectedRowID);
      ((GUI::GFrame*)l_pRow->Object())->PictureNormal(GString(L"000647"));
      UpdateSelectedUnit(l_pRow->m_iAdditionalId);
   }
   else
   {
      UpdateSelectedUnit(-1);
   }

   // display it properly
   m_pUnitListLst->Update();

}

void GUsedUnitWindow::OnShow()
{
   __super::OnShow();
   g_ClientDCL.RequestUsedUnit();
   m_bRequestedUsedUnit = true;
}

void GUsedUnitWindow::UpdateUnitsForSale(list<UINT32> in_ListOfUnitsForSale)
{
   // Don't update if we didn't request.
   if( m_bRequestedUsedUnit== false && Visible() == true && m_iSelectedUnitID > 0)
      return;

   m_bRequestedUsedUnit = false;

   // Update Unit List
   GenerateUnitDesignList(in_ListOfUnitsForSale);
   // if we had a last selection in our list
   UINT32 l_iLastSelectedUnitType = 0;
   // no pointer error please ...
   if(m_pUnitTypeCbo && m_pUnitTypeCbo->SelectedRow())
   {
      l_iLastSelectedUnitType = m_pUnitTypeCbo->SelectedRow()->m_iId;
   }
   BuildUnitDesignCbo(l_iLastSelectedUnitType);
}  


void GUsedUnitWindow::UpdateSelectedUnit(INT32 in_iUnitId)
{
   // are we selecting the same unit ?
   bool l_bSameAsLast = m_iSelectedUnitID == in_iUnitId;
   // new id
   m_iSelectedUnitID = in_iUnitId;
   GUnit*         l_pUnit =(GUnit*) g_Joshua.UnitManager().Unit(in_iUnitId);
   if(in_iUnitId > 0 && l_pUnit)
   {
      GUnitDesign*   l_pUnitDesign = (GUnitDesign*)l_pUnit->Design();
      UpdateDesignInfo(l_pUnitDesign);

      // Set country name
      m_pObjProducerLabel->Text(g_ClientDAL.Country(l_pUnit->Group()->OwnerId()).Name());

      // Set quanity
      m_pObjQuantity->Enabled(true);
      m_pObjQuantity->Minimum(1);
      m_pObjQuantity->Maximum(l_pUnit->Qty());
      // reset the value if we changed the selected unit
      if(!l_bSameAsLast)
      {
         m_pObjQuantity->Value(l_pUnit->Qty());
      }

      // Unit cost
      REAL32 l_fUnitCost = l_pUnitDesign->Cost();
      m_pObjUnitCostLabel->Text(GString::FormatNumber(l_fUnitCost, L" ", L".", L"", L"", 3, 0));

      // Total Cost
      REAL64 l_fTotalCost = m_pObjQuantity->Value() * l_fUnitCost * c_fUsedUnitsRebates;
      m_pObjTotalUnitCostLabel->Text(GString::FormatNumber(l_fTotalCost, L" ", L".", L"", L"", 3, 0));

      m_pObjBuyBtn->Enabled(true); 
   }
   else
   {

      m_pObjQuantity->Maximum(100000000);
      m_pObjQuantity->Value(100000000,false,false);
      m_pObjQuantity->TextValueObj()->Text("");
      m_pObjQuantity->Enabled(false);
      
      m_pObjBuyBtn->Enabled(false);
      m_pObjProducerLabel->Text("");
      m_pObjUnitCostLabel->Text("");
      m_pObjTotalUnitCostLabel->Text("");
      UpdateDesignInfo(NULL);
   }
}

void GUsedUnitWindow::EnableMeters(bool in_bEnable)
{
   // Enable or disable component to write 
   m_pObjGunRangeMeter        ->Enabled(in_bEnable);
   m_pObjDesignNameLabel      ->Enabled(in_bEnable);
   m_pObjGunRangeMeter        ->Enabled(in_bEnable);
   m_pObjGunPrecisionMeter    ->Enabled(in_bEnable);
   m_pObjGunDamageMeter       ->Enabled(in_bEnable);
   m_pObjMissileRangeMeter    ->Enabled(in_bEnable);
   m_pObjMissileDamageMeter   ->Enabled(in_bEnable);
   m_pObjMissilePayloadMeter  ->Enabled(in_bEnable);
   m_pObjMissilePrecisionMeter->Enabled(in_bEnable);
   m_pObjSpeedMeter           ->Enabled(in_bEnable);
   m_pObjSensorsMeter         ->Enabled(in_bEnable);
   m_pObjArmorMeter           ->Enabled(in_bEnable);
   m_pObjStealthMeter         ->Enabled(in_bEnable);
   m_pObjCountermeasuresMeter ->Enabled(in_bEnable);
}

void GUsedUnitWindow::UpdateDesignInfo(GUnitDesign*   in_pUnitDesign )
{
   // Enable component to write 
   EnableMeters(true);
   if(in_pUnitDesign != NULL)
   {
      m_pObjDesignNameLabel->Text(in_pUnitDesign->Name());
      m_pObjGunRangeMeter->Value(in_pUnitDesign->GunRange(),0);
      m_pObjGunPrecisionMeter->Value(in_pUnitDesign->GunPrecision(),0);
      m_pObjGunDamageMeter->Value(in_pUnitDesign->GunDamage(),0);
      m_pObjMissileRangeMeter->Value(in_pUnitDesign->MissileRange(),0);
      m_pObjMissileDamageMeter->Value(in_pUnitDesign->MissileDamage(),0);
      m_pObjMissilePayloadMeter->Value(in_pUnitDesign->MissilePayload(),0);
      m_pObjMissilePrecisionMeter->Value(in_pUnitDesign->MissilePrecision(),0);
      m_pObjSpeedMeter->Value(in_pUnitDesign->SpeedAndManeuverability(),0);
      m_pObjSensorsMeter->Value(in_pUnitDesign->Sensors(),0);
      m_pObjArmorMeter->Value(in_pUnitDesign->Armor(),0);
      m_pObjStealthMeter->Value(in_pUnitDesign->Stealth(),0);
      m_pObjCountermeasuresMeter->Value(in_pUnitDesign->CounterMesures(),0);

      // Update 3D Scene
      m_pUnitScene->DisplayUnit(in_pUnitDesign->Type()->Id(), in_pUnitDesign->m_DesignVariations);
      m_pUnitScene->ShowUnit();
      if(m_bFirstTimeToDisplayUnit)
      {
         m_pUnitScene->CameraControl().OffsetOrientation( 3.90f, 0.40f);
         m_bFirstTimeToDisplayUnit = false;
      }
      g_Joshua.GUIManager()->SetDirty();

   }
   else
   { 
      m_pObjDesignNameLabel->Text("");
      m_pObjGunRangeMeter->Value(0,0);
      m_pObjGunPrecisionMeter->Value(0,0);
      m_pObjGunDamageMeter->Value(0,0);
      m_pObjMissileRangeMeter->Value(0,0);
      m_pObjMissileDamageMeter->Value(0,0);
      m_pObjMissilePayloadMeter->Value(0,0);
      m_pObjMissilePrecisionMeter->Value(0,0);
      m_pObjSpeedMeter->Value(0,0);
      m_pObjSensorsMeter->Value(0,0);
      m_pObjArmorMeter->Value(0,0);
      m_pObjStealthMeter->Value(0,0);
      m_pObjCountermeasuresMeter->Value(0,0);
      m_pUnitScene->HideUnit();
   }
   // Disable component , user shouldn't be able to change it
   EnableMeters(false);
}

GUI::EEventStatus::Enum GUsedUnitWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnCustomEvent(in_EventID,in_EventData, in_pCaller);  

   switch (in_EventID)
   {
   case c_OnUpdatableValueChange:
      {
         UpdateSelectedUnit(m_iSelectedUnitID);
      }
      break;
   case ECustomEvents::OnScrollVertical:
      {
         m_pObjSpecListFrm->Child(L"frmInner")->Top(-(INT32)in_EventData.Data);
         return GUI::EEventStatus::Handled;
      }
      break;
      // Select a new item in the cbo
   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         // fired from our units cbo ?
         if(in_EventData.Data == m_pUnitTypeCbo)
         {
            // Request an update.
            g_ClientDCL.RequestUsedUnit();
            m_bRequestedUsedUnit = true;
         }
      }
      break;
      // Select a new item in the listbox
   case ECustomEvents::OnListItemSelected:
      {
         // fired from our units listbox ?
         if(in_EventData.Data == m_pUnitListLst)
         {
            // retrieve our row 
            GUI::GListBox::GRow* l_pRow = NULL;
            vector<INT32> l_iRowId      = m_pUnitListLst->Get_Selected_Item();

            // valid id ?
            if( l_iRowId.size() && (l_iRowId[0] > -1) )
            {
               if(m_iLastSelectedRowID >= 0)
               {
                  l_pRow = m_pUnitListLst->Get_Row_At(m_iLastSelectedRowID);
                  ((GUI::GFrame*)l_pRow->Object())->PictureNormal(GString(L"000587"));
               }

               m_iLastSelectedRowID = l_iRowId[0];
               l_pRow = m_pUnitListLst->Get_Row_At(l_iRowId[0]);
               ((GUI::GFrame*)l_pRow->Object())->PictureNormal(GString(L"000647"));
               UpdateSelectedUnit(l_pRow->m_iAdditionalId);
            }
            else
            {
               m_iLastSelectedRowID = -1;
               UpdateSelectedUnit(-1);
            }
         }
      }
      break;
   default: 
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);      
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUsedUnitWindow::OnMouseWheel(const GEventData& in_Data, GBaseObject*)
{
   if( in_Data.InitialRecipient == m_pModelFrm)
   {
      m_pUnitScene->CameraControl().OffsetDistanceRelative( (REAL32) in_Data.Mouse.WheelDelta * -0.1f);
   }
   else
   {
      GUI::GBaseObject* l_pTemp = in_Data.InitialRecipient;

      while(l_pTemp && l_pTemp != m_pObjSpecListFrm)
      {
         l_pTemp = l_pTemp->Owner();
      }

      if(l_pTemp == m_pObjSpecListFrm)
      {
         m_pObjDesignScr->OnMouseWheel(in_Data, NULL);
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUsedUnitWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseDown(in_EventData, in_pCaller);
   
   m_bDragView = false;
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if( in_EventData.InitialRecipient == m_pModelFrm)  
      {
         if(in_EventData.Mouse.Down.Bits.Left)
         {
            m_bDragView = true;
            Drag();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GUsedUnitWindow::OnDrag(const GEventData& in_EventData, const GVector2D<INT32>& in_MouseMove, const GVector2D<INT32>& in_3rdParam)
{
   if(m_bDragView)
   {
      m_pUnitScene->CameraControl().OffsetOrientation( (REAL32) in_MouseMove.x * -0.025f, (REAL32) in_MouseMove.y * 0.025f);
   }
   else
   {
      return __super::OnDrag(in_EventData,in_MouseMove,in_3rdParam);
   }
}

GUI::EEventStatus::Enum GUsedUnitWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);  

   if(in_pCaller == NULL)
      return GUI::EEventStatus::NotHandled;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjBuyBtn)
      {
         if(m_iSelectedUnitID > 0)
         {
            BuySelectedUnit();
            m_bRequestedUsedUnit = true;
         }
      } 
      else if(in_pCaller == m_pObjExitBtn)
      {
         Hide();
      }
   }

   return GUI::EEventStatus::Handled;
}


void GUsedUnitWindow::BuySelectedUnit(void)
{
   // Send a buy order
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTransfertUnit);
   SP2::Event::GTransfertUnit* l_pTransfertUnitRequest = (SP2::Event::GTransfertUnit*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pTransfertUnitRequest->m_iTarget  = SDK::Event::ESpecialTargets::Server;
   l_pTransfertUnitRequest->m_iSource  = g_SP2Client->Id();
   l_pTransfertUnitRequest->m_bForSale = false;
   l_pTransfertUnitRequest->m_iUnitId  = m_iSelectedUnitID;
   l_pTransfertUnitRequest->m_iQty     = (UINT32)m_pObjQuantity->Value();

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	
}