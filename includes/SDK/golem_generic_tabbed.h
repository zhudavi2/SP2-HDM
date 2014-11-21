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

#ifndef _GTabbed_H_
#define _GTabbed_H_

/*!
* Represents a basic 2D Frame (which has a Model and a window associated)
**/ 
namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GTabbed : public GFrame
      {
      public:
         bool OnCreate();
         bool OnDestroy();

         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOver(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         void OnShow();

         vector<GToggle*>  m_vTabButtons;
         vector<GTabPage*> m_vTabPage;
         GFrame*           m_pTabFrame;

         void ActiveTab(INT16 in_iTab);
         INT16 ActiveTab();

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GTabbed();
         virtual ~GTabbed();

      private:

         INT16 m_iNbTabs;
         INT16 m_iActiveTab;
         INT16 m_iButtonHorPosition;
         INT16 m_iButtonWidth;     
         INT16 m_iButtonHeight;   
         INT16 m_iButtonDistance;
         INT16 m_iFrameOffsetTop;
      };
   }
}
#endif //_GTabbed_H_
