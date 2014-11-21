#ifndef _MISC_MAP_GLOBALS
#define _MISC_MAP_GLOBALS

using namespace stdext;


namespace Map 
{
   //---------------------------------------------------------
   //  Forward declarations
   //---------------------------------------------------------
   struct GPoint;
   struct GBox;

   //---------------------------------------------------------
   //  Templates
   //---------------------------------------------------------

   //
   // Simple array with no bounds-checking or dynamic insertions/deletions.
   //
   template <class CType>
   struct GArray
   {
      UINT32  NbItems;
      CType*  Items;

      GArray()    { NbItems = 0; Items = NULL; };
      ~GArray()   { Clear(); };

      void Init( UINT32 size )
      {
         Clear();
         NbItems = size;
         if ( NbItems > 0 )
         {
            Items = new CType[ size ];
         }
      };

      void Clear()
      {
         if ( NbItems != 0 )
         {
            delete [] Items;
            NbItems     = 0;
            Items       = NULL;
         }
      };

      inline CType& operator [] ( UINT32 in_iIndex ) const
      {
         return Items[ in_iIndex ];
      };
   };

   //
   // Circular iterator for vector container.
   //
   template <class CType>
   class GCircularIterator
   {
      public:
         GCircularIterator() {};         

         GCircularIterator( const vector<CType>& in_Vector )
         {
            Attach( in_Vector );
         }

         void Attach( const vector<CType>& in_Vector )
         {
            m_pVector         = &in_Vector;
            m_iCurrentIndex   = 0;
         }

         GCircularIterator<CType> operator ++ ( )
         {
            m_iCurrentIndex++;
            if ( m_iCurrentIndex == m_pVector->size() )
            {
               m_iCurrentIndex = 0;
            }
            return *this;
         }

         GCircularIterator<CType> operator ++ ( INT32 )
         {
            GCircularIterator<CType> Copy( *this );            
            ++ ( *this );
            return Copy;
         }

         GCircularIterator<CType> operator -- ( )
         {
            if ( m_iCurrentIndex == 0 )
            {
               m_iCurrentIndex = (UINT32)m_pVector->size() - 1;
            }
            else
            {
               m_iCurrentIndex--;
            }
            return *this;
         }

         GCircularIterator<CType> operator -- ( INT32 )
         {
            GCircularIterator<CType> Copy( *this );
            -- ( *this );
            return *this;
         }

         const CType& operator * ( ) const
         {
            return ( *m_pVector )[ m_iCurrentIndex ];
         }

         const CType* operator -> ( ) const
         {
            return &( *m_pVector )[ m_iCurrentIndex ];
         }

         bool operator == ( const GCircularIterator<CType>& in_Other ) const
         {
            return   this->m_pVector         == in_Other.m_pVector &&
                     this->m_iCurrentIndex   == in_Other.m_iCurrentIndex;
         }

         bool operator != ( const GCircularIterator<CType>& in_Other ) const
         {
            return   this->m_pVector         != in_Other.m_pVector ||
                     this->m_iCurrentIndex   != in_Other.m_iCurrentIndex;
         }

         GCircularIterator<CType> Next() const
         {
            GCircularIterator<CType> ReturnIter = *this;
            return ++ReturnIter;
         }

         GCircularIterator<CType> Previous() const
         {
            GCircularIterator<CType> ReturnIter = *this;
            return --ReturnIter;
         }

         GCircularIterator<CType> NextDistinct() const
         {
            GCircularIterator<CType> ReturnIter = *this;

            // advance to next distinct element
            while ( *( ++ReturnIter ) == **this && ReturnIter != *this );

            return ReturnIter;
         }

         GCircularIterator<CType> PreviousDistinct() const
         {
            GCircularIterator<CType> ReturnIter = *this;

            // advance to previous distinct element
            while ( *( --ReturnIter ) == **this && ReturnIter != *this );

            return ReturnIter;
         }

         GCircularIterator<CType> Index() const
         {
            return m_iCurrentIndex;
         }

         void Rewind() const
         {
            m_iCurrentIndex = 0;
         }

         static GCircularIterator<CType> Begin( const vector<CType>& in_Vector )
         {
            return GCircularIterator<CType>( in_Vector );
         }

      private:
         const vector<CType>*    m_pVector;
         UINT32                  m_iCurrentIndex;
   };

   //
   // Deep cleaning of a vector of pointers.
   //
   template <class CType>
   void DeepCleanVector( vector<CType*>& io_VectorToClean )
   {
      for ( vector<CType*>::iterator i = io_VectorToClean.begin(); i != io_VectorToClean.end(); ++i )
      {
         delete *i;
      }
      io_VectorToClean.clear();
   }

   // 
   // Reverse the order of the elements in a vector
   //
   template <class CType>
   void ReverseOrder( vector<CType>& io_VectorToReverse )
   {
      UINT32 StopIndex = io_VectorToReverse.size() / 2;  // integer division !

      for ( UINT32 i = 0; i < StopIndex; ++i )
      {
         swap( *( io_VectorToReverse.begin() + i ), *( io_VectorToReverse.rbegin() + i ) );
      }
   }

   //---------------------------------------------------------
   //  Constants
   //---------------------------------------------------------

   const UINT8         MAX_UINT8   = 255;
   const REAL64        PI          = 3.14159265;


   //---------------------------------------------------------
   //  Types
   //---------------------------------------------------------

   typedef UINT32          ID;
   typedef GArray<ID>      GSpatialIndexCell;   // contents of an index cell in a vector map
   typedef vector<GPoint>  GPointList;          // dynamic array of points


   //---------------------------------------------------------
   //  Enumerations
   //---------------------------------------------------------

   enum ETraverseDirection    // to specify a traverse direction for a container of points
   {
      FORWARD,
      REVERSE,
   };

   enum EOrientation          // to qualitatively describe the angle between two vectors
   {
      CLOCKWISE,
      COUNTERCLOCKWISE,
      COLLINEAR,
   };

   enum EFeatureType          // to describe contents of a point list
   {
      POLYGON,
      CURVE,
   };


   //---------------------------------------------------------
   //  Structures
   //---------------------------------------------------------

   struct GPixelCoord      // position (discrete) of a pixel, origin is upper left corner of map
   { 
      UINT32 Row;
      UINT32 Col;

      GPixelCoord();
      GPixelCoord( UINT32 in_iRow, UINT32 in_iCol );

      void Set( UINT32 in_iRow, UINT32 in_iCol );

      bool        operator == ( const GPixelCoord& in_Other ) const;
      bool        operator != ( const GPixelCoord& in_Other ) const;
      bool        operator <  ( const GPixelCoord& in_Other ) const;
      GPixelCoord operator +  ( const GPixelCoord& in_Other ) const;
      GPixelCoord operator -  ( const GPixelCoord& in_Other ) const;

      operator size_t() const;
   };

   struct GRasterCoord     // position (continuous) in pixel units, origin is upper left corner of map
   {
      REAL32 VPos;
      REAL32 HPos;

      GRasterCoord();
      GRasterCoord( REAL32 in_VPos, REAL32 in_HPos );

      void Set( REAL32 in_VPos, REAL32 in_HPos );

      bool operator == ( const GRasterCoord& in_Other ) const;
      bool operator != ( const GRasterCoord& in_Other ) const;
   };

   struct GPoint       // position (continuous) in geographic units
   {
      REAL32 X;
      REAL32 Y;

      GPoint();
      GPoint( REAL32 in_fX, REAL32 in_fY );
      GPoint( GVector2D<REAL32> in_Value )
      {
         X = in_Value.x;
         Y = in_Value.y;
      }

      void Set( REAL32 in_fX, REAL32 in_fY );

      bool operator == ( const GPoint& in_Other ) const;
      bool operator != ( const GPoint& in_Other ) const;
      bool operator <  ( const GPoint& in_Other ) const;

      operator const GVector2D<REAL32>& () const
      {
         return *( (GVector2D<REAL32>*) this);
      }
   };

   struct GSequenceRLE     // unit of RLE encoded data
   {
      UINT8 Length;
      UINT8 Value;
   };

   struct GLineSegment    // line segment from point_1 to point_2
   {
      GPoint Point1;
      GPoint Point2;

      GLineSegment();
      GLineSegment( const GPoint& in_Point1, const GPoint& in_Point2 );

      void Set( const GPoint& in_Point1, const GPoint& in_Point2 );
      bool Contains( const GPoint& in_Point ) const;
      bool Crosses( const GLineSegment& in_OtherSegment ) const;
      bool IsHorizontal() const;
      bool IsVertical() const;
      GBox BoundingBox() const;
   };

   struct GBox     // rectangular box
   {
      REAL32 Xmin;
      REAL32 Xmax;
      REAL32 Ymin;
      REAL32 Ymax;

      GBox();
      GBox( REAL32 in_Xmin, REAL32 in_Xmax, REAL32 in_Ymin, REAL32 in_Ymax );
      GBox( const GPoint& in_Point1, const GPoint& in_Point2 );

      void Set( REAL32 in_Xmin, REAL32 in_Xmax, REAL32 in_Ymin, REAL32 in_Ymax );

      REAL32  Width() const;
      REAL32  Height() const;
      bool    Contains( const GPoint& in_Point ) const;
      bool    Overlaps( const GBox& in_Other ) const;

      GPoint  UpperLeftCorner() const;
      GPoint  UpperRightCorner() const;
      GPoint  LowerLeftCorner() const;
      GPoint  LowerRightCorner() const;

      bool operator == ( const GBox& in_Other ) const;
      bool operator != ( const GBox& in_Other ) const;
   };

   struct GLine    // line described by y = slope * x + y_intercept, or x = x_intercept if vertical
   {
      REAL32  Slope;
      REAL32  YIntercept;
      bool    Vertical;
      REAL32  VerticalX;

      GLine();
      GLine( REAL32 in_fSlope, REAL32 in_fYIntercept, bool in_bVertical, REAL32 in_fVerticalX );
      GLine( const GPoint& in_Point1, const GPoint& in_Point2 );
      GLine( const GLineSegment& in_Segment );

      void Set( REAL32 in_fSlope, REAL32 in_fYIntercept, bool in_bVertical, REAL32 in_fVerticalX );

      bool Contains( const GPoint& in_Point ) const;
      REAL32 GetY( REAL32 in_fX ) const;
      REAL32 GetX( REAL32 in_fY ) const;
   };

   struct GCellPosition    // position of an index cell in a vector map
   {
      UINT32 Row;
      UINT32 Col;

      void Set( UINT32 in_iRow, UINT32 in_iCol );

      GCellPosition();
      GCellPosition( UINT32 in_iRow, UINT32 in_iCol );
   };

   struct GSpatialIndex
   {
      UINT32                      NbCellRows;
      UINT32                      NbCellCols;
      GArray<GSpatialIndexCell>   Cells;

      GSpatialIndex();

      GSpatialIndexCell& operator [] ( const GCellPosition& in_CellPosition ) const;
      void Clear();
   };

   //---------------------------------------------------------
   //  Misc functions
   //---------------------------------------------------------

   // find closest integer
   REAL32 Round( REAL32 in_fParam );

   // determine if number is between 2 bounding numbers
   bool NumberIsBetween( REAL32 in_fNumber, REAL32 in_fBoundary1, REAL32 in_fBoundary2 );

   // twice the signed area of triangle ( in_Point1, in_Point2, in_Point3 )
   REAL32 SignedArea( const GPoint& in_Point1, const GPoint& in_Point2, const GPoint& in_Point3 );

   // orientation of angle define by points ( in_Point1, in_Point2, in_Point3 )
   EOrientation Orientation( const GPoint& in_Point1, const GPoint& in_Point2, const GPoint& in_Point3 );

   // determine intersection point between two lines A and B described by points A1, A2, B1 and B2
   // NOTE: function fails if lines are parallel
   GPoint LineIntersection( const GPoint& A1, const GPoint& A2, const GPoint& B1, const GPoint& B2 );

}

#endif