/**************************************************************
*
* golem_gfx_default_shader_itf.h
*
* Description
* ===========
*  Classes GFX::DefaultVertexShader & GFX::CascadeStage 
*    & GFX::CascadePixelShader definitions.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef	_GOLEM_GFX_DEFAULT_SHADER_ITF_
#define	_GOLEM_GFX_DEFAULT_SHADER_ITF_

#include "golem_gfx_shader_itf.h"

class CGFX_Transform_Itf;

namespace GFX
{
	//! Enumerates possible material color sources
	enum EMaterialSource
	{
		SOURCE_MATERIAL = 0,
		SOURCE_COLOR_DIFFUSE,
		SOURCE_COLOR_SPECULAR,
      SOURCE_COUNT,
	};

   //!
   //! \brief Default vertex shader implementation
   //! \remarks If the shader parameters are to be modified dynamically,
   //!          it is essential that they are flagged as animated with
   //!          the Set_Animated method. Failing to do so could result in
   //!          the shader not being updated. It is possible to force an update
   //!          by calling the Refresh method.
   //!
   class GDefaultVertexShader : public GVertexShader
   {
   protected:
	   GDefaultVertexShader( UINT32 in_Type )
		   : GVertexShader( in_Type )
	   {}

   public:
	   //! Get the diffuse color parameter
	   virtual CGFX_Color_Parameter& Diffuse() = 0;

	   //! Get the specular color parameter
	   virtual CGFX_Color_Parameter& Specular() = 0;

	   //! Get the specular power parameter
	   virtual CGFX_Parameter& Power() = 0;

	   //! Get the emissive color parameter
	   virtual CGFX_Color_Parameter& Emissive() = 0;

	   //! Get the ambient color parameter
	   virtual CGFX_Color_Parameter& Ambient() = 0;

	   //! Enable specular color calculation
	   virtual CGFX_Parameter& SpecularEnable() = 0;

	   //! Enable diffuse, ambient & emissive color calculation
	   virtual CGFX_Parameter& LightEnable() = 0;

	   //! Retrieve the current diffuse source
	   virtual EMaterialSource DiffuseSource() = 0;

	   //! Change the current diffuse source
	   virtual void DiffuseSource(EMaterialSource in_Source) = 0;

	   //! Retrieve the current specular source
	   virtual EMaterialSource SpecularSource() = 0;

	   //! Change the current specular source
	   virtual void SpecularSource(EMaterialSource in_Source) = 0;

	   //! Retrieve the current emissive source
	   virtual EMaterialSource EmissiveSource() = 0;

	   //! Change the current emissive source
	   virtual void EmissiveSource(EMaterialSource in_Source) = 0;

	   //! Retrieve the current ambient source
	   virtual EMaterialSource AmbientSource() = 0;

	   //! Change the current ambient source
	   virtual void AmbientSource(EMaterialSource in_Source) = 0;

      //! Get the normalize normal value
      virtual bool NormalizeNormals() = 0;

      //! Set the normalize normal value
      virtual bool NormalizeNormals(bool in_bValue) = 0;
   };

   enum ECascadeOperator
   {
      OPERATOR_NONE = 0,
      OPERATOR_SELECT_ARG_1,
      OPERATOR_SELECT_ARG_2,
      OPERATOR_ADD,
      OPERATOR_ADD_SIGNED,
      OPERATOR_ADD_SIGNED_2X,
      OPERATOR_SUBTRACT,
      OPERATOR_MULTIPLY,
      OPERATOR_MULTIPLY_2X,
      OPERATOR_MULTIPLY_4X,
      OPERATOR_BLEND_DIFFUSE_ALPHA,
      OPERATOR_BLEND_TEXTURE_ALPHA,
      OPERATOR_BLEND_FACTOR_ALPHA,
      OPERATOR_BLEND_CURRENT_ALPHA,
      OPERATOR_DOT_PRODUCT_3,
      OPERATOR_MULTIPLY_ADD,
      OPERATOR_LERP,
      OPERATOR_COUNT,
   };

   enum ECascadeArgument
   {
      ARG_CURRENT = 0,
      ARG_DIFFUSE,
      ARG_SPECULAR,
      ARG_TEXTURE,
      ARG_TEMP,
      ARG_COLOR_FACTOR,
      ARG_COUNT,
   };

   enum ECascadeArgumentModifier
   {
      ARG_MOD_ALPHA_REPLICATE = 0x4000,
      ARG_MOD_COMPLEMENT = 0x8000,
   };

   //! Macro used to modify a cascade argument with one or more cascade argument modifier
   #define ModifyCascadeArgument(arg, mod) ( (GFX::ECascadeArgument) (arg | mod) )

   enum ECascadeDestination
   {
      ARG_DEST_CURRENT = 0,
      ARG_DEST_TEMP,
      ARG_DEST_COUNT,
   };

   enum ECascadeTransformType
   {
      TRANSFORM_TYPE_DEFAULT = 0,
      TRANSFORM_TYPE_COUNT,
   };

   //!
   //! \brief Cascade stage for the cascade pixel shader
   //!
   class GCascadeStage
   {
   protected:
	   GCascadeStage()
	   {}

   public:
      virtual bool ColorOperation(ECascadeOperator in_ColorOp) = 0;
      virtual ECascadeOperator ColorOperation() = 0;

      virtual bool ColorArgument0(ECascadeArgument in_ColorArg) = 0;
      virtual ECascadeArgument ColorArgument0() = 0;

      virtual bool ColorArgument1(ECascadeArgument in_ColorArg) = 0;
      virtual ECascadeArgument ColorArgument1() = 0;

      virtual bool ColorArgument2(ECascadeArgument in_ColorArg) = 0;
      virtual ECascadeArgument ColorArgument2() = 0;

      virtual bool AlphaOperation(ECascadeOperator in_AlphaOp) = 0;
      virtual ECascadeOperator AlphaOperation() = 0;

      virtual bool AlphaArgument0(ECascadeArgument in_AlphaArg) = 0;
      virtual ECascadeArgument AlphaArgument0() = 0;

      virtual bool AlphaArgument1(ECascadeArgument in_AlphaArg) = 0;
      virtual ECascadeArgument AlphaArgument1() = 0;

      virtual bool AlphaArgument2(ECascadeArgument in_AlphaArg) = 0;
      virtual ECascadeArgument AlphaArgument2() = 0;

      virtual bool Destination(ECascadeDestination in_Dest) = 0;
      virtual ECascadeDestination Destination() = 0;

      virtual CGFX_Transform_Itf* Transform() = 0;
      virtual bool Transform(CGFX_Transform_Itf* in_pTransform) = 0;

      virtual ECascadeTransformType TransformType() = 0;
      virtual bool TransformType(ECascadeTransformType in_TransType) = 0;

      virtual UINT32 TextureCoordIndex() = 0;
      virtual bool TextureCoordIndex(UINT32 in_iIndex) = 0;

      virtual UINT32 StageSampler() = 0;
      virtual bool StageSampler(UINT32 in_iSamplerID) = 0;
   };

   //!
   //! \brief Default pixel shader implementation using operation cascade
   //!
   class GCascadePixelShader : public GPixelShader
   {
   protected:
	   GCascadePixelShader( UINT32 in_Type )
		   : GPixelShader( in_Type )
	   {}

   public:

      virtual UINT32 NbStages() = 0;
      virtual bool NbStages(UINT32 in_iCount) = 0;

      virtual GCascadeStage* Stage(UINT32 in_iStageID) = 0;

	   //! Get the color factor parameter
	   virtual CGFX_Color_Parameter& ColorFactor() = 0;

      //! Indicate if current cascade setting is valid
      //! \todo TBA : Enumerate why the validation failed
      virtual bool Validate() = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_DEFAULT_SHADER_ITF_
