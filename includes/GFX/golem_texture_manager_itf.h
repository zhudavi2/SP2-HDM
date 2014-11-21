/************************************************************

golem_texture_manager_itf.h


Owner: Germain Sauve

************************************************************/


#ifndef _GOLEM_TEXTURE_MANAGER_ITF_
#define _GOLEM_TEXTURE_MANAGER_ITF_

#include "..\\..\\hector\\include\\hector.h"
#include "golem_gfx_texture_itf.h"

typedef class CTexture_Manager_Itf
    {
public:
    virtual bool Release() = 0;

    virtual bool    Init() = 0;
    virtual UINT32  NumTextureUnit() = 0;
    virtual UINT32  MaxTextureSizeX() = 0;
    virtual UINT32  MaxTextureSizeY() = 0;
    virtual bool    Support_DOT3() = 0;
    virtual bool    Support_Cube() = 0;
    virtual bool    Support_Compression() = 0;
    virtual bool    Support_NPOT() = 0; //Not Power Of Two ( Rectangle )
    virtual bool    Support_3DTexture() = 0;
    

    virtual CGFX_Texture_Ptr    Create_Texture( CGolem_Basic_Image2D& pImage, const string& pName, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp, bool pAcceptNPOTD, bool pFitInPOTD ) = 0;

    virtual CGFX_Texture_Ptr    Create_Texture( CIO_Itf_Ptr pIO, const string& pName, bool pAcceptNPOTD, bool pFitInPOTD ) = 0;

    //virtual CGFX_Texture_Ptr    Load_Texture_BMP( CIO_Itf& pIO, const string& pName, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp, bool pAcceptNPOTD, bool pFitInPOTD ) = 0;
    //virtual CGFX_Texture_Ptr    Load_Texture_TGA( CIO_Itf& pIO, const string& pName, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp, bool pAcceptNPOTD, bool pFitInPOTD ) = 0;
    //virtual CGFX_Texture_Ptr    Load_Texture_PCX( CIO_Itf& pIO, const string& pName, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp, bool pAcceptNPOTD, bool pFitInPOTD ) = 0;

    //virtual bool    Set_Texture_BMP( CGFX_Texture_Ptr pTxtr, CIO_Itf& pIO, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp ) = 0;
    //virtual bool    Set_Texture_TGA( CGFX_Texture_Ptr pTxtr, CIO_Itf& pIO, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp ) = 0;
    //virtual bool    Set_Texture_PCX( CGFX_Texture_Ptr pTxtr, CIO_Itf& pIO, UINT32 pChannels, UINT32 pTxtrType, UINT32 pComp ) = 0;

    virtual bool    Activate_TextureMapping() = 0;
    virtual bool    Deactivate_TextureMapping() = 0;

    virtual bool    Activate_Texture( UINT32 pStage, string& pName ) = 0;
    virtual bool    Activate_Texture( UINT32 pStage, UINT32 pID ) = 0;

    virtual bool    Deactivate_Texture( UINT32 pStage ) = 0;

    virtual bool    Texture_Stage_Blend( UINT32 pStage, UINT32 pBlend ) = 0;

    virtual UINT32    Get_TextureIDByName( const std::string& pTxtrName ) = 0;
    virtual CGFX_Texture_Ptr    Get_Texture( UINT32 pID ) = 0;
    virtual CGFX_Texture_Ptr    Get_Texture( const std::string& pTxtrName ) = 0;
    virtual bool Valid_Texture( const std::string pTxtrName ) = 0;
    virtual bool Valid_Texture( UINT32 pTxtrID ) = 0; 
    } *CTexture_Manager_Itf_Ptr;



#endif //#ifndef _GOLEM_TEXTURE_MANAGER_ITF_