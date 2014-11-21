#include "golem_pch.hpp"


using namespace EHEActions;


GJoinHostileTreaty* GJoinHostileTreaty::Clone() const
{
   return new GJoinHostileTreaty( *this ); 
}


void GJoinHostileTreaty::Execute()
{
/*!
 * Hostile treaties include:
 * Request military removal, annexation, economic embargo, weapons trade embargo
 **/

	//A country will join an existing treaty first
	REAL32 l_fSideARel = 0.f;
	REAL32 l_fSideBRel = 0.f;
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;
		
		if(l_Treaty.IsSuspended(m_iSource))
			continue;

		if(!( l_Treaty.Private() && (l_Treaty.IsAnOriginalA(m_iSource) || l_Treaty.IsAnOriginalB(m_iSource)) ))
			continue;

		if(l_Treaty.MembersSideA(true).find(m_iSource) != l_Treaty.MembersSideA(true).end() ||
			l_Treaty.MembersSideB(true).find(m_iSource) != l_Treaty.MembersSideB(true).end())
		{
			//Country is already inside that treaty
			continue;
		}		

		if(!g_ServerDCL.CountryCanJoinTreaty(m_iSource,l_Treaty.ID()))
			continue;
		
		l_fSideBRel = 0.f;
		l_fSideARel = 0.f;

		REAL32 l_fPressureStrength = 0.f;
		REAL32 l_fMembersStrength = 0.f;
		REAL32 l_fMilitaryWeight = 1.f;

		UINT32 l_iCountrySide = l_Treaty.CountrySide(m_iSource);

		const set<UINT32>& l_SideA = l_Treaty.MembersSideA(false);
		const set<UINT32>& l_SideB = l_Treaty.MembersSideB(false);
		const set<UINT32>& l_Pressure = l_Treaty.MembersPressure();

		for(set<UINT32>::const_iterator l_Itr = l_Pressure.begin();
			 l_Itr != l_Pressure.end(); l_Itr++)
		{
			l_fPressureStrength += g_ServerDAL.CountryData(*l_Itr)->MilitaryStrength();
		}

		for(set<UINT32>::const_iterator l_Itr = l_SideA.begin();
			l_Itr != l_SideA.end();
			l_Itr++)
		{
			l_fSideARel += g_ServerDAL.RelationBetweenCountries(m_iSource,*l_Itr);
			l_fMembersStrength += g_ServerDAL.CountryData(*l_Itr)->MilitaryStrength();
		}
		if(l_SideA.size() > 0)
			l_fSideARel /= (REAL32)l_SideA.size();

		bool l_bSideBCanLeave = l_Treaty.SideBCanLeave();		
		for(set<UINT32>::const_iterator l_Itr = l_SideB.begin();
			l_Itr != l_SideB.end();
			l_Itr++)
		{
			l_fSideBRel += g_ServerDAL.RelationBetweenCountries(m_iSource,*l_Itr);
			if(l_bSideBCanLeave)
				l_fMembersStrength += g_ServerDAL.CountryData(*l_Itr)->MilitaryStrength();
		}
		if(l_SideA.size() > 0)
			l_fSideBRel /= (REAL32)l_SideB.size();

		if(l_fMembersStrength > 0.f)
		{
			REAL32 l_fRatio = min(1.f,l_fPressureStrength / l_fMembersStrength);
			l_fMilitaryWeight -= l_fRatio;
		}

		//If we are here, that country is able to join that treaty
		switch(l_Treaty.Type())
		{
		case ETreatyType::RequestMilitaryPresenceRemoval:
			{
				if(l_iCountrySide == 1)
				{
					if(l_fSideBRel < (SP2::c_fRelationsFriend * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}
				else if(l_iCountrySide == 2)
				{
					if(l_fSideARel > (SP2::c_fRelationsEnnemy * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),2);
						return;
					}
				}
			}
			break;
		case ETreatyType::Annexation:
			{
				if(l_iCountrySide == 1)
				{
					if(GAI::ShouldAnnexCountry(m_iSource,*(l_SideB.begin())))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}
			}
			break;
		case ETreatyType::EconomicEmbargo:
			{
				if(l_iCountrySide == 1)
				{	
					if(l_fSideBRel < (SP2::c_fRelationsHate * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}	
			}
			break;
		case ETreatyType::WeaponTradeEmbargo:
			{
				if(l_iCountrySide == 1)
				{	
					if(l_fSideBRel < (SP2::c_fRelationsEnnemy * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}
			}
			break;
		}
	}

	//If we are here, we couldn't join any treaties, we'll have to make a new one
	UINT32 l_iConditions[ETreatyConditions::ItemCount];
	Memory::Fill<UINT32>(l_iConditions,0,ETreatyConditions::ItemCount);
	set<UINT32> l_SideATreaty;
	set<UINT32> l_SideBTreaty;
	set<UINT32> l_SidePressureTreaty;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	GString l_sCountryName = g_ServerDAL.GetString(l_pCountryData->NameID());
	const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(m_iSource);	

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	REAL32 l_fRelations = 0.f;
	set<UINT32> l_vCountriesLove;
	set<UINT32> l_vCountriesFriends;
	set<UINT32> l_vCountriesEnnemy;
	set<UINT32> l_vCountriesHate;

	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		if(i == m_iSource || !g_ServerDAL.CountryIsValid(i))
			continue;
		l_fRelations = g_ServerDAL.RelationBetweenCountries(i,m_iSource);
		if(l_fRelations > SP2::c_fRelationsLove)
			l_vCountriesLove.insert(i);
		else if (l_fRelations > SP2::c_fRelationsFriend)
		{
			l_vCountriesLove.insert(i);
			l_vCountriesFriends.insert(i);
		}
		else if (l_fRelations < SP2::c_fRelationsHate)
		{
			l_vCountriesHate.insert(i);
		}
		else if (l_fRelations < SP2::c_fRelationsEnnemy)
		{
			l_vCountriesHate.insert(i);
			l_vCountriesEnnemy.insert(i);
		}
	}
	
	//Ask for military removal, if needed
	for(set<UINT32>::const_iterator l_Itr = l_vCountriesEnnemy.begin(); 
		 l_Itr != l_vCountriesEnnemy.end(); l_Itr++)
	{		
		UINT32 l_iEnnemy = *l_Itr;
		//Cycle through the ennmey units, to find if any is inside our country
		const set<UINT32>& l_vUnitGroups = g_Joshua.UnitManager().CountryUnitGroups(l_iEnnemy);
		for(set<UINT32>::const_iterator l_ItrGrp = l_vUnitGroups.begin();
			 l_ItrGrp != l_vUnitGroups.end();
			 l_ItrGrp++)
		{
			SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(*l_ItrGrp);
			gassert(l_pGroup,"GJoinHostileTreaty::Execute(): Unit Group does not exist");
			UINT32 l_iCurrentRegion = ((SP2::GUnitGroupEx*)l_pGroup)->m_iRegionID;
			if( l_vRegionsPolitical.find( l_iCurrentRegion ) != l_vRegionsPolitical.end() )
			{
				//We found an ennemy unit inside the AI territory, let's ask for military removal
				l_SideATreaty.insert(m_iSource);
				l_SideBTreaty.insert(l_iEnnemy);
				GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::RequestMilitaryPresenceRemoval,l_sCountryName);
				g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::RequestMilitaryPresenceRemoval, true, l_sTreatyName, l_iConditions);
				return;
			}
		}
	}
	l_SideATreaty.clear();
	l_SideBTreaty.clear();

	//Sign a weapon trade embargo
	if(l_vCountriesHate.size() > 0)
	{
		for(set<UINT32>::const_iterator l_Itr = l_vCountriesHate.begin();
			 l_Itr != l_vCountriesHate.end(); l_Itr++)
		{
			if(!g_ServerDAL.WeaponTradeEmbargo(m_iSource,*l_Itr))
			{
				l_SideATreaty.insert(m_iSource);
				l_SideATreaty.insert(l_vCountriesLove.begin(),l_vCountriesLove.end());
				l_SideBTreaty.insert(*l_Itr);
				GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::WeaponTradeEmbargo,l_sCountryName);
				g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::WeaponTradeEmbargo, false, l_sTreatyName,l_iConditions);
				return;
			}
		}
	}	
	l_SideATreaty.clear();
	l_SideBTreaty.clear();
	//Sign an economic trade embargo
	if(l_vCountriesEnnemy.size() > 0)
	{
		for(set<UINT32>::const_iterator l_Itr = l_vCountriesEnnemy.begin();
			 l_Itr != l_vCountriesEnnemy.end(); l_Itr++)
		{
			if(!g_ServerDAL.TradeEmbargo(m_iSource,*l_Itr))
			{
				l_SideATreaty.insert(m_iSource);
				l_SideATreaty.insert(l_vCountriesFriends.begin(),l_vCountriesFriends.end());
				l_SideBTreaty.insert(*l_Itr);
				GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::EconomicEmbargo,l_sCountryName);
				g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::EconomicEmbargo, false, l_sTreatyName,l_iConditions);
				return;
			}
		}
	}	
	l_SideATreaty.clear();
	l_SideBTreaty.clear();
	//Sign annexation treaty, if possible
	const set<UINT32>& l_vRegionsMilitary = g_ServerDAL.CountryPoliticalControl(m_iSource);
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	for(set<UINT32>::const_iterator l_Itr = l_vRegionsMilitary.begin();
		 l_Itr != l_vRegionsMilitary.end(); l_Itr++)
	{
		if(l_vControls[*l_Itr].m_iPolitical == m_iSource)
			continue;
		UINT32 l_iEnnemy = l_vControls[*l_Itr].m_iPolitical;
		if(g_ServerDAL.DiplomaticStatus(l_iEnnemy,m_iSource) == EDiplomaticStatus::Hostile)
			continue;
		//Countries are not at war, so an annexation is possible
		if(GAI::ShouldAnnexCountry(m_iSource,l_iEnnemy))
		{
			l_SideATreaty.insert(m_iSource);
			l_SideBTreaty.insert(l_iEnnemy);
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::Annexation,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
				ETreatyType::Annexation, false, l_sTreatyName,l_iConditions);
			return;
		}
	}

}