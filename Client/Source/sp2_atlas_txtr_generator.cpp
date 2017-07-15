/**************************************************************
*
* sp2_atlas_txtr_generator.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_atlas_txtr_generator.h"

// Define to enable logging of texture generation performance
#define PROFILE_GENERATION

// Macros to facilitate generation profiling
#ifdef PROFILE_GENERATION
   #define SETUP_PROFILER() \
      GTimeCounter l_Timer; \
      REAL32 l_StopTime; \
      REAL32 l_StartTime = l_Timer.GetTime();

   #define LOG_PROFILER(text) \
      l_StopTime = l_Timer.GetTime(); \
      g_Joshua.Log(GString(text) + GString(L" : ") + GString(l_StopTime - l_StartTime) ); \
      l_StartTime = l_Timer.GetTime();

#else
   #define SETUP_PROFILER
   #define LOG_PROFILER(text)

#endif

// Define to enable save of generated textures
//#define WRITE_GENERATION

/*
const GPixelA8R8G8B8 c_LandColor    = {  0,   0,   0, 128};

const GPixelA8R8G8B8 c_RoadColor    = { 20,  58,  84, 255};
const GPixelA8R8G8B8 c_OceanColor   = {150,  96,  38,   0};
const GPixelA8R8G8B8 c_RiverColor   = {255, 211, 133, 255};
const GPixelA8R8G8B8 c_WaterColor   = {255, 236, 204, 255};
const GPixelA8R8G8B8 c_AdminColor   = {150,  64,  64, 255};
const GPixelA8R8G8B8 c_CountryColor = { 16,  16, 175, 255};
*/
const GPixelA8R8G8B8 c_RegionColor        = { 0, 0, 0, 255};
const GPixelA8R8G8B8 c_RegionBlackColor   = { 0, 0, 0, 0};

const GPixelA8R8G8B8 c_CoastIn      = {0, 0, 0, 0};
const GPixelA8R8G8B8 c_CoastOut     = {128, 72, 32, 128};

const GPixelA8R8G8B8 c_DetailOut       = {0, 0, 0, 255};
const GPixelA8R8G8B8 c_DetailWaterIn   = {255, 128, 0, 160};
const GPixelA8R8G8B8 c_DetailIslandIn  = c_DetailOut;
const GPixelA8R8G8B8 c_DetailRiverIn   = {255, 128, 0, 200};
const GPixelA8R8G8B8 c_DetailRoadIn    = {0, 0, 0, 32};

const GPixelA8R8G8B8 c_Border       = {255, 255, 255, 0};
/*
const UINT32 c_iNbHeigthLevels = 9;
const GPixelA8R8G8B8 c_LevelColor[c_iNbHeigthLevels] = 
{
   {152, 228, 140, 255},
   {162, 234, 184, 255},
   {160, 246, 237, 255},
   {116, 205, 224, 255},
   { 53, 112, 173, 255},
   { 28, 103, 134, 255},
   {132,  92,  98, 255},
   {199, 179, 182, 255},
   {229, 226, 225, 255},
};

const INT16 c_iLevelHeight[c_iNbHeigthLevels] = 
{
   200,
   400,
   1000,
   1500,
   2000,
   3000,
   4000,
   5000,
   32767,
};

const INT16 c_iLevelTransition[c_iNbHeigthLevels] = 
{
   40,
   40,
   120,
   100,
   100,
   200,
   200,
   200,
   200,
};

const UINT32 c_iNbLevels = 65536;
const REAL32 c_fPaletteOffset = 128.f;
const REAL32 c_fPaletteFactor = 0.25f;
const UINT32 c_iNbAtlasPaletteColors = (UINT32) (c_iNbLevels * c_fPaletteFactor) + 1;
*/
GPixelA8R8G8B8 GAtlasTxtrGenerator::m_AdminColor;
UINT32 GAtlasTxtrGenerator::m_iPitchByte;
UINT32 GAtlasTxtrGenerator::m_iRegionPitchByte;

GAtlasTxtrGenerator::GAtlasTxtrGenerator()
{
   m_pImage = GImage2D::AllocObject();
   m_pRegionImage = GImage2D::AllocObject();
/*
   // Create atlas palette
   m_pAtlasPalette = new GPixelA8R8G8B8[c_iNbAtlasPaletteColors];

   for(UINT16 i = 0;i < c_iNbAtlasPaletteColors;i ++)
   {
      INT16 l_iHeigth = (INT16) ( (i - c_fPaletteOffset) / c_fPaletteFactor);

      UINT32 l_iLevelIndex = 0;
      while(l_iHeigth > c_iLevelHeight[l_iLevelIndex])
      {
         l_iLevelIndex ++;
      }

      m_pAtlasPalette[i] = c_LevelColor[l_iLevelIndex];

      if(l_iLevelIndex < c_iNbHeigthLevels - 1)
      {
         INT16 l_iDelta = c_iLevelHeight[l_iLevelIndex] - l_iHeigth;
         if(l_iDelta < c_iLevelTransition[l_iLevelIndex] )
         {
            INT16 l_iDeltaInv = c_iLevelTransition[l_iLevelIndex] - l_iDelta;
            m_pAtlasPalette[i].R = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].R * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].R * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
            m_pAtlasPalette[i].G = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].G * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].G * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
            m_pAtlasPalette[i].B = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].B * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].B * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
            m_pAtlasPalette[i].A = 255;
         }
      }
   }

   // Add this section if you want to create a bitmap of the height atlas
#ifdef WRITE_GENERATION
   {
      RGBTRIPLE* l_RGBData = new RGBTRIPLE[c_iNbAtlasPaletteColors];
      RGBTRIPLE* l_RGBDataPtr = l_RGBData;

      for(UINT32 x = 0;x < c_iNbAtlasPaletteColors;x ++)
      {
         l_RGBDataPtr->rgbtBlue = m_pAtlasPalette[x].B;
         l_RGBDataPtr->rgbtGreen = m_pAtlasPalette[x].G;
         l_RGBDataPtr->rgbtRed = m_pAtlasPalette[x].R;

         l_RGBDataPtr ++;
      }
      // Open bitmap file
	   HANDLE l_DstFile = CreateFile(L"AtlasPalette.bmp",
										      GENERIC_WRITE,
										      FILE_SHARE_READ,
										      NULL,
										      CREATE_ALWAYS,
										      FILE_ATTRIBUTE_NORMAL,
										      NULL);

      m_pImage->SaveColorBitmap(l_DstFile, l_RGBData, c_iNbAtlasPaletteColors, 1);

      CloseHandle(l_DstFile);
   }
#endif
*/
   m_pCoastlineView = NULL;
   m_pAdminView = NULL;
   m_pWaterView = NULL;
   m_pElevView = NULL;

   m_pRiverView = NULL;
   m_pRoadView = NULL;

   m_bContinue = false;

   m_PolyLine.Outer().Start() = 0;
   m_PolyLine.Outer().NbIndexes() = 4;
   m_ShapeLine.NbPositions(4);
}

GAtlasTxtrGenerator::~GAtlasTxtrGenerator()
{
   GImage2D::FreeObject(m_pRegionImage);
   GImage2D::FreeObject(m_pImage);
//   SAFE_DELETE_ARRAY(m_pAtlasPalette);
}

void GAtlasTxtrGenerator::Prepare(const GVector2D<REAL32>& in_Coord, 
                                  const GVector2D<REAL32>& in_Size, 
                                  const GVector2D<UINT32>& in_Dim)
{
   SETUP_PROFILER();

   m_Coord = in_Coord;
   m_Size = in_Size;
   m_Dim = in_Dim;

   // Create the atlas image
   GVector2D<UINT32> l_DimPow2;
   l_DimPow2.x = Util::RoundToPowerOfTwo(in_Dim.x);
   l_DimPow2.y = Util::RoundToPowerOfTwo(in_Dim.y);
   m_pImage->Create(in_Dim, PIXELFMT_A8R8G8B8);
   m_iPitchByte = m_pImage->Pitch();

   m_Window.Set(1, 1, in_Dim.x - 3, in_Dim.y - 3);

   // Calculate the desired area
   GRectangle<REAL32> l_Area;
   l_Area.Set(in_Coord - (in_Size * 0.5f), in_Size);

   // Make sure the area does not cross south pole
   if(l_Area.Top() < -90)
   {
      l_Area.Top(l_Area.Top() + l_Area.Top() + 90);
   }

   // Make sure the area does not cross north pole
   if(l_Area.Bottom() > 90)
   {
      l_Area.Top(l_Area.Top() - l_Area.Bottom() + 90);
   }

   m_bCrosses180 = false;
   m_bCrosses180Neg = false;

   if(l_Area.Left() < -180.f)
   {
      m_bCrosses180Neg = true;
   }
   if(l_Area.Right() > 180.f)
   {
      m_bCrosses180 = true;
   }

   gassert(!m_bCrosses180 || !m_bCrosses180Neg, "Atlas area should not cross -180/180 boundary twice");

   // Calculate a scale & offset which will transform map coordinates to buffer coordinates
   m_Scale.x = (REAL32) (in_Dim.x - 1) / in_Size.x;
   m_Scale.y = (REAL32) (in_Dim.y - 1) / -in_Size.y;
   m_Scale.z = 0.f;
   m_Offset.x = -m_Scale.x * (in_Coord.x - 0.5f * in_Size.x);
   m_Offset.y = -m_Scale.y * (in_Coord.y + 0.5f * in_Size.y);
   m_Offset.z = 0.f;

   /*
   m_Offset.x = -in_Coord.x;
   m_Offset.y = -in_Coord.y;
   m_Offset.z = 0.f;
   m_Scale.x = (REAL32) in_Dim.x / in_Size.x;
   m_Scale.y = (REAL32) in_Dim.y / in_Size.y;
   m_Scale.z = 0.f;
   m_Offset *= m_Scale;
   m_Offset.x += (REAL32) in_Dim.x * 0.5f - 0.5f;
   m_Offset.y += (REAL32) in_Dim.y * 0.5f - 0.5f;

   m_Scale.y *= -1;
//   m_Offset.y = -m_Offset.y + (REAL32) in_Dim.y - 1.f;
   m_Offset.y = -m_Offset.y + (REAL32) in_Dim.y;// - 1.f;
*/
   LOG_PROFILER(L"Atlas preparation time");
}

CGFX_Texture_Itf* GAtlasTxtrGenerator::GenerateElevation()
{
   IF_RETURN(!m_bContinue, NULL);

   // Make sure previous generation memory was cleared
   Reset();

   SETUP_PROFILER();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch coast line data
   m_pCoastlineView = g_ClientDAL.Coastlines().OpenView(m_Coord.x, m_Coord.y, 
                                                        m_Size.x, m_Size.y);

   LOG_PROFILER(L"Coast line open view");

   IF_RETURN(!m_bContinue, NULL);

   // Fetch elevation data
   m_pElevView = g_ClientDAL.Elevation().OpenView(m_Coord.x, m_Coord.y, 
                                                  m_Size.x, m_Size.y);

   LOG_PROFILER(L"Elevation open view");

   IF_RETURN(!m_bContinue, NULL);

   GVector2D<REAL32> l_RasterSize = m_Size;
   l_RasterSize.x = l_RasterSize.x * (REAL32) (m_Dim.x - 1) / (REAL32) m_Dim.x;
   l_RasterSize.y = l_RasterSize.y * (REAL32) (m_Dim.y - 1) / (REAL32) m_Dim.y;

   // Elevation
   REAL32* l_pElevBuffer;
   {
      // Allocate the buffer for elevation
      l_pElevBuffer = new REAL32[m_Dim.x * m_Dim.y];

      // Get the elevation data
      m_pElevView->FillRectangularGrid(m_Coord.x, m_Coord.y, l_RasterSize.x, l_RasterSize.y, m_Dim.x, m_Dim.y, l_pElevBuffer);
   }

   LOG_PROFILER(L"Elevation raster");

   if(!m_bContinue)
   {
      delete [] l_pElevBuffer;
      return NULL;
   }

   // Clear previous image
   m_pImage->Clear(&c_CoastOut);

   LOG_PROFILER(L"Elevation clear image");

   IF_RETURN(!m_bContinue, NULL);

   // Draw coastlines
   {
      IF_RETURN(!DrawCoasts(m_Coord, m_Size, 0), NULL);

      if(m_bCrosses180)
      {
         IF_RETURN(!DrawCoasts(m_Coord, m_Size, 360), NULL);
      }
      else if(m_bCrosses180Neg)
      {
         IF_RETURN(!DrawCoasts(m_Coord, m_Size, -360), NULL);
      }

   }

   LOG_PROFILER(L"Raster coasts");

   // Create elevation bump
   REAL32 l_fBumpIntensity = 0.025f / m_Size.y;

   GPixelA8R8G8B8* l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, 0) );
   for(UINT32 x = 0;x < m_Dim.x;x ++)
   {
      *l_pLine = c_Border;
      l_pLine ++;
   }

   for(UINT32 y = 1;y < m_Dim.y - 1;y ++)
   {
      l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, y) );

      REAL32* l_pElevLine = l_pElevBuffer + y * m_Dim.x;
      REAL32* l_pElevPrevLine = l_pElevBuffer + (y + 1) * m_Dim.x;
      REAL32* l_pElevNextLine = l_pElevBuffer + (y - 1) * m_Dim.x;

      *l_pLine = c_Border;
      l_pLine ++;

      for(UINT32 x = 1;x < m_Dim.x - 1;x ++)
      {
         if(l_pLine->A == c_CoastIn.A)
         {
			   // Update normal
            REAL32 du = -0.5f * l_pElevLine[x + -1] + 
                        0.5f * l_pElevLine[x +  1];
            REAL32 dv = -0.5f * l_pElevPrevLine[x] + 
                        0.5f * l_pElevNextLine[x];
            du *= l_fBumpIntensity;
            dv *= l_fBumpIntensity;

            REAL32 l_fLengthInv = RSqrt(du * du + dv * dv + 1.0f);

            // Compute pixel intensity by modulating normal
            REAL32 l_fIntensity = -du * l_fLengthInv * 0.40824829046386301636621401245098f + 
                                 -dv * l_fLengthInv * 0.40824829046386301636621401245098f + 
                                       l_fLengthInv * 0.81649658092772603273242802490196f;

            // Make sure intensity is 0 or higher
            _asm
            {
               mov      eax, dword ptr [l_fIntensity]
               xor      edx, edx
               test     eax, eax
               cmovs    eax, edx
               mov      dword ptr [l_fIntensity], eax
            }

            // Modulate intensity with color
            UINT8 l_iValue = (UINT8) RoundToInt(l_fIntensity * 255.f);

            l_pLine->R = l_iValue;
            l_pLine->G = l_iValue;
            l_pLine->B = l_iValue;
         }
         l_pLine ++;
      }
      *l_pLine = c_Border;
   }

   l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, m_Dim.y - 1) );
   for(UINT32 x = 0;x < m_Dim.x;x ++)
   {
      *l_pLine = c_Border;

      l_pLine++;
   }

   delete [] l_pElevBuffer;

   IF_RETURN(!m_bContinue, NULL);

   LOG_PROFILER(L"Elevation bump");

#ifdef WRITE_GENERATION
   // Create a bitmap with the bump result
   {
      RGBTRIPLE* l_RGBData = new RGBTRIPLE[m_Dim.x * m_Dim.y];
      RGBTRIPLE* l_RGBDataPtr = l_RGBData;
      GPixelA8R8G8B8 l_Color;

      for(UINT32 y = 0, yinv = m_Dim.y - 1;y < m_Dim.y;y ++, yinv --)
      {
         for(UINT32 x = 0;x < m_Dim.x;x ++)
         {
            l_Color = *( (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(x, y) ) );
//            l_RGBDataPtr->rgbtBlue = (UINT8) ( (UINT16) l_Color.B * (UINT16) l_Color.A / 255);
//            l_RGBDataPtr->rgbtGreen = (UINT8) ( (UINT16) l_Color.G * (UINT16) l_Color.A / 255);
//            l_RGBDataPtr->rgbtRed = (UINT8) ( (UINT16) l_Color.R * (UINT16) l_Color.A / 255);
            l_RGBDataPtr->rgbtBlue = l_Color.B;
            l_RGBDataPtr->rgbtGreen = l_Color.G;
            l_RGBDataPtr->rgbtRed = l_Color.R;

            l_RGBDataPtr ++;
         }
      }
      // Open bitmap file
	   HANDLE l_DstFile = CreateFile( (GString(L"ElevationTxtr") + GString(rand() ) + GString(L".bmp") ).c_str(),
										      GENERIC_WRITE,
										      FILE_SHARE_READ,
										      NULL,
										      CREATE_ALWAYS,
										      FILE_ATTRIBUTE_NORMAL,
										      NULL);

      m_pImage->SaveColorBitmap(l_DstFile, l_RGBData, m_Dim.x, m_Dim.y);

      CloseHandle(l_DstFile);
   }
#endif

   CGFX_Texture_Itf* l_pResultTxtr = 
      g_Joshua.Renderer()->TextureManager()->Create_Texture(*m_pImage, 
                                                            false, 
                                                            GString(L"Atas txtr") + GString(g_Joshua.TimeCounter().GetTime() ) );

   LOG_PROFILER(L"Elevation texture creation");

   return l_pResultTxtr;
}

CGFX_Texture_Itf* GAtlasTxtrGenerator::GenerateDetails(bool in_bCreateRivers, bool in_bCreateRoads)
{
   IF_RETURN(!m_bContinue, NULL);

   // Make sure previous generation memory was cleared
   Reset();

   SETUP_PROFILER();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch water data
   m_pWaterView = 
      g_ClientDAL.Water().OpenView(m_Coord.x, m_Coord.y, m_Size.x, m_Size.y);

   LOG_PROFILER(L"Water open view");

   IF_RETURN(!m_bContinue, NULL);

   // Clear previous image
   m_pImage->Clear(&c_DetailOut);

   LOG_PROFILER(L"Elevation clear image");

   IF_RETURN(!m_bContinue, NULL);

   // Draw water & islands
   {
      IF_RETURN(!DrawWaterAndIslands(m_Coord, m_Size, 0), NULL);

      if(m_bCrosses180)
      {
         IF_RETURN(!DrawWaterAndIslands(m_Coord, m_Size, 360), NULL);
      }
      else if(m_bCrosses180Neg)
      {
         IF_RETURN(!DrawWaterAndIslands(m_Coord, m_Size, -360), NULL);
      }
   }

   LOG_PROFILER(L"Raster water & islands");

   IF_RETURN(!m_bContinue, NULL);

   if(in_bCreateRivers)
   {
      // Fetch river data
      m_pRiverView = 
         g_ClientDAL.River().OpenView(m_Coord.x, m_Coord.y, m_Size.x, m_Size.y);

      LOG_PROFILER(L"River open view");

      IF_RETURN(!m_bContinue, NULL);

      // Get the river data
      GMultiFacetList l_Rivers;
      GShape l_RiverShape;
      m_pRiverView->ClipCurvesInRectangle(m_Coord.x, m_Coord.y,
                                          m_Size.x - fabsf(1 / m_Scale.x), m_Size.y - fabsf(1 / m_Scale.y),
                                          l_RiverShape,
                                          l_Rivers);

      IF_RETURN(!m_bContinue, NULL);

      AdjustShape(l_RiverShape, m_bCrosses180, m_bCrosses180Neg);

      IF_RETURN(!m_bContinue, NULL);

      // Scale the positions so it fits inside the current viewport
      for(UINT32 p = 0;p < l_RiverShape.NbPositions();p ++)
      {
         l_RiverShape.Position(p) *= m_Scale;
         l_RiverShape.Position(p) += m_Offset;
      }

      IF_RETURN(!m_bContinue, NULL);

      // Update the river lines
      for(UINT32 i = 0;i < l_Rivers.NbFacetLists();i ++)
      {
         ScanConvertLines<GLineStrip, GImage2D*, DrawRiver, 65536>(l_RiverShape, l_Rivers.Facet_List(i).LineStrip(), m_pImage);
      }

      LOG_PROFILER(L"Raster rivers");

      IF_RETURN(!m_bContinue, NULL);
   }

   if(in_bCreateRoads)
   {
      // Fetch road data
      m_pRoadView = 
         g_ClientDAL.Road().OpenView(m_Coord.x, m_Coord.y, m_Size.x, m_Size.y);

      LOG_PROFILER(L"Road open view");

      IF_RETURN(!m_bContinue, NULL);

      // Get the road data
      GMultiFacetList l_Roads;
      GShape l_RoadShape;
      m_pRoadView->ClipCurvesInRectangle(m_Coord.x, m_Coord.y,
                                         m_Size.x - fabsf(1 / m_Scale.x), m_Size.y - fabsf(1 / m_Scale.y),
                                         l_RoadShape,
                                         l_Roads);

      IF_RETURN(!m_bContinue, NULL);

      AdjustShape(l_RoadShape, m_bCrosses180, m_bCrosses180Neg);

      IF_RETURN(!m_bContinue, NULL);

      // Scale the positions so it fits inside the current viewport
      for(UINT32 p = 0;p < l_RoadShape.NbPositions();p ++)
      {
         l_RoadShape.Position(p) *= m_Scale;
         l_RoadShape.Position(p) += m_Offset;
      }

      IF_RETURN(!m_bContinue, NULL);

      // Update the river lines
      for(UINT32 i = 0;i < l_Roads.NbFacetLists();i ++)
      {
         ScanConvertLines<GLineStrip, GImage2D*, DrawRoad, 65536>(l_RoadShape, l_Roads.Facet_List(i).LineStrip(), m_pImage);
      }

      LOG_PROFILER(L"Raster roads");

      IF_RETURN(!m_bContinue, NULL);
   }

   CGFX_Texture_Itf* l_pResultTxtr = 
      g_Joshua.Renderer()->TextureManager()->Create_Texture(*m_pImage, 
                                                            false, 
                                                            GString(L"Detail txtr") + GString(g_Joshua.TimeCounter().GetTime() ) );

   LOG_PROFILER(L"Detail texture creation");

   return l_pResultTxtr;
}

bool GAtlasTxtrGenerator::GenerateRegions(vector<GRegionTxtr>& out_vRegionTxtrs)
{
   return false;
}

/*
CGFX_Texture_Itf* GAtlasTxtrGenerator::Generate(const GVector2D<REAL32>& in_Coord, 
                                                const GVector2D<REAL32>& in_Size, 
                                                const GVector2D<UINT32>& in_Dim, 
                                                bool in_bCreateDetails,
                                                vector<GRegionTxtr>& out_vRegionTxtrs)
{
   // Make sure previous generation memory was cleared
   Reset();

   // Create the atlas image
   GVector2D<UINT32> l_DimPow2;
   l_DimPow2.x = Util::RoundToPowerOfTwo(in_Dim.x);
   l_DimPow2.y = Util::RoundToPowerOfTwo(in_Dim.y);
   m_pImage->Create(in_Dim, PIXELFMT_A8R8G8B8);
   m_iPitchByte = m_pImage->Pitch();

   // Calculate the desired area
   GRectangle<REAL32> l_Area;
   l_Area.Set(in_Coord - (in_Size * 0.5f), in_Size);

   // Make sure the area does not cross south pole
   if(l_Area.Top() < -90)
   {
      l_Area.Top(l_Area.Top() + l_Area.Top() + 90);
   }

   // Make sure the area does not cross north pole
   if(l_Area.Bottom() > 90)
   {
      l_Area.Top(l_Area.Top() - l_Area.Bottom() + 90);
   }

   bool l_bCrosses180 = false;
   bool l_bCrosses180Neg = false;

   if(l_Area.Left() < -180.f)
   {
      l_bCrosses180Neg = true;
   }
   if(l_Area.Right() > 180.f)
   {
      l_bCrosses180 = true;
   }

   gassert(!l_bCrosses180 || !l_bCrosses180Neg, "Atlas area should not cross -180/180 boundary twice");

   // Calculate a scale & offset which will transform map coordinates to buffer coordinates
   m_Offset.x = -in_Coord.x;
   m_Offset.y = -in_Coord.y;
   m_Offset.z = 0.f;
   m_Scale.x = (REAL32) in_Dim.x / in_Size.x;
   m_Scale.y = (REAL32) in_Dim.y / in_Size.y;
   m_Scale.z = 0.f;
   m_Offset *= m_Scale;
   m_Offset.x += (REAL32) in_Dim.x * 0.5f - 0.5f;
   m_Offset.y += (REAL32) in_Dim.y * 0.5f - 0.5f;

   REAL32 l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation setup time : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch coast line data
   m_pCoastlineView = 
      g_ClientDAL.Coastlines().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Coast line open view : ") + GString(l_StopTime - l_StartTime) );
#endif

   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch region data
   m_pAdminView = 
      g_ClientDAL.Admin().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Admin open view : ") + GString(l_StopTime - l_StartTime) );
#endif

   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch water data
   m_pWaterView = 
      g_ClientDAL.Water().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);


   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Water open view : ") + GString(l_StopTime - l_StartTime) );
#endif

   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Fetch elevation data
   m_pElevView = 
      g_ClientDAL.Elevation().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Elevation open view : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Clear image
   m_pImage->Clear(&c_OceanColor);

   m_Window.Set(0, 0, in_Dim.x - 1, in_Dim.y - 1);

   IF_RETURN(!m_bContinue, NULL);

   // Coastlines, water & islands
   {
      IF_RETURN(!DrawCoasts(in_Coord, in_Size, 0), NULL);
      IF_RETURN(!DrawWaterAndIslands(in_Coord, in_Size, 0), NULL);

      if(l_bCrosses180)
      {
         IF_RETURN(!DrawCoasts(in_Coord, in_Size, 360), NULL);
         IF_RETURN(!DrawWaterAndIslands(in_Coord, in_Size, 360), NULL);
      }
      else if(l_bCrosses180Neg)
      {
         IF_RETURN(!DrawCoasts(in_Coord, in_Size, -360), NULL);
         IF_RETURN(!DrawWaterAndIslands(in_Coord, in_Size, -360), NULL);
      }
   }


   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation raster coasts, water, islands : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   GVector2D<REAL32> l_RasterSize = in_Size;
   l_RasterSize.x = l_RasterSize.x * (REAL32) (in_Dim.x - 1) / (REAL32) in_Dim.x;
   l_RasterSize.y = l_RasterSize.y * (REAL32) (in_Dim.y - 1) / (REAL32) in_Dim.y;

   // Elevation
   REAL32* l_pElevBuffer;
   {
      // Allocate the buffer for elevation
      l_pElevBuffer = new REAL32[in_Dim.x * in_Dim.y];

      // Get the elevation data
      m_pElevView->FillRectangularGrid(in_Coord.x, in_Coord.y, l_RasterSize.x, l_RasterSize.y, in_Dim.x, in_Dim.y, l_pElevBuffer);
   }

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(L"Area dimensions: X:" + GString(in_Dim.x) + L" Y:"+ GString(in_Dim.y));
   g_Joshua.Log(GString(L"Atlas generation raster elevation : ")   + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   if(!m_bContinue)
   {
      delete [] l_pElevBuffer;
      return NULL;
   }

   // Copy information
   for(UINT32 y = 0, yinv = in_Dim.y - 1;y < in_Dim.y;y ++, yinv --)
   {
      GPixelA8R8G8B8* l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, y) );
      for(UINT32 x = 0;x < in_Dim.x;x ++)
      {
         if(l_pLine->A == c_LandColor.A)
         {
            *l_pLine = m_pAtlasPalette[RoundToInt(l_pElevBuffer[yinv * in_Dim.x + x] * c_fPaletteFactor + c_fPaletteOffset) ];
         }
         l_pLine ++;
      }
   }
   IF_RETURN(!m_bContinue, NULL);

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation colorize elevation : ")   + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   // Create elevation bump
   REAL32 l_fBumpIntensity = 0.025f / in_Size.y;

   GPixelA8R8G8B8* l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, 0) );
   for(UINT32 x = 0;x < in_Dim.x;x ++)
   {
      l_pLine->R = l_pLine->R >> 1;
      l_pLine->G = l_pLine->G >> 1;
      l_pLine->B = l_pLine->B >> 1;
      l_pLine ++;
   }
   for(UINT32 y = 1, yinv = in_Dim.y - 2;y < in_Dim.y - 1;y ++, yinv --)
   {
      l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, y) );

      REAL32* l_pElevLine = l_pElevBuffer + yinv * in_Dim.x;
      REAL32* l_pElevPrevLine = l_pElevBuffer + (yinv + 1) * in_Dim.x;
      REAL32* l_pElevNextLine = l_pElevBuffer + (yinv - 1) * in_Dim.x;

      l_pLine->R = l_pLine->R >> 1;
      l_pLine->G = l_pLine->G >> 1;
      l_pLine->B = l_pLine->B >> 1;
      l_pLine ++;

      for(UINT32 x = 1;x < in_Dim.x - 1;x ++)
      {
			// Update normal
         REAL32 du = -0.5f * l_pElevLine[x + -1] + 
                      0.5f * l_pElevLine[x +  1];
         REAL32 dv = -0.5f * l_pElevPrevLine[x] + 
                      0.5f * l_pElevNextLine[x];
         du *= l_fBumpIntensity;
         dv *= l_fBumpIntensity;

         REAL32 l_fLengthInv = RSqrt(du * du + dv * dv + 1.0f);
//         l_pLine->R = (UINT8) RoundToInt( ( -du * l_fLengthInv + 1.0f ) * 127.5f);
//         l_pLine->G = (UINT8) RoundToInt( ( -dv * l_fLengthInv + 1.0f ) * 127.5f);
//         l_pLine->B = (UINT8) RoundToInt( ( 1.f * l_fLengthInv + 1.0f ) * 127.5f);

         // Compute pixel intensity by modulating normal
         REAL32 l_fIntensity = -du * l_fLengthInv * 0.40824829046386301636621401245098f + 
                               -dv * l_fLengthInv * 0.40824829046386301636621401245098f + 
                               l_fLengthInv * 0.81649658092772603273242802490196f;

         // Make sure intensity is 0 or higher
         _asm
         {
            mov      eax, dword ptr [l_fIntensity]
            xor      edx, edx
            test     eax, eax
            cmovs    eax, edx
            mov      dword ptr [l_fIntensity], eax
         }

         // Modulate intensity with color
         l_pLine->R = (UINT8) RoundToInt(l_pLine->R * l_fIntensity);
         l_pLine->G = (UINT8) RoundToInt(l_pLine->G * l_fIntensity);
         l_pLine->B = (UINT8) RoundToInt(l_pLine->B * l_fIntensity);

         l_pLine ++;
      }
      l_pLine->R = l_pLine->R >> 1;
      l_pLine->G = l_pLine->G >> 1;
      l_pLine->B = l_pLine->B >> 1;
   }
   l_pLine = (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(0, in_Dim.y - 1) );
   for(UINT32 x = 0;x < in_Dim.x;x ++)
   {
      l_pLine->R = l_pLine->R >> 1;
      l_pLine->G = l_pLine->G >> 1;
      l_pLine->B = l_pLine->B >> 1;
   }

   delete [] l_pElevBuffer;

   IF_RETURN(!m_bContinue, NULL);

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation create bump with elevation : ")   + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   if(in_bCreateDetails)
   {
      // Fetch river data
      m_pRiverView = 
         g_ClientDAL.River().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);

      l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
      g_Joshua.Log(GString(L"River open view : ") + GString(l_StopTime - l_StartTime) );
#endif
      l_StartTime = l_Timer.GetTime();

      IF_RETURN(!m_bContinue, NULL);

      // Fetch road data
      m_pRoadView = 
         g_ClientDAL.Road().OpenView(in_Coord.x, in_Coord.y, in_Size.x, in_Size.y);

      l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
      g_Joshua.Log(GString(L"Road open view : ") + GString(l_StopTime - l_StartTime) );
#endif
      l_StartTime = l_Timer.GetTime();

      IF_RETURN(!m_bContinue, NULL);

      // Rivers
      {
         // Get the river data
         GMultiFacetList l_Rivers;
         GShape l_RiverShape;
         m_pRiverView->ClipCurvesInRectangle(in_Coord.x, in_Coord.y,
                                             in_Size.x - 1 / m_Scale.x, in_Size.y - 1 / m_Scale.y,
                                             l_RiverShape,
                                             l_Rivers);

         IF_RETURN(!m_bContinue, NULL);

         AdjustShape(l_RiverShape, l_bCrosses180, l_bCrosses180Neg);

         IF_RETURN(!m_bContinue, NULL);

         // Scale the positions so it fits inside the current viewport
         for(UINT32 p = 0;p < l_RiverShape.NbPositions();p ++)
         {
            l_RiverShape.Position(p) *= m_Scale;
            l_RiverShape.Position(p) += m_Offset;
         }

         IF_RETURN(!m_bContinue, NULL);

         // Update the river lines
         for(UINT32 i = 0;i < l_Rivers.NbFacetLists();i ++)
         {
            ScanConvertLines<GLineStrip, GImage2D*, DrawRiver, 65536>(l_RiverShape, l_Rivers.Facet_List(i).LineStrip(), m_pImage);
         }

         IF_RETURN(!m_bContinue, NULL);
      }

      // Roads
      {
         // Get the road data
         GMultiFacetList l_Roads;
         GShape l_RoadShape;
         m_pRoadView->ClipCurvesInRectangle(in_Coord.x, in_Coord.y,
                                            in_Size.x - 1 / m_Scale.x, in_Size.y - 1 / m_Scale.y,
                                            l_RoadShape,
                                            l_Roads);

         IF_RETURN(!m_bContinue, NULL);

         AdjustShape(l_RoadShape, l_bCrosses180, l_bCrosses180Neg);

         IF_RETURN(!m_bContinue, NULL);

         // Scale the positions so it fits inside the current viewport
         for(UINT32 p = 0;p < l_RoadShape.NbPositions();p ++)
         {
            l_RoadShape.Position(p) *= m_Scale;
            l_RoadShape.Position(p) += m_Offset;
         }

         IF_RETURN(!m_bContinue, NULL);

         // Update the river lines
         for(UINT32 i = 0;i < l_Roads.NbFacetLists();i ++)
         {
            ScanConvertLines<GLineStrip, GImage2D*, DrawRoad, 65536>(l_RoadShape, l_Roads.Facet_List(i).LineStrip(), m_pImage);
         }

         IF_RETURN(!m_bContinue, NULL);
      }

      l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
      g_Joshua.Log(GString(L"Atlas generation raster rivers & roads : ") + GString(l_StopTime - l_StartTime) );
#endif
      l_StartTime = l_Timer.GetTime();
   }

   // Administrative boundaries
   UINT32 l_iRegionCount = 0;
   {
      // Draw each region edge only once
      hash_set<UINT32> l_VisitedEdges;
      vector<const Map::GEdge*> l_vCountryEdges;

      m_AdminColor = c_AdminColor;

      const Map::GMultiface* l_pMultiFace = m_pAdminView->FirstMultiface();
      while(l_pMultiFace)
      {
         l_iRegionCount ++;

         for(UINT32 f = 0;f < l_pMultiFace->NbFaces();f ++)
         {
            const Map::GPolygon* l_pPoly = l_pMultiFace->Face(f)->Outer();
            for(UINT32 e = 0;e < l_pPoly->NbEdges();e ++)
            {
               const Map::GEdge* l_pEdge = l_pPoly->Edge(e);
               if(l_VisitedEdges.count(l_pEdge->Id() ) == 0)
               {
                  l_VisitedEdges.insert(l_pEdge->Id() );

                  // Do not draw coast edges
                  if( (l_pEdge->RightFace() != l_pEdge->LeftFace() ) && 
                      (l_pEdge->LeftFace() ) &&
                      (l_pEdge->RightFace() ) )
                  {
                     // Test if edge is spliting regions or countries
                     if(g_ClientDAL.RegionControl(l_pEdge->RightFace()->Multiface()->Id() ).m_iPolitical != 
                        g_ClientDAL.RegionControl(l_pEdge->LeftFace()->Multiface()->Id() ).m_iPolitical )
                     {
                        // Edge is spliting a country, draw it later
                        l_vCountryEdges.push_back(l_pEdge);
                     }
                     else
                     {
                        DrawMapEdge(1.5f, l_pEdge, l_bCrosses180, l_bCrosses180Neg);
                     }
                  }
               }
            }
         }

         l_pMultiFace = m_pAdminView->NextMultiface();
      }

      m_AdminColor = c_CountryColor;
      for(UINT32 i = 0;i < l_vCountryEdges.size();i ++)
      {
         DrawMapEdge(2.5f, l_vCountryEdges[i], l_bCrosses180, l_bCrosses180Neg);
      }
   }

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation draw administrative regions : ")   + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   m_pImage->FlipVertical();

   IF_RETURN(!m_bContinue, NULL);

   UINT32 l_iPitch = m_pImage->Pitch();
   GPixelA8R8G8B8* l_pPixs = (GPixelA8R8G8B8*) m_pImage->Pixels();
   GPixelA8R8G8B8* l_pPixsEnd = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + ( (in_Dim.y - 1) * l_iPitch) );

   for(UINT32 x = 0;x < in_Dim.x;x ++)
   {
      l_pPixs[x].A = 0;
      l_pPixsEnd[x].A = 0;
   }

   l_pPixs = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + l_iPitch);
   l_pPixsEnd = l_pPixs + in_Dim.x - 1;
   for(UINT32 y = 1;y < in_Dim.y - 1;y ++)
   {
      l_pPixs->A = 0;
      l_pPixsEnd->A = 0;

      l_pPixs = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + l_iPitch);
      l_pPixsEnd = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixsEnd + l_iPitch);
   }

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas generation vertical flip : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   IF_RETURN(!m_bContinue, NULL);

   // Add this section if you want to create a bitmap of the result
#if WRITE_GENERATION
   // Create a bitmap with the result
   {
      RGBTRIPLE* l_RGBData = new RGBTRIPLE[in_Dim.x * in_Dim.y];
      RGBTRIPLE* l_RGBDataPtr = l_RGBData;
      GPixelA8R8G8B8 l_Color;

      for(UINT32 y = 0, yinv = in_Dim.y - 1;y < in_Dim.y;y ++, yinv --)
      {
         for(UINT32 x = 0;x < in_Dim.x;x ++)
         {
            l_Color = *( (GPixelA8R8G8B8*) m_pImage->PixelAt(GVector2D<UINT32>(x, y) ) );
            l_RGBDataPtr->rgbtBlue = (UINT8) ( (UINT16) l_Color.B * (UINT16) l_Color.A / 255);
            l_RGBDataPtr->rgbtGreen = (UINT8) ( (UINT16) l_Color.G * (UINT16) l_Color.A / 255);
            l_RGBDataPtr->rgbtRed = (UINT8) ( (UINT16) l_Color.R * (UINT16) l_Color.A / 255);

            l_RGBDataPtr ++;
         }
      }
      // Open bitmap file
	   HANDLE l_DstFile = CreateFile( (GString(L"AtlasTxtr") + GString(rand() ) + GString(L".bmp") ).c_str(),
										      GENERIC_WRITE,
										      FILE_SHARE_READ,
										      NULL,
										      CREATE_ALWAYS,
										      FILE_ATTRIBUTE_NORMAL,
										      NULL);

      m_pImage->SaveColorBitmap(l_DstFile, l_RGBData, in_Dim.x, in_Dim.y);

      CloseHandle(l_DstFile);
   }
#endif

   CGFX_Texture_Itf* l_pResultTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(*m_pImage, false, GString(L"Atas txtr") + GString(g_Joshua.TimeCounter().GetTime() ) );

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas texture creation : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   // Recreate region textures
   {
      GShape l_RegionShape;
      out_vRegionTxtrs.reserve(l_iRegionCount);

      const Map::GMultiface* l_pMultiFace = m_pAdminView->FirstMultiface();
      while(l_pMultiFace)
      {
         // Compute the location of the region texture
         GRegionTxtr l_Info;
         l_Info.m_iID = l_pMultiFace->Id();
         const Map::GBox& l_BBox = l_pMultiFace->BoundingBox();
         GVector2D<REAL32> l_ULCoord(l_BBox.Xmin, l_BBox.Ymin);
         GVector2D<REAL32> l_LRCoord(l_BBox.Xmax, l_BBox.Ymax);

         // Convert the location in pixel coordinates
         GVector2D<REAL32> l_ULPixel = (l_ULCoord * m_Scale) + m_Offset;
         GVector2D<REAL32> l_LRPixel = (l_LRCoord * m_Scale) + m_Offset;

         l_ULPixel.x = floorf(l_ULPixel.x - 1);
         l_ULPixel.y = floorf(l_ULPixel.y - 1);

         l_LRPixel.x = ceilf(l_LRPixel.x + 2);
         l_LRPixel.y = ceilf(l_LRPixel.y + 2);

         // Clip pixel coordinates inside the currently generated texture
         l_ULPixel.x = max(l_ULPixel.x, 0);
         l_ULPixel.y = max(l_ULPixel.y, 0);

         l_LRPixel.x = min(l_LRPixel.x, in_Dim.x);
         l_LRPixel.y = min(l_LRPixel.y, in_Dim.y);

         if( (l_ULPixel.x < l_LRPixel.x) && 
             (l_ULPixel.y < l_LRPixel.y) )
         {
            // Convert pixel coordinate back to map coordinate
            l_Info.m_Coord = (l_ULPixel - m_Offset) / m_Scale;

            // Create the texture
            l_Info.m_pTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(
               PIXELFMT_A8R8G8B8, 
               Util::RoundToPowerOfTwo(RoundToInt(l_LRPixel.x - l_ULPixel.x) ),
               Util::RoundToPowerOfTwo(RoundToInt(l_LRPixel.y - l_ULPixel.y) ),
               false,
               GString(L"RegionTxtr") + 
               GString(l_Info.m_iID) + 
               GString(in_Coord.x) + 
               GString(in_Coord.y) + 
               GString(in_Size.x) + 
               GString(in_Size.y) + 
               GString( (UINT32) in_bCreateDetails) );

            l_Info.m_pTxtr->LockImage(m_pRegionImage);
            m_pRegionImage->Clear();
            m_iRegionPitchByte = m_pRegionImage->Pitch();
            m_RegionWindow.Set(0, 0, m_pRegionImage->Width() - 1, m_pRegionImage->Height() - 1);
            l_Info.m_Size.x = fabsf( ( (REAL32) m_pRegionImage->Width() ) / m_Scale.x);
            l_Info.m_Size.y = fabsf( ( (REAL32) m_pRegionImage->Height() ) / m_Scale.y);

            Map::GMultifaceView* l_pRegionView = 
               g_ClientDAL.Admin().OpenView(l_Info.m_iID);

            // Draw the current region
            IF_RETURN(!DrawRegion(l_pRegionView, in_Coord, in_Size, l_ULPixel, 0), NULL);

            if(l_bCrosses180)
            {
               IF_RETURN(!DrawRegion(l_pRegionView, in_Coord, in_Size, l_ULPixel, 360), NULL);
            }
            else if(l_bCrosses180Neg)
            {
               IF_RETURN(!DrawRegion(l_pRegionView, in_Coord, in_Size, l_ULPixel, -360), NULL);
            }

            g_ClientDAL.Admin().CloseView(l_pRegionView);

            UINT32 l_iPitch = m_pRegionImage->Pitch();
            GPixelA8R8G8B8* l_pPixs = (GPixelA8R8G8B8*) m_pRegionImage->Pixels();
            GPixelA8R8G8B8* l_pPixsEnd = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + ( (m_pRegionImage->Height() - 1) * l_iPitch) );

            for(UINT32 x = 0;x < m_pRegionImage->Width();x ++)
            {
               l_pPixs[x].A = 0;
               l_pPixsEnd[x].A = 0;
            }

            l_pPixs = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + l_iPitch);
            l_pPixsEnd = l_pPixs + m_pRegionImage->Width() - 1;
            for(UINT32 y = 1;y < m_pRegionImage->Height() - 1;y ++)
            {
               l_pPixs->A = 0;
               l_pPixsEnd->A = 0;

               l_pPixs = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixs + l_iPitch);
               l_pPixsEnd = (GPixelA8R8G8B8*) ( (UINT8*) l_pPixsEnd + l_iPitch);
            }

#if WRITE_GENERATION
            // Create a bitmap with the result
            {
               RGBTRIPLE* l_RGBData = new RGBTRIPLE[m_pRegionImage->Width() * m_pRegionImage->Height() ];
               RGBTRIPLE* l_RGBDataPtr = l_RGBData;
               GPixelA8R8G8B8 l_Color;

               for(UINT32 y = 0, yinv = m_pRegionImage->Height() - 1;y < m_pRegionImage->Height();y ++, yinv --)
               {
                  for(UINT32 x = 0;x < m_pRegionImage->Width();x ++)
                  {
                     l_Color = *( (GPixelA8R8G8B8*) m_pRegionImage->PixelAt(GVector2D<UINT32>(x, y) ) );
                     l_RGBDataPtr->rgbtBlue = l_Color.A;
                     l_RGBDataPtr->rgbtGreen = l_Color.A;
                     l_RGBDataPtr->rgbtRed = l_Color.A;

                     l_RGBDataPtr ++;
                  }
               }
               // Open bitmap file
	            HANDLE l_DstFile = CreateFile( (GString(L"RegionTxtr") + GString(l_Info.m_iID) + GString( (UINT32) in_bCreateDetails) + GString(L".bmp") ).c_str(),
										               GENERIC_WRITE,
										               FILE_SHARE_READ,
										               NULL,
										               CREATE_ALWAYS,
										               FILE_ATTRIBUTE_NORMAL,
										               NULL);

               m_pRegionImage->SaveColorBitmap(l_DstFile, l_RGBData, m_pRegionImage->Width(), m_pRegionImage->Height() );

               CloseHandle(l_DstFile);
               SAFE_DELETE_ARRAY(l_RGBData);
            }
#endif
            l_Info.m_pTxtr->Unlock();

            out_vRegionTxtrs.push_back(l_Info);
         }

         l_pMultiFace = m_pAdminView->NextMultiface();
      }
   }

   l_StopTime = l_Timer.GetTime();
#ifdef PROFILE_GENERATION
   g_Joshua.Log(GString(L"Atlas region txtr generation : ") + GString(l_StopTime - l_StartTime) );
#endif
   l_StartTime = l_Timer.GetTime();

   return l_pResultTxtr;
}
*/
void GAtlasTxtrGenerator::Reset()
{
   GTimeCounter l_Timer;
#ifdef PROFILE_GENERATION
   REAL32 l_StartTime = l_Timer.GetTime();
#endif

   if(m_pCoastlineView)
   {
      g_ClientDAL.Coastlines().CloseView(m_pCoastlineView);
      m_pCoastlineView = NULL;
   }
   
   if(m_pAdminView)
   {
      g_ClientDAL.Admin().CloseView(m_pAdminView);
      m_pAdminView = NULL;
   }

   if(m_pWaterView)
   {
      g_ClientDAL.Water().CloseView(m_pWaterView);
      m_pWaterView = NULL;
   }
   
   if(m_pElevView)
   {
      g_ClientDAL.Elevation().CloseView(m_pElevView);
      m_pElevView = NULL;
   }

   if(m_pRiverView)
   {
      g_ClientDAL.River().CloseView(m_pRiverView);
      m_pRiverView = NULL;
   }

   if(m_pRoadView)
   {
      g_ClientDAL.Road().CloseView(m_pRoadView);
      m_pRoadView = NULL;
   }

#ifdef PROFILE_GENERATION
   REAL32 l_StopTime = l_Timer.GetTime();
   g_Joshua.Log(GString(L"Atlas generation memory clear time : ") + GString(l_StopTime - l_StartTime) );
#endif
}

void GAtlasTxtrGenerator::EnableGeneration()
{
   m_bContinue = true;
}

void GAtlasTxtrGenerator::DisableGeneration()
{
   m_bContinue = false;
}

bool GAtlasTxtrGenerator::IsEnabled()
{
   return m_bContinue;
}

bool GAtlasTxtrGenerator::DrawCoasts(const GVector2D<REAL32>& in_Coord, 
                                     const GVector2D<REAL32>& in_Size, 
                                     REAL32 in_fLongitudeAdjust)
{
   // Get the coast line data
   GFaceList<GPolygon> l_OuterCoasts, l_InnerCoasts;
   GShape l_CoastShape;
   m_pCoastlineView->ClipOuterLimitsInRectangle(in_Coord.x + in_fLongitudeAdjust, 
                                                in_Coord.y,
                                                in_Size.x, 
                                                in_Size.y,
                                                l_CoastShape,
                                                l_OuterCoasts,
                                                l_InnerCoasts);

   IF_RETURN(!m_bContinue, false);

   if(in_fLongitudeAdjust)
   {
      AdjustShape(l_CoastShape, in_fLongitudeAdjust);
   }

   IF_RETURN(!m_bContinue, false);

   // Scale the positions so it fits inside the current viewport
   for(UINT32 p = 0;p < l_CoastShape.NbPositions();p ++)
   {
      l_CoastShape.Position(p) *= m_Scale;
      l_CoastShape.Position(p) += m_Offset;
   }

   IF_RETURN(!m_bContinue, false);

   // Scan convert all coast line polygons
   for(UINT32 i = 0;i < l_OuterCoasts.NbFaces();i ++)
   {
      l_OuterCoasts.Face(i).ScanConvertUniform(l_CoastShape, m_Window, SpanLand, m_pImage->Pixels() );
   }

   return m_bContinue;
}

bool GAtlasTxtrGenerator::DrawWaterAndIslands(const GVector2D<REAL32>& in_Coord, 
                                              const GVector2D<REAL32>& in_Size, 
                                              REAL32 in_fLongitudeAdjust)
{
   // Get the water data
   GFaceList<GPolygon> l_Water, l_Island;
   GShape l_WaterShape;
   m_pWaterView->ClipLoneAreasInRectangle(in_Coord.x + in_fLongitudeAdjust, 
                                          in_Coord.y,
                                          in_Size.x, 
                                          in_Size.y,
                                          l_WaterShape,
                                          l_Water,
                                          l_Island);

   IF_RETURN(!m_bContinue, false);

   if(in_fLongitudeAdjust)
   {
      AdjustShape(l_WaterShape, in_fLongitudeAdjust);
   }

   IF_RETURN(!m_bContinue, false);

   // Scale the positions so it fits inside the current viewport
   for(UINT32 p = 0;p < l_WaterShape.NbPositions();p ++)
   {
      l_WaterShape.Position(p) *= m_Scale;
      l_WaterShape.Position(p) += m_Offset;
   }

   IF_RETURN(!m_bContinue, false);

   // Scan convert all water polygons
   for(UINT32 i = 0;i < l_Water.NbFaces();i ++)
   {
      l_Water.Face(i).ScanConvertUniform(l_WaterShape, m_Window, SpanWater, m_pImage->Pixels() );
   }

   IF_RETURN(!m_bContinue, false);

   // Scan convert all island polygons
   for(UINT32 i = 0;i < l_Island.NbFaces();i ++)
   {
      l_Island.Face(i).ScanConvertUniform(l_WaterShape, m_Window, SpanIsland, m_pImage->Pixels() );
   }

   return m_bContinue;
}

bool GAtlasTxtrGenerator::DrawRegion(const Map::GMultifaceView* in_pRegionView, 
                                     const GVector2D<REAL32>& in_Coord, 
                                     const GVector2D<REAL32>& in_Size, 
                                     const GVector2D<REAL32>& in_PixelAdjust,
                                     REAL32 in_fLongitudeAdjust)
{
   // Get the region data
   GFaceList<GPolygon> l_RegionOut, l_RegionIn;
   GShape l_RegionShape;
   in_pRegionView->ClipOuterLimitsInRectangle(in_Coord.x + in_fLongitudeAdjust, 
                                              in_Coord.y,
                                              in_Size.x, 
                                              in_Size.y,
                                              l_RegionShape,
                                              l_RegionOut,
                                              l_RegionIn);

   IF_RETURN(!m_bContinue, false);

   if(in_fLongitudeAdjust)
   {
      AdjustShape(l_RegionShape, in_fLongitudeAdjust);
   }

   IF_RETURN(!m_bContinue, false);

   // Scale the positions so it fits inside the current viewport
   GVector2D<REAL32> l_NewOffset(m_Offset - in_PixelAdjust);
   for(UINT32 p = 0;p < l_RegionShape.NbPositions();p ++)
   {
      l_RegionShape.Position(p) *= m_Scale;
      l_RegionShape.Position(p) += l_NewOffset;
   }

   IF_RETURN(!m_bContinue, false);

   // Scan convert all region outer polygons
   for(UINT32 i = 0;i < l_RegionOut.NbFaces();i ++)
   {
      l_RegionOut.Face(i).ScanConvertUniform(l_RegionShape, m_RegionWindow, SpanRegionIn, m_pRegionImage->Pixels() );
   }

   IF_RETURN(!m_bContinue, false);

   // Scan convert all region inner polygons
   for(UINT32 i = 0;i < l_RegionIn.NbFaces();i ++)
   {
      l_RegionIn.Face(i).ScanConvertUniform(l_RegionShape, m_RegionWindow, SpanRegionOut, m_pRegionImage->Pixels() );
   }

   return m_bContinue;
}

void GAtlasTxtrGenerator::AdjustShape(GShape& in_Shape, REAL32 in_fAdjustValue)
{
   for(UINT32 i = 0;i < in_Shape.NbPositions();i ++)
   {
      in_Shape.Position(i).x += in_fAdjustValue;
   }
}

void GAtlasTxtrGenerator::AdjustShape(GShape& in_Shape, bool in_bCrosses180, bool in_bCrosses180Neg)
{
   if(in_bCrosses180)
   {
      for(UINT32 p = 0;p < in_Shape.NbPositions();p ++)
      {
         if(in_Shape.Position(p).x < 0.f)
         {
            in_Shape.Position(p).x += 360.f;
         }
      }
   }
   else if(in_bCrosses180Neg)
   {
      for(UINT32 p = 0;p < in_Shape.NbPositions();p ++)
      {
         if(in_Shape.Position(p).x > 0.f)
         {
            in_Shape.Position(p).x -= 360.f;
         }
      }
   }
}

void GAtlasTxtrGenerator::DrawMapEdge(REAL32 in_fWidth, 
                                      const Map::GEdge* in_pEdge, 
                                      bool in_bCrosses180, 
                                      bool in_bCrosses180Neg)
{
   GVector2D<REAL32> l_Adjust(0,0);
   if(in_bCrosses180 && (in_pEdge->Vertex(0)->X < 0) )
   {
      l_Adjust.x = 360;
   }
   else if(in_bCrosses180Neg && (in_pEdge->Vertex(0)->X > 0) )
   {
      l_Adjust.x = -360;
   }

   GVector2D<REAL32> l_PrevPoint = *(in_pEdge->Vertex(0) );
   l_PrevPoint += l_Adjust;
   l_PrevPoint *= m_Scale;
   l_PrevPoint += m_Offset;
   for(UINT32 v = 1;v < in_pEdge->NbVertices();v ++)
   {
      GVector2D<REAL32> l_NextPoint = *(in_pEdge->Vertex(v) );
      l_NextPoint += l_Adjust;
      l_NextPoint *= m_Scale;
      l_NextPoint += m_Offset;

      GVector2D<REAL32> l_LineNormal = GVector3D<REAL32>(l_NextPoint - l_PrevPoint).CrossProduct(GVector3D<REAL32>(0,0,1) );
      l_LineNormal *= in_fWidth * RSqrt(l_LineNormal.DotProduct(l_LineNormal) );

      m_ShapeLine.Position(0) = l_PrevPoint + l_LineNormal;
      m_ShapeLine.Position(1) = l_NextPoint + l_LineNormal;
      m_ShapeLine.Position(2) = l_NextPoint - l_LineNormal;
      m_ShapeLine.Position(3) = l_PrevPoint - l_LineNormal;

      m_PolyLine.ScanConvertUniform(m_ShapeLine, m_Window, SpanAdmin, m_pImage->Pixels() );

      l_PrevPoint = l_NextPoint;
   }
}

void GAtlasTxtrGenerator::SpanLand(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = c_CoastIn;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::SpanWater(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = c_DetailWaterIn;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::SpanIsland(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = c_DetailIslandIn;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::SpanAdmin(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = m_AdminColor;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::SpanRegionIn(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iRegionPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = c_RegionColor;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::SpanRegionOut(void* in_pBuf, INT32 y, INT32 xl, INT32 xr)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) ( (UINT8*) in_pBuf + (m_iRegionPitchByte * y) ) + xl;
   for(INT32 x = xl;x <= xr;x ++)
   {
      *l_pBufPtr = c_RegionBlackColor;
      l_pBufPtr ++;
   }
}

void GAtlasTxtrGenerator::DrawRiver(GImage2D* in_pImage, INT32 x, INT32 y)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) in_pImage->PixelAt(GVector2D<UINT32>(x, y) );
   *l_pBufPtr = c_DetailRiverIn;
}

void GAtlasTxtrGenerator::DrawRoad(GImage2D* in_pImage, INT32 x, INT32 y)
{
   GPixelA8R8G8B8* l_pBufPtr = (GPixelA8R8G8B8*) in_pImage->PixelAt(GVector2D<UINT32>(x, y) );
   *l_pBufPtr = c_DetailRoadIn;
}
