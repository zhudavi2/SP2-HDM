/**************************************************************
*
* golem_random.h
*
* Description
* ===========
*  Prototypes for random number generation classes
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __GOLEM_RANDOM_H__
#define __GOLEM_RANDOM_H__

namespace Hector
{
   namespace Random
   {
      const UINT32   RANDOM_TABLE   = 32;
      const UINT32   RANDOM_MAX     = 0x7fffffffUL;
      const UINT32   IM1      = 2147483563UL;
      const UINT32   IM2      = 2147483399UL;
      const UINT32   IMM1     = IM1-1;
      const UINT32   DIVTABLE = (1+IMM1/RANDOM_TABLE);

      const REAL32   AM       = 4.6566130573917691960466940253829e-10f;
      const REAL32   AMfQ     = 2.3283064370807973754314699618685e-10f;
      const REAL32   AMfF     = 4.6566134130101370809246474645274e-10f;
      const UINT32   IA1      = 40014UL;
      const UINT32   IA2      = 40692UL;
      const UINT32   IQ1      = 53668UL;
      const UINT32   IQ2      = 52774UL;
      const UINT32   IR1      = 12211UL;
      const UINT32   IR2      = 3791UL;

      //! GQuick random number generation class
      /*!
       * Gives a 32 bits unsigned integer bewtween 0 & 0xFFFFFFFF.
       * This class is 6x faster than rand() standard function
       */
      class GQuick
      {
      public:
         //! Set the random generation seed
         void     Seed(UINT32 in_iSeedNumber) { m_iRandomNumber = in_iSeedNumber; }

         //! Get a (quite) random number
         UINT32   Random();

         //! Get a (quite) random floating point number between 0.0f and 1.0f (excluded)
         REAL32   RandomReal();

         //! Get a (quite) random floating point number between 0.0f and in_fFactor (excluded)
         REAL32   RandomReal(REAL32 in_fFactor)
         {
            return RandomReal() * in_fFactor;
         }

      private:
         UINT32   m_iRandomNumber;
      };

      //! Good random number generation class
      /*!
       * Random number generation class that gives a very good results
       */
      class GFull
      {
      public:
         //! Default constructor, which initializes seed to 1
         GFull()
         {
            Seed(1);
         }

         //! Set the random generation seed
         void     Seed(UINT32 in_iSeedNumber);

         //! Get a (full) random number
         UINT32   Random();

         //! Get a (full) random floating point number between 0.0f and 1.0f (excluded)
         REAL32   RandomReal();

         //! Get a (full) random floating point number between 0.0f and in_fFactor (excluded)
         REAL32   RandomReal(REAL32 in_fFactor)
         {
            return RandomReal() * in_fFactor;
         }

      private:
         UINT32   m_iRandomNumber;
         UINT32   m_iRandomNumber2;
         UINT32   m_pShuffleTable[RANDOM_TABLE];
         UINT32   m_iResult;
      };

   }

}  

#endif // #ifndef __GOLEM_RANDOM_H__
