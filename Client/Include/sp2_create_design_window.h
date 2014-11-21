/**************************************************************
*
* sp2_Create_Design_Window.h
*
* Description
* ===========
*  Describes the interface of class GCreateDesignWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_Create_Unit_Design_Window_H_
#define _SP2_Create_Unit_Design_Window_H_

#include "sp2_unit_view_parts.h"

namespace SP2
{
   /*!
   * Unit design window
   **/
   class GCreateDesignWindow : public GFrame, Hector::DesignPattern::GObserver
   {
      friend class GTutorials;
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GCreateDesignWindow();
      ~GCreateDesignWindow();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel(const GEventData&, GBaseObject*);
      void OnDrag(const GEventData&, const GVector2D<INT32>&, const GVector2D<INT32>&);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32, const GUI::GEventData &, GUI::GBaseObject *);
      void OnSubjectChange(DesignPattern::GSubject &in_pSubject, const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );
      bool OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller);

   protected:
      friend class GDataDisplayLayer;
      friend class GMilitaryWindow;

      GUI::GComboBox*   m_pDesignTypeCbo;
      GUI::GFrame*      m_pObjSpecListFrm;
      GUI::GEditBox*    m_pObjNameEdt;
      GUI::GLabel*      m_pObjCostTxt;

      class GTechnologyMeterBlue* m_pTechnologyMeters[EUnitDesignCharacteristics::ItemCount];
		class GLabel*					 m_pTechnologyLabels[EUnitDesignCharacteristics::ItemCount];

      GUI::GButton*     m_pObjLeftBtn;
		GUI::GButton*     m_pObjCancelBtn;
      GUI::GButton*     m_pObjRightBtn;
      GUI::GButton*     m_pObjBtnExit;
      GUI::GButton*     m_pObjConfirmCreationBtn;
      GUI::GToggle*     m_pObjLockTog;
      GUI::GComboBox*   m_pObjPieceTypeCbo;

      vector<GUI::GButton*> m_vpObjColorBtn;
      vector<GUI::GButton*> m_vpObjPartBtn;

      GUI::GUIContainer*    m_pModelFrm;
      GUI::GLabel*          m_pObjDescTxt;
      GUI::GFrame*          m_pObjDescFrm;
      GUI::GVScrollBar*     m_pObjDescScr;
      GUI::GUIContainer*    m_pPartLeftFrm;
      GUI::GUIContainer*    m_pPartMiddleFrm;
      GUI::GUIContainer*    m_pPartRightFrm;

      EUnitType::Enum ActuallySelectedUnitType() const;
      void            FillPieceTypeCombo(EUnitType::Enum in_eType);

   private:
      bool SaveDesign();
      void UpdateLevelFilter();
      void ResetSpecs(GString in_sName = L"");

      void OnLeftButton();
      void OnRightButton();
      void OnColorButton(INT32 in_iColor);
      void OnPartClick(INT32 in_iPart);

      void OnUnitTypeChange(EUnitType::Enum in_eNewType);
      void OnUnitPieceChange(UINT32 in_iNewPieceId);


      void DisplayInitialUnitModel(EUnitType::Enum in_eType);

      SP2::GUnitDesign::GDisplay m_SelectedDisplay;

      INT32 m_iCurrentPiece;
      INT32 m_iCurrentNbPieces;
      INT32 m_iCurrentVariation;

      class GUnitViewScene*      m_pUnitScene;
      vector<GFX::GWindow*>      m_vPartWindows;

      GUnitViewParts*            m_pUnitParts;

      vector<GUnitViewParts::GPicture>  m_UnitPartPictures[c_iMaxDesignPieceCount];
      GVector2D<INT32> m_MouseClick;
      void UpdateUnitParts();
      void ReleaseUnitParts();
   };

};//end namespace SP2
#endif //_SP2_Create_Unit_Design_Window_H_
