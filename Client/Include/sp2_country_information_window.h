/**************************************************************
*
* sp2_country_information_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_COUNTRY_INFORMATION_WINDOW_H_
#define _CSP2_COUNTRY_INFORMATION_WINDOW_H_

#include "sp2_frame_cursor_meter.h"

namespace SP2
{

   class GCountryInformationWindow : public GBaseWindow
   {
   public:
	   GCountryInformationWindow();
	   ~GCountryInformationWindow();

	   bool OnCreate();
      bool OnDestroy();

      void OnShow();
      
      bool Update();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_Data, GUI::GBaseObject* in_pCaller = NULL);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      GUI::GFrame*       m_pObjMap;

      CGFX_Texture_Itf*  m_pMapTxtr;
      CGFX_Texture_Renderer_Itf* m_pMapRender;
      GImage2D*          m_pImage;

      CGFX_Scene_Itf*    m_pScene;
      CGFX_Camera_Itf*   m_pCamera;
      CGFX_Model_Itf*    m_pCountryModel;
      GFX::GMeshComplex* m_pCountryMesh;
      CGFX_Model_Itf*    m_pCountryBorderModel;
      GFX::GMeshComplex* m_pCountryBorderMesh;
      CGFX_Model_Itf*    m_pCountryRegionsModel;
      GFX::GMeshComplex* m_pCountryRegionsMesh;
      GFX::GOverridePSCascade*   m_pOverrideRegionColor;
      GFX::GOverridePSCascade*   m_pOverrideFillColor;

      SP2::GGradientBar* m_pObjArableMtr;
      SP2::GGradientBar* m_pObjForestMtr;
      SP2::GGradientBar* m_pObjParksMtr;
      SP2::GGradientBar* m_pObjUnusableMtr;

      GUI::GLabel*       m_pObjArableLbl;
      GUI::GLabel*       m_pObjForestLbl;
      GUI::GLabel*       m_pObjParksLbl;
      GUI::GLabel*       m_pObjUnusableLbl;

      GUI::GLabel*       m_pObjPop15Lbl;
      GUI::GLabel*       m_pObjPop1565Lbl;
      GUI::GLabel*       m_pObjPop65Lbl;
      GUI::GLabel*       m_pObjPopTotalLbl;

      GUI::GLabel*       m_pObjPop15RatioLbl;
      GUI::GLabel*       m_pObjPop1565RatioLbl;
      GUI::GLabel*       m_pObjPop65RatioLbl;
      GUI::GLabel*       m_pObjPopTotalRatioLbl;

      GUI::GLabel*       m_pObjLandAreaLbl;
      GUI::GLabel*       m_pObjWaterAreaLbl;
      GUI::GLabel*       m_pObjTotalAreaLbl;

      GUI::GLabel*       m_pObjBirthRateTxt;
      GUI::GLabel*       m_pObjDeathRateTxt;
      GUI::GLabel*       m_pObjTelecomTxt;
      GUI::GLabel*       m_pObjInfrastructureTxt;

      GUI::GFrame*       m_pObjClimateFrm;
      GUI::GLabel*       m_pObjClimateLbl;
      GUI::GVScrollBar*  m_pObjClimateScr;

      GUI::GLabel*       m_pObjCountryLbl;
      GUI::GPictureBox*  m_pObjCountryPic;
      GUI::GPictureBox*  m_pObjWorldPic;

      GDataCountryInformationWindow* m_pData;
   };
};

#endif //_CSP2_COUNTRY_INFORMATION_WINDOW_H_
