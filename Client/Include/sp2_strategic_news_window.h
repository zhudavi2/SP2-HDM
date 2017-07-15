/**************************************************************
*
* sp2_strategic_news_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
/*
#ifndef _CSP2_StrategicNewsWindow_H_
#define _CSP2_StrategicNewsWindow_H_

namespace SP2
{
   class GStrategicNewsWindow : public GUI::GFrame
   {
   public:
	   GStrategicNewsWindow();
	   ~GStrategicNewsWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      static const GString TypeName;

      CGFX_Track_Itf* & AnimationSlidingTrack();
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();
   private:
      CGFX_Track_Itf* m_pSlidingTrack;
   };

   /*!
   * MAIN Bar construction function
   **//*
   GUI::GBaseObject* CreateStrategicNewsWindow();
};

#endif //_CSP2_StrategicNewsWindow_H_
*/