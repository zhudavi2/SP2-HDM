/**************************************************************
*
* golem_gfx_implement_importer.h
*
* Description
* ===========
*  Describes & implements class GFX::Implement::GImporter,
*  which imports Golem3ds export files.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_IMPLEMENT_IMPORTER_H
#define _GOLEM_GFX_IMPLEMENT_IMPORTER_H

#include "../../xml/include/golem_xml.h"
#include "golem_gfx_model_3ds.h"

// Forward declarations
class CGFX_Renderer_Itf;
class CGFX_Model_Itf;
class CGFX_Action_Itf;
class CGFX_Animated_Obj_Itf;
class CGFX_Animation_Itf;

namespace GFX
{
   // Forward declarations
   class GMaterial;
   class GMesh;
   class GMeshComplex;

   namespace Implement
   {
      namespace GolemImport
      {
         namespace ENodeID
         {
            //! Id of Golem3dsExport XML files nodes
            enum Enum
            {
               Unknown = 0,
               Golem3DSExport,
               GlobalInfo,
               FirstFrame,
               LastFrame,
               FrameRate,
               TicksPerFrame,
               File3DS,
               MaterialLibrary,
               Material,
               Ambient,
               Diffuse,
               Specular,
               Emissive,
               Power,
               RParam,
               GParam,
               BParam,
               Texture2D,
               Offset,
               UParam,
               VParam,
               Model,
               MaterialID,
               Primitive,
               Mesh3DS,
               Morpher,
               Deformer,
               NullPrimitive,
               Transform,
               Translation,
               Rotation,
               Scale,
               XParam,
               YParam,
               ZParam,
               Anim,
               BezierKey,
            };
         }

         class GGlobalData
         {
         public:
            //! Directory of the file that is loaded (including last \)
            GString                 m_sImportDirectory;

            //! Renderer used to create gfx objects
            CGFX_Renderer_Itf*      m_pRenderer;

            //! Root model where where all models are parented
            CGFX_Model_Itf*         m_pRootModel;

            //! Action that must be filled with data
            CGFX_Action_Itf*        m_pAction;

            //! First frame of animation data
            REAL32                  m_fFirstFrame;

            //! Last frame of animation data
            REAL32                  m_fLastFrame;

            //! Frame rate of animation data
            REAL32                  m_fFrameRate;

            //! Number of ticks during a frame
            REAL32                  m_fTicksPerFrame;

            //! Ratio used to convert key tangent from frames to seconds
            REAL32                  m_fTanRatio;

            //! The current index for mesh morpher deformers
            UINT32                  m_iDeformIndex;

            //! 3ds file loader for mesh data import
            CGFX_Model_3DS_Loader*  m_p3dsLoader;

            //! List of all loaded materials
            vector<GFX::GMaterial*> m_vMaterialLib;

            //! Flag that remembers that the material library has been loaded
            bool                    m_bMatLibLoaded;

            //! Option that indicates if hierarchy must be created when file is loaded
            bool                    m_bCreateHierarchy;

            //! Option that indicates if static keys must be created when no animation is present on a parameter
            bool                    m_bCreateStatic;
         };

         //! Context class used for importing Golem3dsExport XML files
         class GContext
         {
         public:
            //! Currently loading node
            GTreeNode<GXMLNode>*    m_pXMLNode;

            //! Id of current node
            ENodeID::Enum           m_eID;

            //! Id of Parent node
            ENodeID::Enum           m_eParentID;

            //! Current loading Model
            CGFX_Model_Itf*         m_pModel;

            //! Current parameter connection object
            CGFX_Animated_Obj_Itf*  m_pParamConnection;

            //! Current parameter type
            UINT32                  m_iParamType;

            //! May be negative when converting from right-handed to left-handed
            REAL32                  m_fNegate;

            //! Current animation
            CGFX_Animation_Itf*     m_pAnimation;

            //! Global import data
            GGlobalData*            m_pGlobalData;
         };

         //! Import function pointer type definition
         typedef bool (*ImportFunction) (GContext*);

         //! Describes an imported node
         class GNodeDesc
         {
         public:
            //! Import function of node
            ImportFunction       m_pImportFunction;

            //! Id of node
            ENodeID::Enum        m_eID;

            //! Constructors
            GNodeDesc();
            GNodeDesc(const GNodeDesc& in_Desc);
            GNodeDesc(ImportFunction in_pFunc, ENodeID::Enum in_eID);
         };

         //! Map between node name and node import description
         typedef stdext::hash_map<GString, GNodeDesc> ImportMap;
      }

      class GImporter
      {
      protected:
         class CGFX_Renderer_Itf*   m_pRenderer;

         GolemImport::ImportMap     m_GolemLoadFunctionMap;

         static bool LoadFile(GolemImport::GContext* in_pContext, const GString& in_sFileName, GFile& out_File);

         static GFX::GMesh* CreateMeshFrom3ds(CMesh_3DS* in_p3dsMesh, 
                                              CGFX_Renderer_Itf* in_pRenderer,
                                              bool in_bCreateNormals, 
                                              bool in_bPerVertexNormal);

         bool ImportGolemNodeRecursively(GolemImport::GContext* in_pContext);
         bool ImportGlobalInfo(GolemImport::GContext* in_pContext);

         static bool ImportFirstFrame(GolemImport::GContext* in_pContext);
         static bool ImportLastFrame(GolemImport::GContext* in_pContext);
         static bool ImportFrameRate(GolemImport::GContext* in_pContext);
         static bool ImportTicksPerFrame(GolemImport::GContext* in_pContext);
         static bool ImportFile3ds(GolemImport::GContext* in_pContext);

         bool ImportMaterialLibrary(GolemImport::GContext* in_pContext);

         static bool ImportMaterial(GolemImport::GContext* in_pContext);
         static bool ImportMaterialItem(GolemImport::GContext* in_pContext);
         static bool ImportMaterialParam(GolemImport::GContext* in_pContext);

         static bool ImportTexture2D(GolemImport::GContext* in_pContext);
         static UINT32 StringToAddressMode(const GString& in_sText);
         static bool ImportTextureItem(GolemImport::GContext* in_pContext);

         static bool ImportModel(GolemImport::GContext* in_pContext);

         static bool ImportMaterialID(GolemImport::GContext* in_pContext);

         static bool ImportPrimitive(GolemImport::GContext* in_pContext);
         static bool ImportMesh3ds(GolemImport::GContext* in_pContext);

         static bool ImportMorpher(GolemImport::GContext* in_pContext);
         static bool ImportDeformer(GolemImport::GContext* in_pContext);

         static bool ImportTransform(GolemImport::GContext* in_pContext);
         static bool ImportTransformItem(GolemImport::GContext* in_pContext);
         static bool ImportTransformParam(GolemImport::GContext* in_pContext);

         static bool ImportParam(GolemImport::GContext* in_pContext);

         static bool ImportAnim(GolemImport::GContext* in_pContext);
         static bool ImportBezierKey(GolemImport::GContext* in_pContext);

      public:
         GImporter(CGFX_Renderer_Itf* in_pRenderer);

         //! Animations and static animations will be loaded only when in_Action is not NULL.
         bool LoadGolemFile(  GFile& in_File, 
                              CGFX_Model_Itf* in_pRoot, 
                              CGFX_Action_Itf* in_pAction, 
                              bool in_bCreateHierarchy,
                              bool in_bCreateStaticKeys);
      };
   }
}

#endif // #ifndef _GOLEM_GFX_IMPLEMENT_IMPORTER_H
