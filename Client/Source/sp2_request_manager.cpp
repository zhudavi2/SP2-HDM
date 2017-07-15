/**************************************************************
*
* sp2_request_manager.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

// GRequestAcknowledgement class
// constructor
GRequestObject::GRequestObject()
{
   m_iRequestID = SP2::Event::NO_REQUEST;
}

/*!
 * No more request pending
 **/
void GRequestObject::ReceiveRequest()
{
   m_iRequestID = SP2::Event::NO_REQUEST;
}

/*!
 * Cancel a pending request
 **/
void GRequestObject::RemoveRequest()
{
   m_iRequestID = SP2::Event::NO_REQUEST;
}

/*!
 * Retrieve the id of our request
 **/
UINT64 GRequestObject::RequestAction() const
{
   return m_iRequestID;
}

/*!
 * Is our request currently waiting for an acknowledgement ?
 **/
bool GRequestObject::RequestPending() const
{
   // if we have an id it means we are waiting 
   return (m_iRequestID != SP2::Event::NO_REQUEST);
}


// GRequestManager class
/*!
 * empty the requests list and reset the primary key
 **/
void GRequestManager::ClearAll()
{
   // empty the requests list
   m_Requests.clear();
   m_uiUniqueRID = 0;
}

/*!
 * remove every requests related to a specific request acknowledgement
 **/
void GRequestManager::RemoveRequest(SP2::GRequestObject* in_pRequest)
{   
   GRequestsList::iterator l_it = m_Requests.begin();
   // loop into our hash_map and remove every associated object
   while(l_it != m_Requests.end())
   {
      // objects are equal ?
      if(l_it->second == in_pRequest)
      {
         // keep a ref to our iterator and remove it
         GRequestsList::iterator l_RemoveIt = l_it++;
         l_RemoveIt->second->RemoveRequest();
         m_Requests.erase(l_RemoveIt);
      }
      else
         l_it++;
   }
}

/*!
 * remove a request with a specific id
 **/
void GRequestManager::RemoveRequest(UINT64 in_iRID)
{
   // retrieve our unique id
   GRequestsList::iterator l_it = m_Requests.find(in_iRID);
   // if presents, remove it
   if(l_it != m_Requests.end())
   {
      l_it->second->RemoveRequest();
      m_Requests.erase(l_it);
   }
}

/*!
 * add a new request to the request list and returns the id
 **/
UINT64 GRequestManager::AddRequest(SP2::GRequestObject* in_pRequest)
{
   gassert(in_pRequest, "cant add a null acknowledgement request in manager !");
   // get a new unique id for our request
   UINT64 l_uiNewID          = GetNextRID();
   // assign this new id to our acknowledgement
   in_pRequest->m_iRequestID = l_uiNewID;
   // add it into the hash map
   m_Requests[l_uiNewID]     = in_pRequest;
   // return the unique id
   return l_uiNewID;
}

/*!
 * add a new request to the request list and returns the id
 **/
UINT64 GRequestManager::AddRequest(SP2::GRequestObject* in_pRequest, 
                                   SP2::Event::GRequestGameEvent* in_pRequestEvent)
{
   gassert(in_pRequestEvent, "cant add a null acknowledgement request in manager !");
   // set the request id to our event
   in_pRequestEvent->m_iRequestID = AddRequest(in_pRequest);
   // return the unique id
   return in_pRequestEvent->m_iRequestID;
}

/*!
 * Event Handler in client to receive acknowledgemnet requests
 **/
void GRequestManager::HandleAnswerAcknowledgement(SDK::GGameEventSPtr in_Event)
{
   // retrieve specific event data
   SP2::Event::GAnswerAckGameEvent* l_pEvent = (SP2::Event::GAnswerAckGameEvent*)in_Event.get();
   // receive our acknowledgement through its id
   UINT64 l_iRID = l_pEvent->m_iRequestID;   
   // the request could have been removed by someone else,
   // does it still exists ??
   if(m_Requests.count(l_iRID) > 0)
   {
      // Acknowledge received
      m_Requests[l_iRID]->ReceiveRequest();
      // remove the request from the list
      m_Requests.erase(l_iRID);
   }
}

UINT64 GRequestManager::GetNextRID() 
{  
   // increment
   ++m_uiUniqueRID;
   // be sure to never have 0
   if(m_uiUniqueRID == 0) m_uiUniqueRID = 1;
   // return it
   return m_uiUniqueRID;
}
