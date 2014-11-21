/**************************************************************
*
* golem_gfx_override_ps_cascade_itf.h
*
* Description
* ===========
*  Class GFX::GOverridePSCascade definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
**************************************************************/
#ifndef  _GOLEM_GFX_OVERRIDE_PS_CASCADE_ITF_
#define  _GOLEM_GFX_OVERRIDE_PS_CASCADE_ITF_

#include "golem_gfx_override_itf.h"

// Forward declaration
class CGFX_Transform_Itf;

namespace GFX
{
   class GOverridePSCascade : public GOverride
   {
   protected:
      GOverridePSCascade( UINT32 in_Type )
         : GOverride( in_Type )
      {}

   public:
      //! Activate or deactivate an override on color factor
      virtual void UseColorFactorOverride(bool in_bValue) = 0;

      //! Retrieve if color factor override is activated
      virtual bool UseColorFactorOverride() const = 0;

      //! Get color factor override parameter
      virtual CGFX_Color_Parameter& ColorFactorOverride() = 0;

      //! Add a transform override on a stage texture coordinate
      virtual bool AddUVTransformOverride(UINT32 in_iStageID, CGFX_Transform_Itf* in_pTransform) = 0;

      //! Remove a transform override from a stage texture coordinate
      virtual bool RemoveUVTransformOverride(UINT32 in_iStageID) = 0;

      //! Retrieve the transform override for the specified stage texture coordinate
      virtual CGFX_Transform_Itf* UVTransformOverride(UINT32 in_iStageID) const = 0;

      //! Retrieve if specified stage has an override on its texture coordinate transform
      virtual bool UVTransformIsOverriden(UINT32 in_iStageID) const = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_OVERRIDE_PS_CASCADE_ITF_
