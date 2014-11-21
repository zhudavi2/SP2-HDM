/**************************************************************
*
* golem_nuclearmissile.h
*
* Description
* ===========
*  Describes the interface of class GNuclearMissile
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_NUCLEARMISSILE_H_
#define _GOLEM_NUCLEARMISSILE_H_


namespace SP2
{
   class  GUnitGroup;
   struct SCitiesInfo;

   namespace ENuclearMissileType
   {
      enum Enum
      {
         Standard,
         OnSubmarines,
         All,
         None
      };
   };


   //! Defines the possible targets of a nuke missile
   namespace ENuclearTargetType
   {
      enum Enum
      {
         Civil,
         Military,
         Specific,
         Undefined
      };
   };

   /*!
    * Nuclear Launch Request
    **/
   class GNukeLaunchRequest : GSerializable
   {
   public:
      GNukeLaunchRequest();
      ~GNukeLaunchRequest();

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      UINT32                     m_iAttackerID;
      UINT32                     m_iTargetID;
      GVector2D<REAL32>          m_TargetCoordinates;
      UINT32                     m_iMissileID;
      ENuclearTargetType::Enum   m_TargetType;
   };




   /*!
   * Logical representation of a nuke
   **/
   class GNuclearMissile : GSerializable
   {
   public:
      GNuclearMissile();
      ~GNuclearMissile();

      UINT32            m_iID;
      UINT32            m_iOwnerID;
      UINT32            m_iDesignID;
      UINT32            m_iQty;
      GVector2D<REAL32> Position() const;
      void              Position(const GVector2D<REAL32>& in_Position){m_Position = in_Position;}

      bool              FromSubmarine() const {return m_bFromSubmarine;}
      void              FromSubmarine(bool in_bFromSub){m_bFromSubmarine = in_bFromSub;}
      UINT32            AttachedSubmarine() const {return m_iAttachedSubmarineUnit;}
      void              AttachedSubmarine(UINT32 in_iAttachedSub){m_iAttachedSubmarineUnit = in_iAttachedSub;}


      //! Get the distance in KMs where the missiles does 100% damage
      REAL32 FullDamageRange() const;

      REAL32 RangeKMs() const;
      UINT32 Megatons() const;

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);
   protected:
   private:
      GVector2D<REAL32> m_Position;
      bool              m_bFromSubmarine;
      UINT32            m_iAttachedSubmarineUnit;
   };



   /*!
    * Defines a nuclear target
    **/
   class GNukeTarget
   {
   public:
      ENuclearTargetType::Enum m_TargetType;


      GNukeTarget()
      {
         m_TargetType         = ENuclearTargetType::Undefined;
         m_iTargetScore       = 0;
         m_iCityID            = 0;
         m_iTargetCountryID   = 0;
         m_Position           = GVector2D<REAL32>(0,0);
      }

      union
      {
         UINT32  m_iUnitGroupID;
         UINT32  m_iCityID;
      };
      UINT32 m_iTargetScore;
      union
      {
         UINT32 m_iTargetCountryID;
         UINT32 m_iTargetRegionID;
      };

      bool operator <(const GNukeTarget& in_Target) const
      {
         return ( m_Position.x + m_Position.y + m_iTargetCountryID*m_iUnitGroupID) < (in_Target.m_Position.x + in_Target.m_Position.y + in_Target.m_iTargetCountryID*in_Target.m_iUnitGroupID);
      }

      bool operator >(const GNukeTarget& in_Target) const
      {
         return ( m_Position.x + m_Position.y + m_iTargetCountryID*m_iUnitGroupID) > (in_Target.m_Position.x + in_Target.m_Position.y + in_Target.m_iTargetCountryID*in_Target.m_iUnitGroupID);
      }

      bool operator ==(const GNukeTarget& in_Target) const
      {
         if(Position() == in_Target.Position())
         {
            return true;
         }
         else
            return false;
      }

      operator size_t() const {return (size_t)*(INT32 *)this;}//return (size_t)( (m_iUnitGroupID << 16) + m_iTargetCountryID); }

      GVector2D<REAL32> Position() const;

      //! \TODO check if it can fit the union
      GVector2D<REAL32> m_Position;
/*
      UINT32 EstimatedCasualtiesCivil(const GNuclearMissile&);
      UINT32 EstimatedCasualtiesMilitary(const GNuclearMissile&);
*/

   };


   /*!
    * Define a nuke once launched
    **/
   class GLaunchedNuke : GSerializable
   {
   public:
      GNuclearMissile   m_Missile;
      REAL64            m_fLaunchClock;
      REAL64            m_fHitClock;
      GVector2D<REAL32> m_TargetPosition;
      REAL64            m_fInterceptionClock;
      UINT8             m_iAMDSIntercepted;  //!< 0 not intercepted, 1 long range interception, 2 short range interception
      UINT8             m_iAMDSLaunched;     //!< 0 no AMDS, 1 long range AMDS, 2 short range AMDS, 3 long & short range AMDS

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);




   };


};
#endif //_GOLEM_NUCLEARMISSILE_H_
