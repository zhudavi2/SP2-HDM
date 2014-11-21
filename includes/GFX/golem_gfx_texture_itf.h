/**************************************************************
*
* golem_gfx_texture_itf.h
*
* Description
* ===========
*  Definition of CGFX_Texture_Itf interface
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_GFX_TEXTURE_ITF_
#define _GOLEM_GFX_TEXTURE_ITF_


#include "golem_gfx_obj_itf.h"
#include "../../includes/hector/golem_image_2d.h"

namespace GFX
{
   enum ELockType
   {
      LOCK_READ = 0,
      LOCK_WRITE,
      LOCK_READWRITE,
   };
}

class CGFX_Texture_Itf : public CGFX_Obj_Itf
{
   friend class CGFX_Texture_Manager_Itf;
protected:
   CGFX_Texture_Itf(UINT32 in_Type)
      : CGFX_Obj_Itf(in_Type)
   {}
   ~CGFX_Texture_Itf();

public:
   // CGFX_Obj_Itf implementation
   const GString& Name() const
   {
      return CGFX_Obj_Itf::Name();
   }

   bool Name(const GString& in_sNewName);

   //! Internal function for texture manager
   virtual void* TxtrHandle() = 0;

   virtual EPixelFormat ColorFormat() const = 0;

   virtual UINT32  NumMipmap() const = 0;

   virtual UINT32  Width() const = 0;
   virtual UINT32  Height() const = 0;

   virtual bool    IsPartial() const = 0;
   virtual UINT32  PartialWidth() const = 0;
   virtual UINT32  PartialHeight() const = 0;

   virtual bool GetImage( GImage2D& pImage ) = 0;
   virtual bool GetMipmap( GImage2D& pImage, UINT32 pMipmapNumber ) = 0;

   virtual bool SetImage( GImage2D& pImage ) = 0;
   virtual bool SetMipmap( GImage2D& pImage, UINT32 pMipmapNumber ) = 0;

//   virtual bool LockImage( UINT32 in_iMipLevel, GImage2D* in_pImage, GFX::ELockType in_LockType ) = 0;
   virtual bool LockImage(GImage2D* in_pImage) = 0;
   virtual bool Unlock() = 0;

};

#endif //#ifndef _GOLEM_GFX_TEXTURE_ITF_