/****************************************************************************

golem_gfx_3d_satic_mesh.h

Owner : Germain Sauve

****************************************************************************/

#ifndef _GOLEM_GFX_3D_STATIC_MESH_
#define _GOLEM_GFX_3D_STATIC_MESH_

#include "golem_gfx_obj_itf.h"


/*
utile pour l'edition et le traitement d'objet de base 3D ( sans deformation et n'etant
compose qu'un d'un seul mesh ).

 Devrait ajouter plusieurs textures ( material? )

Chaque Renderer devrait exporter un type d'objet pouvant s'initialiser a partir de
celui-ci
*/

typedef class C3D_Static_Mesh
    {
    bool m_UseNormal;
    bool m_UseTxtr;
    bool m_UseColor;
    bool m_UseIndex;
    bool m_FormatU32;

    vector<CVector3Df>          m_Points;
    vector<CVector3Df>          m_Normals;
    vector<CVector2Df>          m_TxtrCoord;
    vector<CRGBColori>          m_Color;
    vector<CDraw_Command_U32>   m_Command;

    vector<UINT32>              m_IndexPoint;
    vector<UINT32>              m_IndexNormal;
    vector<UINT32>              m_IndexTxtrCoord;
    vector<UINT32>              m_IndexColor;

    char    m_TextureName[256];
public:
    C3D_Static_Mesh();
    ~C3D_Static_Mesh();

    bool Init( bool pUseIndex, bool pUseNormal, bool pUseTxtrCoord, bool pUseColor, bool pFormatU32 );
    
    bool    UseNormal();
    bool    UseTexture();
    bool    UseColor();
    bool    UseIndex();
    bool    FormatU32();
    bool    SetFormat( bool pU32 );

    UINT32  GetNumPoints();
    UINT32  GetNumNormals();
    UINT32  GetNumTxtrCoord();
    UINT32  GetNumColor();

    UINT32  GetNumIndex();
    UINT32  GetNumCommand();

    bool    Setup_Points( CVector3Df *pPoints, UINT32 pNumPoints );
    bool    Setup_Normals( CVector3Df* pNormals, UINT32 pNumNormal );
    bool    Setup_TextureCoord( CVector2Df *pTxtrCoords, UINT32 pNumTxtrCoord );
    
    bool    Setup_Color( CRGBColorf *pColor, UINT32 pNumColor );
    bool    Setup_Color( CRGBColori *pColor, UINT32 pNumColor );
    bool    Setup_Command( CDraw_Command_U32_Ptr pCommands, UINT32 pNumCommand );
    bool    Setup_Command( CDraw_Command_U16_Ptr pCommands, UINT32 pNumCommand );

    bool    Setup_Texture( std::string& pTextureName );

    bool    Setup_IndexPoints( UINT32 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexPoints( UINT16 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexNormal( UINT32 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexNormal( UINT16 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexTextureCoord( UINT32 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexTextureCoord( UINT16 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexColor( UINT32 *pIndex, UINT32 pNumIndex );
    bool    Setup_IndexColor( UINT16 *pIndex, UINT32 pNumIndex );

    bool    Add_Point( CVector3Df& pPoint );
    bool    Add_TextureCoord( CVector2Df& pTxtrCoord );
    bool    Add_Normal( CVector3Df& pNormal );
    bool    Add_Color( CRGBColorf& pColor );
    bool    Add_Color( CRGBColori& pColor );
    bool    Add_Command( UINT32 pCommand );
    bool    End_Command();

    bool    Fill_ArrayPoints( CVector3Df *pV3DArray );
    bool    Fill_ArrayNormals( CVector3Df *pV3DArray );
    bool    Fill_ArrayTextureCoord( CVector2Df *pV2DArray );
    bool    Fill_ArrayColor( CRGBColorf *pColorArray );
    bool    Fill_ArrayColor( CRGBColori *pColorArray );

    bool    Fill_ArrayIndexPoints( UINT32 *pIndexArray );
    bool    Fill_ArrayIndexPoints( UINT16 *pIndexArray );
    
    bool    Fill_ArrayIndexNormal( UINT32 *pIndexArray );
    bool    Fill_ArrayIndexNormal( UINT16 *pIndexArray );

    bool    Fill_ArrayIndexTextureCoord( UINT32 *pIndexArray );
    bool    Fill_ArrayIndexTextureCoord( UINT16 *pIndexArray );
    bool    Fill_ArrayIndexColor( UINT32 *pIndexArray );
    bool    Fill_ArrayIndexColor( UINT16 *pIndexArray );

    bool    Fill_ArrayCommand( CDraw_Command_U32_Ptr pCommandArray );
    bool    Fill_ArrayCommand( CDraw_Command_U16_Ptr pCommandArray );

    bool    Get_Point( UINT32 pIndex, CVector3Df& pPoint );
    bool    Get_Normal( UINT32 pIndex, CVector3Df& pNormal );
    bool    Get_TextureCoord( UINT32 pIndex, CVector2Df& pTextureCoord );
    bool    Get_Color( UINT32 pIndex, CRGBColori& pColor );
    bool    Get_Color( UINT32 pIndex, CRGBColorf& pColor );

    bool    Get_Index_Point( UINT32 pIndex, UINT32& pPointIdx );
    bool    Get_Index_Point( UINT32 pIndex, UINT16& pPointIdx );
    bool    Get_Index_Normal( UINT32 pIndex, UINT32& pNormalIdx );
    bool    Get_Index_Normal( UINT32 pIndex, UINT16& pNormalIdx );
    bool    Get_Index_TextureCoord( UINT32 pIndex, UINT32& pTextureCoordIdx );
    bool    Get_Index_TextureCoord( UINT32 pIndex, UINT16& pTextureCoordIdx );
    bool    Get_Index_Color( UINT32 pIndex, UINT32& pColorIdx );
    bool    Get_Index_Color( UINT32 pIndex, UINT16& pColorIdx );

    bool    Get_Command( UINT32 pIndex, CDraw_Command_U32& pCommand );
    bool    Get_Command( UINT32 pIndex, CDraw_Command_U16& pCommand );

    bool    Save( CIO_Itf_Ptr pIO );
    bool    Save( const char *pPath );

    bool    Load( CIO_Itf_Ptr pIO );
    bool    Load( const char *pPath );
    } *C3D_Static_Mesh_Ptr;


#endif //#ifndef _GOLEM_GFX_3D_STATIC_MESH_