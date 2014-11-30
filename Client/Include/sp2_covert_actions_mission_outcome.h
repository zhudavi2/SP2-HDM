/**************************************************************
*
* sp2_covert_actions_mission_outcome.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GCovertActionsMissionOutcomeWindow_H_
#define _CSP2_GCovertActionsMissionOutcomeWindow_H_

namespace SP2
{

   class GCovertActionsMissionOutcomeWindow : public GBaseWindow
   {
   public:
	   GCovertActionsMissionOutcomeWindow();
	   ~GCovertActionsMissionOutcomeWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      void Update(UINT32 in_iCellID, bool in_bSuccess, bool in_bCaptured, bool in_bFoundOut);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GLabel* m_pObjCellNameLbl;
      GUI::GLabel* m_pObjMissionTypeLbl;
      GUI::GLabel* m_pObjTargetSectorLbl;
      GUI::GLabel* m_pObjComplexityLbl;
      GUI::GLabel* m_pObjResourceUnitLbl;
      GUI::GLabel* m_pObjTargetCountryLbl;
      GUI::GLabel* m_pObjFramedCountryLbl;

      GUI::GLabel* m_pObjResultLbl;
      GUI::GLabel* m_pObjDetailedResultsLbl;

   private:
   };
};

#endif //_CSP2_GCovertActionsMissionOutcomeWindow_H_
