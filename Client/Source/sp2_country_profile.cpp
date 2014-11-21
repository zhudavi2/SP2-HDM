/**************************************************************
*
* sp2_country_profile.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_country_profile.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCountryProfileWindow::TypeName(L"Country_Profile_Window");

GUI::GBaseObject* GCountryProfileWindow::New()
{
	return (GUI::GBaseObject*) new GCountryProfileWindow();
}


GCountryProfileWindow::GCountryProfileWindow() : GBaseWindow()
{
}

GCountryProfileWindow::~GCountryProfileWindow()
{
}

bool GCountryProfileWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

   m_pObjConfirmBtn = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancelBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

   m_pObjSuggestedLst = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmSuggested")->Child(L"lstCountries");

   for(INT32 i=0; i<10; i++)
   {
      m_pObjSuggestedLst->Insert_New_Row();
   }
      
   m_pObjWantedEconBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"frmEconomic");
   m_pObjWantedMiliBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"frmMilitary");
   m_pObjWantedPoliBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"frmPolitic");

   m_pObjSuggesEconBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmSuggested")->Child(L"frmEconomic");
   m_pObjSuggesMiliBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmSuggested")->Child(L"frmMilitary");
   m_pObjSuggesPoliBar = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmSuggested")->Child(L"frmPolitic");

   m_pObjPoliticTog    = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"togPolitic");
   m_pObjEconomicTog   = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"togEconomic");
   m_pObjMilitaryTog   = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmDesired")->Child(L"togMilitary");

   m_pObjWantedEconBar->Initialize();
   m_pObjWantedMiliBar->Initialize();
   m_pObjWantedPoliBar->Initialize();

   m_pObjWantedEconBar->CursorDragable()[0] = true;
   m_pObjWantedMiliBar->CursorDragable()[0] = true;
   m_pObjWantedPoliBar->CursorDragable()[0] = true;

   m_pObjWantedPoliBar->RightBound(190);
   m_pObjWantedMiliBar->RightBound(190);
   m_pObjWantedEconBar->RightBound(190);

   m_pObjWantedEconBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjWantedMiliBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjWantedPoliBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));

   m_pObjWantedEconBar->Owner(this);
   m_pObjWantedMiliBar->Owner(this);
   m_pObjWantedPoliBar->Owner(this);


   m_pObjSuggesEconBar->Enabled(false);
   m_pObjSuggesMiliBar->Enabled(false);
   m_pObjSuggesPoliBar->Enabled(false);

   m_pObjSuggesEconBar->Initialize();
   m_pObjSuggesMiliBar->Initialize();
   m_pObjSuggesPoliBar->Initialize();

   m_pObjSuggesEconBar->RightBound(190);
   m_pObjSuggesMiliBar->RightBound(190);
   m_pObjSuggesPoliBar->RightBound(190);

   m_pObjSuggesEconBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjSuggesMiliBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjSuggesPoliBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));

   m_pObjWantedEconBar->Value(0.5, 0, false, false);
   m_pObjWantedMiliBar->Value(0.5, 0, false, false);
   m_pObjWantedPoliBar->Value(0.5, 0, false, false);

   m_pObjConfirmBtn->Enabled(false);

   m_pObjSuggesEconBar->Value(0, 0, false, false);
   m_pObjSuggesMiliBar->Value(0, 0, false, false);
   m_pObjSuggesPoliBar->Value(0, 0, false, false);

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCountryProfileWindow::OnShow()
{
   __super::OnShow();

   CalculatePossibleCountries();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GCountryProfileWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn || in_pCaller == m_pObjCancelBtn)
         {
            Hide();
            return GUI::EEventStatus::Handled;
         }
         else if(in_pCaller == m_pObjConfirmBtn)
         {
            if(m_pObjSuggestedLst->Get_Selected_Item().size())
            {
               GUI::GListBox::GRow* l_pRow = m_pObjSuggestedLst->Get_Row_At( m_pObjSuggestedLst->Get_Selected_Item()[0] );
               
               const GCountry & l_Ctry = g_ClientDAL.Country( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );
               GEventData l_Data;
               l_Data.Data = (void*)(INT32)l_Ctry.Id();
               m_pCaller->FireCustomEvent(c_iOnCountryProfileWindowSelection, l_Data, false, this);

               Hide();
            }
            else
            {
               m_pObjConfirmBtn->Enabled(false);
            }
         }
         else if(in_pCaller == m_pObjPoliticTog || in_pCaller == m_pObjEconomicTog || in_pCaller == m_pObjMilitaryTog)
         {
            CalculatePossibleCountries();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GCountryProfileWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{   
   switch(in_iEventID)
   {
   case c_OnMeterChangeEvent:
      {
         CalculatePossibleCountries();
      }
      break;
   case GUI::ECustomEvents::OnListItemSelected:
      {
         const vector<INT32> & l_viSelected = m_pObjSuggestedLst->Get_Selected_Item();
         if(l_viSelected.size())
         {
            if(l_viSelected[0] < 0)
               return GUI::EEventStatus::Error;

            GUI::GListBox::GRow* l_pRow = m_pObjSuggestedLst->Get_Row_At( l_viSelected[0] );

            if(((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() != L"")
            {
               const GCountry & l_Ctry = g_ClientDAL.Country( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );

               if(l_Ctry.Id() > 0)
               {
                  INT16 l_iNbCountries = g_ClientDAL.NbCountry();

                  m_pObjConfirmBtn->Enabled(true);

                  UINT32 i=0;
                  for(i=0; i< m_pCountriesID->size(); i++)
                  {
                     if(m_pCountriesID->at(i) == l_Ctry.Id())
                     {
                        m_pObjSuggesEconBar->Value((REAL32)(l_iNbCountries + 1 - m_pEconomicRank->at(i)) / (REAL32)l_iNbCountries, 0, false, false);
                        m_pObjSuggesPoliBar->Value((REAL32)(l_iNbCountries + 1 - m_pPoliticRank->at(i) ) / (REAL32)l_iNbCountries, 0, false, false);
                        m_pObjSuggesMiliBar->Value((REAL32)(l_iNbCountries + 1 - m_pMilitaryRank->at(i)) / (REAL32)l_iNbCountries, 0, false, false);
                        break;
                     }
                  }
                  gassert(i < m_pCountriesID->size(),"value was not found");
               }
               else
               {
                  m_pObjConfirmBtn->Enabled(false);
                  m_pObjSuggesEconBar->Value(0, 0, false, false);
                  m_pObjSuggesPoliBar->Value(0, 0, false, false);
                  m_pObjSuggesMiliBar->Value(0, 0, false, false);
               }
            }
            else
            {
               m_pObjConfirmBtn->Enabled(false);
               m_pObjSuggesEconBar->Value(0, 0, false, false);
               m_pObjSuggesPoliBar->Value(0, 0, false, false);
               m_pObjSuggesMiliBar->Value(0, 0, false, false);
            }
         }
      }
      break;
   default:
      return __super::OnCustomEvent(in_iEventID, in_EventData, in_pCaller);
   }

   return GUI::EEventStatus::Handled;
}

void GCountryProfileWindow::CalculatePossibleCountries()
{
   m_pObjConfirmBtn->Enabled(false);
   m_pObjSuggestedLst->ClearSelection();
   
   INT16 l_iNbCountries = (INT16)m_pCountriesID->size();

   //Get the wanted rank
   INT16 l_iWantedEcon = -1;
   if(m_pObjEconomicTog->ToggleState())
   {
      l_iWantedEcon = (INT16)floor( ((1 - m_pObjWantedEconBar->Value(0)) * (REAL32)l_iNbCountries) );
      if(!l_iWantedEcon)
         l_iWantedEcon = 1;
   }

   INT16 l_iWantedPoli = -1;
   if(m_pObjPoliticTog->ToggleState())
   {
      l_iWantedPoli = (INT16)floor( ((1 - m_pObjWantedPoliBar->Value(0)) * (REAL32)l_iNbCountries) );
      if(!l_iWantedPoli)
         l_iWantedPoli = 1;
   }

   INT16 l_iWantedMili = -1;
   if(m_pObjMilitaryTog->ToggleState())
   {
      l_iWantedMili = (INT16)floor( ((1 - m_pObjWantedMiliBar->Value(0)) * (REAL32)l_iNbCountries) );
      if(!l_iWantedMili)
         l_iWantedMili = 1;
   }

   //If the wanted rank result is 0, the wanted rank is 1 : (1 - 1) * 193 = 0

   multimap<REAL32, INT16> m_Results;
   multimap<REAL32, INT16>::iterator l_It;
      
   //for each country
   for(INT32 i=0; i<(INT32)m_pCountriesID->size(); i++)
   {
      //Get the sum of all ranks
      INT16 l_iCountryID = m_pCountriesID->at(i);
      UINT8 l_iFiltersActive   = 0;

      //Get the delta average, between want the user want and the country
      REAL32 l_fDeltaAvg = 0.f;
      
      if(l_iWantedEcon > 0)
      {
         l_fDeltaAvg += (REAL32)abs(l_iWantedEcon - m_pEconomicRank->at(i));
         l_iFiltersActive++;
      }

      if(l_iWantedPoli > 0)
      {
         l_fDeltaAvg += (REAL32)abs(l_iWantedPoli - m_pPoliticRank->at(i));
         l_iFiltersActive++;
      }

      if(l_iWantedMili > 0)
      {
         l_fDeltaAvg += (REAL32)abs(l_iWantedMili - m_pMilitaryRank->at(i));
         l_iFiltersActive++;
      }
      
      if(l_iFiltersActive)
      {
         l_fDeltaAvg = l_fDeltaAvg / (REAL32)l_iFiltersActive;
      }
      else
      {
         l_fDeltaAvg = -1;
      }
      
      m_Results.insert( make_pair(l_fDeltaAvg, l_iCountryID) );
   }
   
   m_pObjSuggesEconBar->Value(0, 0, false, false);
   m_pObjSuggesPoliBar->Value(0, 0, false, false);
   m_pObjSuggesMiliBar->Value(0, 0, false, false);

   //Fill the five found country
   INT32 l_iCounter = 0;
   l_It = m_Results.begin();
   while(l_iCounter < 10)
   {
      if(l_It->first != -1)
      {
         const GCountry & l_Ctry = g_ClientDAL.Country( l_It->second );
         
         GUI::GListBox::GRow* l_pRow = m_pObjSuggestedLst->Get_Row_At(l_iCounter);
         ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(l_Ctry.Name());
      }
      else
      {
         GUI::GListBox::GRow* l_pRow = m_pObjSuggestedLst->Get_Row_At(l_iCounter);
         ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal( GString(L"000159"));
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(L"");
      }

      l_iCounter++;
      l_It++;
   }
}

void GCountryProfileWindow::Show(GUI::GBaseObject* in_pCaller, vector<INT16> & in_viCountriesID, vector<INT16> & in_viEconomicRank, vector<INT16> & in_viPoliticRank, vector<INT16> & in_viMilitaryRank)
{
   m_pCaller = in_pCaller; 
   m_pCountriesID  = &in_viCountriesID;
   m_pMilitaryRank = &in_viMilitaryRank;
   m_pPoliticRank  = &in_viPoliticRank;
   m_pEconomicRank = &in_viEconomicRank;

   __super::Show();
}
