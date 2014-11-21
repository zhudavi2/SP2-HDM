/**************************************************************
*
* golem_arenainfo.h
*
* Description
* ===========
*  Describes the interface of class GArenaInfo
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_ARENAINFO_H_
#define _GOLEM_ARENAINFO_H_
namespace SP2
{

   //Enum to specify which actions are happening in a combat
   namespace ECombatAction
   {
      enum Enum
      {
         Hit,
         Moving,
      };  
   };

   /*!
    * Packet send during when the combat overview window is visible
    **/
   struct GCombatInformationPacket
   {
      INT32  m_iAction;     //ECombatAction converted into a 2 byte value
      INT16  m_iArenaID;   
      INT16  m_iID1;
      INT16  m_iID2;
      UINT32 m_iValue;      //Associated value, depends on the Action
   };


   namespace EUnitAction
   {
      enum Enum
      {
         Attacking,
         Defending
      };
   }

   class GUnitTypeCbtInfo : public GSerializable
   {
   public:
      REAL64               m_fLastMovementTime;
      REAL32               m_fPosition; //0 to c_iBattleArenaSize
      EUnitAction::Enum    m_eAction; 
      REAL32               m_fUnitsValue; //In $, can be used to calculate the strength ratio
      UINT32               m_iUnitQty;

      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);

   };

   namespace ECombatSide
   {  
      enum Enum
      {
         Attacker = 0,
         Defender = 1,
         CantDetermine,        
         ItemCount//Should always be the last item of the enum
      };
   
   }



   /*!
    * Combat MemberInformation
    **/
   class GActorInfo : public GSerializable
   {
   public:

      GActorInfo();
      ~GActorInfo();

      UINT16 m_iCountryID;
      REAL32                m_fMaxFrontLine; //!<Between 0 and c_iBattleArenaSize, maximal front line
      ECombatSide::Enum     m_Side;
      bool                  m_bRetreating;

      vector<GUnitTypeCbtInfo>    m_vUnitTypeInfo;

      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);

      //! Get the quantity of units for the given category
      INT32 UnitQty(EUnitCategory::Enum) const;

      //! Fill an array with the number of units of each category
      /*!
       * The function does not reset the buffer to zero before beginning counting units.
       * This can be useful when counting combat sides totals.
       * @param io_pCounts       : Buffer that will receive the results
       **/
      void CountUnitTypes(UINT32 io_pCounts[EUnitCategory::ItemCount] ) const;

   private:
   };

   /*!
   * SP2 Arena Information
   **/
   class GArenaInfo : public SDK::Combat::GArenaInfo
   {
      friend class GArena;
      friend class GServer;
      friend class GDataControlLayer;

      static SDK::Combat::GArenaInfo* New();
   public:
      GArenaInfo();
      ~GArenaInfo();

      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);

      GActorInfo* Actor(UINT32 in_iCountryID);

      const hash_map<UINT32,GActorInfo>& Actors() const;

      vector<UINT32> m_vUnitGroupIDs;


      UINT32 m_pArenaLosses[ECombatSide::ItemCount][EUnitCategory::ItemCount];

      //! List of attacking countries that has taken part into combat
      vector<UINT16> m_vAttackerCountriesList; 
      //! List of defending countries that has taken part into combat
      vector<UINT16> m_vDefenderCountriesList; 

      REAL32 m_fFurthestAttackerUnit;
      REAL32 m_fFurthestDefenderUnit;
      UINT32 m_iRegionID;//!< Region where the arena is located

      ECombatSide::Enum m_eWinnerSide;

   protected:
   private:
      //! ActorID, Actor (actor id = country id)
      hash_map<UINT32,GActorInfo> m_Actors;
   };


};
#endif //_GOLEM_ARENAINFO_H_
