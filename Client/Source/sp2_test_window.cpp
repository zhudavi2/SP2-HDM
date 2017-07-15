/**************************************************************
*
* sp2_test_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_test_window.h"

const GString GTestWindow::TypeName(L"frmTest");
/*!
 * Creation function
 **/
GUI::GBaseObject* GTestWindow::New()
{
   return new GTestWindow();
}
GTestWindow::GTestWindow()
{
}

GTestWindow::~GTestWindow()
{
}

bool GTestWindow::OnCreate()
{
   __super::OnCreate();

   ((GFX::GWindow*)this->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Stretch);
   ((GFX::GWindow*)this->Model()->Get_Primitive())->RepeatX() = 3.f;
   ((GFX::GWindow*)this->Model()->Get_Primitive())->RepeatY() = 4.f;


   return true;
}