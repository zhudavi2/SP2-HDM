#include "golem_pch.hpp"


using namespace EHEActions;


GApplyPressure* GApplyPressure::Clone() const
{
   return new GApplyPressure( *this ); 
}


void GApplyPressure::Execute()
{
	//It will first try to put pressure on treaties containing its ennemy
	//Then it will put pressure on treaties 	
	REAL32 l_fSideARel = 0.f;
	REAL32 l_fSideBRel = 0.f;
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();		
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;
		if(l_Treaty.Active() || l_Treaty.CountrySide(m_iSource) > 0)
			continue;

		l_fSideBRel = 0.f;
		l_fSideARel = 0.f;

		const set<UINT32>& l_SideA = l_Treaty.MembersSideA(false);
		const set<UINT32>& l_SideB = l_Treaty.MembersSideB(false);

		for(set<UINT32>::const_iterator l_Itr = l_SideA.begin();
			 l_Itr != l_SideA.end();
			 l_Itr++)
		{
			l_fSideARel += g_ServerDAL.RelationBetweenCountries(m_iSource,*l_Itr);
		}
		if(l_SideA.size() > 0)
			l_fSideARel /= (REAL32)l_SideA.size();

		for(set<UINT32>::const_iterator l_Itr = l_SideB.begin();
			 l_Itr != l_SideB.end();
			 l_Itr++)
		{
			l_fSideBRel += g_ServerDAL.RelationBetweenCountries(m_iSource,*l_Itr);
		}
		if(l_SideA.size() > 0)
			l_fSideBRel /= (REAL32)l_SideB.size();

		switch(l_Treaty.Type())
		{
		case ETreatyType::War:
			{
				if(l_fSideBRel < SP2::c_fRelationsDislike)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::CeaseFire:
			{
				if(l_fSideARel > SP2::c_fRelationsLike && l_fSideBRel > SP2::c_fRelationsLike)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::PeaceLiberation:
			{
				if(l_fSideARel > SP2::c_fRelationsGood && l_fSideBRel > SP2::c_fRelationsGood)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::PeaceAnnexation:
			{
				//Must check first which sides will conquer the most regions
				UINT32 l_iNbRegionsStolenByA = 0;
				UINT32 l_iNbRegionsStolenByB = 0;
				const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
				UINT32 l_iSideACountry = *(l_Treaty.MembersSideA(false).begin());
				UINT32 l_iSideBCountry = *(l_Treaty.MembersSideB(false).begin());				
				const set<UINT32>& l_vRegionsSideA = g_ServerDAL.CountryMilitaryControl(l_iSideACountry);
				const set<UINT32>& l_vRegionsSideB = g_ServerDAL.CountryMilitaryControl(l_iSideBCountry);

				for(set<UINT32>::const_iterator l_Itr = l_vRegionsSideA.begin();
					 l_Itr != l_vRegionsSideA.end(); l_Itr++)
				{
					if(l_vControls[*l_Itr].m_iPolitical == l_iSideBCountry)
						l_iNbRegionsStolenByA++;
				}
				for(set<UINT32>::const_iterator l_Itr = l_vRegionsSideB.begin();
					 l_Itr != l_vRegionsSideB.end(); l_Itr++)
				{
					if(l_vControls[*l_Itr].m_iPolitical == l_iSideACountry)
						l_iNbRegionsStolenByB++;
				}

				if(l_iNbRegionsStolenByA > l_iNbRegionsStolenByB)
				{
					if(l_fSideARel > SP2::c_fRelationsGood && l_fSideBRel < SP2::c_fRelationsDislike)
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
						return;
					}
				}
				else if(l_iNbRegionsStolenByB > l_iNbRegionsStolenByA)
				{
					if(l_fSideBRel > SP2::c_fRelationsGood && l_fSideARel < SP2::c_fRelationsDislike)
					{
						g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
						return;
					}
				}				
			}
			break;
		case ETreatyType::Alliance:
			{
				if(l_fSideARel > SP2::c_fRelationsFriend)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::MilitaryAccess:
			{
				if(l_fSideBRel > SP2::c_fRelationsFriend)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::RequestMilitaryPresenceRemoval:
			{
				if(l_fSideBRel < SP2::c_fRelationsBad && l_fSideARel > SP2::c_fRelationsGood)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::Annexation:
			{
				if(l_fSideBRel < SP2::c_fRelationsEnnemy && l_fSideARel > SP2::c_fRelationsLove)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::FreeRegion:
			{
				if(l_fSideARel < SP2::c_fRelationsDislike && l_fSideBRel > SP2::c_fRelationsGood)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::CulturalExchanges:
		case ETreatyType::NobleCause:
		case ETreatyType::ResearchPartnership:
		case ETreatyType::HumanDevelopmentCollaboration:
		case ETreatyType::EconomicPartnership:
		case ETreatyType::CommonMarket:
		case ETreatyType::EconomicAid:
			{
				if(l_fSideARel > SP2::c_fRelationsLike)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::AssumeForeignDebt:
			{
				if(l_fSideBRel > SP2::c_fRelationsFriend)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		case ETreatyType::EconomicEmbargo:
			{
				if(l_fSideBRel < SP2::c_fRelationsDislike)
				{
					g_ServerDCL.JoinTreaty(m_iSource,l_Treaty.ID(),3);
					return;
				}
			}
			break;
		}		
	}

	//If we are here, it means the AI hasn't found any treaties to join and put pressure on.
	//It will then try to create a new one		
	const hash_set<SP2::SWarPair>& l_Wars = g_ServerDAL.Wars();
	const UINT8* l_DiploStatus = g_ServerDAL.CountryDiplomaticStatuses(m_iSource);
	UINT32 l_iFirstCountry = 0;
	UINT32 l_iSecondCountry = 0;
	REAL32 l_fRelationsFirst = 0.f;
	REAL32 l_fRelationsSecond = 0.f;
	for(hash_set<SP2::SWarPair>::const_iterator it = l_Wars.begin();
		 it != l_Wars.end(); it++)
	{
		l_iFirstCountry = (*it).m_iFirstCountry;
		l_iSecondCountry = (*it).m_iSecondCountry;
		if(l_DiploStatus[l_iFirstCountry] == EDiplomaticStatus::Hostile || 
			l_DiploStatus[l_iSecondCountry] == EDiplomaticStatus::Hostile)
			continue;
		l_fRelationsFirst = g_ServerDAL.RelationBetweenCountries(l_iFirstCountry,m_iSource);
		l_fRelationsSecond = g_ServerDAL.RelationBetweenCountries(l_iSecondCountry,m_iSource);		
		if( (l_fRelationsFirst > SP2::c_fRelationsLike && l_fRelationsSecond < SP2::c_fRelationsDislike) ||
			 (l_fRelationsSecond > SP2::c_fRelationsLike && l_fRelationsFirst < SP2::c_fRelationsDislike) )
		{
			set<UINT32> l_SideA;
			l_SideA.insert(l_iFirstCountry);
			set<UINT32> l_SideB;
			l_SideB.insert(l_iSecondCountry);
			set<UINT32> l_Pressure;
			l_Pressure.insert(m_iSource);
			UINT32 l_iConditions[ETreatyConditions::ItemCount];
			Memory::Fill<UINT32>(l_iConditions,0,ETreatyConditions::ItemCount);
			GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::CeaseFire,g_ServerDAL.GetString(g_ServerDAL.CountryData(m_iSource)->NameID()));
			g_ServerDCL.CreateNewTreaty(m_iSource,l_SideA,l_SideB,l_Pressure,ETreatyType::CeaseFire,true,l_sTreatyName,l_iConditions);
			return;
		}
	}

}