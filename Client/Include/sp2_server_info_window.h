/**************************************************************
*
* sp2_server_info_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_ServerInfoWindow_H_
#define _CSP2_ServerInfoWindow_H_

namespace SP2
{
   const UINT32 c_OnServerInfoWindowJoin = 98421664;
   
   class GServerInfoWindow : public GBaseWindow
   {
   public:
	   GServerInfoWindow();
	   ~GServerInfoWindow();

	   bool OnCreate();
      void OnShow();
      void Show(GUI::GBaseObject* in_pCaller);

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      static const GString TypeName;

      void Update(const GListedServer & in_Server);

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();      

   private:
      GUI::GLabel* m_pObjServerNameTxt;
      GUI::GLabel* m_pObjIpAddressTxt;
      GUI::GLabel* m_pObjPlayersTxt;
      GUI::GLabel* m_pObjModsTxt;
      GUI::GLabel* m_pObjScenarioTxt;

      GUI::GToggle* m_pObjPasswordCbo;

      GUI::GButton* m_pObjJoinGameBtn;

      GUI::GBaseObject* m_pCaller;
   };
};

#endif //_CSP2_ServerInfoWindow_H_
