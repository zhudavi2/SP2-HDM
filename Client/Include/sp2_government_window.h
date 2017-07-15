/**************************************************************
*
* sp2_communications_window.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _CSP2_Government_H_
#define _CSP2_Government_H_

namespace SP2
{
   class GGovernmentWindow : public GDialogObject
   {
   public:
	   GGovernmentWindow();
	   ~GGovernmentWindow();

	   bool on_create();

	   //! called when the button is released into our menu
	   GUI::EEventStatus on_mouse_down(INT16 in_iX,INT16 in_iY,INT16 in_iZ,INT16 in_iButtons,GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus on_click(INT16 in_iX, INT16 in_iY, INT16 in_iZ, INT16 in_iButtons, GUI::GBaseObject *in_pCaller);
      bool on_show();
      bool on_hide();

   protected:
   private:
   };

   /*!
   * MAIN MENU construction function
   **/
   GUI::GBaseObject* CreateGovernmentWindow();

};
#endif //_CSP2_Government_H_
