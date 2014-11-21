/**************************************************************
*
* SDX_WaveFile.h
*
* Description
* ===========
* This file contain the implementation to access and read a wave file.  
*
*
* Creator
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes Guards
#ifndef _SDX_WaveFile_h
#define _SDX_WaveFile_h


// Includes
#include "../golem_sfx_waveFile_itf.h"

class CSDX_WaveFile : public CSFX_WaveFile_Itf
{
private:

   //! MM I/O handle for the WAVE
   HMMIO         m_hmmio;       
   //! Multimedia RIFF chunk
   MMCKINFO      m_ck;          

   //! Use in opening a WAVE file
   MMCKINFO      m_ckRiff;      

   //! Reset the file
   bool ResetFile(void);

   //! Read the header 
   bool ReadMMIO(void);

public:

   //! Constructor and Destructor
   CSDX_WaveFile();
   ~CSDX_WaveFile();


   //! Open the given file, return true if sucess,
   bool Open( const GString& in_pFileName);

   //! Close opened file, 
   void Close();

   // Read in in_pBuffer, in_iSizeToRead byte to read, and put in out_iSizeRed the total byte red. Return false if it didn't succed.
   bool Read( INT8* in_pBuffer, const UINT32 in_iSizeToRead, UINT32& out_iSizeRed );

   //!Implement the release interface from sfx_obj_itf;
   bool Release();
};

#endif //_SDX_WaveFile_h

