/**************************************************************
*
* golem_unitproductor.h
*
* Description
* ===========
*  Describes the interface of class GUnitProductor
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_UNITPRODUCTOR_H_
#define _GOLEM_UNITPRODUCTOR_H_


namespace SP2
{
   class GUnitProductor;

   /*!
    * Represents a unit production request
    **/
   class GUnitProductionRequest
   {
      friend class GUnitProductor;      
      friend class GDataControlLayer;

      UINT32                        m_iID;
      UINT32                        m_iQtyWanted;
      UINT32                        m_iQtyShipped;
      REAL32                        m_fQtyShippedAccumulated;
      UINT32                        m_iDesignID;
      UINT32                        m_iBuilderCountryID;
      UINT32                        m_iDestinationCountryID;
      REAL32                        m_fGameTimeInProduction;
      REAL32                        m_fLastUpdateTime;
      REAL32                        m_fBonusFromTech;
      UINT8                         m_iProductionSpeed;
      bool                          m_bUnitIsBeingBuilt;
      EUnitProductionPriority::Enum m_ePriority;

   public:
      GUnitProductionRequest();
      ~GUnitProductionRequest();

   };


   /*!
    * Represents a Country Production Queue
    **/
   class GProductionQueue
   {
   public:
      GProductionQueue();
      list<GUnitProductionRequest> m_ActiveQueue;
      list<GUnitProductionStatus>  m_ProductionStatusList;
      REAL32                       m_fProductionCapacity;
      bool                         m_bProductionStatusIsDirty;
   };

   /*!
    * Manages the unit production for all the countries.
    **/
   class GUnitProductor : public SDK::GGameDataNode
   {
      friend class GDataControlLayer;
   private:
      GUnitProductor();
      ~GUnitProductor();

      //! Initializes the unit productor
      bool Initialize();
      void Shutdown();

      //! Iterate the production of the units
      void Iterate();

      //!Insert the production Request and returns the ID of the request
      INT32 InsertProductionRequest(GUnitProductionRequest& in_Request);

      //! Get a production Request
      bool  GetProductionRequest(UINT32 in_iCountryId, UINT32 in_iProductionRequestId,GUnitProductionRequest& out_ProductionRequest);

      //! Cancel unit production
      bool CancelUnitProduction(UINT32 in_iCountryID, UINT32 in_iProductionRequestID,GUnitProductionRequest& out_RemovedProductionRequest);

      //! Production Percentage
      REAL32 ProductionProgress(UINT32 in_iCountryID,UINT32 in_iRequestID) const;

      //! Updates the position of a unit production request in a production queue
      bool UpdateProductionPriority(UINT32 in_iCountryID,UINT32 in_iRequestID,UINT32 in_iNewPositionFromTop);

      //! Actually ship the units by adding them to the deployment list of a country
      bool   ShipUnits(UINT32 in_iBuilderCountryID,UINT32 in_iDestinationCountryID,UINT32 in_iQty,UINT32 in_iDesignID) const;

      //! Get a Unique Unit Production Request ID
      UINT32 NextProductionRequestID();

      REAL64 CountryMilitaryProduction(UINT32 in_iCountryID) const;      
      //! Unit production queues, one by contry
      vector<GProductionQueue>   m_vProductionQueues;

      void SendProductionQueueToCountry(UINT32 in_iCountryID) ;
      void CleanProductionStatus(void);
      void UpdateProductionStatus(UINT16 in_iCountryId, GUnitProductionRequest &in_ProductionRequest,REAL32 in_fAvailableProductionPoint);

      REAL64                   m_iLastIterationGameTime;
      UINT32                   m_iNextProductionRequestID;

		void ClearProductionForCountry(UINT32 in_iCountryID);

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();
   };


};
#endif //_GOLEM_UNITPRODUCTOR_H_
