/*************************************************************

golem_gfx_3d_mesh_itf.h

Owner :	Frederic O'Reilly

History	:

March 6	2003 : foreilly
- Creation

*************************************************************/

#ifndef	_GOLEM_GFX_3D_MESH_ITF_
#define	_GOLEM_GFX_3D_MESH_ITF_

#include "golem_gfx_primitive_itf.h"
#include "golem_gfx_3d_static_mesh.h"

// TBM : Add support for multiple texture coordinates
typedef	class CGFX_3D_Mesh_Itf : public	CGFX_Primitive_Itf
{
public:
//	virtual	bool CreateFrom3DMesh( C3D_Static_Mesh_Ptr in_Mesh)	= 0;
	virtual	bool	Init( bool in_UseIndex, bool in_UseNormal, bool in_UseTxtrCoord, bool in_UseColor) = 0;

	virtual bool	Use_Normal() = 0;
	virtual void	Use_Normal(bool in_UseNormal) = 0;

	virtual bool	Use_Texture() = 0;
	virtual void	Use_Texture(bool in_UseTexture) = 0;

	virtual bool	Use_Color() = 0;
	virtual void	Use_Color(bool in_UseColor) = 0;

	virtual bool	Use_Index() = 0;
	virtual void	Use_Index(bool in_UseIndex) = 0;

	virtual UINT32	Get_Nb_Positions() = 0;
	virtual UINT32	Get_Nb_Normals() = 0;
	virtual UINT32	Get_Nb_TxtrCoords() = 0;
	virtual UINT32	Get_Nb_Colors() = 0;

	virtual UINT32	Get_Nb_Index_Positions() = 0;
	virtual UINT32	Get_Nb_Index_Normals() = 0;
	virtual UINT32	Get_Nb_Index_TxtrCoords() = 0;
	virtual UINT32	Get_Nb_Index_Colors() = 0;

	virtual bool	Clear_Positions() = 0;
	virtual bool	Clear_Normals() = 0;
	virtual bool	Clear_TxtrCoords() = 0;
	virtual bool	Clear_Colors() = 0;

	virtual bool	Clear_Index_Positions() = 0;
	virtual bool	Clear_Index_Normals() = 0;
	virtual bool	Clear_Index_TxtrCoords() = 0;
	virtual bool	Clear_Index_Colors() = 0;

	virtual bool	Setup_Positions( CVector3Df *in_Positions, UINT32 in_NbPositions ) = 0;
	virtual bool	Setup_Normals( CVector3Df *in_Normals, UINT32 in_NbNormal ) = 0;
	virtual bool	Setup_TextureCoords(	CVector2Df *in_TxtrCoords, UINT32 in_NbTxtrCoord ) = 0;

	virtual bool	Setup_Colors( CRGBColorf *in_Colors, UINT32 in_NbColor ) = 0;
	virtual bool	Setup_Colors( CRGBColori *in_Colors, UINT32 in_NbColor ) = 0;

	virtual bool	Setup_Indexes( UINT32 *in_Indexes, UINT32 in_NbIndex ) = 0;
	virtual bool	Setup_IndexPositions( UINT32 *in_Indexes, UINT32 in_NbIndex ) = 0;
	virtual bool	Setup_IndexNormals( UINT32 *in_Indexes, UINT32 in_NbIndex ) = 0;
	virtual bool	Setup_IndexTextureCoords( UINT32 *in_Indexes, UINT32 in_NbIndex ) = 0;
	virtual bool	Setup_IndexColors( UINT32 *in_Indexes, UINT32 in_NbIndex ) = 0;

	virtual bool	Add_Position( CVector3Df& in_Position ) = 0;
	virtual bool	Add_TextureCoord( CVector2Df& in_TxtrCoord ) = 0;
	virtual bool	Add_Normal(	CVector3Df&	in_Normal ) = 0;
	virtual bool	Add_Color( CRGBColorf& in_Color ) = 0;
	virtual bool	Add_Color( CRGBColori& in_Color ) = 0;

	virtual bool	Add_Index( UINT32 in_Index ) = 0;
	virtual bool	Add_IndexPosition( UINT32 in_Index ) = 0;
	virtual bool	Add_IndexNormal( UINT32 in_Index ) = 0;
	virtual bool	Add_IndexTextureCoord( UINT32 in_Index ) = 0;
	virtual bool	Add_IndexColor( UINT32 in_Index ) = 0;

	virtual bool	Fill_ArrayPositions( CVector3Df *out_V3DArray	) = 0;
	virtual bool	Fill_ArrayNormals( CVector3Df *out_V3DArray ) = 0;
	virtual bool	Fill_ArrayTextureCoord(	CVector2Df *out_V2DArray ) = 0;
	virtual bool	Fill_ArrayColor( CRGBColorf	*out_ColorArray ) = 0;
	virtual bool	Fill_ArrayColor( CRGBColori	*out_ColorArray ) = 0;

	virtual bool	Fill_ArrayIndexPositions( UINT32 *out_IndexArray ) = 0;
	virtual bool	Fill_ArrayIndexNormal( UINT32 *out_IndexArray ) = 0;
	virtual bool	Fill_ArrayIndexTextureCoord( UINT32	*out_IndexArray ) = 0;
	virtual bool	Fill_ArrayIndexColor( UINT32 *out_IndexArray ) = 0;

	virtual bool	Get_Position( UINT32 in_Index, CVector3Df& out_Position ) = 0;
	virtual bool	Get_Normal(	UINT32 in_Index, CVector3Df& out_Normal ) = 0;
	virtual bool	Get_TextureCoord( UINT32 in_Index, CVector2Df& out_TextureCoord ) = 0;
	virtual bool	Get_Color( UINT32 in_Index, CRGBColori& out_Color ) = 0;
	virtual bool	Get_Color( UINT32 in_Index, CRGBColorf& out_Color ) = 0;

	virtual bool	Get_Index_Position( UINT32	in_Index,	UINT32&	out_PositionIdx ) = 0;
	virtual bool	Get_Index_Normal( UINT32 in_Index, UINT32& out_NormalIdx ) = 0;
	virtual bool	Get_Index_TextureCoord(	UINT32 in_Index, UINT32& out_TextureCoordIdx ) = 0;
	virtual bool	Get_Index_Color( UINT32	in_Index,	UINT32&	out_ColorIdx ) = 0;

}	*CGFX_3D_Mesh_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_3D_MESH_ITF_
