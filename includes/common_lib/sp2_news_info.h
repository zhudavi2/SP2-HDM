/**************************************************************
*
* golem_newsinfo.h
*
* Description
* ===========
*  Describes the interface of class News::GInfo
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_NEWSINFO_H_
#define _GOLEM_NEWSINFO_H_



namespace SP2
{
namespace News
{




   //Gives the category of the news
   namespace ECategory
   {
      enum Enum
      {
         Political = 1,
         Military,
         Economical,
			Demographic,
         Undefined
      };
   };


	/*!
	* Enum of all news sentences
	**/
	namespace EType
	{
		enum Enum
		{
      //-------Low priority news begin--------
         BeginLowPriorityNews = 0,
			MartialLawOn = 1,
			MartialLawOff,
			PublicControlRes,
			PrivateControlRes,
			RaiseTaxRes,
			LowerTaxRes,
			RaiseGlobalModTax,
			LowerGlobalModTax,
			ChangeSuffrage,
			RaiseInterestRate,
			LowerInterestRate,
			RaisePersonalIncomeTax,
			LowerPersonalIncomeTax,
			RaiseMinimumWage,
			LowerMinimumWage,
			DisbandUnits,
			StartProductionUnits,
			CancelProductionUnits,
			BoughtUnits,
			REAvalanche,
			REBlizzard,
			REColdWave,
			RECyclone,
			REDrought,
			REEarthquake,
			REFlood,
			REHeatwave,
			REHurricane,
			REInsectInfestation,
			RELandslide,
			REStorm,
			RETidalwave,
			RETornado,
			RETsunami,
			RETyphoon,
			REVolcanicEruption,
			REWildfire,
			REStockMarketCrash,
			REEconomicDepression,
			REEconomicBoom,
			REPoorHarvest,
			REBountifulHarvest,
			RELivestockEpidemic,
			REResourceDiscoveryOre,
			REResourceDiscoveryEnergy,
			REResourceDiscoveryWealth,
			REEpidemic,
			REPopulationBoom,
			REFamine,
			RENuclearMeltdown,
			REChemicalSpill,
			REIndustrialAccident,
			REAntiGlobalProtest,
         EndLowPriorityNews,
      //--------  Low priority news end ------------------
      //--------  Medium priority news begin -------------
         BeginMediumPriorityNews,
			TakenMilitaryControl,
			AnnexedRegions,
			ChangedGovernmentType,
			FallenAnarchy,
			ChangeStatusLanguage,
			ChangeStatusReligion,
			IllegalizedRes,
			LegalizedRes,
			ChangeStatusPoliticalParties,			
			OpenedEmigration,
			ClosedEmigration,
			OpenedImmigration,
			ClosedImmigration,
			SignedCeaseFire,
			SignedTreatySingle,
			SignedTreatyTwo,
			NuclearAttacks,
			MoveCapital,
			OrderedUnits,
			ReadiedUnits,
			ProposedTreaty,
         BattleOver,
			LegalizedLaw,
			IllegalizedLaw,
			NbChildrenLimited,
			NbChildrenIllimited,
			EspionageMilitary,
			SabotageMilitary,
			EspionageCivilian,
			SabotageCivilian,
			SocialUpheaval,
			EspionageCivilianFailed,
			SabotageCivilianFailed,
			EspionageMilitaryFailed,
			SabotageMilitaryFailed,
			SocialUpheavalFailed,
			EspionageCivilianFramed,
			SabotageCivilianFramed,
			EspionageMilitaryFramed,
			SabotageMilitaryFramed,
			SocialUpheavalFramed,
			LongTermTreatySigned,
         EndMediumPriorityNews,
      //------- Medium priority news End ---------
      //------- High priority news begin ---------
         BeginHighPriorityNews,
			DeclareWar,
			Conquered,
			PeaceTreatyLiberation,
			PeaceTreatyAnnexation,
			AMDSResearch,			
			LaunchNuclearAttack,
			DeclareIntentionToAnnexRegions,
			AMDSActivation,
			Elections,
			Assassination,
			Terrorism,
			CoupEtat,
			AssassinationFailed,
			TerrorismFailed,
			CoupEtatFailed,
			AssassinationFramed,
			TerrorismFramed,
			CoupEtatFramed,
         EndHighPriorityNews,
      //------- High priority news end ---------           
         Undefined,
         Count, //--Must absolutely be the last item
		};		
	};//End namespace	ENewsType

    

/*!
 * Class that holds the information of the news
 **/
class GInfo : public GSerializable
{
public:
   GInfo();
   ~GInfo();
	UINT32	            m_iCountryA;
   UINT32	            m_iCountryB;
	REAL64	            m_fGameTime;
   News::ECategory::Enum  m_eCategory;
   News::EType::Enum      m_eType;
	UINT8		            m_iPriority;     
   vector<INT32>        m_vNumbers;//!< Numbers to write in the string (replacing tag with)
   vector<INT32>        m_vStrings;//!< String to write inside the string (replacing tag with)

   bool Serialize(GIBuffer&) const;
   bool Unserialize(GOBuffer&);

   GString ToString() const;

protected:
//   GString ReplaceNextPattern(GString in_sSentence, 
//                              GString in_ReplacingString, 
//                              GString in_sPattern) const;
private:
};

};
};
#endif //_GOLEM_NEWSINFO_H_
