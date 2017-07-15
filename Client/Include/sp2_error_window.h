/**************************************************************
*
* golem_errorwindow.h
*
* Description
* ===========
*  Describes the interface of class GErrorWindow
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_ERRORWINDOW_H_
#define _GOLEM_ERRORWINDOW_H_


namespace SP2
{

   /*!
   * Error notification window
   **/
   class GErrorWindow : GUI::GFrame
   {
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GErrorWindow();
      ~GErrorWindow();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);


      bool Set(GString in_sText);
   protected:
   private:
   };

};//end namespace SP2
#endif //_GOLEM_ERRORWINDOW_H_
