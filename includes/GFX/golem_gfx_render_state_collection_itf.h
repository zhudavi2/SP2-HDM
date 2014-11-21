/**************************************************************
*
* golem_gfx_render_state_collection_itf.h
*
* Description
* ===========
*	Class GFX::RenderStateCollection definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
**************************************************************/
#ifndef	_GOLEM_GFX_RENDER_STATE_COLLECTION_ITF_
#define	_GOLEM_GFX_RENDER_STATE_COLLECTION_ITF_

#include "golem_gfx_animated_obj_itf.h"

namespace GFX
{
   //! Enumerates all types of render states
   enum ERenderStateType
   {
	   RSTATE_FILL_MODE = 0,
      RSTATE_CULL_MODE,
   	RSTATE_SHADE_MODE,
      RSTATE_ZBUFFER_ENABLE,
      RSTATE_ZBUFFER_WRITE_ENABLE,
      RSTATE_ZBUFFER_TEST_FUNCTION,
      RSTATE_ALPHA_TEST_ENABLE,
      RSTATE_ALPHA_TEST_REFERENCE,
      RSTATE_ALPHA_TEST_FUNCTION,
      RSTATE_ALPHA_BLEND_ENABLE,
      RSTATE_ALPHA_BLEND_FUNCTION,
      RSTATE_ALPHA_SOURCE_BLEND,
      RSTATE_ALPHA_DEST_BLEND,
      RSTATE_ALPHA_BLEND_FACTOR, //< Values are the same as GColorRGBInt
      RSTATE_DITHERING_ENABLE,
      RSTATE_COLOR_WRITE_ENABLE,
      RSTATE_COUNT,
   };

	//! Enumerates possible fill modes
	enum EFillMode
	{
		FILL_POINT = 1,
		FILL_WIREFRAME,
		FILL_SOLID,
	};

	//! Enumerates possible cull modes
	enum ECullMode
	{
		CULL_NONE = 1,
		CULL_CW,
		CULL_CCW,
	};

	//! Enumerates possible shade modes
	enum EShadeMode
	{
		SHADE_FLAT = 1,
		SHADE_GOURAUD,
	};

	//! Enumerates possible test functions
	enum ETestFunction
	{
		TEST_NEVER = 1,
		TEST_LESS,
		TEST_EQUAL,
		TEST_LESSEQUAL,
		TEST_GREATER,
		TEST_NOTEQUAL,
		TEST_GREATEREQUAL,
		TEST_ALWAYS,
	};

	//! Enumerates possible blend functions
	enum EBlendFunction
	{
		BLEND_ADD = 1,
		BLEND_SUBTRACT,
		BLEND_REVSUBTRACT,
		BLEND_MIN,
		BLEND_MAX,
	};

	//! Enumerates possible blend modes
	enum EBlendMode
	{
		BLEND_ZERO = 1,
		BLEND_ONE,
		BLEND_SRCCOLOR,
		BLEND_INVSRCCOLOR,
		BLEND_SRCALPHA,
		BLEND_INVSRCALPHA,
		BLEND_DESTALPHA,
		BLEND_INVDESTALPHA,
		BLEND_DESTCOLOR,
		BLEND_INVDESTCOLOR,
		BLEND_SRCALPHASAT,
		BLEND_BOTHSRCALPHA,
		BLEND_BOTHINVSRCALPHA,
		BLEND_BLENDFACTOR,
		BLEND_INVBLENDFACTOR,
	};

   //! Enumerates color write enables values
   enum EColorWriteEnable
   {
      COLOR_WRITE_RED = 1,
      COLOR_WRITE_GREEN = 2,
      COLOR_WRITE_BLUE  = 4,
      COLOR_WRITE_ALPHA = 8,
   };

   //!
   //! \brief Describes a collection of render states
   //!
   class GRenderStateCollection : public CGFX_Animated_Obj_Itf
   {
   protected:
	   GRenderStateCollection( UINT32 in_Type )
		   : CGFX_Animated_Obj_Itf( in_Type )
	   {}

   public:
      //!
      //! \brief Read the specified render state.
      //!
      //! If the render state is invalid, a reference to a dummy memory zone is returned.
      //!
      //! \param in_RenderState : Render state which value must be modified.
      //! \return A const reference to the render state value.
      //!
      virtual const UINT32& Value(ERenderStateType in_RenderState) const = 0;

      //!
      //! \brief Modify the specified render state.
      //!
      //! If the render state is invalid, a reference to a dummy memory zone is returned.
      //!
      //! \param in_RenderState : Render state which value must be modified.
      //! \return A reference to the render state value.
      //!
      virtual UINT32& Value(ERenderStateType in_RenderState) = 0;

      //!
      //! \brief Indicate if a render state is animated.
      //!
      //! \param in_RenderState : Type of render state to modify.
      //! \param in_bAnimated : Indicate if the value of the specified render state will be animated or simply change value between render frames.
      //! \return A boolean that says if animation notification was successful.
      //!
      virtual bool Animate(ERenderStateType in_RenderState, bool in_bAnimated) = 0;

      //!
      //! \brief Return if a render state is animated.
      //!
      //! \param in_RenderState : Type of render state to modify.
      //! \return A boolean that says if render state is animated.
      //!
      virtual bool Animate(ERenderStateType in_RenderState) const = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_RENDER_STATE_COLLECTION_ITF_
