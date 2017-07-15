/**************************************************************
*
* sp2_research_information_holder.cpp
*
* Description
* ===========
*  Contains all the data for research management
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

GResearchInformationHolder::GResearchInformationHolder()
{
   memset(m_fBudget     ,-1,sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
   memset(m_fMaxValues  ,-1,sizeof(REAL32)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
}

GResearchInformationHolder& GResearchInformationHolder::operator =(const GResearchInformationHolder& in_Original)
{
   m_fBudgetGround   = in_Original.m_fBudgetGround;
   m_fBudgetAir      = in_Original.m_fBudgetAir;
   m_fBudgetNaval    = in_Original.m_fBudgetNaval;
   m_fBudgetNuclear  = in_Original.m_fBudgetNuclear;
   memcpy(m_fBudget     , in_Original.m_fBudget   ,sizeof(REAL64)*EUnitDesignCharacteristics::ItemCount*EUnitCategory::ItemCount);
   memcpy(m_fMaxValues  , in_Original.m_fMaxValues,sizeof(REAL32)*EUnitDesignCharacteristics::ItemCount*EUnitCategory::ItemCount);
   return *this;
}

void GResearchInformationHolder::Update()
{

   SDK::GGameEventSPtr l_pEvent                  = CREATE_GAME_EVENT(SP2::Event::GUnitResearchUpdate);
   SP2::Event::GUnitResearchUpdate* l_pInfoEvent = (SP2::Event::GUnitResearchUpdate*)l_pEvent.get();
   l_pInfoEvent->m_iCountryId                    = g_SP2Client->DAL().ControlledCountryID();
   l_pInfoEvent->m_iTarget                       = SDK::Event::ESpecialTargets::Server;
   l_pInfoEvent->m_iSource                       = g_SP2Client->Id();
   g_Joshua.RaiseEvent(l_pEvent);
}