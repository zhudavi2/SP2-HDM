#include "golem_pch.hpp"


using namespace EHEActions;


GTrainSpecops* GTrainSpecops::Clone() const
{
   return new GTrainSpecops( *this ); 
}


void GTrainSpecops::Execute()
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells = l_pCountryData->CovertActionCells();
	
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{		
		if(l_vCells[i].ExperienceLevelType() == ECovertActionCellTraining::Elite)
			continue;
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Active)
		{
			l_vCells[i].ChangeState(ECovertActionsCellState::Training);
			l_pCountryData->m_bCovertActionCellsDirty = true;
			return;
		}
		else if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant)
		{
			l_vCells[i].ChangeState(ECovertActionsCellState::Active);
			l_vCells[i].SubsequentStateAdd(ECovertActionsCellState::Training);
			l_pCountryData->m_bCovertActionCellsDirty = true;
			return;
		}
	}
}

bool GTrainSpecops::CheckHardConditions() const
{
	//Must have one cell elligible

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells = l_pCountryData->CovertActionCells();
	
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{		
		if(l_vCells[i].ExperienceLevelType() == ECovertActionCellTraining::Elite)
			continue;
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Active)
		{
			return true;
		}
		else if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant)
		{
			return true;
		}
	}

	return false;
}