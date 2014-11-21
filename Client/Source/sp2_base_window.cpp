/**************************************************************
*
* sp2_base_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_save_confirmation_window.h"
#include "../include/sp2_generic_message_window.h"

const GString GBaseWindow::TypeName(L"frmBaseWindow");

GUI::GBaseObject* GBaseWindow::New()
{
	return (GUI::GBaseObject*) new GBaseWindow();
}


GBaseWindow::GBaseWindow() : GFrame()
{
  
   //m_bDragging  = false;
   m_bMoveable  = true;
   Focusable(true);
   m_bWindowCanDisable = false;

   m_pObjFrameTitle           = NULL;
   m_pObjLockTog              = NULL;
   m_pObjExitBtn              = NULL;
   m_pObjApplyBtn             = NULL;
   m_pWindowData              = NULL;
   m_pObjGradientPic          = NULL;
   m_pActionResize            = NULL;
   m_pObjColorGradientPic     = NULL;
   m_pObjInnerFrm             = NULL;
   m_fWhenSaved               = 0.f;
   m_bDirty                   = false;
   m_bNeedCountryCheckToSave  = true;
}

GBaseWindow::~GBaseWindow()
{
}

bool GBaseWindow::Enabled()
{
   return m_bEnabled;
}

void GBaseWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   if(m_pObjFrameTitle && m_pObjFrameTitle->Child(L"lblTitle"))
   {
      ((GUI::GLabel*)m_pObjFrameTitle->Child(L"lblTitle"))->Enabled(in_bEnabled);
   }
}

GUI::EEventStatus::Enum GBaseWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(m_bMoveable)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         GBaseObject* l_pClicked = g_Joshua.GUIManager()->MouseDownTarget();
         if(l_pClicked && (l_pClicked->Name() == L"lblTitle" || l_pClicked->Name() == L"frmTitle") )         
         {
            if(l_pClicked->Name() == L"lblTitle" && l_pClicked->Owner()->Name() != L"frmTitle")
            {
               return EEventStatus::Handled;
            }
            else if(l_pClicked->Name() == L"frmTitle" && l_pClicked->Owner() != this)
            {
               return EEventStatus::Handled;
            }

            m_MouseClick = Position();
            Drag();
         }
      }
   }

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBaseWindow::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            ExitWindow();
         }
         else if(in_pCaller == m_pObjApplyBtn)
         {
            Apply();
         }
      }
   }
   return EEventStatus::Handled;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GBaseWindow::OnShow()
{
   __super::OnShow();

   if(m_pObjGradientPic)
      m_pObjGradientPic->BringToFront();

   // window has focus when showed
   Focus();

   Dirty(false);

   if(m_bWindowCanDisable)
   {
      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
         Enabled(true);
      else
         Enabled(false);
   }
}

bool GBaseWindow::OnCreate()
{
   __super::OnCreate();

   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : " + this->Name());


   m_pObjFrameTitle = Child(L"frmTitle");
   if(m_pObjFrameTitle)
   {
      m_pObjLockTog  = (GUI::GToggle*)m_pObjFrameTitle->Child(L"togLockWindow");
      m_pObjExitBtn  = (GUI::GButton*)m_pObjFrameTitle->Child(L"btnExit");
      m_pObjApplyBtn = (GUI::GButton*)m_pObjFrameTitle->Child(L"btnApply");

      if(m_pObjLockTog)
      {
         m_pObjLockTog->BringToFront();
         m_pObjLockTog->Hide();
      }
      
      if(m_pObjExitBtn)
         m_pObjExitBtn->BringToFront();
      
      if(m_pObjApplyBtn)
      {
         m_pObjApplyBtn->BringToFront();

         if(m_pObjLockTog)
         {
            m_pObjApplyBtn->Left( m_pObjLockTog->Left() );
         }
      }
   }

   m_pObjGradientPic = (GUI::GPictureBox*)Child(L"picShadowGradient");
   if(m_pObjGradientPic)
   {
      m_pObjGradientPic->BringToFront();
      m_pObjGradientPic->ColorNormal(GColorRGBInt(0,0,0,128));
   }

   if(m_pWindowData)
   {
      if(m_pWindowData->Left >= 0)
         Left(m_pWindowData->Left);
      if(m_pWindowData->Top >= 0)
         Top(m_pWindowData->Top);
   }

   if (Owner() == (GBaseObject*)g_ClientDDL.BackgroundEarth())
      Owner(NULL);

   if(!g_ClientDDL.SaveConfirmationWindow())
      g_ClientDDL.SaveConfirmationWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.SaveConfirmationWindow());

   m_iLastHeight = Height();
   m_bMinimized = false;

   return true;
}

bool GBaseWindow::OnDestroy()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : " + this->Name());

   if(m_pWindowData)
   {  
      m_pWindowData->Left = (INT16)Left();
      m_pWindowData->Top  = (INT16)Top();
      //m_pWindowData->PreviouslyShown = Visible();
   }

   if(m_pActionResize)
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pActionResize);
      SAFE_RELEASE(m_pActionResize);
      m_pActionResize = NULL;
   }

   RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

bool GBaseWindow::Moveable()
{
   return m_bMoveable;
}

void GBaseWindow::Moveable(bool in_bMoveable)
{
   m_bMoveable = in_bMoveable;
}

void GBaseWindow::Dirty(bool in_bDirty)
{
   m_bDirty = in_bDirty;
   if(m_bDirty)
   {
      if(m_pObjApplyBtn)
         m_pObjApplyBtn->Enabled(true);
   }
   else
   {
      if(m_pObjApplyBtn)
         m_pObjApplyBtn->Enabled(false);
   }
}

bool GBaseWindow::Dirty()
{
   return m_bDirty;
}

bool GBaseWindow::OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller)
{
   if(in_pFocusFrom == g_ClientDDL.GenericMessageWindow())
      return true;

   BringToFront();  

   if(g_ClientDDL.MainBar() && g_ClientDDL.MainBar()->Visible())
      g_ClientDDL.MainBar()->BringToFront();

   if(g_ClientDDL.MiniMap() && g_ClientDDL.MiniMap()->Visible())      
      g_ClientDDL.MiniMap()->BringToFront();
   
   return false;
}

void GBaseWindow::OnDrag(const GEventData & in_EventData, const GVector2D<INT32> &, const GVector2D<INT32> & in_MouseRelative)
{
   GVector2D<INT32> l_NewPos = m_MouseClick + in_MouseRelative;
   
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if(l_NewPos.x < 50 - Width())
      l_NewPos.x = 50 - Width();
   
   if(l_NewPos.y < 0)
      l_NewPos.y = 0;
   
   if(l_NewPos.x > mode.m_Resolution.x - 50)
      l_NewPos.x = mode.m_Resolution.x - 50;

   if(l_NewPos.y + Height() > mode.m_Resolution.y)
      l_NewPos.y = mode.m_Resolution.y - Height();

   Position(l_NewPos);
}

void GBaseWindow::OnDragEnd()
{
   ClipCursor(NULL);
}

void GBaseWindow::Minimize()
{
   if(m_pActionResize)
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pActionResize);
      SAFE_RELEASE(m_pActionResize);
      m_pActionResize = NULL;
   }

   if(Visible())
   {     
      m_pActionResize = g_Joshua.Renderer()->Create_Action();
      m_pActionResize->Length(0.5f);

      CGFX_Key_Linear l_ModelKey[2] =
      {
         {0.f,  (REAL32)m_iLastHeight},
         {0.5f,  34.f},
      };

      CGFX_Animation_Linear_Itf* l_pSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
      l_pSizeYAnim->Setup_Keys(l_ModelKey, 2);
      l_pSizeYAnim->Connect(Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
      m_pActionResize->Add_Animation(l_pSizeYAnim);
      SAFE_RELEASE(l_pSizeYAnim);

      if(m_pObjInnerFrm)
      {
         CGFX_Key_Linear l_InnerSizeKey[2] =
         {
            {0.f,  (REAL32)m_pObjInnerFrm->Height()},
            {0.5f, 0.f},
         };

         CGFX_Animation_Linear_Itf* l_pInnerSizeAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pInnerSizeAnim->Setup_Keys(l_InnerSizeKey, 2);
         l_pInnerSizeAnim->Connect(m_pObjInnerFrm->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pInnerSizeAnim);
         SAFE_RELEASE(l_pInnerSizeAnim);
      }

      if(m_pObjColorGradientPic)
      {
         CGFX_Key_Linear l_ModelKey[2] =
         {
            {0.f,  (REAL32)(m_iLastHeight - 8)},
            {0.5f,  26.f},
         };

         CGFX_Animation_Linear_Itf* l_pGradientSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pGradientSizeYAnim->Setup_Keys(l_ModelKey, 2);
         l_pGradientSizeYAnim->Connect(m_pObjColorGradientPic->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pGradientSizeYAnim);
         SAFE_RELEASE(l_pGradientSizeYAnim);
      }

      if(m_pObjGradientPic)
      {
         CGFX_Key_Linear l_ModelKey[2] =
         {
            {0.f,  (REAL32)(m_iLastHeight - 8)},
            {0.5f,  26.f},
         };

         CGFX_Animation_Linear_Itf* l_pGradientSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pGradientSizeYAnim->Setup_Keys(l_ModelKey, 2);
         l_pGradientSizeYAnim->Connect(m_pObjGradientPic->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pGradientSizeYAnim);
         SAFE_RELEASE(l_pGradientSizeYAnim);
      }

      CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
      UINT32 l_iId = l_pScene->Get_Animation_System()->Get_Action_Index(m_pActionResize);
      if(l_iId == INVALID_INDEX)
      {
         l_iId = l_pScene->Get_Animation_System()->Add_Action(m_pActionResize);
      }
      else
      {
         l_pScene->Get_Animation_System()->Stop_Action(l_iId);
      }

      l_pScene->Get_Animation_System()->Set_Action_Time(l_iId, 0);
      l_pScene->Get_Animation_System()->Play_Action(l_iId);
   }
   else
   {
      Height(34);

      if(m_pObjColorGradientPic)
         m_pObjColorGradientPic->Height(26);

      if(m_pObjInnerFrm)
         m_pObjInnerFrm->Model()->Visibility() = false;
   }

   m_bMinimized = true;
}

bool GBaseWindow::Minimized()
{
   return m_bMinimized;
}

void GBaseWindow::Maximize()
{
   if(m_pActionResize)
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pActionResize);
      SAFE_RELEASE(m_pActionResize);
      m_pActionResize = NULL;
   }

   if(Visible())
   {
      m_pActionResize = g_Joshua.Renderer()->Create_Action();
      m_pActionResize->Length(0.5f);

      CGFX_Key_Linear l_ModelKey[2] =
      {
         {0.f,  (REAL32)Height()},
         {0.5f, (REAL32)m_iLastHeight},
      };

      CGFX_Animation_Linear_Itf* l_pSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
      l_pSizeYAnim->Setup_Keys(l_ModelKey, 2);
      l_pSizeYAnim->Connect(Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
      m_pActionResize->Add_Animation(l_pSizeYAnim);
      SAFE_RELEASE(l_pSizeYAnim);

      if(m_pObjInnerFrm)
      {
         CGFX_Key_Linear l_InnerSizeKey[2] =
         {
            {0.f,  (REAL32)m_pObjInnerFrm->Height()},
            {0.5f, (REAL32)(m_iLastHeight - m_pObjInnerFrm->Top() - 4)},
         };

         CGFX_Animation_Linear_Itf* l_pInnerSizeAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pInnerSizeAnim->Setup_Keys(l_InnerSizeKey, 2);
         l_pInnerSizeAnim->Connect(m_pObjInnerFrm->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pInnerSizeAnim);
         SAFE_RELEASE(l_pInnerSizeAnim);
      }

      if(m_pObjColorGradientPic)
      {
         CGFX_Key_Linear l_ModelKey[2] =
         {
            {0.f,  (REAL32)m_pObjColorGradientPic->Height()},
            {0.5f, (REAL32)(m_iLastHeight - 8)},
         };

         CGFX_Animation_Linear_Itf* l_pGradientSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pGradientSizeYAnim->Setup_Keys(l_ModelKey, 2);
         l_pGradientSizeYAnim->Connect(m_pObjColorGradientPic->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pGradientSizeYAnim);
         SAFE_RELEASE(l_pGradientSizeYAnim);
      }

      if(m_pObjGradientPic)
      {
         CGFX_Key_Linear l_ModelKey[2] =
         {
            {0.f,  (REAL32)m_pObjColorGradientPic->Height()},
            {0.5f, (REAL32)(m_iLastHeight - 8)},
         };

         CGFX_Animation_Linear_Itf* l_pGradientSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
         l_pGradientSizeYAnim->Setup_Keys(l_ModelKey, 2);
         l_pGradientSizeYAnim->Connect(m_pObjGradientPic->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
         m_pActionResize->Add_Animation(l_pGradientSizeYAnim);
         SAFE_RELEASE(l_pGradientSizeYAnim);
      }

      CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
      UINT32 l_iId = l_pScene->Get_Animation_System()->Get_Action_Index(m_pActionResize);
      if(l_iId == INVALID_INDEX)
      {
         l_iId = l_pScene->Get_Animation_System()->Add_Action(m_pActionResize);
      }
      else
      {
         l_pScene->Get_Animation_System()->Stop_Action(l_iId);
      }

      l_pScene->Get_Animation_System()->Set_Action_Time(l_iId, 0);
      l_pScene->Get_Animation_System()->Play_Action(l_iId);

   }
   else
   {
      Height(m_iLastHeight);

      if(m_pObjColorGradientPic)
         m_pObjColorGradientPic->Height(m_iLastHeight - 8);

      if(m_pObjInnerFrm)
      {
         m_pObjInnerFrm->Model()->Visibility() = true;
         m_pObjInnerFrm->Height(m_iLastHeight - m_pObjInnerFrm->Top() - 4);
      }
   }

   m_bMinimized = false;
}

/*!
 * close the window without applying
 **/
void GBaseWindow::ExitWindow()
{
   // check if we had something to save with the user before hiding
   if(Dirty() && (g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() || 
                  !NeedCountryCheckToSave()))
   {
      if(!g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow)
      {
         g_ClientDDL.SaveConfirmationWindow()->Caller(this);
         g_ClientDDL.SaveConfirmationWindow()->Show();
      }
      else if(g_ClientDAL.GenericDataHolder().m_bSaveConfirmationAlwaysApply)
      {
         Save();
         Hide();
      }
   }
   else
   {
      Hide();
   }
}

/*!
 * apply window data changes
 **/
void GBaseWindow::Apply()
{
   if((g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID()) || 
      !NeedCountryCheckToSave())
      Save();
}

/*!
 * returns true if the window accept the escape key
 **/
bool GBaseWindow::AcceptEscape() const
{
   bool l_bAcceptEscape = false;
   if(m_pObjExitBtn)
   {
      if(m_pObjExitBtn->Visible() && m_pObjExitBtn->Enabled())
      {
         l_bAcceptEscape = true;
      }
   }
   return l_bAcceptEscape;
}

/*!
 * on key down event handler
 **/
GUI::EEventStatus::Enum GBaseWindow::OnKeyDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Key())
   {
      // escape key pressed ?
      case VK_ESCAPE:
      {
         // handle it ?
         if(AcceptEscape())
         {
            // close our window
            Close();
            return EEventStatus::Handled;
         }
      }
      default:
      {
         // check if we dont have any owner but we have a parent
         if(!Owner() && Parent())
         {
            // broadcast our key down event to the parent
            if(Parent()->OnKeyDown(in_EventData, in_pCaller) == EEventStatus::Handled)
            {
               // if our parent handled it, notify
               return EEventStatus::Handled;
            }
         }
      }
   }
   // default keydown behavior instead, gframe will handle it
   return __super::OnKeyDown(in_EventData, in_pCaller);
}

/*!
 * Close the window like the click on the exit button
 **/
void GBaseWindow::Close()
{
   GEventData l_Event; l_Event.Mouse.Actor.Bits.Left = 1;
   OnMouseClick(l_Event, m_pObjExitBtn);
}
