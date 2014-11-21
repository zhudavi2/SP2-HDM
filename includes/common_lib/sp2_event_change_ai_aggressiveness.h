/**************************************************************
*
* sp2_event_change_ai_aggressiveness.h
*
* Description
* ===========
*  Event to change the AI Aggressiveness
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CHANGEAIAGGRESSIVENESS_H_
#define _GOLEM_CHANGEAIAGGRESSIVENESS_H_

namespace SP2
{
   namespace Event
   {

      /*!
      * Change the game speed (send to server)
      **/
      class GChangeAIAggressiveness : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
      public:
         GChangeAIAggressiveness();
         ~GChangeAIAggressiveness();

         bool Serialize(GIBuffer& out_Buffer) const;
         bool Unserialize(GOBuffer& in_Buffer);

         const REAL32& AIAggressiveness() const;
         void AIAggressiveness(const REAL32& in_fAIAggressiveness);


      protected:
      private:
         static SDK::GGameEventSPtr New();
         REAL32 m_fNewAIAggressiveness;
      };

   };
};
#endif //_GOLEM_CHANGEAIAGGRESSIVENESS_H_
