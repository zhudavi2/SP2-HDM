/**************************************************************
*
* sp2_unit_production_event.h
*
* Description
* ===========
*  Game events that request to build a certain type of unit (or train infantry)
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_UNIT_PRODUCTION_EVENT_H
#define _GOLEM_SP2_UNIT_PRODUCTION_EVENT_H

namespace SP2
{
   class GUnitProductionEventHandler;

   /*!
   * Enumeration to give the priority of Unit Production, only used when inserting 
   * the request into the production queue
   **/
   namespace EUnitProductionPriority
   {
      enum Enum
      {
         OnHold = 0,
         Low,
         Normal,
         High,
         PriorityCount
      };
   };

   /*!
   * The status of a production request
   **/
   struct  GUnitProductionStatus
   {
      UINT32                           m_iDesignID;
      EUnitProductionPriority::Enum    m_ePriority;
      UINT32                           m_iQuantity;
      UINT32                           m_iDaysLeft;
      UINT32                           m_iRequestID;
      UINT16                           m_iBuildingCountry;
      UINT16                           m_iPayingCountry;
      REAL32                           m_fProgress;
      bool                             m_bIsUnitBeingBuilt;
   };

   namespace Event
   {

      /*!
      * Order to build a new unit
      **/
      class GBuildUnitOrder: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();

      public:
         UINT32                           m_iDesignID;
         EUnitProductionPriority::Enum    m_ePriority;
         UINT16                           m_iBuildingCountryID;
         UINT16                           m_iDestinationCountryID;
         UINT32                           m_iQuantity;
         UINT8                            m_iProductionSpeed;
         

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      protected:
      private:
         GBuildUnitOrder();
         ~GBuildUnitOrder();
      };


      /*!
      * Change a previous given order, either to cancel it or change its priority
      **/
      class GChangeUnitOrder: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:

         //! Order Id
         UINT32                        m_iOrderId;
         //! Cancelation flag
         bool                          m_bCancelOrder;
         //! Priority to update (if order is not canceled)
         EUnitProductionPriority::Enum m_eNewPriority;
         //! Building country
         UINT16                        m_iCountryId;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      protected:
      private:
         GChangeUnitOrder();
         ~GChangeUnitOrder();
      };

      /*!
      * Receive all the information to update the production queue
      **/
      class GUpdateUnitOrderStatus: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:

         //! List of all the poduction request.
         list<GUnitProductionStatus> m_ProductionStatusList;
         REAL32   m_fProductionCapacity;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      /*!
      * Request the production queue
      **/
      class GRequestProductionQueue: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      };


      /*!
      * Request to deploy a given unit
      **/
      class GDeployUnits: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         //! Place where to deploy the unit
         GVector2D<REAL32> m_DeployementCoordinate;

         //! List of Id to deployed 
         list<UINT32>      m_UnitListToDeploy;


         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      /*!
      * Request to cancel a deployement
      **/
      class GCancelDeployement: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         //! List of groups Id to cancel
         list<UINT32>      m_GroupIdListToCancel;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GSplitUnits: public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         UINT32 m_iUnitID;
         INT32  m_iQty;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GRequestForeignBuildingCountryList : public SDK::Event::GGameEvent
      {
      private:

         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;


         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      };

      class GUpdateForeignBuildingCountryList : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         set<UINT32> m_iForeignBuildingCountryList;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GRegisterAsBuildingCountry : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         bool  m_bAcceptForeignCountryOrder;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GTrainUnitRequest : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         list<UINT32> m_UnitIdListToTrain;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GDisbandUnitRequest : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         list<UINT32> m_UnitIdListToDisband;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GRequestForSaleUnitList : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GUpdateForSaleUnitList : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;


         static SDK::GGameEventSPtr New();
      public:

         list<UINT32> m_ForSaleUnitIdList;


         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GTransfertUnit : public SDK::Event::GGameEvent
      {
         // Our best friends. The client and server.
         friend SP2::GClient;
         friend SP2::GServer;

         static SDK::GGameEventSPtr New();
      public:
         
         UINT32 m_iUnitId;
         UINT32 m_iQty;
         bool   m_bForSale;

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_SP2_UNIT_PRODUCTION_EVENT_H
