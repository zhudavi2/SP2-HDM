/**************************************************************
*
* golem_dll_loader.h
*
* Description
* ===========
*  Describes & implements GDllLoader class.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _WIN32
#error GDllLoader class only on win32 platform
#endif // #ifndef _WIN32

#ifndef _GOLEM_DLLIMPORT_
#define _GOLEM_DLLIMPORT_

#include "golem_types.h"
#include "golem_version.h"

namespace Hector
{

   /*!
   * Describes a pointer to a DLL function that retrieves the version of the DLL
   **/
   typedef bool (*GetInterfaceVersionProc) (GGolemVersion& pVersion);

   /*!
   * Describes a pointer to a DLL function that retrieves the name of the DLL interface
   **/
   typedef const char* (*GetInterfaceNameProc) ();

   /*!
   * Describes a pointer to a DLL function that retrieves the description of the DLL interface
   **/
   typedef const char* (*GetInterfaceDescriptionProc) ();

   /*!
   * Describes a pointer to a DLL function that retrieves the interface object of the DLL
   **/
   typedef void*   (*GetInterfaceObjectProc)();

   /*!
   * This class allows dynamic loading of a DLL which implements four functions
   **/
   class GDllLoader
   {
      HMODULE     m_HModule;
      GString     m_sPath;

      GetInterfaceVersionProc       m_InterfaceVersionProc;
      GetInterfaceNameProc          m_InterfaceNameProc;
      GetInterfaceDescriptionProc   m_InterfaceDescProc;

   public:
      //! Default constructor
      GDllLoader();

      //! Destructor
      ~GDllLoader();

      //! Loads a DLL using the specified path
      bool Load( const GString& in_sPath );

      //! Unloads a previously loaded DLL
      bool Unload();

      //! Retrieves the version of the DLL interface
      bool InterfaceVersion( GGolemVersion& out_Version );

      //! Retrieves the name of the DLL interface
      bool InterfaceName( GString& out_sName );

      //! Retrieves the description of the DLL interface
      bool InterfaceDescription( GString& out_sDesc );

      //! Retrieves an object allocated by the DLL using the specified function name
      void *Object( const GString& in_ObjectFuncName );
   };

}

#endif //#ifndef _GOLEM_DLLIMPORT_