/**************************************************************
*
* sp2_trade_detailed_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

// our file header
#include "../include/sp2_trade_detailed_window.h"

// headers
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_frame_cursor_meter.h"

// controls defines 
#define LISTITEM_HEIGHT       14
#define LISTITEM_MAX          10
#define CBO_DROPSIZE_MAX      142
#define TAB_REGIONS_IDX       0
#define TAB_TECHNOLOGIES_IDX  1
#define TAB_MONEY_IDX         2
#define TAB_UNITS_IDX         3
#define STRID_LEVEL           102317

// window type name
const GString GTradeDetailedWindow::TypeName(L"Trade_Detailed_Window");

/*!
 * Window allocator
 **/
GUI::GBaseObject* GTradeDetailedWindow::New()
{
	return (GUI::GBaseObject*)new GTradeDetailedWindow();
}

/*!
 * Constructor
 **/
GTradeDetailedWindow::GTradeDetailedWindow() : GBaseWindow()
{
   // Init our objects data
   m_pObjInnerFrm               = NULL;
   m_pFrmProposal               = NULL;
   m_pFrmRequest                = NULL;
   m_pFrmProposalItems          = NULL;
   m_pFrmRequestItems           = NULL;
   m_pFrmBalance                = NULL;
   m_pCboCountryRequest         = NULL;
   m_pCboRegionsRequest         = NULL;
   m_pCboRegionsProposal        = NULL;
   m_pEdtMoneyProposalVal       = NULL;
   m_pEdtMoneyRequestVal        = NULL;
   m_pEdtMoneyProposal          = NULL;
   m_pEdtMoneyRequest           = NULL;
   m_pLstBoxProposal            = NULL;
   m_pLstBoxRequest             = NULL;
   m_pTabRequest                = NULL;
   m_pTabProposal               = NULL;
   m_pBtnAddProposalRegion      = NULL;
   m_pBtnAddProposalMoney       = NULL;
   m_pBtnAddRequestedRegion     = NULL;
   m_pBtnAddRequestedMoney      = NULL;
   m_pBtnRemoveSelectedProposal = NULL;
   m_pBtnRemoveSelectedRequest  = NULL;
   m_pBtnRemoveAllProposal      = NULL;
   m_pBtnRemoveAllRequest       = NULL;
   m_pBtnPropose                = NULL;
   m_pBtnCancel                 = NULL;
   m_pLblCountryProposal        = NULL;
   m_pLblTotalProposal          = NULL;
   m_pLblTotalRequest           = NULL;
   m_pLblProposalRegionVal      = NULL;
   m_pLblRequestRegionVal       = NULL;
   m_pLblProposalMoneyVal       = NULL;
   m_pLblRequestMoneyVal        = NULL;
   m_pPicCountryProposal        = NULL;     
   m_pPicCountryRequest         = NULL;
   m_pObjTradeOverviewMeter     = NULL;
   m_pBtnAccept                 = NULL;
   m_pBtnRefuse                 = NULL;
   m_pCboTechTypeRequest        = NULL;
   m_pCboTechNameRequest        = NULL;
   m_pCboTechLevelRequest       = NULL;
   m_pCboTechTypeProposal       = NULL;
   m_pCboTechNameProposal       = NULL;
   m_pCboTechLevelProposal      = NULL;
   m_pBtnAddRequestedTech       = NULL;
   m_pBtnAddProposalTech        = NULL;
   m_pLblProposalTechVal        = NULL;
   m_pLblRequestTechVal         = NULL;

   // Init member data
   m_tradeData.Init();
   m_RegionsValues.clear();
   ResetCountryTechLevels(true);
   ResetCountryTechLevels(false);
   m_mSideARegionsRows.clear();
   m_mSideBRegionsRows.clear();
   m_mSideATechRows.clear();
   m_mSideBTechRows.clear();
   m_pSideARowMoney = NULL;
   m_pSideBRowMoney = NULL;
   m_iLastSelectedSideARowId = -1;
   m_iLastSelectedSideBRowId = -1;
}

/*!
 * Destructor
 **/
GTradeDetailedWindow::~GTradeDetailedWindow()
{
}

/*!
 * Called when the object is created
 **/
bool GTradeDetailedWindow::OnCreate()
{
   //! Retrieve objects pointers
	__super::OnCreate();   

   //! Frames
   m_pObjInnerFrm              = (GUI::GFrame*)Child(L"frmInner");
   m_pFrmProposal              = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmSideA");
   m_pFrmRequest               = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmSideB");
   m_pFrmProposalItems         = (GUI::GFrame*)m_pFrmProposal->Child(L"frmItems");
   m_pFrmRequestItems          = (GUI::GFrame*)m_pFrmRequest->Child(L"frmItems");
   m_pFrmBalance               = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmBalance");

   //! Tabs
   m_pTabProposal              = (GUI::GTabbed*)m_pFrmProposal->Child(L"tabGroups"); 
   m_pTabRequest               = (GUI::GTabbed*)m_pFrmRequest->Child(L"tabGroups");   

   //! Tabs frames
   // regions
   GUI::GFrame *l_pFrmTabProRegion = (GUI::GFrame*)m_pTabProposal->m_vTabPage[TAB_REGIONS_IDX];   
   GUI::GFrame *l_pFrmTabReqRegion = (GUI::GFrame*)m_pTabRequest->m_vTabPage[TAB_REGIONS_IDX];
   // techs
   GUI::GFrame *l_pFrmTabProTech   = (GUI::GFrame*)m_pTabProposal->m_vTabPage[TAB_TECHNOLOGIES_IDX];   
   GUI::GFrame *l_pFrmTabReqTech   = (GUI::GFrame*)m_pTabRequest->m_vTabPage[TAB_TECHNOLOGIES_IDX];
   // money
   GUI::GFrame *l_pFrmTabProMoney  = (GUI::GFrame*)m_pTabProposal->m_vTabPage[TAB_MONEY_IDX];
   GUI::GFrame *l_pFrmTabReqMoney  = (GUI::GFrame*)m_pTabRequest->m_vTabPage[TAB_MONEY_IDX];
   
   //! Country combo box
   m_pCboCountryRequest        = (SP2::GComboListBoxLess*)m_pFrmRequest->Child(L"cboCountry");

   //! Region Combo boxes
   m_pCboRegionsProposal       = (GUI::GComboBox*)l_pFrmTabProRegion->Child(L"cboRegion");
   m_pCboRegionsRequest        = (GUI::GComboBox*)l_pFrmTabReqRegion->Child(L"cboRegion"); 

   //! Technologies combo
   m_pCboTechTypeProposal      = (GUI::GComboBox*)l_pFrmTabProTech->Child(L"cboType");
   m_pCboTechNameProposal      = (GUI::GComboBox*)l_pFrmTabProTech->Child(L"cboTechnology");
   m_pCboTechLevelProposal     = (GUI::GComboBox*)l_pFrmTabProTech->Child(L"cboLevel");
   m_pCboTechTypeRequest       = (GUI::GComboBox*)l_pFrmTabReqTech->Child(L"cboType");
   m_pCboTechNameRequest       = (GUI::GComboBox*)l_pFrmTabReqTech->Child(L"cboTechnology");
   m_pCboTechLevelRequest      = (GUI::GComboBox*)l_pFrmTabReqTech->Child(L"cboLevel");

   //! Updatable values
   m_pEdtMoneyProposalVal      = (SP2::GUpdatableValue*)l_pFrmTabProMoney->Child(L"frmMoney");
   m_pEdtMoneyRequestVal       = (SP2::GUpdatableValue*)l_pFrmTabReqMoney->Child(L"frmMoney"); 

   //! Editboxes
   m_pEdtMoneyProposal         = (SP2::GEditBoxNumeric*)m_pEdtMoneyProposalVal->Child(L"edtText");   
   m_pEdtMoneyRequest          = (SP2::GEditBoxNumeric*)m_pEdtMoneyRequestVal->Child(L"edtText");  

   //! Listboxes
   m_pLstBoxProposal           = (GUI::GListBox*)m_pFrmProposalItems->Child(L"lstItems");
   m_pLstBoxRequest            = (GUI::GListBox*)m_pFrmRequestItems->Child(L"lstItems");   

   //! Buttons
   m_pBtnAddProposalRegion      = (GUI::GButton*)l_pFrmTabProRegion->Child(L"btnAdd");
   m_pBtnAddProposalMoney       = (GUI::GButton*)l_pFrmTabProMoney->Child(L"btnAdd");
   m_pBtnAddProposalTech        = (GUI::GButton*)l_pFrmTabProTech->Child(L"btnAdd");
   m_pBtnAddRequestedRegion     = (GUI::GButton*)l_pFrmTabReqRegion->Child(L"btnAdd");
   m_pBtnAddRequestedMoney      = (GUI::GButton*)l_pFrmTabReqMoney->Child(L"btnAdd");
   m_pBtnAddRequestedTech       = (GUI::GButton*)l_pFrmTabReqTech->Child(L"btnAdd");
   m_pBtnRemoveSelectedProposal = (GUI::GButton*)m_pFrmProposalItems->Child(L"btnRemoveSelected");
   m_pBtnRemoveSelectedRequest  = (GUI::GButton*)m_pFrmRequestItems->Child(L"btnRemoveSelected");
   m_pBtnRemoveAllProposal      = (GUI::GButton*)m_pFrmProposalItems->Child(L"btnRemoveAll");
   m_pBtnRemoveAllRequest       = (GUI::GButton*)m_pFrmRequestItems->Child(L"btnRemoveAll");
   m_pBtnPropose                = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnPropose");
   m_pBtnCancel                 = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");
   m_pBtnAccept                 = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnAccept");
   m_pBtnRefuse                 = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCounterOffer1");

   //! Labels
   m_pLblCountryProposal        = (GUI::GLabel*)m_pFrmProposal->Child(L"txtCountry");
   m_pLblTotalProposal          = (GUI::GLabel*)m_pFrmBalance->Child(L"txtPriceA");
   m_pLblTotalRequest           = (GUI::GLabel*)m_pFrmBalance->Child(L"txtPriceB");
   m_pLblProposalRegionVal      = (GUI::GLabel*)l_pFrmTabProRegion->Child(L"txtPrice");
   m_pLblRequestRegionVal       = (GUI::GLabel*)l_pFrmTabReqRegion->Child(L"txtPrice");
   m_pLblProposalMoneyVal       = (GUI::GLabel*)l_pFrmTabProMoney->Child(L"txtPrice");
   m_pLblRequestMoneyVal        = (GUI::GLabel*)l_pFrmTabReqMoney->Child(L"txtPrice");
   m_pLblProposalTechVal        = (GUI::GLabel*)l_pFrmTabProTech->Child(L"txtPrice");
   m_pLblRequestTechVal         = (GUI::GLabel*)l_pFrmTabReqTech->Child(L"txtPrice");

   //! Picture boxes
   m_pPicCountryProposal        = (GUI::GPictureBox*)m_pFrmProposal->Child(L"picFlag");     
   m_pPicCountryRequest         = (GUI::GPictureBox*)m_pFrmRequest->Child(L"picFlag");

   //! Background colors
   m_pPicCountryRequest ->ColorNormal(L"RGBA: 20, 20, 20", L"255");
   m_pPicCountryProposal->ColorNormal(L"RGBA: 20, 20, 20", L"255");

   m_pPicCountryRequest->BringToFront();
   m_pPicCountryProposal->BringToFront();

   //! Trade overview meter
   m_pObjTradeOverviewMeter = (GBattleOverviewMeter*)m_pFrmBalance->Child(L"frmOverviewMeter");

   // Init objects display
   InitTradeTabControl(m_pTabRequest         );
   InitTradeTabControl(m_pTabProposal        );
   InitTradeMoneyEdit (m_pEdtMoneyProposalVal);
   InitTradeMoneyEdit (m_pEdtMoneyRequestVal );

   // Align labels
   m_pLblTotalRequest     ->TextAlignment(L"MiddleCenter");
   m_pLblTotalProposal    ->TextAlignment(L"MiddleCenter");
   m_pLblProposalRegionVal->TextAlignment(L"MiddleCenter");
   m_pLblRequestRegionVal ->TextAlignment(L"MiddleCenter");
   m_pLblProposalMoneyVal ->TextAlignment(L"MiddleCenter");
   m_pLblRequestMoneyVal  ->TextAlignment(L"MiddleCenter");   
   m_pLblProposalTechVal  ->TextAlignment(L"MiddleCenter");
   m_pLblRequestTechVal   ->TextAlignment(L"MiddleCenter"); 

   // combo without listbox   
   m_pCboCountryRequest->SetListBox(g_ClientDDL.CountriesListBox());   

   // Hide unsued buttons if present
   if(m_pObjInnerFrm->Child(L"btnCounterOffer"))
      ((GUI::GButton*)m_pObjInnerFrm->Child(L"btnCounterOffer"))->Hide();

   // lock toggle not displayed
   m_pObjLockTog->Hide();

	return true;
}

/*!
 * Called when the object is destroyed
 **/
bool GTradeDetailedWindow::OnDestroy()
{
   return __super::OnDestroy();
}

/*!
 * Init our tab controls
 **/
void GTradeDetailedWindow::InitTradeTabControl(GUI::GTabbed* in_pTab)
{
   // for each tab page, set up our display
   for(UINT32 i = 0; i < in_pTab->m_vTabPage.size(); i++)
   {
      in_pTab->m_vTabPage[i]->ColorNormal(GColorRGBInt(0, 0, 0, 255));
      in_pTab->m_vTabPage[i]->PictureNormal(L"000647");
      in_pTab->m_vTabPage[i]->Borders(GBorders(1, 1, 1, 1));
      in_pTab->m_vTabPage[i]->Height(in_pTab->m_vTabPage[i]->Height() + 1);      
   }
   // set up tab buttons captions
   in_pTab->m_vTabButtons[TAB_REGIONS_IDX]->Text(g_ClientDAL.GetString(TradeStrings::c_StrIdRegion));
   in_pTab->m_vTabButtons[TAB_TECHNOLOGIES_IDX]->Text(g_ClientDAL.GetString(TradeStrings::c_StrIdTechnologies));
   in_pTab->m_vTabButtons[TAB_MONEY_IDX]->Text(g_ClientDAL.GetString(TradeStrings::c_StrIdMoney));
   // disable the units tab for now   
   in_pTab->m_vTabButtons[TAB_UNITS_IDX]->Hide();
}
 
/*!
 * Init our tab displays
 **/
void GTradeDetailedWindow::InitTabDisplays(GUI::GTabbed* in_pTab)
{  
   // temp event
   SDK::GUI::GEventData l_Event;
   l_Event.Mouse.Actor.Bits.Left = 1;
   // select the regions tab by default;
   in_pTab->OnMouseDown(l_Event, in_pTab->m_vTabButtons[TAB_REGIONS_IDX]);
   // regions will be toggled
   in_pTab->m_vTabButtons[TAB_REGIONS_IDX]     ->ToggleState(true);
   in_pTab->m_vTabButtons[TAB_MONEY_IDX]       ->ToggleState(false);
   in_pTab->m_vTabButtons[TAB_TECHNOLOGIES_IDX]->ToggleState(false);
}

/*!
 * Init our money editboxed control
 **/
void GTradeDetailedWindow::InitTradeMoneyEdit(SP2::GUpdatableValue* in_pUpd)
{
   // Set up steps values
   vector<UINT32> l_viStep; l_viStep.push_back(1); l_viStep.push_back(10);
   vector<REAL64> l_vfStep; l_vfStep.push_back(1000000.f); l_vfStep.push_back(10000000.f);  
   // init the updatable value behavior
   in_pUpd->TextValueObj()->TextOffset(0, 0);
   in_pUpd->TextValueObj()->TextAlignment(L"MiddleCenter");
   in_pUpd->Initialize(l_viStep, l_vfStep, L"", L" $", 0, 0, 
                       TradeValues::c_fMinMoneyToOffer, TradeValues::c_fMaxMoneyToOffer);
}
 
/*!
 * Retrieve the trade items listbox hence the side
 **/
GUI::GListBox* GTradeDetailedWindow::GetTradeItemsListBox(bool in_bProposal) const
{
   // return the appropriate listbox
   return in_bProposal ? m_pLstBoxProposal : m_pLstBoxRequest;
}

/*!
 * Init our trade listboxes
 **/
void GTradeDetailedWindow::EmptyTradeItemsListBox(bool in_bProposal)
{
   // remove rows associations
   GetRegionsRows(in_bProposal).clear();
   SetMoneyRow(in_bProposal, NULL);
   GetTechRows(in_bProposal).clear();
   // no last selected row id
   SetLastSelectedRowId(in_bProposal, -1);
   // retrieve our listbox
   GUI::GListBox* l_pListBox = GetTradeItemsListBox(in_bProposal);
   // remove items from list and refresh
   l_pListBox->Clear();
   l_pListBox->Update();
}

/*!
 * Retrieve the last selected row id from proposal flag
 **/
INT32 GTradeDetailedWindow::GetLastSelectedRowId(bool in_bProposal)
{
   return in_bProposal ? m_iLastSelectedSideARowId : m_iLastSelectedSideBRowId;
}

/*!
 * set the last selected row id from proposal flag
 **/
void GTradeDetailedWindow::SetLastSelectedRowId(bool in_bProposal, INT32 in_iRowId)
{
   in_bProposal ? m_iLastSelectedSideARowId = in_iRowId : m_iLastSelectedSideBRowId = in_iRowId;
}

/*!
 * Is this row the money row ?
 **/
bool GTradeDetailedWindow::IsMoneyRow(GUI::GListBox::GRow* in_pRow) const
{
   // retrieve the row label
   GUI::GLabel* l_pLabel = (GUI::GLabel*)in_pRow->Object()->Child(L"txtText");
   // money row ?
   return (l_pLabel->Text() == g_ClientDAL.GetString(TradeStrings::c_StrIdMoney));
}

/*!
 * Is this row a region row ?
 **/
bool GTradeDetailedWindow::IsRegionRow(GUI::GListBox::GRow* in_pRow) const
{
   // region row ?
   return ((!IsMoneyRow(in_pRow) && !IsTechRow(in_pRow)));
}

/*!
 * Is this row a tech row ?
 **/
bool GTradeDetailedWindow::IsTechRow(GUI::GListBox::GRow* in_pRow) const
{
   // tech row ?
   return ((!IsMoneyRow(in_pRow) && in_pRow->m_iAdditionalId == 0));
}

/*!
 * Set up a trade list item row info
 **/
void GTradeDetailedWindow::SetUpTradeListItemInfo(GUI::GListBox::GRow* in_pRow, GString in_strName, 
                                                  const REAL64 &in_fValue)
{
   // set up row props     
   in_pRow->m_pObject->Height(LISTITEM_HEIGHT);
   // set up text and value
   ((GUI::GLabel*)in_pRow->m_pObject->Child("txtText"))->Text(in_strName);
   UpdateRowMoneyVal(in_pRow, in_fValue);
}

/*!
 * Set up row money value
 **/
void GTradeDetailedWindow::UpdateRowMoneyVal(GUI::GListBox::GRow* in_pRow, const REAL64 &in_fValue)
{   
   // set value
   UpdateMoneyLabelValue((GUI::GLabel*)in_pRow->m_pObject->Child("txtPrice"), in_fValue);
}

/*!
 * Update the in_pLabel value label
 **/
void GTradeDetailedWindow::UpdateMoneyLabelValue(GUI::GLabel* in_pLabel, const REAL64 &in_fValue)
{
   // set up text as money and refresh
   in_pLabel->Text(GString::FormatNumber(in_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
   in_pLabel->Update();
}

/*!
 * Retrieve the additionnal id of the current row of our combo
 **/
INT32 GTradeDetailedWindow::GetCurrentAdditonnalIdFromCombo(GUI::GComboBox* in_pCbo) const
{
   // our selected id
   INT32 l_iCurrentAdditonnalId = -1;
   // retrieve the current cbo row
   GUI::GListBox::GRow* l_pRow = in_pCbo->SelectedRow();
   // valid row ?
   if(l_pRow)
   {
      // get the id from the row
      l_iCurrentAdditonnalId = l_pRow->m_iAdditionalId;
   }
   // return our id 
   return l_iCurrentAdditonnalId;
}

/*!
 * Retrieve the regions combo box
 **/
GUI::GComboBox* GTradeDetailedWindow::GetRegionsComboBox(bool in_bProposal) const
{
   // return the appropriate combobox
   return in_bProposal ? m_pCboRegionsProposal : m_pCboRegionsRequest;
}

/*!
 * Retrieve the technologies types combo box
 **/
GUI::GComboBox* GTradeDetailedWindow::GetTechTypesComboBox(bool in_bProposal) const
{
   return in_bProposal ? m_pCboTechTypeProposal : m_pCboTechTypeRequest;
}

/*!
 * Retrieve the technologies names combo box
 **/
GUI::GComboBox* GTradeDetailedWindow::GetTechNamesComboBox(bool in_bProposal) const
{
   return in_bProposal ? m_pCboTechNameProposal : m_pCboTechNameRequest;
}

/*!
 * Retrieve the technologies levels combo box
 **/
GUI::GComboBox* GTradeDetailedWindow::GetTechLevelsComboBox(bool in_bProposal) const
{
   return in_bProposal ? m_pCboTechLevelProposal : m_pCboTechLevelRequest;
}
 
/*!
 * Retrieve the money edit box
 **/
SP2::GEditBoxNumeric* GTradeDetailedWindow::GetMoneyEditBox(bool in_bProposal) const
{
   return in_bProposal ? m_pEdtMoneyProposal : m_pEdtMoneyRequest;
}

/*!
 * Retrieve the money label
 **/
GUI::GLabel* GTradeDetailedWindow::GetMoneyLabel(bool in_bProposal) const
{
   return in_bProposal ? m_pLblProposalMoneyVal : m_pLblRequestMoneyVal;
}
 
/*!
 * Retrieve the region value label
 **/
GUI::GLabel* GTradeDetailedWindow::GetRegionLabel(bool in_bProposal) const
{
   return in_bProposal ? m_pLblProposalRegionVal : m_pLblRequestRegionVal;
}

/*!
 * Retrieve the technologie value label
 **/
GUI::GLabel* GTradeDetailedWindow::GetTechnologieLabel(bool in_bProposal) const
{
   return in_bProposal ? m_pLblProposalTechVal : m_pLblRequestTechVal;
}

/*!
 * Retrieve the total trade value label
 **/
GUI::GLabel* GTradeDetailedWindow::GetTotalTradeValueLabel(bool in_bProposal) const
{
   // return the appropriate label
   return in_bProposal ? m_pLblTotalProposal : m_pLblTotalRequest;
}

/*!
 * Retrieve the money row
 **/
GUI::GListBox::GRow* GTradeDetailedWindow::GetMoneyRow(bool in_bProposal)
{
   return in_bProposal ? m_pSideARowMoney : m_pSideBRowMoney;
}

/*!
 * Set the money row
 **/
void GTradeDetailedWindow::SetMoneyRow(bool in_bProposal, GUI::GListBox::GRow* l_pRow)
{
   if(in_bProposal)
      m_pSideARowMoney = l_pRow;
   else
      m_pSideBRowMoney = l_pRow;
}

/*!
 * Retrieve the regions rows
 **/
GRegionsRows& GTradeDetailedWindow::GetRegionsRows(bool in_bProposal)
{
   return in_bProposal ? m_mSideARegionsRows : m_mSideBRegionsRows;
}

/*!
 * Retrieve the technologies rows
 **/
GTechnologiesRows& GTradeDetailedWindow::GetTechRows(bool in_bProposal)
{
   return in_bProposal ? m_mSideATechRows : m_mSideBTechRows;
}  

/*!
* Retrieve the techs values
**/
UINT8 GTradeDetailedWindow::GetCountryTechMaxLevel(bool in_bProposal, EUnitCategory::Enum in_eType, 
                                                   EUnitDesignCharacteristics::Enum in_eName)
{
   // retrieve our max level from our 2d array
   UINT8 l_iLevel = 0;
   if(in_bProposal)
   {
      l_iLevel = m_TechLevelsSideA[in_eType][in_eName];
   }
   else
   {
      l_iLevel = m_TechLevelsSideB[in_eType][in_eName];
   }
   return l_iLevel;
}
 
/*!
* Retrieve the currently selected tech value
**/
REAL64 GTradeDetailedWindow::GetCurrentTechValue(bool in_bProposal)
{
   REAL64 l_fValue = 0.f;
   // retrieve the current technologie settings
   EUnitCategory::Enum              l_eTechType  = (EUnitCategory::Enum)GetSelectedTechTypeFromCombo(in_bProposal);
   EUnitDesignCharacteristics::Enum l_eTechName  = (EUnitDesignCharacteristics::Enum)GetSelectedTechNameFromCombo(in_bProposal);
   INT32                            l_iTechLevel = GetSelectedTechLevelFromCombo(in_bProposal);
   // valid selections in combo ?
   if(l_eTechType  != EUnitCategory::Undefined              && 
      l_eTechName  != EUnitDesignCharacteristics::Undefined &&
      l_iTechLevel != TradeValues::c_iNoTechLevel)
   {
      // evaluate
      l_fValue = g_ClientDAL.GetTechValues((INT8)l_iTechLevel,  
                                           GetCountryTechMaxLevel(!in_bProposal, l_eTechType, l_eTechName));
   }
   // return our value
   return l_fValue;
}

/*!
* Reset a coutries tech values
**/
void GTradeDetailedWindow::ResetCountryTechLevels(bool in_bProposal)
{
   if(in_bProposal)
   {
      memset(m_TechLevelsSideA,0,EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
   }
   else
   {
      memset(m_TechLevelsSideB,0,EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
   }
}

/*!
 * Called when the object is displayed
 **/
void GTradeDetailedWindow::OnShow()
{
   __super::OnShow();

   // Bring to front main bar and mini map
   if(g_ClientDDL.MainBar())
      g_ClientDDL.MainBar()->BringToFront();
   if(g_ClientDDL.MiniMap())
      g_ClientDDL.MiniMap()->BringToFront();

   // watch for canceling or approving events
   g_ClientDCL.ClientAdvisor().Attach(this, EClientAdvisorNotification::EventWasCanceled);
   g_ClientDCL.ClientAdvisor().Attach(this, EClientAdvisorNotification::EventIsOk);
   // init our data on display
   m_tradeData.Init();
   // init country on side a 
   InitProposalCountry(g_ClientDAL.ControlledCountryID());
   // reset trade on proposal side
   ResetTrade(true);
   // init country on side b
   InitRequestCountry(TradeValues::c_iNoCountry);
   // reset trade on proposal side
   ResetTrade(false);
   // we can edit
   EnableWhatCanBeDone();
   // reset tab controls displays
   InitTabDisplays(m_pTabRequest);
   InitTabDisplays(m_pTabProposal);
}

/*!
 * Called when the object is hidden
 **/
void GTradeDetailedWindow::OnHide()
{
   __super::OnHide();
   // watch for canceling event and approving events
   g_ClientDCL.ClientAdvisor().Detach(this, EClientAdvisorNotification::EventWasCanceled);
   g_ClientDCL.ClientAdvisor().Detach(this, EClientAdvisorNotification::EventIsOk);
}

/*!
 * Observer on subject change, used for the generic message window notification
 **/
void GTradeDetailedWindow::OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                                           const Hector::DesignPattern::GSubjectNotification& in_Notification,
                                           void* in_pAdditionalMsg)
{
   // from the advisor ?
   if(&in_Subject == &g_ClientDCL.ClientAdvisor())
   {
      // trade proposal event ?
      if((UINT32)in_pAdditionalMsg == GAME_EVENT_TYPE(SP2::Event::GEventTradeProposal))
      {
         // event wont be send ?
         if(in_Notification == (DesignPattern::GSubjectNotification)EClientAdvisorNotification::EventWasCanceled)
         {
            // cancel the trade
            CancelTrade(false);
         }
         else
         {
            // window will be closed since our trade proposal event was fired
            Close();
         }
      }
   }
}

/*!
 * Close our window
 **/
void GTradeDetailedWindow::Close()
{
   Hide();
}
 
/*!
 * Set the window in read only ?
 **/
void GTradeDetailedWindow::EnableWhatCanBeDone()
{
   // can we add things in the trade ?
   bool l_bCanPlay = false;
   if(m_tradeData.Status() == ETradeStatus::None)
   {
      l_bCanPlay = true;
   }
   //! Enable buttons if we can modify the trade
   m_pBtnAddProposalRegion     ->Enabled(l_bCanPlay);
   m_pBtnAddProposalMoney      ->Enabled(l_bCanPlay);
   m_pBtnAddProposalTech       ->Enabled(l_bCanPlay);
   m_pBtnAddRequestedRegion    ->Enabled(l_bCanPlay);
   m_pBtnAddRequestedMoney     ->Enabled(l_bCanPlay);
   m_pBtnAddRequestedTech      ->Enabled(l_bCanPlay);
   m_pBtnRemoveSelectedProposal->Enabled(l_bCanPlay);
   m_pBtnRemoveSelectedRequest ->Enabled(l_bCanPlay);
   m_pBtnRemoveAllProposal     ->Enabled(l_bCanPlay);
   m_pBtnRemoveAllRequest      ->Enabled(l_bCanPlay);
   m_pCboCountryRequest        ->Enabled(l_bCanPlay);
   m_pCboRegionsRequest        ->Enabled(l_bCanPlay);
   m_pCboRegionsProposal       ->Enabled(l_bCanPlay);
   m_pEdtMoneyProposalVal      ->Enabled(l_bCanPlay);
   m_pEdtMoneyRequestVal       ->Enabled(l_bCanPlay);
   m_pCboTechTypeProposal      ->Enabled(l_bCanPlay);
   m_pCboTechNameProposal      ->Enabled(l_bCanPlay);
   m_pCboTechLevelProposal     ->Enabled(l_bCanPlay);
   m_pCboTechTypeRequest       ->Enabled(l_bCanPlay);
   m_pCboTechNameRequest       ->Enabled(l_bCanPlay);
   m_pCboTechLevelRequest      ->Enabled(l_bCanPlay);
   // never able to propose at first
   m_pBtnPropose               ->Enabled(false);
   // show or hide with what we want
   if(l_bCanPlay)
   {
      // we are proposing a trade
      m_pBtnAccept ->Hide();
      m_pBtnRefuse ->Hide();
      m_pBtnPropose->Show();
      m_pBtnCancel ->Show();
   }
   else
   {
      // wont be able to propose in other states
      m_pBtnPropose->Hide();      
      if(m_tradeData.Status() != ETradeStatus::Pending)
      {
         // if not pending, means really read only
         m_pBtnAccept->Hide();
         m_pBtnRefuse->Hide();         
         m_pBtnCancel->Show();
      }
      else
      {
         // can accept or refuse it
         m_pBtnAccept->Show();
         m_pBtnRefuse->Show();
         m_pBtnCancel->Hide();
      }
   }
}

/*!
 * Show a trade proposal 
 **/
void GTradeDetailedWindow::ShowTradeProposal(const GTradeData &in_tradeData)
{
   // Show it if not visible
   if(!Visible())
      Show();
   // assign new window data
   m_tradeData = in_tradeData;
   // our country info
   InitProposalCountry(m_tradeData.GetTradeDataFromSide(true).CountryId());
   // country we want to trade with info
   InitRequestCountry(m_tradeData.GetTradeDataFromSide(false).CountryId());
   // Update data on the screen
   Update();
}

/*!
 * Called when the mouse is pressed down
 **/
EEventStatus::Enum GTradeDetailedWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return __super::OnMouseDown(in_EventData, in_pCaller);
}

/*!
 * Called when the mouse is moved
 **/
EEventStatus::Enum GTradeDetailedWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the mouse is up
 **/
EEventStatus::Enum GTradeDetailedWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the object is clicked
 **/
EEventStatus::Enum GTradeDetailedWindow::OnMouseClick(const GEventData &in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {         
         // Clicked on the exit or cancel button ?
         if(in_pCaller == m_pObjExitBtn || in_pCaller == m_pBtnCancel)
         {
            // will refuse the trade if appropriate
            CancelTrade(true);
         }
         // add a region to proposal
         else if(in_pCaller == m_pBtnAddProposalRegion)
         {
            TryAddRegionToTrade(true);
         }
         // add a region to requested
         else if(in_pCaller == m_pBtnAddRequestedRegion)
         {
            TryAddRegionToTrade(false);    
         }
         // add money to proposal
         else if(in_pCaller == m_pBtnAddProposalMoney)
         {
            TryAddMoneyToTrade(true);
         }
         // add money to requested
         else if(in_pCaller == m_pBtnAddRequestedMoney)
         {
            TryAddMoneyToTrade(false);
         }
         // add tech to proposal
         else if(in_pCaller == m_pBtnAddProposalTech)
         {
            TryAddTechnologieToTrade(true);
         }
         // add tech to requested
         else if(in_pCaller == m_pBtnAddRequestedTech)
         {
            TryAddTechnologieToTrade(false);
         }
         // remove selected item from the proposal
         else if(in_pCaller == m_pBtnRemoveSelectedProposal)
         {
            RemoveItemFromTrade(true);
         }
         // remove selected item from the requested
         else if(in_pCaller == m_pBtnRemoveSelectedRequest)
         {
            RemoveItemFromTrade(false);
         }
         // remove all from the proposal 
         else if(in_pCaller == m_pBtnRemoveAllProposal)
         {
            ResetTrade(true);
         }
         // remove all from the requested 
         else if(in_pCaller == m_pBtnRemoveAllRequest)
         {
            ResetTrade(false);
         }
         // propose selected button 
         else if(in_pCaller == m_pBtnPropose)
         {
            ProposeTrade();
         }
         // accept selected button 
         else if(in_pCaller == m_pBtnAccept)
         {
            AcceptTrade();
         }
         // refuse selected button 
         else if(in_pCaller == m_pBtnRefuse)
         {
            CancelTrade(true);
         }
      }
   }
   return GUI::EEventStatus::Handled;
}

/*!
 * Called when a custom event happens
 **/
EEventStatus::Enum GTradeDetailedWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData &in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      switch(in_EventId)
      {
         // list item selected
         case ECustomEvents::OnListItemSelected:
         {
            if(in_pCaller == m_pLstBoxProposal || in_pCaller == m_pLstBoxRequest)
            {
               // set up list box
               GUI::GListBox* l_pLstBox = (GUI::GListBox*)in_pCaller;
               // temp row obj
               GUI::GListBox::GRow* l_pRow = NULL;
               // no last selected row id
               INT32 l_iLastId = GetLastSelectedRowId(in_pCaller == m_pLstBoxProposal);
               // unselect previous selection
               if(l_iLastId >= 0)
               {
                  l_pRow = l_pLstBox->Get_Row_At(l_iLastId);
                  if(l_pRow)
                  {
                     // invalid id
                     SetLastSelectedRowId(in_pCaller == m_pLstBoxProposal, -1);
                     // unhighlight it
                     ((GUI::GLabel*)l_pRow->Object()->Child(L"txtText"))->PictureNormal(L"000587");
                     ((GUI::GLabel*)l_pRow->Object()->Child(L"txtPrice"))->PictureNormal(L"000587");
                  }
               }
               // retrieve the selected item
               if(l_pLstBox->Get_Selected_Item().size())
               {
                  l_pRow = l_pLstBox->Get_Row_At(l_pLstBox->Get_Selected_Item()[0]);
                  // if really valid
                  if(l_pRow)
                  {
                     // set new id
                     SetLastSelectedRowId(in_pCaller == m_pLstBoxProposal, l_pLstBox->Get_Selected_Item()[0]);
                     // highlight it
                     ((GUI::GLabel*)l_pRow->Object()->Child(L"txtText"))->PictureNormal(L"000647");
                     ((GUI::GLabel*)l_pRow->Object()->Child(L"txtPrice"))->PictureNormal(L"000647");
                  }
               }
            }
            break;
         }
         // money value changed
         case c_OnUpdatableValueChange:
         {
            // from the proposal ?
            if(in_pCaller == m_pEdtMoneyProposalVal)
            {
               // update data on screen
               UpdateMoneyLabelValue(m_pLblProposalMoneyVal, m_pEdtMoneyProposal->Value());
            }
            // from the request ?
            else if(in_pCaller == m_pEdtMoneyRequestVal)
            {
               // update data on screen
               UpdateMoneyLabelValue(m_pLblRequestMoneyVal, m_pEdtMoneyRequest->Value());
            }
            break;
         }
         // combo box item change
         case ECustomEvents::OnComboBoxSelectionEvent:
         {
            // country combo box changes ?
            if(in_pCaller == m_pCboCountryRequest)
            {               
               // retrieve the selected country id in the combo and init 
               InitRequestCountry(GetTradeCountryId(false));  
               // reset the trade for the request side
               ResetTrade(false);
            }
            // regions proposal combo box changes ?
            else if(in_pCaller == m_pCboRegionsProposal)
            {
               INT32 l_iRegionId = GetSelectedRegionIdFromCombo(true);
               // update data on screen
               UpdateMoneyLabelValue(m_pLblProposalRegionVal, GetCurrentRegionValue(l_iRegionId));
            }
            // regions request combo box changes ?
            else if(in_pCaller == m_pCboRegionsRequest)
            {
               INT32 l_iRegionId = GetSelectedRegionIdFromCombo(false);
               // update data on screen
               UpdateMoneyLabelValue(m_pLblRequestRegionVal, GetCurrentRegionValue(l_iRegionId));
            }
            else if(in_pCaller == m_pCboTechTypeProposal || in_pCaller == m_pCboTechNameProposal)
            {
               UpdateTechLevelsDisplay(true);
            }
            else if(in_pCaller == m_pCboTechTypeRequest || in_pCaller == m_pCboTechNameRequest)
            {
               UpdateTechLevelsDisplay(false);
            }
            else if(in_pCaller == m_pCboTechLevelProposal)
            {
               UpdateMoneyLabelValue(m_pLblProposalTechVal, GetCurrentTechValue(true));
            }
            else if(in_pCaller == m_pCboTechLevelRequest)
            {
               UpdateMoneyLabelValue(m_pLblRequestTechVal, GetCurrentTechValue(false));
            }
            break;            
         }   
         // unhandled custom event
         default:
            return EEventStatus::NotHandled;
      }
   }
   else if(in_EventId == ECustomEvents::OnComboBoxBeforeOpen)
   {
      // refill the countries list box if needed
      if(in_EventData.Data == m_pCboCountryRequest)
      {
         g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), 
                                                   (GUI::GComboBox*)in_EventData.Data, true);
      }
   }      
   return EEventStatus::Handled;
}

/*!
 * Init our proposal Country side
 **/
void GTradeDetailedWindow::InitProposalCountry(INT16 in_iCountryId)
{
   // set up country info
   m_tradeData.GetTradeDataFromSide(true).CountryId(in_iCountryId);
   // Set up country look information in window
   m_pLblCountryProposal->Text(g_ClientDAL.Country(in_iCountryId).Name());
   m_pPicCountryProposal->PictureNormal(g_ClientDDL.SmallFlagsTextures()[in_iCountryId]);   
   m_pPicCountryProposal->PictureBackground( m_pPicCountryProposal->PictureNormal() );
   m_pLblCountryProposal->Update(); 
   m_pPicCountryProposal->BringToFront();
   // comboes will only be fed if we start the trade
   if(m_tradeData.Status() == ETradeStatus::None)
   {
      // feed regions combos
      FeedTradeRegionsCombo(true);
      // feed the technologies combo
      FeedTradeTechnologiesCombo(true);
   }
}

/*!
 * Set the country we want to trade with
 **/
void GTradeDetailedWindow::InitRequestCountry(INT16 in_iCountryId)
{
   // set up our requested country
   m_tradeData.GetTradeDataFromSide(false).CountryId(in_iCountryId);
   // comboes will only be fed if we start the trade
   if(m_tradeData.Status() == ETradeStatus::None)
   {
      // feed regions combos
      FeedTradeRegionsCombo(false);
      // feed the technologies combo
      FeedTradeTechnologiesCombo(false);
   }
   // Edit box offset
   m_pCboCountryRequest->GetEditBox()->TextOffset(25, 0);
   // no country ?
   if(in_iCountryId == TradeValues::c_iNoCountry)
   {
      // inform user to pick a country         
      m_pCboCountryRequest->Selected_Content(g_ClientDAL.GetString(TradeStrings::c_StrIdSelectCountry));
      m_pPicCountryRequest->PictureClear();
   }         
   else
   {      
      // set requested country flag and name
      m_pCboCountryRequest->Selected_Content(g_ClientDAL.Country(in_iCountryId).Name());
      m_pPicCountryRequest->PictureNormal(g_ClientDDL.SmallFlagsTextures()[in_iCountryId]);
      m_pPicCountryRequest->PictureBackground( m_pPicCountryRequest->PictureNormal() );
   }      
   m_pCboCountryRequest->Update();            
}

/*!
 * Retrieve the currently selected country in our country combo
 **/
INT16 GTradeDetailedWindow::GetTradeCountryId(bool in_bProposal)
{
   // our selected country id
   INT16 l_iCurrentRequestedCountryId = TradeValues::c_iNoCountry;
   // the country id of the proposal ?
   if(in_bProposal)
   {
      // our country
      l_iCurrentRequestedCountryId = g_ClientDAL.ControlledCountryID();
   }
   // country id from request
   else
   {
      // retrieve the current row
      GUI::GListBox::GRow* l_pRow = m_pCboCountryRequest->SelectedRow();
      // valid row ?
      if(l_pRow)
      {
         // get the id from the row
         l_iCurrentRequestedCountryId = (INT16)l_pRow->m_iAdditionalId;
      }
   }
   // return our id 
   return l_iCurrentRequestedCountryId;
}

/*!
 * Init our region comboboxes control
 **/
void GTradeDetailedWindow::FeedTradeRegionsCombo(bool in_bProposal)
{
   // our region combobox
   GUI::GComboBox *l_pCbo = GetRegionsComboBox(in_bProposal);
   // our country id
   INT16 l_iCountryId     = m_tradeData.GetTradeDataFromSide(in_bProposal).CountryId();
   // clear current selection
   l_pCbo->GetListBox()->ClearSelection();
   l_pCbo->Clear();
   // if no country selected
   if(l_iCountryId == TradeValues::c_iNoCountry)
   {      
      // Select a region
      l_pCbo->Selected_Content(g_ClientDAL.GetString(TradeStrings::c_StrIdSelectCountry));
   } 
   // we got a country
   else
   {
      l_pCbo->Selected_Content(g_ClientDAL.GetString(TradeStrings::c_StrIdSelectRegion));
      //retrieve the region of the country capital
      UINT32 l_CapitlaRegionId = g_ClientDAL.CapitalRegion(l_iCountryId);
      // map will be alphabetically ordered by region name
      multimap<GString, UINT32> l_Regions;
      // retrieve the regions controlled by the country
      const set<UINT32> &l_vPoliticalControls = g_ClientDAL.CountryPoliticalControl(l_iCountryId);
      // loop into every regions
      for(set<UINT32>::const_iterator l_It = l_vPoliticalControls.begin(); l_It != l_vPoliticalControls.end(); l_It++)
      {         
         // only possible to trade if we also have military control amd if its not our capital region
         if(g_ClientDAL.RegionControl(*l_It).m_iMilitary == l_iCountryId && (*l_It != l_CapitlaRegionId))
         {
            l_Regions.insert(multimap<GString, UINT32>::value_type(g_ClientDAL.RegionName(*l_It),*l_It));
         }
      }
      // loop into every regions
	   for(multimap<GString, UINT32>::const_iterator l_It = l_Regions.begin(); l_It != l_Regions.end(); l_It++)
	   {
         // add in the cbo the region name with the region id as additonnal info
         l_pCbo->Add_Item(l_It->first, l_It->second);
      }     
      // retrieve regions values from server
      RetrieveRegionsValues(l_iCountryId);
   }
   // Set up our combo drop sizes
   if(l_pCbo->NbItems() >= LISTITEM_MAX)
      l_pCbo->DropSize(CBO_DROPSIZE_MAX);
   else
      l_pCbo->DropSize((l_pCbo->NbItems() * LISTITEM_HEIGHT) + 2);
   // refresh
   l_pCbo->Update();
}

/*!
 * Init our technologies comboboxes control
 **/
void GTradeDetailedWindow::FeedTradeTechnologiesCombo(bool in_bProposal)
{
   // retrieve our technologies comboboxes
   GUI::GComboBox *l_pCboType  = GetTechTypesComboBox(in_bProposal);
   GUI::GComboBox *l_pCboName  = GetTechNamesComboBox(in_bProposal);
   // clear current selection
   l_pCboType->GetListBox()->ClearSelection();
   l_pCboName->GetListBox()->ClearSelection();   
   l_pCboType->Clear();
   l_pCboName->Clear();   
   // reset the tech values we have for this country
   ResetCountryTechLevels(in_bProposal);
   // our country id used to get technologies
   INT16 l_iCountryId = m_tradeData.GetTradeDataFromSide(in_bProposal).CountryId();
   // if no country selected
   if(l_iCountryId == TradeValues::c_iNoCountry)
   {      
      // Select a country
      l_pCboType->Selected_Content(g_ClientDAL.GetString(TradeStrings::c_StrIdSelectCountry));
      l_pCboName->Selected_Content(g_ClientDAL.GetString(TradeStrings::c_StrIdSelectCountry));      
   } 
   // we got a country
   else
   {
      // feed the types combo
      for(INT32 i = 0; i < (INT32)EUnitCategory::ItemCount; i++)
      {
         // retrieve the current type
         EUnitCategory::Enum l_eType = (EUnitCategory::Enum)i;
         // add it only if not infantry type
         if(l_eType != EUnitCategory::Infantry)
            l_pCboType->Add_Item(g_ClientDAL.GetString(SP2::c_iUnitCategoryStringID[l_eType]), i);
      }
      // feed the names combo
      for(INT32 i = 0; i < (INT32)EUnitDesignCharacteristics::ItemCount; i++)
      {
         // retrieve the current name and add it
         EUnitDesignCharacteristics::Enum l_eName = (EUnitDesignCharacteristics::Enum)i;
         l_pCboName->Add_Item(g_ClientDAL.GetString(SP2::c_iUnitDesignCharacteristicsStringID[l_eName]), i);
      }
      // retrieve the technologies values
      RetrieveTechnologiesLevels(l_iCountryId);         
   }
   // refresh the levels combo
   UpdateTechLevelsDisplay(in_bProposal);
   // Set up our combos drop sizes
   if(l_pCboType->NbItems() >= LISTITEM_MAX)
      l_pCboType->DropSize(CBO_DROPSIZE_MAX);
   else
      l_pCboType->DropSize((l_pCboType->NbItems() * LISTITEM_HEIGHT) + 2);
   if(l_pCboName->NbItems() >= LISTITEM_MAX)
      l_pCboName->DropSize(CBO_DROPSIZE_MAX);
   else
      l_pCboName->DropSize((l_pCboName->NbItems() * LISTITEM_HEIGHT) + 2);
   // refresh
   l_pCboType->Update();
   l_pCboName->Update();
}

/*!
 * Update which technology can be seen
 **/
void GTradeDetailedWindow::UpdateTechLevelsDisplay(bool in_bProposal)
{
   // Clear the level listbox
   GUI::GComboBox *l_pCboLevel = GetTechLevelsComboBox(in_bProposal);
   l_pCboLevel->GetListBox()->ClearSelection();
   l_pCboLevel->Clear();
   // retrieve the current technologie settings
   EUnitCategory::Enum              l_eTechType  = (EUnitCategory::Enum)GetSelectedTechTypeFromCombo(in_bProposal);
   EUnitDesignCharacteristics::Enum l_eTechName  = (EUnitDesignCharacteristics::Enum)GetSelectedTechNameFromCombo(in_bProposal);
   // valid selections in combo ?
   if(l_eTechType  != EUnitCategory::Undefined && 
      l_eTechName  != EUnitDesignCharacteristics::Undefined)
   {
      // our country tech level for that technologie
      UINT8 l_iMaxLevelSideA = GetCountryTechMaxLevel(true,  l_eTechType, l_eTechName);
      UINT8 l_iMaxLevelSideB = GetCountryTechMaxLevel(false, l_eTechType, l_eTechName);
      // are we feeding the techs we have
      if(in_bProposal)
      {
         // feed the levels combo
         for(UINT8 i = l_iMaxLevelSideB+1; i <= l_iMaxLevelSideA; i++)
         {
            l_pCboLevel->Add_Item(g_ClientDAL.GetString(STRID_LEVEL) + L" " + GString(i), i);
         }
      }
      // feed the techs we want if technologie exists
      else
      {
         // feed the levels combo
         for(UINT8 i = l_iMaxLevelSideA+1; i <= l_iMaxLevelSideB; i++)
         {
            l_pCboLevel->Add_Item(g_ClientDAL.GetString(STRID_LEVEL) + L" " + GString(i), i);
         }         
      }
   }
   if(l_pCboLevel->NbItems() >= LISTITEM_MAX)
      l_pCboLevel->DropSize(CBO_DROPSIZE_MAX);
   else
      l_pCboLevel->DropSize((l_pCboLevel->NbItems() * LISTITEM_HEIGHT) + 2);
   l_pCboLevel->Update();
}

/*!
 * Reset our form data
 **/
void GTradeDetailedWindow::ResetTrade(bool in_bProposal)
{
   // reset underlying data
   m_tradeData.GetTradeDataFromSide(in_bProposal).Reset();
   // refresh screen values with what we have in memory
   SyncWithDataFromMemory(in_bProposal);
}

/*!
 * Update window
 **/
bool GTradeDetailedWindow::Update()
{
   // enable hence trade status
   EnableWhatCanBeDone();
   // refresh screen values with what we have in memory
   SyncWithDataFromMemory(true);
   SyncWithDataFromMemory(false);

   return true;
}

/*!
 * Refresh screen values with what we have in memory
 **/
void GTradeDetailedWindow::SyncWithDataFromMemory(bool in_bProposal)
{
   // empty the trade items listboxes
   EmptyTradeItemsListBox(in_bProposal);
   // set up the region values
   UpdateRegionTradeValues(in_bProposal);
   // set up the money values
   UpdateMoneyTradeValues(in_bProposal);
   // set up the technologies values
   UpdateTechnologieTradeValues(in_bProposal);
   // update our total trades values
   UpdateTotalTradeValue(in_bProposal);
}
 
/*!
 * Update the region labels value
 **/
void GTradeDetailedWindow::UpdateRegionTradeValues(bool in_bProposal)
{
   GRegionsValues::const_iterator l_It;
   // retrieve trade data from requested side
   GTradeDataForOneSide &l_Trade = m_tradeData.GetTradeDataFromSide(in_bProposal);
   // loop into the regions and add them   
   for(l_It = l_Trade.GetRegions().begin(); l_It != l_Trade.GetRegions().end(); l_It++)
   {
      // find the regions in our region rows array
      GRegionsRows::iterator l_iRegInRow = GetRegionsRows(in_bProposal).find(l_It->first);
      // if list item not already present
      if(l_iRegInRow == GetRegionsRows(in_bProposal).end())
      {
         // add the region
         AddRegionToListBox(l_It->first, l_It->second, in_bProposal);
      }
      else
      {
         // update the row money value
         UpdateRowMoneyVal(l_iRegInRow->second, l_It->second);
      }
   }
   // update the appropriate region value label
   UpdateMoneyLabelValue(GetRegionLabel(in_bProposal), GetCurrentRegionValue(GetSelectedRegionIdFromCombo(in_bProposal)));
}

/*!
 * Update the technologies values
 **/
void GTradeDetailedWindow::UpdateTechnologieTradeValues(bool in_bProposal)
{
   GTechnologiesValues::iterator l_It;
   // retrieve trade data from requested side
   GTradeDataForOneSide &l_Trade = m_tradeData.GetTradeDataFromSide(in_bProposal);
   // loop into the techs and add them   
   for(l_It = l_Trade.GetTechnologies().begin(); l_It != l_Trade.GetTechnologies().end(); l_It++)
   {
      // find the tech in our tech rows array
      GTechnologiesRows::iterator l_TechInRow = GetTechRows(in_bProposal).find(l_It->first);
      // if list item not already present
      if(l_TechInRow == GetTechRows(in_bProposal).end())
      {
         // add the tech
         AddTechnologieToListBox(l_It->first, l_It->second, in_bProposal);
      }
      else
      {
         // update the row money value
         UpdateRowMoneyVal(l_TechInRow->second, l_It->second);
      }      
   }
   UpdateMoneyLabelValue(GetTechnologieLabel(in_bProposal), GetCurrentTechValue(in_bProposal));
}
 
/*!
 * Update the money labels value
 **/
void GTradeDetailedWindow::UpdateMoneyTradeValues(bool in_bProposal)
{
   // needs to add money row ?
   if(m_tradeData.GetTradeDataFromSide(in_bProposal).Money() > 0.f)
   {
      // refresh money listitem
      AddMoneyToListBox(m_tradeData.GetTradeDataFromSide(in_bProposal).Money(), in_bProposal);
   }
   UpdateMoneyLabelValue(GetMoneyLabel(in_bProposal), m_tradeData.GetTradeDataFromSide(in_bProposal).Money());
   GetMoneyEditBox(in_bProposal)->Value(m_tradeData.GetTradeDataFromSide(in_bProposal).Money());
}

/*!
 * Update our display of the total trade value
 **/
void GTradeDetailedWindow::UpdateTotalTradeValue(bool in_bProposal)
{
   // Update the total value label of the requested side
   UpdateMoneyLabelValue(GetTotalTradeValueLabel(in_bProposal), 
                         m_tradeData.GetTradeDataFromSide(in_bProposal).GetTotalValue());
   // reposition the meter
   UpdateTradeOverviewMeter();   
   // Can we propose ?
   EvaluateCanPropose();
}
 
/*!
 * Update the overview meter
 **/
void GTradeDetailedWindow::UpdateTradeOverviewMeter()
{
   // Update Trade overview meter
   m_pObjTradeOverviewMeter->Value(m_tradeData.GetTradeEvalPercentage(true), 0);
   // retrieve the separator button
   GUI::GButton* l_pButton = (GUI::GButton*)m_pObjTradeOverviewMeter->Child(L"btnSeparator1");
   // set up separator position
   l_pButton->Left(m_pObjTradeOverviewMeter->MeterBtn()[0]->Left() + 
                   m_pObjTradeOverviewMeter->MeterBtn()[0]->Width() - 2);
   l_pButton->Hide();
}

/*!
 * Retrieve the regions values from the server for a given country and feed 
 * our local map with it
 **/
void GTradeDetailedWindow::RetrieveRegionsValues(INT16 in_iCountryId)
{
   //create game event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventGetRegionsValues);
   SP2::Event::GEventGetRegionsValues* l_pUpdate = (SP2::Event::GEventGetRegionsValues*)l_pEvent.get();
   // country from wich we want the regions
   l_pUpdate->m_iCountryIdRequested = in_iCountryId;
   l_pUpdate->m_RegionsValues.clear();
   // set up source and destination
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();
   // raise it
   g_Joshua.RaiseEvent(l_pEvent);
}

/*!
 * Set the region values in our local map and update in consequence
 **/
void GTradeDetailedWindow::SetRegionsValues(const GRegionsValues &in_RegionValues)
{
   // valid region and we have its value?
   for(GRegionsValues::const_iterator l_It = in_RegionValues.begin(); 
       l_It != in_RegionValues.end(); 
       l_It++)
   {
      // set up the region id money value in memory
      m_RegionsValues[l_It->first] = l_It->second;
      // check if region is in trade for side a and update it if appropriate
      m_tradeData.GetTradeDataFromSide(true).UpdateRegionValue(l_It->first, l_It->second);
      // check if region is in trade for side b and update it if appropriate
      m_tradeData.GetTradeDataFromSide(false).UpdateRegionValue(l_It->first, l_It->second);
   }
   // Update displayed rows
   UpdateRegionTradeValues(true);
   UpdateRegionTradeValues(false);   
}

/*!
 * Retrieve the value in $ of a region on our local map
 **/
REAL64 GTradeDetailedWindow::GetCurrentRegionValue(INT32 in_iRegionId)
{
   // our region value
   REAL64 l_fRegionValue = 0.f;
   // valid region and we have its value?
   if(in_iRegionId > TradeValues::c_iNoRegion && m_RegionsValues.count(in_iRegionId) > 0)
   {
      // Retrieve the region value
      l_fRegionValue = m_RegionsValues[in_iRegionId];
   }
   // return our region value
   return l_fRegionValue;
}

/*!
 * Retrieve the currently selected region from the requested side
 **/
INT32 GTradeDetailedWindow::GetSelectedRegionIdFromCombo(bool in_bProposal)
{
   // region id in additonnal id of the current row
   return GetCurrentAdditonnalIdFromCombo(GetRegionsComboBox(in_bProposal));
}

/*!
 * Try adding a region to our trade
 **/
void GTradeDetailedWindow::TryAddRegionToTrade(bool in_bProposal)
{
   // retrieve the current region
   INT32 l_iRegionId = GetSelectedRegionIdFromCombo(in_bProposal);
   // valid region ?
   if(l_iRegionId > TradeValues::c_iNoRegion)
   {
      // yes, add it
      AddRegionToTrade(l_iRegionId, in_bProposal);            
   }
}

/*!
 * Add Region to our trade
 **/
bool GTradeDetailedWindow::AddRegionToTrade(INT32 in_iRegionId, bool in_bProposal)
{
   // did we add a region successfully ?
   bool l_bRegionAdded = false;
   // trade side used
   GTradeDataForOneSide &l_Trade = m_tradeData.GetTradeDataFromSide(in_bProposal);   
   // if region not already in trade
   if(!l_Trade.IsRegionInTrade(in_iRegionId))
   {
      // retrieve region value
      REAL64 l_fRegionValue  = GetCurrentRegionValue(in_iRegionId);
      // add into the listbox
      if(AddRegionToListBox(in_iRegionId, l_fRegionValue, in_bProposal))
      {
         // add it into our trade side
         l_Trade.AddRegion(in_iRegionId, l_fRegionValue);    
         // update the screen
         UpdateTotalTradeValue(in_bProposal);
         // region was added
         l_bRegionAdded = true;
      }
   }
   // region added ?
   return l_bRegionAdded;
}

/*!
 * Add a Region into the listbox
 **/
GUI::GListBox::GRow* GTradeDetailedWindow::AddRegionToListBox(INT32 in_iRegionId, const REAL64 &in_fRegionVal, 
                                                              bool in_bProposal)
{
   // retrieve the listbox
   GUI::GListBox* l_pLstBox = GetTradeItemsListBox(in_bProposal);
   // add it into the listbox
   GUI::GListBox::GRow *l_pRow = l_pLstBox->Insert_New_Row(in_iRegionId);
   // set region row reference
   GetRegionsRows(in_bProposal)[in_iRegionId] = l_pRow;
   // set up our list item info
   SetUpTradeListItemInfo(l_pRow, g_ClientDAL.RegionName(in_iRegionId), in_fRegionVal);      
   // refresh
   l_pLstBox->Update();
   // added ok ?
   return l_pRow;
}

/*!
 * Try adding money to our trade
 **/
void GTradeDetailedWindow::TryAddMoneyToTrade(bool in_bProposal)
{
   // retrieve the money we have
   REAL64 l_fMoney = GetMoneyEditBox(in_bProposal)->Value();
   // valid money amout
   if(l_fMoney > 0.f)
   {
      // add the money 
      AddMoneyToTrade(l_fMoney, in_bProposal);
   }
}

/*!
 * Add Money to our trade
 **/
bool GTradeDetailedWindow::AddMoneyToTrade(const REAL64 &in_fMoney, bool in_bProposal)
{
   // money added to trade ?
   bool l_bMoneyAddedToTrade = false; 
   // add the money into the list box
   if(AddMoneyToListBox(in_fMoney, in_bProposal))
   {
      // set up our new money amount
      m_tradeData.GetTradeDataFromSide(in_bProposal).Money(in_fMoney);
      // update the screen
      UpdateTotalTradeValue(in_bProposal);
      // added successfully !
      l_bMoneyAddedToTrade = true;
   }
   // added succesfully ?
   return l_bMoneyAddedToTrade;
}

/*!
 * Add money into the listbox
 **/
GUI::GListBox::GRow* GTradeDetailedWindow::AddMoneyToListBox(const REAL64 &in_fMoney, bool in_bProposal)
{
   // retrieve the listbox
   GUI::GListBox* l_pLstBox = GetTradeItemsListBox(in_bProposal);
   // is the row already in the listbox
   GUI::GListBox::GRow *l_pMoneyRow = GetMoneyRow(in_bProposal);
   // if money row not already added
   if(l_pMoneyRow == NULL)
   {
      // add it
      l_pMoneyRow = l_pLstBox->Insert_New_Row(0);
   }
   //set up our row reference
   SetMoneyRow(in_bProposal, l_pMoneyRow);
   // set up our list item info
   SetUpTradeListItemInfo(l_pMoneyRow, g_ClientDAL.GetString(TradeStrings::c_StrIdMoney), in_fMoney);
   // refresh
   l_pLstBox->Update();
   // added ok ?
   return l_pMoneyRow;
}

/*!
 * Retrieve the currently selected technologies ids from their comboes
 **/
INT32 GTradeDetailedWindow::GetSelectedTechTypeFromCombo(bool in_bProposal)
{
   // tech type id in additonnal id of the current row
   return GetCurrentAdditonnalIdFromCombo(GetTechTypesComboBox(in_bProposal));
}
INT32 GTradeDetailedWindow::GetSelectedTechNameFromCombo(bool in_bProposal)
{
   // tech name id in additonnal id of the current row
   return GetCurrentAdditonnalIdFromCombo(GetTechNamesComboBox(in_bProposal));
}
INT32 GTradeDetailedWindow::GetSelectedTechLevelFromCombo(bool in_bProposal)
{
   // tech level id in additonnal id of the current row
   return GetCurrentAdditonnalIdFromCombo(GetTechLevelsComboBox(in_bProposal));
}
 
/*!
 * Retrieve the technologies values for our country and will update
 * our local array when retrieved
 **/
void GTradeDetailedWindow::RetrieveTechnologiesLevels(INT16 in_iCountryId)
{
   //create game event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventGetTechnologiesValues);
   SP2::Event::GEventGetTechnologiesValues* l_pUpdate = (SP2::Event::GEventGetTechnologiesValues*)l_pEvent.get();
   // country from wich we want the techs
   l_pUpdate->m_iCountryIdRequested = in_iCountryId;
   // set up source and destination
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();
   // raise it
   g_Joshua.RaiseEvent(l_pEvent);
}

/*!
 * Set the local technologies values from the given map
 **/
void GTradeDetailedWindow::SetTechnologiesLevels(INT32 in_iCountryId, 
																 UINT8 in_TechnologiesLevels[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount])
{
   // is our update for one of our country ?
   if(m_tradeData.GetTradeDataFromSide(true).CountryId()  == in_iCountryId ||
      m_tradeData.GetTradeDataFromSide(false).CountryId() == in_iCountryId)
   {
      bool l_bSideACountry = m_tradeData.GetTradeDataFromSide(true).CountryId() == in_iCountryId;
      // copy our new techs levels
      if(l_bSideACountry)
      {
         memcpy(m_TechLevelsSideA, in_TechnologiesLevels, 
                EUnitCategory::ItemCount * EUnitDesignCharacteristics::ItemCount);
      }  
      else
      {
         memcpy(m_TechLevelsSideB, in_TechnologiesLevels, 
                EUnitCategory::ItemCount * EUnitDesignCharacteristics::ItemCount);
      }
      // Update displayed technologies rows
      UpdateTechnologieTradeValues(l_bSideACountry);
      // update trade tech info for the other country
      // update the technologies values already added on our other side      
      for(GTechnologiesRows::iterator l_ITechInRow = GetTechRows(!l_bSideACountry).begin();
         l_ITechInRow != GetTechRows(!l_bSideACountry).end();
         l_ITechInRow++)
      {
         // retrieve our levels
         UINT8 l_iLevelSideOffer = l_ITechInRow->first.Level();
         UINT8 l_iLevelSideWant  = GetCountryTechMaxLevel(l_bSideACountry, l_ITechInRow->first.Type(), 
                                                         l_ITechInRow->first.Name());
         // retrieve our new value
         REAL64 l_fValue = g_ClientDAL.GetTechValues(l_iLevelSideOffer, l_iLevelSideWant);
         // retrieve region value
         m_tradeData.GetTradeDataFromSide(!l_bSideACountry).UpdateTechnologieValue(l_ITechInRow->first, 
                                                                                 l_fValue);
      }
      // Update what the user sees on screen for the other country
      UpdateTechLevelsDisplay(!l_bSideACountry);
      UpdateTechnologieTradeValues(!l_bSideACountry);        
      UpdateTotalTradeValue(!l_bSideACountry);
   }
}

/*!
 * Try adding a technologie to our trade
 **/
void GTradeDetailedWindow::TryAddTechnologieToTrade(bool in_bProposal)
{
   // retrieve the current technologie settings
   EUnitCategory::Enum              l_eTechType  = (EUnitCategory::Enum)GetSelectedTechTypeFromCombo(in_bProposal);
   EUnitDesignCharacteristics::Enum l_eTechName  = (EUnitDesignCharacteristics::Enum)GetSelectedTechNameFromCombo(in_bProposal);
   INT8                             l_iTechLevel = (INT8)GetSelectedTechLevelFromCombo(in_bProposal);
   // valid selections in combo ?
   if(l_eTechType  != EUnitCategory::Undefined              && 
      l_eTechName  != EUnitDesignCharacteristics::Undefined &&
      l_iTechLevel != TradeValues::c_iNoTechLevel)
   {
      // create our trade technologie
      GTradeTechnologie l_TradeTech(l_eTechType, l_eTechName, l_iTechLevel);
      // add it to our trade
      AddTechnologieToTrade(l_TradeTech, in_bProposal);            
   }
}

/*!
 * Add technologie to our trade
 **/
bool GTradeDetailedWindow::AddTechnologieToTrade(const GTradeTechnologie &in_TradeTech, 
                                                 bool in_bProposal)
{
   // did we add our tech successfully ?
   bool l_bTechAdded = false;
   // trade side used
   GTradeDataForOneSide &l_Trade = m_tradeData.GetTradeDataFromSide(in_bProposal); 
   // retrieve our levels
   UINT8 l_iLevelSideOffer = in_TradeTech.Level();
   UINT8 l_iLevelSideWant  = GetCountryTechMaxLevel(!in_bProposal, in_TradeTech.Type(), in_TradeTech.Name());
   // retrieve region value
   REAL64 l_fTechnologieValue = g_ClientDAL.GetTechValues(l_iLevelSideOffer, l_iLevelSideWant);
   // add into the listbox
   if(AddTechnologieToListBox(in_TradeTech, l_fTechnologieValue, in_bProposal))
   {
      // add it into our trade side
      l_Trade.AddTechnologie(in_TradeTech, l_fTechnologieValue);
      // update the screen
      UpdateTotalTradeValue(in_bProposal);
      // tech was added
      l_bTechAdded = true;
   }    
   // tech added ?
   return l_bTechAdded;
}

/*!
 * Add a TEchnologie into the listbox
 **/
GUI::GListBox::GRow* GTradeDetailedWindow::AddTechnologieToListBox(const GTradeTechnologie &in_TradeTech, 
                                                                   const REAL64 &in_fTechnologieValue, 
                                                                   bool in_bProposal)
{
   // retrieve the listbox
   GUI::GListBox* l_pLstBox    = GetTradeItemsListBox(in_bProposal);
   GUI::GListBox::GRow *l_pRow = NULL;
   // if already in listbox
   if(GetTechRows(in_bProposal).count(in_TradeTech))
   {
      // retrieve our row
      l_pRow = GetTechRows(in_bProposal)[in_TradeTech];
      // remove the row ref
      GetTechRows(in_bProposal).erase(in_TradeTech);
   }
   else
   {
      // add the new row
      l_pRow = l_pLstBox->Insert_New_Row(0);
   }
   // set up the reference
   GetTechRows(in_bProposal)[in_TradeTech] = l_pRow;
   // technologie listitem text
   GString l_sTechText = g_ClientDAL.GetString(in_TradeTech.CurrentTechTypeStrId()); l_sTechText += L" ";   
   l_sTechText        += g_ClientDAL.GetString(in_TradeTech.CurrentTechNameStrId()); l_sTechText += L" ";   
   l_sTechText        += g_ClientDAL.GetString(STRID_LEVEL);                         l_sTechText += L" ";
   l_sTechText        += GString(in_TradeTech.Level());
   // set up our list item info
   SetUpTradeListItemInfo(l_pRow, l_sTechText, in_fTechnologieValue);      
   // refresh
   l_pLstBox->Update();
   return l_pRow;
}

/*!
 * Remove the currently selected item from the trade listbox
 **/
bool GTradeDetailedWindow::RemoveItemFromTrade(bool in_bProposal)
{
   // did we remove a row 
   bool l_bRowRemoved = false;
   // Retrieve the appropriate list box
   GUI::GListBox* l_pListBox = GetTradeItemsListBox(in_bProposal);
   // retrieve the selected items
   vector<INT32> l_vSel = l_pListBox->Get_Selected_Item();
   // any item selected ?
   if(l_vSel.size() > 0 && l_vSel[0] > -1)
   {
      // retrieve our row
      GUI::GListBox::GRow* l_pRow = l_pListBox->Get_Row_At(l_vSel[0]);
      // valid row ?
      if(l_pRow)
      {
         // money type ?
         if(IsMoneyRow(l_pRow))
         {
            RemoveAllMoneyFromTrade(in_bProposal);
         }
         // technologie type ?
         else if(IsTechRow(l_pRow))
         {
            RemoveTechnologieFromTrade(l_pRow, in_bProposal);
         }
         // region type ?
         else if(IsRegionRow(l_pRow))
         {
            RemoveRegionFromTrade(l_pRow, in_bProposal);
         }
         // unset the last selected row
         if(GetLastSelectedRowId(in_bProposal) == l_pRow->m_iId)
         {
            SetLastSelectedRowId(in_bProposal, -1);
         }
         // remove it from the list box
         l_pListBox->RemoveRow(l_pRow);
         // refresh
         l_pListBox->Update();
         // update the screen
         UpdateTotalTradeValue(in_bProposal);
         // row removed successfully
         l_bRowRemoved = true;
      }
   }
   // row removed ?
   return l_bRowRemoved;
}

/*!
 * Remove the money from the trade
 **/
void GTradeDetailedWindow::RemoveAllMoneyFromTrade(bool in_bProposal)
{
   // remove from trade side
   m_tradeData.GetTradeDataFromSide(in_bProposal).Money(0.f);
   // set up reference row to null
   SetMoneyRow(in_bProposal, NULL);
}

/*!
 * Remove a region from the trade
 **/
void GTradeDetailedWindow::RemoveRegionFromTrade(GUI::GListBox::GRow* in_pRow, bool in_bProposal)
{
   INT32 l_iRegionId = in_pRow->m_iAdditionalId;
   // remove from trade side
   m_tradeData.GetTradeDataFromSide(in_bProposal).RemoveRegion(l_iRegionId);
   // remove the reference to row
   GetRegionsRows(in_bProposal).erase(l_iRegionId);
}

/*!
 * Remove a technologie from the trade
 **/
void GTradeDetailedWindow::RemoveTechnologieFromTrade(GUI::GListBox::GRow* in_pRow, 
                                                      bool in_bProposal)
{
   // ptr to our technologie
   GTradeTechnologie *l_pTechToRemove = NULL;
   // loop into the current tech rows to get the one we want
   for(GTechnologiesRows::const_iterator l_It = GetTechRows(in_bProposal).begin(); 
       (l_It != GetTechRows(in_bProposal).end() && !l_pTechToRemove); 
       l_It++)
   {
      // is it our row 
      if(l_It->second == in_pRow)
      {
         // we found it, assign the technologie associated to the row
         l_pTechToRemove = (GTradeTechnologie*)&l_It->first;
      }
   }
   // valid tech found ?
   if(l_pTechToRemove)
   {
      // remove from trade side
      m_tradeData.GetTradeDataFromSide(in_bProposal).RemoveTechnologie(*l_pTechToRemove);
      // remove the reference to row
      GetTechRows(in_bProposal).erase(*l_pTechToRemove);
   }
}

/*!
 * Send a trade query to the server
 **/
void GTradeDetailedWindow::SendCurrentTrade(ETradeStatus::Enum in_eTradeStatus, bool in_bThroughAdvisor)
{
   // create game event
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
   SP2::Event::GEventTradeProposal* l_pUpdate = (SP2::Event::GEventTradeProposal*)l_pEvent.get();
   // set up trade data
   l_pUpdate->m_trdData = m_tradeData;
   // set up new trade status
   l_pUpdate->m_trdData.Status(in_eTradeStatus);
   // set up source and destination
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();
   // raise it
   if(in_bThroughAdvisor)
   {
      // event could be dangerous, evaluate it
      g_ClientDCL.ClientAdvisor().RaiseEvent(l_pEvent);
   }
   else
   {
      // nothing dangerous, raise it
      g_Joshua.RaiseEvent(l_pEvent);
   }
}
 
/*!
 * Returns true if the trade is valid, false if not
 **/
bool GTradeDetailedWindow::IsCurrentTradeValid()
{
   // retrieve trade total value
   REAL64 l_fTotal = m_tradeData.GetTradeDataFromSide(true).GetTotalValue() + 
                     m_tradeData.GetTradeDataFromSide(false).GetTotalValue();
   // if we trade something and not the same country
   return((l_fTotal > 0.f)                                       && 
          (m_tradeData.GetTradeDataFromSide(true).CountryId() != 
           m_tradeData.GetTradeDataFromSide(false).CountryId())  &&
          (m_tradeData.GetTradeDataFromSide(false).CountryId() > -1));
}

/*!
 * Evaluate if we can propose a trade
 **/
void GTradeDetailedWindow::EvaluateCanPropose()
{
   // if we can propose the trade, enable the button
   if((m_pBtnPropose->Visible()) && 
      (IsCurrentTradeValid())    && 
      (m_tradeData.Status() == ETradeStatus::None))
   {
      m_pBtnPropose->Enabled(true);
   }
   else
   {
      m_pBtnPropose->Enabled(false);
   }
}

/*!
 * Propose the current trade
 **/
void GTradeDetailedWindow::ProposeTrade()
{
   // if we have a trade, send it
   if(IsCurrentTradeValid())
   {
      // send the trade to propose
      SendCurrentTrade(ETradeStatus::Pending, true);
   }
}
 
/*!
 * Accept the current trade
 **/
void GTradeDetailedWindow::AcceptTrade()
{
   // send the trade answer
   SendCurrentTrade(ETradeStatus::Accepted, true);
}

/*!
 * Refuse the current trade
 **/
void GTradeDetailedWindow::RefuseTrade()
{
   // send the trade anwer
   SendCurrentTrade(ETradeStatus::Refused, false);
}

/*!
 * Cancel the current trade
 **/
void GTradeDetailedWindow::CancelTrade(bool in_bCloseWindow)
{
   // if we were in accepting/refusing mode, refuse
   if(m_tradeData.Status() == ETradeStatus::Pending)
   {
      RefuseTrade();
      Close();
   }
   // close the window if requested
   else if(in_bCloseWindow)
   {
      Close();
   }
}