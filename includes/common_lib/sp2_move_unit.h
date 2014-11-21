/**************************************************************
*
* sp2_move_unit.h
*
* Description
* ===========
*  Event for Moving Units. Contains only data.
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#pragma once

namespace SP2
{
   class GRequestTroopsMoveHandler;
   namespace Event
   {
      class GRequestTroopsMove :
         public SDK::Event::GGameEvent
      {
      private:
         friend class SP2::GRequestTroopsMoveHandler;
         GRequestTroopsMove(void);
         ~GRequestTroopsMove(void);

      public:
         GVector2D<REAL32>          Destination;
         vector<UINT32>             UnitIDs;
         EMilitaryAction::Flags     Action;

         static SDK::GGameEventSPtr New();

         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };
   }
}
