#include "golem_sdk_api.h"
/************************************************
* File:   golem_generic_scroll_bar.h
* Owner:  Mathieu Tremblay, Nicolas Hatier
* Description:
*		Defines the Generic Scrollbar and the interface
*		used to define a scrollable object
* History:
* ========
*
*************************************************/

#ifndef _GScroll_Bar_H_
#define _GScroll_Bar_H_

/*!
* Class definition for the Generic Scrollbar
* Events OnScrollVertical or OnScrollHorizontal are sent to the
* owner of the bar
**/
namespace SDK
{
   namespace GUI
   {
      namespace EScrollType { enum Enum
      {
         Vertical,
         Horizontal
      };}

      class GOLEM_SDK_API GScroll_Bar : public GFrame
      {
      public:

         //!  Initializes the Scroll Bar object
         bool Initialize (
            GUI::GBaseObject* in_owner,             
            INT32             in_Minimum,
            INT32             in_Maximum,
            INT32             in_SmallChange,
            INT32             in_LargeChange,
            INT32             in_Value = 0
            );

         void LargeChange(INT32);
         void SmallChange(INT32);
         void Maximum(INT32);
         void Minimum(INT32);
         void Value(INT32);

         INT32 LargeChange();
         INT32 SmallChange();
         INT32 Maximum();
         INT32 Minimum();
         INT32 Value();

         GString ButtonLTName(){return m_sButtonLTName;}
         GString ButtonRBName(){return m_sButtonRBName;}
         GString ButtonTrackerName(){return m_sButtonTrackerName;}
         GString ButtonCursorName(){return m_sButtonCursorName;}

         void ButtonLTName(const GString & in_sTypeName){m_sButtonLTName = in_sTypeName;}
         void ButtonRBName(const GString & in_sTypeName){m_sButtonRBName = in_sTypeName;}
         void ButtonTrackerName(const GString & in_sTypeName){m_sButtonTrackerName = in_sTypeName;}
         void ButtonCursorName(const GString & in_sTypeName){m_sButtonCursorName = in_sTypeName;}

         static VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

         //Must put public again because declara_param leaves everything as private after
      public:

         //! Updates the scrollbar to reflect changes on the Parent (does a callback)
         bool Update();

         //! On Create function
         bool OnCreate();
         //! On Show handler
         void OnShow();

         //! Click function - handles clicks on the Scrollbars and its buttons         
         void OnDrag(const GUI::GEventData &, const GVector2D<INT32> &, const GVector2D<INT32> &);
         EEventStatus::Enum OnMouseClick(const GUI::GEventData &, GUI::GBaseObject *);
         EEventStatus::Enum OnMouseDown(const GUI::GEventData &, GUI::GBaseObject *);
         EEventStatus::Enum OnMouseUp(const GUI::GEventData &, GUI::GBaseObject *);
         EEventStatus::Enum OnMouseWheel(const GUI::GEventData &, GUI::GBaseObject *);

      protected:
         //! Default Constructor
         GScroll_Bar(EScrollType::Enum in_Type);
         virtual ~GScroll_Bar();

         GString m_sButtonLTName;
         GString m_sButtonRBName;
         GString m_sButtonTrackerName;
         GString m_sButtonCursorName;

         EScrollType::Enum m_ScrollType;

         //Drag variables, to drag the scroll
         INT32        m_iOrigValue;

         //Gui Objects   
         GUI::GButton*	m_pButtonLT;  // Cursor Object
         GUI::GButton*  m_pButtonRB;  // Down/Right button
         GUI::GButton*  m_pButtonCR;  // Cursor Object
         GUI::GButton*  m_pButtonTZ;  // Tracker zone button

         INT32 m_iLargeChange;
         INT32 m_iSmallChange;
         INT32 m_iMaximum;
         INT32 m_iMinimum;
         INT32 m_iValue;

         INT32 Get_Value_From_XY(INT32 x, INT32 y);

         static struct GScrollTimerParams
         {
            GScrollTimerParams() : m_pScrollbar(NULL), m_pButton(NULL) {}

            GScroll_Bar*         m_pScrollbar;
            GUI::GBaseObject*    m_pButton;
            INT32                m_TimerID;
            GEventData           m_EventData;
            INT32                m_iTimerCounter;

         } ScrollTimerParams;         

         bool  m_bEmbedded;   
         bool	m_bInitialized;
      };

      class GVScrollBar : public GScroll_Bar
      {
      public:
         static const GString TypeName;
      protected:
         GVScrollBar();
         virtual ~GVScrollBar() {}
         friend class GUI::GManager;
         static GUI::GBaseObject* New();
      };

      class GHScrollBar : public GScroll_Bar
      {
      public:
         static const GString TypeName;
      protected:
         GHScrollBar();
         friend class GUI::GManager;
         static GUI::GBaseObject* New();
      };
   };
}
#endif //_GScroll_Bar_H_
