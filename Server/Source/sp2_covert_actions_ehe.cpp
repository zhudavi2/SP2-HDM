#include "golem_pch.hpp"


using namespace EHEActions;

ENTITY_ID GAssassination::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GAssassination* GAssassination::Clone() const
{
   return new GAssassination( *this ); 
}


void GAssassination::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Assassination);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
}

bool GAssassination::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Assassination,
		ECovertActionsTargetSector::Undefined,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}


ENTITY_ID GEspionageCivilian::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GEspionageCivilian* GEspionageCivilian::Clone() const
{
   return new GEspionageCivilian( *this ); 
}


void GEspionageCivilian::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Espionage);
	l_pCellToExecute->TargetSector(ECovertActionsTargetSector::Civilian);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
	l_pCellToExecute->ResourceType(EResources::ItemCount);
}

bool GEspionageCivilian::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Espionage,
		ECovertActionsTargetSector::Civilian,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

ENTITY_ID GEspionageMilitary::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GEspionageMilitary* GEspionageMilitary::Clone() const
{
   return new GEspionageMilitary( *this ); 
}


void GEspionageMilitary::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Espionage);
	l_pCellToExecute->TargetSector(ECovertActionsTargetSector::Military);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
	l_pCellToExecute->UnitCategory(EUnitCategory::ItemCount);
}

bool GEspionageMilitary::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Espionage,
		ECovertActionsTargetSector::Military,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

ENTITY_ID GSabotageCivilian::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GSabotageCivilian* GSabotageCivilian::Clone() const
{
   return new GSabotageCivilian( *this ); 
}


void GSabotageCivilian::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Sabotage);
	l_pCellToExecute->TargetSector(ECovertActionsTargetSector::Civilian);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
	l_pCellToExecute->ResourceType(EResources::ItemCount);
}

bool GSabotageCivilian::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Sabotage,
		ECovertActionsTargetSector::Civilian,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

ENTITY_ID GSabotageMilitary::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GSabotageMilitary* GSabotageMilitary::Clone() const
{
   return new GSabotageMilitary( *this ); 
}


void GSabotageMilitary::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Sabotage);
	l_pCellToExecute->TargetSector(ECovertActionsTargetSector::Military);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
	l_pCellToExecute->UnitCategory(EUnitCategory::ItemCount);
}

bool GSabotageMilitary::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Sabotage,
		ECovertActionsTargetSector::Military,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

ENTITY_ID GTerrorism::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GTerrorism* GTerrorism::Clone() const
{
   return new GTerrorism( *this ); 
}


void GTerrorism::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::Terrorism);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
}

bool GTerrorism::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Medium,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::Terrorism,
		ECovertActionsTargetSector::Undefined,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

ENTITY_ID GCoupEtat::FindTarget() const
{
	return GAI::FindBestTargetForNextMission(m_iSource);   
}

GCoupEtat* GCoupEtat::Clone() const
{
   return new GCoupEtat( *this ); 
}


void GCoupEtat::Execute()
{
	if(!m_iTarget)
		return;
	GCovertActionCell* l_pCellToExecute = GAI::FindBestCellForJob(m_iSource,m_iTarget);
	if(!l_pCellToExecute)
		return;
	if(l_pCellToExecute->ActualState() == ECovertActionsCellState::Dormant)
	{
		l_pCellToExecute->ChangeState(ECovertActionsCellState::GettingReady);
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);									
	}
	else
	{
		if(l_pCellToExecute->AssignedCountry() != m_iTarget)
		{
			l_pCellToExecute->NextAssignedCountry(m_iTarget);
			l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::InTransit);			
		}
		l_pCellToExecute->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
	}	
	l_pCellToExecute->MissionType(ECovertActionsMissionType::CoupEtat);
	l_pCellToExecute->MissionComplexity(ECovertActionsMissionComplexity::Medium);
}

bool GCoupEtat::CheckHardConditions() const
{	
	if(!m_iTarget)
		return false;
	REAL32 l_fSuccessRate = g_ServerDCL.FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::High,
		ECovertActionCellTraining::Recruit,ECovertActionsMissionType::CoupEtat,
		ECovertActionsTargetSector::Undefined,false,m_iTarget,-1);

	if(l_fSuccessRate < c_fWillingnessToTry)
		return false;

	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant || 
			l_vCells[i].ActualState() == ECovertActionsCellState::Active)
			return true;
	}

	return false;
}

GCreateSpecOpsCells* GCreateSpecOpsCells::Clone() const
{
   return new GCreateSpecOpsCells( *this ); 
}


void GCreateSpecOpsCells::Execute()
{
	GCountryData* l_pData = g_ServerDAL.CountryData(m_iSource);
   
   SP2::GCovertActionCell l_Cell;

   l_Cell.OwnerID(m_iSource);
	l_Cell.Name( g_ServerDAL.GetString(101479) );
   l_Cell.AssignedCountry( m_iSource );
   l_Cell.ExperienceLevel( 0.f );
   l_Cell.Initialize();	

   l_pData->AddCovertActionCell(l_Cell);
	l_pData->m_bCovertActionCellsDirty = true;
}

bool GCreateSpecOpsCells::CheckHardConditions() const
{	
	if(g_ServerDCL.FindNationalSecurity(m_iSource) >= 0.95f)
		return false;
	return true;
}

GDisbandSpecOpsCells* GDisbandSpecOpsCells::Clone() const
{
   return new GDisbandSpecOpsCells( *this ); 
}


void GDisbandSpecOpsCells::Execute()
{
	//Remove dormant cells first, then active one's
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	GCovertActionCell* l_pCellToRemove = NULL;
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		ECovertActionsCellState::Enum l_eState = (ECovertActionsCellState::Enum)l_vCells[i].ActualState();
		if(l_eState == ECovertActionsCellState::Dormant)
			l_pCellToRemove = &l_vCells[i];
		else if(l_eState == ECovertActionsCellState::Active && !l_pCellToRemove)
			l_pCellToRemove = &l_vCells[i];
		else if(!l_pCellToRemove)
			l_pCellToRemove = &l_vCells[i];
	}

	if(l_pCellToRemove)
	{
		l_pCountryData->RemoveCovertActionCell(*l_pCellToRemove);
		l_pCountryData->NationalSecurity( g_ServerDCL.FindNationalSecurity(m_iSource) );
		l_pCountryData->m_bCovertActionCellsDirty = true;
	}
}

bool GDisbandSpecOpsCells::CheckHardConditions() const
{	
	//Must have at least 1 sleeping or active cell
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	if(l_vCells.size() > 0)
		return true;

	return false;
}