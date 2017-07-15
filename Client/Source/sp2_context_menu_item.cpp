/**************************************************************
*
* sp2_context_menu_item.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_context_menu_item.h"

#include "../include/sp2_context_menu.h"

const GString GContextMenuItem::TypeName(L"frmContextMenuItem");

//! Selected Country Window creation function
GUI::GBaseObject* GContextMenuItem::New()
{
   return new GContextMenuItem();
}


//! Constructor
GContextMenuItem::GContextMenuItem()
{
   m_pChildMenu  = NULL;
   m_pArrow      = NULL;
   m_pTextBox    = NULL;
   m_ItemIndex   = 0;
   m_ItemType    = EContextMenuItemType::Normal;
}

//! Destructor
GContextMenuItem::~GContextMenuItem()
{
}

//! Create handler
bool GContextMenuItem::OnCreate()
{
   __super::OnCreate();

   m_pArrow   = (GButton*)this->Child(L"btnArrow");
   m_pTextBox = (GLabel*)this->Child(L"btnText");
   m_pArrow->Focusable(false);
   m_pTextBox->Focusable(false);

   return true;
}

bool GContextMenuItem::OnDestroy()
{
   __super::OnDestroy();
   if(m_pChildMenu)
      g_Joshua.GUIManager()->ReleaseObject(m_pChildMenu);

   return true;
}

void GContextMenuItem::OnHide()
{
   __super::OnHide();
   if(m_pChildMenu) 
      m_pChildMenu->Hide();
}

void GContextMenuItem::Set(UINT32 in_ItemIndex, const GString & in_sText, EContextMenuItemType::Enum in_Type, GContextMenu* in_pContextMenu)
{
   if(in_Type == EContextMenuItemType::Popup)
      m_pArrow->Show();
   else
      m_pArrow->Hide();

   m_pTextBox->Text(in_sText);
   m_ItemType = in_Type;
   m_pChildMenu = in_pContextMenu;
   m_ItemIndex = in_ItemIndex;
}
