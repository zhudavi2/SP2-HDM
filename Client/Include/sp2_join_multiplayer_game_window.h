/**************************************************************
*
* sp2_join_multiplayer_game_window.h
*
* Description
* ===========
*
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_JoinMPGame_H_
#define _CSP2_JoinMPGame_H_

bool operator < (const sockaddr & lsh, const sockaddr & rsh);
_STDEXT_BEGIN
size_t hash_value(const sockaddr & _Keyval);
_STDEXT_END

namespace SP2
{

   namespace EConnectionType
   {
      enum Enum
      {
         Internet = 1,
         Lan      = 2,
         Favorites= 3,
      };
   };


   namespace ESortField { enum Enum
   {
      ServerName,
      PlayersCount,
      PingTime
   };}

   class GJoinMPGameWindow : public SP2::GBaseWindow, DesignPattern::GObserver
   {
   public:
	   GJoinMPGameWindow();
	   ~GJoinMPGameWindow();

	   bool OnCreate();
      bool OnDestroy();
      void Show();

	   //! called when the button is released into our menu
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = NULL);
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      int  m_iReceivingThreadId;
      int  m_iSocketId;
      hash_map<sockaddr, GListedServer> m_InternetServerList;
      hash_map<sockaddr, GListedServer> m_LanServerList;

      vector<sockaddr> m_ShownInternetServerList;
      vector<sockaddr> m_ShownLanServerList;

      GTabbed* m_pObjTabGames;

      static const GString TypeName;

      void Refresh();

      GMutex m_ListsMutex;
      bool m_ReadyToRefresh;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      void SortServersBy(ESortField::Enum, bool Inverse);
      
      static ESortField::Enum s_SortServersBy;
      static bool s_SortInverse;
      static hash_map<sockaddr, GListedServer> * s_ServerList;
      static bool ServerBinaryPredicate(const sockaddr & lsh, const sockaddr & rsh);

      void Update(EConnectionType::Enum in_eType);

      bool JoinSelectedServer(GString in_sPassword = L"", bool in_bConfirmed = false);

   private:
      GMutex m_ListboxMutex;
      GUI::GPictureBox* m_pObjGameSpy;

      //! Remove our last selected row
      void RemoveSelection(EConnectionType::Enum in_eConnectType);
      void UnselectRow(GUI::GListBox::GRow* in_pRow);
      void SelectRow(GUI::GListBox::GRow* in_pRow);
      GListedServer GetSelectedServer();

      INT32 m_iNetLastSelectedId;
      INT32 m_iLanLastSelectedId;

      GUI::GButton* m_pObjUpdateBtn;
      GUI::GButton* m_pObjServerInfoBtn;
      GUI::GButton* m_pObjJoinBtn;
      GUI::GButton* m_pObjCancelBtn;

      GUI::GButton* m_pObjHostBtn;
      GUI::GButton* m_pObjLoadBtn;

      GUI::GLabel* m_pObjLoginAs;

      GUI::GListBox* m_pObjNetLst;
      GUI::GListBox* m_pObjLanLst;
	  
	  //RGAPI
	  void GetRGAPIServerListAsync();
   };

};
#endif //_CSP2_JoinMPGame_H_
