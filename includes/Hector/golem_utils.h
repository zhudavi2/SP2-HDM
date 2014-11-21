//SWIG-EXPORT 1
/***********************

golem_utils.h

Various utilities, and useful functions

Owner : Mathieu Tremblay, Germain Sauve

************************/

#ifndef _GOLEM_UTILS_
#define _GOLEM_UTILS_

#include "golem_string.h"
#include "golem_types.h"

namespace Hector
{
   namespace Util
   {

      /********************
      * Get a timestamp()
      *********************/
      GString TimeStamp();


      //! Replaces end lines (textual \n with new end lines)
      GString ReplaceEndLinesByRealEndLines(const GString& in_String);


      /********************
      *  Sleep(sec) : Hector sleep function
      *********************/
      void Sleep(REAL64 seconds);



      /****************************
      * StartThread --- démarre une fonction 
      *  (dont le pointeur est passé en paramètre)
      *  dans un thread.
      ****************************/
      const    UINT32 ERROR_VALUE = 666666666;      
      int             StartThread(LPTHREAD_START_ROUTINE ptr,void* arg);
      bool            StopThread(UINT32 threadHandle);


      //Byte swapping functions
      double BiteSwap64( double value);
      WORD BiteSwap16(WORD w);
      DWORD BiteSwap32(DWORD w);
      WORD GetBigWord(FILE *fp);
      WORD GetLittleWord(FILE *fp);
      DWORD GetBigDoubleWord(FILE *fp);
      DWORD GetLittleDoubleWord(FILE *fp);
      void PutBigWord(WORD w, FILE *fp);
      void PutLittleWord(WORD w, FILE *fp);
      void PutBigDoubleWord(DWORD dw, FILE *fp);
      void PutLittleDoubleWord(DWORD dw, FILE *fp);

      //Macro to safely delete a pointer
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete [](p); (p)=NULL; } }
#define IF_RETURN(in_Cond,in_Return){ if(in_Cond) return in_Return; }
#define IF_RETURN_ELSE(in_Cond,in_ReturnIf,in_ReturnElse){ if(in_Cond){return in_ReturnIf;}else{return in_ReturnElse;} }

      /*************************************
      Util::BitTest
      Tests if a given bit is true or false

      Params : 
      BIT_ID:  Id of the bit to test, 
      1 being the 1st bit 
      (least significant)
      BIT_ARRAY: Array of bits to test

      Returns:
      True  if bit is high (1)
      False if bit is low  (0)
      *************************************/
      inline bool BitTest(UINT8 BIT_ID,UINT64 BITARRAY)
      {
         if(BIT_ID <= 0)
         {
            return false;
         }
         UINT64 comparator;
         comparator = (UINT64)pow(2.0,(BIT_ID-1));
         UINT64 result = (BITARRAY & comparator);
         if(!result)
            return false;
         else
            return true;        
      }

      /******************************************
      * Util::SetBit()
      *      Set a bit x to the value given in an
      *      array
      *
      *
      *  Params:
      *      id : Id of the bit to test, 
      *             1 being the 1st bit 
      *            (least significant)
      *      value: Value to set the bit to :
      *               true : 1
      *               false: 0
      *      array: array in which we want to set the bit
      *
      *  Returns true in case of success
      ******************************************/
      inline bool SetBit(UINT8 id,bool value,UINT64 &array)
      {    

         //64 bit array maximum, cannot set the 65fth bit
         if(id > 64)
            return false;

         UINT64 comparator = (UINT64)pow(2.0,(id-1))*value;

         array = array | comparator;

         return true;
      }


      /*****************************
      ReportError()
      Function to centralize the report error functions

      Params: 
      modulename : the name of the module where the error occured
      errormsg   : The error msg
      *****************************/
      void ReportError(const GString & modulename,const GString & errormsg,const GString & filename,UINT32 linenb);

      /***************************************
      * CreateDirectory()
      *      Creates the specified directory
      ***************************************/
      bool  CreateDirectory(const GString & path);

      /*!
       * Round the specified value the the closest higher power of two
       * @param   in_iValue    : Value to round
       * @return  Rounded value
       **/
      UINT32 RoundToPowerOfTwo(UINT32 in_iValue);

      template <class T>
      void GetSortedVectorIndices(const vector<T> & in_Vector, vector<UINT32> & out_Vector)
      {
         multimap<T, UINT32> l_SortedMap;

         for (vector<T>::size_type i = 0; i != in_Vector.size(); i++)
         {
            l_SortedMap.insert(make_pair(in_Vector[i], i));
         }

         out_Vector.clear();
         for (multimap<T, UINT32>::iterator itr = l_SortedMap.begin(); itr != l_SortedMap.end(); itr++)
         {
            out_Vector.push_back(itr->second);
         }
      }

      /*! used to search a key in a hash_map in a single instruction
       * hash_map[key] will add the key if it's not found, this function will not.
       * return true when found, false when not.
      */
      template <class T, class U>
         bool hash_find(const stdext::hash_map<T, U> & l_Hash, const T & l_Value, U & l_Sink)
      {
         stdext::hash_map<T, U>::const_iterator itr = l_Hash.find(l_Value);
         if (itr == l_Hash.end())
            return false;
         l_Sink = itr->second;
         return true;
      }
   }

   /*!
   * Assert MACRO
   **/
#ifdef GOLEM_DEBUG
#  ifdef _DEBUG
#     define gassertline(expr, msgbef, linen, msgaft)\
   _ASSERT_BASE(expr, #msgbef #linen #msgaft)
#     define gassert(expr, msg)\
         gassertline(expr, #expr "\n\n" __FILE__ " ", __LINE__, " : " #msg)
#  else
   void GOLEM_DEBUG_Assert(void* in_Expr,char* filename,int linenum, GString in_sMsg);
#  define gassert(expr,msg) GOLEM_DEBUG_Assert((void*)(expr),__FILE__,__LINE__,msg)
#  endif
#else  
#  define gassert(expr, msg)
#endif

}



#endif
//EOF
