/**************************************************************
*
* sp2_economic_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_EconomicWindow_H_
#define _CSP2_EconomicWindow_H_

namespace SP2
{
   class GEconomicWindow : public GBaseWindow, DesignPattern::GObserver
   {
   public:
      //! Window type name
      static const GString TypeName;

      //! constructor and destructor
	   GEconomicWindow();
	   ~GEconomicWindow();

      //! Refresh window display
      bool Update();

      bool Enabled();
      void Enabled(bool in_bEnabled);
     
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      //! Event handlers
	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      //! Mouse events handlers
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      //! Subject changes 
      void OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject, const Hector::DesignPattern::GSubjectNotification& in_Notification, void* in_pAdditionalMsg);
    
      //! Meters
      class GIdeologyMeter*   m_pObjModel;
      class GCursorMeter*     m_pObjHealth;
      class GCursorMeter*     m_pObjResources;

      //! Labels
      GUI::GLabel*  m_pObjHealthTxt;
      GUI::GLabel*  m_pObjResourcesTxt;
      GUI::GLabel*  m_pObjIncomeTxt;
      GUI::GLabel*  m_pObjExpensesTxt;
      GUI::GLabel*  m_pObjBalanceTxt;
      GUI::GLabel*  m_pObjAvailableMoneyTxt;

      GUI::GLabel*  m_pObjIncomeLbl;
      GUI::GLabel*  m_pObjExpensesLbl;
      GUI::GLabel*  m_pObjBalanceLbl;
      GUI::GLabel*  m_pObjAvailableMoneyLbl;

      //! Buttons 
      GUI::GButton* m_pObjMoreHealth;
      GUI::GButton* m_pObjMoreBudget;
      GUI::GButton* m_pObjMoreResources;
      GUI::GButton* m_pObjBtnTrade;

      // Picture boxes
      GUI::GPictureBox* m_pObjPlus1;
      GUI::GPictureBox* m_pObjPlus2;
      GUI::GPictureBox* m_pObjPlus3;
      GUI::GPictureBox* m_pObjPlus4;
      
      // window data
      GDataEconomicWindow* m_pData;

   private:
   };
};

#endif //_CSP2_EconomicWindow_H_
