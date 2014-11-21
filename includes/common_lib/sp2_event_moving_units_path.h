/**************************************************************
*
* golem_event_moving_unit_path.h
*
* Description
* ===========
*  Describes the interface of class GMovingUnitPath
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_MovingUnitPath_H_
#define _GOLEM_MovingUnitPath_H_

namespace SP2
{
   namespace Event
   {

      /*!
      * Event sent from the server to the client to give
      * information on unit moves.
      **/
      class GMovingUnitPath : public SDK::Event::GGameEvent
      {
      private:

         //Members to give information on the unit
         UINT32         m_iId;
         UINT32         m_iOwner;
         UINT32         m_iDesign;
         UINT32         m_iQty;
         UINT32         m_iHP;
         SDK::Combat::GPath  m_Path; // Key: Time, Val: Coordinate


         GMovingUnitPath();
      protected:
      public:

         ~GMovingUnitPath();

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         bool Set(UINT32       in_iID,
            UINT32       in_iOwner,
            UINT32       in_iDesign,
            UINT32       in_iQty,
            UINT32       in_iHP,
            const SDK::Combat::GPath& in_Path);

         //! GMovingUnitPath construction function
         static SDK::GGameEventSPtr New();

      };


   }; //End of namespace Event
}; //End namespace SP2::Event

#endif //_GOLEM_MovingUnitPath_H_
