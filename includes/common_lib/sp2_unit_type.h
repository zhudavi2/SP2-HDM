/**************************************************************
*
* golem_unittype.h
*
* Description
* ===========
*  Describes the interface of class GUnitType
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_UNITTYPE_H_
#define _GOLEM_UNITTYPE_H_

namespace SP2
{
   namespace EUnitCategory
   {
      enum Enum
      {
         Undefined = -1,
         Infantry = 0,
         Ground,
         Air,
         Naval,
         Nuclear,         
         ItemCount//Should always be last item of the enum
      };
   };

   const UINT32 c_iUnitCategoryStringID[EUnitCategory::ItemCount] = 
   {
      100034,//Infantry
      100035,//Ground
      100036,//Air
      100037,//Naval
      100218,//Nuclear
   };

	namespace EUnitType
	{
		enum Enum
		{
			Infantry = 1,
			InfantryVehicle,
			AirDefense,
			MobileLauncher,
			Tank,
			ArtilleryGun,
			AttackHelicopter,
			TransportHelicopter,
			ASWHelicopter,
			FighterAircraft,
			AttackAircraft,
			Bomber,
			PatrolCraft,
			Corvette,
			Frigate,
			Destroyer,
			AttackSubmarine,
			BallisticMissileSubmarine,
			AircraftCarrier,
			BallisticMissile,
         TotalCount
		};
	};

   const EUnitCategory::Enum c_eUnitCategoryByType[EUnitType::TotalCount] = 
   {
         (EUnitCategory::Enum)-1, //Undefined
         EUnitCategory::Infantry, //Infantry
         EUnitCategory::Ground,   //InfantryVehicle
         EUnitCategory::Ground,   //AirDefense,
			EUnitCategory::Ground,   //MobileLauncher,
			EUnitCategory::Ground,   //Tank,
			EUnitCategory::Ground,   //ArtilleryGun,
         EUnitCategory::Air,  	 //AttackHelicopter,
			EUnitCategory::Air,  	 //TransportHelicopter,
			EUnitCategory::Naval,  	 //ASWHelicopter,
			EUnitCategory::Air,  	 //FighterAircraft,
			EUnitCategory::Air,  	 //AttackAircraft,
			EUnitCategory::Air,  	 //Bomber,
         EUnitCategory::Naval,    //PatrolCraft,
			EUnitCategory::Naval,    //Corvette,
			EUnitCategory::Naval,    //Frigate,
			EUnitCategory::Naval,    //Destroyer,
			EUnitCategory::Naval,    //AttackSubmarine,
			EUnitCategory::Naval,    //BallisticMissileSubmarine,
			EUnitCategory::Naval,    //AircraftCarrier,
         EUnitCategory::Nuclear,  //BallisticMissile,
   };


   

   const UINT32   c_iUnitTypeStringID[EUnitType::TotalCount] = 
   {
         0,      //Undefined
         100262,  //Infantry
         100244,  //InfantryVehicle
         100245,  //AirDefense,
			100246,  //MobileLauncher,
			100247,  //Tank,
			100248,  //ArtilleryGun,
         100249,  //AttackHelicopter,
			100250,  //TransportHelicopter,
			100251,  //ASWHelicopter,
			100252,  //FighterAircraft,
			100253,  //AttackAircraft,
			100254,  //Bomber,
         100255,  //PatrolCraft,
			100256,  //Corvette,
			100257,  //Frigate,
			100258,  //Destroyer,
			100259,  //AttackSubmarine,
			100260,  //BallisticMissileSubmarine,
			100261,  //AircraftCarrier,
         100263,  //BallisticMissile,
   };

   /*!
   * Defines a type of unit, to be registered into the GCombatManager
   **/
   class GUnitType
   {
   public:
      GUnitType();
      ~GUnitType();

      UINT32 Id() const;
      void   Id(const UINT32 in_iID);
      void   Name(const GString& in_sName);
      const GString& Name() const;

      inline EUnitCategory::Enum Category() const{return m_Category;}
      void Category(EUnitCategory::Enum in_Category);

      void  HP(INT32 in_iHP);
      INT32 HP() const;
      inline REAL64 InverseTwiceHPByType(void){  return  m_fInverseDoubledHP;};
      void GunFireRate(REAL32);
      REAL32 GunFireRate();
      void MissileFireRate(REAL32);
      REAL32 MissileFireRate();

      void MissileRearmingDelay(REAL32);
      REAL32 MissileRearmingDelay() const;

      const REAL32* SpeedByLevel();
      void  SpeedByLevel(REAL32* in_SpeedLevel);
      const REAL32* GunRangeByLevel();
      void  GunRangeByLevel(REAL32* in_RangeLevel);
      void  MissileRangeByLevel(REAL32* in_RangeLevel);
      const REAL32* MissileRangeByLevel();
      const REAL32* MissilePayloadByLevel();
      void  MissilePayloadByLevel(REAL32* in_RangeLevel);
      const REAL32* MissileDamageByLevel();
      void  MissileDamageByLevel(REAL32* in_RangeLevel);
      const REAL32* GunDamageByLevel();
      void  GunDamageByLevel(REAL32* in_RangeLevel);
      void  ArmorByLevel(REAL32* in_ArmorLevel);
      const REAL32* ArmorByLevel();
      void  SensorsByLevel(REAL32* in_SensorsLevel);
      const REAL32* SensorsByLevel();
      void  CounterMesuresByLevel(REAL32* in_CounterMesuresLevel);
      const REAL32* CounterMesuresByLevel();
      void  GunPrecisionByLevel(REAL32* in_GunPrecisionLevel);
      const REAL32* GunPrecisionByLevel();
      void  MissilePrecisionByLevel(REAL32* in_MissilePrecisionLevel);
      const REAL32* MissilePrecisionByLevel();
      void  StealthByLevel(REAL32* in_StealthLevel);
      const REAL32* StealthByLevel();
      REAL32 Density();
      void   Density(REAL32 in_fDensity){m_fDensity = in_fDensity;}

   protected:
   private:
      //Unique identifier for the unit type
      UINT32         m_iId;
      //Name for the type (ex: Soldier, Helicopter...)
      GString        m_sName;
      INT32          m_iHP;
      REAL32         m_fGunFireRate;
      REAL32         m_fMissileFireRate;
      REAL32         m_fMissileRearmingDelay;
      REAL64         m_fInverseDoubledHP;
      EUnitCategory::Enum m_Category;

      REAL32         m_SpeedByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_MissileRangeByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_MissilePayloadByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_GunRangeByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_MissileDamageByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_GunDamageByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_ArmorByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_SensorsByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_CounterMesuresByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_GunPrecisionByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_MissilePrecisionByLevel[MAX_CHARACTERISTIC_LEVEL];
      REAL32         m_StealthByLevel[MAX_CHARACTERISTIC_LEVEL];

      //! Qty of units per SqKM
      REAL32         m_fDensity;



   };//end class GUnitType

};//end namespace combat
#endif //_GOLEM_UNITTYPE_H_
