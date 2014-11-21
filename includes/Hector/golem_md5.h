//SWIG-EXPORT 1
/********************************************************

golem_md5.h


Owner: Mathieu Tremblay

MD5 signature class.
Generates a 128 bit signature that fits any type of data.


history:


*********************************************************/
#ifndef MD5CLASS_H
#define	MD5CLASS_H

#include "golem_types.h"

namespace Hector
{

   struct GMD5_CTX 
   {
      UINT32 buf[4];
      UINT32 bits[2];
      unsigned char in1[64];
   };

   struct GMD5Signature
   {
      UINT8 m_iElements[16];
   };

   class GMD5
   {
   public:
      GMD5();
      ~GMD5();

      char* Generate_Signature(void* source,UINT32 size);
      bool  Compare_Signature(char* sig1,char* sig2);

      void GenerateSignature(void* in_pSource, UINT32 in_iSize, GMD5Signature* out_pSignature);
      bool CompareSignatures(const GMD5Signature& in_Sign1, const GMD5Signature& in_Sign2) const;

      GString SignatureToText(const GMD5Signature& in_Signature);

   private:
      void Init(GMD5_CTX *ctx);
      void Update(GMD5_CTX *ctx, unsigned char *buf, unsigned len);
      void Finalize(unsigned char digest[16], GMD5_CTX *ctx);
      void Transform(UINT32 buf[4], UINT32 in[16]);
   };

}

#endif
