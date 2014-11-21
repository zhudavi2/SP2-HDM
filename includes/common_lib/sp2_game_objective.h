/**************************************************************
*
* golem_gameobjective.h
*
* Description
* ===========
*  Describes the interface of class GGameObjective
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GAMEOBJECTIVE_H_
#define _GOLEM_GAMEOBJECTIVE_H_

namespace SP2
{
   namespace EObjectiveType
   {
      enum Enum
      {
         Undefined,
         //Negative objectives that must not be reached
         FirstNegativeObjective,
         LoseElections,
         EconomicFailure,
         BeTargetOfSuccessfulCoupEtat,
         BeAnnexed,
         LastNegativeObjective,
         //Positive objectives that must be reached
         FirstPositiveObjective,
         RaiseGDPPerCapita,
         ConquerTheWorld,
         RaiseStability,
         MeetResourcesNeeds,
         InvadeCountry,
         BeInTreaty,
         RaiseEconomicHealth,
         WorldProduction,
         AnnexCountry,
         ResourceControl,
         PureCommunistEonomy,
         GovernementType,
         BuildUnitCategory,
         LaunchANuke,
         AchieveWorldPeace,
         DevelopNation,
         LastPositiveObjective,
      };
   };

   namespace EObjectiveStatus
   {
      enum Enum
      {
         Reached,
         AlwaysReached,
         NotReached,
         TimedOut
      };
   };
 
  /*!
   * Defines a game objective
   **/
   class GGameObjective : public GSerializable
   {
   public:
      GGameObjective();
      virtual ~GGameObjective();

      EObjectiveType::Enum    m_eObjectiveType;
      EObjectiveStatus::Enum  m_eObjectiveStatus;
      REAL32                  m_fValueToReach;
      REAL64                  m_fTimeOutTime;
      GString                 m_Title;
      UINT32                  m_Id;

      GIOBuffer            m_Buffer;

      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);

      bool IsTimedOut();

      
   protected:
   private:
      static UINT32 m_iIDCounter;
      bool m_bMustFreeMemory;
   };
};
#endif //_GOLEM_GAMEOBJECTIVE_H_
