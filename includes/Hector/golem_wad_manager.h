/************************************************
* File: golem_wad_manager.h
* Owner: Mathieu Tremblay
* Description:
*     Classes to create and manage WAD m_Files
*
* History:
* ========
*     23 avril 2003        Creation
*
*************************************************/

#ifndef __GOLEM_WAD__H
#define __GOLEM_WAD__H


#include "golem_file_manager.h"

namespace Hector
{

   /*!
   === GOLEM WAD FORMAT === GOLEM WAD FORMAT === GOLEM WAD FORMAT ===

   Each file has a byte specifying if it is locked or it can be replaced by a file with the same
   name at the same location in a directory tree.


   Here is the description of the WAD file format:

   4 bytes : Number of directories in the wad file including the root directory
   For each directory:
   4 bytes : Nb chars (X) in the directory name
   X bytes : Directory Name (full path ex: /models/air/)
   4 bytes : Nb of m_Files in the directory
   For each File:
   4 bytes : Nb chars (X) in the filename
   X bytes : Filename (with the extension : ex: tank1.3ds)
   8 bytes : Offset (in bytes) indicating the beginning of the file withing the WAD
   8 bytes : Size of the file
   1 byte  : Locked ( != 0)  Unlocked (0)
   */


   /*!
   * Class used to generate WADs, Add_Files to the wad and then call Generate_Wad
   **/

   //! Represents a file in the wad directory
   class GFileEntry
   {
   public:
      GFileEntry()
      {
         m_iSize = 0;
         m_iCalculatedOffset = 0;
         m_bLocked = true;
      }
      //!name into the wad file
      GString m_sWadName;  
      //!name of the corresponding file on the disk
      GString m_sDiskName; 
      INT64   m_iSize;
      INT64   m_iCalculatedOffset;
      bool    m_bLocked;
      GString     m_sName;
      INT64       m_iOffset;
   };

   typedef stdext::hash_map<GString,GFileEntry>  DirectoryFiles;

   class GDirectory
   {
   public:
      GString              m_sName;
      DirectoryFiles       m_Files;
      vector<GDirectory>   m_Subdirs;

      void Clear()
      {
         m_Files.clear();
         for(INT32 i = 0; i < (INT32)m_Subdirs.size() ; i++)
         {
            m_Subdirs[i].Clear();
         }
         m_Subdirs.clear();
      }
   };


   class GWadMaker
   {
   public:
      GWadMaker();
      ~GWadMaker();

      //! Adds the file to the wad with the given name and location in the wad
      bool AddFile(GString in_sFilePath,
         GString in_sWadLocationAndName,
         bool in_bLocked);

      //! Adds a complete directory (recursively) to the WAD, 
      bool AddDirectory(GString in_sDirPath, bool in_bLocked);
      //! Generates the wad file
      bool GenerateWad(GString in_sWadName);

      //! Automatically determine if a file included in a WAD must be extracted to be used
      //bool   AutoMustBeExtracted(GString in_sFilename);


   protected:
   private:



      GDirectory m_Root;
      GString    m_sWadDirectory; //used in add_Directory
      GString    m_sRememberDir;
      //! Number of directories in the wad file, incremented when a directory is created
      //! /bob/roger counts as two because bob and roger must be created
      INT64       m_iHeaderSize;
      INT64      m_iActualOffset;

      //! Count the number of directories in the given directory (recursively)
      INT32   CountDirectories(GDirectory* in_Dir);
      INT32   GetDirectoryNamesSize(GDirectory* in_Dir);
      //! Count the number of m_Files in the given directory (recursively)
      INT32   CountFiles(GDirectory* in_Dir);
      INT32   GetFileNamesSize(GDirectory* in_Dir);

      bool   WriteHeader(FILE* in_Wad,GDirectory* in_Dir,GString in_sCompletePath);
      bool   WriteDirectory(FILE* in_Wad,GDirectory* in_Dir);
      bool   WriteFile(FILE* in_Original,FILE* in_Destination);
   };


   /*!
   * Wad Manager, 
   *  Used to open an existing WAD file and access its content
   **/
   class GWadManager
   {
   public:
      GWadManager();
      ~GWadManager();

      bool LoadWad(GString in_sWadFile);
      bool CloseWad();

      //! Gets a file withing the wad
      bool File(GString in_sFileName, GFile& in_File);

      //! Checks if the given file is locked
      bool IsLocked(GString in_sFileName);

      GDirectory & Root();

   protected:
   private:
      GString m_sWadName;

      //! Adds the file to the wad with the given name and location in the wad
      bool AddFile(GString in_sFilePath, INT64 in_iFileSize, INT64 in_iFileOffset, bool in_bLocked);

      GDirectory m_Root;
      vector<GString> m_TempFiles;

   };


   GString CleanPath(GString in_sTargetString);

}

#endif //__GOLEM_WAD__H
