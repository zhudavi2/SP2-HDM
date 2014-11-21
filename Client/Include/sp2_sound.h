/**************************************************************
*
* sp2_sound.h
*
* Description
* ===========
* Implement sound interface
*
* Owner
* =====
*  Jonathan Mecier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Inlcude Guars
#ifndef _CSP2_SOUND_H_
#define _CSP2_SOUND_H_

// Foward Declaration
class CSDX_Context;

namespace SP2
{
   namespace ESoundContext
   {
      enum Enum
      {
         Left =0,
         Center,
         Right,
         Total
      };
   }
   namespace ESoundEventID
   {
      enum Enum
      {
         //---------Interface-----------
         ConfirmOrder = 0,
         IncomingChat1,
         Orders1,
         OrderMove1,
         OrderAttack1,
         OrderDefend1,
         CombatWin1,
         CombatLost1,
         IncomingMail,
         AdvisorWarning,
         InvalidAction,
         MouseClick,
         ZoomToCountry,

         //---------Strategic-----------
         LaunchNuclearAttack1,
         NuclearImpact1,
         AMDSInterecptionLaunch,
         NuclearWarning,
         AMDSInterecptionSucess,
         AMDSInterecptionFail,
         //---------Cobat-----------
         CombatFirstElement,
         CombatInfantryFireA = CombatFirstElement,
         CombatInfantryFireB,
         CombatInfantryFireC,
         CombatInfantryDyingA,
         CombatInfantryDyingB,
         CombatInfantryVehicleFireA,
         CombatInfantryVehicleFireB,
         CombatInfantryVehicleFireC,
         CombatTankFireA,
         CombatTankFireB,
         CombatTankFireC,
         CombatMissileFireA,
         CombatMissileFireB,
         CombatMissileFireC,
         CombatMissileFireD,
         CombatMissileFireE,
         CombatMissileFireF,
         CombatTorpedoFireA,
         CombatTorpedoFireB,
         CombatTorpedoHitA,
         CombatMotorizedGroundUnitA,
         CombatMotorizedGroundUnitB,
         CombatMotorizedGroundUnitC,
         CombatHelicopterA,
         CombatHelicopterB,
         CombatHelicopterC,
         CombatAircraftA,
         CombatAircraftB,
         CombatAircraftC,
         CombatAircraftD,
         CombatShipA,
         CombatShipB,
         CombatAircraftCarrierTakeoffA,
         CombatExplosionA,
         CombatExplosionB,
         CombatExplosionC,
         CombatExplosionD,
         CombatExplosionE,
         CombatExplosionF,
         MaxEventID// NOTE : This ID should always be the last one.
      };
   }

   class GSP2Sound
   {
   private:

   public:

      GSP2Sound();
      ~GSP2Sound();

      void CreateSoundTable(void);
      void LoadSounds(void);
      void PlaySound(ESoundEventID::Enum in_eSoundID);
      void PlaySoundWithDelay(ESoundEventID::Enum in_eSoundID,REAL32 in_fDelayInSecond, ESoundContext::Enum in_soundContext);
      REAL32 GetTimeForDelayedSound(ESoundEventID::Enum in_eSoundID,ESoundContext::Enum in_soundContext);

       //! Get sounds
      CSFX_Sound_Itf* GetSound(ESoundEventID::Enum in_eSoundID);

      //! Get context
      CSFX_Context_Itf* SoundContext(ESoundContext::Enum in_eContext){return (CSFX_Context_Itf *) m_vSoundContext[in_eContext];};
   
   private:
      // Structure that contain the information for a sound
      struct SoundInformation
      {
         CSFX_Sound_Itf*   m_Sound;                       // Sound itself
         GString           m_SoundFileName;               // Sound filename
      };

      SoundInformation m_vSoundTable[ESoundEventID::MaxEventID];

      // Sound Context to play sounds
      CSDX_Context*  m_vSoundContext[ESoundContext::Total];


      bool m_bMusicPlaying;
   };
}
#endif // _CSP2_SOUND_H_