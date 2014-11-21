/**************************************************************
*
* golem_basic_math.h
*
* Description
* ===========
*	Classes that describes mathematical objects.
*
* Owner
* =====
*	Germain Sauve & Frederic O'Reilly
*
* History
* =======
*	13 sept 2002
*		- creation
*	s'4 novembre au soir : gsauve
*		- Added CVector3Di and CVector3Df
*		- Added CBox, CSphere, GRectangle and CRay
*	June 09 2003 (foreilly):
*		- Added the power of templates
*
**************************************************************/
#ifndef _GOLEM_BASIC_MATH_
#define _GOLEM_BASIC_MATH_

BEGIN_NAMESPACE(Hector)

namespace GL_MATH
{
	const REAL32 PI   = 3.14159265359f;
	const REAL32 EPSILON = 0.000001f;
};

template<typename T>
class CVector2D
{
public:
	//! First dimension component
	T	x;

	//! Second dimension component
	T	y;

	//! Default constructor
    CVector2D()
	{
	}

	//! Constructor with initialization
    CVector2D( T xi, T yi )
		: x(xi)
		, y(yi)
	{
	}

	//! Copy constructor
	template<typename Y>
	CVector2D( const CVector2D<Y>& in_Other )
		: x( (T) in_Other.x)
		, y( (T) in_Other.y)
	{
	}

	//! Set all dimension components
	void	Set(T in_X, T in_Y)
	{
		x = in_X;
		y = in_Y;
	}

	//! Returns true if other vector is equal to this vector
	bool	operator == ( const CVector2D<T>& in_Other ) const
	{
		return ( (x == in_Other.x) && (y == in_Other.y) );
	}

	//! Returns true if other vector is not equal to this vector
	bool	operator != ( const CVector2D<T>& in_Other ) const
	{
		return !(in_Other == *this);
	}

	//! Returns the ID'nth dimension component constant reference
	const T&	operator [] ( UINT32 in_Id ) const
	{
		assert(in_Id < 2);
		return *( (&x)[in_Id] );
	}

	//! Returns the ID'nth dimension component reference
	T&	operator [] ( UINT32 in_Id )
	{
		assert(in_Id < 2);
		return *( (&x)[in_Id] );
	}

	//! Returns a new vector containing the sum of other vector and this vector
	CVector2D<T>	operator + ( const CVector2D<T>& in_Other ) const
	{
		return CVector2D<T>( x + in_Other.x, y + in_Other.y ); 
	}

	//! Returns a new vector containing the difference of other vector and this vector
	CVector2D<T>	operator - ( const CVector2D<T>& in_Other ) const
	{
		return CVector2D<T>( x - in_Other.x, y - in_Other.y ); 
	}

	//! Returns a new vector containing the product of a scalar and this vector
	CVector2D<T>	operator * ( T in_Scalar ) const
	{
		return CVector2D<T>( x * in_Scalar, y * in_Scalar ); 
	}

	//! Returns a new vector containing the quotient of a scalar and this vector
	CVector2D<T>	operator / ( T in_Scalar ) const
	{
		return CVector2D<T>( x / in_Scalar, y / in_Scalar ); 
	}

	//! Set this vector to the result of the sum of other vector and this vector
	CVector2D<T>&	operator += ( const CVector2D<T>& in_Other )
	{
		x += in_Other.x;
		y += in_Other.y;
		return *this;
	}

	//! Set this vector to the result of the difference of other vector and this vector
	CVector2D<T>&	operator -= ( const CVector2D<T>& in_Other )
	{
		x -= in_Other.x;
		y -= in_Other.y;
		return *this;
	}

	//! Set this vector to the result of the product of a scalar and this vector
	CVector2D<T>&	operator *= ( T in_Scalar )
	{
		x *= in_Scalar;
		y *= in_Scalar;
		return *this;
	}

	//! Set this vector to the result of the quotient of a scalar and this vector
	CVector2D<T>&	operator /= ( T in_Scalar )
	{
		x /= in_Scalar;
		y /= in_Scalar;
		return *this;
	}

	//! Returns the length of this vector
	T	Length() const
	{
		return (T) sqrt( (x * x) + (y * y) );
	}

	//! Returns the squared length of this vector
	T	Length_Square() const
	{
		return (x * x) + (y * y);
	}

	//! Make sure the vector is 1 unit long
	void	Normalize()
	{
		T l_Length = Lenght();

		x /= l_Length;
		y /= l_Length;
	}

	//! Returns true only if vector is at origin (0,0)
	bool	Is_Zero() const
	{
		return ( (x == 0) && (y == 0) )
	}

	//! Compute the distance between two vector end-points
	T	Distance( const CVector2D<T>& in_Other ) const
	{
		T l_DeltaX = x - in_Other.x;
		T l_DeltaY = y - in_Other.y;

		return (T) sqrt( (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) );
	}

	//! Compute the squared distance between two vector end-points
	T	Distance_Square( const CVector2D<T>& in_Other ) const
	{
		T l_DeltaX = x - in_Other.x;
		T l_DeltaY = y - in_Other.y;

		return (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY);
	}

	//! Returns the dot product of this vector and another vector
	REAL32	Dot_Product(const CVector2D<T>& in_Other) const
	{
		return x * in_Other.x + y * in_Other.y;
	}

	//! Returns the "z" component of the cross product of this vector and another vector
	REAL32	Cross_Product(const CVector2D<T>& in_Other) const
	{
		return x * in_Other.y - y * in_Other.x;
	}
};

typedef CVector2D<INT32>	CVector2Di;
typedef CVector2D<REAL32>	CVector2D;
typedef CVector2D<REAL64>	CVector2Dr;

template<typename T>
class CVector3D
{
public:
	//! First dimension component
	T	x;

	//! Second dimension component
	T	y;

	//! Third dimension component
	T	z;

	//! Default constructor
    CVector3D()
	{
	}

	//! Constructor with initialization
    CVector3D( T xi, T yi, T zi )
		: x(xi)
		, y(yi)
		, z(zi)
	{
	}

	//! Copy constructor
	template<typename Y>
	CVector3D( const CVector3D<Y>& in_Other )
		: x( (T) in_Other.x)
		, y( (T) in_Other.y)
		, z( (T) in_Other.z)
	{
	}

	//! Conversion constructor
	template<typename Y>
	CVector3D( const CVector2D<Y>& in_Other )
		: x(in_Other.x)
		, y(in_Other.y)
		, z(0)
	{
	}

	//! Convert this 3D vector to a 2D vector
	template<typename Y>
	operator CVector2D<Y> () const
	{
		return CVector2D<Y>( (Y) x, (Y) y);
	}

	//! Set all dimension components
	void	Set(T in_X, T in_Y, T in_Z)
	{
		x = in_X;
		y = in_Y;
		z = in_Z;
	}

	//! Returns true if other vector is equal to this vector
	bool	operator == ( const CVector3D<T>& in_Other ) const
	{
		return ( (x == in_Other.x) && (y == in_Other.y) && (z == in_Other.z));
	}

	//! Returns true if other vector is not equal to this vector
	bool	operator != ( const CVector3D<T>& in_Other ) const
	{
		return !(in_Other == *this);
	}

	//! Returns the ID'nth dimension component constant reference
	const T&	operator [] ( UINT32 in_Id ) const
	{
		assert(in_Id < 3);
		return *( (&x)[in_Id] );
	}

	//! Returns the ID'nth dimension component reference
	T&	operator [] ( UINT32 in_Id )
	{
		assert(in_Id < 3);
		return *( (&x)[in_Id] );
	}

	//! Returns a new vector containing the sum of other vector and this vector
	CVector3D<T>	operator + ( const CVector3D<T>& in_Other ) const
	{
		return CVector3D<T>( x + in_Other.x, y + in_Other.y, z + in_Other.z ); 
	}

	//! Returns a new vector containing the difference of other vector and this vector
	CVector3D<T>	operator - ( const CVector3D<T>& in_Other ) const
	{
		return CVector3D<T>( x - in_Other.x, y - in_Other.y, z - in_Other.z ); 
	}

	//! Returns a new vector containing the product of a scalar and this vector
	CVector3D<T>	operator * ( T in_Scalar ) const
	{
		return CVector3D<T>( x * in_Scalar, y * in_Scalar, z * in_Scalar ); 
	}

	//! Returns a new vector containing the quotient of a scalar and this vector
	CVector3D<T>	operator / ( T in_Scalar ) const
	{
		return CVector3D<T>( x / in_Scalar, y / in_Scalar, z / in_Scalar ); 
	}

	//! Set this vector to the result of the sum of other vector and this vector
	CVector3D<T>&	operator += ( const CVector3D<T>& in_Other )
	{
		x += in_Other.x;
		y += in_Other.y;
		z += in_Other.z;
		return *this;
	}

	//! Set this vector to the result of the difference of other vector and this vector
	CVector3D<T>&	operator -= ( const CVector3D<T>& in_Other )
	{
		x -= in_Other.x;
		y -= in_Other.y;
		z -= in_Other.z;
		return *this;
	}

	//! Set this vector to the result of the product of a scalar and this vector
	CVector3D<T>&	operator *= ( T in_Scalar )
	{
		x *= in_Scalar;
		y *= in_Scalar;
		z *= in_Scalar;
		return *this;
	}

	//! Set this vector to the result of the quotient of a scalar and this vector
	CVector3D<T>&	operator /= ( T in_Scalar )
	{
		x /= in_Scalar;
		y /= in_Scalar;
		z /= in_Scalar;
		return *this;
	}

	//! Returns the dot product of this vector and another vector
	REAL32	Dot_Product(const CVector3D<T>& in_Other) const
	{
		return x * in_Other.x + y * in_Other.y + z * in_Other.z;
	}

	//! Returns the cross product of this vector and another vector
	CVector3D<T>	Cross_Product(const CVector3D<T>& in_Other) const
	{
		CVector3Df l_Result;

		l_Result.x = y * in_Other.z - z * in_Other.y;
		l_Result.y = z * in_Other.x - x * in_Other.z;
		l_Result.z = x * in_Other.y - y * in_Other.x;

		return l_Result;
	}

	//! Returns the length of this vector
	T	Length() const
	{
		return (T) sqrt( (x * x) + (y * y) + (z * z) );
	}

	//! Returns the squared length of this vector
	T	Length_Square() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	//! Make sure the vector is 1 unit long
	void	Normalize()
	{
		T l_Length = Length();

		x /= l_Length;
		y /= l_Length;
		z /= l_Length;
	}

	//! Returns true only if vector is at origin (0,0,0)
	bool	Is_Zero() const
	{
		return ( (x == 0) && (y == 0) && (z == 0) )
	}

	//! Compute the distance between two vector end-points
	T	Distance( const CVector3D<T>& in_Other ) const
	{
		T l_DeltaX = x - in_Other.x;
		T l_DeltaY = y - in_Other.y;
		T l_DeltaZ = z - in_Other.z;

		return (T) sqrt( (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) + (l_DeltaZ * l_DeltaZ) );
	}

	//! Compute the squared distance between two vector end-points
	T	Distance_Square( const CVector3D<T>& in_Other ) const
	{
		T l_DeltaX = x - in_Other.x;
		T l_DeltaY = y - in_Other.y;
		T l_DeltaZ = z - in_Other.z;

		return (l_DeltaX * l_DeltaX) + (l_DeltaY * l_DeltaY) + (l_DeltaZ * l_DeltaZ);
	}
};

typedef CVector3D<INT32>	CVector3Di;
typedef CVector3D<REAL32>	CVector3Df;
typedef CVector3D<REAL64>	CVector3Dr;

template<typename T>
class CMatrix4x4
{
	T	m_Value[4][4];
public:
	//! Default constructor
	CMatrix4x4()
	{
	}

	//! Copy constructor
	CMatrix4x4(const CMatrix4x4<T>& in_Other)
	{
		memcpy(m_Value, in_Other.m_Value, sizeof(m_Value) );
	}

	float operator () ( UINT32 in_Row, UINT32 in_Col ) const
	{
		assert(in_Row < 4);
		assert(in_Col < 4);

		return m_Value[in_Row][in_Col]; 
	}

	float& operator () ( UINT32 in_Row, UINT32 in_Col )
	{
		assert(in_Row < 4);
		assert(in_Col < 4);

		return m_Value[in_Row][in_Col]; 
	}

	CVector3D<T> operator * (const CVector3D<T>&  in_Vector) const
	{
		CVector3D<T> l_Result;
		l_Result.x =	m_Value[0][0] * in_Vector.x + 
						m_Value[1][0] * in_Vector.y + 
						m_Value[2][0] * in_Vector.z + 
						m_Value[3][0];

		l_Result.y =	m_Value[0][1] * in_Vector.x + 
						m_Value[1][1] * in_Vector.y + 
						m_Value[2][1] * in_Vector.z + 
						m_Value[3][1];

		l_Result.z =	m_Value[0][2] * in_Vector.x + 
						m_Value[1][2] * in_Vector.y + 
						m_Value[2][2] * in_Vector.z + 
						m_Value[3][2];

		return l_Result;
	}

	CMatrix4x4<T> Transpose() const
	{
		CMatrix4x4<T> l_Dest;
		for(int j = 0;j < 4;j ++)
		{
			for(int i = 0;i < 4;i ++)
			{
				l_Dest.m_Value[i][j] = m_Value[j][i];
			}
		}

		return l_Dest;
	}

	CMatrix4x4<T> Inverse() const
	{
		// Note : Code in comment is for reference use

		CMatrix4x4<T> l_Dest;
		T l_Tmp[12];	// temp array for pairs
		CMatrix4x4<T> l_Src;	// array of transpose source matrix
		T l_Det;			// determinant

		// transpose matrix
		for ( UINT32 i = 0; i < 4; i++)
		{
			l_Src(0, i) = m_Value[i][0];
			l_Src(1, i) = m_Value[i][1];
			l_Src(2, i) = m_Value[i][2];
			l_Src(3, i) = m_Value[i][3];
		}

		// calculate pairs for first 8 elements (cofactors)
		l_Tmp[0]	= l_Src(2,2) * l_Src(3,3);//src[10] * src[15];
		l_Tmp[1]	= l_Src(2,3) * l_Src(3,2);//src[11] * src[14];
		l_Tmp[2]	= l_Src(2,1) * l_Src(3,3);//src[9] * src[15];
		l_Tmp[3]	= l_Src(2,3) * l_Src(3,1);//src[11] * src[13];
		l_Tmp[4]	= l_Src(2,1) * l_Src(3,2);//src[9] * src[14];
		l_Tmp[5]	= l_Src(2,2) * l_Src(3,1);//src[10] * src[13];
		l_Tmp[6]	= l_Src(2,0) * l_Src(3,3);//src[8] * src[15];
		l_Tmp[7]	= l_Src(2,3) * l_Src(3,0);//src[11] * src[12];
		l_Tmp[8]	= l_Src(2,0) * l_Src(3,2);//src[8] * src[14];
		l_Tmp[9]	= l_Src(2,2) * l_Src(3,0);//src[10] * src[12];
		l_Tmp[10]	= l_Src(2,0) * l_Src(3,1);//src[8] * src[13];
		l_Tmp[11]	= l_Src(2,1) * l_Src(3,0);//src[9] * src[12];

		// calculate first 8 elements (cofactors)
		//dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
		//dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
		l_Dest(0,0)	= (l_Tmp[0]*l_Src(1,1) + l_Tmp[3]*l_Src(1,2) + l_Tmp[4]*l_Src(1,3) )
					- (l_Tmp[1]*l_Src(1,1) + l_Tmp[2]*l_Src(1,2) + l_Tmp[5]*l_Src(1,3) );
		//dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
		//dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
		l_Dest(0,1)	= (l_Tmp[1]*l_Src(1,0) + l_Tmp[6]*l_Src(1,2) + l_Tmp[9]*l_Src(1,3) )
					- (l_Tmp[0]*l_Src(1,0) + l_Tmp[7]*l_Src(1,2) + l_Tmp[8]*l_Src(1,3) );
		//dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
		//dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
		l_Dest(0,2)	= (l_Tmp[2]*l_Src(1,0) + l_Tmp[7]*l_Src(1,1) + l_Tmp[10]*l_Src(1,3) )
					- (l_Tmp[3]*l_Src(1,0) + l_Tmp[6]*l_Src(1,1) + l_Tmp[11]*l_Src(1,3) );
		//dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
		//dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
		l_Dest(0,3)	= (l_Tmp[5]*l_Src(1,0) + l_Tmp[8]*l_Src(1,1) + l_Tmp[11]*l_Src(1,2) )
					- (l_Tmp[4]*l_Src(1,0) + l_Tmp[9]*l_Src(1,1) + l_Tmp[10]*l_Src(1,2) );
		//dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
		//dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
		l_Dest(1,0)	= (l_Tmp[1]*l_Src(0,1) + l_Tmp[2]*l_Src(0,2) + l_Tmp[5]*l_Src(0,3) )
					- (l_Tmp[0]*l_Src(0,1) + l_Tmp[3]*l_Src(0,2) + l_Tmp[4]*l_Src(0,3) );
		//dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
		//dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
		l_Dest(1,1)	= (l_Tmp[0]*l_Src(0,0) + l_Tmp[7]*l_Src(0,2) + l_Tmp[8]*l_Src(0,3) )
					- (l_Tmp[1]*l_Src(0,0) + l_Tmp[6]*l_Src(0,2) + l_Tmp[9]*l_Src(0,3) );
		//dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
		//dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
		l_Dest(1,2)	= (l_Tmp[3]*l_Src(0,0) + l_Tmp[6]*l_Src(0,1) + l_Tmp[11]*l_Src(0,3) )
					- (l_Tmp[2]*l_Src(0,0) + l_Tmp[7]*l_Src(0,1) + l_Tmp[10]*l_Src(0,3) );
		//dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
		//dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
		l_Dest(1,3)	= (l_Tmp[4]*l_Src(0,0) + l_Tmp[9]*l_Src(0,1) + l_Tmp[10]*l_Src(0,2) )
					- (l_Tmp[5]*l_Src(0,0) + l_Tmp[8]*l_Src(0,1) + l_Tmp[11]*l_Src(0,2) );

		// calculate pairs for second 8 elements (cofactors)
		l_Tmp[0]	= l_Src(0,2) * l_Src(1,3);//src[2]*src[7];
		l_Tmp[1]	= l_Src(0,3) * l_Src(1,2);//src[3]*src[6];
		l_Tmp[2]	= l_Src(0,1) * l_Src(1,3);//src[1]*src[7];
		l_Tmp[3]	= l_Src(0,3) * l_Src(1,1);//src[3]*src[5];
		l_Tmp[4]	= l_Src(0,1) * l_Src(1,2);//src[1]*src[6];
		l_Tmp[5]	= l_Src(0,2) * l_Src(1,1);//src[2]*src[5];
		l_Tmp[6]	= l_Src(0,0) * l_Src(1,3);//src[0]*src[7];
		l_Tmp[7]	= l_Src(0,3) * l_Src(1,0);//src[3]*src[4];
		l_Tmp[8]	= l_Src(0,0) * l_Src(1,2);//src[0]*src[6];
		l_Tmp[9]	= l_Src(0,2) * l_Src(1,0);//src[2]*src[4];
		l_Tmp[10]	= l_Src(0,0) * l_Src(1,1);//src[0]*src[5];
		l_Tmp[11]	= l_Src(0,1) * l_Src(1,0);//src[1]*src[4];

		// calculate second 8 elements (cofactors)
		//dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
		//dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
		l_Dest(2,0)	= (l_Tmp[0]*l_Src(3,1) + l_Tmp[3]*l_Src(3,2) + l_Tmp[4]*l_Src(3,3) )
					- (l_Tmp[1]*l_Src(3,1) + l_Tmp[2]*l_Src(3,2) + l_Tmp[5]*l_Src(3,3) );
		//dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
		//dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
		l_Dest(2,1)	= (l_Tmp[1]*l_Src(3,0) + l_Tmp[6]*l_Src(3,2) + l_Tmp[9]*l_Src(3,3) )
					- (l_Tmp[0]*l_Src(3,0) + l_Tmp[7]*l_Src(3,2) + l_Tmp[8]*l_Src(3,3) );
		//dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
		//dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
		l_Dest(2,2)	= (l_Tmp[2]*l_Src(3,0) + l_Tmp[7]*l_Src(3,1) + l_Tmp[10]*l_Src(3,3) )
					- (l_Tmp[3]*l_Src(3,0) + l_Tmp[6]*l_Src(3,1) + l_Tmp[11]*l_Src(3,3) );
		//dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
		//dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
		l_Dest(2,3)	= (l_Tmp[5]*l_Src(3,0) + l_Tmp[8]*l_Src(3,1) + l_Tmp[11]*l_Src(3,2) )
					- (l_Tmp[4]*l_Src(3,0) + l_Tmp[9]*l_Src(3,1) + l_Tmp[10]*l_Src(3,2) );
		//dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
		//dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
		l_Dest(3,0)	= (l_Tmp[2]*l_Src(2,2) + l_Tmp[5]*l_Src(2,3) + l_Tmp[3]*l_Src(2,1) )
					- (l_Tmp[4]*l_Src(2,3) + l_Tmp[0]*l_Src(2,1) + l_Tmp[1]*l_Src(2,2) );
		//dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
		//dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
		l_Dest(3,1)	= (l_Tmp[8]*l_Src(2,3) + l_Tmp[0]*l_Src(2,0) + l_Tmp[7]*l_Src(2,2) )
					- (l_Tmp[6]*l_Src(2,2) + l_Tmp[9]*l_Src(2,3) + l_Tmp[1]*l_Src(2,0) );
		//dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
		//dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
		l_Dest(3,2)	= (l_Tmp[6]*l_Src(2,1) + l_Tmp[11]*l_Src(2,3) + l_Tmp[3]*l_Src(2,0) )
					- (l_Tmp[10]*l_Src(2,3) + l_Tmp[2]*l_Src(2,0) + l_Tmp[7]*l_Src(2,1) );
		//dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
		//dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
		l_Dest(3,3)	= (l_Tmp[10]*l_Src(2,2) + l_Tmp[4]*l_Src(2,0) + l_Tmp[9]*l_Src(2,1) )
					- (l_Tmp[8]*l_Src(2,1) + l_Tmp[11]*l_Src(2,2) + l_Tmp[5]*l_Src(2,0) );

		// calculate determinant
		//l_Det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
		l_Det	=	l_Src(0,0)*l_Dest(0,0) + 
					l_Src(0,1)*l_Dest(0,1) + 
					l_Src(0,2)*l_Dest(0,2) + 
					l_Src(0,3)*l_Dest(0,3);

		// calculate matrix inverse
		l_Det	=	1 / l_Det;
		for ( UINT32 j = 0; j < 4; j++)
		{
			l_Dest(j,0) *= l_Det;
			l_Dest(j,1) *= l_Det;
			l_Dest(j,2) *= l_Det;
			l_Dest(j,3) *= l_Det;
		}

		return l_Dest;
	}
};

typedef CMatrix4x4<REAL32>	CMatrix4x4f;

/**********************************************

class GRectangle

// Rectangle in 2D

***********************************************/
class GRectangle
    {
public:
    float m_left, m_top, m_width, m_height;

    GRectangle()
        {
        m_left = 0;
        m_top = 0;
        m_width = 0;
        m_height = 0;
        }
    GRectangle( float pLeft, float pTop, float pWidth, float pHeight ) : 
        m_left(pLeft), m_top(pTop), m_width(pWidth), m_height(pHeight) {}
         
    GRectangle( CVector2Df& pTopLeft, CVector2Df& pWidthHeight )
        {
        m_left = pTopLeft.x;
        m_top = pTopLeft.y;
        m_width = pWidthHeight.x;
        m_height = pWidthHeight.y;
        }

    GRectangle( GRectangle& pOther )
        {
        m_left = pOther.m_left;
        m_top = pOther.m_top;
        m_width = pOther.m_width;
        m_height = pOther.m_height;
        }

    void Set( float pLeft, float pTop, float pWidth, float pHeight )
        {
        m_left = pLeft;
        m_top = pTop;
        m_width = pWidth;
        m_height = pHeight;
        }

    void Set( CVector2Df& pLeftTop, CVector2Df pWidthHeight )
        {
        m_left = pLeftTop.x;
        m_top = pLeftTop.y;
        m_width = pWidthHeight.x;
        m_height = pWidthHeight.y;
        }

    float Left()	const	{ return m_left; }
    float Top()		const	{ return m_top; }
    float Width()	const	{ return m_width; }
    float Height()	const	{ return m_height; }
    float Right()	const	{ return m_left + m_width; }
    float Bottom()	const	{ return m_top + m_height; }

    void Left( float pValue )      { m_left = pValue; }
    void Top( float pValue )       { m_top = pValue; }
    void Width( float pValue )     { m_width = pValue; }
    void Height( float pValue )    { m_height = pValue; }
    void Right( float pValue )     { m_width = pValue - m_left; }
    void Bottom( float pValue )    { m_height = pValue - m_top; }

    void Inset( float pX, float pY )
        {
        m_left += pX;
        m_width -= pX;
        m_top += pY;
        m_height -= pY;
        }

    void Inset( CVector2Df& pInset )
        {
        m_left += pInset.x;
        m_width -= pInset.x;
        m_top += pInset.y;
        m_height -= pInset.y;
        }

    void Offset( float pX, float pY )
        {
        m_left += pX;
        m_top += pY;
        }

    void Offset( CVector2Df& pOffset )
        {
        m_left += pOffset.x;
        m_top += pOffset.y;
        }

    bool PointInRect( CVector2Di& pPoint )	const
        {
        if ( pPoint.x > Left() )
            {
            if ( pPoint.x < Right() )
                {
                if ( pPoint.y > Top() )
                    {
                    if ( pPoint.y < Bottom() )
                        {
                        return true;
                        }
                    }
                }
            }
        return false;
        }

    bool PointInRect( CVector2Df& pPoint )	const
        {
        if ( pPoint.x > Left() )
            {
            if ( pPoint.x < Right() )
                {
                if ( pPoint.y > Top() )
                    {
                    if ( pPoint.y < Bottom() )
                        {
                        return true;
                        }
                    }
                }
            }
        return false;
        }
    void Union( GRectangle& pOther )
        {
        // not optimise at all stie!
        float l1, l2, r1, r2, t1, t2, b1, b2;

        l1 = Left();
        l2 = pOther.Left();

        t1 = Top();
        t2 = pOther.Top();

        r1 = Right();
        r2 = pOther.Right();

        b1 = Bottom();
        b2 = pOther.Bottom();
        
        if ( l2 < l1 ) l1 = l2;
        if ( t2 < t1 ) t1 = t2;
        if ( r2 > r1 ) r1 = r2;
        if ( b2 > b1 ) b1 = b2; 
        
        Set( l1, t1, r1 - l1, b1 - t1 );
        }
    };

// Box in 3D
class CBox
    {
    CVector3Df m_LeftTopFront;
    CVector3Df m_WidthHeightDepth;
public:

    CBox()
        {
        m_LeftTopFront.Set( 0.0f, 0.0f, 0.0f );
        m_WidthHeightDepth.Set( 0.0f, 0.0f, 0.0f );
        }
    CBox( CVector3Df& pLeftTopFront, CVector3Df& pWidthHeightDepth )
        {
        m_LeftTopFront = pLeftTopFront;
        m_WidthHeightDepth = pWidthHeightDepth;
        } 
    CBox( CBox& pOther ) : m_LeftTopFront(pOther.m_LeftTopFront), m_WidthHeightDepth(pOther.m_WidthHeightDepth) {}
    
    float Left()	const	{ return m_LeftTopFront.x; }
    float Top()		const	{ return m_LeftTopFront.y; }
    float Front()	const	{ return m_LeftTopFront.z; }
    float Width()	const	{ return m_WidthHeightDepth.x; }
    float Height()	const	{ return m_WidthHeightDepth.y; }
    float Depth()	const	{ return m_WidthHeightDepth.z; }
    float Right()	const	{ return m_LeftTopFront.x + m_WidthHeightDepth.x; }
    float Bottom()	const	{ return m_LeftTopFront.y + m_WidthHeightDepth.y; }
    float Back()	const	{ return m_LeftTopFront.z + m_WidthHeightDepth.z; }

    void Left( float pValue )      { m_LeftTopFront.x = pValue; }
    void Top( float pValue )       { m_LeftTopFront.y = pValue; }
    void Front( float pValue )     { m_LeftTopFront.z = pValue; }
    void Width( float pValue )     { m_WidthHeightDepth.x = pValue; }
    void Height( float pValue )    { m_WidthHeightDepth.y = pValue; }
    void Depth( float pValue )     { m_WidthHeightDepth.z = pValue; }
    void Right( float pValue )     { m_WidthHeightDepth.x = pValue - m_LeftTopFront.x; }
    void Bottom( float pValue )    { m_WidthHeightDepth.y = pValue - m_LeftTopFront.y; }
    void Back( float pValue )      { m_WidthHeightDepth.z = pValue - m_LeftTopFront.z; }

    void Inset( CVector3Df& pInset )
        {
        m_LeftTopFront.x += pInset.x;
        m_LeftTopFront.y += pInset.y;
        m_LeftTopFront.z += pInset.z;
        m_WidthHeightDepth.x -= pInset.x;
        m_WidthHeightDepth.y -= pInset.y;
        m_WidthHeightDepth.z -= pInset.z;
        }

    void Offset( CVector3Df& pOffset )
        {
        m_LeftTopFront.x += pOffset.x;
        m_LeftTopFront.y += pOffset.y;
        m_LeftTopFront.z += pOffset.z;
        }
    };

// 3D sphere
class CSphere
    {
    CVector3Df  m_Center;
    float      m_Radius;
public:

    CSphere( CVector3Df& pCenter, float pRadius )
        {
        m_Center = pCenter;
        m_Radius = pRadius;
        }
    CSphere( CVector3Df& pCenter, CVector3Df& pPoint )
        {
        m_Center = pCenter;
        m_Radius = m_Center.Distance( pPoint ); // distance from m_Center to pPoint
        }

    CSphere ( const CSphere& pOther )
        {
        m_Center = pOther.m_Center;
        m_Radius = pOther.m_Radius;
        }

    void Offset( CVector3Df& pOffset )
        {
        m_Center += pOffset;
        }

    float Radius() { return m_Radius; }
    CVector3Df  Center() const { return m_Center; }

    void Radius( float pValue ) { m_Radius = pValue; }
    void Center( CVector3Df& pValue ) { m_Center = pValue; }
    };

// 3D ray
class CRay
    {
public:
    CVector3Df  m_Center;
    CVector3Df  m_Direction;

    CRay()
        {
        m_Center.Set( 0.0f, 0.0f, 0.0f );
        m_Direction.Set( 0.0f, 0.0f, 0.0f );
        }

    CRay( CVector3Df& pCenter, CVector3Df& pDir ) : m_Center(pCenter), m_Direction(pDir) {}

    CRay( const CRay& pOther )
        {
        m_Center = pOther.m_Center;
        m_Direction = pOther.m_Direction;
        }
    };

END_NAMESPACE(Hector)

#endif //#ifndef _GOLEM_BASIC_MATH_