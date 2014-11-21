/**************************************************************
*
* golem_gui_hook_itf.h
*
* Description
* ===========
*  Describes the interface of class GHookItf
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_HOOKITF_H_
#define _GOLEM_HOOKITF_H_

namespace SDK
{
   namespace GUI
   {
      class GHookItf
      {
         friend class GUI::GManager;
      protected:      
         virtual bool CanFireEvent(EKeyAndMouseEvents::Enum, GUI::GBaseObject *& in_pTarget, GEventData &) = 0;
         virtual bool CanFireHotkey(const GEventData &, GBaseObject*) = 0;
         virtual bool CanGiveFocusTo(GBaseObject* in_pObject) = 0;
         virtual bool CanBringToFront(GBaseObject* in_pObject) = 0;
      };

   }
}
#endif //_GOLEM_HOOKITF_H_
