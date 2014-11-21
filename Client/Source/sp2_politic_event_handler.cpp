/**************************************************************
*
* sp2_Politic_event_handler.cpp
*
* Description
* ===========
*  Used to handle every Politic events that happen
*  on client side
*
*  This file is currently empty, i'm not even sure it's supposed to
*  exist but by the way it's here if ever we want to handle something
*  on client side
*  
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_politic_window.h"
#include "../include/sp2_constitutional_form_window.h"
#include "../include/sp2_internal_laws_window.h"

GPoliticEventHandler::GPoliticEventHandler()
{
}

GPoliticEventHandler::~GPoliticEventHandler()
{
}

bool GPoliticEventHandler::HandlePoliticWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GPoliticUpdate* l_pUpdate = (SP2::Event::GPoliticUpdate*)in_pEvent.get();

   GDataPoliticWindow & DataPoliticWindow = g_ClientDAL.DataPoliticWindow;

   DataPoliticWindow.GvtType             = l_pUpdate->GvtType;
   DataPoliticWindow.PoliticalIdeology   = l_pUpdate->PoliticalIdeology;
   DataPoliticWindow.GvtApproval         = l_pUpdate->GvtApproval;
   DataPoliticWindow.GvtStability        = l_pUpdate->GvtStability;
   DataPoliticWindow.CorruptionLevel     = l_pUpdate->CorruptionLevel;
	DataPoliticWindow.Pressure				  = l_pUpdate->Pressure;
   DataPoliticWindow.MartialLaw          = l_pUpdate->MartialLaw;

   
   if(g_ClientDDL.PoliticWindow() && g_ClientDDL.PoliticWindow()->Visible())
      g_ClientDDL.PoliticWindow()->Update();

   return true;
}

bool GPoliticEventHandler::HandleConstitutionalFormInfoRequest(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GConstitutionalFormUpdate* l_pUpdate = (SP2::Event::GConstitutionalFormUpdate*)in_pEvent.get(); 

   GDataConstitutionalForm & l_Data = g_ClientDAL.DataConstitutionalForm;
   
   l_Data.CapitalId     = l_pUpdate->CapitalId;
   l_Data.GvtType       = l_pUpdate->GvtType;
   l_Data.MartialLaw    = l_pUpdate->MartialLaw;
   l_Data.NextElection  = l_pUpdate->NextElection;

   if(g_ClientDDL.ConstitutionalFormWindow() && g_ClientDDL.ConstitutionalFormWindow()->Visible())
   {
      g_ClientDDL.ConstitutionalFormWindow()->Reset();
      g_ClientDDL.ConstitutionalFormWindow()->Update(true);
   }

   return true;
}

bool GPoliticEventHandler::HandleInternalLawsUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GInternalLawsUpdate* l_pUpdate = (SP2::Event::GInternalLawsUpdate*)in_pEvent.get();

   if(l_pUpdate->m_bGetData)
   {
      GDataInternalLaws* l_pData = g_ClientDAL.DataInternalLawsWindow;

      for(UINT32 i=0; i<3; i++)
      {
         GDataInternalLaws & l_Data = l_pData[i];
         
         l_Data.m_vbChanged.clear();
         l_Data.m_vfPerc.clear();
         l_Data.m_viId.clear();
         l_Data.m_viStatus.clear();
         l_Data.m_vpCombo.clear();
         l_Data.m_viGvtType.clear();

         if(g_ClientDDL.InternalLawsWindow() && g_ClientDDL.InternalLawsWindow()->Visible())
            g_ClientDDL.InternalLawsWindow()->Clear();
      }
      
      INT32 l_iIndex = 0;
      for(UINT32 i=0; i<l_pUpdate->m_viType.size(); i++)
      {
         GDataInternalLaws & l_Data = l_pData[l_pUpdate->m_viType[i] - 1];
         
         l_Data.m_viId.push_back( l_pUpdate->m_viId[i] );
         l_Data.m_viStatus.push_back( l_pUpdate->m_viStatus[i] );
         l_Data.m_vfPerc.push_back( l_pUpdate->m_vfPerc[i] );
         l_Data.m_vbChanged.push_back(false);
         l_Data.m_vpCombo.push_back(NULL);

         if(l_pUpdate->m_viGvtType.size() > 0 && l_pUpdate->m_viType[i] == 3)
         {
            l_Data.m_viGvtType.push_back(l_pUpdate->m_viGvtType[l_iIndex]);
            l_iIndex++;
         }

         if(i == 0)
         {
            l_Data.m_bEmigrationClosed  = l_pUpdate->m_bEmigrationClosed;
            l_Data.m_bImmigrationClosed = l_pUpdate->m_bImmigrationClosed;
            l_Data.m_fEmigration        = l_pUpdate->m_fEmigrationLevel;
				gassert(!_isnan(l_Data.m_fEmigration) && l_Data.m_fEmigration >= 0.f,"Emigration is NAN");
            l_Data.m_fImmigration       = l_pUpdate->m_fImmigrationLevel;
            l_Data.m_iGvtType           = l_pUpdate->m_iGvtType;

            for(UINT32 j=0; j<EInternalLaws::ItemCount; j++)
            {
               l_Data.m_vbLaws[(EInternalLaws::Enum)j] = l_pUpdate->m_vbLaws[j] ;
            }
         }
      }
   }

   if(g_ClientDDL.InternalLawsWindow() && g_ClientDDL.InternalLawsWindow()->Visible())
      g_ClientDDL.InternalLawsWindow()->Update(true);

   return true;
}

/*!
 * Handle a treaty refusal details
 **/
bool GPoliticEventHandler::HandleTreatyRefusalDetails(SDK::GGameEventSPtr in_pEvent)
{
   // retrieve the GEventTradeProposal pointer
   SP2::Event::GEventTreatyRefusalDetails* l_pUpdate = (SP2::Event::GEventTreatyRefusalDetails*)in_pEvent.get();   
	g_ClientDAL.AddTreatyRefusalMail(l_pUpdate->m_TreatyRefusalDetails);   
   return true;
}