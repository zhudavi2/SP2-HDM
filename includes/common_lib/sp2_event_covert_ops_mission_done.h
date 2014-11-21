/**************************************************************
*
* sp2_event_covert_ops_mission_done.h
*
* Description
* ===========
*  Sends the information for after a covert ops mission is done
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "sp2_covert_actions_cell.h"

#ifndef _GOLEM_EVENT_COVERT_MISSION_DONE_H_
#define _GOLEM_EVENT_COVERT_MISSION_DONE_H_

namespace SP2
{   
   namespace Event
   {
      /*!
      * Event Sent from the server to the client, to tell him a covert ops mission has been executed
      **/
      class GSendCovertOpsMissionDone: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			UINT32											m_iCellID;
			UINT32											m_iCellSourceCountry;
			GString											m_sCellName;
			ECovertActionsMissionType::Enum			m_eType;
			ECovertActionsTargetSector::Enum			m_eTargetSector;
			ECovertActionsMissionComplexity::Enum	m_eComplexity;
			INT32												m_iResourceOrUnitType;
			UINT32											m_iTargetCountry;
			UINT32											m_iFramedCountry;
			bool												m_bSuccessful;
			bool												m_bCaptured;
			bool												m_bKnowAttacker;

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_COVERT_MISSION_DONE_H_
