#include "golem_pch.hpp"


using namespace EHEActions;


GTradeInRegion* GTradeInRegion::Clone() const
{
   return new GTradeInRegion( *this ); 
}


void GTradeInRegion::Execute()
{

	if(m_iSource == m_iTarget)
		return;

	GTradeData l_Trade;
	l_Trade.Init();
	GTradeDataForOneSide& l_SideA = l_Trade.GetTradeDataFromSide(true);
	GTradeDataForOneSide& l_SideB = l_Trade.GetTradeDataFromSide(false);
	l_SideA.CountryId((INT16)m_iSource);
	l_SideB.CountryId((INT16)m_iTarget);

	UINT32 l_iRegionID = 0;

	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(m_iSource);

	for(set<UINT32>::const_iterator it = l_PoliticalRegions.begin();
		 it != l_PoliticalRegions.end(); it++)
	{
		if(l_vControls[*it].m_iMilitary == m_iTarget)
		{
			l_iRegionID = *it;
			break;
		}
	}

	if(l_iRegionID)
	{
		REAL64 l_fRegionValue = g_ServerDAL.RegionValue(l_iRegionID);
		l_SideB.AddRegion(l_iRegionID,l_fRegionValue);

		l_SideA.Money(l_fRegionValue * 10.f);
		
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
		REAL64 l_fExpectedDebt = l_pCountryData->MonetarySupplyAmount() - l_SideA.Money();
		if( (l_fExpectedDebt < 0.f) && abs(l_fExpectedDebt * 0.1f) < l_pCountryData->BudgetRevenues() )
			g_ServerDCL.TradeProposal(l_Trade);
	}
	

}

bool GTradeInRegion::CheckHardConditions() const
{	
	//Find a country that owns one of its political region
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(m_iSource);

	for(set<UINT32>::const_iterator it = l_PoliticalRegions.begin();
		 it != l_PoliticalRegions.end(); it++)
	{
		if(l_vControls[*it].m_iMilitary != m_iSource)
			return true;
	}

	//found none
	return false;
}

ENTITY_ID GTradeInRegion::FindTarget() const
{
	//Find a country that owns one of its political region
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_PoliticalRegions = g_ServerDAL.CountryPoliticalControl(m_iSource);

	for(set<UINT32>::const_iterator it = l_PoliticalRegions.begin();
		 it != l_PoliticalRegions.end(); it++)
	{
		if(l_vControls[*it].m_iMilitary != m_iSource)
			return l_vControls[*it].m_iMilitary;
	}

	//found none
	return m_iSource;
}

GTradeInResearch* GTradeInResearch::Clone() const
{
   return new GTradeInResearch( *this ); 
}


void GTradeInResearch::Execute()
{

}

bool GTradeInResearch::CheckHardConditions() const
{	
	return true;
}