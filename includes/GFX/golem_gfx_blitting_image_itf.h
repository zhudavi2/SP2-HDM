/*********************************************************

golem_gfx_blitting_image_itf.h

Owner : Germain Sauve

*********************************************************/

#ifndef _GOLEM_GFX_BLITTING_IMAGE_ITF_
#define _GOLEM_GFX_BLITTING_IMAGE_ITF_

#include "golem_gfx_obj_itf.h"

/*

Utile pour afficher une image autre qu'une texture

devrait utlilse les texture si possible, separer en plusieur texture l'image
si necessaire

*/
typedef class CGFX_Blitting_Image_Itf : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool Setup( CImage2D_Itf& pImage, bool pForceTexture ) = 0;
    
    virtual bool Set_Image( CImage2D_Itf& pImage ) = 0;
    virtual bool Get_Image( CGolem_Basic_Image2D& pImage ) = 0;
    } *CGFX_Blitting_Image_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_BLITTING_IMAGE_ITF_