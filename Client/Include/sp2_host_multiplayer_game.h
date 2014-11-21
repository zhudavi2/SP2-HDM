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
#ifndef _CSP2_HostMPGame_H_
#define _CSP2_HostMPGame_H_

namespace SP2
{
   class GHostMPGameWindow : public GBaseWindow
   {
   public:
	   GHostMPGameWindow();
	   ~GHostMPGameWindow();

	   bool OnCreate();
	   bool OnDestroy();	   

	   //! called when the button is released into our menu
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
	   GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject * in_pCaller);

      GString m_sServerName;
      GString m_sPassword;
      INT32   m_iPort;
      INT32   m_iMaxPlayers;
      INT32   m_iGameTimeLimit;
      bool    m_bPrivate;
      bool    m_bDedicated;
      bool    m_bNuclearWarfareEnabled;
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
	  GUI::GPictureBox* m_pObjGameSpy;
      GUI::GButton*  m_pObjStartGame;
      GUI::GButton*  m_pObjCancelBtn;
      
      GUI::GEditBox* m_edtServerName;
      GUI::GEditBox* m_edtPassword;
      GUI::GEditBox* m_edtPort;
      GUI::GEditBox* m_edtMaxPlayers;
      GUI::GEditBox* m_edtTimeLimit;

      GUI::GToggle*   m_togPrivate;
      GUI::GToggle*   m_togDedicated;

      GUI::GComboBox* m_cboGameSpeed;
      GUI::GComboBox* m_cboNuclearWarfare;
      
      GUI::GToggle*   m_pObjWorldPeaceTog;
      GUI::GToggle*   m_pObjBalanceResTog;
      GUI::GToggle*   m_pObjConquerWorldTog;
      GUI::GToggle*   m_pObjDevelopNationTog;
      GUI::GToggle*   m_pObjRaiseGDPTog;

      GUI::GComboBox* m_pObjWorldPeaceCbo;
      GUI::GComboBox* m_pObjBalanceResCbo;
      GUI::GComboBox* m_pObjConquerWorldCbo;
      GUI::GComboBox* m_pObjDevelopNationCbo;
      GUI::GComboBox* m_pObjRaiseGDPCbo;

      void AddGameObjective(EObjectiveType::Enum in_eType,
                            UINT32 in_fTimeLimitYears = 0, 
                            REAL32 in_fValueToReach = 1.f);
      void RemoveGameObjective(EObjectiveType::Enum in_eType);
      void UpdateObjective(EObjectiveType::Enum in_eType);

      //! Feed the comboboxes with the goals we can set
      void FeedComboGoals(GUI::GComboBox* in_pCbo);
   };
};
#endif //_CSP2_HostMPGame_H_
