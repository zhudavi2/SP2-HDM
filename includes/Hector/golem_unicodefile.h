
#ifndef _GOLEM_UNICODE_FILE_H
#define _GOLEM_UNICODE_FILE_H

#include "golem_string.h"

namespace Hector
{

   /************************************
   * GUnicodeFile class
   *   Simple unicode file class that reads
   *   an Unicode file into 1 big UnicodeString   
   ************************************/
   class GUnicodeFile
   {
   public:
      GUnicodeFile();
      ~GUnicodeFile();

      //1 shot load file
      GString LoadFile(const GString & filename);

      //1 shot write file
      bool WriteFile(const GString &, const GString & data, bool Overwrite);

   };

}

#endif
