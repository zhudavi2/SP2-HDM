#pragma warning (disable : 4786) // Debug info not generated (longer that 256 chars...)
#pragma warning (disable : 4100) // Unreferenced Formal Parameter
#pragma warning (disable : 4511) // Copy Constructor could not be generated
#pragma warning (disable : 4512) // Assignement operator could not be generated

#ifndef UNICODE
#   define UNICODE
#endif
#ifndef _UNICODE
#   define _UNICODE
#endif
#ifndef WINVER
#   define WINVER 0x0501 //win xp
#endif
#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501 //win xp
#endif
#ifndef _WIN32_IE
#   define _WIN32_IE 0x0501 //IE 5.01
#endif
#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0501 //win xp
#endif

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <windowsX.h>
#include <memory.h>
#include <tchar.h>
#include <shlwapi.h>
#include <time.h>
#include <math.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <process.h>
#include <assert.h>
#include <crtdbg.h>

#define _DEFINE_DEPRECATED_HASH_CLASSES 0

#pragma warning (push)
#pragma warning (disable : 4701) // local variable  may be used without having been initialized
#pragma warning (disable : 4702) // Unreacheable code

#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <hash_map>
#include <hash_set>
#include <limits>
#include <regex>
#include <sstream>

#pragma warning (pop)

using namespace std;
#include "includes/Hector/hector.h"

#ifdef INCLUDE_DATABASE
    #include "includes/Database/database.h"
   using namespace DB;
#endif