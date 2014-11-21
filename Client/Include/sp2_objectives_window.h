/**************************************************************
*
* sp2_objectives_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_ObjectivesWindow_H_
#define _CSP2_ObjectivesWindow_H_

#include "../include/sp2_frame_scenario.h"

namespace SP2
{
   class GObjectivesWindow : public GBaseWindow
   {
   public:
	   GObjectivesWindow();
	   ~GObjectivesWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      void UpdateSelectedScenario();
      void ClearSelectedScenario();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      SP2::GFrameScenario* m_pScenarioFrm;

   private:
   };
};

#endif //_CSP2_GameObjectivesWindow_H_
