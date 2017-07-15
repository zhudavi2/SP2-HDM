/**************************************************************
*
* golem_errorwindow.h
*
* Description
* ===========
*  Describes the interface of class GTestWindow
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TESTWINDOW_H_
#define _GOLEM_TESTWINDOW_H_


namespace SP2
{

   /*!
   * Error notification window
   **/
   class GTestWindow : GUI::GFrame
   {
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GTestWindow();
      ~GTestWindow();

      bool OnCreate();
   protected:
   private:
   };

};//end namespace SP2
#endif //_GOLEM_TESTWINDOW_H_
