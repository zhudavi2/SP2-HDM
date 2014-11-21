/**************************************************************
*
* sp2_trade_detailed_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_TradeDetailedWindow_H_
#define _CSP2_TradeDetailedWindow_H_

namespace SP2
{
   /*!
   * typedefs used in this header
   **/
   class GBattleOverviewMeter;
   typedef map<UINT32,            GUI::GListBox::GRow*> GRegionsRows;
   typedef map<GTradeTechnologie, GUI::GListBox::GRow*> GTechnologiesRows;   

   /*!
   * Trade system Window
   **/
   class GTradeDetailedWindow : public GBaseWindow, public Hector::DesignPattern::GObserver
   {
   public:
      //! Window type name
      static const GString TypeName;

      //! constructors and destructors
	   GTradeDetailedWindow();
	   ~GTradeDetailedWindow();

      //! Set the region values from the given map
      void SetRegionsValues(const GRegionsValues &in_RegionValues);
      //! Set the technologies values from the given map
		void SetTechnologiesLevels(INT32 in_iCountryId, 
			UINT8 in_TechnologiesLevels[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount]);
      //! Show a trade proposal 
      void ShowTradeProposal(const GTradeData &in_tradeData);
      //! Upate the technologies levels
      void UpdateTechnologiesLevels() {UpdateTechLevelsDisplay(true); UpdateTechLevelsDisplay(false);};

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      //! Events Handlers
	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      //! Close our window
      void Close();

      //! Init our tab controls
      void InitTradeTabControl(GUI::GTabbed* in_pTab);
      //! Init our tab displays
      void InitTabDisplays(GUI::GTabbed* in_pTab);
      //! Init our money editboxed control
      void InitTradeMoneyEdit(SP2::GUpdatableValue* in_pUpd);

      //! Init our proposal Country side
      void InitProposalCountry(INT16 in_iCountryId);
      //! Set the country we want to trade with
      void InitRequestCountry(INT16 in_iCountryId);
      //! Init our region comboboxes control
      void FeedTradeRegionsCombo(bool in_bProposal);
      //! Init our technologies comboboxes control
      void FeedTradeTechnologiesCombo(bool in_bProposal);
      //! Reset our form data
      void ResetTrade(bool in_bProposal);

      //! Retrieve the additionnal id of the current row of our combo
      INT32   GetCurrentAdditonnalIdFromCombo(GUI::GComboBox* in_pCbo) const;
      //! Retrieve the currently selected country in our country combo
      INT16   GetTradeCountryId(bool in_bProposal);
      //! Retrieve the currently selected region
      INT32   GetSelectedRegionIdFromCombo(bool in_bProposal);
      //! Retrieve the currently selected technologies ids from their comboes
      INT32   GetSelectedTechTypeFromCombo(bool in_bProposal);
      INT32   GetSelectedTechNameFromCombo(bool in_bProposal);
      INT32   GetSelectedTechLevelFromCombo(bool in_bProposal);
      //! Retrieve the value in $ of a region
      REAL64  GetCurrentRegionValue(INT32 in_iRegionId);
      //! Retrieve the currently selected tech value
      REAL64  GetCurrentTechValue(bool in_bProposal);
      //! retrieve the techs values
      UINT8   GetCountryTechMaxLevel(bool in_bProposal, EUnitCategory::Enum in_eType, 
                                     EUnitDesignCharacteristics::Enum in_eName);
      //! reset a coutries tech values
      void    ResetCountryTechLevels(bool in_bProposal);        
      //! Retrieve the regions values from the server for a given country
      void    RetrieveRegionsValues(INT16 in_iCountryId);
      //! Retrieve the technolgies values from the server for a given country
      void    RetrieveTechnologiesLevels(INT16 in_iCountryId);

      //! Try adding a region to our trade
      void TryAddRegionToTrade(bool in_bProposal);
      //! Add Region to our trade
      bool AddRegionToTrade(INT32 in_iRegionId, bool in_bProposal);    
      //! Try adding money to our trade
      void TryAddMoneyToTrade(bool in_bProposal);
      //! Add Money to our trade
      bool AddMoneyToTrade(const REAL64 &in_fMoney, bool in_bProposal);   
      //! Try adding a tech to our trade
      void TryAddTechnologieToTrade(bool in_bProposal);
      //! add a tech to our trade
      bool AddTechnologieToTrade(const GTradeTechnologie &in_TradeTech, bool in_bProposal);

      //! Update window
      bool Update();
      //! refresh screen values with what we have in memory
      void SyncWithDataFromMemory(bool in_bProposal);
      //! Update our display of the total trade value
      void UpdateTotalTradeValue(bool in_bProposal);
      //! Update the overview meter
      void UpdateTradeOverviewMeter();
      //! Update the current value label
      void UpdateMoneyLabelValue(GUI::GLabel* in_pLabel, const REAL64 &in_fValue);
      //! Update the money labels value
      void UpdateMoneyTradeValues(bool in_bProposal);
      //! Update the region labels value
      void UpdateRegionTradeValues(bool in_bProposal);
      //! Update the technologies labels value
      void UpdateTechnologieTradeValues(bool in_bProposal);
      //! updsate our rech levels usable
      void UpdateTechLevelsDisplay(bool in_bProposal);

      //! Remove the money from the trade
      void RemoveAllMoneyFromTrade(bool in_bProposal);
      //! Remove a region from the trade
      void RemoveRegionFromTrade(GUI::GListBox::GRow* in_pRow, bool in_bProposal);
      //! Remove a technologie from the trade
      void RemoveTechnologieFromTrade(GUI::GListBox::GRow* in_pRow, bool in_bProposal);

      //! Add money row in list box
      GUI::GListBox::GRow* AddMoneyToListBox(const REAL64 &in_fMoney, bool in_bProposal);
      //! Add region row in list box
      GUI::GListBox::GRow* AddRegionToListBox(INT32 in_iRegionId, const REAL64 &in_fRegionVal, bool in_bProposal);
      //! Add tech row in list box
      GUI::GListBox::GRow* AddTechnologieToListBox(const GTradeTechnologie &in_TradeTech, 
                                                   const REAL64 &in_fTechnologieValue, 
                                                   bool in_bProposal);
      //! Set up a trade list item row info
      void SetUpTradeListItemInfo(GUI::GListBox::GRow* in_pRow, GString in_strName, const REAL64 &in_fValue);
      //! Set up a row money value
      void UpdateRowMoneyVal(GUI::GListBox::GRow* in_pRow, const REAL64 &in_fValue);
      //! Is this row the money row ?
      bool IsMoneyRow(GUI::GListBox::GRow* in_pRow) const;
      //! Is it a region row ?
      bool IsRegionRow(GUI::GListBox::GRow* in_pRow) const;
      //! Is it a tech row ?
      bool IsTechRow(GUI::GListBox::GRow* in_pRow) const;
      //! Init our trade listboxes
      void EmptyTradeItemsListBox(bool in_bProposal);
      //! Remove the currently selected item from the trade listbox
      bool RemoveItemFromTrade(bool in_bProposal);

      //! Retrieve the listbox hence the side
      GUI::GListBox *       GetTradeItemsListBox(bool in_bProposal) const;
      //! Retrieve the total trade value label
      GUI::GLabel *         GetTotalTradeValueLabel(bool in_bProposal) const;
      //! Retrieve the regions combo box
      GUI::GComboBox*       GetRegionsComboBox(bool in_bProposal) const;
      //! Retrieve the technologies types combo box
      GUI::GComboBox*       GetTechTypesComboBox(bool in_bProposal) const;
      //! Retrieve the technologies names combo box
      GUI::GComboBox*       GetTechNamesComboBox(bool in_bProposal) const;
      //! Retrieve the technologies levels combo box
      GUI::GComboBox*       GetTechLevelsComboBox(bool in_bProposal) const;
      //! Retrieve the money edit box
      SP2::GEditBoxNumeric* GetMoneyEditBox(bool in_bProposal) const;
      //! Retrieve the money label
      GUI::GLabel*          GetMoneyLabel(bool in_bProposal) const;
      //! Retrieve the region label
      GUI::GLabel*          GetRegionLabel(bool in_bProposal) const;
      //! Retrieve the technologie labe;
      GUI::GLabel*          GetTechnologieLabel(bool in_bProposal) const;
      //! retrieve the money row
      GUI::GListBox::GRow*  GetMoneyRow(bool in_bProposal);
      //! Set the money row
      void                  SetMoneyRow(bool in_bProposal, GUI::GListBox::GRow* l_pRow);
      //! retrieve the regions rows
      GRegionsRows &        GetRegionsRows(bool in_bProposal);
      //! retrieve the techs rows
      GTechnologiesRows&    GetTechRows(bool in_bProposal);
      //! Set what can be accomplished in the window
      void EnableWhatCanBeDone();

      //! Returns true if the trade is valid, false if not
      bool IsCurrentTradeValid();
      //! Propose the current trade
      void ProposeTrade();
      //! Accept the current trade
      void AcceptTrade();
      //! Refuse the current trade 
      void RefuseTrade();
      //! cancel the current trade
      void CancelTrade(bool in_bCloseWindow);
      //! Send a trade query to the server
      void SendCurrentTrade(ETradeStatus::Enum in_eTradeStatus, bool in_bThroughAdvisor);

      //! Evaluate if we can propose a trade
      void EvaluateCanPropose();

      //! Mouse events handlers
      EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseClick(const GEventData &in_EventData, GBaseObject* in_pCaller);

      //! Custom events
      EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData &in_EventData, GBaseObject* in_pCaller);

      //! Observer on subject change
      void OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                           const Hector::DesignPattern::GSubjectNotification& in_Notification,
                           void* in_pAdditionalMsg);

      //! Objects
      //! Frames
      GUI::GFrame*  m_pObjInnerFrm;
      GUI::GFrame*  m_pFrmProposal;
      GUI::GFrame*  m_pFrmRequest;
      GUI::GFrame*  m_pFrmProposalItems;
      GUI::GFrame*  m_pFrmRequestItems;
      GUI::GFrame*  m_pFrmBalance;

      //! Tab controls
      GUI::GTabbed* m_pTabRequest;
      GUI::GTabbed* m_pTabProposal;

      //! ComboBoxes
      SP2::GComboListBoxLess* m_pCboCountryRequest;
      GUI::GComboBox*         m_pCboRegionsRequest;
      GUI::GComboBox*         m_pCboRegionsProposal;
      GUI::GComboBox*         m_pCboTechTypeProposal;
      GUI::GComboBox*         m_pCboTechNameProposal;
      GUI::GComboBox*         m_pCboTechLevelProposal;
      GUI::GComboBox*         m_pCboTechTypeRequest;
      GUI::GComboBox*         m_pCboTechNameRequest;
      GUI::GComboBox*         m_pCboTechLevelRequest;

      //! Editboxes
      SP2::GEditBoxNumeric* m_pEdtMoneyProposal;
      SP2::GUpdatableValue* m_pEdtMoneyProposalVal;
      SP2::GEditBoxNumeric* m_pEdtMoneyRequest;
      SP2::GUpdatableValue* m_pEdtMoneyRequestVal;

      //! ListBoxes
      GUI::GListBox* m_pLstBoxProposal;
      GUI::GListBox* m_pLstBoxRequest;
      //! retrieve the last selected row id
      INT32 GetLastSelectedRowId(bool in_bProposal); 
      //! Set ou last selected row id
      void  SetLastSelectedRowId(bool in_bProposal, INT32 in_iRowId);
      //! Last selected row ids
      INT32 m_iLastSelectedSideARowId;
      INT32 m_iLastSelectedSideBRowId;

      //! Buttons
      GUI::GButton* m_pBtnAddProposalRegion;
      GUI::GButton* m_pBtnAddProposalMoney;
      GUI::GButton* m_pBtnAddProposalTech;
      GUI::GButton* m_pBtnAddRequestedRegion;
      GUI::GButton* m_pBtnAddRequestedMoney;
      GUI::GButton* m_pBtnAddRequestedTech;
      GUI::GButton* m_pBtnRemoveSelectedProposal;
      GUI::GButton* m_pBtnRemoveSelectedRequest;
      GUI::GButton* m_pBtnRemoveAllProposal;
      GUI::GButton* m_pBtnRemoveAllRequest;
      GUI::GButton* m_pBtnPropose;
      GUI::GButton* m_pBtnCancel;
      GUI::GButton* m_pBtnAccept;
      GUI::GButton* m_pBtnRefuse;

      //! Labels
      GUI::GLabel* m_pLblCountryProposal;
      GUI::GLabel* m_pLblTotalProposal;
      GUI::GLabel* m_pLblTotalRequest;
      GUI::GLabel* m_pLblProposalRegionVal;
      GUI::GLabel* m_pLblRequestRegionVal;
      GUI::GLabel* m_pLblProposalMoneyVal;
      GUI::GLabel* m_pLblRequestMoneyVal;
      GUI::GLabel* m_pLblProposalTechVal;
      GUI::GLabel* m_pLblRequestTechVal;

      //! Picture Boxes
      GUI::GPictureBox *m_pPicCountryProposal;     
      GUI::GPictureBox *m_pPicCountryRequest;

      //! Overview meter
      GBattleOverviewMeter* m_pObjTradeOverviewMeter;

      //! our trade data
      GTradeData m_tradeData;

      //! Regions and techs values
      GRegionsValues m_RegionsValues;
      UINT8          m_TechLevelsSideA[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
      UINT8          m_TechLevelsSideB[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];

      //! Our regions rows      
      GRegionsRows m_mSideARegionsRows;
      GRegionsRows m_mSideBRegionsRows;
      //! Our techs rows
      GTechnologiesRows m_mSideATechRows;
      GTechnologiesRows m_mSideBTechRows;
      //! Our money rows
      GUI::GListBox::GRow* m_pSideARowMoney;
      GUI::GListBox::GRow* m_pSideBRowMoney;

   private:
   };
};

#endif //_CSP2_TradeDetailedWindow_H_
