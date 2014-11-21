/**************************************************************
*
* golem_quaternion.h
*
* Description
* ===========
*  Implementation of class GQuaternion which allows operations
*  on quaternions.
*
*  Most of the code on this page was taken from a forum thread
*  on FlipCode. Also, some mathematics of the quaternion can 
*  be found here : 
*     http://www.j3d.org/matrix_faq/matrfaq_latest.html
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_QUATERNION_
#define _GOLEM_QUATERNION_

#include "golem_vector.h"
#include "golem_matrix.h"

namespace Hector
{
   /*!
    *  general quaternion class ( unit and nonunit ),
    *  designed for 3d rotations and orientation representation.
    *
    *  Main goals:
    *  1. fast math methods
    *  2. clean code
    *  3. avoiding of numerical instabilities (singularities) 
    *
    *  We use quaternion for 3d rotations, and the null length quaternion have no sense. 
    **/
   template <typename T>
   class GQuaternion
   {
   public:
      T x;
      T y;
      T z;
      T w;

      static const T Epsilon;

      GQuaternion()
      {
      }

      GQuaternion(const GQuaternion& in_Copy)
      {
         *this = in_Copy;
      }

      GQuaternion(T in_X, T in_Y, T in_Z, T in_W)
         : x(in_X)
         , y(in_Y)
         , z(in_Z)
         , w(in_W)
      {
      }

      void Set(T in_X, T in_Y, T in_Z, T in_W)
      {
         x = in_X;
         y = in_Y;
         z = in_Z;
         w = in_W;
      }

      void Set(const GQuaternion& in_Quat)
      {
         x = in_Quat.x;
         y = in_Quat.y;
         z = in_Quat.z;
         w = in_Quat.w;
      }

      //-- operations --------------------------------------------
      void LoadIdentity()
      {
         x = 0.0f;
         y = 0.0f;
         z = 0.0f;
         w = 1;
      }

      GQuaternion Conjugate() const
      {
         return GQuaternion( -x, -y, -z, w );
      }

      void Scale(T in_Scale)
      {
         x *= in_Scale;
         y *= in_Scale;
         z *= in_Scale;
         w *= in_Scale;
      }

      T Norm() const
      {
         return x*x + y*y + z*z + w*w;
      }

      T Magnitude(void) const
      {
         return (T) sqrt( Norm() );
      }

      GQuaternion Invert(void) const
      {
         T l_Inv = 1.f / Magnitude();
         return GQuaternion( -x*l_Inv, -y*l_Inv, -z*l_Inv, w*l_Inv );
      } 

      /*!
       * stabilize quaternion length within 1 - 1/4
       * this operation is a lot faster than normalization
       * and preserve length goes to 0 or infinity
       **/
      void StabilizeLength()
      {
         T cs = (T)(fabs(x) + fabs(y) + fabs(z) + fabs(w));
         if( cs > 0.0f )
         {
            Set( x/=cs,  y/=cs,  z/=cs,  w/=cs ); 
         }
         else
         {
            LoadIdentity();
         }
      }

      /*!
       * scale quaternion that its norm goes to 1 
       * the appearing of 0 magnitude or near is a error,
       * so we can be sure that can divide by magnitude
       **/
      void Normalize(void)
      {
         T m = Magnitude();
         if( m < Epsilon )
         {
            StabilizeLength();
            m = Magnitude();
         }
         Scale( 1/m );
      }

      //----operators------------------------------------------------
      GQuaternion operator-()const
      {
         return GQuaternion(-x, -y, -z, -w);
      }

      GQuaternion operator+(const GQuaternion& in_Quat)const
      {
         return GQuaternion(x + in_Quat.x, y + in_Quat.y, z + in_Quat.z, w + in_Quat.w);
      }

      GQuaternion operator-(const GQuaternion& in_Quat)const
      {
         return GQuaternion(x - in_Quat.x, y - in_Quat.y, z - in_Quat.z, w - in_Quat.w);
      }

      GQuaternion operator*(const T& in_Scalar)const
      {
         return GQuaternion(x*in_Scalar, y*in_Scalar, z*in_Scalar, w*in_Scalar);
      }

      /*!
       * in_Quat( cross(v,v') + wv' + w'v, ww' - dot(v,v') )
       **/
      GQuaternion operator*(const GQuaternion& in_Quat)const
      {
         return GQuaternion(w*in_Quat.x + x*in_Quat.w + y*in_Quat.z - z*in_Quat.y,
                            w*in_Quat.y + y*in_Quat.w + z*in_Quat.x - x*in_Quat.z,
                            w*in_Quat.z + z*in_Quat.w + x*in_Quat.y - y*in_Quat.x,
                            w*in_Quat.w - x*in_Quat.x - y*in_Quat.y - z*in_Quat.z);
         // 16 multiplications    12 addidtions    0 variables
      }

      /*!
       * alternative way of quaternion multiplication,
       * can speedup multiplication for some systems (PDA for example)
       * http://mathforum.org/library/drmath/view/51464.html
       * http://www.lboro.ac.uk/departments/ma/gallery/quat/src/quat.ps
       * in provided code by url's many bugs, have to be rewriten.
       **/
      GQuaternion MulAlt( const GQuaternion& in_Quat) const
      {
         T t0 = (x-y)*(in_Quat.y-in_Quat.x);
         T t1 = (w+z)*(in_Quat.w+in_Quat.z);
         T t2 = (w-z)*(in_Quat.y+in_Quat.x);
         T t3 = (x+y)*(in_Quat.w-in_Quat.z);
         T t4 = (x-z)*(in_Quat.z-in_Quat.y);
         T t5 = (x+z)*(in_Quat.z+in_Quat.y);
         T t6 = (w+y)*(in_Quat.w-in_Quat.x);
         T t7 = (w-y)*(in_Quat.w+in_Quat.x);

         T t8 = t5 + t6 + t7;
         T t9 = (t4 + t8)*(T)0.5;
         return GQuaternion( t3+t9-t6,
                             t2+t9-t7,
                             t1+t9-t8,
                             t0+t9-t5 );
         // 9 multiplications    27  addidtions    8 variables
      }

      GQuaternion& operator+=(const GQuaternion& in_Quat)
      {
         x += in_Quat.x; y += in_Quat.y; z += in_Quat.z; w += in_Quat.w;
         return *this;
      };

      GQuaternion& operator-=(const GQuaternion& in_Quat)
      {
         x -= in_Quat.x; y -= in_Quat.y; z -= in_Quat.z; w -= in_Quat.w;
         return *this;
      };

      GQuaternion& operator*=(const GQuaternion& in_Quat)
      {
         Set((*this)*in_Quat);  // have no optimization here
         return *this;
      };

      //---- usefull methods -------------------------------
      /*!
       * return rotation angle from -PI to PI 
       **/
      T RotationAngle()const
      {
         T l_Angle = (T) (acos(w) * 2);

         return l_Angle > (T) Math::PI ? l_Angle - (T) (2 * Math::PI) : l_Angle;
      }

      /*!
       * rotate vector by quaternion
       **/
      GVector3D<T> Rotate(const GVector3D<T>& v)const
      {
         GQuaternion in_Quat(v.x * w + v.z * y - v.y * z,
                             v.y * w + v.x * z - v.z * x,
                             v.z * w + v.y * x - v.x * y,
                             v.x * x + v.y * y + v.z * z);

         return GVector3D<T>(w * in_Quat.x + x * in_Quat.w + y * in_Quat.z - z * in_Quat.y,
                             w * in_Quat.y + y * in_Quat.w + z * in_Quat.x - x * in_Quat.z,
                             w * in_Quat.z + z * in_Quat.w + x * in_Quat.y - y * in_Quat.x)*( 1/Norm() );

         // 29  multiplications, 20 addidtions, 4 variables
         // 5 
         /*
         // refrence implementation  
         GQuaternion r = (*this)*GQuaternion(v.x, v.y, v.z, 0)*this->inverted();
         return GVector3D<T>( r.x, r.y, r.z ); 
         */

         /*
         // alternative implementation
         T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
         x2 = in_Quat.x + in_Quat.x; y2 = in_Quat.y + in_Quat.y; z2 = in_Quat.z + in_Quat.z;

         xx = in_Quat.x * x2;   xy = in_Quat.x * y2;   xz = in_Quat.x * z2;
         yy = in_Quat.y * y2;   yz = in_Quat.y * z2;   zz = in_Quat.z * z2;
         wx = in_Quat.w * x2;   wy = in_Quat.w * y2;   wz = in_Quat.w * z2;

         return GVector3D<T>( v.x  - v.x * (yy + zz) + v.y * (xy - wz) + v.z * (xz + wy),
         v.y  + v.x * (xy + wz) - v.y * (xx + zz) + v.z * (yz - wx),
         v.z  + v.x * (xz - wy) + v.y * (yz + wx) - v.z * (xx + yy) )*( 1/norm() );
         // 18 multiplications, 21 addidtions, 12 variables
         */
      };

      /*!
       * create a unit quaternion from axis angle representation
       **/
      void UnitFromAxisAngle(const GVector3D<T>& in_Axis, const T& in_Angle)
      {
         GVector3D<T> v(in_Axis);
         v.Normalize();
         T half_angle = in_Angle / (T) 2;
         T sin_a = (T) sin(half_angle);
         Set(v.x*sin_a, v.y*sin_a, v.z*sin_a, (T) cos(half_angle) );
      };

      /*!
       * convert a quaternion to axis angle representation, 
       * preserve the axis direction and angle from -PI to +PI
       **/
      void ConvertToAxisAngle(GVector3D<T>& out_Axis, T& out_Angle)const
      {
         T vl = (T) sqrt( x*x + y*y + z*z );
         if( vl > Epsilon )
         {
            T ivl = 1/vl;
            out_Axis.Set( x*ivl, y*ivl, z*ivl );
            if( w < 0 )
            {
               out_Angle = 2.0f*(T)atan2(-vl, -w); //-PI,0 
            }
            else
            {
               out_Angle = 2.0f*(T)atan2( vl,  w); //0,PI 
            }
         }
         else
         {
            out_Axis = GVector3D<T>(0,0,0);
            out_Angle = 0;
         }
      };

      /*!
       * create a unit quaternion by rotation matrix
       * martix must contain only rotation (not scale or shear)
       *
       * For numerical stability we find first the greatest component of quaternion
       * and than search others from this one
       **/
      void UnitFromMatrix( const GMatrix4x4<T>& in_Mat )
      {
         T tr = in_Mat(0,0) + in_Mat(1,1) + in_Mat(2,2); // trace of martix
         if(tr > 0.0f)
         {
            // if trace positive than "w" is biggest component
            Set( in_Mat(1,2) - in_Mat(2,1), 
                 in_Mat(2,0) - in_Mat(0,2), 
                 in_Mat(0,1) - in_Mat(1,0), 
                 tr+1 );
            Scale( (T) (0.5 / sqrt( w ) ) );     // "w" contain the "norm * 4"
         }
         else if( (in_Mat(0,0) > in_Mat(1,1) ) && ( in_Mat(0,0) > in_Mat(2,2)) )
         {
            // Some of vector components is bigger
            Set( 1 + in_Mat(0,0) - in_Mat(1,1) - in_Mat(2,2),
                 in_Mat(1,0) + in_Mat(0,1),
                 in_Mat(2,0) + in_Mat(0,2), 
                 in_Mat(1,2) - in_Mat(2,1) );
            Scale( (T) (0.5 / sqrt( x ) ) );
         }
         else if ( in_Mat(1,1) > in_Mat(2,2) )
         {
            Set( in_Mat(1,0) + in_Mat(0,1), 
                 1 + in_Mat(1,1) - in_Mat(0,0) - in_Mat(2,2),
                 in_Mat(2,1) + in_Mat(1,2), 
                 in_Mat(2,0) - in_Mat(0,2) );
            Scale( (T) (0.5 / sqrt( y ) ) );
         }
         else
         {
            Set( in_Mat(2,0) + in_Mat(0,2), 
                 in_Mat(2,1) + in_Mat(1,2),
                 1 + in_Mat(2,2) - in_Mat(0,0) - in_Mat(1,1), 
                 in_Mat(0,1) - in_Mat(1,0) );
            Scale( (T) (0.5 / sqrt( z ) ) );
         }
      }

      /*!
       * create a nonunit quaternion from rotation matrix 
       * matrix must contain only rotation (not scale or shear)
       * the result quaternion length is numerical stable 
       **/
      void FromMatrix( const GMatrix4x4<T>& in_Mat )
      {
         T tr = in_Mat(0,0) + in_Mat(1,1) + in_Mat(2,2); // trace of martix
         if (tr > 0.0f)
         {
            // if trace positive than "w" is biggest component
            Set( in_Mat(1,2) - in_Mat(2,1), 
                 in_Mat(2,0) - in_Mat(0,2), 
                 in_Mat(0,1) - in_Mat(1,0), 
                 tr + 1 );
         }
         // Some of vector components is bigger
         else  if( (in_Mat(0,0) > in_Mat(1,1) ) && ( in_Mat(0,0) > in_Mat(2,2)) )
         {
            Set( 1 + in_Mat(0,0) - in_Mat(1,1) - in_Mat(2,2), 
                 in_Mat(1,0) + in_Mat(0,1),
                 in_Mat(2,0) + in_Mat(0,2), 
                 in_Mat(1,2) - in_Mat(2,1) );
         }
         else if ( in_Mat(1,1) > in_Mat(2,2) )
         {
            Set( in_Mat(1,0) + in_Mat(0,1), 
                 1 + in_Mat(1,1) - in_Mat(0,0) - in_Mat(2,2),
                 in_Mat(2,1) + in_Mat(1,2), 
                 in_Mat(2,0) - in_Mat(0,2) ); 
         }
         else
         {
            Set( in_Mat(2,0) + in_Mat(0,2), 
                 in_Mat(2,1) + in_Mat(1,2),
                 1 + in_Mat(2,2) - in_Mat(0,0) - in_Mat(1,1), 
                 in_Mat(0,1) - in_Mat(1,0) );
         }
      }

      /*!
       * Set the rotation to matrix
       **/
      void ConvertToMatrix( GMatrix4x4<T>& out_Mat )const
      {
         T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
         T s  = 2.0f/Norm();
         x2 = x * s;    y2 = y * s;    z2 = z * s;
         xx = x * x2;   xy = x * y2;   xz = x * z2;
         yy = y * y2;   yz = y * z2;   zz = z * z2;
         wx = w * x2;   wy = w * y2;   wz = w * z2;

         out_Mat.LoadIdentity();

         out_Mat(0,0) = 1 - (yy + zz);
         out_Mat(1,0) = xy - wz;
         out_Mat(2,0) = xz + wy;

         out_Mat(0,1) = xy + wz;
         out_Mat(1,1) = 1 - (xx + zz);
         out_Mat(2,1) = yz - wx;

         out_Mat(0,2) = xz - wy;
         out_Mat(1,2) = yz + wx;
         out_Mat(2,2) = 1 - (xx + yy);

         //1 division    16 multiplications    15 addidtions    12 variables
      }

      /*!
       * Set the rotation from unit quat to matrix
       **/
      void ConvertUnitToMatrix( GMatrix4x4<T>& out_Mat  )const
      {
         assert(abs(Norm() - (T) 1) < Epsilon);

         T wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
         x2 = x + x;    y2 = y + y;    z2 = z + z;
         xx = x * x2;   xy = x * y2;   xz = x * z2;
         yy = y * y2;   yz = y * z2;   zz = z * z2;
         wx = w * x2;   wy = w * y2;   wz = w * z2;

         out_Mat.LoadIdentity();

         out_Mat(0,0) = 1 - (yy + zz);
         out_Mat(1,0) = xy - wz;
         out_Mat(2,0) = xz + wy;

         out_Mat(0,1) = xy + wz;
         out_Mat(1,1) = 1 - (xx + zz);
         out_Mat(2,1) = yz - wx;

         out_Mat(0,2) = xz - wy;
         out_Mat(1,2) = yz + wx;
         out_Mat(2,2) = 1 - (xx + yy);
      }

      /*!
       * shortest arc quaternion rotate one vector to another by shortest path.
       * create rotation from -> to, for any length vectors.
       **/
      void ShortestArc(const GVector3D<T>& from, const GVector3D<T>& to )
      {
         GVector3D<REAL32> l_FromCopy(from);
         l_FromCopy.Normalize();

         GVector3D<REAL32> l_ToCopy(to);
         l_ToCopy.Normalize();

         GVector3D<T> c;
         T d = l_FromCopy.DotProduct(l_ToCopy);
         T s = (T) sqrt( (1 + d) * 2);

         if( s <= Epsilon ) // angle close to PI
         {
            if( ( l_FromCopy.z*l_FromCopy.z ) > ( l_FromCopy.x*l_FromCopy.x ) )
            {
               c = l_FromCopy.CrossProduct(GVector3D<T>(1,0,0) );
            }
            else 
            {
               c = l_FromCopy.CrossProduct(GVector3D<T>(0,0,1) );
            }
            c.Normalize();
            Set( c.x, c.y, c.z, 0 );
         }
         else
         {
            c = l_FromCopy.CrossProduct(l_ToCopy);
            Set(c.x / s, c.y / s, c.z / s, s * (T) 0.5);
         }
      }
   };
}

#endif // #ifndef _GOLEM_QUATERNION_
