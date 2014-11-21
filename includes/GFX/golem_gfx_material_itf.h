/**************************************************************
*
* golem_gfx_material_itf.h
*
* Description
* ===========
*	Class GFX::Material definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 23 2003 (foreilly):
*		- Creation
*  November 19 2003 (foreilly):
*     - New interface
*
**************************************************************/
#ifndef _GOLEM_GFX_MATERIAL_ITF_
#define _GOLEM_GFX_MATERIAL_ITF_

#include "golem_gfx_animated_obj_itf.h"
#include "golem_gfx_render_state_collection_itf.h"
#include "golem_gfx_shader_itf.h"

namespace GFX
{
   //!
   //! \brief Describes how a Model's primitive is to be rendered
   //!
   //! A material contains several passes, each containing a shader and/or a collection 
   //! of render states.
   //!
   class GMaterial : public CGFX_Animated_Obj_Itf
   {
   protected:
	   GMaterial(UINT32 in_Type)
		   : CGFX_Animated_Obj_Itf(in_Type)
	   {}

   public:
      //!
      //! \brief Retrieve the number of passes on this material.
      //!
      //! \remarks No passes means this material has no effect on the current Model
      //!
      //! \return Integer that contains number of passes described by this material.
      //!
      virtual UINT32 NbPasses() = 0;

      //!
      //! \brief Change the number of passes on this material.
      //!
      //! \param in_iNbPasses  : New number of passes.
      //! \return A boolean indicating the success of the operation.
      //!
      virtual bool NbPasses(UINT32 in_iNbPasses) = 0;

      //!
      //! \brief Retrieve the maximum pass activated on this material.
      //!
      //! \remarks INVALID_INDEX means all material passes are activated
      //!
      //! \return Integer that contains the maximum number of passes activated on the material.
      //!
      virtual UINT32 MaximumPass() = 0;

      //!
      //! \brief Change the number of activated passes on this material.
      //!
      //! \param activated  : New number of activated passes.
      //!
      virtual void MaximumPass(UINT32 in_iNbMaxPass) = 0;

      //!
      //! \brief Retrieve the specified render state collection.
      //!
      //! \param in_iPassID : Index of the pass for the requested render state collection.
      //! \return A RenderStateCollection class pointer.
      //!
      virtual GRenderStateCollection* RenderStates(UINT32 in_iPassID) = 0;

      //! \todo TBA : Add possibility to have only render states on specific passes
//      virtual void EnableRenderStates(UINT32 in_iPassID)

      //!
      //! \brief Retrieve the current per vertex shader.
      //!
      //! \param in_iPassID : Index of the pass for the requested shader.
      //! \return A VertexShader class pointer.
      //!
      virtual GVertexShader* VertexShader(UINT32 in_iPassID) = 0;

      //!
      //! \brief Change the current per vertex shader.
      //!
      //! \param in_iPassID : Index of the pass for the requested shader.
      //! \param in_pNewVertexShader : New vertex shader.
      //! \return A boolean indicating the success of the operation.
      //!
      virtual bool VertexShader(UINT32 in_iPassID, GVertexShader* in_pNewVertexShader) = 0;

      //!
      //! \brief Retrieve the current per pixel shader.
      //!
      //! \param in_iPassID : Index of the pass for the requested shader.
      //! \return A PixelShader class pointer.
      //!
      virtual GPixelShader* PixelShader(UINT32 in_iPassID) = 0;

      //!
      //! \brief Change the current per pixel shader.
      //!
      //! \param in_iPassID : Index of the pass for the requested shader.
      //! \param in_pNewPixelShader : New pixel shader.
      //! \return A boolean indicating the success of the operation.
      //!
      virtual bool PixelShader(UINT32 in_iPassID, GPixelShader* in_pNewPixelShader) = 0;
   };
}

#endif //#ifndef _GOLEM_GFX_MATERIAL_ITF_