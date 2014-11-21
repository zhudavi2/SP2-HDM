/**************************************************************
*
* golem_gfx_animation_multiple_itf.h
*
* Description
* ===========
*  Interface of class GFX::GAnimationMultiple, which describes
*  a single animation that animates multiple parameters.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATION_MULTIPLE_ITF_
#define _GOLEM_GFX_ANIMATION_MULTIPLE_ITF_

#include "golem_gfx_animation_itf.h"

namespace GFX
{
   class GAnimationMultiple : public CGFX_Animation_Itf
   {
   protected:
	   GAnimationMultiple(UINT32 in_Type)
		   : CGFX_Animation_Itf(in_Type)
	   {}

   public:
      //! Get the current animation
      virtual CGFX_Animation_Itf* Animation() const = 0;

      //! Set the current animation
      virtual void Animation(CGFX_Animation_Itf* in_pNewAnim) = 0;

      //! Connect the animation to an object parameter by type
      virtual UINT32 AddParamConnection(CGFX_Animated_Obj_Itf* in_pObject, GFX::EParameter_Type in_eParamType) = 0;

      //! Connect the animation to an object parameter by name
      virtual UINT32 AddParamConnection(CGFX_Animated_Obj_Itf* in_pObject, const GString& in_sParamName) = 0;

      //! Connect the animation to an object custom parameter by index
      virtual UINT32 AddParamConnectionCustom(CGFX_Animated_Obj_Itf* in_pObject, UINT32 in_iIndex) = 0;

      //! Removes a parameter connection
      virtual bool   RemoveParamConnection(UINT32 in_iIndex) = 0;

      //! Removes a parameter connection by object (with lookup)
      virtual bool   RemoveParamConnection(CGFX_Animated_Obj_Itf* in_pObject, GFX::EParameter_Type in_eParamType) = 0;

      //! Removes all parameter connections
      virtual void   ClearParamConnections() = 0;

      //! Return the currently connected parameter
      virtual CGFX_Parameter& ConnectedParameter(UINT32 in_iIndex) = 0;

      //! Return the object currently connected to the animation
      virtual CGFX_Animated_Obj_Itf*   ConnectedObject(UINT32 in_iIndex) = 0;

      //! Return the type of the parameter currently connected to the parameter
      virtual GFX::EParameter_Type  ConnectedParameterType(UINT32 in_iIndex) const = 0;

      //! Return the name of the parameter currently connected to the parameter
      virtual const GString&  ConnectedParameterName(UINT32 in_iIndex) const = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_ANIMATION_MULTIPLE_ITF_
