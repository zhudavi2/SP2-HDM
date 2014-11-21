/**************************************************************
*
* golem_gfx_renderer_itf.h
*
* Description
* ===========
*  Class CGFX_Renderer_Device, CGFX_Renderer_Init & 
*  CGFX_Renderer_Itf definitions.
*
* Owner
* =====
*  Frederic O'Reilly
*
* History
* =======
*  April 08 2003 (foreilly):
*     - Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_RENDERER_ITF_
#define _GOLEM_GFX_RENDERER_ITF_

#include "../../includes/hector/hector.h"

#include "golem_gfx_render_tree_itf.h"
#include "golem_gfx_texture_manager_itf.h"
#include "golem_gfx_helper_itf.h"

#include "golem_gfx_scene_itf.h"
#include "golem_gfx_model_itf.h"
#include "golem_gfx_material_itf.h"
#include "golem_gfx_render_state_collection_itf.h"
#include "golem_gfx_shader_itf.h"
#include "golem_gfx_default_shader_itf.h"
#include "golem_gfx_transform_itf.h"
#include "golem_gfx_transform_srt_itf.h"
#include "golem_gfx_transform_2d_itf.h"
#include "golem_gfx_mesh_itf.h"
#include "golem_gfx_mesh_complex_itf.h"
#include "golem_gfx_facet_group_itf.h"
#include "golem_gfx_2d_multiple_line_list_itf.h"
#include "golem_gfx_window_itf.h"
#include "golem_gfx_text_2d_itf.h"
#include "golem_gfx_viewport_itf.h"
#include "golem_gfx_console_itf.h"
#include "golem_gfx_particle_system_itf.h"
#include "golem_gfx_camera_itf.h"
#include "golem_gfx_light_itf.h"
#include "golem_gfx_animation_system_itf.h"
#include "golem_gfx_track_itf.h"
#include "golem_gfx_action_itf.h"
#include "golem_gfx_animation_itf.h"
#include "golem_gfx_animation_static_itf.h"
#include "golem_gfx_animation_constant_itf.h"
#include "golem_gfx_animation_linear_itf.h"
#include "golem_gfx_animation_cubic_itf.h"
#include "golem_gfx_animation_multiple_itf.h"

#include "golem_gfx_override_itf.h"
#include "golem_gfx_override_rstate_itf.h"
#include "golem_gfx_override_ps_cascade_itf.h"
#include "golem_gfx_override_tsampler_itf.h"
#include "golem_gfx_override_txtr_seq_control_itf.h"

#include "golem_gfx_mesh_morpher_itf.h"

#undef CreateWindow

//! Contains information about a rendering device
class CGFX_Renderer_Device
{
public:
   //! Destroy this render device object
   virtual bool   Release() = 0;

   //! Get the name of the device
   virtual bool   Get_Name(GString& out_Name) = 0;

   //! Get the GDI name of the device
   virtual bool   Get_GDI_Name(GString& out_GDIName) = 0;

   //! Get the device index f this device
   virtual UINT32 Get_Device_Index() = 0;

};

//! Contains device initialization information 
class CGFX_Renderer_Init_Mode
{
public:
   GVector2D<INT32>  m_Resolution;
   UINT32      m_DeviceIdx;
   INT16    m_FrameBufferBitDepth;
   INT16    m_DepthBufferBitDepth;
   INT16    m_StencilBufferBitDepth;
   INT16    m_NumBackBuffer;
   UINT32      m_RefreshRate;
   bool     m_UseMultiHead;
   bool     m_UseStencilBuffer;
   bool     m_Fullscreen;

   CGFX_Renderer_Init_Mode()
   {
      // Set all members to 0.
      // Change this initialization if class becomes virtual.
      memset(this, 0, sizeof(CGFX_Renderer_Init_Mode) );
   }
};

//! Main object for the graphical engine
class CGFX_Renderer_Itf
{
public:
   //! Link the renderer to the engine.
   virtual bool   Startup( GEngineInfo* in_EngineInfo, bool in_SafeMode = false) = 0;

   //! Get the engine information
   virtual GEngineInfo &   Get_Engine() = 0;

   //! Return the last error.
   virtual bool   Get_Last_Error( GString& out_ErrorString ) = 0;

   //! Init the renderer for a specific video mode with linked device.
   virtual bool   Init( CGFX_Renderer_Init_Mode* in_InitMode ) = 0;

   //! Close the init video.
   virtual bool   Close() = 0;

   //! Destroy the renderer object.
   virtual bool   Release() = 0;

   //! Return the number of devices.
   virtual UINT32 Get_Nb_Devices() = 0;

   //! Return information about the specified device.
   virtual CGFX_Renderer_Device* Get_Device( UINT32 in_DeviceNumber ) = 0;

   //! Return the number of video modes supported.
   virtual UINT32 Get_Nb_Init_Modes( CGFX_Renderer_Device* in_Device, UINT32 in_ColorBitDepth ) = 0;

   //! Return the video mode of index in_ModeNumber starting at index 0.
   //! The number of valid indices are GetNumInitMode() - 1.
   virtual bool   Get_Init_Mode( CGFX_Renderer_Device* in_Device, UINT32 in_ModeNumber, CGFX_Renderer_Init_Mode* out_InitMode, UINT32 in_ColorBitDepth ) = 0;

   //! Return the currently init video mode.
   virtual bool   Get_Current_Mode( CGFX_Renderer_Init_Mode* out_Mode ) = 0;

   //! Return the currently main init device.
   virtual CGFX_Renderer_Device* Get_Current_Device() = 0;

   //! Return if the specified mode is supported.
   //! \todo TBA : doesn't check for depth_buffer, stencil buffer and back buffer.
   virtual bool   Mode_Supported( CGFX_Renderer_Device* in_Device, CGFX_Renderer_Init_Mode* in_Mode ) = 0;

   //! Return if the game runs in fullscreen.
   virtual bool   In_Full_Screen() = 0;

   //! Return a pointer to the render tree of main device
   virtual CGFX_Render_Tree_Itf* RenderTree() = 0;

   //! Return a pointer to the texture manager of main device
   virtual CGFX_Texture_Manager_Itf* TextureManager() = 0;

   //! Return a pointer to the object manager of main device
   virtual CGFX_Obj_Manager* ObjectManager() = 0;

   //! Return a pointer to the gfx helper class of functions
   virtual CGFX_Helper_Itf* Helper() = 0;

   //! \brief Render a frame
   //!
   //! This function will render all textures (Texture_Renderer objects) and 
   //! then each render list contained in the render tree in order.
   //! It will return true if render was possible or false if render could not 
   //! be done (in case of a lost device). The rendering will continue as soon as the
   //! device can be reacquired.
   //!
   virtual bool    Render() = 0;

   //! Return the number of frames displayed since initialization
   virtual REAL32 Get_Nb_Frames() = 0;

   //! Return the current FPS
   virtual REAL32 Get_FPS() = 0;

   //! Enable the display of the fps counter in the upper left corner of the screen
   virtual bool   Display_FPS( bool in_Show ) = 0;

   //! Returns if FPS is displayed
   virtual bool   Display_FPS() = 0;

   //! Create a bitmap screenshot using the specified file name
   virtual bool   CreateScreenShot(const GString& in_sFileName) = 0;

   // Get & Creation of Objects Methods

   //! Create a new scene object
   virtual CGFX_Scene_Itf* Create_Scene() = 0;

   //! Create a new Model object
   virtual CGFX_Model_Itf* Create_Model() = 0;

   //! Create a new transform object
   virtual CGFX_Transform_Itf* Create_Transform() = 0;

   //! Create a new transform srt object
   virtual CGFX_Transform_SRT_Itf* Create_Transform_SRT() = 0;

   //! Create a new transform srt object
   virtual CGFX_Transform_2D_Itf* Create_Transform_2D() = 0;

   //! Create a new material object
   virtual GFX::GMaterial* CreateMaterial() = 0;

   //! Create a new default vertex shader object
   virtual GFX::GDefaultVertexShader* CreateDefaultVertexShader() = 0;

   //! Create a new cascade pixel shader object
   virtual GFX::GCascadePixelShader* CreateCascadePixelShader() = 0;

   //! Create a new mesh object
   virtual GFX::GMesh* CreateMesh() = 0;

   //! Create a new complex mesh object
   virtual GFX::GMeshComplex* CreateMeshComplex() = 0;

   //! Create a new 2d multiple line list object
   virtual CGFX_2D_Multiple_Line_List_Itf* Create_2D_Multiple_Line_List() = 0;

   //! Create a new window object
   virtual GFX::GWindow* CreateWindow() = 0;

   //! Create a new text 2d object
   virtual CGFX_Text_2D_Itf* Create_Text_2D() = 0;

   //! Create a new viewport object
   virtual CGFX_Viewport_Itf* Create_Viewport() = 0;

   //! Create a new console object
   virtual CGFX_Console_Itf* Create_Console() = 0;

   //! Create a new particle system object
   virtual CGFX_Particle_System_Itf* Create_Particle_System() = 0;

   //! Create a new camera object
   virtual CGFX_Camera_Itf* Create_Camera() = 0;

   //! Create a new light object
   virtual CGFX_Light_Itf* Create_Light() = 0;

   //! Create a new track object
   virtual CGFX_Track_Itf* Create_Track() = 0;

   //! Create a new action object
   virtual CGFX_Action_Itf* Create_Action() = 0;

   //! Create a new animation static object
   virtual CGFX_Animation_Static_Itf* Create_Animation_Static() = 0;

   //! Create a new animation constant object
   virtual CGFX_Animation_Constant_Itf* Create_Animation_Constant() = 0;

   //! Create a new animation linear object
   virtual CGFX_Animation_Linear_Itf* Create_Animation_Linear() = 0;

   //! Create a new animation cubic object
   virtual CGFX_Animation_Cubic_Itf* Create_Animation_Cubic() = 0;

   //! Create a new animation multiple object
   virtual GFX::GAnimationMultiple* CreateAnimationMultiple() = 0;

   //! Create a new mesh morpher object
   virtual CGFX_Mesh_Morpher_Itf* Create_Mesh_Morpher() = 0;

   //! Create a render state override
   virtual GFX::GOverrideRState* CreateOverrideRState() = 0;

   //! Create a cascade pixel shader override
   virtual GFX::GOverridePSCascade* CreateOverridePSCascade() = 0;

   //! Create a texture sampler override
   virtual GFX::GOverrideTSampler* CreateOverrideTSampler() = 0;

   //! Create a texture sampler override
   virtual GFX::GOverrideTxtrSeqControl* CreateOverrideTxtrSeqControl() = 0;

   //! Clear the frame buffer
   virtual void ClearFrameBuffer(const GColorRGBReal& in_Color) = 0;
};

#endif //#ifndef _GOLEM_GFX_RENDERER_ITF_
