/**************************************************************
*
* sp2_budget_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_BudgetWindow_H_
#define _CSP2_BudgetWindow_H_

#include "sp2_meter_double.h"

namespace SP2
{
   class GBudgetWindow : public SP2::GBaseWindow, Hector::DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
	   GBudgetWindow();
	   ~GBudgetWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      bool Enabled();
      void Enabled(bool in_bEnabled);

      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      GUI::GFrame*   m_pObjIncome;
      GUI::GFrame*   m_pObjExpenses;
      GUI::GFrame*   m_pObjFixedExpenses;

      GUI::GLabel*   m_pObjIncomeTotal;
      GUI::GLabel*   m_pObjIncomePersonalIncomeTax;
      GUI::GLabel*   m_pObjIncomeTrade;
      GUI::GLabel*   m_pObjIncomeTourism;
      GUI::GLabel*   m_pObjIncomeImf;

      GUI::GLabel*   m_pObjExpensesTotal;
      GUI::GLabel*   m_pObjExpensesInfrastuctureLbl;
      GUI::GLabel*   m_pObjExpensesPropagandaLbl;
      GUI::GLabel*   m_pObjExpensesEnvironmentLbl;
      GUI::GLabel*   m_pObjExpensesHealthCareLbl;
      GUI::GLabel*   m_pObjExpensesEducationLbl;
      GUI::GLabel*   m_pObjExpensesTelecomLbl;
      GUI::GLabel*   m_pObjExpensesGovernmentLbl;
      GUI::GLabel*   m_pObjExpensesImfLbl;
      GUI::GLabel*   m_pObjExpensesResearchLbl;
      GUI::GLabel*   m_pObjExpensesTourismLbl;

      SP2::GMeterDouble* m_pObjExpensesInfrastucture;
      SP2::GMeterDouble* m_pObjExpensesPropaganda;
      SP2::GMeterDouble* m_pObjExpensesEnvironment;
      SP2::GMeterDouble* m_pObjExpensesHealthCare;
      SP2::GMeterDouble* m_pObjExpensesEducation;
      SP2::GMeterDouble* m_pObjExpensesTelecom;
      SP2::GMeterDouble* m_pObjExpensesGovernment;
      SP2::GMeterDouble* m_pObjExpensesImf;
      SP2::GMeterDouble* m_pObjExpensesResearch;
      SP2::GMeterDouble* m_pObjExpensesTourism;

      GUI::GLabel*   m_pObjFixedExpensesTotal;
      GUI::GLabel*   m_pObjFixedExpensesSecurity;
      GUI::GLabel*   m_pObjFixedExpensesDiplomacy;
      GUI::GLabel*   m_pObjFixedExpensesTrade;
      GUI::GLabel*   m_pObjFixedExpensesDebt;
      GUI::GLabel*   m_pObjFixedExpensesUnitsUpkeep;
      GUI::GLabel*   m_pObjFixedExpensesCorruption;

      GUI::GButton*  m_pObjPersonalIncomeTaxBtn;
      GUI::GButton*  m_pObjIncomeTradeBtn;
      GUI::GButton*  m_pObjSecurityBtn;
      GUI::GButton*  m_pObjExpensesTradeBtn;
      GUI::GButton*  m_pObjDiplomacyBtn;
      GUI::GButton*  m_pObjUnitUpkeepBtn;

      GUI::GLabel*   m_pObjSurplus;
      GUI::GLabel*   m_pObjAvailableFunds;



      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      bool Update();
      void Save();

      void OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      static const GString TypeName;
   
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      void   Surplus(REAL64 in_fValue);
      
      REAL64 m_fIncomeTotal;
      REAL64 m_fFixedExpensesTotal;

      bool   m_bEditing;
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateBudgetWindow();
};

#endif //_CSP2_BudgetWindow_H_
