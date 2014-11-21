#include "golem_sdk_api.h"
/**************************************************************
*
* golem_combo_box.h
*
* Description
* ===========
*  Describes the interface of class GComboBox
*  (A.K.A The DROP DOWN CONTROL)
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COMBO_BOX_H_
#define _GOLEM_COMBO_BOX_H_

/*!
* Defines the GCombo Box class, which contains an editbox and a listbox
* to pick its controls
**/
namespace SDK
{
   namespace GUI
   {
      class GOLEM_SDK_API GComboBox : public GUIContainer
      {
      public:

         //! Gets the content of the actual combo box selection
         GString Selected_Content() const;

         GUI::GListBox::GRow* SelectedRow() const;

         //! Sets the actually selected content of the combo box
         void Selected_Content(const GString& in_sContent, bool in_bCallEvent = false);
         void Selected_Content(INT32 in_iIDRow, bool in_bCallEvent = false);

         //! Send the list box
         GUI::GListBox* GetListBox() const {return m_pListBox;}
         GUI::GEditBox* GetEditBox() const {return m_pEditBox;}

         //! Clear all the combo boxes choices and the current selection
         void Clear();

         //! Adds item to the combo box selection
         virtual GUI::GListBox::GRow* Add_Item(const GString& in_sItem, UINT32 in_iAdditionalID = 0);

         //! Update combo box
         bool Update();

         //! Lose Focus handler, will be called by the listbox when it loses focus
         bool OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);

         //! On Destroy handler
         bool OnDestroy();

         void Enabled(bool in_bEnabled);
         bool Enabled() const
         {
            return __super::Enabled();
         }

         void  DropSize(INT32 in_iSize);
         INT32 DropSize();

         UINT32 NbItems(){return (UINT32)m_pListBox->Get_Nb_Rows();}

         static const GString TypeName;

      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GComboBox();
         virtual ~GComboBox();

         //! List Box used in the combo box
         GListBox*  m_pListBox;
         //! Text Box used in the combo box
         GEditBox*  m_pEditBox;
         //! Button used in the combo box
         GButton*    m_pButton;

         //! Create handler function
         bool OnCreate();
         bool CreateCboObjects(bool in_bWithListBox = true);

         GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

         //! On mouse down handler
         GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseOver(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);

         //! Actual String Content
         GString m_sContent;
         INT32   m_iDropSize;

         void OnResize();

      private:
      };
   }
}
#endif //_GOLEM_COMBO_BOX_H_
