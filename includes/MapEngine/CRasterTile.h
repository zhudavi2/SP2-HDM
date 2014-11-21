#ifndef _CLASS_RASTER_TILE
#define _CLASS_RASTER_TILE


namespace Map 
{

   /*!********************************************************
   GRasterTile

   Decompressed in-memory rectangular array of data.
   **********************************************************/

   class GRasterTile
   {
   public:
      GRasterTile( const GPixelCoord& in_Position, UINT32 in_iNumRows, UINT32 in_iNumCols );
      ~GRasterTile();

      void ReadFromFile( GFile &in_File);
      void IncrementRefCount();
      void DecrementRefCount();

      const UINT8* operator [] ( UINT32 in_iRow ) const;       // allows read access to pixels: e.g. tile[row][col]

      UINT8 Pixel( const GPixelCoord& in_PixelCoord ) const;   // pixel coord is relative to this->m_Position
      UINT8 PixelAbsolute( const GPixelCoord& in_PixelCoord ) const;    // pixel coord is relative to 
                                                                        // origin of GRasterMap
      UINT32 NumRows() const;
      UINT32 NumCols() const;
      const GPixelCoord& Position() const;
      UINT32 RefCount() const;

   private:
      GPixelCoord     m_Position;     // upper left corner position in pixels relative to origin of GRasterMap
      UINT32          m_iNumRows;
      UINT32          m_iNumCols;
      UINT8*          m_pPixels;      // rectangular buffer of data
      UINT32          m_iRefCount;    // number of views that contain this tile
   };

}

#endif