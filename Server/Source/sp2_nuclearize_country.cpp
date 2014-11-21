#include "golem_pch.hpp"


using namespace EHEActions;


GNuclearizeCountry* GNuclearizeCountry::Clone() const
{
   return new GNuclearizeCountry( *this ); 
}


void GNuclearizeCountry::Execute()
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	REAL32 l_fNuclearLevel = l_pCountryData->NuclearReady();

	if(l_fNuclearLevel < 0.f)
	{
		g_ServerDCL.StartNuclearResearch(m_iSource);
	}
	else
	{
		//Build some nukes, yeehaa!
		SP2::GUnitDesign* l_pBestDesign = NULL;
		const hash_map<UINT32,SDK::Combat::Design::GUnit*>& l_UnitDesigns = g_Joshua.UnitManager().UnitDesigns();
		for(hash_map<UINT32,SDK::Combat::Design::GUnit*>::const_iterator it = l_UnitDesigns.begin();
			 it != l_UnitDesigns.end(); it++)
		{
			GUnitDesign* l_pDesign = (SP2::GUnitDesign*)it->second;
			if(l_pDesign->DesignerID() == m_iSource &&
				l_pDesign->Type()->Id() == EUnitType::BallisticMissile)
			{
				if(!l_pBestDesign ||
					 (l_pDesign->MissileDamage() >= l_pBestDesign->MissileDamage() &&
					  l_pDesign->MissilePrecision() >= l_pBestDesign->MissilePrecision() &&
					  l_pDesign->MissileRange() >= l_pBestDesign->MissileRange()))
				{
					l_pBestDesign = l_pDesign;
				}
			}
		}

		SP2::GResearchInformation* l_pInfo =  (SP2::GResearchInformation*)l_pCountryData->ResearchInfo();

		UINT32 l_iDesignIdOfMissile = 0;

		if(!l_pBestDesign ||
			 (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage])     > l_pBestDesign->MissileDamage() || 
          (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision])  > l_pBestDesign->MissilePrecision() || 
			 (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange])      > l_pBestDesign->MissileRange())
		{
			//Create new design
			SP2::GUnitDesign* l_pNewDesign = GAI::CreateNewOptimalDesign(m_iSource,EUnitCategory::Nuclear,EUnitType::BallisticMissile);
			if(l_pNewDesign)
				l_iDesignIdOfMissile = l_pNewDesign->Id();
		}
		else
			l_iDesignIdOfMissile = l_pBestDesign->Id();

		
		//Build nuke
		if(l_iDesignIdOfMissile)
			g_ServerDCL.BuildUnits(m_iSource,m_iSource,l_iDesignIdOfMissile,10,0,EUnitProductionPriority::High);		
	}
}

bool GNuclearizeCountry::CheckHardConditions() const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	REAL32 l_fNuclearLevel = l_pCountryData->NuclearReady();
	if(l_fNuclearLevel < 0.f)
	{
		//Can the country start the program? (Enough money)
		REAL32 l_fMaxAmountToSpend = (REAL32)(max(0.f,(((l_pCountryData->BudgetRevenues() * 1.1f) - l_pCountryData->BudgetExpenses()) / 0.2f)) + max(0.f,l_pCountryData->MonetarySupplyAmount()));
		if(l_fMaxAmountToSpend < SP2::c_fNuclearResearchCost)
			return false;
	}
	else if(l_fNuclearLevel >= 1.f)
	{
		//Does it have at least 1 in research in every nuke category?
		SP2::GResearchInformation* l_pInfo =  (SP2::GResearchInformation*)l_pCountryData->ResearchInfo();
		
		if(l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileDamage]     < 1.f || 
         l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissilePrecision]  < 1.f || 
			l_pInfo->m_fMaxValues[EUnitCategory::Nuclear][EUnitDesignCharacteristics::MissileRange]      < 1.f)
			return false;
	}
	else
		return false;

	return true;
}