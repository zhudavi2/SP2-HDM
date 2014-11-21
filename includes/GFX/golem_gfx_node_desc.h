/**************************************************************
*
* golem_gfx_node_desc.h
*
* Description
* ===========
*  Description of all known GFX node descriptions
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// GFXF version 0100
const UINT32 c_iGLInfoCount = 5;
const GNodeParameterDesc c_pGLInfoParams[c_iGLInfoCount] = 
{
   {EParameterType::PChar,  0, 0, 0, 0},
   {EParameterType::PChar,  0, 0, 0, 0},
   {EParameterType::Real32, 1, 1, 0, 0},
   {EParameterType::Real32, 1, 1, 0, 0},
   {EParameterType::Real32, 1, 1, 0, 0},
};

const GNodeTypeDesc c_GLInfoDesc = 
{
   "GL_Info",
   1,
   c_iGLInfoCount,
   c_pGLInfoParams
};

const GNodeTypeDesc c_GLMaterialLibraryDesc = 
{
   "GL_Material_Library",
   3,
   0,
   NULL
};

const UINT32 c_iGLMaterialCount = 7;
const GNodeParameterDesc c_pGLMaterialParams[c_iGLMaterialCount] = 
{
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::Real32,  1, 1, 0, 0},
};

const GNodeTypeDesc c_GLMaterialDesc = 
{
   "GL_Material",
   5,
   c_iGLMaterialCount,
   c_pGLMaterialParams
};

const UINT32 c_iGLTexture2DCount = 5;
const GNodeParameterDesc c_pGLTexture2DParams[c_iGLTexture2DCount] = 
{
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PReal32, 2, 1, 0, 0},
   {EParameterType::PReal32, 2, 1, 0, 0},
};

const GNodeTypeDesc c_GLTexture2DDesc = 
{
   "GL_Texture_2D",
   6,
   c_iGLTexture2DCount,
   c_pGLTexture2DParams
};

const UINT32 c_iGLSceneCount = 7;
const GNodeParameterDesc c_pGLSceneParams[c_iGLSceneCount] = 
{
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PReal32, 4, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PReal32, 4, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::Real32,  1, 1, 0, 0},
};

const GNodeTypeDesc c_GLSceneDesc = 
{
   "GL_Scene",
   2,
   c_iGLSceneCount,
   c_pGLSceneParams
};

const UINT32 c_iGLModelCount = 3;
const GNodeParameterDesc c_pGLModelParams[c_iGLModelCount] = 
{
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
};

const GNodeTypeDesc c_GLModelDesc = 
{
   "GL_Model",
   4,
   c_iGLModelCount,
   c_pGLModelParams
};

const GNodeTypeDesc c_GLPrimitiveDesc = 
{
   "GL_Primitive",
   7,
   0,
   NULL
};

const GNodeTypeDesc c_GLMeshDesc = 
{
   "GL_Mesh",
   9,
   0,
   NULL
};

const GNodeTypeDesc c_GLShapeDesc = 
{
   "GL_Shape",
   10,
   0,
   NULL
};

const UINT32 c_iGLShapeElementCount = 4;
const GNodeParameterDesc c_pGLShapeElementParams[c_iGLShapeElementCount] = 
{
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PReal32, 0, 0, 1, 2},
};

const GNodeTypeDesc c_GLShapeElementDesc = 
{
   "GL_Shape_Element",
   11,
   c_iGLShapeElementCount,
   c_pGLShapeElementParams
};

const UINT32 c_iGLFacetGroupCount = 7;
const GNodeParameterDesc c_pGLFacetGroupParams[c_iGLFacetGroupCount] = 
{
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PUInt32, 0, 0, 4, 5},
};

const GNodeTypeDesc c_GLFacetGroupDesc = 
{
   "GL_Facet_Group",
   12,
   c_iGLFacetGroupCount,
   c_pGLFacetGroupParams
};

const UINT32 c_iGLTransformCount = 3;
const GNodeParameterDesc c_pGLTransformParams[c_iGLTransformCount] = 
{
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
   {EParameterType::PReal32, 3, 1, 0, 0},
};

const GNodeTypeDesc c_GLTransformDesc = 
{
   "GL_Transform",
   8,
   c_iGLTransformCount,
   c_pGLTransformParams
};

const UINT32 c_iGLAnimationCount = 5;
const GNodeParameterDesc c_pGLAnimationParams[c_iGLAnimationCount] = 
{
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::PChar,   0, 0, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::UInt32,  1, 1, 0, 0},
   {EParameterType::PReal32, 0, 0, 2, 3},
};

const GNodeTypeDesc c_GLAnimationDesc = 
{
   "GL_Animation",
   13,
   c_iGLAnimationCount,
   c_pGLAnimationParams
};

namespace EAnimTypes
{
   enum Enum
   {
      Static,
      Constant,
      Linear,
      Cubic,
      MaxCount,
   };
}

const char * c_pAnimNames[EAnimTypes::MaxCount] =
{
   "STATIC",
   "CONSTANT",
   "LINEAR",
   "CUBIC",
};

const UINT32 c_pAnimKeySize[EAnimTypes::MaxCount] =
{
   2,
   2,
   2,
   6,
};

namespace EParamID
{
   enum Enum
   {
      Visibility,
      TranslationX,
      TranslationY,
      TranslationZ,
      RotationX,
      RotationY,
      RotationZ,
      ScaleX,
      ScaleY,
      ScaleZ,
      SceneAmbientR,
      SceneAmbientG,
      SceneAmbientB,
      SceneAmbientA,
      ClearR,
      ClearG,
      ClearB,
      ClearA,
      Depth,
      AmbientR,
      AmbientG,
      AmbientB,
      DiffuseR,
      DiffuseG,
      DiffuseB,
      EmissiveR,
      EmissiveG,
      EmissiveB,
      SpecularR,
      SpecularG,
      SpecularB,
      SpecularPower,
      OffsetX,
      OffsetY,
      TilingX,
      TilingY,
      MaxCount,
   };
}

const char * c_pParamNames[EParamID::MaxCount] =
{
   "VISIBILITY",
   "POSITION_X",
   "POSITION_Y",
   "POSITION_Z",
   "ROTATION_X",
   "ROTATION_Y",
   "ROTATION_Z",
   "SCALE_X",
   "SCALE_Y",
   "SCALE_Z",
   "SCENE_AMBIENT_R",
   "SCENE_AMBIENT_G",
   "SCENE_AMBIENT_B",
   "SCENE_AMBIENT_A",
   "CLEAR_R",
   "CLEAR_G",
   "CLEAR_B",
   "CLEAR_A",
   "DEPTH",
   "AMBIENT_R",
   "AMBIENT_G",
   "AMBIENT_B",
   "DIFFUSE_R",
   "DIFFUSE_G",
   "DIFFUSE_B",
   "EMISSIVE_R",
   "EMISSIVE_G",
   "EMISSIVE_B",
   "SPECULAR_R",
   "SPECULAR_G",
   "SPECULAR_B",
   "SPECULAR_POWER",
   "OFFSET_X",
   "OFFSET_Y",
   "TILING_X",
   "TILING_Y",
};
