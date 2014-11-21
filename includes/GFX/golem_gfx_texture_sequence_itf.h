/**************************************************************
*
* golem_gfx_texture_sequence_itf.h
*
* Description
* ===========
*  Definition of GFX::GTextureSequence & 
*  GFX::GTextureSequenceController interfaces
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_TEXTURE_SEQUENCE_ITF_
#define _GOLEM_GFX_TEXTURE_SEQUENCE_ITF_

#include "golem_gfx_animated_obj_itf.h"
#include "golem_gfx_texture_itf.h"

namespace GFX
{
   //!
   //! Class that controls a texture sequence
   //!
   //! To display each image in the sequence, the "Frame" parameter must
   //! be animated. It can be animated in two ways: by frame or by relative
   //! time. When animated by frame, the "Frame" parameter muste be in the
   //! interval [0, NbFrames[ and directly controls which frame is displayed.
   //! When animated by relative time, the "Frame" parameter must be in the
   //! interval [0, 1[, 0 displaying the first image, 1 / NbFrames displaying
   //! the second image, x / NbFrames displaying the (x + 1)th image.
   //!
   class GTextureSequenceController : public CGFX_Animated_Obj_Itf
   {
   protected:
      GTextureSequenceController(UINT32 in_Type)
         : CGFX_Animated_Obj_Itf(in_Type)
      {}

   public:
      //! Get if sequence is animated by frame or by relative time
      virtual bool IsAnimatedByFrame() const = 0;

      //! Set if sequence is animated by frame or by relative time
      virtual void AnimateByFrame(bool in_bValue) = 0;

      //! Get the sequence animation parameter
      virtual CGFX_Parameter& Frame() = 0;
   };

   //!
   //! Class that describes a texture that displays a sequence of images
   //!
   class GTextureSequence : public CGFX_Texture_Itf
   {
   protected:
      GTextureSequence(UINT32 in_Type)
         : CGFX_Texture_Itf(in_Type)
      {}

   public:
      //! Get the number of frames in the sequence
      virtual UINT32 NbFrames() const = 0;

      //! Get the specified image in the sequence
      virtual CGFX_Texture_Itf* Frame(UINT32 in_iIndex) = 0;

      //! Get the controller for this sequence
      virtual GTextureSequenceController* Controller() = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_TEXTURE_SEQUENCE_ITF_