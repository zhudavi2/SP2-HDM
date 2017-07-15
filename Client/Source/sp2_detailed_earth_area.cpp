/**************************************************************
*
* sp2_detailed_earth_area.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_detailed_earth_area.h"

#include "../../hector/include/golem_quad_tree.h"

//! This class is used to quickly find a triangle
class GTriFinder
{
public:
   //! Constructs a new triangle finder using the specified mesh (only first facet list is used)
   GTriFinder(const CGFX_Mesh_Itf* in_pMesh);

   //! Returns the mesh associated with this triangle finder
   const CGFX_Mesh_Itf* Mesh() const;

   //! Returns a triangle index that intersect the specified depth ray
   //! (a ray that passes through the points (x,y,0) & (x,y,-1) )
   bool Locate(const GVector2D<REAL32>& in_DepthRay, UINT32& out_iResult);

   //! Returns an array of triangle indexes that intersect the specified depth ray
   void Locate(const GVector2D<REAL32>& in_DepthRay, vector<UINT32>& out_Result);

   //! Returns the location of the first triangle that is intersected with the specified ray
   bool Locate(const GRay<REAL32>& in_Ray, GVector3D<REAL32>& out_Location, UINT32& out_iTriangle);

private:
   const CGFX_Mesh_Itf*       m_pMesh;

   struct GTriElem
   {
      GRectangle<REAL32>      m_Bounding;
      vector<UINT32>          m_Tris;
   };

   GQuadTree<GTriElem>        m_TriTree;
   REAL32                     m_fHeigthMin;
   REAL32                     m_fHeigthMax;

   void CreateQuadTree(GQuadTree<GTriElem>::GNode in_Root, UINT32 in_iNbLevels);
   GQuadTree<GTriElem>::GNode GTriFinder::FindNode(GQuadTree<GTriElem>::GNode in_Root,
                                                   const GVector2D<REAL32>& in_A,
                                                   const GVector2D<REAL32>& in_B,
                                                   const GVector2D<REAL32>& in_C) const;

   bool LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, const GVector2D<REAL32>& in_DepthRay, UINT32& out_iResult);
   void LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, const GVector2D<REAL32>& in_DepthRay, vector<UINT32>& out_Result);

   struct GLocationInfo
   {
      GRay<REAL32>      m_Ray;
      GVector3D<REAL32> m_TopIntersect;
      GVector3D<REAL32> m_BottomIntersect;
      GVector3D<REAL32> m_IntersectedPos;
      UINT32            m_IntersectedTri;
   };

   bool LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, GTriFinder::GLocationInfo* in_pLocationInfo);
};

GTriFinder::GTriFinder(const CGFX_Mesh_Itf* in_pMesh)
{
   m_fHeigthMin = 0;
   m_fHeigthMin = 0;

   if( (in_pMesh->NbFacetLists() > 0) && (in_pMesh->Facet_List(0).Positions().Type() == FACETS_TRIANGLE) && (in_pMesh->Facet_List(0).Positions().Indexed() ) )
   {
      m_pMesh = in_pMesh;
      const GShape& l_Shape = m_pMesh->Shape();

      // Find bounding rect of mesh
      GRectangle<REAL32>& l_BoundRect = m_TriTree.Root().Data().m_Bounding;
      l_BoundRect.Set(l_Shape.Position(0).x, l_Shape.Position(0).y, 0, 0);

      for(UINT32 p = 0;p < l_Shape.NbPositions();p ++)
      {
         // Update bounding rect
         l_BoundRect.Union(GRectangle<REAL32>(l_Shape.Position(p).x, l_Shape.Position(p).y, 0, 0) );
      }

      // Calc number of quad tree levels required
      const GTriangleListIndexed& l_Tris = m_pMesh->Facet_List(0).Positions().TriangleListIndexed();
      UINT32 l_NbTris = l_Tris.NbTriangles();
      UINT32 l_NbLevels = (UINT32) (logf( (REAL32) l_NbTris) / logf(4.f) ) - 1;

      CreateQuadTree(m_TriTree.Root(), l_NbLevels);

      // Fill the quad tree
      for(UINT32 t = 0;t < l_Tris.NbTriangles();t ++)
      {
         // Fetch current triangle corners
         const GTriangleIndex& l_TriIndex = l_Tris.Triangle(t);
         GVector2D<REAL32>* l_pA = (GVector2D<REAL32>*) &l_Shape.Position(l_TriIndex.m_pVertices[0] );
         GVector2D<REAL32>* l_pB = (GVector2D<REAL32>*) &l_Shape.Position(l_TriIndex.m_pVertices[1] );
         GVector2D<REAL32>* l_pC = (GVector2D<REAL32>*) &l_Shape.Position(l_TriIndex.m_pVertices[2] );

         // Insert this triangle in the quad tree
         GQuadTree<GTriElem>::GNode l_Node = FindNode(m_TriTree.Root(), *l_pA, *l_pB, *l_pC);
         l_Node.Data().m_Tris.push_back(t);

         m_fHeigthMin = min(m_fHeigthMin, l_Shape.Position(l_TriIndex.m_pVertices[0] ).z);
         m_fHeigthMin = min(m_fHeigthMin, l_Shape.Position(l_TriIndex.m_pVertices[1] ).z);
         m_fHeigthMin = min(m_fHeigthMin, l_Shape.Position(l_TriIndex.m_pVertices[2] ).z);

         m_fHeigthMax = max(m_fHeigthMax, l_Shape.Position(l_TriIndex.m_pVertices[0] ).z);
         m_fHeigthMax = max(m_fHeigthMax, l_Shape.Position(l_TriIndex.m_pVertices[1] ).z);
         m_fHeigthMax = max(m_fHeigthMax, l_Shape.Position(l_TriIndex.m_pVertices[2] ).z);
      }
   }
   else
   {
      m_pMesh = NULL;
   }
}

void GTriFinder::Locate(const GVector2D<REAL32>& in_DepthRay, vector<UINT32>& out_Result)
{
   // Quickly reject points outside of rect
   if(m_TriTree.Root().Data().m_Bounding.PointInRect(in_DepthRay) && m_pMesh)
   {
      LocateRecurse(m_TriTree.Root(), in_DepthRay, out_Result);
   }
}

void GTriFinder::LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, const GVector2D<REAL32>& in_DepthRay, vector<UINT32>& out_Result)
{
   const GShape& l_Shape = m_pMesh->Shape();
   const GTriangleListIndexed& l_Tris = m_pMesh->Facet_List(0).Positions().TriangleListIndexed();

   for(UINT32 t = 0;t < in_Root.Data().m_Tris.size();t ++)
   {
      GVector2D<REAL32> l_TriPoints[3];
      UINT32 l_TriIndex = in_Root.Data().m_Tris[t];
      l_TriPoints[0] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[0] );
      l_TriPoints[1] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[1] );
      l_TriPoints[2] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[2] );

      if(PointIsInsideTriangleCW(l_TriPoints, in_DepthRay) )
      {
         out_Result.push_back(l_TriIndex);
      }
   }

   for(UINT32 c = 0;c < in_Root.NbChildren();c ++)
   {
      if(in_Root.Child(c).Data().m_Bounding.PointInRect(in_DepthRay) )
      {
         return LocateRecurse(in_Root.Child(c), in_DepthRay, out_Result);
      }
   }
}

bool GTriFinder::Locate(const GVector2D<REAL32>& in_DepthRay, UINT32& out_iResult)
{
   // Quickly reject points outside of rect
   if(m_TriTree.Root().Data().m_Bounding.PointInRect(in_DepthRay) && m_pMesh)
   {
      return LocateRecurse(m_TriTree.Root(), in_DepthRay, out_iResult);
   }

   return false;
}

bool GTriFinder::LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, const GVector2D<REAL32>& in_DepthRay, UINT32& out_iResult)
{
   const GShape& l_Shape = m_pMesh->Shape();
   const GTriangleListIndexed& l_Tris = m_pMesh->Facet_List(0).Positions().TriangleListIndexed();

   for(UINT32 t = 0;t < in_Root.Data().m_Tris.size();t ++)
   {
      GVector2D<REAL32> l_TriPoints[3];
      UINT32 l_TriIndex = in_Root.Data().m_Tris[t];
      l_TriPoints[0] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[0] );
      l_TriPoints[1] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[1] );
      l_TriPoints[2] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[2] );

      if(PointIsInsideTriangleCW(l_TriPoints, in_DepthRay) )
      {
         out_iResult = l_TriIndex;
         return true;
      }
   }

   for(UINT32 c = 0;c < in_Root.NbChildren();c ++)
   {
      if(in_Root.Child(c).Data().m_Bounding.PointInRect(in_DepthRay) )
      {
         return LocateRecurse(in_Root.Child(c), in_DepthRay, out_iResult);
      }
   }

   return false;
}

bool GTriFinder::LocateRecurse(GQuadTree<GTriElem>::GNode in_Root, GTriFinder::GLocationInfo* in_pLocationInfo)
{
   const GShape& l_Shape = m_pMesh->Shape();
   const GTriangleListIndexed& l_Tris = m_pMesh->Facet_List(0).Positions().TriangleListIndexed();

   for(UINT32 t = 0;t < in_Root.Data().m_Tris.size();t ++)
   {
      GVector3D<REAL32> l_TriPoints[3];
      UINT32 l_TriIndex = in_Root.Data().m_Tris[t];
      l_TriPoints[0] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[0] );
      l_TriPoints[1] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[1] );
      l_TriPoints[2] = l_Shape.Position(l_Tris.Triangle(l_TriIndex).m_pVertices[2] );

      REAL32 l_fDistance;
      REAL32 l_fDummyU, l_fDummyV;

      if(in_pLocationInfo->m_Ray.IntersectTriangle(l_TriPoints, l_fDistance, l_fDummyU, l_fDummyV) )
      {
         if(l_fDistance >= 0)
         {
            GVector3D<REAL32> l_CurIntersection = in_pLocationInfo->m_Ray.m_Center + in_pLocationInfo->m_Ray.m_Direction * l_fDistance;
            if(in_pLocationInfo->m_IntersectedTri == 0xFFFFFFFF)
            {
               in_pLocationInfo->m_IntersectedTri = l_TriIndex;
               in_pLocationInfo->m_IntersectedPos = l_CurIntersection;
            }
            else if(l_CurIntersection.DistanceSquared(in_pLocationInfo->m_Ray.m_Center) < in_pLocationInfo->m_IntersectedPos.DistanceSquared(in_pLocationInfo->m_Ray.m_Center) )
            {
               in_pLocationInfo->m_IntersectedTri = l_TriIndex;
               in_pLocationInfo->m_IntersectedPos = l_CurIntersection;
            }
         }
      }
   }

   for(UINT32 c = 0;c < in_Root.NbChildren();c ++)
   {
      LocateRecurse(in_Root.Child(c), in_pLocationInfo);
   }

   return false;
}

bool GTriFinder::Locate(const GRay<REAL32>& in_Ray, GVector3D<REAL32>& out_Location, UINT32& out_iTriangle)
{
   // Intersect ray with max & min triangle height
   GLocationInfo l_LocInfo;
   l_LocInfo.m_Ray = in_Ray;
   l_LocInfo.m_IntersectedTri = 0xFFFFFFFF;

   bool l_fIntersected = l_LocInfo.m_Ray.IntersectPlane( GVector3D<REAL32>(0,0,1), -m_fHeigthMin, l_LocInfo.m_BottomIntersect);
   if(!l_fIntersected)
      return false;

   l_fIntersected = l_LocInfo.m_Ray.IntersectPlane( GVector3D<REAL32>(0,0,1), -m_fHeigthMax, l_LocInfo.m_TopIntersect);
   if(!l_fIntersected)
      return false;

   // Quickly verify if intersections are outside of bounding rect
   GRectangle<REAL32>& l_Bounds = m_TriTree.Root().Data().m_Bounding;
   if( ( (l_LocInfo.m_TopIntersect.x < l_Bounds.Left()   ) && (l_LocInfo.m_BottomIntersect.x < l_Bounds.Left()   ) ) ||
       ( (l_LocInfo.m_TopIntersect.x > l_Bounds.Right()  ) && (l_LocInfo.m_BottomIntersect.x > l_Bounds.Right()  ) ) ||
       ( (l_LocInfo.m_TopIntersect.y < l_Bounds.Top()    ) && (l_LocInfo.m_BottomIntersect.y < l_Bounds.Top()    ) ) ||
       ( (l_LocInfo.m_TopIntersect.y > l_Bounds.Bottom() ) && (l_LocInfo.m_BottomIntersect.y > l_Bounds.Bottom() ) ) )
   {
      return false;
   }

   // Recurse through quad tree to find closets intersecting triangle
   LocateRecurse(m_TriTree.Root(), &l_LocInfo);

   if(l_LocInfo.m_IntersectedTri == 0xFFFFFFFF)
   {
      return false;
   }

   out_iTriangle = l_LocInfo.m_IntersectedTri;
   out_Location = l_LocInfo.m_IntersectedPos;

   return true;
}

void GTriFinder::CreateQuadTree(GQuadTree<GTriElem>::GNode in_Root, UINT32 in_iNbLevels)
{
   REAL32 l_fHalfX = in_Root.Data().m_Bounding.m_Left + in_Root.Data().m_Bounding.m_Width * 0.5f;
   REAL32 l_fHalfY = in_Root.Data().m_Bounding.m_Top + in_Root.Data().m_Bounding.m_Height * 0.5f;

   in_Root.AddQuadChildren();

   GTriElem l_New;
   l_New.m_Bounding.m_Left = in_Root.Data().m_Bounding.m_Left;
   l_New.m_Bounding.Right(l_fHalfX);
   l_New.m_Bounding.m_Top = in_Root.Data().m_Bounding.m_Top;
   l_New.m_Bounding.Bottom(l_fHalfY);
   in_Root.Child(0).Data() = l_New;

   l_New.m_Bounding.m_Left = l_fHalfX;
   l_New.m_Bounding.Right(in_Root.Data().m_Bounding.Right() );
   in_Root.Child(1).Data() = l_New;

   l_New.m_Bounding.m_Left = in_Root.Data().m_Bounding.m_Left;
   l_New.m_Bounding.Right(l_fHalfX);
   l_New.m_Bounding.m_Top = l_fHalfY;
   l_New.m_Bounding.Bottom(in_Root.Data().m_Bounding.Bottom() );
   in_Root.Child(2).Data() = l_New;

   l_New.m_Bounding.m_Left = l_fHalfX;
   l_New.m_Bounding.Right(in_Root.Data().m_Bounding.Right() );
   in_Root.Child(3).Data() = l_New;

   if(in_iNbLevels != 0)
   {
      for(UINT32 c = 0;c < in_Root.NbChildren();c ++)
      {
         CreateQuadTree(in_Root.Child(c), in_iNbLevels - 1);
      }
   }
}

GQuadTree<GTriFinder::GTriElem>::GNode GTriFinder::FindNode(GQuadTree<GTriElem>::GNode in_Root,
                                                            const GVector2D<REAL32>& in_A,
                                                            const GVector2D<REAL32>& in_B,
                                                            const GVector2D<REAL32>& in_C) const
{
   for(UINT32 c = 0;c < in_Root.NbChildren();c ++)
   {
      if( (in_Root.Child(c).Data().m_Bounding.PointInRect(in_A) ) &&
          (in_Root.Child(c).Data().m_Bounding.PointInRect(in_B) ) &&
          (in_Root.Child(c).Data().m_Bounding.PointInRect(in_C) ) )
      {
         return FindNode(in_Root.Child(c), in_A, in_B, in_C);
      }
   }

   return in_Root;
}

// Earth radius in meters
const REAL32 EARTH_RADIUS = 6378134.3444077055809380168171535f;

// Land mesh color indexes
enum ETerrainType
{
   TERRAIN_WATER = 0,
   TERRAIN_LAND,
   TERRAIN_RIVER,
   TERRAIN_SELECTED, 
   TERRAin_iCount,
};


const GString GDetailedEarthArea::TypeName(L"DETAILED_EARTH_AREA");

/*!
* Function which is called to create a Detailed Earth Area GUI object
**/
GUI::GBaseObject* GDetailedEarthArea::New()
{
   return new GDetailedEarthArea();
}

/*!
* GDetailedEarthArea default constructor which sets all GFX member objects as empty
* & initializes all other members to default values.
**/
GDetailedEarthArea::GDetailedEarthArea() : GUIContainer()
{
   // Reset GFX objects
   m_SkyCamera       = NULL;
   m_SkyRList        = NULL;

   m_Scene           = NULL;

   m_CameraModel     = NULL;
   m_CameraTransform = NULL;
   m_Camera          = NULL;

   m_SunModel        = NULL;
   m_SunTransform    = NULL;
   m_SunLight        = NULL;

   m_LandModel       = NULL;
   m_LandMesh        = NULL;
   m_LandMaterial    = NULL;
   m_LandTexture     = NULL;

   m_OutlineModel    = NULL;
   m_OutlineMesh     = NULL;

   m_GizmoModel      = NULL;
   m_GizmoTransform  = NULL;
   m_GizmoMesh       = NULL;

   m_RList           = NULL;

   m_FreeCamSpeed    = 0.00625f;
   m_LandCamSpeed    = 0.0015625f;
   m_NbDivs          = 100;

   m_SubScene        = NULL;
   m_SubTransform    = NULL;
	m_SubRList        = NULL;
   m_ClickText       = NULL;

   // Reset children GUI objects
   m_CloseButton     = NULL;

   m_TriFinder       = NULL;

   m_pUnitPathModel      = NULL;

}

/*!
 * GDetailedEarthArea destructor which ensures that alll GFX members objects 
 * are correctly released.
 **/

/*!
* Creation of all GFX objects used to display the Detailed Earth Area.
* Also, creation of children GUI objects.
* For parameters, see GUI::GBaseObject::OnCreate
**/
bool GDetailedEarthArea::OnCreate()
{
   __super::OnCreate();

   GFile l_File;
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   // Create sky box scene
   CGFX_Scene_Itf* l_SkyScene = g_Joshua.Renderer()->Create_Scene();

   // Create sky box camera
   m_SkyCamera = g_Joshua.Renderer()->Create_Camera();
   m_SkyCamera->View_Type(GFX::VIEW_PERSPECTIVE);
   m_SkyCamera->Near_Plane() = 0.1f;
   m_SkyCamera->Far_Plane() = 10;

   // Create sky box
   CGFX_Model_Itf* l_SkyModel = g_Joshua.Renderer()->Create_Model();
   CGFX_Mesh_Itf* l_SkyMesh = g_Joshua.Renderer()->Create_Mesh();
   GFX::GMaterial* l_SkyMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
   g_Joshua.FileManager()->File(L"SkyBox/SkyBox1.bmp", l_File);
   CGFX_Texture_Itf* l_SkyTexture = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, false, GString( "TxtSky" ) );

   // Setup sky box scene
   l_SkyScene->Get_Root_Model()->Set_Primitive(m_SkyCamera);
   l_SkyScene->Get_Root_Model()->Add_Child(l_SkyModel);
   l_SkyModel->Set_Primitive(l_SkyMesh);
   l_SkyModel->Set_Material(l_SkyMaterial);

   // Initialize sky material
   l_SkyMaterial->PixelShader(0)->TextureSampler(0)->Texture(l_SkyTexture);
   l_SkyMaterial->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);

   l_SkyMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
   l_SkyMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
   l_SkyMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;

   // Initialize sky mesh
   g_Joshua.Renderer()->Helper()->CreateCube(l_SkyMesh, -1.f, 1, HELPER_MESH_CREATE_UV);

   // Setup sky box render list
   m_SkyRList = l_SkyScene->Create_Render_List();
   g_Joshua.Renderer()->RenderTree()->Add_Render_List(m_SkyRList);

   // Create terrain gfx objects

   // Create scene
   m_Scene = g_Joshua.Renderer()->Create_Scene();

   // Create camera
   m_Camera = g_Joshua.Renderer()->Create_Camera();
   m_CameraModel = g_Joshua.Renderer()->Create_Model();
   m_CameraTransform = g_Joshua.Renderer()->Create_Transform_SRT();

   // Create sun
   m_SunModel = g_Joshua.Renderer()->Create_Model();
   m_SunTransform = g_Joshua.Renderer()->Create_Transform_SRT();
   m_SunLight = g_Joshua.Renderer()->Create_Light();

   // Create land
   m_LandModel = g_Joshua.Renderer()->Create_Model();
   m_LandMesh = g_Joshua.Renderer()->Create_Mesh();
   m_LandMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);

   g_Joshua.FileManager()->File(L"Detail.bmp", l_File);
   m_LandTexture = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, false, GString( "TxtLand" ) );

   // Create outline
   m_OutlineModel = g_Joshua.Renderer()->Create_Model();
   m_OutlineMesh = g_Joshua.Renderer()->Create_Mesh();

   // Create water
   m_WaterModel = g_Joshua.Renderer()->Create_Model();
   m_WaterMesh = g_Joshua.Renderer()->Create_Mesh();

   // Create location gizmo
   m_GizmoModel = g_Joshua.Renderer()->Create_Model();
   m_GizmoTransform = g_Joshua.Renderer()->Create_Transform_SRT();
   m_GizmoMesh = g_Joshua.Renderer()->Create_Mesh();

   // Initialize background scene
   m_Scene->ClearFrameBuffer(false);
   m_Scene->GlobalAmbient() = GColorRGBReal(0,0,0,1);

   // Initialize camera
   m_Scene->Set_Active_Camera(m_Camera);
   m_Scene->Get_Root_Model()->Add_Child(m_CameraModel);
   m_CameraModel->Set_Primitive(m_Camera);
   m_CameraModel->Set_Transform(m_CameraTransform);
   m_Camera->View_Type(GFX::VIEW_PERSPECTIVE);
   m_Camera->Near_Plane() = 0.0001f;
   m_Camera->Far_Plane() = 1;
   m_Camera->Look_At().X = 0;
   m_Camera->Look_At().Y = 1;
   m_Camera->Look_At().Z = -0.01f;
   m_Camera->Up_Vector().X = 0;
   m_Camera->Up_Vector().Y = 0;
   m_Camera->Up_Vector().Z = -1;

   m_SkyCamera->Look_At() = m_Camera->Look_At();
   m_SkyCamera->Look_At().Z = 0;
   m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

   m_CameraTransform->Translation().X = 0;
   m_CameraTransform->Translation().Y = 0;
   m_CameraTransform->Translation().Z = -0.01f;

   // Initialize sun
   m_Scene->Get_Root_Model()->Add_Child(m_SunModel);
   m_SunModel->Set_Primitive(m_SunLight);
   m_SunModel->Set_Transform(m_SunTransform);
   m_SunLight->On() = true;
   m_SunLight->Range() = 100;
   m_SunLight->Attenuation_Linear() = 0.01f;
   m_SunLight->Color() = GColorRGBReal(1,1,1,1);

   m_SunTransform->Translation().X = -1.f;
   m_SunTransform->Translation().Z = -1.f;

   // Initialize land Model
   m_Scene->Get_Root_Model()->Add_Child(m_LandModel );
   m_LandModel->Set_Material(m_LandMaterial);
   m_LandModel->Set_Primitive(m_LandMesh);

   // Initialize land material
//   m_LandMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
//   m_LandMaterial->RenderStates(0)->Value(GFX::RSTATE_FILL_MODE) = GFX::FILL_WIREFRAME;

   m_LandMaterial->PixelShader(0)->TextureSampler(0)->Texture(m_LandTexture);
   m_LandMaterial->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
   m_LandMaterial->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_WRAP);
   m_LandMaterial->PixelShader(0)->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_WRAP);

   ( (GFX::GDefaultVertexShader*) m_LandMaterial->VertexShader(0) )->Diffuse() = GColorRGBReal(0,0.4f,0,1);
   ( (GFX::GDefaultVertexShader*) m_LandMaterial->VertexShader(0) )->Ambient() = GColorRGBReal(0,0.4f,0,1);
//   ( (GFX::GDefaultVertexShader*) m_LandMaterial->VertexShader(0) )->LightEnable() = false;

   // Initialize outline Model
   m_Scene->Get_Root_Model()->Add_Child(m_OutlineModel);
   m_OutlineModel->Set_Primitive(m_OutlineMesh);

   // Initialize water Model
   m_Scene->Get_Root_Model()->Add_Child(m_WaterModel);
   m_WaterModel->Set_Primitive(m_WaterMesh);

   // Initialize location gizmo
   //m_Scene->Get_Root_Model()->Add_Child(m_GizmoModel);
//   m_GizmoModel->Visibility() = false;
   m_GizmoModel->Set_Transform(m_GizmoTransform);
   m_GizmoModel->Set_Material(m_Scene->DefaultMaterial() );
   m_GizmoModel->Set_Primitive(m_GizmoMesh);

   g_Joshua.FileManager()->File(L"gizmo.3ds", l_File);
   m_GizmoModel->Add_Child(g_Joshua.Renderer()->Helper()->LoadModel3ds(l_File, ".", true, true) );

   g_Joshua.Renderer()->Helper()->ScaleHierarchy(m_GizmoModel, 0.01f);

   // Setup background rendering
   m_RList = m_Scene->Create_Render_List();
   g_Joshua.Renderer()->RenderTree()->Add_Render_List(m_RList);

   // Start animations
   m_Scene->Get_Animation_System()->Play_Animation();

   // Sub-scene creation
   // Create scene
   m_SubScene = g_Joshua.Renderer()->Create_Scene();

   // Create camera
   CGFX_Camera_Itf* l_SubCamera = g_Joshua.Renderer()->Create_Camera();

   // Create orientation gizmo Model
   CGFX_Model_Itf* l_SubModel = g_Joshua.Renderer()->Create_Model();

   // Create orientation gizmo mesh
   CGFX_Mesh_Itf* l_SubMesh = g_Joshua.Renderer()->Create_Mesh();

   // Create orientation gizmo material
   GFX::GMaterial* l_SubMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT);

   // Create orientation gizmo transform
   m_SubTransform = g_Joshua.Renderer()->Create_Transform();

   // Setup scene
   m_SubScene->ClearFrameBuffer(false);
   m_SubScene->Get_Root_Model()->Set_Primitive(l_SubCamera);
   m_SubScene->Get_Root_Model()->Add_Child(l_SubModel);

   // Setup camera
   l_SubCamera->View_Type(GFX::VIEW_ORTHOGONAL);
   l_SubCamera->Near_Plane() = -10.f;
   l_SubCamera->Far_Plane() = 10.f;
   l_SubCamera->Look_At().X = 0;
   l_SubCamera->Look_At().Y = 0;
   l_SubCamera->Look_At().Z = 1;
   l_SubCamera->Up_Vector().X = 0;
   l_SubCamera->Up_Vector().Y = 1;
   l_SubCamera->Up_Vector().Z = 0;

   // Setup orientation gizmo Model
   l_SubModel->Set_Primitive(l_SubMesh);
   l_SubModel->Set_Material(l_SubMaterial);
   l_SubModel->Set_Transform(m_SubTransform);

   // Setup orientation gizmo material

   // Setup orientation gizmo transform
   GMatrix4x4<REAL32> l_TranslationMat;
   l_TranslationMat.LoadIdentity();
   l_TranslationMat(1,1) = -1;
   l_TranslationMat(3,0) = 25;
   l_TranslationMat(3,1) = 25;
   l_TranslationMat(3,2) = 0;

   m_SubTransform->Set_Matrix(l_TranslationMat);

   // Setup orientation gizmo mesh
   GShape& l_Shape = l_SubMesh->Shape();
   l_Shape.NbPositions(4);
   l_Shape.Position(0) = GVector3D<REAL32>(0,0,0);
   l_Shape.Position(1) = GVector3D<REAL32>(20,0,0);
   l_Shape.Position(2) = GVector3D<REAL32>(0,20,0);
   l_Shape.Position(3) = GVector3D<REAL32>(0,0,20);

   l_Shape.NbColors(3);
   l_Shape.Color(0) = GColorRGBInt(255, 0, 0, 128);
   l_Shape.Color(1) = GColorRGBInt(0, 255, 0, 128);
   l_Shape.Color(2) = GColorRGBInt(0, 0, 255, 128);

   l_SubMesh->NbFacetLists(1);
   l_SubMesh->Facet_List(0).Positions().Type(FACETS_LINE, true);
   GLineListIndexed& l_PosLines = l_SubMesh->Facet_List(0).Positions().LineListIndexed();

   l_PosLines.NbLines(3);
   l_PosLines.Line(0).m_pVertices[0] = 0;
   l_PosLines.Line(0).m_pVertices[1] = 1;
   l_PosLines.Line(1).m_pVertices[0] = 0;
   l_PosLines.Line(1).m_pVertices[1] = 2;
   l_PosLines.Line(2).m_pVertices[0] = 0;
   l_PosLines.Line(2).m_pVertices[1] = 3;

   l_SubMesh->Facet_List(0).Colors().Type(FACETS_LINE, true);
   GLineListIndexed& l_ColLines = l_SubMesh->Facet_List(0).Colors().LineListIndexed();

   l_ColLines.NbLines(3);
   l_ColLines.Line(0).m_pVertices[0] = 0;
   l_ColLines.Line(0).m_pVertices[1] = 0;
   l_ColLines.Line(1).m_pVertices[0] = 1;
   l_ColLines.Line(1).m_pVertices[1] = 1;
   l_ColLines.Line(2).m_pVertices[0] = 2;
   l_ColLines.Line(2).m_pVertices[1] = 2;

   // Object creation for clicked triangle information
   CGFX_Model_Itf* l_ClickModel = g_Joshua.Renderer()->Create_Model();
   m_ClickText = g_Joshua.Renderer()->Create_Text_2D();
   CGFX_Transform_2D_Itf* l_ClickTransform = g_Joshua.Renderer()->Create_Transform_2D();

   // Model setup for clicked triangle information
   m_SubScene->Get_Root_Model()->Add_Child(l_ClickModel);
   l_ClickModel->Set_Transform(l_ClickTransform);
   l_ClickModel->Set_Primitive(m_ClickText);

   // Transform setup for clicked triangle information
   l_ClickTransform->Position().X = 10;
   l_ClickTransform->Position().Y = 50;

   l_ClickTransform->Size().X = 390;
   l_ClickTransform->Size().Y = 550;

   // Text setup for clicked triangle information
   m_ClickText->Init(L"Courier New", 12, true);
   m_ClickText->Text(L"Selected Triangle Info:");
   m_ClickText->Color() = GColorRGBReal(1, 1, 1, 0.5);

   // Setup sub-scene rendering
   m_SubRList = m_SubScene->Create_Render_List();
   g_Joshua.Renderer()->RenderTree()->Add_Render_List(m_SubRList);

   // Delete temporary objects
   SAFE_RELEASE(l_SubCamera);
   SAFE_RELEASE(l_SubModel);
   SAFE_RELEASE(l_SubMesh);
   SAFE_RELEASE(l_SubMaterial);
 
   // Get the close button ptr
//   m_CloseButton = CREATE_GUI_OBJECT_NAMED_VISIBLE(GButton,"MAIN_MENU",this);

   ///////// Old coordinates
   // France-Spain meditarean boundary
//   DisplayArea(GVector2D<REAL32>(3.f, 43.f), GVector2D<REAL32>(2.f, 2.f) );
   // Haiti
//   DisplayArea(GVector2D<REAL32>(-73.75f, 18.75f), GVector2D<REAL32>(2.5f, 2.5f) );
   // Hawaii
//   DisplayArea(GVector2D<REAL32>(-155.5f, 19.5f), GVector2D<REAL32>(1.f, 1.f) );
   // Saguenay
//   DisplayArea(GVector2D<REAL32>(-71.5f, 48.25f), GVector2D<REAL32>(1.f, 1.f) );
   // Yukon-BC-Alaska
//   DisplayArea(GVector2D<REAL32>(-137.5f, 58.5f), GVector2D<REAL32>(1.f, 1.f) );
   /////////

   // Corse
//   DisplayArea(GVector2D<REAL32>(9, 42), GVector2D<REAL32>(200000, 200000) );
   // 1/16th of planet
//   DisplayArea(GVector2D<REAL32>(0, 45), GVector2D<REAL32>(EARTH_RADIUS * 2 * Math::PI / 16, EARTH_RADIUS * 2 * Math::PI / 16) );
   // Haiti
//   DisplayArea(GVector2D<REAL32>(-72.85f, 18.90f), GVector2D<REAL32>(250000, 250000) );
   // Montreal
//   DisplayArea(GVector2D<REAL32>(-73.70f, 45.45f), GVector2D<REAL32>(75000, 75000) );
   // Saguenay
//   DisplayArea(GVector2D<REAL32>(-70.5f, 48.25f), GVector2D<REAL32>(100000, 100000) );
   // Niagara
//   DisplayArea(GVector2D<REAL32>(-79.f, 43.f), GVector2D<REAL32>(100000, 100000) );
//    Yemen  
//   DisplayArea(GVector2D<REAL32>(44.0f, 15.75f), GVector2D<REAL32>(250000, 100000) );

   return true;
}

/*!
* Destroy the Background Earth, 
*     releases all the GUI & GFX objects that must be
*     released
**/
bool GDetailedEarthArea::OnDestroy()
{
   g_Joshua.Renderer()->RenderTree()->Remove_Render_List(m_SkyRList);
   g_Joshua.Renderer()->RenderTree()->Remove_Render_List(m_RList);

   SAFE_RELEASE(m_SkyCamera);
   SAFE_RELEASE(m_SkyRList);

   SAFE_RELEASE(m_Scene);

   SAFE_RELEASE(m_CameraModel);
   SAFE_RELEASE(m_CameraTransform);
   SAFE_RELEASE(m_Camera);

   SAFE_RELEASE(m_SunModel);
   SAFE_RELEASE(m_SunTransform);
   SAFE_RELEASE(m_SunLight);

   SAFE_RELEASE(m_LandModel);
   SAFE_RELEASE(m_LandMesh);
   SAFE_RELEASE(m_LandMaterial);
   SAFE_RELEASE(m_LandTexture);

   SAFE_RELEASE(m_OutlineModel);
   SAFE_RELEASE(m_OutlineMesh);

   SAFE_RELEASE(m_GizmoModel);
   SAFE_RELEASE(m_GizmoTransform);
   SAFE_RELEASE(m_GizmoMesh);

   SAFE_RELEASE(m_RList);

   g_Joshua.Renderer()->RenderTree()->Remove_Render_List(m_SubRList);

   SAFE_RELEASE(m_SubScene);
   SAFE_RELEASE(m_SubTransform);
   SAFE_RELEASE(m_SubRList);

   // Releases the Model & our children
   GUI::GBaseObject::OnDestroy();

   return true;
}

/*!
 * Called when a key is pressed & the Detailed Earth Area has focus
 */
GUI::EEventStatus::Enum GDetailedEarthArea::OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* callerPtr)
{
   switch(in_EventData.Key())
   {
   case VK_UP:
   case 'W':
      {
         // Move camera forward
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = l_Look - l_Pos;
         l_PosInc.Normalize();
         l_PosInc *= m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case VK_DOWN:
   case 'S':
      {
         // Move camera backward
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = l_Pos - l_Look;
         l_PosInc.Normalize();
         l_PosInc *= m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case VK_LEFT:
      {
         // Rotate camera left
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();

         REAL32 l_ThetaInc = m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = l_CosT;
         l_TurnTrans(0,1) = 0;
         l_TurnTrans(0,2) = l_SinT;

         l_TurnTrans(1,0) = 0;
         l_TurnTrans(1,1) = 1;
         l_TurnTrans(1,2) = 0;

         l_TurnTrans(2,0) = -l_SinT;
         l_TurnTrans(2,1) = 0;
         l_TurnTrans(2,2) = l_CosT;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_DirLook.Set(l_CamTrans(2,0), l_CamTrans(2,1), l_CamTrans(2,2) );

         l_Look = l_DirLook + l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_DirLook;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case VK_RIGHT:
      {
         // Rotate camera right
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();

         REAL32 l_ThetaInc = -m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = l_CosT;
         l_TurnTrans(0,1) = 0;
         l_TurnTrans(0,2) = l_SinT;

         l_TurnTrans(1,0) = 0;
         l_TurnTrans(1,1) = 1;
         l_TurnTrans(1,2) = 0;

         l_TurnTrans(2,0) = -l_SinT;
         l_TurnTrans(2,1) = 0;
         l_TurnTrans(2,2) = l_CosT;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_DirLook.Set(l_CamTrans(2,0), l_CamTrans(2,1), l_CamTrans(2,2) );

         l_Look = l_DirLook + l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_DirLook;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case 'A':
      {
         // Strafe camera left
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = l_Look - l_Pos;
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         l_PosInc = l_PosInc.CrossProduct(l_Up);
         l_PosInc.Normalize();
         l_PosInc *= m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case 'D':
      {
         // Strafe camera right
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = l_Look - l_Pos;
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         l_PosInc = l_PosInc.CrossProduct(l_Up) * -1;
         l_PosInc.Normalize();
         l_PosInc *= m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case 'X':
      {
         // Move camera up
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = m_Camera->Up_Vector();
         l_PosInc *= m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case 'Z':
      {
         // Move camera down
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();

         GVector3D<REAL32> l_PosInc = m_Camera->Up_Vector();
         l_PosInc *= -m_FreeCamSpeed;

         l_Pos += l_PosInc;
         l_Look += l_PosInc;

         m_CameraTransform->Translation() = l_Pos;
         m_Camera->Look_At() = l_Look;
         m_SkyCamera->Look_At() = l_Look - l_Pos;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case VK_PRIOR:
      {
         // Rotate camera up
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();
         l_Up.Normalize();

         REAL32 l_ThetaInc = m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = 1;
         l_TurnTrans(0,1) = 0;
         l_TurnTrans(0,2) = 0;

         l_TurnTrans(1,0) = 0;
         l_TurnTrans(1,1) = l_CosT;
         l_TurnTrans(1,2) = -l_SinT;

         l_TurnTrans(2,0) = 0;
         l_TurnTrans(2,1) = l_SinT;
         l_TurnTrans(2,2) = l_CosT;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_DirLook.Set(l_CamTrans(2,0), l_CamTrans(2,1), l_CamTrans(2,2) );
         l_Up.Set(l_CamTrans(1,0), l_CamTrans(1,1), l_CamTrans(1,2) );

         l_Look = l_DirLook + l_Pos;
         m_Camera->Look_At() = l_Look;
         m_Camera->Up_Vector() = l_Up;
         m_SkyCamera->Look_At() = l_DirLook;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case VK_NEXT:
      {
         // Rotate camera down
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();
         l_Up.Normalize();

         REAL32 l_ThetaInc = -m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = 1;
         l_TurnTrans(0,1) = 0;
         l_TurnTrans(0,2) = 0;

         l_TurnTrans(1,0) = 0;
         l_TurnTrans(1,1) = l_CosT;
         l_TurnTrans(1,2) = -l_SinT;

         l_TurnTrans(2,0) = 0;
         l_TurnTrans(2,1) = l_SinT;
         l_TurnTrans(2,2) = l_CosT;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_DirLook.Set(l_CamTrans(2,0), l_CamTrans(2,1), l_CamTrans(2,2) );
         l_Up.Set(l_CamTrans(1,0), l_CamTrans(1,1), l_CamTrans(1,2) );

         l_Look = l_DirLook + l_Pos;
         m_Camera->Look_At() = l_Look;
         m_Camera->Up_Vector() = l_Up;
         m_SkyCamera->Look_At() = l_DirLook;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case 'Q':
      {
         // Roll camera left
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();

         REAL32 l_ThetaInc = -m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = l_CosT;
         l_TurnTrans(0,1) = -l_SinT;
         l_TurnTrans(0,2) = 0;

         l_TurnTrans(1,0) = l_SinT;
         l_TurnTrans(1,1) = l_CosT;
         l_TurnTrans(1,2) = 0;

         l_TurnTrans(2,0) = 0;
         l_TurnTrans(2,1) = 0;
         l_TurnTrans(2,2) = 1;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_Up.Set(l_CamTrans(1,0), l_CamTrans(1,1), l_CamTrans(1,2) );

         m_Camera->Up_Vector() = l_Up;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case 'E':
      {
         // Roll camera right
         GVector3D<REAL32> l_Pos = m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         GVector3D<REAL32> l_Up = m_Camera->Up_Vector();
         GVector3D<REAL32> l_DirLook = (l_Look - l_Pos);
         l_DirLook.Normalize();

         REAL32 l_ThetaInc = m_FreeCamSpeed * (REAL32)Math::PI;
         REAL32 l_CosT = cosf(l_ThetaInc);
         REAL32 l_SinT = sinf(l_ThetaInc);

         GMatrix4x4<REAL32> l_CamTrans;
         l_CamTrans.LoadIdentity();
         l_CamTrans(2,0) = l_DirLook.x;
         l_CamTrans(2,1) = l_DirLook.y;
         l_CamTrans(2,2) = l_DirLook.z;

         l_CamTrans(1,0) = l_Up.x;
         l_CamTrans(1,1) = l_Up.y;
         l_CamTrans(1,2) = l_Up.z;

         GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_DirLook);

         l_CamTrans(0,0) = l_Right.x;
         l_CamTrans(0,1) = l_Right.y;
         l_CamTrans(0,2) = l_Right.z;

         GMatrix4x4<REAL32> l_TurnTrans;
         l_TurnTrans.LoadIdentity();
         l_TurnTrans(0,0) = l_CosT;
         l_TurnTrans(0,1) = -l_SinT;
         l_TurnTrans(0,2) = 0;

         l_TurnTrans(1,0) = l_SinT;
         l_TurnTrans(1,1) = l_CosT;
         l_TurnTrans(1,2) = 0;

         l_TurnTrans(2,0) = 0;
         l_TurnTrans(2,1) = 0;
         l_TurnTrans(2,2) = 1;

         l_CamTrans = l_TurnTrans * l_CamTrans;

         l_Up.Set(l_CamTrans(1,0), l_CamTrans(1,1), l_CamTrans(1,2) );

         m_Camera->Up_Vector() = l_Up;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();

         l_CamTrans = l_CamTrans.Transpose();
         GMatrix4x4<REAL32> l_Gizmo = m_SubTransform->Get_Matrix();
         l_CamTrans(0,1) = -l_CamTrans(0,1);
         l_CamTrans(1,1) = -l_CamTrans(1,1);
         l_CamTrans(2,1) = -l_CamTrans(2,1);
         l_CamTrans(3,0) = l_Gizmo(3,0);
         l_CamTrans(3,1) = l_Gizmo(3,1);
         l_CamTrans(3,2) = l_Gizmo(3,2);

         m_SubTransform->Set_Matrix(l_CamTrans);
      }
      break;
   case VK_NUMPAD8:
      {
         const GShape& l_Shape = m_LandMesh->Shape();

         // Get the current camera position
         GVector2D<REAL32> l_Pos = (GVector3D<REAL32>) m_CameraTransform->Translation();
         GVector3D<REAL32> l_Look = m_Camera->Look_At();
         l_Look -= l_Pos;

         // Advance the camera in the current direction
         l_Pos += l_Look * m_LandCamSpeed;
         GVector3D<REAL32> l_CamPos(l_Pos);
         GVector3D<REAL32> l_Normal(0,0,-1);

         if(m_TriFinder)
         {
            // Fetch the triangle that is beneath the camera
            UINT32 l_Tri;
            if(m_TriFinder->Locate(l_Pos, l_Tri) )
            {
               // Extract the triangle corners
               const GVector3D<REAL32>& l_A = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[0] );
               const GVector3D<REAL32>& l_B = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[1] );
               const GVector3D<REAL32>& l_C = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[2] );

               // Compute the triangle plane normal
               l_Normal = (l_B - l_A).CrossProduct(l_C - l_A);
               l_Normal.Normalize();

               // Compute the triangle plane distance
               REAL32 l_fPlaneDist = l_Normal.DotProduct(l_A);

               // Compute the heigth of the triangle at the camera position using intersection of triangle plane & vertical ray
               GVector3D<REAL32> l_Ray(0,0,1);
               REAL32 t = (l_fPlaneDist - l_Normal.DotProduct(l_Pos) ) / l_Normal.DotProduct(l_Ray);

               l_CamPos += l_Ray * t;
            }
         }

         // Offset camera at 1 meter over ground
         l_CamPos.z -= m_fUnitToMeterRatio * 10;
         m_CameraTransform->Translation() = l_CamPos;

         // Set look vector to be parallel with ground (triangle plane)
         REAL32 l_fProj = l_Look.DotProduct(l_Normal);
         l_Look -= (l_Normal * l_fProj);
         l_Look.Normalize();
         m_Camera->Look_At() = l_Look + l_CamPos;

         // Set up vector to be world
         GVector3D<REAL32> l_Up(0,0,-1);
         l_Up -= l_Look * -l_Look.z;
         m_Camera->Up_Vector() = l_Up;
         m_SkyCamera->Look_At() = l_Look;
         m_SkyCamera->Up_Vector() = m_Camera->Up_Vector();
      }
      break;
   case 'F':
      if(m_LandMaterial->RenderStates(0)->Value(GFX::RSTATE_FILL_MODE) != GFX::FILL_WIREFRAME)
      {
         m_LandMaterial->RenderStates(0)->Value(GFX::RSTATE_FILL_MODE) = GFX::FILL_WIREFRAME;
      }
      else
      {
         m_LandMaterial->RenderStates(0)->Value(GFX::RSTATE_FILL_MODE) = GFX::FILL_SOLID;
      }
      break;
   case VK_ESCAPE:
/*      g_SP2Client->SwitchToBackgroundEarth();*/
      break;
   }
   return GUI::EEventStatus::Handled;
}

/*!
* Mouse down handler
**/
GUI::EEventStatus::Enum GDetailedEarthArea::on_mouse_down(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* in_pCaller)
{
      
/*
   //If it is not a callback
   if(!in_pCaller)
   {

      //Get the coordinates of the click
      GVector3D<REAL32> l_Destination;
      MouseToCoordinate(GVector2D<UINT32>(x, y), l_Destination);
      //Translate into latitude longitude
      ConvertRectangularCoordinate(l_Destination);


      //Find where the Model actually is
      GVector3D<REAL32> l_UnitPos(0,0,0);
      if(m_pUnitPathModel)
      {
         GMatrix4x4<REAL32> l_UnitTransMatrix = m_pUnitPathModel->Get_Child(0)->Get_Transform()->Get_Matrix();
         l_UnitPos.x = l_UnitTransMatrix(3,0);
         l_UnitPos.y = l_UnitTransMatrix(3,1);
         l_UnitPos.z = l_UnitTransMatrix(3,2);
      }

      //Translate into latitude longitude
      ConvertRectangularCoordinate(l_UnitPos);

      GFullPath l_UnitPath = g_ClientDCL.CalculatePath(l_UnitPos,l_Destination);

      if(l_UnitPath.size())
      {
         l_UnitPath[0].second.x = l_UnitPos.x;
         l_UnitPath[0].second.y = l_UnitPos.y;
   
         l_UnitPath[l_UnitPath.size()-1].second.x = l_Destination.x;
         l_UnitPath[l_UnitPath.size()-1].second.y = l_Destination.y;
      }



      CreateUnitAnimation(l_UnitPath);




      
      // Reset old triangle colors
      for(UINT32 t = 0;t < m_ClickedTris.size();t ++)
      {
         GTriangleIndex l_NewTri;
         UINT32 l_iColorIndex = m_ClickedTrisColors[t];
         l_NewTri.m_pVertices[0] = l_iColorIndex;
         l_NewTri.m_pVertices[1] = l_iColorIndex;
         l_NewTri.m_pVertices[2] = l_iColorIndex;

         m_LandMesh->Facet_List(0).Colors().TriangleListIndexed().Triangle(m_ClickedTris[t] ) = l_NewTri;
      }

      GString l_TextInfo(L"Selected Triangle Info:\n");

      GVector3D<REAL32> l_Location;
      if( (m_TriFinder) && MouseToCoordinate(GVector2D<UINT32>(x, y), l_Location) )
      {
         m_ClickedTris.resize(0);
         m_TriFinder->Locate(l_Location, m_ClickedTris);

         m_ClickedTrisColors.resize(m_ClickedTris.size() );
         for(UINT32 t = 0;t < m_ClickedTris.size();t ++)
         {
            m_ClickedTrisColors[t] = m_LandMesh->Facet_List(0).Colors().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[0];

            GTriangleIndex l_NewTri;
            l_NewTri.m_pVertices[0] = TERRAIN_SELECTED;
            l_NewTri.m_pVertices[1] = TERRAIN_SELECTED;
            l_NewTri.m_pVertices[2] = TERRAIN_SELECTED;

            m_LandMesh->Facet_List(0).Colors().TriangleListIndexed().Triangle(m_ClickedTris[t] ) = l_NewTri;

            GVector3D<REAL32>& l_A = m_LandMesh->Shape().Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[0]);
            GVector3D<REAL32>& l_B = m_LandMesh->Shape().Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[1]);
            GVector3D<REAL32>& l_C = m_LandMesh->Shape().Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[2]);

            GVector3D<REAL32>& l_ANormal = m_LandMesh->Shape().Normal(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[0]);
            GVector3D<REAL32>& l_BNormal = m_LandMesh->Shape().Normal(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[1]);
            GVector3D<REAL32>& l_CNormal = m_LandMesh->Shape().Normal(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(m_ClickedTris[t] ).m_pVertices[2]);

            WCHAR l_sTempStr[512];
            swprintf(l_sTempStr, L"%5d : %2.4f , %2.4f , %2.4f\n        %2.4f , %2.4f , %2.4f\n        %2.4f , %2.4f , %2.4f\n        %2.4f , %2.4f , %2.4f\n        %2.4f , %2.4f , %2.4f\n        %2.4f , %2.4f , %2.4f\n",
                     m_ClickedTris[t],
                     l_A.x, l_A.y, l_A.z,
                     l_B.x, l_B.y, l_B.z,
                     l_C.x, l_C.y, l_C.z,
                     l_ANormal.x, l_ANormal.y, l_ANormal.z,
                     l_BNormal.x, l_BNormal.y, l_BNormal.z,
                     l_CNormal.x, l_CNormal.y, l_CNormal.z);
            l_TextInfo += l_sTempStr;
         }
      }

      m_ClickText->Text(l_TextInfo);
      
   }
   //------------------------------------------------
   //             Context menu handle
   //------------------------------------------------

   //Return to the globe view
   else if(in_pCaller->Name() == (SP2::CONTEXT_MENU_ITEM + GString(1)))
   {
      g_ClientDDL.ContextMenuKill();
      g_ClientDCL.DetailedAreaBackgroundEarthToggle();


   }
   //----------------- end context menu handle --------------
*/
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GDetailedEarthArea::on_mouse_move(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr)
{
   return __super::on_mouse_move(x, y, z, buttons, callerPtr);
}

GUI::EEventStatus::Enum GDetailedEarthArea::on_mouse_up(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr)
{
   return __super::on_mouse_up(x, y, z, buttons, callerPtr);

}

GUI::EEventStatus::Enum GDetailedEarthArea::on_mouse_wheel(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr)
{
   return __super::on_mouse_wheel(x, y, z, buttons, callerPtr);
}

GUI::EEventStatus::Enum GDetailedEarthArea::on_click(INT16 x, INT16 y, INT16 z, INT16 in_iButtons, GUI::GBaseObject* callerPtr)
{
   //If right mouse click
   if( ((INT8*)&in_iButtons)[1] == MOUSE_RIGHT)
   {/*
      g_ClientDDL.ContextMenuSpawn();
      g_ClientDDL.ContextMenu()->Owner(this);
      g_ClientDDL.ContextMenu()->AddItem(L"-- to Globe view --");
      return GUI::EEventStatus::Handled;*/
   }

   return __super::on_click(x, y, z, in_iButtons, callerPtr);
}

bool GDetailedEarthArea::DisplayArea(const GVector2D<REAL32>& in_Center, const GVector2D<REAL32>& in_Size)
{
   SAFE_DELETE(m_TriFinder);

   // Convert coordinates from degrees to radians
   REAL32 l_fCenterXRad = in_Center.x * (REAL32)Math::PI / 180.0f;
   REAL32 l_fCenterYRad = in_Center.y * (REAL32)Math::PI / 180.0f;

   // Calculate maximum longitude & latitude deltas for requested detailed view
   REAL32 l_fLatitudeDeltaRad = in_Size.y / EARTH_RADIUS;
   REAL32 l_fLongitudeDeltaNorthRad = in_Size.x / (EARTH_RADIUS * cosf(l_fCenterYRad + l_fLatitudeDeltaRad * 0.5f) );
   REAL32 l_fLongitudeDeltaSouthRad = in_Size.x / (EARTH_RADIUS * cosf(l_fCenterYRad - l_fLatitudeDeltaRad * 0.5f) );

   // Convert deltas from radians to degrees
   REAL32 l_fLatitudeDelta = l_fLatitudeDeltaRad * 180.0f / (REAL32)Math::PI;
   REAL32 l_fLongitudeDeltaNorth = l_fLongitudeDeltaNorthRad * 180.0f / (REAL32)Math::PI;
   REAL32 l_fLongitudeDeltaSouth = l_fLongitudeDeltaSouthRad * 180.0f / (REAL32)Math::PI;
   REAL32 l_fLongitudeDeltaMax = max(l_fLongitudeDeltaNorth, l_fLongitudeDeltaSouth);


   // Create outlines
   GShape& l_OutlineShape = m_OutlineMesh->Shape();

   GFaceList<GPolygon> l_Coast;
   GFaceList<GPolygon> l_InnerCoast;

   g_ClientDAL.MapEngineCoastlinesClipOuterLimitsRectangle(in_Center.x, 
                                                           in_Center.y, 
                                                           l_fLongitudeDeltaMax * 1.01f, 
                                                           l_fLatitudeDelta * 1.01f, 
                                                           l_OutlineShape, 
                                                           l_Coast, 
                                                           l_InnerCoast);


   // Load a view that contains the necessary data in each direction
   Map::GContinuousRasterView* l_ElevationView = g_ClientDAL.MapEngineElevationView(in_Center.x, 
                                                                                    in_Center.y, 
                                                                                    l_fLongitudeDeltaMax, 
                                                                                    l_fLatitudeDelta);

   // Fetch water data
   GFaceList<GPolygon> l_Water, l_Island;
   GShape& l_WaterShape = m_WaterMesh->Shape();
   g_ClientDAL.MapEngineWaterData(in_Center.x, 
                                  in_Center.y, 
                                  l_fLongitudeDeltaMax * 1.01f, 
                                  l_fLatitudeDelta * 1.01f,
                                  l_WaterShape, 
                                  l_Water, 
                                  l_Island);


   // Clear current mesh
   m_LandMesh->NbFacetLists(0);
   GShape& l_LandShape = m_LandMesh->Shape();
   l_LandShape.ClearAll();

   // Create land mesh in latitude/longitude coordinates
   UINT32 l_NbAlignedPoints = (m_NbDivs + 1) * (m_NbDivs + 1);

   l_LandShape.NbPositions(l_NbAlignedPoints);
   UINT32 l_PosID = 0;
   for(UINT32 y = 0;y <= m_NbDivs;y ++)
   {
      REAL32 l_fSlope = (REAL32) y / ( (REAL32) (m_NbDivs) );
      REAL32 l_fPhi = (in_Center.y + l_fLatitudeDelta / 2) - (l_fLatitudeDelta * l_fSlope);
      REAL32 l_fThetaDelta = (1 - l_fSlope) * l_fLongitudeDeltaNorth + l_fSlope * l_fLongitudeDeltaSouth;

      for(UINT32 x = 0;x <= m_NbDivs;x ++)
      {
         REAL32 l_fTheta = (in_Center.x - l_fThetaDelta / 2) + l_fThetaDelta * (REAL32) x / ( (REAL32) (m_NbDivs) );

         GVector3D<REAL32> l_Pos(l_fTheta,
                                 l_fPhi,
                                 0);
         l_LandShape.Position(l_PosID) = l_Pos;
         l_PosID ++;
      }
   }

   // Create all triangles
   m_LandMesh->NbFacetLists(1);
   GTriangleListIndexed& l_LandTris = m_LandMesh->Facet_List(0).Positions().TriangleListIndexed();
   {
      l_LandTris.NbTriangles(2 * m_NbDivs * m_NbDivs);
      UINT32 l_TriID = 0;
      for(UINT32 y = 0;y < m_NbDivs;y ++)
      {
         for(UINT32 x = 0;x < m_NbDivs;x ++)
         {
            GTriangleIndex* l_Tri = &l_LandTris.Triangle(l_TriID);

            l_Tri[0].m_pVertices[0] = y * (m_NbDivs + 1) + x;
            l_Tri[0].m_pVertices[2] = (y + 1) * (m_NbDivs + 1) + x;
            l_Tri[0].m_pVertices[1] = y * (m_NbDivs + 1) + x + 1;

            l_Tri[1].m_pVertices[0] = y * (m_NbDivs + 1) + x + 1;
            l_Tri[1].m_pVertices[2] = (y + 1) * (m_NbDivs + 1) + x;
            l_Tri[1].m_pVertices[1] = (y + 1) * (m_NbDivs + 1) + x + 1;

            l_TriID += 2;
         }
      }

      l_LandTris.NbTriangles(l_TriID);
   }

   // Merge all contours
//   UINT32 l_iIslandIndex = 0;
   GFaceList<GPolygon> l_Contours = l_Island;

   UINT32 l_iRiverIndex = l_Contours.NbFaces();
   l_Contours.Append(l_Water);

   UINT32 l_iWaterIndex = l_Contours.NbFaces();
   l_Contours.Append(l_InnerCoast);

   UINT32 l_iLandIndex = l_Contours.NbFaces();
   l_Contours.Append(l_Coast);

   // Merge contour shapes
   UINT32 l_iWaterOffset = l_OutlineShape.NbPositions();
   l_OutlineShape.NbPositions(l_iWaterOffset + l_WaterShape.NbPositions() );
   for(UINT32 p = 0;p < l_WaterShape.NbPositions();p ++)
   {
      l_OutlineShape.Position(p + l_iWaterOffset) = l_WaterShape.Position(p);
   }

   // Update water faces
   for(UINT32 f = 0;f < l_iWaterIndex;f ++)
   {
      l_Contours.Face(f).Outer().Start() += l_iWaterOffset;
      for(UINT32 i = 0;i < l_Contours.Face(f).NbInners();i ++)
      {
         l_Contours.Face(f).Inner(i).Start() += l_iWaterOffset;
      }
   }

   // Split triangles
   vector<UINT32> l_LandResults;
   g_Joshua.Renderer()->Helper()->SplitTriangleList(
      l_LandShape, 
      l_LandTris, 
      l_OutlineShape, 
//      l_Coast, 
      l_Contours,
      l_LandShape, 
      l_LandTris, 
      l_LandResults);

   // Create land colors
   l_LandShape.NbColors(TERRAin_iCount);
   l_LandShape.Color(TERRAIN_WATER) = GColorRGBInt(0,0,200,255);
   l_LandShape.Color(TERRAIN_LAND) = GColorRGBInt(200,150,50,255);
   l_LandShape.Color(TERRAIN_RIVER) = GColorRGBInt(128,128,255,255);
   l_LandShape.Color(TERRAIN_SELECTED) = GColorRGBInt(255,0,0,255);

   m_LandMesh->Facet_List(0).Colors().TriangleListIndexed().NbTriangles(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().NbTriangles() );
   for(UINT32 t = 0;t < l_LandResults.size();t ++)
   {
      GTriangleIndex l_NewTri;
      if(l_LandResults[t] == 0xFFFFFFFF)
      {
         l_LandResults[t] = TERRAIN_WATER;
         l_NewTri.m_pVertices[0] = TERRAIN_WATER;
         l_NewTri.m_pVertices[1] = TERRAIN_WATER;
         l_NewTri.m_pVertices[2] = TERRAIN_WATER;
      }
      else if(l_LandResults[t] >= l_iLandIndex)
      {
         l_LandResults[t] = TERRAIN_LAND;
         l_NewTri.m_pVertices[0] = TERRAIN_LAND;
         l_NewTri.m_pVertices[1] = TERRAIN_LAND;
         l_NewTri.m_pVertices[2] = TERRAIN_LAND;
      }
      else if(l_LandResults[t] >= l_iWaterIndex)
      {
         l_LandResults[t] = TERRAIN_WATER;
         l_NewTri.m_pVertices[0] = TERRAIN_WATER;
         l_NewTri.m_pVertices[1] = TERRAIN_WATER;
         l_NewTri.m_pVertices[2] = TERRAIN_WATER;
      }
      else if(l_LandResults[t] >= l_iRiverIndex)
      {
         l_LandResults[t] = TERRAIN_RIVER;
         l_NewTri.m_pVertices[0] = TERRAIN_RIVER;
         l_NewTri.m_pVertices[1] = TERRAIN_RIVER;
         l_NewTri.m_pVertices[2] = TERRAIN_RIVER;
      }
      else
      {
         l_LandResults[t] = TERRAIN_LAND;
         l_NewTri.m_pVertices[0] = TERRAIN_LAND;
         l_NewTri.m_pVertices[1] = TERRAIN_LAND;
         l_NewTri.m_pVertices[2] = TERRAIN_LAND;
      }
      m_LandMesh->Facet_List(0).Colors().TriangleListIndexed().Triangle(t) = l_NewTri;
   }

   // Create a rotation matrix that centers the spherical mesh to face direction {0,0,-1}
   m_RotMatrix.LoadIdentity();
   GVector3D<REAL32> l_Out(-cosf(l_fCenterYRad) * sinf(l_fCenterXRad),
                           -sinf(l_fCenterYRad),
                           -cosf(l_fCenterYRad) * -cosf(l_fCenterXRad) );

   m_RotMatrix(0,2) = l_Out.x;
   m_RotMatrix(1,2) = l_Out.y;
   m_RotMatrix(2,2) = l_Out.z;

   GVector3D<REAL32> l_UpWorld(0, 1, 0);
   GVector3D<REAL32> l_Up = l_UpWorld - (l_Out * l_UpWorld.DotProduct(l_Out) );
   l_Up.Normalize();

   m_RotMatrix(0,1) = l_Up.x;
   m_RotMatrix(1,1) = l_Up.y;
   m_RotMatrix(2,1) = l_Up.z;

   GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_Out);

   m_RotMatrix(0,0) = l_Right.x;
   m_RotMatrix(1,0) = l_Right.y;
   m_RotMatrix(2,0) = l_Right.z;

   // Extract corners
   GVector3D<REAL32> l_Corners[6];

   // Min X, Min Y
   l_Corners[0].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
   l_Corners[0].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
   l_Corners[0].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);

   // Center X, Min Y
   l_Corners[1].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad);
   l_Corners[1].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
   l_Corners[1].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad);

   // Max X, Min Y
   l_Corners[2].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
   l_Corners[2].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
   l_Corners[2].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);

   // Min X, Max Y
   l_Corners[3].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
   l_Corners[3].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
   l_Corners[3].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);

   // Center X, Max Y
   l_Corners[4].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad);
   l_Corners[4].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
   l_Corners[4].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad);

   // Max X, Max Y
   l_Corners[5].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
   l_Corners[5].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
   l_Corners[5].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);

   // Transform corners to face camera
   for(UINT32 c = 0;c < 6;c ++)
   {
      l_Corners[c] =  m_RotMatrix * l_Corners[c];
   }

   // Fetch minimum & maximum coordinates
   REAL32 l_fXMin = l_Corners[0].x;
   REAL32 l_fXMax = l_Corners[0].x;
   REAL32 l_fYMin = l_Corners[0].y;
   REAL32 l_fYMax = l_Corners[0].y;

   for(UINT32 c = 1;c < 6;c ++)
   {
      l_fXMin = min(l_fXMin, l_Corners[c].x);
      l_fXMax = max(l_fXMax, l_Corners[c].x);
      l_fYMin = min(l_fYMin, l_Corners[c].y);
      l_fYMax = max(l_fYMax, l_Corners[c].y);
   }

   // Calc a ratio to make the largest increment to fit in one screen
   REAL32 l_fXDelta = l_fXMax - l_fXMin;
   REAL32 l_fYDelta = l_fYMax - l_fYMin;

   m_fRadToUnitRatio = min(1 / l_fXDelta, 1 / l_fYDelta);

   // Fill positions with transformed aligned points
   m_fUnitToMeterRatio = 4 / max(in_Size.x, in_Size.y);

   // Fetch aligned height data
   REAL32* l_HBuffer = new REAL32[l_NbAlignedPoints];
   l_ElevationView->FillTrapezoidalGrid(in_Center.x, in_Center.y, l_fLongitudeDeltaNorth, l_fLongitudeDeltaSouth, l_fLatitudeDelta, (m_NbDivs + 1), (m_NbDivs + 1), l_HBuffer);

   // Convert land mesh from latitude/longitude coordinates to local world coordinates with heigth
   l_PosID = 0;
   for(UINT32 y = 0;y <= m_NbDivs;y ++)
   {
      for(UINT32 x = 0;x <= m_NbDivs;x ++)
      {
         GVector3D<REAL32>& l_Pos = l_LandShape.Position(l_PosID);
         l_Pos.z = l_HBuffer[l_PosID];

         ConvertSphericalCoordinate(l_Pos);

         l_PosID ++;
      }
   }

   // Convert remaining land mesh positions from latitude/longitude coordinates to local world coordinates
   for(;l_PosID < l_LandShape.NbPositions();l_PosID ++)
   {
      GVector3D<REAL32>& l_Pos = l_LandShape.Position(l_PosID);
      l_Pos.z = l_ElevationView->Value(l_Pos.x, l_Pos.y);

      ConvertSphericalCoordinate(l_Pos);
   }

   g_ClientDAL.MapEngineElevationCloseView(l_ElevationView);

   // Create per vertex (smooth) normals
   l_LandShape.NbNormals(l_LandShape.NbPositions() + 1);

   // Clear all normals
   for(UINT32 i = 0;i < l_LandShape.NbNormals() - 1;i ++)
   {
      l_LandShape.Normal(i) = GVector3D<REAL32>(0,0,0);
   }

   // Set all water height to 0 & normals to face up
//   UINT32 l_iNormalID = l_LandShape.NbNormals() - 1;
//   l_LandShape.Normal(l_iNormalID) = GVector3D<REAL32>(0,0,-1);
//   GTriangleListIndexed& l_WaterTris = m_LandMesh->Facet_List(0).Positions().TriangleListIndexed();
//   GTriangleListIndexed& l_WaterNormalTris = m_LandMesh->Facet_List(0).Normals().TriangleListIndexed();
//   l_WaterNormalTris.NbIndexes(l_WaterTris.NbIndexes() );
//   for(UINT32 i = 0;i < l_WaterTris.NbIndexes();i ++)
//   {
//      l_LandShape.Position(l_WaterTris.Index(i) ).z = 0;
////      l_WaterNormalTris.Index(i) = l_iNormalID;
//   }

   // Set all water height to 0
   GTriangleListIndexed& l_WaterTris = m_LandMesh->Facet_List(0).Positions().TriangleListIndexed();

   for(UINT32 t = 0;t < l_WaterTris.NbTriangles();t ++)
   {
      if(l_LandResults[t] == TERRAIN_WATER)
      {
         for(UINT32 v = 0;v < 3;v ++)
         {
            l_LandShape.Position(l_WaterTris.Triangle(t).m_pVertices[v] ).z = 0;
         }
      }
   }

   for(UINT32 f = 0;f < m_LandMesh->NbFacetLists();f ++)
   {
      GTriangleListIndexed& l_Tris = m_LandMesh->Facet_List(f).Positions().TriangleListIndexed();

      // Sum all face normals of each point
      for(UINT32 t = 0;t < l_Tris.NbTriangles();t ++)
      {
//         if(f != 0)
         {
            // Fetch face vertices
            GVector3D<REAL32> l_A, l_B, l_C;
            GTriangleIndex& l_Face = l_Tris.Triangle(t);
            l_A = l_LandShape.Position(l_Face.m_pVertices[0]);
            l_B = l_LandShape.Position(l_Face.m_pVertices[1]);
            l_C = l_LandShape.Position(l_Face.m_pVertices[2]);

            // Compute normal
            GVector3D<REAL32> l_Normal = (l_C - l_B).CrossProduct( (l_A - l_B) );
//            l_Normal.Normalize();

            // Add normal to each vertex of the face in the shape
            l_LandShape.Normal(l_Face.m_pVertices[0]) += l_Normal;
            l_LandShape.Normal(l_Face.m_pVertices[1]) += l_Normal;
            l_LandShape.Normal(l_Face.m_pVertices[2]) += l_Normal;
         }
      }
   }

   // Normalize vertex normals
   for(UINT32 i = 0;i < l_LandShape.NbNormals();i ++)
   {
      l_LandShape.Normal(i).Normalize();
//      l_LandShape.Normal(i) = GVector3D<REAL32>(0,0,-1);
   }

   // Create per vertex texture coordinates
   l_LandShape.NbTxtrs(1);
   l_LandShape.NbUVCoords(0, l_LandShape.NbPositions() );

   for(UINT32 p = 0;p < l_LandShape.NbPositions();p ++)
   {
      l_LandShape.UVCoord(0, p) = l_LandShape.Position(p) * 64;
   }

   delete[] l_HBuffer;

   m_TriFinder = new GTriFinder(m_LandMesh);
/*
   vector< pair<REAL32, GVector2D<REAL32> > > l_Path;
   l_Path.push_back(pair<REAL32, GVector2D<REAL32> >(0, in_Center) );
   l_Path.push_back(pair<REAL32, GVector2D<REAL32> >(10, in_Center + GVector2D<REAL32>(0.1f, 0.1f) ) );
   l_Path.push_back(pair<REAL32, GVector2D<REAL32> >(25, in_Center + GVector2D<REAL32>(-0.1f, 0.1f) ) );
   CreateUnitAnimation(l_Path);
*/
   // hack for the camera to be above ground

   GEventData l_Event;
   l_Event.Key() = VK_NUMPAD8;
   OnKeyDown(l_Event, NULL);

   return true;
}

//bool GDetailedEarthArea::DisplayArea(GVector2D<REAL32>& in_Center, GVector2D<REAL32>& in_Size)
//{
//   Map::GRasterMap l_RasterMap;
//   l_RasterMap.Init(L"./mods/sp2/map/config/raster_map_config.xml");
//
//   Map::GRasterView* l_ElevationView = l_RasterMap.LoadView(in_Center.x, in_Center.y, in_Size.x, in_Size.y);
//
//   // Clear current mesh
//   m_LandMesh->NbFacetLists(0);
//   GShape& l_LandShape = m_LandMesh->Shape();
//   l_LandShape.ClearAll();
//
//   // Create aligned points
//   REAL32 l_HeightFactor = 2.f / (in_Size.x * 40075000.f / 360.f);
//   UINT32 l_NbAlignedPoints = (m_NbDivs + 1) * (m_NbDivs + 1);
//   REAL32* l_HBuffer = new REAL32[l_NbAlignedPoints];
//   l_ElevationView->FillRectangularGrid(in_Center.x, in_Center.y, in_Size.x, in_Size.y, (m_NbDivs + 1), (m_NbDivs + 1), l_HBuffer);
//
//   l_LandShape.NbPositions(l_NbAlignedPoints);
//
//   // Fill positions with aligned points
//   UINT32 l_PosID = 0;
//   for(UINT32 y = 0;y <= m_NbDivs;y ++)
//   {
//      REAL32 v = 1 - 2 * ( (REAL32) y) / ( (REAL32) m_NbDivs);
//
//      for(UINT32 x = 0;x <= m_NbDivs;x ++)
//      {
//         REAL32 u = -1 + 2 * ( (REAL32) x) / ( (REAL32) m_NbDivs);
//
//         l_LandShape.Position(l_PosID) = GVector3D<REAL32>(u, v, l_HBuffer[l_PosID] * -l_HeightFactor);
//         l_PosID ++;
//      }
//   }
//
//   // Create center points
//   UINT32 l_NbCenterPoints = m_NbDivs * m_NbDivs;
//   REAL32 l_AlignedVsCenterRatio = (REAL32) (m_NbDivs - 1) / (REAL32) m_NbDivs;
//   l_ElevationView->FillRectangularGrid(in_Center.x, in_Center.y, in_Size.x * l_AlignedVsCenterRatio, in_Size.y * l_AlignedVsCenterRatio, m_NbDivs, m_NbDivs, l_HBuffer);
//
//   l_LandShape.NbPositions(l_NbAlignedPoints + l_NbCenterPoints);
//
//   // Fill positions with center points
//   l_PosID = 0;
//   for(UINT32 y = 0;y < m_NbDivs;y ++)
//   {
//      REAL32 v = 1 - 2 * ( ( (REAL32) y) + 0.5f) / ( (REAL32) m_NbDivs);
//
//      for(UINT32 x = 0;x < m_NbDivs;x ++)
//      {
//         REAL32 u = -1 + 2 * ( ( (REAL32) x) + 0.5f) / ( (REAL32) m_NbDivs);
//
//         l_LandShape.Position(l_PosID + l_NbAlignedPoints) = GVector3D<REAL32>(u, v, l_HBuffer[l_PosID] * -l_HeightFactor);
//         l_PosID ++;
//      }
//   }
//
//   // Create all triangles
//   m_LandMesh->NbFacetLists(1);
//   GTriangleListIndexed& l_Tris = m_LandMesh->Facet_List(0).Positions().TriangleListIndexed();
//   l_Tris.NbTriangles(4 * m_NbDivs * m_NbDivs);
//   UINT32 l_TriID = 0;
//   for(UINT32 y = 0;y < m_NbDivs;y ++)
//   {
//      for(UINT32 x = 0;x < m_NbDivs;x ++)
//      {
//         GTriangleIndex* l_Tri = &l_Tris.Triangle(l_TriID);
//
//         l_Tri[0].m_pVertices[0] = y * (m_NbDivs + 1) + x;
//         l_Tri[0].m_pVertices[2] = y * (m_NbDivs) + x + l_NbAlignedPoints;
//         l_Tri[0].m_pVertices[1] = y * (m_NbDivs + 1) + x + 1;
//
//         l_Tri[1].m_pVertices[0] = y * (m_NbDivs + 1) + x + 1;
//         l_Tri[1].m_pVertices[2] = y * (m_NbDivs) + x + l_NbAlignedPoints;
//         l_Tri[1].m_pVertices[1] = (y + 1) * (m_NbDivs + 1) + x + 1;
//
//         l_Tri[2].m_pVertices[0] = (y + 1) * (m_NbDivs + 1) + x + 1;
//         l_Tri[2].m_pVertices[2] = y * (m_NbDivs) + x + l_NbAlignedPoints;
//         l_Tri[2].m_pVertices[1] = (y + 1) * (m_NbDivs + 1) + x;
//
//         l_Tri[3].m_pVertices[0] = (y + 1) * (m_NbDivs + 1) + x;
//         l_Tri[3].m_pVertices[2] = y * (m_NbDivs) + x + l_NbAlignedPoints;
//         l_Tri[3].m_pVertices[1] = y * (m_NbDivs + 1) + x;
//
//         l_TriID += 4;
//      }
//   }
//
//   l_Tris.NbTriangles(l_TriID);
//
//   // Create per vertex normals
//   l_LandShape.NbNormals(l_Tris.NbTriangles() );
//
//   // Clear all normals
//   for(UINT32 i = 0;i < l_LandShape.NbNormals();i ++)
//   {
//      l_LandShape.Normal(i) = GVector3D<REAL32>(0,0,0);
//   }
//
//   // Sum all face normals of each point
//   for(UINT32 t = 0;t < l_Tris.NbTriangles();t ++)
//   {
//      // Fetch face vertices
//      GVector3D<REAL32> l_A, l_B, l_C;
//      GTriangleIndex& l_Face = l_Tris.Triangle(t);
//      l_A = l_LandShape.Position(l_Face.m_pVertices[0]);
//      l_B = l_LandShape.Position(l_Face.m_pVertices[1]);
//      l_C = l_LandShape.Position(l_Face.m_pVertices[2]);
//
//      // Compute normal
//      GVector3D<REAL32> l_Normal = (l_C - l_B).CrossProduct( (l_A - l_B) ) ;
//      l_Normal.Normalize();
//
//      // Add normal to each vertex of the face in the shape
//      l_LandShape.Normal(l_Face.m_pVertices[0]) += l_Normal;
//      l_LandShape.Normal(l_Face.m_pVertices[1]) += l_Normal;
//      l_LandShape.Normal(l_Face.m_pVertices[2]) += l_Normal;
//   }
//
//   // Normalize vertex normals
//   for(UINT32 i = 0;i < l_LandShape.NbNormals();i ++)
//   {
//      l_LandShape.Normal(i).Normalize();
//   }
//
//   //// This creates per face normal
//   //GTriangleListIndexed& l_NormalTris = m_LandMesh->Facet_List(0).Normals().TriangleListIndexed();
//   //l_LandShape.NbNormals(l_Tris.NbTriangles() );
//   //l_NormalTris.NbTriangles(l_Tris.NbTriangles() );
//   //for(UINT32 t = 0;t < l_Tris.NbTriangles();t ++)
//   //{
//   //   GVector3D<REAL32> l_A, l_B, l_C;
//   //   l_A = l_LandShape.Position(l_Tris.Triangle(t).m_pVertices[0]);
//   //   l_B = l_LandShape.Position(l_Tris.Triangle(t).m_pVertices[1]);
//   //   l_C = l_LandShape.Position(l_Tris.Triangle(t).m_pVertices[2]);
//
//   //   // Compute normal
//   //   GVector3D<REAL32> l_Normal = (l_C - l_B).CrossProduct( (l_A - l_B) ) ;
//   //   l_Normal.Normalize();
//
//   //   // Add normal to shape
//   //   l_LandShape.Normal(t) = l_Normal;
//
//   //   // Add triangle normal
//   //   GTriangleIndex l_Tri = {t, t, t};
//   //   l_NormalTris.Triangle(t) = l_Tri;
//   //}
//
//   delete[] l_HBuffer;
//
//   return true;
//}

//bool GDetailedEarthArea::DisplayArea(GVector2D<REAL32>& in_Center, GVector2D<REAL32>& in_Size)
//{
//   // Convert coordinates from degrees to radians
//   REAL32 l_fCenterXRad = in_Center.x * Math::PI / 180.0f;
//   REAL32 l_fCenterYRad = in_Center.y * Math::PI / 180.0f;
//
//   // Calculate maximum longitude & latitude deltas for requested detailed view
//   REAL32 l_fLatitudeDeltaRad = in_Size.y / EARTH_RADIUS;
//   REAL32 l_fLongitudeDeltaNorthRad = in_Size.x / (EARTH_RADIUS * cosf(l_fCenterYRad + l_fLatitudeDeltaRad * 0.5f) );
//   REAL32 l_fLongitudeDeltaSouthRad = in_Size.x / (EARTH_RADIUS * cosf(l_fCenterYRad - l_fLatitudeDeltaRad * 0.5f) );
//
//   // Convert deltas from radians to degrees
//   REAL32 l_fLatitudeDelta = l_fLatitudeDeltaRad * 180.0f / Math::PI;
//   REAL32 l_fLongitudeDeltaNorth = l_fLongitudeDeltaNorthRad * 180.0f / Math::PI;
//   REAL32 l_fLongitudeDeltaSouth = l_fLongitudeDeltaSouthRad * 180.0f / Math::PI;
//   REAL32 l_fLongitudeDeltaMax = max(l_fLongitudeDeltaNorth, l_fLongitudeDeltaSouth);
//
//   // Create a rotation matrix that centers the spherical mesh to face direction {0,0,-1}
//   GMatrix4x4<REAL32> l_RotMatrix;
//   l_RotMatrix.LoadIdentity();
//   GVector3D<REAL32> l_Out(-cosf(l_fCenterYRad) * sinf(l_fCenterXRad),
//                           -sinf(l_fCenterYRad),
//                           -cosf(l_fCenterYRad) * -cosf(l_fCenterXRad) );
//
//   l_RotMatrix(0,2) = l_Out.x;
//   l_RotMatrix(1,2) = l_Out.y;
//   l_RotMatrix(2,2) = l_Out.z;
//
//   GVector3D<REAL32> l_UpWorld(0, 1, 0);
//   GVector3D<REAL32> l_Up = l_UpWorld - (l_Out * l_UpWorld.DotProduct(l_Out) );
//   l_Up.Normalize();
//
//   l_RotMatrix(0,1) = l_Up.x;
//   l_RotMatrix(1,1) = l_Up.y;
//   l_RotMatrix(2,1) = l_Up.z;
//
//   GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_Out);
//
//   l_RotMatrix(0,0) = l_Right.x;
//   l_RotMatrix(1,0) = l_Right.y;
//   l_RotMatrix(2,0) = l_Right.z;
//
//   // Extract corners
//   GVector3D<REAL32> l_Corners[6];
//
//   // Min X, Min Y
//   l_Corners[0].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
//   l_Corners[0].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
//   l_Corners[0].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
//
//   // Center X, Min Y
//   l_Corners[1].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad);
//   l_Corners[1].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
//   l_Corners[1].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad);
//
//   // Max X, Min Y
//   l_Corners[2].x = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
//   l_Corners[2].y = sinf(l_fCenterYRad - l_fLatitudeDeltaRad / 2);
//   l_Corners[2].z = cosf(l_fCenterYRad - l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
//
//   // Min X, Max Y
//   l_Corners[3].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
//   l_Corners[3].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
//   l_Corners[3].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad - l_fLongitudeDeltaNorthRad / 2);
//
//   // Center X, Max Y
//   l_Corners[4].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad);
//   l_Corners[4].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
//   l_Corners[4].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad);
//
//   // Max X, Max Y
//   l_Corners[5].x = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * sinf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
//   l_Corners[5].y = sinf(l_fCenterYRad + l_fLatitudeDeltaRad / 2);
//   l_Corners[5].z = cosf(l_fCenterYRad + l_fLatitudeDeltaRad / 2) * -cosf(l_fCenterXRad + l_fLongitudeDeltaNorthRad / 2);
//
//   // Transform corners to face camera
//   for(UINT32 c = 0;c < 6;c ++)
//   {
//      l_Corners[c] = l_RotMatrix * l_Corners[c];
//   }
//
//   // Fetch minimum & maximum coordinates
//   REAL32 l_fXMin = l_Corners[0].x;
//   REAL32 l_fXMax = l_Corners[0].x;
//   REAL32 l_fYMin = l_Corners[0].y;
//   REAL32 l_fYMax = l_Corners[0].y;
//
//   for(UINT32 c = 1;c < 6;c ++)
//   {
//      l_fXMin = min(l_fXMin, l_Corners[c].x);
//      l_fXMax = max(l_fXMax, l_Corners[c].x);
//      l_fYMin = min(l_fYMin, l_Corners[c].y);
//      l_fYMax = max(l_fYMax, l_Corners[c].y);
//   }
//
//   // Calc a ratio to make the largest increment to fit in one screen
//   REAL32 l_fXDelta = l_fXMax - l_fXMin;
//   REAL32 l_fYDelta = l_fYMax - l_fYMin;
//
//   REAL32 l_fRatio = min(1 / l_fXDelta, 1 / l_fYDelta);
//
//   // Fill positions with transformed aligned points
//   REAL32 l_fHeightFactor = 4 * 2 / max(in_Size.x, in_Size.y);
//
//   // Load country map data
//   UINT32 l_CountryID = 2492;
//   Map::CMultifaceMap l_CountryMap;
//   l_CountryMap.init(L"./mods/sp2/map/config/multiface_map_config.xml");
//
//   Map::CMultifaceView* l_CountryView = l_CountryMap.openView(l_CountryID);
//
//   // Create outlines
//   GMultiFacetList l_ExternalContours, l_InternalContours;
//   GShape& l_OutlineShape = m_OutlineMesh->Shape();
//   l_CountryView->BuildContours(l_OutlineShape, l_ExternalContours, l_InternalContours);
//
//   GFaceList<GPolygon> l_Coast;
//   l_CountryView->BuildFaceList(l_OutlineShape, l_Coast, l_ExternalContours, l_InternalContours);
//
//   m_OutlineMesh->NbFacetLists(l_ExternalContours.NbFacetLists() );
//   for(UINT32 i = 0;i < l_ExternalContours.NbFacetLists();i ++)
//   {
//      m_OutlineMesh->Facet_List(i).Positions() = l_ExternalContours.Facet_List(i);
//   }
//
//   // Transform outlines to fit in {1,1}
//   for(UINT32 p = 0;p < l_OutlineShape.NbPositions();p ++)
//   {
//      GVector3D<REAL32>& l_Pos = l_OutlineShape.Position(p);
//
//      REAL32 l_fPhi = l_Pos.y * Math::PI / 180.0f;
//      REAL32 l_fCosPhi = cosf(l_fPhi);
//      REAL32 l_fSinPhi = sinf(l_fPhi);
//
//      REAL32 l_fTheta = l_Pos.x * Math::PI / 180.0f;
//
//      l_Pos.Set(l_fCosPhi * sinf(l_fTheta),
//                l_fSinPhi,
//                l_fCosPhi * -cosf(l_fTheta) );
//
//      l_Pos = l_RotMatrix * l_Pos;
//
//      l_Pos.z = 0;
//      l_Pos.x *= l_fRatio;
//      l_Pos.y *= l_fRatio;
//   }
//
//   // Load elevation map data
//   Map::GRasterMap l_RasterMap;
//   l_RasterMap.Init(L"./mods/sp2/map/config/raster_map_config.xml");
//
//   // Load a view that contains the necessary data in each direction
//   Map::GRasterView* l_ElevationView = l_RasterMap.LoadView(in_Center.x, in_Center.y, l_fLongitudeDeltaMax, l_fLatitudeDelta);
//
//   // Load water data
//   Map::CLoneAreaMap l_WaterMap;
//   l_WaterMap.init(L"./mods/sp2/map/config/lone_area_map_config.xml");
//
//   // Load a view that contains the necessary water data
//   Map::CLoneAreaView* l_WaterView = l_WaterMap.openView(in_Center.x, in_Center.y, l_fLongitudeDeltaMax, l_fLatitudeDelta);
//
//   // Fetch water data
//   GFaceList<GPolygon> l_Water;
//   GShape& l_WaterShape = m_WaterMesh->Shape();
//   l_WaterView->BuildFaceList(l_WaterShape, l_Water);
//
//   UINT32 l_CCW = 0;
//   UINT32 l_CW = 0;
//   m_WaterMesh->NbFacetLists(l_Water.NbFaces() );
//   for(UINT32 f = 0;f < l_Water.NbFaces();f ++)
//   {
//      REAL32 l_Area = 0;
//      for(UINT32 v = 0;v < l_Water.Face(f).Outer().NbIndexes();v ++)
//      {
//         UINT32 next = v + 1;
//         if(next == l_Water.Face(f).Outer().NbIndexes() )
//         {
//            next = 0;
//         }
//         l_Area += l_WaterShape.Position(l_Water.Face(f).Outer().Index(v) ).x * 
//                   l_WaterShape.Position(l_Water.Face(f).Outer().Index(next) ).y - 
//                   l_WaterShape.Position(l_Water.Face(f).Outer().Index(v) ).y * 
//                   l_WaterShape.Position(l_Water.Face(f).Outer().Index(next) ).x;
//      }
//
//      if(l_Area <= 0)
//      {
//         l_CW ++;
//      }
//      else
//      {
//         l_CCW ++;
//      }
//
//      l_Water.Face(f).ConvertToTriangleList(l_WaterShape, m_WaterMesh->Facet_List(f).Positions().TriangleListIndexed() );
//   }
//
//   // Transform water to fit in {1,1}
//   for(UINT32 p = 0;p < l_WaterShape.NbPositions();p ++)
//   {
//      GVector3D<REAL32>& l_Pos = l_WaterShape.Position(p);
//
//      REAL32 l_fPhi = l_Pos.y * Math::PI / 180.0f;
//      REAL32 l_fCosPhi = cosf(l_fPhi);
//      REAL32 l_fSinPhi = sinf(l_fPhi);
//
//      REAL32 l_fTheta = l_Pos.x * Math::PI / 180.0f;
//
//      l_Pos.Set(l_fCosPhi * sinf(l_fTheta),
//                l_fSinPhi,
//                l_fCosPhi * -cosf(l_fTheta) );
//
//      l_Pos = l_RotMatrix * l_Pos;
//
//      l_Pos.z = -0.5;
//      l_Pos.x *= l_fRatio;
//      l_Pos.y *= l_fRatio;
//   }
//
//   // Clear current mesh
//   m_LandMesh->NbFacetLists(0);
//   GShape& l_LandShape = m_LandMesh->Shape();
//   l_LandShape.ClearAll();
//
//   // Create aligned points
//   UINT32 l_NbAlignedPoints = (m_NbDivs + 1) * (m_NbDivs + 1);
//   REAL32* l_HBuffer = new REAL32[l_NbAlignedPoints];
//   l_ElevationView->FillTrapezoidalGrid(in_Center.x, in_Center.y, l_fLongitudeDeltaNorth, l_fLongitudeDeltaSouth, l_fLatitudeDelta, (m_NbDivs + 1), (m_NbDivs + 1), l_HBuffer);
//
//   l_LandShape.NbPositions(l_NbAlignedPoints);
////   l_LandShape.NbColors(l_NbAlignedPoints);
//   UINT32 l_PosID = 0;
//   for(UINT32 y = 0;y <= m_NbDivs;y ++)
//   {
//      REAL32 l_fSlope = (REAL32) y / ( (REAL32) (m_NbDivs) );
//      REAL32 l_fPhi = (l_fCenterYRad + l_fLatitudeDeltaRad / 2) - (l_fLatitudeDeltaRad * l_fSlope);
//      REAL32 l_fThetaDelta = (1 - l_fSlope) * l_fLongitudeDeltaNorthRad + l_fSlope * l_fLongitudeDeltaSouthRad;
//
//      REAL32 l_fCosPhi = cosf(l_fPhi);
//      REAL32 l_fSinPhi = sinf(l_fPhi);
//
//      for(UINT32 x = 0;x <= m_NbDivs;x ++)
//      {
//         REAL32 l_fTheta = (l_fCenterXRad - l_fThetaDelta / 2) + l_fThetaDelta * (REAL32) x / ( (REAL32) (m_NbDivs) );
//
//         GVector3D<REAL32> l_Pos(l_fCosPhi * sinf(l_fTheta),
//                                 l_fSinPhi,
//                                 l_fCosPhi * -cosf(l_fTheta) );
//
//         l_Pos = l_RotMatrix * l_Pos;
//
//         l_Pos.z = -l_fHeightFactor * l_HBuffer[l_PosID];
//         l_Pos.x *= l_fRatio;
//         l_Pos.y *= l_fRatio;
//
//         l_LandShape.Position(l_PosID) = l_Pos;
////         l_LandShape.Color(l_PosID) = GColorRGBInt(x * 255 / m_NbDivs, y * 255 / m_NbDivs, 0, 255);
//         l_PosID ++;
//      }
//   }
//
//   // Create all triangles
//   m_LandMesh->NbFacetLists(1);
//   {
//      GTriangleListIndexed& l_Tris = m_LandMesh->Facet_List(0).Positions().TriangleListIndexed();
//      l_Tris.NbTriangles(2 * m_NbDivs * m_NbDivs);
//      UINT32 l_TriID = 0;
//      for(UINT32 y = 0;y < m_NbDivs;y ++)
//      {
//         for(UINT32 x = 0;x < m_NbDivs;x ++)
//         {
//            GTriangleIndex* l_Tri = &l_Tris.Triangle(l_TriID);
//
//            l_Tri[0].m_pVertices[0] = y * (m_NbDivs + 1) + x;
//            l_Tri[0].m_pVertices[2] = (y + 1) * (m_NbDivs + 1) + x;
//            l_Tri[0].m_pVertices[1] = y * (m_NbDivs + 1) + x + 1;
//
//            l_Tri[1].m_pVertices[0] = y * (m_NbDivs + 1) + x + 1;
//            l_Tri[1].m_pVertices[2] = (y + 1) * (m_NbDivs + 1) + x;
//            l_Tri[1].m_pVertices[1] = (y + 1) * (m_NbDivs + 1) + x + 1;
//
//            l_TriID += 2;
//         }
//      }
//
//      l_Tris.NbTriangles(l_TriID);
//   }
//
//   // Clip triangles with coast lines
//   g_Joshua.Renderer()->Helper()->SplitMesh(m_LandMesh, l_OutlineShape, l_Coast);
//
//   // Create land colors
//   l_LandShape.NbColors(3);
//   l_LandShape.Color(0) = GColorRGBInt(0,0,200,255);
//   l_LandShape.Color(1) = GColorRGBInt(100,75,25,255);
//   l_LandShape.Color(2) = GColorRGBInt(255,0,255,255);
//
//   // Create per vertex (smooth) normals
//   l_LandShape.NbNormals(l_LandShape.NbPositions() );
//
//   // Clear all normals
//   for(UINT32 i = 0;i < l_LandShape.NbNormals();i ++)
//   {
//      l_LandShape.Normal(i) = GVector3D<REAL32>(0,0,0);
//   }
//
//   for(UINT32 f = 0;f < m_LandMesh->NbFacetLists();f ++)
//   {
//      GTriangleListIndexed& l_Tris = m_LandMesh->Facet_List(f).Positions().TriangleListIndexed();
//      GTriangleListIndexed& l_ColorTris = m_LandMesh->Facet_List(f).Colors().TriangleListIndexed();
//      l_ColorTris.NbTriangles(l_Tris.NbTriangles() );
//
//      // Sum all face normals of each point
//      for(UINT32 t = 0;t < l_Tris.NbTriangles();t ++)
//      {
//         l_ColorTris.Triangle(t).m_pVertices[0] = f;
//         l_ColorTris.Triangle(t).m_pVertices[1] = f;
//         l_ColorTris.Triangle(t).m_pVertices[2] = f;
//
//         // Fetch face vertices
//         GVector3D<REAL32> l_A, l_B, l_C;
//         GTriangleIndex& l_Face = l_Tris.Triangle(t);
//         l_A = l_LandShape.Position(l_Face.m_pVertices[0]);
//         l_B = l_LandShape.Position(l_Face.m_pVertices[1]);
//         l_C = l_LandShape.Position(l_Face.m_pVertices[2]);
//
//         // Compute normal
//         GVector3D<REAL32> l_Normal = (l_C - l_B).CrossProduct( (l_A - l_B) ) ;
//         l_Normal.Normalize();
//
//         // Add normal to each vertex of the face in the shape
//         l_LandShape.Normal(l_Face.m_pVertices[0]) += l_Normal;
//         l_LandShape.Normal(l_Face.m_pVertices[1]) += l_Normal;
//         l_LandShape.Normal(l_Face.m_pVertices[2]) += l_Normal;
//      }
//   }
//
//   // Normalize vertex normals
//   for(UINT32 i = 0;i < l_LandShape.NbNormals();i ++)
//   {
//      l_LandShape.Normal(i).Normalize();
//   }
//
//   delete[] l_HBuffer;
//
//   return true;
//}
//
//

bool GDetailedEarthArea::CreateUnitAnimation(const vector< pair<REAL32, GVector2D<REAL32> > >& in_vPath)
{
   const REAL32 l_fUnitSize = 50.f;
   const GShape& l_Shape = m_LandMesh->Shape();

   if(!in_vPath.size())
      return false;

   REAL32 l_fCurrentTime = m_Scene->Get_Animation_System()->Get_Animation_Time();

   // Destroy last path
   m_LandModel->Remove_Child(m_pUnitPathModel);
   SAFE_RELEASE(m_pUnitPathModel);
   while(m_Scene->Get_Animation_System()->Get_Nb_Tracks() > 0)
   {
      m_Scene->Get_Animation_System()->Remove_Track_At(0);
   }

   // Create a new path
   m_pUnitPathModel = g_Joshua.Renderer()->Create_Model();
   m_LandModel->Add_Child(m_pUnitPathModel);

   // Load a unit to display
   GFile l_File;
   g_Joshua.FileManager()->File(L"unit.xml", l_File);
   CGFX_Model_Itf* l_pUnitModel = g_Joshua.Renderer()->Helper()->LoadModelGolem(l_File);
   m_pUnitPathModel->Add_Child(l_pUnitModel);

   // Load the walk (no advance) animation of the unit
   CGFX_Action_Itf* l_pUnitWalkAction = g_Joshua.Renderer()->Helper()->LoadActionGolem(l_File, l_pUnitModel, false);

   // Scale unit dimensions
   g_Joshua.Renderer()->Helper()->ScaleHierarchy(l_pUnitModel, m_fUnitToMeterRatio * l_fUnitSize / 100 );
   g_Joshua.Renderer()->Helper()->ScaleAction(l_pUnitWalkAction, m_fUnitToMeterRatio * l_fUnitSize / 100);

   // Create a track to playback this unit animation
   CGFX_Track_Itf* l_UnitWalkTrack = g_Joshua.Renderer()->Create_Track();
   l_UnitWalkTrack->Set_Action(l_pUnitWalkAction);
   l_UnitWalkTrack->Set_Start_Time(l_fCurrentTime);
   l_UnitWalkTrack->Playback_Count(0);
   l_UnitWalkTrack->Playback(GFX::PLAYBACK_FORWARD);

   m_Scene->Get_Animation_System()->Add_Track(l_UnitWalkTrack);

   CGFX_Transform_SRT_Itf* l_pUnitTrans = g_Joshua.Renderer()->Create_Transform_SRT();

   l_pUnitModel->Set_Transform(l_pUnitTrans);

   // Create an animation that will move the unit trough the land
   CGFX_Animation_Linear_Itf* l_pUnitAnimX = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Animation_Linear_Itf* l_pUnitAnimY = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Animation_Linear_Itf* l_pUnitAnimZ = g_Joshua.Renderer()->Create_Animation_Linear();

   l_pUnitAnimX->Connect(l_pUnitTrans, GFX::PARAM_TRANSLATION_X);
   l_pUnitAnimY->Connect(l_pUnitTrans, GFX::PARAM_TRANSLATION_Y);
   l_pUnitAnimZ->Connect(l_pUnitTrans, GFX::PARAM_TRANSLATION_Z);

   // Also create an animation that will align the unit with its path
   CGFX_Animation_Constant_Itf* l_pUnitRotX = g_Joshua.Renderer()->Create_Animation_Constant();
   CGFX_Animation_Constant_Itf* l_pUnitRotZ = g_Joshua.Renderer()->Create_Animation_Constant();

   l_pUnitRotX->Connect(l_pUnitTrans, GFX::PARAM_ROTATION_X);
   l_pUnitRotZ->Connect(l_pUnitTrans, GFX::PARAM_ROTATION_Z);

   // Create a path with lines that will be the same as unit animation
   CGFX_Model_Itf* l_pPathModel = g_Joshua.Renderer()->Create_Model();
   CGFX_Mesh_Itf* l_pPathMesh = g_Joshua.Renderer()->Create_Mesh();

   m_pUnitPathModel->Add_Child(l_pPathModel);
   l_pPathModel->Set_Primitive(l_pPathMesh);
   l_pPathModel->Set_Material(m_Scene->DefaultMaterial() );

   GShape& l_PathShape = l_pPathMesh->Shape();
   l_PathShape.NbPositions(in_vPath.size() );
   l_PathShape.NbColors(in_vPath.size() );

   l_pPathMesh->NbFacetLists(1);
   l_pPathMesh->Facet_List(0).Positions().Type(FACETS_LINE_STRIP, false);

   GLineStrip& l_PathPosLines = l_pPathMesh->Facet_List(0).Positions().LineStrip();
   l_PathPosLines.Start() = 0;
   l_PathPosLines.NbLines(in_vPath.size() - 1);

   GVector3D<REAL32> l_LastCoord;

   for(UINT32 i = 0;i < in_vPath.size();i ++)
   {
      l_PathShape.Color(i) = GColorRGBInt(255, 0, 0, 128);

      GVector3D<REAL32> l_Coord = in_vPath[i].second;

      ConvertSphericalCoordinate(l_Coord);

      CGFX_Key_Linear l_Key;
      l_Key.m_Time = in_vPath[i].first;
      l_Key.m_Value = l_Coord.x;

      l_pUnitAnimX->Add_Key(l_Key);

      l_Key.m_Value = l_Coord.y;

      l_pUnitAnimY->Add_Key(l_Key);

      l_Key.m_Value = 0;

      if(m_TriFinder)
      {
         // Fetch the triangle that is beneath the camera
         UINT32 l_Tri;
         if(m_TriFinder->Locate(l_Coord, l_Tri) )
         {
            // Extract the triangle corners
            const GVector3D<REAL32>& l_A = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[0] );
            const GVector3D<REAL32>& l_B = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[1] );
            const GVector3D<REAL32>& l_C = l_Shape.Position(m_LandMesh->Facet_List(0).Positions().TriangleListIndexed().Triangle(l_Tri).m_pVertices[2] );

            // Compute the triangle plane normal
            GVector3D<REAL32> l_Normal = (l_B - l_A).CrossProduct(l_C - l_A);
            l_Normal.Normalize();

            // Compute the triangle plane distance
            REAL32 l_fPlaneDist = l_Normal.DotProduct(l_A);

            // Compute the heigth of the triangle at the camera position using intersection of triangle plane & vertical ray
            GVector3D<REAL32> l_Ray(0,0,1);
            REAL32 t = (l_fPlaneDist - l_Normal.DotProduct(l_Coord) ) / l_Normal.DotProduct(l_Ray);

            l_Key.m_Value += l_Ray.z * t - m_fUnitToMeterRatio * l_fUnitSize / 5;
         }
      }

      l_Coord.z = l_Key.m_Value;

      l_pUnitAnimZ->Add_Key(l_Key);

      l_PathShape.Position(i) = l_Coord;

      if(i != 0)
      {
         CGFX_Key_Constant l_KeyRot;
         l_KeyRot.m_Time = in_vPath[i - 1].first;

         l_KeyRot.m_Value = -atan2f(l_LastCoord.x - l_Coord.x, l_LastCoord.y - l_Coord.y);

         l_pUnitRotZ->Add_Key(l_KeyRot);

         REAL32 l_Length = (l_LastCoord - l_Coord).Length();

         l_KeyRot.m_Value = asinf( (l_LastCoord.z - l_Coord.z) / l_Length);
         l_pUnitRotX->Add_Key(l_KeyRot);
      }

      l_LastCoord = l_Coord;
   }

   // Add all animations to an action
   CGFX_Action_Itf* l_UnitAction = g_Joshua.Renderer()->Create_Action();
   l_UnitAction->Length(in_vPath[in_vPath.size() - 1].first);

   l_UnitAction->Add_Animation(l_pUnitAnimX);
   l_UnitAction->Add_Animation(l_pUnitAnimY);
   l_UnitAction->Add_Animation(l_pUnitAnimZ);
   l_UnitAction->Add_Animation(l_pUnitRotX);
   l_UnitAction->Add_Animation(l_pUnitRotZ);

   CGFX_Track_Itf* l_UnitTrack = g_Joshua.Renderer()->Create_Track();
   l_UnitTrack->Set_Action(l_UnitAction);
   l_UnitTrack->Set_Start_Time(l_fCurrentTime);
//   l_UnitTrack->Length_Relative(1);
   l_UnitTrack->Playback(GFX::PLAYBACK_FORWARD);
   l_UnitWalkTrack->Set_Length(l_fCurrentTime);

   m_Scene->Get_Animation_System()->Add_Track(l_UnitTrack);
   SAFE_RELEASE(l_pUnitAnimX);
   SAFE_RELEASE(l_pUnitAnimY);
   SAFE_RELEASE(l_pUnitAnimZ);
   SAFE_RELEASE(l_pUnitRotX);
   SAFE_RELEASE(l_pUnitRotZ);
   SAFE_RELEASE(l_UnitAction);
   SAFE_RELEASE(l_UnitTrack);
   SAFE_RELEASE(l_UnitWalkTrack);

   SAFE_RELEASE(l_pUnitModel);

   m_Scene->Update_Render_List(m_RList);

   m_Scene->Get_Animation_System()->Play_Animation();

   return true;
}

inline void GDetailedEarthArea::ConvertSphericalCoordinate(GVector3D<REAL32>& io_Point)
{
   // Convert to rectangular coordinates
   REAL32 l_fHeigth = io_Point.z;
   io_Point.z = 0;
   REAL32 l_fPhi = io_Point.y * (REAL32)Math::PI / 180.0f;
   REAL32 l_fCosPhi = cosf(l_fPhi);
   REAL32 l_fSinPhi = sinf(l_fPhi);

   REAL32 l_fTheta = io_Point.x * (REAL32)Math::PI / 180.0f;

   io_Point.Set(l_fCosPhi * sinf(l_fTheta),
                l_fSinPhi,
                l_fCosPhi * -cosf(l_fTheta) );

   io_Point = m_RotMatrix * io_Point;

   io_Point.z = -m_fUnitToMeterRatio * l_fHeigth;
   io_Point.x *= m_fRadToUnitRatio;
   io_Point.y *= m_fRadToUnitRatio;
}

inline void GDetailedEarthArea::ConvertRectangularCoordinate(GVector3D<REAL32>& io_Point)
{
   /*
   // Convert to spherical coordinates
   REAL32 l_fHeigth = io_Point.z;
   io_Point.z = 0;
   REAL32 l_fPhi = io_Point.y * Math::PI / 180.0f;
   REAL32 l_fCosPhi = cosf(l_fPhi);
   REAL32 l_fSinPhi = sinf(l_fPhi);

   REAL32 l_fTheta = io_Point.x * Math::PI / 180.0f;

   io_Point.Set(l_fCosPhi * sinf(l_fTheta),
                l_fSinPhi,
                l_fCosPhi * -cosf(l_fTheta) );

   io_Point = m_RotMatrix * io_Point;

   io_Point.z = -m_fUnitToMeterRatio * l_fHeigth;
   io_Point.x *= m_fRadToUnitRatio;
   io_Point.y *= m_fRadToUnitRatio;
   */
   REAL32 l_fHeigth = -io_Point.z / m_fUnitToMeterRatio;

   io_Point.x /= m_fRadToUnitRatio;
   io_Point.y /= m_fRadToUnitRatio;
   io_Point.z = -sqrtf(1 - (io_Point.x * io_Point.x + io_Point.y * io_Point.y) );

   io_Point = m_RotMatrix.Transpose() * io_Point;
   REAL32 l_fTheta = atan2f(io_Point.x, -io_Point.z);
   REAL32 l_fPhi = asinf(io_Point.y);

   io_Point.x = l_fTheta * 180.0f / (REAL32)Math::PI;
   io_Point.y = l_fPhi * 180.0f / (REAL32)Math::PI;
   io_Point.z = l_fHeigth;
}

bool GDetailedEarthArea::MouseToCoordinate(const GVector2D<UINT32>& in_MousePos, GVector3D<REAL32>& out_Coord)
{
   // Get the current resolution
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);

   // Get the projection matrix
   GMatrix4x4<REAL32> l_ProjMat = m_Camera->Get_Projection_Matrix();

   // Compute the vector of the pick ray in screen space
   GVector3D<REAL32> l_ScreenPos;
   l_ScreenPos.x =  ( ( ( 2.0f * in_MousePos.x ) / l_Mode.m_Resolution.x ) - 1 ) / l_ProjMat(0,0);
   l_ScreenPos.y = -( ( ( 2.0f * in_MousePos.y ) / l_Mode.m_Resolution.y ) - 1 ) / l_ProjMat(1,1);
   l_ScreenPos.z =  1.0f;

   // Get the inverse view matrix
   GMatrix4x4<REAL32> l_ViewMat = m_Camera->Get_View_Matrix(m_CameraTransform->Get_Matrix() );
   l_ViewMat = l_ViewMat.Inverse();

   // Transform the screen space pick ray into 3D space
   GRay<REAL32> l_Ray;
   l_Ray.m_Direction.x  = l_ScreenPos.x*l_ViewMat(0,0) + l_ScreenPos.y*l_ViewMat(1,0) + l_ScreenPos.z*l_ViewMat(2,0);
   l_Ray.m_Direction.y  = l_ScreenPos.x*l_ViewMat(0,1) + l_ScreenPos.y*l_ViewMat(1,1) + l_ScreenPos.z*l_ViewMat(2,1);
   l_Ray.m_Direction.z  = l_ScreenPos.x*l_ViewMat(0,2) + l_ScreenPos.y*l_ViewMat(1,2) + l_ScreenPos.z*l_ViewMat(2,2);

   l_Ray.m_Center.x = l_ViewMat(3,0);
   l_Ray.m_Center.y = l_ViewMat(3,1);
   l_Ray.m_Center.z = l_ViewMat(3,2);

   // If tri finder exists, use it to locate the intersection with terrain
   if(m_TriFinder)
   {
      UINT32 l_iDummy;
      return m_TriFinder->Locate(l_Ray, out_Coord, l_iDummy);
   }

   // Simply intersect with plane z = 0
   return l_Ray.IntersectPlane( GVector3D<REAL32>(0.f, 0.f, 1.f), 0.f, out_Coord);
}
