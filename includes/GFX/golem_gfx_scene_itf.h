/**************************************************************
*
* golem_gfx_scene_itf.h
*
* Description
* ===========
*	Class CGFX_Scene_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 09 2003 (foreilly):
*		- Creation
*	May 30 2003 (foreilly):
*		- Now derives from CGFX_Animated_Obj_Itf
*
**************************************************************/
#ifndef _GOLEM_GFX_SCENE_ITF_
#define _GOLEM_GFX_SCENE_ITF_

#include "golem_gfx_animated_obj_itf.h"

namespace GFX
{
   class GMaterial;
}

//! Describes a complete scene with a camera, lights, animations & models
class CGFX_Scene_Itf : public CGFX_Animated_Obj_Itf
{
   friend class CGFX_Render_Tree_Itf;

protected:
	CGFX_Scene_Itf(UINT32 in_Type)
		: CGFX_Animated_Obj_Itf(in_Type)
	{}

   virtual void Render(REAL32 in_fTime) = 0;

public:
	//! Set the active camera of the scene
	virtual bool	Set_Active_Camera( class CGFX_Camera_Itf* in_Camera) = 0;

	//! Get the active camera of the scene
	virtual class CGFX_Camera_Itf*	Get_Active_Camera() = 0;

	//! Get the root Model of the scene
	virtual class CGFX_Model_Itf*	Get_Root_Model() = 0;

	//! Return the default material of the scene
	virtual GFX::GMaterial*	DefaultMaterial() = 0;

	//! Change the default material of the scene
	virtual bool	DefaultMaterial(GFX::GMaterial* in_pNewMaterial) = 0;

	// Return the animation system of the scene
	virtual class CGFX_Animation_System_Itf*	Get_Animation_System() = 0;

	// Return the default action of the scene
	virtual class CGFX_Action_Itf*		Get_Default_Action() = 0;

	//! Returns the global ambient parameter
	virtual CGFX_Color_Parameter&	GlobalAmbient() = 0;

	//! Returns the clear frame color parameter
	virtual CGFX_Color_Parameter&	ClearFrameColor() = 0;

   //! Indicate if frame buffer must be cleared
   virtual void ClearFrameBuffer(bool in_bClear) = 0;

   //! Return if frame buffer will be cleared
   virtual bool ClearFrameBuffer() = 0;

	//! Returns the clear depth value parameter
	virtual CGFX_Parameter&	ClearDepthValue() = 0;

   //! Indicate if depth buffer must be cleared
   virtual void ClearDepthBuffer(bool in_bClear) = 0;

   //! Return if depth buffer will be cleared
   virtual bool ClearDepthBuffer() = 0;

};

#endif //#ifndef _GOLEM_GFX_SCENE_ITF_
