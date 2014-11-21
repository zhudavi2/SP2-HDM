/**************************************************************
*
* sp2_war_list_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_WarListWindow_H_
#define _CSP2_WarListWindow_H_

namespace SP2
{

   class GWarListWindow : public SP2::GBaseWindow, DesignPattern::GObserver
   {
   public:
	   GWarListWindow();
	   ~GWarListWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
		EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller = 0);

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      void PlayerState(const INT32 in_iWarID, bool & io_bPlayerIsIn, bool & io_bPlayerIsMaster, bool & io_bPlayerAttacker);
      void UpdateWarList();
      void UpdateSides(const INT32 in_iWarID);
      void AddWar(const SP2::GWar & in_War);
      bool ActualWarStatus(bool in_bAttacker, bool & out_bWantsPeace);

      void AddDirtyWar(UINT32 in_iWarID, bool in_bAtWar);
      void RemoveDirtyWar(UINT32 in_iWarID);
      bool IsWarDirty(UINT32 in_iWarID, bool out_bAtWar);

		//        (ROWID, WARID)
      hash_map< UINT32, UINT32 > m_ShownWars;
      
      //        (WARID, INWAR)
      hash_map< UINT32, bool > m_ChangedWars;

      GUI::GListBox* m_pObjWarLst;
      GUI::GListBox* m_pObjSideALst;
      GUI::GListBox* m_pObjSideBLst;

      GUI::GButton*  m_pObjJoinABtn;
      GUI::GButton*  m_pObjJoinBBtn;

      GUI::GToggle*  m_pObjPeaceATog;
      GUI::GToggle*  m_pObjPeaceBTog;
      GUI::GToggle*  m_pObjWarATog;
      GUI::GToggle*  m_pObjWarBTog;

      GUI::GLabel*   m_pObjMasterALbl;
      GUI::GLabel*   m_pObjMasterBLbl;

      GUI::GPictureBox*   m_pObjMasterAPic;
      GUI::GPictureBox*   m_pObjMasterBPic;

      GUI::GPictureBox*   m_pObjStatusAPic;
      GUI::GPictureBox*   m_pObjStatusBPic;

		INT32 m_iCurrentWarID;
      INT32 m_iSelectedRowID;
   };
};

#endif //_CSP2_WarListWindow_H_
