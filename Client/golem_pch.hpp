/**************************************************************
*
* golem_pch.hpp
*
* Description
* ===========
*  Precompiled header for SP2 Client project.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef GOLEM_SP2_CLIENT_PCH
#define GOLEM_SP2_CLIENT_PCH

#include "../golem_pch.hpp"

#include "../includes/MapEngine/map_engine.h"
#include "../includes/SDK/golem_sdk.h"
#include "../includes/common_lib/sp2_constants.h"
#include "../includes/common_lib/sp2_constants_str_id.h"
#include "../includes/common_lib/sp2_common.h"

using namespace SP2;

using namespace SDK;

#include <deque>

#include "include/sp2_sound.h"
#include "../includes/common_lib/sp2_constants.h"
#include "../includes/common_lib/sp2_data_access_layer.h"

using namespace GUI;

#include "include/sp2_client.h"
#include "include/sp2_base_window.h"

// Place DZDebug here so it can pick up GString and related definitions
#include "../Common/Include/sp2_dzdebug.h"
#include "../Common/Include/sp2_hdm_event_country_info.h"
#include "../Common/Include/sp2_hdm_set_player_info.h"

#endif