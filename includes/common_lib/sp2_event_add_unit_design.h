/**************************************************************
*
* sp2_event_add_unit_design.h
*
* Description
* ===========
*  Game events that have something to do with starting the game
*  (requesting to start the game and starting it)
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_ADD_UNIT_DESIGN_H_
#define _GOLEM_EVENT_ADD_UNIT_DESIGN_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GAddUnitDesign: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString m_sName;
         INT16 m_iDesignerId;
         UINT8 m_iArmor;
         UINT8 m_iCountermeasures;
         UINT8 m_iGunDamage;
         UINT8 m_iGunRange;
         UINT8 m_iGunPrecision;
         UINT8 m_iMissileDamage;
         UINT8 m_iMissilePayload;
         UINT8 m_iMissilePrecision;
         UINT8 m_iMissileRange;
         UINT8 m_iSensors;
         UINT8 m_iSpeed;
         UINT8 m_iStealth;
         UINT8 m_iUnitType;
         SP2::GUnitDesign::GDisplay m_Variations;
         

      protected:
      private:
         GAddUnitDesign();
         ~GAddUnitDesign();



      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_ADD_UNIT_DESIGN_H_
