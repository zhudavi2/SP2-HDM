/**************************************************************
*
* golem_sfx_waveFile_itf.h
*
* Description
* ===========
* This file contain the interface to a wave file.  
*
*
* Creator
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef	_GOLEM_SFX_WAVEFILE_ITF_
#define	_GOLEM_SFX_WAVEFILE_ITF_

#include "golem_sfx_obj_itf.h"

class CSFX_WaveFile_Itf : public CSFX_Obj_Itf
{

public:
   //! Open the given file, return true if sucess,
   virtual bool Open( const GString& in_pFileName) = 0;

   //! Close opened file, 
   virtual void Close() = 0;

   //! Read in in_pBuffer, in_iSizeToRead byte to read, and put in out_iSizeRed the total byte red. Return false if it didn't succed.
   virtual bool Read( INT8* in_pBuffer, const UINT32 in_iSizeToRead, UINT32& out_iSizeRed ) = 0;

};
   

#endif // _GOLEM_SFX_WAVEFILE_ITF_
