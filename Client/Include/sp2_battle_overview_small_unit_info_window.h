/**************************************************************
*
* sp2_battle_overview_small_unit_info.h
*
* Description
* ===========
*  Describes the interface of class GUpdatableValue
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_GBattleOverviewSmallUnitInfoWindow_H_
#define _SP2_GBattleOverviewSmallUnitInfoWindow_H_

namespace SP2
{
   /*!
   * Defines the progress meter.
   **/
   class GBattleOverviewSmallUnitInfoWindow : public GUI::GFrame
   {
   public:
      static const GString TypeName;

      GBattleOverviewSmallUnitInfoWindow();
      ~GBattleOverviewSmallUnitInfoWindow();

      GUI::EEventStatus::Enum OnMouseEnter(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseLeave(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

      bool OnCreate();
      bool OnDestroy();
      void AddItem(UINT64 in_iQty, INT32 in_iCountryId);
      void Clear();
      bool Update();

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GListBox*  m_pUnitLst;
   
   private:
      vector<INT32> m_viCountryList;
      UINT64        m_iUnitQty;
      GUI::GLabel*  m_pObjTotalTxt;
   };
};

#endif //_SP2_GBattleOverviewSmallUnitInfoWindow_H_