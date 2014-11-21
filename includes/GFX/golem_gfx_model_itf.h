/**************************************************************
*
* golem_gfx_model_itf.h
*
* Description
* ===========
*  Class CGFX_Model_Itf definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
* History
* =======
*  April 08 2003 (foreilly):
*     - Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_MODEL_ITF_
#define _GOLEM_GFX_MODEL_ITF_

#include "golem_gfx_render_obj_itf.h"

// Forward delacration
namespace GFX
{
   class GMaterial;
   class GOverride;
   class GRenderContext;
}

//! The model is a node which binds together a primitive, a transform & a material.
/*!
 * The model is the main building block of a GFX scene, as it organizes the
 * rendering elements into a scene graph (or tree).
 **/
class CGFX_Model_Itf : public CGFX_Render_Obj_Itf
{
protected:
   CGFX_Model_Itf(UINT32 in_Type)
      : CGFX_Render_Obj_Itf(in_Type)
   {}

   //! Set the Parent Model
   virtual bool   Set_Parent( CGFX_Model_Itf* in_Parent) = 0;

   //! Set the Parent Model of another moder
   inline bool Set_Parent( CGFX_Model_Itf* in_pModel, CGFX_Model_Itf* in_pNewParent)
   {
      return in_pModel->Set_Parent(in_pNewParent);
   }

public:
   //! Returns the parameter that indicates if current Model (and its hierarchy) is visible
   virtual CGFX_Parameter&    Visibility() = 0;

   //! Returns the current transformation object of the Model
   virtual class CGFX_Transform_Itf* Get_Transform() = 0;

   //! Changes the current transformation object of the Model
   virtual bool   Set_Transform( class CGFX_Transform_Itf* in_Transform) = 0;

   //! Returns the current primitive of the Model
   virtual class CGFX_Primitive_Itf* Get_Primitive() = 0;

   //! Changes the current primitive of the Model
   virtual bool   Set_Primitive( class CGFX_Primitive_Itf* in_Primitive) = 0;

   //! Returns the current material of the Model
   virtual GFX::GMaterial* Get_Material() = 0;

   //! Changes the current material of the Model
   virtual bool   Set_Material( GFX::GMaterial* in_Material) = 0;

   //! Returns the Parent Model
   virtual CGFX_Model_Itf* Get_Parent() = 0;

   //! Add a new Child
   virtual bool   Add_Child( CGFX_Model_Itf* in_Child) = 0;

   //! Insert a new Child at the specified position
   virtual bool   Insert_Child( CGFX_Model_Itf* in_Child, UINT32 in_Index) = 0;

   //! Returns how many Children we have
   virtual UINT32 Get_Nb_Children() = 0;

   //! Returns the specified Child (if it exists)
   virtual CGFX_Model_Itf* Get_Child( UINT32 in_Index ) = 0;

   //! Returns the index of the requested Child
   virtual UINT32 Get_Child_Index( CGFX_Model_Itf* in_Child ) = 0;

   //! Remove the specified Child
   virtual bool   Remove_Child( CGFX_Model_Itf* in_Child ) = 0;

   //! Remove the Child at the specified index
   virtual bool   Remove_Child_At( UINT32 in_Index ) = 0;

   //! Remove all Children
   virtual bool   Clear_Children() = 0;

   //! Send specified child in front of all other children (drawn last)
   virtual bool   Bring_To_Front( CGFX_Model_Itf* in_Child ) = 0;

   //! Send specified child behind all other children (drawn first)
   virtual bool   Send_To_Back( CGFX_Model_Itf* in_Child ) = 0;

   //! Move specified child in front of next child (drawn after)
   virtual bool   Move_Front( CGFX_Model_Itf* in_Child ) = 0;

   //! Move specified child in behind previous child (drawn before)
   virtual bool   Move_Back( CGFX_Model_Itf* in_Child ) = 0;

   //! Get number of overrides on this Model
   virtual UINT32 NbOverrides() = 0;

   //! Add an override to this Model
   virtual bool   AddOverride( GFX::GOverride* in_pOverride ) = 0;

   //! Remove the specified override
   virtual bool   RemoveOverride( GFX::GOverride* in_pOverride ) = 0;

   //! Remove the override at the specified index
   virtual bool   RemoveOverrideAt( UINT32 in_Index ) = 0;

   //! Remove all overrides of this model
   virtual void   ClearOverrides() = 0;

   //! Return the specified override (if it exists)
   virtual GFX::GOverride* Override( UINT32 in_iIndex ) = 0;

   //! Update the rendering context of the current model & all its children
   virtual void   UpdateRenderContext(const GFX::GRenderContext* in_pContext) = 0;
};

#endif //#ifndef _GOLEM_GFX_MODEL_ITF_
