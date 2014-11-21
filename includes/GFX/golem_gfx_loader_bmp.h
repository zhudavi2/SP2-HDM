/*********************************************************************************

golem_gfx_loader_bmp.h

Owner : Germain Sauve

*********************************************************************************/


#ifndef _GOLEM_GFX_LOADER_BMP_
#define _GOLEM_GFX_LOADER_BMP_


#include "golem_gfx_image_loader_itf.h"

/*
Objet permettant la lecture et l'ecriture de BMP

*/
typedef class CImage_Loader_BMP : public CImage_Loader_Itf
    {
    
public:
    CImage_Loader_BMP();
    ~CImage_Loader_BMP();

    //bool Load( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );
    //bool Save( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );

    } *CImage_Loader_BMP_Ptr;


#endif //#ifndef _GOLEM_GFX_LOADER_BMP_