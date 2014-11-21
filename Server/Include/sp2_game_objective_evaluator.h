/**************************************************************
*
* golem_gameobjectiveevaluator.h
*
* Description
* ===========
*  Describes the interface of class GGameObjectiveEvaluator
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GAMEOBJECTIVEEVALUATOR_H_
#define _GOLEM_GAMEOBJECTIVEEVALUATOR_H_

namespace SP2
{
   class GWorldBehavior;

   /*!
   * Evaluate game objectives
   **/
   class GGameObjectiveEvaluator : public Hector::DesignPattern::GObserver , public SDK::GGameDataNode
   {
   public:
      GGameObjectiveEvaluator();
      ~GGameObjectiveEvaluator();

      //! Evaluates the objectives for the given player
      bool EvaluateObjectives();
     
      //! Observer received a notification
      void OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                           const Hector::DesignPattern::GSubjectNotification& in_Notification,
                           void* in_pAdditionalMsg);

   protected:
   private:
      void EvaluateObjectivesForPlayer(SDK::GPlayer* in_pPlayer,
                                       bool& out_vAllObjectivesMetSuccessfully,
                                       vector<GGameObjective*>& out_vCompletedObjectives,
                                       vector<GGameObjective*>& out_vTimedOutObjectives,
                                       vector<GGameObjective*>& out_vGameOverObjectives);

       void EvaluateSteamAchievementsForPlayer(SDK::GPlayer* in_pPlayer);

      //EObjectiveEvaluatorState::Enum m_eStatus;


      //! Creates a copy of all the objectives contained in in_vObjectives and adds them to out_vObjectives
      void CopyGameObjectives(vector<GGameObjective*>& in_vObjectives,vector<GGameObjective>& out_vObjectives);

      EObjectiveStatus::Enum EvalRaiseGDPPerCapita(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalConquerTheWorld(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalEconomicFailure(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalBeAnnexed(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalLoseElections(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalBeTargetOfSuccessfulCoupEtat(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalAchieveWorldPeace(SDK::GPlayer* in_Player, GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalDevelopNation(SDK::GPlayer* in_Player, GGameObjective& in_Objective);

      // Scenarios
      EObjectiveStatus::Enum EvalStabilizeCountry(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalMeetRessourcesNeed(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalInvasion(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalBeInATreaty(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalRaiseEconomicHealth(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalRessourceControl(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalAnnexCountry(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalGovernmentType(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalPureCommunistEonomy(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalBuildUnitCategory(SDK::GPlayer& in_Player,GGameObjective& in_Objective);
      EObjectiveStatus::Enum EvalLaunchANuke(SDK::GPlayer& in_pPlayer,GGameObjective& in_Objective);      

      vector<UINT32>                m_vCountryWherePoliticalPartyChanged;
      vector<UINT32>                m_vCountryTargetOfSuccessfulCoupEtat;
      vector<pair<UINT32,UINT32> >  m_vCountryThatLaunchedNukes; //!<pair key = attacker country id, pair value = target country id
      bool                          m_bWorldPeace;

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();
   };
};
#endif //_GOLEM_GAMEOBJECTIVEEVALUATOR_H_
