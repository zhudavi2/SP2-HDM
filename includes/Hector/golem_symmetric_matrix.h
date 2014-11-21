/**************************************************************
*
* golem_symmetricmatrix.h
*
* Description
* ===========
*  Describes the interface of class GSymmetricMatrix
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SYMMETRICMATRIX_H_
#define _GOLEM_SYMMETRICMATRIX_H_


/*!
 * Symmetric matrix class (uses less than half the space of a normal matrix)
 *   Does not include the middle row
 **/
namespace Hector
{
template<class T>
class GSymmetricMatrix
{
public:
   GSymmetricMatrix(UINT32 in_iDimension = 0);
   //! Copy constructor
   GSymmetricMatrix(const GSymmetricMatrix<T>& in_Original);
   ~GSymmetricMatrix();

   //! Clear the matrix to the given value
   void ClearTo(const T& in_Value);

   //! Resize the matrix to a new in_iNewDimension X in_iNewDimension matrix
   void Resize(UINT32 in_iNewDimension);

   //! Set the matrix element at the given position, asserts if out of bounds
   void Set(UINT32 in_iX,UINT32 in_iY,const T& in_Value);
   //! Get the matrix element at the given position, asserts if out of bounds
   const T& Get(UINT32 in_iX,UINT32 in_iY);

   //! = operator Copies the data
   GSymmetricMatrix<T>& operator =(const GSymmetricMatrix &);

   // Usable only when using simple template component
    bool Serialize(GIBuffer&) const;
    bool Unserialize(GOBuffer&);

protected:
private:

   //! Get a reference to the element at the given position
   T& ElementAt(UINT32 in_iX,UINT32 in_iY);

   //! Checks if coordinates are valid
   bool CoordinatesValid(UINT32 in_iX,UINT32 in_iY);


   T*     m_pMatrix;
   UINT32 m_iMatrixArraySize;
   UINT32 m_iMatrixDimension;
};

};
#endif //_GOLEM_SYMMETRICMATRIX_H_
