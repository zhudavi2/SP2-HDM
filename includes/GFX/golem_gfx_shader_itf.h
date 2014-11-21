/**************************************************************
*
* golem_gfx_shader_itf.h
*
* Description
* ===========
*  Class GFX::Shader, GFX::VertexShader, GFX::PixelShader,
*    GFX::TextureSampler definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
**************************************************************/
#ifndef  _GOLEM_GFX_SHADER_ITF_
#define  _GOLEM_GFX_SHADER_ITF_

#include "golem_gfx_animated_obj_itf.h"

// Forward declaration
class CGFX_Texture_Itf;

namespace GFX
{
   // Forward declaration
   class GVertexShader;
   class GPixelShader;
   class GTextureSampler;

   struct SVertexShaderContext;
   struct SPixelShaderContext;
   struct STextureSamplerContext;

   //!
   //! \brief Describes a shader that affects per vertex data & prepare the pixel shader
   //!
   class GVertexShader : public CGFX_Animated_Obj_Itf
   {
   protected:
      GVertexShader( UINT32 in_Type )
         : CGFX_Animated_Obj_Itf( in_Type )
      {}

   public:
      //!
      //! \brief Update the vertex shader for the upcoming primitive draw.
      //! \remarks Update is called at least once for each primitive draw
      //!
      //! \param in_pContext : Context that can be used by this vertex shader to affect the way it is applied.
      //!                      It contains at least the current world, view & projection transforms and the list of active lights.
      //!
      virtual void Update(SVertexShaderContext* in_pContext) = 0;

      //!
      //! \brief Apply the vertex shader for the upcoming primitive draw.
      //! \remarks Apply is called each time the shader is different
      //!
      //! \param in_pContext : Context that can be used by this vertex shader to affect the way it is applied.
      //!                      It contains at least the current world, view & projection transforms and the list of active lights.
      //!
      virtual void Apply(SVertexShaderContext* in_pContext) = 0;
   };

   //!
   //! \brief Describes a shader that affects per pixel draw
   //!
   class GPixelShader : public CGFX_Animated_Obj_Itf
   {
   protected:
      GPixelShader( UINT32 in_Type )
         : CGFX_Animated_Obj_Itf( in_Type )
      {}

   public:
      //! Retrieve the number of texture samplers needed for this pixel shader
      virtual UINT32 NbTextureSamplers() = 0;

      //! Change the number of texture samplers needed for this pixel shader
      virtual bool NbTextureSamplers(UINT32 in_iCount) = 0;

      //! Retrieve the specified texture sampler
      virtual GTextureSampler* TextureSampler(UINT32 in_iID) = 0;

      //!
      //! \brief Update the pixel shader for the upcoming primitive draw.
      //! \remarks Update is called at least once for each primitive draw
      //!
      //! \param in_pContext : Context that can be used by this pixel shader to affect the way it is applied.
      //!
      virtual void Update(SPixelShaderContext* in_pContext) = 0;

      //!
      //! \brief Apply the pixel shader for the upcoming primitive draw.
      //! \remarks Apply is called each time the shader is different
      //!
      //! \param in_pContext : Context that can be used by this pixel shader to affect the way it is applied.
      //!
      virtual void Apply(SPixelShaderContext* in_pContext) = 0;
   };

   //! Enumerates all types of texture filtering
   enum EFilterType
   {
      FILTER_NONE = 0,
      FILTER_POINT,
      FILTER_LINEAR,
      FILTER_COUNT,
   };

   //! Enumerates all texture addressing modes
   enum EAddressingMode
   {
      ADDRESS_WRAP = 0,
      ADDRESS_MIRROR,
      ADDRESS_CLAMP,
      ADDRESS_BORDER,
      ADDRESS_MIRRORONCE,
      ADDRESS_COUNT,
   };

   //!
   //! \brief Describes a texture sampler affects a texture lookup for a pixel shader
   //!
   class GTextureSampler : public CGFX_Animated_Obj_Itf
   {
   protected:
      GTextureSampler( UINT32 in_Type )
         : CGFX_Animated_Obj_Itf( in_Type )
      {}

      public:
      //! Change the texture associated with this sampler
      virtual bool Texture(CGFX_Texture_Itf* in_pTexture) = 0;

      //! Retrieve the texture associated with this sampler
      virtual class CGFX_Texture_Itf* Texture() = 0;

      //! Change the texture sampler magnification filtering type
      virtual bool MagnificationFilter(EFilterType in_FilterType) = 0;

      //! Retrieve the texture sampler magnification filtering type
      virtual EFilterType MagnificationFilter() = 0;

      //! Change the texture sampler minification filtering type
      virtual bool MinificationFilter(EFilterType in_FilterType) = 0;

      //! Retrieve the texture sampler minification filtering type
      virtual EFilterType MinificationFilter() = 0;

      //! Change the texture sampler addressing mode in u direction
      virtual bool AddressingModeU(EAddressingMode in_Mode) = 0;

      //! Retrieve the texture sampler addressing mode in u direction
      virtual EAddressingMode AddressingModeU() = 0;

      //! Change the texture sampler addressing mode in v direction
      virtual bool AddressingModeV(EAddressingMode in_Mode) = 0;

      //! Retrieve the texture sampler addressing mode in v direction
      virtual EAddressingMode AddressingModeV() = 0;

      //! Change the texture sampler addressing mode in w direction
      virtual bool AddressingModeW(EAddressingMode in_Mode) = 0;

      //! Retrieve the texture sampler addressing mode in w direction
      virtual EAddressingMode AddressingModeW() = 0;

      //! Get the border color parameter
      virtual CGFX_Color_Parameter& BorderColor() = 0;

      //!
      //! \brief Apply the texture sampler for the current pixel shader.
      //!
      //! \param in_pContext : Context that can be used by this texture sampler to affect the way it is applied.
      //!                      It contains at least the Id of the hardware smapler that is to be used
      //!
      virtual void Apply(STextureSamplerContext* in_pContext) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_SHADER_ITF_
