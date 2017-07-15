/**************************************************************
*
* sp2_history_marker_window.h
*
* Description
* ===========
*  Definition of class SP2::GHistoryMarkerWindow, which 
*  describes a marker inside the history window
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_HISTORY_MARKER_WINDOW_H_
#define _SP2_HISTORY_MARKER_WINDOW_H_

namespace SP2
{
   const UINT32 c_iMaxNbMarkerInfo = 6;

   class GHistoryMarkerWindow : public GFrame
   {
   public:
      // Window creation utilities
      static const GString TypeName;
      static GUI::GBaseObject* New();

      // UI event functions
      bool OnCreate();

      EEventStatus::Enum OnMouseOver(const GEventData &, GBaseObject* = NULL);
      EEventStatus::Enum OnMouseOut(const GEventData &, GBaseObject* = NULL);

      //! Retrieve how many history markers are described by this window
      UINT32 CountMarkers() const;

      //! Retrieve the history marker type at the location
      UINT32 MarkerType(UINT32 in_iIndex) const;

      //! Retrieve the history marker value at the location
      REAL32 MarkerValue(UINT32 in_iIndex) const;

      //! Add a new marker reference, sorting it with the already inserted markers by decreasing importance
      //! and merging if needed
      void InsertMarker(UINT32 in_iType, REAL32 in_fValue);

      //! Remove all marker references
      void ClearMarkers();

   private:
      struct GMarkerRef
      {
         UINT32      m_iType;
         REAL32      m_fValue;
      };

      GMarkerRef     m_pMarkers[c_iMaxNbMarkerInfo];
      UINT32         m_iCurrentCount;
   };
}

#endif // #ifndef _SP2_HISTORY_MARKER_WINDOW_H_