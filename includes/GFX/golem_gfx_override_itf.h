/**************************************************************
*
* golem_gfx_override_itf.h
*
* Description
* ===========
*  Class GFX::GOverride definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
**************************************************************/
#ifndef  _GOLEM_GFX_OVERRIDE_ITF_
#define  _GOLEM_GFX_OVERRIDE_ITF_

#include "golem_gfx_animated_obj_itf.h"

namespace GFX
{
   // Forward declaration
   struct SOverrideContext;

   //! Override class
   /*!
   * This class describes an override, which modifies a branch effect for a Model or a facet.
   * The most widely used override is the material override, that modifies only few
   * parameters value of a branch material.
   */
   class GOverride : public CGFX_Animated_Obj_Itf
   {
   protected:
      GOverride( UINT32 in_Type )
         : CGFX_Animated_Obj_Itf( in_Type )
         , m_iPass(0xFFFFFFFF)
      {}

      UINT32               m_iPass;

   public:
      //! Applies the override
      virtual void Apply(SOverrideContext* in_pContext) = 0;

      //! Removes the override
      virtual void Remove(const SOverrideContext* in_pContext) { ; }

      //! Returns on which pass this override must be applied
      /*!
       * If return value is 0xFFFFFFFF, override will be applied on all passes
       **/
      virtual UINT32 ApplyOnPass() { return m_iPass; }

      //! Change on which pass this override must be applied
      /*!
       * Passing value 0xFFFFFFFF tells this override to be applied on all passes
       **/
      virtual void ApplyOnPass(UINT32 in_iPass) { m_iPass = in_iPass; }
   };
}

#endif //#ifndef _GOLEM_GFX_OVERRIDE_ITF_
