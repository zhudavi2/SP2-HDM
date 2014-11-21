#ifndef _EHE_ACTIONS
#define _EHE_ACTIONS


#include "sp2_raise_taxes.h"
#include "sp2_lower_taxes.h"
#include "sp2_enroll_men.h"
#include "sp2_declare_war.h"
#include "sp2_train_specops.h"
#include "sp2_disband_nuke.h"
#include "sp2_interest_rate.h"
#include "sp2_launch_nuke.h"
#include "sp2_nuclearize_country.h"
#include "sp2_increase_production.h"
#include "sp2_raise_resource_taxes.h"
#include "sp2_lower_resource_taxes.h"
#include "sp2_declare_martial_law.h"
#include "sp2_change_democracy.h"
#include "sp2_change_communism.h"
#include "sp2_change_monarchy.h"
#include "sp2_change_theocracy.h"
#include "sp2_change_dictatorship.h"
#include "sp2_improve_status_rel_lang.h"
#include "sp2_decrease_status_rel_lang.h"
#include "sp2_legal_freedom_speech.h"
#include "sp2_illegal_freedom_speech.h"
#include "sp2_legal_freedom_demonstration.h"
#include "sp2_illegal_freedom_demonstration.h"
#include "sp2_illimited_nb_children.h"
#include "sp2_limited_nb_children.h"
#include "sp2_legal_abortion.h"
#include "sp2_illegal_abortion.h"
#include "sp2_legal_contraception.h"
#include "sp2_illegal_contraception.h"
#include "sp2_alliance.h"
#include "sp2_military_trespassing_rights.h"
#include "sp2_annexation.h"
#include "sp2_free_region.h"
#include "sp2_cultural_exchanges.h"
#include "sp2_noble_cause.h"
#include "sp2_research_partnership.h"
#include "sp2_human_development.h"
#include "sp2_economic_partnership.h"
#include "sp2_common_market.h"
#include "sp2_economic_aid.h"
#include "sp2_assume_foreign_debt.h"
#include "sp2_economic_embargo.h"
#include "sp2_weapon_trade.h"
#include "sp2_research_amds.h"
#include "sp2_air_forces.h"
#include "sp2_ground_forces.h"
#include "sp2_naval_forces.h"
#include "sp2_control_resource.h"
#include "sp2_covert_actions_ehe.h"
#include "sp2_emigration_immigration.h"
#include "sp2_trade_ins.h"

namespace EHEActions
{


	namespace Ids
	{
		const ACTION_ID   RAISE_TAXES                      = 1;
		const ACTION_ID   LOWER_TAXES                      = 2;
		const ACTION_ID	DECLARE_MARTIAL_LAW					= 3;
		const ACTION_ID	CHANGE_TO_DEMOCRACY					= 4;
		const ACTION_ID	CHANGE_TO_MONARCHY					= 5;
		const ACTION_ID	CHANGE_TO_COMMUNISM					= 6;
		const ACTION_ID	CHANGE_TO_THEOCRACY					= 7;
		const ACTION_ID	CHANGE_TO_DICTATORSHIP				= 8;
		const ACTION_ID	GIVE_BETTER_STATUS_REL_LANG		= 9;
		const ACTION_ID	GIVE_WORSE_STATUS_REL_LANG			= 10;
		const ACTION_ID   RAISE_INTEREST_LEVEL             = 11;
		const ACTION_ID   LOWER_INTEREST_LEVEL             = 12;
		const ACTION_ID   LEGAL_FREEDOM_SPEECH             = 13;
		const ACTION_ID   ILLEGAL_FREEDOM_SPEECH           = 14;
		const ACTION_ID   LEGAL_FREEDOM_DEMONSTRATION      = 15;
		const ACTION_ID   ILLEGAL_FREEDOM_DEMONSTRATION    = 16;
		const ACTION_ID   ILLIMITED_NB_CHILDREN            = 17;
		const ACTION_ID   LIMITED_NB_CHILDREN              = 18;				
		const ACTION_ID   INCREASE_PRODUCTION              = 19;
		const ACTION_ID   LEGAL_ABORTION                   = 20;
		const ACTION_ID   ILLEGAL_ABORTION	               = 21;
		const ACTION_ID   LEGAL_CONTRACEPTION              = 22;		
		const ACTION_ID   ILLEGAL_CONTRACEPTION            = 23;
		const ACTION_ID   ASSASSINATION	                  = 24;
		const ACTION_ID   TRAIN_SPECOPS                    = 25;
		const ACTION_ID   DISBAND_SPECOPS						= 26;
		const ACTION_ID   DISBAND_NUKE                     = 27;
		const ACTION_ID   LAUNCH_NUKE                      = 28;
		const ACTION_ID   NUCLEARIZE_COUNTRY               = 29;
		const ACTION_ID   CREATE_SPECOPS                   = 30;
		const ACTION_ID   WAR_DECLARATION                  = 31;
		const ACTION_ID   ALLIANCE			                  = 32;
		const ACTION_ID   MILITARY_TRESPASSING_RIGHT       = 33;
		const ACTION_ID   ANNEXATION			               = 34;
		const ACTION_ID   RAISE_RESOURCE_TAXES             = 35;
		const ACTION_ID   LOWER_RESOURCE_TAXES             = 36;
		const ACTION_ID   FREE_REGION						      = 37;
		const ACTION_ID   CULTURAL_EXCHANGES               = 38;
		const ACTION_ID   NOBLE_CAUSE				            = 39;
		const ACTION_ID   RESEARCH_PARTNERSHIP             = 40;
		const ACTION_ID   HUMAN_DEVELOPMENT_COLLABORATION  = 41;
		const ACTION_ID   ECONOMIC_PARTNERSHIP             = 42;
		const ACTION_ID   COMMON_MARKET			            = 43;
		const ACTION_ID   ECONOMIC_AID_GIVE	               = 44;
		const ACTION_ID   ECONOMIC_AID_RECEIVE             = 45;
		const ACTION_ID   ASSUME_FOREIGN_DEBT_GIVE         = 46;
		const ACTION_ID   ASSUME_FOREIGN_DEBT_RECEIVE      = 47;
		const ACTION_ID   ECONOMIC_EMBARGO				      = 48;
		const ACTION_ID   WEAPON_TRADE					      = 49;
		const ACTION_ID   WEAPON_TRADE_EMBARGO			      = 50;
		const ACTION_ID   RESEARCH_AMDS					      = 51;
		const ACTION_ID   TRAIN_INFANTRY					      = 52;
		const ACTION_ID   ENROLL_INFANTRY				      = 53;
		const ACTION_ID   DISBAND_INFANTRY				      = 54;
		const ACTION_ID   TRAIN_GROUND_UNITS			      = 55;
		const ACTION_ID   BUILD_GROUND_UNITS			      = 56;
		const ACTION_ID   DISBAND_GROUND_UNITS			      = 57;
		const ACTION_ID   TRAIN_AIR_UNITS						= 58;
		const ACTION_ID   BUILD_AIR_UNITS						= 59;
		const ACTION_ID   DISBAND_AIR_UNITS						= 60;
		const ACTION_ID   TRAIN_NAVAL_UNITS						= 61;
		const ACTION_ID   BUILD_NAVAL_UNITS						= 62;
		const ACTION_ID   DISBAND_NAVAL_UNITS			      = 63;
		const ACTION_ID   PRIVATE_CONTROL_RESOURCE	      = 64;
		const ACTION_ID   PUBLIC_CONTROL_RESOURCE		      = 65;
		const ACTION_ID   ESPIONAGE_CIVILIAN			      = 66;
		const ACTION_ID   ESPIONAGE_MILITARY			      = 67;
		const ACTION_ID   TERRORISM						      = 68;
		const ACTION_ID   COUP_ETAT						      = 69;
		const ACTION_ID   SABOTAGE_CIVILIAN				      = 70;
		const ACTION_ID   SABOTAGE_MILITARY				      = 71;
		const ACTION_ID   CLOSE_EMIGRATION				      = 72;
		const ACTION_ID   CLOSE_IMMIGRATION				      = 73;
		const ACTION_ID   OPEN_EMIGRATION				      = 74;
		const ACTION_ID   OPEN_IMMIGRATION				      = 75;
		const ACTION_ID   TRADE_IN_REGION				      = 76;
		const ACTION_ID   TRADE_IN_RESEARCH				      = 77;
		
	};

	namespace Names
	{
		const GString RAISE_TAXES									= "RAISE_TAXES";
		const GString LOWER_TAXES									= "LOWER_TAXES";
		const GString DECLARE_MARTIAL_LAW						= "DECLARE_MARTIAL_LAW";
		const GString CHANGE_TO_DEMOCRACY						= "CHANGE_TO_DEMOCRACY";
		const GString CHANGE_TO_MONARCHY							= "CHANGE_TO_MONARCHY";
		const GString CHANGE_TO_COMMUNISM						= "CHANGE_TO_COMMUNISM";
		const GString CHANGE_TO_THEOCRACY						= "CHANGE_TO_THEOCRACY";
		const GString CHANGE_TO_DICTATORSHIP					= "CHANGE_TO_DICTATORSHIP";
		const GString GIVE_BETTER_STATUS_REL_LANG				= "GIVE_BETTER_STATUS_REL_LANG";
		const GString GIVE_WORSE_STATUS_REL_LANG				= "GIVE_WORSE_STATUS_REL_LANG";
		const GString RAISE_INTEREST_LEVEL						= "RAISE_INTEREST_LEVEL";
		const GString LOWER_INTEREST_LEVEL						= "LOWER_INTEREST_LEVEL";
		const GString LEGAL_FREEDOM_SPEECH						= "LEGAL_FREEDOM_SPEECH";
		const GString ILLEGAL_FREEDOM_SPEECH					= "ILLEGAL_FREEDOM_SPEECH";
		const GString LEGAL_FREEDOM_DEMONSTRATION				= "LEGAL_FREEDOM_DEMONSTRATION";
		const GString ILLEGAL_FREEDOM_DEMONSTRATION			= "ILLEGAL_FREEDOM_DEMONSTRATION";
		const GString ILLIMITED_NB_CHILDREN						= "ILLIMITED_NB_CHILDREN";
		const GString LIMITED_NB_CHILDREN						= "LIMITED_NB_CHILDREN";
		const GString INCREASE_PRODUCTION						= "INCREASE_PRODUCTION";
		const GString LEGAL_ABORTION								= "LEGAL_ABORTION";
		const GString ILLEGAL_ABORTION							= "ILLEGAL_ABORTION";
		const GString LEGAL_CONTRACEPTION						= "LEGAL_CONTRACEPTION";
		const GString ILLEGAL_CONTRACEPTION						= "ILLEGAL_CONTRACEPTION";
		const GString ASSASSINATION								= "ASSASSINATION";
		const GString TRAIN_SPECOPS								= "TRAIN_SPECOPS";
		const GString DISBAND_SPECOPS								= "DISBAND_SPECOPS";
		const GString DISBAND_NUKE									= "DISBAND_NUKE";
		const GString LAUNCH_NUKE									= "LAUNCH_NUKE";
		const GString NUCLEARIZE_COUNTRY							= "NUCLEARIZE_COUNTRY";
		const GString CREATE_SPECOPS								= "CREATE_SPECOPS";
		const GString WAR_DECLARATION								= "WAR_DECLARATION";
		const GString ALLIANCE										= "ALLIANCE";
		const GString MILITARY_TRESPASSING_RIGHT				= "MILITARY_TRESPASSING_RIGHT";
		const GString ANNEXATION									= "ANNEXATION";
		const GString RAISE_RESOURCE_TAXES						= "RAISE_RESOURCE_TAXES";
		const GString LOWER_RESOURCE_TAXES						= "LOWER_RESOURCE_TAXES";
		const GString FREE_REGION									= "FREE_REGION";
		const GString CULTURAL_EXCHANGES							= "CULTURAL_EXCHANGES";
		const GString NOBLE_CAUSE									= "NOBLE_CAUSE";
		const GString RESEARCH_PARTNERSHIP						= "RESEARCH_PARTNERSHIP";
		const GString HUMAN_DEVELOPMENT_COLLABORATION		= "HUMAN_DEVELOPMENT_COLLABORATION";
		const GString ECONOMIC_PARTNERSHIP						= "ECONOMIC_PARTNERSHIP";
		const GString COMMON_MARKET								= "COMMON_MARKET";
		const GString ECONOMIC_AID_GIVE							= "ECONOMIC_AID_GIVE";
		const GString ECONOMIC_AID_RECEIVE						= "ECONOMIC_AID_RECEIVE";
		const GString ASSUME_FOREIGN_DEBT_GIVE					= "ASSUME_FOREIGN_DEBT_GIVE";
		const GString ASSUME_FOREIGN_DEBT_RECEIVE				= "ASSUME_FOREIGN_DEBT_RECEIVE";
		const GString ECONOMIC_EMBARGO							= "ECONOMIC_EMBARGO";
		const GString WEAPON_TRADE									= "WEAPON_TRADE";
		const GString WEAPON_TRADE_EMBARGO						= "WEAPON_TRADE_EMBARGO";
		const GString RESEARCH_AMDS								= "RESEARCH_AMDS";
		const GString TRAIN_INFANTRY								= "TRAIN_INFANTRY";
		const GString ENROLL_INFANTRY								= "ENROLL_INFANTRY";
		const GString DISBAND_INFANTRY							= "DISBAND_INFANTRY";
		const GString TRAIN_GROUND_UNITS							= "TRAIN_GROUND_UNITS";
		const GString BUILD_GROUND_UNITS							= "BUILD_GROUND_UNITS";
		const GString DISBAND_GROUND_UNITS						= "DISBAND_GROUND_UNITS";
		const GString TRAIN_AIR_UNITS								= "TRAIN_AIR_UNITS";
		const GString BUILD_AIR_UNITS								= "BUILD_AIR_UNITS";
		const GString DISBAND_AIR_UNITS							= "DISBAND_AIR_UNITS";
		const GString TRAIN_NAVAL_UNITS							= "TRAIN_NAVAL_UNITS";
		const GString BUILD_NAVAL_UNITS							= "BUILD_NAVAL_UNITS";
		const GString DISBAND_NAVAL_UNITS						= "DISBAND_NAVAL_UNITS";
		const GString PRIVATE_CONTROL_RESOURCE					= "PRIVATE_CONTROL_RESOURCE";
		const GString PUBLIC_CONTROL_RESOURCE					= "PUBLIC_CONTROL_RESOURCE";
		const GString ESPIONAGE_CIVILIAN							= "ESPIONAGE_CIVILIAN";
		const GString ESPIONAGE_MILITARY							= "ESPIONAGE_MILITARY";
		const GString TERRORISM										= "TERRORISM";
		const GString COUP_ETAT										= "COUP_ETAT";
		const GString SABOTAGE_CIVILIAN							= "SABOTAGE_CIVILIAN";
		const GString SABOTAGE_MILITARY							= "SABOTAGE_MILITARY";
		const GString CLOSE_EMIGRATION							= "CLOSE_EMIGRATION";
		const GString CLOSE_IMMIGRATION							= "CLOSE_IMMIGRATION";
		const GString OPEN_EMIGRATION								= "OPEN_EMIGRATION";
		const GString OPEN_IMMIGRATION							= "OPEN_IMMIGRATION";
		const GString TRADE_IN_REGION								= "TRADE_IN_REGION";
		const GString TRADE_IN_RESEARCH							= "TRADE_IN_RESEARCH";
		
	};


};

#endif