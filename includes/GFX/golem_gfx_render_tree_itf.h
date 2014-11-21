/**************************************************************
*
* golem_gfx_render_tree_itf.h
*
* Description
* ===========
*	Class CGFX_Render_Tree_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 09 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_RENDER_TREE_ITF_
#define _GOLEM_GFX_RENDER_TREE_ITF_

#include "golem_gfx_obj_itf.h"

#include "golem_gfx_scene_itf.h"

//! Manages multiple scenes
class CGFX_Render_Tree_Itf : public CGFX_Obj_Itf
{
protected:
	CGFX_Render_Tree_Itf(UINT32 in_Type)
		: CGFX_Obj_Itf(in_Type)
	{}

   //! Access to scene protected render method
   void RenderScene(CGFX_Scene_Itf* in_pScene, REAL32 in_fTime)
   {
      in_pScene->Render(in_fTime);
   }

public:
	//! Add a new scene to the render tree
	virtual UINT32	AddScene(CGFX_Scene_Itf* in_pScene) = 0;

	//! Add a new scene to the render tree at the specified position
	virtual UINT32	InsertScene(CGFX_Scene_Itf* in_pScene, UINT32 in_Index) = 0;

	//! Remove the specified scene
	virtual bool	RemoveScene(CGFX_Scene_Itf* in_pScene) = 0;

	//! Remove the scene at the specified index
	virtual bool	RemoveSceneAt(UINT32 in_Index) = 0;

	//! Return the index of the requested scene
	virtual UINT32	SceneIndex(CGFX_Scene_Itf* in_pScene) = 0;

	//! Return the number of scenes in the render tree
	virtual UINT32	NbScenes() = 0;

	//! Return the scene at the specified index
	virtual CGFX_Scene_Itf*	Scene(UINT32 in_Index) = 0;

	//! Remove all scenes
	virtual void	ClearScenes() = 0;
};

#endif //#ifndef _GOLEM_GFX_RENDER_TREE_ITF_
