/**************************************************************
*
* sp2_request_manager.h
*
* Description
* ===========
*   Handle acknowldgements requests objects 
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_RequestManager_H_
#define _CSP2_RequestManager_H_

namespace SP2
{
   // Forward declarations
   class GRequestManager;

   /*!
   * Base class for objects wich will handle acknowledgement request
   **/
   class GRequestObject
   {
      friend class GRequestManager;
   public:
      virtual UINT64 RequestAction()  const;
   protected:
      //! constructor
      GRequestObject();
      //! virtual destructor
      virtual ~GRequestObject(){};
      //! is there a request pending ?
      virtual bool RequestPending() const;
      //! called when the acknowledge is received
      virtual void ReceiveRequest();
      //! Cancel a request
      virtual void RemoveRequest();
   private:
      UINT64 m_iRequestID;
   };

   /*!
   * The request manager wich will handle every request from the generation
   * to the handling of the events wich are coming from the server
   **/
   class GRequestManager
   {
   public:
      //! constructor
      GRequestManager() {ClearAll();}
      //! virtual destructor
      virtual ~GRequestManager(){};
      //! free requests in the manager
      void ClearAll();  
      //! remove a request in the manager through its id
      void RemoveRequest(UINT64 in_iRID);
      //! remove a request in the manager through its obj
      void RemoveRequest(SP2::GRequestObject* in_pRequest);
      //! add a new request in the manager
      UINT64 AddRequest(SP2::GRequestObject* in_pRequest);
      UINT64 AddRequest(SP2::GRequestObject* in_pRequest, 
                        SP2::Event::GRequestGameEvent* in_pRequestEvent);
      //! event handler from the server
      void HandleAnswerAcknowledgement(SDK::GGameEventSPtr in_Event);
   protected:      
      //! retrieve the next unique id of our request
      UINT64 GetNextRID();
   private:         
      //! special typedef for a request list
      typedef hash_map<UINT64, SP2::GRequestObject*> GRequestsList;
      //! our requests list
      GRequestsList m_Requests;  
      //! unique id used for our request
      UINT64        m_uiUniqueRID;
   };
};

#endif //_CSP2_RequestManager_H_