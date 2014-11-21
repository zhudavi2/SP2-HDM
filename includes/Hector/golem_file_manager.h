/************************************************
* File: golem_file_manager.h
* Owner: Mathieu Tremblay
* Description:
*		GFile and GModManager
*		To manage the files of a mod including WAD and Directories.
* History:
* ========	
*	25 april 2003: Creation
*************************************************/

#ifndef _GFile_H_
#define _GFile_H_

namespace Hector
{

   enum SEEK_TYPE
   {
      e_SEEK_SET = 0,
      e_SEEK_CUR,
      e_SEEK_END

   };

   /*!
   * Represents a file inside a WAD or directly on the disk
   **/
   class GFile
   {
      friend class GWadManager;
      friend class GFileManager;
   public:

      GFile();
      ~GFile();	

      //! Reads a file and returns the number of bytes read
      INT32 Read(void*  buffer,INT32 count);
      INT32 Seek(SEEK_TYPE type,INT32 offset);

      GString Name();
      bool    Open(); 
      bool    Close();

      INT64   Size();
      INT64   Offset();

      //! Return true if file pointer is at end
      bool    EndOfFile();

      //! Extracts the file from the .WAD or Itself and returns its new name
      bool Extract(GString &filePath);

      //! Extracts the file from the .WAD to the given name and path
      bool Extract_As(GString destination_name_and_path);

      bool Unextract();

   protected:
   private:
      GFile(INT64 size,GString name,GString disk_name,INT64 offset_beginning);
      GString m_sName;
      GString m_disk_name;
      FILE*   m_filePtr;
      INT64   m_currentOffset;
      INT64   m_size;
      INT64   m_file_beginning; //Offset at which the file begins

      GString m_tempFilename;
   };



   struct GModContext
   {
      GString                    m_rootDirectory;
      GWadManager*               m_wman;
   };

   class GWadManager;
   class GDirectory;


   /*!
   * GFileManager
   *
   *	Manages the files of the WAD and the directory that is specified
   *	 
   *  It must be initialized with a "MOD" 
   *
   *	When a modder needs a file, it asks the GFileManager for the file 
   *  (with the File function).  The GFileManager then checks the WAD if the file exists in the wad.
   *	If so, it checks if it is locked.  If it is locked, it returns the GFile corresponding to the WAD
   *	file.  If it is not locked, it checks at the same location into the directory it has been initialized
   *	with to see if there is a file with the same name present.  If so, the GFile corresponding to the 
   *	directory file is returned, else, it returns the GFile of the WAD File.
   **/
   class GFileManager
   {
   public:
      GFileManager();
      ~GFileManager();

      //! Initializes everything
      bool Initialize(list<GString> &in_ListOfSearchPath);

      //! Gets a file from the OS file system
      static bool FileOnDisk(GString in_sPath, GFile& out_File);

      //! Gets a file either from the WAD or the Directory (see class comment)
      bool File(GString file_path_name,GFile& file);

      //! Gets a file by ressource ID
      bool FileByRessourceId(UINT32 in_iRessourceId, GFile& out_File);

      //! Closes the file manager
      bool Close();
      
      
   protected:
   private:

      list<GModContext>                   m_ListOfModContext;
      vector<const GString>               m_vsResourcesFiles;
      stdext::hash_map<UINT32 , GFile>    m_vRessourcesFilesById;


      void BuildRessourcesFileVector(void);
      UINT32 ExtractRessourceIdFromFileName(GString& in_sFileName);
      vector<const GString> FindFile(GString in_sFile,GModContext &in_ModContext);
      void RecursiveFindFile(const GString& in_sFile, vector<const GString> & in_vFiles, GDirectory & in_Directory, const GString& in_sCurrentDir,GModContext &in_ModContext);
      void RecursiveFindFileInDirectory(const GString& in_sFile, const GString& in_sCurrentDir, vector<const GString> & io_vFiles,GModContext &in_ModContext);
   };

}

#endif //_GFile_H_
