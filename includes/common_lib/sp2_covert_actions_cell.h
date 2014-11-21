/**************************************************************
*
* golem_covertactioncell.h
*
* Description
* ===========
*  Describes the interface of class GCovertActionCell
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COVERTACTIONCELL_H_
#define _GOLEM_COVERTACTIONCELL_H_

namespace SP2
{

   const REAL32 c_fCovertActionCellCreationTimeDays         = 60.f;
   const REAL32 c_fCovertActionsCellGettingReadyTimeDays    = 28.f;
   const REAL32 c_fCovertActionsCellTrainingOneLevelDays    = 120.f;
   const REAL32 c_fCovertActionsCellInTransitDays           = 40.f;
   const REAL32 c_fCovertActoinsCellPreparingMissionDays    = 60.f;

/*
const REAL32 c_fExperienceNecessaryRegular = 1000.f;
const REAL32 c_fExperienceNecessaryVeteran = 2000.f;
const REAL32 c_fExperienceNecessaryElite = 10000.f;
*/

	const REAL32 c_fMissionComplexityModifierLow = 0.25f;
	const REAL32 c_fMissionComplexityModifierMedium = 1.f;
	const REAL32 c_fMissionComplexityModifierHigh = 2.f;

	const REAL32 c_fMissionComplexitySuccessModifierLow = 2.f;
	const REAL32 c_fMissionComplexitySuccessModifierMedium = 1.f;
	const REAL32 c_fMissionComplexitySuccessModifierHigh = 0.5f;

	const REAL32 c_fMissionTypeEspionageSuccessModifier = 1.f;
	const REAL32 c_fMissionTypeSabotageSuccessModifier = 1.f;
	const REAL32 c_fMissionTypeSocialUpheavalSuccessModifier = 1.f;
	const REAL32 c_fMissionTypeAssassinationSuccessModifier = 1.f;
	const REAL32 c_fMissionTypeTerrorismSuccessModifier = 1.25f;
	const REAL32 c_fMissionTypeCoupEtatSuccessModifier = 0.5f;
	const REAL32 c_fMissionTypeSpecificTargetModifier = 0.75f;

	const REAL32 c_fStabilityChangeSocialUpheaval = -0.15f;
	const REAL32 c_fStabilityChangeAssassination = -0.075f;
	const REAL32 c_fStabilityChangeTerrorism = -0.1f;

	const REAL32 c_fChanceOfCapture = 0.33f;


	//Modifier of knowing who did the action
	const REAL32 c_fMissionTypeEspionageKwnowingModifier = 0.5f;
	const REAL32 c_fMissionTypeSabotageKwnowingModifier = 1.f;
	const REAL32 c_fMissionTypeAssassinationKwnowingModifier = 0.75f;
	const REAL32 c_fMissionTypeTerrorismKwnowingModifier = 1.25f;
	const REAL32 c_fMissionTypeCoupEtatKwnowingModifier = 1.5f;

	const REAL32 c_fRelationBonusCoupEtat = 50.f;
	const REAL32 c_fRelationLossCoupEtat = 100.f;
	const REAL32 c_fRelationLossTerrorism = 80.f;
	const REAL32 c_fRelationLossAssassination = 50.f;
	const REAL32 c_fRelationLossSocialUpheaval = 40.f;
	const REAL32 c_fRelationLossSabotage = 30.f;
	const REAL32 c_fRelationLossEspionage = 20.f;	
	const REAL64 c_fPercentageResourceStolenByEspionnage = 0.02f;
	const REAL64 c_fPercentageResourceLostBySabotage	  = 0.01f;
	const REAL32 c_fSabotageEffectsOnProduction = 2.f;

	const REAL64 c_fCovertActionsCellsDormant = 0.5f;
	const REAL64 c_fCovertActionsCellsRecruit = 0.75f;
	const REAL64 c_fCovertActionsCellsRegular = 1.f;
	const REAL64 c_fCovertActionsCellsVeteran = 4.f;
	const REAL64 c_fCovertActionsCellsElite = 10.f;
	const REAL64 c_fCovertActionsBaseUpkeepFee = 10000000.f;

	const REAL32 c_fCovertActionsCellsRecruitSuccessRate = 0.5f;
	const REAL32 c_fCovertActionsCellsRegularSuccessRate = 1.f;
	const REAL32 c_fCovertActionsCellsVeteranSuccessRate = 1.5f;
	const REAL32 c_fCovertActionsCellsEliteSuccessRate = 2.f;

	const REAL32 c_fAccuracyOfInformationBonusByActiveCell = 0.1f;

	const REAL32 c_fMaximumProtectionCovertActions = 0.95f;
	const REAL32 c_fMaximumSuccessRateAllowed = 0.8f;
	const REAL32 c_fCountryToFrameModifier = 0.25f;

   namespace ECovertActionsMissionType
   {
      enum Enum
      {
         Sabotage,
         SocialUpheaval,
         Assassination,
         Espionage,
         Terrorism,
         CoupEtat,
         Undefined
      };
   };

   namespace ECovertActionsTargetSector
   {
      enum Enum
      {
         Civilian,
         Military,
         Undefined
      };
   };

   namespace ECovertActionsMissionComplexity
   {
      enum Enum
      {
         Low,
         Medium,
         High,
         Undefined
      };
   };

   namespace ECovertActionsCellState
   {
      enum Enum
      {
         InCreation = 1,
         Dormant,
         GettingReady,
         Active,
         Training,
         InTransit,
         PreparingMission,
         ReadyToExecute,
         Undefined,
      };
   };


   namespace ECovertActionCellTraining
   {
      enum Enum
      {
         Recruit =     0,
         Regular =  1000,
         Veteran =  2500,
         Elite   = 10000,
         Unknown =     -1
      };
   };


   //Definition of the different states
   class GStateInCreation : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      GStateInCreation();
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   private:
      REAL64 m_fCreationTimeBeginning;
      REAL64 m_fTimeToCreate;

   };
   //Definition of the different states
   class GStateDormant : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   };
   //Definition of the different states
   class GStateGettingReady : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      GStateGettingReady();
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   private:
      REAL64 m_fTimeBeginning;
      bool   m_bCancel;
   };
   //Definition of the different states
   class GStateActive : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   };
   //Definition of the different states
   class GStateTraining : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      GStateTraining();
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   private:
      REAL64 m_fTimeBeginning;
      REAL64 m_fTimeToTrain;
      bool   m_bCancel;

   };
   //Definition of the different states
   class GStateInTransit : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      GStateInTransit();
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   private:
      REAL64 m_fTimeBeginning;
      bool   m_bCancel;
   };
   //Definition of the different states
   class GStatePreparingMission : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      GStatePreparingMission();
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);
   private:
      REAL64 m_fTimeBeginning;
      bool   m_bCancel;
   };

   //Definition of the different states
   class GStateReadyToExecute : public Hector::GState, public GSerializable
   {
      friend class GCovertActionCell;
   public:
      void OnEnter(void);
      void OnExit(void) ;
      void OnIterate(void);
      bool CanTransitTo(INT32 in_iNewState);

      bool Serialize(GIBuffer& io_Obj) const;
      bool Unserialize(GOBuffer& io_Obj);

   private:
      bool   m_bCancel;
   };


   /*!
   * [Class description]
   **/
   class GCovertActionCell : public GSerializable, public GStateMachineItf
   {
      friend class GStateTraining;
      friend class GStateInTransit;
      friend class GStateInCreation;
      friend class GStateDormant;
      friend class GStateGettingReady;
      friend class GStateActive;
      friend class GStatePreparingMission;
      friend class GStateReadyToExecute;
   public:
      static UINT32 m_iNextId;

      GCovertActionCell();
      ~GCovertActionCell();
      GCovertActionCell(const GCovertActionCell& in_Original);

      void Initialize(ECovertActionsCellState::Enum in_eInitialState = ECovertActionsCellState::InCreation);
      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);

      GCovertActionCell& operator=(const GCovertActionCell&);
      bool operator ==(const GCovertActionCell&) const;

      bool Dirty() const;
      void Dirty(bool in_bDirty);

      UINT32                                    ID() const;
      void                                      ID(UINT32 in_iId);
	   UINT32							               OwnerID() const;
	   void								               OwnerID(UINT32 in_iOwnerID);
	   SP2::EUnitCategory::Enum	               UnitCategory() const;
	   void								               UnitCategory(SP2::EUnitCategory::Enum in_UnitCategory);
	   SP2::EResources::Enum		               ResourceType() const;
	   void								               ResourceType(SP2::EResources::Enum in_ResourceType);
	   UINT32							               CountryToFrame() const;
	   void								               CountryToFrame(UINT32 in_iCountryToFrame);
	   UINT32							               AssignedCountry() const;
	   void								               AssignedCountry(UINT32 in_iAssignedCountry);
	   REAL32							               ExperienceLevel() const;
	   void								               ExperienceLevel(REAL32 in_fExperienceLevel);
      ECovertActionCellTraining::Enum           ExperienceLevelType() const;
      ECovertActionCellTraining::Enum           NextExperienceLevelType() const;
      void                                      Name(const GString & in_sName);
      GString                                   Name() const;
		void                                      NameID(INT32 in_iNameID);
      INT32		                                 NameID() const;
	   ECovertActionsMissionType::Enum	         MissionType() const;
	   void										         MissionType(ECovertActionsMissionType::Enum in_MissionType);
	   ECovertActionsTargetSector::Enum	         TargetSector() const;
	   void													TargetSector(ECovertActionsTargetSector::Enum in_TargetSector);
	   ECovertActionsMissionComplexity::Enum		MissionComplexity() const;
	   void													MissionComplexity(ECovertActionsMissionComplexity::Enum in_MissionComplexity);
		REAL64							               UpkeepFee() const;


      const vector<ECovertActionsCellState::Enum>&  SubsequentStates() const;
      void                                          SubsequentStateAdd(ECovertActionsCellState::Enum in_eSubsequentState);
      ECovertActionsCellState::Enum                 SubsequentStatePop();

      UINT32                     NextAssignedCountry() const;
      void                       NextAssignedCountry(UINT32 in_iCountryId);

      void                       CancelAction();

    
      //! Get the actual progression in the current state
      REAL32                                    Progression() const;

   protected:
      //Members that define the covert action cell (present in the db)
      //---------------------------------------------------------------
      GString                                m_sName;
		INT32												m_iNameID;
      UINT32                                 m_iId;
      UINT32                                 m_iOwnerID;
      union
      {
         SP2::EUnitCategory::Enum            m_eTargetUnitCategory;
         SP2::EResources::Enum               m_eTargetRessource;      
      };
      REAL32                                 m_fTraining;
      UINT32                                 m_iCountryToFrame;
      UINT32                                 m_iAssignedCountry;
      ECovertActionsMissionType::Enum        m_eMissionType;
      ECovertActionsTargetSector::Enum       m_eTargetSector;
      ECovertActionsMissionComplexity::Enum  m_eMissionComplexity;   

      ECovertActionsCellState::Enum          m_ePreviousState;
      vector<ECovertActionsCellState::Enum>  m_vSubsequentStates;

      //--- when adding members, dont forget to update the copy constructor


      mutable bool                       m_bDirty;

      UINT32                     m_iNextAssignedCountry;
      //List of state objects for the State machine
      GStateInCreation           m_StateInCreation;
      GStateDormant              m_StateDormant;
      GStateGettingReady         m_StateGettingReady;
      GStateActive               m_StateActive;
      GStateTraining             m_StateTraining;
      GStateInTransit            m_StateInTransit;
      GStatePreparingMission     m_StatePreparingMission;
      GStateReadyToExecute       m_StateReadyToExecute;
   };


};
#endif //_GOLEM_COVERTACTIONCELL_H_
