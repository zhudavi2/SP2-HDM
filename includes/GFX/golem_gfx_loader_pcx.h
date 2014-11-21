/*********************************************************************************

golem_gfx_loader_pcx.h

Owner : Germain Sauve

*********************************************************************************/


#ifndef _GOLEM_GFX_LOADER_PCX_
#define _GOLEM_GFX_LOADER_PCX_


#include "golem_gfx_image_loader_itf.h"

/*
Objet permettant la lecture et l'ecriture de BMP
*/
typedef class CImage_Loader_PCX : public CImage_Loader_Itf
    {
    
public:
    CImage_Loader_PCX();
    ~CImage_Loader_PCX();

    //bool Load( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );
    //bool Save( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );

    } *CImage_Loader_PCX_Ptr;


#endif //#ifndef _GOLEM_GFX_LOADER_PCX_