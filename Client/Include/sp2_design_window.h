/**************************************************************
*
* golem_designwindow2.h
*
* Description
* ===========
*  Describes the interface of class GDesignWindow2
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_DESIGNWINDOW2_H_
#define _GOLEM_DESIGNWINDOW2_H_

#include "sp2_unit_view_scene.h"
namespace SP2
{
   class GTechnologyMeterBlue;
  /*!
   * [Class description]
   **/
   class GDesignWindow : public SP2::GBaseWindow , Hector::DesignPattern::GObserver
   {
      friend class GTutorials;
      friend class GDataDisplayLayer;
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
   public:
      GDesignWindow();
      ~GDesignWindow();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel(const GEventData& in_Data, GBaseObject*);
      void OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                           const Hector::DesignPattern::GSubjectNotification& in_Notification, 
                           void* in_pAdditionalMsg);


      void                    SelectedDesign(const SP2::GUnitDesign* in_pDesign);
      const SP2::GUnitDesign* SelectedDesign() const;

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject* in_pCaller);
      

	  void OnDrag(const GEventData&, const GVector2D<INT32>& in_MouseMove, const GVector2D<INT32>&);
	  GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      void UpdateAvailableSpecs(EUnitType::Enum in_eActuallySelectedUnitType);

   protected:

      //! Fills the combo box that gives the name of the units
      void FillNamesComboBox(EUnitType::Enum in_eType);

      //! Init the display of the window
      void InitDisplayedData();

      const SP2::GUnitDesign*    m_pSelectedUnitDesign;
      GUI::GComboBox*            m_pObjUnitTypeCbo;
      GUI::GComboBox*            m_pObjUnitNameCbo;
      GUI::GLabel*               m_pObjDescTxtLbl;
      GUI::GTabbed*              m_pObjTabbed;
      GUI::GUIContainer*         m_pObjTab1Container;
      GUI::GLabel*               m_pObjUnitCostLbl;

      GUI::GButton*              m_pObjCreateDesignBtn;


      SP2::GUnitViewScene        m_UnitViewScene;
      CGFX_Viewport_Itf*         m_pUnitSceneViewPort;

      GVScrollBar*               m_pObjSpecsScr;
      GVScrollBar*               m_pObjDescScr;
      

      SP2::GTechnologyMeterBlue* m_pObjTechnologyMeters[EUnitDesignCharacteristics::ItemCount];
      GUI::GLabel*               m_pObjTechnologyLbl[EUnitDesignCharacteristics::ItemCount];
      bool                       m_bDragView;


   private:
   };
};
#endif //_GOLEM_DESIGNWINDOW2_H_
