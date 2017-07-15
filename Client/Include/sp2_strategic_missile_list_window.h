/**************************************************************
*
* sp2_strategic_missile_list_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_StrategicMissileListWindow_H_
#define _CSP2_StrategicMissileListWindow_H_

namespace SP2
{
   class GCursorMeter;

   class GStrategicMissileListWindow : public GBaseWindow
   {
   public:
	   GStrategicMissileListWindow();
	   ~GStrategicMissileListWindow();

	   bool OnCreate();
      void OnShow();

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      void Initialize(bool in_bFromTargetSingleCountry);
      GUI::EEventStatus::Enum On_List_Items_Selected(GUI::GListBox* in_pList);
      void UpdateMissileUsed();
      void UpdateMissileCount();

      static const GString TypeName;

      bool m_bFromTargetSingleCountry;

      INT32 m_iInternalID;
   
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjMissileLst;
      GUI::GButton*  m_pObjSelectAllBtn;
      GUI::GButton*  m_pObjSelectNoneBtn;
      GUI::GButton*  m_pObjConfirmBtn;

   private:
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateStrategicMissileListWindow();
};

#endif //_CSP2_StrategicMissileListWindow_H_
