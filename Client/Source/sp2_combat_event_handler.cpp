#include "golem_pch.hpp"

using namespace SP2::Event;


/*!
 * Constructor
 **/
SP2::GCombatEventHandler::GCombatEventHandler()
{
}

/*!
 * Destructor
 **/
SP2::GCombatEventHandler::~GCombatEventHandler()
{
}

/*!
 * Handle the move troops event.
 **/
bool SP2::GCombatEventHandler::HandleMovingUnitPath(SDK::GGameEventSPtr in_Event)
{
   return false;
}