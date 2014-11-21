#include "golem_pch.hpp"


using namespace EHEActions;


GBuildGroundForces* GBuildGroundForces::Clone() const
{
   return new GBuildGroundForces( *this ); 
}


void GBuildGroundForces::Execute()
{
	m_iUnitInProductionId = GAI::BuildOrBuyUnits(m_iSource,m_fStrength,EUnitCategory::Ground);
}

bool GBuildGroundForces::CheckHardConditions() const
{	
	return true;
}

bool GBuildGroundForces::ReadyToStepForward() const
{
	if(m_iUnitInProductionId == UINT_MAX)
		return true;
	REAL32 l_fProductionProgress = g_ServerDCL.UnitProductionProgress(m_iSource,m_iUnitInProductionId);
   if (l_fProductionProgress >= 0.f && l_fProductionProgress < 1.f)
      return false;
	return true;
}

GDisbandGroundForces* GDisbandGroundForces::Clone() const
{
   return new GDisbandGroundForces( *this ); 
}


void GDisbandGroundForces::Execute()
{
	GAI::DisbandUnits(m_iSource,EUnitCategory::Ground);
}

bool GDisbandGroundForces::CheckHardConditions() const
{	
	if(g_ServerDAL.TotalUnitsValue(m_iSource,EUnitCategory::Ground) > 0.f)
		return true;
	return false;
}

GTrainGroundForces* GTrainGroundForces::Clone() const
{
   return new GTrainGroundForces( *this ); 
}


void GTrainGroundForces::Execute()
{
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	list<UINT32> l_ListOfUnitsIdToTrain;	
	
	const set<UINT32>& l_Groups = g_Joshua.UnitManager().CountryUnitGroups(m_iSource);

	for(set<UINT32>::const_iterator it = l_Groups.begin();
		 it != l_Groups.end(); it++)
	{
		//Iterate and choose the first ground group
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		gassert(l_pUnitGroup,"GTrainGroundForces::Execute(): Unit Group should be valid.");

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
				//Check if a ground unit exist inside that unit group
				const vector<SDK::Combat::GUnit*>& l_vUnits = l_pUnitGroup->Units();
				for(vector<SDK::Combat::GUnit*>::const_iterator itr = l_vUnits.begin();
					itr != l_vUnits.end(); itr++)
				{
					SP2::GUnit* l_pUnit = (SP2::GUnit*)*itr;
					if( (EUnitType::Enum)((SP2::GUnitType*)((SP2::GUnitDesign*)l_pUnit->Design())->Type())->Category() == EUnitCategory::Ground)
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

bool GTrainGroundForces::CheckHardConditions() const
{	
	if(g_ServerDAL.TotalUnitsValue(m_iSource,EUnitCategory::Ground) > 0.f)
		return true;
	return false;
}