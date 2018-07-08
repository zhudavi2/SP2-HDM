/**************************************************************
*
* sp2_game_lobby_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Include files
#include "golem_pch.hpp"

#include "../include/sp2_fsm_main.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_sound.h"
#include "../include/sp2_tutorials.h"
#include "../include/sp2_country_profile.h"
#include "../include/sp2_party_selection_window.h"

// Constants

const UINT32       c_iMaximumDisplayedMessage = 50;
const UINT8        c_bWindowScrollingSpeed = 4;

const GString GGameLobbyWindow::TypeName(L"Game_Lobby");


const INT32 c_iMiniChatWindowHeight    = 96;
/*!
 * MAIN MENU construction function
 **/
GUI::GBaseObject* GGameLobbyWindow::New()
{
	return (GUI::GBaseObject*) new GGameLobbyWindow();
}


GGameLobbyWindow::GGameLobbyWindow() : GBaseWindow(),
   m_pWaitLabel(NULL)
{
   m_iSelectedPartyID = -1;

   m_bMustShowProfile = false;
   m_bMustShowParties = false;
}

bool GGameLobbyWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm         = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjLstChat          = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmChat")->Child(L"lstChat");
   m_pObjEditMsg          = (GUI::GEditBox*)m_pObjInnerFrm->Child(L"frmChat")->Child(L"edtMsg");

   m_pObjPlayersList      = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmPlayers")->Child(L"lstPlayers");
   m_pObjKickBtn          = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnKickPlayer");
   m_pObjBanBtn           = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnBanPlayer");

   m_pObjAdminFrame       = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmAdministrator");

   m_pCountryByProfileBtn = (GUI::GButton*)m_pObjAdminFrame->Child(L"btnProfile");
   m_pPoliticalPartyBtn   = (GUI::GButton*)m_pObjAdminFrame->Child(L"btnParty");   

   m_pPartyLbl            = (GUI::GLabel*)m_pObjAdminFrame->Child(L"txtParty");

   m_pObjStartGameBtn     = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnStartGame");

   m_pObjSendBtn          = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnSend");
   m_pObjSendPrivateBtn   = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnSendPrivate");


   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   // Admin Frame
   m_pObjPlayerNameEditBox = (GUI::GEditBox*)m_pObjAdminFrame->Child(L"edtPlayerName");   
   m_pObjPlayerToggleReady = (GUI::GToggle*)m_pObjAdminFrame->Child(L"togReady");
   // reduce width, we want to see the toggle ready
   m_pObjPlayerNameEditBox->Width(m_pObjPlayerNameEditBox->Width() - 10); 
   m_pObjPlayerToggleReady->Left(m_pObjPlayerToggleReady->Left() + 5);

   // Create Country ComboBox
   m_pObjCountryCbo = (SP2::GComboListBoxLess*)m_pObjAdminFrame->Child(L"cboCountry");
   ((GUI::GEditBox*)m_pObjCountryCbo->Child(L"Edit"))->FontColorNormal(GColorRGBInt(200,200,200,255));

   // Create mini chat window to received message when the lobby is closed.
   m_pMiniChatWindow       = (GUI::GListBox*)CREATE_GUI_OBJECT(L"lstSmallChat",L"MiniChatFrame",(SDK::GUI::GBaseObject *)g_ClientDDL.BackgroundEarth(),true,false);
   //m_pMiniChatWindow->Size(1024,c_iMiniChatWindowHeight);
   m_pMiniChatWindow->Height(c_iMiniChatWindowHeight);
   m_pMiniChatWindow->Position(0,-c_iMiniChatWindowHeight);
   m_pMiniChatWindow->ColorNormal(GColorRGBInt(0,0,0,64));
   m_pMiniChatWindow->Update();
   m_pMiniChatWindow->Hide();
   

   //Create a label over everything saying to wait for information
   GUI::GBaseObject* l_pParentOfWaitLabel = (GUI::GBaseObject*)g_ClientDDL.BackgroundEarth();
   m_pWaitLabel = CREATE_GUI_OBJECT_NAMED_STATIC(GUI::GLabel, L"WaitLbl",l_pParentOfWaitLabel, true);
   m_pWaitLabel->Text(g_ClientDAL.GetString(EStrId::WaitReceivingInformation));
   m_pWaitLabel->ColorNormal(GColorRGBInt(192,192,192,200));
   m_pWaitLabel->Width((INT16)m_pWaitLabel->Text2D()->Width()+10);
   m_pWaitLabel->Height((INT16)m_pWaitLabel->Text2D()->Height()+2); 
   m_pWaitLabel->Position(l_pParentOfWaitLabel->Width()/2-m_pWaitLabel->Width()/2,l_pParentOfWaitLabel->Height()/2-m_pWaitLabel->Height()/2);
   m_pWaitLabel->FontColorNormal(GColorRGBInt(0,0,0,255));
   m_pWaitLabel->FontColor(GColorRGBInt(0,0,0,255));
   m_pWaitLabel->Hide();

   // Set PlayerName
   m_pObjPlayerNameEditBox->Text(g_Joshua.Client()->PlayerName());

   m_pPoliticalPartyBtn->Enabled(false);

   m_pObjCountryCbo->SetListBox(g_ClientDDL.CountriesListBox());

	return true;
}

bool GGameLobbyWindow::OnDestroy()
{
   // be sure we set our global listbox ok
   if(g_ClientDDL.CountriesListBox())
      g_ClientDDL.CountriesListBoxEnableNotAvailable();

   return __super::OnDestroy();
}

void GGameLobbyWindow::OnShow()
{
   __super::OnShow();

	m_pObjPlayerNameEditBox->Text(g_Joshua.Client()->PlayerName().substr(0, 25) );	

   if(!g_ClientDDL.CountryProfileWindow())
      g_ClientDDL.CountryProfileWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.CountryProfileWindow());

   if(!g_ClientDDL.PartySelectionWindow())
      g_ClientDDL.PartySelectionWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.PartySelectionWindow());

   // set up default text color in the combobox
   if(m_pObjCountryCbo && m_pObjCountryCbo->Child(L"Edit"))
   {
      ((GUI::GEditBox*)m_pObjCountryCbo->Child(L"Edit"))->FontColorNormal(GColorRGBInt(200,200,200,255));
   }
   
   m_pObjEditMsg->Focus();
}

void GGameLobbyWindow::OnHide()
{
   RegisteredModalWindow.clear();

   // be sure we set our global listbox ok
   if(g_ClientDDL.CountriesListBox())
      g_ClientDDL.CountriesListBoxEnableNotAvailable();

   // Something it doesnt disapear
   if(m_pObjCountryCbo->GetListBox())
      m_pObjCountryCbo->GetListBox()->Hide();

   __super::OnHide();
}

GGameLobbyWindow::~GGameLobbyWindow()
{

}

void GGameLobbyWindow::ShowWaitLabel(void)
{
   // Display wait label
   m_pWaitLabel->Show();
}


void GGameLobbyWindow::HideWaitLabel(void)
{
   // Hide wait label
   m_pWaitLabel->Hide();
}

GUI::EEventStatus::Enum GGameLobbyWindow::OnChar(const GEventData & in_EventData, SDK::GUI::GBaseObject * in_pCaller)
{   
   if (in_EventData.Key() == VK_RETURN)
   {
      if (in_pCaller == m_pObjEditMsg)
      {
         list<UINT32> l_DestinationPlayer;

         if(in_EventData.Keyboard.Shift != 0)
         {
            const vector<INT32> & l_viSel = m_pObjPlayersList->Get_Selected_Item();
            
            for(UINT32 i=0;i<l_viSel.size();i++)
            {
               if(l_viSel[i] >=0)
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjPlayersList->Get_Row_At( l_viSel[i] );
                  if( l_pRow && l_pRow->Object() )
                  {
                     l_DestinationPlayer.push_back(l_pRow->m_iAdditionalId);
                  }
               }
            }
         }

         SendToSelected(m_pObjEditMsg->Text(),l_DestinationPlayer);
         m_pObjEditMsg->Text("");
         g_Joshua.GUIManager()->Focus(m_pObjEditMsg);
      }
      else if (in_pCaller == m_pObjPlayerNameEditBox)
      {
         SendPlayerInfo();
      }
   }
   return __super::OnChar(in_EventData, in_pCaller);
}

void GGameLobbyWindow::RequestGameStart(void)
{
   // Create event
   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GStartGameRequest);
   l_Event->m_iSource  = g_Joshua.Client()->Id();
   l_Event->m_iTarget= SDK::Event::ESpecialTargets::Server;
   g_Joshua.RaiseEvent(l_Event);  
}
 
void GGameLobbyWindow::SendPlayerInfo(void)
{
   GDZLOG(EDZLogLevel::Entry, L"");

   // Create event
   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GHdmSetPlayerInfo);
   l_Event->m_iSource  = g_Joshua.Client()->Id();
   l_Event->m_iTarget= SDK::Event::ESpecialTargets::Server;

   // Get Event structure
   SP2::Event::GHdmSetPlayerInfo* l_pPlayerInfo = dynamic_cast<SP2::Event::GHdmSetPlayerInfo*>(l_Event.get());

   // Gather Player Info to be sent
   // Get Client ID
   l_pPlayerInfo->m_PlayerInfo.ClientID = g_Joshua.Client()->Id();
   GDZLOG(EDZLogLevel::Info1, L"Client ID = " + GString(l_pPlayerInfo->m_PlayerInfo.ClientID));

   // Get Selected Country
   GString l_sSelectedCountry = m_pObjCountryCbo->Selected_Content();
   GDZLOG(EDZLogLevel::Info1, L"Selected country = " + l_sSelectedCountry);
   if(l_sSelectedCountry != g_ClientDAL.GetString(EStrId::SelectACountry) && m_pObjCountryCbo->SelectedRow() != nullptr)
   {
      const GCountry& l_country = g_ClientDAL.Country(l_sSelectedCountry);
      l_pPlayerInfo->m_PlayerInfo.CountryID  = l_country.Id();
      l_pPlayerInfo->m_PlayerInfo.PartyID    = m_iSelectedPartyID;
   }
   else
   {
      l_pPlayerInfo->m_PlayerInfo.CountryID  = -1;
      l_pPlayerInfo->m_PlayerInfo.PartyID = -1;
   }

   // Get Name 
   l_pPlayerInfo->m_PlayerInfo.PlayerName = m_pObjPlayerNameEditBox->Text();
   GDZLOG(EDZLogLevel::Info1, L"Player name = " + GString(l_pPlayerInfo->m_PlayerInfo.PlayerName));

   // Get Ready state only if we are not active
   //\TODO (JMERCIER)This should be handled differently... we should received our status once everything is set...
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
   {
      if(m_pObjPlayerToggleReady->ToggleState() == true)
      {
         l_pPlayerInfo->m_PlayerInfo.PlayerStatus = SDK::PLAYER_STATUS_READY;
      }
      else
      {
         l_pPlayerInfo->m_PlayerInfo.PlayerStatus = SDK::PLAYER_STATUS_IDLE;
      }
   }
   else
   {
      l_pPlayerInfo->m_PlayerInfo.PlayerStatus  = SDK::PLAYER_STATUS_ACTIVE;
   }

   l_pPlayerInfo->m_iPassword = g_SP2Client->InternalPasswordToServer();

   // Fire event
   g_Joshua.RaiseEvent(l_Event);      

   GDZLOG(EDZLogLevel::Exit, L"Returning");
}

GUI::EEventStatus::Enum GGameLobbyWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if (in_EventId == ECustomEvents::OnComboBoxSelectionEvent)
   {   
      GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
      gassert(l_pCombo,"The ID says Combo box...but the pointer says null");

      // Note : This assume that there is only one combo box in that window...
      if(l_pCombo)
      {
         // If we can select it, that means we can put it in white ?
         GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");
         l_pEditBox->FontColorNormal(c_colorAvailableCountry);
         // party not selected anymore
         m_iSelectedPartyID = -1;
         // send the player info
         SendPlayerInfo();

         if( m_pObjCountryCbo->Selected_Content() != g_ClientDAL.GetString(EStrId::SelectACountry) )
         {
            SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGetCountryParties);
            SP2::Event::GGetCountryParties* l_Evt = (SP2::Event::GGetCountryParties*)l_Event.get();
            l_Evt->m_iSource = g_Joshua.Client()->Id();
            l_Evt->m_iCountryId = g_ClientDAL.Country( m_pObjCountryCbo->Selected_Content() ).Id();
            l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
            g_Joshua.RaiseEvent(l_Event);   
            
            m_pPoliticalPartyBtn->Enabled(true);
         }
      }
   }
   else if (in_EventId == c_iOnCountryProfileWindowSelection)
   {
      INT32 l_iCountryID = (INT32)in_EventData.Data;
      if( l_iCountryID > 0 )
      {
         m_pObjCountryCbo->Selected_Content( g_ClientDAL.Country(l_iCountryID).Name() );

         GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");
         l_pEditBox->FontColorNormal(c_colorAvailableCountry);
         // party not selected anymore
         m_iSelectedPartyID = -1;
         // send the player info
         SendPlayerInfo();

         if( m_pObjCountryCbo->Selected_Content() != g_ClientDAL.GetString(EStrId::SelectACountry) )
         {
            SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGetCountryParties);
            SP2::Event::GGetCountryParties* l_Evt = (SP2::Event::GGetCountryParties*)l_Event.get();
            l_Evt->m_iSource = g_Joshua.Client()->Id();
            l_Evt->m_iCountryId = g_ClientDAL.Country( m_pObjCountryCbo->Selected_Content() ).Id();
            l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
            g_Joshua.RaiseEvent(l_Event);   
            
            m_pPoliticalPartyBtn->Enabled(true);
         }
      }
   }
   else if (in_EventId == c_iOnPartySelection)
   {
      INT32 l_iPartyID = (INT32)in_EventData.Data;

      for(UINT32 i=0; i<m_vParties.size(); i++)
      {
         if(m_vParties[i].Id() == l_iPartyID)
         {
            m_iSelectedPartyID = m_vParties[i].Id();
            m_pPartyLbl->Text( g_ClientDAL.GetString( m_vParties[i].NameId() ) );
            break;
         }
      }
   }
   else if (in_EventId == ECustomEvents::OnComboBoxBeforeOpen)
      {
         if(in_EventData.Data == m_pObjCountryCbo)
         {
            g_ClientDDL.CountriesListBoxEnableNotAvailable(false);
            g_ClientDDL.CountriesListBox()->Update();
         }

         return GUI::EEventStatus::Handled;
      }

   return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);
}

//! called when the button is released into our menu
GUI::EEventStatus::Enum GGameLobbyWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if (in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if (in_pCaller == m_pObjPlayerToggleReady)
         {

            GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");

            if(m_pObjPlayerToggleReady->ToggleState() == true)
            {
               // Make sure we can be ready...
               if( ( l_pEditBox->FontColorNormal() == c_colorTakenCountry) ||
                  ( m_pObjCountryCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SelectACountry)))
               {
                  // Can't select a non valid country
                  m_pObjPlayerToggleReady->ToggleState(false);
               }
               else
               {
                  l_pEditBox->FontColorNormal(c_colorTakenCountry);
                  m_pObjCountryCbo->Enabled(false);

                  // Something has change, lets tell everyobdy
                  SendPlayerInfo();
               }
            }
            else
            {
               // Assuming what we select belong to us...
               m_pObjCountryCbo->Enabled(true);
               l_pEditBox->FontColorNormal(c_colorAvailableCountry);
               SendPlayerInfo();
            }
         }
         else if (in_pCaller == m_pObjExitBtn)
         {
            if(g_SP2Client->CurrentFSMState() == FSM::EStates::Playing)
            {
               Hide();
            }
            else
            {
               g_ClientDCL.CancelConnect();
            }
         }
         else if(in_pCaller == m_pCountryByProfileBtn)
         {
            if(m_viCountriesID.size())
            {
               g_ClientDDL.CountryProfileWindow()->Show(this, m_viCountriesID, m_viEconomicRanks, m_viPoliticRanks, m_viMilitaryRanks);
            }
            else
            {
               m_bMustShowProfile = true;
               m_pCountryByProfileBtn->Enabled(false);
            }
         }
         else if(in_pCaller == m_pPoliticalPartyBtn)
         {
            if(!g_ClientDDL.PartySelectionWindow())
               g_ClientDDL.PartySelectionWindowSpawn();

            if( m_vParties.size() )
            {
               g_ClientDDL.PartySelectionWindow()->Show( m_vParties, m_iSelectedPartyID, this );
            }
            else
            {
               m_bMustShowParties = true;
               m_pPoliticalPartyBtn->Enabled(false);
            }
         }
         else if(in_pCaller == m_pObjKickBtn)
         {
            const vector<INT32> & l_viSel = m_pObjPlayersList->Get_Selected_Item();
            
            for(UINT32 i=0;i<l_viSel.size();i++)
            {
               if(l_viSel[i] >=0)
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjPlayersList->Get_Row_At( l_viSel[i] );
                  if( l_pRow && l_pRow->Object() )
                  {
                     KickPlayer(l_pRow->m_iAdditionalId);
                  }
               }
            }
         }
         else if(in_pCaller == m_pObjBanBtn)
         {
            const vector<INT32> & l_viSel = m_pObjPlayersList->Get_Selected_Item();
            
            for(UINT32 i=0;i<l_viSel.size();i++)
            {
               if(l_viSel[i] >=0)
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjPlayersList->Get_Row_At( l_viSel[i] );
                  if( l_pRow && l_pRow->Object() )
                  {
                     BanPlayer(l_pRow->m_iAdditionalId);
                  }
               }
            }
         }
         else if(in_pCaller == m_pObjStartGameBtn)
         {
            // Request the game to be started.
            RequestGameStart();
         }
         else if(in_pCaller == m_pObjSendBtn)
         {                     
            SDK::GUI::GEventData l_Data;
            l_Data.Keyboard.Actor = VK_RETURN;

            OnChar(l_Data, m_pObjEditMsg);
         }
         else if(in_pCaller == m_pObjSendPrivateBtn)
         {
            SDK::GUI::GEventData l_Data;
            l_Data.Keyboard.Actor = VK_RETURN;
            l_Data.Keyboard.Shift = true;

            OnChar(l_Data, m_pObjEditMsg);
         }
      }
   }


   return __super::OnMouseClick(in_EventData, in_pCaller);
}

void GGameLobbyWindow::DisplayMessageInChatListBox(GUI::GListBox & in_listBox, const GChatMessage & in_Message,GColorRGBInt &in_colorBackground)
{
   //Ignore chat messages if initial data was not received.
   if(!g_ClientDAL.Countries().size() )
   {
      return;
   }

   // Delete first row if there is too many messages
   while(in_listBox.Get_Nb_Rows()>c_iMaximumDisplayedMessage)
   {
      in_listBox.RemoveRow((UINT32)0);
   }

   // Create a new row in the chat box
   GListBox::GRow* l_pRow = in_listBox.Insert_New_Row();
   gassert(l_pRow,"Invalid pointer for that new row");

   // Set Default background color
   ((GUI::GFrame *)l_pRow->Object())->ColorNormal(in_colorBackground);

   // Get all components for 
   GUI::GLabel* l_pPlayerName       = (GUI::GLabel*)l_pRow->Object()->Child(L"lblPlayerName");
   GUI::GLabel* l_pText             = (GUI::GLabel*)l_pRow->Object()->Child(L"lblText");
   GUI::GPictureBox* l_pCountryPic  = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag");
   
   // Set Sender country flag
   // Get selected country from player name
   if(in_Message.m_countryID > 0)
   {  
	   const GCountry& l_pPlayerCountry = g_ClientDAL.Country(in_Message.m_countryID);
      // Set Country flag
      l_pCountryPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_pPlayerCountry.Id()]);
   }

   // Set Sender Name
   l_pPlayerName->Text(in_Message.m_sender + L" -");
   if((INT32)l_pPlayerName->Text2D()->Width() > l_pPlayerName->Width())
      l_pPlayerName->Width(l_pPlayerName->Text2D()->Width());

   l_pPlayerName->FontColorNormal(GColorRGBInt(200,200,200,255));
   if(in_Message.m_bPrivate)
      l_pPlayerName->FontColor(GColorRGBInt(20,232,20,255));
   
   l_pText->Left(l_pPlayerName->Text2D()->Width() + l_pPlayerName->Left() + 3);
   l_pText->Width(l_pRow->Object()->Width() - l_pText->Left());
   
   // Set Sender Message
   l_pText->Text(in_Message.m_message);
   if(in_Message.m_bPrivate)
      l_pText->FontColor(GColorRGBInt(20,232,20,255));

   UINT32 l_iLabelWidth = l_pText->Width();
   UINT32 l_iTextWidth  = (INT32)l_pText->Text2D()->WidthMax();  
   
   //if the text is too long to fit the label
   if(l_iTextWidth > l_iLabelWidth)
   {
      INT32 l_iIndex = -1;
      INT32 l_iPrevLine = 0;
      GString l_sOldText(L"");

      //for each character
      for(UINT32 i=0; i<in_Message.m_message.length(); i++)
      {
         //add the new char to the string
         l_sOldText += wstring(1,in_Message.m_message.c_str()[i]);

         //check if we find a space
         if(in_Message.m_message.c_str()[i] == L' ')
            l_iIndex = i;

         l_pText->Text(l_sOldText);
         if(in_Message.m_bPrivate)
              l_pText->FontColor(GColorRGBInt(20,232,20,255));

         l_iTextWidth = l_pText->Text2D()->WidthMax();
      
         //if the text is too long to fit the label
         if(l_iTextWidth > l_iLabelWidth)
         {
            //check if we found a space and crop the text to the space
            if(l_iIndex != -1)
            {
               l_pText->Text(l_sOldText.substr(0,l_iIndex - l_iPrevLine));
               if(in_Message.m_bPrivate)
                  l_pText->FontColor(GColorRGBInt(20,232,20,255));

               i = l_iIndex;
               l_iPrevLine = l_iIndex;
               l_iIndex = -1;
            }
            else
            {
               l_pText->Text(l_sOldText.substr(0,i));
               if(in_Message.m_bPrivate)
                  l_pText->FontColor(GColorRGBInt(20,232,20,255));

            }

            //insert a new row and get the label
            l_pRow = in_listBox.Insert_New_Row();
            gassert(l_pRow,"Invalid pointer for that new row");

            ((GUI::GFrame *)l_pRow->Object())->ColorNormal(in_colorBackground);
            // Note : we are cheating a bit by taking the player name space also (and hidding the real text label)
            l_pText = (GUI::GLabel*)l_pRow->Object()->Child(L"lblPlayerName");
            l_pText->Width(l_pText->Width() + l_pRow->Object()->Child(L"lblText")->Width());
            l_pRow->Object()->Child(L"lblText")->Hide();
            l_iLabelWidth = l_pText->Width();
            
            l_sOldText = L"";
         }
      }
   }

   // Update list box
   in_listBox.Update();  
   in_listBox.ScrollTo(in_listBox.Get_Nb_Rows() - 1);
 
}

void GGameLobbyWindow::OnNewMessageReceived(const GChatMessage & in_Message)
{
   GColorRGBInt l_colorBackgroundColor;
      l_colorBackgroundColor = GColorRGBInt(20,20,20,255);

   // Send sound event
   g_SP2Client->PlaySound(ESoundEventID::IncomingChat1);

   // Add text to lobby window
   DisplayMessageInChatListBox(*m_pObjLstChat,in_Message,l_colorBackgroundColor);

   // Add text to mini chat window
   DisplayMessageInChatListBox(*m_pMiniChatWindow,in_Message,l_colorBackgroundColor);
  
   // Show mini chat window if lobby is not visible
   if(!Visible())
   {
      if(m_pMiniChatWindow->Visible() == false)
      {
         m_pMiniChatWindow->Show();
      }

      // Display window for a certain period
      m_miniChatWindowCounterDisplay  = 10;
  }

}

void GGameLobbyWindow::UpdateAvailableCountries(void)
{
   g_ClientDDL.ComboboxSetCountriesAndFlag( g_ClientDAL.Countries(), m_pObjCountryCbo, true);

   if(g_SP2Client->LoadingGame() && g_ClientDCL.m_CurrentGameData.m_iCountryID != 0)
   {
      m_pObjPlayerNameEditBox->Text(g_ClientDCL.m_CurrentGameData.m_sPlayerName);
      m_pObjCountryCbo->Selected_Content(g_ClientDAL.Country(g_ClientDCL.m_CurrentGameData.m_iCountryID).Name() );
   }
}

void GGameLobbyWindow::KickPlayer(UINT32 in_iPlayerID)
{
   GGameEventSPtr ge = CREATE_GAME_EVENT(SDK::Event::GSystemGameEvent);
   SDK::Event::GSystemGameEvent* sysGe = (SDK::Event::GSystemGameEvent*)ge.get();
   sysGe->SystemEventType(SDK::Event::GSystemGameEvent::KICK_PLAYER);
   ge->m_iSource   = g_SP2Client->Id();
   ge->m_iTarget = SDK::Event::ESpecialTargets::Server;
   sysGe->m_iSystemEventData = in_iPlayerID;
   g_Joshua.RaiseEvent(ge);
}

void GGameLobbyWindow::BanPlayer(UINT32 in_iPlayerID)
{
   GGameEventSPtr ge = CREATE_GAME_EVENT(SDK::Event::GSystemGameEvent);
   SDK::Event::GSystemGameEvent* sysGe = (SDK::Event::GSystemGameEvent*)ge.get();
   sysGe->SystemEventType(SDK::Event::GSystemGameEvent::BAN_PLAYER);
   ge->m_iSource   = g_SP2Client->Id();
   ge->m_iTarget = SDK::Event::ESpecialTargets::Server;
   sysGe->m_iSystemEventData = in_iPlayerID;
   g_Joshua.RaiseEvent(ge);
}

void GGameLobbyWindow::KickPlayer(GString in_PlayerName)
{
   for (vector<GChatPeer>::const_iterator itr = PlayersList().begin(); itr != PlayersList().end(); itr++)
   {
	   if(itr->PlayerName == in_PlayerName)
	   {
         KickPlayer(itr->ClientID);
 	   }
   }
}

void GGameLobbyWindow::BanPlayer(GString in_PlayerName)
{
   for (vector<GChatPeer>::const_iterator itr = PlayersList().begin(); itr != PlayersList().end(); itr++)
   {
	   if(itr->PlayerName == in_PlayerName)
	   {
         BanPlayer(itr->ClientID);
 	   }
   }
}


void GGameLobbyWindow::OnPlayerListChanged()
{
   // Remember previous selection
   const vector<INT32> & l_viSel = m_pObjPlayersList->Get_Selected_Item();
   set<INT32> l_vSelectedPlayers;

   for(UINT32 i = 0;i < l_viSel.size();i ++)
   {
      GUI::GListBox::GRow* l_pRow = m_pObjPlayersList->Get_Row_At( l_viSel[i] );
      if( l_pRow && l_pRow->Object() )
      {
         l_vSelectedPlayers.insert(l_pRow->m_iAdditionalId);
      }
   }

   // Clear list box for player list
   m_pObjPlayersList->Clear();

   // rebuild list box
   for (vector<GChatPeer>::const_iterator itr = PlayersList().begin(); itr != PlayersList().end(); itr++)
   {
      // NOTE : If we want to hide ourself, just put the remainin code in the else
      if(itr->ClientID == g_SP2Client->Id())
      {
         // Its us, set our data 
         GString l_playerName = itr->PlayerName;
         g_SP2Client->PlayerName(l_playerName);

         // JMercier Note : This is very important and should not actually get set there.
         // When we will have the GamePreStart ... to received IDs and all cossin.
         g_ClientDAL.ControlledCountryID((INT16)itr->CountryID);
      }

      // New row to add
      GListBox::GRow * l_Row = m_pObjPlayersList->Insert_New_Row(itr->ClientID);

      // Set color to black and transparent
      ((GUI::GFrame*)l_Row->Object())->ColorNormal(GColorRGBInt(0,0,0,0));
      
      // Set player state toggle
      bool l_bPlayerReadyOrPlaying = itr->PlayerStatus == SDK::PLAYER_STATUS_READY || itr->PlayerStatus == SDK::PLAYER_STATUS_ACTIVE;
      ((GUI::GToggle*)l_Row->Object()->Child(L"togReady"))->ToggleState(l_bPlayerReadyOrPlaying);
      l_Row->Object()->Child(L"togReady")->Enabled(false);

      // Set player name
      GUI::GLabel* l_pPlayerName = (GUI::GLabel*)l_Row->Object()->Child(L"lblPlayerName");
      l_pPlayerName->Text(itr->PlayerName);
      if(itr->ClientID == g_Joshua.AdminPlayerID())
      {  
         l_pPlayerName->FontColorNormal(GColorRGBInt(200,200,200,255));
      }
   
      // Set Player Selected country
      GString l_sCountryName;
      if(itr->CountryID >0)
         l_sCountryName = g_ClientDAL.Countries()[itr->CountryID-1].Name();
      else
         l_sCountryName = g_ClientDAL.GetString(EStrId::NotSelected);
      ((GUI::GLabel*)l_Row->Object()->Child(L"lblCountry"))->Text(l_sCountryName);
   }

   if(g_SP2Client->Id() == g_Joshua.AdminPlayerID())
   {
      m_pObjStartGameBtn->Show();
      m_pObjKickBtn->Show();
      m_pObjBanBtn->Show();
   }
   else
   {

      m_pObjStartGameBtn->Hide();
      m_pObjKickBtn->Hide();
      m_pObjBanBtn->Hide();
   }

   m_pObjPlayersList->Update();

   vector<INT32> l_vNewSel;
   l_vNewSel.reserve(l_vSelectedPlayers.size() );
   for(INT32 i = 0;i < m_pObjPlayersList->Get_Nb_Rows();i ++)
   {
      GUI::GListBox::GRow* l_pRow = m_pObjPlayersList->Get_Row_At(i);
      if(l_vSelectedPlayers.count(l_pRow->m_iAdditionalId) > 0)
      {
         l_vNewSel.push_back(i);
      }
   }

   m_pObjPlayersList->Select_Items(l_vNewSel, false);
}

void GGameLobbyWindow::AvailableCountries(vector<UINT16> & l_Coutries)
{
   m_AvailableCountries = l_Coutries;
   // Sort them in order
   sort(m_AvailableCountries.begin(),m_AvailableCountries.end());
   UpdateAvailableCountries();
}



void GGameLobbyWindow::HideMiniChatWindow(void)
{
   m_pMiniChatWindow->Hide();
}

void GGameLobbyWindow::Animate(void)
{
   
   if(m_pMiniChatWindow->Visible() == true)
   {

      // Mode show window
      if(m_miniChatWindowCounterDisplay > 0)
      {
         
         GVector2D<INT32> l_vWindowPosition  = m_pMiniChatWindow->Position();
         if(l_vWindowPosition.y < 0)
         {
            l_vWindowPosition.y +=c_bWindowScrollingSpeed *3;
            if(l_vWindowPosition.y >0)
               l_vWindowPosition.y = 0;
            m_pMiniChatWindow->Position(0,l_vWindowPosition.y);
         }
      }
      // Mode display or hide window
      else
      {
         GVector2D<INT32> l_vWindowPosition  = m_pMiniChatWindow->Position();
         if(l_vWindowPosition.y > -c_iMiniChatWindowHeight)
         {
            l_vWindowPosition.y -=c_bWindowScrollingSpeed;
            m_pMiniChatWindow->Position(0,l_vWindowPosition.y);
         }
         else
         {
            m_pMiniChatWindow->Hide();
         }
      }

     m_miniChatWindowCounterDisplay -= 1/10.0f;
   }
}

void GGameLobbyWindow::OnEndLoadData(void)
{
   HideWaitLabel();

   // Fill it out 
   m_pObjCountryCbo->SetListBox(g_ClientDDL.CountriesListBox());

   if(Visible())
   {
      // Explain to the player that its here he/she can choose a country.
      GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");
      m_pObjCountryCbo->Selected_Content(g_ClientDAL.GetString(EStrId::SelectACountry));
      m_pObjCountryCbo->Enabled(true);
      l_pEditBox->FontColorNormal(GColorRGBInt(200,200,200,255));
      // cant put at ready already
      m_pObjPlayerToggleReady->ToggleState(false);
      // set default player name
      m_pObjPlayerNameEditBox->Text(g_ClientDAL.m_DataOptionsWindow.m_sPlayerName);
      // political party not set
      m_iSelectedPartyID = -1;
      m_pPoliticalPartyBtn->Enabled(false);
      m_pPartyLbl->Text("");
      // send info to server
      SendPlayerInfo();
   }
}

void GGameLobbyWindow::Show(EGameLobbyType::Enum in_eGameLobyType)
{
   GUI::GLabel *l_pTitleLabel = (GUI::GLabel*)m_pObjFrameTitle->Child("lblTitle");
   GUI::GFrame *l_pFrmInner = (GUI::GFrame*)Child(L"frmInner");
   GUI::GFrame *l_pFrmPlayers = (GUI::GFrame*)l_pFrmInner->Child(L"frmPlayers");
   GUI::GFrame *l_pFrmAdmin = (GUI::GFrame*)l_pFrmInner->Child(L"frmAdministrator");
   GUI::GFrame *l_pFrmChat = (GUI::GFrame*)l_pFrmInner->Child(L"frmChat");
   GUI::GLabel *l_pPlayerReadyLabel  = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmPlayers")->Child("lblReady");
   GUI::GLabel *l_pAdminReadyLabel = (GUI::GLabel*)m_pObjAdminFrame->Child("lblReady");
   GUI::GPictureBox *l_pWhiteGradient = (GUI::GPictureBox*)Child("picWhiteGradient");

   switch(in_eGameLobyType)
   {
   case EGameLobbyType::MultiPlayer:
   case EGameLobbyType::SinglePlayer:

      l_pAdminReadyLabel->Show();
      m_pObjPlayerToggleReady->Show();
      m_pObjPlayerToggleReady->ToggleState(false);      
      m_pCountryByProfileBtn->Show();
      m_pPoliticalPartyBtn->Show();


      switch(in_eGameLobyType)
      {
      case EGameLobbyType::MultiPlayer:
         l_pPlayerReadyLabel->Text(g_ClientDAL.GetString(EStrId::Ready));                  
         l_pTitleLabel->Text(g_ClientDAL.GetString(EStrId::GameLobby));
         l_pFrmPlayers->Show();
         l_pFrmChat->Show();
         m_pObjKickBtn->Hide();
         m_pObjBanBtn->Hide();
         m_pObjStartGameBtn->Hide();
         l_pFrmAdmin->Left(l_pFrmPlayers->Left());         
         Height( 419 );
         break;
      case EGameLobbyType::SinglePlayer:
         l_pTitleLabel->Text(g_ClientDAL.GetString(EStrId::StartGame));
         l_pFrmPlayers->Hide();
         l_pFrmChat->Hide();
         m_pObjKickBtn->Hide();
         m_pObjBanBtn->Hide();
         l_pFrmAdmin->Left(l_pFrmChat->Left());
         Height(/*l_pFrmAdmin->Top() + l_pFrmAdmin->Height() + 38*/ 419 );
         break;
      }
      Width(l_pFrmAdmin->Left() + l_pFrmAdmin->Width() + 33);
      l_pWhiteGradient->Height(Height() - 6);
      l_pFrmInner->Width(Width() - 21);
      l_pFrmInner->Height(Height() - 32);
      OnUpdateLayout();

      {
         CGFX_Renderer_Init_Mode l_Mode;
         g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
         Left((l_Mode.m_Resolution.x - Width()) / 2);
         Top((INT32)((l_Mode.m_Resolution.y - Height()) * 0.4));
      }
      break;
   case EGameLobbyType::ChatRoom:
      // Hide toggle ready button and toggle label
      l_pAdminReadyLabel->Hide();
      m_pObjPlayerToggleReady->Hide();
      m_pCountryByProfileBtn->Hide();
      m_pPoliticalPartyBtn->Hide();

      if(g_SP2Client->Id() == g_Joshua.AdminPlayerID())
      {
         m_pObjKickBtn->Show();
         m_pObjBanBtn->Show();
      }
      else
      {
         m_pObjKickBtn->Hide();
         m_pObjBanBtn->Hide();
      }

      m_pObjStartGameBtn->Hide();


      // Change the ready label to a playin label in the participants list         
      l_pPlayerReadyLabel->Text(g_ClientDAL.GetString(EStrId::Playing));

      // Set Window Title
      l_pTitleLabel->Text(g_ClientDAL.GetString(EStrId::ChatRoom));
      break;
   }

   // show of the frame
   GBaseWindow::Show();
}



bool  GGameLobbyWindow::OnLoseFocus(const SDK::GUI::GBaseObject *in_pFocusTo, SDK::GUI::GBaseObject *in_pCaller)
{
   // If the Player name edit box lost focus, may be we need to update the player name
   if(in_pCaller == m_pObjPlayerNameEditBox)
   {
      // If the player name has change , update the server
      if(m_pObjPlayerNameEditBox->Text() != g_SP2Client->PlayerName())
      {
         SendPlayerInfo();
      }
      return true;
   }
   // if the window loses the focus
   else if (in_pCaller == this)
   {
      g_ClientDDL.CountriesListBoxEnableNotAvailable();
   }
   return __super::OnLoseFocus(in_pFocusTo,in_pCaller);
}

bool GGameLobbyWindow::OnGotFocus(const GBaseObject* in_pFocusFrom, GBaseObject* Caller)
{
   // if the window gets the focus
//   if (Caller == this)
//      g_ClientDDL.CountriesListBoxEnableNotAvailable(false);
   return __super::OnGotFocus(in_pFocusFrom, Caller);
}

void GGameLobbyWindow::SetCountryRanks(const vector<INT16> & in_viMilitaryRanks, const vector<INT16> & in_viPoliticRanks, const vector<INT16> & in_viEconomicRanks, const vector<INT16> & in_viCountriesID)
{
   m_viEconomicRanks = in_viEconomicRanks;
   m_viMilitaryRanks = in_viMilitaryRanks;
   m_viPoliticRanks  = in_viPoliticRanks;
   m_viCountriesID   = in_viCountriesID;

   if(m_bMustShowProfile)
   {
      g_ClientDDL.CountryProfileWindow()->Show(this, m_viCountriesID, m_viEconomicRanks, m_viPoliticRanks, m_viMilitaryRanks);
      m_pCountryByProfileBtn->Enabled(true);
      m_bMustShowProfile = false;
   }
}

void GGameLobbyWindow::SetPoliticalParties(const vector<GPoliticalParty> & in_vParties)
{
   m_vParties = in_vParties;

   for(UINT32 i=0; i<m_vParties.size(); i++)
   {
      if( m_vParties[i].InPower() )
      {
         m_iSelectedPartyID = m_vParties[i].Id();
         m_pPartyLbl->Text( g_ClientDAL.GetString( m_vParties[i].NameId() ) );
         m_pPartyLbl->Update();
         break;
      }
   }

   if(m_bMustShowParties)
      g_ClientDDL.PartySelectionWindow()->Show( m_vParties, m_iSelectedPartyID, this );
}

void GGameLobbyWindow::CleanMessageQueue(void)
{
	// Clear list box for inner message queue (the one inside the lobby)
	m_pObjLstChat->Clear();
	m_pObjLstChat->Update();
	

   // Clear message list for mini chat window
	m_pMiniChatWindow->Clear();
	m_pMiniChatWindow->Update();

   // Clear Player list too
   m_pObjPlayersList->Clear();
}

void GGameLobbyWindow::OnCountryNameChanged()
{
    GDZLOG(EDZLogLevel::Entry, L"");

    m_pObjCountryCbo->SetListBox(g_ClientDDL.CountriesListBox());

    if(Visible())
    {
        // Currently, this resets all active players' selected country and country name in players list
        if(m_pObjPlayerToggleReady->ToggleState())
        {
            const GString& l_sCountryName = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID()).Name();
            m_pObjCountryCbo->Selected_Content(l_sCountryName);
        }

        OnPlayerListChanged();
    }

    GDZLOG(EDZLogLevel::Exit, L"");
}
