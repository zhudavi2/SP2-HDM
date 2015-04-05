/**************************************************************
*
* sp2_economic_health_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_economic_health_window.h"

const GString GEconomicHealthWindow::TypeName(L"Economic_Health_Window");

GUI::GBaseObject* GEconomicHealthWindow::New()
{
	return (GUI::GBaseObject*) new GEconomicHealthWindow();
}


GEconomicHealthWindow::GEconomicHealthWindow() : GBaseWindow()
{
   m_pData       = &(g_ClientDAL.DataEconomicHealthWindow);
   m_pWindowData = &(g_ClientDAL.DataEconomicHealthWindow);

   m_pObjInnerFrame           = NULL;
   m_pObjGdpTotal             = NULL;
   m_pObjGdpPerCapita         = NULL;
   m_pObjUnemploymentTxt      = NULL;
   m_pObjPovertyTxt           = NULL;
   m_pObjUnemploymentMtr      = NULL;
   m_pObjPovertyMtr           = NULL;
   m_pObjPersonalIncomeTaxVal = NULL;
   m_pObjInterestLevelVal     = NULL;
   m_pObjPersonalIncomeTax    = NULL;
   m_pObjInflationValue       = NULL;
   m_pObjInflationGrowth      = NULL;
   m_pObjInterestLevel        = NULL;
   m_pObjDelimiter            = NULL;
   m_bBeingEdited             = false;
   m_bWindowCanDisable = true;
}

GEconomicHealthWindow::~GEconomicHealthWindow()
{
}

bool GEconomicHealthWindow::Enabled()
{
   return m_bEnabled;
}

void GEconomicHealthWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   m_pObjInterestLevelVal->Enabled(in_bEnabled);
   m_pObjPersonalIncomeTaxVal->Enabled(in_bEnabled);
   
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"lblTitle"))->Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"lblTax"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"lblCapita"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"lblTotal"))->Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"lblUnemployment"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"lblPoverty"))->Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"lblInterest"))->Enabled(in_bEnabled);

   m_pObjGdpTotal->Enabled(in_bEnabled);
   m_pObjGdpPerCapita->Enabled(in_bEnabled);
   m_pObjUnemploymentTxt->Enabled(in_bEnabled);
   m_pObjPovertyTxt->Enabled(in_bEnabled);
   m_pObjInflationValue->Enabled(in_bEnabled);
}

bool GEconomicHealthWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrame = (GUI::GFrame*)Child(L"frmInner");

   m_pObjGdpTotal          = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"txtTotal");
   m_pObjGdpPerCapita      = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"txtCapita");
   m_pObjUnemploymentTxt   = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"txtUnemployment");
   m_pObjPovertyTxt        = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"txtPoverty");
   m_pObjUnemploymentMtr   = (SP2::GCursorMeter*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"frmUnemployment");
   m_pObjPovertyMtr        = (SP2::GCursorMeter*)m_pObjInnerFrame->Child(L"frmPopulation")->Child(L"frmPoverty");

   m_pObjPersonalIncomeTax = (GEditBoxNumeric*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"frmTax")->Child(L"edtText");
   m_pObjInflationValue    = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"txtInflation");
   m_pObjInflationGrowth   = (GUI::GLabel*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"txtInflationVar");
   m_pObjDelimiter         = (GUI::GPictureBox*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"picDelimiter");

   m_pObjInterestLevel     = (GEditBoxNumeric*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"frmInterest")->Child(L"edtText");

   m_pObjPersonalIncomeTaxVal   = (SP2::GUpdatableValue*)m_pObjInnerFrame->Child(L"frmGdp")->Child(L"frmTax");
   m_pObjInterestLevelVal       = (SP2::GUpdatableValue*)m_pObjInnerFrame->Child(L"frmInflation")->Child(L"frmInterest");

   m_pObjPersonalIncomeTaxVal->TextValueObj()->TextOffset(0,0);
   m_pObjInterestLevelVal->TextValueObj()->TextOffset(0,0);

   m_pObjPersonalIncomeTaxVal->TextValueObj()->TextAlignment(L"MiddleCenter");
   m_pObjInterestLevelVal->TextValueObj()->TextAlignment(L"MiddleCenter");

   m_pObjDelimiter->BringToFront();

   vector<UINT32> l_viStep;
   l_viStep.push_back(1);
   l_viStep.push_back(10);
   vector<REAL64> l_vfStep;
   l_vfStep.push_back(0.1f);
   l_vfStep.push_back(1.0f);   
   
   m_pObjPersonalIncomeTaxVal->Initialize(l_viStep, l_vfStep, L"", L" %",1,0, SP2::PersonalTaxes_LowerCap * 100, SP2::PersonalTaxes_UpperCap * 100);
   m_pObjInterestLevelVal->Initialize(l_viStep, l_vfStep, L"", L" %",1,0, SP2::InterestRate_LowerCap * 100, SP2::InterestRate_UpperCap * 100);

   m_pObjUnemploymentMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_RED));
   m_pObjPovertyMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_RED));

   if(Visible())
      Update();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GEconomicHealthWindow::OnShow()
{
   __super::OnShow();

   if(m_pObjInnerFrame)
   {
      m_fInterestLevel  = m_pData->m_fInterestLevel;
      m_fIncomeTax      = m_pData->m_fIncomeTax;

      Update();
   }

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Economic);
   g_ClientDCL.ClientAdvisor().Attach(this, EClientAdvisorNotification::EventWasCanceled);
}

void GEconomicHealthWindow::OnHide()
{
   __super::OnHide();
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Economic);
   g_ClientDCL.ClientAdvisor().Detach(this, EClientAdvisorNotification::EventWasCanceled);
   // remove the request associated with our economic health window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);
}


/*
 * On subject change (observer design pattern)
 */
void GEconomicHealthWindow::OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   // subject is dal ?
   if(&in_Subject == &g_ClientDAL)
   {      
      // is there any request pending for our economic health window, if yes do not notify
      if(!RequestPending())
         if(in_Notification == (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Economic)        
            Update();
   }
   // subject is advisor ?
   else if(&in_Subject == &g_ClientDCL.ClientAdvisor())
   {
      // economic health event ?
      if((UINT32)in_pAdditionalMsg == GAME_EVENT_TYPE(SP2::Event::GEconomicHealthUpdate))
      {
         // event wont be send ?
         if(in_Notification == (DesignPattern::GSubjectNotification)EClientAdvisorNotification::EventWasCanceled)
            g_ClientDCL.RequestManager().RemoveRequest(this);
      }
   }
}

GUI::EEventStatus::Enum GEconomicHealthWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if (in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if( ( in_pCaller->Name() == L"btnUp" || in_pCaller->Name() == L"btnDown" ) && in_pCaller->Enabled())
            Dirty(true);
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GEconomicHealthWindow::OnCustomEvent(UINT32 in_iEventID, const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == c_OnUpdatableValueChange)
   {
      Dirty(true);
   }
   else if(in_iEventID == c_iOnEditBoxEditBegin)
   {
      m_bBeingEdited = true;
   }  
   else if(in_iEventID == c_iOnEditBoxEditEnd)
   {
      m_bBeingEdited = false;
   }
   else
   {
      return GBaseObject::OnCustomEvent(in_iEventID, in_EventData, in_pCaller);
   }

   return EEventStatus::Handled;
}


void GEconomicHealthWindow::Update(bool in_bForceUpdate)
{
   if( ( Dirty() || m_bBeingEdited ) && !in_bForceUpdate)
      return;

   REAL64 l_fGdpValue        ;
   UINT64 l_iPop             ;
   REAL64 l_fPopUnemployed   ;
   REAL64 l_fPopPoverty      ;
   REAL64 l_fInflationLevel  ;
   REAL64 l_fInterestLevel   ;
   REAL64 l_fIncomeTax       ;

   //If it is our country that is selected, fetch the data from the country data
   if(g_ClientDCL.SelectedCountryID() == g_ClientDAL.ControlledCountryID())
   {
      l_fGdpValue        = g_ClientDAL.m_PlayerCountryData.GDPValue();
      l_iPop             = g_ClientDAL.m_PlayerCountryData.Population();
      l_fPopUnemployed   = g_ClientDAL.m_PlayerCountryData.Pop1565Unemployed();
      l_fPopPoverty      = g_ClientDAL.m_PlayerCountryData.PopInPoverty();
      l_fInflationLevel  = g_ClientDAL.m_PlayerCountryData.InflationLevel();
      l_fInterestLevel   = g_ClientDAL.m_PlayerCountryData.InterestLevel();
      l_fIncomeTax       = g_ClientDAL.m_PlayerCountryData.PersonalIncomeTax();
   }
   else
   {
      l_fGdpValue        = m_pData->m_fGdp;
      l_iPop             = m_pData->m_iPopulation;
      l_fPopUnemployed   = m_pData->m_fUnemploymentRate;
      l_fPopPoverty      = m_pData->m_fPovertyRate;
      l_fInflationLevel  = m_pData->m_fInflation;
      l_fInterestLevel   = m_pData->m_fInterestLevel;
      l_fIncomeTax       = m_pData->m_fIncomeTax;
   }

   GString l_sGdpValue        = GString::FormatNumber(l_fGdpValue,L" ",L"",L"$ ",L"",3,0);

   REAL64 l_fGdpPerCapita     = l_fGdpValue / l_iPop;
   GString l_sPop             = GString::FormatNumber(l_fGdpPerCapita,L" ",L"",L"$ ",L"",3,0);
   //Append the income index to the GDP per capita.
   l_sPop                    += L" (" + GString::FormatNumber(GHumanDevelopmentUtilities::FindIncomeIndex(l_fGdpPerCapita), 3) + L")";

   GString l_sPopUnemployed   = GString::FormatNumber(l_fPopUnemployed * 100,L"",L".",L"",L" %",0,1);
   GString l_sPopPoverty      = GString::FormatNumber(l_fPopPoverty * 100,L"",L".",L"",L" %",0,1);
   GString l_sInflationLevel  = GString::FormatNumber(l_fInflationLevel * 100,L"",L".",L"",L" %",0,1);

   INT32 l_iLeftMostPos    = 58;
   INT32 l_iRightMostPos   = 201 - m_pObjDelimiter->Width();

   if(l_fInflationLevel <= -0.12f)
      m_pObjDelimiter->Left(l_iLeftMostPos);
   else if(l_fInflationLevel >= 0.18f)
      m_pObjDelimiter->Left(l_iRightMostPos);
   else
   {
      REAL64 l_fTemp = l_fInflationLevel + 0.12f;
      l_fTemp = l_fTemp / 0.3f;

      m_pObjDelimiter->Left( (INT32)( ( (REAL64)(l_iRightMostPos - l_iLeftMostPos) * l_fTemp ) + (REAL64)l_iLeftMostPos ) );
   }

   m_pObjGdpTotal->Text(l_sGdpValue);
   m_pObjGdpPerCapita->Text(l_sPop);
   m_pObjUnemploymentTxt->Text(l_sPopUnemployed);
   m_pObjUnemploymentMtr->Value(l_fPopUnemployed,0);
   m_pObjPovertyTxt->Text(l_sPopPoverty);
   m_pObjPovertyMtr->Value(l_fPopPoverty,0);
   m_pObjInflationValue->Text(l_sInflationLevel);

   m_pObjInterestLevelVal->Value(l_fInterestLevel * 100);
   m_pObjPersonalIncomeTaxVal->Value(l_fIncomeTax * 100);

   Dirty(false);
}

void GEconomicHealthWindow::Save()
{
   if(!Dirty())
      return;

   m_fWhenSaved = g_Joshua.GameTime();
   
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEconomicHealthUpdate);
   SP2::Event::GEconomicHealthUpdate* l_pUpdateEvent = (SP2::Event::GEconomicHealthUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_bGetData       = false;
   l_pUpdateEvent->m_iCountryId     = g_ClientDAL.ControlledCountryID();
   l_pUpdateEvent->m_iSource        = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget        = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->m_fIncomeTax     = m_pObjPersonalIncomeTaxVal->Value() / 100;
   l_pUpdateEvent->m_fInterestLevel = m_pObjInterestLevelVal->Value() / 100;
   m_fIncomeTax                     = m_pObjPersonalIncomeTaxVal->Value() / 100;
   m_fInterestLevel                 = m_pObjInterestLevelVal->Value() / 100;
   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdateEvent);

   g_ClientDCL.ClientAdvisor().RaiseEvent(l_pEvent);
   Dirty(false);
   //g_ClientDAL.FetchEconomicHealthWindowInfo(g_ClientDAL.ControlledCountryID());
}