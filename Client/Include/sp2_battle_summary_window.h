/**************************************************************
*
* golem_battleabstractwindow.h
*
* Description
* ===========
*  Describes the interface of class GBattleAbstractWindow
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_BATTLEABSTRACTWINDOW_H_
#define _GOLEM_BATTLEABSTRACTWINDOW_H_



namespace SP2
{
   /*!
   * Battle Abstract window
   **/
   class GBattleSummaryWindow : public SP2::GBaseWindow
   {
   public:
      GBattleSummaryWindow();
      ~GBattleSummaryWindow();
      static GUI::GBaseObject* New();

      bool OnCreate();
      bool OnDestroy();
      void OnHide();
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      bool SetArenaInformation(const SP2::GArenaInfo& in_ArenaInformation);

      void PlayResultMusic(const SP2::GArenaInfo& in_ArenaInformation);

      //Side A = Defenders
      //Side B = Attackers 
      SDK::GUI::GListBox* m_pListBoxSideA;
      SDK::GUI::GListBox* m_pListBoxSideB;
      SDK::GUI::GLabel*   m_pLossesInfantrySideA;
      SDK::GUI::GLabel*   m_pLossesInfantrySideB;
      SDK::GUI::GLabel*   m_pLossesGroundSideA;
      SDK::GUI::GLabel*   m_pLossesGroundSideB;
      SDK::GUI::GLabel*   m_pLossesAirSideA;
      SDK::GUI::GLabel*   m_pLossesAirSideB;
      SDK::GUI::GLabel*   m_pLossesNavalSideA;
      SDK::GUI::GLabel*   m_pLossesNavalSideB;
      SDK::GUI::GLabel*   m_pStatusSideA;
      SDK::GUI::GLabel*   m_pStatusSideB;
      SDK::GUI::GLabel*   m_pBattleName;




      static const GString TypeName;
   protected:
   private:
   };

}
#endif //_GOLEM_BATTLEABSTRACTWINDOW_H_
