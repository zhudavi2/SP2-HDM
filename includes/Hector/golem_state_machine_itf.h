/**************************************************************
*
* golem_statemachineitf.h
*
* Description
* ===========
*  Describes the interface of class GStateMachineItf
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_STATEMACHINEITF_H_
#define _GOLEM_STATEMACHINEITF_H_

namespace Hector
{

   class GStateMachineItf;
   class GState
   {
      friend class GStateMachineItf;
   public:
      GState()              {};
      virtual      ~GState(){};
      virtual void OnEnter(void)    = 0;
      virtual void OnExit(void)     = 0;
      virtual void OnIterate(void)  = 0;

      virtual bool CanTransitTo(INT32 in_iNewState){return true;}

   protected:
      GStateMachineItf* m_pMachine;
   };



   class GStateMachineItf
   {
   public:
      GStateMachineItf();
      virtual ~GStateMachineItf();

      //! Register a state
      void RegisterState(GState *, INT32 in_iStateID);
      void UnregisterState(INT32 in_iStateID);

      //! Gets the actual state
      INT32 ActualState(void) const;

      //! Set the actual state
      bool ChangeState(INT32 in_iStateID);

      //! Iterate
      void Iterate(void);



   protected:
      INT32                               m_iActualState;
      GState*                             m_pActualStateCachedPtr;
      stdext::hash_map<INT32,GState*>     m_mapStates;
   private:
   };
};
#endif //_GOLEM_STATEMACHINEITF_H_
