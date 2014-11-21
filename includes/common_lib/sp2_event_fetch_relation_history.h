/**************************************************************
*
* golem_relationhistoryfetch.h
*
* Description
* ===========
*  Describes the interface of class GRelationHistoryFetch
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_RELATIONHISTORYFETCH_H_
#define _GOLEM_RELATIONHISTORYFETCH_H_

namespace SP2
{
   namespace Event
   {
      /*!
      * Game event to fetch the history relation betwen 2 given countries
      **/
      class GRelationHistoryFetch :  public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
      public:
         GRelationHistoryFetch();
         ~GRelationHistoryFetch();

         UINT32         m_iCountryA;
         UINT32         m_iCountryB;
         bool           m_bStartFetch;//!<True if we want to start fetching and receiving values regularly false otherwise
         vector<REAL32> m_vRelationsHistoryValues;
      protected:
      private:
         static SDK::GGameEventSPtr New();
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);


      };

   };
};
#endif //_GOLEM_RELATIONHISTORYFETCH_H_
