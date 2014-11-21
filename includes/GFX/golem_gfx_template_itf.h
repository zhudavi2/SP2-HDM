/**************************************************************
*
* golem_gfx_template_itf.h
*
* Description
* ===========
*  Class GFX::Template definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef  _GOLEM_GFX_TEMPLATE_ITF_
#define  _GOLEM_GFX_TEMPLATE_ITF_

#include "golem_gfx_render_obj_itf.h"

namespace GFX
{
   //! Template class
   class GTemplate : public CGFX_Render_Obj_Itf
   {
   protected:
      GTemplate( UINT32 in_Type )
         : CGFX_Render_Obj_Itf( in_Type )
      {}

   public:
      //! Setup template
      virtual bool Setup() = 0;

      //! Get template parameter
      virtual CGFX_Parameter& Param() = 0;

   };
}

#endif //#ifndef _GOLEM_GFX_TEMPLATE_ITF_
