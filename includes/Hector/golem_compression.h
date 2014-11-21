/**************************************************************
*
* golem_compression.h
*
* Description
* ===========
*   Compression tools
*
* Owner
* =====
*  Mathieu Tremblay, François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COMPRESSION_H__
#define _GOLEM_COMPRESSION_H__

/********************
ZLib basic functions
compress() and uncompress()
*******************/
#include "golem_types.h"
#include "zlib/zlib.h"

namespace Hector
{

   //! Class used for data compression
   /*!
   * Data compression class using the ZLib to compress and uncompress
   * data.
   **/
   class GDataCompressor
   {
   public:
      //! Compress data into a buffer
      GSPtrUINT8 Compress(const UINT8 * in_pSource, INT32 in_iSourceSize, INT32& out_iCompressedDataSize);

      //! Uncompress a compressed data buffer
      GSPtrUINT8 Uncompress(const UINT8 * in_pSource, INT32 in_iSourceSize, INT32& out_iUncompressedDataSize);
   };

}

#endif