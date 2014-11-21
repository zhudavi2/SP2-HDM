/**************************************************************
*
* sp2_event_plan.h
*
* Description
* ===========
*  Event sent by the advisor to ask the player if he/she would 
*	like to execute some proposed actions
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


#ifndef _GOLEM_EVENT_PLAN_H_
#define _GOLEM_EVENT_PLAN_H_


/*! Type of plan enum:
 * EHE means it's coming from the EHE
 * Specific means it's coming from elsewhere in the code. 
 *	 When receiving Specific plans, it is handled case by case.
 **/
namespace SP2
{   	
   namespace ETypeOfPlan
   {
	   enum Enum
	   {
		   EHE,
		   Specific
	   };
   };

   namespace Event
   {
      /*!
      * Event Sent from the server to the client, to tell him a news has been posted
      **/
      class GSendPlan: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
         			
			UINT32									m_iObjectiveID; //!<Objective id
			vector<pair<UINT32, UINT32> >		m_vActionsID;	//!< first is target country, second is action id
			vector<pair<UINT32, bool> >		m_vObjectiveChanges;	//!< first is objective id, second is bool meaning improvement or not
			ETypeOfPlan::Enum						m_TypeOfPlan;
			bool										m_bAccept;	//!<set to false when a player refused it
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_PLAN_H_
