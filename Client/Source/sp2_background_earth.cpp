/**************************************************************
*
* sp2_background_earth.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_background_earth.h"

#include "../include/sp2_relations_window.h"
#include "../include/sp2_treaties_detailed_window.h"
#include "../include/sp2_strategic_target_country_window.h"
#include "../include/sp2_atlas_earth.h"
#include "../include/sp2_context_menu.h"
#include "../include/sp2_context_menu_item.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_overlay_city.h"
#include "../include/sp2_progress_meter.h"
#include "../include/sp2_sound.h"
#include "../include/sp2_sphere_control_window.h"
#include "../include/sp2_unit_info_small_window.h"
#include "../include/sp2_unit_list_window.h"
#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_group_split_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_unit_production_window.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_unit_ground_deployment_window.h"
#include "../include/sp2_war_list_window.h"
#include "../include/sp2_budget_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_battle_overview_window.h"

#ifdef GOLEM_DEBUG
#define __LOG_DIFFERENT_CONFIG_PARAMS__
#endif

#ifdef __LOG_DIFFERENT_CONFIG_PARAMS__
#define LOG_DIFF_CONFIG(Config, Value, Default) \
      { \
      if(Value != Default) \
      (g_Joshua.Log(GString(L"Parameter ") + \
      GString(Config) + \
      GString(L" is different from its default value : ") + \
      GString(Value) + \
      GString(L" != ") + \
      GString(Default) ) ); \
      }
#else // #ifdef __LOG_MISSING_CONFIG_PARAMS__
#define LOG_DIFF_CONFIG(Config, Value, Default) ;
#endif // #ifdef __LOG_MISSING_CONFIG_PARAMS__

CWSTR c_AtmoHaloTxtrName                           = L"/data/textures/earth/other/atmohalo.tga";
CWSTR c_CloudTxtrName                              = L"/data/textures/earth/cloud/cloud_1024.dds";
CWSTR c_ModelPath                                  = L"/data/models/earth/";
CWSTR c_MoonModelName                              = L"/data/models/earth/moon.3DS";
CWSTR c_MoonTxtrName                               = L"/data/textures/earth/other/MOON.JPG";
CWSTR c_NightTxtrName                              = L"/data/textures/earth/city_2048.dds";
CWSTR c_ParticleTxtrName                           = L"/data/textures/earth/other/Particle.dds";
CWSTR c_RegionDataFileName                         = L"/data/textures/region/RegionList.dat";
CWSTR c_RegionTxtrExt                              = L".dds";
CWSTR c_RegionTxtrName                             = L"/data/textures/region/Region";
CWSTR c_SignWindowResName                          = L"Notification_Sign";
CWSTR c_SignWindowResTextName                      = L"Notification_Sign_Text";
CWSTR c_StarModelName                              = L"/data/models/earth/stars.3DS";
CWSTR c_SunTxtrName                                = L"/data/textures/earth/other/Sun.png";
CWSTR c_WaterTxtrName                              = L"/data/textures/earth/color/water_1024.dds";
CWSTR c_sConfigAMDSExplodeFrameRate                = L"Background_Earth>>AMDS>>Explode_Frame_Rate";
CWSTR c_sConfigAMDSExplodeSize                     = L"Background_Earth>>AMDS>>Explode_Size";
CWSTR c_sConfigAMDSExplodeTxtrSequence             = L"Background_Earth>>AMDS>>Explode_Sequence";
CWSTR c_sConfigAMDSLongBlinkColor                  = L"Background_Earth>>AMDS>>Long_Blink_Color";
CWSTR c_sConfigAMDSLongRangeStart                  = L"Background_Earth>>AMDS>>Long_Range_Start";
CWSTR c_sConfigAMDSMissFrameRate                   = L"Background_Earth>>AMDS>>Miss_Frame_Rate";
CWSTR c_sConfigAMDSMissSize                        = L"Background_Earth>>AMDS>>Miss_Size";
CWSTR c_sConfigAMDSMissTxtrSequence                = L"Background_Earth>>AMDS>>Miss_Sequence";
CWSTR c_sConfigAMDSShortBlinkColor                 = L"Background_Earth>>AMDS>>Short_Blink_Color";
CWSTR c_sConfigAMDSShortRangeStart                 = L"Background_Earth>>AMDS>>Short_Range_Start";
CWSTR c_sConfigAtlasFadeTime                       = L"Background_Earth>>Atlas>>Fade_Time";
CWSTR c_sConfigCloudFadeStart                      = L"Background_Earth>>Clouds>>Fade_Start";
CWSTR c_sConfigCloudFadeStop                       = L"Background_Earth>>Clouds>>Fade_Stop";
CWSTR c_sConfigCombatSequence                      = L"Background_Earth>>Combat>>Sequence";
CWSTR c_sConfigCombatSequenceFPS                   = L"Background_Earth>>Combat>>Sequence_Frame_Rate";
CWSTR c_sConfigBombardmentSequence                 = L"Background_Earth>>Bombardment>>Sequence";
CWSTR c_sConfigBombardmentSequenceFPS              = L"Background_Earth>>Bombardment>>Sequence_Frame_Rate";
CWSTR c_sConfigMaxDistance                         = L"Background_Earth>>Maximum_Distance";
CWSTR c_sConfigMinDistance                         = L"Background_Earth>>Minimum_Distance";
CWSTR c_sConfigNightColor                          = L"Background_Earth>>Night>>Color";
CWSTR c_sConfigNukeBlinkColor                      = L"Background_Earth>>Nuke>>Blink_Color";
CWSTR c_sConfigNukeBlinkFreq                       = L"Background_Earth>>Nuke>>Blink_Freq";
CWSTR c_sConfigNukeBlinkOffset                     = L"Background_Earth>>Nuke>>Blink_Offset";
CWSTR c_sConfigNukeBlinkScale                      = L"Background_Earth>>Nuke>>Blink_Scale";
CWSTR c_sConfigNukeExplodeFrameRate                = L"Background_Earth>>Nuke>>Explode_Frame_Rate";
CWSTR c_sConfigNukeExplodeSize                     = L"Background_Earth>>Nuke>>Explode_Size";
CWSTR c_sConfigNukeExplodeDelay                    = L"Background_Earth>>Nuke>>Explode_Delay";
CWSTR c_sConfigNukeExplodeTxtrSequence             = L"Background_Earth>>Nuke>>Explode_Sequence";
CWSTR c_sConfigNukeFlashFrameRate                  = L"Background_Earth>>Nuke>>Flash_Frame_Rate";
CWSTR c_sConfigNukeFlashSize                       = L"Background_Earth>>Nuke>>Flash_Size";
CWSTR c_sConfigNukeFlashTxtrSequence               = L"Background_Earth>>Nuke>>Flash_Sequence";
CWSTR c_sConfigNukeIcon                            = L"Background_Earth>>Nuke>>Icon";
CWSTR c_sConfigNukeMaxPathHeight                   = L"Background_Earth>>Nuke>>Max_Path_Height";
CWSTR c_sConfigNukeMinPathDiv                      = L"Background_Earth>>Nuke>>Min_Path_Division";
CWSTR c_sConfigNukePath                            = L"Background_Earth>>Nuke>>Path";
CWSTR c_sConfigNukeTrailFadeTime                   = L"Background_Earth>>Nuke>>Trail_Fade_Time";
CWSTR c_sConfigPathDestSmallTxtr                   = L"Background_Earth>>Path>>Dest_Small";
CWSTR c_sConfigPathDestTxtr                        = L"Background_Earth>>Path>>Dest";
CWSTR c_sConfigPathElemOffset                      = L"Background_Earth>>Path>>Elem_Offset";
CWSTR c_sConfigPathElemTxtr                        = L"Background_Earth>>Path>>Elem";
CWSTR c_sConfigPathLength                          = L"Background_Earth>>Path>>Length";
CWSTR c_sConfigPathTxtr                            = L"Background_Earth>>Path>>Line";
CWSTR c_sConfigStrategicAmbientColor               = L"Background_Earth>>Strategic>>Ambient_Color";
CWSTR c_sConfigStrategicDiffuseColor               = L"Background_Earth>>Strategic>>Diffuse_Color";
CWSTR c_sConfigStrategicLineColor                  = L"Background_Earth>>Strategic>>Line_Color";
CWSTR c_sConfigStrategicLineOffset                 = L"Background_Earth>>Strategic>>Line_Offset";
CWSTR c_sConfigStrategicNbLinesX                   = L"Background_Earth>>Strategic>>Nb_Lines_X";
CWSTR c_sConfigStrategicNbLinesY                   = L"Background_Earth>>Strategic>>Nb_Lines_Y";
CWSTR c_sConfigStrategicNbSegments                 = L"Background_Earth>>Strategic>>Nb_Segments";
CWSTR c_sConfigUnitProgressBGColor                 = L"Background_Earth>>Progress>>BG";
CWSTR c_sConfigUnitProgressDistance                = L"Background_Earth>>Progress>>Distance";
CWSTR c_sConfigUnitProgressFGColor                 = L"Background_Earth>>Progress>>FG";
CWSTR c_sConfigUnitProgressHeight                  = L"Background_Earth>>Progress>>Height";
CWSTR c_sDeadZoneIconTxtrName                      = L"/data/textures/info/DeadZone.png";
CWSTR c_sDefaultAMDSExplodeTxtrSequence            = L"/data/textures/info/amds_explode/Missile_explosion_32_0000.ifl";
CWSTR c_sDefaultAMDSMissTxtrSequence               = L"/data/textures/info/amds_miss/Missile_explosion_32_0000.ifl";
CWSTR c_sDefaultCombatSequence                     = L"/data/textures/info/combat/Battle32_0000.ifl";
CWSTR c_sDefaultNukeExplodeTxtrSequence            = L"/data/textures/info/nuke/Nuke320000.ifl";
CWSTR c_sDefaultBombardmentSequence                = L"/data/textures/info/combat/Naval_Hit_16_0000.ifl";
CWSTR c_sDefaultNukeFlashTxtrSequence              = L"/data/textures/info/nuke/Nuke320000.ifl";
CWSTR c_sDefaultNukeIcon                           = L"/data/textures/info/Nuke.png";
CWSTR c_sDefaultNukePath                           = L"/data/textures/info/NukePath.dds";
CWSTR c_sDefaultPathDestSmallTxtr                  = L"/data/textures/info/path_dest_small.png";
CWSTR c_sDefaultPathDestTxtr                       = L"/data/textures/info/path_dest.png";
CWSTR c_sDefaultPathElemTxtr                       = L"/data/textures/info/path_elem.dds";
CWSTR c_sDefaultPathTxtr                           = L"/data/textures/info/pathsmall.dds";
CWSTR c_sNukeCustomParamBlnk                       = L"NukeBlink";
CWSTR c_sNukeCustomParamVis                        = L"NukeVis";
CWSTR c_sNukeCustomParamX                          = L"NukePosX";
CWSTR c_sNukeCustomParamY                          = L"NukePosY";
CWSTR c_sNukeCustomParamZ                          = L"NukePosZ";
const UINT32  c_DefaultSphereWindowControlKey      = VK_TAB;                                       
const UINT32  c_DefaultUnitIconsToggleControlKey   = VK_SPACE;
const EUnitFriendship::Enum c_DefaultPathFriendship= EUnitFriendship::Player;
const GColorRGBInt c_DefaultAMDSLongBlinkColor     = GColorRGBInt(0, 0, 128, 255);
const GColorRGBInt c_DefaultAMDSShortBlinkColor    = GColorRGBInt(0, 128, 0, 255);
const GColorRGBInt c_DefaultNightColor             = GColorRGBInt(107, 110, 153, 255);
const GColorRGBInt c_DefaultNukeBlinkColor         = GColorRGBInt(255, 255, 255, 255);
const GColorRGBInt c_DefaultPathPlayerColor        = GColorRGBInt(0, 255, 0, 255);
const GColorRGBInt c_DefaultPathFriendlyColor      = GColorRGBInt(0, 0, 255, 255);
const GColorRGBInt c_DefaultPathHostileColor       = GColorRGBInt(255, 0, 0, 255);
const GColorRGBInt c_DefaultPathNeutralColor       = GColorRGBInt(255, 255, 0, 255);
const GColorRGBInt c_DefaultStrategicAmbientColor  = GColorRGBInt(128, 0, 0, 255);
const GColorRGBInt c_DefaultStrategicDiffuseColor  = GColorRGBInt(255, 0, 0, 255);
const GColorRGBInt c_DefaultStrategicLineColor     = GColorRGBInt(192, 0, 0, 255);
const GColorRGBInt c_DefaultUnitProgressBGColor    = GColorRGBInt(255, 0, 0, 255);
const GColorRGBInt c_DefaultUnitProgressFGColor    = GColorRGBInt(0, 0, 255, 255);
const INT32 c_iSphereCombatWindowOffset            = 16;
const INT32 c_iDeadZoneIconSize                    = 32;
const INT32 c_iDeadZoneIconSizeHalf                = c_iDeadZoneIconSize / 2;
const INT32 c_iDefaultStrategicNbLinesX            = 48;
const INT32 c_iDefaultStrategicNbLinesY            = 23;
const INT32 c_iDefaultStrategicNbSegments          = 64;
const INT32 c_iDefaultUnitProgressDistance         = 10;
const INT32 c_iDefaultUnitProgressHeight           = 4;
const REAL32 c_AtmoHaloScale                       = 1.2f;
const REAL32 c_CloudSpeed                          = -1.f / 200.f;
const REAL32 c_DaySpeed                            = 1.f / 100.f;
const REAL32 c_EarthAngle                          = 0.40927970959267028578860548521058f;
const REAL32 c_EarthAngleCos                       = 0.91740769935748826377361893702175f;
const REAL32 c_EarthAngleSin                       = 0.39794863130761038954479576746719f;
const REAL32 c_FlareMaxDist                        = 0.5f;
const REAL32 c_MoonDist                            = 20.f;
const REAL32 c_MoonRevYear                         = 351.f;
const REAL32 c_MoonRevYearInv                      = 1.f / c_MoonRevYear;
const REAL32 c_SunDist                             = 20.f;
const REAL32 c_YearSpeed                           = 1.f / 365.25f;
const REAL32 c_fDefaultAMDSExplodeFrameRate        = 15.f;
const REAL32 c_fDefaultAMDSExplodeSize             = 0.04f;
const REAL32 c_fDefaultAMDSLongRangeStart          = 0.5f;
const REAL32 c_fDefaultAMDSMissFrameRate           = 15.f;
const REAL32 c_fDefaultAMDSMissSize                = 0.035f;
const REAL32 c_fDefaultAMDSShortRangeStart         = 0.75f;
const REAL32 c_fDefaultAtlasFadeTime               = 0.5f;
const REAL32 c_fDefaultCloudFadeStart              = 1.f;
const REAL32 c_fDefaultCloudFadeStop               = 0.075f;
const REAL32 c_fDefaultCombatSequenceFPS           = 15.f;
const REAL32 c_fDefaultBombardmentSequenceFPS      = 15.f;
const REAL32 c_fDefaultMaxDistance                 = 2.f;
const REAL32 c_fDefaultMinDistance                 = 0.05f;
const REAL32 c_fDefaultNukeBlinkFreq               = 150.f;
const REAL32 c_fDefaultNukeBlinkOffset             = 0.4f;
const REAL32 c_fDefaultNukeBlinkScale              = 0.7f;
const REAL32 c_fDefaultNukeExplodeFrameRate        = 20.f;
const REAL32 c_fDefaultNukeExplodeSize             = 0.05f;
const REAL32 c_fDefaultNukeExplodeDelay            = 0.05f;
const REAL32 c_fDefaultNukeFlashFrameRate          = 20.f;
const REAL32 c_fDefaultNukeFlashSize               = 0.05f;
const REAL32 c_fDefaultNukeMaxPathHeight           = 0.2f;
const REAL32 c_fDefaultNukeMinPathDiv              = 0.05f;
const REAL32 c_fDefaultNukeTrailFadeTime           = 0.75f;
const REAL32 c_fDefaultPathElemOffset              = 16.f;
const REAL32 c_fDefaultPathLength                  = 2.f;
const REAL32 c_fDefaultStrategicLineOffset         = 1.0025f;
const REAL32 c_fParticleMaxSize                    = 0.05f;
const REAL32 c_fSignFlipZoneX                      = 100.0f;

const REAL32 c_fSignLength                         = 75.0f;
const REAL32 c_fSignFlipSlopeX                     = c_fSignLength / c_fSignFlipZoneX;
const UINT32 c_FlareTxtrID[c_NbFlares]             = { 0, 1, 3, 2, 1, 3, 2, 1};
const UINT32 c_iContextMenuMilitaryCount           = 7;
const UINT32 c_iContextMenuDeployementCount        = 3;
const UINT32 c_iContextMenuMilitarySubMenuCount[c_iContextMenuMilitaryCount] = { 0, 0, 3, 0 };
const UINT32 c_iNotificationSignDisplayTime        = 5;

CWSTR c_sCursorFilenames[] =
{  L"/data/cursors/MoveUnit.cur"
,  L"/data/cursors/MoveUnitTo.cur"
,  L"/data/cursors/MoveNo.cur"
,  L"/data/cursors/MergeUnit.cur"
,  L"/data/cursors/MoveUnitAttack.cur"
};

CWSTR c_sConfigPathColor[EUnitFriendship::Count] =
{  L"Background_Earth>>Path>>Hostile"
,  L"Background_Earth>>Path>>Neutral"
,  L"Background_Earth>>Path>>Friendly"
,  L"Background_Earth>>Path>>Player"
};

CWSTR c_sConfigUnitTxtr[EUnitFriendship::Count] =
{  L"Background_Earth>>Icons>>Enemy_Units"
,  L"Background_Earth>>Icons>>Neutral_Units"
,  L"Background_Earth>>Icons>>Ally_Units"
,  L"Background_Earth>>Icons>>Player_Units"
};

CWSTR c_sConfigNavalTxtr[EUnitFriendship::Count] =
{  L"Background_Earth>>Icons>>Enemy_Ships"
,  L"Background_Earth>>Icons>>Neutral_Ships"
,  L"Background_Earth>>Icons>>Ally_Ships"
,  L"Background_Earth>>Icons>>Player_Ships"
};

CWSTR c_sConfigNavalBombardingTxtr[EUnitFriendship::Count] =
{  L"Background_Earth>>Icons>>Enemy_Bombarding_Ships"
,  L"Background_Earth>>Icons>>Neutral_Bombarding_Ships"
,  L"Background_Earth>>Icons>>Ally_Bombarding_Ships"
,  L"Background_Earth>>Icons>>Player_Bombarding_Ships"
};

CWSTR c_sConfigUnitSelTxtr[EUnitFriendship::Count] =
{  L"Background_Earth>>Icons>>Enemy_Select"
,  L"Background_Earth>>Icons>>Neutral_Select"
,  L"Background_Earth>>Icons>>Ally_Select"
,  L"Background_Earth>>Icons>>Player_Select"
};

CWSTR c_ConfigUnitFlagSelTxtr[EUnitFriendship::Count] =
{  L"Background_Earth>>Icons>>Flag_Enemy_Select"
,  L"Background_Earth>>Icons>>Flag_Neutral_Select"
,  L"Background_Earth>>Icons>>Flag_Ally_Select"
,  L"Background_Earth>>Icons>>Flag_Player_Select"
};

CWSTR c_sDefaultUnitFlagSelTxtr[EUnitFriendship::Count] =
{  L"/data/textures/info/flag_select_enemy.tga"
,  L"/data/textures/info/flag_select_neutral.tga"
,  L"/data/textures/info/flag_select_ally.tga"
,  L"/data/textures/info/flag_select_player.tga"
};

CWSTR c_sConfigUnitStatusTxtr[EMilitaryStatus::CountVisible] =
{  L"Background_Earth>>Icons>>Parked"
,  L"Background_Earth>>Icons>>Ready"
,  L"Background_Earth>>Icons>>Fortified"
,  L"Background_Earth>>Icons>>Moving"
,  L"Background_Earth>>Icons>>InDeployement"
};


const GColorRGBInt c_DefaultPathColor[EUnitFriendship::Count] =
{  c_DefaultPathHostileColor
,  c_DefaultPathNeutralColor
,  c_DefaultPathFriendlyColor
,  c_DefaultPathPlayerColor
};

CWSTR c_sDefaultUnitTxtr[EUnitFriendship::Count] =
{  L"/data/textures/info/icon_enemy_units.tga"
,  L"/data/textures/info/icon_neutral_units.tga"
,  L"/data/textures/info/icon_ally_units.tga"
,  L"/data/textures/info/icon_player_units.tga"
};

CWSTR c_sDefaultNavalUnitTxtr[EUnitFriendship::Count] = 
{
   L"/data/textures/info/icon_enemy_ships.tga"
,  L"/data/textures/info/icon_neutral_ships.tga"
,  L"/data/textures/info/icon_ally_ships.tga"
,  L"/data/textures/info/icon_player_ships.tga"
};

CWSTR c_sDefaultUnitSelTxtr[EUnitFriendship::Count] =
{  L"/data/textures/info/icon_enemy_units_select.tga"
,  L"/data/textures/info/icon_neutral_units_select.tga"
,  L"/data/textures/info/icon_ally_units_select.tga"
,  L"/data/textures/info/icon_player_units_select.tga"
};

CWSTR c_sDefaultUnitStatusTxtr[EMilitaryStatus::CountVisible] =
{  L"/data/textures/info/icon_parked.tga"
,  L"/data/textures/info/icon_ready.tga"
,  L"/data/textures/info/icon_fortified.tga"
,  L"/data/textures/info/icon_move.png"
,  L"/data/textures/info/icon_deployement.tga"
};

const UINT32 c_iContextMenuMilitaryTextId[] =
{  101098 //Move
,  101099 //Move and attack
,  101100 //Change status
,  101101 //Deselect
,  101102 //Merge with
,  102094 //Split
,  101103 //View units list
};

INT32 c_iContextMenuOccupyTerritory = 101662;
INT32 c_iContextMenuRelations       = 101663;

INT32 c_iContextMenuBombardRegion   = 102095;




const UINT32 c_iContextMenuDeployementTextId[] =
{  101103 //L"View units list"
,  101101 //L"Deselect"
,  101104 //L"Cancel Deployement"
};


const UINT32 c_iContextMenuMilitaryChangeStatusId[3] =
{  101105 //L"Park"
,  101106 //L"Ready"
,  101107 //L"Fortify"
};

const UINT32 * const c_iContextMenuMilitarySubText[c_iContextMenuMilitaryCount] =
{  NULL
,  NULL
,  c_iContextMenuMilitaryChangeStatusId
,  NULL
};



const UINT32 c_iThematicMapsSubmenuEntry                    = 101135;
const UINT32 c_iThematicMapResourceProductionSubmenuEntry   = 101136;
const UINT32 c_iThematicMapQuickTreatySubmenuEntry				= 101664;


const UINT32 c_ContextMenuEntriesId[] =
{  
   101108   //L"Geographic"
,  101109   //L"Economic Strength"
,  101110   //L"Empire"
,	101661	//L"Political Control"
,  101111   //L"Diplomatic Relations"
,  101112   //L"Resource Balance"
,  101113   //L"GDP"
,  101114   //L"GDP Growth"
,  101115   //L"Human Development"
,  101116   //L"Military Strength"
,  101117   //L"Nuclear Strength"
,  101118   //L"Political Strength"
,  101119   //L"Population Growth"
,  101120   //L"Government Stability"
,  101121   //L"Government Approval"
,  101122   //L"Human-controlled Countries"
,  101123   //L"Population Density (by region)"
,  101124   //L"Population (by region)"
,  101125   //L"All Resources"
,  100359   //L"Cereals"
,  100360   //L"Vegetable & Fruits"
,  100361   //L"Meat"
,  100362   //L"Dairy"
,  100363   //L"Tobacco"
,  100364   //L"Drugs"
,  100365   //L"Electricity"
,  100366   //L"Fossile Fuels"
,  100367   //L"Wood & Paper"
,  100368   //L"Minerals"
,  100369   //L"Iron & Steel"
,  100370   //L"Precious Stones"
,  100371   //L"Fabrics"
,  100372   //L"Plastics"
,  100373   //L"Chemicals"
,  100374   //L"Pharmaceuticals"
,  100375   //L"Appliances"
,  100376   //L"Vehicules"
,  100377   //L"Machinery"
,  100378   //L"Commodities"
,  100379   //L"Luxury Commodities"
,  100380   //L"Construction"
,  100381   //L"Engineering"
,  100382   //L"Health Care"
,  100383   //L"Retail"
,  100384   //L"Legal Services"
,  100385   //L"Marketing & Advertising"
,  101126   //L"Move"
,  101127   //L"Attack"
,  101128   //L"Merge"
};

namespace EContextMenuEntries 
{ 
   enum Enum
   {
      Geographic                          = ECharacteristicToHighlight::Geographic,
      ThematicMapsBegin							= Geographic,
      CountryEconomicalStrength           = ECharacteristicToHighlight::CountryEconomicalStrength,
      CountryEmpire                       = ECharacteristicToHighlight::CountryEmpire,
		PoliticalControl                    = ECharacteristicToHighlight::PoliticalControl,
      CountryRelations                    = ECharacteristicToHighlight::CountryRelations,
      CountryExceedingResources           = ECharacteristicToHighlight::CountryExceedingResources,
      CountryGrossDomesticProduct         = ECharacteristicToHighlight::CountryGrossDomesticProduct,
      CountryGrossDomesticProductGrowth   = ECharacteristicToHighlight::CountryGrossDomesticProductGrowth,
      CountryHumanDevelopment             = ECharacteristicToHighlight::CountryHumanDevelopment,
      CountryMilitaryStrength             = ECharacteristicToHighlight::CountryMilitaryStrength,
      CountryNuclearStrength              = ECharacteristicToHighlight::CountryNuclearStrength,
      CountryPoliticalStrength            = ECharacteristicToHighlight::CountryPoliticalStrength,
      CountryPopulationGrowth             = ECharacteristicToHighlight::CountryPopulationGrowth,
      CountryPopulationStabilty           = ECharacteristicToHighlight::CountryPopulationStabilty,
      CountryPopulationSupport            = ECharacteristicToHighlight::CountryPopulationSupport,
      HumanControlledCountries            = ECharacteristicToHighlight::HumanControlledCountries,
      RegionPopulationDensity             = ECharacteristicToHighlight::RegionPopulationDensity,
      RegionPopulationTotal               = ECharacteristicToHighlight::RegionPopulationTotal,
      RegionResourcesProduction           = ECharacteristicToHighlight::RegionResourcesProduction,
      Cereals,                //all the following items must not be reordered.
      Vegetable_Fruits,
      Meat,
      Dairy,
      Tobacco,
      Drugs,
      Electricity,
      Fossile_Fuels,
      Wood_Paper,
      Minerals,
      Iron_Steel,
      Precious_Stones,
      Fabrics,
      Plastics,
      Chemicals,
      Pharmaceuticals,
      Appliances,
      Vehicules,
      Machinery,
      Commodities,
      Luxury_Commodities,
      Construction,
      Engineering,
      Health_Care,
      Retail,
      Legal_Services,
      Marketing_Advertising,

      ThematicMapsEnd = Marketing_Advertising,
            
      MoveSelectedUnit,
      AttackWithSelected,
      MergeSelectedWith  
   };
};

CWSTR c_FlareTxtrName[4] =
{  { L"/data/textures/earth/flare/SunHalo.png" }
,  { L"/data/textures/earth/flare/Flare0.png"  }
,  { L"/data/textures/earth/flare/Flare1.png"  }
,  { L"/data/textures/earth/flare/Flare2.png"  }
};

const REAL32 c_FlareDistScale[c_NbFlares][2] =
{  {1.f,   3.5f}
,  {1.3f,  0.3f}
,  {0.75f, 0.1f}
,  {-0.2f, 0.2f}
,  {-0.4f, 0.4f}
,  {-0.6f, 0.2f}
,  {-0.8f, 0.3f}
,  {-1.f,  0.1f}
};

const GString GBackgroundEarth::TypeName(L"BACKGROUND_EARTH");

/*!
* Background Earth constructor utility function
**/
GUI::GBaseObject* GBackgroundEarth::New()
{
   return new GBackgroundEarth();
}

/*!
* GBackgroundEarth constructor
**/
GBackgroundEarth::GBackgroundEarth() : GUIContainer()
{
   m_pConfigData           = NULL;

   m_eView                 = EBackgroundView::Realistic;

   // Set gfx object to NULL
   m_pScene                = NULL;

   m_pCameraModel          = NULL;
   m_pCameraTransform      = NULL;
   m_pCamera               = NULL;
   m_pCameraAction         = NULL;

   m_pSunModel             = NULL;
   m_pSunTransform         = NULL;
   m_pSunLight             = NULL;

   m_pRealisticMasterModel = NULL;
   m_pRealisticOverlayModel= NULL;

   m_pFlareMasterModel     = NULL;

   for(UINT32 f = 0;f <= c_NbFlares;f ++)
   {
      m_pFlareModel[f]     = NULL;
      m_pFlareTransform[f] = NULL;
   }
   
   m_pStarsModel           = NULL;
   m_pStarsMesh            = NULL;
   m_pStarsMaterial        = NULL;
   m_pStarsTexture         = NULL;

   m_pMoonModel            = NULL;
   m_pMoonTransform        = NULL;
   m_pMoonMaterial         = NULL;

   m_pEarthModelMaster     = NULL;
   m_pEarthTransformTemp   = NULL;
   m_pEarthMaterial        = NULL;
   m_pEarthMaterialNoSun   = NULL;

   Memory::Clear(m_pEarthModel,     NB_EARTH_DIVS);
   Memory::Clear(m_pEarthMesh,      NB_EARTH_DIVS);
   Memory::Clear(m_pEarthOverride,  NB_EARTH_DIVS);
   Memory::Clear(m_pEarthTexture,   NB_EARTH_DIVS);
   Memory::Clear(m_pEarthTextureLow,NB_EARTH_DIVS);

   Memory::Clear(m_pEarthCorners,   NB_EARTH_DIVS);

   m_pNightTexture         = NULL;

   m_pWaterModel           = NULL;
   m_pWaterMesh            = NULL;
   m_pWaterMaterial        = NULL;
   m_pWaterTexture         = NULL;

   m_pAtmoHaloModel        = NULL;
   m_pAtmoHaloTransform    = NULL;

   m_pAtlasEarth           = NULL;
   m_pCityOverlay          = NULL;

   m_pDisplayParticlesModelMaster   = NULL;
   m_pDisplayParticlesMaterial      = NULL;
   m_pDisplayParticlesMesh          = NULL;

   m_pCloudModel       = NULL;
   m_pCloudMesh        = NULL;
   m_pCloudTransform   = NULL;
   m_pCloudMaterial    = NULL;
   m_pCloudTexture     = NULL;

   m_pStrategicMasterModel          = NULL;

   m_pStrategicEarthModelMaster     = NULL;
   m_pStrategicEarthMaterial        = NULL;

   Memory::Clear(m_pStrategicEarthModels, NB_EARTH_DIVS);
   Memory::Clear(m_pStrategicEarthOverrides, NB_EARTH_DIVS);
   Memory::Clear(m_pStrategicEarthBumpOverrides, NB_EARTH_DIVS);
   Memory::Clear(m_pStrategicEarthTextures, NB_EARTH_DIVS);

   m_pStrategicEarthLinesModel      = NULL;
   m_pStrategicEarthLines           = NULL;
   m_pStrategicEarthLinesMaterial   = NULL;

   m_pTransitionViewTrack           = NULL;

   m_pFillModel         = NULL;
   m_pFillModelStrategic= NULL;
   m_pFillMesh          = NULL;
   m_pFillVS            = NULL;

   m_pOutlineModel      = NULL;
   m_pOutlineMesh       = NULL;

   m_pDragBoxModel      = NULL;
   m_pDragBoxTransform  = NULL;
   m_pDragBoxTexture    = NULL;

   m_pSignMesh          = NULL;
   m_pSignMaterial      = NULL;

   m_pOverlay           = NULL;

   m_pUnitPathDisplayModel = NULL;
   m_pNukeIconMasterModel  = NULL;
   m_pNukeIconMesh         = NULL;
   m_fNukeExplodeDelay     = 0;

   // Reset camera, earth & sun positions

   m_fLastTime          = 0;
   m_fDayFactor         = 0;
   m_fYearFactor        = 0;

   m_fCloudFadeStart    = 0;
   m_fCloudFadeStop     = 0;
   m_fCloudFadeDeltaInv = 1.f;

   m_fCloudFactor       = 0;
   m_iNbClouds          = 0;
   m_iCurrentCloud      = 0;

   m_bSelectBox         = false;

   m_iLastRegionSelected= 0;

   Memory::Clear(&m_OutlineColor);

   m_ClickLongLat.Set(0, 0);

   m_fMinCamDist        = 0;
   m_fMaxCamDist        = 0;

   m_fDistCalcA         = 0;
   m_fDistCalcB         = 0;

   m_pfCamDist          = NULL;
   m_pfCamAngleX        = NULL;
   m_pfCamAngleY        = NULL;

   m_fDragSpeed         = 0;

   Memory::Clear(&m_vfEarthClick);
   Memory::Clear(&m_InitialState);


   m_bSelectBox         = false;

   m_pMiniMap           = NULL;

   m_bEarthDragging     = false;   
   m_DragCoord.Set(-1,-1);   
   m_UnitToDrag = 0;
   m_CancelAllMouseEventsUntilAllButtonsUp = false;
   m_UnitToKeepOnDeferDeselect = 0;
   m_UnitToDeferDeselect= 0;

   Memory::Clear(&m_ViewMat);
   Memory::Clear(&m_ProjMat);

   Memory::Clear(&m_OriginPos);
   Memory::Clear(&m_CameraPos);

   m_bShowCombat     = false;

   m_fMinPathDivLength  = 0;
   m_fUnitPathElemOffset= 0;

   m_iCountryToOccupy   = 0;
	m_iCountryToCheckRelations = 0;
   m_iCountryToCheckMilitary  = 0;
   m_iRegionToBombard         = 0;

   m_pUnitPathElemDisplayModel = NULL;
   Memory::Clear(m_pUnitPathElemSubModels, EUnitFriendship::Count);
   m_pUnitPathElemMesh  = NULL;
   Memory::Clear(m_pUnitPathColorOverrides, EUnitFriendship::Count);
   Memory::Clear(m_UnitPathColors, EUnitFriendship::Count);

   Memory::Clear(&m_UnitPathElemSize);
   Memory::Clear(&m_UnitPathElemOffset);

   m_pUnitTargetDisplayModel = NULL;
   Memory::Clear(m_pUnitTargetMesh, 2);
   Memory::Clear(m_pUnitTargetTSamplers, 2);
   Memory::Clear(m_UnitTargetSize, 2);
   Memory::Clear(m_UnitTargetOffset, 2);

   m_pUnitDisplayModel = NULL;
   Memory::Clear(m_pUnitMeshes, EUnitFriendship::Count);
   Memory::Clear(m_pUnitTSamplers, EUnitFriendship::Count);
   Memory::Clear(m_pUnitShipsTSamplers,EUnitFriendship::Count);
   Memory::Clear(m_pUnitShipsBombardingTSamplers,EUnitFriendship::Count);


   Memory::Clear(&m_UnitSize, EUnitFriendship::Count);
   Memory::Clear(&m_UnitSizeHalf, EUnitFriendship::Count);

   Memory::Clear(m_pUnitSelectionMeshes, EUnitFriendship::Count);
   Memory::Clear(m_pUnitSelectionTSamplers, EUnitFriendship::Count);

   Memory::Clear(&m_UnitSelectionSize, EUnitFriendship::Count);
   Memory::Clear(&m_UnitSelectionSizeHalf, EUnitFriendship::Count);

   m_pUnitStatusDisplayModel = NULL;
   m_pUnitStatusMesh = NULL;
   m_UnitToDrag = 0;

   Memory::Clear(m_pUnitStatusTSamplers, EMilitaryStatus::CountVisible);
   Memory::Clear(&m_UnitStatusSize);

   m_pUnitProgressMasterModel = NULL;
   m_pUnitProgressBGMesh = NULL;
   m_pUnitProgressPercentMesh = NULL;

   m_iUnitProgressHeight = 0;
   m_iUnitProgressDistance = 0;

   m_bUnitPicked = false;

   m_pCombatDisplayModel = NULL;
   m_pCombatMesh = NULL;
   m_pCombatSequence = NULL;
   m_fCombatFPS = 1.f;

   m_pBombardmentDisplayModel = NULL;
   m_pBombardmentMesh = NULL;
   m_pBombardmentSequence = NULL;
   m_fBombardmentFPS = 1.f;

   Memory::Clear(&m_CombatSize);
   Memory::Clear(&m_CombatOffset);
   Memory::Clear(&m_BombardmentSize);
   Memory::Clear(&m_BombardmentOffset);

   m_pNukeFlashMasterModel = NULL;
   m_pNukeFlashMesh = NULL;
   m_pNukeFlashSequence = NULL;
   m_fNukeFlashFPS = 1.f;

   m_pProgressMeter = NULL;

   m_ReturnClickedPosition = NULL;
   m_pReturnWindow         = NULL;

   g_Joshua.FileManager()->File(c_sCursorFilenames[0], m_PointerMoveUnit);
   g_Joshua.FileManager()->File(c_sCursorFilenames[1], m_PointerMoveUnitTo);
   g_Joshua.FileManager()->File(c_sCursorFilenames[2], m_PointerMoveNo);
   g_Joshua.FileManager()->File(c_sCursorFilenames[3], m_PointerMergeUnit);
   g_Joshua.FileManager()->File(c_sCursorFilenames[4], m_PointerMoveUnitAttack);

   m_pUnitFlagMesh = NULL;      

   Memory::Clear(m_pUnitFlagSelectionMeshes,EUnitFriendship::Count);
   Memory::Clear(m_pUnitFlagSelectionTSamplers,EUnitFriendship::Count);
   m_bUnitDragging = false;

   m_bDisplayUnitsAsFlags = false;

   m_pNotificationParent = NULL;
   m_pNotificationContainer = NULL;

   m_fMaxLatitude = ((75.f  * (REAL32)Math::PI) / 180.f);
   m_fMinLatitude = ((-75.f * (REAL32)Math::PI) / 180.f);

   m_fOriginDistInv =    0;

   m_bDayNightCycle = true;
   m_eTxtrQuality = EQualityLevel::High;

   m_bLocked = false;
}

/*!
* Called when the Background Earth object is created
**/
bool GBackgroundEarth::OnCreate()
{
   __super::OnCreate();

   m_eCurrentEarthQuality = EQualityLevel::Medium;

   m_pProgressMeter = g_ClientDDL.ProgressMeter();
   if(m_pProgressMeter)
      m_pProgressMeter->PercentageUpdate(2);

   g_Joshua.Renderer()->Get_Current_Mode(&m_Mode);


   GFile l_FileHandle;

   m_pOwner = m_pParent;

   // Get background scene gfx objects properties
   m_fDragSpeed = Property(L"DragSpeed").ToREAL32();

   // Get properties from client config file
   const Hector::GTreeNode<GXMLNode>* l_pConfigTreeNode = g_ClientDAL.XMLNode(c_sBkgndNode);

   // Allocate hash_map object
   m_pConfigData = new hash_map<GString, GString>;

   ReadConfig(l_pConfigTreeNode, GString() );

   // Update camera distance limits
   m_fMinCamDist = ConfigValue(c_sConfigMinDistance, c_fDefaultMinDistance);
   m_fMaxCamDist = ConfigValue(c_sConfigMaxDistance, c_fDefaultMaxDistance);

   // Compute calculation constants, knowing that final formula is :
   //    CameraDist = 2 ^ (A * zoom + B)
   // When zoom = 0, CameraDist equals l_MaxDist,
   // when zoom = 1, CameraDist equals l_MinDist
   m_fDistCalcB = logf(m_fMaxCamDist) / logf(2);
   m_fDistCalcA = logf(m_fMinCamDist) / logf(2) - m_fDistCalcB;


   if(m_pProgressMeter)
   {
      m_pProgressMeter->PercentageUpdate(15);
      m_pProgressMeter->Split(75);
   }

   // Create background scene gfx objects
   IF_RETURN(!CreateBackgroundScene(), false);

   m_DefaultMouseCursor = g_Joshua.GUIManager()->MousePointer();


   if(m_pProgressMeter)
   {
      m_pProgressMeter->EndSplit();
      m_pProgressMeter->Update();
   }

   {
      // Read unit path display constants
      m_fMinPathDivLength = ConfigValue(c_sConfigPathLength, c_fDefaultPathLength);
      m_fUnitPathElemOffset = ConfigValue(c_sConfigPathElemOffset, c_fDefaultPathElemOffset);

      for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
      {
         m_UnitPathColors[i] = GColorRGBReal(ConfigValue(c_sConfigPathColor[i], 
            c_DefaultPathColor[i] ) );
      }

      // Setup unit path display
      m_pUnitPathDisplayModel = g_Joshua.Renderer()->Create_Model();
      GFX::GMaterial* l_UnitPathMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      CGFX_Texture_Itf* l_pUnitPathTxtr =    g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigPathTxtr, c_sDefaultPathTxtr) );
      ( (GFX::GCascadePixelShader*) l_UnitPathMat->PixelShader(0) )->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
      ( (GFX::GCascadePixelShader*) l_UnitPathMat->PixelShader(0) )->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
      l_UnitPathMat->PixelShader(0)->TextureSampler(0)->Texture(l_pUnitPathTxtr);
      l_UnitPathMat->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      m_pScene->Get_Root_Model()->Add_Child(m_pUnitPathDisplayModel);
      m_pUnitPathDisplayModel->Set_Material(l_UnitPathMat);
      l_UnitPathMat->Release();
      l_pUnitPathTxtr->Release();

      // Load nuke explosion sequence
      m_pNukeExplodeSequence = 
         g_Joshua.Renderer()->Helper()->LoadTextureSequence(
         ConfigValue(c_sConfigNukeExplodeTxtrSequence,
         c_sDefaultNukeExplodeTxtrSequence) );

      // Load nuke flash sequence
      m_pNukeFlashSequence = 
         g_Joshua.Renderer()->Helper()->LoadTextureSequence(
         ConfigValue(c_sConfigNukeFlashTxtrSequence,
         c_sDefaultNukeFlashTxtrSequence) );

      // Load AMDS explosion sequence
      m_pAMDSExplodeSequence[1] = 
         g_Joshua.Renderer()->Helper()->LoadTextureSequence(
         ConfigValue(c_sConfigAMDSExplodeTxtrSequence,
         c_sDefaultAMDSExplodeTxtrSequence) );

      // Load AMDS miss sequence
      m_pAMDSExplodeSequence[0] = 
         g_Joshua.Renderer()->Helper()->LoadTextureSequence(
         ConfigValue(c_sConfigAMDSMissTxtrSequence,
         c_sDefaultAMDSMissTxtrSequence) );

      m_fAMDSExplodeSize[0] = ConfigValue(c_sConfigAMDSMissSize, c_fDefaultAMDSMissSize);
      m_fAMDSExplodeSize[1] = ConfigValue(c_sConfigAMDSExplodeSize, c_fDefaultAMDSExplodeSize);

      // Setup nuke explosion display
      m_pNukeExplodeMasterModel = g_Joshua.Renderer()->Create_Model();
      GFX::GMaterial* l_NukeExplodeMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = true;
      l_NukeExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
      CGFX_Texture_Itf* l_pNukeExplodeTxtr = m_pNukeExplodeSequence;
      if(!l_pNukeExplodeTxtr)
      {
         l_pNukeExplodeTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(L"");
      }
      l_NukeExplodeMat->PixelShader(0)->TextureSampler(0)->Texture(l_pNukeExplodeTxtr);
      l_NukeExplodeMat->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_NukeExplodeMat->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_NukeExplodeMat->PixelShader(0)->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
      m_pScene->Get_Root_Model()->Add_Child(m_pNukeExplodeMasterModel);
      m_pNukeExplodeMasterModel->Set_Material(l_NukeExplodeMat);
      l_NukeExplodeMat->Release();

      // Setup nuke flash display
      m_pNukeFlashMasterModel = g_Joshua.Renderer()->Create_Model();
      GFX::GMaterial* l_NukeFlashMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_ONE;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ONE;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = true;
      l_NukeFlashMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
      CGFX_Texture_Itf* l_pNukeFlashTxtr = m_pNukeFlashSequence;
      m_pNukeFlashMesh = g_Joshua.Renderer()->CreateMesh();
      if(!l_pNukeFlashTxtr)
      {
         l_pNukeFlashTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(L"");
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pNukeFlashMesh, l_pNukeFlashTxtr);
      }
      else
      {
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pNukeFlashMesh, m_pNukeFlashSequence->Frame(0) );
      }
      l_NukeFlashMat->PixelShader(0)->TextureSampler(0)->Texture(l_pNukeFlashTxtr);
      l_NukeFlashMat->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_NukeFlashMat->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_NukeFlashMat->PixelShader(0)->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
      m_pScene->Get_Root_Model()->Add_Child(m_pNukeFlashMasterModel);
      m_pNukeFlashMasterModel->Set_Material(l_NukeFlashMat);
      l_NukeFlashMat->Release();

      {
         REAL32 l_fFlashSize = ConfigValue(c_sConfigNukeFlashSize, c_fDefaultNukeFlashSize);
         GShape& l_Shape = m_pNukeFlashMesh->Shape();
         for(UINT32 p = 0;p < l_Shape.NbPositions();p ++)
         {
            GVector3D<REAL32>& l_Pos = l_Shape.Position(p);

            l_Pos += GVector3D<REAL32>(-0.5f, -0.5f, 0.f);
            l_Pos *= l_fFlashSize;
         }
      }

      // Setup AMDS explosion display
      for(UINT32 i = 0;i < 2;i ++)
      {
         m_pAMDSExplodeMasterModel[i] = g_Joshua.Renderer()->Create_Model();
         GFX::GMaterial* l_AMDSExplodeMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = true;
         l_AMDSExplodeMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
         CGFX_Texture_Itf* l_pAMDSExplodeTxtr = m_pAMDSExplodeSequence[i];
         m_pAMDSExplodeMesh[i] = g_Joshua.Renderer()->CreateMesh();
         if(!l_pAMDSExplodeTxtr)
         {
            l_pAMDSExplodeTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(L"");
            g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pAMDSExplodeMesh[i], l_pAMDSExplodeTxtr);
         }
         else
         {
            g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pAMDSExplodeMesh[i], m_pAMDSExplodeSequence[i]->Frame(0) );
         }
         l_AMDSExplodeMat->PixelShader(0)->TextureSampler(0)->Texture(l_pAMDSExplodeTxtr);
         l_AMDSExplodeMat->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
         l_AMDSExplodeMat->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
         l_AMDSExplodeMat->PixelShader(0)->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
         m_pScene->Get_Root_Model()->Add_Child(m_pAMDSExplodeMasterModel[i]);
         m_pAMDSExplodeMasterModel[i]->Set_Material(l_AMDSExplodeMat);

         GShape& l_Shape = m_pAMDSExplodeMesh[i]->Shape();
         for(UINT32 p = 0;p < l_Shape.NbPositions();p ++)
         {
            GVector3D<REAL32>& l_Pos = l_Shape.Position(p);

            l_Pos += GVector3D<REAL32>(-0.5f, -0.5f, 0.f);
            l_Pos *= m_fAMDSExplodeSize[i];
         }

         l_AMDSExplodeMat->Release();
      }

      // Setup nuke/AMDS trail display
      m_pNukeTrailMasterModel = g_Joshua.Renderer()->Create_Model();
      GFX::GMaterial* l_NukeTrailMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = true;
      l_NukeTrailMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
      CGFX_Texture_Itf* l_pNukeTrailTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigNukePath, c_sDefaultNukePath) );
      l_NukeTrailMat->PixelShader(0)->TextureSampler(0)->Texture(l_pNukeTrailTxtr);
      l_NukeTrailMat->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_NukeTrailMat->PixelShader(0)->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      m_pScene->Get_Root_Model()->Add_Child(m_pNukeTrailMasterModel);
      m_pNukeTrailMasterModel->Set_Material(l_NukeTrailMat);
      l_NukeTrailMat->Release();
      l_pNukeTrailTxtr->Release();

      // Read remaining constants
      m_fNukeMinPathDiv = ConfigValue(c_sConfigNukeMinPathDiv, c_fDefaultNukeMinPathDiv);
      m_fNukeMaxPathHeightFactor = (REAL32) (ConfigValue(c_sConfigNukeMaxPathHeight, c_fDefaultNukeMaxPathHeight) / Math::PI);
      m_fNukeTrailFadeTime = ConfigValue(c_sConfigNukeTrailFadeTime, c_fDefaultNukeTrailFadeTime);

      m_fNukeBlinkFreq = ConfigValue(c_sConfigNukeBlinkFreq, c_fDefaultNukeBlinkFreq);
      m_fNukeBlinkFreqInv = 1.f / m_fNukeBlinkFreq;

      m_fNukeBlinkScale = ConfigValue(c_sConfigNukeBlinkScale, c_fDefaultNukeBlinkScale);
      m_fNukeBlinkOffset = ConfigValue(c_sConfigNukeBlinkOffset, c_fDefaultNukeBlinkOffset);

      m_NukeBlinkColors[0] = ConfigValue(c_sConfigNukeBlinkColor, c_DefaultNukeBlinkColor);
      m_NukeBlinkColors[1] = ConfigValue(c_sConfigAMDSLongBlinkColor, c_DefaultAMDSLongBlinkColor);
      m_NukeBlinkColors[2] = ConfigValue(c_sConfigAMDSShortBlinkColor, c_DefaultAMDSShortBlinkColor);

      m_fNukeExplodeSize = ConfigValue(c_sConfigNukeExplodeSize, c_fDefaultNukeExplodeSize);
      m_fNukeExplodeFPS = ConfigValue(c_sConfigNukeExplodeFrameRate, c_fDefaultNukeExplodeFrameRate);

      m_fNukeExplodeDelay = ConfigValue(c_sConfigNukeExplodeDelay, c_fDefaultNukeExplodeDelay);

      m_fNukeFlashFPS = ConfigValue(c_sConfigNukeFlashFrameRate, c_fDefaultNukeFlashFrameRate);

      m_fAMDSStart[0] = ConfigValue(c_sConfigAMDSLongRangeStart, c_fDefaultAMDSLongRangeStart);
      m_fAMDSStart[1] = ConfigValue(c_sConfigAMDSShortRangeStart, c_fDefaultAMDSShortRangeStart);

      m_fAMDSExplodeFPS[0] = ConfigValue(c_sConfigAMDSMissFrameRate, c_fDefaultAMDSMissFrameRate);
      m_fAMDSExplodeFPS[1] = ConfigValue(c_sConfigAMDSExplodeFrameRate, c_fDefaultAMDSExplodeFrameRate);
   }

   m_pScene->Get_Root_Model()->Bring_To_Front(m_pRealisticOverlayModel);
   m_pScene->Get_Root_Model()->Bring_To_Front(m_pNukeTrailMasterModel);
   m_pScene->Get_Root_Model()->Bring_To_Front(m_pAMDSExplodeMasterModel[1]);
   m_pScene->Get_Root_Model()->Bring_To_Front(m_pAMDSExplodeMasterModel[0]);

   UpdateSun();
   UpdateFlares();

   if(m_pProgressMeter)
      m_pProgressMeter->PercentageUpdate(80);

   // Setup earth overlay
   m_pOverlay = g_Joshua.Renderer()->Create_Model();
   m_pModel->Add_Child(m_pOverlay);


   GFX::GWindow*   				window	= NULL;
   //Create a Model and its primitive (WINDOW)
   window = g_Joshua.Renderer()->CreateWindow();
   window->BorderBottom() = 1;
   window->BorderTop() = 1;
   window->BorderRight() = 1;
   window->BorderLeft() = 1;
   window->BackgroundType(GFX::EBgndType::Border);



   // Create drag box
   m_pDragBoxModel = g_Joshua.Renderer()->Create_Model();
   m_pDragBoxTransform = g_Joshua.Renderer()->Create_Transform_2D();
   m_pDragBoxTexture = g_Joshua.Renderer()->Helper()->LoadTexture("/data/images/DragBox.png");

   // Initialize drag box Model
   m_pOverlay->Add_Child(m_pDragBoxModel);
   m_pDragBoxModel->Set_Transform(m_pDragBoxTransform);
   m_pDragBoxModel->Set_Primitive(window);
   m_pDragBoxModel->Visibility() = false;

   // Initialize drag box transform
   m_pDragBoxTransform->Position().X = 0;
   m_pDragBoxTransform->Position().Y = 0;
   m_pDragBoxTransform->Size().X = 0;
   m_pDragBoxTransform->Size().Y = 0;

   // Initialize drag box material
   window->SetImage(m_pDragBoxTexture);
   window->Release();

   // Initialize the generic mesh for a notification sign on the earth
   m_pSignMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_DIFFUSE);

   m_pSignMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   m_pSignMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   m_pSignMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

   m_pSignMesh = g_Joshua.Renderer()->CreateMesh();
   GShape& l_SignShape = m_pSignMesh->Shape();
   l_SignShape.NbPositions(2);
   l_SignShape.Position(0) = GVector3D<REAL32>(0, 0, 0);
   l_SignShape.Position(1) = GVector3D<REAL32>(1, 1, 0);

   l_SignShape.NbColors(2);
   l_SignShape.Color(0) = GColorRGBInt(255, 255, 255, 128);
   l_SignShape.Color(1) = GColorRGBInt(255, 255, 255, 128);

   m_pSignMesh->NbFacetGroups(1);
   m_pSignMesh->FacetGroup(0).Facets().Type(FACETS_LINE, false);
   m_pSignMesh->FacetGroup(0).Facets().LineList().Start() = 0;
   m_pSignMesh->FacetGroup(0).Facets().LineList().NbLines(1);

   //! \todo TBR : This is a test for notification signs on earth (Paris)
   //AddNotificationSignOnEarth(GVector2D<REAL32>(2.34f, 48.84f), L"Paris is Here !!!");

   //Create custom parameters
   UINT32 l_iTemp;
   l_iTemp = m_pCamera->Add_Custom_Parameter(L"EarthLongitude");
   assert(l_iTemp == CUSTOM_PARAM_LONGITUDE);

   l_iTemp = m_pCamera->Add_Custom_Parameter(L"EarthLatitude");
   assert(l_iTemp == CUSTOM_PARAM_LATITUDE);

   l_iTemp = m_pCamera->Add_Custom_Parameter(L"EarthDistance");
   assert(l_iTemp == CUSTOM_PARAM_DISTANCE);

   m_pfCamAngleX = &(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LONGITUDE));
   m_pfCamAngleY = &(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LATITUDE));
   m_pfCamDist   = &(m_pCamera->Custom_Parameter(CUSTOM_PARAM_DISTANCE));

   // Create unit path elem GFX objects
   m_pUnitPathElemDisplayModel = g_Joshua.Renderer()->Create_Model();

   // Create unit target GFX objects
   m_pUnitTargetDisplayModel = g_Joshua.Renderer()->Create_Model();

   GFX::GDefaultVertexShader* l_pUnitTargetVS;
   GFX::GCascadePixelShader* l_pUnitTargetPS;
   GFX::GMaterial* l_pUnitTargetMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pUnitTargetVS,
      &l_pUnitTargetPS);

   // Create unit GFX objects
   m_pUnitDisplayModel = g_Joshua.Renderer()->Create_Model();

   GFX::GDefaultVertexShader* l_pUnitVS;
   GFX::GCascadePixelShader* l_pUnitPS;
   GFX::GMaterial* l_pUnitMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pUnitVS,
      &l_pUnitPS);

   // Create unit status GFX objects
   m_pUnitStatusDisplayModel = g_Joshua.Renderer()->Create_Model();
   m_pUnitStatusMesh = g_Joshua.Renderer()->CreateMesh();

   GFX::GDefaultVertexShader* l_pUnitStatusVS;
   GFX::GCascadePixelShader* l_pUnitStatusPS;
   GFX::GMaterial* l_pUnitStatusMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pUnitStatusVS,
      &l_pUnitStatusPS);

   // Create unit progress GFX objects
   m_pUnitProgressMasterModel = g_Joshua.Renderer()->Create_Model();
   m_pUnitProgressBGMesh = g_Joshua.Renderer()->CreateMesh();
   m_pUnitProgressPercentMesh = g_Joshua.Renderer()->CreateMesh();

   GFX::GDefaultVertexShader* l_pUnitProgressVS;
   GFX::GCascadePixelShader* l_pUnitProgressPS;
   GFX::GMaterial* l_pUnitProgressMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_DIFFUSE,
      &l_pUnitProgressVS,
      &l_pUnitProgressPS);
   
   // Create combat GFX objects
   m_pCombatDisplayModel = g_Joshua.Renderer()->Create_Model();
   GFX::GDefaultVertexShader* l_pCombatVS;
   GFX::GCascadePixelShader* l_pCombatPS;
   GFX::GMaterial* l_pCombatMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pCombatVS,
      &l_pCombatPS);

   m_pCombatMesh = g_Joshua.Renderer()->CreateMesh();

   //Create bombardment GFX Objects
   m_pBombardmentDisplayModel = g_Joshua.Renderer()->Create_Model();
   GFX::GDefaultVertexShader* l_pBombardmentVS;
   GFX::GCascadePixelShader* l_pBombardmentPS;
   GFX::GMaterial* l_pBombardmentMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pBombardmentVS,
      &l_pBombardmentPS);
   m_pBombardmentMesh = g_Joshua.Renderer()->CreateMesh();

   // Initialize unit path color overrides
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      if(i != c_DefaultPathFriendship)
      {
         m_pUnitPathColorOverrides[i] = g_Joshua.Renderer()->CreateOverridePSCascade();
         m_pUnitPathColorOverrides[i]->UseColorFactorOverride(true);
         m_pUnitPathColorOverrides[i]->ColorFactorOverride() = m_UnitPathColors[i];
      }
   }

   // Initialize unit path elem models
   m_pOverlay->Add_Child(m_pUnitPathElemDisplayModel);

   // Initialize unit path elem texture
   CGFX_Texture_Itf* l_pUnitElemTxtr = 
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigPathElemTxtr,
      c_sDefaultPathElemTxtr),
      true);

   m_UnitPathElemSize.x = (INT32) l_pUnitElemTxtr->PartialWidth();
   m_UnitPathElemSize.y = (INT32) l_pUnitElemTxtr->PartialHeight();

   m_UnitPathElemOffset.x = (-m_UnitPathElemSize.x) >> 1;
   m_UnitPathElemOffset.y = (-m_UnitPathElemSize.y) >> 1;

   // Initialize unit path elem mesh
   m_pUnitPathElemMesh = g_Joshua.Renderer()->CreateMesh();
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitPathElemMesh, l_pUnitElemTxtr);

   // Initialize unit path elem sub-models and materials
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      m_pUnitPathElemSubModels[i] = g_Joshua.Renderer()->Create_Model();

      m_pUnitPathElemDisplayModel->Add_Child(m_pUnitPathElemSubModels[i] );

      GFX::GDefaultVertexShader* l_pUnitElemVS;
      GFX::GCascadePixelShader* l_pUnitElemPS;
      GFX::GMaterial* l_pUnitElemMaterial =
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
         GFX::PSTYPE_DEF_SIMPLE_TXTR,
         &l_pUnitElemVS,
         &l_pUnitElemPS);

      m_pUnitPathElemSubModels[i]->Set_Material(l_pUnitElemMaterial);

      l_pUnitElemMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pUnitElemMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pUnitElemMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pUnitElemMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      l_pUnitElemPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
      l_pUnitElemPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
      l_pUnitElemPS->ColorFactor() = m_UnitPathColors[i];
      l_pUnitElemPS->TextureSampler(0)->Texture(l_pUnitElemTxtr);



      l_pUnitElemMaterial->Release();
      l_pUnitElemVS->Release();
      l_pUnitElemPS->Release();
   }
   l_pUnitElemTxtr->Release();

   // Initialize unit target model
   m_pOverlay->Add_Child(m_pUnitTargetDisplayModel);
   m_pUnitTargetDisplayModel->Set_Material(l_pUnitTargetMaterial);

   // Initialize unit target material
   l_pUnitTargetMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pUnitTargetMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pUnitTargetMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pUnitTargetMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

   // Initialize small unit target texture
   CGFX_Texture_Itf* l_pUnitTargetSmallTxtr = 
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigPathDestSmallTxtr,
      c_sDefaultPathDestSmallTxtr),
      true);
   l_pUnitTargetPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
   l_pUnitTargetPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
   l_pUnitTargetPS->ColorFactor() = m_UnitPathColors[c_DefaultPathFriendship];
   l_pUnitTargetPS->TextureSampler(0)->Texture(l_pUnitTargetSmallTxtr);
   m_UnitTargetSize[0].x = (INT32) l_pUnitTargetSmallTxtr->PartialWidth();
   m_UnitTargetSize[0].y = (INT32) l_pUnitTargetSmallTxtr->PartialHeight();

   m_UnitTargetOffset[0].x = (-m_UnitTargetSize[0].x) >> 1;
   m_UnitTargetOffset[0].y = (-m_UnitTargetSize[0].y) >> 1;

   // Initialize small unit target mesh
   m_pUnitTargetMesh[0] = g_Joshua.Renderer()->CreateMesh();
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitTargetMesh[0], l_pUnitTargetSmallTxtr);

   l_pUnitTargetMaterial->Release();
   l_pUnitTargetVS->Release();
   l_pUnitTargetPS->Release();
   l_pUnitTargetSmallTxtr->Release();

   // Initialize selected unit target texture
   CGFX_Texture_Itf* l_pUnitTargetSelectTxtr =
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigPathDestTxtr,
      c_sDefaultPathDestTxtr),
      true);
   m_pUnitTargetTSamplers[1] = g_Joshua.Renderer()->CreateOverrideTSampler();
   m_pUnitTargetTSamplers[1]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
   m_pUnitTargetTSamplers[1]->OriginalTextureSamplerID(0);
   m_pUnitTargetTSamplers[1]->OverrideTextureSampler()->Texture(l_pUnitTargetSelectTxtr);

   m_UnitTargetSize[1].x = (INT32) l_pUnitTargetSelectTxtr->PartialWidth();
   m_UnitTargetSize[1].y = (INT32) l_pUnitTargetSelectTxtr->PartialHeight();

   m_UnitTargetOffset[1].x = (-m_UnitTargetSize[1].x) >> 1;
   m_UnitTargetOffset[1].y = (-m_UnitTargetSize[1].y) >> 1;

   // Initialize selected unit target mesh
   m_pUnitTargetMesh[1] = g_Joshua.Renderer()->CreateMesh();
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitTargetMesh[1], l_pUnitTargetSelectTxtr);
   l_pUnitTargetSelectTxtr->Release();

   // Initialize unit Model
   m_pOverlay->Add_Child(m_pUnitDisplayModel);
   m_pUnitDisplayModel->Set_Material(l_pUnitMaterial);

   // Initialize unit material
   l_pUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

   CGFX_Texture_Itf* l_pUnitTxtr =
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigUnitTxtr[EUnitFriendship::Neutral],
      c_sDefaultUnitTxtr[EUnitFriendship::Neutral] ),
      true);
   l_pUnitPS->TextureSampler(0)->Texture(l_pUnitTxtr);
   l_pUnitPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
   l_pUnitPS->ColorFactor().A = 1.f;

   // Initialize all unit overrides & meshes
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      m_pUnitMeshes[i] = g_Joshua.Renderer()->CreateMesh();

      CGFX_Texture_Itf* l_pOverrideTxtr =
         g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigUnitTxtr[i], c_sDefaultUnitTxtr[i] ), true);

      m_UnitSize[i].x = l_pOverrideTxtr->PartialWidth();
      m_UnitSize[i].y = l_pOverrideTxtr->PartialHeight();

      m_UnitSizeHalf[i] = m_UnitSize[i] / 2;


      //Create the Unit Ships textures
      {
         //This code assumes that the size of the naval icons is the same as the std icons
         CGFX_Texture_Itf* l_pNavalOverrideTxtr =g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigNavalTxtr[i], c_sDefaultNavalUnitTxtr[i] ), true);
         m_pUnitShipsTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitShipsTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitShipsTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitShipsTSamplers[i]->OverrideTextureSampler()->Texture(l_pNavalOverrideTxtr);
         l_pNavalOverrideTxtr->Release();
      }

      //Create the bombarding ships textures
      {
         //This code assumes that the size of the naval icons is the same as the std icons
         CGFX_Texture_Itf* l_pNavalOverrideTxtr =g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigNavalBombardingTxtr[i], c_sDefaultNavalUnitTxtr[i] ), true);
         m_pUnitShipsBombardingTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitShipsBombardingTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitShipsBombardingTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitShipsBombardingTSamplers[i]->OverrideTextureSampler()->Texture(l_pNavalOverrideTxtr);
         l_pNavalOverrideTxtr->Release();
      }

      if(l_pOverrideTxtr != l_pUnitTxtr)
      {
         m_pUnitTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitTSamplers[i]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);

         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitMeshes[i], l_pOverrideTxtr);
      }
      else
      {
         m_pUnitTSamplers[i] = NULL;
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitMeshes[i], l_pUnitTxtr);
      }
      l_pOverrideTxtr->Release();
   }
   
   // Initialize all unit selection overrides & meshes
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      //Unit Selection Override & Mesh
      {
         m_pUnitSelectionMeshes[i] = g_Joshua.Renderer()->CreateMesh();

         CGFX_Texture_Itf* l_pOverrideTxtr =
            g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigUnitSelTxtr[i], c_sDefaultUnitSelTxtr[i] ), true);
         
         m_UnitSelectionSize[i].x = l_pOverrideTxtr->PartialWidth();
         m_UnitSelectionSize[i].y = l_pOverrideTxtr->PartialHeight();

         m_UnitSelectionSizeHalf[i] = m_UnitSelectionSize[i] / 2;

         m_pUnitSelectionTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitSelectionTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitSelectionTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitSelectionTSamplers[i]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitSelectionMeshes[i], l_pOverrideTxtr);
         l_pOverrideTxtr->Release();
      }
      //Flag selection override & mesh (when units are displayed as flags and units are selected)
      {
         m_pUnitFlagSelectionMeshes[i] = g_Joshua.Renderer()->CreateMesh();
         CGFX_Texture_Itf* l_pOverrideTxtr =
            g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_ConfigUnitFlagSelTxtr[i], c_sDefaultUnitFlagSelTxtr[i] ), true);
         m_UnitFlagSelectionSize[i].x = l_pOverrideTxtr->PartialWidth();
         m_UnitFlagSelectionSize[i].y = l_pOverrideTxtr->PartialHeight();
         m_UnitFlagSelectionSizeHalf[i] = m_UnitFlagSelectionSize[i] / 2;
         m_pUnitFlagSelectionTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitFlagSelectionTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitFlagSelectionTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitFlagSelectionTSamplers[i]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitFlagSelectionMeshes[i], l_pOverrideTxtr);
         l_pOverrideTxtr->Release();
      }
   }

   // Initialize status unit Model
   m_pOverlay->Add_Child(m_pUnitStatusDisplayModel);
   m_pUnitStatusDisplayModel->Set_Material(l_pUnitStatusMaterial);
   m_pUnitStatusDisplayModel->Set_Transform(g_Joshua.Renderer()->Create_Transform_2D() );
   m_pUnitStatusDisplayModel->Get_Transform()->Release();

   // Initialize status unit material
   l_pUnitStatusMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pUnitStatusMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pUnitStatusMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pUnitStatusMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
   CGFX_Texture_Itf* l_pUnitStatusTxtr =
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigUnitStatusTxtr[EMilitaryStatus::Parked],
      c_sDefaultUnitStatusTxtr[EMilitaryStatus::Parked] ),
      true);
   l_pUnitStatusPS->TextureSampler(0)->Texture(l_pUnitStatusTxtr);

   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitStatusMesh, l_pUnitStatusTxtr);

   m_UnitStatusSize.x = l_pUnitStatusTxtr->PartialWidth();
   m_UnitStatusSize.y = l_pUnitStatusTxtr->PartialHeight();

   // Initialize all unit status overrides
   for(UINT32 i = 0;i < EMilitaryStatus::CountVisible;i ++)
   {
      CGFX_Texture_Itf* l_pOverrideTxtr =
         g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigUnitStatusTxtr[i], c_sDefaultUnitStatusTxtr[i] ), true);
      if(l_pOverrideTxtr != l_pUnitStatusTxtr)
      {
         m_pUnitStatusTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pUnitStatusTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pUnitStatusTSamplers[i]->OriginalTextureSamplerID(0);
         m_pUnitStatusTSamplers[i]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);
      }
      else
      {
         m_pUnitStatusTSamplers[i] = NULL;
      }
      l_pOverrideTxtr->Release();
   }

   m_pUnitStatusDisplayModel->Set_Primitive(m_pUnitStatusMesh);

   // Initialize unit progress model
   m_pOverlay->Add_Child(m_pUnitProgressMasterModel);
   m_pUnitProgressMasterModel->Set_Material(l_pUnitProgressMaterial);

   // Initialize unit progress material
   l_pUnitProgressMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pUnitProgressMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pUnitProgressMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pUnitProgressMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

   // Initialize unit progress meshes
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitProgressBGMesh,
      ConfigValue(c_sConfigUnitProgressBGColor, c_DefaultUnitProgressBGColor) );

   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitProgressPercentMesh,
      ConfigValue(c_sConfigUnitProgressFGColor, c_DefaultUnitProgressFGColor) );

   // Initialize remaining progress constants
   m_iUnitProgressHeight = ConfigValue(c_sConfigUnitProgressHeight, c_iDefaultUnitProgressHeight);
   m_iUnitProgressDistance = ConfigValue(c_sConfigUnitProgressDistance, c_iDefaultUnitProgressDistance);

   // Initialize combat Model
   m_pOverlay->Add_Child(m_pCombatDisplayModel);
   m_pCombatDisplayModel->Set_Material(l_pCombatMaterial);

   //Initialize Bombardment Model
   m_pOverlay->Add_Child(m_pBombardmentDisplayModel);
   m_pBombardmentDisplayModel->Set_Material(l_pBombardmentMaterial);

   // Initialize combat material
   l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
   CGFX_Texture_Itf* l_pCombatTxtr = 
      g_Joshua.Renderer()->Helper()->LoadTextureSequence(ConfigValue(c_sConfigCombatSequence, 
      c_sDefaultCombatSequence) );
   l_pCombatPS->TextureSampler(0)->Texture(l_pCombatTxtr);

   // Initialize combat sequence
   if(l_pCombatTxtr)
   {
      m_pCombatSequence = (GFX::GTextureSequence*) l_pCombatTxtr;
      l_pCombatTxtr = ( (GFX::GTextureSequence*) l_pCombatTxtr)->Frame(0);
      l_pCombatTxtr->Inc_Ref();
      m_fCombatFPS = ConfigValue(c_sConfigCombatSequenceFPS,
         c_fDefaultCombatSequenceFPS);

      CGFX_Animation_Linear_Itf* l_pCombatAnim =
         g_Joshua.Renderer()->Helper()->AnimateTextureSequence( m_pCombatSequence,
         m_pCombatSequence->Controller(), 
         m_fCombatFPS);

      CGFX_Action_Itf* l_pCombatAction = g_Joshua.Renderer()->Create_Action();
      l_pCombatAction->Add_Animation(l_pCombatAnim);
      l_pCombatAction->Length(l_pCombatAnim->Key(l_pCombatAnim->Get_Nb_Keys() - 1).m_Time);

      CGFX_Track_Itf* l_pCombatTrack = g_Joshua.Renderer()->Create_Track();
      l_pCombatTrack->Set_Action(l_pCombatAction);
      l_pCombatTrack->Playback_Count(0);

      m_pScene->Get_Animation_System()->Add_Track(l_pCombatTrack);

      SAFE_RELEASE(l_pCombatAnim);
      SAFE_RELEASE(l_pCombatAction);
      SAFE_RELEASE(l_pCombatTrack);
   }
   else
   {
      l_pCombatTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(L"");
      l_pCombatPS->TextureSampler(0)->Texture(l_pCombatTxtr);
   }

   // Initialize Bombardment material
   l_pBombardmentMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pBombardmentMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pBombardmentMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pBombardmentMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
   CGFX_Texture_Itf* l_pBombardmentTxtr = 
      g_Joshua.Renderer()->Helper()->LoadTextureSequence(ConfigValue(c_sConfigBombardmentSequence, 
      c_sDefaultBombardmentSequence) );
   l_pBombardmentPS->TextureSampler(0)->Texture(l_pBombardmentTxtr);

   // Initialize Bombardment sequence
   if(l_pBombardmentTxtr)
   {
      m_pBombardmentSequence = (GFX::GTextureSequence*) l_pBombardmentTxtr;
      l_pBombardmentTxtr = ( (GFX::GTextureSequence*) l_pBombardmentTxtr)->Frame(0);
      l_pBombardmentTxtr->Inc_Ref();
      m_fBombardmentFPS = ConfigValue(c_sConfigBombardmentSequenceFPS,
         c_fDefaultBombardmentSequenceFPS);

      CGFX_Animation_Linear_Itf* l_pBombardmentAnim =
         g_Joshua.Renderer()->Helper()->AnimateTextureSequence( m_pBombardmentSequence,
         m_pBombardmentSequence->Controller(), 
         m_fBombardmentFPS);

      CGFX_Action_Itf* l_pBombardmentAction = g_Joshua.Renderer()->Create_Action();
      l_pBombardmentAction->Add_Animation(l_pBombardmentAnim);
      l_pBombardmentAction->Length(l_pBombardmentAnim->Key(l_pBombardmentAnim->Get_Nb_Keys() - 1).m_Time);

      CGFX_Track_Itf* l_pBombardmentTrack = g_Joshua.Renderer()->Create_Track();
      l_pBombardmentTrack->Set_Action(l_pBombardmentAction);
      l_pBombardmentTrack->Playback_Count(0);

      m_pScene->Get_Animation_System()->Add_Track(l_pBombardmentTrack);

      SAFE_RELEASE(l_pBombardmentAnim);
      SAFE_RELEASE(l_pBombardmentAction);
      SAFE_RELEASE(l_pBombardmentTrack);
   }
   else
   {
      l_pBombardmentTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(L"");
      l_pBombardmentPS->TextureSampler(0)->Texture(l_pBombardmentTxtr);
   }

   
   m_SelectedCountryTrack = g_Joshua.Renderer()->Create_Track();  

   for (int i = 0; i < 3; i++)
   { 
      m_SelectedCountryAnimation[i] = g_Joshua.Renderer()->CreateAnimationMultiple();
      m_SelectedCountryLinearAnimation[i] = g_Joshua.Renderer()->Create_Animation_Linear();
   }

   m_SelectedCountryTrack->Playback_Count(0);
   m_SelectedCountryTrack->Set_Start_Time(0);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_SelectedCountryTrack);

   // Start animations
   m_pScene->Get_Animation_System()->Play_Animation();


   m_CombatSize.x = (INT32) l_pCombatTxtr->PartialWidth();
   m_CombatSize.y = (INT32) l_pCombatTxtr->PartialHeight();

   m_CombatOffset.x = (-m_CombatSize.x) >> 1;
   m_CombatOffset.y = (-m_CombatSize.y) >> 1;

   // Initialize combat mesh
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pCombatMesh, l_pCombatTxtr);



   m_BombardmentSize.x = (INT32) l_pBombardmentTxtr->PartialWidth();
   m_BombardmentSize.y = (INT32) l_pBombardmentTxtr->PartialHeight();

   m_BombardmentOffset.x = (-m_BombardmentSize.x) >> 1;
   m_BombardmentOffset.y = (-m_BombardmentSize.y) >> 1;

   // Initialize Bombardment mesh
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pBombardmentMesh, l_pBombardmentTxtr);




   // Release temporary GFX objects
   l_pUnitMaterial->Release();
   l_pUnitVS->Release();
   l_pUnitPS->Release();
   l_pUnitTxtr->Release();

   l_pUnitStatusMaterial->Release();
   l_pUnitStatusVS->Release();
   l_pUnitStatusPS->Release();
   l_pUnitStatusTxtr->Release();
   l_pUnitProgressMaterial->Release();
   l_pUnitProgressVS->Release();
   l_pUnitProgressPS->Release();

   l_pCombatMaterial->Release();
   l_pCombatVS->Release();
   l_pCombatPS->Release();
   l_pCombatTxtr->Release();
   l_pBombardmentMaterial->Release();
   l_pBombardmentVS->Release();
   l_pBombardmentPS->Release();
   l_pBombardmentTxtr->Release();

   // Create nuke GFX objects
   m_pNukeIconMasterModel = g_Joshua.Renderer()->Create_Model();

   GFX::GMaterial* l_pNukeMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR);

   m_pNukeIconMesh = g_Joshua.Renderer()->CreateMesh();

   // Initialize nuke Model
   m_pOverlay->Add_Child(m_pNukeIconMasterModel);
   m_pNukeIconMasterModel->Set_Material(l_pNukeMaterial);

   // Initialize nuke material
   l_pNukeMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pNukeMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pNukeMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pNukeMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
   CGFX_Texture_Itf* l_pNukeTxtr =
      g_Joshua.Renderer()->Helper()->LoadTexture(ConfigValue(c_sConfigNukeIcon, c_sDefaultNukeIcon) );
   l_pNukeMaterial->PixelShader(0)->TextureSampler(0)->Texture(l_pNukeTxtr);
   ( (GFX::GCascadePixelShader*) l_pNukeMaterial->PixelShader(0) )->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);

   m_NukeIconSize.x = l_pNukeTxtr->PartialWidth();
   m_NukeIconSize.y = l_pNukeTxtr->PartialHeight();

   m_NukeIconSizeHalf = m_NukeIconSize / 2;

   // Initialize nuke mesh
   g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pNukeIconMesh, l_pNukeTxtr);

   // Release temporary nuke objects
   l_pNukeMaterial->Release();
   l_pNukeTxtr->Release();

   // Display path elements, then path destinations, then combats, then progress,
   // then units, then selection, then nukes/AMDS
   m_pOverlay->Send_To_Back(m_pNukeIconMasterModel);
   m_pOverlay->Send_To_Back(m_pUnitDisplayModel);
   m_pOverlay->Send_To_Back(m_pUnitProgressMasterModel);
   m_pOverlay->Send_To_Back(m_pCombatDisplayModel);
   m_pOverlay->Send_To_Back(m_pBombardmentDisplayModel);
   m_pOverlay->Send_To_Back(m_pUnitTargetDisplayModel);
   m_pOverlay->Send_To_Back(m_pUnitPathElemDisplayModel);

   m_pCityOverlay = new GOverlayCity(this);
   m_pCityOverlay->InitCities();

   m_pNotificationParent = g_Joshua.Renderer()->Create_Model();
   m_pModel->Add_Child(m_pNotificationParent);

   m_pNotificationContainer = (GUI::GUIContainer*)CREATE_GUI_OBJECT_VISIBLE(GUI::GUIContainer, this);
   m_pNotificationContainer->Position(GVector2D<INT32>(0, 0));
   m_pNotificationContainer->Size(GVector2D<INT32>(0, 0));

   // hot keys used on background earth
   RegisterHotkeys();

   //Display combat units
   DisplayCombats(true);
   DisplayBombardments(true);

   if(m_pProgressMeter)
      m_pProgressMeter->PercentageUpdate(100);

   m_pProgressMeter = NULL;

   return true;
}

void GBackgroundEarth::SendToBackUnits()
{
   Model()->Send_To_Back(m_pOverlay);
}

/*!
* Destroy the Background Earth,
*     releases all the GUI objects that must be
*     released
**/
bool GBackgroundEarth::OnDestroy()
{
   UnregisterHotkeys();

   g_Joshua.Renderer()->RenderTree()->RemoveScene(m_pScene);

   m_pModel->Remove_Child(m_pNotificationParent);
   SAFE_RELEASE(m_pNotificationParent);

   SAFE_RELEASE(m_pScene);

   SAFE_RELEASE(m_pCameraModel);
   SAFE_RELEASE(m_pCameraTransform);
   SAFE_RELEASE(m_pCamera);

   SAFE_RELEASE(m_pSunModel);
   SAFE_RELEASE(m_pSunTransform);
   SAFE_RELEASE(m_pSunLight);

   SAFE_RELEASE(m_pRealisticMasterModel);
   SAFE_RELEASE(m_pRealisticOverlayModel);

   SAFE_RELEASE(m_pFlareMasterModel);

   for(UINT32 f = 0;f < c_NbFlares + 1;f ++)
   {
      SAFE_RELEASE(m_pFlareModel[f] );
      SAFE_RELEASE(m_pFlareTransform[f] );
   }

   SAFE_RELEASE(m_pStarsModel);
   SAFE_RELEASE(m_pStarsMesh);
   SAFE_RELEASE(m_pStarsMaterial);
   SAFE_RELEASE(m_pStarsTexture);

   SAFE_RELEASE(m_pMoonModel);
   SAFE_RELEASE(m_pMoonTransform);
   SAFE_RELEASE(m_pMoonMaterial);

   SAFE_RELEASE(m_pEarthModelMaster);
   SAFE_RELEASE(m_pEarthTransformTemp);
   SAFE_RELEASE(m_pEarthMaterial);
   SAFE_RELEASE(m_pEarthMaterialNoSun);

   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      SAFE_RELEASE(m_pEarthModel[i]);
      SAFE_RELEASE(m_pEarthMesh[i]);
      SAFE_RELEASE(m_pEarthOverride[i]);
      SAFE_RELEASE(m_pEarthTexture[i]);
      SAFE_RELEASE(m_pEarthTextureLow[i]);

      SAFE_RELEASE(m_pDarkOverride[i]);

      SAFE_RELEASE(m_pBumpOverride[i]);
      SAFE_RELEASE(m_pBumpTexture[i]);
   }

   SAFE_RELEASE(m_pNightTexture);

   SAFE_RELEASE(m_pWaterMesh);
   SAFE_RELEASE(m_pWaterModel);
   SAFE_RELEASE(m_pWaterMaterial);
   SAFE_RELEASE(m_pWaterTexture);

   SAFE_DELETE(m_pAtlasEarth);
   SAFE_DELETE(m_pCityOverlay);

   SAFE_RELEASE(m_pFillModel);
   SAFE_RELEASE(m_pFillModelStrategic);
   SAFE_RELEASE(m_pFillMesh);
   SAFE_RELEASE(m_pFillVS);

   SAFE_RELEASE(m_pDisplayParticlesModelMaster);
   SAFE_RELEASE(m_pDisplayParticlesMesh);
   SAFE_RELEASE(m_pDisplayParticlesMaterial);

   SAFE_RELEASE(m_pCloudModel);
   SAFE_RELEASE(m_pCloudMesh);
   SAFE_RELEASE(m_pCloudTransform);
   SAFE_RELEASE(m_pCloudMaterial);
   SAFE_RELEASE(m_pCloudTexture);

   SAFE_RELEASE(m_pStrategicMasterModel);
   SAFE_RELEASE(m_pStrategicEarthModelMaster);
   SAFE_RELEASE(m_pStrategicEarthMaterial);

   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      SAFE_RELEASE(m_pStrategicEarthModels[i] );
      SAFE_RELEASE(m_pStrategicEarthOverrides[i] );
      SAFE_RELEASE(m_pStrategicEarthBumpOverrides[i] );
      SAFE_RELEASE(m_pStrategicEarthTextures[i] );
   }

   SAFE_RELEASE(m_pStrategicEarthLinesModel);
   SAFE_RELEASE(m_pStrategicEarthLines);
   SAFE_RELEASE(m_pStrategicEarthLinesMaterial);

   SAFE_RELEASE(m_pAtmoHaloModel);
   SAFE_RELEASE(m_pAtmoHaloTransform);

   SAFE_RELEASE(m_pOutlineModel);
   SAFE_RELEASE(m_pOutlineMesh);

   SAFE_RELEASE(m_pUnitPathDisplayModel);
   SAFE_RELEASE(m_pNukeIconMasterModel);
   SAFE_RELEASE(m_pNukeIconMesh);
   SAFE_RELEASE(m_pNukeTrailMasterModel);
   SAFE_RELEASE(m_pNukeExplodeMasterModel);
   SAFE_RELEASE(m_pAMDSExplodeMasterModel[0] );
   SAFE_RELEASE(m_pAMDSExplodeMasterModel[1] );
   SAFE_RELEASE(m_pAMDSExplodeMesh[0] );
   SAFE_RELEASE(m_pAMDSExplodeMesh[1] );

   SAFE_RELEASE(m_pNukeFlashMasterModel);
   SAFE_RELEASE(m_pNukeFlashMesh);

   SAFE_RELEASE(m_pTransitionViewTrack);

   SAFE_RELEASE(m_pDragBoxModel);
   SAFE_RELEASE(m_pDragBoxTransform);
   SAFE_RELEASE(m_pDragBoxTexture);

   SAFE_RELEASE(m_pSignMesh);
   SAFE_RELEASE(m_pSignMaterial);

   GNotificationSignNode l_Iter = m_Signs.begin();
   while(l_Iter != m_Signs.end() )
   {
      GBackgroundEarth::GNotificationSignData* l_Data = (GBackgroundEarth::GNotificationSignData*)&l_Iter->first;
      
      SAFE_RELEASE(l_Data->m_pModel);
      SAFE_RELEASE(l_Data->m_pTrans);

      ++ l_Iter;
   }
   m_Signs.clear();

   SAFE_RELEASE(m_pOverlay);

   SAFE_RELEASE(m_pUnitPathElemMesh);

   SAFE_RELEASE(m_pUnitTargetDisplayModel);
   SAFE_RELEASE(m_pUnitTargetMesh[0] );
   SAFE_RELEASE(m_pUnitTargetMesh[1] );

   SAFE_RELEASE(m_pUnitTargetTSamplers[0] );
   SAFE_RELEASE(m_pUnitTargetTSamplers[1] );
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      SAFE_RELEASE(m_pUnitPathColorOverrides[i] );
      SAFE_RELEASE(m_pUnitPathElemSubModels[i] );
   }

   SAFE_RELEASE(m_pUnitPathElemDisplayModel);

   SAFE_RELEASE(m_pUnitDisplayModel);
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      SAFE_RELEASE(m_pUnitMeshes[i] );
      SAFE_RELEASE(m_pUnitTSamplers[i] );
      SAFE_RELEASE(m_pUnitShipsTSamplers[i]);
      SAFE_RELEASE(m_pUnitShipsBombardingTSamplers[i]);
   }

   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      SAFE_RELEASE(m_pUnitSelectionMeshes[i] );
      SAFE_RELEASE(m_pUnitSelectionTSamplers[i] );
   }

   SAFE_RELEASE(m_pUnitStatusDisplayModel);
   SAFE_RELEASE(m_pUnitStatusMesh);

   for(UINT32 i = 0;i < EMilitaryStatus::CountVisible;i ++)
   {
      SAFE_RELEASE(m_pUnitStatusTSamplers[i] );
   }

   SAFE_RELEASE(m_pUnitProgressMasterModel);
   SAFE_RELEASE(m_pUnitProgressBGMesh);
   SAFE_RELEASE(m_pUnitProgressPercentMesh);

   SAFE_RELEASE(m_pCombatDisplayModel);
   SAFE_RELEASE(m_pCombatMesh);
   SAFE_RELEASE(m_pCombatSequence);

   for(UINT32 i = 0;i < m_vCombatAnims.size();i ++)
   {
      SAFE_RELEASE(m_vCombatAnims[i] );
   }
   m_vCombatAnims.clear();

   SAFE_RELEASE(m_pBombardmentDisplayModel);
   SAFE_RELEASE(m_pBombardmentMesh);
   SAFE_RELEASE(m_pBombardmentSequence);

   for(UINT32 i = 0;i < m_vBombardmentAnims.size();i ++)
   {
      SAFE_RELEASE(m_vBombardmentAnims[i] );
   }
   m_vBombardmentAnims.clear();

   //Release the texture sequences
   SAFE_RELEASE(m_pNukeExplodeSequence);
   SAFE_RELEASE(m_pNukeFlashSequence);
   SAFE_RELEASE(m_pAMDSExplodeSequence[1]);
   SAFE_RELEASE(m_pAMDSExplodeSequence[0]);

   //Release the Unit Flags
   {
      SAFE_RELEASE(m_pUnitFlagMesh);
      for(UINT32 i = 0 ; i < m_vUnitFlagsTSamplers.size() ; i++)
      {
         SAFE_RELEASE(m_vUnitFlagsTSamplers[i]);
      }
      m_vUnitFlagsTSamplers.clear();
      for(UINT32 i = 0 ; i < EUnitFriendship::Count ; i++)
      {
         SAFE_RELEASE(m_pUnitFlagSelectionMeshes[i]);
         SAFE_RELEASE(m_pUnitFlagSelectionTSamplers[i]);
      }
   }

   SAFE_RELEASE(m_SelectedCountryTrack);

   for (int i = 0; i < 3; i++)
   { 
      SAFE_RELEASE(m_SelectedCountryAnimation[i] );
      SAFE_RELEASE(m_SelectedCountryLinearAnimation[i] );
   }

   //Releases the Model
   GUI::GBaseObject::OnDestroy();

   // Kill Sphere Control Window
   g_ClientDDL.SphereControlWindowKill();

   return true;
}

/*!
* Called when the Background Earth must be displayed
**/
void GBackgroundEarth::OnShow()
{
   g_ClientDAL.SelectCombat(0);
   g_ClientDCL.KillCombatInfo();
   __super::OnShow();
   g_Joshua.Renderer()->RenderTree()->InsertScene(m_pScene, 0);
}

void GBackgroundEarth::OnHide()
{
   __super::OnHide();
   g_Joshua.Renderer()->RenderTree()->RemoveScene(m_pScene);

}

void GBackgroundEarth::ReadConfig(const Hector::GTreeNode<GXMLNode>* in_pTreeNode,
                                  const GString& in_sPrefix)
{
   // Fetch current XML node
   const GXMLNode& l_XMLNode = in_pTreeNode->Data();

   // Add value
   m_pConfigData->insert(make_pair(in_sPrefix + l_XMLNode.m_sName, l_XMLNode.m_value) );

   // Add each attribute
   for(UINT32 a = 0;a < l_XMLNode.m_attributeList.size();a ++)
   {
      (*m_pConfigData)[in_sPrefix + l_XMLNode.m_sName + L">" + l_XMLNode.m_attributeList[a].m_sName] =
         l_XMLNode.m_attributeList[a].m_value;
   }

   // Add each child
   GString l_Prefix = in_sPrefix + l_XMLNode.m_sName + ">>";
   for(UINT32 c = 0;c < in_pTreeNode->NbChilds();c ++)
   {
      ReadConfig(in_pTreeNode->Child(c), l_Prefix);
   }
}

INT32 SP2::GBackgroundEarth::ConfigValue(const GString& in_sParamName,
                                         INT32 in_iDefaultValue)
{
   gassert(m_pConfigData, "Config map is not allocated (integer)");

   hash_map<GString, GString>::iterator l_ConfigIt = m_pConfigData->find(in_sParamName);
   if(l_ConfigIt != m_pConfigData->end() )
   {
      LOG_DIFF_CONFIG(in_sParamName, l_ConfigIt->second.ToINT32(), in_iDefaultValue);
      return l_ConfigIt->second.ToINT32();
   }

   g_Joshua.Log(GString(L"Missing config parameter : ") + in_sParamName);

   return in_iDefaultValue;
}

REAL32 SP2::GBackgroundEarth::ConfigValue(const GString& in_sParamName,
                                          REAL32 in_fDefaultValue)
{
   gassert(m_pConfigData, "Config map is not allocated (floating point)");

   hash_map<GString, GString>::iterator l_ConfigIt = m_pConfigData->find(in_sParamName);
   if(l_ConfigIt != m_pConfigData->end() )
   {
      LOG_DIFF_CONFIG(in_sParamName, l_ConfigIt->second.ToREAL32(), in_fDefaultValue);
      return l_ConfigIt->second.ToREAL32();
   }

#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(L"Missing config parameter : ") + in_sParamName);
#endif

   return in_fDefaultValue;
}

GColorRGBInt SP2::GBackgroundEarth::ConfigValue(const GString& in_sParamName,
                                                GColorRGBInt in_DefaultValue)
{
   gassert(m_pConfigData, "Config map is not allocated (color)");

   hash_map<GString, GString>::iterator l_ConfigIt = m_pConfigData->find(in_sParamName);
   if(l_ConfigIt != m_pConfigData->end() )
   {
      GColorRGBInt l_Value = GUI::GFrame::GetColorFromString(l_ConfigIt->second).ConvertToInt();
      LOG_DIFF_CONFIG(in_sParamName, *( (UINT32*) &l_Value), *( (UINT32*) &in_DefaultValue) );
      return l_Value;
   }

#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(L"Missing config parameter : ") + in_sParamName);
#endif

   return in_DefaultValue;
}

const GString& SP2::GBackgroundEarth::ConfigValue(const GString& in_sParamName,
                                                  const GString& in_sDefaultValue)
{
   gassert(m_pConfigData, "Config map is not allocated (string)");

   hash_map<GString, GString>::iterator l_ConfigIt = m_pConfigData->find(in_sParamName);
   if(l_ConfigIt != m_pConfigData->end() )
   {
      LOG_DIFF_CONFIG(in_sParamName, l_ConfigIt->second, in_sDefaultValue);
      return l_ConfigIt->second;
   }

#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(L"Missing config parameter : ") + in_sParamName);
#endif

   return in_sDefaultValue;
}

void SP2::GBackgroundEarth::ClearConfig()
{
   SAFE_DELETE(m_pConfigData);
}


bool GBackgroundEarth::CreateBackgroundScene()
{
   // Background scene GFX object creation
   {
      // Create scene
      m_pScene = g_Joshua.Renderer()->Create_Scene();

      // Create camera
      m_pCamera = g_Joshua.Renderer()->Create_Camera();
      m_pCameraModel = g_Joshua.Renderer()->Create_Model();
      m_pCameraTransform = g_Joshua.Renderer()->Create_Transform_SRT();

      // Create sun
      m_pSunModel = g_Joshua.Renderer()->Create_Model();
      m_pSunTransform = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pSunLight = g_Joshua.Renderer()->Create_Light();
   }

   // Background scene GFX object initialization
   {
      // Initialize background scene
      m_pScene->GlobalAmbient() = GColorRGBReal(1, 1, 1, 1);

      // Initialize camera
      m_pScene->Set_Active_Camera(m_pCamera);
      m_pScene->Get_Root_Model()->Add_Child(m_pCameraModel);
      m_pCameraModel->Set_Primitive(m_pCamera);
      m_pCameraModel->Set_Transform(m_pCameraTransform);
      m_pCamera->View_Type(GFX::VIEW_PERSPECTIVE);
      m_pCamera->Near_Plane() = 0.01f;

      // Initialize sun
      m_pScene->Get_Root_Model()->Add_Child(m_pSunModel);
      m_pSunModel->Set_Primitive(m_pSunLight);
      m_pSunModel->Set_Transform(m_pSunTransform);
      m_pSunLight->On() = true;
      m_pSunLight->Range() = 100;
      m_pSunLight->Attenuation_Constant() = 1.f;
      m_pSunLight->Attenuation_Linear() = 0.f;
      GColorRGBInt l_SunColor = GUI::GFrame::GetColorFromString(Property(L"SunColor") + COLOR_SEPARATOR + L"255").ConvertToInt();
      m_pSunLight->Color() = GColorRGBReal(l_SunColor);

      if(m_pProgressMeter)
      {
         m_pProgressMeter->PercentageUpdate(5);
         m_pProgressMeter->Split(40);
      }

      // Create background sub-compoments
      IF_RETURN(!CreateRealisticEarth(), false);

      if(m_pProgressMeter)
      {
         m_pProgressMeter->EndSplit();
         m_pProgressMeter->Update();
         m_pProgressMeter->Split(70);
      }

      IF_RETURN(!CreateStrategicEarth(), false);

      if(m_pProgressMeter)
      {
         m_pProgressMeter->EndSplit();
         m_pProgressMeter->Update();
         m_pProgressMeter->Split(100);
      }

      IF_RETURN(!CreateTransitionAnimation(), false);

      IF_RETURN(!CreateBackgroundInfo(), false);

      if(m_pProgressMeter)
      {
         m_pProgressMeter->EndSplit();
         m_pProgressMeter->Update();
      }
   }

   return true;
}

bool GBackgroundEarth::CreateRealisticEarth()
{
   IF_RETURN(!m_pScene, false);

   GFile l_FileHandle;

   // Realistic earth constant read
   {
      m_fCloudFadeStart = ConfigValue(c_sConfigCloudFadeStart, c_fDefaultCloudFadeStart);
      m_fCloudFadeStop = ConfigValue(c_sConfigCloudFadeStop, c_fDefaultCloudFadeStop); 
      m_fCloudFadeDeltaInv = 1.f / (m_fCloudFadeStop - m_fCloudFadeStart);
   }

   // Realistic earth GFX object creation
   {
      // Create realistic earth master model
      m_pRealisticMasterModel = g_Joshua.Renderer()->Create_Model();
      m_pRealisticOverlayModel = g_Joshua.Renderer()->Create_Model();

      // Create flares
      m_pFlareMasterModel = g_Joshua.Renderer()->Create_Model();

      // Create moon
      if(g_Joshua.FileManager()->File(c_MoonModelName, l_FileHandle) )
         m_pMoonModel = g_Joshua.Renderer()->Helper()->LoadModel3ds(l_FileHandle, c_ModelPath, true, true);

      // Create stars
      if(g_Joshua.FileManager()->File(c_StarModelName, l_FileHandle) )
         m_pStarsModel = g_Joshua.Renderer()->Helper()->LoadModel3ds(l_FileHandle, c_ModelPath, false, false);

      // Create earth
      m_pEarthModelMaster = g_Joshua.Renderer()->Create_Model();
      m_pEarthTransformTemp = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pEarthMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      m_pEarthMaterialNoSun = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate(5);

      GString l_MapFileName (L"/data/textures/earth/color/earth_512_01.dds");
      GString l_MapFileNameLow (L"/data/textures/earth/color/earth_256_01.dds");
      GString l_BumpFileName(L"/data/textures/earth/bump/bump_512_01.dds");
      for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
      {
         m_pEarthModel[i] = g_Joshua.Renderer()->Create_Model();
         m_pEarthMesh[i] = g_Joshua.Renderer()->CreateMesh();
         m_pEarthOverride[i] = g_Joshua.Renderer()->CreateOverrideTSampler();

         l_MapFileName.at(37) = (wchar_t) ( (i + 1) / 10) + '0';
         l_MapFileName.at(38) = (wchar_t) ( (i + 1) % 10) + '0';
         m_pEarthTexture[i] = g_Joshua.Renderer()->Helper()->LoadTexture(l_MapFileName);

         l_MapFileNameLow.at(37) = (wchar_t) ( (i + 1) / 10) + '0';
         l_MapFileNameLow.at(38) = (wchar_t) ( (i + 1) % 10) + '0';
         m_pEarthTextureLow[i] = g_Joshua.Renderer()->Helper()->LoadTexture(l_MapFileNameLow);

         m_pDarkOverride[i] = g_Joshua.Renderer()->CreateOverrideTSampler();

         m_pBumpOverride[i] = g_Joshua.Renderer()->CreateOverrideTSampler();

         l_BumpFileName.at(35) = (wchar_t) ( (i + 1) / 10) + '0';
         l_BumpFileName.at(36) = (wchar_t) ( (i + 1) % 10) + '0';
         m_pBumpTexture[i] = g_Joshua.Renderer()->Helper()->LoadTexture(l_BumpFileName);

         if(m_pProgressMeter)
            m_pProgressMeter->PercentageUpdate( (UINT8) (5 + ( (i + 1) * 60 / NB_EARTH_DIVS) ) );
      }

      // Create night
      m_pNightTexture = g_Joshua.Renderer()->Helper()->LoadTexture(c_NightTxtrName);

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate(65);

      // Create water specular
      m_pWaterModel = g_Joshua.Renderer()->Create_Model();
      m_pWaterMesh = g_Joshua.Renderer()->CreateMesh();
      m_pWaterMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      m_pWaterTexture = g_Joshua.Renderer()->Helper()->LoadTexture(c_WaterTxtrName);

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate(70);

      // Create atmospheric halo
      //   m_pAtmoHaloModel = CreateHaloPlane(c_AtmoHaloTxtrName);
      m_pAtmoHaloTransform = g_Joshua.Renderer()->Create_Transform();

      // Create clouds
      m_pCloudModel = g_Joshua.Renderer()->Create_Model();
      m_pCloudMesh = g_Joshua.Renderer()->CreateMesh();
      m_pCloudTransform = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pCloudMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      m_pCloudTexture = g_Joshua.Renderer()->Helper()->LoadTexture(c_CloudTxtrName);

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate(75);
   }

   // Realistic earth GFX object initialization
   {
      // Initialize realistic earth master model
      m_pScene->Get_Root_Model()->Add_Child(m_pRealisticMasterModel);
      m_pScene->Get_Root_Model()->Add_Child(m_pRealisticOverlayModel);

      // Initialize stars Model
      m_pRealisticMasterModel->Add_Child(m_pStarsModel);

      // Initialize stars material
      m_pStarsMaterial = m_pStarsModel->Get_Child(0)->Get_Material();
      m_pStarsMaterial->Inc_Ref();
      m_pStarsMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_CW;
      m_pStarsMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
      m_pStarsMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      m_pStarsMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      m_pStarsMaterial->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);

      ( (GFX::GCascadePixelShader*) m_pStarsMaterial->PixelShader(0) )->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);

      // Initialize stars mesh
      m_pStarsMesh = (GFX::GMesh*) m_pStarsModel->Get_Child(0)->Get_Primitive();
      m_pStarsMesh->Inc_Ref();

      m_pStarsMesh->Shape().ClearAll();
      m_pStarsMesh->NbFacetGroups(0);
      g_Joshua.Renderer()->Helper()->CreateCube(m_pStarsMesh, -50, 1, HELPER_MESH_CREATE_UV);

      GShape& l_StarsShape = m_pStarsMesh->Shape();
      for(UINT32 i = 0;i < l_StarsShape.NbUVCoords(0);i ++)
      {
         l_StarsShape.UVCoord(0, i).x *= 5;
         l_StarsShape.UVCoord(0, i).y *= 5;
      }

      // Initialize sun flare
      m_pFlareModel[0] = CreateFlarePlane(c_SunTxtrName);
      m_pFlareTransform[0] = g_Joshua.Renderer()->Create_Transform();
      m_pFlareModel[0]->Set_Transform(m_pFlareTransform[0] );
      m_pRealisticMasterModel->Add_Child(m_pFlareModel[0] );

      // Initialize other flares
      m_pRealisticOverlayModel->Add_Child(m_pFlareMasterModel);
      for(UINT32 f = 0;f < c_NbFlares;f ++)
      {
         m_pFlareModel[1 + f] = CreateFlarePlane(c_FlareTxtrName[c_FlareTxtrID[f] ] );
         m_pFlareTransform[1 + f] = g_Joshua.Renderer()->Create_Transform();
         m_pFlareModel[1 + f]->Set_Transform(m_pFlareTransform[1 + f] );

         m_pFlareMasterModel->Add_Child(m_pFlareModel[1 + f] );
      }

      // Initialize master earth Model
      m_pRealisticMasterModel->Add_Child(m_pEarthModelMaster);
      m_pEarthModelMaster->Set_Material(m_pEarthMaterial);

      m_pEarthMaterial->NbPasses(3);

      // Initialize earth material
      {
         // Pass 0 : Lit earth (day)
         {
            GFX::GDefaultVertexShader* l_pEarthVS = (GFX::GDefaultVertexShader*) m_pEarthMaterial->VertexShader(0);
            l_pEarthVS->Ambient().R = 0.f;
            l_pEarthVS->Ambient().G = 0.f;
            l_pEarthVS->Ambient().B = 0.f;

            GFX::GCascadePixelShader* l_pEarthPS = (GFX::GCascadePixelShader*) m_pEarthMaterial->PixelShader(0);
            l_pEarthPS->NbStages(2);

            GFX::GCascadeStage* l_pStage = l_pEarthPS->Stage(0);
            l_pStage->ColorArgument1(GFX::ARG_DIFFUSE);
            l_pStage->ColorArgument2(GFX::ARG_DIFFUSE);
            l_pStage->ColorOperation(GFX::OPERATOR_ADD);
            l_pStage->AlphaArgument2(GFX::ARG_DIFFUSE);
            l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);

            l_pStage = l_pEarthPS->Stage(1);
            l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
            l_pStage->ColorArgument2(GFX::ARG_CURRENT);
            l_pStage->ColorOperation(GFX::OPERATOR_MULTIPLY);
            l_pStage->AlphaOperation(GFX::OPERATOR_NONE);
            l_pStage->StageSampler(0);

            l_pEarthPS->TextureSampler(0)->Texture(m_pEarthTextureLow[0] );
            l_pEarthPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
            l_pEarthPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
            l_pEarthPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
         }

         // Pass 1 : Unlit earth (night)
         {
            GColorRGBReal l_NightColor(ConfigValue(c_sConfigNightColor, c_DefaultNightColor) );

            GFX::GRenderStateCollection* l_pDarkRS = m_pEarthMaterial->RenderStates(1);
            GFX::GDefaultVertexShader* l_pDarkVS = g_Joshua.Renderer()->CreateDefaultVertexShader();
            GFX::GCascadePixelShader* l_pDarkPS = g_Joshua.Renderer()->CreateCascadePixelShader();

            m_pEarthMaterial->VertexShader(1, l_pDarkVS);
            m_pEarthMaterial->PixelShader(1, l_pDarkPS);

            l_pDarkRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
            l_pDarkRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
            l_pDarkRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ONE;

            l_pDarkVS->Diffuse() = GColorRGBReal(1.f, 1.f, 1.f, 1.f);
            l_pDarkVS->Ambient() = GColorRGBReal(0.f, 0.f, 0.f, 1.f);

            l_pDarkPS->ColorFactor() = l_NightColor;
            l_pDarkPS->NbStages(2);

            GFX::GCascadeStage* l_pStage = l_pDarkPS->Stage(0);
            l_pStage->ColorArgument1(GFX::ARG_DIFFUSE);
            l_pStage->ColorArgument2(GFX::ARG_DIFFUSE);
            l_pStage->ColorOperation(GFX::OPERATOR_ADD);
            l_pStage->AlphaArgument1(GFX::ARG_TEXTURE);
            l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->StageSampler(0);

            l_pStage = l_pDarkPS->Stage(1);
            l_pStage->ColorArgument1(GFX::ARG_COLOR_FACTOR);
            l_pStage->ColorArgument2(ModifyCascadeArgument(GFX::ARG_CURRENT, GFX::ARG_MOD_COMPLEMENT) );
            l_pStage->ColorOperation(GFX::OPERATOR_MULTIPLY);
            l_pStage->AlphaOperation(GFX::OPERATOR_NONE);

            l_pDarkPS->NbTextureSamplers(1);
            l_pDarkPS->TextureSampler(0)->Texture(m_pEarthTextureLow[0] );
            l_pDarkPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
            l_pDarkPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
            l_pDarkPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

            SAFE_RELEASE(l_pDarkVS);
            SAFE_RELEASE(l_pDarkPS);
         }

         // Pass 2 : Bump earth
         {
            GFX::GRenderStateCollection* l_pBumpRS = m_pEarthMaterial->RenderStates(2);
            GFX::GDefaultVertexShader* l_pBumpVS = g_Joshua.Renderer()->CreateDefaultVertexShader();
            GFX::GCascadePixelShader* l_pBumpPS = g_Joshua.Renderer()->CreateCascadePixelShader();

            m_pEarthMaterial->VertexShader(2, l_pBumpVS);
            m_pEarthMaterial->PixelShader(2, l_pBumpPS);

            l_pBumpRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
            l_pBumpRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
            l_pBumpRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ZERO;

            l_pBumpVS->LightEnable() = false;

            l_pBumpPS->NbStages(1);

            GFX::GCascadeStage* l_pStage = l_pBumpPS->Stage(0);
            l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
            l_pStage->ColorOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->AlphaArgument1(GFX::ARG_DIFFUSE);
            l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->StageSampler(0);

            l_pBumpPS->NbTextureSamplers(1);
            l_pBumpPS->TextureSampler(0)->Texture(m_pBumpTexture[0] );
            l_pBumpPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
            l_pBumpPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
            l_pBumpPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

            SAFE_RELEASE(l_pBumpVS);
            SAFE_RELEASE(l_pBumpPS);
         }
      }

      m_pEarthMaterialNoSun->NbPasses(2);

      // Initialize earth material (no sun)
      {
         // Pass 0 : Completely lit earth
         {
            GFX::GDefaultVertexShader* l_pEarthVS = (GFX::GDefaultVertexShader*) m_pEarthMaterialNoSun->VertexShader(0);
            l_pEarthVS->Ambient().R = 0.f;
            l_pEarthVS->Ambient().G = 0.f;
            l_pEarthVS->Ambient().B = 0.f;

            GFX::GCascadePixelShader* l_pEarthPS = (GFX::GCascadePixelShader*) m_pEarthMaterialNoSun->PixelShader(0);
            l_pEarthPS->NbStages(1);

            GFX::GCascadeStage* l_pStage = l_pEarthPS->Stage(0);
            l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
            l_pStage->ColorArgument2(GFX::ARG_DIFFUSE);
            l_pStage->ColorOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->StageSampler(0);

            l_pEarthPS->TextureSampler(0)->Texture(m_pEarthTextureLow[0] );
            l_pEarthPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
            l_pEarthPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
            l_pEarthPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
         }

         // Pass 1 : Bump earth
         {
            GFX::GRenderStateCollection* l_pBumpRS = m_pEarthMaterialNoSun->RenderStates(1);
            GFX::GDefaultVertexShader* l_pBumpVS = g_Joshua.Renderer()->CreateDefaultVertexShader();
            GFX::GCascadePixelShader* l_pBumpPS = g_Joshua.Renderer()->CreateCascadePixelShader();

            m_pEarthMaterialNoSun->VertexShader(1, l_pBumpVS);
            m_pEarthMaterialNoSun->PixelShader(1, l_pBumpPS);

            l_pBumpRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
            l_pBumpRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
            l_pBumpRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ZERO;

            l_pBumpVS->LightEnable() = false;

            l_pBumpPS->NbStages(1);

            GFX::GCascadeStage* l_pStage = l_pBumpPS->Stage(0);
            l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
            l_pStage->ColorOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->AlphaArgument1(GFX::ARG_DIFFUSE);
            l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);
            l_pStage->StageSampler(0);

            l_pBumpPS->NbTextureSamplers(1);
            l_pBumpPS->TextureSampler(0)->Texture(m_pBumpTexture[0] );
            l_pBumpPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
            l_pBumpPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
            l_pBumpPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

            SAFE_RELEASE(l_pBumpVS);
            SAFE_RELEASE(l_pBumpPS);
         }
      }

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate(80);

      // Initialize earth
      UINT32 l_EarthDivs = Property(L"EarthDivisions").ToINT32();
      UINT32 l_Index = 0;
      for(UINT32 y = 0;y < NB_EARTH_DIVS_Y;y ++)
      {
         REAL32 v = (REAL32)Math::PI * (0.5f - (REAL32) (y + 1) / (REAL32) NB_EARTH_DIVS_Y);
         REAL32 vnext = (REAL32)Math::PI * (0.5f - (REAL32) (y) / (REAL32) NB_EARTH_DIVS_Y);

         for(UINT32 x = 0;x < NB_EARTH_DIVS_X;x ++)
         {
            REAL32 u = 2.f * (REAL32)Math::PI * (REAL32) x / (REAL32) NB_EARTH_DIVS_X;
            REAL32 unext = 2.f * (REAL32)Math::PI * (REAL32) (x + 1) / (REAL32) NB_EARTH_DIVS_X;

            // Setup earth Model
            m_pEarthModelMaster->Add_Child(m_pEarthModel[l_Index] );
            m_pEarthModel[l_Index]->Set_Primitive(m_pEarthMesh[l_Index]);
            m_pEarthModel[l_Index]->Visibility() = false;

            // Initialize earth override
            m_pEarthModel[l_Index]->AddOverride(m_pEarthOverride[l_Index] );
            m_pEarthOverride[l_Index]->ApplyOnPass(0);
            m_pEarthOverride[l_Index]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
            m_pEarthOverride[l_Index]->OriginalTextureSamplerID(0);
            m_pEarthOverride[l_Index]->OverrideTextureSampler()->Texture(m_pEarthTexture[l_Index] );

            // Initialize dark earth override
            m_pEarthModel[l_Index]->AddOverride(m_pDarkOverride[l_Index] );
            m_pDarkOverride[l_Index]->ApplyOnPass(1);
            m_pDarkOverride[l_Index]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
            m_pDarkOverride[l_Index]->OriginalTextureSamplerID(0);
            m_pDarkOverride[l_Index]->OverrideTextureSampler()->Texture(m_pEarthTexture[l_Index] );

            // Initialize bump earth override
            m_pEarthModel[l_Index]->AddOverride(m_pBumpOverride[l_Index] );
            m_pBumpOverride[l_Index]->ApplyOnPass(2);
            m_pBumpOverride[l_Index]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
            m_pBumpOverride[l_Index]->OriginalTextureSamplerID(0);
            m_pBumpOverride[l_Index]->OverrideTextureSampler()->Texture(m_pBumpTexture[l_Index] );

            // Initialize earth mesh
            g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(m_pEarthMesh[l_Index], 1, u, unext, v, vnext, l_EarthDivs, l_EarthDivs, HELPER_MESH_CREATE_ALL );

            m_pEarthCorners[l_Index][0] = 
               ConvertSphericalToRectangular(GVector2D<REAL32>(u - (REAL32) Math::PI, v) * 180.f / (REAL32) Math::PI);
            m_pEarthCorners[l_Index][1] = 
               ConvertSphericalToRectangular(GVector2D<REAL32>(u - (REAL32) Math::PI, vnext) * 180.f / (REAL32) Math::PI);
            m_pEarthCorners[l_Index][2] = 
               ConvertSphericalToRectangular(GVector2D<REAL32>(unext - (REAL32) Math::PI, v) * 180.f / (REAL32) Math::PI);
            m_pEarthCorners[l_Index][3] = 
               ConvertSphericalToRectangular(GVector2D<REAL32>(unext - (REAL32) Math::PI, vnext) * 180.f / (REAL32) Math::PI);
            m_pEarthCorners[l_Index][4] = 
               ConvertSphericalToRectangular(GVector2D<REAL32>( 0.5f * (u + unext) - (REAL32) Math::PI, 0.5f * (v + vnext) ) * 180.f / (REAL32) Math::PI);

            l_Index ++;
         }

         if(m_pProgressMeter)
            m_pProgressMeter->PercentageUpdate( (UINT8) (80 + ( (y + 1) * 15 / NB_EARTH_DIVS_Y) ) );
      }

      // Initialize water specular Model
      m_pEarthModelMaster->Add_Child(m_pWaterModel);
      m_pWaterModel->Set_Material(m_pWaterMaterial);
      m_pWaterModel->Set_Primitive(m_pWaterMesh);

      // Initialize night material
      GFX::GMaterial* l_pNightTempMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      m_pWaterMaterial->NbPasses(2);
      m_pWaterMaterial->VertexShader(1, m_pWaterMaterial->VertexShader(0) );
      m_pWaterMaterial->PixelShader(1, m_pWaterMaterial->PixelShader(0) );

      m_pWaterMaterial->VertexShader(0, l_pNightTempMaterial->VertexShader(0) );
      m_pWaterMaterial->PixelShader(0, l_pNightTempMaterial->PixelShader(0) );
      l_pNightTempMaterial->Release();

      m_pWaterMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_ENABLE) = FALSE;
      m_pWaterMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = FALSE;
      m_pWaterMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
      m_pWaterMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      m_pWaterMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ONE;

      GFX::GDefaultVertexShader* l_pNightVS = (GFX::GDefaultVertexShader*) m_pWaterMaterial->VertexShader(0);
      l_pNightVS->Diffuse().R = 1.f;
      l_pNightVS->Diffuse().G = 1.f;
      l_pNightVS->Diffuse().B = 1.f;
      l_pNightVS->Ambient().R = 0.f;
      l_pNightVS->Ambient().G = 0.f;
      l_pNightVS->Ambient().B = 0.f;

      GFX::GCascadePixelShader* l_pNightPS = (GFX::GCascadePixelShader*) m_pWaterMaterial->PixelShader(0);
      /*
      l_pNightPS->NbStages(2);
      l_pNightPS->Stage(0)->ColorOperation(GFX::OPERATOR_SUBTRACT);
      l_pNightPS->Stage(0)->ColorArgument1(ModifyCascadeArgument(GFX::ARG_DIFFUSE, GFX::ARG_MOD_COMPLEMENT) );
      l_pNightPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);

      l_pNightPS->Stage(1)->ColorOperation(GFX::OPERATOR_MULTIPLY_4X);
      l_pNightPS->Stage(1)->ColorArgument1(GFX::ARG_TEXTURE);
      l_pNightPS->Stage(1)->ColorArgument2(GFX::ARG_CURRENT);
      l_pNightPS->Stage(1)->StageSampler(0);
      l_pNightPS->Stage(1)->TextureCoordIndex(0);

      l_pNightPS->ColorFactor().R = 0.75f;
      l_pNightPS->ColorFactor().G = 0.75f;
      l_pNightPS->ColorFactor().B = 0.75f;
      l_pNightPS->ColorFactor().A = 1.f;
      */
      l_pNightPS->ColorFactor() = GColorRGBReal(1.f, 1.f, 1.f, 1.f);
      l_pNightPS->NbStages(2);

      GFX::GCascadeStage* l_pStage = l_pNightPS->Stage(0);
      l_pStage->ColorArgument1(GFX::ARG_DIFFUSE);
      l_pStage->ColorArgument2(GFX::ARG_COLOR_FACTOR);
      l_pStage->ColorOperation(GFX::OPERATOR_MULTIPLY_2X);

      l_pStage = l_pNightPS->Stage(1);
      l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
      l_pStage->ColorArgument2(ModifyCascadeArgument(GFX::ARG_CURRENT, GFX::ARG_MOD_COMPLEMENT) );
      l_pStage->ColorOperation(GFX::OPERATOR_MULTIPLY);
      l_pStage->StageSampler(0);

      l_pNightPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
      l_pNightPS->Stage(0)->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);
      l_pNightPS->Stage(1)->AlphaOperation(GFX::OPERATOR_NONE);

      GFX::GTextureSampler* l_pNightTS = m_pWaterMaterial->PixelShader(0)->TextureSampler(0);
      l_pNightTS->Texture(m_pNightTexture);
      l_pNightTS->MagnificationFilter(GFX::FILTER_LINEAR);

      // Initialize water specular material
      m_pWaterMaterial->RenderStates(1)->Value(GFX::RSTATE_ZBUFFER_ENABLE) = FALSE;
      m_pWaterMaterial->RenderStates(1)->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = FALSE;
      m_pWaterMaterial->RenderStates(1)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
      m_pWaterMaterial->RenderStates(1)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      m_pWaterMaterial->RenderStates(1)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ONE;

      GFX::GDefaultVertexShader* l_pWaterVS = (GFX::GDefaultVertexShader*) m_pWaterMaterial->VertexShader(1);
      l_pWaterVS->Ambient().R = 0.f;
      l_pWaterVS->Ambient().G = 0.f;
      l_pWaterVS->Ambient().B = 0.f;
      l_pWaterVS->Ambient().A = 0.f;
      l_pWaterVS->Diffuse() = l_pWaterVS->Ambient();
      l_pWaterVS->Emissive() = l_pWaterVS->Ambient();
      l_pWaterVS->Specular().R = 0.75f;
      l_pWaterVS->Specular().G = 0.75f;
      l_pWaterVS->Specular().B = 0.75f;
      l_pWaterVS->SpecularEnable() = true;
      l_pWaterVS->Power() = 50.0f;

      GFX::GCascadePixelShader* l_pWaterPS = (GFX::GCascadePixelShader*) m_pWaterMaterial->PixelShader(1);
      l_pWaterPS->Stage(0)->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);

      GFX::GTextureSampler* l_pWaterTS = m_pWaterMaterial->PixelShader(1)->TextureSampler(0);
      l_pWaterTS->Texture(m_pWaterTexture);
      l_pWaterTS->MagnificationFilter(GFX::FILTER_LINEAR);

      // Initialize water specular mesh
      g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(m_pWaterMesh, 1.f, 0.f, (REAL32)Math::PI * 2.f, (REAL32)-Math::PI*0.5f, (REAL32)Math::PI*0.5f, 64, 32, HELPER_MESH_CREATE_ALL);

      // Initialize atmospheric halo transform
      //   m_pRealisticMasterModel->Add_Child(m_pAtmoHaloModel);
      //   m_pAtmoHaloModel->Set_Transform(m_pAtmoHaloTransform);
      m_pAtmoHaloTransform->Get_Matrix().LoadIdentity();

      Memory::Set(m_iAtlasID, 0xFF, 2);

      m_pAtlasEarth = new GAtlasEarth(this, 
         l_EarthDivs * NB_EARTH_DIVS_X, 
         l_EarthDivs * NB_EARTH_DIVS_Y, 
         ConfigValue(c_sConfigAtlasFadeTime, c_fDefaultAtlasFadeTime) );

      // Initialize cloud Model
      m_pRealisticOverlayModel->Add_Child(m_pCloudModel);
      m_pCloudModel->Set_Material(m_pCloudMaterial);
      m_pCloudModel->Set_Transform(m_pCloudTransform);
      m_pCloudModel->Set_Primitive(m_pCloudMesh);

      // Initialize cloud material
      GFX::GDefaultVertexShader* l_pCloudVS = (GFX::GDefaultVertexShader*) m_pCloudMaterial->VertexShader(0);
      l_pCloudVS->Set_Animated();
      l_pCloudVS->Ambient().R = 0.f;
      l_pCloudVS->Ambient().G = 0.f;
      l_pCloudVS->Ambient().B = 0.f;
      l_pCloudVS->Ambient().A = 0.f;

      GFX::GRenderStateCollection* l_pRenderStates = m_pCloudMaterial->RenderStates(0);
      l_pRenderStates->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_pRenderStates->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
      l_pRenderStates->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pRenderStates->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pRenderStates->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      l_pRenderStates->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = true;
      l_pRenderStates->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
      l_pRenderStates->Value(GFX::RSTATE_ALPHA_TEST_FUNCTION) = GFX::TEST_GREATEREQUAL;

      // Initialize cloud shader
      GFX::GCascadePixelShader* l_pCloudPS = (GFX::GCascadePixelShader*) m_pCloudMaterial->PixelShader(0);
      l_pCloudPS->TextureSampler(0)->Texture(m_pCloudTexture);
      l_pCloudPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);

      l_pCloudPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);

      // Initialize cloud mesh
      g_Joshua.Renderer()->Helper()->CreateSphere(m_pCloudMesh, 1.f + 1.f / 256.f, 32, 32, HELPER_MESH_CREATE_ALL);

      // Initialize moon Model
      m_pRealisticOverlayModel->Add_Child(m_pMoonModel);
      m_pMoonTransform = m_pMoonModel->Get_Child(0)->Get_Transform();
      m_pMoonTransform->Inc_Ref();
      UpdateMoon();

      // Initialize moon material
      m_pMoonMaterial = m_pMoonModel->Get_Child(0)->Get_Material();
      m_pMoonMaterial->PixelShader(0)->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      m_pMoonMaterial->Inc_Ref();

      GFX::GDefaultVertexShader* l_pMoonVS = (GFX::GDefaultVertexShader*) m_pMoonMaterial->VertexShader(0);
      l_pMoonVS->Ambient().R = 0.f;
      l_pMoonVS->Ambient().G = 0.f;
      l_pMoonVS->Ambient().B = 0.f;

      m_pRealisticOverlayModel->Bring_To_Front(m_pFlareMasterModel);
   }

   return true;
}

bool GBackgroundEarth::CreateStrategicEarth()
{
   IF_RETURN(!m_pScene, false);

   // Strategic warfare earth GFX object creation
   {
      // Create strategic warfare earth master model
      m_pStrategicMasterModel = g_Joshua.Renderer()->Create_Model();

      // Create strategic earth
      m_pStrategicEarthModelMaster = g_Joshua.Renderer()->Create_Model();
      m_pStrategicEarthMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);

      GString l_sGrayFileName (L"/data/textures/earth/gray/");
      GString l_FileName(L"AA");
      UINT32 l_iIndex = 0;
      for(UINT32 y = 0;y < NB_EARTH_DIVS_Y;y ++)
      {
         for(UINT32 x = 0;x < NB_EARTH_DIVS_X;x ++)
         {
            m_pStrategicEarthModels[l_iIndex] = g_Joshua.Renderer()->Create_Model();
            m_pStrategicEarthOverrides[l_iIndex] = g_Joshua.Renderer()->CreateOverrideTSampler();
            m_pStrategicEarthBumpOverrides[l_iIndex] = g_Joshua.Renderer()->CreateOverrideTSampler();

            l_FileName.at(0) = L'A' + (WCHAR) y;
            l_FileName.at(1) = L'A' + (WCHAR) x;
            GString l_sTxtrName = l_sGrayFileName + l_FileName + ".DDS";
            m_pStrategicEarthTextures[l_iIndex] = g_Joshua.Renderer()->Helper()->LoadTexture(l_sTxtrName);
            l_iIndex ++;
         }
      }

      // Create strategic earth lines
      m_pStrategicEarthLinesModel = g_Joshua.Renderer()->Create_Model();
      m_pStrategicEarthLines = g_Joshua.Renderer()->CreateMesh();
      m_pStrategicEarthLinesMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_FACTOR);
   }

   // Strategic warfare earth GFX object initialization
   {
      m_pScene->Get_Root_Model()->Add_Child(m_pStrategicMasterModel);
      m_pStrategicMasterModel->Visibility() = false;

      m_pStrategicMasterModel->Add_Child(m_pStrategicEarthModelMaster);
      m_pStrategicEarthModelMaster->Set_Material(m_pStrategicEarthMaterial);

      GFX::GRenderStateCollection* l_pRS = m_pStrategicEarthMaterial->RenderStates(0);
      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      GFX::GDefaultVertexShader* l_pVS = (GFX::GDefaultVertexShader*) m_pStrategicEarthMaterial->VertexShader(0);
      l_pVS->Diffuse() = GColorRGBReal(ConfigValue(c_sConfigStrategicDiffuseColor, c_DefaultStrategicDiffuseColor) );
      l_pVS->Ambient() = GColorRGBReal(ConfigValue(c_sConfigStrategicAmbientColor, c_DefaultStrategicAmbientColor) );

      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) m_pStrategicEarthMaterial->PixelShader(0);

      l_pPS->NbTextureSamplers(2);

      l_pPS->TextureSampler(0)->Texture(m_pStrategicEarthTextures[0] );
      l_pPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_pPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_pPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

      l_pPS->TextureSampler(1)->Texture(m_pBumpTexture[0] );
      l_pPS->TextureSampler(1)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_pPS->TextureSampler(1)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_pPS->TextureSampler(1)->AddressingModeV(GFX::ADDRESS_CLAMP);

      l_pPS->NbStages(2);

      l_pPS->Stage(0)->ColorArgument1(GFX::ARG_TEXTURE);
      l_pPS->Stage(0)->ColorArgument2(GFX::ARG_DIFFUSE);
      l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_MULTIPLY);

      l_pPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
      l_pPS->Stage(0)->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);

      l_pPS->Stage(1)->ColorArgument1(GFX::ARG_TEXTURE);
      l_pPS->Stage(1)->ColorArgument2(GFX::ARG_CURRENT);
      l_pPS->Stage(1)->ColorOperation(GFX::OPERATOR_MULTIPLY);
      l_pPS->Stage(1)->StageSampler(1);
      l_pPS->Stage(1)->TextureCoordIndex(0);

      l_pPS->Stage(1)->AlphaOperation(GFX::OPERATOR_NONE);

      for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
      {
         m_pStrategicEarthModelMaster->Add_Child(m_pStrategicEarthModels[i] );
         m_pStrategicEarthModels[i]->AddOverride(m_pStrategicEarthOverrides[i] );
         m_pStrategicEarthModels[i]->AddOverride(m_pStrategicEarthBumpOverrides[i] );
         m_pStrategicEarthModels[i]->Set_Primitive(m_pEarthMesh[i] );

         m_pStrategicEarthOverrides[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pStrategicEarthOverrides[i]->OriginalTextureSamplerID(0);
         m_pStrategicEarthOverrides[i]->OverrideTextureSampler()->Texture(m_pStrategicEarthTextures[i] );

         m_pStrategicEarthBumpOverrides[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pStrategicEarthBumpOverrides[i]->OriginalTextureSamplerID(1);
         m_pStrategicEarthBumpOverrides[i]->OverrideTextureSampler()->Texture(m_pBumpTexture[i] );
      }

      m_pStrategicEarthModelMaster->Add_Child(m_pStrategicEarthLinesModel);
      m_pStrategicEarthLinesModel->Set_Material(m_pStrategicEarthLinesMaterial);
      m_pStrategicEarthLinesModel->Set_Primitive(m_pStrategicEarthLines);

      l_pRS = m_pStrategicEarthLinesMaterial->RenderStates(0);
      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_INVDESTCOLOR;
      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      l_pPS = (GFX::GCascadePixelShader*) m_pStrategicEarthLinesMaterial->PixelShader(0);
      l_pPS->ColorFactor() = GColorRGBReal(ConfigValue(c_sConfigStrategicLineColor, c_DefaultStrategicLineColor) );
      l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_MULTIPLY);
      l_pPS->Stage(0)->ColorArgument1(ModifyCascadeArgument(GFX::ARG_COLOR_FACTOR, GFX::ARG_MOD_ALPHA_REPLICATE) );

      UINT32 l_iNbSegments = ConfigValue(c_sConfigStrategicNbSegments, c_iDefaultStrategicNbSegments);
      UINT32 l_iNbLinesX = ConfigValue(c_sConfigStrategicNbLinesX, c_iDefaultStrategicNbLinesX);
      UINT32 l_iNbLinesY = ConfigValue(c_sConfigStrategicNbLinesY, c_iDefaultStrategicNbLinesY);
      REAL32 l_fLineOffset = ConfigValue(c_sConfigStrategicLineOffset, c_fDefaultStrategicLineOffset);

      UINT32 l_iSegmentCountX = (l_iNbLinesY - 1) * (UINT32) ceilf( (REAL32) l_iNbSegments / (REAL32) (2 * (l_iNbLinesY + 1) ) );
      UINT32* l_pSegmentCountY = new UINT32[l_iNbLinesY];

      UINT32 l_iPointCount = 0;
      for(UINT32 x = 0;x < l_iNbLinesX;x ++)
      {
         l_iPointCount += (l_iSegmentCountX + 1);
      }

      for(UINT32 y = 0;y < l_iNbLinesY;y ++)
      {
         REAL32 l_fTheta = (REAL32) Math::PI * (0.5f - (REAL32) (y + 1) / (REAL32) (l_iNbLinesY + 1) );

         l_pSegmentCountY[y] = l_iNbLinesX * (UINT32) ceilf( (REAL32) l_iNbSegments * cosf(l_fTheta) / (REAL32) l_iNbLinesX);
         l_iPointCount += (l_pSegmentCountY[y] + 1);
      }

      GShape& l_Shape = m_pStrategicEarthLines->Shape();
      l_Shape.NbPositions(l_iPointCount);
      UINT32 l_iPointIndex = 0;

      m_pStrategicEarthLines->NbFacetGroups(l_iNbLinesX + l_iNbLinesY);
      UINT32 l_iFaceIndex = 0;

      REAL32 l_fStartPhi = (REAL32) Math::PI * (0.5f - 1.f / (REAL32) (l_iNbLinesY + 1) );
      REAL32 l_fRangePhi = -2 * l_fStartPhi;
      for(UINT32 x = 0;x < l_iNbLinesX;x ++)
      {
         REAL32 l_fTheta = 2.f * (REAL32) Math::PI * (REAL32) x / (REAL32) l_iNbLinesX;
         REAL32 l_fSinTheta = sinf(l_fTheta);
         REAL32 l_fCosTheta = cosf(l_fTheta);

         UINT32 l_iFirstPointIndex = l_iPointIndex;

         for(UINT32 i = 0;i <= l_iSegmentCountX;i ++)
         {
            REAL32 l_fPhi = l_fStartPhi + l_fRangePhi * (REAL32) i / (REAL32) l_iSegmentCountX;

            GVector3D<REAL32>& l_Point = l_Shape.Position(l_iPointIndex);

            REAL32 l_fCosPhi = cosf(l_fPhi);
            l_Point.x = l_fCosTheta * l_fCosPhi * l_fLineOffset;
            l_Point.y = sinf(l_fPhi) * l_fLineOffset;
            l_Point.z = l_fSinTheta * l_fCosPhi * l_fLineOffset;

            l_iPointIndex ++;
         }

         m_pStrategicEarthLines->FacetGroup(l_iFaceIndex).Facets().Type(FACETS_LINE_STRIP, false);
         GLineStrip& l_Line = m_pStrategicEarthLines->FacetGroup(l_iFaceIndex).Facets().LineStrip();

         l_Line.Start() = l_iFirstPointIndex;
         l_Line.NbLines(l_iSegmentCountX);

         l_iFaceIndex ++;
      }

      for(UINT32 y = 0;y < l_iNbLinesY;y ++)
      {
         REAL32 l_fPhi = (REAL32) Math::PI * (0.5f - (REAL32) (y + 1) / (REAL32) (l_iNbLinesY + 1) );

         REAL32 l_fSinPhi = sinf(l_fPhi);
         REAL32 l_fCosPhi = cosf(l_fPhi);

         UINT32 l_iFirstPointIndex = l_iPointIndex;

         for(UINT32 i = 0;i < l_pSegmentCountY[y];i ++)
         {
            REAL32 l_fTheta = 2.f * (REAL32) Math::PI * (REAL32) i / (REAL32) l_pSegmentCountY[y];

            GVector3D<REAL32>& l_Point = l_Shape.Position(l_iPointIndex);

            l_Point.x = cosf(l_fTheta) * l_fCosPhi * l_fLineOffset;
            l_Point.y = l_fSinPhi * l_fLineOffset;
            l_Point.z = sinf(l_fTheta) * l_fCosPhi * l_fLineOffset;

            l_iPointIndex ++;
         }

         l_Shape.Position(l_iPointIndex) = l_Shape.Position(l_iFirstPointIndex);
         l_iPointIndex ++;

         m_pStrategicEarthLines->FacetGroup(l_iFaceIndex).Facets().Type(FACETS_LINE_STRIP, false);
         GLineStrip& l_Line = m_pStrategicEarthLines->FacetGroup(l_iFaceIndex).Facets().LineStrip();

         l_Line.Start() = l_iFirstPointIndex;
         l_Line.NbLines(l_pSegmentCountY[y] );

         l_iFaceIndex ++;
      }

      SAFE_DELETE_ARRAY(l_pSegmentCountY);
   }

   return true;
}

bool GBackgroundEarth::CreateBackgroundInfo()
{
   IF_RETURN(!m_pScene, false);

   // Earth information display GFX object creation
   {
      // Create fill for earth
      m_pFillModel = g_Joshua.Renderer()->Create_Model();
      m_pFillModelStrategic = g_Joshua.Renderer()->Create_Model();
      m_pFillMesh = g_Joshua.Renderer()->CreateMesh();

      // Create information particles for earth
      m_pDisplayParticlesModelMaster = g_Joshua.Renderer()->Create_Model();
      m_pDisplayParticlesMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      m_pDisplayParticlesMesh = g_Joshua.Renderer()->CreateMesh();

      // Create country border
      m_pOutlineModel = g_Joshua.Renderer()->Create_Model();
      m_pOutlineMesh = g_Joshua.Renderer()->CreateMesh();
   }

   // Earth information display GFX object initialization
   {
      // Initialize master fill Model
      m_pEarthModelMaster->Add_Child(m_pFillModel);
      m_pStrategicMasterModel->Add_Child(m_pFillModelStrategic);
      m_pFillModel->Set_Primitive(m_pFillMesh);
      m_pFillModelStrategic->Set_Primitive(m_pFillMesh);

      // Open the region display information file
      GFile l_RegionDataFile;
      if(g_Joshua.FileManager()->File(c_RegionDataFileName, l_RegionDataFile) )
      {
         l_RegionDataFile.Open();

         //Check the config file if we must load the regions or not
         GString l_sTemp;
         bool l_bLoadRegionTextures = true;
         if(g_ClientDAL.XMLValue(SP2::LOAD_REGION_TEXTURES,l_sTemp))
         {
            l_bLoadRegionTextures = (l_sTemp.ToINT32()? true : false);
         }

         // Read the number of regions to display
         UINT32 l_iRegionCount = 0;
         if(l_bLoadRegionTextures)
         {
            l_RegionDataFile.Read(&l_iRegionCount, sizeof(UINT32) );
         }

         // Reset region ref counts to 0x80
         m_vRegionRefCount.resize(l_iRegionCount, -128);
         m_vRegionVisibilityParams.resize(l_iRegionCount, 0);
         m_vRegionColorParams.resize(l_iRegionCount, 0);

         // Read the resolution of the region display
         GVector2D<UINT32> l_RegionRes;
         l_RegionDataFile.Read(&l_RegionRes, sizeof(GVector2D<UINT32>) );

         // Calc the resolution of a single earth subdivision
         UINT32 l_EarthDivs = Property(L"EarthDivisions").ToINT32();
         GVector2D<UINT32> l_SubDivs;
         l_SubDivs.x = NB_EARTH_DIVS_X * l_EarthDivs;
         l_SubDivs.y = NB_EARTH_DIVS_Y * l_EarthDivs;

         // Create a temporary mesh with a complete sphere
         GFX::GMesh* l_pSphereMesh = g_Joshua.Renderer()->CreateMesh();
         g_Joshua.Renderer()->Helper()->CreateSphere(l_pSphereMesh, 1, l_SubDivs.x, l_SubDivs.y, HELPER_MESH_CREATE_ALL);
         GTriangleListIndexed& l_SphereTris = l_pSphereMesh->FacetGroup(0).Facets().TriangleListIndexed();

         // Copy sphere shape to the regions mesh
         GShape& l_FillShape = m_pFillMesh->Shape();
         l_FillShape = l_pSphereMesh->Shape();

         // Create material for region
         GFX::GCascadePixelShader* l_pRegionPS;
         GFX::GMaterial* l_pRegionMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR, &m_pFillVS, &l_pRegionPS);
         m_pFillModel->Set_Material(l_pRegionMat);
         m_pFillModelStrategic->Set_Material(l_pRegionMat);

         // Initialize region material
         GFX::GRenderStateCollection* l_RegionStates = l_pRegionMat->RenderStates(0);

         l_RegionStates->Value(GFX::RSTATE_ZBUFFER_ENABLE) = FALSE;

         l_RegionStates->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = TRUE;
         l_RegionStates->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
         l_RegionStates->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

         l_RegionStates->Value(GFX::RSTATE_ALPHA_TEST_ENABLE) = TRUE;
         l_RegionStates->Value(GFX::RSTATE_ALPHA_TEST_REFERENCE) = 16;
         l_RegionStates->Value(GFX::RSTATE_ALPHA_TEST_FUNCTION) = GFX::TEST_GREATEREQUAL;

         m_pFillVS->DiffuseSource(GFX::SOURCE_MATERIAL);
         m_pFillVS->Set_Animated(GFX::PARAM_DIFFUSE_A);

         l_pRegionPS->ColorFactor() = GColorRGBReal(0,0,1,1);
         l_pRegionPS->TextureSampler(0)->Texture(m_pEarthTexture[0] );
         l_pRegionPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
         l_pRegionPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
         l_pRegionPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

         // Modify region material shader
         l_pRegionPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
         l_pRegionPS->Stage(0)->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);
         l_pRegionPS->Stage(0)->AlphaArgument2(GFX::ARG_DIFFUSE);
         l_pRegionPS->Stage(0)->AlphaOperation(GFX::OPERATOR_MULTIPLY);

         // Create a facet for each region
         m_pFillMesh->NbFacetGroups(l_iRegionCount);

         // Initialize each region display
         for(UINT32 r = 0;r < l_iRegionCount;r ++)
         {
            // Read current region information
            GVector2D<UINT32> l_RegionPos;
            l_RegionDataFile.Read(&l_RegionPos, sizeof(GVector2D<UINT32>) );

            m_vRegionVisibilityParams[r] = &(m_pFillMesh->FacetGroup(r).Visibility() );
            *(m_vRegionVisibilityParams[r] ) = 0;

            // Create a constant material override for region
            GFX::GOverridePSCascade* l_pPSOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
            l_pPSOverride->UseColorFactorOverride(true);
            l_pPSOverride->ColorFactorOverride() = GColorRGBReal(1, 0, 1, 1);
            m_pFillMesh->FacetGroup(r).AddOverride(l_pPSOverride);

            m_vRegionColorParams[r] = &(l_pPSOverride->ColorFactorOverride() );

            if( (l_RegionPos.x != 0xFFFFFFFF) && (l_RegionPos.y != 0xFFFFFFFF) && (r != 111) )
            {
               // Create a texture shader override for region
               GFX::GOverrideTSampler* l_pTSOverride = g_Joshua.Renderer()->CreateOverrideTSampler();
               l_pTSOverride->OriginalTextureSamplerID(0);
               m_pFillMesh->FacetGroup(r).AddOverride(l_pTSOverride);

               // Load texture for region
               GString l_RegionTxtFile = GString(c_RegionTxtrName) + GString(r) + GString(c_RegionTxtrExt);
               CGFX_Texture_Itf* l_pRegionTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(l_RegionTxtFile);
               GVector2D<UINT32> l_RegionSize(l_pRegionTxtr->Width(), l_pRegionTxtr->Height() );

               l_pTSOverride->OverrideTextureSampler()->Texture(l_pRegionTxtr);
               l_pTSOverride->AddOverrides(GFX::TSOVERRIDE_TEXTURE);

               if(l_RegionPos.y + l_RegionSize.y > l_RegionRes.y)
               {
                  l_RegionSize.y = l_RegionRes.y - l_RegionPos.y;
               }

               // Initialize region facet
               UINT32 l_YEnd = l_SubDivs.y - (l_RegionPos.y * l_SubDivs.y / l_RegionRes.y);
               UINT32 l_YStart = l_SubDivs.y - ( ( (l_RegionPos.y + l_RegionSize.y) * l_SubDivs.y + l_RegionRes.y - 1) / l_RegionRes.y);

               UINT32 l_XStart = l_RegionPos.x * l_SubDivs.x / l_RegionRes.x;
               l_XStart = (l_XStart + l_SubDivs.x / 2) % l_SubDivs.x;
               UINT32 l_XEnd = ( (l_RegionPos.x + l_RegionSize.x) * l_SubDivs.x + l_RegionRes.x - 1) / l_RegionRes.x;
               l_XEnd = (l_XEnd + l_SubDivs.x / 2) % l_SubDivs.x;

               if(l_YEnd > l_SubDivs.y)
               {
                  l_YEnd = l_SubDivs.y;
               }

               if(l_XEnd < l_XStart)
               {
                  l_XEnd += l_SubDivs.x;
               }

               GTriangleListIndexed& l_RegionTris = m_pFillMesh->FacetGroup(r).Facets().TriangleListIndexed();
               l_RegionTris.NbTriangles( (l_YEnd - l_YStart) * (l_XEnd - l_XStart) * 2);
               UINT32 l_iTriIndex = 0;
               for(UINT32 y = l_YStart;y < l_YEnd;y ++)
               {
                  for(UINT32 x = l_XStart;(x < l_XEnd) && (x < l_SubDivs.x);x ++)
                  {
                     l_RegionTris.Triangle(l_iTriIndex) = l_SphereTris.Triangle( (y * l_SubDivs.x + x) * 2);
                     l_RegionTris.Triangle(l_iTriIndex + 1) = l_SphereTris.Triangle( (y * l_SubDivs.x + x) * 2 + 1);

                     l_iTriIndex += 2;
                  }

                  for(UINT32 x = l_SubDivs.x;x < l_XEnd;x ++)
                  {
                     GTriangleIndex l_NonOverlappedTri = l_SphereTris.Triangle( (y * l_SubDivs.x + x - l_SubDivs.x) * 2);
                     GTriangleIndex l_OverlappedTri = l_NonOverlappedTri;

                     for(UINT32 t = 0;t < 3;t ++)
                     {
                        l_OverlappedTri.m_pVertices[t] = l_FillShape.AddPosition(l_FillShape.Position(l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.AddNormal(l_FillShape.Normal(l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.AddUVCoord(0, l_FillShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.UVCoord(0, l_OverlappedTri.m_pVertices[t] ).x += 1;
                     }

                     l_RegionTris.Triangle(l_iTriIndex) = l_OverlappedTri;

                     l_NonOverlappedTri = l_SphereTris.Triangle( (y * l_SubDivs.x + x - l_SubDivs.x) * 2 + 1);

                     for(UINT32 t = 0;t < 3;t ++)
                     {
                        l_OverlappedTri.m_pVertices[t] = l_FillShape.AddPosition(l_FillShape.Position(l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.AddNormal(l_FillShape.Normal(l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.AddUVCoord(0, l_FillShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] ) );
                        l_FillShape.UVCoord(0, l_OverlappedTri.m_pVertices[t] ).x += 1;
                     }

                     l_RegionTris.Triangle(l_iTriIndex + 1) = l_OverlappedTri;

                     l_iTriIndex += 2;
                  }
               }

               // Initialize region texture transform
               CGFX_Transform_2D_Itf* l_pRegionTrans = g_Joshua.Renderer()->Create_Transform_2D();

               GVector2D<REAL32> l_Start;
               l_Start.x = (REAL32) ( (l_RegionPos.x + l_RegionRes.x / 2) % l_RegionRes.x) / (REAL32) l_RegionRes.x;
               l_Start.y = (REAL32) (l_RegionPos.y + l_RegionSize.y) / (REAL32) l_RegionRes.y;

               GVector2D<REAL32> l_End;
               l_End.x = (REAL32) ( (l_RegionPos.x + l_RegionSize.x + l_RegionRes.x / 2) % l_RegionRes.x) / (REAL32) l_RegionRes.x;
               l_End.y = (REAL32) (l_RegionPos.y) / (REAL32) l_RegionRes.y;

               if(l_End.x < l_Start.x)
                  l_End.x += 1;

               l_pRegionTrans->Position().X = l_Start.x / (l_Start.x - l_End.x);
               l_pRegionTrans->Position().Y = 1 - l_Start.y / (l_Start.y - l_End.y);
               l_pRegionTrans->Size().X = -1.f / (l_Start.x - l_End.x);
               l_pRegionTrans->Size().Y = 1.f / (l_Start.y - l_End.y);

               l_pPSOverride->AddUVTransformOverride(0, l_pRegionTrans);

               // Associate this region to all earth divisions that it touches
               GVector2D<UINT32> l_StartDiv, l_EndDiv;
               l_StartDiv.x = l_RegionPos.x * NB_EARTH_DIVS_X / l_RegionRes.x;
               l_StartDiv.y = l_RegionPos.y * NB_EARTH_DIVS_Y / l_RegionRes.y;
               l_EndDiv.x = (l_RegionPos.x + l_RegionSize.x - 1) * NB_EARTH_DIVS_X / l_RegionRes.x;
               l_EndDiv.y = (l_RegionPos.y + l_RegionSize.y - 1) * NB_EARTH_DIVS_Y / l_RegionRes.y;

               for(UINT32 y = l_StartDiv.y;y <= l_EndDiv.y;y ++)
               {
                  vector<UINT32>* l_pLine = m_pEarthRegions + y * NB_EARTH_DIVS_X;
                  for(UINT32 x = l_StartDiv.x;x <= l_EndDiv.x;x ++)
                  {
                     l_pLine[x % NB_EARTH_DIVS_X].push_back(r);
                  }
               }

               SAFE_RELEASE(l_pTSOverride);

               SAFE_RELEASE(l_pRegionTrans);
               SAFE_RELEASE(l_pRegionTxtr);
            }
            SAFE_RELEASE(l_pPSOverride);
         }

         SAFE_RELEASE(l_pRegionMat);
         SAFE_RELEASE(l_pRegionPS);
         SAFE_RELEASE(l_pSphereMesh);

         l_RegionDataFile.Close();
      }

      // Load information particles texture
      CGFX_Texture_Itf* l_pParticleTxtr = g_Joshua.Renderer()->Helper()->LoadTexture(c_ParticleTxtrName);

      // Initialize information particles material
      GFX::GDefaultVertexShader* l_pParticlesVS = (GFX::GDefaultVertexShader*) m_pDisplayParticlesMaterial->VertexShader(0);
      l_pParticlesVS->Diffuse() = GColorRGBReal(0,0,0,1);
      l_pParticlesVS->Specular() = GColorRGBReal(0,0,0,1);
      l_pParticlesVS->Emissive() = GColorRGBReal(0,1,0,1);
      l_pParticlesVS->Ambient() = GColorRGBReal(0,0,0,1);

      m_pDisplayParticlesMaterial->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;

      m_pDisplayParticlesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      m_pDisplayParticlesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      m_pDisplayParticlesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      GFX::GTextureSampler* l_pParticlesTS = m_pDisplayParticlesMaterial->PixelShader(0)->TextureSampler(0);
      l_pParticlesTS->Texture(l_pParticleTxtr);
      l_pParticlesTS->MagnificationFilter(GFX::FILTER_LINEAR);
      l_pParticlesTS->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_pParticlesTS->AddressingModeV(GFX::ADDRESS_CLAMP);

      SAFE_RELEASE(l_pParticleTxtr);

      m_pEarthModelMaster->Add_Child(m_pAtlasEarth->Master());

      // Initialize information particles mesh
      g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(m_pDisplayParticlesMesh,
         1,
         -c_fParticleMaxSize,
         c_fParticleMaxSize,
         -c_fParticleMaxSize,
         c_fParticleMaxSize,
         1, 1,
         HELPER_MESH_CREATE_ALL);

      // Initialize outline model
      m_pEarthModelMaster->Add_Child(m_pOutlineModel);
      m_pOutlineModel->Set_Primitive(m_pOutlineMesh);

      // Initialize outline material
      GFX::GMaterial* l_pOutlineMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT);
      m_pOutlineModel->Set_Material(l_pOutlineMat);
      l_pOutlineMat->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_TEST_FUNCTION) = GFX::TEST_ALWAYS;
      SAFE_RELEASE(l_pOutlineMat);

      m_OutlineColor.R  = 255;
      m_OutlineColor.G  = 255;
      m_OutlineColor.B  = 255;
      m_OutlineColor.A  = 255;
   }

   return true;
}

bool GBackgroundEarth::CreateTransitionAnimation()
{
   const UINT32 c_iNbTransitionKeys = 2;
   const REAL32 c_fTransitionTime = 1.f;

   static CGFX_Key_Linear s_pTransitionKeys[c_iNbTransitionKeys] =
   {
      {0,                  0},
      {c_fTransitionTime,  1},
   };

   static CGFX_Key_Linear s_pTransitionInvKeys[c_iNbTransitionKeys] =
   {
      {0,                  1},
      {c_fTransitionTime,  0},
   };

   // Create all animation objects
   CGFX_Action_Itf* l_pAction = g_Joshua.Renderer()->Create_Action();
   m_pTransitionViewTrack = g_Joshua.Renderer()->Create_Track();

   CGFX_Animation_Linear_Itf* l_pForwardAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Animation_Linear_Itf* l_pReverseAnim = g_Joshua.Renderer()->Create_Animation_Linear();

   GFX::GAnimationMultiple* l_pForwardMultiAnim = g_Joshua.Renderer()->CreateAnimationMultiple();
   GFX::GAnimationMultiple* l_pReverseMultiAnim = g_Joshua.Renderer()->CreateAnimationMultiple();

   // Setup base animations
   l_pForwardAnim->Setup_Keys(s_pTransitionKeys, c_iNbTransitionKeys);
   l_pReverseAnim->Setup_Keys(s_pTransitionInvKeys, c_iNbTransitionKeys);

   // Connect all parameters to correct animation
   // (the forward transition animation goes from realistic view to strategic view)
   l_pForwardMultiAnim->Animation(l_pForwardAnim);
   l_pForwardMultiAnim->AddParamConnection(m_pStrategicMasterModel, GFX::PARAM_VISIBILITY);
   l_pForwardMultiAnim->AddParamConnection(m_pStrategicEarthMaterial->PixelShader(0), GFX::PARAM_COLOR_FACTOR_A);
   l_pForwardMultiAnim->AddParamConnection(m_pStrategicEarthLinesMaterial->PixelShader(0), GFX::PARAM_COLOR_FACTOR_A);

   m_pCloudMaterial->Add_Custom_Parameter(GString(L"Transparency") );

   l_pReverseMultiAnim->Animation(l_pReverseAnim);
   l_pReverseMultiAnim->AddParamConnection(m_pRealisticMasterModel, GFX::PARAM_VISIBILITY);
   l_pReverseMultiAnim->AddParamConnection(m_pRealisticOverlayModel, GFX::PARAM_VISIBILITY);
   l_pReverseMultiAnim->AddParamConnection(m_pStarsMaterial->PixelShader(0), GFX::PARAM_COLOR_FACTOR_A);
   l_pReverseMultiAnim->AddParamConnectionCustom(m_pCloudMaterial, 0);

   // Setup the action
   l_pAction->Length(c_fTransitionTime);
   l_pAction->Add_Animation(l_pForwardMultiAnim);
   l_pAction->Add_Animation(l_pReverseMultiAnim);

   // Setup the track
   m_pTransitionViewTrack->Set_Action(l_pAction);
   m_pTransitionViewTrack->ExtrapolationAfter(true);
   m_pTransitionViewTrack->Playback_Count(1);
   m_pTransitionViewTrack->Playback(GFX::PLAYBACK_FORWARD);

   m_pTransitionViewTrack->Update_Animations(0);

   l_pAction->Release();
   l_pForwardAnim->Release();
   l_pReverseAnim->Release();
   l_pForwardMultiAnim->Release();
   l_pReverseMultiAnim->Release();

   return true;
}

void GBackgroundEarth::UpdateCamera()
{
   REAL32 l_fAlphaValue = m_pStrategicMasterModel->Visibility() ? 0.25f : 0.75f - (m_CurrentState.m_Zoom * 0.60f);
   m_pFillVS->Diffuse().A = l_fAlphaValue;

   // Compute camera distance from earth center
   REAL32 t = ((*m_pfCamDist) + 1);

   // Compute camera position
   GVector3D<REAL32> l_Pos;
   l_Pos.y = sinf((*m_pfCamAngleY)) * t;
   t *= cosf((*m_pfCamAngleY));
   l_Pos.x = cosf((*m_pfCamAngleX)) * t;
   l_Pos.z = sinf((*m_pfCamAngleX)) * t;

   // Update camera gfx transform
   m_pCameraTransform->Translation() = l_Pos;

   // Get current view & projection matrices
   m_pCameraTransform->Update();
   m_ViewMat = m_pCamera->Get_View_Matrix(m_pCameraTransform->Get_Matrix() );
   m_ProjMat = m_pCamera->Get_Projection_Matrix();

   g_Joshua.Renderer()->Get_Current_Mode(&m_Mode);

   m_ScreenTransformScale = GVector2D<REAL32>(m_Mode.m_Resolution) * 0.5f;
   m_ScreenTransformOffset = m_ScreenTransformScale;
   m_ScreenTransformScale.y *= -1;

   m_OriginPos = m_ViewMat * GVector3D<REAL32>(0, 0, 0);
   m_fOriginDist = m_OriginPos.x * m_OriginPos.x +
      m_OriginPos.y * m_OriginPos.y +
      m_OriginPos.z * m_OriginPos.z - 1;
   m_fOriginDistInv = m_OriginPos.z - 1 / m_OriginPos.z;
   m_CameraPos = m_pCameraTransform->Translation();

   // Update atlas display
   m_pAtlasEarth->UpdateCamera();

   // Update atmospheric halo
   UpdateHalo();

   // Only show parts that have all four corners visible
   vector<UINT32> l_vNowVisible, l_vNowInvisible;
   l_vNowVisible.reserve(NB_EARTH_DIVS);
   l_vNowInvisible.reserve(NB_EARTH_DIVS);
   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      if(ClipOnEarth(m_pEarthCorners[i][0] ) ||
         ClipOnEarth(m_pEarthCorners[i][1] ) || 
         ClipOnEarth(m_pEarthCorners[i][2] ) || 
         ClipOnEarth(m_pEarthCorners[i][3] ) || 
         ClipOnEarth(m_pEarthCorners[i][4] ) )
      {
         if(!m_pEarthModel[i]->Visibility() )
         {
            m_pEarthModel[i]->Visibility() = true;
            l_vNowVisible.push_back(i);
         }
      }
      else
      {
         if(m_pEarthModel[i]->Visibility() )
         {
            m_pEarthModel[i]->Visibility() = false;
            l_vNowInvisible.push_back(i);
         }
      }
   }

   // Add a ref (& make visible) all regions that are now visible
   for(UINT32 i = 0;i < l_vNowVisible.size();i ++)
   {
      vector<UINT32>& l_vRegions = m_pEarthRegions[l_vNowVisible[i] ];
      for(UINT32 j = 0;j < l_vRegions.size();j ++)
      {
         if(m_vRegionRefCount[l_vRegions[j] ] == 0)
         {
            *(m_vRegionVisibilityParams[l_vRegions[j] ] ) = 1;
         }
         m_vRegionRefCount[l_vRegions[j] ] ++;
      }
   }

   // Remove a ref (& make invisible if needed) all regions that are no longuer invisible
   for(UINT32 i = 0;i < l_vNowInvisible.size();i ++)
   {
      vector<UINT32>& l_vRegions = m_pEarthRegions[l_vNowInvisible[i] ];
      for(UINT32 j = 0;j < l_vRegions.size();j ++)
      {
         m_vRegionRefCount[l_vRegions[j] ] --;
         if(m_vRegionRefCount[l_vRegions[j] ] <= 0)
         {
            *(m_vRegionVisibilityParams[l_vRegions[j] ] ) = 0;
         }
      }
   }

   // Update notification signs positions
   UpdateAllNotificationSigns();

   // Update combat display
   UpdateCombats();
   UpdateBombardments();

   // Update city display
   m_pCityOverlay->UpdateCities();
}

void GBackgroundEarth::UpdateHalo()
{
   if( !m_pAtmoHaloModel || !m_pAtmoHaloModel->Visibility() )
      return; 

   // Get transformation matrix of halo
   GMatrix4x4<REAL32>& l_HaloMatrix = m_pAtmoHaloTransform->Get_Matrix();

   // Make a scale matrix
   GMatrix4x4<REAL32> l_Scale;
   l_Scale.LoadIdentity();
   l_Scale(0,0) = c_AtmoHaloScale;
   l_Scale(1,1) = c_AtmoHaloScale;
   l_Scale(2,2) = c_AtmoHaloScale;

   // Make halo rotation matrix
   GVector3D<REAL32> l_Up(0, 1, 0);

   // Calculate view vector
   GVector3D<REAL32> l_View = m_CameraPos * -1;
   l_View.Normalize();

   // Calculate right vector
   GVector3D<REAL32> l_Right = l_Up.CrossProduct(l_View);
   l_Right.Normalize();

   // Calculate up vector
   l_Up = l_View.CrossProduct(l_Right);

   // Update halo matrix
   l_HaloMatrix(0,0) = l_Right.x;
   l_HaloMatrix(0,1) = l_Right.y;
   l_HaloMatrix(0,2) = l_Right.z;

   l_HaloMatrix(1,0) = l_Up.x;
   l_HaloMatrix(1,1) = l_Up.y;
   l_HaloMatrix(1,2) = l_Up.z;

   l_HaloMatrix(2,0) = l_View.x;
   l_HaloMatrix(2,1) = l_View.y;
   l_HaloMatrix(2,2) = l_View.z;

   // Apply scale
   l_HaloMatrix = l_HaloMatrix * l_Scale;
}

void GBackgroundEarth::UpdateSun()
{
   if( !m_pSunModel || !m_pSunModel->Visibility() )
      return;
   
   if( DayNightCycleEnabled() )
   {
      GVector3D<REAL32> l_SunPos;

      // Compute sun angle
      REAL32 t = c_EarthAngle * sinf( (m_fYearFactor - 0.5f) * (REAL32)Math::PI * 2);
      REAL32 tc = cosf(t);
      REAL32 ts = sinf(t);

      // Compute sun position
      l_SunPos.y = c_SunDist * ts;
      l_SunPos.x = cosf(-m_fDayFactor * (REAL32)Math::PI * 2) * tc * c_SunDist;
      l_SunPos.z = sinf(-m_fDayFactor * (REAL32)Math::PI * 2) * tc * c_SunDist;

      // Update sun gfx transform
      m_pSunTransform->Translation() = l_SunPos;
   }
   else
   {
      // Update sun gfx transform
      GVector3D<REAL32> l_CamPosNorm = m_pCameraTransform->Translation();
      l_CamPosNorm.Normalize();
      m_pSunTransform->Translation() = (l_CamPosNorm * c_SunDist);
   }
}

void GBackgroundEarth::UpdateMoon()
{
   if( !m_pMoonModel || !m_pMoonModel->Visibility() )
      return;

   // Compute moon angle
   REAL32 l_MoonAngle = -2 * (REAL32) Math::PI * m_fYearFactor * c_MoonRevYear + (REAL32) Math::PI;

   // Compute unit moon
   GVector3D<REAL32> l_MoonPos;
   l_MoonPos.y = 0;
   l_MoonPos.x = cosf(l_MoonAngle);
   l_MoonPos.z = sinf(l_MoonAngle);

   // Compute moon orientation
   GVector3D<REAL32> l_MoonForward;
   GVector3D<REAL32> l_MoonUp(0,1,0);
   GVector3D<REAL32> l_MoonRight;

   l_MoonForward = GVector3D<REAL32>(0,0,0) - l_MoonPos;

   l_MoonRight = l_MoonUp.CrossProduct(l_MoonForward);

   l_MoonUp = l_MoonForward.CrossProduct(l_MoonRight);

   // TEST
   GMatrix4x4<REAL32> l_DirMatrix;
   l_DirMatrix.LoadIdentity();

   l_DirMatrix(2,0) = 1;
   l_DirMatrix(2,1) = 0;
   l_DirMatrix(2,2) = 0;

   l_DirMatrix(1,0) = 0;
   l_DirMatrix(1,1) = 1;
   l_DirMatrix(1,2) = 0;

   l_DirMatrix(0,0) = 0;
   l_DirMatrix(0,1) = 0;
   l_DirMatrix(0,2) = -1;

   // Offset moon
   l_MoonPos.x *= c_MoonDist;
   l_MoonPos.z *= c_MoonDist;

   // Update sun gfx transform
   GMatrix4x4<REAL32>& l_MoonMatrix = m_pMoonTransform->Get_Matrix();
   l_MoonMatrix.LoadIdentity();

   l_MoonMatrix(0,0) = l_MoonRight.x;
   l_MoonMatrix(0,1) = l_MoonRight.y;
   l_MoonMatrix(0,2) = l_MoonRight.z;

   l_MoonMatrix(1,0) = l_MoonUp.x;
   l_MoonMatrix(1,1) = l_MoonUp.y;
   l_MoonMatrix(1,2) = l_MoonUp.z;

   l_MoonMatrix(2,0) = l_MoonForward.x;
   l_MoonMatrix(2,1) = l_MoonForward.y;
   l_MoonMatrix(2,2) = l_MoonForward.z;

   l_MoonMatrix = l_DirMatrix.Transpose() * l_MoonMatrix;

   l_MoonMatrix(3,0) = l_MoonPos.x;
   l_MoonMatrix(3,1) = l_MoonPos.y;
   l_MoonMatrix(3,2) = l_MoonPos.z;
}

void GBackgroundEarth::UpdateClouds()
{
   if( m_pCloudModel && m_pCloudModel->Visibility() )
   {
      REAL32 l_fAlphaValue = 1 - ( (m_fOriginDist - 1) - m_fCloudFadeStart) * m_fCloudFadeDeltaInv;
      ( (GFX::GCascadePixelShader*) m_pCloudMaterial->PixelShader(0) )->ColorFactor().A = l_fAlphaValue * m_pCloudMaterial->Custom_Parameter(0);

      ( (GFX::GCascadePixelShader*) m_pWaterMaterial->PixelShader(0) )->ColorFactor().A = l_fAlphaValue;
   }
}

void GBackgroundEarth::UpdateFlares()
{
   if( !m_pSunModel || !m_pSunModel->Visibility() )
      return;

   GMatrix4x4<REAL32> l_BaseMatrix = m_ViewMat.Transpose();
   l_BaseMatrix(0,3) = 0;
   l_BaseMatrix(1,3) = 0;
   l_BaseMatrix(2,3) = 0;

   GVector3D<REAL32> l_SunPos = m_pSunTransform->Translation();

   l_BaseMatrix(3,0) = l_SunPos.x;
   l_BaseMatrix(3,1) = l_SunPos.y;
   l_BaseMatrix(3,2) = l_SunPos.z;
   m_pFlareTransform[0]->Set_Matrix(l_BaseMatrix);

   GVector3D<REAL32> l_RayDir = l_SunPos - m_CameraPos;

   REAL32 l_fA = l_RayDir.x * l_RayDir.x +
      l_RayDir.y * l_RayDir.y +
      l_RayDir.z * l_RayDir.z;
   REAL32 l_fB = 2 * l_RayDir.x * l_SunPos.x +
      2 * l_RayDir.y * l_SunPos.y +
      2 * l_RayDir.z * l_SunPos.z;
   REAL32 l_fC = l_SunPos.x * l_SunPos.x +
      l_SunPos.y * l_SunPos.y +
      l_SunPos.z * l_SunPos.z - 1;

   REAL32 l_fDis = l_fB * l_fB - 4 * l_fA * l_fC;

   m_pFlareMasterModel->Visibility() = false;

   if(l_fDis < 0)
   {
      GVector3D<REAL32> l_SunPosView = m_ViewMat * l_SunPos;
      REAL32 l_SunDist = l_SunPosView.Length();

      if(l_SunPosView.DotProduct( GVector3D<REAL32>(0,0,1) ) > l_SunDist * c_FlareMaxDist)
      {
         GMatrix4x4<REAL32> l_ViewMatInv = m_ViewMat.Inverse();
         l_SunPosView.x = 0; l_SunPosView.y = 0;

         GVector3D<REAL32> l_FlarePlaneCenter = l_ViewMatInv * l_SunPosView;
         GVector3D<REAL32> l_FlareVector = l_SunPos - l_FlarePlaneCenter;

         m_pFlareMasterModel->Visibility() = true;

         for(UINT32 f = 0;f < c_NbFlares;f ++)
         {
            GVector3D<REAL32> l_Pos = l_FlarePlaneCenter + (l_FlareVector * c_FlareDistScale[f][0] );

            GMatrix4x4<REAL32> l_CurrentMatrix;
            l_CurrentMatrix.LoadIdentity();
            l_CurrentMatrix(0,0) = c_FlareDistScale[f][1];
            l_CurrentMatrix(1,1) = c_FlareDistScale[f][1];
            l_CurrentMatrix(2,2) = c_FlareDistScale[f][1];

            l_CurrentMatrix = l_CurrentMatrix * l_BaseMatrix;

            l_CurrentMatrix(3,0) = l_Pos.x;
            l_CurrentMatrix(3,1) = l_Pos.y;
            l_CurrentMatrix(3,2) = l_Pos.z;

            m_pFlareTransform[f + 1]->Set_Matrix(l_CurrentMatrix);
         }
      }
   }
}

void GBackgroundEarth::UpdateCameraParameters(const GVector2D<REAL32>& in_Coord,
                                              REAL32 in_fDistance,
                                              CGFX_Camera_Itf* in_pCam, 
                                              CGFX_Transform_SRT_Itf* in_pTfx)
{
   // Compute camera distance from earth center
   REAL32 t = in_fDistance + 1;

   // Compute camera position
   GVector3D<REAL32> l_Pos;
   l_Pos.y = sinf(in_Coord.y) * t;
   t *= cosf(in_Coord.y);
   l_Pos.x = cosf(in_Coord.x + (REAL32) Math::PI) * t;
   l_Pos.z = sinf(in_Coord.x + (REAL32) Math::PI) * t;

   // Update camera gfx transform
   in_pTfx->Translation() = l_Pos;
   in_pTfx->Rotation() = GVector3D<REAL32>(0, 0, 0);
   in_pTfx->Scale() = GVector3D<REAL32>(1, 1, 1);

   in_pCam->View_Type(m_pCamera->View_Type() );
   in_pCam->Aspect_Ratio() = m_pCamera->Aspect_Ratio();
   in_pCam->FOV() = m_pCamera->FOV();
   in_pCam->Up_Vector() = m_pCamera->Up_Vector();
   in_pCam->Look_At() = m_pCamera->Look_At();
   in_pCam->Near_Plane() = m_pCamera->Near_Plane();
   in_pCam->Far_Plane() = m_pCamera->Far_Plane();
}

GUI::EEventStatus::Enum GBackgroundEarth::OnHotkey(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   // Do specific action depending on key pressed
   switch(in_EventData.Key())
   {
   case VK_HOME:
      g_ClientDCL.SelectHomeCountry();
      break;
   // earth material
   case 'B':
      if(m_pEarthMaterial->MaximumPass() == 2)
      {
         m_pEarthMaterial->MaximumPass(INVALID_INDEX);
      }
      else
      {
         m_pEarthMaterial->MaximumPass(2);
      }
      break;
   default:
      return GUI::EEventStatus::NotHandled;
   }
   return GUI::EEventStatus::Handled;
}

UINT32 GBackgroundEarth::FindVisibleUnitAt(const GVector2D<INT32> & in_Position)
{
   for(INT32 i = g_ClientDAL.VisibleUnits().size() - 1; i >= 0; i--)
   {
      INT32 l_UnitSpaceX = in_Position.x - g_ClientDAL.VisibleUnits()[i].m_Pos.x;
      INT32 l_UnitSpaceY = in_Position.y - g_ClientDAL.VisibleUnits()[i].m_Pos.y;
      if((l_UnitSpaceX >= 0) && (l_UnitSpaceX < g_ClientDAL.VisibleUnits()[i].m_Size.x) &&
         (l_UnitSpaceY >= 0) && (l_UnitSpaceY < g_ClientDAL.VisibleUnits()[i].m_Size.y))
      {
         if(g_Joshua.UnitManager().UnitGroup(g_ClientDAL.VisibleUnits()[i].m_iId) )
         {
            return g_ClientDAL.VisibleUnits()[i].m_iId;
         }
      }
   }
   return 0;
}

UINT32 GBackgroundEarth::FindVisibleCombatAt(const GVector2D<INT32> & in_Position)
{
   for(INT32 i = g_ClientDAL.VisibleCombats().size() - 1; i >= 0; i--)
   {
      INT32 l_CombatSpaceX = in_Position.x - g_ClientDAL.VisibleCombats()[i].m_Pos.x;
      INT32 l_CombatSpaceY = in_Position.y - g_ClientDAL.VisibleCombats()[i].m_Pos.y;
      if((l_CombatSpaceX >= 0) && (l_CombatSpaceX < g_ClientDAL.VisibleCombats()[i].m_Size.x) &&
         (l_CombatSpaceY >= 0) && (l_CombatSpaceY < g_ClientDAL.VisibleCombats()[i].m_Size.y) )
      {
         return g_ClientDAL.VisibleCombats()[i].m_iId;
      }
   }
   return 0;
}


void GBackgroundEarth::ShowDeployementMenu()
{
   GContextMenu* l_pMenu = g_ClientDDL.ContextMenuSpawn();
   l_pMenu->Owner(this);
   l_pMenu->ClearList();

   // if no tutorials only
   if(GTutorials::Instance().StartedTutorial() == ETutorials::None)
   {
      for(UINT32 i = 0; i < c_iContextMenuDeployementCount; i++)
      {      
         l_pMenu->AddItem(g_ClientDAL.GetString(c_iContextMenuDeployementTextId[i]));
      }
   }

   l_pMenu->RegisterHandler(this, (CALLBACK_HANDLER_void_pvoid)&GBackgroundEarth::HandleContextMenuDeployement);
   l_pMenu->Show();
}

void GBackgroundEarth::ShowMilitaryMenu()
{
   GContextMenu* l_pMenu =  g_ClientDDL.ContextMenuSpawn();
   l_pMenu->Owner(this);
   l_pMenu->ClearList();
   for(UINT32 i = 0; i < c_iContextMenuMilitaryCount; i++)
   {
      // if no tutorials only or only move and change status menu ...
      if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      {  
         if((GTutorials::Instance().StartedTutorial() != ETutorials::MilitaryTutorial) ||
            ((c_iContextMenuMilitaryTextId[i] != 101098) &&
             (c_iContextMenuMilitaryTextId[i] != 101100)))
            continue;   
      }         	      

      //If we are adding split to menu, ensure there is only one unit selected
      if(c_iContextMenuMilitaryTextId[i] == 102094)
         if( g_ClientDAL.SelectedUnitsID().size() != 1)
            continue;

      // add menu
      GString l_sMilitaryMenuText = g_ClientDAL.GetString(c_iContextMenuMilitaryTextId[i]);
      if(c_iContextMenuMilitarySubMenuCount[i] > 0)
      {
         GContextMenu* l_pSubMenu = l_pMenu->CreateSubMenu(l_sMilitaryMenuText, this);
         for(UINT32 j = 0; j < c_iContextMenuMilitarySubMenuCount[i]; j++)
            l_pSubMenu->AddItem(g_ClientDAL.GetString(c_iContextMenuMilitarySubText[i][j]) );
         l_pMenu->AddItem(l_sMilitaryMenuText, EContextMenuItemType::Popup, l_pSubMenu);
      }
      else
      {
         l_pMenu->AddItem(l_sMilitaryMenuText);
      }
   }

   l_pMenu->RegisterHandler(this, (CALLBACK_HANDLER_void_pvoid)&GBackgroundEarth::HandleContextMenuMilitary);
   l_pMenu->Show();
}

void GBackgroundEarth::ShowContextMenu()
{
   if (g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;	

   GContextMenu* l_pMenu = g_ClientDDL.ContextMenuSpawn();
   l_pMenu->Owner(this);
   l_pMenu->ClearList();

   bool l_SelectedOwnUnit = g_ClientDAL.SelectedUnitsID().size() > 0;

   if (g_ClientDAL.SelectedUnitsID().size() == 1)
   {   
      SDK::Combat::GUnitGroup* l_pSelectedUnitGroup = g_Joshua.UnitManager().UnitGroup(*g_ClientDAL.SelectedUnitsID().begin());
      if (l_pSelectedUnitGroup->OwnerId() != (UINT32)g_ClientDAL.ControlledCountryID())
         l_SelectedOwnUnit = false;
   }

   if (l_SelectedOwnUnit)
   {

      bool l_ClickedUnitGroup = false;
      bool l_ClickedOwnUnitGroup = false;
      UINT32 l_ClickedUnitID = FindVisibleUnitAt(m_ClickPosition);
      if (l_ClickedUnitID)
      {
         l_ClickedUnitGroup = true;
         if (g_Joshua.UnitManager().UnitGroup(l_ClickedUnitID)->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID())
            l_ClickedOwnUnitGroup = true;
      }
      if (l_ClickedUnitGroup)
      {
         // if no tutorials only ...
         if(GTutorials::Instance().StartedTutorial() == ETutorials::None)
         {
            if (l_ClickedOwnUnitGroup)
               l_pMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::MergeSelectedWith]));
            else
               l_pMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::AttackWithSelected]));
         }
      }
      // if not in interface tutorial only ...
      if(GTutorials::Instance().StartedTutorial() != ETutorials::InterfaceTutorial)
         l_pMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::MoveSelectedUnit]));
   }

   //Check if we must show the context menu to occupy region and show it if we must
   {
      UINT32 l_iRegionID = g_ClientDCL.EarthLocateRegion(m_ClickLongLat.x,m_ClickLongLat.y);

      CheckAndShowOccupyTerritoryContextMenu (l_iRegionID,l_pMenu);
		CheckAndShowRelationsContextMenu       (l_iRegionID,l_pMenu);
      CheckAndShowBombardmentContextMenu     (l_iRegionID,l_pMenu);
   }


	//Add the Quick Treaty menu
   // if no tutorials only ...
   if(GTutorials::Instance().StartedTutorial() == ETutorials::None)
	{  
      // flags to know what is on about the selected region
      bool l_bNoPoliticalControl = m_iCountryToCheckRelations != 0 &&
			                          m_iCountryToCheckRelations != (UINT32)g_ClientDAL.ControlledCountryID();
      bool l_bNoMilitaryControl  = m_iCountryToCheckMilitary != 0 &&
			                          m_iCountryToCheckMilitary != (UINT32)g_ClientDAL.ControlledCountryID();

		if(l_bNoPoliticalControl || l_bNoMilitaryControl)
		{
         // create the quick treaty sub menu entry
			GString l_sThematicMapQuickTreatySubmenuName = g_ClientDAL.GetString(c_iThematicMapQuickTreatySubmenuEntry);
			GContextMenu * l_QuickTreatyMenu = l_pMenu->CreateSubMenu(l_sThematicMapQuickTreatySubmenuName, this);
			l_pMenu->AddItem(l_sThematicMapQuickTreatySubmenuName, EContextMenuItemType::Popup, l_QuickTreatyMenu); 

         // quick treaties with region we dont have political control on
         if(l_bNoPoliticalControl)
         {
			   CheckAndShowQuickTreatyAllianceContextMenu(l_QuickTreatyMenu,g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
			   CheckAndShowQuickTreatyAnnexationContextMenu(l_QuickTreatyMenu,g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
			   CheckAndShowQuickTreatyMilitaryAccessContextMenu(l_QuickTreatyMenu,g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
			   l_QuickTreatyMenu->AddSeparator();
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt));	
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyCommonMarket));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyEconomicAid));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyNobleCause));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyResearchPartnership));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyWeaponTrade));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyRequestWar));
			   l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebtInverted));
            l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyFreeRegionInverted));
         }
         // quick treaties with region we dont have military control on but have political control
         else if(l_bNoMilitaryControl)
         {
            l_QuickTreatyMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyFreeRegionInverted));
         }
		}		    
	}

   GString l_sThematicMapSubmenuName = g_ClientDAL.GetString(c_iThematicMapsSubmenuEntry);
   GContextMenu * l_ThematicMapsMenu = l_pMenu->CreateSubMenu(l_sThematicMapSubmenuName, this);
   l_pMenu->AddItem(l_sThematicMapSubmenuName, EContextMenuItemType::Popup, l_ThematicMapsMenu);

   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Geographic                        ]));
   l_ThematicMapsMenu->AddSeparator();
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryEmpire                     ]));
	l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::PoliticalControl						  ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryRelations                  ]));   
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryPoliticalStrength          ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryPopulationGrowth           ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryPopulationStabilty         ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryPopulationSupport          ]));   
   l_ThematicMapsMenu->AddSeparator();
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::RegionPopulationTotal             ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::RegionPopulationDensity           ]));
   l_ThematicMapsMenu->AddSeparator();
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryEconomicalStrength         ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryGrossDomesticProduct       ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryGrossDomesticProductGrowth ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryExceedingResources         ]));

   GString l_sThematicMapResourceProductionSubmenuName = g_ClientDAL.GetString(c_iThematicMapResourceProductionSubmenuEntry);
   GContextMenu * l_ResourcesMenu = l_pMenu->CreateSubMenu(l_sThematicMapResourceProductionSubmenuName, this);
   l_ThematicMapsMenu->AddItem(l_sThematicMapResourceProductionSubmenuName, EContextMenuItemType::Popup, l_ResourcesMenu);
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Cereals               ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Vegetable_Fruits      ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Meat                  ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Dairy                 ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Tobacco               ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Drugs                 ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Electricity           ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Fossile_Fuels         ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Wood_Paper            ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Minerals              ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Iron_Steel            ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Precious_Stones       ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Fabrics               ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Plastics              ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Chemicals             ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Pharmaceuticals       ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Appliances            ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Vehicules             ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Machinery             ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Commodities           ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Luxury_Commodities    ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Construction          ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Engineering           ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Health_Care           ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Retail                ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Legal_Services        ]));
   l_ResourcesMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Marketing_Advertising ])); 
   
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryHumanDevelopment           ]));
   l_ThematicMapsMenu->AddSeparator();
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryMilitaryStrength           ]));
   l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::CountryNuclearStrength            ]));

   if ( g_ClientDDL.GameLobby() && ( g_ClientDDL.GameLobby()->PlayersList().size() > 1 ) )
   {
      l_ThematicMapsMenu->AddSeparator();
      l_ThematicMapsMenu->AddItem(g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::HumanControlledCountries]));
   }

   l_pMenu->RegisterHandler(this, (CALLBACK_HANDLER_void_pvoid)&GBackgroundEarth::HandleContextMenu);
   l_pMenu->Show();
}

/*!
* Mouse down handler
**/

GUI::EEventStatus::Enum GBackgroundEarth::OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{


   if (m_CancelAllMouseEventsUntilAllButtonsUp)
      return EEventStatus::Handled;

   if(in_pCaller || in_EventData.InitialRecipient != this)
      return EEventStatus::Handled;

   if (!in_EventData.Mouse.IsDoubleClick && m_PreventShowUnitListJustAfterSelect)
      m_PreventShowUnitListJustAfterSelect = false;


   m_ClickPosition = in_EventData.Mouse.Position;
   bool l_bClickedOnEarth = ScreenToLongLat(m_ClickPosition, m_ClickLongLat);

   if(!l_bClickedOnEarth)
   {
      m_ClickLongLat.Set(-FLT_MAX, -FLT_MAX);

      if(!m_pReturnWindow && in_EventData.Mouse.Actor.Bits.Left && !in_EventData.Keyboard.Ctrl && !in_EventData.Keyboard.Shift)
      {
         g_ClientDCL.KillCombatInfo();
         g_ClientDAL.UnselectUnits();
         if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
            g_ClientDDL.UnitListWindowNew()->Hide();

         g_ClientDAL.SelectCombat(0);         
         g_ClientDDL.UnitInfoSmallWindow()->Hide();         
      }

      return GUI::EEventStatus::Handled;
   }

   if(m_pReturnWindow && in_EventData.Mouse.Actor.Bits.Left)
   {
      if(l_bClickedOnEarth)
         (m_pReturnWindow->*m_ReturnClickedPosition)(m_ClickLongLat);
   }
	else if(m_eView == EBackgroundView::Strategic && in_EventData.Mouse.Actor.Bits.Left)
	{
		UINT32 l_iRegionID      = g_ClientDCL.EarthLocateRegion(m_ClickLongLat.x, m_ClickLongLat.y);

		//If the selected area is invalid
		if(l_iRegionID != 0)
		{
			//Get the owner of the country
			UINT32 l_iSelectedCountry = g_ClientDAL.RegionControl(l_iRegionID).m_iMilitary;
			if(l_iSelectedCountry)
			{
				SP2::GStrategicTargetCountryWindow* l_pTargetWindow = g_ClientDDL.StrategicTargetCountryWindow();
				if(l_pTargetWindow && !l_pTargetWindow->SpecificTargetOn())
				{
					l_pTargetWindow->ChangeSelectedCountryToNuke(l_iSelectedCountry);
				}			
			}
		}
	}


   // Verify is click is inside background earth
   if(MouseToEarth(in_EventData.Mouse.Position, m_vfEarthClick) )
   {
      m_DragCoord = in_EventData.Mouse.Position;
      if(in_EventData.Mouse.Actor.Bits.Right)
      {
         if(m_bSelectBox)
         {
            m_bSelectBox = false;
            m_pDragBoxModel->Visibility() = false;
            if (!in_EventData.Keyboard.Alt)
            {
               g_ClientDAL.UnselectUnits();
               for (set<UINT32>::iterator itr = m_OrigSelectedGroups.begin(); itr != m_OrigSelectedGroups.end(); itr++)
                  g_ClientDAL.SelectUnit(*itr, true);
            }
            m_CancelAllMouseEventsUntilAllButtonsUp = true;
         }

         //Set the dragspeed relative to the zoom level
         m_fDragSpeed = Property(L"DragSpeed").ToREAL32() * m_pfCamDist->Value();
         m_InitialState = g_ClientDCL.m_EarthState;
      }
      else if(in_EventData.Mouse.Actor.Bits.Left)
      {
         if (m_bUnitPicked)
         {
            m_bUnitPicked = false;
            g_ClientDDL.ResetMousePointer();
            UINT32 l_ClickedUnit = 0;
            if (m_UnitPickedAction == EMilitaryAction::MergeWith)
            {
               l_ClickedUnit = FindVisibleUnitAt(in_EventData.Mouse.Position); 
               if (!l_ClickedUnit) 
                  return GUI::EEventStatus::Handled;

               SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_ClickedUnit);
               if( (l_pUnitGroup->OwnerId() != (UINT32)g_ClientDAL.ControlledCountryID()) || 
                   (l_pUnitGroup->Status() == EMilitaryStatus::InDeployement )
                 )
                  return GUI::EEventStatus::Handled;

               m_ClickLongLat = l_pUnitGroup->Position();              
            }

            vector<UINT32> l_vMovedUnits;
            for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
            {
               if (*itr != l_ClickedUnit)
                  l_vMovedUnits.push_back(*itr);
            }            
            g_ClientDCL.RequestUnitMove(l_vMovedUnits, m_ClickLongLat, m_UnitPickedAction);
         }
         else
         {
            UINT32 l_ClickedUnit = FindVisibleUnitAt(in_EventData.Mouse.Position);      
            if (l_ClickedUnit)
            {
               if (g_ClientDAL.SelectedUnitsID().find(l_ClickedUnit) != g_ClientDAL.SelectedUnitsID().end())
               {
                  if (in_EventData.Keyboard.Ctrl)
                  {
                     m_UnitToDeferDeselect = l_ClickedUnit;                  
                     return GUI::EEventStatus::Handled;
                  }
                  else if(!in_EventData.Keyboard.Shift)
                     if (g_ClientDAL.SelectedUnitsID().size() > 1)
                        m_UnitToKeepOnDeferDeselect = l_ClickedUnit;                  
               }
               else
               {
                  g_ClientDAL.SelectUnit(l_ClickedUnit, in_EventData.Keyboard.Shift || in_EventData.Keyboard.Ctrl);
                  m_PreventShowUnitListJustAfterSelect = true;
               }

               if (g_ClientDAL.SelectedUnitsID().find(l_ClickedUnit) != g_ClientDAL.SelectedUnitsID().end())
                  if (g_Joshua.UnitManager().UnitGroup(l_ClickedUnit)->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID())
                     m_UnitToDrag = l_ClickedUnit;
            }
            else
            {               
               if (!in_EventData.Keyboard.Ctrl && !in_EventData.Keyboard.Shift)
               {
                  // Don't try to select units or combat in not playing mode
                  if((g_SP2Client->CurrentFSMState() != FSM::EStates::Playing))
                     return EEventStatus::Handled;

                  g_ClientDAL.UnselectUnits();                  
                  if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
                     g_ClientDDL.UnitListWindowNew()->Hide();
                  g_ClientDDL.UnitInfoSmallWindow()->Hide();
                  UINT32 l_ClickedCombat = FindVisibleCombatAt(in_EventData.Mouse.Position);
                  if (l_ClickedCombat)
                  {

                     SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)g_Joshua.UnitManager().ArenaInfo(l_ClickedCombat);
                     if(l_pArenaInfo)
                     {
                        g_ClientDAL.SelectCombat(l_ClickedCombat);
                        if(l_pArenaInfo->Actor(g_ClientDAL.ControlledCountryID() ) )
                        {
                           g_ClientDCL.DisplayCombatInfo(l_pArenaInfo);
                        }
                        else
                        {
                           g_ClientDCL.DisplayCombatInfoSmall(l_pArenaInfo);
                        }
                     }
                  }
               }
            }
         }
      }

      if (in_EventData.Mouse.Actor.Bits.Left)
      {
         //Check for unit list display it on dlb click on a unit
         if(in_EventData.Mouse.IsDoubleClick)
         {
            if (m_PreventShowUnitListJustAfterSelect)
            {
               m_PreventShowUnitListJustAfterSelect = false;
            }
            else
            {
               if(g_ClientDAL.SelectedUnitsID().size())
               {
                  if(!g_ClientDDL.UnitListWindowNew() )
                     g_ClientDDL.UnitListWindowNewSpawn();

                  SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );                  
                  g_ClientDDL.UnitListWindowNew()->UpdateUnitList( (UINT16)l_pGroup->OwnerId(), l_pGroup->Id() );

                  if(!g_ClientDDL.UnitListWindowNew()->Visible())
                     g_ClientDDL.UnitListWindowNew()->Show();
               }
            }
         }
         else
         {
            //Check if unit list needs to be refreshed
            if(!in_EventData.Mouse.IsDoubleClick)
            {
               if(g_ClientDAL.SelectedUnitsID().size())
               {
                  if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
                  {
                     SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );                  
                     g_ClientDDL.UnitListWindowNew()->UpdateUnitList( (UINT16)l_pGroup->OwnerId(), l_pGroup->Id() );
                  }
               }
            }
         }
      }

   }
   return EEventStatus::Handled;
}

void GBackgroundEarth::OnDragEnd(UINT8 in_Condition)
{
   switch (in_Condition)
   {
   case 1:
      m_bEarthDragging = false;
      break;
   }

}

void GBackgroundEarth::OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative)
{
   if (m_bEarthDragging)
   {
      if(!Locked())
      {
         if(m_pCameraAction)
         {
            UINT32 l_iID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pCameraAction);
            g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iID);
         }

         // Update camera position using right click drag
         m_InitialState.m_Longitude += in_MouseOffset.x * -m_fDragSpeed;
         m_InitialState.m_Latitude  += in_MouseOffset.y *  m_fDragSpeed;

         // Update minimap display
         g_ClientDCL.m_EarthState = m_InitialState;

         // Update camera position
         g_ClientDCL.UpdateWorldPosition(m_InitialState);
      }
   }
   else if(in_EventData.Mouse.Down.Bits.Right)
   {
      m_bEarthDragging = true;   
      Drag(1);         
   }

   if((g_SP2Client->CurrentFSMState() != FSM::EStates::Playing))
      return;

   if (m_bUnitDragging)
   {
      GVector3D<REAL32> l_Dummy;
      if ((g_Joshua.GUIManager()->FindTargetObject(g_Joshua.GUIManager()->MousePosition()) == this) 
         && (MouseToEarth(g_Joshua.GUIManager()->MousePosition(), l_Dummy)))
         g_Joshua.GUIManager()->SetMousePointer(m_PointerMoveUnit);
      else
         g_Joshua.GUIManager()->SetMousePointer(m_PointerMoveNo);

      if(g_ClientDDL.UnitInfoSmallWindow()->Visible())
         g_ClientDDL.UnitInfoSmallWindow()->Hide();
   }
   if (m_bSelectBox)
   {      
      GVector2D<INT32> l_PositionTL((INT32)m_pDragBoxTransform->Position().X, (INT32)m_pDragBoxTransform->Position().Y);
      GVector2D<INT32> l_PositionBR(g_Joshua.GUIManager()->MousePosition());

      m_pDragBoxTransform->Size() = l_PositionBR - l_PositionTL;      
      if (!in_EventData.Keyboard.Alt)
      {
         set<UINT32> l_UnitsInBox;
         if (l_PositionTL.x > l_PositionBR.x)
            swap(l_PositionTL.x, l_PositionBR.x);
         if (l_PositionTL.y > l_PositionBR.y)
            swap(l_PositionTL.y, l_PositionBR.y);
         for(INT32 i = g_ClientDAL.VisibleUnits().size() - 1; i >= 0; i--)
         {
            SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(g_ClientDAL.VisibleUnits()[i].m_iId);
            if (l_pGroup)
            {
               if ((l_pGroup->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID()))
               {
                  INT32 l_UnitX = g_ClientDAL.VisibleUnits()[i].m_Pos.x + (g_ClientDAL.VisibleUnits()[i].m_Size.x >> 1);
                  INT32 l_UnitY = g_ClientDAL.VisibleUnits()[i].m_Pos.y + (g_ClientDAL.VisibleUnits()[i].m_Size.y >> 1);
                  if ((l_PositionTL.x < l_UnitX) && 
                     (l_PositionBR.x > l_UnitX) && 
                     (l_PositionTL.y < l_UnitY) && 
                     (l_PositionBR.y > l_UnitY))
                     l_UnitsInBox.insert(g_ClientDAL.VisibleUnits()[i].m_iId);
               }
            }
         }

         if (in_EventData.Keyboard.Ctrl)
         {
            set<UINT32> l_NewSet;
            for (set<UINT32>::iterator itr = m_OrigSelectedGroups.begin(); itr != m_OrigSelectedGroups.end(); itr++)
               if (l_UnitsInBox.find(*itr) == l_UnitsInBox.end())
                  l_NewSet.insert(*itr);
            for (set<UINT32>::iterator itr = l_UnitsInBox.begin(); itr != l_UnitsInBox.end(); itr++)
               if (m_OrigSelectedGroups.find(*itr) == m_OrigSelectedGroups.end())
                  l_NewSet.insert(*itr);
            l_UnitsInBox = l_NewSet;               
         }
         else if (in_EventData.Keyboard.Shift)
            l_UnitsInBox.insert(m_OrigSelectedGroups.begin(), m_OrigSelectedGroups.end());         
     
         if (l_UnitsInBox != g_ClientDAL.SelectedUnitsID())
         {
            g_ClientDAL.UnselectUnits();
            for (set<UINT32>::iterator itr = l_UnitsInBox.begin(); itr != l_UnitsInBox.end(); itr++)
               g_ClientDAL.SelectUnit(*itr, true);
         }         
      }
   }
}

// Sphere Control (Political vs Military control)
void GBackgroundEarth::ShowSphereControlWindow(const GVector2D<INT32>& in_MousePos)
{
   // If we are not playing we should not need this information
   // (Or the info is not valid because we didnt receivew all the country and region name
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   // Only display if the background  Earth is dirrectly under the mouse
   GBaseObject* l_pObject ;
   l_pObject =  g_Joshua.GUIManager()->FindTargetObject(in_MousePos);

   // Get sphere control window
   GSphereControlWindow *l_pSphereControlWindow = g_ClientDDL.SphereControlWindow();

   if( l_pObject && 
      ( l_pObject != l_pSphereControlWindow ) && 
      ( l_pObject->Parent() != l_pSphereControlWindow ) && 
      ( l_pObject != this ) 
      )
   {
      l_pSphereControlWindow->Hide();
      return;
   }


   // Get longitude and latitude from mouse position
   GVector2D<REAL32> l_vLongLatOfMousePosition;
   ScreenToLongLat(in_MousePos,l_vLongLatOfMousePosition);

   // Get Region Id from longitude and latitude
   UINT32 l_iRegionID;
   l_iRegionID =g_ClientDCL.EarthLocateRegion(l_vLongLatOfMousePosition.x,l_vLongLatOfMousePosition.y);

   // We are pointing in space or in the ocean or there is no region yet.
   if(l_iRegionID == 0)
   {
      l_pSphereControlWindow->Hide();
      return;
   }
   // Get Region name
   GString l_RegionName = g_ClientDAL.RegionName(l_iRegionID);

   // Get country ID from Region ID
   GRegionControl l_CountryControl = g_ClientDAL.RegionControl(l_iRegionID);


   // Country is not yet initialize
   if(!l_CountryControl.m_iPolitical || !l_CountryControl.m_iMilitary)
   {
      l_pSphereControlWindow->Hide();
      return;
   }
   // Get country from country ID
   const GCountry& l_pPolCountry = g_ClientDAL.Country(l_CountryControl.m_iPolitical);
   const GCountry& l_pMilCountry = g_ClientDAL.Country(l_CountryControl.m_iMilitary);

   // Set political and miliary control from contry
   l_pSphereControlWindow->SetPoliticalControl(&l_pPolCountry);
   l_pSphereControlWindow->SetMilitaryControl(&l_pMilCountry);
   l_pSphereControlWindow->SetRegionName(l_RegionName);

   // Set its position just above the mouse
   l_pSphereControlWindow->Position(in_MousePos.x+c_iSphereCombatWindowOffset,in_MousePos.y-c_iSphereCombatWindowOffset);

   // Show Sphere Control Window
   l_pSphereControlWindow->Show();
}

void GBackgroundEarth::HideSphereControlWindow(void)
{
   // Get sphere control window
   GSphereControlWindow *l_pSphereControlWindow = g_ClientDDL.SphereControlWindow();

   if(l_pSphereControlWindow)
   {
      // Hide Window
      l_pSphereControlWindow->Hide();
   }
}

GUI::EEventStatus::Enum  GBackgroundEarth::OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Keyboard.Actor)
   {
      //Hide control window (political and military)
      case c_DefaultSphereWindowControlKey:
         HideSphereControlWindow();
         return EEventStatus::Handled;   
      default:
         return EEventStatus::NotHandled;
   }
}

GUI::EEventStatus::Enum  GBackgroundEarth::OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   // only handle event if we are playing, if we have our main bar and not in tutorials mode
   if (g_SP2Client->CurrentFSMState() != FSM::EStates::Playing      || 
       !g_ClientDDL.MainBar()                                       ||
       GTutorials::Instance().StartedTutorial() != ETutorials::None ||
       (g_ClientDDL.BattleOverviewWindow() && g_ClientDDL.BattleOverviewWindow()->Visible())) 
      return EEventStatus::NotHandled;

   // check for strategic warfare
   if(g_ClientDDL.StrategicTargetCountryWindow() && in_EventData.Key() != 'S')
      return EEventStatus::NotHandled;

   switch(in_EventData.Key())
   {
      //Display control window (political and military)
      case c_DefaultSphereWindowControlKey:
         ShowSphereControlWindow(in_EventData.Mouse.Position);
         break;   
      // Toggle unit display (flags or icons)
      case c_DefaultUnitIconsToggleControlKey:
         //Prevent the repeated events key to do a toggle
         ToggleUnitDisplayIconsFlags();                  
         break;
      // close top most window
      case VK_ESCAPE:    
         // If zooming a region, stop this operation
         if(m_bSelectBox || m_bUnitPicked || m_bUnitDragging)
            OnLoseFocus(NULL, NULL);
         // Close any opened context menu
         else if(g_ClientDDL.ContextMenu())
         {
            g_ClientDDL.ContextMenuKill();
         }
         else
         {
            return GUI::EEventStatus::NotHandled;
         }
         break;
      // Show politic window
      case 'P':
         g_ClientDDL.MainBar()->TogglePoliticWindow();
         break;
      // Show economic window
      case 'E':
         g_ClientDDL.MainBar()->ToggleEconomicWindow();
         break;
      // Show military window
      case 'M':
         g_ClientDDL.MainBar()->ToggleMilitaryWindow();
         break;
      // Show mail window
      case 'I':
         g_ClientDDL.MainBar()->ToggleMailWindow();
         break;
      // set a new game speed
      case '0':
         if(g_ClientDCL.RequestGameSpeedChange(c_fSP2GamePausedSpeed))
            g_ClientDDL.MainBar()->UpdateSpeed(c_fSP2GamePausedSpeed);
         break;
      case '1':
         if(g_ClientDCL.RequestGameSpeedChange(c_fSP2SlowestTimeSpeed))
            g_ClientDDL.MainBar()->UpdateSpeed(c_fSP2SlowestTimeSpeed);
         break;
      case '2':
         if(g_ClientDCL.RequestGameSpeedChange(c_fSP2NormalTimeSpeed))
            g_ClientDDL.MainBar()->UpdateSpeed(c_fSP2NormalTimeSpeed);
         break;
      case '3':
         if(g_ClientDCL.RequestGameSpeedChange(c_fSP2HighTimeSpeed))
            g_ClientDDL.MainBar()->UpdateSpeed(c_fSP2HighTimeSpeed);
         break;
      case '4':
         if(g_ClientDCL.RequestGameSpeedChange(c_fSP2HighestTimeSpeed))
            g_ClientDDL.MainBar()->UpdateSpeed(c_fSP2HighestTimeSpeed);
         break;
      // Show units list window
      case 'L':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.UnitListWindowNew())
               g_ClientDDL.UnitListWindowNewSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.UnitListWindowNew()->Visible())
            {
               g_ClientDDL.UnitListWindowNew()->UpdateUnitList(g_ClientDAL.ControlledCountryID(), -1);
               g_ClientDDL.UnitListWindowNew()->Show();
            }
            else
            {
               g_ClientDDL.UnitListWindowNew()->Hide();
            }
         }
         break;
      // Show deploy window
      case 'D':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.UnitGroundDeploymentWindow())
               g_ClientDDL.UnitGroundDeploymentWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.UnitGroundDeploymentWindow()->Visible())
            {
               g_ClientDDL.UnitGroundDeploymentWindow()->Show();
            }
            else
            {
               g_ClientDDL.UnitGroundDeploymentWindow()->Hide();
            }
         }
         break;
      // Show build window
      case 'U':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.UnitProductionWindow())
               g_ClientDDL.UnitProductionWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.UnitProductionWindow()->Visible())
            {
               g_ClientDDL.UnitProductionWindow()->Show();
            }
            else
            {
               g_ClientDDL.UnitProductionWindow()->Hide();
            }
         }
         break;
      // Show covert ops window
      case 'C':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.CovertActionsWindow())
               g_ClientDDL.CovertActionsWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.CovertActionsWindow()->Visible())
            {
               g_ClientDDL.CovertActionsWindow()->Update();
               g_ClientDDL.CovertActionsWindow()->Show();
            }
            else
            {
               g_ClientDDL.CovertActionsWindow()->Hide();
            }
         }
         break;
      // Show strategic window
      case 'S':
         {
            if(!g_ClientDDL.StrategicTargetCountryWindow())
               g_ClientDDL.SwapToStrategicWarfare();
            else
               g_ClientDDL.SwapToMilitary();
         }
         break;
      // Show war list window
      case 'W':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.WarListWindow())
               g_ClientDDL.WarListWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.WarListWindow()->Visible())
            {
               g_ClientDDL.WarListWindow()->Show();
            }
            else
            {
               g_ClientDDL.WarListWindow()->Hide();
            }
         }
         break;
      // Show budget window
      case 'B':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.BudgetWindow())
               g_ClientDDL.BudgetWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.BudgetWindow()->Visible())
            {
               g_ClientDDL.BudgetWindow()->Update();
               g_ClientDDL.BudgetWindow()->Show();
            }
            else
            {
               g_ClientDDL.BudgetWindow()->Hide();
            }
         }
         break;
      // Show resources window
      case 'R':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.ResourcesWindow())
               g_ClientDDL.ResourcesWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.ResourcesWindow()->Visible())
            {
               g_ClientDDL.ResourcesWindow()->Update();
               g_ClientDDL.ResourcesWindow()->Show();
            }
            else
            {
               g_ClientDDL.ResourcesWindow()->Hide();
            }
         }
         break;
      // Show treaties window
      case 'T':
         {
            // spawn if doesnt exists
            if(!g_ClientDDL.TreatiesWindow())
               g_ClientDDL.TreatiesWindowSpawn();
            // toggle displayed or not
            if(!g_ClientDDL.TreatiesWindow()->Visible())
            {
               g_ClientDDL.TreatiesWindow()->Update();
               g_ClientDDL.TreatiesWindow()->Show();
            }
            else
            {
               g_ClientDDL.TreatiesWindow()->Hide();
            }
         }
         break;
      default:
         return EEventStatus::NotHandled;
   }
   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBackgroundEarth::OnMouseMove(const GEventData & in_EventData, 
                                                      GUI::GBaseObject* in_pCaller)
{
   if (m_CancelAllMouseEventsUntilAllButtonsUp)
   {
      if (!in_EventData.Mouse.Down.Field)
        m_CancelAllMouseEventsUntilAllButtonsUp = false;
      else
         return EEventStatus::Handled;
   }

   if(in_pCaller || in_EventData.InitialRecipient != this)
      return EEventStatus::Handled;

   m_pCityOverlay->ShowCityName(in_EventData.Mouse.Position);


   // If a specific key is pressed, show the Sphere control
   if(HIWORD(GetKeyState(c_DefaultSphereWindowControlKey)) & 1)
   {
      ShowSphereControlWindow(in_EventData.Mouse.Position);
   }
   else
   {
      HideSphereControlWindow();
   }

   if(in_EventData.Mouse.Down.Bits.Right)
   {
      if (!m_bEarthDragging)
      {
         if ((abs(m_DragCoord.x - in_EventData.X()) < GetSystemMetrics(SM_CXDRAG)) && ((abs(m_DragCoord.y - in_EventData.Y()) < GetSystemMetrics(SM_CYDRAG))))
            return GUI::EEventStatus::Handled;
         m_bEarthDragging = true;   
         Drag(1);         
      }
   }
   else
   {
      m_bEarthDragging = false;
   }

   if(in_EventData.Mouse.Down.Bits.Left)
   {
      if (!m_bUnitDragging && m_UnitToDrag)
      {
         if ((abs(m_DragCoord.x - in_EventData.X()) < GetSystemMetrics(SM_CXDRAG)) && ((abs(m_DragCoord.y - in_EventData.Y()) < GetSystemMetrics(SM_CYDRAG))))
            return GUI::EEventStatus::Handled;
         m_bUnitDragging = true;
         m_UnitToKeepOnDeferDeselect = 0;
         m_UnitToDeferDeselect = 0;
         g_Joshua.GUIManager()->SetMousePointer(m_PointerMoveUnit);         
         Drag();
      }
      else if (!m_bSelectBox)
      {
         if ((abs(m_DragCoord.x - in_EventData.X()) < GetSystemMetrics(SM_CXDRAG)) && ((abs(m_DragCoord.y - in_EventData.Y()) < GetSystemMetrics(SM_CYDRAG))))
            return GUI::EEventStatus::Handled;
         m_OrigSelectedGroups = g_ClientDAL.SelectedUnitsID();
         m_bSelectBox = true;
         m_pDragBoxTransform->Position().X = (REAL32)m_ClickPosition.x;
         m_pDragBoxTransform->Position().Y = (REAL32)m_ClickPosition.y;
         m_pDragBoxTransform->Size().X = 0;
         m_pDragBoxTransform->Size().Y = 0;
         m_pDragBoxModel->Visibility() = true;
         Drag();
      }
   }

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBackgroundEarth::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if (m_CancelAllMouseEventsUntilAllButtonsUp)
   {
      if (!in_EventData.Mouse.Down.Field)
        m_CancelAllMouseEventsUntilAllButtonsUp = false;
      return EEventStatus::Handled;
   }

   if(in_EventData.Mouse.Actor.Bits.Right)
   {
      m_bEarthDragging = false;
   }
   else if(in_EventData.Mouse.Actor.Bits.Left)
   {
      if (m_bUnitDragging)
      {
         bool l_bClickedOnEarth = ScreenToLongLat(in_EventData.Mouse.Position, m_ClickLongLat);
         g_ClientDDL.UnitInfoSmallWindow()->Show();
         g_ClientDDL.ResetMousePointer();
         if (l_bClickedOnEarth)
         {
            vector<UINT32> l_vMovedUnits;
            for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
            {
               l_vMovedUnits.push_back(*itr);
            }
            g_ClientDCL.RequestUnitMove(l_vMovedUnits, m_ClickLongLat, EMilitaryAction::Default);
         }
      }
      if (m_bSelectBox)
      {
         m_bSelectBox = false;
         m_pDragBoxModel->Visibility() = false;

         if((in_EventData.Keyboard.Alt) && !Locked())
         {
            GVector2D<INT32> l_PositionTL((INT32)m_pDragBoxTransform->Position().X, (INT32)m_pDragBoxTransform->Position().Y);
            GVector2D<INT32> l_PositionBR(in_EventData.Mouse.Position);
            if (l_PositionTL.x > l_PositionBR.x)
               swap(l_PositionTL.x, l_PositionBR.x);
            if (l_PositionTL.y > l_PositionBR.y)
               swap(l_PositionTL.y, l_PositionBR.y);

            // Get the current resolution
            g_Joshua.Renderer()->Get_Current_Mode(&m_Mode);

            GVector3D<REAL32> l_Center;
            l_Center.x = (REAL32)(l_PositionTL.x + l_PositionBR.x) / 2.f;
            l_Center.y = (REAL32)(l_PositionTL.y + l_PositionBR.y) / 2.f;

            // Transform drag box center from window coordinates to
            // background earth coordinates
            l_Center.x =  ( ( ( 2.0f * l_Center.x ) / (REAL32) m_Mode.m_Resolution.x ) - 1.f ) * ((*m_pfCamDist) + 1) / 1.8106600f;
            l_Center.y = -( ( ( 2.0f * l_Center.y ) / (REAL32) m_Mode.m_Resolution.y ) - 1.f ) * ((*m_pfCamDist) + 1) / 2.4142134f;
            l_Center.z =  0.0f;

            // Find intersection with sphere
            REAL32 d2 = ((*m_pfCamDist) + 1) * ((*m_pfCamDist) + 1);
            REAL32 x2 = l_Center.x * l_Center.x;
            REAL32 y2 = l_Center.y * l_Center.y;
            REAL32 a = x2 + y2 + d2;
            REAL32 b = -2 * d2;
            REAL32 c = d2 - 1;

            // Make sure closest point is inside background earth
            if( (b * b - 4 * a * c) < 0)
               return GUI::EEventStatus::Handled;

            REAL32 t0 = (-b + sqrtf(b * b - 4 * a * c) ) / (2 * a);
            REAL32 t1 = (-b - sqrtf(b * b - 4 * a * c) ) / (2 * a);
            REAL32 t;
            t = (( t0 > 1) || (t0 < 1)) ? t1 : t0;

            GVector3D<REAL32> l_StartNT, l_Start;
            l_StartNT.x = t * l_Center.x;
            l_StartNT.y = t * l_Center.y;
            l_StartNT.z = t * ((*m_pfCamDist) + 1) - ((*m_pfCamDist) + 1);
            if(l_StartNT.z > 0)
            {
               MessageBox(NULL, L"ZOOM REGION MATH ERROR", L"ERROR", MB_OK);
            }

            // Transform point to absolute coordinate
            m_pEarthTransformTemp->Rotation().X = (*m_pfCamAngleY);
            m_pEarthTransformTemp->Rotation().Y = -((*m_pfCamAngleX) - (REAL32)Math::PI);
            m_pEarthTransformTemp->Update();
            GMatrix4x4<REAL32> l_Mat = m_pEarthTransformTemp->Get_Matrix();
            l_Start = l_Mat * l_StartNT;

            // Calculate the horizontal angle
            // Calculate the horizontal arc
            REAL32 l_Dist = ((*m_pfCamDist) + 1 + l_StartNT.z);
            REAL32 l_Xa = ( ( ( 2.0f * l_PositionTL.x ) / (REAL32) m_Mode.m_Resolution.x ) - 1.f ) * l_Dist / 1.8106600f;
            REAL32 l_Xb = ( ( ( 2.0f * l_PositionBR.x ) / (REAL32) m_Mode.m_Resolution.x ) - 1.f ) * l_Dist / 1.8106600f;
            REAL32 l_Ya = ( ( ( 2.0f * l_PositionTL.y ) / (REAL32) m_Mode.m_Resolution.y ) - 1.f ) * l_Dist / 2.4142134f;
            REAL32 l_Yb = ( ( ( 2.0f * l_PositionBR.y ) / (REAL32) m_Mode.m_Resolution.y ) - 1.f ) * l_Dist / 2.4142134f;
            REAL32 l_ArcH =  fabsf(l_Xb - l_Xa);
            REAL32 l_ArcV =  fabsf(l_Yb - l_Ya);

            REAL32 l_AngleH = atan2f(l_Start.x, -l_Start.z);
            REAL32 l_AngleV = ((REAL32)-Math::PI * 0.5f) + acosf(-l_Start.y);

            // Calculate optimal camera in x and y distance
            //          REAL32 l_MaxDist = sqrtf(1.8106600f * 1.8106600f + 1) - 1;
            //          REAL32 l_MaxAngle = asinf( (l_MaxDist * l_MaxDist + 2 * l_MaxDist) / (2 * l_MaxDist + 2) ) * 2;
            REAL32 l_CamDistX = sinf( fabsf(l_ArcH / 2) ) * 1.8106600f * 1.25f - 1 + cosf(l_ArcH / 2);            
            REAL32 l_CamDistY = sinf( fabsf(l_ArcV / 2) ) * 2.4142134f * 1.25f - 1 + cosf(l_ArcV / 2);

            // Keep maximum camera distance
            (*m_pfCamDist) = l_CamDistX;
            if(l_CamDistX < l_CamDistY)
               (*m_pfCamDist) = l_CamDistY;

            (*m_pfCamAngleX) = l_AngleH;
            (*m_pfCamAngleY) = l_AngleV;

            // Update camera & mini-map with computed zoom region
            SEarthState l_State = g_ClientDCL.m_EarthState;
            l_State.m_Zoom = ( (logf((*m_pfCamDist)) / logf(2) ) - m_fDistCalcB) / m_fDistCalcA;
            l_State.m_Zoom = min(1, l_State.m_Zoom);
            l_State.m_Latitude = (*m_pfCamAngleY);
            l_State.m_Longitude = (*m_pfCamAngleX);
            g_ClientDCL.m_EarthState = l_State;
            g_ClientDCL.UpdateWorldPosition(l_State);

            if(m_pCameraAction)
            {
               UINT32 l_iID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pCameraAction);
               g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iID);
            }

            g_SP2Client->DDL().MiniMap()->UpdateZoomBar();            
         }

         if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
         {
            if(g_ClientDAL.SelectedUnitsID().size())
            {
               if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
               {
                  SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );                  
                  g_ClientDDL.UnitListWindowNew()->UpdateUnitList( (UINT16)l_pGroup->OwnerId(), l_pGroup->Id() );
               }
            }
         }
      }
      m_bUnitDragging = false;
      m_UnitToDrag = 0;
      m_bUnitPicked = false;
   }

   return EEventStatus::Handled;
}

/*!
* Mouse click HANDLER
*   Uses the m_pSelectedCtryWindow object to display the actually selected country
*  @param  The usual parameters for a mouse event
*  @return true if the was handled.
**/
GUI::EEventStatus::Enum GBackgroundEarth::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   //If the user click a notification sign hide it  
   GNotificationSignNode l_Iter = m_Signs.begin();
   while(l_Iter != m_Signs.end())
   {
      GVector2D<INT32> l_Pos  = l_Iter->first.m_pWindow->Position();
      GVector2D<INT32> l_Size = l_Iter->first.m_pWindow->Size();

      if( ( in_EventData.X() >= l_Pos.x && in_EventData.X() <= (l_Pos.x + l_Size.x) ) && ( in_EventData.Y() >= l_Pos.y && in_EventData.Y() <= (l_Pos.y + l_Size.y) ))
      {
         
         RemoveNotificationSignOnEarth( *( (GNotificatonSign*) (&l_Iter) ));
         
         return EEventStatus::Handled;
      }

      ++l_Iter;
   }

   if( in_EventData.InitialRecipient->Name() == c_SignWindowResName || in_EventData.InitialRecipient->Name() == c_SignWindowResTextName )
   {
      if(in_EventData.InitialRecipient->Name() == c_SignWindowResTextName)
      {
         in_EventData.InitialRecipient->Owner()->Hide();
      }
      else
      {
         in_EventData.InitialRecipient->Hide();
      }

      return EEventStatus::Handled;
   }

   if (m_CancelAllMouseEventsUntilAllButtonsUp || (m_ClickPosition.Distance(in_EventData.Mouse.Position) > 2))
      return EEventStatus::Handled;

   if(m_pReturnWindow && in_EventData.Mouse.Actor.Bits.Left || m_bUnitDragging)
      return GUI::EEventStatus::Handled;

   if (in_EventData.Mouse.Actor.Bits.Right)
   {

      if (!m_bEarthDragging && !m_bUnitDragging && !m_bUnitPicked)
      {
         //Checked if a unit was clicked at the position
         UINT32 l_ClickedUnitID = FindVisibleUnitAt(in_EventData.Mouse.Position);
         if (l_ClickedUnitID && (g_ClientDAL.SelectedUnitsID().find(l_ClickedUnitID) != g_ClientDAL.SelectedUnitsID().end()))
         {
            SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_ClickedUnitID);
            if (l_pUnitGroup->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID())
            {
               if(l_pUnitGroup->Status() == EMilitaryStatus::InDeployement)
               {
                  ShowDeployementMenu();
               }
               else
               {
                  ShowMilitaryMenu();
               }
            }
         }
         //Show the standard context menu
         else
         {
            ShowContextMenu();
         }
      }
      else if (!m_bEarthDragging && (m_bUnitPicked || m_bUnitDragging))
      {
         m_bUnitDragging = m_bUnitPicked = false;         
         g_ClientDDL.ResetMousePointer();
      }
      else if(m_bSelectBox)
      {
         m_bSelectBox = false;
         m_pDragBoxModel->Visibility() = false;
         if (!in_EventData.Keyboard.Alt)
         {
            g_ClientDAL.UnselectUnits();
            if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
               g_ClientDDL.UnitListWindowNew()->Hide();
         }
      }

      return GUI::EEventStatus::Handled;
   }

   if (in_EventData.Mouse.Actor.Bits.Left)
   {   
      if (m_UnitToKeepOnDeferDeselect)
      {
         g_ClientDAL.SelectUnit(m_UnitToKeepOnDeferDeselect);
         m_UnitToKeepOnDeferDeselect = 0;
      }
      if (m_UnitToDeferDeselect)
      {
         g_ClientDAL.UnselectUnit(m_UnitToDeferDeselect);         
         g_ClientDDL.UnitInfoSmallWindow()->Hide();
         m_UnitToDeferDeselect = 0;
      }


      if (!m_bUnitPicked && !m_bSelectBox)
      {
         UINT32 l_ClickedUnit = FindVisibleUnitAt(in_EventData.Mouse.Position);      
         if (!l_ClickedUnit)
         {
            UINT32 l_ClickedCombat = FindVisibleCombatAt(in_EventData.Mouse.Position);
            if (!l_ClickedCombat)
            {
               g_ClientDCL.KillCombatInfo();
               g_ClientDAL.UnselectUnits();
               if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
                  g_ClientDDL.UnitListWindowNew()->Hide();
               g_ClientDDL.UnitInfoSmallWindow()->Hide();
               SelectCountryAt(m_ClickLongLat);
            }
         }
      }		
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GBackgroundEarth::OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   // Only allow zoom with mouse wheel when not zooming and no button pressed
   if(in_pCaller || in_EventData.InitialRecipient != this)
      return GUI::EEventStatus::Handled;

   if(!m_bSelectBox && !in_EventData.Mouse.Down.Field)
      g_SP2Client->DDL().MiniMap()->OnMouseWheel(in_EventData, in_pCaller);

   return GUI::EEventStatus::Handled;
}

bool GBackgroundEarth::SelectCountryAt(const GVector2D<REAL32> & in_LongLat)
{
   UINT32 l_iID      = g_ClientDCL.EarthLocateRegion(in_LongLat.x, in_LongLat.y);

   //If the selected area is invalid
   if( (l_iID == 0) || (m_iLastRegionSelected == (INT32)l_iID))
      return false;

   m_iLastRegionSelected = l_iID;

   //Get the owner of the country
   UINT32 l_iCountryID = g_ClientDAL.RegionControl(m_iLastRegionSelected).m_iMilitary;
   if(l_iCountryID > 0)
   {
      const GCountry& l_Country = g_ClientDAL.Country(l_iCountryID);
      g_ClientDCL.SelectedCountrySet(l_Country);
   }

   return true;
}

void GBackgroundEarth::SwitchView(EBackgroundView::Enum in_eNewView, REAL32 in_fTime)
{
   if( (in_eNewView != m_eView) && (in_eNewView < EBackgroundView::Count) )
   {
      m_pScene->Get_Animation_System()->Remove_Track(m_pTransitionViewTrack);
      if(in_eNewView == EBackgroundView::Realistic)
      {
         m_pTransitionViewTrack->Playback(GFX::PLAYBACK_REVERSE);
      }
      else
      {
         m_pTransitionViewTrack->Playback(GFX::PLAYBACK_FORWARD);
      }
      m_pTransitionViewTrack->Set_Length(in_fTime);
      m_pTransitionViewTrack->Set_Start_Time(m_pScene->Get_Animation_System()->Get_Animation_Time() );

      m_pScene->Get_Animation_System()->Add_Track(m_pTransitionViewTrack);
      m_eView = in_eNewView;
   }
}

bool GBackgroundEarth::Animate()
{
   REAL32 l_Time = m_pScene->Get_Animation_System()->Get_Animation_Time();

   // Rotate clouds around earth
   m_pCloudTransform->Rotation().Y = l_Time * c_CloudSpeed;

   // Rotate sun around earth (yep, earth is once again center of universe ;)
   REAL32 l_fDayInc = c_DaySpeed * (l_Time - m_fLastTime);
   m_fDayFactor = fmodf(m_fDayFactor + l_fDayInc, 1);
   m_fYearFactor = fmodf(m_fYearFactor + l_fDayInc * c_YearSpeed, 1);
   //   m_fDayFactor = fmodf(g_Joshua.GameTime(), 1);
   //   m_fYearFactor = fmodf(g_Joshua.GameTime() * c_YearSpeed, 1);
   UpdateSun();

   // Rotate moon around earth
   UpdateMoon();

   m_fLastTime = l_Time;

   SEarthState l_ZoomState;
   l_ZoomState.m_Zoom = ( (logf((*m_pfCamDist)) / logf(2) ) - m_fDistCalcB) / m_fDistCalcA;
   l_ZoomState.m_Longitude = (*m_pfCamAngleX - (REAL32)Math::PI);
   l_ZoomState.m_Latitude  = (*m_pfCamAngleY);

   g_ClientDCL.UpdateWorldPosition(l_ZoomState);
   g_ClientDCL.m_EarthState = l_ZoomState;

   if(m_bCamDirty)
   {
      UpdateCamera();
   }

   if(m_pCameraAction)
   {
      UINT32 l_iID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pCameraAction);
      REAL32 l_iActionTime   = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_iID);
      REAL32 l_iActionLength = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action(l_iID)->Length();
      if(l_iActionTime >= l_iActionLength)
         g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iID);
   }

   // Display flares (if necessary)
   UpdateFlares();

   UpdateClouds();

   // Update units display
   UpdateUnits();

   // Update nuke display
   UpdateNukes();

   // Update atlas txtr generation
   m_pAtlasEarth->UpdateContent();

   REAL64 l_fTime = g_Joshua.Clock();
   
   GNotificationSignNode l_Iter = m_Signs.begin();
   while(l_Iter != m_Signs.end())
   {
      if(l_fTime > l_Iter->second)
      {
         l_Iter = RemoveNotificationSignOnEarth( *( (GNotificatonSign*) (&l_Iter) ) );
      }
      else
      {
         l_Iter++;
      }
   }

   return true;
}

bool GBackgroundEarth::MouseToEarth(GVector2D<INT32> in_Pos, GVector3D<REAL32>& out_EarthPos)
{
   // Get the current resolution
   g_Joshua.Renderer()->Get_Current_Mode(&m_Mode);

   // Transform mouse position from window coordinates to
   // background earth coordinates
   out_EarthPos.x =  ( ( ( 2.0f * (REAL32) in_Pos.x ) / (REAL32) m_Mode.m_Resolution.x ) - 1.f ) * ((*m_pfCamDist) + 1) / 1.8106600f;
   out_EarthPos.y = -( ( ( 2.0f * (REAL32) in_Pos.y ) / (REAL32) m_Mode.m_Resolution.y ) - 1.f ) * ((*m_pfCamDist) + 1) / 2.4142134f;
   out_EarthPos.z =  0.0f;

   // Find intersection with sphere
   REAL32 d2 = ((*m_pfCamDist) + 1) * ((*m_pfCamDist) + 1);
   REAL32 x2 = out_EarthPos.x * out_EarthPos.x;
   REAL32 y2 = out_EarthPos.y * out_EarthPos.y;
   REAL32 a = x2 + y2 + d2;
   REAL32 b = -2 * d2;
   REAL32 c = d2 - 1;

   // Make sure closest point is inside background earth
   if( (b * b - 4 * a * c) < 0)
      return false;

   REAL32 t0 = (-b + sqrtf(b * b - 4 * a * c) ) / (2 * a);
   REAL32 t1 = (-b - sqrtf(b * b - 4 * a * c) ) / (2 * a);
   REAL32 t;
   if( ( t0 > 1) || (t0 < 1) )
   {
      t = t1;
   }
   else
   {
      t = t0;
   }
   out_EarthPos.x = t * out_EarthPos.x;
   out_EarthPos.y = t * out_EarthPos.y;
   out_EarthPos.z = t * ((*m_pfCamDist) + 1) - ((*m_pfCamDist) + 1);
   if(out_EarthPos.z > 0)
   {
      MessageBox(NULL, L"ZOOM REGION MATH ERROR", L"ERROR", MB_OK);
      return false;
   }

   return true;
}

void GBackgroundEarth::Initialize()
{
   g_ClientDDL.UnitInfoSmallWindowSpawn();
   //   g_ClientDDL.UnitInfoSmallWindow()->SendToBack();
   g_SP2Client->DDL().MiniMapSpawn();
   m_pMiniMap = g_SP2Client->DDL().MiniMap();


   // Create Sphere Control Window
   g_ClientDDL.SphereControlWindowSpawn();

   // Update initial zoom of minimap
   g_ClientDCL.m_EarthState.m_Zoom = 0;
   g_ClientDCL.m_EarthState.m_Latitude = 0;
   g_ClientDCL.m_EarthState.m_Longitude = 0;
   g_ClientDCL.UpdateWorldPosition(g_ClientDCL.m_EarthState);

   UpdateCamera();
}

/*!
* Outline the selected country
* @param in_vFaceID: Vector of IDs of the faces containing the country to display
* @return true if function was succesful.
**//*
bool GBackgroundEarth::OutlineCountry(vector<UINT32> in_viRegionIDs)
{
// Make sure at least one face needs to be displayed
if(in_viRegionIDs.size() == 0)
{
return false;
}

// Fetch a country to display
GFaceList<GPolygon> l_CountryData;

Map::GMultifaceView* l_pView = g_ClientDCL.CountryView();
if(!l_pView)
{
return false;
}
GShape& l_Shape = m_pOutlineMesh->Shape();

GMultiFacetList l_ExteriorContours;
GMultiFacetList l_InteriorContours;
l_pView->BuildFaceList(in_viRegionIDs,
l_Shape,
l_CountryData,
l_ExteriorContours,
l_InteriorContours);

m_pOutlineMesh->NbFacetGroups(0);
m_pOutlineMesh->NbFacetGroups(l_ExteriorContours.NbFacetLists() );

// Add border color
l_Shape.NbColors(1);
l_Shape.Color(0) = m_OutlineColor;

// Add each country border to the mesh
for(UINT32 i = 0;i < l_ExteriorContours.NbFacetLists();i ++)
{
// Add outline for current face
m_pOutlineMesh->FacetGroup(i).Facets().Type(FACETS_LINE_STRIP, false);

m_pOutlineMesh->FacetGroup(i).Facets().Start() = l_ExteriorContours.FacetGroup(i).Start();
m_pOutlineMesh->FacetGroup(i).Facets().Count() = l_ExteriorContours.FacetGroup(i).Count();

m_pOutlineMesh->FacetGroup(i).Colors().Type(FACETS_LINE_STRIP, true);
m_pOutlineMesh->FacetGroup(i).Colors().NbIndexes(m_pOutlineMesh->FacetGroup(i).Facets().Count() );

for(UINT32 t = 0;t < m_pOutlineMesh->FacetGroup(i).Colors().NbIndexes();t ++)
{
m_pOutlineMesh->FacetGroup(i).Colors().Index(t) = 0;
}
}
// Transform outline coordinates from spherical to rectangular
for(UINT32 p = 0;p < l_Shape.NbPositions();p ++)
{
GVector3D<REAL32>& l_Pos = l_Shape.Position(p);
REAL32 l_fPhi = l_Pos.x * (REAL32) Math::PI / 180.f;
REAL32 l_fTheta = (90 - l_Pos.y) * (REAL32) Math::PI / 180.f;

l_Pos.z = - sinf(l_fTheta) * sinf(l_fPhi);
l_Pos.y = cosf(l_fTheta);
l_Pos.x = - sinf(l_fTheta) * cosf(l_fPhi);
}

return true;
}
*/
bool GBackgroundEarth::SetRegionColor(UINT32 in_iRegionID, const GColorRGBReal& in_Color, bool in_Animate)
{
   m_pAtlasEarth->SetRegionColor(in_iRegionID, in_Color);
   if(in_iRegionID < m_pFillMesh->NbFacetGroups() )
   {
      m_vRegionRefCount[in_iRegionID] &= 0x7F;
      if(m_vRegionRefCount[in_iRegionID] )
      {
         m_pFillMesh->FacetGroup(in_iRegionID).Visibility() = 1.f;
      }

      REAL32 l_fAlpha = m_vRegionColorParams[in_iRegionID]->A;
      *(m_vRegionColorParams[in_iRegionID] ) = in_Color;
      m_vRegionColorParams[in_iRegionID]->A = l_fAlpha;

      if (in_Animate)
      {
         CGFX_Action_Itf* l_Action = g_Joshua.Renderer()->Create_Action();

         GColorRGBReal l_HiColor(in_Color);
         GColorRGBReal l_LowColor(in_Color.R - 0.1f, in_Color.G - 0.1f, in_Color.B - 0.1f, 255);

         const INT32 l_MaxKeys = 5;
         REAL32  l_Times[l_MaxKeys] = {0, 0.25f, 0.75f, 2.25f, 2.75f};   
         GColorRGBReal * l_Colors[l_MaxKeys] = {&l_HiColor, &l_HiColor, &l_LowColor, &l_LowColor, &l_HiColor};   
         CGFX_Key_Linear l_SelectedCountryAnimationKeys[3][5];

         for (int i = 0; i < l_MaxKeys; i++)
         {
            l_SelectedCountryAnimationKeys[0][i].m_Time   = l_Times[i];
            l_SelectedCountryAnimationKeys[0][i].m_Value  = l_Colors[i]->R;
         }
         for (int i = 0; i < l_MaxKeys; i++)
         {
            l_SelectedCountryAnimationKeys[1][i].m_Time  = l_Times[i];
            l_SelectedCountryAnimationKeys[1][i].m_Value  = l_Colors[i]->G;
         }
         for (int i = 0; i < l_MaxKeys; i++)
         {
            l_SelectedCountryAnimationKeys[2][i].m_Time  = l_Times[i];
            l_SelectedCountryAnimationKeys[2][i].m_Value  = l_Colors[i]->B;
         }

         for (int i = 0; i < 3; i++)
         {      
            m_SelectedCountryLinearAnimation[i]->Setup_Keys(l_SelectedCountryAnimationKeys[i], l_MaxKeys);
            m_SelectedCountryAnimation[i]->Animation(m_SelectedCountryLinearAnimation[i]);
            l_Action->Add_Animation(m_SelectedCountryAnimation[i]);
         }

         l_Action->Length(l_Times[l_MaxKeys-1]);

         m_SelectedCountryTrack->Set_Action(l_Action);
         m_SelectedCountryAnimation[0]->AddParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_R);
         m_SelectedCountryAnimation[1]->AddParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_G);
         m_SelectedCountryAnimation[2]->AddParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_B);

         m_SelectedCountryTrack->Set_Start_Time(g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time());

         SAFE_RELEASE(l_Action);
      }
      else
      {
         m_SelectedCountryAnimation[0]->RemoveParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_R);
         m_SelectedCountryAnimation[1]->RemoveParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_G);
         m_SelectedCountryAnimation[2]->RemoveParamConnection(m_pFillMesh->FacetGroup(in_iRegionID).Override(0), GFX::PARAM_COLOR_FACTOR_B);
      }

      return true;
   }

   return false;
}

bool GBackgroundEarth::ClearRegionColor(UINT32 in_iRegionID)
{
   m_pAtlasEarth->ClearRegionColor(in_iRegionID);

   if(in_iRegionID < m_pFillMesh->NbFacetGroups() )
   {
      m_vRegionRefCount[in_iRegionID] |= 0x80;
      m_pFillMesh->FacetGroup(in_iRegionID).Visibility() = 0.f;

      return true;
   }

   return false;
}

bool GBackgroundEarth::ClearAllRegionColors()
{
   for(UINT32 f = 0;f < m_pFillMesh->NbFacetGroups();f ++)
   {
      m_vRegionRefCount[f] |= 0x80;
      m_pAtlasEarth->ClearRegionColor(f);
      m_pFillMesh->FacetGroup(f).Visibility() = 0.f;
   }
   m_SelectedCountryAnimation[0]->ClearParamConnections();
   m_SelectedCountryAnimation[1]->ClearParamConnections();
   m_SelectedCountryAnimation[2]->ClearParamConnections();

   return true;
}

bool GBackgroundEarth::DisplaySpotInformation(const vector<GSpotInfo>& in_vData)
{
   // Remove old display information
   m_pEarthModelMaster->Remove_Child(m_pDisplayParticlesModelMaster);
   SAFE_RELEASE(m_pDisplayParticlesModelMaster);

   // Create new display information master Model
   m_pDisplayParticlesModelMaster = g_Joshua.Renderer()->Create_Model();
   m_pEarthModelMaster->Add_Child(m_pDisplayParticlesModelMaster);

   // Create each particle using input information
   for(UINT32 i = 0;i < in_vData.size();i ++)
   {
      // Create particle Model
      CGFX_Model_Itf* l_pParticleModel = g_Joshua.Renderer()->Create_Model();
      CGFX_Transform_Itf* l_pParticleTransform = g_Joshua.Renderer()->Create_Transform();

      // Add Model to master
      m_pDisplayParticlesModelMaster->Add_Child(l_pParticleModel);

      // Initialize Model
      l_pParticleModel->Set_Transform(l_pParticleTransform);
      l_pParticleModel->Set_Material(m_pDisplayParticlesMaterial);
      l_pParticleModel->Set_Primitive(m_pDisplayParticlesMesh);

      // Create transform
      REAL32 l_fLatRad = -in_vData[i].m_Coord.y * (REAL32)Math::PI / 180.f;
      REAL32 l_fLongRad = (REAL32)Math::PI + (in_vData[i].m_Coord.x * (REAL32)Math::PI) / 180.f;

      GMatrix4x4<REAL32> l_MatA;
      l_MatA.LoadIdentity();
      l_MatA(0, 0) =  cosf(l_fLongRad);
      l_MatA(2, 0) = -sinf(l_fLongRad);
      l_MatA(0, 2) =  sinf(l_fLongRad);
      l_MatA(2, 2) =  cosf(l_fLongRad);
      GMatrix4x4<REAL32> l_MatB;
      l_MatB.LoadIdentity();
      l_MatB(0, 0) =  cosf(l_fLatRad);
      l_MatB(1, 0) =  sinf(l_fLatRad);
      l_MatB(0, 1) = -sinf(l_fLatRad);
      l_MatB(1, 1) =  cosf(l_fLatRad);
      GMatrix4x4<REAL32> l_Mat;
      l_Mat.LoadIdentity();

      l_Mat = l_MatB * l_MatA;
      //l_Mat(0, 0) = cosf(l_fLatRad) * cosf(l_fLongRad);
      //l_Mat(1, 0) = sinf(l_fLatRad);
      //l_Mat(2, 0) = cosf(l_fLatRad) * -sinf(l_fLongRad);
      //l_Mat(0, 1) = -sinf(l_fLatRad) * cos(l_fLongRad);
      //l_Mat(1, 1) = cosf(l_fLatRad);
      //l_Mat(2, 1) = -sinf(l_fLatRad) * -sinf(l_fLongRad);
      //l_Mat(0, 2) = sinf(l_fLongRad);
      //l_Mat(2, 2) = cosf(l_fLongRad);

      l_pParticleTransform->Set_Matrix(l_Mat);

      //! \todo TBA : Use material override
      // Update material color
      GFX::GDefaultVertexShader* l_pParticleVS;
      GFX::GCascadePixelShader* l_pParticlePS;
      GFX::GMaterial* l_pParticleMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR, &l_pParticleVS, &l_pParticlePS);
      l_pParticleModel->Set_Material(l_pParticleMaterial);

      GFX::GRenderStateCollection* l_pParticleRS = l_pParticleMaterial->RenderStates(0);
      //      l_pParticleRS->Value(GFX::RSTATE_FILL_MODE) = GFX::FILL_WIREFRAME;
      l_pParticleRS->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;

      l_pParticleRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pParticleRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pParticleRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;//GFX::BLEND_ONE;

      l_pParticleVS->Diffuse() = GColorRGBReal(0,0,0,1);
      l_pParticleVS->Specular() = GColorRGBReal(0,0,0,1);
      l_pParticleVS->Emissive() = in_vData[i].m_Color;
      l_pParticleVS->Ambient() = GColorRGBReal(0,0,0,1);

      l_pParticlePS->TextureSampler(0)->Texture(m_pDisplayParticlesMaterial->PixelShader(0)->TextureSampler(0)->Texture() );
      l_pParticlePS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_pParticlePS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_pParticlePS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

      //! \todo TBA : Use material override
      // Update size
      CGFX_Transform_2D_Itf* l_pParticleTxtrTransform = g_Joshua.Renderer()->Create_Transform_2D();
      REAL32 l_fSize = in_vData[i].m_fRadius;
      REAL32 l_fOffset = (1.f - l_fSize) * 0.5f;
      l_pParticleTxtrTransform->Position().X = -l_fOffset / l_fSize;
      l_pParticleTxtrTransform->Position().Y = -l_fOffset / l_fSize;
      l_pParticleTxtrTransform->Size().X = 1 / l_fSize;
      l_pParticleTxtrTransform->Size().Y = 1 / l_fSize;
      l_pParticlePS->Stage(0)->Transform(l_pParticleTxtrTransform);

      SAFE_RELEASE(l_pParticleVS);
      SAFE_RELEASE(l_pParticlePS);
      SAFE_RELEASE(l_pParticleMaterial);
      SAFE_RELEASE(l_pParticleTxtrTransform);
      SAFE_RELEASE(l_pParticleModel);
      SAFE_RELEASE(l_pParticleTransform);
   }

   return true;
}

void GBackgroundEarth::AddNotificationSignOnEarth(const GVector2D<REAL32>& in_Coordinates, const GString& in_NotificationMessage)
{
   static UINT32 l_ID = 0;
   l_ID ++;

   GNotificationSignData l_NewData;

   // Convert input spherical coordinates into rectangular coordinates
   l_NewData.m_Pos = ConvertSphericalToRectangular(in_Coordinates);

   // Create the sign objects
   l_NewData.m_pModel = g_Joshua.Renderer()->Create_Model();
   l_NewData.m_pModel->Set_Primitive(m_pSignMesh);
   l_NewData.m_pModel->Set_Material(m_pSignMaterial);

   // Create a transform for this sign
   l_NewData.m_pTrans = g_Joshua.Renderer()->Create_Transform_2D();
   l_NewData.m_pModel->Set_Transform(l_NewData.m_pTrans);

   // Add the sign to the earth object
   m_pNotificationParent->Add_Child(l_NewData.m_pModel);

   // Create the window for the sign
   l_NewData.m_pWindow = CREATE_GUI_OBJECT(c_SignWindowResName, GString(l_ID), m_pNotificationContainer, true, false);

   // Set the text of the sign
   GUI::GLabel* l_pLabel = (GUI::GLabel*) l_NewData.m_pWindow->Child(c_SignWindowResTextName);
   l_pLabel->Text(in_NotificationMessage);

   //! \todo TBA : Make sure text is not too big to fit in window

   // Add this new sign to our current list
   //GNotificationSignNode l_SignNode = 
   m_Signs.push_back( make_pair(l_NewData, g_Joshua.Clock() + c_iNotificationSignDisplayTime ) );

   // Update positions
   UpdateNotificationSign(l_NewData);
}

GBackgroundEarth::GNotificationSignNode GBackgroundEarth::RemoveNotificationSignOnEarth(GNotificatonSign in_SignIter)
{
   // Upcast the incoming iterator
   GNotificationSignNode l_SignNode = *( (GNotificationSignNode*) &in_SignIter);

   // Remove the sign "pole"
   m_pNotificationParent->Remove_Child(l_SignNode->first.m_pModel);

   // Delete the sign objects
   GBackgroundEarth::GNotificationSignData* l_Data = (GBackgroundEarth::GNotificationSignData*)&l_SignNode->first;

   SAFE_RELEASE(l_Data->m_pModel);
   SAFE_RELEASE(l_Data->m_pTrans);

   g_Joshua.GUIManager()->ReleaseObjectAsync(l_Data->m_pWindow);
   l_Data->m_pWindow = NULL;

   // Remove the node from the notification signs list
   return m_Signs.erase(l_SignNode);
}

void GBackgroundEarth::RemoveNotificationSigns()
{
   list< pair<GNotificationSignData, REAL64> >::iterator l_It = m_Signs.begin();
   
   while(l_It != m_Signs.end())
   {
      m_pNotificationParent->Remove_Child(l_It->first.m_pModel);

      // Delete the sign objects
      GBackgroundEarth::GNotificationSignData* l_Data = (GBackgroundEarth::GNotificationSignData*)&l_It->first;

      SAFE_RELEASE(l_Data->m_pModel);
      SAFE_RELEASE(l_Data->m_pTrans);

      g_Joshua.GUIManager()->ReleaseObjectAsync(l_Data->m_pWindow);
      l_Data->m_pWindow = NULL;

      l_It++;
   }

   m_Signs.clear();
}

void GBackgroundEarth::UpdateAllNotificationSigns()
{
   // Loop on all notification signs
   GNotificationSignNode l_SignNode = m_Signs.begin();

   while(l_SignNode != m_Signs.end() )
   {
      // Update current sign
      UpdateNotificationSign( l_SignNode->first );

      // Go to next sign
      ++ l_SignNode;
   }
}

void GBackgroundEarth::UpdateNotificationSign(const GNotificationSignData& in_Sign)
{
   // Fetch current
   bool l_bVisible = false;
   GVector3D<REAL32> l_SignPos = m_ViewMat * in_Sign.m_Pos;

   // Verify if sign is in front of earth
   if(l_SignPos.z < m_OriginPos.z - 1 / m_OriginPos.z)
   {
      // Project the sign position onto screen
      GVector3D<REAL32> l_SignViewportPos = (m_ProjMat * GVector4D<REAL32>(l_SignPos) ).ApplyWeigth();

      // Calculate the screen coordinate
      GVector3D<REAL32> l_SignScreenPos;
      l_SignScreenPos.x = (l_SignViewportPos.x + 1) * (REAL32) m_Mode.m_Resolution.x * 0.5f;
      l_SignScreenPos.y = (l_SignViewportPos.y - 1) * (REAL32) m_Mode.m_Resolution.y * -0.5f;

      // Make sure the screen coordinate is within the screen resolution
      if( (l_SignScreenPos.x >= 0) &&
         (l_SignScreenPos.x < m_Mode.m_Resolution.x) &&
         (l_SignScreenPos.y >= 0) &&
         (l_SignScreenPos.y < m_Mode.m_Resolution.y) )
      {
         // Update the sign position
         in_Sign.m_pTrans->Position() = GVector2D<REAL32>(l_SignScreenPos);
         l_bVisible = true;

         // Calculate the theorical window position
         REAL32 l_fNotificationWidth = (REAL32)in_Sign.m_pWindow->Width();
         REAL32 l_fNotificationHeigth = (REAL32)in_Sign.m_pWindow->Height();

         REAL32 l_fWindowPosX = l_SignScreenPos.x + min( max( ( (l_SignScreenPos.x - m_Mode.m_Resolution.x * 0.5f) * c_fSignFlipSlopeX), -c_fSignLength), c_fSignLength);
         REAL32 l_fWindowPosY = l_SignScreenPos.y - c_fSignLength - l_fNotificationHeigth * 0.5f;

         // Adjust the window position so it will never go outside of screen
         if(l_fWindowPosX < 0)
         {
            l_fWindowPosX = 0;
         }

         if(l_fWindowPosX+ l_fNotificationWidth > m_Mode.m_Resolution.x)
         {
            l_fWindowPosX= m_Mode.m_Resolution.x - l_fNotificationWidth;
         }

         if(l_fWindowPosY< 0)
         {
            l_fWindowPosY= 0;
         }

         if(l_fWindowPosY+ l_fNotificationHeigth > m_Mode.m_Resolution.y)
         {
            l_fWindowPosY= m_Mode.m_Resolution.y - l_fNotificationHeigth;
         }

         // Update the sign window position
         in_Sign.m_pWindow->Position((INT32)l_fWindowPosX, (INT32)l_fWindowPosY);

         in_Sign.m_pTrans->Size().X = l_fWindowPosX- l_SignScreenPos.x;
         in_Sign.m_pTrans->Size().Y = l_fWindowPosY- l_SignScreenPos.y + l_fNotificationHeigth * 0.5f;
      }
   }

   if(l_bVisible)
   {
      in_Sign.m_pModel->Visibility() = true;
      in_Sign.m_pWindow->Show();
   }
   else
   {
      in_Sign.m_pModel->Visibility() = false;
      in_Sign.m_pWindow->Hide();
   }
}

/*!
* Takes screen coordinates (x, y) and translates them onto earth coordinates
* returns false if coordinates of the screen are not on the earth
* @param in_iX: X screen coord
* @param in_iY: Y screen coord
* @param out_fLat: Latitude (resulting)
* @param out_fLong: Longitude (resulting)
**/
bool GBackgroundEarth::ScreenToLongLat(GVector2D<INT32> in_Pos, GVector2D<REAL32> & out_LongLat)
{
   //Find the region id of the selected area
   GVector3D<REAL32> l_Coord;
   if(!MouseToEarth(in_Pos, l_Coord))
      return false;

   m_pEarthTransformTemp->Rotation().X = (*m_pfCamAngleY);
   m_pEarthTransformTemp->Rotation().Y = -((*m_pfCamAngleX) - (REAL32)Math::PI);
   m_pEarthTransformTemp->Update();

   GMatrix4x4<REAL32> l_Mat = m_pEarthTransformTemp->Get_Matrix();
   l_Coord = l_Mat * l_Coord;
   out_LongLat.x = atan2f(l_Coord.x, -l_Coord.z) * 180 / (REAL32)Math::PI;
   out_LongLat.y = (((REAL32)-Math::PI * 0.5f) + acosf(-l_Coord.y) )* 180 / (REAL32)Math::PI;
   return true;
}

void GBackgroundEarth::CreateCameraAnimation(const GVector2D<REAL32> * in_pLocation)
{
   if(m_pCameraAction)
   {
      UINT32 l_iID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pCameraAction);
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iID);
      m_pCameraAction = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action(l_iID);
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pCameraAction);
      m_pCameraAction = NULL;
   }

   if(!g_ClientDAL.CapitalExists(g_ClientDCL.SelectedCountryID()))
   {
      return;
   }

   GVector2D<REAL32> l_CapitalPos;
   if(!in_pLocation)
      l_CapitalPos = g_ClientDAL.CapitalPosition(g_ClientDCL.SelectedCountryID() );
   else
   {
      if(in_pLocation->x >= -180.f && in_pLocation->y >= -180.f)
         l_CapitalPos = *in_pLocation;
      else
         return;
   }

   m_pCameraAction = g_Joshua.Renderer()->Create_Action();

   REAL32 l_fLongDelta = abs((REAL32)(l_CapitalPos.y * Math::PI / 180.f + Math::PI) - (REAL32)(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LONGITUDE)));
   REAL32 l_fLatDelta  = abs((REAL32)(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LATITUDE)) - (REAL32)(l_CapitalPos.x * Math::PI / 180.f));

   REAL32 l_fDistance  = sqrt((l_fLongDelta * l_fLongDelta) + (l_fLatDelta * l_fLatDelta));

   m_pCameraAction->Length(max((REAL32)(l_fDistance / 2),1.0f));

   CGFX_Animation_Linear_Itf*    l_LongitudeAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Key_Linear l_LongitudeKeys[2] =
   {
      {0.0f,                      (REAL32)(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LONGITUDE))},
      {m_pCameraAction->Length(), (REAL32)(l_CapitalPos.x * Math::PI / 180.f + Math::PI)},
   };
   l_LongitudeAnim->Setup_Keys(l_LongitudeKeys,2);
   l_LongitudeAnim->Connect_Custom(m_pCamera,CUSTOM_PARAM_LONGITUDE);
   m_pCameraAction->Add_Animation(l_LongitudeAnim);
   l_LongitudeAnim->Release();


   CGFX_Animation_Linear_Itf*    l_LatitudeAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Key_Linear l_LatitudeKeys[2] =
   {
      {0.0f,                      (REAL32)(m_pCamera->Custom_Parameter(CUSTOM_PARAM_LATITUDE))},
      {m_pCameraAction->Length(), (REAL32)(l_CapitalPos.y * Math::PI / 180.f)},
   };
   l_LatitudeAnim->Setup_Keys(l_LatitudeKeys,2);
   l_LatitudeAnim->Connect_Custom(m_pCamera,CUSTOM_PARAM_LATITUDE);
   m_pCameraAction->Add_Animation(l_LatitudeAnim);
   l_LatitudeAnim->Release();

   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Action(m_pCameraAction);

   m_pCameraAction->Release();
}

REAL32 GBackgroundEarth::PlayCameraAnimation()
{
   if(!m_pCameraAction)
      return 0.f;

   UINT32 l_iID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pCameraAction);

   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_iID, 0);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_iID);

   return m_pCameraAction->Length();
}

CGFX_Model_Itf* GBackgroundEarth::CreateFlarePlane(const GString& in_sFlareTxtr)
{
   // Create the Model that will contain the flare
   CGFX_Model_Itf* l_pModel = g_Joshua.Renderer()->Create_Model();

   // Create the material for the flare
   GFX::GDefaultVertexShader* l_pVShader;
   GFX::GCascadePixelShader* l_pPShader;
   GFX::GMaterial* l_pMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR, &l_pVShader, &l_pPShader);

   // Update render states
   GFX::GRenderStateCollection* l_pRStates = l_pMaterial->RenderStates(0);
   l_pRStates->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pRStates->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_ONE;
   l_pRStates->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCCOLOR;
   l_pRStates->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
   l_pRStates->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
   l_pRStates->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;

   // Update the pixel shader
   l_pPShader->Stage(0)->ColorOperation(GFX::OPERATOR_SELECT_ARG_1);
   l_pPShader->Stage(0)->AlphaOperation(GFX::OPERATOR_SELECT_ARG_1);

   // Load the flare texture
   GFile l_File;
   g_Joshua.FileManager()->File(in_sFlareTxtr, l_File);
   CGFX_Texture_Itf* l_pTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(l_File, false, in_sFlareTxtr);
   gassert(l_pTxtr,"Texture should be valid");

   // Update the texture sampler with flare texture
   l_pPShader->TextureSampler(0)->Texture(l_pTxtr);

   l_pPShader->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
   l_pPShader->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);

   l_pPShader->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);

   // Create the mesh for the flare
   GFX::GMesh* l_pMesh = g_Joshua.Renderer()->CreateMesh();

   const UINT32 NB_FLARE_VERTICES = 9;

   static REAL32 s_fPositions[NB_FLARE_VERTICES][3] =
   {
      {-2.f, -2.f, 0.f},
      { 0.f, -2.f, 0.f},
      { 2.f, -2.f, 0.f},
      {-2.f,  0.f, 0.f},
      { 0.f,  0.f, 0.f},
      { 2.f,  0.f, 0.f},
      {-2.f,  2.f, 0.f},
      { 0.f,  2.f, 0.f},
      { 2.f,  2.f, 0.f},
   };

   static REAL32 s_fUVs[NB_FLARE_VERTICES][2] =
   {
      { 1.f,  0.f},
      { 0.f,  0.f},
      { 1.f,  0.f},
      { 1.f,  1.f},
      { 0.f,  1.f},
      { 1.f,  1.f},
      { 1.f,  0.f},
      { 0.f,  0.f},
      { 1.f,  0.f},
   };

   const UINT32 NB_FLARE_TRIANGLES = 8;

   static UINT32 s_iTris[NB_FLARE_TRIANGLES][3] =
   {
      {0, 3, 4},
      {0, 4, 1},
      {1, 4, 5},
      {1, 5, 2},
      {3, 6, 7},
      {3, 7, 4},
      {4, 7, 8},
      {4, 8, 5},
   };

   // Update shape of the mesh
   GShape& l_pShape = l_pMesh->Shape();

   l_pShape.SetupPositions( (GVector3D<REAL32>*) s_fPositions, NB_FLARE_VERTICES);

   l_pShape.NbTxtrs(1);
   l_pShape.SetupUVCoords(0, (GVector2D<REAL32>*) s_fUVs, NB_FLARE_VERTICES);

   // Update facets of the mesh
   l_pMesh->NbFacetGroups(1);
   l_pMesh->FacetGroup(0).Facets().SetupIndexes( (UINT32*) s_iTris, NB_FLARE_TRIANGLES * 3);

   // Set all objects to the mesh
   l_pModel->Set_Primitive(l_pMesh);
   l_pModel->Set_Material(l_pMaterial);

   // Release temporary objects
   SAFE_RELEASE(l_pMesh);
   SAFE_RELEASE(l_pMaterial);
   SAFE_RELEASE(l_pVShader);
   SAFE_RELEASE(l_pPShader);
   SAFE_RELEASE(l_pTxtr);

   return l_pModel;
}


void GBackgroundEarth::HideAllCountryUnits()
{
   m_vCountryUnitsToDisplay.clear();
}

void GBackgroundEarth::DisplayCountryUnits(UINT32 in_iCountryID)
{
   // Find if country units are already displayed
   vector<UINT32>::iterator l_It = m_vCountryUnitsToDisplay.begin();
   while( (l_It != m_vCountryUnitsToDisplay.end() ) && (*l_It != in_iCountryID) )
   {
      ++ l_It;
   }

   // Country not already inserted, add it
   if(l_It == m_vCountryUnitsToDisplay.end() )
   {
      m_vCountryUnitsToDisplay.push_back(in_iCountryID);
   }
}

void GBackgroundEarth::HideCountryUnits(UINT32 in_iCountryID)
{
   // Find if country units are already displayed
   vector<UINT32>::iterator l_It = m_vCountryUnitsToDisplay.begin();
   while( (l_It != m_vCountryUnitsToDisplay.end() ) && (*l_It != in_iCountryID) )
   {
      ++ l_It;
   }

   // Country was found in list, remove it
   if(l_It != m_vCountryUnitsToDisplay.end() )
   {
      m_vCountryUnitsToDisplay.erase(l_It);
   }
}

bool GBackgroundEarth::IsCountryUnitsDisplayed(UINT32 in_iCountryID)
{
   bool l_bResult = false;

   // Find if country units are already displayed
   vector<UINT32>::iterator l_It = m_vCountryUnitsToDisplay.begin();
   while( (l_It != m_vCountryUnitsToDisplay.end() ) && (*l_It != in_iCountryID) )
   {
      ++ l_It;
   }

   if(l_It != m_vCountryUnitsToDisplay.end() )
   {
      l_bResult = true;
   }

   return l_bResult;
}

void GBackgroundEarth::DisplayCombats(bool in_bShow)
{
   m_bShowCombat = in_bShow;

   UpdateCombats();
}

bool GBackgroundEarth::DisplayCombats() const
{
   return m_bShowCombat;
}

void GBackgroundEarth::DisplayBombardments(bool in_bShow)
{
   m_bShowBombardment = in_bShow;

   UpdateBombardments();
}

bool GBackgroundEarth::DisplayBombardments() const
{
   return m_bShowBombardment;
}




void GBackgroundEarth::UpdateUnits()
{
   GRectangle<INT32> l_ClipWindow;
   l_ClipWindow.Left(0);
   l_ClipWindow.Top(0);
   l_ClipWindow.Right(m_Mode.m_Resolution.x - 1);
   l_ClipWindow.Bottom(m_Mode.m_Resolution.y - 1);
   REAL64 l_fTime = g_Joshua.GameTime();

	const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();

   UINT32 l_iSelectionCount = 0;
   UINT32 l_iLastSelection = 0;
   INT32 l_iLastSelectionID = -1;

   g_ClientDAL.VisibleUnits().clear();
   m_pUnitStatusDisplayModel->Visibility() = false;
   if(m_vCountryUnitsToDisplay.size() == 0)
   {
      // Hide units
      m_pUnitPathElemDisplayModel->Visibility() = false;
      m_pUnitTargetDisplayModel->Visibility() = false;
      m_pUnitDisplayModel->Visibility() = false;
      m_pUnitPathDisplayModel->Visibility() = false;
      m_pUnitProgressMasterModel->Visibility() = false;
      g_ClientDDL.UnitInfoSmallWindow()->Hide();

      return;
   }

   // Show units
   set<UINT32> l_iSelectedUnit;
   m_pUnitDisplayModel->Visibility() = true;
   m_pUnitPathDisplayModel->Visibility() = true;
   m_pUnitPathElemDisplayModel->Visibility() = true;
   m_pUnitProgressMasterModel->Visibility() = true;

   set<UINT32> l_DisplayedPathsIDs;
   UINT32 l_iCurrentPathChild  = 0;
   UINT32 l_iCurrentElemChild[EUnitFriendship::Count];
   Memory::Clear(l_iCurrentElemChild, EUnitFriendship::Count);
   UINT32 l_iCurrentDestChild = 0;

   // Compute factors to convert from screen coordiantes to 3D coordinates
   GVector2D<REAL32> l_ScreenTransformScale = GVector2D<REAL32>(m_Mode.m_Resolution) * 0.5f;
   GVector2D<REAL32> l_ScreenTransformOffset(l_ScreenTransformScale);
   l_ScreenTransformScale.y *= -1;
   l_ScreenTransformOffset.x -= 0.5f;
   l_ScreenTransformOffset.y -= 0.5f;

   // Calculate threshold for unit display
   //   UINT32 l_Threshold = (UINT32) max(0, (m_OriginPos.z - m_fMinCamDist) * c_fMinUnitValue / m_fMaxCamDist);

   UINT32 l_iCurrentChild = 0;
   UINT32 l_iCurrentProgressChild = 0;
   for(UINT32 i = 0;i < m_vCountryUnitsToDisplay.size();i ++)
   {
      // Fetch the list of units for this country
      UINT32 l_iCountryID = m_vCountryUnitsToDisplay[i];
      const set<UINT32>& l_vGroupIDs = g_Joshua.UnitManager().CountryUnitGroups(l_iCountryID);

      const GCountry& l_Country = g_ClientDAL.Country(l_iCountryID);

      // Verify if this country is allied, enemy, neutral or are the player units
      EDiplomaticStatus::Enum l_eStatus = g_ClientDAL.DiplomaticStatus(g_ClientDAL.ControlledCountryID(), l_iCountryID);
      if(l_eStatus == EDiplomaticStatus::MilitaryAccess)
         l_eStatus = EDiplomaticStatus::Neutral;

      EUnitFriendship::Enum l_UnitFriendship           = (l_iCountryID == (UINT32) g_ClientDAL.ControlledCountryID() ) ?  EUnitFriendship::Player : (EUnitFriendship::Enum) l_eStatus;
      GFX::GOverrideTSampler* l_pUnitTS                = NULL;
      GFX::GOverrideTSampler* l_pNavalUnitTS           = NULL;
      GFX::GOverrideTSampler* l_pNavalBombardingUnitTS = NULL;

      GFX::GMesh* l_pUnitMesh = NULL;
      
      //Set the texture override sampler to use
      if(!m_bDisplayUnitsAsFlags)
      {
         l_pUnitTS                = m_pUnitTSamplers[l_UnitFriendship];
         l_pNavalUnitTS           = m_pUnitShipsTSamplers[l_UnitFriendship];
         l_pUnitMesh              = m_pUnitMeshes[l_UnitFriendship];
         l_pNavalBombardingUnitTS = m_pUnitShipsBombardingTSamplers[l_UnitFriendship];
      }
      else
      {
         //Check if the texture override already exists for that country
         if(m_vUnitFlagsTSamplers.size() <= l_iCountryID)
         {
            //Resize the vector, set all the new entries to null
            UINT32 l_iPreviousSize = m_vUnitFlagsTSamplers.size();
            m_vUnitFlagsTSamplers.resize(l_iCountryID+1);
            for(UINT32 i = l_iPreviousSize ; i< m_vUnitFlagsTSamplers.size() ; i++)
            {
               m_vUnitFlagsTSamplers[i] = NULL;
            }
         }
         //Check if the texture override exists
         if(m_vUnitFlagsTSamplers[l_iCountryID] == NULL)
         {
            CGFX_Texture_Itf* l_pOverrideTxtr =
               g_Joshua.Renderer()->Helper()->LoadTexture(GString(FLAG_SMALL_DEFAULT_FOLDER) + l_Country.Flag(), true);

            if(!m_pUnitFlagMesh)
            {
               m_pUnitFlagMesh = g_Joshua.Renderer()->CreateMesh();
               g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pUnitFlagMesh, l_pOverrideTxtr);
            }

            m_UnitFlagSize.x = l_pOverrideTxtr->PartialWidth();
            m_UnitFlagSize.y = l_pOverrideTxtr->PartialHeight();
            m_UnitFlagHalfSize  = m_UnitFlagSize / 2;

            m_vUnitFlagsTSamplers[l_iCountryID] = g_Joshua.Renderer()->CreateOverrideTSampler();
            m_vUnitFlagsTSamplers[l_iCountryID]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
            m_vUnitFlagsTSamplers[l_iCountryID]->OriginalTextureSamplerID(0);
            m_vUnitFlagsTSamplers[l_iCountryID]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);
            l_pOverrideTxtr->Release();
         }//end texture creation

         l_pUnitTS = m_vUnitFlagsTSamplers[l_iCountryID];
         l_pUnitMesh = m_pUnitFlagMesh;
      }

      // For each unit for this country, change its information (location) to reflect a visible unit group
      set<UINT32>::const_iterator l_GroupIt = l_vGroupIDs.begin();
      while(l_GroupIt != l_vGroupIDs.end() )
      {
         const SP2::GUnitGroup * l_Unit = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupIt);

         if(l_Unit->Status( (REAL32) l_fTime) < EMilitaryStatus::CountVisible)
         {
            bool l_bSelected = false;
            if(g_ClientDAL.SelectedUnitsID().find(l_Unit->Id()) != g_ClientDAL.SelectedUnitsID().end())
            {
               l_bSelected = true;
               l_iSelectionCount++;
               l_iLastSelection = l_Unit->Id();
            }

            // Verify if unit is moving
            const GUnitPath& l_Path = l_Unit->Path();
            if((l_Path.Points().size() > 0))
            {
               l_DisplayedPathsIDs.insert(l_Unit->Id() );

               // Verify if path was already created for that unit && path has not changed
               bool l_bUpdatePath = false;
               GUnitPathMapIter l_UnitPath = m_UnitPaths.find(l_Unit->Id() );
               if(l_UnitPath == m_UnitPaths.end() )
               {
                  l_bUpdatePath = true;
               }
               else if(l_UnitPath->second.second != l_Path.Id() )
               {
                  l_bUpdatePath = true;
               }

               if(l_bUpdatePath)
               {
                  CGFX_Model_Itf* l_pUnitPathModel = NULL;
                  GFX::GMesh* l_pUnitPathMesh = NULL;
                  if(l_UnitPath != m_UnitPaths.end() )
                  {
                     // Path has changed, modify it
                     l_pUnitPathModel = l_UnitPath->second.first;
                     l_pUnitPathModel->Inc_Ref();

                     l_pUnitPathMesh = (GFX::GMesh*) l_UnitPath->second.first->Get_Primitive();
                     l_pUnitPathMesh->Inc_Ref();

                     l_UnitPath->second.second = l_Path.Id();
                  }
                  else
                  {
                     // Path does not exist, create it
                     // Find a non-visible Model to show this new path
                     while(l_iCurrentPathChild < m_pUnitPathDisplayModel->Get_Nb_Children() )
                     {
                        if(m_pUnitPathDisplayModel->Get_Child(l_iCurrentPathChild)->Visibility() == 0)
                        {
                           // A non-visible Model was found, update its associated unit path
                           l_pUnitPathModel = m_pUnitPathDisplayModel->Get_Child(l_iCurrentPathChild);
                           l_pUnitPathModel->Inc_Ref();

                           l_pUnitPathMesh = (GFX::GMesh*) l_pUnitPathModel->Get_Primitive();
                           l_pUnitPathMesh->Inc_Ref();

                           break;
                        }
                        ++ l_iCurrentPathChild;
                     }

                     // Verify a non visible Model was found
                     if(l_pUnitPathModel == NULL)
                     {
                        // No Model is available, create a new one
                        l_pUnitPathModel = g_Joshua.Renderer()->Create_Model();
                        l_pUnitPathMesh = g_Joshua.Renderer()->CreateMesh();
                        GFX::GOverridePSCascade* l_pOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
                        CGFX_Transform_2D_Itf* l_pOverrideTfx = g_Joshua.Renderer()->Create_Transform_2D();

                        // Setup the Model
                        m_pUnitPathDisplayModel->Add_Child(l_pUnitPathModel);
                        l_pUnitPathModel->Set_Primitive(l_pUnitPathMesh);
                        l_pUnitPathModel->AddOverride(l_pOverride);
                        l_pOverride->AddUVTransformOverride(0, l_pOverrideTfx);
                        l_pOverride->UseColorFactorOverride(true);
                        l_pOverrideTfx->Size().X = 1;
                        l_pOverrideTfx->Size().Y = 1;

                        // Release temporary objects
                        SAFE_RELEASE(l_pOverride);
                        SAFE_RELEASE(l_pOverrideTfx);
                     }

                     // Associate this GFX path to the unit
                     m_UnitPaths.insert(pair<UINT32, GUnitPathInfo>(l_Unit->Id(), GUnitPathInfo(l_pUnitPathModel, l_Path.Id() ) ) );
                  }

                  l_pUnitPathModel->Visibility() = true;

                  // Update the mesh
                  GShape& l_Shape = l_pUnitPathMesh->Shape();
                  l_Shape.ClearAll();
                  l_Shape.NbTxtrs(1);
                  vector<GVector2D<REAL32> > l_Positions2D;

                  l_pUnitPathMesh->NbFacetGroups(0);
                  l_pUnitPathMesh->NbFacetGroups(1);
                  l_pUnitPathMesh->FacetGroup(0).Facets().Type(FACETS_LINE_STRIP, false);
                  GLineStrip& l_LineStrip = l_pUnitPathMesh->FacetGroup(0).Facets().LineStrip();

                  REAL32 l_fTotalDistInv = 1.f / l_Path.Points().back().m_fDistance;
                  REAL32 l_fPathDist = 0;
                  GUnitPathPoint l_PreviousPoint = l_Path.Points().front();

                  l_Positions2D.push_back(l_PreviousPoint.m_Position);
                  l_Shape.AddUVCoord(0, GVector2D<REAL32>(0, 0.5) );
                  for(UINT32 i = 1;i < l_Path.Points().size();i ++)
                  {
                     const GUnitPathPoint& l_CurrentPoint = l_Path.Points().at(i);
                     if(fabsf(l_CurrentPoint.m_Position.x - l_PreviousPoint.m_Position.x) < 270.f)
                     {
                        GVector2D<REAL32> l_Dir = l_CurrentPoint.m_Position - l_PreviousPoint.m_Position;
                        GVector2D<REAL32> l_Point = l_PreviousPoint.m_Position;
                        REAL32 l_fLength = l_Dir.Length();
                        if(l_fLength > 0)
                        {
                           UINT32 l_iNbDivs = (UINT32) (1 + (l_fLength / m_fMinPathDivLength) );
                           REAL32 l_fDivInc = 1.f / (REAL32) l_iNbDivs;
                           REAL32 l_fDistInc = (l_CurrentPoint.m_fDistance - l_PreviousPoint.m_fDistance) * l_fDivInc;

                           GVector2D<REAL32> l_LineInc(l_Dir.x * l_fDivInc, l_Dir.y * l_fDivInc);

                           // Compute remaining positions & create triangles
                           for(UINT32 j = 0;j < l_iNbDivs;j ++)
                           {
                              l_fPathDist += l_fDistInc;
                              l_Point += l_LineInc;

                              l_Positions2D.push_back(l_Point);
                              l_Shape.AddUVCoord(0, GVector2D<REAL32>(l_fPathDist * l_fTotalDistInv, 0.5) );
                           }
                        }
                     }

                     l_PreviousPoint = l_CurrentPoint;
                  }

                  // Setup the line strip
                  l_LineStrip.Start() = 0;
                  l_LineStrip.NbLines(l_Positions2D.size() - 1);

                  // Convert all 2D positions to 3D coordinates & update the shape
                  l_Shape.NbPositions(l_Positions2D.size() );
                  for(UINT32 i = 0;i < l_Positions2D.size();i ++)
                  {
                     l_Shape.Position(i) = ConvertSphericalToRectangular(l_Positions2D[i] );
                  }

                  // Release temporary objects
                  l_pUnitPathModel->Release();
                  l_pUnitPathMesh->Release();
               }
               else
               {
                  // Simply make it visible
                  l_UnitPath->second.first->Visibility() = true;
               }

               // At this point, a model should exist for the current path
               gassert(m_UnitPaths.count(l_Unit->Id() ), "Path was not created for displayed unit");
               CGFX_Model_Itf* l_pUnitPathModel = m_UnitPaths.find(l_Unit->Id() )->second.first;
               GFX::GOverridePSCascade* l_pPathOverride = (GFX::GOverridePSCascade*) l_pUnitPathModel->Override(0);
               gassert(l_pPathOverride, "Invalid path override");
               gassert(l_pPathOverride->Type() == GFX::TYPE_OVERRIDE_PS_CASCADE, "Invalid path override type");

               // Update the transform of the texture to reflect the location of the unit
               UINT32 l_iTargetCountryID = l_vControls[l_Path.Points().back().m_iRegionID].m_iMilitary;
               EUnitFriendship::Enum l_PathFriendship =
                  (l_iCountryID == 0) ? EUnitFriendship::Neutral : 
               ( (l_iCountryID == l_iTargetCountryID) ?
                  EUnitFriendship::Player : 
               (EUnitFriendship::Enum) g_ClientDAL.DiplomaticStatus(l_iTargetCountryID, l_iCountryID) );
               REAL32 l_fTotalDistanceInv = RSqrt(l_Path.Points().back().m_fDistance);
               l_fTotalDistanceInv *= l_fTotalDistanceInv;

               REAL32 l_fDistanceRatio = l_Path.Distance( (REAL32) l_fTime) * l_fTotalDistanceInv;

               l_pPathOverride->ColorFactorOverride() = m_UnitPathColors[l_PathFriendship];
               ( (CGFX_Transform_2D_Itf*) l_pPathOverride->UVTransformOverride(0) )->Position().X = -l_fDistanceRatio + 0.5f;

               const GFX::GMesh* l_pUnitPathMesh = (GFX::GMesh*) l_pUnitPathModel->Get_Primitive();
               gassert(l_pUnitPathMesh, "Invalid path mesh");
               gassert(l_pUnitPathMesh->Type() == GFX::TYPE_MESH, "Invalid path mesh type");

               ( (GFX::GFacetGroup&) (l_pUnitPathMesh->FacetGroup(0) ) ).Visibility() = !l_bSelected ? 1.f : 0.f;

               UINT32 l_iTargetType = 0;
               if(l_bSelected)
               {
                  CGFX_Model_Itf* l_pElemSubModel = m_pUnitPathElemSubModels[l_PathFriendship];
                  l_iTargetType = 1;

                  // Display dotted path when unit is selected
                  const GShape& l_Shape = l_pUnitPathMesh->Shape();
                  const GLineStrip& l_PathLines = l_pUnitPathMesh->FacetGroup(0).Facets().LineStrip();
                  REAL32 l_fCurRatio = 1.f;
                  REAL32 l_fScreenDistance = -m_fUnitPathElemOffset;
                  for(INT32 l = l_PathLines.NbLines() - 1;(l >= 0) && (l_fCurRatio > l_fDistanceRatio);l --)
                  {
                     GLineIndex l_Line = l_PathLines.Line(l);
                     GVector3D<REAL32> l_StartCoord = l_Shape.Position(l_Line.m_pVertices[0] );
                     const GVector3D<REAL32>& l_EndCoord = l_Shape.Position(l_Line.m_pVertices[1] );

                     const GVector2D<REAL32>& l_StartUV = l_Shape.UVCoord(0, l_Line.m_pVertices[0] );
                     const GVector2D<REAL32>& l_EndUV = l_Shape.UVCoord(0, l_Line.m_pVertices[1] );

                     if(ClipOnEarth(l_StartCoord) && ClipOnEarth(l_EndCoord) )
                     {
                        // Test if line must be "shortened" because unit is currently inside the segment
                        if(l_StartUV.x < l_fDistanceRatio)
                        {
                           l_StartCoord = l_EndCoord + 
                              (l_StartCoord - l_EndCoord) * 
                              ( (l_fDistanceRatio - l_EndUV.x) / (l_StartUV.x - l_EndUV.x) );
                        }

                        // Project the line in 2D
                        GVector2D<INT32> l_StartScreen, l_EndScreen;
                        Project3DPointIn2D(l_StartCoord, l_StartScreen);
                        Project3DPointIn2D(l_EndCoord, l_EndScreen);

                        // Clip line inside current window
                        UINT32 l_iClipResult = Geometry::ClipLine(l_StartScreen, l_EndScreen, l_ClipWindow);
                        if(l_iClipResult)
                        {
                           // Reset elem position if end point was clipped
                           if(l_iClipResult & 0x02)
                           {
                              l_fScreenDistance = 0;
                           }
                           GVector2D<REAL32> l_Dir(l_StartScreen - l_EndScreen);
                           REAL32 l_fLengthInv = RSqrt(l_Dir.DotProduct(l_Dir) );
                           REAL32 l_fLength = RSqrt(l_fLengthInv);
                           l_fLength *= l_fLength;
                           l_Dir *= l_fLengthInv;
                           GVector2D<REAL32> l_StartElemPos(l_EndScreen);

                           if(l_fScreenDistance < 0)
                           {
                              if(l_fLength < -l_fScreenDistance)
                              {
                                 l_fScreenDistance += l_fLength;
                              }
                              else
                              {
                                 l_StartElemPos = l_StartElemPos + (l_Dir * (-l_fScreenDistance) );
                                 l_fLength += l_fScreenDistance;
                                 l_fScreenDistance = 0;
                              }
                           }

                           if(l_fScreenDistance >= 0)
                           {
                              while(l_fScreenDistance < l_fLength)
                              {
                                 GVector2D<REAL32> l_CurElemPos = l_StartElemPos + l_Dir * l_fScreenDistance;

                                 // Verify if a child is already created for this unit target
                                 CGFX_Model_Itf* l_pModelElem = NULL;
                                 CGFX_Transform_2D_Itf* l_pTransformElem = NULL;
                                 if(l_iCurrentElemChild[l_PathFriendship] < l_pElemSubModel->Get_Nb_Children() )
                                 {
                                    // Fetch child data
                                    l_pModelElem = l_pElemSubModel->Get_Child(l_iCurrentElemChild[l_PathFriendship] );
                                    l_pModelElem->Visibility() = true;
                                    l_pTransformElem = (CGFX_Transform_2D_Itf*) l_pModelElem->Get_Transform();
                                 }
                                 else
                                 {
                                    // Create a new child & its transform
                                    l_pModelElem = g_Joshua.Renderer()->Create_Model();
                                    l_pTransformElem = g_Joshua.Renderer()->Create_Transform_2D();

                                    // Setup child
                                    l_pElemSubModel->Add_Child(l_pModelElem);
                                    l_pModelElem->Set_Transform(l_pTransformElem);
                                    l_pModelElem->Set_Primitive(m_pUnitPathElemMesh);

                                    l_pTransformElem->Size().X = (REAL32) m_UnitPathElemSize.x;
                                    l_pTransformElem->Size().Y = (REAL32) m_UnitPathElemSize.y;

                                    g_Joshua.GUIManager()->SetDirty();

                                    l_pModelElem->Release();
                                    l_pTransformElem->Release();
                                 }
                                 l_iCurrentElemChild[l_PathFriendship] ++;

                                 l_pTransformElem->Position() = ScreenAlignTransform(l_CurElemPos, m_UnitPathElemOffset);
                                 l_fScreenDistance += (REAL32) m_UnitPathElemSize.x;
                              }

                              l_fScreenDistance -= l_fLength;
                           }
                        }
                     }
                     else
                     {
                        l_fScreenDistance = 0;
                     }
                     l_fCurRatio = l_StartUV.x;
                  }
               }

               // Display destination target
               const GVector2D<REAL32>& l_fDestPosition2D = l_Path.Points().back().m_Position;
               GVector3D<REAL32> l_fDestPos = ConvertSphericalToRectangular(l_fDestPosition2D);

               // Convert from 3D coordinate to screen coordinate
               GVector2D<INT32> l_ScreenTargetPos;
               if(ProjectAndClipOnEarth3DPointIn2D(l_fDestPos, l_ScreenTargetPos) )
               {
                  // Verify if a child is already created for this unit target
                  CGFX_Model_Itf* l_pModel = NULL;
                  CGFX_Transform_2D_Itf* l_pTransform = NULL;
                  if(l_iCurrentDestChild < m_pUnitTargetDisplayModel->Get_Nb_Children() )
                  {
                     // Update child data
                     l_pModel = m_pUnitTargetDisplayModel->Get_Child(l_iCurrentDestChild);
                     l_pModel->Visibility() = true;
                     l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();

                     l_pModel->ClearOverrides();

                     if(l_pModel->Get_Primitive() != m_pUnitTargetMesh[l_iTargetType] )
                     {
                        l_pModel->Set_Primitive(m_pUnitTargetMesh[l_iTargetType] );
                        g_Joshua.GUIManager()->SetDirty();
                     }
                  }
                  else
                  {
                     // Create a new child & its transform
                     l_pModel = g_Joshua.Renderer()->Create_Model();
                     l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                     // Setup child
                     m_pUnitTargetDisplayModel->Add_Child(l_pModel);
                     l_pModel->Set_Transform(l_pTransform);
                     l_pModel->Set_Primitive(m_pUnitTargetMesh[l_iTargetType] );

                     g_Joshua.GUIManager()->SetDirty();

                     l_pModel->Release();
                     l_pTransform->Release();
                  }
                  l_iCurrentDestChild ++;

                  l_pModel->AddOverride(m_pUnitTargetTSamplers[l_iTargetType] );
                  l_pModel->AddOverride(m_pUnitPathColorOverrides[l_PathFriendship] );

                  l_pTransform->Position() = ScreenAlignTransform(l_ScreenTargetPos, m_UnitTargetOffset[l_iTargetType] );
                  l_pTransform->Size().X = (REAL32) m_UnitTargetSize[l_iTargetType].x;
                  l_pTransform->Size().Y = (REAL32) m_UnitTargetSize[l_iTargetType].y;
               }
            }
            else
            {
               // Verify if a path was previously created for that unit
               GUnitPathMapIter l_UnitPath = m_UnitPaths.find(l_Unit->Id() );
               if(l_UnitPath != m_UnitPaths.end() )
               {
                  // Hide unit path display
                  l_UnitPath->second.first->Visibility() = false;
               }
            }
            // Make sure unit is large enough to be displayed
            //         if(l_Unit->Qty() < l_Threshold)
            //            continue;

            // Convert unit earth coordinates into world coordinates
            GVector3D<REAL32> l_UnitPos = ConvertSphericalToRectangular(l_Unit->Position() );

            // Make sure this location on earth is visible
            GVector3D<REAL32> l_ViewUnitPos = m_ViewMat * l_UnitPos;

            // Verify if unit is in front of earth && 
            if((l_ViewUnitPos.z < m_OriginPos.z - 1 / m_OriginPos.z))
            {
               GVector2D<REAL32> l_ScreenUnitPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewUnitPos) ).ApplyWeigth() );

               // Verify if unit is visible in screen
               if( (l_ScreenUnitPos.x >= -1.1f) && 
                  (l_ScreenUnitPos.x <=  1.1f) && 
                  (l_ScreenUnitPos.y >= -1.1f) && 
                  (l_ScreenUnitPos.y <=  1.1f) )
               {
                  // Update current child object with arena screen position
                  l_ScreenUnitPos *= l_ScreenTransformScale;
                  l_ScreenUnitPos += l_ScreenTransformOffset;
                  GVector2D<REAL32> l_ScreenUnitPosLeftUp = l_ScreenUnitPos;
                  if(!m_bDisplayUnitsAsFlags)
                     l_ScreenUnitPosLeftUp -= m_UnitSizeHalf[l_UnitFriendship];
                  else
                     l_ScreenUnitPosLeftUp -= m_UnitFlagHalfSize;

                  l_ScreenUnitPosLeftUp.x = (REAL32) RoundToInt(l_ScreenUnitPosLeftUp.x);
                  l_ScreenUnitPosLeftUp.y = (REAL32) RoundToInt(l_ScreenUnitPosLeftUp.y);

                  SScreenOverlayInfo l_UnitInfo;
                  l_UnitInfo.m_iId = l_Unit->Id();
                  l_UnitInfo.m_eType = EOverlayType::Unit;
                  l_UnitInfo.m_Pos = l_ScreenUnitPosLeftUp;
                  if(!m_bDisplayUnitsAsFlags)
                     l_UnitInfo.m_Size = m_UnitSize[l_UnitFriendship];
                  else
                     l_UnitInfo.m_Size = m_UnitFlagSize;

                  g_ClientDAL.VisibleUnits().push_back(l_UnitInfo);

                  // Verify if a child is already created for this unit
                  CGFX_Model_Itf* l_pModel = NULL;
                  CGFX_Transform_2D_Itf* l_pTransform = NULL;

                  //If we dislpay the unit icons.
                  if(l_iCurrentChild < m_pUnitDisplayModel->Get_Nb_Children() )
                  {
                     // Update child data
                     l_pModel = m_pUnitDisplayModel->Get_Child(l_iCurrentChild);
                     l_pModel->Visibility() = true;
                     l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
                     l_pModel->ClearOverrides();

                     if(l_pModel->Get_Primitive() != l_pUnitMesh )
                     {
                        l_pModel->Set_Primitive(l_pUnitMesh );
                        g_Joshua.GUIManager()->SetDirty();
                     }
                  }
                  else
                  {
                     // Create a new child & its transform
                     l_pModel = g_Joshua.Renderer()->Create_Model();
                     l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                     // Setup child
                     m_pUnitDisplayModel->Add_Child(l_pModel);
                     l_pModel->Set_Transform(l_pTransform);
                     l_pModel->Set_Primitive(l_pUnitMesh);
                     g_Joshua.GUIManager()->SetDirty();
                     l_pModel->Release();
                     l_pTransform->Release();
                  }

                  // Go to next child
                  l_iCurrentChild ++;

                  //If we do not display unit as flags and the unit is naval and not bombarding, use the naval icon
                  if(!m_bDisplayUnitsAsFlags && l_Unit->IsNaval() && !l_Unit->RegionToBombard())
                     l_pModel->AddOverride(l_pNavalUnitTS);
                  //If unit is naval and bombarding
                  else if(!m_bDisplayUnitsAsFlags && l_Unit->IsNaval() && l_Unit->RegionToBombard())
                  {
                     //if it is moving, it is going to bombard soon, use the std naval icon
                     if(l_Unit->CombatStatus() == EMilitaryStatus::Moving || l_Unit->NextStatus() == EMilitaryStatus::Moving)
                     {
                        l_pModel->AddOverride(l_pNavalUnitTS);
                     }
                     else
                     {
                        l_pModel->AddOverride(l_pNavalBombardingUnitTS);
                     }
                  }
                  else//Use the standard icon
                  {
                     l_pModel->AddOverride(l_pUnitTS);
                  }

                  // Setup transform
                  l_pTransform->Position() = (l_ScreenUnitPosLeftUp - GVector2D<REAL32>(0.5f, 0.5f) );
                  if(!m_bDisplayUnitsAsFlags)
                  {
                     l_pTransform->Size().X = (REAL32) m_UnitSize[l_UnitFriendship].x;
                     l_pTransform->Size().Y = (REAL32) m_UnitSize[l_UnitFriendship].y;
                  }
                  else
                  {
                     l_pTransform->Size().X = (REAL32) m_UnitFlagSize.x;
                     l_pTransform->Size().Y = (REAL32) m_UnitFlagSize.y;
                  }

                  // Select unit if needed
                  if(l_bSelected)
                  {
                     l_iLastSelectionID = g_ClientDAL.VisibleUnits().size() - 1;


                     //Determine which mesh should be used (depending if we display units as flags or icons)
                     //and determine which half size should be used
                     GFX::GMesh*          l_pMeshToUse         = NULL;
                     GFX::GOverrideTSampler* l_pTSamplerToUse     = NULL;
                     GVector2D<UINT32>       l_SelectionHalfSize        ;
                     GVector2D<UINT32>       l_SelectionSize            ;
                     {
                        if(!m_bDisplayUnitsAsFlags)
                        {
                           l_pMeshToUse         = m_pUnitSelectionMeshes[l_UnitFriendship];
                           l_SelectionHalfSize  = m_UnitSelectionSizeHalf[l_UnitFriendship];
                           l_SelectionSize      = m_UnitSelectionSize[l_UnitFriendship];
                           l_pTSamplerToUse     = m_pUnitSelectionTSamplers[l_UnitFriendship];
                        }
                        else
                        {
                           l_pMeshToUse         = m_pUnitFlagSelectionMeshes[l_UnitFriendship];
                           l_SelectionHalfSize  = m_UnitFlagSelectionSizeHalf[l_UnitFriendship];
                           l_SelectionSize      = m_UnitFlagSelectionSize[l_UnitFriendship];
                           l_pTSamplerToUse     = m_pUnitFlagSelectionTSamplers[l_UnitFriendship]; 
                        }
                     }

                     // Verify if a child is already created for this selection
                     CGFX_Model_Itf* l_pSelectModel = NULL;
                     CGFX_Transform_2D_Itf* l_pSelectTransform = NULL;
                     if(l_iCurrentChild < m_pUnitDisplayModel->Get_Nb_Children() )
                     {
                        // Update child data
                        l_pSelectModel = m_pUnitDisplayModel->Get_Child(l_iCurrentChild);
                        l_pSelectModel->Visibility() = true;
                        l_pSelectTransform = (CGFX_Transform_2D_Itf*) l_pSelectModel->Get_Transform();
                        l_pSelectModel->RemoveOverrideAt(0);

                        if(l_pSelectModel->Get_Primitive() != l_pMeshToUse)
                        {
                           l_pSelectModel->Set_Primitive(l_pMeshToUse);
                           g_Joshua.GUIManager()->SetDirty();
                        }
                     }
                     else
                     {
                        // Create a new child & its transform
                        l_pSelectModel = g_Joshua.Renderer()->Create_Model();
                        l_pSelectTransform = g_Joshua.Renderer()->Create_Transform_2D();

                        // Setup child
                        m_pUnitDisplayModel->Add_Child(l_pSelectModel);
                        l_pSelectModel->Set_Transform(l_pSelectTransform);
                        l_pSelectModel->Set_Primitive(l_pMeshToUse);

                        g_Joshua.GUIManager()->SetDirty();
                        l_pSelectModel->Release();
                        l_pSelectTransform->Release();
                     }
                     // Go to next child
                     l_iCurrentChild ++;

                     l_pSelectTransform->Position() = l_ScreenUnitPos - l_SelectionHalfSize;
                     l_pSelectTransform->Size().X = (REAL32) l_SelectionSize.x;
                     l_pSelectTransform->Size().Y = (REAL32) l_SelectionSize.y;

                     l_pSelectModel->AddOverride(l_pTSamplerToUse);
                  }

                  // Display unit status change if needed and if unit belongs to player?
                  if(l_UnitFriendship == EUnitFriendship::Player)
                  {
                     REAL32 l_fProgress = l_Unit->StatusChangeProgress( (REAL32) l_fTime);
                     if(l_fProgress < 1.f)
                     {
                        // Create progress content or reuse previous progress.
                        CGFX_Model_Itf* l_pProgressBGModel = NULL;
                        CGFX_Transform_2D_Itf* l_pProgressBGTfx = NULL;
                        CGFX_Transform_Itf* l_pProgressPercentTxf = NULL;
                        if(l_iCurrentProgressChild < m_pUnitProgressMasterModel->Get_Nb_Children() )
                        {
                           // Update child data
                           l_pProgressBGModel = m_pUnitProgressMasterModel->Get_Child(l_iCurrentProgressChild);
                           l_pProgressBGModel->Visibility() = true;
                           l_pProgressBGTfx = (CGFX_Transform_2D_Itf*) l_pProgressBGModel->Get_Transform();

                           CGFX_Model_Itf* l_pProgressChild = l_pProgressBGModel->Get_Child(0);
                           l_pProgressPercentTxf = l_pProgressChild->Get_Transform();
                        }
                        else
                        {
                           // Create a new child & its content
                           l_pProgressBGModel = g_Joshua.Renderer()->Create_Model();
                           l_pProgressBGTfx = g_Joshua.Renderer()->Create_Transform_2D();

                           CGFX_Model_Itf* l_pProgressChild = g_Joshua.Renderer()->Create_Model();
                           l_pProgressPercentTxf = g_Joshua.Renderer()->Create_Transform();

                           // Setup child
                           m_pUnitProgressMasterModel->Add_Child(l_pProgressBGModel);
                           l_pProgressBGModel->Set_Transform(l_pProgressBGTfx);
                           l_pProgressBGModel->Set_Primitive(m_pUnitProgressBGMesh);

                           l_pProgressBGModel->Add_Child(l_pProgressChild);
                           l_pProgressChild->Set_Transform(l_pProgressPercentTxf);
                           l_pProgressChild->Set_Primitive(m_pUnitProgressPercentMesh);

                           l_pProgressPercentTxf->Get_Matrix().LoadIdentity();

                           l_pProgressBGModel->Release();
                           l_pProgressBGTfx->Release();
                           l_pProgressChild->Release();
                           l_pProgressPercentTxf->Release();
                        }
                        l_iCurrentProgressChild ++;

                        l_pProgressBGTfx->Size().X = l_UnitInfo.m_Size.x;
                        l_pProgressBGTfx->Size().Y = (REAL32) m_iUnitProgressHeight;
                        l_pProgressBGTfx->Position().X = l_UnitInfo.m_Pos.x;
                        l_pProgressBGTfx->Position().Y = 
                           l_UnitInfo.m_Pos.y + (REAL32) m_iUnitProgressDistance + l_UnitInfo.m_Size.y;
                        l_pProgressPercentTxf->Get_Matrix()(0,0) = l_fProgress;
                     }
                  }
               }
            }
         }
         ++ l_GroupIt;
      }

   }

   // Hide remaining path elementsdestinations
   for(UINT32 i = 0;i < EUnitFriendship::Count;i ++)
   {
      HideOverlayComponents(m_pUnitPathElemSubModels[i], l_iCurrentElemChild[i] );
   }

   // Hide remaining path destinations
   HideOverlayComponents(m_pUnitTargetDisplayModel, l_iCurrentDestChild);

   // Hide remaining children
   HideOverlayComponents(m_pUnitDisplayModel, l_iCurrentChild);

   // Hide remaining progress
   HideOverlayComponents(m_pUnitProgressMasterModel, l_iCurrentProgressChild);

   if( (l_iSelectionCount == 1) && (l_iLastSelectionID >= 0) )
   {
      SP2::GUnitGroup * l_Unit = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(l_iLastSelection);

      const SScreenOverlayInfo& l_Info = g_ClientDAL.VisibleUnits()[l_iLastSelectionID];
      g_ClientDDL.UnitInfoSmallWindow()->DescribeGroup(l_Unit);

      // Display current unit status
      m_pUnitStatusDisplayModel->Visibility() = true;
      m_pUnitStatusDisplayModel->RemoveOverrideAt(0);
      m_pUnitStatusDisplayModel->AddOverride(m_pUnitStatusTSamplers[l_Unit->Status( (REAL32) l_fTime) ] );

      const GVector2D<INT32> l_Offset( (l_Info.m_Size.x / 2) - m_UnitStatusSize.x, -m_UnitStatusSize.y);

      CGFX_Transform_2D_Itf* l_pTfx =  (CGFX_Transform_2D_Itf*) m_pUnitStatusDisplayModel->Get_Transform();
      l_pTfx->Size() = m_UnitStatusSize;
      l_pTfx->Position() = ScreenAlignTransform(l_Info.m_Pos, l_Offset);

      //If we are not dragging, and the window is hidden and the cursor is the default mouse cursor
      //Show the unit info small window
      if( !m_bUnitDragging && 
          !g_ClientDDL.UnitInfoSmallWindow()->Visible() && 
          (g_Joshua.GUIManager()->MousePointer() == m_DefaultMouseCursor) 
        )
      {
         g_ClientDDL.UnitInfoSmallWindow()->Show();
      }
      g_ClientDDL.UnitInfoSmallWindow()->Position(
         (INT32) (l_Info.m_Pos.x + l_Info.m_Size.x + 8.f),
         (INT32) l_Info.m_Pos.y);
   }
   else
   {
      if(g_ClientDDL.UnitInfoSmallWindow()->Visible() )
      {
         g_ClientDDL.UnitInfoSmallWindow()->Hide();
      }
   }

   // Hide non visible paths
   GUnitPathMapIter l_PathIter = m_UnitPaths.begin();
   vector<UINT32> l_vPathToRemove;

   while(l_PathIter != m_UnitPaths.end() )
   {
      if(l_DisplayedPathsIDs.find(l_PathIter->first) == l_DisplayedPathsIDs.end() )
      {
         l_vPathToRemove.push_back(l_PathIter->first);
      }
      ++ l_PathIter;
   }

   for(UINT32 p = 0;p < l_vPathToRemove.size();p ++)
   {
      GUnitPathMapIter l_It = m_UnitPaths.find(l_vPathToRemove[p] );
      l_It->second.first->Visibility() = false;
      m_UnitPaths.erase(l_It);
   }
}

void GBackgroundEarth::UpdateCombats()
{
   g_ClientDAL.VisibleCombats().clear();
   m_pCombatDisplayModel->Visibility() = false;
   if(m_bShowCombat)
   {
      // Fetch the list of combat arenas
      const stdext::hash_map<UINT32, Combat::GArenaInfo*>& l_ArenaInfoList = g_Joshua.UnitManager().ArenaInfos();

      // Simply stop here if there is no combat
      if(l_ArenaInfoList.size() > 0)
      {
         // Show combats
         m_pCombatDisplayModel->Visibility() = false;

         // For each child we currently have for this Model, change its information (location) to reflect a visible combat
         UINT32 l_iCurrentChild = 0;
         stdext::hash_map<UINT32, Combat::GArenaInfo*>::const_iterator l_CombatIt = l_ArenaInfoList.begin();
         while(l_CombatIt != l_ArenaInfoList.end() )
         {
            const Combat::GArenaInfo& l_Arena = *(l_CombatIt->second);

            // Convert arena earth coordinates into world coordinates
            GVector3D<REAL32> l_ArenaPos = ConvertSphericalToRectangular(l_Arena.m_Coordinate);

            // Convert to 2D screen coordinates & make sure it is in front of earth
            GVector2D<INT32> l_ScreenPos;
            if(ProjectAndClipOnEarth3DPointIn2D(l_ArenaPos, l_ScreenPos) )
            {
               // Verify if a child is already created for this combat
               CGFX_Transform_2D_Itf* l_pTransform = NULL;
               if(l_iCurrentChild < m_pCombatDisplayModel->Get_Nb_Children() )
               {
                  // Update child data
                  m_pCombatDisplayModel->Get_Child(l_iCurrentChild)->Visibility() = true;
                  l_pTransform = (CGFX_Transform_2D_Itf*) m_pCombatDisplayModel->Get_Child(l_iCurrentChild)->Get_Transform();
               }
               else
               {
                  // Create a new child & its transform
                  CGFX_Model_Itf* l_pModel = g_Joshua.Renderer()->Create_Model();
                  l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                  // Setup child
                  m_pCombatDisplayModel->Add_Child(l_pModel);
                  l_pModel->Set_Transform(l_pTransform);
                  l_pModel->Set_Primitive(m_pCombatMesh);

                  // Setup transform
                  l_pTransform->Size().X = (REAL32) m_CombatSize.x;
                  l_pTransform->Size().Y = (REAL32) m_CombatSize.y;

                  l_pTransform->Release();
                  l_pModel->Release();

                  // Create an override for the combat sequence
                  if(m_pCombatSequence)
                  {
                     GFX::GOverrideTxtrSeqControl* l_pOverride = g_Joshua.Renderer()->CreateOverrideTxtrSeqControl();
                     l_pOverride->TextureSequence(m_pCombatSequence);

                     CGFX_Animation_Linear_Itf* l_pCombatAnim =
                        g_Joshua.Renderer()->Helper()->AnimateTextureSequence( m_pCombatSequence,
                        l_pOverride->OverrideController(), 
                        m_fCombatFPS);

                     CGFX_Action_Itf* l_pCombatAction = g_Joshua.Renderer()->Create_Action();
                     l_pCombatAction->Add_Animation(l_pCombatAnim);
                     l_pCombatAction->Length(l_pCombatAnim->Key(l_pCombatAnim->Get_Nb_Keys() - 1).m_Time);

                     CGFX_Track_Itf* l_pCombatTrack = g_Joshua.Renderer()->Create_Track();
                     l_pCombatTrack->Set_Action(l_pCombatAction);
                     l_pCombatTrack->Playback_Count(0);
                     l_pCombatTrack->Set_Start_Time(m_pScene->Get_Animation_System()->Get_Animation_Time() );

                     m_vCombatAnims.push_back(l_pCombatTrack);
                     m_pScene->Get_Animation_System()->Add_Track(l_pCombatTrack);
                     l_pModel->AddOverride(l_pOverride);

                     SAFE_RELEASE(l_pOverride);
                  }

                  g_Joshua.GUIManager()->SetDirty();
               }

               l_pTransform->Position() = ScreenAlignTransform(l_ScreenPos, m_CombatOffset);

               SScreenOverlayInfo l_CombatInfo;
               l_CombatInfo.m_iId = l_Arena.m_iId;
               l_CombatInfo.m_eType = EOverlayType::Combat;
               l_CombatInfo.m_Pos = GVector2D<INT16>( (INT16)l_pTransform->Position().X,
                  (INT16)l_pTransform->Position().Y);
               l_CombatInfo.m_Size = m_CombatSize;
               g_ClientDAL.VisibleCombats().push_back(l_CombatInfo);

               // Go to next child
               l_iCurrentChild ++;
            }

            // Go to next combat arena
            ++ l_CombatIt;
         }

         // Hide remaining children
         HideOverlayComponents(m_pCombatDisplayModel, l_iCurrentChild);
      }
   }
}




const REAL32 c_fPercentChancesAnimStart = 0.10f;
const REAL32 c_fPercentChancesAnimStop  = 0.95f;
void GBackgroundEarth::UpdateBombardments()
{
   m_pBombardmentDisplayModel->Visibility() = false;
   REAL32 l_fRandom = 0.f;

   //If the bombardments must be displayed
   if(m_bShowBombardment)
   {
      //If there are bombardments to display
      hash_map<UINT32,GDisplayedBombardedRegion>& l_BombardedRegions = g_ClientDAL.BombardedRegions();

      UINT32 l_iCurrentChild = 0;

      // Simply stop here if there is no Bombardment
      if(l_BombardedRegions.size() > 0)
      {
         // Show Bombardments
         m_pBombardmentDisplayModel->Visibility() = false;


         //For each bombarded region
         for(hash_map<UINT32,GDisplayedBombardedRegion>::iterator l_It = l_BombardedRegions.begin();
                                                                  l_It != l_BombardedRegions.end();
                                                                  l_It++)
         {
            //For every point that must be displayed in that region
            for(UINT32 i = 0 ; i < l_It->second.m_vBombardmentPoints.size() ; i++)
            {
               REAL32& l_fDisplayedClock = l_It->second.m_vDisplayedClock[i];

               //If the clock is above 0, mean the animation has been playing
               if(l_fDisplayedClock >  0)
               {
                  //If the point is displayed and has been playing since long enough (c_fBombardmentMinimalExplosionTime), 
                  //X% chances it can stop
                  if((g_Joshua.Clock() - l_fDisplayedClock) > c_fBombardmentMinimalExplosionTime )
                  {
                     l_fRandom = g_CombatManager.RandomReal(); 
                     //X percent of chances anim will stop
                     if(l_fRandom < c_fPercentChancesAnimStop)
                     {
                        //set a negative clock so it will not play until enough time has passed
                        l_fDisplayedClock = (REAL32)-g_Joshua.Clock();;
                        continue;
                     }
                     else
                     {
                        //Wait again c_fBombardmentMinimalExplosionTime before checking to stop it
                        l_fDisplayedClock = (REAL32)g_Joshua.Clock();
                     }
                     
                  }
                  else
                  {//not yet the time for the check, play the anim
                     //Play the anim... dont do anything
                  }
               }
               //A clock < 0 mean the animation is stopped
               else if(l_fDisplayedClock < 0)
               {
                  //If the point has been stopped since long enough, 
                  if((g_Joshua.Clock() - abs(l_fDisplayedClock)) > c_fBombardmentMinimalExplosionTime)                        
                  {
                     //X% chances point will start playing again, if miss, has to wait another 0.5 secs
                     l_fRandom = g_CombatManager.RandomReal();
                     //X percent of chances anim start again
                     if(l_fRandom < c_fPercentChancesAnimStart)
                     {
                        //Anim will start again
                        l_fDisplayedClock = 0;
                     }
                     else
                     {
                        //Wait for c_fBombardmentMinimalExplosionTime before trying again to start it
                        //set a negative clock so it will not play until enough time has passed
                        l_fDisplayedClock = (REAL32)-g_Joshua.Clock();
                        continue;
                     }                  
                  }
                  else
                  {//not yet the time for the check, dont play the anim
                     //Dont play the anim
                     continue;
                  }
               }
               //Start the anim
               else if(!l_It->second.m_vDisplayedClock[i])
               {
                  l_fDisplayedClock = (REAL32)g_Joshua.Clock();
               }

 

               // Convert arena earth coordinates into world coordinates
               GVector3D<REAL32> l_ExplosionPos = ConvertSphericalToRectangular(l_It->second.m_vBombardmentPoints[i]);

               // Convert to 2D screen coordinates & make sure it is in front of earth
               GVector2D<INT32> l_ScreenPos;
               if(ProjectAndClipOnEarth3DPointIn2D(l_ExplosionPos, l_ScreenPos) )
               {
                  // Verify if a child is already created for this explosion
                  CGFX_Transform_2D_Itf* l_pTransform = NULL;
                  if(l_iCurrentChild < m_pBombardmentDisplayModel->Get_Nb_Children() )
                  {
                     // Update child data
                     m_pBombardmentDisplayModel->Get_Child(l_iCurrentChild)->Visibility() = true;
                     l_pTransform = (CGFX_Transform_2D_Itf*) m_pBombardmentDisplayModel->Get_Child(l_iCurrentChild)->Get_Transform();
                  }
                  else
                  {
                     // Create a new child & its transform
                     CGFX_Model_Itf* l_pModel = g_Joshua.Renderer()->Create_Model();
                     l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                     // Setup child
                     m_pBombardmentDisplayModel->Add_Child(l_pModel);
                     l_pModel->Set_Transform(l_pTransform);
                     l_pModel->Set_Primitive(m_pBombardmentMesh);

                     // Setup transform
                     l_pTransform->Size().X = (REAL32) m_BombardmentSize.x;
                     l_pTransform->Size().Y = (REAL32) m_BombardmentSize.y;

                     l_pTransform->Release();
                     l_pModel->Release();

                     // Create an override for the Bombardment sequence
                     if(m_pBombardmentSequence)
                     {
                        GFX::GOverrideTxtrSeqControl* l_pOverride = g_Joshua.Renderer()->CreateOverrideTxtrSeqControl();
                        l_pOverride->TextureSequence(m_pBombardmentSequence);

                        CGFX_Animation_Linear_Itf* l_pBombardmentAnim =
                           g_Joshua.Renderer()->Helper()->AnimateTextureSequence( m_pBombardmentSequence,
                           l_pOverride->OverrideController(), 
                           m_fBombardmentFPS);

                        CGFX_Action_Itf* l_pBombardmentAction = g_Joshua.Renderer()->Create_Action();
                        l_pBombardmentAction->Add_Animation(l_pBombardmentAnim);
                        l_pBombardmentAction->Length(l_pBombardmentAnim->Key(l_pBombardmentAnim->Get_Nb_Keys() - 1).m_Time);

                        CGFX_Track_Itf* l_pBombardmentTrack = g_Joshua.Renderer()->Create_Track();
                        l_pBombardmentTrack->Set_Action(l_pBombardmentAction);
                        l_pBombardmentTrack->Playback_Count(0);
                        l_pBombardmentTrack->Set_Start_Time(m_pScene->Get_Animation_System()->Get_Animation_Time() );

                        m_vBombardmentAnims.push_back(l_pBombardmentTrack);
                        m_pScene->Get_Animation_System()->Add_Track(l_pBombardmentTrack);
                        l_pModel->AddOverride(l_pOverride);

                        SAFE_RELEASE(l_pOverride);
                     }

                     g_Joshua.GUIManager()->SetDirty();
                  }

                  l_pTransform->Position() = ScreenAlignTransform(l_ScreenPos, m_BombardmentOffset);

                  // Go to next child
                  l_iCurrentChild ++;
               }
            }
         }
      }
         // Hide remaining children
         HideOverlayComponents(m_pBombardmentDisplayModel, l_iCurrentChild);
   }

}





















bool GBackgroundEarth::DisplayNuke(const GLaunchedNuke& in_NukeInfo)
{
   // Make sure a nuke with same Id is destroyed before moving on
   DestroyNuke(in_NukeInfo.m_Missile.m_iID);

   // Add the nuke to the DAL
   g_ClientDAL.AddLaunchedNuke(in_NukeInfo);

   // Create an action that will automatically update nuke position, trail fade out, explosion, blink & nuke display
   SNukeInfo l_Info;
   Memory::Clear(&l_Info);
   Memory::Fill(l_Info.m_iExplodeModelIDs, 0xFFFFFFFF, 4);
   Memory::Fill(l_Info.m_iModelIDs, 0xFFFFFFFF, 3);
   l_Info.m_pNukeAction = g_Joshua.Renderer()->Create_Action();
   l_Info.m_fNukeStartTime = in_NukeInfo.m_fLaunchClock;
   REAL32 l_fNukeTime = (REAL32) (in_NukeInfo.m_fHitClock - in_NukeInfo.m_fLaunchClock);
   REAL32 l_fInterceptionTime = (REAL32) (in_NukeInfo.m_fInterceptionClock - in_NukeInfo.m_fLaunchClock);

   const GVector2D<REAL32>& l_Source = in_NukeInfo.m_Missile.Position();

   // Create nuke trail display
   REAL32 l_fInterceptionRatio = 1.f;
   if(in_NukeInfo.m_iAMDSIntercepted)
   {
      l_fInterceptionRatio = l_fInterceptionTime / l_fNukeTime;
   }

   l_Info.m_iModelIDs[0] = CreateMissileTrail(l_Info.m_pNukeAction,
      l_Source,
      in_NukeInfo.m_TargetPosition,
      0,
      l_fNukeTime,
      l_fInterceptionRatio);

   CreateAMDS(0, in_NukeInfo, l_Info);
   CreateAMDS(1, in_NukeInfo, l_Info);

   if(!in_NukeInfo.m_iAMDSIntercepted)
   {
      // Compute the nuke explosion position
      GVector3D<REAL32> l_ExplodeCenter = ConvertSphericalToRectangular(in_NukeInfo.m_TargetPosition);

      l_Info.m_iExplodeModelIDs[3] = CreateExplosionAnimation(m_pNukeFlashMasterModel,
         l_Info.m_pNukeAction,
         m_pNukeFlashSequence,
         m_fNukeFlashFPS,
         l_fNukeTime);

      CGFX_Model_Itf* l_pFlashModel = m_pNukeFlashMasterModel->Get_Child(l_Info.m_iExplodeModelIDs[3] );
      l_pFlashModel->Set_Primitive(m_pNukeFlashMesh);

      CGFX_Transform_Itf* l_pFlashTfx = l_pFlashModel->Get_Transform();
      if(!l_pFlashTfx)
      {
         l_pFlashTfx = g_Joshua.Renderer()->Create_Transform();
         l_pFlashModel->Set_Transform(l_pFlashTfx);
      }

      GMatrix4x4<REAL32>& l_Mat = l_pFlashTfx->Get_Matrix();
      l_Mat(3, 0) = l_ExplodeCenter.x;
      l_Mat(3, 1) = l_ExplodeCenter.y;
      l_Mat(3, 2) = l_ExplodeCenter.z;

      l_Info.m_iExplodeModelIDs[0] = CreateExplosionAnimation(m_pNukeExplodeMasterModel,
         l_Info.m_pNukeAction,
         m_pNukeExplodeSequence,
         m_fNukeExplodeFPS,
         l_fNukeTime + m_fNukeExplodeDelay);

      CGFX_Model_Itf* l_pExplodeModel = m_pNukeExplodeMasterModel->Get_Child(l_Info.m_iExplodeModelIDs[0] );
      GFX::GMesh* l_pExplodeMesh = (GFX::GMesh*) l_pExplodeModel->Get_Primitive();
      if(!l_pExplodeMesh)
      {
         l_pExplodeMesh = g_Joshua.Renderer()->CreateMesh();
         l_pExplodeModel->Set_Primitive(l_pExplodeMesh);
         l_pExplodeMesh->Release();
      }

      // Reset the nuke explosion mesh
      l_pExplodeMesh->NbFacetGroups(0);
      GShape& l_ExplodeShape = l_pExplodeMesh->Shape();
      l_ExplodeShape.ClearAll();
      l_ExplodeShape.NbTxtrs(1);

      l_pExplodeMesh->NbFacetGroups(1);
      l_pExplodeMesh->FacetGroup(0).Facets().Type(FACETS_TRIANGLE_STRIP, false);
      GTriangleStrip& l_ExplodeTriStrip = l_pExplodeMesh->FacetGroup(0).Facets().TriangleStrip();

      // Get a random nuke explosion orientation
      Random::GQuick l_Rand;
      l_Rand.Seed( ( (UINT32) (in_NukeInfo.m_fHitClock * 100) * (UINT32) in_NukeInfo.m_Missile.m_iID) & 0x7FFFFFFF);
      l_Rand.RandomReal();
      REAL32 l_fExplodeAngle = l_Rand.RandomReal() * (REAL32) Math::PI * 2.f;

      REAL32 c = cosf(l_fExplodeAngle);
      REAL32 t = 1.f - c;
      REAL32 s = sinf(l_fExplodeAngle);
      GVector3D<REAL32> l_UpExplode;
      l_UpExplode.x = t * l_ExplodeCenter.x * l_ExplodeCenter.y - s * l_ExplodeCenter.z;
      l_UpExplode.y = t * l_ExplodeCenter.y * l_ExplodeCenter.y + c;
      l_UpExplode.z = t * l_ExplodeCenter.y * l_ExplodeCenter.z + s * l_ExplodeCenter.x;

      // Compute the nuke explosion corners
      GVector3D<REAL32> l_RightVector = l_ExplodeCenter.CrossProduct(l_UpExplode);
      l_RightVector.Normalize();

      GVector3D<REAL32> l_UpVector = l_RightVector.CrossProduct(l_ExplodeCenter);

      l_RightVector *= m_fNukeExplodeSize;
      l_UpVector *= m_fNukeExplodeSize;

      // Update nuke explosion shape
      l_ExplodeShape.NbPositions(4);
      l_ExplodeShape.Position(0) = l_ExplodeCenter + l_RightVector + l_UpVector;
      l_ExplodeShape.Position(1) = l_ExplodeCenter + l_RightVector - l_UpVector;
      l_ExplodeShape.Position(2) = l_ExplodeCenter - l_RightVector + l_UpVector;
      l_ExplodeShape.Position(3) = l_ExplodeCenter - l_RightVector - l_UpVector;

      l_ExplodeShape.NbUVCoords(0, 4);
      l_ExplodeShape.UVCoord(0, 0) = GVector2D<REAL32>(1, 0);
      l_ExplodeShape.UVCoord(0, 1) = GVector2D<REAL32>(1, 1);
      l_ExplodeShape.UVCoord(0, 2) = GVector2D<REAL32>(0, 0);
      l_ExplodeShape.UVCoord(0, 3) = GVector2D<REAL32>(0, 1);

      // Update nuke explosion triangles
      l_ExplodeTriStrip.Start() = 0;
      l_ExplodeTriStrip.NbTriangles(2);
   }

   // Play the animation at the specified moment
   m_NukeMap.insert(pair<UINT32, SNukeInfo>(in_NukeInfo.m_Missile.m_iID, l_Info) );

   UpdateNukes();

   return true;
}

void GBackgroundEarth::CreateAMDS(UINT32 in_iAmdsID, 
                                  const GLaunchedNuke& in_NukeInfo, 
                                  SNukeInfo& io_Info)
{
   if(in_NukeInfo.m_iAMDSLaunched & (1 << in_iAmdsID) )
   {
      const REAL32 l_fNukeTime = (REAL32) (in_NukeInfo.m_fHitClock - in_NukeInfo.m_fLaunchClock);
      const REAL32 l_fInterceptionTime = (REAL32) (in_NukeInfo.m_fInterceptionClock - in_NukeInfo.m_fLaunchClock);
      const REAL32 l_fAMDSStart = m_fAMDSStart[in_iAmdsID];

      REAL32 l_fInterceptionRatio = (1 - l_fAMDSStart) * 0.5f;
      UINT32 l_iSequenceID = 0;
      if(in_NukeInfo.m_iAMDSIntercepted == (1 << in_iAmdsID) )
      {
         l_iSequenceID = 1;
         l_fInterceptionRatio = min( (l_fNukeTime - l_fInterceptionTime) / l_fNukeTime,
            1.f - l_fAMDSStart * 0.9f);
      }

      // Play AMDS launch
      g_SP2Client->PlaySoundWithGametimeDelay(ESoundEventID::AMDSInterecptionLaunch,l_fAMDSStart + g_Joshua.GameTime());

      // Play AMDS Interception successful
      if(in_NukeInfo.m_iAMDSIntercepted == (1 << in_iAmdsID))
      {
         g_SP2Client->PlaySoundWithGametimeDelay(ESoundEventID::AMDSInterecptionSucess,(1 - l_fInterceptionRatio) * l_fNukeTime + g_Joshua.GameTime());

      }
      // Play AMDS interception fail
      else
      {
         g_SP2Client->PlaySoundWithGametimeDelay(ESoundEventID::AMDSInterecptionFail,(1 - l_fInterceptionRatio) * l_fNukeTime + g_Joshua.GameTime());
      }



      io_Info.m_iExplodeModelIDs[in_iAmdsID + 1] = 
         CreateExplosionAnimation(m_pAMDSExplodeMasterModel[l_iSequenceID],
         io_Info.m_pNukeAction, 
         m_pAMDSExplodeSequence[l_iSequenceID], 
         m_fAMDSExplodeFPS[l_iSequenceID],
         (1 - l_fInterceptionRatio) * l_fNukeTime);

      CGFX_Model_Itf* l_pModel = m_pAMDSExplodeMasterModel[l_iSequenceID]->
         Get_Child(io_Info.m_iExplodeModelIDs[in_iAmdsID + 1] );

      l_pModel->Set_Primitive(m_pAMDSExplodeMesh[l_iSequenceID] );
      CGFX_Transform_Itf* l_pTransform = l_pModel->Get_Transform();
      if(!l_pTransform)
      {
         l_pTransform = g_Joshua.Renderer()->Create_Transform();
         l_pModel->Set_Transform(l_pTransform);
      }

      GVector3D<REAL32> l_LastPos;
      io_Info.m_iModelIDs[in_iAmdsID + 1] = 
         CreateMissileTrail(io_Info.m_pNukeAction,
         in_NukeInfo.m_TargetPosition,
         in_NukeInfo.m_Missile.Position(),
         l_fNukeTime * l_fAMDSStart,
         l_fNukeTime,
         l_fInterceptionRatio,
         &l_LastPos);

      GMatrix4x4<REAL32>& l_Mat = l_pTransform->Get_Matrix();
      l_Mat(3, 0) = l_LastPos.x;
      l_Mat(3, 1) = l_LastPos.y;
      l_Mat(3, 2) = l_LastPos.z;

      io_Info.m_iAMDSExplodeType[in_iAmdsID] = l_iSequenceID;
   }
}

UINT32 GBackgroundEarth::CreateMissileTrail(CGFX_Action_Itf* in_pAction,
                                            const GVector2D<REAL32>& in_SourceCoord,
                                            const GVector2D<REAL32>& in_TargetCoord,
                                            REAL32 in_fStartTime,
                                            REAL32 in_fTotalTime,
                                            REAL32 in_fInterceptionRatio,
                                            GVector3D<REAL32>* out_pLastPosition)
{
   // Find a Model that will display this nuke
   UINT32 l_iModelID = 0;
   while(l_iModelID < m_pNukeTrailMasterModel->Get_Nb_Children() )
   {
      if(m_pNukeTrailMasterModel->Get_Child(l_iModelID)->Visibility() == 0)
      {
         break;
      }

      l_iModelID ++;
   }

   CGFX_Model_Itf* l_pTrailModel = NULL;
   GFX::GMesh* l_pTrailMesh = NULL;
   GFX::GOverridePSCascade* l_pTrailOverride = NULL;
   CGFX_Transform_2D_Itf* l_pTrailTxtrTx = NULL;

   // Create a new Model if no one is available
   if(l_iModelID == m_pNukeTrailMasterModel->Get_Nb_Children() )
   {
      // Create tail
      l_pTrailModel = g_Joshua.Renderer()->Create_Model();
      l_pTrailMesh = g_Joshua.Renderer()->CreateMesh();
      m_pNukeTrailMasterModel->Add_Child(l_pTrailModel);
      l_pTrailModel->Set_Primitive(l_pTrailMesh);

      l_pTrailOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
      l_pTrailModel->AddOverride(l_pTrailOverride);
      l_pTrailTxtrTx = g_Joshua.Renderer()->Create_Transform_2D();
      l_pTrailOverride->AddUVTransformOverride(0, l_pTrailTxtrTx);

      UINT32 l_iCustomID = l_pTrailModel->Add_Custom_Parameter(c_sNukeCustomParamX);
      assert(l_iCustomID == 0);
      l_iCustomID = l_pTrailModel->Add_Custom_Parameter(c_sNukeCustomParamY);
      assert(l_iCustomID == 1);
      l_iCustomID = l_pTrailModel->Add_Custom_Parameter(c_sNukeCustomParamZ);
      assert(l_iCustomID == 2);
      l_iCustomID = l_pTrailModel->Add_Custom_Parameter(c_sNukeCustomParamVis);
      assert(l_iCustomID == 3);
      l_iCustomID = l_pTrailModel->Add_Custom_Parameter(c_sNukeCustomParamBlnk);
      assert(l_iCustomID == 4);

      l_pTrailModel->Release();
      l_pTrailMesh->Release();
      l_pTrailOverride->Release();
      l_pTrailTxtrTx->Release();
   }
   else
   {
      // Fetch trail objects
      l_pTrailModel = m_pNukeTrailMasterModel->Get_Child(l_iModelID);
      l_pTrailMesh = (GFX::GMesh*) l_pTrailModel->Get_Primitive();
      l_pTrailOverride = (GFX::GOverridePSCascade*) l_pTrailModel->Override(0);
      l_pTrailTxtrTx = (CGFX_Transform_2D_Itf*) l_pTrailOverride->UVTransformOverride(0);
   }

   l_pTrailModel->Visibility() = true;

   CGFX_Animation_Linear_Itf* l_pAnimX = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Animation_Linear_Itf* l_pAnimY = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Animation_Linear_Itf* l_pAnimZ = g_Joshua.Renderer()->Create_Animation_Linear();

   l_pAnimX->Connect_Custom(l_pTrailModel, 0);
   l_pAnimY->Connect_Custom(l_pTrailModel, 1);
   l_pAnimZ->Connect_Custom(l_pTrailModel, 2);

   in_pAction->Add_Animation(l_pAnimX);
   in_pAction->Add_Animation(l_pAnimY);
   in_pAction->Add_Animation(l_pAnimZ);

   REAL32 l_fTrailTime = in_fTotalTime * in_fInterceptionRatio;

   // Create trail (in)visibility animation
   CGFX_Animation_Constant_Itf* l_pAnimVis = g_Joshua.Renderer()->Create_Animation_Constant();
   l_pAnimVis->Connect_Custom(l_pTrailModel, 3);
   in_pAction->Add_Animation(l_pAnimVis);

   CGFX_Key_Constant l_VisKey;
   if(in_fStartTime > 0)
   {
      l_VisKey.m_Time = 0;
      l_VisKey.m_Value = 1;
      l_pAnimVis->Add_Key(l_VisKey);
   }

   l_VisKey.m_Time = in_fStartTime;
   l_VisKey.m_Value = 0;
   l_pAnimVis->Add_Key(l_VisKey);

   l_VisKey.m_Time = in_fStartTime + l_fTrailTime;
   l_VisKey.m_Value = 1;
   l_pAnimVis->Add_Key(l_VisKey);

   l_pAnimVis->Release();

   // Create animation that will automatically fade trail
   CGFX_Animation_Linear_Itf* l_pAnimTrail = g_Joshua.Renderer()->Create_Animation_Linear();
   in_pAction->Add_Animation(l_pAnimTrail);
   l_pAnimTrail->Connect(l_pTrailTxtrTx, GFX::PARAM_POSITION_X);

   l_pTrailTxtrTx->Size().Y = 1;
   l_pTrailTxtrTx->Position().Y = 0;

   // Update the nuke trail mesh
   l_pTrailMesh->NbFacetGroups(0);
   GShape& l_TrailShape = l_pTrailMesh->Shape();
   l_TrailShape.ClearAll();
   l_TrailShape.NbTxtrs(1);

   l_pTrailMesh->NbFacetGroups(1);
   l_pTrailMesh->FacetGroup(0).Facets().Type(FACETS_LINE_STRIP, false);
   GLineStrip& l_TrailLineStrip = l_pTrailMesh->FacetGroup(0).Facets().LineStrip();

   vector<GVector3D<REAL32> > l_PositionsPlanar;

   // Compute fading animation
   REAL32 l_fFadeLength = m_fNukeTrailFadeTime / l_fTrailTime;
   l_pTrailTxtrTx->Size().X = 1.f / l_fFadeLength;
   l_pTrailTxtrTx->Position().X = 1;

   CGFX_Key_Linear l_TrailKey;
   l_TrailKey.m_Time = in_fStartTime;
   l_TrailKey.m_Value = 1;
   l_pAnimTrail->Add_Key(l_TrailKey);

   l_TrailKey.m_Time = in_fStartTime + l_fTrailTime + m_fNukeTrailFadeTime;
   l_TrailKey.m_Value = -1 * l_pTrailTxtrTx->Size().X;
   l_pAnimTrail->Add_Key(l_TrailKey);

   in_pAction->Length(max(in_pAction->Length(), in_fStartTime + l_fTrailTime + m_fNukeTrailFadeTime) );

   GVector3D<REAL32> l_Right, l_UpLocal, l_Up, l_Out;

   // Convert source coordinate in cartesian space
   l_Right = ConvertSphericalToRectangular(in_SourceCoord);

   // Convert destination coordinate in cartesian space
   l_UpLocal = ConvertSphericalToRectangular(in_TargetCoord);

   // Compute the coordinate system for the plane of the trajectory
   l_Out = l_Right.CrossProduct(l_UpLocal);

   // Calculate angle (& distance) between source & destination
   REAL32 l_AngleMax = acosf(l_Right.DotProduct(l_UpLocal) );
   REAL32 l_Angle = l_AngleMax * in_fInterceptionRatio;

   l_Out.Normalize();
   l_Up = l_Out.CrossProduct(l_Right);

   // Compute increments between endpoints
   UINT32 l_iNbDivs = (UINT32) (1 + (l_Angle / m_fNukeMinPathDiv) );
   REAL32 l_fDivInc = 1.f / (REAL32) l_iNbDivs;
   REAL32 l_fTimeInc = l_fTrailTime * l_fDivInc;

   // Trace line on the trajectory plane
   for(UINT32 i = 0;i <= l_iNbDivs;i ++)
   {
      REAL32 l_fDelta = (REAL32) i * l_fDivInc;
      REAL32 l_fRadius = 4 * (1 - l_fDelta * in_fInterceptionRatio) * l_fDelta * in_fInterceptionRatio;
      l_fRadius *= l_AngleMax * m_fNukeMaxPathHeightFactor;
      l_fRadius += 1;

      GVector3D<REAL32> l_CurPos(cosf(l_fDelta * l_Angle), sinf(l_fDelta * l_Angle), 0);
      l_CurPos *= l_fRadius;
      l_PositionsPlanar.push_back(l_CurPos);

      l_TrailShape.AddUVCoord(0, GVector2D<REAL32>(l_fDelta, 0.5f) );
   }

   // Update line strip
   l_TrailLineStrip.Start() = 0;
   l_TrailLineStrip.NbLines(l_iNbDivs);

   // Build trajectory rotation matrix
   GMatrix4x4<REAL32> l_PlaneMatrix;
   l_PlaneMatrix.LoadIdentity();

   l_PlaneMatrix(0,0) = l_Right.x;
   l_PlaneMatrix(0,1) = l_Right.y;
   l_PlaneMatrix(0,2) = l_Right.z;

   l_PlaneMatrix(1,0) = l_Up.x;
   l_PlaneMatrix(1,1) = l_Up.y;
   l_PlaneMatrix(1,2) = l_Up.z;

   l_PlaneMatrix(2,0) = l_Out.x;
   l_PlaneMatrix(2,1) = l_Out.y;
   l_PlaneMatrix(2,2) = l_Out.z;

   // Rotate trajectory plane & update the shape
   l_TrailShape.NbPositions(l_PositionsPlanar.size() );
   for(UINT32 i = 0;i < l_PositionsPlanar.size();i ++)
   {
      GVector3D<REAL32>& l_TrailPos = l_TrailShape.Position(i);

      l_TrailPos = l_PlaneMatrix * l_PositionsPlanar[i];

      CGFX_Key_Linear l_Key;
      l_Key.m_Time = in_fStartTime + i * l_fTimeInc;
      l_Key.m_Value = l_TrailPos.x;
      l_pAnimX->Add_Key(l_Key);

      l_Key.m_Value = l_TrailPos.y;
      l_pAnimY->Add_Key(l_Key);

      l_Key.m_Value = l_TrailPos.z;
      l_pAnimZ->Add_Key(l_Key);
   }

   if(out_pLastPosition)
   {
      *out_pLastPosition = l_TrailShape.Position(l_PositionsPlanar.size() - 1);
   }

   // Create the nuke blink animation
   CGFX_Animation_Linear_Itf* l_pAnimBlink = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pAnimBlink->Connect_Custom(l_pTrailModel, 4);
   in_pAction->Add_Animation(l_pAnimBlink);

   CGFX_Key_Linear l_BlinkKey;

   Random::GQuick l_Rand;
   l_Rand.Seed( (UINT32) (in_TargetCoord.x * in_TargetCoord.y * in_SourceCoord.x * in_SourceCoord.y) & 0x7FFFFFFF);

   UINT32 l_iNbBlinkKeys = 1 + (UINT32) (m_fNukeBlinkFreq * l_fTrailTime);
   REAL32 l_fCurTime = in_fStartTime;
   for(UINT32 i = 0;i < l_iNbBlinkKeys;i ++)
   {
      l_BlinkKey.m_Time = l_fCurTime;
      l_BlinkKey.m_Value = l_Rand.RandomReal() * m_fNukeBlinkScale + m_fNukeBlinkOffset;
      l_pAnimBlink->Add_Key(l_BlinkKey);

      l_fCurTime += m_fNukeBlinkFreqInv;
   }

   l_pAnimX->Release();
   l_pAnimY->Release();
   l_pAnimZ->Release();

   return l_iModelID;
}

UINT32 GBackgroundEarth::CreateExplosionAnimation(CGFX_Model_Itf* in_pModel, 
                                                  CGFX_Action_Itf* in_pAction, 
                                                  GFX::GTextureSequence* in_pSequence, 
                                                  REAL32 in_fFPS,
                                                  REAL32 in_fStartTime)
{
   // Find a Model that will display this nuke
   UINT32 l_iExplodeModelID = 0;
   while(l_iExplodeModelID < in_pModel->Get_Nb_Children() )
   {
      if(!in_pModel->Get_Child(l_iExplodeModelID)->Animated() )
      {
         break;
      }

      l_iExplodeModelID ++;
   }

   CGFX_Model_Itf* l_pExplodeModel = NULL;
   GFX::GOverrideTxtrSeqControl* l_pExplodeControl = NULL;

   // Create a new explosion model if no one is available
   if(l_iExplodeModelID == in_pModel->Get_Nb_Children() )
   {
      l_pExplodeModel = g_Joshua.Renderer()->Create_Model();
      in_pModel->Add_Child(l_pExplodeModel);

      l_pExplodeControl = g_Joshua.Renderer()->CreateOverrideTxtrSeqControl();
      l_pExplodeControl->TextureSequence(in_pSequence);

      l_pExplodeModel->AddOverride(l_pExplodeControl);

      l_pExplodeModel->Release();
      l_pExplodeControl->Release();
   }
   else
   {
      // Fetch explosion objects
      l_pExplodeModel = in_pModel->Get_Child(l_iExplodeModelID);
      l_pExplodeControl = (GFX::GOverrideTxtrSeqControl*) l_pExplodeModel->Override(0);
   }

   // Create the explosion visibility animation
   if(!in_pSequence)
   {
      l_pExplodeModel->Visibility() = false;
      return l_iExplodeModelID;
   }

   REAL32 l_fExplosionTime = in_pSequence->NbFrames() / in_fFPS;

   CGFX_Animation_Constant_Itf* l_pAnimVis = g_Joshua.Renderer()->Create_Animation_Constant();
   l_pAnimVis->Connect(l_pExplodeModel, GFX::PARAM_VISIBILITY);
   in_pAction->Add_Animation(l_pAnimVis);

   CGFX_Key_Constant l_VisKey;
   l_VisKey.m_Time = 0;
   l_VisKey.m_Value = 0;
   l_pAnimVis->Add_Key(l_VisKey);

   l_VisKey.m_Time = in_fStartTime;
   l_VisKey.m_Value = 1;
   l_pAnimVis->Add_Key(l_VisKey);

   l_VisKey.m_Time = in_fStartTime + l_fExplosionTime;
   l_VisKey.m_Value = 0;
   l_pAnimVis->Add_Key(l_VisKey);

   SAFE_RELEASE(l_pAnimVis);

   // Create the nuke explosion sequence animation
   l_pExplodeControl->OverrideController()->AnimateByFrame(false);
   CGFX_Animation_Linear_Itf* l_pAnimSequence = g_Joshua.Renderer()->Create_Animation_Linear();

   CGFX_Key_Linear l_Keys[2] =
   {
      {in_fStartTime, 0},
      {in_fStartTime + l_fExplosionTime, 1},
   };
   l_pAnimSequence->Setup_Keys(l_Keys, 2);
   l_pAnimSequence->Connect(l_pExplodeControl->OverrideController(), GFX::PARAM_FRAME);
   in_pAction->Add_Animation(l_pAnimSequence);

   SAFE_RELEASE(l_pAnimSequence);

   in_pAction->Length(max(in_pAction->Length(), in_fStartTime + l_fExplosionTime) );

   return l_iExplodeModelID;
}


bool GBackgroundEarth::DestroyAllNukes()
{
   while(m_NukeMap.size())
   {
      DestroyNuke(m_NukeMap.begin()->first);
   }
   return true;
}

bool GBackgroundEarth::DestroyNuke(UINT32 in_iNukeID)
{
   hash_map<UINT32, SNukeInfo>::iterator l_It = m_NukeMap.find(in_iNukeID);
   if(l_It != m_NukeMap.end() )
   {
      // Hide nuke trails
      for(UINT32 i = 0;i < 3;i++)
      {
         UINT32 l_iModelID = l_It->second.m_iModelIDs[i];
         if(l_iModelID != 0xFFFFFFFF)
         {
            m_pNukeTrailMasterModel->Get_Child(l_iModelID)->Visibility() = false;
         }
      }

      // Remove nuke explosion animation
      UINT32 l_iModelID = l_It->second.m_iExplodeModelIDs[0];
      if(l_iModelID != 0xFFFFFFFF)
      {
         m_pNukeExplodeMasterModel->Get_Child(l_iModelID)->Reset_Animated(false);
         m_pNukeExplodeMasterModel->Get_Child(l_iModelID)->Visibility() = false;
      }

      // Remove nuke flash animation
      l_iModelID = l_It->second.m_iExplodeModelIDs[3];
      if(l_iModelID != 0xFFFFFFFF)
      {
         m_pNukeFlashMasterModel->Get_Child(l_iModelID)->Reset_Animated(false);
         m_pNukeFlashMasterModel->Get_Child(l_iModelID)->Visibility() = false;
      }

      // Remove AMDS explosion animation
      l_iModelID = l_It->second.m_iExplodeModelIDs[1];
      if(l_iModelID != 0xFFFFFFFF)
      {
         m_pAMDSExplodeMasterModel[l_It->second.m_iAMDSExplodeType[0] ]->Get_Child(l_iModelID)->Reset_Animated(false);
         m_pAMDSExplodeMasterModel[l_It->second.m_iAMDSExplodeType[0] ]->Get_Child(l_iModelID)->Visibility() = false;
      }

      // Remove AMDS explosion animation
      l_iModelID = l_It->second.m_iExplodeModelIDs[2];
      if(l_iModelID != 0xFFFFFFFF)
      {
         m_pAMDSExplodeMasterModel[l_It->second.m_iAMDSExplodeType[1] ]->Get_Child(l_iModelID)->Reset_Animated(false);
         m_pAMDSExplodeMasterModel[l_It->second.m_iAMDSExplodeType[1] ]->Get_Child(l_iModelID)->Visibility() = false;
      }

      // Remove nuke trail animation & delete it
      l_It->second.m_pNukeAction->Release();

      // Remove the nuke from the DAL
      g_ClientDAL.RemoveLaunchedNuke(in_iNukeID);

      m_NukeMap.erase(l_It);

      return true;
   }

   return false;
}

void GBackgroundEarth::UpdateNukes()
{
   // Make sure there is at least one nuke to display
   if(m_NukeMap.size() > 0)
   {
      m_pNukeTrailMasterModel->Visibility() = true;
      m_pNukeIconMasterModel->Visibility() = true;

      vector<UINT32> l_vNukesToHide;

      GVector2D<REAL32> l_ScreenTransformScale = GVector2D<REAL32>(m_Mode.m_Resolution) * 0.5f;
      GVector2D<REAL32> l_ScreenTransformOffset(l_ScreenTransformScale);
      l_ScreenTransformScale.y *= -1;
      l_ScreenTransformOffset.x -= (m_NukeIconSizeHalf.x + 0.5f);
      l_ScreenTransformOffset.y -= (m_NukeIconSizeHalf.x + 0.5f);

      // For each child we currently have for this Model, change its information (location) to reflect a visible nuke
      UINT32 l_iCurrentChild = 0;
      stdext::hash_map<UINT32, SNukeInfo>::iterator l_NukeIt = m_NukeMap.begin();
      while(l_NukeIt != m_NukeMap.end() )
      {
         SNukeInfo& l_NukeInfo = l_NukeIt->second;
         REAL32 l_fActionTime = (REAL32) (g_Joshua.GameTime() - l_NukeInfo.m_fNukeStartTime);
         l_NukeInfo.m_pNukeAction->Update_Animations(l_fActionTime);

         if(l_NukeInfo.m_iExplodeModelIDs[0] != 0xFFFFFFFF)
         {
            CGFX_Model_Itf* l_pExplodeModel = m_pNukeFlashMasterModel->Get_Child(l_NukeInfo.m_iExplodeModelIDs[3] );
            if(l_pExplodeModel->Visibility() && !(l_NukeInfo.m_iFlags & SNukeInfo::c_fExploded))
            {
               l_NukeInfo.m_iFlags |= SNukeInfo::c_fExploded;
               // Play sound nuke impact
               g_SP2Client->PlaySound(ESoundEventID::NuclearImpact1);
            }
         }

         // Check if nuke explosion is over
         if(l_fActionTime > l_NukeInfo.m_pNukeAction->Length() )
         {
            l_vNukesToHide.push_back(l_NukeIt->first);
         }
         else
         {
            // Update nuke/AMDS icon
            for(UINT32 i = 0;i < 3;i ++)
            {
               UINT32 l_iModelID = l_NukeInfo.m_iModelIDs[i];
               if(l_iModelID != 0xFFFFFFFF)
               {
                  CGFX_Model_Itf* l_pTrailModel = m_pNukeTrailMasterModel->Get_Child(l_iModelID);

                  // Make sure nuke has not exploded yet
                  if(l_pTrailModel->Custom_Parameter(3) == 0)
                  {
                     // Fetch nuke coordinates
                     GVector3D<REAL32> l_NukeCoordinate;
                     l_NukeCoordinate.x = l_pTrailModel->Custom_Parameter(0);
                     l_NukeCoordinate.y = l_pTrailModel->Custom_Parameter(1);
                     l_NukeCoordinate.z = l_pTrailModel->Custom_Parameter(2);

                     // Transform in camera space
                     GVector3D<REAL32> l_ViewNukePos = m_ViewMat * l_NukeCoordinate;

                     REAL32 l_fA = l_ViewNukePos.x * l_ViewNukePos.x +
                        l_ViewNukePos.y * l_ViewNukePos.y +
                        l_ViewNukePos.z * l_ViewNukePos.z;
                     REAL32 l_fB = -2 * l_ViewNukePos.x * m_OriginPos.x +
                        -2 * l_ViewNukePos.y * m_OriginPos.y +
                        -2 * l_ViewNukePos.z * m_OriginPos.z;
                     REAL32 l_fC = m_OriginPos.x * m_OriginPos.x +
                        m_OriginPos.y * m_OriginPos.y +
                        m_OriginPos.z * m_OriginPos.z - 1;

                     REAL32 l_fDis = l_fB * l_fB - 4 * l_fA * l_fC;

                     // Verify if nuke is in front of earth
                     if( (l_fDis < 0) || (l_ViewNukePos.z < m_OriginPos.z - 1 / m_OriginPos.z) )
                     {
                        GVector2D<REAL32> l_ScreenNukePos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewNukePos) ).ApplyWeigth() );

                        // Verify if nuke is visible in screen
                        if( (l_ScreenNukePos.x >= -1.1f) &&
                           (l_ScreenNukePos.x <=  1.1f) &&
                           (l_ScreenNukePos.y >= -1.1f) &&
                           (l_ScreenNukePos.y <=  1.1f) )
                        {
                           // Update current child object with nuke screen position
                           l_ScreenNukePos *= l_ScreenTransformScale;
                           l_ScreenNukePos += l_ScreenTransformOffset;

                           // Verify if a child is already created for this nuke
                           GFX::GOverridePSCascade* l_pPSOverride = NULL;
                           if(l_iCurrentChild < m_pNukeIconMasterModel->Get_Nb_Children() )
                           {
                              // Update child data
                              m_pNukeIconMasterModel->Get_Child(l_iCurrentChild)->Visibility() = true;

                              l_pPSOverride = (GFX::GOverridePSCascade*) m_pNukeIconMasterModel->Get_Child(l_iCurrentChild)->Override(0);

                              CGFX_Transform_2D_Itf* l_pTransform = (CGFX_Transform_2D_Itf*) m_pNukeIconMasterModel->Get_Child(l_iCurrentChild)->Get_Transform();
                              l_pTransform->Position() = l_ScreenNukePos;
                           }
                           else
                           {
                              // Create a new child & its transform
                              CGFX_Model_Itf* l_pModel = g_Joshua.Renderer()->Create_Model();
                              CGFX_Transform_2D_Itf* l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                              // Setup child
                              m_pNukeIconMasterModel->Add_Child(l_pModel);
                              l_pModel->Set_Transform(l_pTransform);
                              l_pModel->Set_Primitive(m_pNukeIconMesh);

                              // Setup override
                              l_pPSOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
                              l_pPSOverride->UseColorFactorOverride(true);
                              l_pModel->AddOverride(l_pPSOverride);

                              // Setup transform
                              l_pTransform->Position() = l_ScreenNukePos;
                              l_pTransform->Size() = m_NukeIconSize;

                              g_Joshua.GUIManager()->SetDirty();
                           }

                           l_pPSOverride->ColorFactorOverride() = m_NukeBlinkColors[i] * l_pTrailModel->Custom_Parameter(4);

                           // Go to next child
                           l_iCurrentChild ++;
                        }
                     }
                  }
               }
            }

            // Update AMDS interception
            for(UINT32 i = 0;i < 2;i ++)
            {
               UINT32 l_iModelID = l_NukeInfo.m_iExplodeModelIDs[i + 1];
               if(l_iModelID != 0xFFFFFFFF)
               {
                  CGFX_Model_Itf* l_pExplodeModel = m_pAMDSExplodeMasterModel[l_NukeInfo.m_iAMDSExplodeType[i] ]->Get_Child(l_iModelID);

                  // Make sure AMDS is exploding
                  if(l_pExplodeModel->Visibility() )
                  {
                     CGFX_Transform_Itf* l_pTfx = l_pExplodeModel->Get_Transform();
                     CameraAlignTransform(l_pTfx);
                  }
               }
            }

            // Update flash
            UINT32 l_iFlashModelID = l_NukeInfo.m_iExplodeModelIDs[3];
            if(l_iFlashModelID != 0xFFFFFFFF)
            {
               CGFX_Model_Itf* l_pFlashModel = m_pNukeFlashMasterModel->Get_Child(l_iFlashModelID);

               // Make sure AMDS is exploding
               if(l_pFlashModel->Visibility() )
               {
                  CGFX_Transform_Itf* l_pTfx = l_pFlashModel->Get_Transform();
                  GMatrix4x4<REAL32>& l_Mat = l_pTfx->Get_Matrix();
                  GVector3D<REAL32> l_Pos(l_Mat(3, 0), l_Mat(3, 1), l_Mat(3, 2) );

                  if(ClipInFrontOfEarth(l_Pos) )
                  {
                     CameraAlignTransform(l_pTfx);
                  }
                  else
                  {
                     Memory::Clear(&l_Mat);
                     l_Mat(3, 0) = l_Pos.x;
                     l_Mat(3, 1) = l_Pos.y;
                     l_Mat(3, 2) = l_Pos.z;
                     l_Mat(3, 3) = 1.f;

                     l_pTfx->Set_Matrix(l_Mat);
                  }
               }
            }
         }

         // Go to next nuke
         ++ l_NukeIt;
      }

      // Hide remaining children
      HideOverlayComponents(m_pNukeIconMasterModel, l_iCurrentChild);

      // Destroy nuke that have reached destination
      for(UINT32 i = 0;i < l_vNukesToHide.size();i ++)
      {
         DestroyNuke(l_vNukesToHide[i] );
      }
   }
   else
   {
      // Hide nukes
      m_pNukeTrailMasterModel->Visibility() = false;
      m_pNukeIconMasterModel->Visibility() = false;
   }
}

/*!
* Handles the context menu command.
*  Check which item of the menu has been pressed and does what
*  must be done given the situation.
*  @param GContextMenuItem* in_pItem: Pointer to the selectionned item
*  @return void
**/
void GBackgroundEarth::HandleContextMenu(GContextMenuItem* in_pItem)
{
   g_ClientDAL.FetchCountriesRelations();

	//      Occupy territory handler
   //----------------------------------
   if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuOccupyTerritory))
   {
      //m_iCountryToOccupy is valid here.
      g_ClientDCL.OccupyTerritory(m_iCountryToOccupy);
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuRelations))
   {
      //m_iCountryToCheckRelations is valid here.
		g_ClientDDL.RelationsWindow()->UpdateWithNewCountries(g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
		g_ClientDDL.RelationsWindow()->Show();
      return;      
   }
   //      Bombard region handler
   //----------------------------------
   if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuBombardRegion))
   {
      g_ClientDCL.BombardRegionWithSelectedUnits(m_iRegionToBombard);
      return;
   }

	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyAlliance))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::Alliance,g_ClientDAL.GetString(EStrId::TreatyAlliance),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }

	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyAnnexation))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::Annexation,g_ClientDAL.GetString(EStrId::TreatyAnnexation),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::AssumeForeignDebt,g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyCommonMarket))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::CommonMarket,g_ClientDAL.GetString(EStrId::TreatyCommonMarket),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::CulturalExchanges,g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyEconomicAid))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::EconomicAid,g_ClientDAL.GetString(EStrId::TreatyEconomicAid),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::EconomicEmbargo,g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::EconomicPartnership,g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyFreeRegion))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::FreeRegion,g_ClientDAL.GetString(EStrId::TreatyFreeRegion),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::HumanDevelopmentCollaboration,g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::MilitaryAccess,g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyNobleCause))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::NobleCause,g_ClientDAL.GetString(EStrId::TreatyNobleCause),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::RequestMilitaryPresenceRemoval,g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyResearchPartnership))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::ResearchPartnership,g_ClientDAL.GetString(EStrId::TreatyResearchPartnership),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyWeaponTrade))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::WeaponTrade,g_ClientDAL.GetString(EStrId::TreatyWeaponTrade),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
		g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::WeaponTradeEmbargo,g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo),g_ClientDAL.ControlledCountryID(),m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
   // Request war treaty
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyRequestWar))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
      g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::War, 
                                                           g_ClientDAL.GetString(EStrId::TreatyWar),
                                                           g_ClientDAL.ControlledCountryID(),
                                                           m_iCountryToCheckRelations);
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
   // Assume foreign debt inverted treaty
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebtInverted))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
      g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::AssumeForeignDebt,
                                                           g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt),
                                                           m_iCountryToCheckRelations,
                                                           g_ClientDAL.ControlledCountryID());
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }
   // Free region inverted treaty
	if(in_pItem->Text() == g_ClientDAL.GetString(EStrId::TreatyFreeRegionInverted))
   {
		if(!g_ClientDDL.TreatyDetailsWindow())
			g_ClientDDL.TreatiesWindowSpawn();
      g_ClientDDL.TreatyDetailsWindow()->QuickTreatyWindow(ETreatyType::FreeRegion,
                                                           g_ClientDAL.GetString(EStrId::TreatyFreeRegion),
                                                           m_iCountryToCheckMilitary,
                                                           g_ClientDAL.ControlledCountryID());
      g_ClientDDL.TreatyDetailsWindow()->Show();
      return;      
   }

   if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::AttackWithSelected]))
   {
      UINT32 l_ClickedUnit = FindVisibleUnitAt(m_ClickPosition); 
      if (l_ClickedUnit) 
      {
         SDK::Combat::GUnitGroup * l_UnitGroup = g_Joshua.UnitManager().UnitGroup(l_ClickedUnit);
         if (l_UnitGroup->OwnerId() != (UINT32)g_ClientDAL.ControlledCountryID())
         {
            m_ClickLongLat = l_UnitGroup->Position();     
            vector<UINT32> l_vMovedUnits;
            for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
            {
               l_vMovedUnits.push_back(*itr);
            }
            g_ClientDCL.RequestUnitMove(l_vMovedUnits, m_ClickLongLat, EMilitaryAction::Attack);
         }
      }
      return;
   }
   //       Merge selected with
   //---------------------------------
   if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::MergeSelectedWith]))
   {
      //Hide the small unit info window
      g_ClientDDL.UnitInfoSmallWindow()->Hide();


      UINT32 l_ClickedUnit = FindVisibleUnitAt(m_ClickPosition); 
      if (l_ClickedUnit) 
      {
         SP2::GUnitGroup * l_pUnitGroup = (SP2::GUnitGroup *)g_Joshua.UnitManager().UnitGroup(l_ClickedUnit);
         if( (l_pUnitGroup->OwnerId() == (UINT32)g_ClientDAL.ControlledCountryID()) && 
             (l_pUnitGroup->Status() != EMilitaryStatus::InDeployement)
           )
         {
            m_ClickLongLat = l_pUnitGroup->Position();     
            vector<UINT32> l_vMovedUnits;
            for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
            {
               if (*itr != l_ClickedUnit)
               {
                  l_vMovedUnits.push_back(*itr);
               }
            }
            g_ClientDCL.RequestUnitMove(l_vMovedUnits, m_ClickLongLat, EMilitaryAction::MergeWith);
         }
      }
      return;
   }

   //       Move selected unit
   //---------------------------------
   if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::MoveSelectedUnit]))
   {
      vector<UINT32> l_vMovedUnits;
      for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
      {
         l_vMovedUnits.push_back(*itr);
      }
      g_ClientDCL.RequestUnitMove(l_vMovedUnits, m_ClickLongLat, EMilitaryAction::Default);
      return;
   }

   //       GeographicMap
   //---------------------------------
   if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::Geographic]))
   {
      g_ClientDCL.CurrentThematicMap = ECharacteristicToHighlight::Geographic;
      g_ClientDCL.HighlightCountry((void*)g_ClientDCL.SelectedCountryID());
      //Hide the thematic map legend
      g_ClientDDL.ThematicMapLegendKill();
   }
	else if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[EContextMenuEntries::PoliticalControl]))  
   {
      g_ClientDCL.CurrentThematicMap = ECharacteristicToHighlight::PoliticalControl;
      g_ClientDCL.HighlightCountry((void*)g_ClientDCL.SelectedCountryID());
      //Hide the thematic map legend
      g_ClientDDL.ThematicMapLegendKill();
   }



   //       Thematic map painting
   //-----------------------------------
   {
      SP2::ECharacteristicToHighlight::Enum l_eCharac = ECharacteristicToHighlight::CountryEmpire;
      bool l_bMustPaint                               = false;
      for (int i = EContextMenuEntries::ThematicMapsBegin; i <= EContextMenuEntries::ThematicMapsEnd; i++)   
      {
         if(in_pItem->Text() == g_ClientDAL.GetString(c_ContextMenuEntriesId[i]))
         {
            l_eCharac = (ECharacteristicToHighlight::Enum)i;
            l_bMustPaint = true;
            break;
         }
      }
   
      if(l_bMustPaint)
      {         
         g_ClientDAL.RegionsCharacteristic(l_eCharac, g_ClientDCL.SelectedCountryID());
      }
   }

   return;
}


/*!
* Handles the military context menu command.
*  Check which item of the menu has been pressed and does what
*  must be done given the situation.
*  @param GContextMenuItem* in_pItem: Pointer to the selectionned item
*  @return void
**/
void GBackgroundEarth::HandleContextMenuDeployement(GContextMenuItem* in_pItem)
{
  
   // View Unit List
   if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuDeployementTextId[0]))
   {
      if(g_ClientDAL.SelectedUnitsID().size())
      {
         if(!g_ClientDDL.UnitListWindowNew())
            g_ClientDDL.UnitListWindowNewSpawn();

         SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );
         
         g_ClientDDL.UnitListWindowNew()->UpdateUnitList( (UINT16)l_pGroup->OwnerId(), l_pGroup->Id() );
         g_ClientDDL.UnitListWindowNew()->Show();   
      }
   }
   else
   // Deselect
   if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuDeployementTextId[1]))
   {
      g_ClientDAL.UnselectUnits();
      g_ClientDDL.UnitInfoSmallWindow()->Hide();
      if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
         g_ClientDDL.UnitListWindowNew()->Hide();
   }
   else
   //  Cancel Order
   if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuDeployementTextId[2]))
   {
      // Build Event
      SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GCancelDeployement);
      SP2::Event::GCancelDeployement* l_pCancelDeployementEvent = (SP2::Event::GCancelDeployement*) (l_pEvent.get() );

      // Set Target and Source for this event
      l_pCancelDeployementEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
      l_pCancelDeployementEvent->m_iSource = g_SP2Client->Id();

      // Iterate over selected group
      set<UINT32>::const_iterator l_GroupIdIterator = g_ClientDAL.SelectedUnitsID().begin();
      while(l_GroupIdIterator != g_ClientDAL.SelectedUnitsID().end())
      {
         // Send group that are in deployement
         SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupIdIterator);
         if(l_pGroup->Status() == EMilitaryStatus::InDeployement)
            l_pCancelDeployementEvent->m_GroupIdListToCancel.push_back(*l_GroupIdIterator);
         l_GroupIdIterator++;
      }

      // Fire event
      g_Joshua.RaiseEvent(l_pEvent);	
   }
}


/*!
* Handles the military context menu command.
*  Check which item of the menu has been pressed and does what
*  must be done given the situation.
*  @param GContextMenuItem* in_pItem: Pointer to the selectionned item
*  @return void
**/
void GBackgroundEarth::HandleContextMenuMilitary(GContextMenuItem* in_pItem)
{
   for(UINT32 i = 0;i < c_iContextMenuMilitaryCount;i ++)
   {
      GString l_sItemText = g_ClientDAL.GetString(c_iContextMenuMilitaryTextId[i]);
      if(in_pItem->Text() == l_sItemText)
      {
         //Move
         if(in_pItem->Text() == g_ClientDAL.GetString(101098))
         {
            m_bUnitPicked = true;
            m_UnitPickedAction = EMilitaryAction::Default;
            g_Joshua.GUIManager()->SetMousePointer(m_PointerMoveUnitTo);
            return;
         }//Move and attack
         else if(in_pItem->Text() == g_ClientDAL.GetString(101099))
         {
            m_bUnitPicked = true;
            m_UnitPickedAction = EMilitaryAction::Attack;
            g_Joshua.GUIManager()->SetMousePointer(m_PointerMoveUnitAttack);
            return;
         }// Deselect
         else if(in_pItem->Text() == g_ClientDAL.GetString(101101))
         {
            g_ClientDAL.UnselectUnits();
            g_ClientDDL.UnitInfoSmallWindow()->Hide();
            if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
               g_ClientDDL.UnitListWindowNew()->Hide();

            return;
         }//Merge with
         else if(in_pItem->Text() == g_ClientDAL.GetString(101102))
         {
            //Hide the small unit info window
            g_ClientDDL.UnitInfoSmallWindow()->Hide();
            m_bUnitPicked = true;            
            m_UnitPickedAction = EMilitaryAction::MergeWith;
            g_Joshua.GUIManager()->SetMousePointer(m_PointerMergeUnit);
            return;
         }//View units list
         else if(in_pItem->Text() == g_ClientDAL.GetString(101103))
         {
            if(g_ClientDAL.SelectedUnitsID().size())
            {
               if(!g_ClientDDL.UnitListWindowNew())
                  g_ClientDDL.UnitListWindowNewSpawn();

               SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );
               
               g_ClientDDL.UnitListWindowNew()->UpdateUnitList( (UINT16)l_pGroup->OwnerId(), l_pGroup->Id() );
               g_ClientDDL.UnitListWindowNew()->Show();   
            }
         }//Split
         else if(in_pItem->Text() == g_ClientDAL.GetString(102094)) 
         {
            if( g_ClientDAL.SelectedUnitsID().size() )
            {
               if(!g_ClientDDL.GroupSplitWindow())
                  g_ClientDDL.GroupSplitWindowSpawn();

               SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *g_ClientDAL.SelectedUnitsID().begin() );
               
               g_ClientDDL.GroupSplitWindow()->Update( l_pGroup->Id() );
               g_ClientDDL.GroupSplitWindow()->Show();
            }
         }
      }
   }

   for(UINT32 i = 0;i <= EMilitaryStatus::Fortified;i ++)
   {
      if(in_pItem->Text() == g_ClientDAL.GetString(c_iContextMenuMilitaryChangeStatusId[i]) )
      {
         g_ClientDCL.ChangeUnitStatus( (EMilitaryStatus::Enum) i);
         return;
      }
   }
}

bool GBackgroundEarth::UpdateEarthPosition(SEarthState & in_State)
{
   // Make sure longitude is in range [-pi;pi]
   while(in_State.m_Longitude > Math::PI)
   {
      in_State.m_Longitude -= 2 * (REAL32)Math::PI;
   }

   while(in_State.m_Longitude < -Math::PI)
   {
      in_State.m_Longitude += 2 * (REAL32)Math::PI;
   }

   // Make sure latitude is in range [-max_latitude;max_latitude]
   in_State.m_Latitude = (REAL32)max(min(in_State.m_Latitude, m_fMaxLatitude), m_fMinLatitude);

   m_bCamDirty = true;
   m_CurrentState = in_State;

   // Update camera using current state
   REAL32 l_Pow = m_CurrentState.m_Zoom * m_fDistCalcA + m_fDistCalcB;
   m_pfCamDist->Value(powf(2, l_Pow));
   m_pfCamAngleX->Value(m_CurrentState.m_Longitude + (REAL32)Math::PI);
   m_pfCamAngleY->Value(m_CurrentState.m_Latitude);

   g_ClientDDL.MiniMap()->UpdateCrosshair(in_State);

   return true;
}

GVector3D<REAL32> GBackgroundEarth::ConvertSphericalToRectangular(const GVector2D<REAL32>& in_Position)
{
   REAL32 l_fPhi = in_Position.x * (REAL32) Math::PI / 180.f;
   REAL32 l_fTheta = (90 - in_Position.y) * (REAL32) Math::PI / 180.f;

   GVector3D<REAL32> l_Pos3D;
   l_Pos3D.z = - sinf(l_fTheta) * sinf(l_fPhi);
   l_Pos3D.y = cosf(l_fTheta);
   l_Pos3D.x = - sinf(l_fTheta) * cosf(l_fPhi);

   return l_Pos3D;
}

GVector3D<REAL32> GBackgroundEarth::ConvertSphericalToRectangular(const GVector3D<REAL32>& in_Position)
{
   return ConvertSphericalToRectangular( *( (GVector2D<REAL32>*) &in_Position) ) * in_Position.z;
}

GVector2D<REAL32> GBackgroundEarth::ConvertRectangularToSpherical(const GVector3D<REAL32>& in_Position)
{
   GVector2D<REAL32> l_Result;
   l_Result.x = 90.f + atan2f(in_Position.x, -in_Position.z) * 180 / (REAL32)Math::PI;
   l_Result.y = (((REAL32)-Math::PI * 0.5f) + acosf(-in_Position.y) )* 180 / (REAL32)Math::PI;

   return l_Result;
}

bool GBackgroundEarth::ClipOnEarth(const GVector3D<REAL32>& in_Point) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;

   // Verify if point is in front of earth (on earth)
   if(l_ViewPos.z < m_fOriginDistInv)
   {
      return true;
   }

   return false;
}

bool GBackgroundEarth::ClipInFrontOfEarth(const GVector3D<REAL32>& in_Point) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;

   REAL32 l_fA = l_ViewPos.x * l_ViewPos.x +
      l_ViewPos.y * l_ViewPos.y +
      l_ViewPos.z * l_ViewPos.z;

   REAL32 l_fB = -2 * (l_ViewPos.x * m_OriginPos.x +
      l_ViewPos.y * m_OriginPos.y +
      l_ViewPos.z * m_OriginPos.z);

   REAL32 l_fDis = l_fB * l_fB - 4 * l_fA * m_fOriginDist;

   // Verify if point is in front of earth
   if( (l_fDis < 0) || (l_ViewPos.z < m_fOriginDistInv) )
   {
      return true;
   }

   return false;
}

bool GBackgroundEarth::Clip3DPointIn2D(const GVector3D<REAL32>& in_Point) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;
   GVector2D<REAL32> l_ScreenPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewPos) ).ApplyWeigth() );

   // Verify if point is visible in screen
   if( (l_ScreenPos.x >= -1.f) &&
       (l_ScreenPos.x <=  1.f) &&
       (l_ScreenPos.y >= -1.f) &&
       (l_ScreenPos.y <=  1.f) )
   {
      return true;
   }

   return false;
}

void GBackgroundEarth::Project3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;
   GVector2D<REAL32> l_ScreenPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewPos) ).ApplyWeigth() );

   // Transform to window coordinate
   out_Result.x = RoundToInt(l_ScreenPos.x * m_ScreenTransformScale.x + m_ScreenTransformOffset.x);
   out_Result.y = RoundToInt(l_ScreenPos.y * m_ScreenTransformScale.y + m_ScreenTransformOffset.y);
}

bool GBackgroundEarth::ProjectAndClip3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;
   GVector2D<REAL32> l_ScreenPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewPos) ).ApplyWeigth() );

   // Verify if point is visible in screen
   if( (l_ScreenPos.x >= -1.1f) &&
      (l_ScreenPos.x <=  1.1f) &&
      (l_ScreenPos.y >= -1.1f) &&
      (l_ScreenPos.y <=  1.1f) )
   {
      // Transform to window coordinate
      out_Result.x = RoundToInt(l_ScreenPos.x * m_ScreenTransformScale.x + m_ScreenTransformOffset.x);
      out_Result.y = RoundToInt(l_ScreenPos.y * m_ScreenTransformScale.y + m_ScreenTransformOffset.y);

      return true;
   }

   return false;
}

bool GBackgroundEarth::ProjectAndClipOnEarth3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;

   // Verify if point is in front of earth (on earth)
   if(l_ViewPos.z < m_fOriginDistInv)
   {
      GVector2D<REAL32> l_ScreenPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewPos) ).ApplyWeigth() );

      // Verify if point is visible in screen
      if( (l_ScreenPos.x >= -1.1f) &&
         (l_ScreenPos.x <=  1.1f) &&
         (l_ScreenPos.y >= -1.1f) &&
         (l_ScreenPos.y <=  1.1f) )
      {
         // Transform to window coordinate
         out_Result.x = RoundToInt(l_ScreenPos.x * m_ScreenTransformScale.x + m_ScreenTransformOffset.x);
         out_Result.y = RoundToInt(l_ScreenPos.y * m_ScreenTransformScale.y + m_ScreenTransformOffset.y);

         return true;
      }
   }

   return false;
}

bool GBackgroundEarth::ProjectAndClipInFrontOfEarth3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const
{
   // Transform in camera space
   GVector3D<REAL32> l_ViewPos = m_ViewMat * in_Point;

   REAL32 l_fA = l_ViewPos.x * l_ViewPos.x +
      l_ViewPos.y * l_ViewPos.y +
      l_ViewPos.z * l_ViewPos.z;

   REAL32 l_fB = -2 * (l_ViewPos.x * m_OriginPos.x +
      l_ViewPos.y * m_OriginPos.y +
      l_ViewPos.z * m_OriginPos.z);

   REAL32 l_fDis = l_fB * l_fB - 4 * l_fA * m_fOriginDist;

   // Verify if point is in front of earth
   if( (l_fDis < 0) || (l_ViewPos.z < m_fOriginDistInv) )
   {
      GVector2D<REAL32> l_ScreenPos = ( (m_ProjMat * GVector4D<REAL32>(l_ViewPos) ).ApplyWeigth() );

      // Verify if point is visible in screen
      if( (l_ScreenPos.x >= -1.1f) &&
         (l_ScreenPos.x <=  1.1f) &&
         (l_ScreenPos.y >= -1.1f) &&
         (l_ScreenPos.y <=  1.1f) )
      {
         // Transform to window coordinate
         out_Result.x = RoundToInt(l_ScreenPos.x * m_ScreenTransformScale.x + m_ScreenTransformOffset.x);
         out_Result.y = RoundToInt(l_ScreenPos.y * m_ScreenTransformScale.y + m_ScreenTransformOffset.y);

         return true;
      }
   }

   return false;
}

GVector2D<REAL32> GBackgroundEarth::ScreenAlignTransform(const GVector2D<REAL32>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset)
{
   return GVector2D<REAL32>( (REAL32) (RoundToInt(in_ScreenPosition.x) + in_ScreenOffset.x) - 0.5f, 
      (REAL32) (RoundToInt(in_ScreenPosition.y) + in_ScreenOffset.y) - 0.5f);
}

GVector2D<REAL32> GBackgroundEarth::ScreenAlignTransform(const GVector2D<INT32>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset)
{
   return GVector2D<REAL32>( (REAL32) (in_ScreenPosition.x + in_ScreenOffset.x) - 0.5f, 
      (REAL32) (in_ScreenPosition.y + in_ScreenOffset.y) - 0.5f);
}

GVector2D<REAL32> GBackgroundEarth::ScreenAlignTransform(const GVector2D<INT16>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset)
{
   return GVector2D<REAL32>( (REAL32) (in_ScreenPosition.x + in_ScreenOffset.x) - 0.5f, 
      (REAL32) (in_ScreenPosition.y + in_ScreenOffset.y) - 0.5f);
}

void GBackgroundEarth::CameraAlignTransform(CGFX_Transform_Itf* in_pTfx)
{
   gassert(in_pTfx->Type() == GFX::TYPE_TRANSFORM, "Invalid tranform type");

   GMatrix4x4<REAL32>& l_Matrix = in_pTfx->Get_Matrix();
   GVector3D<REAL32> l_Pos(l_Matrix(3, 0), 
      l_Matrix(3, 1), 
      l_Matrix(3, 2) );

   l_Matrix = m_ViewMat.Transpose();

   l_Matrix(0, 3) = 0;
   l_Matrix(1, 3) = 0;
   l_Matrix(2, 3) = 0;

   l_Matrix(3, 0) = l_Pos.x;
   l_Matrix(3, 1) = l_Pos.y;
   l_Matrix(3, 2) = l_Pos.z;

   in_pTfx->Set_Matrix(l_Matrix);
}

void GBackgroundEarth::HideOverlayComponents(CGFX_Model_Itf* in_pModel, UINT32 l_iNextAvailableChild)
{
   if(l_iNextAvailableChild)
   {
      in_pModel->Visibility() = true;
      while(l_iNextAvailableChild < in_pModel->Get_Nb_Children() )
      {
         in_pModel->Get_Child(l_iNextAvailableChild)->Visibility() = false;

         l_iNextAvailableChild ++;
      }
   }
   else
   {
      in_pModel->Visibility() = false;
   }
}

/*!
 * Toggles the display of the units (either as flags or as icons)
 **/
void GBackgroundEarth::ToggleUnitDisplayIconsFlags()
{
   m_bDisplayUnitsAsFlags = !m_bDisplayUnitsAsFlags;
   // be insync with the mini map
   if(g_ClientDDL.MiniMap())
   {
      g_ClientDDL.MiniMap()->ToggleMagnifier(m_bDisplayUnitsAsFlags);
   }
}

bool GBackgroundEarth::OnLoseFocus(const GBaseObject * in_pFocusTo, GBaseObject * Caller)
{
   if(m_bSelectBox)
   {
      m_bSelectBox = false;
      m_pDragBoxModel->Visibility() = false;
      g_ClientDAL.UnselectUnits();
      for (set<UINT32>::iterator itr = m_OrigSelectedGroups.begin(); itr != m_OrigSelectedGroups.end(); itr++)
         g_ClientDAL.SelectUnit(*itr, true);
   }
   else if (m_bUnitPicked || m_bUnitDragging)
   {
      m_bUnitDragging = m_bUnitPicked = false;         
      m_UnitToDrag = 0;
      g_ClientDDL.ResetMousePointer();         
   }      

   return true;
}

void GBackgroundEarth::CheckAndShowQuickTreatyAllianceContextMenu(GContextMenu* in_pMenu,
																						  UINT32 in_iHumanControlledCountry,
																						  UINT32 in_iAICountry)
{
	EDiplomaticStatus::Enum l_eStatus = g_ClientDAL.DiplomaticStatus(in_iHumanControlledCountry,in_iAICountry);
	if(l_eStatus == EDiplomaticStatus::Allied ||
		l_eStatus == EDiplomaticStatus::Hostile)
		return;
	in_pMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyAlliance));
}

void GBackgroundEarth::CheckAndShowQuickTreatyAnnexationContextMenu(GContextMenu* in_pMenu,
																						  UINT32 in_iHumanControlledCountry,
																						  UINT32 in_iAICountry)
{
   // add only if in tutorials mode
   if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      return;

	//Check if the first country has military control over a the other country
	const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();
	const set<UINT32>& l_vPoliticalControls = g_ClientDAL.CountryPoliticalControl(in_iAICountry);

	for(set<UINT32>::const_iterator it = l_vPoliticalControls.begin();
		 it != l_vPoliticalControls.end(); it++)
	{
		if(l_vControls[*it].m_iMilitary == in_iHumanControlledCountry)
		{
			in_pMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyAnnexation));
			in_pMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyFreeRegion));
			return;
		}
	}
}

void GBackgroundEarth::CheckAndShowQuickTreatyMilitaryAccessContextMenu(GContextMenu* in_pMenu,
																								UINT32 in_iHumanControlledCountry,
																								UINT32 in_iAICountry)
{
   // add only if in tutorials mode
   if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      return;

	EDiplomaticStatus::Enum l_eStatus = g_ClientDAL.DiplomaticStatus(in_iHumanControlledCountry,in_iAICountry);
	if(l_eStatus == EDiplomaticStatus::MilitaryAccess ||
		l_eStatus == EDiplomaticStatus::Hostile)
		return;
	in_pMenu->AddItem(g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess));
}

/*!
 * Checks if the we must show the relations context menu
 * and shows it if we must
 * (will be shown if we click on a country, beside our own
 * @param in_iRegionID: ID of the region to check for units in
 **/
void GBackgroundEarth::CheckAndShowRelationsContextMenu(UINT32 in_iRegionID,
																		  GContextMenu* in_pMenu)
{
   // add only if in tutorials mode
   if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      return;

	GRegionControl l_Controller = g_ClientDAL.RegionControl(in_iRegionID);

   m_iCountryToCheckRelations = l_Controller.m_iPolitical;
   m_iCountryToCheckMilitary  = l_Controller.m_iMilitary;
	
	if(m_iCountryToCheckRelations == 0 ||
		m_iCountryToCheckRelations == (UINT32)g_ClientDAL.ControlledCountryID())
		return;

	in_pMenu->AddItem(g_ClientDAL.GetString(c_iContextMenuRelations));   
}



/*!
 * Checks if the bombardment context menu should be displayed and displays it
 * if it must
 **/
void GBackgroundEarth::CheckAndShowBombardmentContextMenu(UINT32 in_iRegionID,GContextMenu* in_pMenu)
{
   // add only if in tutorials mode
   if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      return;

   //dont process water and Antartica
   if(g_ClientDAL.RegionControl(in_iRegionID).m_iPolitical == 0)
      return;

   m_iRegionToBombard       = in_iRegionID;

   GRegionControl l_Controller = g_ClientDAL.RegionControl(in_iRegionID);

   if(l_Controller.m_iPolitical != g_ClientDAL.ControlledCountryID())
   {
      in_pMenu->AddItem(g_ClientDAL.GetString(c_iContextMenuBombardRegion));   
   }	
}




/*!
 * Checks if the we must show the occupy territory context menu
 * and shows it if we must
 * (will be shown if we have units in the country that controls the given region)
 * @param in_iRegionID: ID of the region to check for units in
 **/
void GBackgroundEarth::CheckAndShowOccupyTerritoryContextMenu(UINT32 in_iRegionID,
                                                              GContextMenu* in_pMenu)
{   
   // add only if in tutorials mode
   if(GTutorials::Instance().StartedTutorial() != ETutorials::None)
      return;

	GRegionControl     l_Controller            = g_ClientDAL.RegionControl(in_iRegionID);

   m_iCountryToOccupy = l_Controller.m_iPolitical;
	
	if(m_iCountryToOccupy == 0)
		return;	

   const set<UINT32>& l_ClickedCountryRegions = g_ClientDAL.CountryPoliticalControl(l_Controller.m_iPolitical);

   //Check if we have a unit in any of those regions
   const set<UINT32>& l_OurUnits             = g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID());


   bool l_bHaveUnitsInCountry = false;

   //For each region, check if it contains any of the units
   for(set<UINT32>::const_iterator l_RegionIt = l_ClickedCountryRegions.begin();
       l_RegionIt                         != l_ClickedCountryRegions.end();
       l_RegionIt++) 
   {
      //Get the multiface that describes this region
      const Map::GMultiface* l_pMultiFace = g_ClientDAL.CountryView()->Multiface(*l_RegionIt);
      gassert(l_pMultiFace,"GBackgroundEarth::CheckAndShowOccupyTerritoryContextMenu Invalid null multiface ptr");

   
      //For each unit
      for(set<UINT32>::const_iterator l_UnitGrpIt =  l_OurUnits.begin() ; 
         l_UnitGrpIt                             != l_OurUnits.end();
         l_UnitGrpIt++)
      {
         Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(*l_UnitGrpIt);
         Map::GPoint l_GroupPosition(l_pGroup->Position());
         if(l_pMultiFace->Contains(l_GroupPosition))
         {
            l_bHaveUnitsInCountry = true;
            break;
         }
      }
      if(l_bHaveUnitsInCountry)
         break;      
   }

   //If we have units in the country
   //Add the entry to the context menu
   if(l_bHaveUnitsInCountry)
   {
      in_pMenu->AddItem(g_ClientDAL.GetString(c_iContextMenuOccupyTerritory));
   }
   return;
}

void GBackgroundEarth::ApplyElevationBump(bool in_bApply)
{
   if(in_bApply)
   {
      m_pEarthMaterial->MaximumPass(INVALID_INDEX);
      m_pEarthMaterialNoSun->MaximumPass(INVALID_INDEX);
   }
   else
   {
      m_pEarthMaterial->MaximumPass(2);
      m_pEarthMaterialNoSun->MaximumPass(1);
   }
}

void GBackgroundEarth::CloudsEnable()
{
   if(m_pCloudModel)
      m_pCloudModel->Visibility() = true;
}

void GBackgroundEarth::CloudsDisable()
{
   if(m_pCloudModel)
      m_pCloudModel->Visibility() = false;
}

void GBackgroundEarth::MoonEnable()
{
   if(m_pMoonModel)
      m_pMoonModel->Visibility() = true;
}

void GBackgroundEarth::MoonDisable()
{
   if(m_pMoonModel)
      m_pMoonModel->Visibility() = false;
}

void GBackgroundEarth::SunEnable()
{
   if(m_pSunModel)
      m_pSunModel->Visibility() = true;
   
   if(m_pFlareMasterModel)
      m_pFlareMasterModel->Visibility() = true;
   
   if(m_pWaterModel)
      m_pWaterModel->Visibility() = true;
   
   if(m_pAtmoHaloModel)
      m_pAtmoHaloModel->Visibility() = true;

   if(m_pFlareModel[0])
      m_pFlareModel[0]->Visibility() = true;

   m_pEarthModelMaster->Set_Material(m_pEarthMaterial);
   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      m_pEarthModel[i]->AddOverride(m_pDarkOverride[i] );
      m_pBumpOverride[i]->ApplyOnPass(2);
   }

   ( (GFX::GDefaultVertexShader*) (m_pCloudMaterial->VertexShader(0) ) )->Ambient() = GColorRGBReal(0, 0, 0, 1);
}

void GBackgroundEarth::SunDisable()
{
   if(m_pSunModel)
      m_pSunModel->Visibility() = false;
   
   if(m_pFlareMasterModel)
      m_pFlareMasterModel->Visibility() = false;
   
   if(m_pWaterModel)
      m_pWaterModel->Visibility() = false;
   
   if(m_pAtmoHaloModel)
      m_pAtmoHaloModel->Visibility() = false;

   if(m_pFlareModel[0])
      m_pFlareModel[0]->Visibility() = false;

   m_pEarthModelMaster->Set_Material(m_pEarthMaterialNoSun);
   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      m_pEarthModel[i]->RemoveOverride(m_pDarkOverride[i] );
      m_pBumpOverride[i]->ApplyOnPass(1);
   }

   ( (GFX::GDefaultVertexShader*) (m_pCloudMaterial->VertexShader(0) ) )->Ambient() = GColorRGBReal(1, 1, 1, 1);
}

bool GBackgroundEarth::CloudsEnabled()
{
   if( m_pCloudModel && m_pCloudModel->Visibility() )
      return true;
   else
      return false;
}

bool GBackgroundEarth::MoonEnabled()
{
   if( m_pMoonModel && m_pMoonModel->Visibility() )
      return true;
   else
      return false;
}

bool GBackgroundEarth::SunEnabled()
{
   if( m_pSunModel && m_pSunModel->Visibility() )
      return true;
   else
      return false;
}

void GBackgroundEarth::SetAtlasZoomLevel(SP2::EZoomLevel::Enum in_eZoomLevel)
{
   m_pAtlasEarth->SetZoomLevel(in_eZoomLevel);
}

void GBackgroundEarth::ChangeEarthDetail(EQualityLevel::Enum in_eNewQuality)
{
   if(in_eNewQuality == m_eCurrentEarthQuality)
   {
      return;
   }

   vector<REAL32> l_vLastVisibility;
   l_vLastVisibility.resize(m_vRegionVisibilityParams.size() );
   for(UINT32 i = 0;i < m_vRegionVisibilityParams.size();i ++)
   {
      l_vLastVisibility[i] = *(m_vRegionVisibilityParams[i] );
   }

   vector<GColorRGBReal> l_vLastColor;
   l_vLastColor.resize(m_vRegionColorParams.size() );
   for(UINT32 i = 0;i < m_vRegionColorParams.size();i ++)
   {
      l_vLastColor[i] = *(m_vRegionColorParams[i] );
   }

   m_eCurrentEarthQuality = in_eNewQuality;
   UINT32 l_iDetailLevel = 0;
   switch(in_eNewQuality)
   {
   case EQualityLevel::Low:
      l_iDetailLevel = 3;
      break;
   case EQualityLevel::Medium:
      l_iDetailLevel = 8;
      break;
   case EQualityLevel::High:
      l_iDetailLevel = 16;
      break;
   default:
      gassert(0, "Invalid BG earth quality level");
   }

   // Rebuild earth mesh
   UINT32 l_Index = 0;
   for(UINT32 y = 0;y < NB_EARTH_DIVS_Y;y ++)
   {
      REAL32 v = (REAL32)Math::PI * (0.5f - (REAL32) (y + 1) / (REAL32) NB_EARTH_DIVS_Y);
      REAL32 vnext = (REAL32)Math::PI * (0.5f - (REAL32) (y) / (REAL32) NB_EARTH_DIVS_Y);

      for(UINT32 x = 0;x < NB_EARTH_DIVS_X;x ++)
      {
         REAL32 u = 2.f * (REAL32)Math::PI * (REAL32) x / (REAL32) NB_EARTH_DIVS_X;
         REAL32 unext = 2.f * (REAL32)Math::PI * (REAL32) (x + 1) / (REAL32) NB_EARTH_DIVS_X;

         m_pEarthMesh[l_Index]->Shape().ClearAll();
         m_pEarthMesh[l_Index]->NbFacetGroups(0);
         g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(m_pEarthMesh[l_Index], 1, u, unext, v, vnext, l_iDetailLevel, l_iDetailLevel, HELPER_MESH_CREATE_ALL );

         l_Index ++;
      }

      if(m_pProgressMeter)
         m_pProgressMeter->PercentageUpdate( (UINT8) (80 + ( (y + 1) * 15 / NB_EARTH_DIVS_Y) ) );
   }

   // Rebuild water specular mesh
   m_pWaterMesh->Shape().ClearAll();
   m_pWaterMesh->NbFacetGroups(0);
   g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(m_pWaterMesh, 1.f, 0.f, (REAL32)Math::PI * 2.f, (REAL32)-Math::PI*0.5f, (REAL32)Math::PI*0.5f, l_iDetailLevel * 8, l_iDetailLevel * 4, HELPER_MESH_CREATE_ALL);

   m_pCloudMesh->Shape().ClearAll();
   m_pCloudMesh->NbFacetGroups(0);
   g_Joshua.Renderer()->Helper()->CreateSphere(m_pCloudMesh, 1.f + 1.f / 256.f, l_iDetailLevel * 16, l_iDetailLevel * 8, HELPER_MESH_CREATE_ALL);

   // Rebuild atlas texture generation
   m_pEarthModelMaster->Remove_Child(m_pAtlasEarth->Master());
   delete m_pAtlasEarth;
   m_pAtlasEarth = new GAtlasEarth(this, 
      l_iDetailLevel * NB_EARTH_DIVS_X, 
      l_iDetailLevel * NB_EARTH_DIVS_Y, 
      ConfigValue(c_sConfigAtlasFadeTime, c_fDefaultAtlasFadeTime) );
   m_pEarthModelMaster->Add_Child(m_pAtlasEarth->Master());

   // Rebuild region texture display
   m_pFillMesh->Shape().ClearAll();

   // Open the region display information file
   GFile l_RegionDataFile;
   if(g_Joshua.FileManager()->File(c_RegionDataFileName, l_RegionDataFile) )
   {
      l_RegionDataFile.Open();

      //Check the config file if we must load the regions or not
      GString l_sTemp;
      bool l_bLoadRegionTextures = true;
      if(g_ClientDAL.XMLValue(SP2::LOAD_REGION_TEXTURES,l_sTemp))
      {
         l_bLoadRegionTextures = (l_sTemp.ToINT32()? true : false);
      }

      // Read the number of regions to display
      UINT32 l_iRegionCount = 0;
      if(l_bLoadRegionTextures)
      {
         l_RegionDataFile.Read(&l_iRegionCount, sizeof(UINT32) );
      }

      // Read the resolution of the region display
      GVector2D<UINT32> l_RegionRes;
      l_RegionDataFile.Read(&l_RegionRes, sizeof(GVector2D<UINT32>) );

      // Calc the resolution of a single earth subdivision
      GVector2D<UINT32> l_SubDivs;
      l_SubDivs.x = NB_EARTH_DIVS_X * l_iDetailLevel;
      l_SubDivs.y = NB_EARTH_DIVS_Y * l_iDetailLevel;

      // Create a temporary mesh with a complete sphere
      GFX::GMesh* l_pSphereMesh = g_Joshua.Renderer()->CreateMesh();
      g_Joshua.Renderer()->Helper()->CreateSphere(l_pSphereMesh, 1, l_SubDivs.x, l_SubDivs.y, HELPER_MESH_CREATE_ALL);
      GTriangleListIndexed& l_SphereTris = l_pSphereMesh->FacetGroup(0).Facets().TriangleListIndexed();

      // Copy sphere shape to the regions mesh
      GShape& l_FillShape = m_pFillMesh->Shape();
      l_FillShape = l_pSphereMesh->Shape();

      // Create a facet for each region
      gassert(m_pFillMesh->NbFacetGroups() == l_iRegionCount, "Region count not the same when rebuilding region display mesh");

      // Initialize each region display
      for(UINT32 r = 0;r < l_iRegionCount;r ++)
      {
         // Read current region information
         GVector2D<UINT32> l_RegionPos;
         l_RegionDataFile.Read(&l_RegionPos, sizeof(GVector2D<UINT32>) );

         m_vRegionVisibilityParams[r] = &(m_pFillMesh->FacetGroup(r).Visibility() );
         *(m_vRegionVisibilityParams[r] ) = l_vLastVisibility[r];

         // Fetch the material override for region
         GFX::GOverridePSCascade* l_pPSOverride = (GFX::GOverridePSCascade*) m_pFillMesh->FacetGroup(r).Override(0);
         m_vRegionColorParams[r] = &(l_pPSOverride->ColorFactorOverride() );
         *(m_vRegionColorParams[r] ) = l_vLastColor[r];

         // Fetch the texture shader override for region
         GFX::GOverrideTSampler* l_pTSOverride = (GFX::GOverrideTSampler*) m_pFillMesh->FacetGroup(r).Override(1);

         if( (l_RegionPos.x != 0xFFFFFFFF) && (l_RegionPos.y != 0xFFFFFFFF) && (r != 111) )
         {
            // Fetch texture for region
            CGFX_Texture_Itf* l_pRegionTxtr = l_pTSOverride->OverrideTextureSampler()->Texture();
            GVector2D<UINT32> l_RegionSize(l_pRegionTxtr->Width(), l_pRegionTxtr->Height() );

            if(l_RegionPos.y + l_RegionSize.y > l_RegionRes.y)
            {
               l_RegionSize.y = l_RegionRes.y - l_RegionPos.y;
            }

            // Initialize region facet
            UINT32 l_YEnd = l_SubDivs.y - (l_RegionPos.y * l_SubDivs.y / l_RegionRes.y);
            UINT32 l_YStart = l_SubDivs.y - ( ( (l_RegionPos.y + l_RegionSize.y) * l_SubDivs.y + l_RegionRes.y - 1) / l_RegionRes.y);

            UINT32 l_XStart = l_RegionPos.x * l_SubDivs.x / l_RegionRes.x;
            l_XStart = (l_XStart + l_SubDivs.x / 2) % l_SubDivs.x;
            UINT32 l_XEnd = ( (l_RegionPos.x + l_RegionSize.x) * l_SubDivs.x + l_RegionRes.x - 1) / l_RegionRes.x;
            l_XEnd = (l_XEnd + l_SubDivs.x / 2) % l_SubDivs.x;

            if(l_YEnd > l_SubDivs.y)
            {
               l_YEnd = l_SubDivs.y;
            }

            if(l_XEnd < l_XStart)
            {
               l_XEnd += l_SubDivs.x;
            }

            GTriangleListIndexed& l_RegionTris = m_pFillMesh->FacetGroup(r).Facets().TriangleListIndexed();
            l_RegionTris.NbTriangles( (l_YEnd - l_YStart) * (l_XEnd - l_XStart) * 2);
            UINT32 l_iTriIndex = 0;
            for(UINT32 y = l_YStart;y < l_YEnd;y ++)
            {
               for(UINT32 x = l_XStart;(x < l_XEnd) && (x < l_SubDivs.x);x ++)
               {
                  l_RegionTris.Triangle(l_iTriIndex) = l_SphereTris.Triangle( (y * l_SubDivs.x + x) * 2);
                  l_RegionTris.Triangle(l_iTriIndex + 1) = l_SphereTris.Triangle( (y * l_SubDivs.x + x) * 2 + 1);

                  l_iTriIndex += 2;
               }

               for(UINT32 x = l_SubDivs.x;x < l_XEnd;x ++)
               {
                  GTriangleIndex l_NonOverlappedTri = l_SphereTris.Triangle( (y * l_SubDivs.x + x - l_SubDivs.x) * 2);
                  GTriangleIndex l_OverlappedTri = l_NonOverlappedTri;

                  for(UINT32 t = 0;t < 3;t ++)
                  {
                     l_OverlappedTri.m_pVertices[t] = l_FillShape.AddPosition(l_FillShape.Position(l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.AddNormal(l_FillShape.Normal(l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.AddUVCoord(0, l_FillShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.UVCoord(0, l_OverlappedTri.m_pVertices[t] ).x += 1;
                  }

                  l_RegionTris.Triangle(l_iTriIndex) = l_OverlappedTri;

                  l_NonOverlappedTri = l_SphereTris.Triangle( (y * l_SubDivs.x + x - l_SubDivs.x) * 2 + 1);

                  for(UINT32 t = 0;t < 3;t ++)
                  {
                     l_OverlappedTri.m_pVertices[t] = l_FillShape.AddPosition(l_FillShape.Position(l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.AddNormal(l_FillShape.Normal(l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.AddUVCoord(0, l_FillShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] ) );
                     l_FillShape.UVCoord(0, l_OverlappedTri.m_pVertices[t] ).x += 1;
                  }

                  l_RegionTris.Triangle(l_iTriIndex + 1) = l_OverlappedTri;

                  l_iTriIndex += 2;
               }
            }

            // Initialize region texture transform
            CGFX_Transform_2D_Itf* l_pRegionTrans = g_Joshua.Renderer()->Create_Transform_2D();

            GVector2D<REAL32> l_Start;
            l_Start.x = (REAL32) ( (l_RegionPos.x + l_RegionRes.x / 2) % l_RegionRes.x) / (REAL32) l_RegionRes.x;
            l_Start.y = (REAL32) (l_RegionPos.y + l_RegionSize.y) / (REAL32) l_RegionRes.y;

            GVector2D<REAL32> l_End;
            l_End.x = (REAL32) ( (l_RegionPos.x + l_RegionSize.x + l_RegionRes.x / 2) % l_RegionRes.x) / (REAL32) l_RegionRes.x;
            l_End.y = (REAL32) (l_RegionPos.y) / (REAL32) l_RegionRes.y;

            if(l_End.x < l_Start.x)
               l_End.x += 1;

            l_pRegionTrans->Position().X = l_Start.x / (l_Start.x - l_End.x);
            l_pRegionTrans->Position().Y = 1 - l_Start.y / (l_Start.y - l_End.y);
            l_pRegionTrans->Size().X = -1.f / (l_Start.x - l_End.x);
            l_pRegionTrans->Size().Y = 1.f / (l_Start.y - l_End.y);

            l_pPSOverride->AddUVTransformOverride(0, l_pRegionTrans);

            SAFE_RELEASE(l_pRegionTrans);
         }
      }

      SAFE_RELEASE(l_pSphereMesh);

      l_RegionDataFile.Close();
   }
}

EQualityLevel::Enum GBackgroundEarth::CurrentEarthDetail() const
{
   return m_eCurrentEarthQuality;
}

void GBackgroundEarth::DayNightCycleEnable()
{
   m_bDayNightCycle = true;
}

void GBackgroundEarth::DayNightCycleDisable()
{
   m_bDayNightCycle = false;
}

bool GBackgroundEarth::DayNightCycleEnabled()
{
   return m_bDayNightCycle;
}

void GBackgroundEarth::ChangeTxtrQuality(EQualityLevel::Enum in_eTxtrQuality)
{
   m_eTxtrQuality = in_eTxtrQuality;

   // Determine which texture set to use
   CGFX_Texture_Itf** l_pTxtrs = NULL;
   switch(in_eTxtrQuality)
   {
   case EQualityLevel::High:
      l_pTxtrs = m_pEarthTexture;
      break;
   case EQualityLevel::Low:
   case EQualityLevel::Medium:
      l_pTxtrs = m_pEarthTextureLow;
      break;
   default:
      gassert(0, "Invalid BG texture quality");
      break;
   }

   // Change all earth textures
   for(UINT32 i = 0;i < NB_EARTH_DIVS;i ++)
   {
      m_pEarthOverride[i]->OverrideTextureSampler()->Texture(l_pTxtrs[i] );
      m_pDarkOverride[i]->OverrideTextureSampler()->Texture(l_pTxtrs[i] );
   }
}

EQualityLevel::Enum GBackgroundEarth::TxtrQuality()
{
   return m_eTxtrQuality;
}

void GBackgroundEarth::SendOverlayToBack()
{
   Model()->Send_To_Back(m_pOverlay);
}

/*!
* Register the global client hotkeys
**/
void GBackgroundEarth::RegisterHotkeys()
{
#ifndef GOLEM_DEBUG
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_HOME,   false, false, false), this);
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo('B',       false, true,  false), this);
#endif // GOLEM_DEBUG
}

/*!
* Unregister global hot keys
**/
void GBackgroundEarth::UnregisterHotkeys()
{
#ifndef GOLEM_DEBUG
   g_Joshua.GUIManager()->Hotkeys.UnregisterHotkeysBoundTo(this);
#endif // GOLEM_DEBUG
}
