/**************************************************************
*
* golem_gfx_texture_manager_itf.h
*
* Description
* ===========
*	Class CGFX_Texture_Manager_Itf definition.
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
#ifndef _GOLEM_GFX_TEXTURE_MANAGER_ITF_
#define _GOLEM_GFX_TEXTURE_MANAGER_ITF_

#include "golem_gfx_obj_itf.h"
#include "golem_gfx_texture_itf.h"
#include "golem_gfx_texture_sequence_itf.h"
#include "golem_gfx_texture_renderer_itf.h"

class CGFX_Texture_Itf;
typedef stdext::hash_map<GString, CGFX_Texture_Itf*> TextureNameMap;

//! Manages texture images and pipeline
//! \todo TBM : Change interface to allow only access to textures
class CGFX_Texture_Manager_Itf : public CGFX_Obj_Itf
{
   friend class CGFX_Texture_Itf;

protected:
	CGFX_Texture_Manager_Itf(UINT32 in_Type)
		: CGFX_Obj_Itf(in_Type)
	{}

   static TextureNameMap& TextureNames();

   void* TxtrHandle(CGFX_Texture_Itf* in_Txtr) { return in_Txtr->TxtrHandle(); }

public:
	//! Initialize the texture manager
	virtual bool	Init() = 0;

	//! Returns the number of texture units available
	virtual UINT32	Nb_Texture_Units() = 0;

   //! \todo TBM : We must decide where to regroup limitations
	//! Returns the maximum x dimention of a texture image
	virtual UINT32	Max_Texture_Size_X() = 0;

	//! Returns the maximum y dimention of a texture image
	virtual UINT32	Max_Texture_Size_Y() = 0;

	//! Returns if DOT3 textures are supported
	virtual bool	Support_DOT3() = 0;

	//! Returns if cubic environment textures are supported
	virtual bool	Support_Cube() = 0;

	//! Returns if compressed textures are supported
	virtual bool	Support_Compression() = 0;

	//! \todo TBM : Power of two & rectangle limits could be handled automatically
	//! Returns if not power of two x and y dimensions are supported
	virtual bool	Support_NPOT() = 0;

	//! Returns if rectangle textures are supported
	virtual bool	Support_Rectangle() = 0;

	//! Returns if 3D textures are supported
	virtual bool	Support_3DTexture() = 0;

   //! Create a new texture using the specified texture properties
   virtual CGFX_Texture_Itf*  CreateTexture( EPixelFormat in_PixelFormat, 
                                             UINT32 in_iWidth, UINT32 in_iHeigth, 
                                             bool in_bCreatePartial, 
                                             const GString in_sName) = 0;

	//! Create a new texture using the specified image in memory
	virtual CGFX_Texture_Itf*	Create_Texture( GImage2D& in_Image, 
                                              bool in_bCreatePartial,
                                              const GString& in_Name ) = 0;

	//! Create a new texture using the specified opened file
	virtual CGFX_Texture_Itf*	Create_Texture( GFile& in_File, 
                                              bool in_bCreatePartial,
                                              const GString& in_Name ) = 0;

	//! Create a normal map texture using another texture
	virtual CGFX_Texture_Itf*	Create_Normal_Map( CGFX_Texture_Itf* in_pHeigthTxtr, 
                                                 REAL32 in_Amount, 
                                                 const GString& in_Name ) = 0;

   //! Create a texture sequence using a list of files
   virtual GFX::GTextureSequence* CreateTextureSequence(
                                    const vector<GFile>& in_vFileSequence,
                                    bool in_bCreatePartial) = 0;

   //! Create a texture sequence using a string containing a list of files
   virtual GFX::GTextureSequence* CreateTextureSequence(
                                    const GString& in_sBaseDirectgory,
                                    const GString& in_sFileSequence,
                                    bool in_bCreatePartial) = 0;

   //! Create a texture sequence using a file containing a list of files
   virtual GFX::GTextureSequence* CreateTextureSequence(
                                    GFile& in_FileSequence,
                                    bool in_bCreatePartial) = 0;

   //! Create a new texture using the specified texture properties
   virtual CGFX_Texture_Renderer_Itf*  CreateTextureRenderer( EPixelFormat in_PixelFormat, 
                                                              UINT32 in_iWidth, UINT32 in_iHeigth, 
                                                              bool in_bCreatePartial, 
                                                              const GString in_sName) = 0;

   //! Update the content of the specified texture renderer
   virtual bool   UpdateTextureRenderer(CGFX_Texture_Renderer_Itf* in_pTxtRenderer) = 0;

	//! Get an existing texture by its name
	virtual CGFX_Texture_Itf* Get_Texture( const GString& in_Name) = 0;

   //!
   //! \brief Release a texture from the texture manager
   //! Only works when a texture is no longer referenced elsewhere
   //!
   virtual bool ReleaseTexture(CGFX_Texture_Itf* in_pTxtr) = 0;

};

#endif //#ifndef _GOLEM_GFX_TEXTURE_MANAGER_ITF_