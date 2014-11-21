/**************************************************************
*
* sp2_mail_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_OVERLAY_TEXT_H_
#define _CSP2_OVERLAY_TEXT_H_

namespace SP2
{

   class GTextOverlay : public GUI::GLabel
   {
   public:
	   GTextOverlay();
	   ~GTextOverlay();

      GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      void Update(const GString & in_sText, GUI::GLabel* in_pCaller);

      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
   };
};

#endif //_CSP2_OVERLAY_TEXT_H_
