#include "golem_sdk_api.h"
/************************************************
* File:   golem_gui_object_itf.h
* Owner:  Mathieu Tremblay
* Description:
*		Defines the generic interface to a GUI object
*
* History:
* ========
*	18/03/03	Creation
*************************************************/

#ifndef _GBaseObject_H_
#define _GBaseObject_H_

namespace SDK
{
   class GJoshua;
   
   namespace GUI
   {
      class GListBox;
      class GComboBox;
      class GTreeNode;
      class GBaseObject;
      class GManager;
      
      typedef stdext::hash_map<GString, GUI::GBaseObject*>     GGUIObjects;
      typedef GList<GBaseObject*> GBaseObjectList;

      //! Return value for the mouse and keyboard events
      namespace EEventStatus { enum Enum
      {         
         Error,
         NotHandled,
         Handled,
         Callback
      };}

      struct GWin32KeyEvent
      {
         UINT32 RepeatCount      : 16;
         UINT32 ScanCode         : 8;
         UINT32 IsExtendedKey    : 1;
         UINT32 Reserved         : 4;
         UINT32 ContextCode      : 1;
         UINT32 PreviousKeyState : 1;
         UINT32 TransitionState  : 1;
      };

      union GMouseBits
      {
         UINT8 Field;
         struct
         {
            UINT8 Left:1; 
            UINT8 Right:1; 
            UINT8 Middle:1; 
            UINT8 Unused:5;
         } Bits;
      };

      struct GKeyboardInfo
      {
         INT16 Actor; // use VK_* Win32 constants
         INT16 Shift:1;
         INT16 Ctrl:1;
         INT16 Alt:1;
         INT16 Unused:13;

         GKeyboardInfo() : Actor(0), Shift(0), Ctrl(0), Alt(0), Unused(0) {}

         GKeyboardInfo(INT16 in_Actor, bool in_Shift, bool in_Ctrl, bool in_Alt)
            : Actor(in_Actor), Shift(in_Shift ? 1 : 0), Ctrl(in_Ctrl ? 1 : 0), Alt(in_Alt ? 1 : 0), Unused(0) {}

         // Used for hash_mapping
         operator size_t() const
         {
            return (size_t)*(INT32 *)this;
         }

         bool operator < (const GKeyboardInfo & in_Right) const
         {
            return *((INT32 *)this) < *((INT32 *)&in_Right);
         }
      };

      struct GMouseInfo
      {         
         INT32             WheelDelta;
         GVector2D<INT32>  Position;
         GMouseBits        Actor;
         GMouseBits        Down;
         bool              IsDoubleClick;
      };

      struct GAnchorPoints
      {
         GAnchorPoints()
         {
            m_bAnchorLeft     = false;
            m_bAnchorTop      = false;
            m_bAnchorRight    = false;
            m_bAnchorBottom   = false;
         }

         ~GAnchorPoints(){}

         GAnchorPoints(const GAnchorPoints & in_AnchorToCopy)
         {
            *this = in_AnchorToCopy;
         }

         GAnchorPoints(const GString & in_sAnchorStr)
         {
            m_bAnchorLeft   = in_sAnchorStr.find(L"Left")   != GString::npos;
            m_bAnchorRight  = in_sAnchorStr.find(L"Right")  != GString::npos;
            m_bAnchorTop    = in_sAnchorStr.find(L"Top")    != GString::npos;
            m_bAnchorBottom = in_sAnchorStr.find(L"Bottom") != GString::npos;
         }

         GAnchorPoints & operator=(const GAnchorPoints & in_AnchorToCopy)
         {
            m_bAnchorLeft     = in_AnchorToCopy.m_bAnchorLeft;
            m_bAnchorTop      = in_AnchorToCopy.m_bAnchorTop;
            m_bAnchorRight    = in_AnchorToCopy.m_bAnchorRight;
            m_bAnchorBottom   = in_AnchorToCopy.m_bAnchorBottom;

            return *this;
         }

         bool m_bAnchorLeft;
         bool m_bAnchorTop;
         bool m_bAnchorRight;
         bool m_bAnchorBottom;         
      };

      namespace EKeyAndMouseEvents { enum Enum
      {
         None,
         OnMouseClick,
         OnMouseDown,
         OnMouseEnter,
         OnMouseLeave,
         OnMouseMove,
         OnMouseOut,
         OnMouseOver,
         OnMouseUp,
         OnMouseWheel,         
         OnKeyDown,
         OnKeyUp,
         OnChar,
         Max
      };}

      struct GEventData
      {      
         GEventData() { memset(this, 0, sizeof(GEventData)); Mouse.Position.Set(-1, -1); Keyboard.Actor = VK_NONAME;}
         GMouseInfo    Mouse;
         GKeyboardInfo Keyboard;
         EKeyAndMouseEvents::Enum RaisedEvent;

         void * Data;            
         GBaseObject * InitialRecipient;

         INT32 & X()                {return Mouse.Position.x;}
         INT32 & Y()                {return Mouse.Position.y;}
         INT16 & Key()              {return Keyboard.Actor;}
         const INT32 & X()   const  {return Mouse.Position.x;}
         const INT32 & Y()   const  {return Mouse.Position.y;}
         const INT16 & Key() const  {return Keyboard.Actor;}
      };

      //! Custom Events IDs, UserEvent member can be used to create application specific custom events IDs
      namespace ECustomEvents
      {
         enum Enum
         {
            OnNodeSelected,
            OnNodeUnSelected,
            OnNodeDoubleClicked,
            OnNodeExpand,
            OnNodeCollapse,
            OnListItemSelected,
            OnListItemDoubleClicked,
            OnComboBoxSelectionEvent,
            OnComboBoxBeforeOpen,
            OnComboBoxAfterOpen,
            OnComboBoxBeforeClose,
            OnComboBoxAfterClose,
            OnScrollVertical,
            OnScrollHorizontal,    

            UserEvent = 0x1000,
         };
      }

      /*!
      * GUI object abstract class, represents
      *	a generic gui object that is to be used in the game
      *	Must be derived to be used.  The GUI Manager
      *	holds a list of GBaseObject to manage the GUI
      *
      *	Methods that the user want to customize must be overriden.
      *
      *	For many event handling functions, there is a parameter
      *	  in_pCaller, this is the identifier of the person that called the
      *	  function.  The receiving function can then access all the information
      *	  it needs to Process the event.
      *
      *	A in_pCaller parameter that is NULL means that the event comes
      *	from the CGUI_Manager
      *
      *	The parameter buttons is constructed as following:
      *			16 BITS
      *			First 8 bits:
      *				1 Bit per button pressed (button 1 => bit 0 at 1
      *			Last 8  bits:
      *				Code to specify which button has been pressed last
      *				to handle when there are multiple buttons pressed
      *
      **/
      class GOLEM_SDK_API GBaseObject 
      {
         //GUI Manager is a friend class - so it can easily create GUI Objects
         friend class GUI::GManager;

      private:
         //! List of visible childs Ordered by visibility (first one is topmost)
         GList<GBaseObject*>              m_VisibleChildren;
         GList<GBaseObject*>::GIterator   m_VisibleListNode;
         
         GString                          m_sType;       //! Object Type         
         GString                          m_sTooltipTextId; //! Tooltip text
         bool                             m_bFocused;    //! is the object focused         
         CGFX_Transform_2D_Itf*           m_pTransform;  //! Transform  
         GDODefClass_SPtr                 m_spDefinition;
         bool                             m_bCreationDone;

      protected:         
         bool                             OnUpdateLayout    ();
         
         
         bool                             m_bEnabled;
         
         GAnchorPoints                    m_Anchors;
         
         GGUIObjects                      m_Children;       //Child list of the object         
         GDOProperties                    m_Properties;     //Property list         
         CGFX_Model_Itf*                  m_pModel;         //Model of the object         
         GBaseObject*                     m_pParent;        //Pointer to the Parent         
         INT32	                           m_iId;            //Id         
         GString                          m_sName;
         GBaseObject*                     m_pOwner;         //!< Owner - object that will receive the events, defaults to the parent
         bool                             m_bFocusable;
                                          
         GBaseObject                                        ();                   
         virtual                          ~GBaseObject      ();


         void		                        VisibleInternal   (bool);

      public:

         virtual EEventStatus::Enum       OnChar            (const GEventData &, GBaseObject* = NULL);
         virtual bool                     OnCreate          ();
         virtual EEventStatus::Enum       OnCustomEvent     (UINT32, const GUI::GEventData &, GBaseObject* = NULL);
         virtual bool                     OnDestroy         ();
         virtual void                     OnDrag            (const GEventData &, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative);
         virtual void                     OnDragEnd         (UINT8 in_ButtonCondition);
         virtual bool                     OnGotFocus        (const GBaseObject* in_pFocusFrom, GBaseObject* Caller = NULL);
         virtual void                     OnHide            ();         
         virtual EEventStatus::Enum       OnHotkey          (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnKeyDown         (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnKeyUp           (const GEventData &, GBaseObject* = NULL);
         virtual bool                     OnLoseFocus       (const GBaseObject* in_pFocusTo,   GBaseObject* Caller = NULL);       
         virtual EEventStatus::Enum       OnMouseClick      (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseDown       (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseEnter      (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseLeave      (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseMove       (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseOut        (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseOver       (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseUp         (const GEventData &, GBaseObject* = NULL);
         virtual EEventStatus::Enum       OnMouseWheel      (const GEventData &, GBaseObject* = NULL);
         virtual void                     OnPropertyChange  (const GString &, const GString & in_OldValue, const GString & in_NewValue) {;}
         virtual void                     OnMove            () {};
         virtual void                     OnResize          () {};
         virtual void                     OnShow            ();

         /*! 
          * Fire a custom event. If you don't need the Mouse Position to be sent, 
          * set it to (-1,-1). Otherwise, set it to the ABOSLUTE value. A relative value
          * will be sent to targets.
         */
         bool                             FireCustomEvent   (UINT32, const GUI::GEventData &, bool in_RecurseOwner = true, GBaseObject * in_pCaller = NULL);         

         const GAnchorPoints &            Anchors           (){return m_Anchors;}
         void                             Anchors           (const GAnchorPoints & in_Anchors);

         void                             BringToFront      ();
         GBaseObject*                     Child             (const GString &) const;
         GString			                  ChildName         (INT32 in_iId);
         bool                             ContainsPoint     (const GVector2D<int> &) const;
         GDODefClass_SPtr                 Definition        ()                         {return m_spDefinition;}
         bool                             Drag              (UINT8 in_ButtonCondition = 0);         
         virtual bool                     Enabled           () const;
         virtual void                     Enabled           (bool in_bEnabled);
         void                             Focus             ();
         bool                             Focusable         ()                         {return m_bFocusable;}
         void                             Focusable         (bool in_bFocusable)       {m_bFocusable = in_bFocusable;}
         bool                             HasFocus          () const;
         INT32                            Height            () const;
         void                             Height            (INT32 in_iHeight);
         void		                        Hide              ();
         GString                          Id                () const                   {return GString(m_iId);}         
         bool                             IsTopMost         ();
         INT32                            Left              () const;
         void                             Left              (INT32 in_Left);
         const CGFX_Model_Itf*            Model             () const                   {return m_pModel;}
         CGFX_Model_Itf*                  Model             ()                         {return m_pModel;}
         GString                          Name              ()                         {return m_sName;}
         void                             Name              (const GString & in_sName) {m_sName = in_sName;}
         UINT32                           NbVisibleObjects  ();
         hash_set<GBaseObject*>           ObjectsFromPos    (const GVector2D<INT32> &);         
         const GBaseObject*               Owner             () const                   {return m_pOwner;}
         GBaseObject*                     Owner             ()                         {return m_pOwner;}    
         void                             Owner             (GBaseObject* in_pOwner)   {m_pOwner = in_pOwner;}    
         void			                     Parent            (GBaseObject*);
         const GBaseObject*               Parent            () const                   {return m_pParent;}   
         GBaseObject*                     Parent            ()                         {return m_pParent;}
         virtual void                     Position          (INT32, INT32);
         void                             Position          (const GVector2D<INT32> &);
         GVector2D<INT32>                 Position          () const;
         bool                             Property          (const GString &, const GString &);
         GString                          Property          (const GString &);
         vector<GUI::GBaseObject*>        RegisteredModalWindow;
         void                             SendToBack        ();
         virtual void                     Show              ();                  
         virtual void                     Size              (INT32, INT32);
         void                             Size              (const GVector2D<INT32>);
         GVector2D<INT32>                 Size              () const;
         GString                          TooltipText       ()                         {return m_sTooltipTextId;}
         void                             TooltipText       (const GString & in_sText ){m_sTooltipTextId = in_sText;}
         INT32                            Top               () const;
         void                             Top               (INT32 in_iPosY);
         const GString &                  Type              () const                   {return m_sType;};
         bool		                        Visible           () const                   {return m_VisibleListNode.IsValid();}                                          
         GBaseObject*                     VisibleObject     (UINT32 in_iIndex);
         INT32                            Width             () const;
         void                             Width             (INT32 in_iWidth);

         
         //! Translates relative coordinates to absolute coordinates
         GVector2D<INT32>                 RelativeToAbsolute(const GVector2D<INT32> &) const;
         //! Translates absolute coordinates to relative coordinates
         GVector2D<INT32>                 AbsoluteToRelative(const GVector2D<INT32> &) const;

         GGUIObjects & GetChildren() { return m_Children; }
      };


   };//end namespace gui
}
#endif //_GBaseObject_H_

