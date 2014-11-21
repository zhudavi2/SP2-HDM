/**************************************************************
*
* sp2_budget_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_budget_window.h"
#include "../include/sp2_unit_research_window.h"
#include "../include/sp2_economic_health_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_treaties_window.h"
#include "../include/sp2_covert_actions_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_frame_updatable_value.h"

const GString GBudgetWindow::TypeName(L"Budget_Window");

GUI::GBaseObject* SP2::GBudgetWindow::New()
{
	return (GUI::GBaseObject*) new GBudgetWindow();
}

GBudgetWindow::GBudgetWindow() : SP2::GBaseWindow()
{
   m_bEditing = false;
   m_bWindowCanDisable = true;
}

GBudgetWindow::~GBudgetWindow()
{
}

bool GBudgetWindow::Enabled()
{
   return m_bEnabled;
}

void GBudgetWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);
}

bool GBudgetWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm                 = (GUI::GFrame*)Child(L"frmInner");
   ((GFX::GWindow*)m_pObjInnerFrm->Model()->Get_Primitive())->ClipChildren(true);
   m_pObjIncome                     = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmIncome");
   m_pObjExpenses                   = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmExpenses");
   m_pObjFixedExpenses              = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmFixedExpenses");

   m_pObjIncomeTotal                = (GUI::GLabel*)m_pObjIncome->Child(L"txtTotal");
   m_pObjIncomePersonalIncomeTax    = (GUI::GLabel*)m_pObjIncome->Child(L"txtPersonnalIncomeTax");
   m_pObjIncomeTrade                = (GUI::GLabel*)m_pObjIncome->Child(L"txtTrade");
   m_pObjIncomeTourism              = (GUI::GLabel*)m_pObjIncome->Child(L"txtTourism");
   m_pObjIncomeImf                  = (GUI::GLabel*)m_pObjIncome->Child(L"txtImf");

   m_pObjExpensesTotal              = (GUI::GLabel*)m_pObjExpenses->Child(L"txtTotal");
   m_pObjExpensesInfrastuctureLbl   = (GUI::GLabel*)m_pObjExpenses->Child(L"txtInfrastructure");
   m_pObjExpensesPropagandaLbl      = (GUI::GLabel*)m_pObjExpenses->Child(L"txtPropaganda");
   m_pObjExpensesEnvironmentLbl     = (GUI::GLabel*)m_pObjExpenses->Child(L"txtEnvironment");
   m_pObjExpensesHealthCareLbl      = (GUI::GLabel*)m_pObjExpenses->Child(L"txtHealthCare");
   m_pObjExpensesEducationLbl       = (GUI::GLabel*)m_pObjExpenses->Child(L"txtEducation");
   m_pObjExpensesTelecomLbl         = (GUI::GLabel*)m_pObjExpenses->Child(L"txtTelecom");
   m_pObjExpensesGovernmentLbl      = (GUI::GLabel*)m_pObjExpenses->Child(L"txtGovernment");
   m_pObjExpensesImfLbl             = (GUI::GLabel*)m_pObjExpenses->Child(L"txtImf");
   m_pObjExpensesResearchLbl        = (GUI::GLabel*)m_pObjExpenses->Child(L"txtResearch");
   m_pObjExpensesTourismLbl         = (GUI::GLabel*)m_pObjExpenses->Child(L"txtTourism");

   m_pObjExpensesInfrastucture      = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterInfrastructure");
   m_pObjExpensesPropaganda         = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterPropaganda");
   m_pObjExpensesEnvironment        = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterEnvironment");
   m_pObjExpensesHealthCare         = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterHealthCare");
   m_pObjExpensesEducation          = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterEducation");
   m_pObjExpensesTelecom            = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterTelecom");
   m_pObjExpensesGovernment         = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterGovernment");
   m_pObjExpensesImf                = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterImf");
   m_pObjExpensesResearch           = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterResearch");
   m_pObjExpensesTourism            = (SP2::GMeterDouble*)m_pObjExpenses->Child(L"frmMeterTourism");

   m_pObjFixedExpensesTotal         = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtTotal");
   m_pObjFixedExpensesSecurity      = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtSecurity");
   m_pObjFixedExpensesDiplomacy     = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtDiplomacy");
   m_pObjFixedExpensesTrade         = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtTrade");
   m_pObjFixedExpensesDebt          = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtDebt");
   m_pObjFixedExpensesUnitsUpkeep   = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtUnitsUpkeep");
   m_pObjFixedExpensesCorruption    = (GUI::GLabel*)m_pObjFixedExpenses->Child(L"txtCorruption");
   
   m_pObjPersonalIncomeTaxBtn       = (GUI::GButton*)m_pObjIncome->Child(L"btnPersonalIncomeTax");
   m_pObjIncomeTradeBtn             = (GUI::GButton*)m_pObjIncome->Child(L"btnTrade");
   m_pObjSecurityBtn                = (GUI::GButton*)m_pObjFixedExpenses->Child(L"btnSecurity");
   m_pObjExpensesTradeBtn           = (GUI::GButton*)m_pObjFixedExpenses->Child(L"btnTrade");
   m_pObjDiplomacyBtn               = (GUI::GButton*)m_pObjFixedExpenses->Child(L"btnDiplomacy");
   m_pObjUnitUpkeepBtn              = (GUI::GButton*)m_pObjFixedExpenses->Child(L"btnUnitUpkeep");

   m_pObjSurplus                    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBalance")->Child(L"txtAmount");
   m_pObjAvailableFunds             = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmAvailable")->Child(L"txtAmount");
   m_pObjColorGradientPic           = (GUI::GPictureBox*)Child(L"picBar");

   m_pObjPersonalIncomeTaxBtn->BringToFront();
   m_pObjIncomeTradeBtn->BringToFront();
   m_pObjSecurityBtn->BringToFront();
   m_pObjExpensesTradeBtn->BringToFront();
   m_pObjDiplomacyBtn->BringToFront();
   m_pObjUnitUpkeepBtn->BringToFront();

   m_pObjIncomeTotal->FontColorNormal(GColorRGBInt(113, 230, 111, 255));
   m_pObjExpensesTotal->FontColorNormal(GColorRGBInt(233, 119, 119, 255));
   m_pObjFixedExpensesTotal->FontColorNormal(GColorRGBInt(233, 119, 119, 255));

	return true;
}

GUI::EEventStatus::Enum GBudgetWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch(in_EventID)
   {
   case c_OnMeterDoubleChange:
   case c_OnUpdatableValueChange:
   case c_iOnEditBoxEditBegin:
   case c_iOnEditBoxEditEnd:
      break;
   default:
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);
   }

   if(in_EventID == c_iOnEditBoxEditBegin)
   {
      m_bEditing = true;
   }

   if(in_EventID == c_iOnEditBoxEditEnd)
   {
      m_bEditing = false;
   }

   if(in_EventID == c_OnMeterDoubleChange)
   {
      REAL64 l_fValue = 0.f;

      if(in_pCaller == m_pObjExpensesInfrastucture)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseInfrastructureReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesInfrastuctureLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }

      if(in_pCaller == m_pObjExpensesPropaganda)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpensePropagandaReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesPropagandaLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesEnvironment)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseEnvironmentReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesEnvironmentLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesHealthCare)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseHealthcareReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesHealthCareLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesEducation)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseEducationReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesEducationLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesTelecom)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseTelecomReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesTelecomLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesGovernment)
      {
         l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseGovernmentReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesGovernmentLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesImf)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseIMFReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesImfLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesResearch)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseResearchReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesResearchLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }
      
      if(in_pCaller == m_pObjExpensesTourism)
      {
			l_fValue = g_ClientDAL.m_PlayerCountryData.BudgetExpenseTourismReturn(((SP2::GMeterDouble*)in_pCaller)->Value());
         m_pObjExpensesTourismLbl->Text(GString::FormatNumber(l_fValue, L" ", L".", L"$ ", L"", 3, 0, true));
      }

      //Set expenses meter and values
      REAL64 l_fExpensesTotal = 0.f;
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseInfrastructureReturn(m_pObjExpensesInfrastucture->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpensePropagandaReturn(m_pObjExpensesPropaganda->Value());
      l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseEnvironmentReturn(m_pObjExpensesEnvironment->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseHealthcareReturn(m_pObjExpensesHealthCare->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseEducationReturn(m_pObjExpensesEducation->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseTelecomReturn(m_pObjExpensesTelecom->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseGovernmentReturn(m_pObjExpensesGovernment->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseIMFReturn(m_pObjExpensesImf->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseResearchReturn(m_pObjExpensesResearch->Value());
		l_fExpensesTotal += g_ClientDAL.m_PlayerCountryData.BudgetExpenseTourismReturn(m_pObjExpensesTourism->Value());

      m_pObjExpensesTotal->Text( (GString::FormatNumber(l_fExpensesTotal, L" ", L".", L"$ ", L"", 3, 0, false) ) );

      Surplus(m_fIncomeTotal - l_fExpensesTotal - m_fFixedExpensesTotal);

      Dirty(true);
   }

   return GUI::EEventStatus::Handled;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GBudgetWindow::OnShow()
{
   __super::OnShow();

   Dirty(false);
   
   if(m_pObjInnerFrm)
      Update();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
   
   
   g_ClientDAL.Attach(this, ENotifyWindowMsg::Budget);
}

GUI::EEventStatus::Enum GBudgetWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);   

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjPersonalIncomeTaxBtn)
         {
            if(!g_ClientDDL.EconomicHealthWindow())
               g_ClientDDL.EconomicHealthWindowSpawn();

            if(!g_ClientDDL.EconomicHealthWindow()->Visible())
               g_ClientDDL.EconomicHealthWindow()->Show();
            else
               g_ClientDDL.EconomicHealthWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjIncomeTradeBtn)
         {
            if(!g_ClientDDL.ResourcesWindow())
               g_ClientDDL.ResourcesWindowSpawn();

            if(!g_ClientDDL.ResourcesWindow()->Visible())
               g_ClientDDL.ResourcesWindow()->Show();
            else
               g_ClientDDL.ResourcesWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjExpensesTradeBtn)
         {
            if(!g_ClientDDL.ResourcesWindow())
               g_ClientDDL.ResourcesWindowSpawn();

            if(!g_ClientDDL.ResourcesWindow()->Visible())
               g_ClientDDL.ResourcesWindow()->Show();
            else
               g_ClientDDL.ResourcesWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjDiplomacyBtn)
         {
            if(!g_ClientDDL.TreatiesWindow())
               g_ClientDDL.TreatiesWindowSpawn();

            if(!g_ClientDDL.TreatiesWindow()->Visible())
            {
               g_ClientDDL.TreatiesWindow()->FilterMember( g_ClientDAL.Country(g_ClientDAL.ControlledCountryID()).Name(), false);
               g_ClientDDL.TreatiesWindow()->FilterType( ETreatyType::Undefined, false);
               g_ClientDDL.TreatiesWindow()->Show();
            }
            else
               g_ClientDDL.TreatiesWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjSecurityBtn)
         {
            if(!g_ClientDDL.CovertActionsWindow())
               g_ClientDDL.CovertActionsWindowSpawn();

            if(!g_ClientDDL.CovertActionsWindow()->Visible())
               g_ClientDDL.CovertActionsWindow()->Show();
            else
               g_ClientDDL.CovertActionsWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjUnitUpkeepBtn)
         {
            if(!g_ClientDDL.UnitListWindowNew())
               g_ClientDDL.UnitListWindowNewSpawn();

            g_ClientDDL.UnitListWindowNew()->UpdateUnitList( g_ClientDAL.ControlledCountryID(), -1 );
            
            if(!g_ClientDDL.UnitListWindowNew()->Visible())
               g_ClientDDL.UnitListWindowNew()->Show();
            else
               g_ClientDDL.UnitListWindowNew()->BringToFront();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GBudgetWindow::Save()
{
   if(!Dirty())
      return;

   // save it
   m_fWhenSaved = g_Joshua.GameTime();

   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GBudgetUpdate);
   SP2::Event::GBudgetUpdate* l_pUpdate = (SP2::Event::GBudgetUpdate*)l_pEvent.get();

   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->CountryId = g_ClientDAL.ControlledCountryID();

   l_pUpdate->BudgetExpenseInfrastructure = m_pObjExpensesInfrastucture->Value();
   l_pUpdate->BudgetExpensePropaganda     = m_pObjExpensesPropaganda->Value();
   l_pUpdate->BudgetExpenseEnvironment    = m_pObjExpensesEnvironment->Value();
   l_pUpdate->BudgetExpenseHealthcare     = m_pObjExpensesHealthCare->Value();
   l_pUpdate->BudgetExpenseEducation      = m_pObjExpensesEducation->Value();
   l_pUpdate->BudgetExpenseTelecom        = m_pObjExpensesTelecom->Value();
   l_pUpdate->BudgetExpenseGovernment     = m_pObjExpensesGovernment->Value();
   l_pUpdate->BudgetExpenseImf            = m_pObjExpensesImf->Value();
   l_pUpdate->BudgetExpenseResearch       = m_pObjExpensesResearch->Value();
   l_pUpdate->BudgetExpenseTourism        = m_pObjExpensesTourism->Value();
   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdate);
   
   g_Joshua.RaiseEvent(l_pEvent);

   Dirty(false);

   //g_ClientDAL.FetchEconomicWindowInfo(g_ClientDAL.ControlledCountryID());

   if(g_ClientDDL.UnitResearchWindow() && g_ClientDDL.UnitResearchWindow()->Visible())
   {
      g_ClientDDL.UnitResearchWindow()->Update( (REAL32) (g_ClientDAL.m_PlayerCountryData.TotalDemand() * 
                                                SP2::c_fBudgetCalculatingConstant * 
                                                SP2::c_fBudgetResearchValue * 1.f * 
                                                l_pUpdate->BudgetExpenseResearch) );
   }
}

bool GBudgetWindow::Update()
{ 
   // no update from server if dirty or editing
   if(m_bEditing || Dirty())
      return true;

   SP2::GCountryDataClient & l_Data = g_ClientDAL.m_PlayerCountryData;
   m_fIncomeTotal = 0.f;

   //Set income text
   for(INT32 i=0; i<4; i++)
   {
      switch(i)
      {
      case 0:
         m_pObjIncomePersonalIncomeTax->Text(GString::FormatNumber(l_Data.BudgetRevenueTax(), L" ", L".", L"$ ", L"", 3, 0));
         m_fIncomeTotal += l_Data.BudgetRevenueTax();
         break;
      case 1:
         m_pObjIncomeTrade->Text(GString::FormatNumber(l_Data.BudgetRevenueTrade(), L" ", L".", L"$ ", L"", 3, 0));
         m_fIncomeTotal += l_Data.BudgetRevenueTrade();
         break;
      case 2:
         m_pObjIncomeTourism->Text(GString::FormatNumber(l_Data.BudgetRevenueTourism(), L" ", L".", L"$ ", L"", 3, 0));
         m_fIncomeTotal += l_Data.BudgetRevenueTourism();
         break;
      case 3:
         m_pObjIncomeImf->Text(GString::FormatNumber(l_Data.BudgetRevenueIMF(), L" ", L".", L"$ ", L"", 3, 0));   
         m_fIncomeTotal += l_Data.BudgetRevenueIMF();
         break;
      }
   }
   m_pObjIncomeTotal->Text(GString::FormatNumber(m_fIncomeTotal, L" ", L".", L"$ ", L"", 3, 0));

   //Set expenses meter and values
   REAL64 l_fExpensesTotal = 0.f;

	REAL64 l_fExpensesInfrastructure = g_ClientDAL.m_PlayerCountryData.BudgetExpenseInfrastructureReturn(l_Data.BudgetExpenseInfrastructureRatio());
   REAL64 l_fExpensesPropaganda     = g_ClientDAL.m_PlayerCountryData.BudgetExpensePropagandaReturn(l_Data.BudgetExpensePropagandaRatio());
   REAL64 l_fExpensesEnvironment    = g_ClientDAL.m_PlayerCountryData.BudgetExpenseEnvironmentReturn(l_Data.BudgetExpenseEnvironmentRatio());
   REAL64 l_fExpensesHealthcare     = g_ClientDAL.m_PlayerCountryData.BudgetExpenseHealthcareReturn(l_Data.BudgetExpenseHealthcareRatio());
   REAL64 l_fExpensesEducation      = g_ClientDAL.m_PlayerCountryData.BudgetExpenseEducationReturn(l_Data.BudgetExpenseEducationRatio());
   REAL64 l_fExpensesTelecom        = g_ClientDAL.m_PlayerCountryData.BudgetExpenseTelecomReturn(l_Data.BudgetExpenseTelecomRatio());
   REAL64 l_fExpensesGovernment     = g_ClientDAL.m_PlayerCountryData.BudgetExpenseGovernmentReturn(l_Data.BudgetExpenseGovernmentRatio());
   REAL64 l_fExpensesImf            = g_ClientDAL.m_PlayerCountryData.BudgetExpenseIMFReturn(l_Data.BudgetExpenseIMFRatio());
   REAL64 l_fExpensesResearch       = g_ClientDAL.m_PlayerCountryData.BudgetExpenseResearchReturn(l_Data.BudgetExpenseResearchRatio());
   REAL64 l_fExpensesTourism        = g_ClientDAL.m_PlayerCountryData.BudgetExpenseTourismReturn(l_Data.BudgetExpenseTourismRatio());

   l_fExpensesTotal += l_fExpensesInfrastructure;
   l_fExpensesTotal += l_fExpensesPropaganda;
   l_fExpensesTotal += l_fExpensesEnvironment;
   l_fExpensesTotal += l_fExpensesHealthcare;
   l_fExpensesTotal += l_fExpensesEducation;
   l_fExpensesTotal += l_fExpensesTelecom;
   l_fExpensesTotal += l_fExpensesGovernment;
   l_fExpensesTotal += l_fExpensesImf;
   l_fExpensesTotal += l_fExpensesResearch;
   l_fExpensesTotal += l_fExpensesTourism;

   if(!Dirty())
   {
      m_pObjExpensesTotal->Text( (GString::FormatNumber(l_fExpensesTotal, L" ", L".", L"$ ", L"", 3, 0, false) ) );

      m_pObjExpensesInfrastucture->Value(l_Data.BudgetExpenseInfrastructureRatio(), false);
      m_pObjExpensesInfrastuctureLbl->Text(GString::FormatNumber(l_fExpensesInfrastructure, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesPropaganda->Value(l_Data.BudgetExpensePropagandaRatio(), false);
      m_pObjExpensesPropagandaLbl->Text(GString::FormatNumber(l_fExpensesPropaganda, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesEnvironment->Value(l_Data.BudgetExpenseEnvironmentRatio(), false);
      m_pObjExpensesEnvironmentLbl->Text(GString::FormatNumber(l_fExpensesEnvironment, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesHealthCare->Value(l_Data.BudgetExpenseHealthcareRatio(), false);
      m_pObjExpensesHealthCareLbl->Text(GString::FormatNumber(l_fExpensesHealthcare, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesEducation->Value(l_Data.BudgetExpenseEducationRatio(), false);
      m_pObjExpensesEducationLbl->Text(GString::FormatNumber(l_fExpensesEducation, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesTelecom->Value(l_Data.BudgetExpenseTelecomRatio(), false);
      m_pObjExpensesTelecomLbl->Text(GString::FormatNumber(l_fExpensesTelecom, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesGovernment->Value(l_Data.BudgetExpenseGovernmentRatio(), false);
      m_pObjExpensesGovernmentLbl->Text(GString::FormatNumber(l_fExpensesGovernment, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesImf->Value(l_Data.BudgetExpenseIMFRatio(), false);
      m_pObjExpensesImfLbl->Text(GString::FormatNumber(l_fExpensesImf, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesResearch->Value(l_Data.BudgetExpenseResearchRatio(), false);
      m_pObjExpensesResearchLbl->Text(GString::FormatNumber(l_fExpensesResearch, L" ", L".", L"$ ", L"", 3, 0, true));

      m_pObjExpensesTourism->Value(l_Data.BudgetExpenseTourismRatio(), false);
      m_pObjExpensesTourismLbl->Text(GString::FormatNumber(l_fExpensesTourism, L" ", L".", L"$ ", L"", 3, 0, true));
   }

   //Set Fixed expenses
   m_fFixedExpensesTotal               = 0.f;
   REAL64 l_fFixedExpensesDebt         = l_Data.BudgetExpenseDebt();
   REAL64 l_fFixedExpensesDiplomacy    = l_Data.BudgetExpenseDiplomacy();
   REAL64 l_fFixedExpensesSecurity     = l_Data.BudgetExpenseSecurity();
   REAL64 l_fFixedExpensesTrade        = l_Data.BudgetExpenseImports();
   REAL64 l_fFixedExpensesUnitsUpkeep  = l_Data.BudgetExpenseUnitUpkeep();
   REAL64 l_fFixedExpensesCorruption   = l_Data.BudgetExpenseCorruption();

   m_fFixedExpensesTotal += l_fFixedExpensesDebt;
   m_fFixedExpensesTotal += l_fFixedExpensesDiplomacy;
   m_fFixedExpensesTotal += l_fFixedExpensesSecurity;
   m_fFixedExpensesTotal += l_fFixedExpensesTrade;
   m_fFixedExpensesTotal += l_fFixedExpensesUnitsUpkeep;
   m_fFixedExpensesTotal += l_fFixedExpensesCorruption;
   
   m_pObjFixedExpensesTotal->Text(GString::FormatNumber(m_fFixedExpensesTotal, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesDebt->Text(GString::FormatNumber(l_fFixedExpensesDebt, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesDiplomacy->Text(GString::FormatNumber(l_fFixedExpensesDiplomacy, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesSecurity->Text(GString::FormatNumber(l_fFixedExpensesSecurity, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesTrade->Text(GString::FormatNumber(l_fFixedExpensesTrade, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesUnitsUpkeep->Text(GString::FormatNumber(l_fFixedExpensesUnitsUpkeep, L" ", L".", L"$ ", L"", 3, 0, false));
   m_pObjFixedExpensesCorruption->Text(GString::FormatNumber(l_fFixedExpensesCorruption, L" ", L".", L"$ ", L"", 3, 0, false));

   //Set Balance
   Surplus(m_fIncomeTotal - l_fExpensesTotal - m_fFixedExpensesTotal);
   
   //Set Available Funds
   if(l_Data.MonetarySupplyAmount() >= 0)
      m_pObjAvailableFunds->FontColorNormal(GColorRGBInt(113, 230, 111, 255));
   else
      m_pObjAvailableFunds->FontColorNormal(GColorRGBInt(233, 119, 119, 255));
   
   m_pObjAvailableFunds->Text(GString::FormatNumber(l_Data.MonetarySupplyAmount(), L" ", L".", L"$ ", L"", 3, 0, false));

   return true;
}

void GBudgetWindow::Surplus(REAL64 in_fValue)
{
   if(in_fValue >= 0)
      m_pObjSurplus->FontColorNormal(GColorRGBInt(113, 230, 111, 255));
   else
      m_pObjSurplus->FontColorNormal(GColorRGBInt(233, 119, 119, 255));
   
   m_pObjSurplus->Text(GString::FormatNumber(in_fValue, L" ", L".", L"$ ", L"", 3, 0, false));
}


void GBudgetWindow::OnSubjectChange(DesignPattern::GSubject&  in_Subject,
                                    const DesignPattern::GSubjectNotification& in_Notification,
                                    void* in_pAdditionalMsg )
{
   // is there any request pending for our budget window, if yes do not notify
   if(!RequestPending())
      if(&in_Subject == &g_ClientDAL)
         if(in_Notification == (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Budget)
            Update();
}

void GBudgetWindow::OnHide()
{
   __super::OnHide();
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Budget);
   // remove the request associated with our budget window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);
}