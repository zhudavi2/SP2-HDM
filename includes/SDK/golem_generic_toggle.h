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

#ifndef _GToggle_H_
#define _GToggle_H_

/*!
* Represents a basic 2D Frame (which has a Model and a window associated)
**/ 

namespace SDK
{
   namespace GUI
   {
      class GToggle;

      typedef pair < INT32, GUI::GBaseObject* > GToggleMMSetKey;
      typedef multimap < GToggleMMSetKey , GToggle* > GToggleMMSet;
      typedef GToggleMMSet::value_type GToggleMMSetPair;


      class GOLEM_SDK_API GToggle : public GUI::GButton
      {
      public:
         bool OnCreate();
         bool OnDestroy();

         void Enabled(bool in_bEnabled);
         bool Enabled() const
         {
            return __super::Enabled();
         }

         GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);

         bool ToggleState();
         void ToggleState(bool in_bState);

         INT32 ToggleGroup();

         static GToggleMMSet m_StaticToggleSets;

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         bool Update();

         GToggle();
         virtual ~GToggle();

      private:
         bool  m_bToggleState;
         INT32 m_iToggleGroup;
      };
   }
}
#endif //_GToggle_H_
