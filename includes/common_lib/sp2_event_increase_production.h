/**************************************************************
*
* sp2_event_increase_production.h
*
* Description
* ===========
*  Event generated when increase production is done
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_INCREASE_PROD_H_
#define _SP2_EVENT_INCREASE_PROD_H_

namespace SP2
{
   class GClient;
   class GServer;

   namespace Event
   {
      /*!
       * Game event used to send updates on data from the server to the
       * client so the client will be able to cache the data instead of
       * always request it.
       **/
      class GIncreaseProduction :  public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         INT32  m_iResourceID;
         REAL64 m_fIncrease;

      protected:
         static SDK::GGameEventSPtr New();

         GIncreaseProduction();

         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };
   }
}

#endif // #ifndef _SP2_EVENT_INCREASE_PROD_H_
