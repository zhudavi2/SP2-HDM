#include "golem_sdk_api.h"
/************************************************
* File: golem_generic_list_item.h
* Owner: Francois Durand
* Description: Defines the generic list item object
* History:
* ========
*	2003-11-04		Creation			Francois Durand
*				
*
*************************************************/

#ifndef _GListItem_H_
#define _GListItem_H_


namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GListItem : public GUI::GFrame
      {
      public:
         //! Create object handler
         bool OnCreate();
         //! Delete the listbox
         bool OnDestroy();

         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         GUI::EEventStatus::Enum OnMouseEnter(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseLeave(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GListItem();
         virtual ~GListItem();
      private:
      };
   }
}
#endif //_GListItem_H_
