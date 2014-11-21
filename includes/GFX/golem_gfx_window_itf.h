/**************************************************************
*
* golem_gfx_window_itf.h
*
* Description
* ===========
*  Class GFX::GWindow definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_WINDOW_ITF_
#define _GOLEM_GFX_WINDOW_ITF_

#include "golem_gfx_primitive_itf.h"

// Forward declaration
class CGFX_Texture_Itf;

namespace GFX
{
   // Forward declaration
   class GRenderStateCollection;
   enum EFilterType;
   enum ECascadeOperator;
   enum EBlendMode;

   namespace EBgndType
   {
      enum Enum
      {
         Normal = 0,
         Stretch,
         Border,
         Mosaic,
      };
   }

   class GWindow : public CGFX_Primitive_Itf
   {
   protected:
      GWindow( UINT32 in_Type )
         : CGFX_Primitive_Itf( in_Type )
      {}

   public:
      virtual bool UseModelMaterial() = 0;
      virtual void UseModelMaterial(bool in_bValue) = 0;

      virtual bool SetImage(GFile& in_File) = 0;
      virtual bool SetImage(CGFX_Texture_Itf* in_pTxtr) = 0;
      virtual CGFX_Texture_Itf* GetImage() = 0;
      virtual bool ClearImage() = 0;

      virtual CGFX_Color_Parameter& Color() = 0;

      virtual void ChangeBlending(GFX::EBlendMode in_eSourceBlend, GFX::EBlendMode in_eDestBlend) = 0;
      virtual void RestoreBlending() = 0;

      //! Select the opration wanted for color & texture components of the window
      /*!
       * ARG1 is considered to be texture and ARG2 is considered to be color.
       * If no texture is present, color will always be used directly.
       * Valid operations are:
       *    OPERATOR_SELECT_ARG_1
       *    OPERATOR_SELECT_ARG_2
       *    OPERATOR_ADD
       *    OPERATOR_ADD_SIGNED
       *    OPERATOR_ADD_SIGNED_2X
       *    OPERATOR_SUBTRACT             -> Substracts color from image
       *    OPERATOR_MULTIPLY
       *    OPERATOR_MULTIPLY_2X
       *    OPERATOR_MULTIPLY_4X
       *    OPERATOR_BLEND_TEXTURE_ALPHA  -> Uses alpha of image
       *    OPERATOR_BLEND_FACTOR_ALPHA   -> Uses alpha of color
       **/
      virtual bool ColorOperation(GFX::ECascadeOperator in_eOperation) = 0;
      virtual GFX::ECascadeOperator ColorOperation() const = 0;

      virtual bool AlphaOperation(GFX::ECascadeOperator in_eOperation) = 0;
      virtual GFX::ECascadeOperator AlphaOperation() const = 0;

      virtual EFilterType ImageMinFilter() = 0;
      virtual bool ImageMinFilter(EFilterType in_FilterType) = 0;

      virtual EFilterType ImageMagFilter() = 0;
      virtual bool ImageMagFilter(EFilterType in_FilterType) = 0;

      virtual EBgndType::Enum BackgroundType() = 0;
      virtual void BackgroundType(EBgndType::Enum in_eBgndType) = 0;

      //! Only used when background type is "Border"
      virtual CGFX_Parameter& BorderTop() = 0;
      virtual CGFX_Parameter& BorderLeft() = 0;
      virtual CGFX_Parameter& BorderBottom() = 0;
      virtual CGFX_Parameter& BorderRight() = 0;

      //! Only used when background type is "Stretch"
      virtual CGFX_Parameter& RepeatX() = 0;
      virtual CGFX_Parameter& RepeatY() = 0;

      virtual bool ClipChildren() = 0;
      virtual void ClipChildren(bool in_bValue) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_WINDOW_ITF_
