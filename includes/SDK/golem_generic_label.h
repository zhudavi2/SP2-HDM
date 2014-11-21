#include "golem_sdk_api.h"
/************************************************
* File:  golem_generic_frame.h
* Owner: Mathieu Tremblay
* Description:
*	  Describes the generic frame to be used in the GUI
*
* History:
* ========
*	19/03/2003			Creation
*************************************************/

#ifndef _GLabel_H_
#define _GLabel_H_


class CGFX_Text_2D_Itf;

namespace SDK
{
   namespace GUI
   {
      /*!
      * Specifies how the text is aligned
      **/
      namespace ETextAlignment 
      { 
         enum Enum
         {      
            TopLeft,
            TopCenter,
            TopRight,
            MiddleLeft,
            MiddleCenter,
            MiddleRight,
            BottomLeft,
            BottomCenter,
            BottomRight,
            Unknown,
         };
      }
      
      /*!
      * Represents a simple Label (also called: label)
      **/
      class GOLEM_SDK_API GLabel : public GFrame
      {
      public:
         void Enabled(bool in_bEnabled);
         bool Enabled() const
         {
            return __super::Enabled();
         }

         //! Updates the Label look
         bool Update();

         //! Sets the text content of the Text box and the text style
         bool Text(const GString & text,const GString & font,INT32 fontSize,const GString & textColor,const GString & textAlpha,const GString & TextAlignment);
         //! Sets the text content of the Text box
         virtual bool Text(const GString & in_sText);

         //! Sets the font to use to set the text
         bool Font(const GString & in_sFontName, INT32 in_iSize = -1);

         void FontName(const GString & in_sFontName, bool in_bUpdate = false);
         GString FontName();
         
         //! Sets the size of the text of the Label
         void  FontSize(INT32 in_iSize, bool in_bUpdate = false);
         INT32 FontSize();
                 
         //! Sets the text position
         void TextAlignment(GString in_sAlignment, bool in_bUpdate = false);

         GUI::EEventStatus::Enum OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         GColorRGBInt FontColorNormal();
         GColorRGBInt FontColorDisabled();

         bool FontColorNormal(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorNormal(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorNormal(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         
         bool FontColorDisabled(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorDisabled(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorDisabled(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         
         GColorRGBInt FontColor();
         void FontColor(const GColorRGBInt & in_rgbaFontColor);

         void TextOffset(INT16 in_iX, INT16 in_iY);
         void TextOffset(const GVector2D<INT32> & in_TextOffset, bool in_bUpdate = false);
         GVector2D<INT32> TextOffset();

         virtual void Position(INT32, INT32);
         virtual void Size(INT32, INT32);

         virtual void Multiline(bool);
         virtual bool Multiline() const;

         //! Gets the text contained in the Label
         virtual const GString& Text() const;

         CGFX_Text_2D_Itf* Text2D();

         bool OnCreate();
         bool OnDestroy();

         static ETextAlignment::Enum GetTextAlignmentFromString(const GString & in_sTextAlignment);

         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         GLabel();
         virtual ~GLabel();

         CGFX_Model_Itf*   			m_pTextModel;
         CGFX_Text_2D_Itf*		      m_pText2D;

         //! String content of the text box
         GString                    m_sText;
         GString                    m_sFontName;
         INT32                      m_iFontSize;
         UINT8                      m_iFontAlpha;

         GVector2D<INT32>           m_TextOffset;
         ETextAlignment::Enum   m_eTextAlignment;
         
         GColorRGBInt               m_FontColorNormal;
         GColorRGBInt               m_FontColorDisabled;
         
         bool m_bUseFontColorDisabled;
         bool m_bMultiline;


      private:

      };
   }
};
#endif //_GLabel_H_
