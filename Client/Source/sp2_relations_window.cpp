/**************************************************************
*
* sp2_relations_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_graph.h"
#include "../include/sp2_relations_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GRelationsWindow::TypeName(L"Relations_Window");

GUI::GBaseObject* GRelationsWindow::New()
{
	return (GUI::GBaseObject*) new GRelationsWindow();
}


GRelationsWindow::GRelationsWindow() : GBaseWindow()
{
   m_pObjCountryACbo = NULL;
   m_pObjCountryBCbo = NULL;
   m_pGraph          = NULL;
}

GRelationsWindow::~GRelationsWindow()
{
}

bool GRelationsWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjCountryACbo = (SDK::GUI::GComboBox*)Child(L"frmInner")->Child(L"frmRelations")->Child(L"cboCountryA");
   m_pObjCountryBCbo = (SDK::GUI::GComboBox*)Child(L"frmInner")->Child(L"frmRelations")->Child(L"cboCountryB");

   //Fill the combo boxes with the list of countries
   {
      const GCountries& l_Countries = g_ClientDAL.Countries();
      g_ClientDDL.ComboboxSetCountriesAndFlag(l_Countries,m_pObjCountryACbo);
      g_ClientDDL.ComboboxSetCountriesAndFlag(l_Countries,m_pObjCountryBCbo);
   }

   //Set the country A as the player controlled country
   {
      GCountry l_PlayerCountry = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID());
      m_pObjCountryACbo->Selected_Content(l_PlayerCountry.Name());
   }

   GUI::GBaseObject* l_pGraphParent = (SDK::GUI::GComboBox*)Child(L"frmInner")->Child(L"frmRelations")->Child(L"UIGraph");

   //Create and position the graph
   {
      m_pGraph = new GGraph(l_pGraphParent->Model());
      m_pGraph->Position(GVector2D<INT32>(0,0));
      m_pGraph->Size(GVector2D<INT32>(l_pGraphParent->Width(),l_pGraphParent->Height()));
      m_pGraph->AxisVisibility(true);
      m_pGraph->AxisColor(c_RelationWindowGraphXAxisColor);
      m_pGraph->NbValues(c_iRelationHistoryNumberOfValuesToKeep);
      m_pGraph->LineColor(c_RelationWindowGraphValueColor);
      m_pGraph->AutomaticValueScale(false);
      m_pGraph->MaxValue(100.f);
      m_pGraph->MinValue(-100.f);
   }

   m_pObjApplyBtn->Hide();

	return true;
}


bool GRelationsWindow::OnDestroy()
{
   //Free allocated memory during the OnCreate process
   SAFE_DELETE(m_pGraph);
   //Unregister ourselves as we want to receive relations update
   g_ClientDCL.EndRequestRelationHistory();
   return __super::OnDestroy();
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GRelationsWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

void GRelationsWindow::OnHide()
{
   __super::OnHide();

   //Unregister ourselves as we want to receive relations update
   g_ClientDCL.EndRequestRelationHistory();
}

void GRelationsWindow::UpdateWithNewCountries(UINT32 in_iCountryA, UINT32 in_iCountryB)
{
	m_pObjCountryACbo->Selected_Content(g_ClientDAL.Country(in_iCountryA).Name());
	m_pObjCountryBCbo->Selected_Content(g_ClientDAL.Country(in_iCountryB).Name());
	g_ClientDCL.StartRequestRelationHistory(in_iCountryA,in_iCountryB);	
}

GUI::EEventStatus::Enum GRelationsWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch(in_EventId)
   {
      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         //Request the history graph for the 2 countries
         //UINT32 l_iCountry1 = m_pC

         //Get the Id of the selected item
         GString l_sNameA = m_pObjCountryACbo->Selected_Content(); 
         GString l_sNameB = m_pObjCountryBCbo->Selected_Content(); 
         if( 
             (l_sNameA != L"") && 
             (l_sNameB != L"") && 
             (l_sNameB != l_sNameA)
           )
         {
            const GCountry& l_CountryA = g_ClientDAL.Country(l_sNameA);
            const GCountry& l_CountryB = g_ClientDAL.Country(l_sNameB);
            g_ClientDCL.StartRequestRelationHistory(l_CountryA.Id(),l_CountryB.Id());
         }
         break;
      }      
   }
   return EEventStatus::Handled;
}

void GRelationsWindow::SetHistoryRelation(const vector<REAL32>& in_vRelationValues)
{
   m_pGraph->Update(in_vRelationValues);
}