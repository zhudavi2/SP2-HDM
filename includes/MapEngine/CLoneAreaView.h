#ifndef _CLASS_LONE_AREA_VIEW
#define _CLASS_LONE_AREA_VIEW


#include "CVectorView.h"
#include "CLoneArea.h"


namespace Map 
{

   class GLoneAreaView: public GVectorView
   {
      friend class GLoneAreaMap;

   public:
      // access particular lone area
      const GLoneArea* LoneArea( ID in_iLoneArea ) const;

      // find which area, if any, contains the given point
      const GLoneArea* Locate( const GPoint& in_Point ) const;
      const GLoneArea* Locate( REAL32 in_fX, REAL32 in_fY ) const;

      // produce geometric constructs for graphical representation of lone areas
      void BuildFaceList(  ID in_iLoneArea, 
                           Geometry::GShape& out_Shape,
                           Geometry::GFaceList<Geometry::GPolygon>& out_FaceList ) const;
      void BuildFaceList(  const vector<ID>& in_LoneAreas,
                           Geometry::GShape& out_Shape,
                           Geometry::GFaceList<Geometry::GPolygon>& out_FaceList ) const;
      void BuildFaceList(  Geometry::GShape& out_Shape,
                           Geometry::GFaceList<Geometry::GPolygon>& out_FaceList ) const;
      void ClipLoneAreasInRectangle(   REAL32 in_fCenterX,  
                                       REAL32 in_fCenterY,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight,
                                       Geometry::GShape& out_Shape,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Outers,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Inners ) const;
      void ClipLoneAreasInRectangle(   const GPoint& in_Center,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight,
                                       Geometry::GShape& out_Shape,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Outers,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Inners ) const;

      // write lone areas in text file using given separator
      void WriteLoneAreaPoints( const GString& in_sFileName, const GString& in_sSeparator ) const;

   private:
      GLoneAreaView( GLoneAreaMap* in_pParentMap );
      ~GLoneAreaView();

      // maps of features contained in the view
      hash_map<ID, GLoneArea*> m_LoneAreas;

      // load features from data streams in parent map
      void Load();                                    // load all lone areas
      void Load( const vector<ID>& in_LoneAreas );    // load multifaces in lone area list

      // create GLoneArea, read it from stream, and add it to lone areas map
      void CreateLoneArea( GFile &in_File );

      // helper functions to build geometry objects
      void InitGeometryFaceList( Geometry::GShape& out_Shape,
                                 Geometry::GFaceList<Geometry::GPolygon>& out_FaceList ) const;
      void InitGeometryFaceList( Geometry::GShape& out_Shape,
                                 Geometry::GFaceList<Geometry::GPolygon>& out_FaceList,
                                 const vector<ID>& in_LoneAreas ) const;
      void LoneAreaToGeometryFace( const GLoneArea& in_LoneArea,
                                   Geometry::GFace<Geometry::GPolygon>& out_Face,
                                   Geometry::GShape& out_Shape,
                                   UINT32& io_iCurrentShapePosition ) const;
      void FillGeometryPolygon( const GLoop& in_Loop,
                                Geometry::GPolygon& out_Polygon,
                                Geometry::GShape& out_Shape,
                                UINT32& io_iCurrentShapePosition ) const;
      void GatherVisibleLoneAreas( const GBox& in_ViewPort, vector<GLoneArea*>& out_VisibleLoneAreas ) const;
   };

}

#endif