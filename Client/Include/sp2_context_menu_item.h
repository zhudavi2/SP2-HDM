/**************************************************************
*
* GUI_context_menu.h
*
* Description
* ===========
*  This file contains the code needed to display and
*  manage the GUI::GContextMenu
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GUI_CONTEXT_MENU_ITEM_H_
#define _GOLEM_GUI_CONTEXT_MENU_ITEM_H_

#include "../include/sp2_context_menu.h"

namespace SP2
{
   class GContextMenuItem : public GFrame
   {
   public:
      //! Constructor
      GContextMenuItem();
      //! Destructor
      ~GContextMenuItem();

      //! Create handler
      bool OnCreate();
      bool OnDestroy();
      
      void OnHide();

      EEventStatus::Enum OnKeyDown    (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseClick (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseOver  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseOut   (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}

      EEventStatus::Enum OnMouseMove  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseDown  (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseWheel (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}
      EEventStatus::Enum OnMouseUp    (const GEventData & in_EventData, GUI::GBaseObject * in_pCaller) {return EEventStatus::Callback;}

      void Set(UINT32 in_ItemIndex, const GString & in_sText, EContextMenuItemType::Enum in_Type = EContextMenuItemType::Normal, GContextMenu* in_pSubMenu = NULL);

      const GString &            Text()                        {return m_pTextBox->Text();}
      EContextMenuItemType::Enum ItemType()                    {return m_ItemType;}
      UINT32                     ItemIndex()                   {return m_ItemIndex;}
      class GContextMenu*        ChildMenu()                   {return m_pChildMenu;}      
      void                       Text(const GString & in_Text) {m_pTextBox->Text(in_Text);}

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      class GContextMenu*        m_pChildMenu;
      GButton*                   m_pArrow;
      GLabel*                    m_pTextBox;      
      EContextMenuItemType::Enum m_ItemType;
      UINT32                     m_ItemIndex;
   };
};

#endif //_GOLEM_GUI_CONTEXT_MENU_ITEM_H_
