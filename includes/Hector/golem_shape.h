/**************************************************************
*
* golem_shape.h
*
* Description
* ===========
*  Description & implementation of a class specifying a 
*  geometric shape.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_SHAPE_
#define _GOLEM_SHAPE_

#include "golem_vector.h"
#include "golem_color.h"

namespace Hector
{
   namespace Geometry
   {

      //! Describes the shape of a geometry
      /*!
       * Describes the shape of a geometry
       * Using a GFacetList object with the GShape can describe any primitive
       * All objects references returned are valid until the
       * object is modified. An invalid index returns an invalid reference.
       **/
      class GShape
      {
      public:
         //! Remove all information on this shape
         inline   void  ClearAll()
         {
            // Clear all data
            m_Positions.clear();
            m_Normals.clear();
            m_Colors.clear();
            m_UVCoords.clear();
         }

         //! Get the number of positions in the shape
         inline   UINT32   NbPositions() const
         {
            return (UINT32) m_Positions.size();
         }

         //! Set the number of positions in the shape
         inline   void  NbPositions( UINT32 in_iCount )
         {
            m_Positions.resize(in_iCount);
         }

         //! Set all positions of the shape to the content of the array
         inline   void  SetupPositions( const GVector3D<REAL32>* in_pPositions, UINT32 in_iNbPositions )
         {
            // Make sure buffer is valid
            if(in_pPositions == NULL)
               return;

            // Allocate enough space for new data
            NbPositions(in_iNbPositions);

            // Copy all data
            vector<GVector3D<REAL32> >::iterator l_It;
            for(l_It = m_Positions.begin();l_It != m_Positions.end();++ l_It)
            {
               (*l_It) = *in_pPositions;
               ++ in_pPositions;
            }
         }

         //! Add a single position to the shape
         inline   UINT32   AddPosition( const GVector3D<REAL32>& in_Position )
         {
            m_Positions.push_back( in_Position );
            return (UINT32) m_Positions.size() - 1;
         }

         //! Fill the specified array with all position data of the shape
         inline   void  FillPositionArray( GVector3D<REAL32>* out_pArray ) const
         {
            // Make sure output buffer is valid
            if(out_pArray)
            {
               // Copy all data
               for ( UINT32 i = 0;i < m_Positions.size();++ i )
               {
                  out_pArray[i] = m_Positions[i];
               }
            }
         }

         //! Obtain a position constant reference
         inline   const GVector3D<REAL32>&  Position( UINT32 in_iIndex ) const
         {
            assert(in_iIndex < m_Positions.size() );
            return m_Positions[in_iIndex];
         }

         //! Obtain a position reference
         inline   GVector3D<REAL32>&  Position( UINT32 in_iIndex )
         {
            assert(in_iIndex < m_Positions.size() );
            return m_Positions[in_iIndex];
         }

         //! Get the number of normals in the shape
         inline   UINT32   NbNormals() const
         {
            return (UINT32) m_Normals.size();
         }

         //! Set the number of normals in the shape
         inline   void  NbNormals( UINT32 in_iCount )
         {
            m_Normals.resize(in_iCount);
         }

         //! Set all normals of the shape to the content of the array
         inline   void  SetupNormals( const GVector3D<REAL32>* in_pNormals, UINT32 in_iNbNormal )
         {
            // Make sure buffer is valid
            if(in_pNormals == NULL)
               return;

            // Allocate enough space for new data
            NbNormals(in_iNbNormal);

            // Copy all data
            vector<GVector3D<REAL32> >::iterator l_It;
            for(l_It = m_Normals.begin();l_It != m_Normals.end();++ l_It)
            {
               (*l_It) = *in_pNormals;
               ++ in_pNormals;
            }
         }

         //! Add a single normal to the shape
         inline   UINT32   AddNormal( const GVector3D<REAL32>& in_Normal )
         {
            m_Normals.push_back( in_Normal );
            return (UINT32) m_Normals.size() - 1;
         }

         //! Fill the specified array with all normal data of the shape
         inline   void  FillNormalArray( GVector3D<REAL32>* out_pArray ) const
         {
            // Make sure output buffer is valid
            if(out_pArray)
            {
               // Copy all data
               for( UINT32 i = 0;i < m_Normals.size();++ i )
               {
                  out_pArray[i] = m_Normals[i];
               }
            }
         }

         //! Obtain a normal constant reference
         inline   const GVector3D<REAL32>&  Normal( UINT32 in_iIndex ) const
         {
            assert(in_iIndex < m_Normals.size() );
            return m_Normals[in_iIndex];
         }

         //! Obtain a normal reference
         inline   GVector3D<REAL32>&  Normal( UINT32 in_iIndex )
         {
            assert(in_iIndex < m_Normals.size() );
            return m_Normals[in_iIndex];
         }


         //! Get the number of colors in the shape
         inline   UINT32   NbColors() const
         {
            return (UINT32) m_Colors.size();
         }

         //! Set the number of colors in the shape
         inline   void  NbColors( UINT32 in_iCount )
         {
            m_Colors.resize(in_iCount);
         }

         //! Set all colors of the shape to the content of the float array
         inline   void  SetupColors( const GColorRGBReal* in_pColors, UINT32 in_iNbColor )
         {
            // Make sure buffer is valid
            if(in_pColors == NULL)
               return;

            // Allocate enough space for new data
            NbColors(in_iNbColor);

            // Copy all data
            vector<GColorRGBInt>::iterator l_It;
            for(l_It = m_Colors.begin();l_It != m_Colors.end();++ l_It)
            {
               (*l_It) = (*in_pColors);
               ++ in_pColors;
            }
         }

         //! Set all colors of the shape to the content of the integer array
         inline   void  SetupColors( const GColorRGBInt* in_pColors, UINT32 in_iNbColor )
         {
            // Make sure buffer is valid
            if(in_pColors == NULL)
               return;

            // Allocate enough space for new data
            NbColors(in_iNbColor);

            // Copy all data
            vector<GColorRGBInt>::iterator l_It;
            for(l_It = m_Colors.begin();l_It != m_Colors.end();++ l_It)
            {
               (*l_It) = *in_pColors;
               ++ in_pColors;
            }
         }

         //! Add a single float color to the shape
         inline   UINT32   AddColor( const GColorRGBReal& in_Color )
         {
            m_Colors.push_back(in_Color);
            return (UINT32) m_Colors.size() - 1;
         }

         //! Add a single integer color to the shape
         inline   UINT32   AddColor( const GColorRGBInt& in_Color )
         {
            m_Colors.push_back( in_Color );
            return (UINT32) m_Colors.size() - 1;
         }

         //! Fill the specified array with all float color data of the shape
         inline   void  FillColorArray( GColorRGBReal* out_pColorArray ) const
         {
            // Make sure output buffer is valid
            if(out_pColorArray)
            {
               // Copy all data
               for ( UINT32 i = 0;i < m_Colors.size();++ i )
               {
                  out_pColorArray[i] = m_Colors[i];
               }
            }
         }

         //! Fill the specified array with all integer color data of the shape
         inline   void  FillColorArray( GColorRGBInt* out_pColorArray ) const
         {
            // Make sure output buffer is valid
            if(out_pColorArray)
            {
               // Copy all data
               for ( UINT32 i = 0;i < m_Colors.size();++ i )
               {
                  out_pColorArray[i] = m_Colors[i];
               }
            }
         }

         //! Obtain a color constant reference
         inline   const GColorRGBInt& Color( UINT32 in_iIndex ) const
         {
            assert(in_iIndex < m_Colors.size() );
            return m_Colors[in_iIndex];
         }

         //! Obtain a color reference
         inline   GColorRGBInt& Color( UINT32 in_iIndex )
         {
            assert(in_iIndex < m_Colors.size() );
            return m_Colors[in_iIndex];
         }


         //! Get the number of textures on the shape
         inline   UINT32   NbTxtrs() const
         {
            return (UINT32) m_UVCoords.size();
         }

         //! Set the number of textures on the shape
         inline   void  NbTxtrs(UINT32 in_iNbTxtrs)
         {
            m_UVCoords.resize(in_iNbTxtrs);
         }


         //! Get the number of UV coordinates on the specified texture of the shape
         inline   UINT32   NbUVCoords( UINT32 in_iTxtr ) const
         {
            // Make sure texture Id is valid
            if ( in_iTxtr >= m_UVCoords.size() )
               return 0;

            return (UINT32) m_UVCoords[in_iTxtr].size();
         }

         //! Set the number of UV coordinates on the specified texture of the shape
         inline   void  NbUVCoords( UINT32 in_iTxtr, UINT32 in_iCount )
         {
            // Make sure texture Id is valid
            if ( in_iTxtr >= m_UVCoords.size() )
               return;

            return m_UVCoords[in_iTxtr].resize(in_iCount);
         }

         //! Set all UV coordinates on the specified texture of the shape to the content of the array
         inline   void  SetupUVCoords( UINT32 in_iTxtr, const GVector2D<REAL32>* in_pUVCoords, UINT32 in_iNbUVCoord )
         {
            // Make sure texture Id is valid
            if( in_iTxtr >= m_UVCoords.size() )
               return;

            // Make sure buffer is valid
            if(in_pUVCoords == NULL)
               return;

            // Allocate enough space for new data
            NbUVCoords(in_iTxtr, in_iNbUVCoord);

            // Copy all data
            vector<GVector2D<REAL32> >::iterator l_It;
            for(l_It = m_UVCoords[in_iTxtr].begin();l_It != m_UVCoords[in_iTxtr].end();++ l_It)
            {
               (*l_It) = *in_pUVCoords;
               ++ in_pUVCoords;
            }
         }

         //! Add a single UV coordinates to the specified texture of the shape
         inline   UINT32   AddUVCoord( UINT32 in_iTxtr, const GVector2D<REAL32>& in_UVCoord )
         {
            // Make sure texture Id is valid
            if( in_iTxtr >= m_UVCoords.size() )
               return 0xffffffff;

            m_UVCoords[in_iTxtr].push_back( in_UVCoord );
            return (UINT32) m_UVCoords[in_iTxtr].size() - 1;
         }

         //! Fill the specified array with all UV coordinates data of the specified texture of the shape
         inline   void  FillUVCoordArray( UINT32 in_iTxtr, GVector2D<REAL32>* out_pArray ) const
         {
            // Make sure texture Id is valid
            if( in_iTxtr >= m_UVCoords.size() )
               return;

            // Make sure output buffer is valid
            if(out_pArray)
            {
               // Copy all data
               for ( UINT32 i = 0;i < m_UVCoords[in_iTxtr].size();++ i )
               {
                  out_pArray[i] = m_UVCoords[in_iTxtr][i];
               }
            }
         }

         //! Obtain a UV coordinate constant reference
         inline   const GVector2D<REAL32>&   UVCoord( UINT32 in_iTxtr, UINT32 in_iIndex ) const
         {
            assert(in_iTxtr < m_UVCoords.size() );
            assert(in_iIndex < m_UVCoords[in_iTxtr].size() );
            return m_UVCoords[in_iTxtr][in_iIndex];
         }

         //! Obtain a UV coordinate reference
         inline   GVector2D<REAL32>&   UVCoord( UINT32 in_iTxtr, UINT32 in_iIndex )
         {
            assert(in_iTxtr < m_UVCoords.size() );
            assert(in_iIndex < m_UVCoords[in_iTxtr].size() );
            return m_UVCoords[in_iTxtr][in_iIndex];
         }

      private:
         vector<GVector3D<REAL32> >            m_Positions;
         vector<GVector3D<REAL32> >            m_Normals;
         vector<GColorRGBInt>                   m_Colors;
         vector<vector<GVector2D<REAL32> > >   m_UVCoords;
      };

   }
}

#endif // _GOLEM_SHAPE_