/**************************************************************
*
* sp2_increase_production_window.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_INCREASE_PRODUCTION_H_
#define _CSP2_INCREASE_PRODUCTION_H_

#include "../include/sp2_frame_updatable_value.h"

const INT32 c_OnResourceIncreaseProduction     = 16070819;

namespace SP2
{ 
   class GIncreaseProductionWindow : public SP2::GBaseWindow
   {
   public:
	   GIncreaseProductionWindow();
	   ~GIncreaseProductionWindow();

	   bool OnCreate();
      void OnShow();

	   //! called when the button is released into our menu
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);

      void Update(REAL64 in_fCurrentProduction);

      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      
      GUI::GFrame*  m_pObjInnerFrm;

      GUI::GToggle* m_pObjIncrease1Btn;
      GUI::GToggle* m_pObjIncrease2Btn;
      GUI::GToggle* m_pObjIncrease5Btn;
      GUI::GToggle* m_pObjIncrease10Btn;
      GUI::GToggle* m_pObjIncrease25Btn;

      GUI::GLabel*  m_pObjIncrease1Txt;
      GUI::GLabel*  m_pObjIncrease2Txt;
      GUI::GLabel*  m_pObjIncrease5Txt;
      GUI::GLabel*  m_pObjIncrease10Txt;
      GUI::GLabel*  m_pObjIncrease25Txt;

   private:
   };
};
#endif //_CSP2_INCREASE_PRODUCTION_H_
