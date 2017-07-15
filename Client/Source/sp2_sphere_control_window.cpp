/**************************************************************
*
* sp2_sphere_conrol_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_sphere_control_window.h"

const GString GSphereControlWindow::TypeName(L"Sphere_Control_Window");
const UINT16  c_iDefaultWidth = 145;

GUI::GBaseObject* SP2::GSphereControlWindow::New()
{
	return (GUI::GBaseObject*) new GSphereControlWindow();
}


GSphereControlWindow::GSphereControlWindow() : GFrame()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Sphere Control Window");
}

GSphereControlWindow::~GSphereControlWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Sphere Control Window");
}

bool GSphereControlWindow::OnCreate()
{
	__super::OnCreate();

	// Get Frames 
	m_pMilitaryFrame  = (GUI::GFrame*)Child(L"frmMilitaryCountry");
	m_pPoliticalFrame = (GUI::GFrame*)Child(L"frmPoliticCountry");
   m_pRegionNameLbl  = (GUI::GLabel*)Child(L"lblRegion");

   // Get pic and label
	m_pMilitaryControlPic	= (GUI::GPictureBox*)m_pMilitaryFrame->Child(L"picFlag");
	m_pMilitaryLabel		   = (GUI::GLabel*)m_pMilitaryFrame->Child(L"lblCountry");

	// Get pic and label
	m_pPoliticalControlPic	= (GUI::GPictureBox*)m_pPoliticalFrame->Child(L"picFlag");
	m_pPoliticalLabel	      = (GUI::GLabel*)m_pPoliticalFrame->Child(L"lblCountry");

   m_pMilitaryLabel->TextOffset( 3, 0 );
   m_pPoliticalLabel->TextOffset( 3, 0 );
   m_pMilitaryLabel->TextAlignment( L"MiddleLeft" );
   m_pPoliticalLabel->TextAlignment( L"MiddleLeft" );
	
   return true;
}

void GSphereControlWindow::SetPoliticalControl(const GCountry  * in_pCountry)
{
    // Make sure we have valid country pointer
	gassert(in_pCountry != NULL,"Invalid country pointer");
	
	// Set Country code and flag
   GString l_sCode(in_pCountry->Flag());

   m_pPoliticalLabel->Text( in_pCountry->Name() );
	m_pPoliticalControlPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[in_pCountry->Id()]);

   m_pPoliticalLabel->Width( m_pPoliticalLabel->Text2D()->Width() + 6 );
   m_pPoliticalFrame->Width( m_pPoliticalLabel->Width() + 25 );

   UpdateSize();
}

void GSphereControlWindow::SetMilitaryControl(const GCountry * in_pCountry)
{
    // Make sure we have valid country pointer
	gassert(in_pCountry != NULL,"Invalid country pointer");

	// Set Country flag and code
   GString l_sCode(in_pCountry->Flag());

   m_pMilitaryLabel->Text( in_pCountry->Name() );
	m_pMilitaryControlPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[in_pCountry->Id()]);
 
   m_pMilitaryLabel->Width( m_pMilitaryLabel->Text2D()->Width() + 6 );
   m_pMilitaryFrame->Width( m_pMilitaryLabel->Width() + 25 );

   UpdateSize();
}


void GSphereControlWindow::SetRegionName(const GString &in_region)
{
   m_pRegionNameLbl->Text(in_region); 
   
   m_pRegionNameLbl->Width(m_pRegionNameLbl->Text2D()->Width() + 2);

   UpdateSize();
}

void GSphereControlWindow::UpdateSize()
{
   //Check the longest label and update size accordingly to the longest
   bool l_bLongestPolitical = false; 
   bool l_bLongestMilitary  = false;
   bool l_bLongestRegion    = false;

   INT32 l_iMilitary  = m_pMilitaryLabel->Text2D()->Width();
   INT32 l_iPolitical = m_pPoliticalLabel->Text2D()->Width();
   INT32 l_iRegion    = m_pRegionNameLbl->Text2D()->Width();

   if( l_iMilitary >= l_iPolitical && (l_iMilitary + 118) >= l_iRegion ) 
   {
      l_bLongestMilitary = true;
   }
   else if( l_iPolitical >= l_iMilitary && (l_iPolitical + 118) >= l_iRegion )
   {  
      l_bLongestPolitical = true;
   }
   else if( l_iRegion >= (l_iPolitical + 118) && l_iRegion >= (l_iMilitary + 118) )
   {
      l_bLongestRegion = true;
   }

   if(l_bLongestMilitary)  
   {
      Width(m_pMilitaryLabel->Left() + l_iMilitary + 8 + 93);
   }
   else if(l_bLongestPolitical)
   {
      Width(m_pPoliticalLabel->Left() + l_iPolitical + 8 + 93);
   }
   else if(l_bLongestRegion)
   {
      Width( l_iRegion + 6 + 8 );
   }

   m_pPoliticalLabel->Width( Width() - 93 - 2 - 25 );
   m_pPoliticalFrame->Width( m_pPoliticalLabel->Width() + 25 );
   m_pMilitaryLabel->Width( Width() - 93 - 2 - 25);
   m_pMilitaryFrame->Width( m_pMilitaryLabel->Width() + 25 );
   m_pRegionNameLbl->Width( Width() - 4 );
}