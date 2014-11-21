/**************************************************************
*
* golem_unitdesign.h
*
* Description
* ===========
*  Describes the interface of class GUnitDesign
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_UNITDESIGN_H_
#define _GOLEM_SP2_UNITDESIGN_H_


namespace SP2
{
   /*!
   * SP2 Design for a Unit
   **/
   class GUnitDesign : public SDK::Combat::Design::GUnit
   {
   public:
      GUnitDesign();
      ~GUnitDesign();

      static   SDK::Combat::Design::GUnit* New();

      UINT32 DesignerID() const;
      void   DesignerID(UINT32);
		INT32 NameId() const;
      void  NameId(INT32);
      UINT8 SpeedAndManeuverability() const;
      void  SpeedAndManeuverability(UINT8);
      UINT8 Sensors() const ;
      void  Sensors(UINT8);
      UINT8 GunRange() const ;
      void  GunRange(UINT8);
      UINT8 GunPrecision() const ;
      void  GunPrecision(UINT8);
      UINT8 GunDamage() const ;
      void  GunDamage(UINT8);
      UINT8 MissilePayload() const ;
      void  MissilePayload(UINT8);
      UINT8 MissileRange() const ;
      void  MissileRange(UINT8);
      UINT8 MissilePrecision() const ;
      void  MissilePrecision(UINT8);
      UINT8 MissileDamage() const ;
      void  MissileDamage(UINT8);
      UINT8 Stealth() const ;
      void  Stealth(UINT8);
      UINT8 CounterMesures() const ;
      void  CounterMesures(UINT8);
      UINT8 Armor() const ;
      void  Armor(UINT8);

      REAL32 Cost() const;
      inline GUnitType* Type() { return m_pUnitType;};
      inline const GUnitType* Type() const {return m_pUnitType;};
      void Type(GUnitType*);

      bool Serialize(GIBuffer& out_Buffer) const;
      bool Unserialize(GOBuffer& in_Buffer);
      
      /*! 
       * Design Variations. First c_iMaxDesignPieceCount are the Variation index, 
       * the last is the color index for all variations
       **/
      struct GDisplay
      {
         short PieceIndex[c_iMaxDesignPieceCount];
         short ColorIndex;
      } m_DesignVariations;

   protected:
   private:

      void UpdateDesignCost();
      REAL32   m_fCost;

      UINT32   m_iDesignerID;                //!<Country ID that designed this
		INT32		m_iNameId;
      UINT8    m_iSpeedAndManeuverability;         
		UINT8    m_iSensors;
      UINT8    m_iGunRange;
      UINT8    m_iGunPrecision;
      UINT8    m_iGunDamage;
      UINT8    m_iMissilePayload;            //!<Missile Qty (1 doesnt mean 1 missile, but means a small qty of missiles)
      UINT8    m_iMissileRange;
      UINT8    m_iMissilePrecision;
      UINT8    m_iMissileDamage;
      UINT8    m_iStealth;
      UINT8    m_iCounterMesures;
      UINT8    m_iArmor;		

      GUnitType*  m_pUnitType;
   };
};

#endif //_GOLEM_UNITDESIGN_H_
