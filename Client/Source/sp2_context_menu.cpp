/**************************************************************
*
* SP2_CONTEXT_MENU.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"
#include "../include/sp2_context_menu_item.h"


UINT32 GContextMenu::m_iItemIndex = 0;

const GString GContextMenu::TypeName(L"Context_Menu");

//! Selected Country Window creation function
GUI::GBaseObject* GContextMenu::New()
{
   return new GContextMenu();
}

/*!
* Constructor
**/
GContextMenu::GContextMenu() : GFrame()
{
   m_iActiveItem        = 0;
   m_pParentMenu        = NULL;
   m_pHandlerFunc       = NULL;
   m_pHandlerInstance   = NULL;
   m_pHighlightedMenuItem = NULL;
}

/*!
*  Destructor
**/
GContextMenu::~GContextMenu()
{
}


bool GContextMenu::OnCreate()
{
   __super::OnCreate();


   Update();
   return true;
}

/*!
* On Show handler, actually it contains a hack since it hides the left bar because
* of zorder problems.  It also calls the default OnShow handler
*
* @return true if show successful, false otherwise
**/
void GContextMenu::OnShow()
{
   __super::OnShow();

   for (vector<SP2::GContextMenuItem*>::iterator itr = m_vpItems.begin(); itr < m_vpItems.end(); itr++)
   {
      (*itr)->Width(Width() - 2 * CONTEXT_MENU_BORDER_SIZE);
      if ((*itr)->ItemType() != EContextMenuItemType::Popup)
      {
         (*itr)->Child("btnText")->Width((*itr)->Width());
      }
      else
      {
         (*itr)->Child("btnText")->Width((*itr)->Width() - (*itr)->Child("btnArrow")->Width());
         (*itr)->Child("btnArrow")->Left(Width() - (*itr)->Child("btnArrow")->Width() - 2 * CONTEXT_MENU_BORDER_SIZE);
      }
   }

   //If there is no parent menu, display the menu beside the mouse
   if(!m_pParentMenu)
   {
      INT32 l_iPosX = g_Joshua.GUIManager()->MousePosition().x;
      INT32 l_iPosY = g_Joshua.GUIManager()->MousePosition().y;

      GUI::GBaseObject* l_pParentPtr = Parent();
      while(l_pParentPtr)
      {
         l_iPosX = l_iPosX - l_pParentPtr->Left();
         l_iPosY = l_iPosY - l_pParentPtr->Top();
         l_pParentPtr = l_pParentPtr->Parent();
      }

      CGFX_Renderer_Init_Mode l_DisplayMode;
      g_Joshua.Renderer()->Get_Current_Mode(&l_DisplayMode);
      //If the object is too far, from the end of the screen, make sure it is displayed inside it
      {
         if(l_DisplayMode.m_Resolution.x < Width() + g_Joshua.GUIManager()->MousePosition().x)
            l_iPosX = l_iPosX - Width();

         if(l_DisplayMode.m_Resolution.y < Height() + g_Joshua.GUIManager()->MousePosition().y)
            l_iPosY = l_iPosY - Height();
      }

      Position((INT16)l_iPosX, (INT16)l_iPosY);      
   }
   else
   {
      CGFX_Renderer_Init_Mode l_DisplayMode;
      g_Joshua.Renderer()->Get_Current_Mode(&l_DisplayMode);
      //If the object is too far, from the end of the screen, make sure it is displayed inside it
      {
         //Menu would be displayed too far on the right, display the menu on the left of the parent menu
         if(l_DisplayMode.m_Resolution.x < Left() + Width())
         {
            Left(m_pParentMenu->Left()-Width());
         }

         if(l_DisplayMode.m_Resolution.y < Top() + Height())
            Top(l_DisplayMode.m_Resolution.y - Height());
      }
   }

   Focus();
}

void GContextMenu::ClearList()
{
   for (vector<SP2::GContextMenuItem*>::iterator itr = m_vpItems.begin(); itr < m_vpItems.end(); itr++)
      g_Joshua.GUIManager()->ReleaseObject((GUI::GBaseObject*)(*itr));

   m_vpItems.clear();   
}

bool GContextMenu::OnDestroy()
{   
   ClearList();
   return __super::OnDestroy();
}

/*!
Add an item to the context menu list. (Simply add another choice)
It also resize the context menu if the text doesn't fit in the previously
created frame.

@param : GString in_sButtonType -> Give the button type
@param : GString in_sButtonText -> Give the button text
@return : The index of the newly created item
*/
INT32 GContextMenu::AddItem(const GString & in_sItemText, EContextMenuItemType::Enum in_Type, GContextMenu* in_pContextMenu)
{
   m_iItemIndex++;

   SP2::GContextMenuItem* l_pItem = CREATE_GUI_OBJECT_NAMED_STATIC(GContextMenuItem, GContextMenuItem::TypeName + GString(m_iItemIndex), this, true);

   m_vpItems.push_back(l_pItem);   

   l_pItem->Owner(this);

   int l_PositionHeight = 0;

   for (vector<SP2::GContextMenuItem*>::iterator itr = m_vpItems.begin(); itr < m_vpItems.end() - 1; itr++)
      l_PositionHeight += (*itr)->Height() + 1;

   l_pItem->Position(CONTEXT_MENU_BORDER_SIZE, l_PositionHeight + CONTEXT_MENU_BORDER_SIZE - 1);
   l_pItem->Set(m_iItemIndex, (in_Type == EContextMenuItemType::Separator ? "" : in_sItemText), in_Type, in_pContextMenu);
   INT32 l_MinWidth = ((CGFX_Text_2D_Itf*) l_pItem->Child("btnText")->Model()->Get_Child(0)->Get_Primitive())->Width();

   if (in_Type == EContextMenuItemType::Separator)
      l_pItem->Height(1);

   if (in_Type == EContextMenuItemType::Popup)
      l_MinWidth += l_pItem->Child("btnArrow")->Width();

   l_PositionHeight += l_pItem->Height() + 1;

   Height(l_PositionHeight + 2 * (CONTEXT_MENU_BORDER_SIZE - 1));
   if (Width() < l_MinWidth + 10)
      Width(l_MinWidth + 10);
   Update();

   return m_iItemIndex;
}

GUI::EEventStatus::Enum GContextMenu::OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if (in_pCaller) 
   {
      GContextMenuItem * l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller);
      if (!l_pItem)
         l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller->Owner());

      if (l_pItem) 
      {
         if (l_pItem != m_pHighlightedMenuItem)
         {
            if (m_pHighlightedMenuItem)
            {
               if (m_pHighlightedMenuItem->ChildMenu() && m_pHighlightedMenuItem->ChildMenu()->Visible())
               {
                  this->Focus();
                  m_pHighlightedMenuItem->ChildMenu()->Hide();
                  
               }

               OnMouseOut(in_EventData, m_pHighlightedMenuItem);
            }

            if (l_pItem->ItemType() != EContextMenuItemType::Separator)
            {
               l_pItem->ColorNormal(L"RGBA: 55, 55, 55",L"153");
               m_pHighlightedMenuItem = l_pItem;

               if((l_pItem->ItemType() == EContextMenuItemType::Popup) && l_pItem->ChildMenu())
               {
                  //Set the position of the menu
                  GVector2D<INT32> l_Pos(l_pItem->RelativeToAbsolute(GVector2D<INT32>(l_pItem->Width() + CONTEXT_MENU_BORDER_SIZE, -2)));
                  //Position correctly in Y
                  l_pItem->ChildMenu()->Position((INT16)l_Pos.x, (INT16)l_Pos.y);
                  l_pItem->ChildMenu()->Show();
               }
            }
         }
      }
   }
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GContextMenu::OnMouseOut(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   if (in_pCaller)
   {
      GContextMenuItem * l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller);
      if (!l_pItem)
         l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller->Owner());

      if (l_pItem && !(l_pItem->ChildMenu() && l_pItem->ChildMenu()->Visible()))
      {
         if (l_pItem->ItemType() != EContextMenuItemType::Separator)
         {
            l_pItem->ColorNormal(L"RGBA: 20, 20, 20",L"179");
            if (m_pHighlightedMenuItem == l_pItem) 
               m_pHighlightedMenuItem = NULL;
         }
      }      
   }
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GContextMenu::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return GUI::EEventStatus::Callback;
}

GUI::EEventStatus::Enum GContextMenu::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(!in_pCaller)
      return GUI::EEventStatus::Handled;
   GContextMenuItem * l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller);
   if (!l_pItem)
      l_pItem = dynamic_cast<GContextMenuItem*>(in_pCaller->Owner());
   
   //Expandable menu
   if(l_pItem && (l_pItem->ItemType() == EContextMenuItemType::Normal))
   {
      //Notify that a command has been selected.
      GContextMenu* l_pMenu = this;
      while(l_pMenu->m_pParentMenu)
         l_pMenu = l_pMenu->m_pParentMenu;

      gassert(VALID_HANDLER(l_pMenu->m_pHandlerInstance, l_pMenu->m_pHandlerFunc), "There should be a registered handler for the context menu");
      //Call the handler
      if (VALID_HANDLER(l_pMenu->m_pHandlerInstance, l_pMenu->m_pHandlerFunc))
         CALL_HANDLER(l_pMenu->m_pHandlerInstance, l_pMenu->m_pHandlerFunc)(l_pItem);

      g_Joshua.GUIManager()->Focus(l_pMenu->Owner());
      l_pMenu->Kill();         
   }
   else if(l_pItem && (l_pItem->ItemType() == EContextMenuItemType::Popup) )
   { 
      if(l_pItem->ChildMenu() && l_pItem->ChildMenu()->Visible())
         l_pItem->ChildMenu()->Hide();
      else if(l_pItem->ChildMenu() && !l_pItem->ChildMenu()->Visible())
         l_pItem->ChildMenu()->Show();
   }

   return GUI::EEventStatus::Handled;
}

/*!
Handle every pressed key, we currently do an action on Down/Up arrow and return
check comments in the function for further details on each button action
*/
GUI::EEventStatus::Enum GContextMenu::OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Key())
   {

   //! Set the highlighted item on the next one, if the last come back to first one
   case VK_DOWN:
      {
         if(m_iActiveItem == m_vpItems.size())
            m_iActiveItem = 1;
         else
            m_iActiveItem++;

         return GUI::EEventStatus::Handled;
      }
      //! Set the highlighted item on the prev one, if the first come back to last one
   case VK_UP:
      {
         if(m_iActiveItem == 0 || m_iActiveItem == 1)
            m_iActiveItem = (UINT8)m_vpItems.size();
         else
            m_iActiveItem--;

         return GUI::EEventStatus::Handled;
      }
      //! Call a mouse down action on the active button
   case VK_RETURN:
      {
         if(m_iActiveItem != 0)
         {            
            GEventData l_EventData;
            l_EventData.Mouse.Actor.Bits.Left = true;
            l_EventData.Mouse.Position = g_Joshua.GUIManager()->MousePosition();
            m_vpItems[m_iActiveItem - 1]->OnMouseDown(l_EventData, m_vpItems[m_iActiveItem - 1]);
            return GUI::EEventStatus::Callback;
         }
         break;
      }
   }
   return GUI::EEventStatus::Callback;
}

/*!
On lose focus we want to kill the context menu so it disappear when you click
another item.
*/
bool GContextMenu::OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller)
{
   //If we lose the focus to another context menu object, dont kill the menu.

   if (  (in_pFocusTo->Type() !=   GContextMenu::TypeName ) 
      && (in_pFocusTo->Type() !=   GContextMenuItem::TypeName ) 
      && (!in_pFocusTo->Parent() || (in_pFocusTo->Parent()->Type() != GContextMenuItem::TypeName)))
      Kill(); //Release the top most menu

   return true;
}

/*!
* Create a submenu to be placed in an expandable item of this menu
**/
GContextMenu* GContextMenu::CreateSubMenu(const GString& in_sName,GUI::GBaseObject* in_pParent)
{
   GContextMenu* l_pSubMenu = CREATE_GUI_OBJECT_NAMED_HIDDEN(GContextMenu, in_sName, in_pParent);

   l_pSubMenu->Owner(this);
   l_pSubMenu->m_pParentMenu = this;
   return l_pSubMenu;
}

/*!
* Kills the top most context menu (so it will kill all the menu items
* and the submenus)
**/
void GContextMenu::Kill()
{
   g_ClientDDL.ContextMenuKill();
}

/*!
* Register a function to be called when an item of the menu is selected.
* @param in_pInstance: Instance of the handler
* @param in_pFunc: Function pointer
**/
void GContextMenu::RegisterHandler(void* in_pInstance,
                                   CALLBACK_HANDLER_void_pvoid in_pFunc)
{
   //Function and instance should be specified
   assert(in_pInstance);
   assert(in_pFunc);
   m_pHandlerFunc = in_pFunc;
   m_pHandlerInstance = in_pInstance;
}



/*!
* Updates the context menu so it is displayed correctly 
*/
bool GContextMenu::Update()
{
   //If the menu dont have enough place, (too much on the left, too much on the bottom etc.)
   //Position it so it can be all viewed.
   INT32 l_iMaxWidth  = 0;
   INT32 l_iMaxHeight = 0;
   if(Parent())
   {
      l_iMaxWidth = Parent()->Width();
      l_iMaxHeight = Parent()->Height();
   }
   else
   {
      //If no parent, means the parent is the screen itself, so get the screen resolution
      CGFX_Renderer_Init_Mode l_Mode;
      g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
      l_iMaxWidth  = (INT16)l_Mode.m_Resolution.x;
      l_iMaxHeight = (INT16)l_Mode.m_Resolution.y;
   }

   //Check if it fits in width
   if(Left() + Width() >= l_iMaxWidth)
   {
      Left(l_iMaxWidth - Width());
   }
   if(Top() + Height() >= l_iMaxHeight)
   {
      Top(l_iMaxHeight - Height());
   }

   return true;
}