#ifndef _EHE_ACTIONS_BUILD_NEW_UNITS
#define _EHE_ACTIONS_BUILD_NEW_UNITS


namespace EHEActions
{


class GBuildNewUnits : public SDK::EHE::GAction
{   
	// object must be cloneable (deep copy) for registration in the manager
   GBuildNewUnits* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	//! Returns whether the action is ready to be executed or not.
	bool ReadyToStepForward() const;

private:
	TIME_SPAN m_fDuration;
	UINT32 m_iUnitInProductionId;	//Id of the unit currently in production	
};


};

#endif