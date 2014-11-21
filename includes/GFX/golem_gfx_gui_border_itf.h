/*********************************************************

golem_gfx_gui_border_itf.h

Owner : Germain Sauve

*********************************************************/

#ifndef _GOLEM_GFX_GUI_BORDER_ITF_
#define _GOLEM_GFX_GUI_BORDER_ITF_

#include "golem_gfx_obj_itf.h"

/*

Interface de base pour la bordure d'objet GUI

*/
typedef class CGFX_GUI_Border_Itf : public CGFX_Obj_Itf
    {
public:
    virtual bool Use_Texture() = 0;
    virtual void Use_Texture( bool pUse ) = 0;
    
    virtual UINT32 TextureID() = 0;
    virtual void TextureID( UINT32 pTxtrID ) = 0;

    virtual Set_Color( CRGBColorf& pColor ) = 0;
    virtual Get_Color( CRGBColorf& pColor ) = 0;

    virtual void Witdth( UINT32 pWidth  ) = 0;
    virtual UINT32 Witdth() = 0;

    virtual void Height( UINT32 pHeight  ) = 0;
    virtual UINT32 Height() = 0;

    virtual void OffsetInTexture( CVector2Di& pOffset ) = 0;
    
    } *CGFX_GUI_Border_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_GUI_BORDER_ITF_