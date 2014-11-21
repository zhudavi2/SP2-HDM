#ifndef _CLASS_2D_VECTOR
#define _CLASS_2D_VECTOR


namespace Map 
{

   /**********************************************************
   G2DVector

   Models a vector in 2D space.
   **********************************************************/

   class G2DVector
   {
   public:
      REAL32  X;
      REAL32  Y;

      G2DVector();
      G2DVector( REAL32 in_fX, REAL32 in_fY );
      G2DVector( const GPoint& in_Point );
      G2DVector( const GPoint& in_FromPoint, const GPoint& in_ToPoint );

      void Set( REAL32 in_fX, REAL32 in_fY );
      void Set( const GPoint& in_Point );
      void Set( const GPoint& in_FromPoint, const GPoint& in_ToPoint );

      REAL32 Magnitude() const;
      REAL32 MagnitudeSquared() const;

      static REAL32   InnerProduct( const G2DVector& in_A, const G2DVector& in_B );
      static REAL32   CrossProduct( const G2DVector& in_A, const G2DVector& in_B );
      static REAL32   Angle( const G2DVector& in_A, const G2DVector& in_B );     // result is between -PI and +PI

      G2DVector   operator +  ( const G2DVector& in_Vector ) const;
      G2DVector   operator -  ( const G2DVector& in_Vector ) const;
      G2DVector   operator *  ( REAL32 in_fMultiplier ) const;
      G2DVector   operator /  ( REAL32 in_fDivider ) const;
      G2DVector&  operator =  ( const G2DVector& in_Vector );
      G2DVector&  operator += ( const G2DVector& in_Vector );
      G2DVector&  operator -= ( const G2DVector& in_Vector );
      G2DVector&  operator *= ( REAL32 in_fMultiplier );
      G2DVector&  operator /= ( REAL32 in_fDivider );

      bool operator == ( const G2DVector& in_Vector ) const;
      bool operator != ( const G2DVector& in_Vector ) const;
   };

};

#endif