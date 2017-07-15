/**************************************************************
*
* sp2_unit_production_window.h
*
* Description
* ===========
*  Describes the interface of class GUnitProductionWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_UNIT_Production_WINDOW_H_
#define _SP2_UNIT_Production_WINDOW_H_

#include "../include/sp2_frame_cursor_meter.h"


namespace SP2
{

   // Foward Declaration
   class GGradientBar;

   /*!
   * Unit Production window
   * This window is used to ask a production of units
   * The window also display the progression of the production queue
   **/
   class GUnitProductionWindow : public GBaseWindow , Hector::DesignPattern::GObserver
   {
      friend class GTutorials;
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      bool OnCreate();

      //! Constructor and Destructor
      GUnitProductionWindow();
      ~GUnitProductionWindow();

      //! UI Functions
      void OnShow();
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      //! Update the current production queue
      void UpdateProductionStatus(list<GUnitProductionStatus> &in_ProductionStatusList,REAL32 in_fProductionCapacity);

      //! Receive Builder country list
      void UpdateBuildingCountry(set<UINT32>& in_BuildingCountryIdList);

      //! Select a military category to show before showing the window
      void SelectedCategory(SP2::EUnitCategory::Enum in_CategoryToShow);

      //! Get notified if a new design occur
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      //! Request the building country list
      void RequestBuildingCountry();

      void Capacity(REAL32 in_iProductionCapacity);

   protected:
      friend class GDataDisplayLayer;
      friend class GMilitaryWindow;

   private:

      //! Update the unit design tree with a category filter
      void BuildUnitDesignTree(SP2::EUnitCategory::Enum in_CategoryToDisplay);

      //! Build  the dre
      void CreateDesignTreeView(void);

      //! Generate the unit design 
      void GenerateUnitDesignList(void);

      //! Update information about the selected design
      void UpdateSelectedDesign(SP2::GUnitDesign *in_pUnitDesign);

      //! Set the priority of the order coresponding to that row.
      void SetRowPriority(GListBox::GRow* in_pRow,EUnitProductionPriority::Enum in_Priority);

      //! Build a combobox with proper color and priority item (Low, Med, high, etc)
      void BuildPriorityComboBox(GUI::GComboBox* in_pComboBox);

      //! Set a combobox color when you change its selected text
      void SetComboboxColorFromText(GUI::GComboBox* in_pComboBox);

      //! Change the priority of an order (on a specific row)
      void ChangePriorityOfOrder(GListBox::GRow* in_pRow);
      
      //! Return the choosen priority from a combo box.
      EUnitProductionPriority::Enum GetPriorityFromCombobox(GUI::GComboBox* in_pPriorityCbo);

      //! Return Training Level from Selected item.
      UINT8 GetProductionSpeedFromCombobox(GUI::GComboBox* in_pPriorityCbo);

      //! Request Build oder to server of the design
      void RequestBuildOrder(void);

      //! Calculate estimated time before delivery date
      INT32 CalculateETA(void);
      
      //! Send the flag if we accept or not forein orders
      void AcceptForeignOrder(bool in_bAcceptForeignOrder);

      // Currently selected design 
      SP2::GUnitDesign *                  m_pSelectedDesign;

      // A list of order Id associate with the specific row.
      hash_map<GListBox::GRow*,UINT32>    m_Orders;

      set<UINT32> m_BuildingCountryIdList;

      // Filters for displaying proper design, Logo and text.
      GString                             m_UnitDesignIDLogo[SP2::EUnitCategory::ItemCount];
      multimap<UINT32,SP2::GUnitDesign*>  m_UnitDesignList[SP2::EUnitCategory::ItemCount];
      GUI::GToggle*                       m_pObjFilter[SP2::EUnitCategory::ItemCount];

      GString                             m_TextPriority[SP2::EUnitProductionPriority::PriorityCount];
      UINT32                              m_iLastETAByPriority[SP2::EUnitProductionPriority::PriorityCount];
      UINT32                              m_iCurrentInfantryUnitsInProduction;
      UINT16                              m_iSelectedBuildingCountry;
      SP2::EUnitCategory::Enum            m_eSelectedCategory;


      // UI Elements
      GUI::GListBox*    m_pObjProductionList;
      GUI::GLabel*      m_pObjUnitNameLbl;
      GUI::GComboBox*   m_pObjPriorityCbo;
      GUI::GComboBox*   m_pObjProductionSpeedCbo;
      SP2::GComboListBoxLess*   m_pObjBuildingCountryCbo;
      GUI::GLabel*      m_pObjUnitCostLbl;
      GUI::GLabel*      m_pObjUnitCostLabel;
      SP2::GUpdatableValue* m_pObjQuantity;
      GUI::GLabel*      m_pObjTotalCostLbl;
      GUI::GLabel*      m_pObjTotalPossibleUnit;
      GUI::GLabel*      m_pObjEtaLabel;

      GUI::GLabel*      m_pObjCapacityTxt;
      GCursorMeter*     m_pObjCapacityFrm;

      GUI::GButton*     m_pObjBuildBtn;
      GUI::GToggle*     m_pObjAcceptForeignOrderTog;
      GUI::GVScrollBar* m_pObjDesignScr;
      GUI::GFrame*      m_pObjDesignFrm;
      GUI::GFrame*      m_pObjDesignInnerFrm;
      GUI::GFrame*      m_pObjTreeViewContainer;
      GTreeView *       m_pObjDesignTree;
   };

};//end namespace SP2
#endif //_SP2_UNIT_Production_WINDOW_H_
