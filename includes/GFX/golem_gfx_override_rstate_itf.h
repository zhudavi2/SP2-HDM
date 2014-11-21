/**************************************************************
*
* golem_gfx_override_rstate_itf.h
*
* Description
* ===========
*	Class GFX::GOverrideRState definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
**************************************************************/
#ifndef	_GOLEM_GFX_OVERRIDE_RSTATE_ITF_
#define	_GOLEM_GFX_OVERRIDE_RSTATE_ITF_

#include "golem_gfx_override_itf.h"
#include "golem_gfx_render_state_collection_itf.h"

namespace GFX
{
   //! Override Render State class
   /*!
   * This class describes an override for one or more render state. It can be used to
   * modify the effect of a branch material render state for a Model.
   *
   * The render state override can be used to modify base material render states.
   */
   class GOverrideRState : public GOverride
   {
   protected:
	   GOverrideRState( UINT32 in_Type )
		   : GOverride( in_Type )
	   {}

   public:
      //! Remove all overrides from this object
      virtual bool ClearAllOverrides() = 0;

      //! Get number of parameter overrides on this object
      virtual UINT32 NbRStateOverrides() = 0;

	   //! Add an override to a render state
      virtual bool AddOverrideOnRState(GFX::ERenderStateType in_Type) = 0;

	   //! Remove an override to a render state
      virtual bool RemoveOverrideOnRState(GFX::ERenderStateType in_Type) = 0;

      //! Get the overriden render state value
      virtual UINT32& OverridenRState(GFX::ERenderStateType in_Type) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_OVERRIDE_RSTATE_ITF_
