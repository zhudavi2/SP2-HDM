#ifndef _CLASS_INDEXED_INPUT_FILE_STREAM
#define _CLASS_INDEXED_INPUT_FILE_STREAM


namespace Map 
{

   /*!********************************************************
   GIndexedInputFileStream

   Provides input access to arbitrary item in binary file
   containing an ordered collection of objects.
   **********************************************************/

   class GIndexedInputFileStream : public GFile
   {
   public:
      GIndexedInputFileStream();
      ~GIndexedInputFileStream();

      void InitFileManager(GFileManager* in_pFileManager);


      // open data file and read associated index file
      bool OpenWithIndex( const GString& in_sDataFileName, const GString& in_sIndexFileName );

      // close stream and erase offset index (overrides ifstream::close)
      void close();

      // position stream at beginning of item item_number (returns false if item can't be reached)
      bool GoToItem( UINT32 in_iItemNumber );

      // return total number of items available
      UINT32 NbItems() const;

   private:
      GArray<UINT32> m_OffsetIndex;
      GFileManager   *m_pFileManager;
      GFile          m_FileStream;

      // read offset index in file
      bool LoadIndex( const GString& in_sIndexFileName );
   };

}

#endif