/********************************************************

golem_gfx_texture_ref_itf.h

Owner : Germain Sauve

*********************************************************/

#ifndef _GOLEM_GFX_TEXTURE_IMAGE_REF_ITF_
#define _GOLEM_GFX_TEXTURE_IMAGE_REF_ITF_

#include "golem_gfx_obj_itf.h"

/*
    Utile pour le NPOT et pour les sprite
*/
typedef class CGFX_Texture_Image_Ref_Itf : public CGFX_Obj_Itf
    {
public:
    virtual bool Set_Texture( UINT32 pID ) = 0;
    virtual bool Set_Texture( std::string& pName ) = 0;

    virtual bool Set_Coordinate_Absolute( CRectangle& pCoordRect ) = 0;
    virtual bool Set_Coordinate_Relative( CRectangle& pCoordRect ) = 0;
    } *CGFX_Texture_Ref_Itf_Ptr;


#endif //#define _GOLEM_GFX_TEXTURE_REF_ITF_