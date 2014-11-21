#include "golem_sdk_api.h"
/************************************************
* File: database_game_events
* Owner: Mathieu Tremblay, Francois Durand
* Description:	
*		Holds the game events used to perform a query on the database
*		via the network.
* History:
* ========
*	4 juin 2003  		Creation
*
*************************************************/

#ifndef _DATABASE_GAME_EVENTS_H_
#define _DATABASE_GAME_EVENTS_H_

namespace SDK
{
   class GJoshua;
   namespace Event
   {
      /*!
      * Define a game event that is used to Send database queries via the engine to the server.
      * The Set_Query function must be called to set the query, and then sent to the server via the
      * Raise_Event function of GJoshua.
      *
      *	Make sure that the member m_iSource of the query is set to the Id of the sending person, so the
      *	result comes back correctly.
      **/
      class GOLEM_SDK_API GEventDBQuery : public GGameEvent
      {
         friend class GDefaultHandler;
         friend class GJoshua;

         static GGameEventSPtr New();
         static UINT32 m_NextQueryID;

         UINT32	m_iQueryID;   
         GIBuffer m_Queries;   
         GOBuffer m_OutQueries;
         UINT32   m_iQueryCount;

         GEventDBQuery(UINT32);
         GSString FetchQueryString();

      public:
         //! Sets the database data
         void AddQuery(const GSString &);   

         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         UINT32 QueryID() {return m_iQueryID;}
         bool m_bAsync;
      };


      /*!
      * Defines a result to a database query.  The Process function of the GEventDBQuery class,
      * once it has processed the query, creates a game event of this type to answer back.
      * When it is processed, it calls the game client to see if there is a registered callback
      * to the query.  If so, the function is called, otherwise, nothing is done.
      **/
      class GOLEM_SDK_API GEventDBQueryResult : public GGameEvent
      {
         friend class GDefaultHandler;
         friend class GJoshua;      

         static GGameEventSPtr New();
         UINT32	m_iQueryID;

      protected:
         EError   m_iErrorCode;
         UINT32   m_iResultCount;   
         GIBuffer m_Results;
         GOBuffer m_OutResults;

         GEventDBQueryResult(UINT32);

      public:   

         struct GOLEM_SDK_API GDBQueryResultHolder
         {      
            friend class GDefaultHandler;
            EError ErrorCode() const;
            UINT32 ResultCount() const;
            UINT32 QueryID() const;
            bool   FillTables(vector<GTable *> &);
            bool   FillTable(GTable &);

         protected:
            GDBQueryResultHolder(GGameEventSPtr);      

         private:
            GGameEventSPtr m_Event;      
         };

         bool m_bAsync;

         bool   FillTables(vector<GTable *> &);
         bool   FillTable(GTable &);

         void  AddQueryResult(GTable &);
         void  ResultCode(EError);

         UINT32 QueryID() {return m_iQueryID;}

         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };
   }
}
#endif //_DATABASE_GAME_EVENTS_H_
