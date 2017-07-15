/**************************************************************
*
* sp2_sound.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

// Includes files
#include "../include/sp2_sound.h"
#include "../../includes/sfx/GingerSoundDX/SDX_Context.h"

// Constant
const GString c_stringInterfaceSoundDirectory("Data\\Sounds\\Interface\\");
const GString c_stringCombatSoundDirectory("Data\\Sounds\\Combat\\");
const GString c_stringStrategicWarfareSoundDirectory("Data\\Sounds\\Strategic\\");

GSP2Sound::GSP2Sound()
{

   // Create a new sound context to add and play sounds
   m_vSoundContext[ESoundContext::Left] = (CSDX_Context*)g_Joshua.SoundPlayer()->Get_New_Context();
   m_vSoundContext[ESoundContext::Center] = (CSDX_Context*)g_Joshua.SoundPlayer()->Get_New_Context();
   m_vSoundContext[ESoundContext::Right] = (CSDX_Context*)g_Joshua.SoundPlayer()->Get_New_Context();

   // Empty sound Table
   for(int i=0;i<ESoundEventID::MaxEventID;i++)
   {
      m_vSoundTable[i].m_SoundFileName == "";
      m_vSoundTable[i].m_Sound = NULL;
   }

   m_bMusicPlaying = false;
}

GSP2Sound::~GSP2Sound()
{
   // Delete sound table
   for(int i=0;i<ESoundEventID::MaxEventID;i++)
   {
      if(m_vSoundTable[i].m_Sound)
         delete m_vSoundTable[i].m_Sound;
   }

   // JMERCIER : Leak context
   //gassert(NULL,"To implement, we are leaking content");
}

void GSP2Sound::CreateSoundTable(void)
{
   // Interface 
   m_vSoundTable[ESoundEventID::InvalidAction].m_SoundFileName = c_stringInterfaceSoundDirectory + "invalid_action_a.wav";
   m_vSoundTable[ESoundEventID::AdvisorWarning].m_SoundFileName= c_stringInterfaceSoundDirectory + "advisor_warning_a.wav";
   m_vSoundTable[ESoundEventID::ConfirmOrder].m_SoundFileName  = c_stringInterfaceSoundDirectory + "confirm_order_a.wav";
   m_vSoundTable[ESoundEventID::IncomingChat1].m_SoundFileName = c_stringInterfaceSoundDirectory + "incoming_chat_a.wav";
   m_vSoundTable[ESoundEventID::OrderMove1].m_SoundFileName    = c_stringInterfaceSoundDirectory + "military_move_a.wav";
   m_vSoundTable[ESoundEventID::Orders1].m_SoundFileName       = c_stringInterfaceSoundDirectory + "military_order_a.wav";
   m_vSoundTable[ESoundEventID::IncomingMail].m_SoundFileName  = c_stringInterfaceSoundDirectory + "incomig_mail_a.wav";
   m_vSoundTable[ESoundEventID::ZoomToCountry].m_SoundFileName = c_stringInterfaceSoundDirectory + "move_globe_medium_a.wav";
   m_vSoundTable[ESoundEventID::MouseClick].m_SoundFileName    = c_stringInterfaceSoundDirectory + "mouse_click_a.wav";

   // Combat interface
   m_vSoundTable[ESoundEventID::CombatWin1].m_SoundFileName    = c_stringCombatSoundDirectory + "Combat_won_a.wav";
   m_vSoundTable[ESoundEventID::CombatLost1].m_SoundFileName   = c_stringCombatSoundDirectory + "Combat_lost_a.wav";
   m_vSoundTable[ESoundEventID::OrderAttack1].m_SoundFileName  = c_stringCombatSoundDirectory + "Order_Attack1.wav";
   m_vSoundTable[ESoundEventID::OrderDefend1].m_SoundFileName  = c_stringCombatSoundDirectory + "Order_Defend1.wav";
   


   // Strategic
   m_vSoundTable[ESoundEventID::LaunchNuclearAttack1].m_SoundFileName   = c_stringStrategicWarfareSoundDirectory + "nuclear_launch_a.wav";
   m_vSoundTable[ESoundEventID::NuclearImpact1].m_SoundFileName         = c_stringStrategicWarfareSoundDirectory + "nuclear_strike_a.wav";
   m_vSoundTable[ESoundEventID::AMDSInterecptionLaunch].m_SoundFileName = c_stringStrategicWarfareSoundDirectory + "launch_interception_a.wav";
   m_vSoundTable[ESoundEventID::NuclearWarning].m_SoundFileName         = c_stringStrategicWarfareSoundDirectory + "nuclear_launch_warning_a.wav";
   m_vSoundTable[ESoundEventID::AMDSInterecptionFail].m_SoundFileName   = c_stringStrategicWarfareSoundDirectory + "failed_interception_a.wav";
   m_vSoundTable[ESoundEventID::AMDSInterecptionSucess].m_SoundFileName = c_stringStrategicWarfareSoundDirectory + "succesful_interception_a.wav";
   
   
   // Combat Event
   m_vSoundTable[ESoundEventID::CombatInfantryFireA].m_SoundFileName          = c_stringCombatSoundDirectory + "infantry_fire_a.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryFireB].m_SoundFileName          = c_stringCombatSoundDirectory + "infantry_fire_b.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryFireC].m_SoundFileName          = c_stringCombatSoundDirectory + "infantry_fire_c.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryDyingA].m_SoundFileName         = c_stringCombatSoundDirectory + "infantry_dying_a.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryDyingB].m_SoundFileName         = c_stringCombatSoundDirectory + "infantry_dying_b.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryVehicleFireA].m_SoundFileName   = c_stringCombatSoundDirectory + "infantry_vehicle_fire_a.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryVehicleFireB].m_SoundFileName   = c_stringCombatSoundDirectory + "infantry_vehicle_fire_b.wav";
   m_vSoundTable[ESoundEventID::CombatInfantryVehicleFireC].m_SoundFileName   = c_stringCombatSoundDirectory + "infantry_vehicle_fire_c.wav";
   m_vSoundTable[ESoundEventID::CombatTankFireA].m_SoundFileName              = c_stringCombatSoundDirectory + "tank_fire_a.wav";
   m_vSoundTable[ESoundEventID::CombatTankFireB].m_SoundFileName              = c_stringCombatSoundDirectory + "tank_fire_b.wav";
   m_vSoundTable[ESoundEventID::CombatTankFireC].m_SoundFileName              = c_stringCombatSoundDirectory + "tank_fire_c.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireA].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_a.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireB].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_b.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireC].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_c.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireD].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_d.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireE].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_e.wav";
   m_vSoundTable[ESoundEventID::CombatMissileFireF].m_SoundFileName           = c_stringCombatSoundDirectory + "missile_fire_f.wav";
   m_vSoundTable[ESoundEventID::CombatTorpedoFireA].m_SoundFileName           = c_stringCombatSoundDirectory + "torpedo_fire_a.wav";
   m_vSoundTable[ESoundEventID::CombatTorpedoFireB].m_SoundFileName           = c_stringCombatSoundDirectory + "torpedo_fire_b.wav";
   m_vSoundTable[ESoundEventID::CombatTorpedoHitA].m_SoundFileName            = c_stringCombatSoundDirectory + "torpedo_hit_a.wav";
   m_vSoundTable[ESoundEventID::CombatMotorizedGroundUnitA].m_SoundFileName   = c_stringCombatSoundDirectory + "ground_unit_a.wav";
   m_vSoundTable[ESoundEventID::CombatMotorizedGroundUnitB].m_SoundFileName   = c_stringCombatSoundDirectory + "ground_unit_b.wav";
   m_vSoundTable[ESoundEventID::CombatMotorizedGroundUnitC].m_SoundFileName   = c_stringCombatSoundDirectory + "ground_unit_c.wav";
   m_vSoundTable[ESoundEventID::CombatHelicopterA].m_SoundFileName            = c_stringCombatSoundDirectory + "helicopter_passby_a.wav";
   m_vSoundTable[ESoundEventID::CombatHelicopterB].m_SoundFileName            = c_stringCombatSoundDirectory + "helicopter_passby_b.wav";
   m_vSoundTable[ESoundEventID::CombatHelicopterC].m_SoundFileName            = c_stringCombatSoundDirectory + "helicopter_passby_c.wav";
   m_vSoundTable[ESoundEventID::CombatAircraftA].m_SoundFileName              = c_stringCombatSoundDirectory + "aircraft_flyby_a.wav";
   m_vSoundTable[ESoundEventID::CombatAircraftB].m_SoundFileName              = c_stringCombatSoundDirectory + "aircraft_flyby_b.wav";
   m_vSoundTable[ESoundEventID::CombatAircraftC].m_SoundFileName              = c_stringCombatSoundDirectory + "aircraft_flyby_c.wav";
   m_vSoundTable[ESoundEventID::CombatAircraftD].m_SoundFileName              = c_stringCombatSoundDirectory + "aircraft_flyby_d.wav";
   m_vSoundTable[ESoundEventID::CombatShipA].m_SoundFileName                  = c_stringCombatSoundDirectory + "ship_loop_a.wav";
   m_vSoundTable[ESoundEventID::CombatShipB].m_SoundFileName                  = c_stringCombatSoundDirectory + "ship_loop_b.wav";
   m_vSoundTable[ESoundEventID::CombatAircraftCarrierTakeoffA].m_SoundFileName= c_stringCombatSoundDirectory + "aircraft_carrier_takeoff_a.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionA].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_a.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionB].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_b.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionC].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_c.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionD].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_d.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionE].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_e.wav";
   m_vSoundTable[ESoundEventID::CombatExplosionF].m_SoundFileName             = c_stringCombatSoundDirectory + "explosion_f.wav";
}


void GSP2Sound::LoadSounds(void)
{
   for(int i=0;i<ESoundEventID::CombatFirstElement;i++)
   {
      if(m_vSoundTable[i].m_SoundFileName != L"")
      {

         // Create the new sound.
         CSFX_Sound_Itf *l_newSound = g_Joshua.SoundPlayer()->Get_New_Sound();
         gassert(l_newSound,"New sound pointer is null");

         // Load from file
         GFile l_fileSound;
         bool l_fileFound = g_Joshua.FileManager()->File(m_vSoundTable[i].m_SoundFileName,l_fileSound);
         gassert(l_fileFound == true,"File not found for a sound");
         bool l_soundLoadedWithSucess = l_newSound->Load_From_File(l_fileSound);
         gassert(l_soundLoadedWithSucess == true,"Can't load a sound from the sound table");

         // Add sound to context
         bool l_soundAddedToContext = m_vSoundContext[ESoundContext::Center]->Add_Sound(l_newSound);
         gassert(l_soundAddedToContext == true,"Context didn't add the sound");

         // Add it to the sound table
         m_vSoundTable[i].m_Sound = l_newSound;
      }
   }

   // Set Center context
  m_vSoundContext[ESoundContext::Center]->Set_Panning(0.5f);
   
   for(int i=ESoundEventID::CombatFirstElement;i<ESoundEventID::MaxEventID;i++)
   {
      if(m_vSoundTable[i].m_SoundFileName != L"")
      {

         // Create the new sound.
         CSFX_Sound_Itf *l_newSound = g_Joshua.SoundPlayer()->Get_New_Sound();
         gassert(l_newSound,"New sound pointer is null");

         // Load from file
         GFile l_fileSound;
         bool l_fileFound = g_Joshua.FileManager()->File(m_vSoundTable[i].m_SoundFileName,l_fileSound);
         gassert(l_fileFound == true,"File not found for a sound");
         bool l_soundLoadedWithSucess = l_newSound->Load_From_File(l_fileSound);
         gassert(l_soundLoadedWithSucess == true,"Can't load a sound from the sound table");

         // Add sound to context
         // Create the Ally sound
         bool l_soundAddedToContext = m_vSoundContext[ESoundContext::Left]->Add_Sound(l_newSound);
         gassert(l_soundAddedToContext == true,"Context didn't add the sound");

         l_soundAddedToContext = m_vSoundContext[ESoundContext::Right]->Add_Sound(l_newSound);
         gassert(l_soundAddedToContext == true,"Context didn't add the sound");

         // Add it to the sound table
         m_vSoundTable[i].m_Sound = l_newSound;
      }
   }

   // Set Left and Right contect
   m_vSoundContext[ESoundContext::Right]->Set_Panning(.90f);
   m_vSoundContext[ESoundContext::Left]->Set_Panning(0.10f);

}


void GSP2Sound::PlaySound(ESoundEventID::Enum in_eSoundID)
{
   gassert(in_eSoundID < ESoundEventID::MaxEventID,"Sound event ID is invalid");

   if(m_vSoundTable)
   {
      CSFX_Sound_Itf *l_newSound =  m_vSoundTable[in_eSoundID].m_Sound;
      gassert(l_newSound,"Sound wasnt loaded");

      bool sound_played = m_vSoundContext[ESoundContext::Center]->Play_Sound(l_newSound);
      gassert(sound_played == true,"Sound could not be played");
   }
}

REAL32 GSP2Sound::GetTimeForDelayedSound(ESoundEventID::Enum in_eSoundID,ESoundContext::Enum in_soundContext)
{
   if(m_vSoundTable)
   {
      CSFX_Sound_Itf *l_pSound =  m_vSoundTable[in_eSoundID].m_Sound;
      return g_Joshua.SoundPlayer()->GetTimeForDelayedSound(m_vSoundContext[in_soundContext],l_pSound);
   }

   return  -1;
}

void GSP2Sound::PlaySoundWithDelay(ESoundEventID::Enum in_eSoundID,REAL32 in_fDelayInSecond, ESoundContext::Enum in_soundContext)
{
   gassert(in_eSoundID < ESoundEventID::MaxEventID,"Sound event ID is invalid");
   if(m_vSoundTable)
   {
      CSFX_Sound_Itf *l_newSound =  m_vSoundTable[in_eSoundID].m_Sound;
      g_Joshua.SoundPlayer()->PlayDelayedSound(m_vSoundContext[in_soundContext],l_newSound,in_fDelayInSecond);
   }
}


CSFX_Sound_Itf* GSP2Sound::GetSound(ESoundEventID::Enum in_eSoundID)
{
   gassert(in_eSoundID < ESoundEventID::MaxEventID,"This sound id, looks to be invalid");
   return m_vSoundTable[in_eSoundID].m_Sound;
}