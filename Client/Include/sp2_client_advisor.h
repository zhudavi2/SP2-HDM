/**************************************************************
*
* golem_clientadvisor.h
*
* Description
* ===========
*  Describes the interface of class GClientAdvisor
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CLIENTADVISOR_H_
#define _GOLEM_CLIENTADVISOR_H_

namespace SP2
{
   const UINT32 c_iWarDeclarationOnYou        = 100958;
   const UINT32 c_iWarDeclarationOther        = 100959;
   const UINT32 c_iGainedControlOfRegion      = 100960;
   const UINT32 c_iLostControlOfRegion        = 100961;
   const UINT32 c_iCountryWasNuked            = 100962;
   const UINT32 c_iCountryToBeAnnexed         = 100963;
   const UINT32 c_iCountryDeficitLongRun      = 100964;
   const UINT32 c_iPopGettingRestless         = 100965;
   const UINT32 c_iPopTurningOnYou            = 100966;
   const UINT32 c_iElectionIn6NotFavorable    = 100967;
   const UINT32 c_iElectionIn6Favorable       = 100968;
   const UINT32 c_iElectionIn1NotFavorable    = 100969;
   const UINT32 c_iElectionIn1Favorable       = 100970;
   const UINT32 c_iCountryBadlyNeedResource   = 100971;
   const UINT32 c_iStrIdTradeImpossible       = 102303;
   const UINT32 c_iStrIdTradeNeedConfirmation = 102302;

   const REAL32  c_fEconomicAlmostImpossibleRatio    = 0.97f;
   const REAL32  c_fEconomicProblemsLongRunRatio     = 0.75f;
   const REAL32  c_fPopulationRestlessApproval       = 0.30f;
   const REAL32  c_fPopulationTurningOnYouApproval   = 0.20f;
   const REAL32  c_fNonFavorableElectionOutcomeVal   = 0.50f;
   const REAL32  c_fResourceNeedVsDemandRatioIsBad   = 0.40f;

   const REAL32  c_iMessageFrequencySeconds          = 5*60;



   /*!
   * Client advisor messages, if you add something in this list, make sure
   *  you add the correct entry in the c_ClientAdvisorMessageClassification
   **/   
   namespace EClientAdvisorMsg
   {
      enum Enum
      {
         WarDeclaration,
         TakenMilitaryControl,
         CountryWasNuked,
         CountryToBeAnnexed,
         DeficitsUnsustainableLongRun,
         PopulationGettingRestless,
         PopulationTurningOnYou,
         ElectionIn6MonthsOutcome,
         ElectionIn1MonthOutcome,
         ResourceNeed,
         ItemCount,  //must be last items, nothing after that
         Undefined = ItemCount,
      };
   };


   /*!
   * Header by sphere of the advisor warnings in the mail system
   **/
   const UINT32 c_iMailSystemAdvisorWarningHeader[ESphere::Count]  =
   {
      0,     //not used
         101300,//Economic
         101301,//Politic
         101302,//Military
         101303,//Demographic
   };


   /*!
   * Classification (in one sphere) of each of the advisor message type
   **/
   const ESphere::Enum c_ClientAdvisorMessageClassification[EClientAdvisorMsg::ItemCount] =
   {
      ESphere::Military,    // WarDeclaration
         ESphere::Military,    // TakenMilitaryControl,
         ESphere::Military,    // CountryWasNuked
         ESphere::Politic,     // CountryToBeAnnexed,
         ESphere::Economic,    // DeficitsUnsustainableLongRun,
         ESphere::Politic,     // PopulationGettingRestless,
         ESphere::Politic,     // PopulationTurningOnYou,
         ESphere::Politic,     // ElectionIn6MonthsOutcome,
         ESphere::Politic,     // ElectionIn1MonthOutcome,   
         ESphere::Economic,    // ResourceNeed,        
   };


   /*!
    * Defines the possible actions 
    **/
   namespace EClientAdvisorIsActionOkResult
   {
      enum Enum
      {
         Ok,
         NeedConfirmation,
         Impossible
      };
   };

    /*!
    * Defines the possible advisor notifications
    **/
   namespace EClientAdvisorNotification
   {
      enum Enum
      {
         EventWasCanceled,
         EventIsOk
      };
   };

   class GClientAdvisor;
   typedef EClientAdvisorIsActionOkResult::Enum (GClientAdvisor::*ClientAdvisorRaiseEventHandler)(GGameEventSPtr in_Event,GString& out_sMsg);


   /*!
   * Really really brilliant advisor with complex ai on the client side
   * (At least as intelligent as Mumba,  the auto-satisfied gorilla from Granby's Zoo))
   **/
   class GClientAdvisor : public Hector::DesignPattern::GObserver, public Hector::DesignPattern::GSubject
   {
   public:
      GClientAdvisor();
      ~GClientAdvisor();

      //! Have the advisor saying something, 
      void AddMessage(EClientAdvisorMsg::Enum in_eMsg,
         UINT32                  in_iParam1 = 0,
         UINT32                  in_iParam2 = 0,
         UINT32                  in_iParam3 = 0);

      //! Cleanup the client advisor
      void CleanUp();

      //! Initializes the client advisor
      void Initialize();
      
      //! Have the advisor evaluate the actual situation
      void EvaluateSituation();

      //! Raise a game event and have the client advisor evaluate its potential consequences (advisor might say something about it)
      void RaiseEvent(GGameEventSPtr in_Evt);

      //! Mailbox callback handler to display the advisor message from the mailbox
      static void DisplayAdvisorMsg(void* in_pParam);

   protected:
   private:

      // event functions
      void SendEventToServer(GGameEventSPtr in_Evt);
      void CancelEvent(GGameEventSPtr in_Evt);

      GTree<GXMLNode>* m_pAdvisorConfigRoot;

      UINT32 m_iNextMessageId;

      REAL64 m_fNextGameTimeCheckForElectionsIn6Months;
      REAL64 m_fNextGameTimeCheckForElectionsIn1Month;


      REAL64 m_fLastMsgTimeGeneratingDeficits;
      REAL64 m_fLastMsgTimePopulationRestless;
      REAL64 m_fLastMsgTimeNeedResources;


      //! Handle a war declaration between 2 countries
      void HandleWarDeclaration(UINT32 in_iCountry1,UINT32 in_iCountry2);
      //!  Handle a region that is now military controlled
      void HandleTakenMilitaryControl(UINT32 in_iNewOwner, UINT32 in_iOldOwner, UINT32 in_iRegionNameStrId);
      //! Handle the fact that a country has been  nuked
      void HandleCountryWasNuked(UINT32 in_iNukedCountry);
      //! Handle the intentions to annex a country
      void HandleCountryToBeAnnexed(UINT32 in_iCountryToBeAnnexed,UINT32 in_iAnnexer);

      //! Evaluate if the Country is generating deficits unsustainable in the long run
      void EvaluateGeneratingDeficits();
      //! Evaluate if population is getting restless or turning on you
      void EvaluatePopulationRestless();
      //! Evaluate Election outcome msg
      void EvaluateElectionOutcome();
      //! Evaluate Resources Needs
      void EvaluateResourceNeeds();


      //! Method that analyses the economical situation of the player and tells if the player can afford the expense.
      EClientAdvisorIsActionOkResult::Enum CanAffordExpense(REAL64 in_fExpense) const;



      //! Observer on subject change
      void OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
         const Hector::DesignPattern::GSubjectNotification& in_Notification,
         void* in_pAdditionalMsg);


      
      EClientAdvisorIsActionOkResult::Enum IsAdjustTaxOk                 (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if the tax adjustment is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsBuildUnitsOk                (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if the unit build order is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsBuyAMDSOk                   (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if starting the amds research is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsBuyUsedUnitsOk              (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if buying used units is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsIncreaseResourceProductionOk(GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if increasing the resource production is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsTrainCovertActionCellOk     (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if updating a covert action cell status is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsCreateCovertActionCellOk    (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if creating a covert action cell status is ok, if ok, returns true, if not, returns false and fills the msg param
      EClientAdvisorIsActionOkResult::Enum IsTradeProposalOk             (GGameEventSPtr in_Event,GString& out_sMsg); //!<Tells if proposing or accepting a trade is ok, if ok, returns true, if not, returns false and fills the msg param



      hash_map<INT32,ClientAdvisorRaiseEventHandler> m_EventIdHandlerMethodMap;

      //! Queues a message in the pending message list
      void QueueMessage(EClientAdvisorMsg::Enum in_eMsgType,const GString& in_sMsg);

      //! List of pending messages of the advisor, key is the ID of the message
      hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> > m_PendingMessages; 

      //! Locate the 1st message of the given type in the pending message list, returns m_vPendingMessages.end if nothing
      hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> >::iterator LocateFirstMessageOfType(EClientAdvisorMsg::Enum in_eType);

      //! Events that were not raised directly and are waiting for a confirmation from the player
      hash_map<INT32,GGameEventSPtr> m_EventsWaitingForConfirmation;


      //! Used to display a message when mail about gain of region is opened
      static void DisplayRegionGained(void*);

      //! Used to display a message when mail about lost of region is opened
      static void DisplayRegionLost(void*);

      //! ID of the mail used to display the gain of regions (0 when no mail sent)
      UINT32 m_iRegionGainedMailID;

      //! ID of the mail used to display the gain of regions (0 when no mail sent)
      UINT32 m_iRegionLostMailID;

      //! List of regions that have been gained since player last checked his mails
      set<UINT32> m_RegionsGained;

      //! List of regions that have been gained since player last checked his mails
      set<UINT32> m_RegionsLost;
   };

};
#endif //_GOLEM_CLIENTADVISOR_H_

