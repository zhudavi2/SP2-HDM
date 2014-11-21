/*********************************************************************************

golem_gfx_image_loader.h

Owner : Germain Sauve

*********************************************************************************/


#ifndef _GOLEM_GFX_IMAGE_LOADER_ITF_
#define _GOLEM_GFX_IMAGE_LOADER_ITF_


#include "..//..//hector//include//hector.h"

/*

Intrface de base pour une loader d'image

*/
typedef class CImage_Loader_Itf
    {
public:
    //virtual bool Load( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage ) = 0;
    //virtual bool Save( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage ) = 0;

    } *CImage_Loader_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_IMAGE_LOADER_ITF_