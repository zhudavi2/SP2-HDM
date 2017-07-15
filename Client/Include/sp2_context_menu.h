/**************************************************************
*
* SP2_context_menu.h
*
* Description
* ===========
*  This file contains the code needed to display and
*  manage the SP2::GContextMenu
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_CONTEXT_MENU_H_
#define _GOLEM_SP2_CONTEXT_MENU_H_

namespace SP2
{
   namespace EContextMenuItemType { enum Enum
   {
      Normal,
      Separator,
      Popup
   };}

   class GContextMenuItem;

   class GContextMenu : public GFrame
   {
   friend class GContextMenuItem;
   public:
      //! Constructor
      GContextMenu();
      //! Destructor
      ~GContextMenu();

      //! Create handler
      bool OnCreate();
      bool OnDestroy();

      //! On Show handler
      void OnShow();
      //! Create a submenu to be placed in an expandable item of this menu
      GContextMenu* CreateSubMenu(const GString& in_sName,GUI::GBaseObject* in_pParent);

      void ClearList();
      
      INT32 AddSeparator() {return AddItem("", EContextMenuItemType::Separator, NULL);}
      INT32 AddItem(const GString & in_sItemText, enum EContextMenuItemType::Enum in_Type = EContextMenuItemType::Normal, GContextMenu* in_pContextMenu = NULL);
     
      EEventStatus::Enum OnKeyDown    (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseClick (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseOver  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseOut   (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);      
      EEventStatus::Enum OnMouseMove  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Handled;}
      EEventStatus::Enum OnMouseDown  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      EEventStatus::Enum OnMouseWheel (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Handled;}
      EEventStatus::Enum OnMouseUp    (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Handled;}
      
      bool OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);

      //! Register a function to be called when an item of the menu is selected.
      void RegisterHandler(void * in_pInstance, CALLBACK_HANDLER_void_pvoid in_pFunc);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
      void Kill();
      bool Update();

   private:
      static UINT32                   m_iItemIndex;

      vector<SP2::GContextMenuItem*>  m_vpItems;

      GContextMenu*                   m_pParentMenu;
      GContextMenuItem*               m_pHighlightedMenuItem;

      void*                           m_pHandlerInstance;
      CALLBACK_HANDLER_void_pvoid     m_pHandlerFunc;

      UINT8                           m_iActiveItem;
   };
};

#endif //_GOLEM_SP2_CONTEXT_MENU_H_
