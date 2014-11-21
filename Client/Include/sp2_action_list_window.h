/**************************************************************
*
* sp2_action_list_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_ActionListWindow_H_
#define _CSP2_ActionListWindow_H_

namespace SP2
{

   class GActionListWindow : public GBaseWindow
   {
   public:
	   GActionListWindow();
	   ~GActionListWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      GFrame* m_pObjInnerFrame;
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GTabbed* m_pObjTabActions;

   private:
   };
};

#endif //_CSP2_ActionListWindow_H_
