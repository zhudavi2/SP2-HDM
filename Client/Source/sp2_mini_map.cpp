/**************************************************************
*
* sp2_mini_map.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_mini_map.h"
#include "../include/sp2_sound.h"
#include "../include/sp2_background_earth.h"

const GString GMiniMap::TypeName(L"Mini_Map");

/*!
 * Mini Map constructor utility function
 **/
GUI::GBaseObject* GMiniMap::New()
{
	return new GMiniMap();
}

GMiniMap::GMiniMap() : GFrame()
{
   m_pObjHome         = NULL;
   m_pObjMagnifier    = NULL;
   m_pObjCrosshair    = NULL;
   m_pObjZoomBar      = NULL;
   m_pObjLongitudeBar = NULL;
   m_pObjLatitudeBar  = NULL;
   m_pObjMap          = NULL;
   m_pObjRealisticFrm = NULL;
   m_pObjStrategicFrm = NULL;
   m_pLookSwapTrack   = NULL;
   
   m_pSlidingTrack     = NULL;
}

GMiniMap::~GMiniMap()
{
}

bool GMiniMap::OnCreate()
{
   __super::OnCreate();

   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : " + this->Name());

   m_fZoomInc         = Property(L"ZoomInc").ToREAL32();

   m_pObjHome         = (GButton*)Child(L"btnHome");
   m_pObjMagnifier    = (GToggle*)Child(L"togMagnifier");
   m_pObjZoomBar      = (GButton*)Child(L"btnZoom");
   m_pObjHidingButton = (GButton*)Child(L"btnHiding");
   m_pObjHidingZoom   = (GButton*)Child(L"btnZoomRect");
   m_pObjMap          = (GButton*)Child(L"frmMap")->Child(L"btnMap");
   m_pObjCrosshair    = (GButton*)Child(L"frmMap")->Child(L"btnCrosshair");
   m_pObjLongitudeBar = CREATE_GUI_OBJECT_NAMED_STATIC(GFrame, L"LongitudeBar", Child(L"frmMap"), false);
   m_pObjLatitudeBar  = CREATE_GUI_OBJECT_NAMED_STATIC(GFrame, L"LatitudeBar",  Child(L"frmMap"), false);
   m_pObjRealisticFrm = (GFrame*)Child(L"frmMap")->Child(L"frmRealistic");
   m_pObjStrategicFrm = (GFrame*)Child(L"frmMap")->Child(L"frmStrategic");

   //The frame frmMap must clip his children so the crosshair doesn't overlap with the mini map border
   ((GFX::GWindow*)((GFrame*)Child(L"frmMap"))->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjHidingButton->Height(m_pObjZoomBar->Height());

   //Initialize the crosshair longitude and latitude bar
   m_pObjLongitudeBar->ColorNormal(GColorRGBInt(255,118,81,153));
   m_pObjLatitudeBar->ColorNormal(GColorRGBInt(255,118,81,153));

   m_pObjLongitudeBar->Size(1,100);
   m_pObjLatitudeBar->Size(184,1); 

   m_pObjLongitudeBar->Top(0);
   m_pObjLatitudeBar->Left(0);


   m_pObjStrategicFrm->BringToFront();
   m_pObjRealisticFrm->BringToFront();
   m_pObjMap->BringToFront();
   m_pObjCrosshair->BringToFront();
   m_pObjHidingButton->BringToFront();
   m_pObjHidingZoom->BringToFront();

   m_pObjLongitudeBar->Show();
   m_pObjLatitudeBar->Show();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   Position((INT16)mode.m_Resolution.x - Width(),(INT16)mode.m_Resolution.y - Height());

   m_pSlidingTrack = g_ClientDDL.AnimationCreateSliding(0.5f, 0.26f, Top() + Height(), Top(), Model()->Get_Transform());


   //Tiny delay so the keys has a difference between each of them
   CreateTransitionAnimation(1.0f, 0.01f);

   return true;
}

void GMiniMap::OnShow()
{
   __super::OnShow();
}

/*!
 * Create the animation for the transition from realistic to strategic and otherwise
 * @param   in_fAnimationTime : Anim time
 * @param   in_fDelay         : Delay before anim / MUST NOT BE 0
 **/
void GMiniMap::CreateTransitionAnimation(REAL32 in_fAnimationTime, REAL32 in_fDelay)
{
   //Create anim
   CGFX_Key_Linear l_pAnimKeys[4] =
   {
      {0.f,                               1.0f},
      {in_fDelay,                         1.0f},
      {in_fDelay + in_fAnimationTime,     0.0f},
      {in_fDelay * 2 + in_fAnimationTime, 0.0f},
   };

   m_fAnimLength = in_fDelay + in_fAnimationTime;

   CGFX_Action_Itf* l_pAction = g_Joshua.Renderer()->Create_Action();
   m_pLookSwapTrack  = g_Joshua.Renderer()->Create_Track();

   CGFX_Animation_Linear_Itf* l_pAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pAnim->Setup_Keys(l_pAnimKeys,4);
   l_pAnim->Connect(m_pObjRealisticFrm->Model()->Get_Primitive(),GFX::PARAM_COLOR_A);

   l_pAction->Length(m_fAnimLength);
   l_pAction->Add_Animation(l_pAnim);

   m_pLookSwapTrack->Set_Action(l_pAction);
   m_pLookSwapTrack->ExtrapolationBefore(true);
   m_pLookSwapTrack->ExtrapolationAfter(true);
   m_pLookSwapTrack->Playback_Count(1);
   m_pLookSwapTrack->Playback(GFX::PLAYBACK_FORWARD);
   m_pLookSwapTrack->Update_Animations(0);

   SAFE_RELEASE(l_pAnim);
   SAFE_RELEASE(l_pAction);

}

bool GMiniMap::OnDestroy()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : " + this->Name());

   __super::OnDestroy();

   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(m_pLookSwapTrack);
   l_pScene->Get_Animation_System()->Remove_Track(m_pSlidingTrack);

   SAFE_RELEASE(m_pLookSwapTrack);
   SAFE_RELEASE(m_pSlidingTrack);

   return true;
}


CGFX_Track_Itf* & GMiniMap::AnimationSlidingTrack()
{
   return m_pSlidingTrack;
}

GUI::EEventStatus::Enum GMiniMap::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if( ( in_EventData.X() >= 0 && in_EventData.X() <= 144 && in_EventData.Y() >= 0 && in_EventData.Y() <= 31 ) ||
       ( in_EventData.X() >= 0 && in_EventData.X() <= 26 && in_EventData.Y() >= 31 && in_EventData.Y() <= 47 ) )
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjMap || in_pCaller == m_pObjCrosshair)
         {         
            SEarthState l_State;

            MiniMapToEarth(m_pObjMap->AbsoluteToRelative(RelativeToAbsolute(in_EventData.Mouse.Position)), l_State);
            g_ClientDCL.UpdateWorldPosition(l_State);
         }

         if(in_pCaller == m_pObjHidingZoom)
         {
            g_ClientDCL.m_EarthState.m_Zoom = ((REAL32)m_pObjZoomBar->Height() - (REAL32)in_EventData.Y() + in_pCaller->Top()) / (REAL32)m_pObjZoomBar->Height();
            UpdateZoomBar();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMiniMap::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if( ( in_EventData.X() >= 0 && in_EventData.X() <= 144 && in_EventData.Y() >= 0 && in_EventData.Y() <= 31 ) ||
       ( in_EventData.X() >= 0 && in_EventData.X() <= 26 && in_EventData.Y() >= 31 && in_EventData.Y() <= 47 ) )
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjHome)
         {
            INT16 l_iControlledCountryID = g_ClientDAL.ControlledCountryID();
            const GCountry& l_Country = g_ClientDAL.Country(l_iControlledCountryID);
            if(g_ClientDCL.SelectedCountryID()!= l_Country.Id())
            {
               // Play a sound
               g_SP2Client->PlaySound(ESoundEventID::ZoomToCountry);
            }
            g_ClientDCL.SelectedCountrySetLookAt(l_Country);
         }

         if(in_pCaller == m_pObjMagnifier)
         {
            g_ClientDDL.BackgroundEarth()->ToggleUnitDisplayIconsFlags();
         }
      }
   }

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMiniMap::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if( ( in_EventData.X() >= 0 && in_EventData.X() <= 144 && in_EventData.Y() >= 0 && in_EventData.Y() <= 31 ) ||
       ( in_EventData.X() >= 0 && in_EventData.X() <= 26 && in_EventData.Y() >= 31 && in_EventData.Y() <= 47 ) )
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   return OnMouseUp(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GMiniMap::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if( ( in_EventData.X() >= 0 && in_EventData.X() <= 144 && in_EventData.Y() >= 0 && in_EventData.Y() <= 31 ) ||
       ( in_EventData.X() >= 0 && in_EventData.X() <= 26 && in_EventData.Y() >= 31 && in_EventData.Y() <= 47 ) )
   {
      ((GEventData &)in_EventData).InitialRecipient = g_ClientDDL.BackgroundEarth();
      return GUI::EEventStatus::NotHandled;
   }

   if (in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      GVector2D<INT32> l_AbsPos = RelativeToAbsolute(in_EventData.Mouse.Position);

      if(g_Joshua.GUIManager()->MouseDownTarget() == m_pObjMap || g_Joshua.GUIManager()->MouseDownTarget() == m_pObjCrosshair)
      {
         GVector2D<INT32> l_Pos = m_pObjMap->AbsoluteToRelative(l_AbsPos);      
         
         if(l_Pos.x < 0)
            l_Pos.x = 0;
         else if(l_Pos.x > m_pObjMap->Width())
            l_Pos.x = m_pObjMap->Width();

         if(l_Pos.y < 0)
            l_Pos.y = 0;
         else if(l_Pos.y > m_pObjMap->Height())
            l_Pos.y = m_pObjMap->Height();

         SEarthState l_State;
         MiniMapToEarth(l_Pos, l_State);
         g_ClientDCL.UpdateWorldPosition(l_State);
      }
      else if(g_Joshua.GUIManager()->MouseDownTarget() == m_pObjHidingZoom)
      {
         GVector2D<INT32> l_Pos = m_pObjZoomBar->AbsoluteToRelative(l_AbsPos);

         if(l_Pos.y < 0)
            l_Pos.y = 0;
         else if(l_Pos.y > m_pObjZoomBar->Height())
            l_Pos.y = m_pObjZoomBar->Height();
         
         g_ClientDCL.m_EarthState.m_Zoom = 1 - (REAL32)l_Pos.y / (REAL32)m_pObjZoomBar->Height();
         UpdateZoomBar();
      }
   }

   return GUI::EEventStatus::Handled;
}


bool GMiniMap::UpdateCrosshair(const SEarthState & in_State)
{
   INT32 l_iPosX,
         l_iPosY,
         l_iLongitude,
         l_iLatitude;

   l_iLongitude = (INT32)((in_State.m_Longitude + Math::PI) * 180.f / Math::PI);
   l_iLatitude  = (INT32)((-in_State.m_Latitude + Math::PI / 2) * 180.f  / Math::PI);

   while(l_iLongitude < 0)
      l_iLongitude = l_iLongitude + 360;

   while(l_iLatitude  < 0)
      l_iLatitude  = l_iLatitude  + 180;

   l_iLongitude = l_iLongitude % 360;
   l_iLatitude  = l_iLatitude  % 180;

   l_iPosX = (l_iLongitude / 2) + (INT32)m_pObjMap->Left();
   l_iPosY = (l_iLatitude  / 2) + (INT32)m_pObjMap->Top();
   
   // -3 Stands to get the middle of the crosshair
   m_pObjCrosshair->Position(l_iPosX - 3, l_iPosY - 3);
   
   m_pObjLongitudeBar->Left(l_iPosX);
   m_pObjLatitudeBar->Top(l_iPosY);

   return true;
}

void GMiniMap::MiniMapToEarth(GVector2D<INT32> in_Position, SEarthState & in_State)
{
   in_Position.x = min(179, in_Position.x);
   in_Position.x = max(  0, in_Position.x);
   in_Position.y = min( 89, in_Position.y);
   in_Position.y = max(  0, in_Position.y);
   in_State.m_Longitude = (REAL32)(((((REAL32)in_Position.x * 2.f) - 180.f) / 180.f) * Math::PI);
   in_State.m_Latitude  = (REAL32)-(((((REAL32)in_Position.y * 2.f) - 90.f ) / 90.f ) * (Math::PI / 2.f));
   in_State.m_Zoom      = g_ClientDCL.m_EarthState.m_Zoom;
   in_State.m_Action    = SEarthState::NONE;
   
   return;
}

GUI::EEventStatus::Enum GMiniMap::OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
	if(!in_pCaller)
	{
      g_ClientDCL.m_EarthState.m_Zoom += m_fZoomInc * in_EventData.Mouse.WheelDelta; 
		UpdateZoomBar();
	}

	return GUI::EEventStatus::Handled;
}

bool GMiniMap::UpdateZoomBar()
{
   g_ClientDCL.m_EarthState.m_Zoom = min(1, g_ClientDCL.m_EarthState.m_Zoom);
   g_ClientDCL.m_EarthState.m_Zoom = max(0, g_ClientDCL.m_EarthState.m_Zoom);

   m_pObjHidingButton->Height((INT16)((REAL32)m_pObjZoomBar->Height() * (1 - g_ClientDCL.m_EarthState.m_Zoom)));

   g_ClientDCL.UpdateWorldPosition();
   
   return true;
}

void GMiniMap::ToggleMapLook(bool in_bNormalLook = true)
{
   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(m_pLookSwapTrack);

   if(in_bNormalLook)
      m_pLookSwapTrack->Playback(GFX::PLAYBACK_REVERSE);
   else
      m_pLookSwapTrack->Playback(GFX::PLAYBACK_FORWARD);

   m_pLookSwapTrack->Set_Length(m_fAnimLength);
   m_pLookSwapTrack->Set_Start_Time(l_pScene->Get_Animation_System()->Get_Animation_Time() );
   l_pScene->Get_Animation_System()->Add_Track(m_pLookSwapTrack);
}

void GMiniMap::ToggleMagnifier(bool in_nToggle)
{
   if(m_pObjMagnifier)
   {
      m_pObjMagnifier->ToggleState(in_nToggle);
   }
}
