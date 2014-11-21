/**************************************************************
*
* golem_time.h
*
* Description
* ===========
*  Time & timer classes description & implementation
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_TIME_
#define _GOLEM_TIME_

#include "golem_types.h"
#include "golem_utils.h"

namespace Hector
{

   //! Uses performance counters to tell elapsed time since application startup
   class GTimeCounter
   {
   private:
      REAL64	m_fTimerResolution;
      UINT64	m_iTimerStart;
      UINT32	m_iMMTimerStart;
      bool	   m_bUsePerformanceCounter;

   public:

      //! Reset time count
      GTimeCounter();

      //! Get time since object creation
      REAL32 GetTime() const;

      //! Get time since object creation with more precision
      REAL64 GetPrecisionTime() const;

   };

}

#endif //#ifndef _GOLEM_TIME_