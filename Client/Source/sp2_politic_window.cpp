/**************************************************************
*
* sp2_politic_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_politic_window.h"

#include "../include/sp2_constitutional_form_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_internal_laws_window.h"
#include "../include/sp2_treaties_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GPoliticWindow::TypeName(L"Politic_Window");

GUI::GBaseObject* GPoliticWindow::New()
{
	return (GUI::GBaseObject*) new GPoliticWindow();
}


GPoliticWindow::GPoliticWindow() : GBaseWindow()
{
   m_pData       = &(g_ClientDAL.DataPoliticWindow);
   m_pWindowData = &(g_ClientDAL.DataPoliticWindow);

   m_pObjIdeology = NULL;
   m_pObjApproval = NULL;
   m_pObjPressure = NULL;
   m_pObjStability = NULL;
   m_pObjCorruption = NULL;

   m_pObjApprovalTxt = NULL;
   m_pObjPressureTxt = NULL;
   m_pObjStabilityTxt = NULL;
   m_pObjCorruptionTxt = NULL;
   m_pObjConstitutionalFormTxt = NULL;

   m_pObjConstitutionalFormMoreBtn = NULL;
   m_pObjInternalLawsBtn = NULL;
   m_pObjTreatiesBtn = NULL;
   m_bWindowCanDisable = true;
}

GPoliticWindow::~GPoliticWindow()
{
}


bool GPoliticWindow::Enabled()
{
   return m_bEnabled;
}

void GPoliticWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   m_pObjApprovalTxt->Enabled( in_bEnabled );
   m_pObjPressureTxt->Enabled( in_bEnabled );
   m_pObjStabilityTxt->Enabled( in_bEnabled );
   m_pObjCorruptionTxt->Enabled( in_bEnabled );
   m_pObjConstitutionalFormTxt->Enabled( in_bEnabled );

   GUI::GFrame* l_pInnerFrame = (GUI::GFrame*)Child(L"frmInner");

   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmIdeology")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmApproval")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmPressure")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmStability")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmCorruption")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)l_pInnerFrame->Child(L"frmConstitutionalForm")->Child(L"lblTitle"))->Enabled( in_bEnabled );
}

GUI::EEventStatus::Enum GPoliticWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if((in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left) && in_pCaller)
      {
         if(in_pCaller == m_pObjConstitutionalFormMoreBtn)
         {
            if(!g_ClientDDL.ConstitutionalFormWindow())
               g_ClientDDL.ConstitutionalFormWindowSpawn();

            if(!g_ClientDDL.ConstitutionalFormWindow()->Visible())
               g_ClientDDL.ConstitutionalFormWindow()->Show();
            else
               g_ClientDDL.ConstitutionalFormWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjInternalLawsBtn)
         {
            if(!g_ClientDDL.InternalLawsWindow())
               g_ClientDDL.InternalLawsWindowSpawn();

            if(!g_ClientDDL.InternalLawsWindow()->Visible())
               g_ClientDDL.InternalLawsWindow()->Show();
            else
               g_ClientDDL.InternalLawsWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjTreatiesBtn)
         {
            if(!g_ClientDDL.TreatiesWindow())         
               g_ClientDDL.TreatiesWindowSpawn();

            if(!g_ClientDDL.TreatiesWindow()->Visible())    
            {
               const GCountry& l_Ctry = g_ClientDAL.Country(g_ClientDCL.SelectedCountryID() );
               g_ClientDDL.TreatiesWindow()->SetFilter(l_Ctry.Name(), ETreatyType::Undefined, false);
               g_ClientDDL.TreatiesWindow()->Show();
               //g_ClientDDL.AnimationPlayRectangle(m_pObjTreatiesBtn, g_ClientDDL.TreatiesWindow());
               //g_ClientDDL.TimedVisibilityUpdate(g_ClientDDL.TreatiesWindow(), true, 0.5f);
            }
            else
               g_ClientDDL.TreatiesWindow()->BringToFront();
         }

         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.PoliticWindow()->Hide();
            g_ClientDDL.KillWindowsPoliticLv2();
            g_ClientDDL.MainBar()->m_pObjPolitic->ToggleState(false);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}


bool GPoliticWindow::OnCreate()
{
	__super::OnCreate();

   //Initialize often used pointer to window objects
   GUI::GFrame* l_pInnerFrame = (GUI::GFrame*)Child(L"frmInner");

   m_pObjIdeology      = (GIdeologyMeter*)l_pInnerFrame->Child(L"frmIdeology")->Child(L"frmIdeologyCursor");
   m_pObjApproval      = (GCursorMeter*)l_pInnerFrame->Child(L"frmApproval")->Child(L"frmGradient");
   m_pObjPressure      = (GCursorMeter*)l_pInnerFrame->Child(L"frmPressure")->Child(L"frmGradient");
   m_pObjStability     = (GCursorMeter*)l_pInnerFrame->Child(L"frmStability")->Child(L"frmGradient");
   m_pObjCorruption    = (GCursorMeter*)l_pInnerFrame->Child(L"frmCorruption")->Child(L"frmGradient");

   m_pObjApprovalTxt   = (GUI::GLabel*)l_pInnerFrame->Child(L"frmApproval")->Child(L"txtValue");
   m_pObjPressureTxt   = (GUI::GLabel*)l_pInnerFrame->Child(L"frmPressure")->Child(L"txtValue");
   m_pObjStabilityTxt  = (GUI::GLabel*)l_pInnerFrame->Child(L"frmStability")->Child(L"txtValue");
   m_pObjCorruptionTxt = (GUI::GLabel*)l_pInnerFrame->Child(L"frmCorruption")->Child(L"txtValue");

   m_pObjConstitutionalFormTxt = (GUI::GLabel*)l_pInnerFrame->Child(L"frmConstitutionalForm")->Child(L"txtValue");

   m_pObjConstitutionalFormMoreBtn = (GUI::GButton*)l_pInnerFrame->Child(L"frmConstitutionalForm")->Child(L"btnMoreConstitutionalForm");
   m_pObjInternalLawsBtn           = (GUI::GButton*)l_pInnerFrame->Child(L"frmIdeology")->Child(L"btnInternalLaws");
   m_pObjTreatiesBtn               = (GUI::GButton*)l_pInnerFrame->Child(L"btnTreaties");

   m_pObjConstitutionalFormMoreBtn->TooltipText(g_ClientDAL.GetString(100540));
   
   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   //Bring to front object that are going to be behind
   m_pObjConstitutionalFormMoreBtn->BringToFront();

   //Initialize Cursor Meter to their good value and cursor picture
   m_pObjApproval->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjPressure->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));  
   m_pObjStability->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjCorruption->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_RED));
   
   //Set window position
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if(m_pData->Left < 0)
      Position(3,(INT16)mode.m_Resolution.y - 99 - Height());  

   if(Visible())
      Update();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GPoliticWindow::OnShow()
{
   __super::OnShow();
   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   if(m_pObjIdeology)
      Update();
   
   if(g_ClientDAL.ControlledCountryID() != g_ClientDCL.SelectedCountryID())
   {
      g_ClientDAL.FetchConstitutionalFormInfo(g_ClientDCL.SelectedCountryID());
      g_ClientDAL.FetchInternalLawsInfo(g_ClientDCL.SelectedCountryID());
   }

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Politic);
}

void GPoliticWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Politic);

   __super::OnHide();
}

void GPoliticWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Politic)
      Update();
}

//! Update politic window data
bool GPoliticWindow::Update()
{
   //Get Data we need to display
   REAL32 l_fApproval   = 0.f;
   REAL32 l_fStability  = 0.f;
	REAL32 l_fPressure	= 0.f;
   REAL32 l_fCorruption = 0.f;
   REAL32 l_fIdeology   = 0.f;
   GString l_sGvtType;

   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      l_fApproval   = g_ClientDAL.m_PlayerCountryData.GvtApproval();
      l_fStability  = g_ClientDAL.m_PlayerCountryData.GvtStability();
		l_fPressure	  = g_ClientDAL.m_PlayerCountryData.Pressure();
      l_fCorruption = g_ClientDAL.m_PlayerCountryData.CorruptionLevel();
      l_fIdeology   = g_ClientDAL.m_PlayerCountryData.CountryPoliticalIdeology();

      if(!g_ClientDAL.m_PlayerCountryData.MartialLaw())
      {
         if(g_ClientDAL.m_PlayerCountryData.GvtType() >= 1 && g_ClientDAL.m_PlayerCountryData.GvtType() <= 9)
            l_sGvtType = g_ClientDAL.GetString(g_ClientDAL.StringIdGvtType(g_ClientDAL.m_PlayerCountryData.GvtType()));
      }
      else
         l_sGvtType = g_ClientDAL.GetString(EStrId::MartialLaw);
   }
   else
   {
      l_fApproval   = m_pData->GvtApproval;
      l_fStability  = m_pData->GvtStability;
		l_fPressure	  = m_pData->Pressure;
      l_fCorruption = m_pData->CorruptionLevel;
      l_fIdeology   = m_pData->PoliticalIdeology;
      
      if(!m_pData->MartialLaw)
      {
         if(m_pData->GvtType >= 1 && m_pData->GvtType <= 9)
            l_sGvtType = g_ClientDAL.GetString(g_ClientDAL.StringIdGvtType(m_pData->GvtType));
      }
      else
         l_sGvtType = g_ClientDAL.GetString(EStrId::MartialLaw);
   }

   //Set the value on the meters
   m_pObjIdeology->Value(l_fIdeology,0);
   m_pObjApproval->Value(l_fApproval,0);
   m_pObjStability->Value(l_fStability,0);
   m_pObjCorruption->Value(l_fCorruption,0);
   m_pObjPressure->Value(l_fPressure,0);
   
   //Set percentage text
   if(l_fApproval == 1.f)
      m_pObjApprovalTxt->Text(GString::FormatNumber((REAL64)l_fApproval * 100,L"",L".",L"",L" %",0,0));
   else
      m_pObjApprovalTxt->Text(GString::FormatNumber((REAL64)l_fApproval * 100,L"",L".",L"",L" %",0,1));

   if(l_fStability == 1.f)
      m_pObjStabilityTxt->Text(GString::FormatNumber((REAL64)l_fStability * 100,L"",L".",L"",L" %",0,0));
   else
      m_pObjStabilityTxt->Text(GString::FormatNumber((REAL64)l_fStability * 100,L"",L".",L"",L" %",0,1));

   if(l_fCorruption == 1.f)
      m_pObjCorruptionTxt->Text(GString::FormatNumber((REAL64)l_fCorruption * 100,L"",L".",L"",L" %",0,0));
   else
      m_pObjCorruptionTxt->Text(GString::FormatNumber((REAL64)l_fCorruption * 100,L"",L".",L"",L" %",0,1));
	
	if(l_fPressure == 1.f)
      m_pObjPressureTxt->Text(GString::FormatNumber((REAL64)l_fPressure * 100,L"",L".",L"",L" %",0,0));
   else
      m_pObjPressureTxt->Text(GString::FormatNumber((REAL64)l_fPressure * 100,L"",L".",L"",L" %",0,1));  
   
   //Set constitutional form text considering martial law
   m_pObjConstitutionalFormTxt->Text(l_sGvtType);

   return true;
}
