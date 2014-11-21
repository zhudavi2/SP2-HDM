#include "golem_pch.hpp"


using namespace EHEActions;


GLowerResourceTaxes* GLowerResourceTaxes::Clone() const
{
   return new GLowerResourceTaxes( *this ); 
}


void GLowerResourceTaxes::Execute()
{
	const REAL32 c_fMinimumTax = 0.02f;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);	
	
	EResources::Enum l_eResource = EResources::Cereals;
	
	//First, choose the right resource
	if(l_pCountryData->ResourceProduction(EResources::Cereals) < l_pCountryData->ResourceDemand(EResources::Cereals) && l_pCountryData->ResourceLegal(EResources::Cereals) && l_pCountryData->ResourceTaxes(EResources::Cereals) >= c_fMinimumTax)
		l_eResource = EResources::Cereals;
	else if(l_pCountryData->ResourceProduction(EResources::Vegetable_Fruits) < l_pCountryData->ResourceDemand(EResources::Vegetable_Fruits) && l_pCountryData->ResourceLegal(EResources::Vegetable_Fruits) && l_pCountryData->ResourceTaxes(EResources::Vegetable_Fruits) >= c_fMinimumTax)
		l_eResource = EResources::Vegetable_Fruits;
	else if(l_pCountryData->ResourceProduction(EResources::Dairy) < l_pCountryData->ResourceDemand(EResources::Dairy) && l_pCountryData->ResourceLegal(EResources::Dairy) && l_pCountryData->ResourceTaxes(EResources::Dairy) >= c_fMinimumTax)
		l_eResource = EResources::Dairy;
	else if(l_pCountryData->ResourceProduction(EResources::Meat) < l_pCountryData->ResourceDemand(EResources::Meat) && l_pCountryData->ResourceLegal(EResources::Meat) && l_pCountryData->ResourceTaxes(EResources::Meat) >= c_fMinimumTax)
		l_eResource = EResources::Meat;
	else if(l_pCountryData->ResourceProduction(EResources::Tobacco) < l_pCountryData->ResourceDemand(EResources::Tobacco) && l_pCountryData->ResourceLegal(EResources::Tobacco) && l_pCountryData->ResourceTaxes(EResources::Tobacco) >= c_fMinimumTax)
		l_eResource = EResources::Tobacco;
	else if(l_pCountryData->ResourceProduction(EResources::Electricity) < l_pCountryData->ResourceDemand(EResources::Electricity) && l_pCountryData->ResourceLegal(EResources::Electricity) && l_pCountryData->ResourceTaxes(EResources::Electricity) >= c_fMinimumTax)
		l_eResource = EResources::Electricity;
	else if(l_pCountryData->ResourceProduction(EResources::Fossile_Fuels) < l_pCountryData->ResourceDemand(EResources::Fossile_Fuels) && l_pCountryData->ResourceLegal(EResources::Fossile_Fuels) && l_pCountryData->ResourceTaxes(EResources::Fossile_Fuels) >= c_fMinimumTax)
		l_eResource = EResources::Fossile_Fuels;
	else if(l_pCountryData->ResourceProduction(EResources::Wood_Paper) < l_pCountryData->ResourceDemand(EResources::Wood_Paper) && l_pCountryData->ResourceLegal(EResources::Wood_Paper) && l_pCountryData->ResourceTaxes(EResources::Wood_Paper) >= c_fMinimumTax)
		l_eResource = EResources::Wood_Paper;
	else if(l_pCountryData->ResourceProduction(EResources::Iron_Steel) < l_pCountryData->ResourceDemand(EResources::Iron_Steel) && l_pCountryData->ResourceLegal(EResources::Iron_Steel) && l_pCountryData->ResourceTaxes(EResources::Iron_Steel) >= c_fMinimumTax)
		l_eResource = EResources::Iron_Steel;
	else if(l_pCountryData->ResourceProduction(EResources::Minerals) < l_pCountryData->ResourceDemand(EResources::Minerals) && l_pCountryData->ResourceLegal(EResources::Minerals) && l_pCountryData->ResourceTaxes(EResources::Minerals) >= c_fMinimumTax)
		l_eResource = EResources::Minerals;
	else if(l_pCountryData->ResourceProduction(EResources::Fabrics) < l_pCountryData->ResourceDemand(EResources::Fabrics) && l_pCountryData->ResourceLegal(EResources::Fabrics) && l_pCountryData->ResourceTaxes(EResources::Fabrics) >= c_fMinimumTax)
		l_eResource = EResources::Fabrics;
	else if(l_pCountryData->ResourceProduction(EResources::Plastics) < l_pCountryData->ResourceDemand(EResources::Plastics) && l_pCountryData->ResourceLegal(EResources::Plastics) && l_pCountryData->ResourceTaxes(EResources::Plastics) >= c_fMinimumTax)
		l_eResource = EResources::Plastics;
	else if(l_pCountryData->ResourceProduction(EResources::Chemicals) < l_pCountryData->ResourceDemand(EResources::Chemicals) && l_pCountryData->ResourceLegal(EResources::Chemicals) && l_pCountryData->ResourceTaxes(EResources::Chemicals) >= c_fMinimumTax)
		l_eResource = EResources::Chemicals;
	else if(l_pCountryData->ResourceProduction(EResources::Pharmaceuticals) < l_pCountryData->ResourceDemand(EResources::Pharmaceuticals) && l_pCountryData->ResourceLegal(EResources::Pharmaceuticals) && l_pCountryData->ResourceTaxes(EResources::Pharmaceuticals) >= c_fMinimumTax)
		l_eResource = EResources::Pharmaceuticals;
	else if(l_pCountryData->ResourceProduction(EResources::Appliances) < l_pCountryData->ResourceDemand(EResources::Appliances) && l_pCountryData->ResourceLegal(EResources::Appliances) && l_pCountryData->ResourceTaxes(EResources::Appliances) >= c_fMinimumTax)
		l_eResource = EResources::Appliances;
	else if(l_pCountryData->ResourceProduction(EResources::Vehicules) < l_pCountryData->ResourceDemand(EResources::Vehicules) && l_pCountryData->ResourceLegal(EResources::Vehicules) && l_pCountryData->ResourceTaxes(EResources::Vehicules) >= c_fMinimumTax)
		l_eResource = EResources::Vehicules;
	else if(l_pCountryData->ResourceProduction(EResources::Machinery) < l_pCountryData->ResourceDemand(EResources::Machinery) && l_pCountryData->ResourceLegal(EResources::Machinery) && l_pCountryData->ResourceTaxes(EResources::Machinery) >= c_fMinimumTax)
		l_eResource = EResources::Machinery;
	else if(l_pCountryData->ResourceProduction(EResources::Commodities) < l_pCountryData->ResourceDemand(EResources::Commodities) && l_pCountryData->ResourceLegal(EResources::Commodities) && l_pCountryData->ResourceTaxes(EResources::Commodities) >= c_fMinimumTax)
		l_eResource = EResources::Commodities;
	else if(l_pCountryData->ResourceProduction(EResources::Luxury_Commodities) < l_pCountryData->ResourceDemand(EResources::Luxury_Commodities) && l_pCountryData->ResourceLegal(EResources::Luxury_Commodities) && l_pCountryData->ResourceTaxes(EResources::Luxury_Commodities) >= c_fMinimumTax)
		l_eResource = EResources::Luxury_Commodities;
	else if(l_pCountryData->ResourceProduction(EResources::Precious_Stones) < l_pCountryData->ResourceDemand(EResources::Precious_Stones) && l_pCountryData->ResourceLegal(EResources::Precious_Stones) && l_pCountryData->ResourceTaxes(EResources::Precious_Stones) >= c_fMinimumTax)
		l_eResource = EResources::Precious_Stones;
	else if(l_pCountryData->ResourceProduction(EResources::Construction) < l_pCountryData->ResourceDemand(EResources::Construction) && l_pCountryData->ResourceLegal(EResources::Construction) && l_pCountryData->ResourceTaxes(EResources::Construction) >= c_fMinimumTax)
		l_eResource = EResources::Construction;
	else if(l_pCountryData->ResourceProduction(EResources::Engineering) < l_pCountryData->ResourceDemand(EResources::Engineering) && l_pCountryData->ResourceLegal(EResources::Engineering) && l_pCountryData->ResourceTaxes(EResources::Engineering) >= c_fMinimumTax)
		l_eResource = EResources::Engineering;
	else if(l_pCountryData->ResourceProduction(EResources::Health_Care) < l_pCountryData->ResourceDemand(EResources::Health_Care) && l_pCountryData->ResourceLegal(EResources::Health_Care) && l_pCountryData->ResourceTaxes(EResources::Health_Care) >= c_fMinimumTax)
		l_eResource = EResources::Health_Care;
	else if(l_pCountryData->ResourceProduction(EResources::Retail) < l_pCountryData->ResourceDemand(EResources::Retail) && l_pCountryData->ResourceLegal(EResources::Retail) && l_pCountryData->ResourceTaxes(EResources::Retail) >= c_fMinimumTax)
		l_eResource = EResources::Retail;
	else if(l_pCountryData->ResourceProduction(EResources::Legal_Services) < l_pCountryData->ResourceDemand(EResources::Legal_Services) && l_pCountryData->ResourceLegal(EResources::Legal_Services) && l_pCountryData->ResourceTaxes(EResources::Legal_Services) >= c_fMinimumTax)
		l_eResource = EResources::Legal_Services;
	else if(l_pCountryData->ResourceProduction(EResources::Marketing_Advertising) < l_pCountryData->ResourceDemand(EResources::Marketing_Advertising) && l_pCountryData->ResourceLegal(EResources::Marketing_Advertising) && l_pCountryData->ResourceTaxes(EResources::Marketing_Advertising) >= c_fMinimumTax)
		l_eResource = EResources::Marketing_Advertising;
	else if(l_pCountryData->ResourceProduction(EResources::Drugs) < l_pCountryData->ResourceDemand(EResources::Drugs) && l_pCountryData->ResourceLegal(EResources::Drugs) && l_pCountryData->ResourceTaxes(EResources::Drugs) >= c_fMinimumTax)
		l_eResource = EResources::Drugs;	
	else
	{
		//No choice available, return
		return;
	}

	g_ServerDCL.ChangeResourceTax(m_iSource,l_eResource,l_pCountryData->ResourceTaxes(l_eResource)*0.5f);
}