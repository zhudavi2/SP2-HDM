#include "golem_sdk_api.h"
/************************************************
* File:  golem_generic_button.h
* Owner: Mathieu Tremblay
* Description:
*	  Describes the generic button to be used in the GUI
*
* History:
* ========
*	24/03/2003			Creation
*************************************************/

#ifndef _GButton_H_
#define _GButton_H_

namespace SDK
{
   namespace GUI
   {

      /*!
      * Represents a basic GUI Button
      **/
      class GOLEM_SDK_API GButton : public GLabel
      {
      public:

         bool OnCreate();
         bool OnDestroy();

         GUI::EEventStatus::Enum OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* callerPt);

         void Enabled(bool in_bEnabled);
         bool Enabled() const
         {
            return __super::Enabled();
         }

         bool PictureHighlight(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureHighlight(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool PictureHighlightDown(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureHighlightDown(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool PictureDisabledDown(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureDisabledDown(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool PictureDown(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureDown(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);

         bool ColorHighlight(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorHighlight(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorHighlight(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorHighlight();

         bool ColorHighlightDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorHighlightDown(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorHighlightDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorHighlightDown();

         bool ColorDisabledDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorDisabledDown(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorDisabledDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorDisabledDown();

         bool ColorDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorDown(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorDown();

         bool FontColorHighlight(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorHighlight(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorHighlight(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         GColorRGBInt FontColorHighlight();

         bool FontColorDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorDown(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         GColorRGBInt FontColorDown();

         bool FontColorHighlightDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorHighlightDown(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorHighlightDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         GColorRGBInt FontColorHighlightDown();

         bool FontColorDisabledDown(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = false);
         void FontColorDisabledDown(const GString &in_sColorString, const UINT8 &in_iAlpha,         bool in_bUpdate = false);
         void FontColorDisabledDown(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = false);
         GColorRGBInt FontColorDisabledDown();

         static const GString TypeName;

      protected:
         GButton();
         virtual ~GButton();

         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         bool Update();

         CGFX_Texture_Itf* m_PictureHighlight;
         CGFX_Texture_Itf* m_PictureHighlightDown;
         CGFX_Texture_Itf* m_PictureDisabledDown;
         CGFX_Texture_Itf* m_PictureDown;

         GColorRGBInt m_ColorHighlight;
         GColorRGBInt m_ColorHighlightDown;
         GColorRGBInt m_ColorDisabledDown;
         GColorRGBInt m_ColorDown;

         GColorRGBInt m_FontColorHighlight;
         GColorRGBInt m_FontColorHighlightDown;
         GColorRGBInt m_FontColorDisabledDown;
         GColorRGBInt m_FontColorDown;

         bool m_bStateMouseDown;
         bool m_bStateOver;         
         bool m_bStatePushed; // for toggle only

         bool m_bUseColorHighlight;
         bool m_bUseColorHighlightDown;
         bool m_bUseColorDisabledDown;
         bool m_bUseColorDown;

         bool m_bUsePictureHighlight;
         bool m_bUsePictureHighlightDown;
         bool m_bUsePictureDisabledDown;
         bool m_bUsePictureDown;

         bool m_bUseFontColorHighlight;
         bool m_bUseFontColorHighlightDown;
         bool m_bUseFontColorDisabledDown;
         bool m_bUseFontColorDown;

      private:
      };
   };
}
#endif //_GFrame_H_
