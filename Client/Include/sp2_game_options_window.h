/**************************************************************
*
* sp2_game_options_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GameOptionsWindow_H_
#define _CSP2_GameOptionsWindow_H_

#include "../include/sp2_frame_cursor_meter.h"

namespace SP2
{

   class GGameOptionsWindow : public GBaseWindow
   {
   public:
	   GGameOptionsWindow();
	   ~GGameOptionsWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = NULL);
      EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      
      bool Update();
      void Save();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:

      // Refresh autosave option and display sub options
      void UpdateAutoSaveOptions(void);

      GUI::GTabbed* m_pObjTab;
      
      GUI::GEditBox* m_pObjPlayerNameEdt;

      GUI::GComboBox* m_pObjResolutionCbo;
      GUI::GComboBox* m_pObjColorDepthCbo;
      GUI::GComboBox* m_pObjTextureDetailCbo;
      GUI::GComboBox* m_pObjZoomLevelCbo;
      GUI::GComboBox* m_pObjGeometricDetailCbo;

      GUI::GButton*   m_pClearBanPlayerListButton;

      GUI::GToggle*   m_pObjCloudsTog;
      GUI::GToggle*   m_pObjSunTog;
      GUI::GToggle*   m_pObjMoonTog;
      GUI::GToggle*   m_pObjDayCycleTog;

      GUI::GToggle*   m_pObjAutoSaveTog;
      GUI::GToggle*   m_pObjIncrementalSaveTog;
      SP2::GEditBoxNumeric*  m_pObjAutoSaveFrequencyEdit;
      GUI::GEditBox*  m_pObjAutoSaveFileName;
      GUI::GFrame*    m_pObjFrameAutoSave;

      SP2::GCursorMeter* m_pObjMusicMtr;
      SP2::GCursorMeter* m_pObjSoundMtr;

		SP2::GCursorMeter* m_pObjAIAggressivenessMtr;

      GUI::GButton*   m_pObjConfirmBtn;
      GUI::GButton*   m_pObjCancelBtn;
      

      GDataOptionsWindow* m_pData;
   };
};

#endif //_CSP2_GameOptionsWindow_H_
