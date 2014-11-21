/**************************************************************
*
* golem_geometry.h
*
* Description
* ===========
*  Description & implementation of a classes specifying a 
*  facet list.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_FACET_LIST_
#define _GOLEM_FACET_LIST_

#include "golem_shape.h"

namespace Hector
{
   namespace Geometry
   {
      template<typename T>
      class GRectangle;

      enum EFacetListType
      {
         FACETS_UNKNOWN = 0,
         FACETS_LINE,
         FACETS_LINE_STRIP,
         FACETS_TRIANGLE,
         FACETS_TRIANGLE_STRIP,
         FACETS_TRIANGLE_FAN,
         FACETS_USER,
      };

      //! This base class is a face list container
      //! By using type information & upcasting it allows
      //! manipulating different types of faces
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
#pragma pack(push, 1)
      const UINT32 MIN_FACET_LIST_ALLOC_SIZE = 16;
      class GFacetList
      {
         //! Structure used by indexed facet list types
         struct GIndexList
         {
            //! Contains all index data
            UINT32*           m_pIndexList;

            //! Contains number of indexes
            UINT32            m_iNbIndexes;

            //! Contains size of allocated index buffer
            UINT32            m_iAllocatedIndexes;
         };

         //! Structure used by ordered facet list types
         struct GFacetSequence
         {
            //! Start index
            UINT32            m_iStartIndex;

            //! Number of indexes
            UINT32            m_iIndexCount;
         };

         //! Since a facet can only be ordered or indexed,
         //! combine both data structure
         union
         {
            GIndexList       m_Indexed;
            GFacetSequence   m_Ordered;
         };

         // Next members are 16 bits integer to make sure this 
         // Data structure will be aligned to a 4 byte boundary in memory

         //! Indicates type of facet list
         UINT16            m_iType;

         //! Indicates if facet list is indexed or ordered
         UINT16            m_iIsIndexed;

      public:
         //! Constructs a new facet list
         //! By default the facet is unknown & indexed
         inline   GFacetList(EFacetListType in_Type = FACETS_UNKNOWN, bool in_bIndexed = true)
         {
            m_iIsIndexed = in_bIndexed ? 1 : 0;
            m_iType = (UINT16) in_Type;

            m_Indexed.m_pIndexList = NULL;
            m_Indexed.m_iNbIndexes = 0;
            m_Indexed.m_iAllocatedIndexes = 0;
         }

         //! Constructs a new facet list using a copy of another facet list
         inline   GFacetList(const GFacetList& in_Copy)
         {
            m_iIsIndexed = in_Copy.m_iIsIndexed;
            m_iType = in_Copy.m_iType;
            if(m_iIsIndexed)
            {
               m_Indexed.m_iNbIndexes = in_Copy.m_Indexed.m_iNbIndexes;
               m_Indexed.m_iAllocatedIndexes = m_Indexed.m_iNbIndexes;
               m_Indexed.m_pIndexList = new UINT32[m_Indexed.m_iNbIndexes];
               memcpy(m_Indexed.m_pIndexList, in_Copy.m_Indexed.m_pIndexList, m_Indexed.m_iNbIndexes * sizeof(UINT32) );
            }
            else
            {
               m_Ordered.m_iIndexCount = in_Copy.m_Ordered.m_iIndexCount;
               m_Ordered.m_iStartIndex = in_Copy.m_Ordered.m_iStartIndex;
            }
         }

         //! Destroys the facet list
         inline   ~GFacetList()
         {
            if(m_iIsIndexed)
            {
               SAFE_DELETE(m_Indexed.m_pIndexList);
            }
         }

         //! Get the type of facet
         inline   EFacetListType  Type() const
         {
            return (EFacetListType) m_iType;
         }

         //! Set the type of facet
         inline   void  Type(EFacetListType in_Type)
         {
            m_iType = (UINT16) in_Type;
         }

         //! Set the type & indexed status of facet
         inline   void  Type(EFacetListType in_Type, bool in_bIndexed)
         {
            m_iType = (UINT16) in_Type;
            Indexed(in_bIndexed);
         }

         //! Returns true if object type is indexed, false if it is ordered
         inline   bool  Indexed() const
         {
            return (m_iIsIndexed == 1);
         }

         //! Sets if object is indexed or is ordered
         //! When converting from indexed to ordered, all data is discarted
         //! When converting from ordered to indexed, all triangles listed by the previous 
         //! ordered data is converted to indexes
         inline   void  Indexed(bool in_bIndexed)
         {
            if(in_bIndexed)
            {
               // Facet list is already indexed, nothing to do
               if(m_iIsIndexed)
                  return;

               // Set facet list to indexed
               m_iIsIndexed = 1;

               // Make a local copy of ordered data
               GFacetSequence l_Data = m_Ordered;

               // Initialize data structures
               m_Indexed.m_iNbIndexes = 0;
               m_Indexed.m_iAllocatedIndexes = 0;
               m_Indexed.m_pIndexList = NULL;

               // Allocate space for ordered data
               NbIndexes(l_Data.m_iIndexCount);

               // Copy index data
               for(UINT32 i = 0;i < l_Data.m_iIndexCount;i ++)
               {
                  Index(i) = i + l_Data.m_iStartIndex;
               }
            }
            else
            {
               // Facet list is already ordered, nothing to do
               if(!m_iIsIndexed)
                  return;

               // Set facet list to ordered
               m_iIsIndexed = 0;

               // Delete old indexed data
               SAFE_DELETE(m_Indexed.m_pIndexList);
               m_Indexed.m_iNbIndexes = 0;
               m_Indexed.m_iAllocatedIndexes = 0;

               // Initialize data structures
               m_Ordered.m_iStartIndex = 0;
               m_Ordered.m_iIndexCount = 0;
            }
         }

         //! Get the number of allocated indexes
         inline   UINT32   NbAllocatedIndexes() const
         {
            if(m_iIsIndexed)
            {
               return m_Indexed.m_iAllocatedIndexes;
            }
            else
            {
               return 0;
            }
         }

         //! Set the number of allocated indexes (will overide number of indexes if smaller)
         inline   void  NbAllocatedIndexes(UINT32 in_iCount)
         {
            if(m_iIsIndexed)
            {
               if(in_iCount > 0)
               {
                  // Allocate new buffer for data
                  UINT32* l_NewBuffer = new UINT32[in_iCount];

                  // Only copy data if old buffer was not empty
                  if(m_Indexed.m_pIndexList)
                  {
                     // Compute how many indexes to copy
                     m_Indexed.m_iNbIndexes = min(m_Indexed.m_iNbIndexes, in_iCount);

                     // Copy old indexes
                     memcpy(l_NewBuffer, m_Indexed.m_pIndexList, m_Indexed.m_iNbIndexes * sizeof(UINT32) );
                  }

                  // Release old index buffer
                  SAFE_DELETE(m_Indexed.m_pIndexList);

                  // Update index buffer
                  m_Indexed.m_pIndexList = l_NewBuffer;

                  // Update index count
                  m_Indexed.m_iAllocatedIndexes = in_iCount;
               }
               else
               {
                  // Empty index data
                  SAFE_DELETE(m_Indexed.m_pIndexList);
                  m_Indexed.m_iNbIndexes = 0;
                  m_Indexed.m_iAllocatedIndexes = 0;
               }
            }
         }

         //! Get the number of indexes
         inline   UINT32   NbIndexes() const
         {
            if(m_iIsIndexed)
            {
               return m_Indexed.m_iNbIndexes;
            }
            else
            {
               return 0;
            }
         }

         //! Set the number of indexes
         inline   void  NbIndexes(UINT32 in_iCount)
         {
            if(m_iIsIndexed)
            {
               // Allocate more data if current capacity is too small
               if(in_iCount > m_Indexed.m_iAllocatedIndexes)
               {
                  // Compute new allocation size
                  UINT32 l_iNbIndexes;
                  if(in_iCount < MIN_FACET_LIST_ALLOC_SIZE)
                  {
                     l_iNbIndexes = MIN_FACET_LIST_ALLOC_SIZE;
                  }
                  else
                  {
                     UINT32 l_iShiftCount = 0;
                     l_iNbIndexes = in_iCount - 1;
                     while(l_iNbIndexes)
                     {
                        l_iShiftCount ++;
                        l_iNbIndexes >>= 1;
                     }

                     l_iNbIndexes = 1;
                     for(UINT32 i = 0;i < l_iShiftCount;i ++)
                     {
                        l_iNbIndexes <<= 1;
                     }
                  }

                  // Allocate needed space
                  NbAllocatedIndexes(l_iNbIndexes);
               }
               // Update index count
               m_Indexed.m_iNbIndexes = in_iCount;
            }
         }

         //! Get the specified constant index reference
         //! This function does not return a valid reference if facet list is not indexed
         inline   const UINT32&  Index(UINT32 in_iIndex) const
         {
            assert(m_iIsIndexed);
            assert(in_iIndex < m_Indexed.m_iNbIndexes);
            return m_Indexed.m_pIndexList[in_iIndex];
         }

         //! Get the specified index reference
         //! This function does not return a valid reference if facet list is not indexed
         inline   UINT32&  Index(UINT32 in_iIndex)
         {
            assert(m_iIsIndexed);
            assert(in_iIndex < m_Indexed.m_iNbIndexes);
            return m_Indexed.m_pIndexList[in_iIndex];
         }

         //! Set all indexes to the content of the array
         inline   void  SetupIndexes(const UINT32* in_pIndexList, UINT32 in_iCount)
         {
            if(m_iIsIndexed)
            {
               // Resize index list
               NbIndexes(in_iCount);

               // Copy all indexes
               memcpy(m_Indexed.m_pIndexList, in_pIndexList, in_iCount * sizeof(UINT32) );
            }
         }

         //! Fill the specified array with all indexes
         inline   void  FillIndexes(UINT32* out_pIndexList) const
         {
            if(m_iIsIndexed)
            {
               // Make sure array is valid
               if(!out_pIndexList)
                  return;

               // Copy all indexes
               memcpy(out_pIndexList, m_Indexed.m_pIndexList, NbIndexes() * sizeof(UINT32) );
            }
         }

         //! Get the start index constant reference
         //! This function does not return a valid reference if facet list is not ordered
         inline   const UINT32&  Start() const
         {
            assert(!m_iIsIndexed);
            return m_Ordered.m_iStartIndex;
         }

         //! Get the start index reference
         //! This function does not return a valid reference if facet list is not ordered
         inline   UINT32&  Start()
         {
            assert(!m_iIsIndexed);
            return m_Ordered.m_iStartIndex;
         }

         //! Get the index count constant reference
         //! This function does not return a valid reference if facet list is not ordered
         inline   const UINT32&  Count() const
         {
            assert(m_iIsIndexed == 0);
            return m_Ordered.m_iIndexCount;
         }

         //! Get the index count reference
         //! This function does not return a valid reference if facet list is not ordered
         inline   UINT32&  Count()
         {
            assert(m_iIsIndexed == 0);
            return m_Ordered.m_iIndexCount;
         }

         //! Copy from an other facet list
         inline   GFacetList&   operator = (const GFacetList& in_FacetSource)
         {
            // Copy type
            m_iIsIndexed = in_FacetSource.m_iIsIndexed;
            m_iType = in_FacetSource.m_iType;

            if(m_iIsIndexed == 0)
            {
               // If facet is ordered, simply copy sequence data
               m_Ordered = in_FacetSource.m_Ordered;
            }
            else
            {
               // If facet is indexed, copy all index data

               // Allocate enough index space
               NbIndexes(in_FacetSource.m_Indexed.m_iNbIndexes);

               // Copy data
               memcpy(m_Indexed.m_pIndexList, in_FacetSource.m_Indexed.m_pIndexList, in_FacetSource.m_Indexed.m_iNbIndexes * sizeof(UINT32) );
            }

            return *this;
         }

         //! Return a up-casted GLineList constant reference
         inline   const class GLineList& LineList() const
         {
            return (const class GLineList&) *this;
         }

         //! Return a up-casted GLineList reference
         inline   class GLineList& LineList()
         {
            return (class GLineList&) *this;
         }

         //! Return a up-casted GLineListIndexed constant reference
         inline   const class GLineListIndexed&  LineListIndexed() const
         {
            return (const class GLineListIndexed&) *this;
         }

         //! Return a up-casted GLineListIndexed reference
         inline   class GLineListIndexed&  LineListIndexed()
         {
            return (class GLineListIndexed&) *this;
         }

         //! Return a up-casted GLineStrip constant reference
         inline   const class GLineStrip&   LineStrip() const
         {
            return (const class GLineStrip&) *this;
         }

         //! Return a up-casted GLineStrip reference
         inline   class GLineStrip&   LineStrip()
         {
            return (class GLineStrip&) *this;
         }

         //! Return a up-casted GLineStripIndexed constant reference
         inline   const class GLineStripIndexed& LineStripIndexed() const
         {
            return (const class GLineStripIndexed&) *this;
         }

         //! Return a up-casted GLineStripIndexed reference
         inline   class GLineStripIndexed& LineStripIndexed()
         {
            return (class GLineStripIndexed&) *this;
         }

         //! Return a up-casted GTriangleList constant reference
         inline   const class GTriangleList&   TriangleList() const
         {
            return (const class GTriangleList&) *this;
         }

         //! Return a up-casted GTriangleList reference
         inline   class GTriangleList&   TriangleList()
         {
            return (class GTriangleList&) *this;
         }

         //! Return a up-casted GTriangleListIndexed constant reference
         inline   const class GTriangleListIndexed& TriangleListIndexed() const
         {
            return (const class GTriangleListIndexed&) *this;
         }

         //! Return a up-casted GTriangleListIndexed reference
         inline   class GTriangleListIndexed& TriangleListIndexed()
         {
            return (class GTriangleListIndexed&) *this;
         }

         //! Return a up-casted GTriangleStrip constant reference
         inline   const class GTriangleStrip&  TriangleStrip() const
         {
            return (class GTriangleStrip&) *this;
         }

         //! Return a up-casted GTriangleStrip reference
         inline   class GTriangleStrip&  TriangleStrip()
         {
            return (class GTriangleStrip&) *this;
         }

         //! Return a up-casted GTriangleStripIndexed constant reference
         inline   const class GTriangleStripIndexed&   TriangleStripIndexed() const
         {
            return (const class GTriangleStripIndexed&) *this;
         }

         //! Return a up-casted GTriangleStripIndexed reference
         inline   class GTriangleStripIndexed&   TriangleStripIndexed()
         {
            return (class GTriangleStripIndexed&) *this;
         }

         //! Return a up-casted GTriangleFan constant reference
         inline   const class GTriangleFan& TriangleFan() const
         {
            return (const class GTriangleFan&) *this;
         }

         //! Return a up-casted GTriangleFan reference
         inline   class GTriangleFan& TriangleFan()
         {
            return (class GTriangleFan&) *this;
         }

         //! Return a up-casted GTriangleFanIndexed constant reference
         inline   const class GTriangleFanIndexed&  TriangleFanIndexed() const
         {
            return (const class GTriangleFanIndexed&) *this;
         }

         //! Return a up-casted GTriangleFanIndexed reference
         inline   class GTriangleFanIndexed&  TriangleFanIndexed()
         {
            return (class GTriangleFanIndexed&) *this;
         }
      };
#pragma pack(pop)

      //! Describes the indexes of a line
      class GLineIndex
      {
      public:
         UINT32      m_pVertices[2];
      };

      //! This facet list contains consecutive lines
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GLineList : public GFacetList
      {
      public:
         //! Constructs a new line list
         inline   GLineList()
            : GFacetList(FACETS_LINE, false)
         {
         }

         //! Gets the number of lines
         inline   UINT32   NbLines() const
         {
            assert( (Type() == FACETS_LINE) && (!Indexed() ) );

            // Divide index count by 2 since there is 2 vertex per line
            return Count() >> 1;
         }

         //! Sets the number of lines
         inline   void  NbLines(UINT32 in_iLineCount)
         {
            assert( (Type() == FACETS_LINE) && (!Indexed() ) );

            // Allocate 2 * in_iLineCount indexes since there is 2 vertex per line
            Count() = (in_iLineCount << 1);
         }

         //! Gets the specified line
         inline   GLineIndex Line(UINT32 in_iLineIndex) const
         {
            assert( (Type() == FACETS_LINE) && (!Indexed() ) );
            assert(in_iLineIndex < NbLines() );

            // Compute first index of requested line
            UINT32 l_FirstIndex = in_iLineIndex * 2 + Start();

            // Create line indexes using computed index and the following
            GLineIndex l_Line = {l_FirstIndex, l_FirstIndex + 1};

            return l_Line;
         }
      };

      //! This facet list contains indexed lines
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GLineListIndexed : public GFacetList
      {
      public:
         //! Constructs a new line list
         inline   GLineListIndexed()
            : GFacetList(FACETS_LINE, true)
         {
         }

         //! Gets the number of lines
         inline   UINT32   NbLines() const
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );

            // Divide index count by 2 since there is 2 vertex per line
            return NbIndexes() >> 1;
         }

         //! Sets the number of lines
         inline   void  NbLines(UINT32 in_iLineCount)
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );

            // Allocate 2 * in_iLineCount indexes since there is 2 vertex per line
            NbIndexes(in_iLineCount << 1);
         }

         //! Add a single line & return its insertion index
         inline   UINT32   AddLine(const GLineIndex& in_Line)
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );

            // New line will be inserted at the end
            UINT32 l_LastIndex = NbLines();

            // Make place for the new line
            NbLines(l_LastIndex + 1);

            // Copy the line
            Line(l_LastIndex) = in_Line;

            // Return the line index
            return l_LastIndex;
         }

         //! Get the specified constant line index reference
         inline   const GLineIndex&   Line(UINT32 in_iIndex) const
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );
            assert(in_iIndex < NbLines() );

            // Return the line using index * 2 since there is 2 vertex per line
            return (const GLineIndex&) Index(in_iIndex << 1);
         }

         //! Get the specified line index reference
         inline   GLineIndex&   Line(UINT32 in_iIndex)
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );
            assert(in_iIndex < NbLines() );

            // Return the line using index * 2 since there is 2 vertex per line
            return (GLineIndex&) Index(in_iIndex << 1);
         }

         //! Set all lines to the content of the array
         inline   void  Setup(const GLineIndex* in_pLineList, UINT32 in_iLineCount)
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );

            // Directly setup indexes
            SetupIndexes( (const UINT32*) in_pLineList, in_iLineCount * 2);
         }

         //! Fill the specified array with all lines
         inline   void  Fill(GLineIndex* out_pLineList) const
         {
            assert( (Type() == FACETS_LINE) && (Indexed() ) );

            // Directly fill indexes
            FillIndexes( (UINT32*) out_pLineList);
         }
      };

      //! This facet list contains a consecutive line strip
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GLineStrip : public GFacetList
      {
      public:
         //! Constructs a new line strip
         inline   GLineStrip()
            : GFacetList(FACETS_LINE_STRIP, false)
         {
         }

         //! Gets the number of lines in the strip
         inline   UINT32   NbLines() const
         {
            assert( (Type() == FACETS_LINE_STRIP) && (!Indexed() ) );

            // Number of line strips is 1 less than number of indexes
            // or 0 if there is less than 1 index
            return (Count() > 0 ? Count() - 1 : 0);
         }

         //! Sets the number of lines in the strip
         inline   void  NbLines(UINT32 in_iLineCount)
         {
            assert( (Type() == FACETS_LINE_STRIP) && (!Indexed() ) );

            if(in_iLineCount > 0)
            {
               // Allocate indexes
               Count() = (in_iLineCount + 1);
            }
            else
            {
               // Empty indexes
               Count() = 0;
            }
         }

         //! Gets the specified line
         inline   GLineIndex Line(UINT32 in_iLineIndex) const
         {
            assert( (Type() == FACETS_LINE_STRIP) && (!Indexed() ) );
            assert(in_iLineIndex < NbLines() );

            // Compute first index of requested line
            UINT32 l_FirstIndex = in_iLineIndex + Start();

            // Create line indexes using computed index and the following
            GLineIndex l_Line = {l_FirstIndex, l_FirstIndex + 1};

            return l_Line;
         }
      };

      //! This facet list contains an indexed line strip
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GLineStripIndexed : public GFacetList
      {
      public:
         //! Constructs a new line strip list
         inline   GLineStripIndexed()
            : GFacetList(FACETS_LINE_STRIP, true)
         {
         }

         //! Gets the number of lines in the strip
         inline   UINT32   NbLines() const
         {
            assert( (Type() == FACETS_LINE_STRIP) && (Indexed() ) );

            // Number of line strips is 1 less than number of indexes
            // or 0 if there is less than 1 index
            return (NbIndexes() > 0 ? NbIndexes() - 1 : 0);
         }

         //! Sets the number of lines in the strip
         inline   void  NbLines(UINT32 in_iLineCount)
         {
            assert( (Type() == FACETS_LINE_STRIP) && (Indexed() ) );

            if(in_iLineCount > 0)
            {
               // Allocate indexes
               NbIndexes(in_iLineCount + 1);
            }
            else
            {
               // Empty indexes
               NbIndexes(0);
            }
         }

         //! Gets the specified line
         inline   GLineIndex Line(UINT32 in_iLineIndex) const
         {
            assert( (Type() == FACETS_LINE_STRIP) && (Indexed() ) );
            assert(in_iLineIndex < NbLines() );

            // Create line indexes using requested index and the following
            GLineIndex l_Line = { Index(in_iLineIndex), Index(in_iLineIndex + 1) };

            return l_Line;
         }
      };

      //! Describes the indexes of a triangle
      class GTriangleIndex
      {
      public:
         UINT32      m_pVertices[3];
      };

      //! This facet list contains consecutive triangles
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleList : public GFacetList
      {
      public:
         //! Constructs a new triangle list
         inline   GTriangleList()
            : GFacetList(FACETS_TRIANGLE, false)
         {
         }

         //! Gets the number of triangles
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE) && (!Indexed() ) );

            // Divide index count by 3 since there is 3 vertex per triangle
            return Count() / 3;
         }

         //! Sets the number of triangles
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE) && (!Indexed() ) );

            // Allocate 3 * in_iTriangleCount indexes since there is 3 vertex per triangle
            Count() = (in_iTriangleCount * 3);
         }

         //! Gets the specified triangle
         inline   GTriangleIndex   Triangle(UINT32 in_iTriangleIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE) && (!Indexed() ) );
            assert(in_iTriangleIndex < NbTriangles() );

            // Compute first index of requested triangle
            UINT32 l_FirstIndex = in_iTriangleIndex * 3 + Start();

            // Create triangle indexes using computed index and the following
            GTriangleIndex l_Triangle = {l_FirstIndex, l_FirstIndex + 1, l_FirstIndex + 2};

            return l_Triangle;
         }
      };

      //! This facet list contains indexed triangles
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleListIndexed : public GFacetList
      {
      public:
         //! Constructs a new triangle list
         inline   GTriangleListIndexed()
            : GFacetList(FACETS_TRIANGLE, true)
         {
         }

         //! Constructs a new triangle list with copy
         inline   GTriangleListIndexed( const GTriangleListIndexed& in_Copy)
            : GFacetList(in_Copy)
         {
         }

         //! Gets the number of triangles
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );

            // Divide index count by 3 since there is 3 vertex per triangle
            return NbIndexes() / 3;
         }

         //! Sets the number of triangles
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );

            // Allocate 3 * in_iTriangleCount indexes since there is 3 vertex per triangle
            NbIndexes(in_iTriangleCount * 3);
         }

         //! Add a single triangle & return its insertion index
         inline   UINT32   AddTriangle(const GTriangleIndex& in_Triangle)
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );

            // New triangle will be inserted at the end
            UINT32 l_LastIndex = NbTriangles();

            // Make place for the new triangle
            NbTriangles(l_LastIndex + 1);

            // Copy the triangle
            Triangle(l_LastIndex) = in_Triangle;

            // Return the triangle index
            return l_LastIndex;
         }

         //! Get the specified constant triangle index reference
         inline   const GTriangleIndex&  Triangle(UINT32 in_iIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );
            assert(in_iIndex < NbTriangles() );

            // Return the triangle using index * 3 since there is 3 vertex per triangle
            return (const GTriangleIndex&) Index(in_iIndex * 3);
         }

         //! Get the specified triangle index reference
         inline   GTriangleIndex&  Triangle(UINT32 in_iIndex)
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );
            assert(in_iIndex < NbTriangles() );

            // Return the triangle using index * 3 since there is3 vertex per triangle
            return (GTriangleIndex&) Index(in_iIndex * 3);
         }

         //! Set all triangles to the content of the array
         inline   void  Setup(const GTriangleIndex* in_pTriangleList, UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );

            // Directly setup indexes
            SetupIndexes( (const UINT32*) in_pTriangleList, in_iTriangleCount * 3);
         }

         //! Fill the specified array with all triangles
         inline   void  Fill(GTriangleIndex* out_pTriangleList) const
         {
            assert( (Type() == FACETS_TRIANGLE) && (Indexed() ) );

            // Directly fill indexes
            FillIndexes( (UINT32*) out_pTriangleList);
         }
      };

      //! This facet list contains a consecutive triangle strip
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleStrip : public GFacetList
      {
      public:
         //! Constructs a new triangle strip
         inline   GTriangleStrip()
            : GFacetList(FACETS_TRIANGLE_STRIP, false)
         {
         }

         //! Gets the number of triangles in the strip
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (!Indexed() ) );

            // Number of triangle strips is 2 less than number of indexes
            // or 0 if there is less than 2 indexes
            return (Count() > 1 ? Count() - 2 : 0);
         }

         //! Sets the number of triangles in the strip
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (!Indexed() ) );

            if(in_iTriangleCount > 0)
            {
               // Allocate indexes
               Count() = (in_iTriangleCount + 2);
            }
            else
            {
               // Empty indexes
               Count() = 0;
            }
         }

         //! Gets the specified triangle
         inline   GTriangleIndex   Triangle(UINT32 in_iTriangleIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (!Indexed() ) );
            assert(in_iTriangleIndex < NbTriangles() );

            // Compute first index of requested triangle
            UINT32 l_FirstIndex = in_iTriangleIndex + Start();

            // Create triangle indexes using computed index and the following
            GTriangleIndex l_Triangle = {l_FirstIndex, l_FirstIndex + 1, l_FirstIndex + 2};

            return l_Triangle;
         }
      };

      //! This facet list contains an indexed triangle strip
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleStripIndexed : public GFacetList
      {
      public:
         //! Constructs a new triangle strip list
         inline   GTriangleStripIndexed()
            : GFacetList(FACETS_TRIANGLE_STRIP, true)
         {
         }

         //! Gets the number of triangles in the strip
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (Indexed() ) );

            // Number of triangle strips is 2 less than number of indexes
            // or 0 if there is less than 2 index
            return (Count() > 1 ? Count() - 2 : 0);
         }

         //! Sets the number of triangles in the strip
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (Indexed() ) );

            if(in_iTriangleCount > 0)
            {
               // Allocate indexes
               Count() = (in_iTriangleCount + 2);
            }
            else
            {
               // Empty indexes
               Count() = 0;
            }
         }

         //! Gets the specified triangle
         inline   GTriangleIndex   Triangle(UINT32 in_iTriangleIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE_STRIP) && (Indexed() ) );
            assert(in_iTriangleIndex < NbTriangles() );

            // Compute first index of requested triangle
            UINT32 l_FirstIndex = in_iTriangleIndex + Start();

            // Create triangle indexes using computed index and the following
            GTriangleIndex l_Triangle = { Index(l_FirstIndex), Index(l_FirstIndex + 1), Index(l_FirstIndex + 2) };

            return l_Triangle;
         }
      };

      //! This facet list contains a consecutive triangle fan
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleFan : public GFacetList
      {
      public:
         //! Constructs a new triangle fan
         inline   GTriangleFan()
            : GFacetList(FACETS_TRIANGLE_FAN, false)
         {
         }

         //! Gets the number of triangles in the fan
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (!Indexed() ) );

            // Number of triangle fans is 2 less than number of indexes
            // or 0 if there is less than 2 indexes
            return (Count() > 1 ? Count() - 2 : 0);
         }

         //! Sets the number of triangles in the fan
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (!Indexed() ) );

            if(in_iTriangleCount > 0)
            {
               // Allocate indexes
               Count() = (in_iTriangleCount + 2);
            }
            else
            {
               // Empty indexes
               Count() = 0;
            }
         }

         //! Gets the specified triangle
         inline   GTriangleIndex   Triangle(UINT32 in_iTriangleIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (!Indexed() ) );
            assert(in_iTriangleIndex < NbTriangles() );

            // Compute first index of requested triangle
            UINT32 l_FirstIndex = in_iTriangleIndex + Start();

            // Create triangle indexes using computed index and the following
            GTriangleIndex l_Triangle = {l_FirstIndex, l_FirstIndex + 1, l_FirstIndex + 2};

            return l_Triangle;
         }
      };

      //! This facet list contains an indexed triangle fan
      //! All objects references returned are valid until the
      //! object is modified. An invalid index returns an invalid reference.
      class GTriangleFanIndexed : public GFacetList
      {
      public:
         //! Constructs a new triangle fan list
         inline   GTriangleFanIndexed()
            : GFacetList(FACETS_TRIANGLE_FAN, true)
         {
         }

         //! Gets the number of triangles in the fan
         inline   UINT32   NbTriangles() const
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (Indexed() ) );

            // Number of triangle fans is 2 less than number of indexes
            // or 0 if there is less than 2 index
            return (Count() > 1 ? Count() - 2 : 0);
         }

         //! Sets the number of triangles in the fan
         inline   void  NbTriangles(UINT32 in_iTriangleCount)
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (Indexed() ) );

            if(in_iTriangleCount > 0)
            {
               // Allocate indexes
               Count() = (in_iTriangleCount + 2);
            }
            else
            {
               // Empty indexes
               Count() = 0;
            }
         }

         //! Gets the specified triangle
         inline   GTriangleIndex   Triangle(UINT32 in_iTriangleIndex) const
         {
            assert( (Type() == FACETS_TRIANGLE_FAN) && (Indexed() ) );
            assert(in_iTriangleIndex < NbTriangles() );

            // Compute first index of requested triangle
            UINT32 l_FirstIndex = in_iTriangleIndex + Start();

            // Create triangle indexes using computed index and the following
            GTriangleIndex l_Triangle = { Index(l_FirstIndex), Index(l_FirstIndex + 1), Index(l_FirstIndex + 2) };

            return l_Triangle;
         }
      };

      //! Constains multiple facet lists, not necessarily of the same type
      class GMultiFacetList
      {
      public:
         //! Get the number of facet lists
         inline	UINT32	NbFacetLists() const
         {
            return (UINT32) m_Facets.size();
         }

         //! Set the number of facet lists
         inline	void	NbFacetLists(UINT32 in_iCount)
         {
            m_Facets.resize(in_iCount);
         }

         //! Get the specified constant facet list
         inline	const GFacetList&	Facet_List(UINT32 in_iIndex) const
         {
            assert(in_iIndex < NbFacetLists() );
            return m_Facets[in_iIndex];
         }

         //! Get the specified facet list
         inline	GFacetList&	Facet_List(UINT32 in_iIndex)
         {
            assert(in_iIndex < NbFacetLists() );
            return m_Facets[in_iIndex];
         }

      private:
         vector< GFacetList >	m_Facets;
      };

      //! Describes an ordered polygon, which has a start index in a shape & an index count
      //! The polygon is always assumed to be closed & there is no need to repeat the first
      //! point index to close it.
      class GPolygon
      {
      public:
         //! Get the start index constant reference
         inline	const UINT32&	Start() const
         {
            return m_iStartIndex;
         }

         //! Get the start index reference
         inline	UINT32&	Start()
         {
            return m_iStartIndex;
         }

         //! Get the index count constant reference
         inline	const UINT32&	NbIndexes() const
         {
            return m_iIndexCount;
         }

         //! Get the index count reference
         inline	UINT32&	NbIndexes()
         {
            return m_iIndexCount;
         }

         //! Get the specified index
         inline	UINT32	Index(UINT32 in_iIndex) const
         {
            assert(in_iIndex < NbIndexes() );
            return m_iStartIndex + in_iIndex;
         }

      private:
         UINT32	m_iStartIndex;
         UINT32	m_iIndexCount;
      };


      //! Describes an indexed polygon, which has a index in a shape for each point.
      //! The polygon is always assumed to be closed & there is no need to repeat the first
      //! point index to close it.
      class GPolygonIndexed
      {
      public:
         //! Get the number of indexes
         inline	UINT32	NbIndexes() const
         {
            return (UINT32) m_Indexes.size();
         }

         //! Set the number of indexes
         inline	void	NbIndexes(UINT32 in_iCount)
         {
            m_Indexes.resize(in_iCount);
         }

         //! Get the specified constant index reference
         inline	const UINT32&	Index(UINT32 in_iIndex) const
         {
            assert(in_iIndex < NbIndexes() );
            return m_Indexes[in_iIndex];
         }

         //! Get the specified index reference
         inline	UINT32&	Index(UINT32 in_iIndex)
         {
            assert(in_iIndex < NbIndexes() );
            return m_Indexes[in_iIndex];
         }

         //! Set all indexes to the content of the array
         inline	void	SetupIndexes(const UINT32* in_pIndexList, UINT32 in_iCount)
         {
            // Resize index list
            NbIndexes(in_iCount);

            // Copy all indexes
            for(UINT32 i = 0;i < in_iCount;i ++)
            {
               m_Indexes[i] = in_pIndexList[i];
            }
         }

         //! Fill the specified array with all indexes
         inline	void	FillIndexes(UINT32* out_pIndexList) const
         {
            // Make sure array is valid
            if(!out_pIndexList)
               return;

            // Copy all indexes
            for(UINT32 i = 0;i < NbIndexes();i ++)
            {
               out_pIndexList[i] = m_Indexes[i];
            }
         }

      private:
         vector<UINT32> m_Indexes;
      };

      //! Class that describes a face made of a polygon and that can contain holes made of polygons
      template<typename TPoly>
      class GFace
      {
      public:
         //! Get the outer contour polygon constant reference
         inline	const TPoly&	Outer() const
         {
            return m_Outer;
         }

         //! Get the outer contour polygon reference
         inline	TPoly&	Outer()
         {
            return m_Outer;
         }

         //! Get the number of inner contours (holes)
         inline	UINT32	NbInners() const
         {
            return (UINT32) m_Inners.size();
         }

         //! Set the number of inner contours (holes)
         inline	void	NbInners(UINT32 in_iCount)
         {
            m_Inners.resize(in_iCount);
         }

         //! Get the specified constant inner contour polygon reference
         inline	const TPoly&	Inner(UINT32 in_iIndex) const
         {
            assert(in_iIndex < NbInners() );
            return m_Inners[in_iIndex];
         }

         //! Get the specified inner contour polygon reference
         inline	TPoly&	Inner(UINT32 in_iIndex)
         {
            assert(in_iIndex < NbInners() );
            return m_Inners[in_iIndex];
         }

         //! Convert the face into a triangle list
         void	ConvertToTriangleList(const GShape& in_Shape, GTriangleListIndexed& out_TriList, bool in_bCW = false);

         //! Scan convert (rasterize) the face winthin the specified rectangle by calling SpanProc callback for each span of pixel on a scan line
         void ScanConvertUniform(const GShape& in_Shape, const class GRectangle<INT32>& in_Window, void (*SpanProc) (void* in_pParam, INT32 y, INT32 x1, INT32 x2), void* in_pSpanParam);

      private:
         TPoly			   m_Outer;
         vector<TPoly>	m_Inners;
      };

      //! Class that describes a primitive made of multiple polygonal faces
      template<class TPoly>
      class GFaceList
      {
      public:
         //! Get the number of faces
         inline	UINT32	NbFaces() const
         {
            return (UINT32) m_Faces.size();
         }

         //! Set the number of faces
         inline	void	NbFaces(UINT32 in_iCount)
         {
            m_Faces.resize(in_iCount);
         }

         //! Get the specified constant face
         inline	const GFace<TPoly>&	Face(UINT32 in_iIndex) const
         {
            assert(in_iIndex < NbFaces() );
            return m_Faces[in_iIndex];
         }

         //! Get the specified face
         inline	GFace<TPoly>&	Face(UINT32 in_iIndex)
         {
            assert(in_iIndex < NbFaces() );
            return m_Faces[in_iIndex];
         }

         //! Append an other face list at the end of this face list
         inline   void  Append(const GFaceList<TPoly>& in_FaceList)
         {
            // Make room new data
            UINT32 l_iLastSize = NbFaces();
            NbFaces(l_iLastSize + in_FaceList.NbFaces() );

            // Copy data
            for(UINT32 f = 0;f < in_FaceList.NbFaces();f ++)
            {
               Face(f + l_iLastSize) = in_FaceList.Face(f);
            }
         }

      private:
         vector< GFace<TPoly> >	m_Faces;
      };

   }
}

#endif // _GOLEM_FACET_LIST_
