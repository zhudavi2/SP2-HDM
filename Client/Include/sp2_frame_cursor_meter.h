/**************************************************************
*
* sp2_frame_cursor_meter.h
*
* Description
* ===========
*  Describes the interface of class GCursorMeter
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_CURSOR_METER_H_
#define _SP2_CURSOR_METER_H_

namespace SP2
{
   /*!
   * Defines the progress meter.
   **/
   class GCursorMeter : public GUI::GFrame
   {
   public:
      static const GString TypeName;

      GCursorMeter();
      ~GCursorMeter();

      bool OnCreate();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      INT16 SpacingBetweenValues();
      void  SpacingBetweenValues(INT16 in_iValue);

      INT16 BoundLeft();
      void  BoundLeft(INT16 in_iValue);

      INT16 BoundRight();
      void  BoundRight(INT16 in_iValue);

      void UpdateCursor();
      void UpdateValue();

		UINT32 NumberOfLinks() const;

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      void OnDrag(const GUI::GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative);

      void Initialize();
      void NbCursor(const INT32 & in_iValue);
      void AddLinkedCursorToGroup(GCursorMeter* in_pLinkedCursor);

      vector<GUI::GButton*> & MeterBtn();
      vector<bool>          & CursorDragable();
      void TextValueObj(GUI::GLabel* in_pTextObj);
      GUI::GLabel* TextValueObj();

      vector<REAL64> & Minimum();
      vector<REAL64> & Maximum();
      const REAL64 & Value(const INT32 & in_iBtnId);
      void  Value(const REAL64 & in_fValue, const INT32 & in_iBtnId, bool in_bCallEvent = false, bool in_bUpdateLinked = true);
      vector<bool>   & Locked();
      vector<bool>   & Lockable();
      vector<INT16>  & BtnWidth();

      void StepWidth(INT16 in_iPixelValue);
      void LeftBound(INT16 in_iPixelValue);
      void RightBound(INT16 in_iPixelValue);

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      vector<GUI::GButton*> m_vpMeterBtn;
      vector<bool>          m_vbCursorDragable;
      GUI::GLabel*          m_pObjTextValue;

   private:
      void UpdateCursors();

      vector<REAL64> m_vfMinimum;
      vector<REAL64> m_vfMaximum;
      vector<REAL64> m_vfValue;
      vector<INT16>  m_viBtnWidth;
      vector<bool>   m_vbLocked;
      vector<bool>   m_vbLockable;
      vector<GCursorMeter*> m_vpLinkedCursors;

      //Data holders
      INT32    m_iLeftBound;
      INT32    m_iRightBound;
      INT32    m_iMeterId;
      REAL64   m_fMeterValue;
      INT32    m_iMeterNewPosition;

      //Generic values
      INT32    m_iStepWidth;
      INT32    m_iNbCursor;
      bool     m_bDragging;

      
   };

   class GGradientBar : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GProgressMeterYellowValue : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GProgressMeterYellowValueSmall : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GTechnologyMeterBlue : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      GTechnologyMeterBlue();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GBattleOverviewMeter : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      GBattleOverviewMeter();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GFinancingMeter : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      GFinancingMeter();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

   class GIdeologyMeter : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      GIdeologyMeter();
   public:
      static const GString TypeName;
      bool OnCreate();
   };
   
   class GSplitMeter : public GCursorMeter
   {
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      GSplitMeter();
   public:
      static const GString TypeName;
      bool OnCreate();
   };

};//End namespace SP2
#endif //_SP2_FRAME_CURSOR_METER_H_
