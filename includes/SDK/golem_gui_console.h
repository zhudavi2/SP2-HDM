#include "golem_sdk_api.h"
/**************************************************************
*
* golem_gui_console.h
*
* Description
* ===========
*  Describes the interface of class GConsole
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_GUI_CONSOLE_H_
#define _GOLEM_GUI_CONSOLE_H_

class CGFX_Console_Itf;
class CGFX_Track_Itf;

namespace SDK
{
   namespace GUI
   {

      /*!
      * GUI console
      *    This console can take the place of the current
      *    engine console.
      **/
      class GOLEM_SDK_API GConsole : public GFrame
      {
      public:

         //! Replace the current engine console
         bool SetAsEngineConsole();

         //! Sets the text color to use
         bool SetTextColor(GColorRGBInt in_Color);

         bool OnCreate();
         bool OnDestroy();
         GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnChar(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         static const GString TypeName;

      protected:
         GConsole();
         ~GConsole();

      private:
         friend class GManager;
         //! GUI console construction function
         static GUI::GBaseObject* New();

         CGFX_Console_Itf*       m_Console;
         CGFX_Track_Itf*         m_Track;
         Hector::GWin32UIConsole*    m_LastConsole;
      };

   };
}
#endif //_GOLEM_GUI_CONSOLE_H_
