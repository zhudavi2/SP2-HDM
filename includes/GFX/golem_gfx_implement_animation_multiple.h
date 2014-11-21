/**************************************************************
*
* golem_gfx_implement_animation_multiple.h
*
* Description
* ===========
*  Description of class GFX::Implement::GAnimationMultiple, which 
*  implements class GFX::GAnimationMultiple
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_IMPLEMENT_ANIMATION_MULTIPLE_
#define _GOLEM_GFX_IMPLEMENT_ANIMATION_MULTIPLE_

#include "golem_gfx_animation_multiple_itf.h"

namespace GFX
{
   namespace Implement
   {
      class GAnimationMultiple : public GFX::GAnimationMultiple
      {
      private:
         struct GParamInfo
         {
            CGFX_Animated_Obj_Itf*  m_pObject;
            GFX::EParameter_Type    m_eType;
            UINT32                  m_iCustomID;
            CGFX_Parameter*         m_pParam;
         };

         vector<GParamInfo>         m_vParams;

         CGFX_Animation_Itf*        m_pAnim;

      public:
         GAnimationMultiple();
         ~GAnimationMultiple();

         // CGFX_Animation_Itf method implementations
         bool     Connect(CGFX_Animated_Obj_Itf*, GFX::EParameter_Type);
         bool     Connect(CGFX_Animated_Obj_Itf*, const GString&);
         bool     Connect_Custom(CGFX_Animated_Obj_Itf*, UINT32);

         UINT32   Get_Data_Pitch() const;
         UINT32   Get_Data_Value_Offset() const;
         UINT32   Get_Data_Size() const;
         UINT32   Get_Nb_Data_Keys() const;
         bool     Fill_Data(UINT8* out_pDataArray) const;
         bool     Setup_Data(UINT8* in_pDataArray, UINT32 in_iDataSize);

         bool     Setup();

         bool     Push_Animation(REAL32 in_fTime);
         bool     Push_Animation(REAL32 in_fTime, REAL32& out_fValue);

         // GFX::GAnimationMultiple method implementations
         CGFX_Animation_Itf*  Animation() const;
         void                 Animation(CGFX_Animation_Itf* in_pNewAnim);

         UINT32               AddParamConnection(CGFX_Animated_Obj_Itf* in_pObject, GFX::EParameter_Type in_eParamType);
         UINT32               AddParamConnection(CGFX_Animated_Obj_Itf* in_pObject, const GString& in_sParamName);
         UINT32               AddParamConnectionCustom(CGFX_Animated_Obj_Itf* in_pObject, UINT32 in_iIndex);

         bool                 RemoveParamConnection(UINT32 in_iIndex);
         bool                 RemoveParamConnection(CGFX_Animated_Obj_Itf* in_pObject, GFX::EParameter_Type in_eParamType);
         void                 ClearParamConnections();

         CGFX_Parameter&         ConnectedParameter(UINT32 in_iIndex);
         CGFX_Animated_Obj_Itf*  ConnectedObject(UINT32 in_iIndex);
         GFX::EParameter_Type    ConnectedParameterType(UINT32 in_iIndex) const;
         const GString&          ConnectedParameterName(UINT32 in_iIndex) const;
      };
   }
}

#endif //#ifndef _GOLEM_GFX_IMPLEMENT_ANIMATION_MULTIPLE_
