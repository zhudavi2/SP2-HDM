/**************************************************************
*
* golem_gfx_override_tsampler_itf.h
*
* Description
* ===========
*  Class GFX::GOverrideTSampler definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
**************************************************************/
#ifndef  _GOLEM_GFX_OVERRIDE_TSAMPLER_ITF_
#define  _GOLEM_GFX_OVERRIDE_TSAMPLER_ITF_

#include "golem_gfx_override_itf.h"

namespace GFX
{
   // Forward declaration
   class GTextureSampler;

   //! Identifies all possibly overridable parameters used to identify which override to use, is used or to remove
   enum ETSamplerOverrideType
   {
      TSOVERRIDE_TEXTURE         = 1,
      TSOVERRIDE_MAG_FILTER      = 4,
      TSOVERRIDE_MIN_FILTER      = 8,
      TSOVERRIDE_ADDRESS_U       = 16,
      TSOVERRIDE_ADDRESS_V       = 32,
      TSOVERRIDE_ADDRESS_W       = 64,
      TSOVERRIDE_BORDER_COLOR    = 128,
      TSOVERRIDE_MASK            = 255,
   };

   //! Override texture sampler class
   /*!
   * This class describes an override for a texture sampler. It can be used to
   * modify the effect of a specific texture sampler of a branch material shader
   * for a Model or a facet.
   *
   * The texture sampler override can be used to modify sampler parameters or the 
   * texture used by the sampler.
   */
   class GOverrideTSampler : public GOverride
   {
   protected:
      GOverrideTSampler( UINT32 in_Type )
         : GOverride( in_Type )
      {}

   public:
      //! Associate a sampler Id to this override
      virtual bool OriginalTextureSamplerID(UINT32 in_iSamplerID) = 0;

      //! Retrieve the current sampler Id associated with this override
      virtual UINT32 OriginalTextureSamplerID() = 0;

      //! Remove all overrides from this object
      virtual bool ClearAllOverrides() = 0;

      //! Add overrides to this object
      virtual bool AddOverrides(UINT32 in_iNewOverrides) = 0;

      //! Fetch all overrides on this object
      virtual UINT32 Overrides() = 0;

      //! Remove overrides from this object
      virtual bool RemoveOverrides(UINT32 in_iRemovedOverrides) = 0;

      //! Retrieve the override object
      virtual GTextureSampler* OverrideTextureSampler() = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_OVERRIDE_TSAMPLER_ITF_
