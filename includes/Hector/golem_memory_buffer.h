/*****************************************************************

golem_memory_buffer.h

Owner: Germain Sauve

*****************************************************************/

#ifndef _GOLEM_MEMORY_BUFFER_
#define _GOLEM_MEMORY_BUFFER_

namespace Hector
{

   class GMemoryBuffer
   {
      UINT8*  m_Buffer;
      UINT32  m_BufferSize;
      UINT32  m_PageSize;
   public:
      GMemoryBuffer();
      ~GMemoryBuffer();

      UINT8*  Buffer();
      bool    Set_Size( UINT32 pSize );
      bool    Allocate( UINT32 pSize );
      bool    Data( void *pData, UINT32 pSize );
      bool    CopyTo( void *pDest, UINT32 pSize = 0 );
      bool    Clear();
      UINT32  Size();
   };

}

#endif //#ifndef _GOLEM_MEMORY_BUFFER_