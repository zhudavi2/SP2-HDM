/**************************************************************
*
* golem_gameover.h
*
* Description
* ===========
*  Describes the interface of class GGameOver
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GAMEOVER_H_
#define _GOLEM_GAMEOVER_H_

namespace SP2
{
   namespace Event
   {

     /*!
      * [Class description]
      **/
      class GGameOver: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:
         GGameOver();
         ~GGameOver();

         bool Serialize(GIBuffer& io_Obj)const;
         bool Unserialize(GOBuffer&);

         //Used when sent from server to client saying if game can continue
         bool                   m_bCanContinue;
         //Used when sent from client to server saying if game is continued or not.
         bool                   m_bWillContinue;
         bool                   m_bAllObjectivesCompletedSuccessfully;
         UINT32                 m_iWinnerCountry;

         //Used when the game is over for somebody without possibility to extend
         UINT32                 m_iTerminatedCountry;

         vector<GGameObjective> m_vCompletedObjectives;
         vector<GGameObjective> m_vGameOverObjectives;
         vector<GGameObjective> m_vGameTimedOutObjectives;

      protected:
      private:

      };

      class GObjectiveCompleted: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:
         GObjectiveCompleted();
         ~GObjectiveCompleted();

         bool Serialize(GIBuffer& io_Obj)const;
         bool Unserialize(GOBuffer&);

         UINT32                 m_iCompletedObjectivesID;
         EObjectiveStatus::Enum m_eStatus;
      protected:
      private:

      };
   };
};
#endif //_GOLEM_GAMEOVER_H_
