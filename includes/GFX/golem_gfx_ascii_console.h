/**********************************************************

golem_gfx_console.h

Owner : Germain Sauve

**********************************************************/

#ifndef _GOLEM_GFX_CONSOLE_
#define _GOLEM_GFX_CONSOLE_

#include "golem_gfx_obj_itf.h"

/*
Non seulement utile pour afficher du texte mais doit emuler l'output d'un vieux terminaux
VT-100 en monochrome

emule l'effet d'interlace et flicker

utlise les caractere graphique ASCII 
*/

class CGFX_ASCII_Console_Itf : public CGFX_Drawable_Obj_Itf
    {
public:
// CGFX_Obj_Itf virtual
    virtual         UINT32              Type() = 0;
    virtual         UINT32              ID() = 0;
    virtual const   CGFX_Obj_Handle     Handle() = 0;
    virtual         bool                Release() = 0;

// methods
    virtual bool    Print( const char* pText ) = 0;
    virtual void    GotoXY( UINT32 pX, UINT32 pY ) = 0;
    
    virtual void    Bg_Color( UINT32 pColor) = 0;
    virtual UINT32  Bg_Color() = 0;
    
    virtual void    Text_Color( UINT32 pColor ) = 0;
    virtual UINT32  Text_Color() = 0;

    virtual void    Interlace( bool pValue ) = 0;
    virtual bool    Interlace() = 0;

    virtual void    Flicker( bool pValue ) = 0;
    virtual bool    Flicker() = 0;
    
    };


#endif  //#ifndef _GOLEM_GFX_CONSOLE_