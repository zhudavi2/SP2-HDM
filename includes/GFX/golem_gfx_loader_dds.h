/*********************************************************************************

golem_gfx_loader_dds.h

Owner : Germain Sauve

*********************************************************************************/

#ifndef _GOLEM_GFX_LOADER_DDS_
#define _GOLEM_GFX_LOADER_DDS_


#include "golem_gfx_image_loader_itf.h"
#include "golem_gfx_directx_util.h"

/*
Objet permettant la lecture et l'ecriture de DDS ( DirectDraw Surface )
contient les mipmap et les image compresser ( S3TC )

*/
typedef class CImage_Loader_DDS : public CImage_Loader_Itf
    {
    
public:
    CImage_Loader_DDS();
    ~CImage_Loader_DDS();

    //bool Load( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );
    //bool Save( CIO_Itf& pIO, CGolem_Basic_Image2D_Ptr pImage );

    } *CImage_Loader_DDS_Ptr;


#endif //#ifndef _GOLEM_GFX_LOADER_BMP_