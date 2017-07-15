/**************************************************************
*
* sp2_history_window.h
*
* Description
* ===========
*  Definition of class SP2::GHistoryWindow, which displays
*  the evolution of specific gameplay values over time.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_HISTORY_WINDOW_H_
#define _SP2_HISTORY_WINDOW_H_

namespace SP2
{
   // Forward declarations;
   class GGraph;
   class GHistoryMarkerWindow;
   class GHistoryMarkerInfoWindow;

   class GHistoryWindow : public GBaseWindow, public DesignPattern::GObserver
   {
   public:
      // Window creation utilities
      static const GString TypeName;
      static GUI::GBaseObject* New();

      // UI event functions
      bool OnCreate();
      bool OnDestroy();
      void OnResize();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseOver(const GEventData &, GBaseObject* = NULL);
      EEventStatus::Enum OnMouseOut(const GEventData &, GBaseObject* = NULL);

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

      // Observer implementation
      void OnSubjectChange(DesignPattern::GSubject& in_Subject, 
                           const DesignPattern::GSubjectNotification& in_Notification, 
                           void* in_pAdditionalMsg);

   private:
      // Update graph with new selection
      void ChangeSelection(UINT32 in_iNewSelection);

      // Use DAL content to properly display the markers on the graph
      void UpdateMarkers();

      // Make sure axis value is in sync with graph display
      void UpdateAxis();

      // Show the specified marker information
      void ShowMarkerInfo(GBaseObject* in_pObj);

      // Show the specified axis element
      void ShowAxisElement(UINT32 in_iAxisID, INT32 in_NewPosX, const GDateTime& in_Date);

      GGraph*                       m_pGraph;

      vector<GHistoryMarkerWindow*> m_vMarkers;

      GLabel*                       m_pMinimumVerticalValue;
      GLabel*                       m_pMaximumVerticalValue;

      struct GHorizontalValue
      {
         GLabel*        m_pValue;
         GFrame*        m_pTrait;
      };

      vector<GHorizontalValue>      m_vHorizontalValues;

      GBaseObject*                  m_pMarkerParent;
      GComboBox*                    m_pHistoryCombo;
      GHistoryMarkerInfoWindow*     m_pHistoryMarkerInfo;
      // toggle buttons and labels
      GToggle*                      m_pTog1Year;
      GToggle*                      m_pTog1Month;
      GToggle*                      m_pTog2Year;
      GToggle*                      m_pTog6Month;
      GLabel*                       m_pLbl1Year;
      GLabel*                       m_pLbl1Month;
      GLabel*                       m_pLbl2Year;
      GLabel*                       m_pLbl6Month;

      GString                       m_ValueTypeNames[EHistoryValues::TotalCount];

      EHistoryValues::Enum          m_eCurrentSelection;
   };
}

#endif // #ifndef _SP2_HISTORY_WINDOW_H_
