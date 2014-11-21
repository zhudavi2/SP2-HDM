/**************************************************************
*
* sp2_unit_list_window_new.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_design_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GUnitListWindowNew::TypeName(L"Unit_List_Window_New");

GUI::GBaseObject* GUnitListWindowNew::New()
{
	return (GUI::GBaseObject*) new GUnitListWindowNew();
}


GUnitListWindowNew::GUnitListWindowNew() : GBaseWindow()
{
   m_eSortType = EUnitListSortType::UnitType;
   m_bSortAscending = true;
   m_iFirstRankShown = 0;
   m_viSelectedUnits.clear();
   m_eFilterType = EUnitType::TotalCount;
}

GUnitListWindowNew::~GUnitListWindowNew()
{
}

bool GUnitListWindowNew::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm          = (GUI::GFrame*)Child(L"frmInner");
   m_pUnitListFrm          = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmList");

   m_pObjApplyBtn->Hide();
   
   for(INT32 i=0; i<20; i++) 
      m_pUnitItem[i] = (GUI::GFrame*)m_pUnitListFrm->Child(L"frmList")->Child( GString(L"frm") + GString(i + 1) );

   m_pUnitTypeCbo          = (GUI::GComboBox*)m_pUnitListFrm->Child(L"cboUnitType");

   m_pObjSortTrainingIcon  = (GUI::GButton*)m_pUnitListFrm->Child(L"btnTrainingIcon");
   m_pObjSortType          = (GUI::GButton*)m_pUnitListFrm->Child(L"btnType");
   m_pObjSortTypeIcon      = (GUI::GButton*)m_pUnitListFrm->Child(L"btnTypeIcon");
   m_pObjSortQty           = (GUI::GButton*)m_pUnitListFrm->Child(L"btnQty");
   m_pObjSortQtyIcon       = (GUI::GButton*)m_pUnitListFrm->Child(L"btnQtyIcon");
   m_pObjSortName          = (GUI::GButton*)m_pUnitListFrm->Child(L"btnName");
   m_pObjSortNameIcon      = (GUI::GButton*)m_pUnitListFrm->Child(L"btnNameIcon");
   m_pObjSortStatus        = (GUI::GButton*)m_pUnitListFrm->Child(L"btnStatus");
   m_pObjSortStatusIcon    = (GUI::GButton*)m_pUnitListFrm->Child(L"btnStatusIcon");
   m_pObjSortUpkeep        = (GUI::GButton*)m_pUnitListFrm->Child(L"btnUpkeep");
   m_pObjSortUpkeepIcon    = (GUI::GButton*)m_pUnitListFrm->Child(L"btnUpkeepIcon");
   m_pObjSortValue         = (GUI::GButton*)m_pUnitListFrm->Child(L"btnValue");
   m_pObjSortValueIcon     = (GUI::GButton*)m_pUnitListFrm->Child(L"btnValueIcon");
   m_pObjSortForSaleIcon   = (GUI::GButton*)m_pUnitListFrm->Child(L"btnForSaleIcon");

   //m_pObjSelectAll         = (GUI::GButton*)m_pUnitListFrm->Child(L"btnClearSelection");
   //m_pObjClearSelection    = (GUI::GButton*)m_pUnitListFrm->Child(L"btnSelectAll");

   m_pObjTrainBtn         = (GUI::GButton*)m_pUnitListFrm->Child(L"btnTrain");
   m_pObjSellBtn          = (GUI::GButton*)m_pUnitListFrm->Child(L"btnSell");
   m_pObjDisbandBtn       = (GUI::GButton*)m_pUnitListFrm->Child(L"btnDisband");

   m_pObjListScr          = (GUI::GVScrollBar*)m_pUnitListFrm->Child(L"scrList");

   m_pAccuracyMtr          = (SP2::GGradientBar*)m_pUnitListFrm->Child(L"frmAccuracy");
   m_pAccuracyTxt          = (GUI::GLabel*)m_pUnitListFrm->Child(L"txtValue");
   m_pAccuracyLbl          = (GUI::GLabel*)m_pUnitListFrm->Child(L"lblAccuracy");

   m_pAccuracyMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));

   InitializeUnitTypeCombo();
   m_pObjListScr->Initialize( this, 0, 0, 1, 5, 0 );

   m_pObjSellBtn->Enabled(false);
   m_pObjTrainBtn->Enabled(false);
   m_pObjDisbandBtn->Enabled(false);

	return true;
}

//Fill the unit type combo box
void GUnitListWindowNew::InitializeUnitTypeCombo()
{
   m_pUnitTypeCbo->Clear();
   
   //Add All item
   m_pUnitTypeCbo->Add_Item( g_ClientDAL.GetString(101665), (UINT32)EUnitType::TotalCount );

   //Add each unit type
   const vector<SP2::GUnitType*> & l_Types = g_ClientDAL.UnitTypes();
   for(UINT32 i=0; i<l_Types.size(); i++)
   {
      m_pUnitTypeCbo->Add_Item( l_Types[i]->Name(), l_Types[i]->Id() );
   }
}

void GUnitListWindowNew::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   if(m_eFilterType == EUnitType::TotalCount)
      m_pUnitTypeCbo->Selected_Content( 0, true );
   else
      m_pUnitTypeCbo->Selected_Content( (UINT32)m_eFilterType, true );

   g_Joshua.UnitManager().Attach( this, SDK::Combat::c_notificationUnitRemoved );
   g_Joshua.UnitManager().Attach( this, SDK::Combat::c_notificationUnitModified );
   g_Joshua.UnitManager().Attach( this, SDK::Combat::c_notificationUnitGroupModified );
}

void GUnitListWindowNew::OnHide()
{
   g_Joshua.UnitManager().Detach( this, SDK::Combat::c_notificationUnitGroupModified );
   g_Joshua.UnitManager().Detach( this, SDK::Combat::c_notificationUnitModified );
   g_Joshua.UnitManager().Detach( this, SDK::Combat::c_notificationUnitRemoved );
   
   __super::OnHide();
}

EEventStatus::Enum GUnitListWindowNew::OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(m_viSelectedUnits.size())
   {
      if(in_EventData.Mouse.IsDoubleClick)
      {
         INT32 l_iRowID = GetMouseOverRowId( in_EventData.Mouse.Position );

         if( l_iRowID >= 0 )
         {
            if( m_ShownUnits[l_iRowID] == m_viSelectedUnits[m_viSelectedUnits.size() - 1] )
            {
               if( ((SP2::GUnitDesign*)g_Joshua.UnitManager().Unit(m_viSelectedUnits[m_viSelectedUnits.size() - 1])->Design())->Type()->Id() != 1 )
               {
                  if(!g_ClientDDL.DesignWindow())
                     g_ClientDDL.DesignWindowSpawn();

                  g_ClientDDL.DesignWindow()->Show();
                  g_ClientDDL.DesignWindow()->SelectedDesign( (SP2::GUnitDesign*)g_Joshua.UnitManager().Unit(m_viSelectedUnits[m_viSelectedUnits.size() - 1])->Design() );
               }
            }
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

EEventStatus::Enum GUnitListWindowNew::OnMouseWheel(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   m_pObjListScr->OnMouseWheel(in_EventData, in_pCaller);

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitListWindowNew::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.UnitListWindowNew()->Hide();
            return GUI::EEventStatus::Handled;
         }
         else if(in_pCaller == m_pObjSortTrainingIcon)
         {
            Sort( EUnitListSortType::UnitTraining );
         }
         else if(in_pCaller == m_pObjSortType || in_pCaller == m_pObjSortTypeIcon)
         {
            Sort( EUnitListSortType::UnitType );
         }
         else if(in_pCaller == m_pObjSortQty || in_pCaller == m_pObjSortQtyIcon)
         {
            Sort( EUnitListSortType::UnitQty );
         }
         else if(in_pCaller == m_pObjSortName || in_pCaller == m_pObjSortNameIcon)
         {
            Sort( EUnitListSortType::UnitName );
         }
         else if(in_pCaller == m_pObjSortStatus || in_pCaller == m_pObjSortStatusIcon)
         {
            Sort( EUnitListSortType::UnitStatus );
         }
         else if(in_pCaller == m_pObjSortUpkeep || in_pCaller == m_pObjSortUpkeepIcon)
         {
            Sort( EUnitListSortType::UnitUpkeep );
         }
         else if(in_pCaller == m_pObjSortValue || in_pCaller == m_pObjSortValueIcon )
         {
            Sort( EUnitListSortType::UnitValue );
         }
         else if(in_pCaller == m_pObjSortForSaleIcon)
         {
            Sort( EUnitListSortType::UnitForSale );
         }
         else if(in_pCaller == m_pObjSellBtn)
         {
            for(UINT32 i = 0; i < m_viSelectedUnits.size(); i++)
               SellUnit( m_viSelectedUnits[i] );

            m_pObjTrainBtn->Enabled(false);
         }
         else if(in_pCaller == m_pObjTrainBtn)
         {
            // Build a list of units to train
            list<UINT32> l_UnitList;
            for(UINT32 i = 0; i < m_viSelectedUnits.size(); i++)
               l_UnitList.push_back(m_viSelectedUnits[i]);
            g_ClientDCL.TrainUnits(l_UnitList, this, this);

            m_pObjTrainBtn->Enabled(false);
            m_pObjSellBtn->Enabled(false);
         }
         else if(in_pCaller == m_pObjDisbandBtn)
         {
            list<UINT32> l_UnitList;
            for(UINT32 i = 0; i < m_viSelectedUnits.size(); i++)
            {
               l_UnitList.push_back(m_viSelectedUnits[i]);
               // mark it as locked
               m_UnitsLocked.insert(m_viSelectedUnits[i]);
            }
            g_ClientDCL.DisbandUnits(l_UnitList);

            m_pObjSellBtn->Enabled(false);
            m_pObjTrainBtn->Enabled(false);
            m_pObjDisbandBtn->Enabled(false);             
         }
      }
   }
   else
   {
      GUI::GBaseObject* l_pObj = in_EventData.InitialRecipient;
      bool l_bRowDeselected = false;
      while( l_pObj != this && l_pObj != m_pUnitItem[0]->Owner() )
      {
         l_pObj = l_pObj->Parent();
      }

      if( l_pObj == m_pUnitItem[0]->Owner() )
      {
         INT32 l_iRowID = GetMouseOverRowId( in_EventData.Mouse.Position );

         // If ctrl is not pressed we deselect all that is currently selected
         if(!in_EventData.Keyboard.Ctrl)
         {
            vector<INT32>::iterator l_ItSel = m_viSelectedUnits.begin();
            while(l_ItSel != m_viSelectedUnits.end())
            {
               vector<INT32>::const_iterator l_It = find(m_ShownUnits.begin(), m_ShownUnits.end(), *l_ItSel);
               if(l_It != m_ShownUnits.end())
               {
                  UnselectRow( (INT32)(l_It - m_ShownUnits.begin()) );
                  l_ItSel = m_viSelectedUnits.erase(l_ItSel);
               }
               else
               {
                  l_ItSel++;
               }
            }
            m_viSelectedUnits.clear();
         }

         if( l_iRowID >= 0 )
         {
            if(m_viSelectedUnits.size())
            {
               vector<INT32>::iterator l_ItSel = m_viSelectedUnits.begin();
               while(l_ItSel != m_viSelectedUnits.end())
               {
                  vector<INT32>::const_iterator l_It = find(m_ShownUnits.begin(), m_ShownUnits.end(), *l_ItSel);
                  if(*l_It == m_ShownUnits[l_iRowID])
                  {
                     UnselectRow( (INT32)(l_It - m_ShownUnits.begin()) );
                     l_ItSel = m_viSelectedUnits.erase(l_ItSel);
                     l_bRowDeselected = true;
                     break;
                  }
                  else
                     l_ItSel++;
               }
            }
            
            //If user clicked a valid row set the last clicked rank and enable sell button
            if( l_iRowID + m_iFirstRankShown < (INT32)m_UnitsMap.size() &&
               !l_bRowDeselected)
            {
               m_viSelectedUnits.push_back(m_ShownUnits[l_iRowID]);
               SelectRow(l_iRowID);
            }
            else if(!l_bRowDeselected)
            {
               m_pObjSellBtn->Enabled(false);
               m_pObjTrainBtn->Enabled(false);
               m_pObjDisbandBtn->Enabled(false);

               return GUI::EEventStatus::Handled;
            }
         }

         //Enable or not training button
         if( m_viSelectedUnits.size() )
         {
            EnableUnitButtons(m_viSelectedUnits);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GUnitListWindowNew::EnableUnitButtons( vector<INT32> in_viUnitsID/*INT32 in_iUnitID*/ )
{
   bool l_bDisbandBtnEnabled = true;
   bool l_bSellBtnEnabled = false;
   bool l_bTrainBtnEnabled = true;

   for(UINT32 i = 0; i < in_viUnitsID.size(); i++)
   {
      //If my unit is in training
      const hash_map<UINT32, GUnitInTraining> & l_TrainMap = g_ClientDAL.UnitInTraining();
      hash_map<UINT32, GUnitInTraining>::const_iterator l_It = l_TrainMap.find(in_viUnitsID[i]);
      //If my unit is for sale
      list<UINT32>::iterator l_It2 = find( g_ClientDAL.UnitsForSale().begin(), g_ClientDAL.UnitsForSale().end(), in_viUnitsID[i] );   
      //If my unit is locked
      set<INT32>::const_iterator l_ItLocked = m_UnitsLocked.find(in_viUnitsID[i]);

      SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_viUnitsID[i]);

      // if we really have our unit and if it isnt locked
      if( l_pUnit && l_ItLocked == m_UnitsLocked.end() )
      {
         bool l_bSellingUnit = false;
         bool l_bTrainingUnit = false;                                      

         if(l_It != l_TrainMap.end())
            l_bTrainingUnit = true;

         if(l_It2 != g_ClientDAL.UnitsForSale().end())
            l_bSellingUnit = true;
         
         if( ((SP2::GUnitDesign*)g_Joshua.UnitManager().Unit(in_viUnitsID[i])->Design())->Type()->Id() != 1 && !l_bTrainingUnit )
         {
            l_bSellBtnEnabled = true;
         }

         if( l_pUnit->Training() == ETrainingLevel::Elite )
         {
            l_bTrainBtnEnabled = false;
         }
         else
         {
            //If my unit is not to be sold
            if( l_bSellingUnit || l_bTrainingUnit )
               l_bTrainBtnEnabled = false;
         }
      }
      else
      {
         l_bTrainBtnEnabled = false;
         l_bDisbandBtnEnabled = false;
      }
   }

   m_pObjSellBtn->Enabled(l_bSellBtnEnabled);
   m_pObjTrainBtn->Enabled(l_bTrainBtnEnabled);
   m_pObjDisbandBtn->Enabled(l_bDisbandBtnEnabled);
}

INT32 GUnitListWindowNew::GetMouseOverRowId( GVector2D<INT32> in_iAbsoluteMousePosition )
{
   INT32 l_iRowID = -1;

   GVector2D<INT32> l_ListPos   = m_pUnitItem[0]->Owner()->RelativeToAbsolute( GVector2D<INT32>(0, 0) );
   GVector2D<INT32> l_ClickPos  = RelativeToAbsolute( in_iAbsoluteMousePosition );

   //Check if user click within unit list frame
   if( l_ClickPos.x >= l_ListPos.x && l_ClickPos.x <= l_ListPos.x + m_pUnitItem[0]->Owner()->Width() )
   {
      if( l_ClickPos.y >= l_ListPos.y && l_ClickPos.y <= l_ListPos.y + m_pUnitItem[0]->Owner()->Height() ) 
      {
         //If so get the clicked row
         GVector2D<INT32> l_Pos(l_ClickPos.x - l_ListPos.x, l_ClickPos.y - l_ListPos.y);
         l_iRowID = (INT32)floor( (REAL32)l_Pos.y / 15.f );

         if( l_iRowID > (INT32)m_ShownUnits.size() )
         {
            l_iRowID = -1;
         }
      }
   }
  
   return l_iRowID;
}

GUI::EEventStatus::Enum GUnitListWindowNew::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, SDK::GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnComboBoxSelectionEvent)
   {
      if(in_pCaller == m_pUnitTypeCbo)
      {
         UpdateUnitList( (UINT16)m_iCountryID, m_iGroupID, (EUnitType::Enum)m_pUnitTypeCbo->SelectedRow()->m_iAdditionalId, true );
      }
   }
   else if(in_iEventID == ECustomEvents::OnScrollVertical)
   {
      if(in_pCaller == m_pObjListScr)
      {
         Update( (INT32)in_EventData.Data );
      }
   }

   return GUI::EEventStatus::Handled;
}

void GUnitListWindowNew::Update( INT32 in_iFromWhichRank )
{
   m_iFirstRankShown = in_iFromWhichRank;

   if( in_iFromWhichRank > (INT32)m_UnitsMap.size() - 20 )
      m_iFirstRankShown = m_UnitsMap.size() - 20;
   
   if( m_iFirstRankShown < 0 )
      m_iFirstRankShown = 0;

   Sort( m_eSortType, m_bSortAscending );
}

void GUnitListWindowNew::Sort(EUnitListSortType::Enum in_eSort)
{
   Sort( in_eSort, !m_bSortAscending );
}

void GUnitListWindowNew::Sort( EUnitListSortType::Enum in_eSort, bool in_bAscending )
{
   m_eSortType = in_eSort;
   m_bSortAscending = in_bAscending;

   GString l_sImageName(L"000888");
   if(m_bSortAscending)
      l_sImageName = L"000889";

   GUI::GButton* l_pBtnToChange;

   m_pObjSortTrainingIcon->PictureNormal(L"000159");
   m_pObjSortTypeIcon->PictureNormal(L"000159");
   m_pObjSortQtyIcon->PictureNormal(L"000159");
   m_pObjSortNameIcon->PictureNormal(L"000159");
   m_pObjSortStatusIcon->PictureNormal(L"000159");
   m_pObjSortUpkeepIcon->PictureNormal(L"000159");
   m_pObjSortValueIcon->PictureNormal(L"000159");
   m_pObjSortForSaleIcon->PictureNormal(L"000159");

   switch(m_eSortType)
   {
   case EUnitListSortType::UnitForSale:
      l_pBtnToChange = m_pObjSortForSaleIcon;
      break;
   case EUnitListSortType::UnitName:
      l_pBtnToChange = m_pObjSortNameIcon;
      break;
   case EUnitListSortType::UnitQty:
      l_pBtnToChange = m_pObjSortQtyIcon;
      break;
   case EUnitListSortType::UnitStatus:
      l_pBtnToChange = m_pObjSortStatusIcon;
      break;
   case EUnitListSortType::UnitTraining:
      l_pBtnToChange = m_pObjSortTrainingIcon;
      break;
   case EUnitListSortType::UnitType:
      l_pBtnToChange = m_pObjSortTypeIcon;
      break;
   case EUnitListSortType::UnitUpkeep:
      l_pBtnToChange = m_pObjSortUpkeepIcon;
      break;
   case EUnitListSortType::UnitValue:
   default:
      l_pBtnToChange = m_pObjSortValueIcon;
      break;
   }
   l_pBtnToChange->PictureNormal(l_sImageName);

   vector<INT32> l_UnitsToDisplay = GetUnitsToDisplay( m_iFirstRankShown, m_eSortType, m_bSortAscending );
   
   for(UINT32 i=0; i<20; i++)
   {
      if(i < l_UnitsToDisplay.size())
         DisplayUnit( i, l_UnitsToDisplay[i] );
      else
         ClearRow( i );
   }
   EnableUnitButtons(m_viSelectedUnits);
}

void GUnitListWindowNew::SelectRow( UINT32 in_iRowID )
{
   m_pUnitItem[in_iRowID]->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtType"))    ->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtQty"))     ->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtName"))    ->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtPosition"))->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtValue"))   ->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtUpkeep"))  ->PictureNormal( GString(L"000647") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))  ->PictureNormal( GString(L"000647") );
}

void GUnitListWindowNew::UnselectRow( UINT32 in_iRowID )
{
   m_pUnitItem[in_iRowID]->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtType"))    ->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtQty"))     ->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtName"))    ->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtPosition"))->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtValue"))   ->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtUpkeep"))  ->PictureNormal( GString(L"000587") );
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))  ->PictureNormal( GString(L"000587") );
}

void GUnitListWindowNew::DisplayUnit( UINT32 in_iRowID, INT32 in_iUnitID )
{
   if( in_iRowID >= 20)
      return;

   SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_iUnitID);
   if(!l_pUnit)
      return;

   if(m_viSelectedUnits.size())
   {
      UINT32 i;
      for(i = 0; i < m_viSelectedUnits.size(); i++)
      {
         if( m_viSelectedUnits[i] == in_iUnitID )
         {
            SelectRow(in_iRowID);
            break;
         }
      }
       
      if(i == m_viSelectedUnits.size())
         UnselectRow(in_iRowID);
   }

	// Get Accuracy
	REAL32 l_fAccuracy = g_ClientDAL.m_PlayerCountryData.AccuracyInformation(m_iCountryID);
	bool l_bKnowUnit = true;
	bool l_bKnowType = true;
	bool l_bKnowExactQty = true;

	//Get Unit Type
	SP2::GUnitType* l_pUnitType = ((SP2::GUnitType*)((SP2::GUnitDesign*)l_pUnit->Design())->Type());

	//Set the seed
	m_Rand.Seed(m_iCountryID * (UINT32)floor(l_fAccuracy*100.f) * l_pUnitType->Id() * l_pUnit->Id());

	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowType = false;
	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowUnit = false;
	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowExactQty = false;

	if(l_bKnowType) 
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtType"))    ->Text( ((SP2::GUnitDesign*)l_pUnit->Design())->Type()->Name() );
	else
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtType"))    ->Text( L"???" );
	
	if(l_bKnowExactQty)
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtQty"))     ->Text( GString( (INT32)((REAL32)l_pUnit->Qty() * (1.f + ( (1.f-l_fAccuracy) * (m_Rand.RandomReal(2.f)-1.f)  ))) ) );
	else
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtQty"))     ->Text( L"???" );
	
   if(l_bKnowUnit) 
   {
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtName"))    ->Text( ((SP2::GUnitDesign*)l_pUnit->Design())->Name() );
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtValue"))   ->Text( GString::FormatNumber( (REAL64)((SP2::GUnitDesign*)l_pUnit->Design())->Cost(), L" ", L"", L"", L"", 3, 0 ) );
   }
   else
   {
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtName"))    ->Text( L"???" );
		((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtValue"))   ->Text( L"???" );
   }

   if( ((SP2::GUnitGroup*)g_Joshua.UnitManager().Unit(in_iUnitID)->Group())->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID() )
      ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtUpkeep"))->Text( GString::FormatNumber( (REAL64)CalculateUnitUpkeep(in_iUnitID), L" ", L"", L"", L"", 3, 0 ) );
   else
	   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtUpkeep"))->Text( L"???" );


   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtPosition"))->Text( GString( g_ClientDAL.GetString(102362) + L" " + GString::FormatNumber(l_pUnit->Group()->Position().x, 2) + L" - " + g_ClientDAL.GetString(102363) + L" " + GString::FormatNumber(l_pUnit->Group()->Position().y, 2) ) );  
   
   UnitUpdateStatus( in_iUnitID, in_iRowID, true );

   if( g_ClientDAL.IsUnitForSale(in_iUnitID) )
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picSell"))->Show();
   else
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picSell"))->Hide();
   
   switch( l_pUnit->Training() )
   {
   case ETrainingLevel::Recruit:
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picTraining"))->PictureNormal( L"000159" );
      break;
   case ETrainingLevel::Regular:
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picTraining"))->PictureNormal( L"000825" );
      break;
   case ETrainingLevel::Veteran:
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picTraining"))->PictureNormal( L"000826" );
      break;
   case ETrainingLevel::Elite:
      ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picTraining"))->PictureNormal( L"000827" );
      break;
   default:
      gassert(0,"Unhandled case in unit training");
   }
}

void GUnitListWindowNew::UnitUpdateStatus( INT32 in_iUnitID, INT32 in_iRowID, bool in_bForceUpdate )
{
   //Get the unit to update
   SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_iUnitID);

   if(!l_pUnit)
      return;

   //Get it's status
   GString l_sStatus = g_ClientDAL.MilitaryStatusToString( ((SP2::GUnitGroup*)l_pUnit->Group())->Status( (REAL32)g_Joshua.GameTime() ) , l_pUnit);

   //If the status hasn't changed don't do anything otherwise update it
   if( in_bForceUpdate || l_sStatus != m_UnitsMap[in_iUnitID].m_StatusIter->first || ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))->Text() == L"" )
   {
      m_UnitsLocked.erase(in_iUnitID);
      switch( ((SP2::GUnitGroup*)l_pUnit->Group())->Status( (REAL32)g_Joshua.GameTime() ) )
      {      
      case SP2::EMilitaryStatus::Attacking:
         m_UnitsLocked.insert(in_iUnitID);
      case SP2::EMilitaryStatus::ReadyToDeploy:         
      case SP2::EMilitaryStatus::Fortified:
         ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(242,227,124,255));
         break;

      case SP2::EMilitaryStatus::Parked:
      case SP2::EMilitaryStatus::Training:
      case SP2::EMilitaryStatus::Upgrading:
      case SP2::EMilitaryStatus::InTransit:
      case SP2::EMilitaryStatus::Moving:         
         ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(174,205,255,255));
         break;

      case SP2::EMilitaryStatus::CanJoinCombat:
         m_UnitsLocked.insert(in_iUnitID);
      case SP2::EMilitaryStatus::Ready:
      default:
         ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(117,219,94,255));
         break;
      }
      ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))->Text( l_sStatus );
   }
}

void GUnitListWindowNew::UnitsRefreshStatuses()
{
   UINT32 i=0;
   for(vector<INT32>::iterator l_It = m_ShownUnits.begin(); l_It != m_ShownUnits.end(); l_It++)
   {
      UnitUpdateStatus( m_ShownUnits[i], i );
      i++;
   }
}

void GUnitListWindowNew::ClearRow(UINT32 in_iRowID)
{
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtType"))    ->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtQty"))     ->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtName"))    ->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtPosition"))->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtValue"))   ->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtUpkeep"))  ->Text(L"");
   ((GUI::GLabel*)m_pUnitItem[in_iRowID]->Child(L"txtStatus"))  ->Text(L"");
   ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picSell"))->Hide();
   ((GUI::GPictureBox*)m_pUnitItem[in_iRowID]->Child(L"picTraining"))->PictureNormal( L"000159" );

   UnselectRow(in_iRowID);
}

//Get a list of unit ID to display
const vector<INT32> & GUnitListWindowNew::GetUnitsToDisplay( INT32 in_iFirstUnitShown, EUnitListSortType::Enum in_eSort, bool in_bAscending )
{
   m_ShownUnits.clear();

   INT32 l_iNbRowToDisplay = 20;

   //Be sure to not exceed the qty of row that can be shown
   if(in_iFirstUnitShown + l_iNbRowToDisplay > (INT32)m_UnitsMap.size())
      in_iFirstUnitShown = m_UnitsMap.size() - l_iNbRowToDisplay;

   if(in_iFirstUnitShown < 0)
      l_iNbRowToDisplay += in_iFirstUnitShown;

   //Depending of the wanted sort we need to use different maps
   switch( in_eSort )
   {
   case EUnitListSortType::UnitForSale:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< bool, INT32 >::const_iterator l_It = m_SaleMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_SaleMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< bool, INT32 >::reverse_iterator l_It = m_SaleMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_SaleMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitStatus:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::const_iterator l_It = m_StatusMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_StatusMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::reverse_iterator l_It = m_StatusMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_StatusMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitTraining:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< ETrainingLevel::Enum, INT32 >::const_iterator l_It = m_TrainingMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_TrainingMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< ETrainingLevel::Enum, INT32 >::reverse_iterator l_It = m_TrainingMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_TrainingMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitUpkeep:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< REAL32, INT32 >::const_iterator l_It = m_UpkeepMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_UpkeepMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< REAL32, INT32 >::reverse_iterator l_It = m_UpkeepMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_UpkeepMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitValue:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< REAL64, INT32 >::const_iterator l_It = m_ValueMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_ValueMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< REAL64, INT32 >::reverse_iterator l_It = m_ValueMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_ValueMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitQty:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< INT32, INT32 >::const_iterator l_It = m_QtyMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_QtyMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< INT32, INT32 >::reverse_iterator l_It = m_QtyMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_QtyMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitName:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::const_iterator l_It = m_NameMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_NameMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::reverse_iterator l_It = m_NameMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_NameMap.rend())
               break;
         }
      }
      break;

   case EUnitListSortType::UnitType:
   default:
      //If the sort is ascending
      if( in_bAscending )
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::const_iterator l_It = m_TypeMap.begin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_TypeMap.end())
               break;
         }
      }
      else
      {
         //Get the first unit id wanted
         multimap< GString, INT32 >::reverse_iterator l_It = m_TypeMap.rbegin();
         for( INT32 i=0; i<in_iFirstUnitShown; i++ )
            l_It++;

         //Put the unit into the display vector
         for( INT32 i=0; i<l_iNbRowToDisplay; i++ )
         {
            m_ShownUnits.push_back( l_It->second );
            l_It++;
            if(l_It == m_TypeMap.rend())
               break;
         }
      }
      break;
   }

   return m_ShownUnits;
}

/*
* Fill all the Unit List maps with a given country units 
*/
void GUnitListWindowNew::UpdateUnitList(UINT16 in_iCountryID, INT32 in_iGroupID, EUnitType::Enum in_eFilter, bool in_bRefreshWindow)
{
   m_iCountryID  = in_iCountryID;
   m_iGroupID    = in_iGroupID; 
   
   if( in_eFilter != (EUnitType::Enum)0xFFFFFFFF )
      m_eFilterType = in_eFilter;

   //Clear every map containing unit data
   m_TypeMap.clear();
   m_QtyMap.clear();
   m_TrainingMap.clear();
   m_NameMap.clear();
   m_StatusMap.clear();
   m_UpkeepMap.clear();
   m_ValueMap.clear();
   m_SaleMap.clear();
   m_UnitsMap.clear();
   m_UnitsLocked.clear();
   
   //Get country unit group
   const set<UINT32> & l_CountryUnitGroups = g_Joshua.UnitManager().CountryUnitGroups( in_iCountryID );
   GUnitGroup* l_pGroup = NULL;

   //For each country units groups
   for(set<UINT32>::const_iterator l_GroupsIt = l_CountryUnitGroups.begin(); 
       l_GroupsIt != l_CountryUnitGroups.end(); 
       l_GroupsIt++)
   {
      //Check if we want a specific group
      if( in_iGroupID >= 0 )
      {
         if( *l_GroupsIt != (UINT32)in_iGroupID )
            continue;
      }
      
      //Get current group
      l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *l_GroupsIt );
      
      //Get units from current group
      const vector<SP2::GUnit*> & l_Units = (const vector<SP2::GUnit*> &)l_pGroup->Units();
      
      //For each unit add them to maps data
      for(UINT32 i=0; i<l_Units.size(); i++)
      {
         //Get current unit
         SP2::GUnit* l_pUnit = l_Units[i];
         if(l_pUnit)
            AddUnit( l_pUnit );

      }//End of for each units from group
   
   }//End of for each groups

   m_pObjListScr->Initialize( this, 0, max((INT32)(m_UnitsMap.size() - 20), 0), 1, 5, max(m_iFirstRankShown, 0) );

   if( m_iCountryID != g_ClientDAL.ControlledCountryID() )
   {
      if(!m_pAccuracyMtr->Visible())
      {
         m_pAccuracyMtr->Show();
         m_pAccuracyTxt->Show();
         m_pAccuracyLbl->Show();
      }

      REAL32 l_fAccuracy = g_ClientDAL.m_PlayerCountryData.AccuracyInformation( m_iCountryID );
      m_pAccuracyMtr->Value( l_fAccuracy, 0 ); 
      m_pAccuracyTxt->Text( GString::FormatNumber(l_fAccuracy * 100, L"", L"", L"", L"%", 0, 0) );

      m_pObjDisbandBtn->Hide();
      m_pObjTrainBtn->Hide();
      m_pObjSellBtn->Hide();
   }
   else
   {
      if(m_pAccuracyMtr->Visible())
      {
         m_pAccuracyMtr->Hide();
         m_pAccuracyTxt->Hide();
         m_pAccuracyLbl->Hide();
      }

      m_pObjDisbandBtn->Show();
      m_pObjTrainBtn->Show();
      m_pObjSellBtn->Show();
   }

   Update(m_iFirstRankShown);
}

//Update main map iterators to quickly access a unit info
void GUnitListWindowNew::UpdateIterators()
{
   multimap< GString,               INT32 >::iterator l_TypeIter     = m_TypeMap.begin();
   multimap< INT32,                 INT32 >::iterator l_QtyIter      = m_QtyMap.begin();
   multimap< ETrainingLevel::Enum,  INT32 >::iterator l_TrainingIter = m_TrainingMap.begin();
   multimap< GString,               INT32 >::iterator l_NameIter     = m_NameMap.begin();
   multimap< GString,               INT32 >::iterator l_StatusIter   = m_StatusMap.begin();
   multimap< REAL32,                INT32 >::iterator l_UpkeepIter   = m_UpkeepMap.begin();
   multimap< REAL64,                INT32 >::iterator l_ValueIter    = m_ValueMap.begin();
   multimap< bool,                  INT32 >::iterator l_SaleIter     = m_SaleMap.begin();
   
   for(UINT32 i=0; i<m_UnitsMap.size(); i++)
   {
      m_UnitsMap[ l_TypeIter->second      ].m_TypeIter      = l_TypeIter;
      m_UnitsMap[ l_QtyIter->second       ].m_QtyIter       = l_QtyIter;
      m_UnitsMap[ l_TrainingIter->second  ].m_TrainingIter  = l_TrainingIter;
      m_UnitsMap[ l_NameIter->second      ].m_NameIter      = l_NameIter;
      m_UnitsMap[ l_StatusIter->second    ].m_StatusIter    = l_StatusIter;
      m_UnitsMap[ l_UpkeepIter->second    ].m_UpkeepIter    = l_UpkeepIter;
      m_UnitsMap[ l_ValueIter->second     ].m_ValueIter     = l_ValueIter;
      m_UnitsMap[ l_SaleIter->second      ].m_SaleIter      = l_SaleIter;

      l_TypeIter++;    
      l_QtyIter++;     
      l_TrainingIter++;
      l_NameIter++;    
      l_StatusIter++;  
      l_UpkeepIter++;  
      l_ValueIter++;   
      l_SaleIter++;
   }
}

void GUnitListWindowNew::Iterate()
{
   //UnitsRefreshStatuses();
}

void GUnitListWindowNew::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if(&in_Subject == &g_Joshua.UnitManager())
   {
      switch(in_Notification)
      {
      case SDK::Combat::c_notificationUnitRemoved:
         {
            const set<UINT32> & l_RemovedUnits = g_Joshua.UnitManager().RemovedUnits();
            UpdateRemovedUnits( l_RemovedUnits );
         }
         break;
      case SDK::Combat::c_notificationUnitModified:
         {
            const hash_map<UINT32, SDK::Combat::GUnit*> & l_ModifiedUnits = g_Joshua.UnitManager().ModifiedUnits();
            UpdateModifiedUnits( l_ModifiedUnits );
         }
         break;
      case SDK::Combat::c_notificationUnitGroupModified:
         {
            const hash_map<UINT32, SDK::Combat::GUnitGroup*> & l_ModifiedGroups = g_Joshua.UnitManager().ModifiedUnitGroups();
            UpdateModifiedUnitGroups( l_ModifiedGroups );
         }
         break;
      }
   }
   else
   {
      // Assuming we only ask this kind of question!!!
      if(in_Notification == c_OnGenericQuestionWindowAnswerOk)
      {
         // Fire Event
         g_ClientDCL.RequestTraining();
      }
   }
}

void GUnitListWindowNew::UpdateRemovedUnits( const set<UINT32> & in_RemovedUnits )
{
   set<UINT32>::const_iterator l_It = in_RemovedUnits.begin();
   
   //For each removed units
   while(l_It != in_RemovedUnits.end())
   {
      RemoveUnit((INT32)*l_It, false);      
      l_It++;
   }

   m_pObjListScr->Initialize( this, 0, max(m_UnitsMap.size() - 20, 0), 1, 5, max(m_iFirstRankShown, 0) );

   Update(m_iFirstRankShown);
}

void GUnitListWindowNew::UpdateModifiedUnits( const hash_map<UINT32, SDK::Combat::GUnit*> & in_ModifiedUnits )
{
   hash_map<UINT32, SDK::Combat::GUnit*>::const_iterator l_It = in_ModifiedUnits.begin();
   
   //For each removed units
   while(l_It != in_ModifiedUnits.end())
   {
      if( m_iGroupID < 0 )
      {
         if( m_iCountryID == (INT32)l_It->second->Group()->OwnerId() )
         {
            RemoveUnit( (INT32)l_It->first );
            AddUnit( (SP2::GUnit*)l_It->second );
         }
      }
      else if( m_iGroupID == (INT32)l_It->second->Group()->Id() )
      {
         RemoveUnit( (INT32)l_It->first );
         AddUnit( (SP2::GUnit*)l_It->second );
      }

      l_It++;
   }

   m_pObjListScr->Initialize( this, 0, max((INT32)(m_UnitsMap.size() - 20), 0), 1, 5, max(m_iFirstRankShown, 0) );

   Update(m_iFirstRankShown);
}

void GUnitListWindowNew::UpdateModifiedUnitGroups( const hash_map<UINT32, SDK::Combat::GUnitGroup*> & in_ModifiedGroups )
{
   hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator l_It = in_ModifiedGroups.begin();
   
   //For each removed units
   while(l_It != in_ModifiedGroups.end())
   {
      const vector<SDK::Combat::GUnit*> & l_Units = l_It->second->Units();
      
      for(UINT32 i=0; i<l_Units.size(); i++)
      {
         //Check if all units are currently displayed
         if( m_iGroupID < 0 )
         {
            //If the units are mine readd it
            if( m_iCountryID == (INT32)l_Units[i]->Group()->OwnerId() )
            {
               RemoveUnit( (INT32)l_Units[i]->Id() );
               AddUnit( (SP2::GUnit*)l_Units[i] );
            }
         } // If there is a specific groups currently displayed
         else if( m_iGroupID == (INT32)l_Units[i]->Group()->Id() )
         {
            RemoveUnit( (INT32)l_Units[i]->Id() );
            AddUnit( (SP2::GUnit*)l_Units[i] );
         }
      }

      l_It++;
   }

   m_pObjListScr->Initialize( this, 0, max(m_UnitsMap.size() - 20, 0), 1, 5, max(m_iFirstRankShown, 0) );

   Update(m_iFirstRankShown);
}

void GUnitListWindowNew::AddUnit( SP2::GUnit* in_pUnit )
{   
	if(!in_pUnit)
      return;

   if( m_eFilterType != EUnitType::TotalCount && ((SP2::GUnitDesign*)in_pUnit->Design())->Type()->Id() != (UINT32)m_eFilterType )
      return;
	
	// Get Accuracy
	REAL32 l_fAccuracy = g_ClientDAL.m_PlayerCountryData.AccuracyInformation(m_iCountryID);
	bool l_bKnowType = true;
	bool l_bKnowUnit = true;
	bool l_bKnowExactQty = true;

	//Get Unit Type
	SP2::GUnitType* l_pUnitType = ((SP2::GUnitType*)((SP2::GUnitDesign*)in_pUnit->Design())->Type());

	//Set the seed
	m_Rand.Seed(m_iCountryID * (UINT32)floor(l_fAccuracy*100.f) * l_pUnitType->Id() * in_pUnit->Id());

	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowType = false;
	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowUnit = false;
	if(m_Rand.RandomReal() > l_fAccuracy)
		l_bKnowExactQty = false;

   //Add unit to Type map
	if(l_bKnowType) 
		m_TypeMap.insert( make_pair( l_pUnitType->Name(), in_pUnit->Id() ) );
	else
		return;

   //Add unit to Qty map
	if(l_bKnowExactQty)
		m_QtyMap.insert( make_pair( (INT32)((REAL32)in_pUnit->Qty() * (1.f + ( (1.f-l_fAccuracy) * (m_Rand.RandomReal(2.f)-1.f)  ))), in_pUnit->Id() ) );
	else
		m_QtyMap.insert( make_pair( -1 , in_pUnit->Id() ) );

   //Add unit to training map
   m_TrainingMap.insert( make_pair( in_pUnit->Training(), in_pUnit->Id() ) );

	//Add unit to name map
	if(l_bKnowUnit) 		   
		m_NameMap.insert( make_pair( in_pUnit->Design()->Name(), in_pUnit->Id() ) );
	else
		m_NameMap.insert( make_pair( L"???", in_pUnit->Id() ) );

   //Add unit to status map
   m_StatusMap.insert( make_pair( g_ClientDAL.MilitaryStatusToString(((SP2::GUnitGroup*)in_pUnit->Group())->Status((REAL32)g_Joshua.GameTime()),in_pUnit), in_pUnit->Id() ) );

   //Add unit to sale map
   if( g_ClientDAL.IsUnitForSale(in_pUnit->Id()) )
      m_SaleMap.insert( make_pair( true, in_pUnit->Id() ) );
   else
      m_SaleMap.insert( make_pair( false, in_pUnit->Id() ) );

   //Add unit to upkeep map
   if( ((SP2::GUnitGroup*)in_pUnit->Group())->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID() )
      m_UpkeepMap.insert( make_pair( CalculateUnitUpkeep(in_pUnit->Id()), in_pUnit->Id()) );
   else
      m_UpkeepMap.insert( make_pair( -1.f, in_pUnit->Id() ) );

   //Add unit to value map
	if(l_bKnowUnit)
		m_ValueMap.insert( make_pair( in_pUnit->Design()->Cost(), in_pUnit->Id() ) );
	else
		m_ValueMap.insert( make_pair( -1.f , in_pUnit->Id() ) );
         
   //Add it to main map
   GUnitListSortData l_SortIterators;         
   m_UnitsMap.insert( make_pair( in_pUnit->Id(), l_SortIterators ) );

   m_UnitsLocked.erase(in_pUnit->Id());
}

void GUnitListWindowNew::RemoveUnit( INT32 in_iUnitID, bool in_bRemoveForModification )
{
   UpdateIterators();
   
   //Check if it currently listed in unit list
   hash_map< INT32, GUnitListSortData >::iterator l_UnitIt = m_UnitsMap.find( in_iUnitID );

   //Erase iterators related to units
   if( l_UnitIt != m_UnitsMap.end() )
   {
      m_TypeMap.erase    ( l_UnitIt->second.m_TypeIter     );
      m_QtyMap.erase     ( l_UnitIt->second.m_QtyIter      );
      m_TrainingMap.erase( l_UnitIt->second.m_TrainingIter );
      m_NameMap.erase    ( l_UnitIt->second.m_NameIter     );
      m_StatusMap.erase  ( l_UnitIt->second.m_StatusIter   );
      m_ValueMap.erase   ( l_UnitIt->second.m_ValueIter    );
      m_SaleMap.erase    ( l_UnitIt->second.m_SaleIter     );
      m_UpkeepMap.erase  ( l_UnitIt->second.m_UpkeepIter   );

      m_UnitsMap.erase( l_UnitIt );
   }

   m_UnitsLocked.erase(in_iUnitID);

   //Erase the unit if it was currently shown
   vector< INT32 >::iterator l_ShownUnitIt = find( m_ShownUnits.begin(), m_ShownUnits.end(), in_iUnitID );
   if( l_ShownUnitIt != m_ShownUnits.end() )
   {
      m_ShownUnits.erase( l_ShownUnitIt );
   }

   // Update the listbox selected row only if thats we really want
   vector<INT32>::const_iterator l_It = find(m_ShownUnits.begin(), m_ShownUnits.end(), in_iUnitID);
   if( l_It != m_ShownUnits.end() && !in_bRemoveForModification )
   {
      m_pObjSellBtn->Enabled(false);
      m_pObjTrainBtn->Enabled(false);
      m_pObjDisbandBtn->Enabled(false);

      if(l_It != m_ShownUnits.end())
      {
         UnselectRow( (INT32)(l_It - m_ShownUnits.begin()) );

		 vector<INT32>::iterator l_ItSel;
         for(l_ItSel = m_viSelectedUnits.begin();
            l_ItSel != m_viSelectedUnits.end(); l_ItSel++)
         {
            if( *l_ItSel == in_iUnitID )
            {
               break;
            }
         }
         gassert(l_ItSel != m_viSelectedUnits.end(), "Unit to remove could not be found");
         m_viSelectedUnits.erase(l_ItSel);
      }
   }
}

void GUnitListWindowNew::SellUnit(UINT32 in_iUnitId)
{
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTransfertUnit);
   SP2::Event::GTransfertUnit* l_pTransfertUnit = (SP2::Event::GTransfertUnit*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pTransfertUnit->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pTransfertUnit->m_iSource = g_SP2Client->Id();
   l_pTransfertUnit->m_bForSale= true;
   l_pTransfertUnit->m_iQty    = 0;
   l_pTransfertUnit->m_iUnitId = in_iUnitId;

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	
}

REAL32 GUnitListWindowNew::CalculateUnitUpkeep( INT32 in_iUnitID )
{
   REAL32 l_fCountryModifier;

   SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_iUnitID);
   if(!l_pUnit)
      return -1;
   
   SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)l_pUnit->Group();
   if(!l_pUnitGroup)
      return -1;

   bool l_bUnitInTraining = false;
   bool l_bUnitForSale = false;

   list<UINT32>::iterator l_It = find( g_ClientDAL.UnitsForSale().begin(), g_ClientDAL.UnitsForSale().end(), in_iUnitID);
   if(l_It != g_ClientDAL.UnitsForSale().end())
      l_bUnitForSale = true;

   hash_map<UINT32, GUnitInTraining>::const_iterator l_It2 = g_ClientDAL.UnitInTraining().find(in_iUnitID);
   if(l_It2 != g_ClientDAL.UnitInTraining().end())
      l_bUnitInTraining = true;

	switch(l_pUnitGroup->Status())
	{
	case SP2::EMilitaryStatus::Ready:
	case SP2::EMilitaryStatus::Attacking:
	case SP2::EMilitaryStatus::Moving:
		l_fCountryModifier = SP2::READY;
		break;
	case SP2::EMilitaryStatus::Fortified:
		l_fCountryModifier = SP2::FORTIFIED;
		break;
	case SP2::EMilitaryStatus::Parked:
		l_fCountryModifier = SP2::PARKED;
		break;
	default:
		l_fCountryModifier = SP2::PARKED;
		break;
   }

	l_fCountryModifier *= (g_ClientDAL.m_PlayerCountryData.HumanDevelopment() + SP2::HUMANDEVMOD);

	if(g_ClientDAL.m_PlayerCountryData.GvtType() == EGovernmentType::MilitaryDictatorship)
		l_fCountryModifier *= SP2::c_fGvtTypeMilitaryUpkeepRebates;

	REAL32 l_fUnitModifier = 1.f;
	switch(l_pUnit->Training())
	{
	case ETrainingLevel::Recruit:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierRecruit; break;
	case ETrainingLevel::Regular:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierRegular; break;
	case ETrainingLevel::Veteran:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierVeteran; break;
	case ETrainingLevel::Elite:
		l_fUnitModifier = SP2::c_fUnitUpkeepModifierElite; break;
   default:
      gassert(0,"Unhandled unit training");
	}

   if(l_bUnitForSale)
	   l_fUnitModifier *= SP2::FORSALE;
   else if(l_bUnitInTraining)
		l_fUnitModifier *= SP2::TRAINING;

   return((SP2::GUnitDesign*)l_pUnit->Design())->Cost() * l_pUnit->Qty() * (l_fUnitModifier * l_fCountryModifier);
}