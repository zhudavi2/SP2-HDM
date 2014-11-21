/******************************************************************************

golem_gfx_model_3ds.h


Owner :	Germain	Sauve

******************************************************************************/


#ifndef	_GOLEM_GFX_MODEL_3DS_
#define	_GOLEM_GFX_MODEL_3DS_

#include "../../includes/hector/hector.h"

/*
Objet pour l'utilisation de .3DS
*/

struct CChunk_3DS
{
	UINT16	m_ID;
	UINT32	m_Lenght;
	UINT32	m_BytesRead;
};

struct CMaterial_3DS
{
	char		m_Name[255];
	GColorRGBReal	m_Ambient;
	GColorRGBReal	m_Diffuse;
	GColorRGBReal	m_Specular;
	REAL32		m_Emissive;
	REAL32		m_Power;
	REAL32		m_SpecularLevel;
	REAL32		m_Transparency;
	REAL32		m_DiffuseBlend;
	char		m_DiffuseTexture[255];
	REAL32		m_BumpBlend;
	char		m_BumpTexture[255];
};

struct CGroup_3DS
{
	char	 m_Name[255];
	INT32	 m_Mat;
	INT32	 m_Start;
	INT32	 m_Size;
	INT32	 *m_Tris;
};

struct CMesh_3DS
{
	char		   m_Name[255];
	UINT32		   m_VertCount;
	UINT32		   m_TexCoordCount;
	UINT32		   m_TriCount;
	UINT32		   m_GroupCount;
	GVector3D<REAL32>	   *m_Verts;
	GVector3D<REAL32>	   *m_Norms;
	GVector2D<REAL32>	   *m_TexCoords;
	REAL32		   (*m_TangentSpace)[9];
	GVector3D<INT32>	   *m_Tris;
	CGroup_3DS	   *m_roups;
	UINT32		   *m_Smooth;
	GVector3D<REAL32>	   m_Axis[3];
	GVector3D<REAL32>	   m_Position;
	GVector3D<REAL32>	   m_Min;
	GVector3D<REAL32>	   m_Max;
	GVector3D<REAL32>	   m_Pivot;
	REAL32         m_RotAngle;
	GVector3D<REAL32>     m_RotAxis;
	GVector3D<REAL32>     m_Scale;
	UINT32         m_MaterialIndex;
};

struct C3DS_State
{
	enum EMapType
	{
		NONE = 0,
		DIFFUSE,
		BUMP,
	};
	CMesh_3DS *m_CurrentMesh;
	CMaterial_3DS *m_CurrentMaterial;
	EMapType m_CurrentMapType;
};

class CGFX_Model_3DS_Loader
{
	// TBR?
	GVector3D<REAL32>	m_Min;
	GVector3D<REAL32>	m_Max;
	GVector3D<REAL32>	m_Center;

	vector<CMesh_3DS>		m_Meshes;
	vector<CMaterial_3DS>	m_Materials;

	class GFile*	m_f;

	bool	ReadChunk( CChunk_3DS& in_Chunk );
	bool	ProcessChunk( CChunk_3DS& in_Chunk, C3DS_State in_State );
	bool	SkipChunk(CChunk_3DS& in_Chunk);

	bool	ReadWORD( CChunk_3DS& in_Chunk, UINT16& in_Value );
	bool	ReadASCII( CChunk_3DS& in_Chunk, char *in_Text );

	bool	ReadPercent( CChunk_3DS& in_Chunk, REAL32& in_Value );
	bool	ReadColor( CChunk_3DS& in_Chunk, GColorRGBReal& in_Color );
	bool	ReadVector3( CChunk_3DS& in_Chunk, GVector3D<REAL32>& in_Vector3 );
	bool	ReadVector2( CChunk_3DS& in_Chunk, GVector2D<REAL32>& in_Vector2 );

	bool	RemoveVertexTransform();
public:
	CGFX_Model_3DS_Loader();
	~CGFX_Model_3DS_Loader();

	UINT32	Get_Nb_Mesh();
	CMesh_3DS*	Get_Mesh(UINT32 in_Index);

	UINT32	Get_Nb_Material();
	CMaterial_3DS*	Get_Material(UINT32 in_Index);

	bool Load_File( GFile& in_File );

};


#endif //#ifndef _GOLEM_GFX_MODEL_3DS_