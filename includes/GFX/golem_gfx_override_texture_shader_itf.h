/**************************************************************
*
* golem_gfx_override_texture_shader_itf.h
*
* Description
* ===========
*	Class CGFX_Override_Texture_Shader_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
**************************************************************/
#ifndef	_GOLEM_GFX_OVERRIDE_TEXTURE_SHADER_ITF_
#define	_GOLEM_GFX_OVERRIDE_TEXTURE_SHADER_ITF_

#include "golem_gfx_override_itf.h"

namespace GFX
{
   //! Identifies all possibly overridable parameters used to identify which override to use, is used or to remove
   enum ETextureShaderOverrideType
   {
      TSOVERRIDE_TEXTURE         = 1,
      TSOVERRIDE_TRANSFORM       = 2,
      TSOVERRIDE_MAG_FILTER      = 4,
      TSOVERRIDE_MIN_FILTER      = 8,
      TSOVERRIDE_ADDRESS_U       = 16,
      TSOVERRIDE_ADDRESS_V       = 32,
      TSOVERRIDE_ADDRESS_W       = 64,
      TSOVERRIDE_BORDER_COLOR    = 128,
   };
}

//! Override Texture Shader class
/*!
 * This class describes an override for a texture shader. It can be used to
 * modify the effect of a specific texture shader of a branch material 
 * for a model or a facet.
 *
 * The texture shader override can be used to modify shader parameters or the 
 * texture used by the shader.
 */
class CGFX_Override_Texture_Shader_Itf : public CGFX_Override_Itf
{
protected:
	CGFX_Override_Texture_Shader_Itf( UINT32 in_Type )
		: CGFX_Override_Itf( in_Type )
	{}

public:
   //! Associate a texture shader to this override
   virtual bool   OriginalTextureShader(class CGFX_Shader_Texture_Itf* in_pTxtrShader) = 0;

   //! Retrieve the texture shader associated with this override
   virtual class CGFX_Shader_Texture_Itf* OriginalTextureShader() = 0;

   //! Remove all overrides from this object
   virtual bool   ClearAllOverrides() = 0;

   //! Add overrides to this object
   virtual bool   AddOverrides(UINT32 in_iNewOverrides) = 0;

   //! Fetch all overrides on this object
   virtual UINT32 Overrides() = 0;

   //! Remove overrides from this object
   virtual bool   RemoveOverrides(UINT32 in_iRemovedOverrides) = 0;

   //! Retrieve the override object
   virtual class CGFX_Shader_Texture_Itf* OverrideTextureShader() = 0;

};

#endif //#ifndef _GOLEM_GFX_OVERRIDE_TEXTURE_SHADER_ITF_
