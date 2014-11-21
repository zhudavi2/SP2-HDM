/**************************************************************
*
* golem_gfx_mesh_itf.h
*
* Description
* ===========
*  Class GFX::GMesh definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef  _GOLEM_GFX_MESH_ITF_
#define  _GOLEM_GFX_MESH_ITF_

#include "golem_gfx_primitive_itf.h"
#include "golem_gfx_facet_group_itf.h"

namespace GFX
{
   //! Class that describes a generic mesh
   class GMesh : public CGFX_Primitive_Itf
   {
   protected:
      GMesh( UINT32 in_Type )
         : CGFX_Primitive_Itf( in_Type )
      {}

   public:
      //! Return a constant reference to the shape of the mesh
      virtual const GShape& Shape() const = 0;

      //! Return a reference to the shape of the mesh
      virtual GShape& Shape() = 0;

      //! Get the number of facet groups on this mesh
      virtual UINT32 NbFacetGroups() const = 0;

      //! Set the number of facet groups on this mesh
      virtual void NbFacetGroups(UINT32 in_iCount) = 0;

      //! Returns the specified facet group constant reference
      virtual const GFacetGroup& FacetGroup(UINT32 in_iIndex) const = 0;

      //! Returns the specified facet group reference
      virtual GFacetGroup& FacetGroup(UINT32 in_iIndex) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_MESH_ITF_
