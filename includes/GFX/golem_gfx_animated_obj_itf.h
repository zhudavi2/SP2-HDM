/**************************************************************
*
* golem_gfx_Animated_obj_itf.h
*
* Description
* ===========
*	Class CGFX_Animated_Obj_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 09 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATED_OBJ_ITF_
#define _GOLEM_GFX_ANIMATED_OBJ_ITF_

#include "golem_gfx_obj_itf.h"

namespace GFX
{
	enum EParameter_Type
	{
		PARAM_NONE = 0,

		PARAM_VISIBILITY,
		PARAM_ACTIVE,

		PARAM_TRANSLATION_X,
		PARAM_TRANSLATION_Y,
		PARAM_TRANSLATION_Z,

		PARAM_ROTATION_X,
		PARAM_ROTATION_Y,
		PARAM_ROTATION_Z,

		PARAM_SCALE_X,
		PARAM_SCALE_Y,
		PARAM_SCALE_Z,

		PARAM_POSITION_X,
		PARAM_POSITION_Y,

		PARAM_SIZE_X,
		PARAM_SIZE_Y,

		PARAM_FOV,
		PARAM_ASPECT_RATIO,
		PARAM_NEAR_PLANE,
		PARAM_FAR_PLANE,

		//! \todo TBR : Replace by constraint system
		PARAM_LOOK_AT_X,
		PARAM_LOOK_AT_Y,
		PARAM_LOOK_AT_Z,

		//! \todo TBR : Replace by constraint system
		PARAM_UP_VECTOR_X,
		PARAM_UP_VECTOR_Y,
		PARAM_UP_VECTOR_Z,

		PARAM_FALLOFF,
		PARAM_RANGE,
		PARAM_ATTENUATION_CONSTANT,
		PARAM_ATTENUATION_LINEAR,
		PARAM_ATTENUATION_SQUARED,

		PARAM_COLOR_R,
		PARAM_COLOR_G,
		PARAM_COLOR_B,
		PARAM_COLOR_A,

      PARAM_LIGHT_ENABLE,
      PARAM_LIGHT_SPECULAR_ENABLE,

		PARAM_DIFFUSE_R,
		PARAM_DIFFUSE_G,
		PARAM_DIFFUSE_B,
		PARAM_DIFFUSE_A,

		PARAM_SPECULAR_R,
		PARAM_SPECULAR_G,
		PARAM_SPECULAR_B,
		PARAM_SPECULAR_A,

		PARAM_SPECULAR_POWER,

		PARAM_EMISSIVE_R,
		PARAM_EMISSIVE_G,
		PARAM_EMISSIVE_B,
		PARAM_EMISSIVE_A,

		PARAM_AMBIENT_R,
		PARAM_AMBIENT_G,
		PARAM_AMBIENT_B,
		PARAM_AMBIENT_A,

		PARAM_GLOBAL_AMBIENT_R,
		PARAM_GLOBAL_AMBIENT_G,
		PARAM_GLOBAL_AMBIENT_B,
		PARAM_GLOBAL_AMBIENT_A,

		PARAM_COLOR_FACTOR_R,
		PARAM_COLOR_FACTOR_G,
		PARAM_COLOR_FACTOR_B,
		PARAM_COLOR_FACTOR_A,

		PARAM_BLEND_FACTOR_R,
		PARAM_BLEND_FACTOR_G,
		PARAM_BLEND_FACTOR_B,
		PARAM_BLEND_FACTOR_A,

		PARAM_BORDER_COLOR_R,
		PARAM_BORDER_COLOR_G,
		PARAM_BORDER_COLOR_B,
		PARAM_BORDER_COLOR_A,

		PARAM_CLEAR_COLOR_R,
		PARAM_CLEAR_COLOR_G,
		PARAM_CLEAR_COLOR_B,
		PARAM_CLEAR_COLOR_A,

		PARAM_CLEAR_DEPTH,

      PARAM_FRAME,

		PARAM_DEFORMATION_FACTOR_0,
		PARAM_DEFORMATION_FACTOR_1,
		PARAM_DEFORMATION_FACTOR_2,
		PARAM_DEFORMATION_FACTOR_3,
		PARAM_DEFORMATION_FACTOR_4,
		PARAM_DEFORMATION_FACTOR_5,
		PARAM_DEFORMATION_FACTOR_6,
		PARAM_DEFORMATION_FACTOR_7,
		PARAM_DEFORMATION_FACTOR_8,
		PARAM_DEFORMATION_FACTOR_9,
		PARAM_DEFORMATION_FACTOR_10,
		PARAM_DEFORMATION_FACTOR_11,
		PARAM_DEFORMATION_FACTOR_12,
		PARAM_DEFORMATION_FACTOR_13,
		PARAM_DEFORMATION_FACTOR_14,
		PARAM_DEFORMATION_FACTOR_15,
		PARAM_DEFORMATION_FACTOR_16,
		PARAM_DEFORMATION_FACTOR_17,
		PARAM_DEFORMATION_FACTOR_18,
		PARAM_DEFORMATION_FACTOR_19,
		PARAM_DEFORMATION_FACTOR_20,
		PARAM_DEFORMATION_FACTOR_21,
		PARAM_DEFORMATION_FACTOR_22,
		PARAM_DEFORMATION_FACTOR_23,
		PARAM_DEFORMATION_FACTOR_24,
		PARAM_DEFORMATION_FACTOR_25,
		PARAM_DEFORMATION_FACTOR_26,
		PARAM_DEFORMATION_FACTOR_27,
		PARAM_DEFORMATION_FACTOR_28,
		PARAM_DEFORMATION_FACTOR_29,
		PARAM_DEFORMATION_FACTOR_30,
		PARAM_DEFORMATION_FACTOR_31,

		PARAM_TRACK_STATE,

		PARAM_CUSTOM,

		PARAM_COUNT,
	};

	// List of all parameters names
	extern const wchar_t* PARAM_NAMES[PARAM_CUSTOM];
};

//! Base class for any object that can be animated via its parameters
class CGFX_Animated_Obj_Itf : public CGFX_Obj_Itf
{
	bool			m_Animated;

	vector<pair<CGFX_Parameter, GString> >	m_CustomParams;

protected:
	CGFX_Animated_Obj_Itf(UINT32 in_Type);

public:
	// Return how many parameters are present on this object
	virtual UINT32 Get_Nb_Parameters() const;

	// Return the type of the specified indexed parameter
	virtual GFX::EParameter_Type	Parameter_Type(UINT32 in_Index) const;

	// Return the type of the specified parameter name
	virtual GFX::EParameter_Type	Parameter_Type(const GString& in_ParamName) const;

	// Return the name of the specified indexed parameter
	virtual const GString&	Parameter_Name(UINT32 in_Index) const;

	// Return the specified parameter by index
	virtual CGFX_Parameter&	Parameter(UINT32 in_ParamIndex);

	// Return the specified parameter by type
	virtual CGFX_Parameter&	Parameter(GFX::EParameter_Type in_ParamType);

	// Return the specified parameter by name
	virtual CGFX_Parameter&	Parameter(const GString& in_ParamName);

	//! Get the number of custom parameters
	virtual UINT32	Get_Nb_Custom_Parameters() const;

	//! Remove all custom parameters
	virtual bool	Clear_Custom_Parameters();

	//! Add a custom parameter
	virtual UINT32	Add_Custom_Parameter( const GString& in_ParamName );

	//! Remove a custom parameter by name
	virtual bool	Remove_Custom_Parameter( const GString& in_ParamName );

	//! Remove a custom parameter by Id (index)
	virtual bool	Remove_Custom_Parameter( UINT32 in_ParamIndex );

	// Return the Id (index) of the specified custom parameter name
	virtual UINT32	Custom_Parameter_Index(const GString& in_ParamName) const;

	// Return the name of the specified custom parameter index
	virtual const GString&	Custom_Parameter_Name(UINT32 in_ParamIndex) const;

	//! Return the specified custom parameter by Id (index)
	virtual CGFX_Parameter&	Custom_Parameter( UINT32 in_ParamIndex );

	//! Return the specified custom parameter by name
	virtual CGFX_Parameter&	Custom_Parameter( const GString& in_ParamName );

	//! Indicate if object is animated
	bool	Animated() const
	{
		return m_Animated;
	}

	//! Set object as animated
   virtual void	Set_Animated( GFX::EParameter_Type in_ParamType = GFX::PARAM_NONE )
	{
		m_Animated = true;
	}

	// Returns if there is an animation on the current object
	virtual bool	Find_Animation();

	//! Reset object animation state
	virtual void	Reset_Animated(bool in_CompleteVerification)
	{
		if(!in_CompleteVerification)
		{
			m_Animated = false;
		}
		else
		{
			m_Animated = Find_Animation();
		}
	}

   //! Refresh object parameters
   virtual bool Refresh();

	//! Connect an animation to a parameter by type
	virtual bool	Connect_Animation(class CGFX_Animation_Itf* in_Animation, GFX::EParameter_Type in_ParamType);

	//! Connect an animation to a parameter by name
	virtual bool	Connect_Animation(class CGFX_Animation_Itf* in_Animation, const GString& in_ParamName);

	//! Connect an animation to a custom parameter by index
	virtual bool	Connect_Custom_Parameter_Animation(class CGFX_Animation_Itf* in_Animation, UINT32 in_ParamIndex);

};


//! This class manages all parameter names and types
class CGFX_Parameter_Manager
{
	//! Map type that links a name to a parameter type.
	typedef map<GString, GFX::EParameter_Type>	CParamStr_Map;

	//! Map containing all parameters type and their name.
	CParamStr_Map			m_ParamNameMap;

	GString					m_ParamNames[GFX::PARAM_COUNT];

	CGFX_Parameter			m_Dummy;

public:
	CGFX_Parameter_Manager();

	//! Return a dummy parameter for the case when parameter type or name is invalid
	virtual CGFX_Parameter&			Dummy_Parameter();

	//! Get parameter name by type
	virtual const GString&			Get_Parameter_Name(GFX::EParameter_Type in_Type) const;

	//! Get parameter type by name
	virtual GFX::EParameter_Type	Get_Parameter_Type(const GString& in_Name) const;
};

#endif //#ifndef _GOLEM_GFX_ANIMATED_OBJ_ITF_
