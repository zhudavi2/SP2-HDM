/**************************************************************
*
* golem_ehe_action.h
*
* Description
* ===========
*  An Action is something a country can do to fullfill its objectives. 
*	Each Action derived from the Action class has its own Execute function, 
*	which describes what this action does.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_ACTION
#define _EHE_ACTION


#include "golem_ehe_globals.h"


namespace SDK
{
   namespace EHE
   {


      class GOLEM_SDK_API GAction
      {
         friend class GManager;
         friend class GEntity;
         friend class GObjective;

      public:
         // constructors / destructors
         GAction();
         GAction( ACTION_ID in_iID, const GString& in_sName );
         virtual ~GAction();

         // object must be cloneable (deep copy) for registration in the manager
         virtual GAction* Clone() const = 0;

         // clear contents
         virtual void Clear();

         // get Id number
         ACTION_ID Id() const;

         // set Id number
         void Id( ACTION_ID in_iID );

			// get Strength
         STRENGTH Strength() const;

         // set Strength value
         void Strength( STRENGTH in_fStrength );

         // get name
         const GString& Name() const;

         // set name
         void Name( const GString& in_sName );

			//! get starting time
         REAL64 StartingTime() const;

         //! set starting time
         void StartingTime( REAL64 in_fStartingTime );

         // get string that describes the action (for debugging purposes of course)
         virtual GString Description() const;

         //! sets target for that action
         void Target( ENTITY_ID in_iTarget );

			//! Returns the target for that action
			ENTITY_ID Target() const;

			//! set source for the action
         void Source( ENTITY_ID in_iSource );

			//! Returns the source for that action
			ENTITY_ID Source() const;

			//! Finds the target for that action
			//! default returns the source entity
			virtual ENTITY_ID FindTarget() const;

         // let the entity execute the action on the specified target with the given strength
         virtual void Execute() = 0;

         // get all the action's conditions
         // (default implementation returns empty list)
         virtual void Conditions( vector<GCondition>& out_Conditions ) const;

			//! Returns true if hard conditions are met. Hard conditions differ from normal conditions,
			//! in a way that if a hard condition isn't met, he doesn't try to make a plan to reach it. 
			//! For example, if the action RaiseTax has a hard condition of having a tax% smaller than the maximum limit
			//! the AI will not try to lower the tax, to later raise it again. 
         virtual bool CheckHardConditions() const;

			//! Returns whether the action is ready to be executed or not.
			//! Return true by default
			virtual bool ReadyToStepForward() const;

      protected:
         ACTION_ID      m_iId;         // id number
         GString        m_sName;       // name
			ENTITY_ID		m_iSource;		// source
			ENTITY_ID		m_iTarget;		// target
			STRENGTH			m_fStrength;	// strength
			REAL64			m_fStartingTime;	// starting time of the action
      };
   }
};

#endif