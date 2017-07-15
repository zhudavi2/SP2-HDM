/**************************************************************
*
* sp2_covert_actions_cell_training.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GCovertActionsCellTrainingWindow_H_
#define _CSP2_GCovertActionsCellTrainingWindow_H_

namespace SP2
{

   class GCovertActionsCellTrainingWindow : public GBaseWindow
   {
   public:
	   GCovertActionsCellTrainingWindow();
	   ~GCovertActionsCellTrainingWindow();

	   bool OnCreate();
      void OnShow();

      void Update(UINT32 in_iID, GString in_sName, SP2::ECovertActionCellTraining::Enum in_Level);
      void CleanUp();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GLabel* m_pObjNameLbl;        
      GUI::GLabel* m_pObjTrainingLbl;    
      GUI::GLabel* m_pObjSetupCostLbl;   
      GUI::GLabel* m_pObjUpkeepLbl;      
      GUI::GLabel* m_pObjUpdateTimeLbl;  

      GUI::GButton* m_pObjConfirmBtn;     
      GUI::GButton* m_pObjCancelBtn;

   private:
      void CalculateCost();
      vector<ECovertActionCellTraining::Enum> m_vTrain;
      vector<UINT32> m_viID;
   };
};

#endif //_CSP2_GCovertActionsCellTrainingWindow_H_
