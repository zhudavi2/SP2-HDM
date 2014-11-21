/**************************************************************
*
* sp2_unit_movement_denial_window.h
*
* Description
* ===========
*  Inform the player a movement was impossible
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __SP2_UNIT_MOVE_DENIAL_WINDOW_H__
#define __SP2_UNIT_MOVE_DENIAL_WINDOW_H__

namespace SP2
{

   class GUnitMovementDenialWindow : public GUI::GFrame
   {
   public:
      GUnitMovementDenialWindow();
      ~GUnitMovementDenialWindow();

      bool OnCreate();

      EEventStatus::Enum OnMouseClick(const GEventData &, GBaseObject* = NULL);

      void ID(UINT32 in_iNewID);
      UINT32 ID() const;

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      GUI::GButton*                 m_pBtnClose;
      GUI::GListBox*                m_pLstDenials;

      vector<SDK::GGameEventSPtr>   m_vMoveEvents;
      UINT32                        m_iID;
      UINT32                        m_iOffset;

      static UINT32                 m_iNextOffset;
   };
}

#endif // #ifndef __SP2_UNIT_MOVE_DENIAL_WINDOW_H__
