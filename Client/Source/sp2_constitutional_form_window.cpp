/**************************************************************
*
* sp2_constitutional_form_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_constitutional_form_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_politic_window.h"

const GString GConstitutionalFormWindow::TypeName(L"Constitutional_Form");

GUI::GBaseObject* GConstitutionalFormWindow::New()
{
	return (GUI::GBaseObject*) new GConstitutionalFormWindow();
}


GConstitutionalFormWindow::GConstitutionalFormWindow() : GBaseWindow()
{
   m_pData       = &(g_ClientDAL.DataConstitutionalForm);
   m_pWindowData = &(g_ClientDAL.DataConstitutionalForm);

	m_pSubjectThatWillAnswerLiberateCountryQuestion = NULL;
   m_pObjInnerFrame         = NULL;
   m_pObjConstitutionalForm = NULL;
   m_pObjCapital            = NULL;
   m_pObjNextElection       = NULL;
   m_pObjMartialLaw         = NULL;
   m_pObjOverride           = NULL;
   m_bWindowCanDisable = true;

   m_iCapitalId = -1;
   m_iGvtId     = -1;
	m_iCountryToLiberateRegions = 0;

	NeedCountryCheckToSave(false);
}

GConstitutionalFormWindow::~GConstitutionalFormWindow()
{
}

bool GConstitutionalFormWindow::Enabled()
{
   return m_bEnabled;
}

void GConstitutionalFormWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmNextElection")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmCapital")->Child(L"lblTitle"))->Enabled(in_bEnabled);

   m_pObjNextElection->Enabled(in_bEnabled);
   m_pObjMartialLaw  ->Enabled(in_bEnabled);
   m_pObjOverride    ->Enabled(in_bEnabled);
}

bool GConstitutionalFormWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrame = (GUI::GFrame*)Child(L"frmInner");

   m_pObjConstitutionalForm = (GUI::GComboBox*)m_pObjInnerFrame->Child(L"frmConstitutionalForm")->Child(L"cboConstitutionalForm");
   m_pObjCapital            = (GUI::GComboBox*)m_pObjInnerFrame->Child(L"frmCapital")->Child(L"cboCapital");

   m_pObjNextElection       = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmNextElection")->Child(L"lblElectionDate");
   m_pObjMartialLaw         = (GUI::GButton*)m_pObjInnerFrame->Child(L"btnDeclareMartialLaw");
   m_pObjOverride           = (GUI::GButton*)m_pObjInnerFrame->Child(L"frmNextElection")->Child(L"btnOverride");

   if(Visible())
      Update();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GConstitutionalFormWindow::OnShow()
{
   __super::OnShow();

	m_iCountryToLiberateRegions = 0;

   MartialLaw(m_pData->MartialLaw);
   
   if(m_pObjInnerFrame)
      Update();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   g_ClientDAL.Attach(this, (DesignPattern::GSubjectNotification)ENotifyWindowMsg::ConstitutionalForm );
}

void GConstitutionalFormWindow::OnHide()
{
   g_ClientDAL.Detach(this, (DesignPattern::GSubjectNotification)ENotifyWindowMsg::ConstitutionalForm );
   // remove the request associated with our internal laws window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);
   __super::OnHide();
}

void GConstitutionalFormWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{

	if(&in_Subject == m_pSubjectThatWillAnswerLiberateCountryQuestion)
   {
      if(in_Notification == c_OnGenericQuestionWindowAnswerOk)
      {         
         Save();
      }
      else if(in_Notification == c_OnGenericQuestionWindowAnswerCancel)
      {
			m_iCountryToLiberateRegions = 0;
			return;
      }
   }

   // is there any request pending for our internal laws window, if yes do not notify
   if(!RequestPending())
   {
      if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::ConstitutionalForm)
      {
         if(in_pAdditionalMsg)
            Update(true);
         else
            Update();
      }
   }
}

GUI::EEventStatus::Enum GConstitutionalFormWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   //Be sure that if the player is viewing another country than his own that he cannot change anything
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(g_ClientDAL.ControlledCountryID() != g_ClientDCL.SelectedCountryID() &&
			g_ClientDAL.ProportionGainedByCountryFromCountry(g_ClientDAL.ControlledCountryID(), g_ClientDCL.SelectedCountryID()) < SP2::c_fPercentageNecessaryToChangeGovernmentType  &&
			g_ClientDAL.DiplomaticStatus(g_ClientDAL.ControlledCountryID(),g_ClientDCL.SelectedCountryID()) != EDiplomaticStatus::Hostile)
      {
         if(in_pCaller && in_pCaller == m_pObjExitBtn)
		   {
			   gassert(Visible(),"GConstitutionalFormWindow::OnMouseClick Window should be visible");
            Hide();
		   }

         return EEventStatus::Handled;
      }
      else if(in_pCaller)
      {
         //on martial law btn
         if(in_pCaller == m_pObjMartialLaw)
         {
            MartialLaw(!MartialLaw());
            m_pData->MartialLaw = MartialLaw();
            Dirty(true);
            return GUI::EEventStatus::Handled;
         }
         // on override elections button
         if(in_pCaller == m_pObjOverride)
         {
            // set governement to military dictatorship
            GString l_strMilDict = g_ClientDAL.GetString(g_ClientDAL.StringIdGvtType(5));
            m_pObjConstitutionalForm->Selected_Content(l_strMilDict, true);
         }
      }
   }
   
   return __super::OnMouseClick(in_EventData, in_pCaller);
}

void GConstitutionalFormWindow::Save()
{
	//Check if it's for another country
	if(g_ClientDCL.SelectedCountryID() != g_ClientDAL.ControlledCountryID() &&
		m_iCountryToLiberateRegions == 0)
	{
		GGenericQuestion l_Question;
		l_Question.m_sMessage         = g_ClientDAL.GetString(102359);
		l_Question.m_pOberserToNotify = this;
		l_Question.m_pParentWindow		= this;

		//Pop the question
		m_pSubjectThatWillAnswerLiberateCountryQuestion = g_ClientDDL.GenericQuestion(l_Question);
		m_iCountryToLiberateRegions = g_ClientDCL.SelectedCountryID();
		return;
	}

   //Get the new window values
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GConstitutionalFormUpdate);
   SP2::Event::GConstitutionalFormUpdate* l_pUpdateEvent = (SP2::Event::GConstitutionalFormUpdate*)l_pEvent.get();

   m_fWhenSaved = g_Joshua.GameTime();

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
	l_pUpdateEvent->CountryId = g_ClientDCL.SelectedCountryID();

   l_pUpdateEvent->GvtType = m_mGvt.find(m_pObjConstitutionalForm->Selected_Content())->second;
   gassert(l_pUpdateEvent->GvtType > 0 && l_pUpdateEvent->GvtType < 10,"Gvt type is not between 1 and 9");
   
   if(m_iCapitalId != -1 && g_ClientDAL.DataConstitutionalForm.CapitalId != m_iCapitalId)
      l_pUpdateEvent->CapitalId = (UINT32)m_iCapitalId;
   else
      l_pUpdateEvent->CapitalId = 0;

   if(m_bMartialLaw)
      l_pUpdateEvent->MartialLaw = true;
   else
      l_pUpdateEvent->MartialLaw = false;

   l_pUpdateEvent->GetData = false;

	m_iCountryToLiberateRegions = 0;

   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdateEvent);   
   //raise the event that will update the server
   g_Joshua.RaiseEvent(l_pEvent);

   Dirty(false);

   if(g_ClientDAL.ControlledCountryID() != g_ClientDCL.SelectedCountryID())
   {
      g_ClientDAL.FetchPoliticWindowInfo(g_ClientDAL.ControlledCountryID());
      g_ClientDAL.FetchConstitutionalFormInfo(g_ClientDAL.ControlledCountryID());
      g_ClientDAL.FetchInternalLawsInfo(g_ClientDAL.ControlledCountryID());
   }
}

GUI::EEventStatus::Enum GConstitutionalFormWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if (in_EventId != ECustomEvents::OnComboBoxSelectionEvent)
      return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);

   GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
   if(l_pCombo)
   {
      //on constitutional form change, if politic window is shown update it
      if(l_pCombo == m_pObjConstitutionalForm)
      {
         if(m_iGvtId == m_mGvt.find(m_pObjConstitutionalForm->Selected_Content())->second)
            return GUI::EEventStatus::Handled;

         m_iGvtId = m_mGvt.find(m_pObjConstitutionalForm->Selected_Content())->second;

         Dirty(true);

         if(MartialLaw() && m_iGvtId != 5)
         {
            MartialLaw(false);
         }

         if(m_iGvtId == 8 || m_iGvtId == 9)
         {
            if(m_pData->GvtType == m_iGvtId)
               m_pObjOverride->Enabled(true);
            else
               m_pObjOverride->Enabled(false);

            m_pObjNextElection->Text(m_NextElection.ToString(GDateTime::etfYYYYMMDD));            
         }
         else
         {
            m_pObjNextElection->Text(g_ClientDAL.GetString(100106));
            m_pObjOverride->Enabled(false);      
         }

         if(m_pData->GvtType == m_iGvtId)
            return GUI::EEventStatus::Handled;

         if((m_pData->GvtType <= 7 && m_pData->GvtType >= 1) && (m_iGvtId == 8 || m_iGvtId == 9))
         {
            m_pObjNextElection->Text(g_ClientDAL.GetString(100323));
         }
      }
      if(l_pCombo == m_pObjCapital)
      {
         // retrieve our currently selected capital
         map<GString,UINT32>::const_iterator l_It = m_mCities.find(m_pObjCapital->Selected_Content());
         if(l_It != m_mCities.end())
         {
            UINT32 l_iSelectedCapitalId = l_It->second;
            // same as the one already selected ?
            if((UINT32)m_iCapitalId != l_iSelectedCapitalId)
            {
               // set up the new onw
               m_iCapitalId = l_iSelectedCapitalId;
               Dirty(true);
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

map<GString,UINT32> & GConstitutionalFormWindow::Cities()
{
   return m_mCities;
}

map<GString,INT32> & GConstitutionalFormWindow::GvtType()
{
   return m_mGvt;
}

bool GConstitutionalFormWindow::MartialLaw()
{
   return m_bMartialLaw;
}

void GConstitutionalFormWindow::MartialLaw(bool in_bMartialLaw)
{
   m_bMartialLaw = in_bMartialLaw;

   if(m_bMartialLaw)
   {
      m_pObjMartialLaw->Text(g_ClientDAL.GetString(100085));
      m_pObjNextElection->Text(g_ClientDAL.GetString(100106));
      m_pObjOverride->Enabled(false);
   }
   else
   {
      m_pObjMartialLaw->Text(g_ClientDAL.GetString(100062));
      INT32 m_iGvtType = m_mGvt.find(m_pObjConstitutionalForm->Selected_Content())->second;

      if(m_iGvtType == 8 || m_iGvtType == 9)
      {
         m_pObjNextElection->Text(m_NextElection.ToString(GDateTime::etfYYYYMMDD));
         
         if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
            m_pObjOverride->Enabled(true);
         else  
            m_pObjOverride->Enabled(false);
      }
   }
}

/*!
 * Update contitutional form window with data contained in GDataConstitutionalForm
 **/
void GConstitutionalFormWindow::Update(bool in_bForce)
{
   //If the window data has changed stop updating
   if(Dirty() && !in_bForce)
      return;

   //Depending if it's your country enable/disable stuff
   bool l_bMyCountry = g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID();
   m_pObjCapital           ->Enabled(l_bMyCountry);   
   m_pObjOverride          ->Enabled(l_bMyCountry);
   m_pObjMartialLaw        ->Enabled(l_bMyCountry);
	
	//If its not its country, it could still be enabled if the country controls 75% of the other country
	{
		if(!l_bMyCountry)
		{
			if(g_ClientDAL.ProportionGainedByCountryFromCountry(g_ClientDAL.ControlledCountryID(), g_ClientDCL.SelectedCountryID()) >= SP2::c_fPercentageNecessaryToChangeGovernmentType &&
				g_ClientDAL.DiplomaticStatus(g_ClientDAL.ControlledCountryID(),g_ClientDCL.SelectedCountryID()) != EDiplomaticStatus::Hostile)
				m_pObjConstitutionalForm->Enabled(true);
			else
				m_pObjConstitutionalForm->Enabled(false);				
		}		
	}

   //If we want to force the update, reset the window so it will rebuild itself
   if(in_bForce)
      Reset();

   INT32  l_iSelectedGvtType = -1;
   INT32  l_iSelectedCapital = -1;
   UINT32 l_iRegion          = 0;
   
   //Check if the capital is a city 
   const SP2::SCityInfo* l_City = g_ClientDAL.CapitalCity( g_ClientDCL.SelectedCountryID() );
   if(!l_City)
   {
      //If it's a region
      l_iRegion = g_ClientDAL.CapitalRegion( g_ClientDCL.SelectedCountryID() );
   }
   else
   {
      //If it's a city
      l_iSelectedCapital = l_City->m_iId;
   }
   
   //Set the data depending it's your country or not
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      l_iSelectedGvtType = g_ClientDAL.m_PlayerCountryData.GvtType();
      MartialLaw( g_ClientDAL.m_PlayerCountryData.MartialLaw() );
      m_pData->GvtType = l_iSelectedGvtType;
      m_pData->MartialLaw = g_ClientDAL.m_PlayerCountryData.MartialLaw();
   }
   else
   {
      l_iSelectedGvtType = m_pData->GvtType;
      MartialLaw( m_pData->MartialLaw );
   }

   //If the country is in anarchy disable all stuff related to constitutional form
   if(l_iSelectedGvtType == 1)
   {
      m_pObjConstitutionalForm->Clear();
      GvtType().clear();
      GString l_sGvtName = g_ClientDAL.GetString(g_ClientDAL.StringIdGvtType(1));
      GvtType().insert(pair<GString,INT32>(l_sGvtName,1));
      m_pObjConstitutionalForm->Add_Item(l_sGvtName);
      m_pObjConstitutionalForm->Selected_Content(l_sGvtName);
      m_pObjConstitutionalForm->Enabled(false);
   }

   const set<UINT32>& l_Regions = g_ClientDAL.CountryMilitaryControl(g_ClientDCL.SelectedCountryID());

   // no citites in the citites combo box
   if(!m_pObjCapital->NbItems())
   {
	   //Clear the cities 
	   Cities().clear();
      // fully clean combo box
      m_pObjCapital->Clear();

      //Fill city map with the table      
      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
      {
         for(set<UINT32>::const_iterator l_it = l_Regions.begin(); l_it != l_Regions.end(); l_it++)
         {
            vector<UINT32> l_vCities = g_ClientDAL.CitiesInRegion(*l_it);
            
            for(UINT32 i=0; i<l_vCities.size(); i++)
            {
               const SP2::SCityInfo* l_pCity = g_ClientDAL.City(l_vCities[i]);
               Cities().insert(pair<GString,UINT32>(g_ClientDAL.GetString(l_pCity->m_iNameStringID),l_pCity->m_iId));
            }
         }
      }

      if(!Cities().size() && l_iSelectedCapital > 0)
      {
         const SP2::SCityInfo* l_pCity = g_ClientDAL.City(l_iSelectedCapital);
         if(l_pCity)
         {
            Cities().insert(pair<GString,UINT32>(g_ClientDAL.GetString(l_pCity->m_iNameStringID),l_pCity->m_iId));
         }
      }

      //If there are any, add every city to combo box and do not fill if not necessary
      if(Cities().size())
      {
         map<GString,UINT32>::iterator it = Cities().begin();
         while(it != Cities().end())
         {
            if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
               m_pObjCapital->Add_Item(it->first);

            if(it->second == (UINT32)l_iSelectedCapital)
            {
               m_pObjCapital->Selected_Content(it->first);
            }
            
            it++;
         }
      }
      else
      {
         //If there is no cities, fill it with regions
         set<UINT32>::const_iterator l_ItRegion = l_Regions.begin();
         while(l_ItRegion != l_Regions.end())
         {
            if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
               m_pObjCapital->Add_Item( g_ClientDAL.RegionName(*l_ItRegion) );

            if(*l_ItRegion == l_iRegion)            
               m_pObjCapital->Selected_Content( g_ClientDAL.RegionName(*l_ItRegion) );

            l_ItRegion++;
         }
      }
      m_pObjCapital->Update();
   }
   else
   {
      if(Cities().size())
      {
         map<GString,UINT32>::iterator it;
         it = Cities().begin();
         while(it != Cities().end())
         {
            if(it->second == (UINT32)l_iSelectedCapital)
            {
               m_pObjCapital->Selected_Content(it->first);
               break;
            }
         
            it++;
         }
      }
      else
      {
         set<UINT32>::const_iterator l_ItRegion = l_Regions.begin();
         while(l_ItRegion != l_Regions.end())
         {
            if(*l_ItRegion == l_iRegion)            
            {
               m_pObjCapital->Selected_Content( g_ClientDAL.RegionName(*l_ItRegion) );
               break;
            }

            l_ItRegion++;
         }
      }
   }
   
   if(!m_pObjConstitutionalForm->NbItems())
   {
      GvtType().clear();
      m_pObjConstitutionalForm->Clear();

      //Fill gvt type map with the table
      for(UINT32 i=0; i<9; i++)
      {
         GString l_sGvtName = g_ClientDAL.GetString(g_ClientDAL.StringIdGvtType(i + 1));
         GvtType().insert(pair<GString,INT32>(l_sGvtName,i + 1));
      }

      //Add every gvt type to combo box depending if user has or not the control
      map<GString,INT32>::iterator it;
      it = GvtType().begin();
      if(l_iSelectedGvtType == 1 || l_iSelectedGvtType == 2 || l_iSelectedGvtType == 3)
      {
         while(it != GvtType().end())
         {
            if(it->second == l_iSelectedGvtType)
            {
               m_pObjConstitutionalForm->Add_Item(it->first);
               m_pObjConstitutionalForm->Selected_Content(it->first);
               break;
            }
            it++;
         }
      }
      else
      {
         map<GString,INT32>::iterator it;
         it = GvtType().begin();
         while(it != GvtType().end())
         {
            //Do not add government type that are not accessible for the user
            if(it->second == 1 || it->second == 2 || it->second == 3)
            {
               it++;
               continue;
            }

            m_pObjConstitutionalForm->Add_Item(it->first);
            if(it->second == l_iSelectedGvtType)
               m_pObjConstitutionalForm->Selected_Content(it->first);

            it++;
         }
      }
   }
   else
   {
      map<GString,INT32>::iterator it;
      it = GvtType().begin();
      while(it != GvtType().end())
      {
         if(it->second == l_iSelectedGvtType)
            m_pObjConstitutionalForm->Selected_Content(it->first);

         it++;
      }
   }

   if(g_ClientDAL.ControlledCountryID() != g_ClientDCL.SelectedCountryID())
   {
      GDateTime l_NextElection(m_pData->NextElection);
      m_NextElection = l_NextElection;
   }
   else
   {
      m_NextElection = g_ClientDAL.m_PlayerCountryData.NextElection();
   }
     
   //If the gvt type is a democracy be sure election override btn is enabled and set the date of next election
   if( (l_iSelectedGvtType == 8 || l_iSelectedGvtType == 9) && !m_pData->MartialLaw)
   {
      if(m_pData->GvtType == 8 || m_pData->GvtType == 9)
      {
         m_pObjNextElection->Text(m_NextElection.ToString(GDateTime::etfYYYYMMDD));
      }
      else
      {
         m_pObjNextElection->Text(g_ClientDAL.GetString(100323));
      }

      if(m_pData->GvtType == l_iSelectedGvtType)
      {
         if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
            m_pObjOverride->Enabled(true);
         else
            m_pObjOverride->Enabled(false);
      }
   }
   else
   {
      m_pObjNextElection->Text(g_ClientDAL.GetString(100106));
      m_pObjOverride->Enabled(false);
   }

   if(m_pData->MartialLaw)
      m_pObjMartialLaw->Text(g_ClientDAL.GetString(100085));
   else
      m_pObjMartialLaw->Text(g_ClientDAL.GetString(100062));
   
   // assign members vars 
   m_iGvtId     = l_iSelectedGvtType;
   m_iCapitalId = (l_iSelectedCapital != -1 ? l_iSelectedCapital : l_iRegion);

   Dirty(false);
}

void GConstitutionalFormWindow::Reset()
{
   m_mCities.clear();
   m_mGvt.clear();
   m_pObjCapital->Clear();
   m_pObjConstitutionalForm->Clear();
}