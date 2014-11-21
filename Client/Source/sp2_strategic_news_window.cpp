/**************************************************************
*
* sp2_strategic_news_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
/*
#include "golem_pch.hpp"

#include "../include/sp2_strategic_news_window.h"

const GString GStrategicNewsWindow::TypeName(L"Strategic_News_Window");

GUI::GBaseObject* SP2::GStrategicNewsWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicNewsWindow();
}


GStrategicNewsWindow::GStrategicNewsWindow() : GFrame()
{
   m_pSlidingTrack = NULL;
}

GStrategicNewsWindow::~GStrategicNewsWindow()
{
}

bool GStrategicNewsWindow::OnCreate()
{
	__super::OnCreate();

   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : " + this->Name());

   Position(0,0);
   m_pSlidingTrack = g_ClientDDL.AnimationCreateSliding(0.5f, 0.51f, Top() - Height(), Top(), Model()->Get_Transform());

	return true;
}

void GStrategicNewsWindow::OnShow()
{
   __super::OnShow();
}

GUI::EEventStatus::Enum GStrategicNewsWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicNewsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   return GUI::EEventStatus::Handled;
}

CGFX_Track_Itf* & GStrategicNewsWindow::AnimationSlidingTrack()
{
   return m_pSlidingTrack;
}

bool GStrategicNewsWindow::OnDestroy()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : " + this->Name());
   __super::OnDestroy();

   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(m_pSlidingTrack);

   SAFE_RELEASE(m_pSlidingTrack);

   return true;
}*/