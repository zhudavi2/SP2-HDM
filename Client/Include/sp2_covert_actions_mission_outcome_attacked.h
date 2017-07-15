/**************************************************************
*
* sp2_covert_actions_mission_outcome_attacked.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GCovertActionsMissionOutcomeAttackedWindow_H_
#define _CSP2_GCovertActionsMissionOutcomeAttackedWindow_H_

namespace SP2
{
   class GCovertActionsMissionOutcomeAttackedWindow : public GUI::GFrame
   {
   public:
	   GCovertActionsMissionOutcomeAttackedWindow();
	   ~GCovertActionsMissionOutcomeAttackedWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      void Update(SDataMissionOver in_Data);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GLabel* m_pObjTitleLbl;

      GUI::GLabel* m_pObjMissionTypeLbl;
      GUI::GLabel* m_pObjAttackerLbl;
      GUI::GLabel* m_pObjOutcomeLbl;
      GUI::GLabel* m_pObjEffectLbl;

      GUI::GLabel* m_pObjMissionTypeTxt;
      GUI::GLabel* m_pObjAttackerTxt;
      GUI::GLabel* m_pObjOutcomeTxt;
      GUI::GLabel* m_pObjEffectTxt;

      GUI::GButton* m_pObjExitBtn;
      GUI::GPictureBox* m_pObjFlagPic;

   private:
   };
};

#endif //_CSP2_GCovertActionsMissionOutcomeWindow_H_
