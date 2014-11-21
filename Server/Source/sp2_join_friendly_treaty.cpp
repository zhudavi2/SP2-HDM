#include "golem_pch.hpp"


using namespace EHEActions;


GJoinFriendlyTreaty* GJoinFriendlyTreaty::Clone() const
{
   return new GJoinFriendlyTreaty( *this ); 
}


void GJoinFriendlyTreaty::Execute()
{

/*!
 * Friendly treaties include:
 * Alliance, military trespassing right, cultural exchanges, noble cause, research partnership,
 * human development collaboration, economic partnership, commom market, economic aid, 
 * assume foreign debt, weapons trade, weapons trade rebates
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

		//If it's private and the country isn't an original member
		if(l_Treaty.Private() && !(l_Treaty.IsAnOriginalA(m_iSource) || l_Treaty.IsAnOriginalB(m_iSource)))
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
		case ETreatyType::Alliance:
			{
				if(l_fSideARel > (SP2::c_fRelationsFriend * l_fMilitaryWeight))
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
					return;
				}
			}
			break;
		case ETreatyType::MilitaryAccess:
			{
				
				if(l_iCountrySide == 1)
				{
					if(l_fSideARel > (SP2::c_fRelationsLove * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),2);
						return;
					}
				}
				else if (l_iCountrySide == 2)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),2);
					return;
				}
			}	
			break;
		case ETreatyType::CulturalExchanges:
		case ETreatyType::NobleCause:
			{
				if(l_fSideARel > (SP2::c_fRelationsGood * l_fMilitaryWeight))
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
					return;
				}
			}
			break;
		case ETreatyType::ResearchPartnership:
		case ETreatyType::HumanDevelopmentCollaboration:
		case ETreatyType::EconomicPartnership:
		case ETreatyType::CommonMarket:
			{
				//Countries with low economic ranks will accept most treaties
				//Countries with high ranks will join treaties with friends only
				REAL32 l_fMinimumRelations = SP2::c_fRelationsLike;
				if(l_Treaty.Type() == ETreatyType::CommonMarket)
					l_fMinimumRelations = SP2::c_fRelationsBad;
				REAL32 l_fRank = 0.f;
				for(set<UINT32>::const_iterator l_ItrA = l_SideA.begin();
					 l_ItrA != l_SideA.end(); l_ItrA++)
				{
					l_fRank += (REAL32)g_ServerDAL.CountryData(*l_ItrA)->EconomicRank();
				}
				if(l_SideA.size() > 0)
					l_fRank /= (REAL32)l_SideA.size();
				if((REAL32)g_ServerDAL.CountryData(m_iSource)->EconomicRank() >= l_fRank)
				{
					if(l_fSideARel > (l_fMinimumRelations * l_fMilitaryWeight))
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}
				else
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
					return;
				}
			}
			break;
		case ETreatyType::EconomicAid:
		case ETreatyType::AssumeForeignDebt:
			{
				if(l_iCountrySide == 2)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),2);
					return;
				}
				else
				{
					if( l_fSideBRel > (SP2::c_fRelationsLike * l_fMilitaryWeight) &&
						 (g_ServerDAL.CountryData(m_iSource)->BudgetExpenses() >
						  g_ServerDAL.CountryData(m_iSource)->BudgetRevenues()) )
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
						return;
					}
				}
			}
			break;

		case ETreatyType::WeaponTrade:
			{
				if(l_fSideARel > (SP2::c_fRelationsGood * l_fMilitaryWeight))
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),1);
					return;
				}
			}
			break;
		}
	}
	
	//If we are here, we couldn't join any treaties, we'll have to make a new one
	const UINT8* l_pDiplomaticStatus = g_ServerDAL.CountryDiplomaticStatuses(m_iSource);
	set<UINT32> l_SideATreaty;
	set<UINT32> l_SideBTreaty;
	set<UINT32> l_SidePressureTreaty;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	GString l_sCountryName = g_ServerDAL.GetString(l_pCountryData->NameID());

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	REAL32 l_fRelations = 0.f;
	set<UINT32> l_vCountriesLove;
	set<UINT32> l_vCountriesFriends;
	set<UINT32> l_vCountriesLike;
	set<UINT32> l_vCountriesGood;
	set<UINT32> l_vCountriesNeutral;
	set<UINT32> l_vCountriesHate;
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		if(i == m_iSource || !g_ServerDAL.CountryIsValid(i))
			continue;
		l_fRelations = g_ServerDAL.RelationBetweenCountries(i,m_iSource);
		if(l_fRelations > SP2::c_fRelationsLove)
			l_vCountriesLove.insert(i);
		if (l_fRelations > SP2::c_fRelationsFriend)		
			l_vCountriesFriends.insert(i);			
		if (l_fRelations > SP2::c_fRelationsLike)		
			l_vCountriesLike.insert(i);
		if (l_fRelations > SP2::c_fRelationsGood)
			l_vCountriesGood.insert(i);
		if (l_fRelations > SP2::c_fRelationsBad)
			l_vCountriesNeutral.insert(i);
		if (l_fRelations < SP2::c_fRelationsHate)
			l_vCountriesHate.insert(i);
	}
	
	UINT32 l_Conditions[ETreatyConditions::ItemCount];
	Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);		

	//First priority is to get allies when that AI has many ennemies
	REAL32 l_fTotalUnitsAllies = g_ServerDAL.TotalUnitsValueWithAllies(m_iSource);
	REAL32 l_fTotalUnitsEnnemies = 0.f;
	for(set<UINT32>::const_iterator l_Itr = l_vCountriesHate.begin();
		 l_Itr != l_vCountriesHate.end(); l_Itr++)
	{
		l_fTotalUnitsEnnemies += g_ServerDAL.TotalUnitsValue(*l_Itr);
	}
	

	if(l_fTotalUnitsEnnemies > l_fTotalUnitsAllies)
	{
		//Will create an alliance treaty
		for(set<UINT32>::const_iterator l_Itr = l_vCountriesLike.begin();
			 l_Itr != l_vCountriesLike.end(); l_Itr++)
		{
			if(l_pDiplomaticStatus[*l_Itr] != EDiplomaticStatus::Allied)
				l_SideATreaty.insert(*l_Itr);
		}
		l_SideATreaty.insert(m_iSource);
		if(l_SideATreaty.size() > 1)
		{
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::Alliance,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
				ETreatyType::Alliance, true, l_sTreatyName,l_Conditions);
			return;
		}	
		l_SideATreaty.clear();
	}

	if(GWorldBehavior::FindResourceFactor(m_iSource) < c_fCriticalResourceFactorBeforeAskingEP)
	{
		//Will create a Economic partnership treaty
		if(l_vCountriesGood.size() > 0)
		{
			l_SideATreaty.insert(m_iSource);
			l_SideATreaty.insert(l_vCountriesGood.begin(),l_vCountriesGood.end());
			bool l_bPrivate = true;
			if(l_pCountryData->EconomicRank() > 100)
				l_bPrivate = false;
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::EconomicPartnership,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::EconomicPartnership, l_bPrivate, l_sTreatyName,l_Conditions);
			return;
		}
		else if(l_vCountriesNeutral.size() > 0)
		{
			//Will create a common market treaty
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesNeutral.begin();
				l_Itr != l_vCountriesNeutral.end(); l_Itr++)
			{
				if(abs(g_ServerDAL.CountryData(*l_Itr)->EconomicRank() - l_pCountryData->EconomicRank()) <= 10)
					l_SideATreaty.insert(*l_Itr);				
			}			
			if(l_SideATreaty.size() > 1)
			{
				l_SideATreaty.insert(m_iSource);
				GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::CommonMarket,l_sCountryName);
				g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::CommonMarket, true, l_sTreatyName,l_Conditions);
				return;
			}	
		}
		l_SideATreaty.clear();
	}

	if(GWorldBehavior::FindHumanDevelopmentFactor(m_iSource) < 0.5f)
	{
		//Any conditions in that treaty?	
		FillConditions(l_Conditions,l_vCountriesGood.size());

		//Will create a human development collaboration treaty
		for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
			 l_Itr != l_vCountriesGood.end(); l_Itr++)
		{
			if(g_ServerDAL.CountryData(*l_Itr)->HumanDevelopment() >= l_pCountryData->HumanDevelopment() &&
				g_ServerDCL.CountryCanJoinThoseConditions(*l_Itr,l_Conditions,l_SideATreaty))
				l_SideATreaty.insert(*l_Itr);
		}
		if(g_ServerDCL.CountryCanJoinThoseConditions(m_iSource,l_Conditions,l_SideATreaty))
			l_SideATreaty.insert(m_iSource);
		if(l_SideATreaty.size() > 1)
		{
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::HumanDevelopmentCollaboration,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
				ETreatyType::HumanDevelopmentCollaboration, false, l_sTreatyName,l_Conditions);
			return;
		}	
		l_SideATreaty.clear();
		Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);
	}

	if(l_pCountryData->MonetarySupplyAmount() < 0.f)
	{
		//Will create a Assume foreign debt treaty
		l_SideBTreaty.insert(m_iSource);
		for(set<UINT32>::const_iterator l_Itr = l_vCountriesNeutral.begin();
			 l_Itr != l_vCountriesNeutral.end(); l_Itr++)
		{
			if(g_ServerDAL.CountryData(*l_Itr)->EconomicRank() >= l_pCountryData->EconomicRank())
				l_SideATreaty.insert(*l_Itr);			
		}
		if(l_SideATreaty.size() > 0)
		{
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::AssumeForeignDebt,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
				ETreatyType::AssumeForeignDebt, true, l_sTreatyName,l_Conditions);
			return;
		}	
		l_SideBTreaty.clear();
		l_SideATreaty.clear();
	}

	if(GWorldBehavior::FindGDPPopulationFactor(m_iSource) < 0.5f)
	{
		l_SideATreaty.insert(m_iSource);
		if (l_vCountriesFriends.size() > 0)
		{
			//Will create a Weapon trade treaty
			l_SideATreaty.insert(l_vCountriesFriends.begin(),l_vCountriesFriends.end());
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::WeaponTrade,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::WeaponTrade, true, l_sTreatyName,l_Conditions);
			return;
		}
		l_SideATreaty.clear();
	}	

	if(l_pCountryData->BudgetExpenses() >= l_pCountryData->BudgetRevenues())
	{		
		if (l_vCountriesFriends.size() > 0)
		{
			//Will create an Economic Aid treaty
			l_SideBTreaty.insert(m_iSource);
			l_SideATreaty.insert(l_vCountriesFriends.begin(),l_vCountriesFriends.end());
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::CommonMarket,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::CommonMarket, true, l_sTreatyName,l_Conditions);
			return;
		}
		l_SideBTreaty.clear();
		l_SideATreaty.clear();
	}	

	if(SP2::GWorldBehavior::FindMilitaryForceFactor(m_iSource) > 0.5f)
	{
		//Will create a Noble Cause treaty
		if(l_vCountriesGood.size() > 0)
		{
			l_SideATreaty.insert(m_iSource);
			l_SideATreaty.insert(l_vCountriesGood.begin(),l_vCountriesGood.end());			
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::NobleCause,l_sCountryName);
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
					ETreatyType::NobleCause, false, l_sTreatyName,l_Conditions);
			return;
		}
		l_SideATreaty.clear();
	}  

	//Will create a Cultural exchanges, as last treaty possible
	l_SideATreaty.insert(m_iSource);
	l_SideATreaty.insert(l_vCountriesNeutral.begin(),l_vCountriesNeutral.end());
	GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::CulturalExchanges,l_sCountryName);
	g_ServerDCL.CreateNewTreaty(m_iSource,l_SideATreaty,l_SideBTreaty,l_SidePressureTreaty,
		ETreatyType::CulturalExchanges, false, l_sTreatyName,l_Conditions);

}

void GJoinFriendlyTreaty::FillConditions(UINT32* in_pConditions, UINT32 l_iNbCountriesPossible)
{
	UINT32 l_iCapitalRegionId = 0;
	EGeoGroups::Enum l_GeoGroup = EGeoGroups::Undefined;
	EContinent::Enum l_Continent = EContinent::Undefined;
	const SCityInfo* l_pCapitalInfo = g_ServerDAL.CapitalCity(m_iSource);
	if(l_pCapitalInfo)
	{
		l_iCapitalRegionId = l_pCapitalInfo->m_iRegionId;
		l_GeoGroup =  g_ServerDAL.RegionGeoGroup(l_iCapitalRegionId);
		l_Continent =  g_ServerDAL.RegionContinent(l_iCapitalRegionId);
	}
			
	if(l_iNbCountriesPossible > 100)
	{
		in_pConditions[ETreatyConditions::RelationsCloseness] = EConditionStatusRelation::Friendly;
	}
	else if (l_iNbCountriesPossible > 80)
	{
		in_pConditions[ETreatyConditions::RelationsCloseness] = EConditionStatusRelation::Neutral;
		if(l_pCapitalInfo)
		{
			switch(l_GeoGroup)
			{
			case EGeoGroups::CentralAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::CentralAmerica; break;
			case EGeoGroups::EasternAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAfrica; break;
			case EGeoGroups::EasternAsia:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAsia; break;
			case EGeoGroups::EasternEurope:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternEurope; break;
			case EGeoGroups::FarEast:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::FarEast; break;
			case EGeoGroups::MiddleEast:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::MiddleEast; break;
			case EGeoGroups::NorthernAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAfrica; break;
			case EGeoGroups::NorthernAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAmerica; break;
			case EGeoGroups::PacificIslands:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::PacificIslands; break;
			case EGeoGroups::SouthernAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAfrica; break;
			case EGeoGroups::SouthernAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAmerica; break;
			case EGeoGroups::SouthernOceania:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernOceania; break;
			case EGeoGroups::WesternAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternAfrica; break;
			case EGeoGroups::WesternEurope:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternEurope; break;
			}
		}
	}
	else if (l_iNbCountriesPossible > 40)
	{
		in_pConditions[ETreatyConditions::RelationsCloseness] = EConditionStatusRelation::NotAtWar;
		if(l_pCapitalInfo)
		{
			switch(l_Continent)
			{
			case EContinent::Africa:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Africa; break;				
			case EContinent::America:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::America; break;
			case EContinent::Asia:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Asia; break;
			case EContinent::Europe:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Europe; break;
			case EContinent::Oceania:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Oceania; break;
			}
		}
	}		
}