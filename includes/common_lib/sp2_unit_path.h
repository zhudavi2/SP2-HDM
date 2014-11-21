/**************************************************************
*
* sp2_unit_path.h
*
* Description
* ===========
*  Describes the interface of class GUnitPath
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_SP2_UNIT_PATH_H_
#define _GOLEM_SP2_UNIT_PATH_H_

namespace SP2
{
   struct GUnitPathPoint
   {
      GVector2D<REAL32> m_Position;
      REAL32            m_fDistance;
      UINT32            m_iRegionID;
   };

   /*!
    * Representation of a SP2 unit path
    **/
   class GUnitPath
   {
      struct GPointDelta
      {
         GVector2D<REAL32> m_fDeltaPos;
         REAL64            m_fDeltaDistInv;
      };

   public:
      GUnitPath();

      //! Return the ID of the path, which changes each time the path is reseted
      UINT32 Id() const { return m_iId; }

      //! Resets the path, which cleans all data structures of the path
      void Reset(UINT32 in_iProjetedNbElems = 0);

      //! Add a new point to the path
      void AddPoint(REAL32 in_fDistance, const GVector2D<REAL32>& in_Position, UINT32 in_iRegionID);

      //! Get the content of the whole path
      const vector<GUnitPathPoint>& Points() const;

      //! Set the speed of the path
      void Speed(REAL32 in_fNewSpeed, REAL32 in_fCurrentTime);

      //! Get the speed of the path
      REAL32 Speed() const { return m_fSpeed; }

      //! Get the start time of the whole path
      REAL32 TimeStart() const { return m_fStartTime; }

      //! Get the end time of the whole path
      REAL32 TimeEnd() const { return m_fSpeed ? m_fStartTime + (REAL32) (m_vPoints.back().m_fDistance * m_fSpeedInv) : FLT_MAX; }

      //! Get the current index in the path
      //! If negative, the path is not yet effective
      //! If equal to the path size - 1, the path is over
      INT32 PointIndex() const;

      //! Update the path at the specified time
      //! Returns true if path is complete
      bool Update(REAL32 in_fTime, GVector2D<REAL32>& out_Position);

      void SetupProtection(REAL32 in_fStart, REAL32 in_fEnd);

      void UpdateProtection(REAL32 in_fEnd);

      //! Return the distance walked at the specified time1
      REAL32 Distance(REAL32 in_fTime) const
      {
         return (in_fTime - m_fStartTime) * m_fSpeed + m_fStartDist;
      }

      REAL32 ProtectionStart() const { return m_fProtectionStart; }
      REAL32 ProtectionEnd() const { return m_fProtectionEnd; }

      bool IsProtected(REAL32 in_fTime) const 
      {
         REAL32 l_fDist = Distance(in_fTime);
         return (l_fDist > m_fProtectionStart) && (l_fDist <= m_fProtectionEnd);
      }

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);

   private:
      vector<GUnitPathPoint>  m_vPoints;
      vector<GPointDelta>     m_vPointDeltas;
      INT32                   m_iIndex;
      UINT32                  m_iId;

      REAL32                  m_fSpeedInv;
      REAL32                  m_fSpeed;
      REAL32                  m_fStartTime;
      REAL32                  m_fStartDist;

      REAL32                  m_fProtectionStart;
      REAL32                  m_fProtectionEnd;
   };

}

#endif //_GOLEM_SP2_UNIT_GROUP_H_
