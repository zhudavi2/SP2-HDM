/**************************************************************
*
* golem_gfx_obj_itf.h
*
* Description
* ===========
*  Classes CGFX_Obj_Handle, CGFX_Obj_Itf & CGFX_Obj_Manager 
*  definition.
*
* Owner
* =====
*  Frederic O'Reilly
*
* History
* =======
*  April 09 2003 (foreilly):
*     - Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_OBJ_ITF_
#define _GOLEM_GFX_OBJ_ITF_

#include "../../includes/hector/hector.h"

using namespace Geometry;

#pragma warning( disable:4267 )

#include "golem_gfx_parameter_itf.h"

const UINT32 INVALID_INDEX = 0xFFFFFFFF;

namespace GFX
{
   enum EObject_Type
   {
      TYPE_UNKNOWN = 0,

      TYPE_RENDER_TREE,
      TYPE_RENDER_LIST,
      TYPE_TEXTURE_MANAGER,

      TYPE_MATERIAL,
      TYPE_RENDER_STATE_COLLECTION,
      TYPE_SHADER,
      TYPE_TEXTURE_SAMPLER,

      TYPE_DEFAULT_VERTEX_SHADER,
      TYPE_CASCADE_PIXEL_SHADER,

      TYPE_TEXTURE,
      TYPE_TEXTURE_SEQUENCE,
      TYPE_TEXTURE_SEQUENCE_CONTROLLER,
      TYPE_TEXTURE_RENDERER,

      TYPE_SCENE,
      TYPE_MODEL,
      TYPE_CAMERA,
      TYPE_LIGHT,
      TYPE_MESH,
      TYPE_MESH_COMPLEX,
      TYPE_MESH_MORPHER,
      TYPE_2D_MULTIPLE_LINE_LIST,
      TYPE_WINDOW,
      TYPE_TEXT_2D,
      TYPE_VIEWPORT,
      TYPE_CONSOLE,

      TYPE_PARTICLE_SYSTEM,

      TYPE_TRANSFORM,
      TYPE_TRANSFORM_SRT,
      TYPE_TRANSFORM_2D,

      TYPE_ANIMATION_SYSTEM,
      TYPE_TRACK,
      TYPE_ACTION,
      TYPE_ANIMATION_CONSTANT,
      TYPE_ANIMATION_LINEAR,
      TYPE_ANIMATION_CUBIC,
      TYPE_ANIMATION_STATIC,
      TYPE_ANIMATION_PARTICLE,
      TYPE_ANIMATION_MULTIPLE,

      TYPE_OVERRIDE,
      TYPE_OVERRIDE_RSTATE,
      TYPE_OVERRIDE_PS_CASCADE,
      TYPE_OVERRIDE_TSAMPLER,
      TYPE_OVERRIDE_TXTR_SEQ_CONTROL,

      TYPE_COUNT,

      TYPE_USER = 1000,
   };
};

// Macro for safe release of GFX_Obj ( and DirectX by the way )
#define SAFE_RELEASE(p) { if(p) {(p)->Release(); (p) = NULL; } }

//! Base handle for graphical objects
class CGFX_Obj_Handle
{
public: 
   //! Type defines the kind of object (useful for upcasting)
   UINT32   m_Type;

   //! Id is unique for each instance of an object type
   //! Id 0 is invalid, all IDs begin at 1
   UINT32   m_ID;

   //! Default constructor. Handle is still invalid.
   CGFX_Obj_Handle()
      : m_Type(0)
      , m_ID(0)
   {
   }

   //! Constructor. Handle will contain specified type and Id.
   CGFX_Obj_Handle( UINT32 in_Type, UINT32 in_ID )
      : m_Type(in_Type)
      , m_ID(in_ID)
   {
   }
};

//! Base GFX object interface
//! This object uses COM style allocation management
//! with functions Inc_Ref() and Release().
class CGFX_Obj_Itf
{
   friend class CGFX_Obj_Manager;

   CGFX_Obj_Handle   m_Handle;
   UINT32            m_iRefCount;
   GString           m_sName;

protected:
   //! Constructor
   //! It is protected to ensure object cannot be instanciated.
   CGFX_Obj_Itf(UINT32 in_Type);

   //! Destructor
   //! It is protected to ensure object can only be deleted by the Release() function.
   virtual ~CGFX_Obj_Itf();
public:
   //! Returns the type of object
   UINT32 Type() const
   {
      return m_Handle.m_Type;
   }

   //! Returns the Id of object
   UINT32 Id() const
   {
      return m_Handle.m_ID;
   }

   //! Returns the complete handle of the object
   CGFX_Obj_Handle Handle() const
   {
      return m_Handle;
   }

   // Returns the name of the object
   const GString& Name() const
   {
      return m_sName;
   }

   // Set a new name for the object
   virtual bool Name(const GString& in_sName);

   //! Returns the number of references this object has
   UINT32 Ref_Count() const
   {
      return m_iRefCount;
   }

   //! Add a new object reference
   void Inc_Ref()
   {
      ++ m_iRefCount;
   }

   //! Decrease reference count and delete the object when it reaches 0. Returns true when object is deleted.
   virtual bool Release();
};

//! This class manages all object instances
class CGFX_Obj_Manager
{
   //! Class that contains all objects of a certain type.
   class CGFX_Obj_Type_Container
   {
   public:
      //! Map type that links an Id to an object.
      typedef map<UINT32, CGFX_Obj_Itf*> CObj_Map;

      //! Next Id used for this type.
      UINT32 m_NewID;

      //! Map containing all objects and their IDs.
      CObj_Map m_MapObj;

      //! Constructor.
      CGFX_Obj_Type_Container()
         : m_NewID(1)
      {}
   };

   //! Map type that links a type to an object type container.
   typedef map<UINT32, CGFX_Obj_Type_Container> CType_Map;

   //! Map containing all object type containers and their type.
   CType_Map            m_MapType;

public:
   //! Add a new object reference
   virtual bool         Insert( CGFX_Obj_Itf* in_Obj );

   //! Remove an object reference
   virtual bool         Remove( CGFX_Obj_Itf* in_Obj );

   //! Get an object reference by using its handle (Id and type)
   virtual CGFX_Obj_Itf*   GFX_Obj( CGFX_Obj_Handle in_Handle);

   //! Get the last allocated Id for the specified type
   virtual UINT32       Last_ID( UINT32 in_Type ) const;

   //! Get a new Id for the specified type
   virtual UINT32       New_ID( UINT32 in_Type );

   //! Remove all referenced objects
   virtual bool         ClearAll();

   //! Create a file containing the list of all referenced objects
   virtual bool         DumpList(const GString& in_FileName) const;

   //! Fill a string describing a type
   virtual bool         Get_GFX_Type_Name( UINT32 in_Type, GString &out_Str) const;
};

#endif // _GOLEM_GFX_OBJ_ITF_
