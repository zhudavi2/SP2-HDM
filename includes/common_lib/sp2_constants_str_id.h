/**************************************************************
*
* sp2_constants_str_id.h
*
* Description
* ===========
*  Hold constants for string table
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

namespace SP2
{
   namespace EStrId
   {
      enum Enum
      {
         TreatyAlliance                                  = 100595,
         TreatyWar                                       = 100591,
         TreatyMilitaryAccess                            = 100596,
         TreatyPeaceLiberation                           = 100593,
         TreatyPeaceAnnexation                           = 100594,
			TreatyCeaseFire                                 = 100592,
			TreatyEconomicPartnership                       = 100604,
			TreatyCommonMarket                              = 100605,
			TreatyEconomicAid                               = 100606,
			TreatyAssumeForeignDebt                         = 100607,
			TreatyEconomicEmbargo                           = 100608,
			TreatyNuclearDisarmament								= 100612,
			TreatyWeaponTrade                               = 100609,
			TreatyWeaponTradeRebates                        = 100610,
			TreatyWeaponTradeEmbargo                        = 100611,
			TreatyHumanDevelopmentCollaboration             = 100603,
			TreatyCulturalExchanges                         = 100600,
			TreatyResearchPartnership                       = 100602,
			TreatyRequestMilitaryPresenceRemoval            = 100597,
			TreatyNobleCause                                = 100601,
			TreatyAnnexation                                = 100598,
			TreatyFreeRegion                                = 100599,
			TreatyRequestWar                                = 102306,
			TreatyAssumeForeignDebtInverted                 = 102307,
			TreatyFreeRegionInverted                        = 102308,

         TreatyDescriptionAlliance                       = 100643,
         TreatyDescriptionWar                            = 100639,
         TreatyDescriptionMilitaryAccess                 = 100644,
         TreatyDescriptionPeaceLiberation                = 100641,
         TreatyDescriptionPeaceAnnexation                = 100642,
         TreatyDescriptionCeaseFire                      = 100640,
         TreatyDescriptionEconomicPartnership            = 100652,
         TreatyDescriptionCommonMarket                   = 100653,
         TreatyDescriptionEconomicAid                    = 100654,
         TreatyDescriptionAssumeForeignDebt              = 100655,
         TreatyDescriptionEconomicEmbargo                = 100656,
         TreatyDescriptionStrategicWeaponsCuts           = 100660,
         TreatyDescriptionWeaponTrade                    = 100657,
         TreatyDescriptionWeaponTradeRebates             = 100658,
         TreatyDescriptionWeaponTradeEmbargo             = 100659,
         TreatyDescriptionHumanDevelopmentCollaboration  = 100651,
         TreatyDescriptionCulturalExchanges              = 100648,
         TreatyDescriptionResearchPartnership            = 100650,
         TreatyDescriptionRequestMilitaryPresenceRemoval = 100645,
         TreatyDescriptionNobleCause                     = 100649,
         TreatyDescriptionAnnexation                     = 100646,
         TreatyDescriptionFreeRegion                     = 100647,
         
         NoFilter                                        = 100618,
         Neighbour                                       = 100632,
         SameGeopoliticalGroup                           = 100633,
         NoLimit                                         = 100634,
         Similar                                         = 100635,
         NotSoSimilar                                    = 100636,
         Close                                           = 100637,
         NotAtWar                                        = 100638,

         SelectTreatyType                                = 100661,

         Ally                                            = 100665,
         Neutral                                         = 100666,
         Enemy                                           = 100667,
         Friendly                                        = 101616,

         Modify                                          = 100190,
         Refuse                                          = 100668,
         Join                                            = 100669,
         Leave                                           = 100670,
         Active                                          = 100302,
         Pending                                         = 100631,
         Suspended                                       = 100814,
         Refused                                         = 100815,
         SponsorTreaty                                   = 100625,
         RemoveSponsorship                               = 100816,

         TreatyHasBeenRemoved                            = 100817,
         MustChooseCountryBeforeStartGame                = 100818,
         TreatyWithSameName                              = 100819,
         TreatyIsAccepted                                = 100820,

         Top10                                           = 100821,
         Top25                                           = 100822,
         Top50                                           = 100823,
         Top100                                          = 100824,

         AllDemocracies                                  = 101600,
         CommunistState                                  = 101601,
         MilitaryDictatorship                            = 101602,
         Monarchy                                        = 101603,
         MultiPartyDemocracy                             = 101604,
         SinglePartyDemocracy                            = 101605,
         Theocracy                                       = 101606,

         Africa                                          = 100826,
		   Europe                                          = 100827,
		   America                                         = 100828,
		   Oceania                                         = 100829,
		   Asia                                            = 100830,
		   CentralAmerica                                  = 100831,
		   EasternAfrica                                   = 100832,
		   EasternAsia                                     = 100833,
		   EasternEurope                                   = 100834,
		   FarEast                                         = 100835,
		   MiddleEast                                      = 100836,
		   NorthernAfrica                                  = 100837,
		   NorthernAmerica                                 = 100838,
		   PacificIslands                                  = 100839,
		   SouthernAfrica                                  = 100840,
		   SouthernAmerica                                 = 100841,
		   SouthernOceania                                 = 100842,
		   WesternAfrica                                   = 100843,
		   WesternEurope                                   = 100844,

         NA                                              = 100106,
         Days                                            = 100672,

			LawFreedomSpeech											= 100682,
			LawFreedomOfDemonstration								= 100683,
			LawWomenSuffrage											= 100684,
			LawNumberOfChildren										= 100685,
			LawContraception											= 100686,
			LawAbortion													= 100687,
			LawChildLabor												= 100688,
			LawPolygamy													= 100689,
			LawSameSexMarriage										= 100690,

         Limited                                         = 100973,
         Unlimited                                       = 100974,
         Permitted                                       = 100975,
         NotPermitted                                    = 100976,
         InPower                                         = 101003,

         GameSpeedSlowest                                = 100787,
         GameSpeedSlow                                   = 100788,
         GameSpeedNormal                                 = 100789,
         GameSpeedFast                                   = 100790,
         GameSpeedFastest                                = 100791,

         Goal1Year                                       = 100792,
         Goal5Years                                      = 100793,
         Goal10Years                                     = 100794,

         Enabled                                         = 100795,
         Disabled                                        = 100796,

         CellInCreation                                  = 101061,
         CellDormant                                     = 101062,
         CellGettingReady                                = 101063,
         CellActive                                      = 101064,
         CellTraining                                    = 101065,
         CellInTransit                                   = 101066,
         CellPreparingMission                            = 101067,
         CellReadyToExecute                              = 101068,


         UnitView                                        = 101493,
         Statistics                                      = 101494,

         MissionTypeSabotage                             = 101069,
         MissionTypeSocialUpheaval                       = 101070,
         MissionTypeAssassination                        = 101071,
         MissionTypeEspionage                            = 101072,
         MissionTypeTerrorism                            = 101073,
         MissionTypeCoupEtat                             = 101074,

         MissionTargetCivilian                           = 101075,
         MissionTargetMilitary                           = 101076,

         MissionComplexityLow                            = 101077,
         MissionComplexityMedium                         = 101078,
         MissionComplexityHigh                           = 101079,

         TrainingRecruit                                 = 101082,
         TrainingRegular                                 = 101083,
         TrainingVeteran                                 = 101084,
         TrainingElite                                   = 101085,

         MustEnterName                                   = 101086,

         Good                                            = 101087,
         NeutralRelation                                 = 101560,
         Bad                                             = 101088,
         High                                            = 101089,
         Low                                             = 101090,
         Selected                                        = 101091,

         Normal                                          = 101142,
         Fast                                            = 101143,
         Enroll                                          = 101144,
         Build                                           = 101145,
         EtaDays                                         = 101146,
         Cancel                                          = 101147,


         SelectACountry                                  = 101148,
         WaitReceivingInformation                        = 101149,
         NotSelected                                     = 101150,
         TutorialUser                                    = 101151,
         Ready                                           = 101152,
         GameLobby                                       = 101153,
         StartGame                                       = 101154,
         Playing                                         = 101155,
         ChatRoom                                        = 101156,
         Month                                           = 101163,

         UnitTypeAir                                     = 101129,
         UnitTypeNaval                                   = 101130,
         UnitTypeGround                                  = 101131,
         UnitTypeStrategic                               = 101132,

         ExecuteMission                                  = 101157,
         NewMission                                      = 101040,

         Success                                         = 101288,
         Failed                                          = 101289,

         CountryTargetOfCovertAction                     = 101295,
         CountryFramed                                   = 101296,
         AttackedCountry                                 = 101297,
         AttackingCountry                                = 101293,
         CovertActionFailedCaptured                      = 101298,
			CovertActionFailedNotCaptured                   = 101660,
         CovertActionSuccessful                          = 101299,

         CovertActionsFramedByCountrySuccessful          = 101304,
         CovertActionsFramedByCountryFailed              = 101305,
         CovertActionsAttackedSuccessful                 = 101306,
         CovertActionsAttackedFailed                     = 101307,
         AttemptBy                                       = 101308,

         UnknownInSymbol                                 = 101637,

         ConditionAverage                                = 101495,
         ConditionLow                                    = 101496,
         ConditionHigh                                   = 101497,

         MartialLaw                                      = 101520,

         None                                            = 101525,

         Description                                     = 101595,
         Objectives                                      = 101596,
         Tips                                            = 101597,

         ButHasBeenRefused                               = 101613,
         AndHasBeenAccepted                              = 101614,
         AskedToJoinTreaty                               = 101161,

         DescriptionInfantryVehicle                      = 101618,
         DescriptionAirDefense                           = 101619,
         DescriptionMobileLauncher                       = 101620,
         DescriptionTank                                 = 101621,
         DescriptionArtilleryGun                         = 101622,         
         DescriptionTransportHelicopter                  = 101623,
         DescriptionAttackHelicopter                     = 101624,
         DescriptionASWHelicopter                        = 101625,
         DescriptionAttackAircraft                       = 101626,
         DescriptionFighterAircraft                      = 101627,
         DescriptionBomber                               = 101628,
         DescriptionPatrolCraft                          = 101629,
         DescriptionCorvette                             = 101630,
         DescriptionFrigate                              = 101631,
         DescriptionDestroyer                            = 101632,
         DescriptionAttackSubmarine                      = 101633,
         DescriptionBallisticMissileSubmarine            = 101634,
         DescriptionAircraftCarrier                      = 101635,
         DescriptionBallisticMissile                     = 101636,

         SmallAll                                        = 101665,
         SmallElevationAndWater                          = 101666,
         SmallElevationOnly                              = 101667,
         SmallNone                                       = 101668,
         
         SmallLow                                        = 101669,
         SmallMedium                                     = 101670,
         SmallHigh                                       = 101671,

         StartingGame                                    = 102081,
         ConnectingGame                                  = 102082,
         SavingGame                                      = 102097,
         LoadingGame                                     = 102102,

         PlayerNotAdminSave                              = 102101,
         PlayerNotAdminLoad                              = 102108,
         CannotCreateGame                                = 102123,
         FreeForALL                                      = 102085,
         Multiplayer                                     = 102086,

         MilitaryStatusReady                             = 100243,
         MilitaryStatusFortified                         = 100241,
         MilitaryStatusParked                            = 100242,
         MilitaryStatusMoving                            = 102074,
         MilitaryStatusAttacking                         = 102075,
         MilitaryStatusTraining                          = 102076,
         MilitaryStatusUpgrading                         = 102077,
         MilitaryStatusForSale                           = 102078,
         MilitaryStatusInTransit                         = 102079,
         MilitaryStatusReadyToDeploy                     = 102080,

         MoveDenialPrefix                                = 102109,
      };

      // This array MUST be in sync with EHistoryValues::Enum content
      const UINT32 c_HistoryValues[EHistoryValues::TotalCount] = 
      {
         100798,
         100940,
         100941,
         100942,
         100944,
         100945,
         100799,
         100946,
         100947,
         100949,
         100800,
         100950,
         100951,
         100801,
         100955,
         100956,
         100957,
      };

      const UINT32 c_TreatyTypes[ETreatyType::ItemCount] =
      {
         EStrId::TreatyAlliance,
         EStrId::TreatyWar,
         EStrId::TreatyMilitaryAccess,
         EStrId::TreatyPeaceLiberation,
         EStrId::TreatyPeaceAnnexation,
			EStrId::TreatyCeaseFire,
			EStrId::TreatyEconomicPartnership,
			EStrId::TreatyCommonMarket,
			EStrId::TreatyEconomicAid,
			EStrId::TreatyAssumeForeignDebt,
			EStrId::TreatyEconomicEmbargo,
			EStrId::TreatyWeaponTrade,
			EStrId::TreatyWeaponTradeEmbargo,
			EStrId::TreatyHumanDevelopmentCollaboration,
			EStrId::TreatyCulturalExchanges,
			EStrId::TreatyResearchPartnership,
			EStrId::TreatyRequestMilitaryPresenceRemoval,
			EStrId::TreatyNobleCause,
			EStrId::TreatyAnnexation,
			EStrId::TreatyFreeRegion,
      };

      // This array MUST be in sync with EHistoryMarkerType::Enum content
      const UINT32 c_HistoryMarkers[EHistoryMarkerType::TotalCount] = 
      {
         100939, // Gain political control (qty)
         100943, // Loose political control (qty)
         100948, // Gain military control (qty)
         100952, // Loose military control (qty)
         100804, // Declare war (qty)
         100805, // Declare peace (qty)
         100807, // Receive nuke (qty)
         100953, // Change government type (name)
         100806, // Launch nuke (qty)
         100802, // Increase taxes (qty)
         100803, // Decrease taxes (qty)
         100954, // Combat started (qty)

         101526, // Increase minimum wage (qty)
         101527, // Decrease minimum wage (qty)
         101528, // Political party legal (name)
         101529, // Political party illegal (name)
         101530, // Religion/language legal (name)
         101531, // Religion/language illegal (name)
         101554, // Religion/language official (name)
         101532, // Declare martial law (nothing)
         101533, // Undeclare martial law (nothing)
         101534, // Covert ops success (type)
         101535, // Covert ops failure (type)
         101553, // Covert ops victim (type)
         101536, // Increase resource production (qty)
         101537, // Increase interest level (qty)
         101538, // Decrease interest level (qty)
         101539, // Resource public (%)
         101540, // Resource private (%)
         101541, // Random event (type)
         101542, // Treaty signed (type)
         101543, // Treaty canceled (type)
         101544, // Increase military arsenal (value)
         101545, // Decrease military arsenal (value)
         101546, // Park units (value)
         101547, // Fortify units (value)
         101548, // Ready units (value)
         101549, // Increase resource tax (percent)
         101550, // Decrease resource tax (percent)
         101551, // Train units (value)
         101552, // Modify law (type)
         101555, // Increase global resource tax (percent)
         101559, // Decrease global resource tax (percent)
      };

      // Associate a research level to a description
      const UINT32 c_ResearhLevel[11] = 
      {
         101566,
         101567,
         101568,
         101569,
         101570,
         101571,
         101572,
         101573,
         101574,
         101575,
         101576,
      };

      // Associate save results to text message
      const UINT32 c_SaveResult[SDK::EGameDataError::ItemCount] =
      {
         102098, // NoError
         0, // CantOpenFile
         102099, // CantCreateFile
         0, // InvalidGameType
         0, // InvalidDataVersion
         0, // NewGameError
         102100, // LoadGameError
         0, // SaveGameError
      };

      // Associate save results to text message
      const UINT32 c_LoadResult[SDK::EGameDataError::ItemCount] =
      {
         102103, // NoError
         102104, // CantOpenFile
         0, // CantCreateFile
         102105, // InvalidGameType
         102106, // InvalidDataVersion
         0, // NewGameError
         102107, // LoadGameError
         0, // SaveGameError
      };

      // Associate movement denial to text message
      const UINT32 c_MoveDenial[ERefusedMoveReason::Count] =
      {
         102110, // Unknowm
         102111, // NoDiplomaticSupport
         102112, // NoMilitarySupport
         102113, // NoAirSupport
         102115, // NoWaterSupport
         102114, // NoLandSupport
      };
   }
}
