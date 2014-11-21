/******************************************************************************

golem_gfx_helper_itf.h


Owner : Frederic O'Reilly

******************************************************************************/


#ifndef _GOLEM_GFX_HELPER_ITF_
#define _GOLEM_GFX_HELPER_ITF_

#include "../../includes/hector/hector.h"

// Foward Declaration
class CGFX_Texture_Itf; 
class CGFX_Animation_Linear_Itf;


namespace GFX
{
   class GMaterial;
   class GMesh;
   class GMeshComplex;
   class GDefaultVertexShader;
   class GCascadePixelShader;
   class GTextureSequence;
   class GTextureSequenceController;


   enum EVSType
   {
      VSTYPE_DEF_NO_LIGHT = 0,
      VSTYPE_DEF_LIGHT,
      VSTYPE_COUNT,
   };

   enum EPSType
   {
      PSTYPE_DEF_DIFFUSE = 0,
      PSTYPE_DEF_FACTOR,
      PSTYPE_DEF_SIMPLE_TXTR,
      PSTYPE_DEF_DUAL_TXTR,
      PSTYPE_COUNT,
   };
}

const UINT32 HELPER_MESH_CREATE_NORMAL = 1;
const UINT32 HELPER_MESH_CREATE_UV = 2;
const UINT32 HELPER_MESH_CREATE_ALL = HELPER_MESH_CREATE_UV | HELPER_MESH_CREATE_NORMAL;

class CGFX_Helper_Itf
{
public:
	//! Load a mesh from a 3ds file
	virtual class CGFX_Model_Itf* LoadModel3ds( class GFile& in_File, const GString& in_sTexturePath, bool in_CreateNormals, bool in_PerVertexNormal ) = 0;

	//! Load a Model hierarchy and its animations from a Golem3dsExport XML file
	virtual class CGFX_Model_Itf* LoadGolem( class GFile& in_File, bool in_CreateStaticKeys, class CGFX_Action_Itf** out_Action ) = 0;

	//! Load a Model hierarchy from a Golem3dsExport XML file
	virtual class CGFX_Model_Itf* LoadModelGolem( class GFile& in_File ) = 0;

	//! Load an animation from a Golem3dsExport XML file
	virtual class CGFX_Action_Itf* LoadActionGolem( class GFile& in_File, class CGFX_Model_Itf* in_Root, bool in_CreateStaticKeys ) = 0;

   //! Load a complete Scene from a GFX file
   virtual class CGFX_Scene_Itf* LoadGFXScene( class GFile& in_File ) = 0;

   //! Load a complete model hierarchy from a GFX file
   virtual class CGFX_Model_Itf* LoadGFXModel( class GFile& in_File ) = 0;

   //! Find a child model by name
   virtual class CGFX_Model_Itf* FindModel( class CGFX_Model_Itf* in_pParent, const GString& in_sName) = 0;

   //! Find a child model by name in all its hierarchy
   virtual class CGFX_Model_Itf* FindModelRecurse( class CGFX_Model_Itf* in_pParent, const GString& in_sName) = 0;

   //! Return the bounding box of the whole hierarchy
   virtual void ComputeBoundingBox(CGFX_Model_Itf* in_pModel, Geometry::GBox<REAL32>& out_BBox) = 0;

   //! Compute the bounding sphere using the specified transformation
   virtual bool ComputeBoundingBox(CGFX_Model_Itf* in_pModel, Geometry::GBox<REAL32>& out_BBox, const GMatrix4x4<REAL32>& in_Transform) = 0;

   //! Return the bounding box of the specified mesh
   virtual bool ComputeBoundingBox(GFX::GMesh* in_pMesh, Geometry::GBox<REAL32>& out_BBox) = 0;

   //! Scales a hierarchy to the specified scale
   virtual bool ScaleHierarchy(class CGFX_Model_Itf* in_Model, REAL32 in_Scale) = 0;

   //! Scales all animations of an action to the specified scale
   virtual bool ScaleAction(class CGFX_Action_Itf* in_Action, REAL32 in_Scale) = 0;

   virtual void AdjustActionLength(class CGFX_Action_Itf* in_pAction) = 0;

   //!
   //! \brief Create a material with default values
   //!
   //! The created material has a single pass that uses a default vertex shader 
   //! and a cascade pixel shader.
   //! The values for each render state are specified and are initially set as follow :
   //!   - RSTATE_FILL_MODE : FILL_SOLID
   //!   - RSTATE_CULL_MODE : CULL_CCW
   //!   - RSTATE_SHADE_MODE : SHADE_GOURAUD
   //!   - RSTATE_ZBUFFER_ENABLE : TRUE
   //!   - RSTATE_ZBUFFER_WRITE_ENABLE : TRUE
   //!   - RSTATE_ZBUFFER_TEST_FUNCTION : TEST_LESSEQUAL
   //!   - RSTATE_ALPHA_TEST_ENABLE : FALSE
   //!   - RSTATE_ALPHA_TEST_REFERENCE : 0
   //!   - RSTATE_ALPHA_TEST_FUNC : TEST_ALWAYS
   //!   - RSTATE_ALPHA_BLEND_ENABLE : FALSE
   //!   - RSTATE_ALPHA_BLEND_FUNCTION : BLEND_ZERO
   //!   - RSTATE_ALPHA_SOURCE_BLEND : BLEND_ONE
   //!   - RSTATE_ALPHA_DEST_BLEND : BLEND_ZERO
   //!   - RSTATE_ALPHA_BLEND_FACTOR : 0xFFFFFFFF (R = G = B = A = 255)
   //!   - RSTATE_DITHERING_ENABLE : FALSE
   //!   - RSTATE_COLOR_WRITE_ENABLE : COLOR_WRITE_RED | COLOR_WRITE_GREEN | COLOR_WRITE_BLUE | COLOR_WRITE_ALPHA
   virtual GFX::GMaterial* CreateCompleteMaterial(GFX::EVSType in_VSType = GFX::VSTYPE_DEF_LIGHT, GFX::EPSType in_PSType = GFX::PSTYPE_DEF_DIFFUSE, GFX::GDefaultVertexShader** out_pVShader = NULL, GFX::GCascadePixelShader** out_pPShader = NULL) = 0;

   virtual GFX::GCascadePixelShader* CreateCascadeShader(GFX::EPSType in_PSType) = 0;

	//! Convert a complex mesh into a (simple) mesh
	virtual GFX::GMesh* ConvertMesh(const GFX::GMeshComplex* in_pMeshComplex) = 0;

	//! Fill a face mesh
	virtual bool CreateFace(class GFX::GMesh* in_Mesh, REAL32 in_Length, UINT32 in_TesselationFactor, UINT32 in_MeshCreationFlags = 0) = 0;

	//! Fill a cube mesh
	virtual bool CreateCube(class GFX::GMesh* in_Mesh, REAL32 in_Length, UINT32 in_TesselationFactor, UINT32 in_MeshCreationFlags = 0) = 0;

	//! Fill a sphere mesh
	virtual bool CreateSphere(class GFX::GMesh* in_Mesh, REAL32 in_Radius, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0) = 0;

	//! Fill a partial sphere surface mesh
	virtual bool CreateSphereSurfacePart(class GFX::GMesh* in_Mesh, REAL32 in_Radius, REAL32 in_LongitudeIn, REAL32 in_LongitudeOut, REAL32 in_LatitudeIn, REAL32 in_LatitudeOut, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0) = 0;

	//! Fill a partial sphere mesh
	virtual bool CreateSpherePart(class GFX::GMesh* in_Mesh, REAL32 in_Radius, REAL32 in_LongitudeIn, REAL32 in_LongitudeOut, REAL32 in_LatitudeIn, REAL32 in_LatitudeOut, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0) = 0;

	//! Fill a cone mesh
	virtual bool CreateCone(class GFX::GMesh* in_Mesh,
                           REAL32 in_fRadiusBottom,
                           REAL32 in_fRadiusTop,
                           REAL32 in_fHeight,
                           UINT32 in_iNbCapDivs,
                           UINT32 in_iNbHeightDivs,
                           UINT32 in_iNbSides,
                           UINT32 in_MeshCreationFlags = 0) = 0;

   //! Create a track to automatically animate a console carret
   virtual class CGFX_Track_Itf* CreateConsoleCarretAnimation(class CGFX_Console_Itf* in_Console, REAL32 in_VisibleTime, REAL32 in_NonVisibleTime) = 0;

   //! Split a facet list using the specified face (can be inplace)
   virtual bool SplitFacetList(Geometry::GShape& io_SplittedShape, 
                               const Geometry::GFacetList& in_SplittedFacetList,
                               const Geometry::GShape& in_SplittingShape, 
                               const Geometry::GFaceList<Geometry::GPolygon>& in_SplittingFace,
                               Geometry::GFacetList* out_pOutsideFacetList,
                               Geometry::GFacetList* out_pInsideFacetList,
                               Geometry::GFacetList* out_pUnknownFacetList) = 0;

   //! Split a facet list using the specified face (can be inplace)
   virtual bool SplitFacetList(Geometry::GShape& io_SplittedShape, 
                               const Geometry::GFacetList& in_SplittedFacetList,
                               const Geometry::GShape& in_SplittingShape, 
                               const Geometry::GFaceList<Geometry::GPolygonIndexed>& in_SplittingFace,
                               Geometry::GFacetList* out_pOutsideFacetList,
                               Geometry::GFacetList* out_pInsideFacetList,
                               Geometry::GFacetList* out_pUnknownFacetList) = 0;

   //! Split a mesh using the specified face
   virtual bool SplitMesh(class GFX::GMesh* io_SplittedMesh, 
                          const Geometry::GShape& in_SplittingShape, 
                          const Geometry::GFaceList<Geometry::GPolygon>& in_SplittingFace) = 0;

   //! Split a mesh using the specified face 
   virtual bool SplitMesh(class GFX::GMesh* io_SplittedMesh, 
                          const Geometry::GShape& in_SplittingShape, 
                          const Geometry::GFaceList<Geometry::GPolygonIndexed>& in_SplittingFace) = 0;

   ///////

   //! \brief Split a source triangle list using the specified face list of contours (can be inplace)
   //!
   //! \remarks To work properly, the triangle list must be 2D & triangles must not overlap
   //! Other conditions (algorithm could be modified to remove those restrictions) :
   //!    Mesh must not contain holes, segments of contour cannot split a triangle without
   //!    having at least one of its enpoint inside a triangle.
   //!
   virtual bool SplitTriangleList(const Geometry::GShape& in_SourceShape, 
                                  const Geometry::GFacetList& in_SourceTriList,
                                  const Geometry::GShape& in_ContoursShape, 
                                  const Geometry::GFaceList<Geometry::GPolygon>& in_ContoursFaces,
                                  Geometry::GShape& out_ResultShape,
                                  Geometry::GFacetList& out_ResultTriList,
                                  vector<UINT32>& out_ResultTriLocation) = 0;

   //JMERCIER : TO DESCRIBE!!!
   virtual CGFX_Texture_Itf* LoadTexture(const GString& in_sTxtrFileName, bool in_bPartial = false) = 0;

   virtual GFX::GTextureSequence* LoadTextureSequence(const GString& in_sTxtrFileName, 
                                                bool in_bPartial = false) = 0;

   virtual CGFX_Animation_Linear_Itf* AnimateTextureSequence(GFX::GTextureSequence* in_pSequence, 
                                                      GFX::GTextureSequenceController* in_pController, 
                                                      REAL32 in_fFrameRate,
                                                      REAL32 in_fStartTime = 0.f) = 0;

   virtual void SetupSpriteMesh(GFX::GMesh* io_pMesh, const CGFX_Texture_Itf* in_pTxtr) = 0;

   virtual void SetupSpriteMesh(GFX::GMesh* io_pMesh, GColorRGBInt in_Color) = 0;

   virtual void SetupSpriteMesh(GFX::GMesh* io_pMesh) = 0;


};

#endif //#ifndef _GOLEM_GFX_HELPER_ITF_
