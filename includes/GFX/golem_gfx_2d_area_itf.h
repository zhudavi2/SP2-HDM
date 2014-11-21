/************************************************************

golem_gfx_2d_area_itf.h

Owner : Germain Sauve

************************************************************/


#ifndef _GOLEM_GFX_2D_AREA_ITF_
#define _GOLEM_GFX_2D_AREA_ITF_

#include "golem_gfx_obj_itf.h"

/*

Utilie pour l'afiichage de donnee topographique
Doit etrtraiter comme un polygone complexe

*/
typedef class CGFX_2D_Area_Itf : public CGFX_Drawable_Obj_Itf
    {
    
public:
    virtual bool Setup( CVector2Df* pPointList, UINT32 pNumPoint ) = 0;

    virtual void Offset( CVector2Df& pOffset ) = 0;

    virtual void Scale( CVector2Df& pScale ) = 0;
    virtual void Scale( REAL32 pScaleX, REAL32 pScaleY ) = 0;

    virtual void Color( CRGBColorf& pColor ) = 0;
    virtual void Color( CRGBColori& pColor ) = 0;

    virtual void Get_Color( CRGBColorf& pColor ) = 0;
    virtual void Get_Color( CRGBColori& pColor ) = 0;

    virtual void Get_Bounding_Rect( CRectangle& pBoundingRect ) = 0;

    virtual void Rescale_In( CRectangle& pBoundingRect ) = 0;

    } *CGFX_2D_Area_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_2D_AREA_ITF_