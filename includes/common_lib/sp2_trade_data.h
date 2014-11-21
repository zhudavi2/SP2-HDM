/**************************************************************
*
* sp2_trade_data.h
*
* Description
* ===========
*  Describes the interface of class GTradeData
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TRADE_DATA_H_
#define _GOLEM_TRADE_DATA_H_


namespace SP2
{
	/*!
 	 *	Class	used in this header
	 **/
   class GTradeData;
   class GTradeDataForOneSide;
   class GTradeTechnologie;   

	/*!
 	 *	List all possibles trade status
	 **/
   namespace ETradeStatus
   {
      enum Enum
      {
         None,
         Pending,
         Refused,
         Accepted,
         Count,   // Must always be last
      };
   }

	/*!
 	 *	List all possibles trade evalutions results
	 **/
   namespace ETradeEvaluationResults
   {
      enum Enum
      {
         VeryUnfair,
         Unfair,
         Fair,
         MoreThanFair,
         WayBetterThanFair,
         Count,   // Must always be last
      };
   }

   /*!
   * Trade System Window Strings
   **/
   namespace TradeStrings
   {
      const UINT32 c_StrIdSelectCountry = 101148;
      const UINT32 c_StrIdSelectRegion  = 102280;
      const UINT32 c_StrIdMoney         = 102281;
      const UINT32 c_StrIdRegion        = 102282;
      const UINT32 c_StrIdTechnologies  = 100237;
      const UINT32 c_StrIdTradeAccepted = 102297;
      const UINT32 c_StrIdTradeRefused  = 102296;
      const UINT32 c_StrIdTradeProposed = 102295;      
   }

   /*!
   * Trade System Window Values
   **/
   namespace TradeValues
   {
      const INT16  c_iNoCountry       = -1;
      const INT16  c_iNoRegion        = -1;
      const INT16  c_iNoTechType      = -1;
      const INT16  c_iNoTechName      = -1;
      const INT16  c_iNoTechLevel     = -1;
      const REAL64 c_fMaxMoneyToOffer = DBL_MAX;
      const REAL64 c_fMinMoneyToOffer = 0.f;
   }   

   /*!
    * Defines a technolige we can trade
    */
   class GTradeTechnologie
   {
   //! friends
   friend class GTradeDataForOneSide;
   public:
      //! construction and destruction
      GTradeTechnologie(EUnitCategory::Enum in_eType = EUnitCategory::Undefined,
                        EUnitDesignCharacteristics::Enum in_eName = EUnitDesignCharacteristics::Undefined,
                        INT8 in_iLevel = TradeValues::c_iNoTechLevel);
		~GTradeTechnologie();

      //! Type mutators
      EUnitCategory::Enum Type() const {return m_eTechType;};
      void Type(EUnitCategory::Enum in_eType) {m_eTechType = in_eType;};

      //! Name mutators
      EUnitDesignCharacteristics::Enum Name() const {return m_eTechName;};
      void Name(EUnitDesignCharacteristics::Enum in_eName) {m_eTechName = in_eName;};

      //! Level mutators
      INT8 Level()const {return m_iTechLevel;};
      void Level(INT8 in_iLevel) {m_iTechLevel = in_iLevel;};

      //! Serialization functions
      bool Serialize  (GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      //! operators
      bool operator==(const GTradeTechnologie &in_TradeTech) const;
      bool operator<(const GTradeTechnologie &in_TradeTech) const;
      bool operator>(const GTradeTechnologie &in_TradeTech) const;

      //! Returns the used string ids
      UINT32 CurrentTechTypeStrId() const;
      UINT32 CurrentTechNameStrId() const;

   private:
      //! tech type 
      EUnitCategory::Enum              m_eTechType;
      // c_iUnitCategoryStringID to get strings
      //! tech name id
      EUnitDesignCharacteristics::Enum m_eTechName;
      // c_iUnitDesignCharacteristicsStringID to get string ids
      //! tech level
      INT8                            m_iTechLevel;
   };//class GTradeTechnolgie

	/*!
 	 *	Class	that contains all	specific	info for	a trade side
	 **/
   class GTradeDataForOneSide : public GSerializable
   {
      //! friends 
      friend class GTradeData;
   public:
      //! construction and destruction
      GTradeDataForOneSide();
		~GTradeDataForOneSide();

      //! Reset trade data
      void Reset();

      //! Country getter and setter
      INT16 CountryId() const {return m_iCountryId;};
      void  CountryId(INT16 in_iCountryId) {m_iCountryId = in_iCountryId;};

      //! Money getter and setter
      const REAL64 & Money() const {return m_fMoney;};
      void Money(const REAL64 &in_fMoney);

      //! Serialization functions
      bool Serialize  (GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      //! return true if region is already added in the trade
      bool IsRegionInTrade(INT32 in_iRegionId);
      //! Add the requested region
      void AddRegion(INT32 in_iRegionId, const REAL64 &in_fMoney);
      //! Remove the requested region
      void RemoveRegion(INT32 in_iRegionId);
      //! Update our region value if approrpiate
      void UpdateRegionValue(INT32 in_iRegionId, const REAL64 &in_fMoney);
      //! Retrieve the current region value
      REAL64 GetRegionValue(INT32 in_iRegionId);
      //! Retrieve the regions list
      GRegionsValues & GetRegions() {return m_RegionsValues;};

      //! return true if Technologie is already added in the trade
      bool IsTechnologieInTrade(const GTradeTechnologie &in_TradeTech);
      //! Add the requested Technologie
      void AddTechnologie(const GTradeTechnologie &in_TradeTech, const REAL64 &in_fMoney);
      //! Remove the requested Technologie
      void RemoveTechnologie(const GTradeTechnologie &in_TradeTech);
      //! Update our Technologie value if approrpiate
      void UpdateTechnologieValue(const GTradeTechnologie &in_TradeTech, const REAL64 &in_fMoney);
      //! Retrieve the current Technologie value
      REAL64 GetTechnologieValue(const GTradeTechnologie &in_TradeTech);
      //! Retrieve the Technologies list
      GTechnologiesValues & GetTechnologies() {return m_TechnologiesValues;};

      //! Retreieve the trade total value
      REAL64 GetTotalValue() const;
   private:
      //! country id 
      INT16 m_iCountryId;
      //! Regions in the trade with their values
      GRegionsValues m_RegionsValues;
      //! Technologies vals in the trade
      GTechnologiesValues m_TechnologiesValues;
      //! money in the trade
      REAL64 m_fMoney;
      //! Total money in the trade
      REAL64 m_fTotalMoney;

      //! Add money amount to trade value
      void AddValueToTrade(const REAL64 &in_fMoney);
      //! Remove a money amout from the trade
      void RemoveValueFromTrade(const REAL64 &in_fMoney);

      //! Recalculate the total trade value
      REAL64 Calculate();
   };//class GTradeDataForOneSide

	/*!
 	 *	Class	that contains all	specific	info for	a trade
	 **/
   class GTradeData : public GSerializable
   {
   public:
      //! construction and destruction
      GTradeData();
		~GTradeData();

      //! Serialization functions
      bool Serialize  (GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      //! retrieve the trade evaluation percentage
      REAL64 GetTradeEvalPercentage(bool in_bFromSideA = true) const;

      //! Evaluates a trade hence its data
      ETradeEvaluationResults::Enum EvaluateTrade(bool in_bFromSideA = true) const;

      //! Get the trade data
      GTradeDataForOneSide & GetTradeDataFromSide(bool in_bFromSideA = true);

      //Init members data
      void Init();

      //! Retrieve and set our trade status
		ETradeStatus::Enum Status() const;
		void Status(ETradeStatus::Enum in_eStatus);

   private:
      // our trade result
      ETradeStatus::Enum m_eTradeStatus;
      // Side a trade information
      GTradeDataForOneSide m_SideA;
      // Side b trade information
      GTradeDataForOneSide m_SideB;

      //! Trade data getters
      GTradeDataForOneSide & GetSideA() {return m_SideA;};
      GTradeDataForOneSide & GetSideB() {return m_SideB;};

   };//class GTradeData

}; //namespace SP2


#endif //_GOLEM_TRADE_DATA_H_