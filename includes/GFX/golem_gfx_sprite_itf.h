/*********************************************************

golem_gfx_sprite_itf.h

Owner : Germain Sauve

*********************************************************/

#ifndef _GOLEM_GFX_SPRTIE_
#define _GOLEM_GFX_SPRITE_

#include "golem_gfx_obj_itf.h"

/*
    Interface de base pour les sprite
*/
class CGFX_Sprite_Itf : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool Set_Texture( UINT32 pRef ) = 0;
    virtual bool Set_Texture( std::string& pName );

    virtual bool Draw_Operation() = 0;

    virtual bool ReverseX() = 0;
    virtual void ReverseX( bool pValue ) = 0;
    
    virtual bool ReverseY() = 0;
    virtual void ReverseY( bool pValue ) = 0;


    };


#endif //#ifndef _GOLEM_GFX_SPRTIE_