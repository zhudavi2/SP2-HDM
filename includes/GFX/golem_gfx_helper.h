/******************************************************************************

golem_gfx_helper.h


Owner : Frederic O'Reilly

******************************************************************************/


#ifndef _GOLEM_GFX_HELPER_
#define _GOLEM_GFX_HELPER_

#include "golem_gfx_helper_itf.h"
#include "golem_gfx_implement_importer.h"
#include "golem_gfx_implement_node_importer.h"

struct GContourNode
{
   GVector2D<REAL32>    m_Point;
   UINT32               m_iIndex;
   GContourNode*        m_pPrev;
   GContourNode*        m_pNext;
};


class GContour
{
public:
   GContour()
   {
      m_pNodes = NULL;
      m_iNbNodes = 0;
   }

   GContour(const GContour& in_Copy)
   {
      m_pNodes = in_Copy.m_pNodes;
      m_iNbNodes = in_Copy.m_iNbNodes;
      m_BoundRect = in_Copy.m_BoundRect;
   }

   ~GContour()
   {
   }

   void Resize(UINT32 in_iNbSegments)
   {
      SAFE_DELETE_ARRAY(m_pNodes);
      m_iNbNodes = in_iNbSegments;
      if(in_iNbSegments > 0)
      {
         m_pNodes = new GContourNode[in_iNbSegments];
      }
   }

   Geometry::GRectangle<REAL32>   m_BoundRect;
   GContourNode*        m_pNodes;
   UINT32               m_iNbNodes;
};

class GContourList
{
public:
   vector<GContour> m_Elems;
};

enum ESplitPosition
{
   SPLIT_UNKNOWN = 0,
   SPLIT_INSIDE,
   SPLIT_OUTSIDE,
};

struct GSplitInfo
{
   ESplitPosition       m_CurPos;
   ESplitPosition       m_SplitPos;
   GContourNode*        m_pStart;
   GContourNode*        m_pCurrent;

   GSplitInfo()
   {
      m_CurPos = SPLIT_OUTSIDE;
      m_SplitPos = SPLIT_UNKNOWN;
      m_pStart = NULL;
      m_pCurrent = NULL;
   }

   GSplitInfo(const GSplitInfo& in_Copy)
   {
      m_CurPos = in_Copy.m_CurPos;
      m_SplitPos = in_Copy.m_SplitPos;
      m_pStart = in_Copy.m_pStart;
      m_pCurrent = in_Copy.m_pCurrent;
   }
};

class CGFX_Helper : public CGFX_Helper_Itf
{
protected:
   class CGFX_Renderer_Itf*      m_pRenderer;
   GFX::Implement::GImporter     m_GolemImporter;
   GFX::Implement::GNodeImporter m_GFXImporter;

   //! Create a mesh from 3ds data
   class GFX::GMesh* CreateMeshFrom3ds(  struct CMesh_3DS* in_3dsMesh, 
                                            bool in_CreateNormals, 
                                            bool in_PerVertexNormal);
/*
   template <typename TPoly>
   bool SplitMeshWithPolygon(class GFX::GMesh* io_SplitedMesh, const Geometry::GShape& in_SplittingShape, const TPoly& in_SplittingContour, bool in_bInside);
*/

   bool PointIsInsideContour(const GContour& in_Contour, const GVector2D<REAL32> in_Point);

   template <typename TPoly>
   void BuildContourList(const Geometry::GShape& in_ContourShape, const Geometry::GFaceList<TPoly>& in_ContourFaceList, GContourList& out_Contours, Geometry::GShape& io_ResultShape);

   template <typename TPoly>
   void BuildContour(const Geometry::GShape& in_ResultShape, UINT32 in_iShapeOffset, const TPoly& in_ContourPoly, GContour& out_Contour);

   bool SplitFacetListWithContour(Geometry::GShape& io_SplittedShape, 
                                  Geometry::GTriangleListIndexed& io_SplittedTris,
                                  vector<struct GSplitInfo>& io_vSplitInfo,
                                  GContour& in_Contour,
                                  bool in_bInside);

   template <typename TPoly>
   bool SplitFacetListTemplate(Geometry::GShape& io_SplittedShape, 
                               const Geometry::GFacetList& in_SplittedFacetList,
                               const Geometry::GShape& in_SplittingShape, 
                               const Geometry::GFaceList<TPoly>& in_SplittingFace,
                               Geometry::GFacetList* out_pOutsideFacetList,
                               Geometry::GFacetList* out_pInsideFacetList,
                               Geometry::GFacetList* out_pUnknownFacetList);

   template <typename TPoly>
   bool SplitMeshTemplate(class GFX::GMesh* io_SplitedMesh, const Geometry::GShape& in_SplittingShape, const Geometry::GFaceList<TPoly>& in_SplittingFace);

public:
   CGFX_Helper(CGFX_Renderer_Itf* in_pRenderer);

   class CGFX_Model_Itf* LoadModel3ds( class GFile& in_File, const GString& in_sTexturePath, bool in_CreateNormals, bool in_PerVertexNormal );

   class CGFX_Model_Itf* LoadGolem( class GFile& in_File, bool in_CreateStaticKeys, class CGFX_Action_Itf** out_Action );
   class CGFX_Model_Itf* LoadModelGolem( class GFile& in_File);
   class CGFX_Action_Itf* LoadActionGolem( class GFile& in_File, class CGFX_Model_Itf* in_Root, bool in_CreateStaticKeys );

   class CGFX_Scene_Itf* LoadGFXScene( class GFile& in_File );
   class CGFX_Model_Itf* LoadGFXModel( class GFile& in_File );

   class CGFX_Model_Itf* FindModel( class CGFX_Model_Itf* in_pParent, const GString& in_sName);
   class CGFX_Model_Itf* FindModelRecurse( class CGFX_Model_Itf* in_pParent, const GString& in_sName);

   void ComputeBoundingBox(CGFX_Model_Itf* in_pModel, Geometry::GBox<REAL32>& out_BBox);
   bool ComputeBoundingBox(CGFX_Model_Itf* in_pModel, Geometry::GBox<REAL32>& out_BBox, const GMatrix4x4<REAL32>& in_Transform);
   bool ComputeBoundingBox(GFX::GMesh* in_pMesh, Geometry::GBox<REAL32>& out_BBox);

   bool ScaleHierarchy(class CGFX_Model_Itf* in_Model, REAL32 in_Scale);
   bool ScaleAction(class CGFX_Action_Itf* in_Action, REAL32 in_Scale);

   void AdjustActionLength(class CGFX_Action_Itf* in_pAction);

   GFX::GMaterial* CreateCompleteMaterial(GFX::EVSType in_VSType = GFX::VSTYPE_DEF_LIGHT, GFX::EPSType in_PSType = GFX::PSTYPE_DEF_DIFFUSE, GFX::GDefaultVertexShader** out_pVShader = NULL, GFX::GCascadePixelShader** out_pPShader = NULL);
   GFX::GCascadePixelShader* CreateCascadeShader(GFX::EPSType in_PSType);

	GFX::GMesh* ConvertMesh(const GFX::GMeshComplex* in_pMeshComplex);

   bool CreateFace(class GFX::GMesh* in_Mesh, REAL32 in_Length, UINT32 in_TesselationFactor, UINT32 in_MeshCreationFlags = 0);
   bool CreateCube(class GFX::GMesh* in_Mesh, REAL32 in_Length, UINT32 in_TesselationFactor, UINT32 in_MeshCreationFlags = 0);
   bool CreateSphere(class GFX::GMesh* in_Mesh, REAL32 in_Radius, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0);
   bool CreateSphereSurfacePart(class GFX::GMesh* in_Mesh, REAL32 in_Radius, REAL32 in_LongitudeIn, REAL32 in_LongitudeOut, REAL32 in_LatitudeIn, REAL32 in_LatitudeOut, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0);
   bool CreateSpherePart(class GFX::GMesh* in_Mesh, REAL32 in_Radius, REAL32 in_LongitudeIn, REAL32 in_LongitudeOut, REAL32 in_LatitudeIn, REAL32 in_LatitudeOut, UINT32 in_LongitudeTesselationFactor, UINT32 in_LatitudeTesselationFactor = 0xffffffff, UINT32 in_MeshCreationFlags = 0);

   bool CreateCone(class GFX::GMesh* in_Mesh,
                   REAL32 in_fRadiusBottom,
                   REAL32 in_fRadiusTop,
                   REAL32 in_fHeight,
                   UINT32 in_iNbCapDivs,
                   UINT32 in_iNbHeightDivs,
                   UINT32 in_iNbSides,
                   UINT32 in_MeshCreationFlags = 0);

   class CGFX_Track_Itf* CreateConsoleCarretAnimation(class CGFX_Console_Itf* in_Console, REAL32 in_VisibleTime, REAL32 in_NonVisibleTime);

   bool SplitFacetList(Geometry::GShape& io_SplittedShape, 
                       const Geometry::GFacetList& in_SplittedFacetList,
                       const Geometry::GShape& in_SplittingShape, 
                       const Geometry::GFaceList<Geometry::GPolygon>& in_SplittingFace,
                       Geometry::GFacetList* out_pOutsideFacetList,
                       Geometry::GFacetList* out_pInsideFacetList,
                       Geometry::GFacetList* out_pUnknownFacetList);

   bool SplitFacetList(Geometry::GShape& io_SplittedShape, 
                       const Geometry::GFacetList& in_SplittedFacetList,
                       const Geometry::GShape& in_SplittingShape, 
                       const Geometry::GFaceList<Geometry::GPolygonIndexed>& in_SplittingFace,
                       Geometry::GFacetList* out_pOutsideFacetList,
                       Geometry::GFacetList* out_pInsideFacetList,
                       Geometry::GFacetList* out_pUnknownFacetList);

   bool SplitMesh(class GFX::GMesh* io_SplitedMesh, const Geometry::GShape& in_SplittingShape, const Geometry::GFaceList<Geometry::GPolygon>& in_SplittingFace);
   bool SplitMesh(class GFX::GMesh* io_SplitedMesh, const Geometry::GShape& in_SplittingShape, const Geometry::GFaceList<Geometry::GPolygonIndexed>& in_SplittingFace);

   ////////////////

   bool SplitTriangleList(const Geometry::GShape& in_SourceShape, 
                          const Geometry::GFacetList& in_SourceTriList,
                          const Geometry::GShape& in_ContoursShape, 
                          const Geometry::GFaceList<Geometry::GPolygon>& in_ContoursFaces,
                          Geometry::GShape& out_ResultShape,
                          Geometry::GFacetList& out_ResultTriList,
                          vector<UINT32>& out_ResultTriLocation);

   CGFX_Texture_Itf* LoadTexture(const GString& in_sTxtrFileName, bool in_bPartial = false);

   GFX::GTextureSequence* LoadTextureSequence(const GString& in_sTxtrFileName, 
                                                bool in_bPartial = false);

   CGFX_Animation_Linear_Itf* AnimateTextureSequence(GFX::GTextureSequence* in_pSequence, 
                                                      GFX::GTextureSequenceController* in_pController, 
                                                      REAL32 in_fFrameRate,
                                                      REAL32 in_fStartTime = 0.f);

   void SetupSpriteMesh(GFX::GMesh* io_pMesh, const CGFX_Texture_Itf* in_pTxtr);

   void SetupSpriteMesh(GFX::GMesh* io_pMesh, GColorRGBInt in_Color);

   void SetupSpriteMesh(GFX::GMesh* io_pMesh);

};

#endif //#ifndef _GOLEM_GFX_HELPER_