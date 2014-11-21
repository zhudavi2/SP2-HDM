/**************************************************************
*
* golem_matrix.h
*
* Description
* ===========
*  Description & implementation of a 4x4 matrix
*
* Owner
* =====
*  Germain Sauve & Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_MATRIX_
#define _GOLEM_MATRIX_

#include "golem_vector.h"

namespace Hector
{

   template<typename T>
   class GMatrix4x4
   {
      T  m_Value[4][4];
   public:
      //! Default constructor
      GMatrix4x4()
      {
      }

      //! Copy constructor
      GMatrix4x4(const GMatrix4x4<T>& in_Other)
      {
         memcpy(m_Value, in_Other.m_Value, sizeof(m_Value) );
      }

      //! Set this matrix to identity
      void LoadIdentity()
      {
         memset(m_Value, 0, sizeof(m_Value) );
         m_Value[0][0] = 1;
         m_Value[1][1] = 1;
         m_Value[2][2] = 1;
         m_Value[3][3] = 1;
      }

      float operator () ( UINT32 in_iRow, UINT32 in_iCol ) const
      {
         assert(in_iRow < 4);
         assert(in_iCol < 4);

         return m_Value[in_iRow][in_iCol]; 
      }

      float& operator () ( UINT32 in_iRow, UINT32 in_iCol )
      {
         assert(in_iRow < 4);
         assert(in_iCol < 4);

         return m_Value[in_iRow][in_iCol]; 
      }

      GVector3D<T> operator * (const GVector3D<T>&  in_Vector) const
      {
         GVector3D<T> l_Result;
         l_Result.x =   m_Value[0][0] * in_Vector.x + 
            m_Value[1][0] * in_Vector.y + 
            m_Value[2][0] * in_Vector.z + 
            m_Value[3][0];

         l_Result.y =   m_Value[0][1] * in_Vector.x + 
            m_Value[1][1] * in_Vector.y + 
            m_Value[2][1] * in_Vector.z + 
            m_Value[3][1];

         l_Result.z =   m_Value[0][2] * in_Vector.x + 
            m_Value[1][2] * in_Vector.y + 
            m_Value[2][2] * in_Vector.z + 
            m_Value[3][2];

         return l_Result;
      }

      GVector4D<T> operator * (const GVector4D<T>&  in_Vector) const
      {
         GVector4D<T> l_Result;
         l_Result.x =   m_Value[0][0] * in_Vector.x + 
            m_Value[1][0] * in_Vector.y + 
            m_Value[2][0] * in_Vector.z + 
            m_Value[3][0] * in_Vector.w;

         l_Result.y =   m_Value[0][1] * in_Vector.x + 
            m_Value[1][1] * in_Vector.y + 
            m_Value[2][1] * in_Vector.z + 
            m_Value[3][1] * in_Vector.w;

         l_Result.z =   m_Value[0][2] * in_Vector.x + 
            m_Value[1][2] * in_Vector.y + 
            m_Value[2][2] * in_Vector.z + 
            m_Value[3][2] * in_Vector.w;

         l_Result.w =   m_Value[0][3] * in_Vector.x + 
            m_Value[1][3] * in_Vector.y + 
            m_Value[2][3] * in_Vector.z + 
            m_Value[3][3] * in_Vector.w;

         return l_Result;
      }

      GMatrix4x4<T> operator * (const GMatrix4x4<T>&  in_Matrix) const
      {
         GMatrix4x4<T> l_Result;

         for(UINT32 i = 0;i < 4;i ++)
         {
            for(UINT32 k = 0;k < 4;k ++)
            {
               l_Result.m_Value[i][k] = 0;

               for(UINT32 j = 0;j < 4;j ++)
               {
                  l_Result.m_Value[i][k] += m_Value[i][j] * in_Matrix.m_Value[j][k];
               }
            }
         }

         return l_Result;
      }

      GMatrix4x4<T> Transpose() const
      {
         GMatrix4x4<T> l_Dest;
         for(int j = 0;j < 4;j ++)
         {
            for(int i = 0;i < 4;i ++)
            {
               l_Dest.m_Value[i][j] = m_Value[j][i];
            }
         }

         return l_Dest;
      }

      GMatrix4x4<T> Inverse() const
      {
         // Note : Code in comment is for reference use

         GMatrix4x4<T> l_Dest;
         T l_Tmp[12];   // temp array for pairs
         GMatrix4x4<T> l_Src;   // array of transpose source matrix
         T l_Det;       // determinant

         // transpose matrix
         for ( UINT32 i = 0; i < 4; i++)
         {
            l_Src(0, i) = m_Value[i][0];
            l_Src(1, i) = m_Value[i][1];
            l_Src(2, i) = m_Value[i][2];
            l_Src(3, i) = m_Value[i][3];
         }

         // calculate pairs for first 8 elements (cofactors)
         l_Tmp[0] = l_Src(2,2) * l_Src(3,3);//src[10] * src[15];
         l_Tmp[1] = l_Src(2,3) * l_Src(3,2);//src[11] * src[14];
         l_Tmp[2] = l_Src(2,1) * l_Src(3,3);//src[9] * src[15];
         l_Tmp[3] = l_Src(2,3) * l_Src(3,1);//src[11] * src[13];
         l_Tmp[4] = l_Src(2,1) * l_Src(3,2);//src[9] * src[14];
         l_Tmp[5] = l_Src(2,2) * l_Src(3,1);//src[10] * src[13];
         l_Tmp[6] = l_Src(2,0) * l_Src(3,3);//src[8] * src[15];
         l_Tmp[7] = l_Src(2,3) * l_Src(3,0);//src[11] * src[12];
         l_Tmp[8] = l_Src(2,0) * l_Src(3,2);//src[8] * src[14];
         l_Tmp[9] = l_Src(2,2) * l_Src(3,0);//src[10] * src[12];
         l_Tmp[10]= l_Src(2,0) * l_Src(3,1);//src[8] * src[13];
         l_Tmp[11]= l_Src(2,1) * l_Src(3,0);//src[9] * src[12];

         // calculate first 8 elements (cofactors)
         //dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
         //dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
         l_Dest(0,0) = (l_Tmp[0]*l_Src(1,1) + l_Tmp[3]*l_Src(1,2) + l_Tmp[4]*l_Src(1,3) )
            - (l_Tmp[1]*l_Src(1,1) + l_Tmp[2]*l_Src(1,2) + l_Tmp[5]*l_Src(1,3) );
         //dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
         //dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
         l_Dest(0,1) = (l_Tmp[1]*l_Src(1,0) + l_Tmp[6]*l_Src(1,2) + l_Tmp[9]*l_Src(1,3) )
            - (l_Tmp[0]*l_Src(1,0) + l_Tmp[7]*l_Src(1,2) + l_Tmp[8]*l_Src(1,3) );
         //dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
         //dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
         l_Dest(0,2) = (l_Tmp[2]*l_Src(1,0) + l_Tmp[7]*l_Src(1,1) + l_Tmp[10]*l_Src(1,3) )
            - (l_Tmp[3]*l_Src(1,0) + l_Tmp[6]*l_Src(1,1) + l_Tmp[11]*l_Src(1,3) );
         //dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
         //dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
         l_Dest(0,3) = (l_Tmp[5]*l_Src(1,0) + l_Tmp[8]*l_Src(1,1) + l_Tmp[11]*l_Src(1,2) )
            - (l_Tmp[4]*l_Src(1,0) + l_Tmp[9]*l_Src(1,1) + l_Tmp[10]*l_Src(1,2) );
         //dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
         //dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
         l_Dest(1,0) = (l_Tmp[1]*l_Src(0,1) + l_Tmp[2]*l_Src(0,2) + l_Tmp[5]*l_Src(0,3) )
            - (l_Tmp[0]*l_Src(0,1) + l_Tmp[3]*l_Src(0,2) + l_Tmp[4]*l_Src(0,3) );
         //dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
         //dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
         l_Dest(1,1) = (l_Tmp[0]*l_Src(0,0) + l_Tmp[7]*l_Src(0,2) + l_Tmp[8]*l_Src(0,3) )
            - (l_Tmp[1]*l_Src(0,0) + l_Tmp[6]*l_Src(0,2) + l_Tmp[9]*l_Src(0,3) );
         //dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
         //dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
         l_Dest(1,2) = (l_Tmp[3]*l_Src(0,0) + l_Tmp[6]*l_Src(0,1) + l_Tmp[11]*l_Src(0,3) )
            - (l_Tmp[2]*l_Src(0,0) + l_Tmp[7]*l_Src(0,1) + l_Tmp[10]*l_Src(0,3) );
         //dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
         //dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
         l_Dest(1,3) = (l_Tmp[4]*l_Src(0,0) + l_Tmp[9]*l_Src(0,1) + l_Tmp[10]*l_Src(0,2) )
            - (l_Tmp[5]*l_Src(0,0) + l_Tmp[8]*l_Src(0,1) + l_Tmp[11]*l_Src(0,2) );

         // calculate pairs for second 8 elements (cofactors)
         l_Tmp[0] = l_Src(0,2) * l_Src(1,3);//src[2]*src[7];
         l_Tmp[1] = l_Src(0,3) * l_Src(1,2);//src[3]*src[6];
         l_Tmp[2] = l_Src(0,1) * l_Src(1,3);//src[1]*src[7];
         l_Tmp[3] = l_Src(0,3) * l_Src(1,1);//src[3]*src[5];
         l_Tmp[4] = l_Src(0,1) * l_Src(1,2);//src[1]*src[6];
         l_Tmp[5] = l_Src(0,2) * l_Src(1,1);//src[2]*src[5];
         l_Tmp[6] = l_Src(0,0) * l_Src(1,3);//src[0]*src[7];
         l_Tmp[7] = l_Src(0,3) * l_Src(1,0);//src[3]*src[4];
         l_Tmp[8] = l_Src(0,0) * l_Src(1,2);//src[0]*src[6];
         l_Tmp[9] = l_Src(0,2) * l_Src(1,0);//src[2]*src[4];
         l_Tmp[10]= l_Src(0,0) * l_Src(1,1);//src[0]*src[5];
         l_Tmp[11]= l_Src(0,1) * l_Src(1,0);//src[1]*src[4];

         // calculate second 8 elements (cofactors)
         //dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
         //dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
         l_Dest(2,0) = (l_Tmp[0]*l_Src(3,1) + l_Tmp[3]*l_Src(3,2) + l_Tmp[4]*l_Src(3,3) )
            - (l_Tmp[1]*l_Src(3,1) + l_Tmp[2]*l_Src(3,2) + l_Tmp[5]*l_Src(3,3) );
         //dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
         //dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
         l_Dest(2,1) = (l_Tmp[1]*l_Src(3,0) + l_Tmp[6]*l_Src(3,2) + l_Tmp[9]*l_Src(3,3) )
            - (l_Tmp[0]*l_Src(3,0) + l_Tmp[7]*l_Src(3,2) + l_Tmp[8]*l_Src(3,3) );
         //dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
         //dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
         l_Dest(2,2) = (l_Tmp[2]*l_Src(3,0) + l_Tmp[7]*l_Src(3,1) + l_Tmp[10]*l_Src(3,3) )
            - (l_Tmp[3]*l_Src(3,0) + l_Tmp[6]*l_Src(3,1) + l_Tmp[11]*l_Src(3,3) );
         //dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
         //dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
         l_Dest(2,3) = (l_Tmp[5]*l_Src(3,0) + l_Tmp[8]*l_Src(3,1) + l_Tmp[11]*l_Src(3,2) )
            - (l_Tmp[4]*l_Src(3,0) + l_Tmp[9]*l_Src(3,1) + l_Tmp[10]*l_Src(3,2) );
         //dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
         //dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
         l_Dest(3,0) = (l_Tmp[2]*l_Src(2,2) + l_Tmp[5]*l_Src(2,3) + l_Tmp[1]*l_Src(2,1) )
            - (l_Tmp[4]*l_Src(2,3) + l_Tmp[0]*l_Src(2,1) + l_Tmp[3]*l_Src(2,2) );
         //dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
         //dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
         l_Dest(3,1) = (l_Tmp[8]*l_Src(2,3) + l_Tmp[0]*l_Src(2,0) + l_Tmp[7]*l_Src(2,2) )
            - (l_Tmp[6]*l_Src(2,2) + l_Tmp[9]*l_Src(2,3) + l_Tmp[1]*l_Src(2,0) );
         //dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
         //dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
         l_Dest(3,2) = (l_Tmp[6]*l_Src(2,1) + l_Tmp[11]*l_Src(2,3) + l_Tmp[3]*l_Src(2,0) )
            - (l_Tmp[10]*l_Src(2,3) + l_Tmp[2]*l_Src(2,0) + l_Tmp[7]*l_Src(2,1) );
         //dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
         //dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
         l_Dest(3,3) = (l_Tmp[10]*l_Src(2,2) + l_Tmp[4]*l_Src(2,0) + l_Tmp[9]*l_Src(2,1) )
            - (l_Tmp[8]*l_Src(2,1) + l_Tmp[11]*l_Src(2,2) + l_Tmp[5]*l_Src(2,0) );

         // calculate determinant
         //l_Det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
         l_Det =  l_Src(0,0)*l_Dest(0,0) + 
            l_Src(0,1)*l_Dest(0,1) + 
            l_Src(0,2)*l_Dest(0,2) + 
            l_Src(0,3)*l_Dest(0,3);

         // calculate matrix inverse
         l_Det =  1 / l_Det;
         for ( UINT32 j = 0; j < 4; j++)
         {
            l_Dest(j,0) *= l_Det;
            l_Dest(j,1) *= l_Det;
            l_Dest(j,2) *= l_Det;
            l_Dest(j,3) *= l_Det;
         }

         return l_Dest;
      }

      GVector3D<T> Scaling()
      {
         GVector3D<REAL32> l_Right(m_Value[0][0], m_Value[1][0], m_Value[2][0] );
         GVector3D<REAL32> l_Up(m_Value[0][1], m_Value[1][1], m_Value[2][1] );
         GVector3D<REAL32> l_Out(m_Value[0][2], m_Value[1][2], m_Value[2][2] );

         return GVector3D<REAL32>(l_Right.Length(), l_Up.Length(), l_Out.Length() );
      }

      GVector3D<T> Translation()
      {
         return GVector3D<REAL32>(m_Value[3][0], m_Value[3][1], m_Value[3][2] );
      }

      GVector3D<T> EulerRotation()
      {
         GVector3D<T> l_Scale = Scaling();
         GMatrix4x4<T> l_ScaleMatInv;
         l_ScaleMatInv.LoadIdentity();
         l_ScaleMatInv(0,0) = (T) 1 / l_Scale.x;
         l_ScaleMatInv(1,1) = (T) 1 / l_Scale.y;
         l_ScaleMatInv(2,2) = (T) 1 / l_Scale.z;

         GMatrix4x4<T> l_RotMat = *this * l_ScaleMatInv;

         GVector3D<REAL32> l_Rot;

         l_Rot.y = (T) asin(l_RotMat(0, 2) );   // Calculate Y-axis angle
         T C = cos(l_Rot.y);
         if(abs(C) > (T) 0.001)                 // Gimball lock?
         {
            T l_Trx  =  l_RotMat(2, 2) / C;     // No, so get X-axis angle
            T l_Try  = -l_RotMat(1, 2)  / C;
            l_Rot.x  =  atan2( l_Try, l_Trx );
            l_Trx    =  l_RotMat(0, 0) / C;     // Get Z-axis angle
            l_Try    = -l_RotMat(0, 1) / C;
            l_Rot.z  =  atan2( l_Try, l_Trx );
         }
         else                                   // Gimball lock has occurred
         {
            l_Rot.x  =  0;                      // Set X-axis angle to zero
            T l_Trx  =  l_RotMat(1, 1);         // And calculate Z-axis angle
            T l_Try  =  l_RotMat(1, 0);
            l_Rot.z  =  atan2( l_Try, l_Trx );
         }

         return l_Rot;
      }


      void ChangeTransform(GVector3D<T>& in_Scale, 
                           GVector3D<T>& in_EulerRotation, 
                           GVector3D<T>& in_Translation)
      {
         T A      = cos(in_EulerRotation.x);
         T B      = sin(in_EulerRotation.x);
         T C      = cos(in_EulerRotation.y);
         T D      = sin(in_EulerRotation.y);
         T E      = cos(in_EulerRotation.z);
         T F      = sin(in_EulerRotation.z);
         T AD     =   A * D;
         T BD     =   B * D;

         m_Value[0][0] =  C * E * in_Scale.x;
         m_Value[0][1] = -C * F * in_Scale.y;
         m_Value[0][2] =  D     * in_Scale.z;
         m_Value[0][3] =  (T) 0;

         m_Value[1][0] =  BD * E + A * F * in_Scale.x;
         m_Value[1][1] = -BD * F + A * E * in_Scale.y;
         m_Value[1][2] = -B * C * in_Scale.z;
         m_Value[1][3] =  (T) 0;

         m_Value[2][0] = -AD * E + B * F * in_Scale.x;
         m_Value[2][1] =  AD * F + B * E * in_Scale.y;
         m_Value[2][2] =  A * C * in_Scale.z;
         m_Value[2][3] =  (T) 0;

         m_Value[3][0] =  in_Translation.x;
         m_Value[3][1] =  in_Translation.y;
         m_Value[3][2] =  in_Translation.z;
         m_Value[3][3] =  (T) 1;
      }
   };

}

#endif //#ifndef _GOLEM_MATRIX_