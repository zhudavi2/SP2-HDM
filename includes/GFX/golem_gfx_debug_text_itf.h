/*****************************************************************

golem_gfx_debug_text_itf.h

Owner : Germain Sauve

******************************************************************/

#ifndef _GOLEM_GFX_DEBUG_TEXT_
#define _GOLEM_GFX_DEBUG_TEXT_

#include "golem_gfx_obj_itf.h"

/*

Objet de base d'affichage de texte
utile seulement pour le debogage

*/
typedef class CGFX_Debug_Font : public CGFX_Obj_Itf
    {
public:

    virtual bool Set_Texture( std::string& pTexture ) = 0;
    virtual bool Set_Texture( UINT32 pTexture ) = 0;

    virtual UINT32 Texture_ID() = 0;
    
    virtual UINT32 Width() = 0;
    virtual UINT32 Height() = 0;

    virtual void Width( UINT32 pWidth ) = 0;
    virtual void Height( UINT32 pHeight ) = 0;

    virtual bool Set_CharDef( UINT32 pIndex, CVector2Df& pTxtrPos ) = 0;
	virtual bool Set_CharDef( UINT32 pIndex, CVector2Di& pTxtrPos ) = 0;
    } *CGFX_Debug_Font_Ptr;

typedef class CGFX_Debug_Text_Output : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool Set_Font( CGFX_Debug_Font_Ptr pDebugFont ) = 0;
    
    virtual bool Set_Text( std::string& pText ) = 0;

    virtual bool TextOut( std::string& pText ) = 0;

    virtual bool Clear() = 0;

    virtual bool Is_Valid() = 0;

    // num char not size in pixel
    virtual bool Resize( UINT32 pWidth, UINT32 pHeight ) = 0;
    virtual bool Resize( CVector2Di& pSize ) = 0;

    virtual bool Offset( UINT32 pX, UINT32 pY ) = 0;
    virtual bool Offset( CVector2Df& pOffset ) = 0;
    } *CGFX_Debug_Text_Output_Ptr;


#endif //#ifndef _GOLEM_GFX_DEBUG_TEXT_