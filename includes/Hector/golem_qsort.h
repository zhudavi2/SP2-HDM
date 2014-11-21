/**************************************************************
*
* golem_qsort.h
*
* Description
* ===========
*  Quick sort algorithm
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef __GOLEM_QSORT_H__
#define __GOLEM_QSORT_H__

namespace Hector
{
   namespace Sort
   {
      template<typename TElem>
         void Swap(TElem* in_pArrayA, TElem* in_pArrayB)
      {
         if ( in_pArrayA != in_pArrayB )
         {
            UINT8 l_iTmp;
            UINT8* l_pA = (UINT8*) in_pArrayA;
            UINT8* l_pB = (UINT8*) in_pArrayB;
            UINT32 l_iCount = sizeof(TElem);

            // Do the swap one character at a time to avoid potential alignment problems.
            while(l_iCount --)
            {
               l_iTmp = *l_pA;
               *l_pA++ = *l_pB;
               *l_pB++ = l_iTmp;
            }
         }
      }

      /***
      *shortsort(l_pHigh, l_pLow, in_CompareParam) - insertion sort for sorting short arrays
      *
      *Purpose:
      *       sorts the sub-array of elements between l_pLow and l_pHigh (inclusive)
      *       side effects:  sorts in place
      *       assumes that l_pLow < l_pHigh
      *
      *Entry:
      *       TElem *l_pLow = pointer to low element to sort
      *       TElem *l_pHigh = pointer to high element to sort
      *       void *in_CompareParam = ...
      *
      *******************************************************************************/
      template<typename TElem, typename TCompareFunc, typename TCompareParam>
         void Short(TElem *l_pLow,
         TElem *l_pHigh,
         TCompareFunc in_CompareFunc, 
         TCompareParam in_CompareParam)
      {
         // Note: in assertions below, i and j are alway inside original bound of array to sort.
         while(l_pHigh > l_pLow)
         {
            // A[i] <= A[j] for i <= j, j > l_pHigh
            TElem *l_pMax = l_pLow;
            for(TElem *p = l_pLow + 1; p <= l_pHigh; p ++)
            {
               // A[i] <= A[l_pMax] for l_pLow <= i < p
               if(in_CompareFunc(p, l_pMax, in_CompareParam) > 0)
               {
                  l_pMax = p;
               }
               // A[i] <= A[l_pMax] for l_pLow <= i <= p
            }

            // A[i] <= A[l_pMax] for l_pLow <= i <= l_pHigh

            Swap(l_pMax, l_pHigh);

            // A[i] <= A[l_pHigh] for i <= l_pHigh, so A[i] <= A[j] for i <= j, j >= l_pHigh

            l_pHigh --;

            // A[i] <= A[j] for i <= j, j > l_pHigh, loop top condition established
         }
         // A[i] <= A[j] for i <= j, j > l_pLow, which implies A[i] <= A[j] for i < j,
         // so array is sorted
      }

      template<typename TElem>
         INT32 DefaultCompareFunc(TElem* in_pA, TElem* in_pB, void *)
      {
         if(*in_pA > *in_pB)
         {
            return 1;
         }
         else if(*in_pA < *in_pB)
         {
            return -1;
         }
         return 0;
      }

      template<typename TElem, INT32 (*CompareFunc) (TElem*, TElem*, void*)>
      void Quick(TElem* in_pArray, UINT32 l_iNbElems, void * in_pCompareParam)
      {
         // Note: the number of stack entries required is no more than
         // 1 + log2(l_iNbElems), so 30 is sufficient for any array
         const UINT32 STACK_SIZE = 30;
         const UINT32 CUTOFF = 8;         // Testing shows that this is good value

         TElem *l_pLow, *l_pHigh;         // Ends of sub-array currently sorting
         TElem *l_pMid;                   // Points to middle of subarray
         TElem *l_pLowGuy, *l_pHighGuy;   // Traveling pointers for partition step
         UINT32 l_iSize;                  // Size of the sub-array
         TElem *l_pLowStack[STACK_SIZE], *l_pHighStack[STACK_SIZE];
         int l_iStackIndex;               // Stack for saving sub-array to be processed

         if(l_iNbElems < 2)
            return;                       // Nothing to do

         l_iStackIndex = 0;               // Initialize stack

         l_pLow = in_pArray;
         l_pHigh = in_pArray + (l_iNbElems - 1);   // Initialize limits

         // This entry point is for pseudo-recursion calling: setting
         // l_pLow and l_pHigh and jumping to here is like recursion, but l_iStackIndex is
         // preserved, locals aren't, so we preserve stuff on the stack
      recurse:

         l_iSize = (UINT32) (l_pHigh - l_pLow) + 1;        // Number of el's to sort

         // Below a certain l_iSize, it is faster to use a O(n^2) sorting method
         if(l_iSize <= CUTOFF)
         {
            Short(l_pLow, l_pHigh, CompareFunc, in_pCompareParam);
         }
         else
         {
            // First we pick a partitioning element.  The efficiency of the
            // algorithm demands that we find one that is approximately the median
            // of the values, but also that we select one fast.  We choose the
            // median of the first, middle, and last elements, to avoid bad
            // performance in the face of already sorted data, or data that is made
            // up of multiple sorted runs appended together.  Testing shows that a
            // median-of-three algorithm provides better performance than simply
            // picking the middle element for the latter case.

            l_pMid = l_pLow + (l_iSize / 2);       // Find middle element

            // Sort the first, middle, last elements into order
            if(CompareFunc(l_pLow, l_pMid, in_pCompareParam) > 0)
            {
               Swap(l_pLow, l_pMid);
            }

            if(CompareFunc(l_pLow, l_pHigh, in_pCompareParam) > 0)
            {
               Swap(l_pLow, l_pHigh);
            }

            if(CompareFunc(l_pMid, l_pHigh, in_pCompareParam) > 0)
            {
               Swap(l_pMid, l_pHigh);
            }

            // We now wish to partition the array into three pieces, one consisting
            // of elements <= partition element, one of elements equal to the
            // partition element, and one of elements > than it.  This is done
            // below; comments indicate conditions established at every step.

            l_pLowGuy = l_pLow;
            l_pHighGuy = l_pHigh;

            // Note that l_pHighGuy decreases and l_pLowGuy increases on every iteration,
            // so loop must terminate.
            for(;;)
            {
               // l_pLow <= l_pLowGuy < l_pHigh, l_pLow < l_pHighGuy <= l_pHigh,
               // A[i] <= A[l_pMid] for l_pLow <= i <= l_pLowGuy,
               // A[i] > A[l_pMid] for l_pHighGuy <= i < l_pHigh,
               // A[l_pHigh] >= A[l_pMid]

               // The doubled loop is to avoid calling comp(l_pMid,l_pMid), since some
               // existing comparison funcs don't work when passed the same
               // value for both pointers.

               if(l_pMid > l_pLowGuy)
               {
                  do 
                  {
                     l_pLowGuy ++;

                  } while(l_pLowGuy < l_pMid && CompareFunc(l_pLowGuy, l_pMid, in_pCompareParam) <= 0);
               }

               if(l_pMid <= l_pLowGuy)
               {
                  do
                  {
                     l_pLowGuy ++;

                  } while (l_pLowGuy <= l_pHigh && CompareFunc(l_pLowGuy, l_pMid, in_pCompareParam) <= 0);
               }

               // l_pLow < l_pLowGuy <= l_pHigh+1, A[i] <= A[l_pMid] for l_pLow <= i < l_pLowGuy,
               // either l_pLowGuy > l_pHigh or A[l_pLowGuy] > A[l_pMid]

               do
               {
                  l_pHighGuy --;

               } while (l_pHighGuy > l_pMid && CompareFunc(l_pHighGuy, l_pMid, in_pCompareParam) > 0);

               // l_pLow <= l_pHighGuy < l_pHigh, A[i] > A[l_pMid] for l_pHighGuy < i < l_pHigh,
               // either l_pHighGuy == l_pLow or A[l_pHighGuy] <= A[l_pMid]

               if(l_pHighGuy < l_pLowGuy)
                  break;

               // If l_pLowGuy > l_pHigh or l_pHighGuy == l_pLow, then we would have exited, so
               // A[l_pLowGuy] > A[l_pMid], A[l_pHighGuy] <= A[l_pMid],
               // l_pLowGuy <= l_pHigh, l_pHighGuy > l_pLow

               Swap(l_pLowGuy, l_pHighGuy);

               // If the partition element was moved, follow it.  Only need
               // to check for l_pMid == l_pHighGuy, since before the swap,
               // A[l_pLowGuy] > A[l_pMid] implies l_pLowGuy != l_pMid.

               if(l_pMid == l_pHighGuy)
                  l_pMid = l_pLowGuy;

               // A[l_pLowGuy] <= A[l_pMid], A[l_pHighGuy] > A[l_pMid]; so condition at top
               // of loop is re-established
            }

            // A[i] <= A[l_pMid] for l_pLow <= i < l_pLowGuy,
            // A[i] > A[l_pMid] for l_pHighGuy < i < l_pHigh,
            // A[l_pHigh] >= A[l_pMid]
            // l_pHighGuy < l_pLowGuy
            // implying:
            // l_pHighGuy == l_pLowGuy-1
            // or l_pHighGuy == l_pHigh - 1, l_pLowGuy == l_pHigh + 1, A[l_pHigh] == A[l_pMid]

            // Find adjacent elements equal to the partition element.  The
            // doubled loop is to avoid calling comp(l_pMid,l_pMid), since some
            // existing comparison funcs don't work when passed the same value
            // for both pointers.

            l_pHighGuy ++;
            if(l_pMid < l_pHighGuy)
            {
               do 
               {
                  l_pHighGuy --;

               } while (l_pHighGuy > l_pMid && CompareFunc(l_pHighGuy, l_pMid, in_pCompareParam) == 0);
            }

            if(l_pMid >= l_pHighGuy)
            {
               do
               {
                  l_pHighGuy --;

               } while (l_pHighGuy > l_pLow && CompareFunc(l_pHighGuy, l_pMid, in_pCompareParam) == 0);
            }

            // OK, now we have the following:
            // l_pHighGuy < l_pLowGuy
            // l_pLow <= l_pHighGuy <= l_pHigh
            // A[i]  <= A[l_pMid] for l_pLow <= i <= l_pHighGuy
            // A[i]  == A[l_pMid] for l_pHighGuy < i < l_pLowGuy
            // A[i]  >  A[l_pMid] for l_pLowGuy <= i < l_pHigh
            // A[l_pHigh] >= A[l_pMid]

            // We've finished the partition, now we want to sort the subarrays
            // [l_pLow, l_pHighGuy] and [l_pLowGuy, l_pHigh].
            // We do the smaller one first to minimize stack usage.
            // We only sort arrays of length 2 or more.

            if( l_pHighGuy - l_pLow >= l_pHigh - l_pLowGuy )
            {
               if (l_pLow < l_pHighGuy)
               {
                  // Save big recursion for later
                  l_pLowStack[l_iStackIndex] = l_pLow;
                  l_pHighStack[l_iStackIndex] = l_pHighGuy;
                  ++l_iStackIndex;
               }

               if (l_pLowGuy < l_pHigh)
               {
                  // Do small recursion
                  l_pLow = l_pLowGuy;
                  goto recurse;
               }
            }
            else
            {
               if (l_pLowGuy < l_pHigh)
               {
                  // Save big recursion for later
                  l_pLowStack[l_iStackIndex] = l_pLowGuy;
                  l_pHighStack[l_iStackIndex] = l_pHigh;
                  ++l_iStackIndex;
               }

               if (l_pLow < l_pHighGuy)
               {
                  // Do small recursion
                  l_pHigh = l_pHighGuy;
                  goto recurse;
               }
            }
         }

         // We have sorted the array, except for any pending sorts on the stack.
         // Check if there are any, and do them.

         --l_iStackIndex;
         if(l_iStackIndex >= 0)
         {
            // Pop subarray from stack
            l_pLow = l_pLowStack[l_iStackIndex];
            l_pHigh = l_pHighStack[l_iStackIndex];
            goto recurse;
         }
         else
         {
            // All subarrays done
            return;
         }
      }
   }
}

#endif