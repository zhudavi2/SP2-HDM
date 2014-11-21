/**************************************************************
*
* golem_gfx_override_material_itf.h
*
* Description
* ===========
*	Class CGFX_Override_Material_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
**************************************************************/
#ifndef	_GOLEM_GFX_OVERRIDE_MATERIAL_ITF_
#define	_GOLEM_GFX_OVERRIDE_MATERIAL_ITF_

#include "golem_gfx_override_itf.h"

//! Override Material class
/*!
 * This class describes an override for a material. It can be used to
 * modify the effect of a branch material for a model.
 *
 * The material override can be used to modify base material parameters.
 */
typedef	class CGFX_Override_Material_Itf : public CGFX_Override_Itf
{
protected:
	CGFX_Override_Material_Itf( UINT32 in_Type )
		: CGFX_Override_Itf( in_Type )
	{}

public:
   //! Remove all overrides from this object
   virtual bool   ClearAllOverrides() = 0;

   //! Get number of parameter overrides on this object
   virtual UINT32 NbParameterOverrides() = 0;

	//! Add an overide to a base material parameter
   virtual UINT32 AddOverrideOnParameter(GFX::EParameter_Type in_ParamType) = 0;

	//! Remove an overide to a base material parameter by addition index
   virtual bool   RemoveOverrideOnParameterAt(UINT32 in_iIndex) = 0;

	//! Remove an overide to a base material parameter by type
   virtual bool   RemoveOverrideOnParameter(GFX::EParameter_Type in_ParamType) = 0;

   //! Get the overriden parameter by addition index
   virtual CGFX_Parameter& OverridenParameter(UINT32 in_iIndex) = 0;

   //! Get the overriden parameter by type
   virtual CGFX_Parameter& OverridenParameter(GFX::EParameter_Type in_ParamType) = 0;

}	*CGFX_Override_Material_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_OVERRIDE_MATERIAL_ITF_
