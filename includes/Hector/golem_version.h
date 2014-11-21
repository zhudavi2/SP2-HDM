/************************************************************

golem_verison.h

Owner : Germain Sauve

************************************************************/

#ifndef _GOLEM_VERSION_
#define _GOLEM_VERSION_

#include "golem_types.h"

namespace Hector
{

   class GGolemVersion
   {
   public:
      UINT16 m_Major;
      UINT16 m_Minor;
      UINT16 m_Sub;
      UINT16 m_Revision;

      GGolemVersion();
   };

}

#endif //#ifndef _GOLEM_VERSION_