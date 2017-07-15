/**************************************************************
*
* sp2_economic_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_economic_window.h"

#include "../include/sp2_budget_window.h"
#include "../include/sp2_economic_health_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_trade_window.h"
#include "../include/sp2_trade_detailed_window.h"

const GString GEconomicWindow::TypeName(L"Economic_Window");

/*!
 * Window allocator
 **/
GUI::GBaseObject* GEconomicWindow::New()
{
	return (GUI::GBaseObject*) new GEconomicWindow();
}

/*!
 * Constructor
 **/
GEconomicWindow::GEconomicWindow() : GBaseWindow()
{
   // Init the data wich is displayed by the window
   m_pData       = &(g_ClientDAL.DataEconomicWindow);
   m_pWindowData = &(g_ClientDAL.DataEconomicWindow);

   // Init our objects data
   m_pObjModel             = NULL;
   m_pObjHealth            = NULL;
   m_pObjResources         = NULL;
   m_pObjHealthTxt         = NULL;
   m_pObjResourcesTxt      = NULL;
   m_pObjIncomeTxt         = NULL;
   m_pObjExpensesTxt       = NULL;
   m_pObjBalanceTxt        = NULL;
   m_pObjAvailableMoneyTxt = NULL;
   m_pObjInnerFrm          = NULL;
   m_pObjMoreHealth        = NULL;
   m_pObjMoreBudget        = NULL;
   m_pObjMoreResources     = NULL;
   m_pObjBtnTrade             = NULL;
   m_pObjPlus1             = NULL;
   m_pObjPlus2             = NULL;
   m_pObjPlus3             = NULL;
   m_pObjPlus4             = NULL;
   
   m_pObjIncomeLbl         = NULL;
   m_pObjExpensesLbl       = NULL;
   m_pObjBalanceLbl        = NULL;
   m_pObjAvailableMoneyLbl = NULL;

   m_bWindowCanDisable = true;
}

/*!
 * Destructor
 **/
GEconomicWindow::~GEconomicWindow()
{
}

bool GEconomicWindow::Enabled()
{
   return m_bEnabled;
}

void GEconomicWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   m_pObjIncomeLbl->Enabled(in_bEnabled);
   m_pObjExpensesLbl->Enabled(in_bEnabled);
   m_pObjBalanceLbl->Enabled(in_bEnabled);
   m_pObjAvailableMoneyLbl->Enabled(in_bEnabled);
   m_pObjBtnTrade->Enabled(in_bEnabled);
   m_pObjHealthTxt->Enabled(in_bEnabled);
   m_pObjResourcesTxt->Enabled(in_bEnabled);
   m_pObjMoreBudget->Enabled(in_bEnabled);
   m_pObjIncomeTxt->Enabled(in_bEnabled);
   m_pObjExpensesTxt->Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmEconomicHealth")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmEconomicModel")->Child(L"lblTitle"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmEconomicModel")->Child(L"lblStateControlled"))->Enabled(in_bEnabled);
   ((GUI::GLabel*)m_pObjInnerFrm->Child(L"frmEconomicModel")->Child(L"lblFreeMarket"))->Enabled(in_bEnabled);
}

/*!
 * Called when the object is created
 **/
bool GEconomicWindow::OnCreate()
{
	__super::OnCreate();
   
   //Initialize often used pointer to window objects
   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   // Meters
   m_pObjModel     = (GIdeologyMeter*)m_pObjInnerFrm->Child(L"frmEconomicModel")->Child(L"frmModelCursor");
   m_pObjHealth    = (GCursorMeter*)m_pObjInnerFrm->Child(L"frmEconomicHealth")->Child(L"frmGradient");
   m_pObjResources = (GCursorMeter*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"frmGradient");

   // Labels
   m_pObjHealthTxt         = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmEconomicHealth")->Child(L"txtValue");
   m_pObjResourcesTxt      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"txtValue");
   m_pObjIncomeTxt         = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"txtIncome");
   m_pObjExpensesTxt       = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"txtExpenses");
   m_pObjBalanceTxt        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"txtBalance");
   m_pObjAvailableMoneyTxt = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"txtDebt");

   m_pObjIncomeLbl         = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"lblIncome");
   m_pObjExpensesLbl       = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"lblExpenses");
   m_pObjBalanceLbl        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"lblBalance");
   m_pObjAvailableMoneyLbl = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"lblAvailable");


   // Buttons
   m_pObjMoreHealth    = (GUI::GButton*)m_pObjInnerFrm->Child(L"frmEconomicHealth")->Child(L"btnMoreEconomicHealth");
   m_pObjMoreBudget    = (GUI::GButton*)m_pObjInnerFrm->Child(L"frmBudget")->Child(L"btnMoreBudget");
   m_pObjMoreResources = (GUI::GButton*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"btnMoreResources");
   m_pObjBtnTrade = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnTrade");

   // Picture Boxes
   m_pObjPlus1 = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"picPlus1");
   m_pObjPlus2 = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"picPlus2");
   m_pObjPlus3 = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"picPlus3");
   m_pObjPlus4 = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmResources")->Child(L"picPlus4");

   // Tooltips
   m_pObjMoreHealth->TooltipText(g_ClientDAL.GetString(100541));
   m_pObjMoreBudget->TooltipText(g_ClientDAL.GetString(100542));
   m_pObjMoreResources->TooltipText(g_ClientDAL.GetString(100543));

   //Bring to front object that are going to be behind
   m_pObjPlus1        ->BringToFront();
   m_pObjPlus2        ->BringToFront();
   m_pObjPlus3        ->BringToFront();
   m_pObjPlus4        ->BringToFront();
   m_pObjMoreHealth   ->BringToFront();
   m_pObjMoreBudget   ->BringToFront();
   m_pObjMoreResources->BringToFront();

   // Hide default unused gbasewindow buttons
   m_pObjApplyBtn->Hide();
   m_pObjLockTog ->Hide();

   // Initialize Cursor Meter to their good value and cursor picture
   m_pObjHealth->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjResources->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));  

   // Display the window at the good position
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   
   if(m_pWindowData->Left < 0)
      Position(3,(INT16)mode.m_Resolution.y - 99 - Height());

   if(Visible())
      Update();

	return true;
}

/*!
 * Called when the object is destroyed
 **/
bool GEconomicWindow::OnDestroy()
{
   return __super::OnDestroy();
}

/*!
 * Called when the object is displayed
 **/
void GEconomicWindow::OnShow()
{
   __super::OnShow();
  
   if(m_pObjInnerFrm)
      Update();

   // Fetch other country info if not our country selected
   if(g_ClientDAL.ControlledCountryID() != g_ClientDCL.SelectedCountryID())
      g_ClientDAL.FetchEconomicHealthWindowInfo(g_ClientDCL.SelectedCountryID());

   // Bring to front main bar and mini map
   if(g_ClientDDL.MainBar())
      g_ClientDDL.MainBar()->BringToFront();
   if(g_ClientDDL.MiniMap())
      g_ClientDDL.MiniMap()->BringToFront();

   // Set up window notifications 
   vector<DesignPattern::GSubjectNotification> l_vNotifications;
   l_vNotifications.push_back((DesignPattern::GSubjectNotification)ENotifyWindowMsg::EconomicLvl1);
   l_vNotifications.push_back((DesignPattern::GSubjectNotification)ENotifyWindowMsg::Economic);
   l_vNotifications.push_back((DesignPattern::GSubjectNotification)ENotifyWindowMsg::Resources);
   g_ClientDAL.Attach(this, l_vNotifications);
}

/*!
 * Called when the object is hidden
 **/
void GEconomicWindow::OnHide()
{
   // Release notifications
   g_ClientDAL.Detach(this);

   __super::OnHide();
}

/*!
 * Called when the object is clicked
 **/
GUI::EEventStatus::Enum GEconomicWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         // Economic Health window button
         if(in_pCaller == m_pObjMoreHealth)
         {
            if(!g_ClientDDL.EconomicHealthWindow())
               g_ClientDDL.EconomicHealthWindowSpawn();

            if(!g_ClientDDL.EconomicHealthWindow()->Visible())
               g_ClientDDL.EconomicHealthWindow()->Show();
            else
               g_ClientDDL.EconomicHealthWindow()->BringToFront();
         }

         // Resources window button
         else if(in_pCaller == m_pObjMoreResources)
         {
            if(!g_ClientDDL.ResourcesWindow())
               g_ClientDDL.ResourcesWindowSpawn();

            if(!g_ClientDDL.ResourcesWindow()->Visible())
               g_ClientDDL.ResourcesWindow()->Show();
            else
               g_ClientDDL.ResourcesWindow()->BringToFront();
         }

         // Budget window button
         else if(in_pCaller == m_pObjMoreBudget)
         {
            if(!g_ClientDDL.BudgetWindow())
               g_ClientDDL.BudgetWindowSpawn();

            if(g_ClientDDL.BudgetWindow()->Minimized())
               g_ClientDDL.BudgetWindow()->Maximize();

            if(!g_ClientDDL.BudgetWindow()->Visible())
               g_ClientDDL.BudgetWindow()->Show();
            else
               g_ClientDDL.BudgetWindow()->BringToFront();
         }

         // Trade system window
         else if(in_pCaller == m_pObjBtnTrade)
         {
            if(!g_ClientDDL.TradeDetailedWindow())
               g_ClientDDL.TradeDetailedWindowSpawn();

            if(!g_ClientDDL.TradeDetailedWindow()->Visible())
               g_ClientDDL.TradeDetailedWindow()->Show();
            else
               g_ClientDDL.TradeDetailedWindow()->BringToFront();
         }

         // Exit button
         else if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
            g_ClientDDL.KillWindowsEconomicLv2();
            g_ClientDDL.MainBar()->m_pObjEconomic->ToggleState(false);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

/*!
 * Called when the data has been modified
 **/
void GEconomicWindow::OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject, const Hector::DesignPattern::GSubjectNotification& in_Notification, void* in_pAdditionalMsg)
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::EconomicLvl1)
      Update();
   else if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Economic)
      Update();
   else if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Budget)
      Update();
   else if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Resources)
      Update();
}

/*!
 * Refresh the window with the data in memory
 **/
bool GEconomicWindow::Update()
{     
   REAL64 l_fExpense        = 0.f;
   REAL64 l_fIncome         = 0.f;
   REAL64 l_fAvailableMoney = 0.f;
   REAL64 l_fBalance        = 0.f;
   REAL64 l_fRessRatio      = 0.f;
   REAL32 l_fEconomicModel  = 0.f;
   REAL32 l_fHealth         = 0.f;

   // Set up data displaying if its our country
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID()) 
   {
      l_fExpense        = g_ClientDAL.m_PlayerCountryData.BudgetExpenses();
      l_fIncome         = g_ClientDAL.m_PlayerCountryData.BudgetRevenues();
      l_fAvailableMoney = g_ClientDAL.m_PlayerCountryData.MonetarySupplyAmount();
      l_fBalance        = l_fIncome - l_fExpense;

      
      REAL64 l_fResProd = 0;
      REAL64 l_fResDemd = 0;
      for(UINT32 i = 0;i < EResources::ItemCount;i ++)
      {
         l_fResProd += g_ClientDAL.m_PlayerCountryData.ResourceProduction( (EResources::Enum) i );
         l_fResProd += g_ClientDAL.m_PlayerCountryData.ResourceImport( (EResources::Enum) i );

         l_fResDemd += g_ClientDAL.m_PlayerCountryData.ResourceExport( (EResources::Enum) i );
         l_fResDemd += g_ClientDAL.m_PlayerCountryData.ResourceDemand( (EResources::Enum) i );
      }

      if( l_fResDemd > 0.f )
         l_fRessRatio = l_fResProd / l_fResDemd;

      l_fEconomicModel  = g_ClientDAL.m_PlayerCountryData.CountryEconomicModel();
      l_fHealth         = g_ClientDAL.m_PlayerCountryData.EconomicHealth() * 100.f;
   }
   // Set up data displying for another country than our
   else
   {
      l_fExpense        = m_pData->Expense;
      l_fIncome         = m_pData->Income;
      l_fAvailableMoney = m_pData->AvailableMoney;
      l_fBalance        = l_fIncome - l_fExpense;
      l_fRessRatio      = m_pData->RessRatio;
      l_fEconomicModel  = m_pData->EconomicModel;
      l_fHealth         = m_pData->EconomicHealth * 100.f;
   }

   // Set up string to display in labels
   GString l_sHealth;
   if(l_fHealth == 100.f)
      l_sHealth = GString::FormatNumber(l_fHealth,L"",L".",L"",L" %",0,0);
   else
      l_sHealth = GString::FormatNumber(l_fHealth,L"",L".",L"",L" %",0,1);

   GString l_sIncome         = GString::FormatNumber(l_fIncome,L" ",L".",L"$ ",L"",3,0);
   GString l_sExpenses       = GString::FormatNumber(l_fExpense,L" ",L".",L"$ ",L"",3,0);
   GString l_sBalance        = GString::FormatNumber(l_fBalance,L" ",L".",L"$ ",L"",3,0);
   GString l_sAvailableMoney = GString::FormatNumber(l_fAvailableMoney,L" ",L".",L"$ ",L"",3,0);

   // Set up meters values
   m_pObjModel    ->Value(l_fEconomicModel, 0);
   m_pObjHealth   ->Value(l_fHealth / 100,  0);
   m_pObjResources->Value(l_fRessRatio,     0);

   // No plus in resources at first
   m_pObjPlus1->Hide();
   m_pObjPlus2->Hide();
   m_pObjPlus3->Hide();
   m_pObjPlus4->Hide();

   //If the ressources ratio is greater than 1 show plus pictures
   if(l_fRessRatio > 1)
   {
      m_pObjPlus1->Show();

      if(l_fRessRatio > 1.25)
      {
         m_pObjPlus2->Show();

         if(l_fRessRatio > 1.5)
         {
            m_pObjPlus3->Show();

            if(l_fRessRatio > 1.75)
            {
               m_pObjPlus4->Show();
            }
         }
      }
   }

   // Set up income and expenses text
   m_pObjIncomeTxt  ->Text(l_sIncome);
   m_pObjExpensesTxt->Text(l_sExpenses);
   
   // Balance
   if(l_fBalance < 0)
      m_pObjBalanceTxt->FontColorNormal(GColorRGBInt(233, 119, 119, 255));
   else
      m_pObjBalanceTxt->FontColorNormal(GColorRGBInt(200,200,200,255));
   m_pObjBalanceTxt->Text(l_sBalance);

   // Available money
   if(l_fAvailableMoney < 0)
      m_pObjAvailableMoneyTxt->FontColorNormal(GColorRGBInt(233, 119, 119, 255));
   else
      m_pObjAvailableMoneyTxt->FontColorNormal(GColorRGBInt(200,200,200,255));   
   m_pObjAvailableMoneyTxt->Text(l_sAvailableMoney);
   
   m_pObjHealthTxt   ->Text(l_sHealth);
   m_pObjResourcesTxt->Text(GString::FormatNumber(l_fRessRatio * 100,L"",L".",L"",L" %",0,0));

   return true;
}