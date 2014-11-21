/**************************************************************
*
* golem_changegamespeed.h
*
* Description
* ===========
*  Describes the interface of class GChangeGameSpeed
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CHANGEGAMESPEED_H_
#define _GOLEM_CHANGEGAMESPEED_H_

namespace SP2
{
   namespace Event
   {

      /*!
      * Change the game speed (send to server)
      **/
      class GChangeGameSpeed : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
      public:
         GChangeGameSpeed();
         ~GChangeGameSpeed();

         bool Serialize(GIBuffer& out_Buffer) const;
         bool Unserialize(GOBuffer& in_Buffer);

         const REAL64& NewSpeed() const;
         void NewSpeed(const REAL64& in_fSpeed);


      protected:
      private:
         static SDK::GGameEventSPtr New();
         REAL64 m_fNewSpeed;
      };

   };
};
#endif //_GOLEM_CHANGEGAMESPEED_H_
