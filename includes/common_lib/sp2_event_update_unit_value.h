/**************************************************************
*
* sp2_event_update_unit_value.h
*
* Description
* ===========
*  Describes the interface of class GUpdateUnitValue
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_UPDATE_UNIT_VALUE_H_
#define _SP2_EVENT_UPDATE_UNIT_VALUE_H_


namespace SP2
{
   class GClient;
   class GServer;
   namespace Event
   {
      /*!
       * Game event used to indicate which country need to update its unit values.
       **/
      class GUpdateUnitValue :  public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New
      public:
         //! List of countries that need to update their unit values
         hash_set<UINT32>              m_DirtyCountryUnitValue;

      protected:
         static SDK::GGameEventSPtr New();

         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };

   }

}

#endif //_SP2_EVENT_UPDATE_UNIT_VALUE_H_
