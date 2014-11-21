/**************************************************************
*
* sp2_history_marker_info_window.h
*
* Description
* ===========
*  Description of class SP2::GHistoryMarkerInfoWindow.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_HISTORY_MARKER_INFO_WINDOW_H_
#define _SP2_HISTORY_MARKER_INFO_WINDOW_H_

namespace SP2
{
   class GHistoryMarkerInfoWindow : public GFrame
   {
   public:
      // Window creation utilities
      static const GString TypeName;
      static GUI::GBaseObject* New();

      // UI event functions
      bool OnCreate();

      void OnResize();

      //! Fetch the text label of the window
      GLabel* Label();

   private:
      GHistoryMarkerInfoWindow();
      GLabel*     m_pLabel;
   };
}

#endif // #ifndef _SP2_HISTORY_MARKER_INFO_WINDOW_H_