/**************************************************************
*
* golem_callback_handler_itf.h
*
* Description
* ===========
*  A console interface that can log information and receive 
*  commands.
*
* Owner
* =====
*   Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SERIALIZABLE_
#define _GOLEM_SERIALIZABLE_

namespace Hector
{
   class GSerializable
   {
   public:
      virtual bool Serialize(GIBuffer&) const = 0;
      virtual bool Unserialize(GOBuffer&) = 0;
   };
}

#endif
