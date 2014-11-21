#include "golem_sdk_api.h"
/********************************************************

golem_gui_manager.h


Owner: Mathieu Tremblay

Overview:
GUI manager - Contains the list of GUIs, the active one
receives the user inputs and passes them to the active GUI

History:

- 16 octobre 2002
creation                mtremblay

- 14 novembre 2002
Register the mouse and kbd events so they are raised here

- 18 mars 2003
Premier draft d'implémentation		mtremblay

- 26 mars 2003
More complete implementation

- 12-13 mai 2003
Convert to follow the new Model of DIALOG XML File



*********************************************************/

#ifndef _GOLEM_GUI_MANAGER_
#define _GOLEM_GUI_MANAGER_

#define REGISTER_GUI_OBJECT(classname)                                     g_Joshua.GUIManager()->RegisterConstructorInternal(classname::TypeName, classname::New)
#define CREATE_GUI_OBJECT(classtypename, name, parent, visible, focusable) g_Joshua.GUIManager()->GetNewGUIObjectInternal(classtypename, name, parent, visible, focusable)
#define CREATE_GUI_OBJECT_VISIBLE(classname, parent)                       (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, classname::TypeName, parent, true, true)
#define CREATE_GUI_OBJECT_HIDDEN(classname, parent)                        (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, classname::TypeName, parent, false, true)
#define CREATE_GUI_OBJECT_STATIC(classname, parent, visible)               (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, classname::TypeName, parent, visible, false)
#define CREATE_GUI_OBJECT_NAMED_VISIBLE(classname, name, parent)           (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, name, parent, true, true)
#define CREATE_GUI_OBJECT_NAMED_HIDDEN(classname, name, parent)            (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, name, parent, false, true)
#define CREATE_GUI_OBJECT_NAMED_STATIC(classname, name, parent, visible)   (classname*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(classname::TypeName, name, parent, visible, false)

namespace SDK
{
   namespace GUI
   {

      // Pointer to member for event processing
      typedef EEventStatus::Enum (GBaseObject::*EventProc)(const GEventData &, GBaseObject *);

      /*!
      * GUI Object construction function prototype typedef
      *	Pointer to a function with the following prototype:
      *		GBaseObject*  function();
      */
      typedef GBaseObject* (*GUIOBJ_CONSTR_FUNC_PTR)();

      /*!
      * GUI Object constructor holder
      *	To hold the list of constructors that are associated with 
      *	a string.
      */
      typedef stdext::hash_map<GString, GUIOBJ_CONSTR_FUNC_PTR>  GDOConstructors;

      struct GHotkeyInfo
      {
         UINT32         ID;
         GKeyboardInfo  Key;
         GBaseObject *  ReceiverParent;
         GString        Description;
      };
      

      struct UniqueHotkey : public pair<GKeyboardInfo, GBaseObject *>
      {
         UniqueHotkey(GKeyboardInfo i, GBaseObject * o) : pair<GKeyboardInfo, GBaseObject *>(i, o) {}
         UniqueHotkey(const UniqueHotkey & rsh)         : pair<GKeyboardInfo, GBaseObject *>((const pair<GKeyboardInfo, GBaseObject *>) rsh) {}

         operator size_t() const
         {
            return (size_t)(*(INT32*)&first + *(INT32*)&second);
         };
      };

      namespace EDefaultMousePointer { enum Enum
      {
         ArrowHourglass = (int)(void*)IDC_APPSTARTING,
         Default        = (int)(void*)IDC_ARROW,
         Arrow          = (int)(void*)IDC_ARROW,
         Precision      = (int)(void*)IDC_CROSS,
         Hand           = (int)(void*)IDC_CROSS,
         ContextHelp    = (int)(void*)IDC_HELP,
         Text           = (int)(void*)IDC_IBEAM,         
         No             = (int)(void*)IDC_NO,
         SizeAll        = (int)(void*)IDC_SIZEALL,
         SizeNESW       = (int)(void*)IDC_SIZENESW,
         SizeNS         = (int)(void*)IDC_SIZENS,
         SizeNWSE       = (int)(void*)IDC_SIZENWSE,
         SizeWE         = (int)(void*)IDC_SIZEWE,
         UpArrow        = (int)(void*)IDC_UPARROW,
         Hourglass      = (int)(void*)IDC_WAIT
      };}

      typedef hash_map<UINT32, GHotkeyInfo> GHotkeys;
      typedef hash_map<UniqueHotkey, UINT32> GDirectHotkeys;
      typedef hash_map<GKeyboardInfo, UINT32> GHotkeysCount;
      typedef multimap<GBaseObject *, UINT32> GBoundHotkeys;      


      /*!
      *  == GUI Manager == GUI Manager == GUI Manager == GUI Manager == GUI Manager
      *
      *  Receives the user input and calls the correct function
      *  of the active gui.  The logic about the user inputs (if it is a 
      *  mouse click, a drag and drop, etc. is done here)
      *
      *	The Id of the GUI Manager is 0
      *
      **/
      class GOLEM_SDK_API GManager : public GEventHandler
      {         
         //List of objects that were inside the previous mouse move, cached for performance concerns
         hash_set<GBaseObject*> m_vWereInside; 

         //Method to get the list of all the gui objects at the given coordinates
         hash_set<GBaseObject*> ObjectsFromPos(const GVector2D<INT32> &) const;

         //Function to create a GUI Object
         GBaseObject* CreateObjectRecurse(GDODefClass_SPtr classDef, GBaseObject* ParentPtr);

         //! Translates a number of pixels (either relative (%) or not) into a real pixel value
         INT32 PropertyToPixels(const GString& in_sPropValue,const INT32 in_iFullValue,bool in_bRelative);
         
         void OnMouseMove(GEventData &, const GVector2D<INT32> &);

         GMutex m_Mutex;

         list<GBaseObject*>      m_ObjectsToClean;//!<List of objects that must be released (deleted) at the ReleaseAsyncObjects() function call
         GDODefinitions          m_Definitions;    //!<Dialog object definitions - resource side
         GDOConstructors         m_Constructors;   //!<List of GUI Objects constructors
         GGUIObjects             m_GUIObjects;     //!<List of GUI objects contained in the GUI Manager
         GList<GBaseObject*>     m_VisibleObjects; //!<List of visible gui objects 
         CGFX_Scene_Itf*   	   m_pScene;         //!<Scene
         CGFX_Text_2D_Itf*       m_pText;          //!<Temp to show mouse coordinates
         bool                    m_bDirty;         //!<Dirty flag
         UINT8                   m_iDragMode;
         GBaseObject*            m_pDraggedObject;
         bool                    m_bDoDoubleClick;
         GBaseObject*            m_pFocusedObject;
         GBaseObject*            m_pMouseDownTarget;
         GBaseObject*            m_pKeyDownTarget;
         GBaseObject*            m_pPreviousTarget;//!<Previous target
         GBaseObject*            m_pLastClickedObject;
         DWORD                   m_uLastMouseDownTime;
         DWORD                   m_uDoubleClickTime;         
         GVector2D<INT32>        m_OrigMousePosition;
         GVector2D<INT32>        m_MousePosition;
         GBaseObject*            m_pRootParent;         
         GKeyboardInfo           m_LastKeyDown;
         bool                    m_LastKeyWasHot;
         HCURSOR                 m_CurrentCursor;

         GBaseObject*            m_pTooltipObj;
         GLabel*                 m_pTooltipTextObj;

         class GHookItf *        m_GUIHooker;

         EventProc               m_EventProcTable[EKeyAndMouseEvents::Max];

      public:
         GManager();             
         ~GManager();

         void FireEvent(GEventData & in_EventData, EKeyAndMouseEvents::Enum, GBaseObject * in_Obj, const GVector2D<INT32> in_MousePosition, bool in_bRecurseSend = true);

         void InstallGUIHook(class GHookItf * in_Hooker) {m_GUIHooker = in_Hooker;}
         void UninstallGUIHook() {m_GUIHooker = NULL;}

         //! Find the target of the mouse events
         GBaseObject* FindTargetObject(const GVector2D<INT32> &, const GBaseObject* = NULL) const;

         //! Cleans up the objects to be released list
         bool              ReleaseAsyncObjects();
         //! Adds an object to the clean up list so it must be released on the next cleanup func call
         void              ReleaseObjectAsync(GBaseObject* in_pObj);
         //! Initialize the GUI Manager	
         bool              Initialize();
         //! Shutdown the GUI Manager
         bool              Shutdown();

         //! Updates the GUI Manager renderer
         bool              Update();
         //! Tells the GUI manager that a GUI object was modified
         void              SetDirty();
         //! Read a resource file and assign its content as the GUIManager GUI Objects definition	
         bool              ReadDialogResource(GFile& in_ResourceFile);

         /*!
         * \brief    Register a constructor for a specific type of GUI Objects
         *           see GUI Manager.doc for details.
         *           Constructors must be registered for every type of object so the
         *           GUI Manager can create them.	
         **/
         bool              RegisterConstructorInternal(const GString& in_sObjectName, GUIOBJ_CONSTR_FUNC_PTR in_pFunc);

         //! Create an object of the given type and start to manage it
         //! The object is a Child of the ParentPtr object.
         GBaseObject*      GetNewGUIObjectInternal(const GString& in_sType, const GString& in_sName, GBaseObject* ParentPtr = NULL, bool in_bVisible = false, bool in_bFocusable = true);

         //! Get the GUI Definition of an object
         bool              Definition(const GString&  in_sObjectType, GDODefClass_SPtr&  out_Definition);
         //! Delete the given object and stop managing it
         bool              ReleaseObject(GBaseObject* in_pObject);

         CGFX_Scene_Itf*   Scene(){return m_pScene;}

         //! Sets the gui manager focus
         void              Focus(GBaseObject* obj);
         //! Gets the gui manager focus
         GBaseObject*      Focus();

         //GBaseObject*      Root();

         void BringToFront(GBaseObject *);

         //! Get a reference to the visible objects list
         GList<GBaseObject*> & VisibleObjects();

         LRESULT OnKeyEvent(UINT32, WPARAM, GWin32KeyEvent *);
         LRESULT OnMouseEvent(UINT32, WPARAM, const GVector2D<INT32> &);
         void    OnSetCursorEvent();
         
         HCURSOR MousePointer();
         bool    SetMousePointer(GFile & in_PointerFile);
         bool    SetMousePointer(EDefaultMousePointer::Enum);
         void    SetMousePosition(const GVector2D<INT32> &);

         //!Tell the GUI Manager that a Frame is currently being dragged         
         bool           Drag(GBaseObject* in_pObject, UINT8);         

         //Get current Mouse Position
         const GVector2D<INT32> & MousePosition() const {return m_MousePosition;}         
         void                     MouseDownTarget(GBaseObject* in_pObject);
         GBaseObject*             MouseDownTarget() const;
         
         void TooltipInitialize(GUI::GBaseObject* in_pWindow, GUI::GLabel* in_pLabel);
         void TooltipText(GString in_sText);
         bool TooltipExistant();
         void TooltipShow();
         void TooltipHide();
         GUI::GBaseObject* TooltipObj();

#ifdef GOLEM_DEBUG
         hash_set<GBaseObject*>  m_DebugGUIObjectList;
#endif

         class GOLEM_SDK_API GHotkeysController
         {
         private:
            friend GManager;
            GHotkeys       m_Hotkeys;
            GDirectHotkeys m_DirectHotkeys;
            GHotkeysCount  m_HotkeysCount;
            GBoundHotkeys  m_HotkeyedObjects;

            mutable GString m_Dummy;
            UINT32 m_MaxID;
            GHotkeysController() {m_MaxID = 0;}
            GHotkeysController(const GHotkeysController&);
            GHotkeysController & operator = (const GHotkeysController&);

         public:
            UINT32            Register(GKeyboardInfo Key, GBaseObject * = NULL);
            bool              Unregister(UINT32);
            GString &         Description(UINT32);
            const GString &   Description(UINT32) const;
            const GHotkeys &  List() const;
            //! unregister the requested key and return the object associated to it
            GBaseObject*      UnregisterKey(INT16 in_iKey);
            void              UnregisterHotkeysBoundTo(GBaseObject *);
         } Hotkeys;
      };//end class GManager
   };//end namespace GUI
}
#endif