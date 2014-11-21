/**************************************************************
*
* sp2_unit_attack_confirm_window.h
*
* Description
* ===========
*  Ask player if he wants to attack a neutral country
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __SP2_UNIT_ATTACK_WINDOW_H__
#define __SP2_UNIT_ATTACK_WINDOW_H__

namespace SP2
{

   class GUnitAttackConfirmWindow : public GUI::GFrame
   {
   public:
      GUnitAttackConfirmWindow();
      ~GUnitAttackConfirmWindow();

      bool OnCreate();

      EEventStatus::Enum OnMouseClick(const GEventData &, GBaseObject* = NULL);

      void TargetCountry(UINT32 in_iTargetCountry);
      UINT32 TargetCountry() const;
      void AddMoveEvent(SDK::GGameEventSPtr in_MoveEvent);

      GUI::GButton* ButtonYes();
      GUI::GButton* ButtonNo();


      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      GUI::GButton*                 m_pBtnYes;
      GUI::GButton*                 m_pBtnNo;
		GUI::GLabel*						m_pCountryName;

      vector<SDK::GGameEventSPtr>   m_vMoveEvents;
      UINT32                        m_iTargetCountry;
   };
};

#endif // #ifndef __SP2_UNIT_ATTACK_WINDOW_H__
