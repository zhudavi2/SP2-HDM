/**************************************************************
*
* sp2_window_politic_update.h
*
* Description
* ===========
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_POLITIC_UPDATE_H_
#define _GOLEM_EVENT_POLITIC_UPDATE_H_

namespace SP2
{
   class GPoliticEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GPoliticUpdate: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         vector<INT32> GvtStrIds;

         INT16    CountryId;
         INT32    GvtType;
         REAL32   PoliticalIdeology;
         REAL32   GvtApproval;
         REAL32   GvtStability;
         REAL32   CorruptionLevel;
			REAL32	Pressure;
         bool     MartialLaw;

      protected:
      private:
         GPoliticUpdate();
         ~GPoliticUpdate();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_POLITIC_UPDATE_H_
