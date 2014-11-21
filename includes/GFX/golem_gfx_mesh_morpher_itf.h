/**************************************************************
*
* golem_gfx_mesh_morpher_itf.h
*
* Description
* ===========
*	Class CGFX_Mesh_Morpher_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	May 22 2003 (foreilly):
*		- Creation
*	June 16 2003 (foreilly):
*		- Using GShape objects for object deformation
*
**************************************************************/
#ifndef	_GOLEM_GFX_MESH_MORPHER_ITF_
#define	_GOLEM_GFX_MESH_MORPHER_ITF_

#include "golem_gfx_primitive_itf.h"

namespace GFX
{
   // Forward declarations
   class GMesh;

   //! Maximum number of deformers that can be set on a mesh morpher
   const UINT32   MORPHER_MAX_DEFORM_COUNT = 32;
};

//! Mesh_Morpher class
class CGFX_Mesh_Morpher_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Mesh_Morpher_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Get the initial mesh
	virtual GFX::GMesh*	Get_Initial_Mesh() = 0;

	//! Set the initial mesh
	virtual bool	Set_Initial_Mesh( GFX::GMesh* in_Mesh ) = 0;

	//! Get the number of deforming shapes
	virtual UINT32	Nb_Deform_Shapes() = 0;

	//! Set the number of deforming shapes
	virtual bool	Nb_Deform_Shapes(UINT32) = 0;

	//! Return a constant reference to the specifed deforming shape
	virtual const GShape&	Deform_Shape(UINT32) const = 0;

	//! Return a reference to the specifed deforming shape
	virtual GShape&	Deform_Shape(UINT32) = 0;

	//! Get the deform mesh blend parameter
	virtual CGFX_Parameter&	Get_Deforming_Factor( UINT32 in_Index ) = 0;

};

#endif //#ifndef _GOLEM_GFX_MESH_MORPHER_ITF_
