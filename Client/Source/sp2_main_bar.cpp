/**************************************************************
*
* sp2_main_bar.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_main_bar.h"

#include "../include/sp2_mail_window.h"
#include "../include/sp2_economic_window.h"
#include "../include/sp2_history_window.h"
#include "../include/sp2_military_window.h"
#include "../include/sp2_politic_window.h"
#include "../include/sp2_advisor_settings_window.h"
#include "../include/sp2_country_information_window.h"
#include "../include/sp2_relations_window.h"
#include "../include/sp2_event_log_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_objectives_window.h"

#include "../include/sp2_sound.h"

const GString GMainBar::TypeName(L"Main_Bar");
UINT32 GMainBar::m_iNewsId = 0;


const REAL64 c_fMailFlashingDelay = 0.75f;


/*!
 * MAIN Bar construction function
 **/
GUI::GBaseObject* GMainBar::New()
{
	return (GUI::GBaseObject*) new GMainBar();
}


GMainBar::GMainBar() : GFrame()
{
   m_pObjTimeSelection  = NULL;
   m_pObjTimeLabel      = NULL;
   m_pObjDateLabel      = NULL;
   m_pObjMoreInfo       = NULL;
   m_pObjRelations      = NULL;
   m_pObjAdvisorBtn     = NULL;
   m_pObjEventLog       = NULL;
   m_pObjTime           = NULL;
   m_pObjFlag           = NULL;
   m_pObjAdvisor        = NULL;
   m_pObjHistory        = NULL;
   m_pObjMail           = NULL;
   m_pObjPolitic        = NULL;
   m_pObjMilitary       = NULL;
   m_pObjEconomic       = NULL;
   m_pObjFlagTrim       = NULL;
   m_pSlidingTrack      = NULL;
   m_pObjNewsFrm        = NULL;
   m_fLastMailFlashClock=    0;
   m_fSpeed             = -1.f;

   m_NewsPosition = GVector2D<REAL32>(-181.f, -181.f);
   
   m_bTimeDisplayed = false;
   m_bMailFlashStateChanged = false;
   m_bMailHighlighted = false;
}

GMainBar::~GMainBar()
{
}

bool GMainBar::OnCreate()
{
	__super::OnCreate();

   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : " + this->Name());

   m_ActualDate = GDateTime(2000, 
                            1,
                            1,
                            0,
                            0,
                            0,
                            0);


   m_pObjTimeSelection  = (GFrame*)Child(L"frmTimeSelection");
   m_pObjTimeLabel      = (GLabel*)Child(L"lblTime");
   m_pObjDateLabel      = (GLabel*)Child(L"lblDate");
   m_pObjMoreInfo       = (GButton*)Child(L"btnMoreInfo");
   m_pObjRelations      = (GButton*)Child(L"btnRelations");
   m_pObjStatus         = (GButton*)Child(L"btnStatus");
   m_pObjEventLog       = (GToggle*)Child(L"togNews");
   m_pObjTime           = (GButton*)Child(L"btnTime");
   m_pObjFlag           = (GButton*)Child(L"btnFlag");
   m_pObjAdvisor        = (GToggle*)Child(L"togAdvisor");
   m_pObjHistory        = (GToggle*)Child(L"togHistory");
   m_pObjMail           = (GToggle*)Child(L"togMail");
   m_pObjPolitic        = (GToggle*)Child(L"btnPolitic");
   m_pObjMilitary       = (GToggle*)Child(L"btnMilitary");
   m_pObjEconomic       = (GToggle*)Child(L"btnEconomic");
   m_pObjFlagTrim       = (GPictureBox*)Child(L"picFlagTrim");   
   m_pObjNewsFrm        = (GFrame*)Child(L"frmNews");
   m_pObjCountryCbo     = (SP2::GComboListBoxLess*)Child(L"cboCountry");
   m_pObjLocateBtn      = (GUI::GButton*)Child(L"btnLocate");

   m_pObjTime1Tog       = (GUI::GToggle*)Child(L"frmTimeSelection")->Child(L"togTime1");
   m_pObjTime2Tog       = (GUI::GToggle*)Child(L"frmTimeSelection")->Child(L"togTime2");
   m_pObjTime3Tog       = (GUI::GToggle*)Child(L"frmTimeSelection")->Child(L"togTime3");
   m_pObjTime4Tog       = (GUI::GToggle*)Child(L"frmTimeSelection")->Child(L"togTime4");
   m_pObjTime5Tog       = (GUI::GToggle*)Child(L"frmTimeSelection")->Child(L"togTime5");


   ((GFX::GWindow*)m_pObjNewsFrm->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjTimeLabel->BringToFront();
   m_pObjDateLabel->BringToFront();
   m_pObjFlagTrim->BringToFront();
   m_pObjFlag->BringToFront();
   m_pObjLocateBtn->BringToFront();
   m_pObjLocateBtn->Enabled(false);

   Update();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   Position(0,(INT16)mode.m_Resolution.y - Height());

   g_ClientDDL.AnimationCreateRectangle();
   m_pSlidingTrack = g_ClientDDL.AnimationCreateSliding(0.5f, 0.26f, Top() + Height(), Top(), Model()->Get_Transform());

   //m_pObjStatus->Enabled(false);
   //m_pObjRelations->Enabled(false);
   //m_pObjMoreInfo->Enabled(false);
   //m_pObjEventLog->Enabled(false);
   //m_pObjTime->Enabled(false);
   //m_pObjAdvisor->Enabled(false);
   //m_pObjActionList->Enabled(false);

   g_ClientDDL.AdvisorSettingsWindowSpawn();
   g_ClientDDL.AdvisorWindowSpawn();
   g_ClientDDL.HistoryWindowSpawn();

   return true;
}

void GMainBar::OnShow()
{
   __super::OnShow();
   SetAdminView();
}

void GMainBar::SetAdminView()
{
   if( g_Joshua.AdminPlayerID() == g_SP2Client->Id() )
   {
      m_pObjTime1Tog->Enabled(true);
      m_pObjTime2Tog->Enabled(true);
      m_pObjTime3Tog->Enabled(true);
      m_pObjTime4Tog->Enabled(true);
      m_pObjTime5Tog->Enabled(true);
   }
   else
   {
      m_pObjTime1Tog->Enabled(false);
      m_pObjTime2Tog->Enabled(false);
      m_pObjTime3Tog->Enabled(false);
      m_pObjTime4Tog->Enabled(false);
      m_pObjTime5Tog->Enabled(false);
   }
}

bool GMainBar::OnDestroy()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : " + this->Name());

   __super::OnDestroy();

   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(m_pSlidingTrack);

   SAFE_RELEASE(m_pSlidingTrack);

   g_ClientDDL.AdvisorWindowKill();
   g_ClientDDL.AdvisorSettingsWindowKill();

   return true;
}

CGFX_Track_Itf* & GMainBar::AnimationSlidingTrack()
{
   return m_pSlidingTrack;
}

GUI::EEventStatus::Enum GMainBar::OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.X() >= 191 && in_EventData.X() <= 689 && in_EventData.Y() >= 0 && in_EventData.Y() <= 16)
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMainBar::OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.X() >= 191 && in_EventData.X() <= 689 && in_EventData.Y() >= 0 && in_EventData.Y() <= 16)
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMainBar::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.X() >= 191 && in_EventData.X() <= 689 && in_EventData.Y() >= 0 && in_EventData.Y() <= 16)
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   //On a left click
   if((in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left) && in_pCaller)
   {
      if(in_pCaller == m_pObjMoreInfo)
      {
         if(g_ClientDDL.CountryInformationWindow() && !g_ClientDDL.CountryInformationWindow()->Visible())
            g_ClientDDL.CountryInformationWindow()->Show();

         return GUI::EEventStatus::Handled;
      }
      else if(in_pCaller == m_pObjRelations)
      {
         if(g_ClientDDL.RelationsWindow() && !g_ClientDDL.RelationsWindow()->Visible())
            g_ClientDDL.RelationsWindow()->Show();

         return GUI::EEventStatus::Handled;
      }
      else if(in_pCaller == m_pObjTime)
      {
         SwapTimeDisplay();
      }
      else if(in_pCaller == m_pObjFlag)
      {
         INT16 l_iSelectedCountryID = (INT16)g_ClientDCL.SelectedCountryID();
         const GCountry& l_Country = g_ClientDAL.Country(l_iSelectedCountryID);
         g_ClientDCL.SelectedCountrySetLookAt(l_Country);
      }
      else if(in_pCaller == m_pObjPolitic)
      {
         TogglePoliticWindow();
      }
      else if(in_pCaller == m_pObjEconomic)
      {
         ToggleEconomicWindow();
      }
      else if(in_pCaller == m_pObjMilitary)
      {
         ToggleMilitaryWindow();
      }
      else if(in_pCaller == m_pObjMail)
      {
         ToggleMailWindow();
      }
      else if(in_pCaller == m_pObjEventLog)
      {
         if(!g_ClientDDL.EventLogWindow())
            g_ClientDDL.EventLogWindowSpawn();

         if(m_pObjEventLog->ToggleState())
            g_ClientDDL.EventLogWindow()->Show();
         else
            g_ClientDDL.EventLogWindow()->Hide();
      }
      else if(in_pCaller == m_pObjHistory)
      {
         if(!g_ClientDDL.HistoryWindow())
            g_ClientDDL.HistoryWindowSpawn();
         
         if(m_pObjHistory->ToggleState())
            g_ClientDDL.HistoryWindow()->Show();
         else
            g_ClientDDL.HistoryWindow()->Hide();
      }
      else if(in_pCaller == m_pObjLocateBtn)
      {
         g_ClientDDL.BackgroundEarth()->CreateCameraAnimation( &m_NewsPosition );
         g_ClientDDL.BackgroundEarth()->PlayCameraAnimation();      
      }
      else if(in_pCaller == m_pObjTime1Tog ||
              in_pCaller == m_pObjTime2Tog ||
              in_pCaller == m_pObjTime3Tog ||
              in_pCaller == m_pObjTime4Tog ||
              in_pCaller == m_pObjTime5Tog)
      {
         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GChangeGameSpeed);
         SP2::Event::GChangeGameSpeed* l_pEvent = (SP2::Event::GChangeGameSpeed*)l_Event.get();

         l_pEvent->m_iSource = g_SP2Client->Id();
         l_pEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

         if(m_pObjTime1Tog->ToggleState())
            l_pEvent->NewSpeed(c_fSP2GamePausedSpeed);
         else if(m_pObjTime2Tog->ToggleState())
            l_pEvent->NewSpeed(c_fSP2SlowestTimeSpeed);
         else if(m_pObjTime3Tog->ToggleState())
            l_pEvent->NewSpeed(c_fSP2NormalTimeSpeed);
         else if(m_pObjTime4Tog->ToggleState())
            l_pEvent->NewSpeed(c_fSP2HighTimeSpeed);
         else if(m_pObjTime5Tog->ToggleState())
            l_pEvent->NewSpeed(c_fSP2HighestTimeSpeed);
         else
            gassert(0, "There is something wrong with time change");

         g_Joshua.RaiseEvent(l_Event);
      }
      else if(in_pCaller == m_pObjAdvisor)
      {
         if(!g_ClientDDL.AdvisorSettingsWindow())
            g_ClientDDL.AdvisorSettingsWindowSpawn();

         if( m_pObjAdvisor->ToggleState() )
            g_ClientDDL.AdvisorSettingsWindow()->Show();
         else 
            CloseAdvisor();
      }
   
      return GUI::EEventStatus::Handled;
   }

   return GUI::EEventStatus::NotHandled;
}

void GMainBar::CloseAdvisor()
{
   if(m_pObjAdvisor->ToggleState())
      m_pObjAdvisor->ToggleState(false);

   if( g_ClientDDL.AdvisorSettingsWindow() && g_ClientDDL.AdvisorSettingsWindow()->Visible() )
      g_ClientDDL.AdvisorSettingsWindow()->Hide();
}

GUI::EEventStatus::Enum GMainBar::OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_pCaller == m_pObjFlag)
      {
         m_pObjFlagTrim->ColorNormal(GColorRGBInt(0,0,0,255));
         m_pObjFlag->BringToFront();
      }
      else if(in_pCaller == m_pObjMail)
      {
         m_bMailHighlighted = true;
         Iterate();
      }
   }

   return __super::OnMouseOver(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GMainBar::OnMouseOut(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_pCaller == m_pObjFlag)
      {
         m_pObjFlagTrim->ColorNormal(GColorRGBInt(0,0,0,0));
         m_pObjFlag->BringToFront();
      }
      else if(in_pCaller == m_pObjMail)
      {
         m_bMailHighlighted = false;
         Iterate();
      }
   }

   return __super::OnMouseOut(in_EventData,in_pCaller);
}

GUI::EEventStatus::Enum GMainBar::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch(in_EventId)
   {
      case ECustomEvents::OnComboBoxSelectionEvent:
         {
            //Get the Id of the selected item
            GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
            GString l_sName = l_pCombo->Selected_Content(); //1 based index

            const GCountry& l_Country = g_ClientDAL.Country(l_sName);
            if(g_ClientDCL.SelectedCountryID()!= l_Country.Id())
            {
               // Play a sound
               g_SP2Client->PlaySound(ESoundEventID::ZoomToCountry);
            }
            if (!g_ClientDCL.SelectedCountrySetLookAt(l_Country))
               return GUI::EEventStatus::Error;

            //Highlight all the regions of the country
            g_ClientDCL.HighlightCountry((void*)l_Country.Id());
            
            return GUI::EEventStatus::Handled; 
         }
         break;
      case ECustomEvents::OnComboBoxBeforeOpen:
         {
            if(in_EventData.Data == m_pObjCountryCbo)
               g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), (GUI::GComboBox*)in_EventData.Data, true);

            return GUI::EEventStatus::Handled;
         }
      case c_iCloseWindowEvent:
         {
            if(in_pCaller == g_ClientDDL.HistoryWindow() )
            {
               m_pObjHistory->ToggleState(false);
            }
         }
         break;
   }

   return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);
}

void GMainBar::Date(const GDateTime& in_Date)
{
   m_ActualDate = in_Date;
   Update();
}

//! Gets the window Date
const GDateTime& GMainBar::Date()
{
   return m_ActualDate;
}

//! Updates the display
/*!
 * Updates the display of the Date according to the 
 * class members
 **/
bool GMainBar::Update()
{
   m_pObjDateLabel->Text(m_ActualDate.ToString(GDateTime::etfYYYYMMDD));
   m_pObjTimeLabel->Text(m_ActualDate.ToString(GDateTime::etfhhmmss).substr(0,5));

   return true;
}



void  GMainBar::HandleContextMenu(const GContextMenuItem* in_pItem)
{
   return;
}

void GMainBar::SwapTimeDisplay()
{
   if(m_bTimeDisplayed)
   {
      m_pObjTimeLabel->BringToFront();
      m_pObjDateLabel->BringToFront();
   }
   else
   {
      m_pObjTimeSelection->BringToFront();
   }

   m_bTimeDisplayed = !m_bTimeDisplayed;
}


void GMainBar::Iterate()
{

   //Iterate the news
   IterateNews();
	
	//Iterate date
   Date(g_ClientDAL.ActualDate());

   //Flash the mailbox if there is mail in it
   if(  g_ClientDDL.MailWindow() && 
       (g_ClientDDL.MailWindow()->NbPendingMessages() > 0)  && 
       ((g_Joshua.Clock() - m_fLastMailFlashClock) >= c_fMailFlashingDelay ) &&
       !g_ClientDDL.MailWindow()->Visible()
     )
   {
      if(m_bMailFlashStateChanged)
      {
         m_pObjMail->PictureNormal(L"000852");
         m_bMailFlashStateChanged = false;
      }
      else
      {
         m_pObjMail->PictureNormal(L"000813");
         m_bMailFlashStateChanged = true;
      }

      m_fLastMailFlashClock = g_Joshua.Clock();
   }
   else if( g_ClientDDL.MailWindow() &&
            !g_ClientDDL.MailWindow()->Visible() &&
            (g_ClientDDL.MailWindow()->NbPendingMessages() == 0))
   {
      m_pObjMail->PictureNormal(L"000813");
      m_bMailFlashStateChanged = true;
   }

   UpdateSpeed( g_Joshua.GameSpeed() );
}

void GMainBar::IterateNews()
{
	News::GInfo* l_NewsToPrintOut = NULL;
	UINT32 l_iActionID = 0;

   if(m_vpNewsAct.size())
		l_iActionID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_vpNewsAct[0]);

	if(!m_vpNewsAct.size() || !m_vpNewsAct[0] || (g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_iActionID) > m_vpNewsAct[0]->Length()))
   {
		l_NewsToPrintOut = (News::GInfo*)g_ClientDAL.GetNextNews();
	}

	if(l_NewsToPrintOut && l_NewsToPrintOut->ToString().size() > 0)
	{
		GColorRGBInt l_Color;
		
		//Create the news animation
		GUI::GFrame* l_pNews  = (GUI::GFrame*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"frmNewsThicker", GString(GMainBar::m_iNewsId), m_pObjNewsFrm, false, false);
		GUI::GFrame* l_pNewsSecondFlag = NULL;
		GMainBar::m_iNewsId++;
		

		m_vpNewsFrm.push_back(l_pNews);

		UINT32 l_iSecondFlagWidth = 0;
		const GCountry* l_pCountryA = &(g_ClientDAL.Country(l_NewsToPrintOut->m_iCountryA) );
		const GCountry* l_pCountryB;

		GString l_sCodeA(l_pCountryA->Flag());
		GString l_sCodeB = L"";

		if(l_NewsToPrintOut->m_iCountryB > 0)
		{
         l_pCountryB = &(g_ClientDAL.Country(l_NewsToPrintOut->m_iCountryB) );
			l_sCodeB = l_pCountryB->Flag();
			l_pNewsSecondFlag = (GUI::GFrame*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"frmCountrySmall", L"frmNewsTickerSecondFlag", l_pNews, false, false);

         l_pNewsSecondFlag->TooltipText( l_pCountryB->Name() );
		}

		((GUI::GLabel*)l_pNews->Child(L"frmCountry")->Child(L"txtCode"))->Text(l_sCodeA.substr(0,3));
		((GUI::GPictureBox*)l_pNews->Child(L"frmCountry")->Child(L"picFlag"))->PictureNormal(g_ClientDAL.CountryFlagSmallPath(l_NewsToPrintOut->m_iCountryA));   
      l_pNews->Child(L"frmCountry")->TooltipText( l_pCountryA->Name() );

		if(l_sCodeB != L"")
		{
			((GUI::GLabel*)l_pNewsSecondFlag->Child(L"txtCode"))->Text(l_sCodeB.substr(0,3));
			((GUI::GPictureBox*)l_pNewsSecondFlag->Child(L"picFlag"))->PictureNormal(g_ClientDAL.CountryFlagSmallPath(l_NewsToPrintOut->m_iCountryB));   
			l_iSecondFlagWidth = (UINT32)(((GUI::GLabel*)l_pNewsSecondFlag->Child(L"txtCode"))->Width() + 
										((GUI::GPictureBox*)l_pNewsSecondFlag->Child(L"picFlag"))->Width());
		}
	   
		switch(l_NewsToPrintOut->m_eCategory)
		{   
		//Politic
      case News::ECategory::Political:
			l_Color.R = 200;
			l_Color.G = 201;
			l_Color.B = 127;
			l_Color.A = 255;
			((GUI::GLabel*)l_pNews->Child(L"txtNews"))->FontColorNormal(l_Color);
			((GUI::GPictureBox*)l_pNews->Child(L"picType"))->PictureNormal(L"000432");
			break;
		//Military
      case News::ECategory::Military:
			l_Color.R = 190;
			l_Color.G = 115;
			l_Color.B = 115;
			l_Color.A = 255;
			((GUI::GLabel*)l_pNews->Child(L"txtNews"))->FontColorNormal(l_Color);
			((GUI::GPictureBox*)l_pNews->Child(L"picType"))->PictureNormal(L"000430");
			break;
		//Economic
      case News::ECategory::Economical:
			l_Color.R = 115;
			l_Color.G = 190;
			l_Color.B = 122;
			l_Color.A = 255;
			((GUI::GLabel*)l_pNews->Child(L"txtNews"))->FontColorNormal(l_Color);
			((GUI::GPictureBox*)l_pNews->Child(L"picType"))->PictureNormal(L"000434");
			break;
		//Demographic
		case 4:
			l_Color.R = 115;
			l_Color.G = 116;
			l_Color.B = 190;
			l_Color.A = 255;
			((GUI::GLabel*)l_pNews->Child(L"txtNews"))->FontColorNormal(l_Color);
			((GUI::GPictureBox*)l_pNews->Child(L"picType"))->PictureNormal(L"000436");
			break;
		//Other
		default:
			l_Color.R = 200;
			l_Color.G = 200;
			l_Color.B = 200;
			l_Color.A = 255;
			((GUI::GLabel*)l_pNews->Child(L"txtNews"))->FontColorNormal(l_Color);
			break;
		}

		((GUI::GLabel*)l_pNews->Child(L"txtNews"))->Text(l_NewsToPrintOut->ToString());
		l_pNews->Left(0);
		l_pNews->Top(m_pObjNewsFrm->Top() + m_pObjNewsFrm->Height());

		//Check if the news need a second label		
		GUI::GLabel* l_pLbl = ((GUI::GLabel*)l_pNews->Child(L"txtNews"));
		GUI::GLabel* l_pSecondLabel = NULL;
		if( (INT32)( l_pLbl->Text2D()->Width() + l_iSecondFlagWidth ) > l_pLbl->Width() )
		{
			l_pSecondLabel = (GUI::GLabel*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"g_def_label", L"frmSecondNewsLabel", l_pNews, true, false);
			//We need another label.
			GString l_sCurrentNews(l_pLbl->Text());
			UINT32 l_iPosition = l_sCurrentNews.size();
			UINT32 i;
			for(i=0; i<l_sCurrentNews.size(); i++)
			{
				if(l_sCurrentNews.at(i) == ' ')
				{
					l_iPosition = i;
					break;
				}
			}
			GString l_sFirstWord = l_sCurrentNews.substr(0,i);
			GString l_sLeft = l_sCurrentNews.substr(i+1,l_sCurrentNews.size());

			l_pLbl->Text(l_sFirstWord);
			while( (INT32)(l_pLbl->Text2D()->Width() + l_iSecondFlagWidth) <  l_pLbl->Width() && (l_pLbl->Text().size() != l_sCurrentNews.size()) )
			{
				GString l_sLine1(l_pLbl->Text());
				l_iPosition = l_sLeft.size();
            UINT32 j;
				for(j=0; j<l_sLeft.size(); j++)
				{
					if(l_sLeft.at(j) == ' ')
					{
						l_iPosition = j;
						break;
					}
				}
				l_sFirstWord = l_sLeft.substr(0,l_iPosition);
				if(j+1 > l_sLeft.size())
					break;
				l_sLeft = l_sLeft.substr(j+1,l_sLeft.size());
				l_pLbl->Text( l_sLine1 + L" " + l_sFirstWord );
			}
			GString l_sLine2(L"");
			//If the line 1 is too large, we need to remove the last word
			if((INT32)(l_pLbl->Text2D()->Width() + l_iSecondFlagWidth) > l_pLbl->Width())
			{
				l_pLbl->Text( l_pLbl->Text().substr(0, l_pLbl->Text().size() - l_sFirstWord.size() ) );
				l_sLine2 = l_sFirstWord;		
			}
			if(l_sLine2 != "")
			{
				l_sLine2 += L" ";
				l_sLine2 += l_sCurrentNews.substr(l_pLbl->Text().size() + l_sLine2.size(),l_sCurrentNews.size());
			}
			else
				l_sLine2 +=  l_sCurrentNews.substr(l_pLbl->Text().size() + l_sLine2.size() +1,l_sCurrentNews.size());
			
			l_pSecondLabel->ColorNormal(GColorRGBInt(0,0,0,0));
			l_pSecondLabel->Text(l_sLine2);
			l_pSecondLabel->Left(l_pLbl->Left());
			l_pSecondLabel->Top(l_pLbl->Height() + 2);			
			l_pSecondLabel->Height(l_pLbl->Height());
			l_pSecondLabel->TextAlignment(L"MiddleLeft");
			l_pSecondLabel->Font(L"Lucida Sans Unicode", 9);
			l_pSecondLabel->FontColorNormal(l_Color);
			l_pSecondLabel->Width(l_pLbl->Width());
			l_pSecondLabel->Show();
			l_pSecondLabel->BringToFront();
			l_pNews->Height(l_pSecondLabel->Top() + l_pSecondLabel->Height());
		}
		
		if(l_pNewsSecondFlag)
		{	
			if(l_pSecondLabel)
			{
				l_pNewsSecondFlag->Left(l_pLbl->Left() + l_pSecondLabel->Text2D()->Width() + 1);
				l_pNewsSecondFlag->Top(l_pLbl->Height() + 2);
			}
			else
			{
				l_pNewsSecondFlag->Left(l_pLbl->Left() + l_pLbl->Text2D()->Width() + 1);
				l_pNewsSecondFlag->Top(0);				
			}
			l_pNewsSecondFlag->Show();
			l_pNewsSecondFlag->BringToFront();
			
		}	
	   
		CGFX_Key_Linear* l_AnimKey = NULL;
      
		CGFX_Action_Itf* l_pAction = g_Joshua.Renderer()->Create_Action();
		CGFX_Animation_Linear_Itf* l_pPositionYAnim = g_Joshua.Renderer()->Create_Animation_Linear();

		REAL32 l_fLength;
		if(!l_pSecondLabel)
		{
			l_AnimKey = new CGFX_Key_Linear[4];

			l_AnimKey[0].m_Time = 0.0f;
			l_AnimKey[1].m_Time = 0.5f;
			l_AnimKey[2].m_Time = 3.5f;
			l_AnimKey[3].m_Time = 4.0f;

			l_AnimKey[0].m_Value =  16.0f;
			l_AnimKey[1].m_Value =   0.0f;
			l_AnimKey[2].m_Value =   0.0f;
			l_AnimKey[3].m_Value = -16.0f;

			l_fLength = 4.0f;
			l_pPositionYAnim->Setup_Keys(l_AnimKey, 4);
		}
		else
		{
			l_AnimKey = new CGFX_Key_Linear[6];

			l_AnimKey[0].m_Time = 0.0f;
			l_AnimKey[1].m_Time = 0.5f;
			l_AnimKey[2].m_Time = 3.5f;
			l_AnimKey[3].m_Time = 4.0f;
			l_AnimKey[4].m_Time = 7.0f;
			l_AnimKey[5].m_Time = 7.5f;

			l_AnimKey[0].m_Value =  16.0f;
			l_AnimKey[1].m_Value =   0.0f;
			l_AnimKey[2].m_Value =   0.0f;
			l_AnimKey[3].m_Value = -16.0f;
			l_AnimKey[4].m_Value = -16.0f;
			l_AnimKey[5].m_Value = -32.0f;

			l_fLength = 7.5f;
			l_pPositionYAnim->Setup_Keys(l_AnimKey, 6);
		}

		l_pPositionYAnim->Connect(l_pNews->Model()->Get_Transform(), GFX::PARAM_POSITION_Y);		

		l_pAction->Length(l_fLength);
		l_pAction->Add_Animation(l_pPositionYAnim);

		g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Action(l_pAction);
		m_vpNewsAct.push_back(l_pAction);
		if(g_ClientDAL.NbCountry())
			m_vNewsPosition.push_back( g_ClientDAL.CapitalPosition(l_NewsToPrintOut->m_iCountryA) );

		SAFE_RELEASE(l_pPositionYAnim);
		SAFE_RELEASE(l_pAction);
		SAFE_DELETE_ARRAY(l_AnimKey);
		
		//Call it to remove the last news just printed out
		g_ClientDAL.IterateNews();
	}		

   if(m_vpNewsAct.size() > 0)
   {
      gassert(m_vpNewsAct.size() == m_vpNewsFrm.size(),"Oh shit !");
      
      UINT32 l_iActionID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_vpNewsAct[0]);

      if(!m_vpNewsAct[0] || (g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_iActionID) > m_vpNewsAct[0]->Length()))
      {
         g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iActionID);
         g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action_At(l_iActionID);
         l_iActionID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_vpNewsAct[0]);
         m_vpNewsAct.erase(m_vpNewsAct.begin());
         m_vpNewsFrm[0]->Hide();
         g_Joshua.GUIManager()->ReleaseObjectAsync(m_vpNewsFrm[0]);
         m_vpNewsFrm.erase(m_vpNewsFrm.begin());
         m_vNewsPosition.erase(m_vNewsPosition.begin());

         m_NewsPosition = GVector2D<REAL32>(-181.f, -181.f);
         m_pObjLocateBtn->Enabled(false);
      }
      
      if(!g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_iActionID))
      {
         m_vpNewsFrm[0]->Hide();
			m_vpNewsFrm[0]->Show();
         m_vpNewsFrm[0]->BringToFront();
         g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_iActionID, 0);
         g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_iActionID);

         m_NewsPosition = m_vNewsPosition[0];
         m_pObjLocateBtn->Enabled(true);
      }

      if(m_vpNewsAct.size() > 1)
      {
         UINT32 l_iActionID2 = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_vpNewsAct[1]);
         REAL32 l_fAnimTime  = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_iActionID);
         if(l_fAnimTime > m_vpNewsAct[0]->Length())
         {
            m_vpNewsFrm[1]->Show();
            m_vpNewsFrm[1]->BringToFront();
            g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_iActionID2, m_vpNewsAct[0]->Length() - l_fAnimTime);
            g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_iActionID2);
         }
      }
   }
   else
   {
      m_pObjLocateBtn->Enabled(false);
   }
}

void GMainBar::UpdateSpeed(REAL64 in_fNewSpeed)
{
   if(m_fSpeed != in_fNewSpeed)
   {
      m_pObjTime1Tog->ToggleState(false);
      m_pObjTime2Tog->ToggleState(false);
      m_pObjTime3Tog->ToggleState(false);
      m_pObjTime4Tog->ToggleState(false);
      m_pObjTime5Tog->ToggleState(false);

      if(in_fNewSpeed == c_fSP2GamePausedSpeed)
         m_pObjTime1Tog->ToggleState(true);
      else if(in_fNewSpeed == c_fSP2SlowestTimeSpeed)
         m_pObjTime2Tog->ToggleState(true);
      else if(in_fNewSpeed == c_fSP2NormalTimeSpeed)
         m_pObjTime3Tog->ToggleState(true);
      else if(in_fNewSpeed == c_fSP2HighTimeSpeed)
         m_pObjTime4Tog->ToggleState(true);
      else if(in_fNewSpeed == c_fSP2HighestTimeSpeed)
         m_pObjTime5Tog->ToggleState(true);

      m_fSpeed = in_fNewSpeed;
   }
}

void GMainBar::Status(EDiplomaticStatus::Enum in_eStatus)
{
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      m_pObjStatus->ColorNormal( GColorRGBInt(76,143,69,255) );      
		m_pObjStatus->Text( g_ClientDAL.PlayerName() );
   }
   else
   {
      switch(in_eStatus)
      {
      case EDiplomaticStatus::Allied:
         m_pObjStatus->ColorNormal( GColorRGBInt(76,143,69,255) );
         m_pObjStatus->Text( g_ClientDAL.GetString(102246) );
         break;
      case EDiplomaticStatus::Neutral:
         m_pObjStatus->ColorNormal( GColorRGBInt(168,164,64,255) );
         m_pObjStatus->Text( g_ClientDAL.GetString(102247) );
         break;
      case EDiplomaticStatus::Hostile:
         m_pObjStatus->ColorNormal( GColorRGBInt(143,69,69,255) );
         m_pObjStatus->Text( g_ClientDAL.GetString(102248) );
         break;
      case EDiplomaticStatus::MilitaryAccess:
         m_pObjStatus->ColorNormal( GColorRGBInt(69,107,143,255) );
         m_pObjStatus->Text( g_ClientDAL.GetString(102249) );
         break;
      }
   }
}

void GMainBar::TogglePoliticWindow()
{
   // spawn and hide the window if not created
   if(!g_ClientDDL.PoliticWindow())
   {
      g_ClientDDL.PoliticWindowSpawn();
      g_ClientDDL.PoliticWindow()->Hide();
   }
   // set up toggle look
   m_pObjPolitic->ToggleState(!g_ClientDDL.PoliticWindow()->Visible());
   // hide or show the window hence visibility
   if(!g_ClientDDL.PoliticWindow()->Visible())
   {
      g_ClientDDL.PoliticWindow()->Show();
   }
   else
   {
      g_ClientDDL.PoliticWindow()->Hide();
      g_ClientDDL.KillWindowsPoliticLv2();
   }
}

void GMainBar::ToggleEconomicWindow()
{
   // spawn and hide the window if not created
   if(!g_ClientDDL.EconomicWindow())
   {
      g_ClientDDL.EconomicWindowSpawn();
      g_ClientDDL.EconomicWindow()->Hide();
   }
   // set up toggle look
   m_pObjEconomic->ToggleState(!g_ClientDDL.EconomicWindow()->Visible());
   // hide or show the window hence visibility
   if(!g_ClientDDL.EconomicWindow()->Visible())
   {
      g_ClientDDL.EconomicWindow()->Show();
   }
   else
   {
      g_ClientDDL.EconomicWindow()->Hide();
      g_ClientDDL.KillWindowsEconomicLv2();
   }
}

void GMainBar::ToggleMilitaryWindow()
{
   // spawn and hide the window if not created
   if(!g_ClientDDL.MilitaryWindow())
   {
      g_ClientDDL.MilitaryWindowSpawn();
      g_ClientDDL.MilitaryWindow()->Hide();
   }
   // set up toggle look
   m_pObjMilitary->ToggleState(!g_ClientDDL.MilitaryWindow()->Visible());
   // hide or show the window hence visibility
   if(!g_ClientDDL.MilitaryWindow()->Visible())
   {
      g_ClientDDL.MilitaryWindow()->Show();
   }
   else
   {
      g_ClientDDL.MilitaryWindow()->Hide();
      g_ClientDDL.KillWindowsMilitaryLv2();
   }
}

void GMainBar::ToggleMailWindow()
{
   // spawn and hide the window if not created
   if(!g_ClientDDL.MailWindow())
      g_ClientDDL.MailWindowSpawn();
   // set up toggle look
   m_pObjMail->ToggleState(!g_ClientDDL.MailWindow()->Visible());
   // hide or show the window hence visibility
   if(g_ClientDDL.MailWindow()->Visible())
      g_ClientDDL.MailWindow()->Hide();
   else
      g_ClientDDL.MailWindow()->Show();
}