/**************************************************************
*
* sp2_combo_listboxless.h
*
* Description
* ===========
*  Describes the interface of class GComboListBoxLess
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_COMBO_LISTBOXLESS_H_
#define _SP2_COMBO_LISTBOXLESS_H_

#include "../../includes/SDK/golem_generic_combo_box.h"

/*!
* Defines the GComboListBoxLess class, which contains an editbox and
* a null ptr listbox to be associated runtime to pick its controls
**/
namespace SP2
{
   class GComboListBoxLess : public GUI::GComboBox
   {
      friend class GDataDisplayLayer;
   public:
      //! Create handler function
      bool OnCreate();
      //! On Destroy handler
      bool OnDestroy();

      //! Set the list box
      void SetListBox(GUI::GListBox* in_pObjListBox);

      //! Clear the combobox
      void Clear();

      //! useful static functions for listbox
      static GUI::GListBox* CreateListBox(const GString & in_sType, const GString &in_sName, const GString &in_sRowType);
      static void ReleaseListBox(GUI::GListBox*& io_pObjListBox);
      static const GString TypeName;

   protected:      
      static GUI::GBaseObject* New();

      GComboListBoxLess();
      ~GComboListBoxLess();      

      //! On mouse down handler
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      //! On lose focus handler
      bool OnLoseFocus(const SDK::GUI::GBaseObject *in_pFocusTo, SDK::GUI::GBaseObject *in_pCaller);

      //! last selected row id in listbox through this cbo
      INT32 m_iLastRowId;

   private:
   };
}
#endif //_SP2_COMBO_LISTBOXLESS_H_