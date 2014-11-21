/**********************************************************

golem_dllimport.h

Owner : Germain Sauve


**********************************************************/

#ifndef _WIN32
#error DLLimport class only on win32 platform
#endif //#ifndef _WIN32

#ifndef _GOLEM_DLLIMPORT_
#define _GOLEM_DLLIMPORT_

#include "golem_types.h"
#include "golem_version.h"

namespace Hector
{

typedef bool (*GetInterfaceVersionProc) (GGolemVersion& pVersion);
typedef const char* (*GetInterfaceNameProc) ();
typedef const char* (*GetInterfaceDescriptionProc) ();
typedef void*   (*GetInterfaceObjectProc)();

class GDllImporter
{
   HMODULE m_HModule;
   char * m_Path;

   GetInterfaceVersionProc       m_InterfaceVersionProc;
   GetInterfaceNameProc          m_InterfaceNameProc;
   GetInterfaceDescriptionProc   m_InterfaceDescProc;
public:
   GDllImporter();
   ~GDllImporter();
   bool Load( const char* pPath );
   bool Free();

   bool InterfaceVersion( GGolemVersion& pVersion );
   const char* InterfaceName();
   const char* InterfaceDescription();
   void *Object( const char* pObjectFuncName );

   HMODULE ModuleHandle();
};

}

#endif //#ifndef _GOLEM_DLLIMPORT_