/**************************************************************
*
* sp2_atlas_txtr_generator.h
*
* Description
* ===========
*  Description & implementation of SP2::GAtlasTxtrGenerator, a
*  class that generates an atlas style texture to be used
*  on the background earth at close view range.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_ATLAS_TXTR_GENERATOR_H_
#define _SP2_ATLAS_TXTR_GENERATOR_H_

namespace SP2
{
   //! Structure to describe a region texture
   struct GRegionTxtr
   {
      UINT32            m_iID;
      CGFX_Texture_Itf* m_pTxtr;
      GVector2D<REAL32> m_Coord;
      GVector2D<REAL32> m_Size;
   };

   /*!
    * Class that create pictures of unit parts
    **/
   class GAtlasTxtrGenerator
   {
   public:
      GAtlasTxtrGenerator();
      ~GAtlasTxtrGenerator();

      //! Prepare all subsequent texture generations at the specified location
      void Prepare(const GVector2D<REAL32>& in_Coord, 
                   const GVector2D<REAL32>& in_Size, 
                   const GVector2D<UINT32>& in_Dim);

      //! Generate the elevation texture (bump) at the currently prepared location
      CGFX_Texture_Itf* GenerateElevation();

      //! Generate the detail texture (water, roads & rivers) at the currently prepared location
      CGFX_Texture_Itf* GenerateDetails(bool in_bCreateRivers, bool in_bCreateRoads);

      //! Generate the region textures at the currently prepared location
      bool GenerateRegions(vector<GRegionTxtr>& out_vRegionTxtrs);

      //! Free memory after generation
      void Reset();

      //! Start texture generation
      void EnableGeneration();

      //! Stop texture generation
      void DisableGeneration();

      //! Check if texture generation is started (true) or stopped (false)
      bool IsEnabled();

   private:
      // Draw all coast lines
      bool DrawCoasts(const GVector2D<REAL32>& in_Coord, 
                      const GVector2D<REAL32>& in_Size, 
                      REAL32 in_fLongitudeAdjust);

      // Draw all water and their island
      bool DrawWaterAndIslands(const GVector2D<REAL32>& in_Coord, 
                               const GVector2D<REAL32>& in_Size, 
                               REAL32 in_fLongitudeAdjust);

      // Draw the specified region
      bool DrawRegion(const Map::GMultifaceView* in_pRegionView, 
                      const GVector2D<REAL32>& in_Coord, 
                      const GVector2D<REAL32>& in_Size, 
                      const GVector2D<REAL32>& in_PixelAdjust,
                      REAL32 in_fLongitudeAdjust);

      // Adjust all longitude (x) coordinate of a shape by the required amount
      void AdjustShape(GShape& in_Shape, REAL32 in_fAdjustValue);

      // Make necessary adjustment to the shape longitude (x) 
      // knowing that its elements crosses the -180/180 longitude boundary
      void AdjustShape(GShape& in_Shape, bool in_bCrosses180, bool in_bCrosses180Neg);

      GVector3D<REAL32>             m_Offset;
      GVector3D<REAL32>             m_Scale;
      GRectangle<INT32>             m_Window;

      GRectangle<INT32>             m_RegionWindow;

      Geometry::GFace<GPolygon>     m_PolyLine;
      GShape                        m_ShapeLine;

      // Draw an adminstrative border (using m_AdminColor)
      void  DrawMapEdge(REAL32 in_fWidth, 
                        const Map::GEdge* in_pEdge, 
                        bool in_bCrosses180, 
                        bool in_bCrosses180Neg);

      static UINT32                 m_iPitchByte;

      // Span (draw) a line of land
      static void SpanLand(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      // Span (draw) a line of water
      static void SpanWater(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      // Span (draw) a line of island
      static void SpanIsland(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      static GPixelA8R8G8B8         m_AdminColor;

      // Span (draw) a line of administrative region border
      static void SpanAdmin(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      static UINT32                 m_iRegionPitchByte;

      // Span (draw) a line of inside region polygon
      static void SpanRegionIn(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      // Span (draw) a line of outside region polygon
      static void SpanRegionOut(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      // Draw a pixel of river
      static void DrawRiver(GImage2D* in_pImage, INT32 x, INT32 y);

      // Draw a pixel of road
      static void DrawRoad(GImage2D* in_pImage, INT32 x, INT32 y);

      GImage2D*                     m_pImage;
      GImage2D*                     m_pRegionImage;
//      GPixelA8R8G8B8*               m_pAtlasPalette;

      Map::GMultifaceView*          m_pCoastlineView;
      Map::GMultifaceView*          m_pAdminView;
      Map::GLoneAreaView*           m_pWaterView;
      Map::GContinuousRasterView*   m_pElevView;

      Map::GCurveView*              m_pRiverView;
      Map::GCurveView*              m_pRoadView;

      GVector2D<REAL32>             m_Coord;
      GVector2D<REAL32>             m_Size;
      GVector2D<UINT32>             m_Dim;

      bool                          m_bCrosses180;
      bool                          m_bCrosses180Neg;

      volatile bool                 m_bContinue;
   };

}

#endif // #ifndef _SP2_ATLAS_TXTR_GENERATOR_H_
