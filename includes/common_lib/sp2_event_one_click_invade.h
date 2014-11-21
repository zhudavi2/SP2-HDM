/**************************************************************
*
* sp2_event_one_click_invade.h
*
* Description
* ===========
*  Ask the server to invade a country
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_EVENT_ONE_CLICK_INVADE_H_
#define _SP2_EVENT_ONE_CLICK_INVADE_H_

namespace SP2
{
   class GClient;
   class GServer;

   namespace Event
   {      
      class GOneClickInvade :  public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         UINT32 m_iOccupyingCountry;
			UINT32 m_iOccupiedCountry;
         set<UINT32> m_OtherCountriesToDeclareWar;

      protected:
         static SDK::GGameEventSPtr New();

         GOneClickInvade();

         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };
   }
}

#endif // #ifndef _SP2_EVENT_ONE_CLICK_INVADE_H_
