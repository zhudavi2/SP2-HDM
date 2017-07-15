/**************************************************************
*
* sp2_used_unit_window.h
*
* Description
* ===========
*  Describes the interface of class GUsedUnitWindow
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/


// Include Guards
#ifndef _SP2_USED_UNIT_WINDOW_H_
#define _SP2_USED_UNIT_WINDOW_H_

namespace SP2
{


   /*!
   * Used unit window 
   * This window is to help buying units from other country
   **/
   class GUsedUnitWindow : public SP2::GBaseWindow
   {
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GUsedUnitWindow();
      ~GUsedUnitWindow();

      //UI Functions
      void OnShow();
      bool OnCreate();

      //! We receive this update when the list of units on the server has changed
      void UpdateUnitsForSale(list<UINT32> in_ListOfUnitsForSale);
      void UpdateDesignInfo(GUnitDesign*   in_pUnitDesign);

   protected:
      friend class GDataDisplayLayer;

   private:
      
      
      void UpdateSelectedUnit(INT32 in_iUnitId);
      void BuySelectedUnit(void);
      void GenerateUnitDesignList(list<UINT32>& in_ListOfUnitForSale);
	   void BuildUnitDesignCbo(UINT32 in_iLastSelectedUnitType);
	   void BuildUnitDesignLstFromId(INT32 in_iUnitId);
      void EnableMeters(bool in_bEnable);

      // UI Events
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum GUsedUnitWindow::OnMouseWheel(const GEventData& in_Data, GBaseObject*);
      GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller);
      void OnDrag(const GEventData&, const GVector2D<INT32>& in_MouseMove, const GVector2D<INT32>&);

      // UI Elements
      GUI::GButton*           m_pObjBuyBtn;
      GUI::GButton*           m_pObjExitBtn;
      GUI::GLabel*            m_pObjProducerLabel;
      GUI::GLabel*            m_pObjUnitCostLabel;
      GUI::GLabel*            m_pObjTotalUnitCostLabel;
      SP2::GUpdatableValue*   m_pObjQuantity;

	  GUI::GComboBox*         m_pUnitTypeCbo;
	  GUI::GListBox*          m_pUnitListLst;

      GUI::GLabel*           m_pObjDesignNameLabel;
      class GTechnologyMeterBlue* m_pObjGunRangeMeter;
      class GTechnologyMeterBlue* m_pObjGunPrecisionMeter;
      class GTechnologyMeterBlue* m_pObjGunDamageMeter;
      class GTechnologyMeterBlue* m_pObjMissileRangeMeter;
      class GTechnologyMeterBlue* m_pObjMissileDamageMeter;
      class GTechnologyMeterBlue* m_pObjMissilePayloadMeter;
      class GTechnologyMeterBlue* m_pObjMissilePrecisionMeter;
      class GTechnologyMeterBlue* m_pObjSpeedMeter;
      class GTechnologyMeterBlue* m_pObjSensorsMeter;
      class GTechnologyMeterBlue* m_pObjArmorMeter;
      class GTechnologyMeterBlue* m_pObjStealthMeter;
      class GTechnologyMeterBlue* m_pObjCountermeasuresMeter;



      GUI::GVScrollBar* m_pObjDesignScr;
      GUI::GFrame*      m_pObjDesignFrm;
      GUI::GFrame*      m_pObjSpecListFrm;

      multimap<UINT32,UINT32>             m_UnitDesignList;
      INT32                               m_iSelectedUnitID;
      INT32                               m_iLastSelectedRowID;
      class GUnitViewScene*               m_pUnitScene;
      GUIContainer*                       m_pModelFrm;
      bool                                m_bFirstTimeToDisplayUnit;
      bool                                m_bDragView;
      bool                                m_bRequestedUsedUnit;
   
   };

};//end namespace SP2
#endif //_SP2_UNIT_GROUND_DEPLOY_WINDOW_H_
