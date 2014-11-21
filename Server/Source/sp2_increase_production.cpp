#include "golem_pch.hpp"


using namespace EHEActions;


GIncreaseProduction* GIncreaseProduction::Clone() const
{
   return new GIncreaseProduction( *this ); 
}


void GIncreaseProduction::Execute()
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);	
	
	EResources::Enum l_eResource = EResources::Cereals;
	//First, choose the right resource
	if(l_pCountryData->ResourceProduction(EResources::Cereals) < l_pCountryData->ResourceDemand(EResources::Cereals) && l_pCountryData->ResourceLegal(EResources::Cereals) && l_pCountryData->ResourceProduction(EResources::Cereals) >= 1.f)
		l_eResource = EResources::Cereals;
	else if(l_pCountryData->ResourceProduction(EResources::Vegetable_Fruits) < l_pCountryData->ResourceDemand(EResources::Vegetable_Fruits) && l_pCountryData->ResourceLegal(EResources::Vegetable_Fruits) && l_pCountryData->ResourceProduction(EResources::Vegetable_Fruits) >= 1.f)
		l_eResource = EResources::Vegetable_Fruits;
	else if(l_pCountryData->ResourceProduction(EResources::Dairy) < l_pCountryData->ResourceDemand(EResources::Dairy) && l_pCountryData->ResourceLegal(EResources::Dairy) && l_pCountryData->ResourceProduction(EResources::Dairy) >= 1.f)
		l_eResource = EResources::Dairy;
	else if(l_pCountryData->ResourceProduction(EResources::Meat) < l_pCountryData->ResourceDemand(EResources::Meat) && l_pCountryData->ResourceLegal(EResources::Meat) && l_pCountryData->ResourceProduction(EResources::Meat) >= 1.f)
		l_eResource = EResources::Meat;
	else if(l_pCountryData->ResourceProduction(EResources::Tobacco) < l_pCountryData->ResourceDemand(EResources::Tobacco) && l_pCountryData->ResourceLegal(EResources::Tobacco) && l_pCountryData->ResourceProduction(EResources::Tobacco) >= 1.f)
		l_eResource = EResources::Tobacco;
	else if(l_pCountryData->ResourceProduction(EResources::Electricity) < l_pCountryData->ResourceDemand(EResources::Electricity) && l_pCountryData->ResourceLegal(EResources::Electricity) && l_pCountryData->ResourceProduction(EResources::Electricity) >= 1.f)
		l_eResource = EResources::Electricity;
	else if(l_pCountryData->ResourceProduction(EResources::Fossile_Fuels) < l_pCountryData->ResourceDemand(EResources::Fossile_Fuels) && l_pCountryData->ResourceLegal(EResources::Fossile_Fuels) && l_pCountryData->ResourceProduction(EResources::Fossile_Fuels) >= 1.f)
		l_eResource = EResources::Fossile_Fuels;
	else if(l_pCountryData->ResourceProduction(EResources::Wood_Paper) < l_pCountryData->ResourceDemand(EResources::Wood_Paper) && l_pCountryData->ResourceLegal(EResources::Wood_Paper) && l_pCountryData->ResourceProduction(EResources::Wood_Paper) >= 1.f)
		l_eResource = EResources::Wood_Paper;
	else if(l_pCountryData->ResourceProduction(EResources::Iron_Steel) < l_pCountryData->ResourceDemand(EResources::Iron_Steel) && l_pCountryData->ResourceLegal(EResources::Iron_Steel) && l_pCountryData->ResourceProduction(EResources::Iron_Steel) >= 1.f)
		l_eResource = EResources::Iron_Steel;
	else if(l_pCountryData->ResourceProduction(EResources::Minerals) < l_pCountryData->ResourceDemand(EResources::Minerals) && l_pCountryData->ResourceLegal(EResources::Minerals) && l_pCountryData->ResourceProduction(EResources::Minerals) >= 1.f)
		l_eResource = EResources::Minerals;
	else if(l_pCountryData->ResourceProduction(EResources::Fabrics) < l_pCountryData->ResourceDemand(EResources::Fabrics) && l_pCountryData->ResourceLegal(EResources::Fabrics) && l_pCountryData->ResourceProduction(EResources::Fabrics) >= 1.f)
		l_eResource = EResources::Fabrics;
	else if(l_pCountryData->ResourceProduction(EResources::Plastics) < l_pCountryData->ResourceDemand(EResources::Plastics) && l_pCountryData->ResourceLegal(EResources::Plastics) && l_pCountryData->ResourceProduction(EResources::Plastics) >= 1.f)
		l_eResource = EResources::Plastics;
	else if(l_pCountryData->ResourceProduction(EResources::Chemicals) < l_pCountryData->ResourceDemand(EResources::Chemicals) && l_pCountryData->ResourceLegal(EResources::Chemicals) && l_pCountryData->ResourceProduction(EResources::Chemicals) >= 1.f)
		l_eResource = EResources::Chemicals;
	else if(l_pCountryData->ResourceProduction(EResources::Pharmaceuticals) < l_pCountryData->ResourceDemand(EResources::Pharmaceuticals) && l_pCountryData->ResourceLegal(EResources::Pharmaceuticals) && l_pCountryData->ResourceProduction(EResources::Pharmaceuticals) >= 1.f)
		l_eResource = EResources::Pharmaceuticals;
	else if(l_pCountryData->ResourceProduction(EResources::Appliances) < l_pCountryData->ResourceDemand(EResources::Appliances) && l_pCountryData->ResourceLegal(EResources::Appliances) && l_pCountryData->ResourceProduction(EResources::Appliances) >= 1.f)
		l_eResource = EResources::Appliances;
	else if(l_pCountryData->ResourceProduction(EResources::Vehicules) < l_pCountryData->ResourceDemand(EResources::Vehicules) && l_pCountryData->ResourceLegal(EResources::Vehicules) && l_pCountryData->ResourceProduction(EResources::Vehicules) >= 1.f)
		l_eResource = EResources::Vehicules;
	else if(l_pCountryData->ResourceProduction(EResources::Machinery) < l_pCountryData->ResourceDemand(EResources::Machinery) && l_pCountryData->ResourceLegal(EResources::Machinery) && l_pCountryData->ResourceProduction(EResources::Machinery) >= 1.f)
		l_eResource = EResources::Machinery;
	else if(l_pCountryData->ResourceProduction(EResources::Commodities) < l_pCountryData->ResourceDemand(EResources::Commodities) && l_pCountryData->ResourceLegal(EResources::Commodities) && l_pCountryData->ResourceProduction(EResources::Commodities) >= 1.f)
		l_eResource = EResources::Commodities;
	else if(l_pCountryData->ResourceProduction(EResources::Luxury_Commodities) < l_pCountryData->ResourceDemand(EResources::Luxury_Commodities) && l_pCountryData->ResourceLegal(EResources::Luxury_Commodities) && l_pCountryData->ResourceProduction(EResources::Luxury_Commodities) >= 1.f)
		l_eResource = EResources::Luxury_Commodities;
	else if(l_pCountryData->ResourceProduction(EResources::Precious_Stones) < l_pCountryData->ResourceDemand(EResources::Precious_Stones) && l_pCountryData->ResourceLegal(EResources::Precious_Stones) && l_pCountryData->ResourceProduction(EResources::Precious_Stones) >= 1.f)
		l_eResource = EResources::Precious_Stones;
	else if(l_pCountryData->ResourceProduction(EResources::Construction) < l_pCountryData->ResourceDemand(EResources::Construction) && l_pCountryData->ResourceLegal(EResources::Construction) && l_pCountryData->ResourceProduction(EResources::Construction) >= 1.f)
		l_eResource = EResources::Construction;
	else if(l_pCountryData->ResourceProduction(EResources::Engineering) < l_pCountryData->ResourceDemand(EResources::Engineering) && l_pCountryData->ResourceLegal(EResources::Engineering) && l_pCountryData->ResourceProduction(EResources::Engineering) >= 1.f)
		l_eResource = EResources::Engineering;
	else if(l_pCountryData->ResourceProduction(EResources::Health_Care) < l_pCountryData->ResourceDemand(EResources::Health_Care) && l_pCountryData->ResourceLegal(EResources::Health_Care) && l_pCountryData->ResourceProduction(EResources::Health_Care) >= 1.f)
		l_eResource = EResources::Health_Care;
	else if(l_pCountryData->ResourceProduction(EResources::Retail) < l_pCountryData->ResourceDemand(EResources::Retail) && l_pCountryData->ResourceLegal(EResources::Retail) && l_pCountryData->ResourceProduction(EResources::Retail) >= 1.f)
		l_eResource = EResources::Retail;
	else if(l_pCountryData->ResourceProduction(EResources::Legal_Services) < l_pCountryData->ResourceDemand(EResources::Legal_Services) && l_pCountryData->ResourceLegal(EResources::Legal_Services) && l_pCountryData->ResourceProduction(EResources::Legal_Services) >= 1.f)
		l_eResource = EResources::Legal_Services;
	else if(l_pCountryData->ResourceProduction(EResources::Marketing_Advertising) < l_pCountryData->ResourceDemand(EResources::Marketing_Advertising) && l_pCountryData->ResourceLegal(EResources::Marketing_Advertising) && l_pCountryData->ResourceProduction(EResources::Marketing_Advertising) >= 1.f)
		l_eResource = EResources::Marketing_Advertising;
	else if(l_pCountryData->ResourceProduction(EResources::Drugs) < l_pCountryData->ResourceDemand(EResources::Drugs) && l_pCountryData->ResourceLegal(EResources::Drugs) && l_pCountryData->ResourceProduction(EResources::Drugs) >= 1.f)
		l_eResource = EResources::Drugs;	
	else
	{
		//Cycle through the resource, and choose the one where we have the highest market share				
		REAL64 l_fMaxShare = 0.f;
		const REAL64* l_pMarketProduction = g_ServerDAL.MarketProduction();
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			if(l_pMarketProduction[k] > 0.f &&
				l_pCountryData->ResourceProduction((EResources::Enum)k) / l_pMarketProduction[k] > l_fMaxShare)
			{
				l_fMaxShare = l_pCountryData->ResourceProduction((EResources::Enum)k) / l_pMarketProduction[k];
				l_eResource = (EResources::Enum)k;
			}
		}
	}
	
	//Then decide which increase production value the computer will use
	REAL64 l_fIncrease = 0.f;
	REAL64 l_fMarginOfIncrease = l_pCountryData->BudgetRevenues() - l_pCountryData->BudgetExpenses() + l_pCountryData->MonetarySupplyAmount();	
	if(l_fMarginOfIncrease < 0.f)
		l_fIncrease = 0.01f;
	else
	{
		REAL64 l_fResourceProduction = l_pCountryData->ResourceProduction(l_eResource);

		if(g_ServerDCL.IncreaseProductionApproxCost(0.25f,l_fResourceProduction) <= l_fMarginOfIncrease)
			l_fIncrease = 0.25f;
		else if(g_ServerDCL.IncreaseProductionApproxCost(0.10f,l_fResourceProduction) <= l_fMarginOfIncrease)
			l_fIncrease = 0.10f;
		else if(g_ServerDCL.IncreaseProductionApproxCost(0.05f,l_fResourceProduction) <= l_fMarginOfIncrease)
			l_fIncrease = 0.05f;
		else if(g_ServerDCL.IncreaseProductionApproxCost(0.02f,l_fResourceProduction) <= l_fMarginOfIncrease)
			l_fIncrease = 0.02f;
	}

	//Increase the production
	g_ServerDCL.IncreaseProduction(m_iSource,l_eResource,l_fIncrease);
}

void GIncreaseProduction::Conditions( vector<GCondition>& out_Conditions ) const
{
	out_Conditions.push_back(GCondition(EHEObjectives::Ids::HAVE_NO_DEBT,-0.2f));
}