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

#ifndef _GFrame_H_
#define _GFrame_H_

/*!
* Represents a basic 2D Frame (which has a Model and a window associated)
**/ 
namespace SDK
{
   namespace GUI
   {
      struct GBorders
      {
         GBorders()
         {
            m_iBorderLeft     = 0;
            m_iBorderTop      = 0;
            m_iBorderRight    = 0;
            m_iBorderBottom   = 0;
         }

         ~GBorders(){}

         GBorders(const GBorders & in_BorderToCopy)
         {
            *this = in_BorderToCopy;
         }

         GBorders(INT32 in_iLeftBorder, INT32 in_iTopBorder, INT32 in_iRightBorder, INT32 in_iBottomBorder)
         {
            m_iBorderLeft   = in_iLeftBorder;
            m_iBorderTop    = in_iTopBorder;
            m_iBorderRight  = in_iRightBorder;
            m_iBorderBottom = in_iBottomBorder;
         }

         GBorders(const GString & in_sBorderStr)
         {
            if(!in_sBorderStr.empty() )
            {
               swscanf(in_sBorderStr.c_str(),L"(%d,%d)-(%d,%d)",&m_iBorderLeft,&m_iBorderTop,&m_iBorderRight,&m_iBorderBottom);
            }
            else
            {
               m_iBorderLeft     = 0;
               m_iBorderTop      = 0;
               m_iBorderRight    = 0;
               m_iBorderBottom   = 0;
            }
         }

         GBorders & operator=(const GBorders & in_BorderToCopy)
         {
            m_iBorderLeft     = in_BorderToCopy.m_iBorderLeft;
            m_iBorderTop      = in_BorderToCopy.m_iBorderTop;
            m_iBorderRight    = in_BorderToCopy.m_iBorderRight;
            m_iBorderBottom   = in_BorderToCopy.m_iBorderBottom;

            return *this;
         }

         INT32 m_iBorderLeft;
         INT32 m_iBorderTop;
         INT32 m_iBorderRight;
         INT32 m_iBorderBottom;         
      };

      class GOLEM_SDK_API GFrame : public GUIContainer
      {
      public:

         bool OnCreate();
         bool OnDestroy();

         UINT8 Alpha();

         void Enabled(bool in_bEnabled);
         bool Enabled() const
         {
            return __super::Enabled();
         }

         bool ColorNormal(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorNormal(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorNormal(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorNormal();

         bool ColorDisabled(const GString &in_sColorString, const GString &in_sAlphaString, bool in_bUpdate = true);
         void ColorDisabled(const GString &in_sColorString, const UINT8 &in_iAlpha, bool in_bUpdate = true);
         void ColorDisabled(const GColorRGBInt & in_rgbaColor, bool in_bUpdate = true);
         GColorRGBInt ColorDisabled();

         bool ColorBackground(const GColorRGBInt &in_rgbaColor);
         

         bool PictureBackground(CGFX_Texture_Itf* in_pTexture);
         bool PictureClear();
         
         bool PictureNormal(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureNormal(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);
         CGFX_Texture_Itf* PictureNormal();

         void PictureDisabled(const GString & in_sFilename, bool in_bUpdate = true);
         void PictureDisabled(CGFX_Texture_Itf* in_pTxtr, bool in_bUpdate = true);
         CGFX_Texture_Itf* PictureDisabled();

         void Borders( const GBorders & in_Borders );

         void ReplaceIfValid(GString & l_Variable, const GString & l_ValueToCheck);

         GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
         GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOver(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
         GUI::EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

         bool Update();

         static CGFX_Texture_Itf* GetTextureFromFilename(const GString & in_sFilename);
         static GColor GetColorFromString(GString);
         static GColor GetColorFromString(GString, GString in_Alpha);

         static const GString TypeName;

      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

         bool IsNoValue(const GString & l_Text);

         GFrame();
         virtual ~GFrame();

         CGFX_Texture_Itf* m_PictureNormal;
         CGFX_Texture_Itf* m_PictureDisabled;

         GColorRGBInt m_ColorNormal;
         GColorRGBInt m_ColorDisabled;

         GBorders m_Borders;
         
         UINT8   m_iAlpha;

         bool m_bUseColorNormal;
         bool m_bUseColorDisabled;
         bool m_bUsePictureNormal;
         bool m_bUsePictureDisabled;

      private:
      };

      class GOLEM_SDK_API GSequenceBox : public GFrame
      {
      public:
         static const GString TypeName;
      protected:
         friend class GUI::GManager;
         static GUI::GBaseObject* New() {return new GSequenceBox;}
      };
   }
}

#endif //_GFrame_H_
