#include "golem_pch.hpp"


using namespace EHEActions;


GPrivateResourceControl* GPrivateResourceControl::Clone() const
{
   return new GPrivateResourceControl( *this ); 
}


void GPrivateResourceControl::Execute()
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	EResources::Enum l_eResource = EResources::ItemCount;
	//First, choose the right resource
	if(l_pCountryData->ResourceGvtCtrl(EResources::Marketing_Advertising))
		l_eResource = EResources::Marketing_Advertising;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Legal_Services))
		l_eResource = EResources::Legal_Services;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Retail))
		l_eResource = EResources::Retail;	
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Health_Care))
		l_eResource = EResources::Health_Care;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Engineering))
		l_eResource = EResources::Engineering;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Construction))
		l_eResource = EResources::Construction;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Precious_Stones))
		l_eResource = EResources::Precious_Stones;	
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Luxury_Commodities))
		l_eResource = EResources::Luxury_Commodities;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Commodities))
		l_eResource = EResources::Commodities;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Machinery))
		l_eResource = EResources::Machinery;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Vehicules))
		l_eResource = EResources::Vehicules;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Appliances))
		l_eResource = EResources::Appliances;		
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Pharmaceuticals))
		l_eResource = EResources::Pharmaceuticals;	
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Chemicals))
		l_eResource = EResources::Chemicals;	
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Plastics))
		l_eResource = EResources::Plastics;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Fabrics))
		l_eResource = EResources::Fabrics;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Minerals))
		l_eResource = EResources::Minerals;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Iron_Steel))
		l_eResource = EResources::Iron_Steel;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Wood_Paper))
		l_eResource = EResources::Wood_Paper;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Fossile_Fuels))
		l_eResource = EResources::Fossile_Fuels;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Electricity))
		l_eResource = EResources::Electricity;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Tobacco))
		l_eResource = EResources::Tobacco;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Meat))
		l_eResource = EResources::Meat;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Dairy))
		l_eResource = EResources::Dairy;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Vegetable_Fruits))
		l_eResource = EResources::Vegetable_Fruits;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Cereals))
		l_eResource = EResources::Cereals;
	else if(l_pCountryData->ResourceGvtCtrl(EResources::Drugs))
		l_eResource = EResources::Drugs;	
	else
	{
		//No choice available, return
		return;
	}

	g_ServerDCL.ChangeResourceGvtControlled(m_iSource,l_eResource,false);

	m_fWhenToStepForward = g_Joshua.GameTime() + 90.f;	
}

bool GPrivateResourceControl::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GPrivateResourceControl::CheckHardConditions() const
{	
	//Check if at least one resource is public
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		if(l_pCountryData->ResourceGvtCtrl((EResources::Enum)i))
			return true;
	}

	return false;
}

GPublicResourceControl* GPublicResourceControl::Clone() const
{
   return new GPublicResourceControl( *this ); 
}


void GPublicResourceControl::Execute()
{

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	EResources::Enum l_eResource = EResources::ItemCount;
	//First, choose the right resource
	if(!l_pCountryData->ResourceGvtCtrl(EResources::Cereals))
		l_eResource = EResources::Cereals;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Vegetable_Fruits))
		l_eResource = EResources::Vegetable_Fruits;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Dairy))
		l_eResource = EResources::Dairy;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Meat))
		l_eResource = EResources::Meat;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Tobacco))
		l_eResource = EResources::Tobacco;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Electricity))
		l_eResource = EResources::Electricity;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Fossile_Fuels))
		l_eResource = EResources::Fossile_Fuels;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Wood_Paper))
		l_eResource = EResources::Wood_Paper;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Iron_Steel))
		l_eResource = EResources::Iron_Steel;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Minerals))
		l_eResource = EResources::Minerals;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Fabrics))
		l_eResource = EResources::Fabrics;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Plastics))
		l_eResource = EResources::Plastics;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Chemicals))
		l_eResource = EResources::Chemicals;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Pharmaceuticals))
		l_eResource = EResources::Pharmaceuticals;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Appliances))
		l_eResource = EResources::Appliances;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Vehicules))
		l_eResource = EResources::Vehicules;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Machinery))
		l_eResource = EResources::Machinery;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Commodities))
		l_eResource = EResources::Commodities;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Luxury_Commodities))
		l_eResource = EResources::Luxury_Commodities;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Precious_Stones))
		l_eResource = EResources::Precious_Stones;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Construction))
		l_eResource = EResources::Construction;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Engineering))
		l_eResource = EResources::Engineering;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Health_Care))
		l_eResource = EResources::Health_Care;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Retail))
		l_eResource = EResources::Retail;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Legal_Services))
		l_eResource = EResources::Legal_Services;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Marketing_Advertising))
		l_eResource = EResources::Marketing_Advertising;
	else if(!l_pCountryData->ResourceGvtCtrl(EResources::Drugs))
		l_eResource = EResources::Drugs;	
	else
	{
		//No choice available, return
		return;
	}

	g_ServerDCL.ChangeResourceGvtControlled(m_iSource,l_eResource,true);

	m_fWhenToStepForward = g_Joshua.GameTime() + 90.f;	
}

bool GPublicResourceControl::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GPublicResourceControl::CheckHardConditions() const
{	
	//Check if at least one resource is private
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);

	for(UINT32 i=0; i<EResources::ItemCount; i++)
	{
		if(!l_pCountryData->ResourceGvtCtrl((EResources::Enum)i))
			return true;
	}

	return false;
}