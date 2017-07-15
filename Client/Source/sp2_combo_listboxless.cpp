/************************************************
* File:  sp2_combo_listboxless.cpp
* Owner: David Shooner 
* Description:
*	  Describes the combolistboxless
*
*************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_combo_listboxless.h"

using namespace SP2;

const GString GComboListBoxLess::TypeName(L"cboListboxLess");

/*!
 * ComboListBoxLess Creation function
 **/
GUI::GBaseObject* GComboListBoxLess::New()
{
   return new GComboListBoxLess();
}

/*!
 * ComboListBoxLess ListBox Creation function
 **/
GUI::GListBox* GComboListBoxLess::CreateListBox(const GString & in_sType, const GString &in_sName, const GString &in_sRowType)
{
   GUI::GListBox* l_pObjLst = NULL;
   l_pObjLst = (GUI::GListBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(in_sType, in_sName, NULL, false, true); 
   gassert(l_pObjLst, "GComboListBoxLess::CreateListBox Unable to create listbox");
   l_pObjLst->RowType(in_sRowType);
   return l_pObjLst;
}

/*!
 * ComboListBoxLess ListBox Deletion function
 **/
void GComboListBoxLess::ReleaseListBox(GUI::GListBox*& io_pObjListBox)
{
   // release the listbox object
   if(io_pObjListBox)
   {
      io_pObjListBox->Clear();
      g_Joshua.GUIManager()->ReleaseObjectAsync(io_pObjListBox);
      io_pObjListBox = NULL;
   }
}

/*!
 * Constructor
 **/
GComboListBoxLess::GComboListBoxLess() : GUI::GComboBox()
{
   m_iLastRowId = 0;
}

/*!
 *  Destructor
 **/
GComboListBoxLess::~GComboListBoxLess()
{
}

/*!
 * Create handler function, called when a combolistboxless is created.
 *  Creates the Associated Label and the Associated button
 * @param in_pCaller : Pointer to the object that called the OnCreate function
 **/
bool GComboListBoxLess::OnCreate()
{
   // no call to super .. we want to override the OnCreate of the combobox
   GUIContainer::OnCreate();
   return __super::CreateCboObjects(false);
}

bool GComboListBoxLess::OnDestroy()
{
   Clear();
   // no call to super .. we want to override the OnDestroy of the combobox
   GUIContainer::OnDestroy();  
   return true;
}

/*!
 *  Clear all the combo boxes choices and the current selection
 **/
void GComboListBoxLess::Clear()
{
   m_pEditBox->Text("");
   if(m_pListBox)
   {
      m_pListBox->Owner(NULL);
      m_pListBox = NULL;
   }
}

/*!
 *  ListBox Setter
 **/
void GComboListBoxLess::SetListBox(GUI::GListBox* in_pObjListBox)
{
   // same listbox ?
   if(m_pListBox != in_pObjListBox) 
   {
      // set default propreties      
      m_pListBox   = in_pObjListBox;    
      m_iLastRowId = 0;
   }
}

/*!
 * On mouse down handler
 **/
GUI::EEventStatus::Enum GComboListBoxLess::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   // really have a list box ?
   if(m_pListBox) 
   {
      // new owner if need to set
      if(m_pListBox->Owner() != this) 
         m_pListBox->Owner(this);
      // good size
      m_pListBox->Size(Width(), m_iDropSize);

      // handler of the combobox
      __super::OnMouseDown(in_EventData, in_pCaller);

      //! set the good item if we can
      if((m_iLastRowId > 0) && (m_iLastRowId < m_pListBox->Get_Nb_Rows()))
         m_pListBox->ScrollTo(m_iLastRowId);
   }
   return GUI::EEventStatus::Handled;
}

/*!
 * On lose focus handler
 **/
bool GComboListBoxLess::OnLoseFocus(const SDK::GUI::GBaseObject *in_pFocusTo, SDK::GUI::GBaseObject *in_pCaller)
{   
   if(this == in_pFocusTo)
      if(m_pListBox && m_pListBox->Visible())
         m_iLastRowId = m_pListBox->FirstRowShown();

   if(this == in_pFocusTo)
      return true;
  
   return __super::OnLoseFocus(in_pFocusTo,in_pCaller);
}