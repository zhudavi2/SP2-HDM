#include "golem_sdk_api.h"
/************************************************
* File:  golem_generic_frame.h
* Owner: Mathieu Tremblay
* Description:
*	  Describes the generic edit box
*
* History:
* ========
*	15/04/2003			Creation - le jour où jean charest est devenu premier
*						ministre du québec.
*************************************************/

#ifndef _GEditBox_H_
#define _GEditBox_H_

typedef UINT32 CharIndex;

/*!
* Represents a basic Edit box (accepts input)
**/ 
namespace SDK
{
   namespace GUI
   {
      namespace ECharacterCase { enum ENUM {
         Normal = 0,
         Lower,
         Upper
      };}

      namespace ECaretMoves { enum ENUM {
         Next = 0,
         Prev,
         Begin,
         End
      };}

      const UINT32 c_iOnEditBoxEditBegin = 952394123;
      const UINT32 c_iOnEditBoxEditEnd   = 952394124;


      class GOLEM_SDK_API GEditBox : public GUI::GLabel
      {
      public:
         static void Clean();

         bool OnCreate();
         bool OnDestroy();
         GUI::EEventStatus::Enum OnChar(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnHotkey (const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         bool OnGotFocus(const SDK::GUI::GBaseObject * in_pFocusFrom, SDK::GUI::GBaseObject * Caller = 0);
         bool OnLoseFocus(const SDK::GUI::GBaseObject * in_pFocusTo, SDK::GUI::GBaseObject * Caller = 0);

         bool Font(const GString & in_sFontName, INT32 in_iSize);

         virtual bool Text(const GString &);
         virtual const GString& Text() const;

         void SetPasswordChar(const wchar_t& in_wchar);
		 void SetMaxLength(INT32 inMaxLength);

         static const GString TypeName;

      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GEditBox();
         virtual ~GEditBox();

         // hotkeys for editbox
         void RegisterHotKey();
         void UnregisterHotKey();

      private:
         //! set our glabel text 
         void SetLabelText(const GString & in_sText);
         //! display the caret in the editbox 
         void RefreshCaretPostion();
         //! calculate the position of the caret in the editbox hence m_iPosInString
         void CalculateCaretPosition();
         //! move the caret in the editbox
         void MoveCaret(ECaretMoves::ENUM in_iMove);
         INT32 GetCharWidthAt(CharIndex in_iPosition) const;

         GString              m_sRealText;      // Text, used when PasswordChar is set
         ECharacterCase::ENUM m_CharCase;       // Force casng to lower or UPPER
         CharIndex            m_iMaxLength;     // Maximum length of the text in edit control
         wchar_t              m_cPasswordChar;  // Unique char used to display password fields
         int                  m_iPasswordCharWidth; 
         CharIndex            m_iMinSel;        // First Selected Character
         CharIndex            m_iMaxSel;        // First Unselected Character
         CharIndex            m_iCaret;         // Caret Position
         CharIndex            m_iDisplayStart;  // Index of leftmost character displayed
         CharIndex            m_iPosInString;   // Index of caret Character
         bool                 m_bFixedWidthFont;
         bool                 m_bReadOnly;


         static CGFX_Track_Itf         *m_CaretTrack;
         static GFX::GWindow           *m_Caret;
         static CGFX_Transform_2D_Itf  *m_CaretTransform;
         static CGFX_Model_Itf         *m_CaretModel;
         GBaseObject                   *m_pRegHotKeyObject;

         HDC   m_pHDC;
         HFONT m_pFont;
         INT32 m_iCaretHeight;
         INT32 m_iCaretPositionX;
         INT32 m_iLineHeight;
         bool  m_bTrueTypeFont;
      };
   }
}

#endif //_GEditBox_H_
