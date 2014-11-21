/*******************************************************

golem_renderer_itf.h


Owner: Germain Sauve

history:
	- March 6 2003
		New approach with possibility of animations

    - 28 fevrier 2003
        Find that there isn't a lot written in the history

    - 15 sept 2002
        Added CRenderer_Init_Mode

    - 12 sept 2002
        Creation of the interface

********************************************************/

#ifndef _GOLEM_RENDERER_INTERFACE_
#define _GOLEM_RENDERER_INTERFACE_

#include "golem_gfx_obj_itf.h"
#include "golem_texture_manager_itf.h"
#include "golem_gfx_2dframe_itf.h"
#include "golem_gfx_overlay_itf.h"
#include "golem_gfx_debug_text_itf.h"
#include "golem_gfx_2dline_list_itf.h"
#include "golem_gfx_2d_point_list_itf.h"
#include "golem_gfx_2d_multiple_line_list_itf.h"
#include "golem_gfx_2d_texture_image.h"

#include "golem_gfx_scene_itf.h"
#include "golem_gfx_ambient_itf.h"
#include "golem_gfx_view_itf.h"
#include "golem_gfx_camera_itf.h"
#include "golem_gfx_light_itf.h"
#include "golem_gfx_light_point_itf.h"
#include "golem_gfx_material_itf.h"
#include "golem_gfx_model_itf.h"
#include "golem_gfx_transform_itf.h"
#include "golem_gfx_transform_srt_itf.h"
#include "golem_gfx_transform_2d_itf.h"
#include "golem_gfx_primitive_itf.h"
#include "golem_gfx_window_itf.h"
#include "golem_gfx_3d_mesh_itf.h"
#include "golem_gfx_2d_text_itf.h"
#include "golem_gfx_viewport_itf.h"
#include "golem_gfx_animation_itf.h"
#include "golem_gfx_animation_static_itf.h"
#include "golem_gfx_animation_constant_itf.h"
#include "golem_gfx_animation_linear_itf.h"
#include "golem_gfx_animation_cubic_itf.h"
#include "golem_gfx_animation_system_itf.h"
#include "golem_gfx_action_itf.h"

#include "golem_gfx_container_transform_2d_itf.h"

#include "golem_rendering_buffer_itf.h"

#include "golem_gfx_util.h"

#include "golem_gfx_helper_itf.h"


typedef class CRenderer_Itf
    {
public:
    // link the renderer to the engine
    virtual bool Startup( CEngine_Info *pEngine_Info, bool pSafeMode ) = 0;

    // return the last error 
    virtual bool GetLastError( GString& pErrorString ) = 0;

    // init the renderer for a specific video mode with linked device
    virtual bool Init( CRenderer_Init_Mode& pInitMode ) = 0;
    
    // init the Device with the video mode of pInitMode, if the device is already init or part of multi_head, return false
    virtual bool Init_Extended( CRenderer_Device_Ptr pDevice, CRenderer_Init_Mode& pInitMode, bool pFullScreen ) = 0;

    // close a device other than the main
    virtual bool Close_Extended( UINT32 pInitDeviceNum ) = 0;

    // close the init video mode and all extended device
    virtual bool Close() = 0;
    
    // release the renderer from the engine
    virtual bool Release() = 0;

    // return the number of device
    virtual UINT32  GetNumDevice() = 0;
    
    // fill pDevice with the content of device Idx pDeviceNumber
    virtual bool    GetDevice( UINT32 pDeviceNumber, CRenderer_Device_Ptr pDevice ) = 0;
    
    // return the number of video mode supported
    virtual UINT32  GetNumInitMode( CRenderer_Device_Ptr pDevice, UINT32 pColorBitDepth ) = 0;

    // return the video mode of index pModeNumber starting at index 0
    // the number of valid indices are GetNumInitMode()-1
    virtual bool    GetInitMode( CRenderer_Device_Ptr pDevice, UINT32 pModeNumber, CRenderer_Init_Mode& pInitMode, UINT32 pColorBitDepth ) = 0;
    
    //return the currently init video mode
    virtual bool    Get_CurrentMode( CRenderer_Init_Mode& pMode ) = 0;

    //return the currently main init device
    virtual bool    Get_CurrentDevice( CRenderer_Device_Ptr pDevice ) = 0;

    // return if the specified mode is supported
    // doesn't check for depth_buffer, stencil buffer and back buffer
    virtual bool    Mode_Supported( CRenderer_Device_Ptr pDevice, CRenderer_Init_Mode& pMode ) = 0;

    // Returns the number of device use by the renderer
    virtual UINT32    Get_Num_Init_Device() = 0;

    // Return in pDevice the init device at ID pInitDevNum
    virtual bool      Get_Init_Device( UINT32 pInitDevNum, CRenderer_Device_Ptr pDevice ) = 0;

    // return the size of the string if pStr = NULL
    // copies in pStr the caps if pStr != NULL
    // return 0 if error
    virtual UINT32    OnceInitCaps( char* pString ) = 0;

    // set the presentation parameter vsync of all devices
    virtual bool    VSync( UINT32 pInterval ) = 0;
    virtual UINT32  VSync() = 0;

    // return if the game runs in fullscreen
    virtual bool    InFullScreen() = 0;
    virtual bool    Use_MultiHead() = 0;
    // gui related function

    // give the control of gui hierarchy to the calling thread
    virtual bool Lock_GUI() = 0;
    // release the control of the gui hierarchy
    virtual bool Unlock_GUI() = 0;


    // return the master container of the main device
    virtual CGFX_Container_Itf_Ptr      Get_Container() = 0;

    // return the master container of the device pInitDevNum
    virtual CGFX_Container_Itf_Ptr      Get_Container_Extended( UINT32 pInitDevNum ) = 0;

    // force the rendering of a frame on all devices
    virtual bool    Render() = 0;

    // render a frame of a specific device
    virtual bool    Render_Extended( UINT32 pInitDevNum ) = 0;

    // return a pointer to the texture manager of main device
    virtual CTexture_Manager_Itf* Texture_Manager() = 0;

    // return a pointer to the texture manager of extended device
    virtual CTexture_Manager_Itf* Texture_Manager_Extended( UINT32 pInitDevNum ) = 0;

    // grab the frame buffer of all init device and set pImage to combine 
    // screen size at 24 bit color
    virtual bool Screenshot( CGolem_Basic_Image2D& pImage ) = 0;

    //grab the frame buffer of pInitDevNum device and set pImage to screen size at 24 bit color
    virtual bool Screenshot_Ex( UINT32 pInitDevNum, CGolem_Basic_Image2D& pImage ) = 0;

    // restart conter of FPS
    virtual bool Reset_FramePerSecond() = 0;
    // return the average FPS
    virtual REAL32 Get_FramePerSecond() = 0;

    // enable the display of the fps counter in the upper left corner of the screen
    virtual bool	DisplayFPS( bool in_Show ) = 0;
    virtual bool	DisplayFPS() = 0;

    //virtual bool BlitToFrameBuffer( CVector2Di& pPos, CGolem_Basic_Image2D& pImage ) = 0;

	// return a pointer to the gfx helper class of functions
	virtual CGFX_Helper_Itf_Ptr GFX_Helper() = 0;
    
//------------------ Get Object Method
    /*
primary todo
    - console ASCII
    - 2D context and 3D context ( scene )
    - 2D picture ( sprite? )
    - simple 3D object ( 1 texture + color vertex )
    */
    
    // 2D gfx drawable obj


    // 2D line list
    virtual CGFX_2D_Line_list_Itf_Ptr Get_New_2D_Line_List() = 0;
    virtual CGFX_2D_Line_list_Itf_Ptr Get_2D_Line_List( UINT32 pID) = 0;


    // 2D Multiple line list
    virtual CGFX_2D_Multiple_Line_List_Itf_Ptr Get_New_2D_Multiple_Line_List() = 0;
    virtual CGFX_2D_Multiple_Line_List_Itf_Ptr Get_2D_Multiple_Line_List( UINT32 pID) = 0;

    // 2D Texture Image
    virtual CGFX_2D_Texture_Image_Itf_Ptr   Get_New_2D_Texture_Image() = 0;
    virtual CGFX_2D_Texture_Image_Itf_Ptr   Get_2D_Texture_Image( UINT32 pID) = 0;
    
    // 2D scene export obj 


    // 2D transform export obj
    virtual CGFX_Container_Transform_2D_Itf_Ptr Get_New_Container_Transform_2D() = 0;
    virtual CGFX_Container_Transform_2D_Itf_Ptr Get_Container_Transform_2D( UINT32 pID ) = 0;
    
    // 3D transform export obj

    // 

	//! \todo TBM : 
    // Scene
    virtual CGFX_Scene_Itf_Ptr Get_New_Scene() = 0;
    virtual CGFX_Scene_Itf_Ptr Get_Scene( UINT32 pID ) = 0;

    // Ambient
    virtual CGFX_Ambient_Itf_Ptr Get_New_Ambient() = 0;
    virtual CGFX_Ambient_Itf_Ptr Get_Ambient( UINT32 pID ) = 0;

    // View
    virtual CGFX_View_Itf_Ptr Get_New_View() = 0;
    virtual CGFX_View_Itf_Ptr Get_View( UINT32 pID ) = 0;

    // Camera
    virtual CGFX_Camera_Itf_Ptr Get_New_Camera() = 0;
    virtual CGFX_Camera_Itf_Ptr Get_Camera( UINT32 pID ) = 0;

    // Light_Point
    virtual CGFX_Light_Point_Itf_Ptr Get_New_Light_Point() = 0;
    virtual CGFX_Light_Point_Itf_Ptr Get_Light_Point( UINT32 pID ) = 0;

    // Model
    virtual CGFX_Model_Itf_Ptr Get_New_Model() = 0;
    virtual CGFX_Model_Itf_Ptr Get_Model( UINT32 pID ) = 0;

    // Material
    virtual CGFX_Material_Itf_Ptr Get_New_Material() = 0;
    virtual CGFX_Material_Itf_Ptr Get_Material( UINT32 pID ) = 0;

	// Transform
    virtual CGFX_Transform_Itf_Ptr Get_New_Transform() = 0;
    virtual CGFX_Transform_Itf_Ptr Get_Transform( UINT32 pID ) = 0;

	// Transform SRT
    virtual CGFX_Transform_SRT_Itf_Ptr Get_New_Transform_SRT() = 0;
    virtual CGFX_Transform_SRT_Itf_Ptr Get_Transform_SRT( UINT32 pID ) = 0;

	// Transform 2D
    virtual CGFX_Transform_2D_Itf_Ptr Get_New_Transform_2D() = 0;
    virtual CGFX_Transform_2D_Itf_Ptr Get_Transform_2D( UINT32 pID ) = 0;

    // Mesh
    virtual CGFX_3D_Mesh_Itf_Ptr Get_New_3D_Mesh() = 0;
    virtual CGFX_3D_Mesh_Itf_Ptr Get_3D_Mesh( UINT32 pID ) = 0;

    // Window
    virtual CGFX_Window_Itf_Ptr Get_New_Window() = 0;
    virtual CGFX_Window_Itf_Ptr Get_Window( UINT32 pID ) = 0;

    // Text 2D
    virtual CGFX_2D_Text_Itf_Ptr Get_New_2D_Text() = 0;
    virtual CGFX_2D_Text_Itf_Ptr Get_2D_Text( UINT32 pID ) = 0;

    // Viewport
    virtual CGFX_Viewport_Itf_Ptr Get_New_Viewport() = 0;
    virtual CGFX_Viewport_Itf_Ptr Get_Viewport( UINT32 pID ) = 0;

	// Animation Static
    virtual CGFX_Animation_Static_Itf_Ptr Get_New_Animation_Static() = 0;
    virtual CGFX_Animation_Static_Itf_Ptr Get_Animation_Static( UINT32 pID ) = 0;

	// Animation Constant
    virtual CGFX_Animation_Constant_Itf_Ptr Get_New_Animation_Constant() = 0;
    virtual CGFX_Animation_Constant_Itf_Ptr Get_Animation_Constant( UINT32 pID ) = 0;

	// Animation Linear
    virtual CGFX_Animation_Linear_Itf_Ptr Get_New_Animation_Linear() = 0;
    virtual CGFX_Animation_Linear_Itf_Ptr Get_Animation_Linear( UINT32 pID ) = 0;

	// Animation Cubic
    virtual CGFX_Animation_Cubic_Itf_Ptr Get_New_Animation_Cubic() = 0;
    virtual CGFX_Animation_Cubic_Itf_Ptr Get_Animation_Cubic( UINT32 pID ) = 0;

	// Action
    virtual CGFX_Action_Itf_Ptr Get_New_Action() = 0;
    virtual CGFX_Action_Itf_Ptr Get_Action( UINT32 pID ) = 0;

	//! \todo TBR or TBM
/*
    //Debug text
    virtual CGFX_Debug_Text_Output_Ptr Get_New_Debug_Text() = 0;
    virtual CGFX_Debug_Text_Output_Ptr Get_Debug_Text( UINT32 pID) = 0;

    //Debug Font
    virtual CGFX_Debug_Font_Ptr Get_New_Debug_Font() = 0;
    virtual CGFX_Debug_Font_Ptr Get_Debug_Font( UINT32 pID) = 0;

*/
    } *CRenderer_Itf_Ptr;

#endif //#ifndef _GOLEM_RENDERER_INTERFACE_
