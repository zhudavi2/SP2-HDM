/**************************************************************
*
* sp2_window_constitutional_form_update.h
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

#ifndef _GOLEM_EVENT_CONSTITUTIONAL_FORM_UPDATE_H_
#define _GOLEM_EVENT_CONSTITUTIONAL_FORM_UPDATE_H_

namespace SP2
{
   class GPoliticEventHandler;
   namespace Event
   {
      class GConstitutionalFormUpdate: public SP2::Event::GRequestAckGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT64 NextElection;

         UINT32 CapitalId;
         INT32 GvtType;
         INT32 CountryId;

         bool  GetData;
         bool  MartialLaw;

      protected:
      private:
         GConstitutionalFormUpdate();
         ~GConstitutionalFormUpdate();

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_CONSTITUTIONAL_FORM_UPDATE_H_
