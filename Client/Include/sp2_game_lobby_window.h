/**************************************************************
*
* sp2_game_lobby_window.h
*
* Description
* ===========
*
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_GameLobby_H_
#define _CSP2_GameLobby_H_

#include "sp2_chat.h"

const GColorRGBInt c_colorAvailableCountry(255,255,255,255);
const GColorRGBInt c_colorTakenCountry(0,0,0,255);

namespace SP2
{
   // This window is used to all spaghetti sauce..
   namespace EGameLobbyType { enum Enum
   {  
      MultiPlayer,
      SinglePlayer,
      ChatRoom,
   };}

   class GGameLobbyWindow : public GBaseWindow, public GChat
   {
      friend GDataDisplayLayer;
   public:

      static const GString TypeName;
	   GGameLobbyWindow();
	   ~GGameLobbyWindow();

	  bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      void Show(EGameLobbyType::Enum in_eGameLobyType);


      // Chat method
      void OnNewMessageReceived(const GChatMessage &);
      void DisplayMessageInChatListBox(GUI::GListBox & in_pListBox, const GChatMessage & in_Message,GColorRGBInt &in_colorBackground);

      void SetChatMode(void);
      void SetLobbyMode(void);
      void HideMiniChatWindow(void);

      // Status 
      void ShowWaitLabel(void);
      void HideWaitLabel(void);

      void OnEndLoadData(void);
	  void CleanMessageQueue(void);
      

      void Animate();
      void OnPlayerListChanged();
      void SendPlayerInfo(void);
      void RequestGameStart(void);
      void UpdateAvailableCountries();

      void AvailableCountries(vector<UINT16> &);

      //! \todo Better handling of country name change. #104
      void OnCountryNameChanged();

	   //! called when the button is released into our menu
      GUI::EEventStatus::Enum OnChar         (const GEventData &, GBaseObject* Caller = NULL);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      bool        OnLoseFocus(const SDK::GUI::GBaseObject *in_pFocusTo, SDK::GUI::GBaseObject *in_pCaller);
      bool        OnGotFocus(const SDK::GUI::GBaseObject* in_pFocusFrom, SDK::GUI::GBaseObject* Caller = NULL);

      void SetCountryRanks(const vector<INT16> & in_viMilitaryRanks, const vector<INT16> & in_viPoliticRanks, const vector<INT16> & in_viEconomicRanks, const vector<INT16> & in_viCountriesID);
      void SetPoliticalParties(const vector<GPoliticalParty> & in_vParties);

      //! Kick all player with this playername
      void KickPlayer(GString in_PlayerName);

      //! Ban a player with the specific name
      void BanPlayer(GString in_PlayerName);

      //! Kick a player with this player id
      void BanPlayer(UINT32 in_iPlayerID);

      //! Kick one player, with that player ID
      void KickPlayer(UINT32 in_PlayerID);

   protected:
      vector<INT16> m_viEconomicRanks;
      vector<INT16> m_viMilitaryRanks;
      vector<INT16> m_viPoliticRanks;
      vector<INT16> m_viCountriesID;
      vector<GPoliticalParty> m_vParties;

   private:

      static GUI::GBaseObject* New();
      GUI::GListBox*     m_pObjLstChat;
      GUI::GListBox*     m_pObjPlayersList;
      
      GUI::GEditBox*     m_pObjEditMsg;

      GUI::GFrame*       m_pObjAdminFrame;      
      GUI::GEditBox*     m_pObjPlayerNameEditBox;
      GUI::GToggle*      m_pObjPlayerToggleReady;

      GUI::GButton*      m_pObjKickBtn;
      GUI::GButton*      m_pObjBanBtn;

      GUI::GLabel*       m_pWaitLabel;
      GUI::GListBox*     m_pMiniChatWindow;

      GUI::GButton*      m_pCountryByProfileBtn;
      GUI::GButton*      m_pPoliticalPartyBtn;
      GUI::GButton*      m_pObjStartGameBtn;

      GUI::GButton*      m_pObjSendBtn;
      GUI::GButton*      m_pObjSendPrivateBtn;

      GUI::GLabel*       m_pPartyLbl;

      SP2::GComboListBoxLess* m_pObjCountryCbo;

      REAL32             m_miniChatWindowCounterDisplay;
      
      vector<UINT16> m_AvailableCountries;      

      INT32 m_iSelectedPartyID;
      bool m_bMustShowProfile;
      bool m_bMustShowParties;

   };
   

};
#endif //_CSP2_InternalLaws_H_
