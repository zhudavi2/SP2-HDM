/**************************************************************
*
* sp2_strategic_launch_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_strategic_launch_window.h"

#include "../include/sp2_strategic_target_country_window.h"
#include "../include/sp2_strategic_target_multiple_countries_window.h"
#include "../include/sp2_sound.h"

const GString GStrategicLaunchWindow::TypeName(L"Strategic_Launch_Window");

GUI::GBaseObject* SP2::GStrategicLaunchWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicLaunchWindow();
}


GStrategicLaunchWindow::GStrategicLaunchWindow() : GFrame()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Strategic Launch Window");

   m_bFromTargetCountryWindow = false;
   m_pOnFlashAnim = NULL;
   m_pOnDowmAnim  = NULL;
   m_pOnShowAnim  = NULL;
}

GStrategicLaunchWindow::~GStrategicLaunchWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Strategic Launch Window");
}

bool GStrategicLaunchWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjLaunchFrm = (GUI::GFrame*) (Child(L"frmButton")->Child(L"frmLaunch") );
   m_pObjAbortBtn  = (GUI::GButton*)Child(L"btnAbort");

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(0,(INT16)mode.m_Resolution.y - Height());

   m_pObjLaunchFrm->PictureBackground(g_ClientDDL.LaunchSequence() );

   if(g_ClientDDL.LaunchSequence() )
   {
      m_pOnShowAnim = g_Joshua.Renderer()->Create_Track();
      m_pOnDowmAnim = g_Joshua.Renderer()->Create_Track();
      m_pOnFlashAnim = g_Joshua.Renderer()->Create_Track();

      AnimateSequence(m_pOnShowAnim, 0, 46);
      AnimateSequence(m_pOnDowmAnim, 46, 60);
      AnimateSequence(m_pOnFlashAnim, 60, 74);

      m_pOnFlashAnim->Playback_Count(10);
   }

	return true;
}

bool GStrategicLaunchWindow::OnDestroy()
{
   SAFE_RELEASE(m_pOnShowAnim);
   SAFE_RELEASE(m_pOnDowmAnim);
   SAFE_RELEASE(m_pOnFlashAnim);

   return __super::OnDestroy();
}

void GStrategicLaunchWindow::OnShow()
{
   m_pObjAbortBtn->Enabled(true);
   m_bPlaying = false;

   m_pOnShowAnim->Set_Start_Time(g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time() );
   m_fDownMinimumTime = m_pOnDowmAnim->Get_Stop_Time();
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_pOnShowAnim);

   __super::OnShow();
}

void GStrategicLaunchWindow::OnHide()
{
   if(!m_bFromTargetCountryWindow)
      g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Hide();

   g_ClientDDL.StrategicTargetCountryWindow()->Show();
   g_ClientDDL.StrategicTargetCountryWindow()->Initialize();

   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pOnShowAnim);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pOnDowmAnim);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pOnFlashAnim);

   m_bPlaying = false;

   __super::OnHide();
}

GUI::EEventStatus::Enum GStrategicLaunchWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(!m_bPlaying)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(!in_pCaller)
         {
            if(in_EventData.InitialRecipient == m_pObjLaunchFrm)
            {
               SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GStrategicWarfareLaunch); 
               SP2::Event::GStrategicWarfareLaunch* l_pLaunchEvent = (SP2::Event::GStrategicWarfareLaunch*)l_pEvent.get();

               l_pLaunchEvent->m_iSource = g_SP2Client->Id();
               l_pLaunchEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
               
               l_pLaunchEvent->m_vNukeLaunchRequests = m_NukeRequests;

               g_Joshua.RaiseEvent(l_pEvent);

               // Play a sound
               g_SP2Client->PlaySoundWithDelay(ESoundEventID::LaunchNuclearAttack1,c_fNukeLaunchDelaySeconds,ESoundContext::Center);

               // Update launch button animation
               m_pOnDowmAnim->Set_Start_Time(max(m_fDownMinimumTime, g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time() ) );
               g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_pOnDowmAnim);

               m_pOnFlashAnim->Set_Start_Time(m_pOnDowmAnim->Get_Stop_Time() );
               g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_pOnFlashAnim);

               m_bPlaying = true;

               m_pObjAbortBtn->Enabled(false);

               //nuke something
               g_Joshua.PassSteamAchievement(1);
            }
         }
      }
   }
   else
   {
      // Hide window when clicked and animation is playing
      Hide();
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicLaunchWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(!m_bPlaying)
   {
      if(in_pCaller)
      {
         if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
         {
            //On abort button
            if(in_pCaller == m_pObjAbortBtn)
            {
               //check which window was shown before (target country or target multiple countries)
               if(m_bFromTargetCountryWindow)
               {
                  g_ClientDDL.StrategicTargetCountryWindow()->Show();
                  g_ClientDDL.StrategicLaunchWindow()->Hide();
               }
               else
               {
                  g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Show();
                  g_ClientDDL.StrategicLaunchWindow()->Hide();
               }
            }
         }
      }
   }
   
   return GUI::EEventStatus::Handled;
}

void GStrategicLaunchWindow::Initialize(bool in_bFromTargetSingleCountry)
{
   m_NukeRequests.clear();

   m_bFromTargetCountryWindow = in_bFromTargetSingleCountry;
   

   //Get the full target list and the missiles used list
   vector< pair<GNukeTarget, vector<GNuclearMissile> > > * l_pTargetList = NULL;
   vector<GNuclearMissile> * l_pMissilesUsed = NULL;
   INT32 l_iCount = 1;

   //If from country window and a specific target
   if(m_bFromTargetCountryWindow && g_ClientDDL.StrategicTargetCountryWindow()->m_bSpecificTargetMode)
   {
      multimap<UINT32,GNuclearMissile>::iterator l_MissileToUse = g_ClientDDL.StrategicTargetCountryWindow()->m_SpecificTargetMissilesByRange.begin();
      gassert(g_ClientDDL.StrategicTargetCountryWindow()->m_SpecificTargetMissilesByRange.size() >= g_ClientDDL.StrategicTargetCountryWindow()->m_iNbMissileUsed,"List of missiles in range not long enough");
      for(UINT32 i = 0 ; i < g_ClientDDL.StrategicTargetCountryWindow()->m_iNbMissileUsed ; i++)
      {
         GNukeTarget* l_pTarget = &g_ClientDDL.StrategicTargetCountryWindow()->m_SpecificTarget;
         GNukeLaunchRequest l_Request;
         l_Request.m_iAttackerID       = g_ClientDAL.ControlledCountryID();
         l_Request.m_TargetType        = l_pTarget->m_TargetType;         
         l_Request.m_iMissileID        = l_MissileToUse->second.m_iID;
         l_Request.m_iTargetID         = g_ClientDDL.StrategicTargetCountryWindow()->m_iTargetCountryId;
         l_Request.m_TargetCoordinates = l_pTarget->m_Position;
         m_NukeRequests.push_back(l_Request);

         l_MissileToUse++;
      }
      return;
   }
   else if(in_bFromTargetSingleCountry)
   {
      l_pTargetList = &(g_ClientDDL.StrategicTargetCountryWindow()->m_NewTargetsMissilesList);
      l_pMissilesUsed = &(g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed);
   }
   else
   {
      l_iCount = (INT32)g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_NewTargetsMissilesList.size();
   }

   for(INT32 c=0; c<l_iCount; c++)
   {
      if(!in_bFromTargetSingleCountry)
      {
         l_pTargetList = &(g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_NewTargetsMissilesList[c]);
         l_pMissilesUsed = &(g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesUsed[c]);
      }

      //For each target
      for(INT32 i=0; i<(INT32)l_pTargetList->size(); i++)
      {
         //Get it's list of missile in range
         vector<GNuclearMissile>* l_pMissileList = &(l_pTargetList->at(i).second);
         
         //For each missile in range
         for(INT32 j=0; j<(INT32)l_pMissileList->size(); j++)
         {
            //Check if the missile in range is a missile used
            for(INT32 k=0; k<(INT32)l_pMissilesUsed->size(); k++)
            {
               //if the missile is used add a launch request
               if(l_pMissilesUsed->at(k).m_iID == l_pMissileList->at(j).m_iID)
               {
                  const GNukeTarget & l_pTarget = l_pTargetList->at(i).first;
                  
                  GNukeLaunchRequest l_Request;
                  l_Request.m_iAttackerID = g_ClientDAL.ControlledCountryID();
                  l_Request.m_TargetType  = l_pTarget.m_TargetType;
                  l_Request.m_iMissileID  = l_pMissileList->at(j).m_iID;
                  if(in_bFromTargetSingleCountry)
                     l_Request.m_iTargetID   = g_ClientDDL.StrategicTargetCountryWindow()->m_iTargetCountryId;
                  else
                     l_Request.m_iTargetID   = g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_iTargetCountryId[c];
                  l_Request.m_TargetCoordinates = l_pTarget.m_Position;

                  m_NukeRequests.push_back(l_Request);
               }
            }
         }
      }
   }
}

void GStrategicLaunchWindow::AnimateSequence(CGFX_Track_Itf* in_pTrack, UINT32 in_iStart, UINT32 in_iEnd)
{
   CGFX_Animation_Linear_Itf* l_pAnim = g_Joshua.Renderer()->Create_Animation_Linear();

   CGFX_Key_Linear l_Keys[2] =
   {
      { 0.f, (REAL32) in_iStart},
      { (REAL32) (in_iEnd - in_iStart) / 15.f, (REAL32) in_iEnd},
   };

   l_pAnim->Setup_Keys(l_Keys, 2);
   l_pAnim->Connect(g_ClientDDL.LaunchSequence()->Controller(), GFX::PARAM_FRAME);

   CGFX_Action_Itf* l_pAction = g_Joshua.Renderer()->Create_Action();
   l_pAction->Add_Animation(l_pAnim);
   l_pAction->Length(l_Keys[1].m_Time);

   in_pTrack->Set_Action(l_pAction);

   SAFE_RELEASE(l_pAnim);
   SAFE_RELEASE(l_pAction);
}

void GStrategicLaunchWindow::Iterate()
{
   if(m_bPlaying)
   {
      if(g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time() > 
         m_pOnFlashAnim->Get_Stop_Time() )
      {
         Hide();
      }
   }
}
