/**************************************************************
*
* golem_gfx_render_obj_itf.h
*
* Description
* ===========
*	Class CGFX_Render_Obj_Itf definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_RENDER_OBJ_ITF_
#define _GOLEM_GFX_RENDER_OBJ_ITF_

#include "golem_gfx_animated_obj_itf.h"

// Forward declarations
class CGFX_Model_Itf;

namespace GFX
{
   class GRenderContext;
}

//! Base class for any object that affects the render pipeline
class CGFX_Render_Obj_Itf : public CGFX_Animated_Obj_Itf
{
   UINT32 m_iPrepared;

protected:
	CGFX_Render_Obj_Itf(UINT32 in_Type)
		: CGFX_Animated_Obj_Itf(in_Type)
      , m_iPrepared(2)
	{}

   //! Called by object implementation to set/reset the current object preparation flag
   inline void Prepared(bool in_bNewPreparationValue)
   {
      m_iPrepared = in_bNewPreparationValue ? 1 : 0;
   }

   //! Prepare object, called each time the render object is added to another object
   virtual void PrepareInternal(CGFX_Obj_Itf* in_pContainer)
   {
      // Nothing to do for object preparation by default
   }

public:
   //! Called by the container object to known if it has to prepare the object for rendering
   inline bool IsPrepared() const
   {
      gassert(m_iPrepared != 2, "Primitive did not tell if it was prepared at construction");
      return m_iPrepared != 0;
   }

   //! Called by the container object to prepare the object for rendering
   inline void Prepare(CGFX_Obj_Itf* in_pContainer)
   {
      if(!IsPrepared() )
      {
         PrepareInternal(in_pContainer);
      }
   }

   //! Render object, called each time the object is encountered in the scene graph at each frame
   virtual void Render(GFX::GRenderContext* )
   {
      gassert(0, "No rendering method for object");
   }
};

#endif //#ifndef _GOLEM_GFX_RENDER_OBJ_ITF_
