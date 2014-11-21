/**************************************************************
*
* golem_gfx_particle_system_itf.h
*
* Description
* ===========
*	Class CGFX_Particle_System_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 29 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef	_GOLEM_GFX_PARTICLE_SYSTEM_ITF_
#define	_GOLEM_GFX_PARTICLE_SYSTEM_ITF_

#include "golem_gfx_primitive_itf.h"

namespace GFX
{
   class GMesh;

	//! Possible particle rendered primitive
	enum EParticle_Type
	{
		PARTICLE_NONE = 0,
		PARTICLE_LINE,
		PARTICLE_QUAD,
		PARTICLE_PRIMITIVE,
		PARTICLE_MODEL,
	};

	//! Possible particle alignment to world
	enum EParticle_Orientation
	{
		ORIENTATION_DEFAULT = 0,
		ORIENTATION_WORLD,
		ORIENTATION_CAMERA,
	};

	//! Possible particle emission types
	enum EParticle_Emission
	{
		EMISSION_POINT = 0,
		EMISSION_LINE,
		EMISSION_SQUARE,
		EMISSION_CIRCLE,
		EMISSION_PLANE,
		EMISSION_DISC,
		EMISSION_BOX,
		EMISSION_SPHERE_SURFACE,
		EMISSION_CUBE,
		EMISSION_SPHERE,
		EMISSION_CUSTOM_EDGE,
		EMISSION_CUSTOM_SURFACE,
		EMISSION_CUSTOM_VOLUME,
	};

};

//! Particle type description
class CGFX_Particle_Type
{
public:
	//! Particle type primitve
	GFX::EParticle_Type			m_Type;

	//! Particle type orientation
	GFX::EParticle_Orientation	m_Orientation;

	//! Particle object reference
	CGFX_Render_Obj_Itf*		m_Object;
};

//! Particle description class
class CGFX_Particle
{
public:
	//! Particle primitive index
	UINT32		m_TypeIndex;

	//! Particle emission position
	GVector3D<REAL32>	m_StartPos;

	//! Particle position
	GVector3D<REAL32>	m_Position;

	//! Particle speed
	GVector3D<REAL32>	m_Speed;

	//! Particle acceleration
	GVector3D<REAL32>	m_Accel;

	//! Particle rotation
	GVector3D<REAL32>	m_Rotation;

	//! Particle rotation speed
	GVector3D<REAL32>	m_RotationSpeed;

	//! Particle rotation acceleration
	GVector3D<REAL32>	m_RotationAccel;

	//! Particle scale
	GVector3D<REAL32>	m_Scale;

	//! Particle scale speed
	GVector3D<REAL32>	m_ScaleSpeed;

	//! Particle scale acceleration
	GVector3D<REAL32>	m_ScaleAccel;

	//! Particle color
	GColorRGBReal	m_Color;

	//! Particle luminescence
	REAL32		m_Luminescence;

	//! Particle life attenuation speed
	REAL32		m_LifeAttenuation;

	//! Particle life
	REAL32		m_Life;

	//! Particle life alpha
	REAL32		m_LifeAlpha;

	//! Particle life alpha speed
	REAL32		m_LifeAlphaSpeed;

	//! Particle life alpha acceleration
	REAL32		m_LifeAlphaAccel;

	//! Particle line length (in time)
	REAL32		m_LineTime;

	//! Particle time creation
	REAL32		m_Time;

	//! User data
	void*		m_UserData;

	//! Indicate that particle position must be updated with current world tranformation
	bool	m_PositionUpdate;

	//! Indicate that particle was newly created and needs initialization
	bool	m_New;
};

//! Particle_System class
class CGFX_Particle_System_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Particle_System_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Get the particle system animation controler
	//! This animation can be added to an action to animation the particle system
	virtual class CGFX_Animation_Itf*	Get_Animation() = 0;

	//! Generate a new particle animation sequence
	virtual bool Randomize() = 0;

	//! Generate a new particle animation sequence using a specific seed
	virtual bool Set_Seed(UINT32 in_Seed) = 0;

	//! Get the seed of the current particle animation sequence
	virtual UINT32 Get_Seed() = 0;

	//! Set the shape emission type and object
	virtual bool Set_Emission_Type(GFX::EParticle_Emission in_Type, GFX::GMesh* in_CustomShape = NULL) = 0;

	//! Get the shape emssion type
	virtual GFX::EParticle_Emission	Get_Emission_Type() = 0;

	//! Get the custom shape emission object
	virtual class GFX::GMesh*	Get_Custom_Shape() = 0;

	//! Add a new particle type
	virtual bool	Add_Particle_Type( CGFX_Particle_Type in_Type) = 0;

	//! Insert a new particle type at the specified position
	virtual bool	Insert_Particle_Type( CGFX_Particle_Type in_Type, UINT32 in_Index) = 0;

	//! Returns how many particle types we can create
	virtual UINT32	Get_Nb_Particle_Types() = 0;

	//! Returns the specified particle type (if it exists)
	virtual CGFX_Particle_Type	Get_Particle_Type( UINT32 in_Index ) = 0;

	//! Remove the particle type at the specified index
	virtual bool	Remove_Particle_Type_At( UINT32 in_Index ) = 0;

	//! Remove all particle types
	virtual bool	Clear_Particle_Types() = 0;

	//! Get number of particles parameter
	virtual CGFX_Parameter&	Nb_Particles() = 0;

	//! Get min relative position parameter
	virtual CGFX_Vector3_Parameter& Min_Position() = 0;

	//! Get max relative position parameter
	virtual CGFX_Vector3_Parameter& Max_Position() = 0;

	//! Get min speed parameter
	virtual CGFX_Vector3_Parameter& Min_Speed() = 0;

	//! Get max speed parameter
	virtual CGFX_Vector3_Parameter& Max_Speed() = 0;

	//! Get min acceleration parameter
	virtual CGFX_Vector3_Parameter& Min_Acceleration() = 0;

	//! Get max acceleration parameter
	virtual CGFX_Vector3_Parameter& Max_Acceleration() = 0;

	//! Get min rotation parameter
	virtual CGFX_Vector3_Parameter& Min_Rotation() = 0;

	//! Get max rotation parameter
	virtual CGFX_Vector3_Parameter& Max_Rotation() = 0;

	//! Get min rotation speed parameter
	virtual CGFX_Vector3_Parameter& Min_Rotation_Speed() = 0;

	//! Get max rotation speed parameter
	virtual CGFX_Vector3_Parameter& Max_Rotation_Speed() = 0;

	//! Get min rotation acceleration parameter
	virtual CGFX_Vector3_Parameter& Min_Rotation_Acceleration() = 0;

	//! Get max rotation acceleration parameter
	virtual CGFX_Vector3_Parameter& Max_Rotation_Acceleration() = 0;

	//! Get min scale parameter
	virtual CGFX_Vector3_Parameter& Min_Scale() = 0;

	//! Get max scale parameter
	virtual CGFX_Vector3_Parameter& Max_Scale() = 0;

	//! Get min scale speed parameter
	virtual CGFX_Vector3_Parameter& Min_Scale_Speed() = 0;

	//! Get max scale speed parameter
	virtual CGFX_Vector3_Parameter& Max_Scale_Speed() = 0;

	//! Get min scale acceleration parameter
	virtual CGFX_Vector3_Parameter& Min_Scale_Acceleration() = 0;

	//! Get max scale acceleration parameter
	virtual CGFX_Vector3_Parameter& Max_Scale_Acceleration() = 0;

	//! Get min life attenuation parameter
	virtual CGFX_Parameter& Min_Life_Attenuation() = 0;

	//! Get max life attenuation parameter
	virtual CGFX_Parameter& Max_Life_Attenuation() = 0;

	//! Get min life alpha parameter
	virtual CGFX_Parameter& Min_Life_Alpha() = 0;

	//! Get max life alpha parameter
	virtual CGFX_Parameter& Max_Life_Alpha() = 0;

	//! Get min life alpha speed parameter
	virtual CGFX_Parameter& Min_Life_Alpha_Speed() = 0;

	//! Get max life alpha speed parameter
	virtual CGFX_Parameter& Max_Life_Alpha_Speed() = 0;

	//! Get min life alpha acceleration parameter
	virtual CGFX_Parameter& Min_Life_Alpha_Acceleration() = 0;

	//! Get max life alpha acceleration parameter
	virtual CGFX_Parameter& Max_Life_Alpha_Acceleration() = 0;

	//! Get min color parameter
	virtual CGFX_Color_Parameter& Min_Color() = 0;

	//! Get max color parameter
	virtual CGFX_Color_Parameter& Max_Color() = 0;

	//! Get min luminescence parameter
	virtual CGFX_Parameter& Min_Luminescence() = 0;

	//! Get max luminescence parameter
	virtual CGFX_Parameter& Max_Luminescence() = 0;

	//! Get min birth interval parameter
	virtual CGFX_Parameter& Min_Birth_Interval() = 0;

	//! Get max birth interval parameter
	virtual CGFX_Parameter& Max_Birth_Interval() = 0;

	//! Get min line time parameter
	virtual CGFX_Parameter& Min_Line_Time() = 0;

	//! Get max line time parameter
	virtual CGFX_Parameter& Max_Line_Time() = 0;

};

#endif //#ifndef _GOLEM_GFX_PARTICLE_SYSTEM_ITF_
