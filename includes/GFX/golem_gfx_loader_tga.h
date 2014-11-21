/*********************************************************************************

golem_gfx_loader_tga.h

Owner : Germain Sauve

*********************************************************************************/


#ifndef _GOLEM_GFX_LOADER_TGA_
#define _GOLEM_GFX_LOADER_TGA_


#include "golem_gfx_image_loader_itf.h"

/*
Objet permettant la lecture et l'ecriture de TGA

*/
typedef class CImage_Loader_TGA : public CImage_Loader_Itf
    {
    
public:
    CImage_Loader_TGA();
    ~CImage_Loader_TGA();

    //bool Load( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );
    //bool Save( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );

    } *CImage_Loader_TGA_Ptr;


#endif //#ifndef _GOLEM_GFX_LOADER_TGA_