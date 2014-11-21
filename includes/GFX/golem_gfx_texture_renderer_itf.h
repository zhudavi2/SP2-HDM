/**************************************************************
*
* golem_gfx_texture_renderer_itf.h
*
* Description
* ===========
*  Definition of CGFX_Texture_Renderer_Itf interface
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_GFX_TEXTURE_RENDERER_ITF_
#define _GOLEM_GFX_TEXTURE_RENDERER_ITF_


#include "golem_gfx_texture_itf.h"
#include "../../includes/hector/golem_image_2d.h"

namespace GFX
{
   enum ETxtrRenderFreq
   {
      TXTRENDER_NEVER = 0,
      TXTRENDER_ONCE,
      TXTRENDER_ALWAYS,
   };
}

//!
//! Class that describes a texture that is drawn dynamically using a render tree
//!
class CGFX_Texture_Renderer_Itf : public CGFX_Texture_Itf
{
protected:
   CGFX_Texture_Renderer_Itf(UINT32 in_Type)
      : CGFX_Texture_Itf(in_Type)
   {}

public:
   //! Set the frequency at which the rendered texture will be rendered
   virtual bool RenderFrequency(GFX::ETxtrRenderFreq in_RenderFreq) = 0;

   //! Get the frequency at which the rendered texture will be rendered
   virtual GFX::ETxtrRenderFreq RenderFrequency() const = 0;

   //! \brief Return a pointer to the render tree of this texture renderer
   //!
   //! Updating the render tree will not cause the texture to be re-rendered
   //! if its frequency is set to "once" (or "never"). To render the texture,
   //! (re)set its frequency to "once".
   //!
   virtual class CGFX_Render_Tree_Itf* RenderTree() = 0;

   //!
   //! The image returned will be the last rendered image to the texture,
   //! not the image that will be used in the next render frame.
   //! This also means that the image will not be valid until at
   //! least one frame is rendered or it is set explicitely with
   //! SetImage.
   //!
   virtual bool GetImage( GImage2D& pImage ) = 0;

   //!
   //! Setting the image will reset the render frequency to "never"
   //!
   virtual bool SetImage( GImage2D& pImage ) = 0;

};

#endif //#ifndef _GOLEM_GFX_TEXTURE_RENDERER_ITF_