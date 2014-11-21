#include "golem_sdk_api.h"
/************************************************
* File: golem_generic_listbox.h
* Owner: Mathieu Tremblay
* Description: Defines the generic listbox object
*				It will be able to hold:
*						- Label
*						- Dropdown (comboboxes)
* History:
* ========
*	18 june 2003		Creation			Mtremblay
*				
*
*************************************************/

#ifndef _GListBox_H_
#define _GListBox_H_

/*!
* Class that is the Listbox object.  It is composed of a list of
* GUI Objects.  So If the user wants many columns, it must create a GUI Object
* that contains multiple items and assign it as the Item type.
*
*  When an item is selected into the List Box, the event
*    On_List_Item_Selected is raised on the Owner.  The Selected List items
*    can then be accessed via Get_Selected_Item method
**/
namespace SDK
{
   namespace EListBoxSelectionMode
   {
      enum Enum
      {
         None = 0,
         One,
         MultiSimple,
         MultiExtended,
      };
   };

   namespace GUI
   {
      class GOLEM_SDK_API GListBox : public GFrame
      {
         friend class GComboBox;
      public:

         class GRow
         {
         public:
            //!Row index
            INT32				      m_iId; 
            GUI::GBaseObject*    m_pObject;
            UINT32               m_iAdditionalId;
            bool                 m_bHidden;

            GUI::GBaseObject*  Object()
            {
               return m_pObject;
            }
         };


         //! Create object handler
         bool OnCreate();
         //! Delete the listbox
         bool OnDestroy();

         //! Show handler
         void OnShow();
         //! When a key is pressed on the listbox (handle them)
         GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         //! When a click is sent on the listbox
         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         
         GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);

         //! When the mouse wheel is used         
         GUI::EEventStatus::Enum OnMouseWheel(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         GUI::EEventStatus::Enum OnMouseOver(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseMove(const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         void OnResize();

         //! Clear all the list items
         void Clear();
         void ScrollTo(int in_Position);
         //! Get the number of rows
         INT32 Get_Nb_Rows();
         INT32 FirstRowShown();
         INT32 NbVisibleRows(){return m_iNbVisibleRows;}

         void ClearSelection();

         //! Updates the display of the listbox
         bool Update();

         //! Update hidden rows 
         void UpdateHiddenRow();

         //! Insert a row into the list
         GRow* Insert_New_Row(UINT32 in_iAdditionalID = 0);
         //! Get a row from the list
         GRow* Get_Row_At(INT32 position);
         GRow* Get_Any_Row_At(INT32 position);
         //! Remove row
         void RemoveRow(UINT32 in_iPosition);
         void RemoveRow(GRow* in_pRowToRemove);
         void SwapRows(UINT32 in_iFrom, UINT32 in_iTo);

         GString RowType();
         void    RowType(GString in_sType);

         //! Gets the selected item id (multiple selection not currently supported)
         const vector<INT32>& Get_Selected_Item();

         void Select_Items(vector<INT32> in_vItems, bool in_bCallEvent = false);

         //! is the row at the given position selected ?
         bool IsRowSelected(UINT32 in_iPosition);

         //! Overloaded handler of the set dimension function
         void Size(INT32 in_iWidth, INT32 in_iHeight);

         //! Retrieve and set the label text
         void    LabelText(const GString &in_sLabel){m_sLabel = in_sLabel;}
         GString LabelText() const                  {return m_sLabel;}

         //! retrieve a given row label
         GUI::GLabel* GetRowLabel(GUI::GListBox::GRow* in_pRow, bool in_bDynaCheck = false) const;

         //! Allow the user to retreive or set the list box selection mode
         bool SelectionMode(GString in_sMode);
         void SelectionMode(EListBoxSelectionMode::Enum in_eMode);
         EListBoxSelectionMode::Enum SelectionMode();

         INT32 RowHeight(){return m_iRowHeight;}
         void  RowHeight(INT32 in_iRowHeight);

         INT32 MouseOverWhichRow();


         bool ItemFontColorDisabled(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemFontColorDisabled(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemFontColorDisabled(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemFontColorDisabled();

         bool ItemFontColorNormal(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemFontColorNormal(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemFontColorNormal(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemFontColorNormal();

         bool ItemFontColorHighlight(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemFontColorHighlight(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemFontColorHighlight(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemFontColorHighlight();

         bool ItemFontColorSelected(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemFontColorSelected(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemFontColorSelected(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemFontColorSelected();

         bool ItemColorDisabled(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemColorDisabled(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemColorDisabled(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemColorDisabled();

         bool ItemColorNormal(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemColorNormal(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemColorNormal(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemColorNormal();

         bool ItemColorHighlight(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemColorHighlight(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemColorHighlight(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemColorHighlight();

         bool ItemColorSelected(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ItemColorSelected(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ItemColorSelected(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ItemColorSelected();

         bool ItemPictureHighlight(const GString & in_sFilename, bool in_bUpdate = true);
         void ItemPictureHighlight(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool ItemPictureNormal(const GString & in_sFilename, bool in_bUpdate = true);
         void ItemPictureNormal(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool ItemPictureDisabled(const GString & in_sFilename, bool in_bUpdate = true);
         void ItemPictureDisabled(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool ItemPictureSelected(const GString & in_sFilename, bool in_bUpdate = true);
         void ItemPictureSelected(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GListBox();
         virtual ~GListBox();

         GUI::EEventStatus::Enum OnCustomEvent(UINT32, const GUI::GEventData &, GUI::GBaseObject *);

         vector<GRow*>    m_ShownObjects;
         vector<GRow*>    m_DisplayableObjects;
         //! Height of each row (item)
         INT32				  m_iRowHeight;
         INT32            m_iRowWidth;
         INT32            m_iVisibleWidth;
         INT32            m_iVisibleHeight;
         //! Number of visible Rows (calculated)
         INT32				  m_iNbVisibleRows;
         //! ID of the first row (item) shown
         INT32				  m_iFirstRowShown;
         //! ID of the selected row
         vector <INT32>	  m_iSelectedRow;
         //! Rows
         vector<GRow*>	  m_vRows;
         //! Scrollbar
         GScroll_Bar*	  m_pVerticalScrollBar;
         GScroll_Bar*     m_pHorizontalScrollBar;
         //! Type descriptor for the row items
         GString          m_sRowType;
         //! Label text to get, get the label as a child if not empty
         GString          m_sLabel;

         //! Listbox selection mode
         EListBoxSelectionMode::Enum m_eSelectionMode;

         GUI::GBaseObject* m_pLastHighlited;

         GString          m_sShowHScroll;
         GString          m_sShowVScroll;

         CGFX_Texture_Itf* m_ItemPictureHighlight;
         CGFX_Texture_Itf* m_ItemPictureNormal;
         CGFX_Texture_Itf* m_ItemPictureSelected;
         CGFX_Texture_Itf* m_ItemPictureDisabled;

         GColorRGBInt m_ItemColorHighlight;
         GColorRGBInt m_ItemColorNormal;
         GColorRGBInt m_ItemColorSelected;
         GColorRGBInt m_ItemColorDisabled;

         GColorRGBInt m_ItemFontColorHighlight;
         GColorRGBInt m_ItemFontColorNormal;
         GColorRGBInt m_ItemFontColorSelected;
         GColorRGBInt m_ItemFontColorDisabled;

         bool m_bUseItemColorHighlight;
         bool m_bUseItemColorNormal;
         bool m_bUseItemColorSelected;
         bool m_bUseItemColorDisabled;

         bool m_bUseItemPictureHighlight;
         bool m_bUseItemPictureNormal;
         bool m_bUseItemPictureSelected;
         bool m_bUseItemPictureDisabled;

         bool m_bUseItemFontColorHighlight;
         bool m_bUseItemFontColorNormal;
         bool m_bUseItemFontColorSelected;
         bool m_bUseItemFontColorDisabled;

         bool             m_bCreated;

         //! Lose Focus Handler
         bool  OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);

      private:


      };
   }
}
#endif //_GListBox_H_
