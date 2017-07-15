/************************************************
 * File: sp2_strategic_select_target_window.h
 * Owner: Francois Durand
 * Description:
 * Hold the window select target in nuclear warfare
 *
 * History:
 * ========
 *
 *************************************************/

#ifndef _CSP2_StrategicSelectTargetWindow_H_
#define _CSP2_StrategicSelectTargetWindow_H_

namespace SP2
{
   class GStrategicSelectTargetWindow : public GUI::GFrame
   {
   public:
	   GStrategicSelectTargetWindow();
	   ~GStrategicSelectTargetWindow();

	   bool OnCreate();
      bool OnDestroy();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GButton* m_pObjCancelBtn;

   private:
   };
};

#endif //_CSP2_StrategicSelectTargetWindow_H_
