/************************************************************

golem_gfx_2dline_list_itf.h

Owner : Germain Sauve

************************************************************/


#ifndef _GOLEM_GFX_2DLINE_LIST_ITF_
#define _GOLEM_GFX_2DLINE_LIST_ITF_

#include "golem_gfx_obj_itf.h"


/*
    Utile pour afficher un sgement de ligne topographique
*/
typedef class CGFX_2D_Line_list_Itf : public CGFX_Drawable_Obj_Itf
    {
    
public:
    virtual bool Setup( CVector2Df* pPointList, UINT32 pNumPoint, CRGBColori pColor ) = 0;

/*
    virtual void Offset( CVector2Df& pOffset ) = 0;

    virtual void Scale( CVector2Df& pScale ) = 0;
    virtual void Scale( REAL32 pScaleX, REAL32 pScaleY ) = 0;


    virtual void Color( CRGBColorf& pColor ) = 0;
    virtual void Color( CRGBColori& pColor ) = 0;

    virtual void Get_Color( CRGBColorf& pColor ) = 0;
    virtual void Get_Color( CRGBColori& pColor ) = 0;


    virtual void Get_Bounding_Rect( CRectangle& pBoundingRect ) = 0;

    virtual void Rescale_In( CRectangle& pBoundingRect ) = 0;
*/

    virtual bool LineLoop() = 0;
    virtual void LineLoop( bool pLoop ) = 0;

    } *CGFX_2D_Line_list_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_2DLINE_LIST_ITF_