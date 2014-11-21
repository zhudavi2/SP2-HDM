/**************************************************************
*
* golem_vector.h
*
* Description
* ===========
*  Description & implementation of 2D & 3D vector classes
*
* Owner
* =====
*  Germain Sauve & Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_VECTOR_
#define _GOLEM_VECTOR_

#include "golem_math.h"

namespace Hector
{

   template<typename T>
   class GVector2D
   {
   public:
      //! First dimension component
      T  x;

      //! Second dimension component
      T  y;

      //! Default constructor
      GVector2D()
      {
      }

      //! Constructor with initialization
      GVector2D( T in_X, T in_Y )
         : x(in_X)
         , y(in_Y)
      {
      }

      //! Copy constructor
      template<typename T2>
         GVector2D( const GVector2D<T2>& in_Other )
         : x( (T) in_Other.x)
         , y( (T) in_Other.y)
      {
      }

      //! Set all dimension components
      void Set(T in_X, T in_Y)
      {
         x = in_X;
         y = in_Y;
      }

      //! Returns true if other vector is equal to this vector
      bool operator == ( const GVector2D<T>& in_Other ) const
      {
         return ( (x == in_Other.x) && (y == in_Other.y) );
      }

      //! Returns true if other vector is not equal to this vector
      bool operator != ( const GVector2D<T>& in_Other ) const
      {
         return !(in_Other == *this);
      }

      //! Returns the Id'nth dimension component constant reference
      const T& operator [] ( UINT32 in_iID ) const
      {
         assert(in_iID < 2);
         return *( (&x)[in_iID] );
      }

      //! Returns the Id'nth dimension component reference
      T& operator [] ( UINT32 in_iID )
      {
         assert(in_iID < 2);
         return *( (&x)[in_iID] );
      }

      //! Returns a new vector containing the sum of other vector and this vector
      GVector2D<T> operator + ( const GVector2D<T>& in_Other ) const
      {
         return GVector2D<T>( x + in_Other.x, y + in_Other.y ); 
      }

      //! Returns a new vector containing the difference of other vector and this vector
      GVector2D<T> operator - ( const GVector2D<T>& in_Other ) const
      {
         return GVector2D<T>( x - in_Other.x, y - in_Other.y ); 
      }

      //! Returns a new vector containing the product of other vector and this vector
      GVector2D<T> operator * ( const GVector2D<T>& in_Other ) const
      {
         return GVector2D<T>( x * in_Other.x, y * in_Other.y ); 
      }

      //! Returns a new vector containing the product of a scalar and this vector
      GVector2D<T> operator * ( T in_Scalar ) const
      {
         return GVector2D<T>( x * in_Scalar, y * in_Scalar ); 
      }

      //! Returns a new vector containing the division of other vector and this vector
      GVector2D<T> operator / ( const GVector2D<T>& in_Other ) const
      {
         return GVector2D<T>( x / in_Other.x, y / in_Other.y ); 
      }

      //! Returns a new vector containing the division of a scalar and this vector
      GVector2D<T> operator / ( T in_Scalar ) const
      {
         return GVector2D<T>( x / in_Scalar, y / in_Scalar ); 
      }


      //! Set this vector to the result of the sum of other vector and this vector
      GVector2D<T>& operator += ( const GVector2D<T>& in_Other )
      {
         x += in_Other.x;
         y += in_Other.y;
         return *this;
      }

      //! Set this vector to the result of the difference of other vector and this vector
      GVector2D<T>& operator -= ( const GVector2D<T>& in_Other )
      {
         x -= in_Other.x;
         y -= in_Other.y;
         return *this;
      }

      //! Set this vector to the result of the product of other vector and this vector
      GVector2D<T>& operator *= ( const GVector2D<T>& in_Other )
      {
         x *= in_Other.x;
         y *= in_Other.y;
         return *this;
      }

      //! Set this vector to the result of the product of a scalar and this vector
      GVector2D<T>& operator *= ( T in_Scalar )
      {
         x *= in_Scalar;
         y *= in_Scalar;
         return *this;
      }

      //! Set this vector to the result of the quotient of other vector and this vector
      GVector2D<T>&  operator /= ( const GVector2D<T>& in_Other )
      {
         x /= in_Other.x;
         y /= in_Other.y;
         return *this;
      }

      //! Returns the length of this vector
      T Length() const
      {
         return (T) sqrt( (x * x) + (y * y) );
      }

      //! Returns the squared length of this vector
      T LengthSquared() const
      {
         return (x * x) + (y * y);
      }

      //! Make sure the vector is 1 unit long
      void Normalize()
      {
         T l_Length = Length();

         x /= l_Length;
         y /= l_Length;
      }

      //! Returns true only if vector is at origin (0,0)
      bool  IsZero() const
      {
         return ( (x == 0) && (y == 0) )
      }

      //! Compute the distance between two vector end-points
      T  Distance( const GVector2D<T>& in_Other ) const
      {
         T l_DeltaX = x - in_Other.x;
         T l_DeltaY = y - in_Other.y;

         return (T) sqrt( (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) );
      }

      //! Compute the squared distance between two vector end-points
      T  DistanceSquared( const GVector2D<T>& in_Other ) const
      {
         T l_DeltaX = x - in_Other.x;
         T l_DeltaY = y - in_Other.y;

         return (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY);
      }

      //! Returns the dot product of this vector and another vector
      T   DotProduct(const GVector2D<T>& in_Other) const
      {
         return x * in_Other.x + y * in_Other.y;
      }

      //! Returns the "z" component of the cross product of this vector and another vector
      T   CrossProduct(const GVector2D<T>& in_Other) const
      {
         return x * in_Other.y - y * in_Other.x;
      }
   };

   template<typename T>
   class GVector3D
   {
   public:
      //! First dimension component
      T  x;

      //! Second dimension component
      T  y;

      //! Third dimension component
      T  z;

      //! Default constructor
      GVector3D()
      {
      }

      //! Constructor with initialization
      GVector3D( T in_X, T in_Y, T in_Z )
         : x(in_X)
         , y(in_Y)
         , z(in_Z)
      {
      }

      //! Copy constructor
      template<typename T2>
         GVector3D( const GVector3D<T2>& in_Other )
         : x( (T) in_Other.x)
         , y( (T) in_Other.y)
         , z( (T) in_Other.z)
      {
      }

      //! Conversion constructor
      template<typename T2>
         GVector3D( const GVector2D<T2>& in_Other )
         : x(in_Other.x)
         , y(in_Other.y)
         , z(0)
      {
      }

      //! Convert this 3D vector to a 2D vector
      template<typename T2>
         operator GVector2D<T2> () const
      {
         return GVector2D<T2>( (T2) x, (T2) y);
      }

      //! Set all dimension components
      void Set(T in_X, T in_Y, T in_Z)
      {
         x = in_X;
         y = in_Y;
         z = in_Z;
      }

      //! Returns true if other vector is equal to this vector
      bool operator == ( const GVector3D<T>& in_Other ) const
      {
         return ( (x == in_Other.x) && (y == in_Other.y) && (z == in_Other.z));
      }

      //! Returns true if other vector is not equal to this vector
      bool operator != ( const GVector3D<T>& in_Other ) const
      {
         return !(in_Other == *this);
      }

      //! Returns the Id'nth dimension component constant reference
      const T& operator [] ( UINT32 in_iID ) const
      {
         assert(in_iID < 3);
         return ( (&x)[in_iID] );
      }

      //! Returns the Id'nth dimension component reference
      T& operator [] ( UINT32 in_iID )
      {
         assert(in_iID < 3);
         return ( (&x)[in_iID] );
      }

      //! Returns a new vector containing the sum of other vector and this vector
      GVector3D<T> operator + ( const GVector3D<T>& in_Other ) const
      {
         return GVector3D<T>( x + in_Other.x, y + in_Other.y, z + in_Other.z ); 
      }

      //! Returns a new vector containing the difference of other vector and this vector
      GVector3D<T> operator - ( const GVector3D<T>& in_Other ) const
      {
         return GVector3D<T>( x - in_Other.x, y - in_Other.y, z - in_Other.z ); 
      }

      //! Returns a new vector containing the product of other vector and this vector
      GVector3D<T> operator * ( const GVector3D<T>& in_Other ) const
      {
         return GVector3D<T>( x * in_Other.x, y * in_Other.y, z * in_Other.z ); 
      }

      //! Returns a new vector containing the product of a scalar and this vector
      GVector3D<T> operator * ( T in_Scalar ) const
      {
         return GVector3D<T>( x * in_Scalar, y * in_Scalar, z * in_Scalar ); 
      }

      //! Returns a new vector containing the quotient of a scalar and this vector
      GVector3D<T> operator / ( T in_Scalar ) const
      {
         return GVector3D<T>( x / in_Scalar, y / in_Scalar, z / in_Scalar ); 
      }

      //! Set this vector to the result of the sum of other vector and this vector
      GVector3D<T>& operator += ( const GVector3D<T>& in_Other )
      {
         x += in_Other.x;
         y += in_Other.y;
         z += in_Other.z;
         return *this;
      }

      //! Set this vector to the result of the difference of other vector and this vector
      GVector3D<T>& operator -= ( const GVector3D<T>& in_Other )
      {
         x -= in_Other.x;
         y -= in_Other.y;
         z -= in_Other.z;
         return *this;
      }

      //! Set this vector to the result of the product of other vector and this vector
      GVector3D<T>& operator *= ( const GVector3D<T>& in_Other )
      {
         x *= in_Other.x;
         y *= in_Other.y;
         z *= in_Other.z;
         return *this;
      }

      //! Set this vector to the result of the product of a scalar and this vector
      GVector3D<T>& operator *= ( T in_Scalar )
      {
         x *= in_Scalar;
         y *= in_Scalar;
         z *= in_Scalar;
         return *this;
      }

      //! Set this vector to the result of the quotient of a scalar and this vector
      GVector3D<T>& operator /= ( T in_Scalar )
      {
         x /= in_Scalar;
         y /= in_Scalar;
         z /= in_Scalar;
         return *this;
      }

      //! Returns the length of this vector
      T Length() const
      {
         return (T) sqrt( (x * x) + (y * y) + (z * z) );
      }

      //! Returns the squared length of this vector
      T LengthSquared() const
      {
         return (x * x) + (y * y) + (z * z);
      }

      //! Make sure the vector is 1 unit long
      void Normalize()
      {
         T l_Length = Length();

         x /= l_Length;
         y /= l_Length;
         z /= l_Length;
      }

      //! Returns true only if vector is at origin (0,0,0)
      bool IsZero() const
      {
         return ( (x == 0) && (y == 0) && (z == 0) )
      }

      //! Compute the distance between two vector end-points
      T Distance( const GVector3D<T>& in_Other ) const
      {
         T l_DeltaX = x - in_Other.x;
         T l_DeltaY = y - in_Other.y;
         T l_DeltaZ = z - in_Other.z;

         return (T) sqrt( (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) + (l_DeltaZ * l_DeltaZ) );
      }

      //! Compute the squared distance between two vector end-points
      T DistanceSquared( const GVector3D<T>& in_Other ) const
      {
         T l_DeltaX = x - in_Other.x;
         T l_DeltaY = y - in_Other.y;
         T l_DeltaZ = z - in_Other.z;

         return (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) + (l_DeltaZ * l_DeltaZ);
      }

      //! Returns the dot product of this vector and another vector
      T DotProduct(const GVector3D<T>& in_Other) const
      {
         return x * in_Other.x + y * in_Other.y + z * in_Other.z;
      }

      //! Returns the cross product of this vector and another vector
      GVector3D<T> CrossProduct(const GVector3D<T>& in_Other) const
      {
         GVector3D<REAL32> l_Result;

         l_Result.x = y * in_Other.z - z * in_Other.y;
         l_Result.y = z * in_Other.x - x * in_Other.z;
         l_Result.z = x * in_Other.y - y * in_Other.x;

         return l_Result;
      }
   };

   template<typename T>
   class GVector4D
   {
   public:
      //! First dimension component
      T  x;

      //! Second dimension component
      T  y;

      //! Third dimension component
      T  z;

      //! Fourth (or weight) dimension component
      T  w;

      //! Default constructor
      GVector4D()
      {
      }

      //! Constructor with initialization
      GVector4D( T in_X, T in_Y, T in_Z, T in_W )
         : x(in_X)
         , y(in_Y)
         , z(in_Z)
         , w(in_W)
      {
      }

      //! Copy constructor
      template<typename T2>
      GVector4D( const GVector4D<T2>& in_Other )
         : x( (T) in_Other.x)
         , y( (T) in_Other.y)
         , z( (T) in_Other.z)
         , w( (T) in_Other.w)
      {
      }

      //! Conversion constructor
      template<typename T2>
      GVector4D( const GVector3D<T2>& in_Other )
         : x(in_Other.x)
         , y(in_Other.y)
         , z(in_Other.z)
         , w(1)
      {
      }

      //! Convert this 4D vector to a 3D vector
      template<typename T2>
         operator GVector3D<T2> () const
      {
         return GVector3D<T2>( (T2) x, (T2) y, (T2) z);
      }

      //! Convert this 4D vector to a 3D vector with weigth
      GVector3D<T> ApplyWeigth() const
      {
         REAL32 l_WInv = 1.f / w;
         return GVector3D<T>(x * l_WInv, y * l_WInv, z * l_WInv);
      }

      //! Set all dimension components
      void Set(T in_X, T in_Y, T in_Z, T in_W)
      {
         x = in_X;
         y = in_Y;
         z = in_Z;
         w = in_W;
      }

      //! Returns true if other vector is equal to this vector
      bool operator == ( const GVector4D<T>& in_Other ) const
      {
         return ( (x == in_Other.x) && (y == in_Other.y) && (z == in_Other.z) && (z == in_Other.w) );
      }

      //! Returns true if other vector is not equal to this vector
      bool operator != ( const GVector3D<T>& in_Other ) const
      {
         return !(in_Other == *this);
      }

      //! Returns the Id'nth dimension component constant reference
      const T& operator [] ( UINT32 in_iID ) const
      {
         assert(in_iID < 4);
         return *( (&x)[in_iID] );
      }

      //! Returns the Id'nth dimension component reference
      T& operator [] ( UINT32 in_iID )
      {
         assert(in_iID < 4);
         return *( (&x)[in_iID] );
      }

      //! Returns a new vector containing the sum of other vector and this vector
      GVector4D<T> operator + ( const GVector4D<T>& in_Other ) const
      {
         return GVector4D<T>( x + in_Other.x, y + in_Other.y, z + in_Other.z, w + in_Other.w ); 
      }

      //! Returns a new vector containing the difference of other vector and this vector
      GVector4D<T> operator - ( const GVector4D<T>& in_Other ) const
      {
         return GVector4D<T>( x - in_Other.x, y - in_Other.y, z - in_Other.z, w - in_Other.w ); 
      }

      //! Returns a new vector containing the product of a scalar and this vector
      GVector4D<T> operator * ( T in_Scalar ) const
      {
         return GVector4D<T>( x * in_Scalar, y * in_Scalar, z * in_Scalar, w * in_Scalar ); 
      }

      //! Returns a new vector containing the quotient of a scalar and this vector
      GVector4D<T> operator / ( T in_Scalar ) const
      {
         return GVector4D<T>( x / in_Scalar, y / in_Scalar, z / in_Scalar, w / in_Scalar ); 
      }

      //! Set this vector to the result of the sum of other vector and this vector
      GVector4D<T>& operator += ( const GVector4D<T>& in_Other )
      {
         x += in_Other.x;
         y += in_Other.y;
         z += in_Other.z;
         w += in_Other.w;
         return *this;
      }

      //! Set this vector to the result of the difference of other vector and this vector
      GVector4D<T>& operator -= ( const GVector4D<T>& in_Other )
      {
         x -= in_Other.x;
         y -= in_Other.y;
         z -= in_Other.z;
         w -= in_Other.w;
         return *this;
      }

      //! Set this vector to the result of the product of a scalar and this vector
      GVector4D<T>& operator *= ( T in_Scalar )
      {
         x *= in_Scalar;
         y *= in_Scalar;
         z *= in_Scalar;
         w *= in_Scalar;
         return *this;
      }

      //! Set this vector to the result of the quotient of a scalar and this vector
      GVector4D<T>& operator /= ( T in_Scalar )
      {
         x /= in_Scalar;
         y /= in_Scalar;
         z /= in_Scalar;
         w /= in_Scalar;
         return *this;
      }
   };

}

#endif //#ifndef _GOLEM_VECTOR_