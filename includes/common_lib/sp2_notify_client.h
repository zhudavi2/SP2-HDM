/**************************************************************
*
* golem_NotifyClient.h
*
* Description
* ===========
*  Describes the interface of class GNotifyClient
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_NotifyClient_H_
#define _GOLEM_NotifyClient_H_


namespace SP2
{

   namespace Event
   {

      /*!
      * Event sent from the server to the client to notify a client that
      * something special has happened
      **/
      class GNotifyClient : public SDK::Event::GGameEvent
      {
      private:
         REAL32  m_fLatitude;
         REAL32  m_fLongitude;
         INT32   m_iTargetRegion;
         INT32   m_iTargetCountry;
         GString m_sMsg;



         GNotifyClient();
      protected:
      public:

         ~GNotifyClient();

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);


         GVector2D<REAL32> Coords(){return GVector2D<REAL32>(m_fLongitude,m_fLatitude);}
         GString Msg(){return m_sMsg;}

         //Set the data about the notify event
         void Set(const REAL32&  in_fLatitude,
            const REAL32&  in_fLongitude,
            const INT32&  in_iTargetRegion,
            const INT32&  in_iTargetCountry,
            const GString& in_sMsg);

         //! GNotifyClient construction function
         static SDK::GGameEventSPtr New();
      };


   }; //End of namespace Event
}; //End of namespace SP2

#endif //_GOLEM_NotifyClient_H_
