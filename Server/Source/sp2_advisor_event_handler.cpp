/**************************************************************
*
* sp2_advisor_event_handler.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

using namespace SP2::Event;
using namespace SDK::Event;

/*!
 * Constructor
 **/
SP2::GAdvisorEventHandler::GAdvisorEventHandler()
{
}

/*!
 * Destructor
 **/
SP2::GAdvisorEventHandler::~GAdvisorEventHandler()
{
}

void SP2::GAdvisorEventHandler::HandleAdvisorAnswer(SDK::GGameEventSPtr in_Event)
{
	GSendPlan* l_pEvent = (GSendPlan*)in_Event.get();	
	switch(l_pEvent->m_TypeOfPlan)
	{
	case ETypeOfPlan::EHE:
		HandleEHEPlan(l_pEvent);
		break;
	case ETypeOfPlan::Specific:
		{
			switch(l_pEvent->m_iObjectiveID)
			{
			case EAdvisorObjectives::TooManyMilitaryUnits:
				HandleTooManyMilitaryUnits(l_pEvent);
				break;
			default:
				gassert(false,"GAdvisorEventHandler::HandleAdvisorAnswer: Undefined type of plan");				
			}
			break;
		}
	default:
		gassert(false,"GAdvisorEventHandler::HandleAdvisorAnswer: Undefined type of plan");
	}	
}

void SP2::GAdvisorEventHandler::HandleEHEPlan(GSendPlan* in_pPlan)
{
   if(!g_Joshua.ActivePlayer(in_pPlan->m_iSource) )
      return;

	UINT32 l_iPlayerCountry = g_Joshua.ActivePlayer( in_pPlan->m_iSource )->ModID();
	GAdvisor* l_pAdvisor = g_ServerDAL.CountryData(l_iPlayerCountry)->Advisor();
	if(!l_pAdvisor)
		return;
	
	UINT32 l_iObjective = in_pPlan->m_iObjectiveID;
	
	//Check if the player has accepted or refused the plan
	if(in_pPlan->m_bAccept)
		l_pAdvisor->AcceptPendingPlan(l_iObjective);
	else
		l_pAdvisor->RemovePlan(l_iObjective);
	
}

void SP2::GAdvisorEventHandler::HandleTooManyMilitaryUnits(GSendPlan* in_pPlan)
{
	if(!in_pPlan->m_bAccept)
		return;

   if(!g_Joshua.ActivePlayer(in_pPlan->m_iSource) )
      return;

	UINT32 l_iPlayerCountry = g_Joshua.ActivePlayer( in_pPlan->m_iSource )->ModID();

	EAdvisorActions::Enum l_Action = (EAdvisorActions::Enum)in_pPlan->m_vActionsID[0].second;
	if(l_Action == EAdvisorActions::ParkUnits)
	{
		const set<UINT32>& l_vUnitGroupsID = g_Joshua.UnitManager().CountryUnitGroups(l_iPlayerCountry);
		for(set<UINT32>::const_iterator l_It = l_vUnitGroupsID.begin();
				l_It != l_vUnitGroupsID.end();
				l_It++)
		{
			SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup((*l_It));
			if(l_pUnitGroup->Status() == EMilitaryStatus::Ready ||
				l_pUnitGroup->Status() == EMilitaryStatus::Fortified ||
				l_pUnitGroup->Status() == EMilitaryStatus::Moving)
			{
				g_ServerDCL.UnitMover().ChangeUnitState((SDK::Combat::GUnitGroup*)l_pUnitGroup,EMilitaryStatus::Parked);
			}
		}
	}

}