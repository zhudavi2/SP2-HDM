/**************************************************************
*
* golem_eventperformoffshorebombardment.h
*
* Description
* ===========
*  Describes the interface of class GEventPerformOffshoreBombardment
*
* Owner
* =====
*  mtremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENTPERFORMOFFSHOREBOMBARDMENT_H_
#define _GOLEM_EVENTPERFORMOFFSHOREBOMBARDMENT_H_


namespace SP2
{
   namespace Event
   {

      class GBombardmentNotification : public SDK::Event::GGameEvent
      {
         static SDK::GGameEventSPtr New();
         friend class GClient;
         friend class GServer;

         GBombardmentNotification();
         ~GBombardmentNotification();

      public:
         UINT16                     m_iTargetRegion;
         bool                       m_bStarting;
         vector<GVector2D<REAL32> > m_vBombardedPoints;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      };





      /*!
      * [Class description]
      **/
      class GPerformOffshoreBombardment: public SDK::Event::GGameEvent
      {
         static SDK::GGameEventSPtr New();
         friend class GClient;
         friend class GServer;

         GPerformOffshoreBombardment();
         ~GPerformOffshoreBombardment();

      public:


         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         set<UINT32> m_vBombardingUnitGroups;
         UINT32      m_iTargetRegion;
      protected:
      private:

      };
   };
};

#endif //_GOLEM_EVENTPERFORMOFFSHOREBOMBARDMENT_H_
