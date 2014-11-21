#include "golem_pch.hpp"


using namespace EHEActions;


GEnrollMen* GEnrollMen::Clone() const
{
   return new GEnrollMen( *this ); 
}


void GEnrollMen::Execute()
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	INT64 l_iManPowerAvailable = l_pCountryData->MiliManpowerAvailable();

	//Enroll 2% * strength
	INT64 l_iNbToEnroll = (INT64)((REAL32)l_iManPowerAvailable * 0.002f * m_fStrength);
	if(l_iNbToEnroll)
		m_iUnitInProductionId = g_ServerDCL.BuildUnits(m_iSource,m_iSource,1,(UINT32)l_iNbToEnroll,0,EUnitProductionPriority::Normal);
	else
		m_iUnitInProductionId = UINT_MAX;
}

bool GEnrollMen::ReadyToStepForward() const
{
	if(m_iUnitInProductionId == UINT_MAX)
		return true;
	REAL32 l_fProductionProgress = g_ServerDCL.UnitProductionProgress(m_iSource,m_iUnitInProductionId);
   if (l_fProductionProgress >= 0.f && l_fProductionProgress < 1.f)
      return false;
	return true;
}

bool GEnrollMen::CheckHardConditions() const
{	
	INT32 l_iRankInfantry = 0;
	INT32 l_iRankGround = 0;
	INT32 l_iRankAir = 0;
	INT32 l_iRankNaval = 0;
	
	g_ServerDAL.FindUnitTypeRanks(m_iSource,l_iRankInfantry,l_iRankGround,l_iRankAir,l_iRankNaval);

	if(g_ServerDAL.CountryData(m_iSource)->MiliManpowerAvailable() > 0 &&
		l_iRankInfantry > 25 )
		return true;
	return false;
}

GTrainInfantry* GTrainInfantry::Clone() const
{
   return new GTrainInfantry( *this ); 
}


void GTrainInfantry::Execute()
{
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	list<UINT32> l_ListOfUnitsIdToTrain;	
	
	const set<UINT32>& l_Groups = g_Joshua.UnitManager().CountryUnitGroups(m_iSource);

	for(set<UINT32>::const_iterator it = l_Groups.begin();
		 it != l_Groups.end(); it++)
	{
		//Iterate and choose the first group of infantry
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		gassert(l_pUnitGroup,"GTrainInfantry::Execute(): Unit Group should be valid.");

		if( l_pUnitGroup->Status() == EMilitaryStatus::Parked ||
			 l_pUnitGroup->Status() == EMilitaryStatus::Fortified ||
			 l_pUnitGroup->Status() == EMilitaryStatus::Ready )
		{
			UINT32 l_iRegionID = ((SP2::GUnitGroupEx*)l_pUnitGroup)->m_iRegionID;
			if( l_iRegionID != 0 && 
				l_vControls[l_iRegionID].m_iMilitary == m_iSource && 
				l_vControls[l_iRegionID].m_iPolitical == m_iSource && 
				!l_pUnitGroup->IsNaval() )
			{
				//Check if an infantry exist inside that unit group
				const vector<SDK::Combat::GUnit*>& l_vUnits = l_pUnitGroup->Units();
				for(vector<SDK::Combat::GUnit*>::const_iterator itr = l_vUnits.begin();
					itr != l_vUnits.end(); itr++)
				{
					SP2::GUnit* l_pUnit = (SP2::GUnit*)*itr;
					if( (EUnitType::Enum)((SP2::GUnitDesign*)l_pUnit->Design())->Type()->Id() == EUnitType::Infantry )
					{
						if(l_pUnit->Training() != ETrainingLevel::Elite)
						{
							l_ListOfUnitsIdToTrain.push_back(l_pUnit->Id());
							g_ServerDCL.UnitMover().TrainUnits(l_ListOfUnitsIdToTrain);
							return;
						}
					}
				}
			}
		}
	}	
}

bool GTrainInfantry::CheckHardConditions() const
{	
	if(g_ServerDAL.TotalUnitsValue(m_iSource,EUnitCategory::Infantry) > 0.f)
		return true;
	return false;
}

GDisbandInfantry* GDisbandInfantry::Clone() const
{
   return new GDisbandInfantry( *this ); 
}


void GDisbandInfantry::Execute()
{
	GAI::DisbandUnits(m_iSource,EUnitCategory::Infantry);
}

bool GDisbandInfantry::CheckHardConditions() const
{	
	if(g_ServerDAL.TotalUnitsValue(m_iSource,EUnitCategory::Infantry) > 0.f)
		return true;
	return false;
}