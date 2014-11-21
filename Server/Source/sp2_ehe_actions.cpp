/**************************************************************
*
* sp2_ehe_actions.cpp
*
* Description
* ===========
*	See sp2_ehe_actions.h
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/

#include "golem_pch.hpp"
#include "sp2_common.h"
#include "../include/sp2_ehe_actions.h"
#include "../include/sp2_server.h"

bool EHE_Action_Raise_Tax::Execute(INT32 in_source, INT32 in_target, INT32 in_intensity)
{
   REAL64 l_fIncomeTax = 0;

   GRSetSPtr l_pOutRecordSet(new GRecordSet());
   g_ServerDAL.DatabaseExecuteQuery(L"SELECT personal_income_tax FROM country WHERE id = " + GString(in_source),(GRSetSPtr)NULL,l_pOutRecordSet);

   if( (l_pOutRecordSet.get() ) && (l_pOutRecordSet.get()->Record(0)->Field(0)->Data() ) )
   {
      l_fIncomeTax = *((REAL64*)l_pOutRecordSet.get()->Record(0)->Field(0)->Data());
      if(l_fIncomeTax < 60.0f) 
         l_fIncomeTax += 1.0f;
      g_ServerDAL.DBUpdateDomesticPolicies((INT16)in_source,&l_fIncomeTax);
   }  
   else
      return false;

   //On close button called we must create an event to adjust the income tax if it has changed
   GGameEventSPtr l_GEAdjustTax = g_Joshua.CreateGameEvent(SP2::ADJUST_INCOME_TAX);
   l_GEAdjustTax->m_from = in_source;
   l_GEAdjustTax->m_target = 0;
   
   SP2::Event::GAdjustIncomeTax* l_pAdjustTax = (SP2::Event::GAdjustIncomeTax*)l_GEAdjustTax.get();
   l_pAdjustTax->IncomeTaxRate(l_fIncomeTax);
   g_Joshua.RaiseEvent(l_GEAdjustTax);
   g_Joshua.Log(L"Raising taxes for " + GString(in_source),MSGTYPE_MSG);

   //Get the country capital and the coordinates for the flag to be placed on earth
   SCitiesInfo l_City = g_ServerDAL.Capital(in_source);

   //Notify the client
   GGameEventSPtr l_GENotifyClient = g_Joshua.CreateGameEvent(SP2::NOTIFY_CLIENT);
   l_GENotifyClient->m_from   = 0;
   SP2::Event::GNotifyClient* l_pNotify = (SP2::Event::GNotifyClient*)l_GENotifyClient.get();
   l_pNotify->Set((REAL32)l_City.m_iY,(REAL32)l_City.m_iX,0,in_source,l_City.m_sName + ": raising taxes");
   for(INT32 i = 0 ; i < (INT32)g_SP2Server->HumanPlayers().size() ; i++)
   {
      l_GENotifyClient->m_target =g_SP2Server->HumanPlayers()[i];
      g_Joshua.RaiseEvent(l_GENotifyClient);
   }
   return true;
}

bool EHE_Action_Lower_Tax::Execute(INT32 in_source, INT32 in_target, INT32 in_intensity)
{
   REAL64 l_fIncomeTax = 0;

   GRSetSPtr l_pOutRecordSet(new GRecordSet());
   g_ServerDAL.DatabaseExecuteQuery(L"SELECT personal_income_tax FROM country WHERE id = " + GString(in_source),(GRSetSPtr)NULL,l_pOutRecordSet);

   if( (l_pOutRecordSet.get() ) && (l_pOutRecordSet.get()->Record(0)->Field(0)->Data() ) )
   {
      l_fIncomeTax = *((REAL64*)l_pOutRecordSet.get()->Record(0)->Field(0)->Data());
      if(l_fIncomeTax > 3.0f) 
         l_fIncomeTax -= 1.0f;
      
      g_ServerDAL.DBUpdateDomesticPolicies((INT16)in_source,&l_fIncomeTax);
   }
   else
      return false;

   //On close button called we must create an event to adjust the income tax if it has changed
   GGameEventSPtr l_GEAdjustTax = g_Joshua.CreateGameEvent(SP2::ADJUST_INCOME_TAX);
   l_GEAdjustTax->m_from = in_source;
   l_GEAdjustTax->m_target = 0;
   
   SP2::Event::GAdjustIncomeTax* l_pAdjustTax = (SP2::Event::GAdjustIncomeTax*)l_GEAdjustTax.get();
   l_pAdjustTax->IncomeTaxRate(l_fIncomeTax);
   g_Joshua.RaiseEvent(l_GEAdjustTax);
   g_Joshua.Log(L"Lowering taxes for " + GString(in_source),MSGTYPE_MSG);

   //Get the country capital and the coordinates for the flag to be placed on earth
   INT32 l_iCapitalID = g_ServerDAL.Capitals()[in_source-1].second;
   SP2::GCity l_City = g_ServerDAL.Cities()[l_iCapitalID-1];

   //Notify the client
   GGameEventSPtr l_GENotifyClient = g_Joshua.CreateGameEvent(SP2::NOTIFY_CLIENT);
   l_GENotifyClient->m_from   = 0;
   SP2::Event::GNotifyClient* l_pNotify = (SP2::Event::GNotifyClient*)l_GENotifyClient.get();
   l_pNotify->Set((REAL32)l_City.m_iY,(REAL32)l_City.m_iX,0,in_source,l_City.m_sName + ": lowering taxes");
   for(INT32 i = 0 ; i < (INT32)g_SP2Server->HumanPlayers().size() ; i++)
   {
      l_GENotifyClient->m_target =g_SP2Server->HumanPlayers()[i];
      g_Joshua.RaiseEvent(l_GENotifyClient);
   }
   return true;
}
