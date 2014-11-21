/**************************************************************
*
* golem_gfx_override_txtr_seq_control_itf.h
*
* Description
* ===========
*  Definition of GFX::GOverrideTxtrSeqControl interface
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_OVERRIDE_TXTR_SEQ_CONTROL_ITF_
#define _GOLEM_GFX_OVERRIDE_TXTR_SEQ_CONTROL_ITF_

#include "golem_gfx_override_itf.h"
#include "golem_gfx_texture_sequence_itf.h"

namespace GFX
{
   //!
   //! Class that overrides control of a texture sequence
   //!
   //! Used to have the same sequence applied to different models but
   //! playing at a different speed and/or time.
   //!
   class GOverrideTxtrSeqControl : public GOverride
   {
   protected:
      GOverrideTxtrSeqControl(UINT32 in_Type)
         : GOverride(in_Type)
      {}

   public:
      //! Get the currently overriden texture sequence
      virtual GTextureSequence* TextureSequence() = 0;

      //! Change the currently overriden texture sequence
      virtual void TextureSequence(GTextureSequence* in_pTxtrSequence) = 0;

      //! Get the override controller for this texture sequence
      virtual GTextureSequenceController* OverrideController() = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_OVERRIDE_TXTR_SEQ_CONTROL_ITF_