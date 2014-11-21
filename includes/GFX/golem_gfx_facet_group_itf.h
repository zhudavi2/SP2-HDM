/**************************************************************
*
* golem_gfx_facet_group_itf.h
*
* Description
* ===========
*  Class GFX::GFacetGroup & GFX::GFacetGroupComplex definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef  _GOLEM_GFX_FACET_GROUP_ITF_
#define  _GOLEM_GFX_FACET_GROUP_ITF_

#include "golem_gfx_parameter_itf.h"

// Forward declaration
namespace GFX
{
   class GOverride;

   //! Class containing a single facet list for all components
   //! Note : When returning a reference, it is only valid as long
   //! as the object is not modified.
   class GFacetGroup
   {
   public:
      //! Returns a constant facet list reference
      virtual const Geometry::GFacetList& Facets() const = 0;

      //! Returns a facet list reference
      virtual Geometry::GFacetList& Facets() = 0;

      //! Get visibility parameter of this facet list
      virtual CGFX_Parameter& Visibility() = 0;

      //! Get number of overrides on this facet list
      virtual UINT32 NbOverrides() const = 0;

      //! Add an override to this facet list
      virtual bool   AddOverride( GFX::GOverride* in_pOverride ) = 0;

      //! Remove the specified override
      virtual bool   RemoveOverride( GFX::GOverride* in_pOverride ) = 0;

      //! Remove the override at the specified index
      virtual bool   RemoveOverrideAt( UINT32 in_Index ) = 0;

      //! Return the specified override (if it exists)
      virtual GFX::GOverride* Override( UINT32 in_iIndex ) const = 0;
   };

   //! Class containing a facet list for each component
   //! Note : When returning a reference, it is only valid as long
   //! as the object is not modified.
   class GFacetGroupComplex : public GFacetGroup
   {
   public:
      //! Get the number of indexes (based on position)
      virtual UINT32 NbIndexes() const = 0;

      //! Get the specified position index
      virtual UINT32 IndexPosition( UINT32 in_Index ) const = 0;

      //! Get the specified normal index
      virtual UINT32 IndexNormal( UINT32 in_Index ) const = 0;

      //! Get the specified color index
      virtual UINT32 IndexColor( UINT32 in_Index ) const = 0;

      //! Get the specified uv coordinate index
      virtual UINT32 IndexUVCoord( UINT32 in_Txtr, UINT32 in_Index ) const = 0;

      //! Returns a constant facet list reference for positions
      virtual const Geometry::GFacetList& Positions() const = 0;

      //! Returns a facet list reference for positions
      virtual Geometry::GFacetList& Positions() = 0;

      //! Returns a constant facet list reference for normals
      virtual const Geometry::GFacetList& Normals() const = 0;

      //! Returns a facet list reference for normals
      virtual Geometry::GFacetList& Normals() = 0;

      //! Returns a constant facet list reference for colors
      virtual const Geometry::GFacetList& Colors() const = 0;

      //! Returns a facet list reference for colors
      virtual Geometry::GFacetList& Colors() = 0;

      //! Returns a constant facet list reference for specified texture coordinates
      virtual const Geometry::GFacetList& UVCoords( UINT32 in_Txtr ) const = 0;

      //! Returns a facet list reference for specified texture coordinates
      virtual Geometry::GFacetList& UVCoords( UINT32 in_Txtr ) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_FACET_GROUP_ITF_
