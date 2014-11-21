/**************************************************************
*
* golem_gfx_implement_node_importer.h
*
* Description
* ===========
*  Describes & implements class GFX::Implement::GNodeImporter,
*  which imports GFX files.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_IMPLEMENT_NODE_IMPORTER_H
#define _GOLEM_GFX_IMPLEMENT_NODE_IMPORTER_H

// Forward declarations
class CGFX_Action_Itf;
class CGFX_Animated_Obj_Itf;
class CGFX_Animation_Itf;
class CGFX_Model_Itf;
class CGFX_Renderer_Itf;
class CGFX_Scene_Itf;
class CGFX_Transform_2D_Itf;

namespace GFX
{
   // Forward declarations
   class GMaterial;
   class GMesh;
   enum EAddressingMode;
   enum EParameter_Type;

   namespace Implement
   {
      namespace GFXImport
      {
         namespace ENodeID
         {
            //! Id of GFX files nodes
            enum Enum
            {
               Unknown,
               Root,
               GLInfo,
               GLMaterialLibrary,
               GLMaterial,
               GLTexture2D,
               GLScene,
               GLModel,
               GLTransform,
               GLPrimitive,
               GLMesh,
               GLShape,
               GLShapeElement,
               GLFacetGroup,
               GLAnimation,
            };
         }

         //! Map between param text and its ID
         typedef stdext::hash_map<GString, GFX::EParameter_Type> ParamMap;

         class GGlobalData
         {
         public:
            //! Directory of the file that is loaded (including last \)
            GString                 m_sImportDirectory;

            //! Renderer used to create gfx objects
            CGFX_Renderer_Itf*      m_pRenderer;

            //! Scene that is currently loaded
            CGFX_Scene_Itf*         m_pScene;

            //! Root model where where all models are parented
            CGFX_Model_Itf*         m_pRootModel;

            //! Action that must be filled with data
            CGFX_Action_Itf*        m_pAction;

            //! Map that associates 
            const ParamMap*         m_pGFXParamMap;

            //! First frame of animation data
            REAL32                  m_fFirstFrame;

            //! Last frame of animation data
            REAL32                  m_fLastFrame;

            //! Frame rate of animation data
            REAL32                  m_fFrameRate;

            //! List of all loaded materials
            vector<GFX::GMaterial*> m_vMaterialLib;

            //! Flag that remembers that the material library has been loaded
            bool                    m_bMatLibLoaded;

            //! Option that indicates if hierarchy must be created when file is loaded
            bool                    m_bCreateHierarchy;

            //! Option that indicates if static keys must be created when no animation is present on a parameter
            bool                    m_bCreateStatic;
         };

         //! Context class used for importing GFX node files
         class GContext
         {
         public:
            //! Currently loading node
            GTreeNode<GNode>*       m_pGFXNode;

            //! Id of current node
            ENodeID::Enum           m_eID;

            //! Id of Parent node
            ENodeID::Enum           m_eParentID;

            //! Current loading Model
            CGFX_Model_Itf*         m_pModel;

            //! Current parameter connection object
            CGFX_Animated_Obj_Itf*  m_pParamConnection;

            //! Node flags
            UINT32                  m_iFlags;

            static const UINT32     c_iTxtr2D         = 0x0001;
            static const UINT32     c_iTransform      = 0x0002;
            static const UINT32     c_iPrimitive      = 0x0004;
            static const UINT32     c_iPrimitiveType  = 0x0008;
            static const UINT32     c_iShape          = 0x0010;
            static const UINT32     c_iPosition       = 0x0020;
            static const UINT32     c_iNormal         = 0x0040;
            static const UINT32     c_iColor          = 0x0080;
            static const UINT32     c_iTxtr0          = 0x0100;
            static const UINT32     c_iTxtr1          = 0x0200;
            static const UINT32     c_iTxtr2          = 0x0400;
            static const UINT32     c_iTxtr3          = 0x0800;
            static const UINT32     c_iTxtr4          = 0x1000;
            static const UINT32     c_iTxtr5          = 0x2000;
            static const UINT32     c_iTxtr6          = 0x4000;
            static const UINT32     c_iTxtr7          = 0x8000;

            //! Global import data
            GGlobalData*            m_pGlobalData;
         };

         //! Import function pointer type definition
         typedef bool (*ImportFunction) (GContext*);

         //! Describes an imported node
         class GNodeImportDesc
         {
         public:
            //! Import function of node
            ImportFunction       m_pImportFunction;

            //! Id of node
            ENodeID::Enum        m_eID;

            //! Constructors
            GNodeImportDesc();
            GNodeImportDesc(const GNodeImportDesc& in_Desc);
            GNodeImportDesc(ImportFunction in_pFunc, ENodeID::Enum in_eID);
         };

         //! Map between node name and node import description
         typedef stdext::hash_map<GString, GNodeImportDesc> ImportMap;
      }

      class GNodeImporter : public DesignPattern::GObserver
      {
      public:
         GNodeImporter(CGFX_Renderer_Itf* in_pRenderer);

         //! Animations and static animations will be loaded only when in_Action is not NULL.
         CGFX_Scene_Itf* LoadGFXScene( GFile& in_File, CGFX_Action_Itf** out_pAction = NULL);

         CGFX_Model_Itf* LoadGFXModel( GFile& in_File );

      protected:
         class CGFX_Renderer_Itf*   m_pRenderer;

         GFXImport::ImportMap       m_GFXLoadFunctionMap;

         GFXImport::ParamMap        m_GFXParamMap;

         GNodeTreeReader            m_TreeReader;

         bool                       m_bVersionIsValid;

         bool LoadGFX(GFile& in_File, CGFX_Scene_Itf* in_pScene, CGFX_Model_Itf* in_pRoot, CGFX_Action_Itf** out_pAction);

         virtual void OnSubjectChange(DesignPattern::GSubject& in_Subject, const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

         static bool LoadFile(GFXImport::GContext* in_pContext, const GString& in_sFileName, GFile& out_File);

         static GTreeNode<GNode>* FindGFXNode(GTreeNode<GNode>* in_pRoot, const GString& in_sNodeName, bool in_bRecurse);

         bool ImportGFXNodeRecursively(GFXImport::GContext* in_pContext);
         bool ImportInfo(GFXImport::GContext* in_pContext);

         void ImportMaterialLibrarySize(GFXImport::GContext* in_pContext);
         void ImportMaterialLibrary(GFXImport::GContext* in_pContext);

         bool ImportScene(GFXImport::GContext* in_pContext);

         static bool ImportMaterial(GFXImport::GContext* in_pContext);
         static void EnableSpecular(CGFX_Animated_Obj_Itf* in_pObj);

         static bool ImportTexture2D(GFXImport::GContext* in_pContext);
         static GFX::EAddressingMode StringToAddressMode(const GString& in_sText);
         static CGFX_Transform_2D_Itf* CreateTextureTransform(GFXImport::GContext* in_pContext);

         static bool ImportModel(GFXImport::GContext* in_pContext);

         static bool ImportTransform(GFXImport::GContext* in_pContext);
         static bool ImportPrimitive(GFXImport::GContext* in_pContext);

         static bool ImportMesh(GFXImport::GContext* in_pContext);
         static bool ImportShape(GFXImport::GContext* in_pContext);
         static bool ImportShapeElement(GFXImport::GContext* in_pContext);
         static bool ImportFacetGroup(GFXImport::GContext* in_pContext);

         static bool ImportAnimation(GFXImport::GContext* in_pContext);
      };
   }
}

#endif // #ifndef _GOLEM_GFX_IMPLEMENT_IMPORTER_H
