/**************************************************************
*
* sp2_treaty_peace_signed.h
*
* Description
* ===========
*  Game Event sent to AI when a peace treaty has been signed
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


#ifndef _GOLEM_EVENT_TREATY_PEACE_SIGNED_H_
#define _GOLEM_EVENT_TREATY_PEACE_SIGNED_H_

namespace SP2
{
   namespace Event
   {
      class GTreatyPeaceSigned: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			UINT32 CountryA;
			UINT32 CountryB;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_TREATY_PEACE_SIGNED_H_