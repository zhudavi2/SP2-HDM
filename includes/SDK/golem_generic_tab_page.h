#include "golem_sdk_api.h"
/************************************************
* File:  golem_generic_frame.h
* Owner: Mathieu Tremblay
* Description:
*	  Describes the generic frame to be used in the GUI
*
* History:
* ========
*	19/03/2003			Creation
*************************************************/

#ifndef _GTab_Page_H_
#define _GTab_Page_H_

/*!
* Represents a basic 2D Frame (which has a Model and a window associated)
**/ 
namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GTabPage : public GFrame
      {
      public:

         bool OnCreate();

         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOver(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GTabPage();
         virtual ~GTabPage();

      };
   }
}

#endif //_GTab_Page_H_
