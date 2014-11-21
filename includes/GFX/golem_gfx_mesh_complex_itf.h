/**************************************************************
*
* golem_gfx_mesh_complex_itf.h
*
* Description
* ===========
*  Class GFX::GMeshComplex definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef  _GOLEM_GFX_MESH_COMPLEX_ITF_
#define  _GOLEM_GFX_MESH_COMPLEX_ITF_

#include "golem_gfx_mesh_itf.h"
#include "golem_gfx_facet_group_itf.h"

namespace GFX
{
   //! Class that describes a generic mesh
   class GMeshComplex : public GMesh
   {
   protected:
      GMeshComplex( UINT32 in_Type )
         : GMesh( in_Type )
      {}

   public:
      //! Returns the specified facet group constant reference
      virtual const GFacetGroupComplex& FacetGroupComplex(UINT32 in_iIndex) const = 0;

      //! Returns the specified facet list reference
      virtual GFacetGroupComplex& FacetGroupComplex(UINT32 in_iIndex) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_MESH_ITF_
