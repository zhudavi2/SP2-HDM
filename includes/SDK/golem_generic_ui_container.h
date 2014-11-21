#include "golem_sdk_api.h"
/************************************************
* File: golem_generic_ui_container.h
* Owner: Mathieu Tremblay
* Description:
*		Describes a generic UI container that only creates a Model
*
* History:
* ========
*		14 may 2003 :		Creation
*************************************************/

#ifndef _GUI_Container_H_
#define _GUI_Container_H_

/*!
* Defines the 2nd simplest GUI Object (the most simplest being the GUI::GBaseObject).
*	It creates a GFX Model, but no window is created
* Should be used to create really custom GUI objects.
**/
namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GUIContainer : public GUI::GBaseObject
      {
      public:
         bool OnCreate();

         /*!
         * GUIContainer construction function
         **/
         static const GString TypeName;

#if defined(_DEBUG)
         bool OnGotFocus(const GBaseObject *, GBaseObject *);
#endif

      protected:
         friend class GManager;
         static GBaseObject* New();

         GUIContainer();
         virtual ~GUIContainer();

      private:
      };
   }
}
#endif //_GUI_Container_H_
