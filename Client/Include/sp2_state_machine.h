/**************************************************************
*
* sp2_state_machine.h
*
* Description
* ===========
*  Describes the interface of class GStateMachine
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_STATEMACHINE_H_
#define _GOLEM_STATEMACHINE_H_

namespace SP2
{
   class GState;
   typedef GSmartPtr<GState> GStateSPtr;

   /*!
    * Generic state machine, which uses events to go from state to state
    **/
   class GStateMachine
   {
   public:
      //! Constant returned when event or iteration does not change state
      static const UINT32 c_iCurrentState = 0;

      //! Constant returned when event or iteration completely ends the state machine
      static const UINT32 c_iTerminate = 0xFFFFFFFF;

      GStateMachine();

      void AddState(GStateSPtr in_pState);

      UINT32 CurrentState() const;

      GStateSPtr State(UINT32 in_iID);

      void SendEvent(UINT32 in_iEvent);

      void Start(UINT32 in_iStartState);

      bool IsStarted() const;

      void Iterate();

   private:
      GState* ChangeState(UINT32 in_iNewState, GState* in_pCurState);

      hash_map<UINT32, GStateSPtr>  m_pStateObjs;

      UINT32                     m_iCurState;

      GMutex                     m_EventMutex;

      list<UINT32>               m_vPendingEvents;
   };

   /*!
    * State of a state machine which transits on iteration or on specific events
    **/
   class GState
   {
   public:
      //! Should return a unique ID
      virtual UINT32 ID() = 0;

      //! Respond to an event (by changing state or not)
      virtual UINT32 OnEvent(UINT32 in_iEventID) { return GStateMachine::c_iCurrentState; }

      //! Called when state is about to be current in a state machine
      virtual void OnEnter() { ; }

      //! Called at each state machine iteration while the state is the current one
      virtual UINT32 OnIterate() { return GStateMachine::c_iCurrentState; }

      //! Called when state is about to stop being current in a state machine
      virtual void OnLeave() { ; }
   };

   /*!
    * Class that describes a state that contains a state machine,
    * which can be used to do hierarchical state machines
    **/
   class GSubStateMachine : public SP2::GState, protected GStateMachine
   {
   public:
      //! Implemented to indicate in which sub-state to start the sub FSM
      virtual UINT32 StartSubState() const = 0;

      //! Must be called by state implementation to iterate on the sub state machine
      virtual UINT32 OnEvent(UINT32 in_iEventID)
      {
         SendEvent(in_iEventID);

         return __super::OnEvent(in_iEventID);
      }

      //! Must be called by state implementation to iterate on the sub state machine
      virtual void OnEnter()
      {
         Start(StartSubState() );
      }

      //! Must be called by state implementation to iterate on the sub state machine
      virtual UINT32 OnIterate()
      {
         // Iterate on current sub-states
         Iterate();

         return c_iCurrentState;
      }

      //! Must be called by state implementation to iterate on the sub state machine
      virtual void OnLeave()
      {
         if(CurrentState() )
         {
            // Make sure sub state leaves
            State(CurrentState() )->OnLeave();
         }
      }
   };
}

#endif // #ifndef _GOLEM_STATEMACHINE_H_
