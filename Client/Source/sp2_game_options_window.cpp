/**************************************************************
*
* sp2_game_options_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_game_options_window.h"
#include "../include/sp2_background_earth.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GGameOptionsWindow::TypeName(L"Options_Window");

GUI::GBaseObject* GGameOptionsWindow::New()
{
	return (GUI::GBaseObject*) new GGameOptionsWindow();
}


GGameOptionsWindow::GGameOptionsWindow() : GBaseWindow()
{
}

GGameOptionsWindow::~GGameOptionsWindow()
{
}

bool GGameOptionsWindow::OnCreate()
{
	__super::OnCreate();

   m_pData = &(g_ClientDAL.m_DataOptionsWindow);

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   //Fix: Resolution: In the game the resolution isn’t shown properly
   m_pData->m_iResolutionX     = mode.m_Resolution.x;
   m_pData->m_iResolutionY     = mode.m_Resolution.y;
   m_pData->m_iColorDepth      = mode.m_FrameBufferBitDepth;

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

   m_pObjTab                  = (GUI::GTabbed*)m_pObjInnerFrm->Child(L"tabOptions");
   
   m_pObjTab->m_vTabPage[0]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjTab->m_vTabPage[0]->PictureNormal( GString(L"000647") );
   m_pObjTab->m_vTabPage[0]->TooltipText( g_ClientDAL.GetString(102228) );
   m_pObjTab->m_vTabButtons[0]->TooltipText( g_ClientDAL.GetString(102228) );

   m_pObjTab->m_vTabPage[1]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjTab->m_vTabPage[1]->PictureNormal( GString(L"000647") );
   m_pObjTab->m_vTabPage[1]->TooltipText( g_ClientDAL.GetString(102229) );
   m_pObjTab->m_vTabButtons[1]->TooltipText( g_ClientDAL.GetString(102229) );

   m_pObjTab->m_vTabPage[2]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjTab->m_vTabPage[2]->PictureNormal( GString(L"000647") );
   m_pObjTab->m_vTabPage[2]->TooltipText( g_ClientDAL.GetString(102230) );
   m_pObjTab->m_vTabButtons[2]->TooltipText( g_ClientDAL.GetString(102230) );

   m_pObjPlayerNameEdt        = (GUI::GEditBox*)m_pObjTab->m_vTabPage[0]->Child(L"edtName");

   m_pObjResolutionCbo        = (GUI::GComboBox*)m_pObjTab->m_vTabPage[1]->Child(L"cboResolution");
   m_pObjColorDepthCbo        = (GUI::GComboBox*)m_pObjTab->m_vTabPage[1]->Child(L"cboColorDepth");
   m_pObjTextureDetailCbo     = (GUI::GComboBox*)m_pObjTab->m_vTabPage[1]->Child(L"cboTextureDetail");
   m_pObjGeometricDetailCbo   = (GUI::GComboBox*)m_pObjTab->m_vTabPage[1]->Child(L"cboGeometricDetail");
   m_pObjZoomLevelCbo         = (GUI::GComboBox*)m_pObjTab->m_vTabPage[1]->Child(L"cboZoomLevel");

   m_pObjCloudsTog            = (GUI::GToggle*)m_pObjTab->m_vTabPage[1]->Child(L"togClouds");
   m_pObjSunTog               = (GUI::GToggle*)m_pObjTab->m_vTabPage[1]->Child(L"togSun");
   m_pObjMoonTog              = (GUI::GToggle*)m_pObjTab->m_vTabPage[1]->Child(L"togMoon");
   m_pObjDayCycleTog          = (GUI::GToggle*)m_pObjTab->m_vTabPage[1]->Child(L"togDayCycle");

   m_pObjMusicMtr             = (SP2::GCursorMeter*)m_pObjTab->m_vTabPage[2]->Child(L"frmMusicVolume");
   m_pObjSoundMtr             = (SP2::GCursorMeter*)m_pObjTab->m_vTabPage[2]->Child(L"frmSoundVolume");
	m_pObjAIAggressivenessMtr	= (SP2::GCursorMeter*)m_pObjTab->m_vTabPage[0]->Child(L"frmAIAggressivenessMtr");

   m_pClearBanPlayerListButton= (GUI::GButton*)m_pObjTab->m_vTabPage[0]->Child("btnClearBan");

   m_pObjMusicMtr->Owner(this);
   m_pObjSoundMtr->Owner(this);        
	m_pObjAIAggressivenessMtr->Owner(this);
   
   m_pObjConfirmBtn           = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancelBtn            = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");         

   m_pObjTextureDetailCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallLow) );
   m_pObjTextureDetailCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallHigh) );

   m_pObjTextureDetailCbo->DropSize( 30 );

   m_pObjGeometricDetailCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallLow) );
   m_pObjGeometricDetailCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallMedium) );
   m_pObjGeometricDetailCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallHigh) );

   m_pObjGeometricDetailCbo->DropSize( 44 );

   m_pObjZoomLevelCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallAll) );
   m_pObjZoomLevelCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallElevationAndWater) );
   m_pObjZoomLevelCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallElevationOnly) );
   m_pObjZoomLevelCbo->Add_Item( g_ClientDAL.GetString(EStrId::SmallNone) );

   m_pObjZoomLevelCbo->DropSize( 58 );

   m_pObjResolutionCbo->Enabled(false);     
   m_pObjColorDepthCbo->Enabled(false);     

   // AutoSave
   m_pObjFrameAutoSave        = (GUI::GFrame*)m_pObjTab->m_vTabPage[0]->Child(L"frmAutosave");
   m_pObjAutoSaveTog          = (GUI::GToggle*)m_pObjTab->m_vTabPage[0]->Child(L"togAutosave");
   m_pObjIncrementalSaveTog   = (GUI::GToggle*)m_pObjFrameAutoSave->Child(L"togIncremental");
   m_pObjAutoSaveFrequencyEdit= (SP2::GEditBoxNumeric*)m_pObjFrameAutoSave->Child(L"edtAutosaveEach");
   m_pObjAutoSaveFileName     = (GUI::GEditBox*)m_pObjFrameAutoSave->Child(L"edtAutosaveName");

   return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GGameOptionsWindow::OnShow()
{
   __super::OnShow();

   BringToFront();
   
   if(g_ClientDDL.MainBar())
      g_ClientDDL.MainBar()->BringToFront();

   if(g_ClientDDL.MiniMap())
      g_ClientDDL.MiniMap()->BringToFront();

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

   // If admin player show clear ban list player list button
   if(g_SP2Client->Id() == g_Joshua.AdminPlayerID())
   {
      m_pClearBanPlayerListButton->Show();
   }
   else
   {
      m_pClearBanPlayerListButton->Hide();
   }

   Update();
}

GUI::EEventStatus::Enum GGameOptionsWindow::OnCustomEvent(UINT32 in_EventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventID)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         if( (GUI::GComboBox*)in_EventData.Data == m_pObjZoomLevelCbo )
         {
            SP2::EZoomLevel::Enum l_eZoomLevel;
            if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallAll) )
               l_eZoomLevel = SP2::EZoomLevel::All;
            else if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallElevationOnly) )
               l_eZoomLevel = SP2::EZoomLevel::Elevation;
            else if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallElevationAndWater) )
               l_eZoomLevel = SP2::EZoomLevel::ElevationWater;
            else
               l_eZoomLevel = SP2::EZoomLevel::None;

            g_ClientDDL.BackgroundEarth()->SetAtlasZoomLevel(l_eZoomLevel);
         }
      }
      break;
   }
   
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GGameOptionsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {

      if(in_pCaller  == m_pObjAutoSaveTog)
      {
         UpdateAutoSaveOptions();
      }
      else
      if(in_pCaller == m_pObjCancelBtn || in_pCaller == m_pObjExitBtn)
      {
         Hide();
      }
      else if(in_pCaller == m_pObjConfirmBtn)
      {
         Save();
         Hide();
      }
      else if(in_pCaller == m_pClearBanPlayerListButton)
      {
         // Send a clear ban player list request to the server
         GGameEventSPtr ge = CREATE_GAME_EVENT(SDK::Event::GSystemGameEvent);
         SDK::Event::GSystemGameEvent* sysGe = (SDK::Event::GSystemGameEvent*)ge.get();
         sysGe->SystemEventType(SDK::Event::GSystemGameEvent::BANNED_PLAYER_LIST_COMMAND);
         ge->m_iSource   = g_SP2Client->Id();
         ge->m_iTarget = SDK::Event::ESpecialTargets::Server;
         sysGe->m_iSystemEventData = 1;
         g_Joshua.RaiseEvent(ge);

      }
   }
   
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GGameOptionsWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller == m_pObjSoundMtr || in_pCaller == m_pObjSoundMtr->MeterBtn()[0] || in_pCaller == m_pObjSoundMtr->MeterBtn()[1])
   {
      g_SP2Client->SoundBox()->PlaySound( SP2::ESoundEventID::IncomingMail );
   }

   return GUI::EEventStatus::Handled;
}

bool GGameOptionsWindow::Update()
{  
   m_pObjPlayerNameEdt->Text(g_ClientDAL.PlayerName());

	m_pObjResolutionCbo->Selected_Content( GString(m_pData->m_iResolutionX) + L"x" + GString(m_pData->m_iResolutionY) );
   m_pObjColorDepthCbo->Selected_Content( GString(m_pData->m_iColorDepth) + GString(L" bits") );

   switch(m_pData->m_eTextureQuality)
   {
   case SP2::EQualityLevel::Low:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallLow) );
      break;
   case SP2::EQualityLevel::Medium:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallMedium) );
      break;
   case SP2::EQualityLevel::High:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallHigh) );
      break;
   }

   switch(m_pData->m_eGeometricDetail)
   {
   case SP2::EQualityLevel::Low:
      m_pObjGeometricDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallLow) );
      break;
   case SP2::EQualityLevel::Medium:
      m_pObjGeometricDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallMedium) );
      break;
   case SP2::EQualityLevel::High:
      m_pObjGeometricDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallHigh) );
      break;
   }

   switch(m_pData->m_eTextureQuality)
   {
   case SP2::EQualityLevel::Low:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallLow) );
      break;
   case SP2::EQualityLevel::Medium:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallMedium) );
      break;
   case SP2::EQualityLevel::High:
      m_pObjTextureDetailCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallHigh) );
      break;
   }

   switch(m_pData->m_eZoomLevel)
   {
   case SP2::EZoomLevel::All:
      m_pObjZoomLevelCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallAll) );
      break;
   case SP2::EZoomLevel::Elevation:
      m_pObjZoomLevelCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallElevationOnly) );
      break;
   case SP2::EZoomLevel::ElevationWater:
      m_pObjZoomLevelCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallElevationAndWater) );
      break;
   case SP2::EZoomLevel::None:
      m_pObjZoomLevelCbo->Selected_Content( g_ClientDAL.GetString(EStrId::SmallNone) );
      break;
   }


	m_pObjAIAggressivenessMtr->Value( m_pData->m_fAIAggressiveness, 0, false, false );
   m_pObjMusicMtr->Value( m_pData->m_fMusicVolume, 0, false, false );
   m_pObjSoundMtr->Value( m_pData->m_fSoundVolume, 0, false, false );
   m_pObjCloudsTog->ToggleState( m_pData->m_bClouds );
   m_pObjSunTog->ToggleState( m_pData->m_bSun );
   m_pObjMoonTog->ToggleState( m_pData->m_bMoon );
   m_pObjDayCycleTog->ToggleState( m_pData->m_bDayCycle );

   // Update Autosave information
   m_pObjAutoSaveTog->ToggleState(g_ClientDCL.AutoSaveInformation().m_bAutoSave);
   m_pObjIncrementalSaveTog->ToggleState(g_ClientDCL.AutoSaveInformation().m_bIncrementalSave);
   m_pObjAutoSaveFrequencyEdit->Value(g_ClientDCL.AutoSaveInformation().m_iSaveFrequency);
   m_pObjAutoSaveFileName->Text(g_ClientDCL.AutoSaveInformation().m_sSaveFileName);

   UpdateAutoSaveOptions();

   return true;
}

void GGameOptionsWindow::UpdateAutoSaveOptions(void)
{
   // Disable or enable information from autosave toggle state.
   if(m_pObjAutoSaveTog->ToggleState())
   {
      m_pObjFrameAutoSave->Show();
   }
   else
   {
      m_pObjFrameAutoSave->Hide();
   }
}

void GGameOptionsWindow::Save()
{
	if(m_pData->m_sPlayerName != m_pObjPlayerNameEdt->Text())
	{
		GString l_sNode = SP2::PLAYER_NAME;
		GString l_sValue = m_pObjPlayerNameEdt->Text();
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
      m_pData->m_sPlayerName = m_pObjPlayerNameEdt->Text();
	}

   if( m_pData->m_bClouds != m_pObjCloudsTog->ToggleState() )
   {
      m_pData->m_bClouds = m_pObjCloudsTog->ToggleState();
      m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::CLOUDS;
		GString l_sValue;
		if(m_pData->m_bClouds)
			l_sValue = L"true";
		else
			l_sValue = L"false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));

   }

   if( m_pData->m_bSun != m_pObjSunTog->ToggleState() )
   {
      m_pData->m_bSun = m_pObjSunTog->ToggleState();
      m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::SUN;
		GString l_sValue;
		if(m_pData->m_bSun)
			l_sValue = L"true";
		else
			l_sValue = L"false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   if( m_pData->m_bMoon != m_pObjMoonTog->ToggleState() )
   {
      m_pData->m_bMoon = m_pObjMoonTog->ToggleState();
      m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::MOON;
		GString l_sValue;
		if(m_pData->m_bMoon)
			l_sValue = L"true";
		else
			l_sValue = L"false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   if( m_pData->m_bDayCycle != m_pObjDayCycleTog->ToggleState() )
   {
      m_pData->m_bDayCycle = m_pObjDayCycleTog->ToggleState();
      m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::DAY_LIGHT_CYCLE;
		GString l_sValue;
		if(m_pData->m_bDayCycle)
			l_sValue = L"true";
		else
			l_sValue = L"false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

	if( m_pData->m_fAIAggressiveness != (REAL32)m_pObjAIAggressivenessMtr->Value( 0 ) )
   {
      m_pData->m_fAIAggressiveness = (REAL32)m_pObjAIAggressivenessMtr->Value( 0 );
		GString l_sNode = SP2::AI_AGGRESSIVENESS;
		GString l_sValue = GString(m_pData->m_fAIAggressiveness);	
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
		

		if(g_Joshua.Client()->Id() > -1)
		{
			//Send Game Event
			SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GChangeAIAggressiveness);
			l_Evt->m_iSource = g_Joshua.Client()->Id();
			l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
			SP2::Event::GChangeAIAggressiveness* l_pEvt = (SP2::Event::GChangeAIAggressiveness*)l_Evt.get();
			l_pEvt->AIAggressiveness(m_pData->m_fAIAggressiveness);
			g_Joshua.RaiseEvent(l_Evt);
		}
	}

   if( m_pData->m_fMusicVolume != (REAL32)m_pObjMusicMtr->Value( 0 ) )
   {
      m_pData->m_fMusicVolume = (REAL32)m_pObjMusicMtr->Value( 0 );
      m_pData->m_bSoundDirty = true;
		GString l_sNode = SP2::AUDIO_VOLUME;
		GString l_sValue = GString(m_pData->m_fMusicVolume);		
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   if( m_pData->m_fSoundVolume != (REAL32)m_pObjSoundMtr->Value( 0 ) )
   {
      m_pData->m_fSoundVolume = (REAL32)m_pObjSoundMtr->Value( 0 );   
      m_pData->m_bSoundDirty = true;
		GString l_sNode = SP2::SOUND_VOLUME;
		GString l_sValue = GString(m_pData->m_fSoundVolume);		
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   SP2::EZoomLevel::Enum l_eZoomLevel;
   if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallAll) )
      l_eZoomLevel = SP2::EZoomLevel::All;
   else if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallElevationOnly) )
      l_eZoomLevel = SP2::EZoomLevel::Elevation;
   else if( m_pObjZoomLevelCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallElevationAndWater) )
      l_eZoomLevel = SP2::EZoomLevel::ElevationWater;
   else
      l_eZoomLevel = SP2::EZoomLevel::None;

   if( l_eZoomLevel != m_pData->m_eZoomLevel )
   {
      m_pData->m_eZoomLevel = l_eZoomLevel;
      //No need to do this since it's applied as soon as selected
      //m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::ZOOM_LEVEL;
		GString l_sValue = GString(m_pData->m_eZoomLevel);		
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   SP2::EQualityLevel::Enum l_eGeometricDetail;

   if( m_pObjGeometricDetailCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallLow) )
      l_eGeometricDetail = EQualityLevel::Low;
   else if( m_pObjGeometricDetailCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallMedium) )
      l_eGeometricDetail = EQualityLevel::Medium;
   else if( m_pObjGeometricDetailCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallHigh) )
      l_eGeometricDetail = EQualityLevel::High;
   else
      l_eGeometricDetail = EQualityLevel::Medium;

   if( l_eGeometricDetail != m_pData->m_eGeometricDetail )
   {
      m_pData->m_eGeometricDetail = l_eGeometricDetail;
      m_pData->m_bGeometricDetailDirty = true;
		GString l_sNode = SP2::GEOMETRIC_DETAIL;
		GString l_sValue = GString(m_pData->m_eGeometricDetail);		
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   SP2::EQualityLevel::Enum l_eTextureDetail;

   if( m_pObjTextureDetailCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::SmallLow) )
      l_eTextureDetail = EQualityLevel::Low;
   else
      l_eTextureDetail = EQualityLevel::High;

   if( l_eTextureDetail != m_pData->m_eTextureQuality )
   {
      m_pData->m_eTextureQuality = l_eTextureDetail;
      m_pData->m_bGraphicDetailsDirty = true;
		GString l_sNode = SP2::TEXTURE_DETAIL;
		GString l_sValue = GString(m_pData->m_eTextureQuality);		
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }

   // Verify autosave
   if(m_pObjAutoSaveTog->ToggleState() != g_ClientDCL.AutoSaveInformation().m_bAutoSave)
   {
      GString l_sNode = SP2::AUTOSAVE_ENABLED;
      GString l_sValue;
      if(m_pObjAutoSaveTog->ToggleState())
		   l_sValue = "true";
      else
         l_sValue = "false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
      g_ClientDCL.AutoSaveInformation().m_bAutoSave = m_pObjAutoSaveTog->ToggleState();
   }
   if(m_pObjIncrementalSaveTog->ToggleState() != g_ClientDCL.AutoSaveInformation().m_bIncrementalSave)
   {
      GString l_sNode = SP2::AUTOSAVE_INCREMENTAL;
      GString l_sValue;
      if(m_pObjIncrementalSaveTog->ToggleState())
		   l_sValue = "true";
      else
         l_sValue = "false";
		g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
      g_ClientDCL.AutoSaveInformation().m_bIncrementalSave = m_pObjIncrementalSaveTog->ToggleState();
   }
   if((UINT32)m_pObjAutoSaveFrequencyEdit->Value() != g_ClientDCL.AutoSaveInformation().m_iSaveFrequency)
   {
      // Do not allow setting frequency lower than 1
      if(m_pObjAutoSaveFrequencyEdit->Value() < 1)
      {
         m_pObjAutoSaveFrequencyEdit->Value(1);
      }

      GString l_sNode = SP2::AUTOSAVE_FREQUENCY;
      GString l_sValue = m_pObjAutoSaveFrequencyEdit->Text();
      g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(SP2::CLIENT_CONFIG));
      g_ClientDCL.AutoSaveInformation().m_iSaveFrequency = (UINT32)m_pObjAutoSaveFrequencyEdit->Value();
   }
   if(m_pObjAutoSaveFileName->Text() != g_ClientDCL.AutoSaveInformation().m_sSaveFileName)
   {
      GString l_sNode = SP2::AUTOSAVE_FILENAME;
      GString l_sValue = m_pObjAutoSaveFileName->Text();
      g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
      g_ClientDCL.AutoSaveInformation().m_sSaveFileName = m_pObjAutoSaveFileName->Text() ;

      //If the file name has changed , reset the counter and save it.
      g_ClientDCL.AutoSaveInformation().m_iSaveCount = 0;
      l_sNode = SP2::AUTOSAVE_COUNTER;
      l_sValue = GString(g_ClientDCL.AutoSaveInformation().m_iSaveCount);
      g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
   }
   
   g_ClientDAL.ApplyOptions();
}
