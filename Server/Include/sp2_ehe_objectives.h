#ifndef _EHE_OBJECTIVES
#define _EHE_OBJECTIVES


#include "sp2_population_support.h"
#include "sp2_have_money.h"
#include "sp2_have_space.h"
#include "sp2_government_stability.h"
#include "sp2_economy_high_enough.h"
#include "sp2_economy_not_too_high.h"
#include "sp2_industrial_strength.h"
#include "sp2_military_strength.h"
#include "sp2_high_self_relations.h"
#include "sp2_have_military_allies.h"
#include "sp2_high_national_security_rating.h"
#include "sp2_high_human_development.h"
#include "sp2_high_telecom_level.h"
#include "sp2_high_infrastructure.h"
#include "sp2_have_high_economic_health.h"
#include "sp2_have_no_debt.h"
#include "sp2_strong_ground_forces.h"
#include "sp2_strong_infantry_forces.h"
#include "sp2_strong_air_forces.h"
#include "sp2_strong_naval_forces.h"
#include "sp2_strong_nuclear_forces.h"
#include "sp2_have_amds.h"
#include "sp2_have_good_research_level.h"
#include "sp2_have_military_access.h"
#include "sp2_enemies_low_stability.h"
#include "sp2_enemies_no_money.h"
#include "sp2_enemies_no_military.h"
#include "sp2_low_corruption_level.h"

namespace EHEObjectives
{


namespace Ids
{
   // local objectives
   const OBJECTIVE_ID   POPULATION_SUPPORT         =  1;
   const OBJECTIVE_ID   HAVE_SPACE                 =  2;
   const OBJECTIVE_ID   GOVERNMENT_STABILITY       =  3;
   const OBJECTIVE_ID   ECONOMY_TOO_LOW            =  4;
   const OBJECTIVE_ID   ECONOMY_TOO_HIGH           =  5;
   const OBJECTIVE_ID   HAVE_POSITIVE_BUDGET       =  6;
   const OBJECTIVE_ID   MEET_RESOURCE_DEMAND		   =  7;
	const OBJECTIVE_ID	HAVE_AMDS						=  8;
	const OBJECTIVE_ID   HAVE_MILITARY_ALLIES			=  9;
	const OBJECTIVE_ID	LOW_CORRUPTION_LEVEL			=  10;
	const OBJECTIVE_ID	ENEMIES_LOW_STABILITY		=  11;
	const OBJECTIVE_ID	ENEMIES_NO_MONEY				=  12;
	const OBJECTIVE_ID	ENEMIES_NO_MILITARY_FORCES	=  13;
   const OBJECTIVE_ID   HIGH_SELF_RELATIONS        =  14;
	const OBJECTIVE_ID	HIGH_NATIONAL_SECURITY		=	15;
	const OBJECTIVE_ID	HIGH_HUMAN_DEVELOPMENT		=	16;
	const OBJECTIVE_ID	HIGH_TELECOM_LEVEL			=	17;
	const OBJECTIVE_ID	HIGH_INFRASTRUCTURE			=	18;
	const OBJECTIVE_ID	HIGH_ECONOMIC_HEALTH			=	19;
	const OBJECTIVE_ID	HAVE_NO_DEBT					=  20;
	const OBJECTIVE_ID	STRONG_GROUND_FORCES			=  21;
	const OBJECTIVE_ID	STRONG_INFANTRY_FORCES		=  22;
	const OBJECTIVE_ID	STRONG_AIR_FORCES				=  23;
	const OBJECTIVE_ID	STRONG_NAVAL_FORCES			=  24;
	const OBJECTIVE_ID	STRONG_NUCLEAR_FORCES		=  25;
	const OBJECTIVE_ID	GOOD_RESEARCH_LEVEL			=  26;
	const OBJECTIVE_ID	MILITARY_ACCESS				=  27;	
};

namespace Names
{
   // local objectives
   const GString        POPULATION_SUPPORT         =  "POPULATION_SUPPORT";
   const GString        HAVE_SPACE                 =  "HAVE_SPACE";
   const GString        GOVERNMENT_STABILITY       =  "GOVERNMENT_STABILITY";
   const GString        ECONOMY_TOO_LOW            =  "ECONOMY_TOO_LOW";
   const GString        ECONOMY_TOO_HIGH			   =  "ECONOMY_TOO_HIGH";
   const GString        HAVE_POSITIVE_BUDGET       =  "HAVE_POSITIVE_BUDGET";
   const GString        MEET_RESOURCE_DEMAND       =  "MEET_RESOURCE_DEMAND";
	const GString			HAVE_AMDS						=  "HAVE_AMDS";
	const GString		   HAVE_MILITARY_ALLIES			=  "HAVE_MILITARY_ALLIES";
	const GString			LOW_CORRUPTION_LEVEL			=  "LOW_CORRUPTION_LEVEL";
   const GString        HIGH_SELF_RELATIONS        =  "HIGH_SELF_RELATIONS";   
	const GString			HIGH_NATIONAL_SECURITY		=	"HIGH_NATIONAL_SECURITY";
	const GString			HIGH_HUMAN_DEVELOPMENT		=	"HIGH_HUMAN_DEVELOPMENT";
	const GString			HIGH_TELECOM_LEVEL			=	"HIGH_TELECOM_LEVEL";
	const GString			HIGH_INFRASTRUCTURE			=	"HIGH_INFRASTRUCTURE";
	const GString			HIGH_ECONOMIC_HEALTH			=	"HIGH_ECONOMIC_HEALTH";
	const GString			HAVE_NO_DEBT					=  "HAVE_NO_DEBT";
	const GString			STRONG_GROUND_FORCES			=  "STRONG_GROUND_FORCES";
	const GString			STRONG_INFANTRY_FORCES		=  "STRONG_INFANTRY_FORCES";
	const GString			STRONG_AIR_FORCES				=  "STRONG_AIR_FORCES";
	const GString			STRONG_NAVAL_FORCES			=  "STRONG_NAVAL_FORCES";
	const GString			STRONG_NUCLEAR_FORCES		=  "STRONG_NUCLEAR_FORCES";
	const GString			GOOD_RESEARCH_LEVEL			=  "GOOD_RESEARCH_LEVEL";
	const GString			MILITARY_ACCESS				=  "MILITARY_ACCESS";
	const GString			ENEMIES_LOW_STABILITY		=  "ENEMIES_LOW_STABILITY";
	const GString			ENEMIES_NO_MONEY				=  "ENEMIES_NO_MONEY";
	const GString			ENEMIES_NO_MILITARY_FORCES	=  "ENEMIES_NO_MILITARY_FORCES";
};


};


#endif