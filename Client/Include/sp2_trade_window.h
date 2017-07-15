/**************************************************************
*
* sp2_trade_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_TradeWindow_H_
#define _CSP2_TradeWindow_H_

namespace SP2
{
   /*!
   * Trade system Window
   **/
   class GTradeWindow : public GBaseWindow
   {
   public:
      //! Window type name
      static const GString TypeName;

      //! constructors and destructors
	   GTradeWindow();
	   ~GTradeWindow();
       
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      //! Events Hanlders
	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      //! Mouse events handlers
      EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseClick(const GEventData &in_EventData, GBaseObject* in_pCaller);

      //! Custom events
      EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData &in_EventData, GBaseObject* in_pCaller);

      //! Frames
      GUI::GFrame*  m_pObjInnerFrm;

   private:      
   };
};

#endif //_CSP2_TradeWindow_H_
