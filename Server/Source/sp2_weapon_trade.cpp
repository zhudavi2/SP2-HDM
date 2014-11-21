#include "golem_pch.hpp"


using namespace EHEActions;


GWeaponTrade* GWeaponTrade::Clone() const
{
   return new GWeaponTrade( *this ); 
}


void GWeaponTrade::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::WeaponTrade);
}

bool GWeaponTrade::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::WeaponTrade] > 3)
		return false;
	return true;
}


GWeaponTradeEmbargo* GWeaponTradeEmbargo::Clone() const
{
   return new GWeaponTradeEmbargo( *this ); 
}


void GWeaponTradeEmbargo::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::WeaponTradeEmbargo);
}

bool GWeaponTradeEmbargo::CheckHardConditions() const
{	
	return true;
}