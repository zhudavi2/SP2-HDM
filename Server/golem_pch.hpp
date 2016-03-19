#ifndef GOLEM_SP2_SERVER_PCH
#define GOLEM_SP2_SERVER_PCH

#include "../includes/golem_pch.hpp"
using namespace SDK::EHE;

#include "../includes/common_lib/sp2_data_access_layer.h"
#include "../includes/common_lib/sp2_constants.h"
#include "../includes/SDK/golem_joshua.h"
#include "include/sp2_game_objective_evaluator.h"
#include "include/sp2_ai.h"
#include "include/sp2_advisor.h"
#include "include/sp2_advisor_event_handler.h"
#include "include/sp2_dzdebug.h"
#include "include/sp2_ehe_plan.h"
#include "include/sp2_combat_planner.h"
#include "include/sp2_unit_group_ex.h"
#include "include/sp2_unit_mover.h"
#include "include/sp2_unit_productor.h"
#include "include/sp2_data_access_layer.h"
#include "include/sp2_data_control_layer.h"
#include "include/sp2_world_behavior.h"
#include "include/sp2_general_event_handler.h"
#include "include/sp2_economic_event_handler.h"
#include "include/sp2_politic_event_handler.h"
#include "include/sp2_military_event_handler.h"
#include "include/sp2_unit_production_event_handler.h"
#include "include/sp2_treaty_event_handler.h"
#include "include/sp2_ai_general_event_handler.h"
#include "include/sp2_server.h"
#include "include/sp2_ehe_actions.h"
#include "include/sp2_ehe_objectives.h"
#include "include/sp2_combat_arena.h"
#include "include/sp2_ehe_constants.h"
#include "include/sp2_military_plan.h"
#include "include/sp2_country_data_holder.h"

#ifdef GOLEM_DEBUG
#undef gassert
#define gassert(expr, msg) GDZDEBUGASSERT(expr, msg)
#endif

#endif