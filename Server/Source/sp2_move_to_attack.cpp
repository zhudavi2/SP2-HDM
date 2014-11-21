#include "golem_pch.hpp"

using namespace EHEActions;


GMoveToAttack* GMoveToAttack::Clone() const
{
   return new GMoveToAttack( *this ); 
}

void GMoveToAttack::Execute()
{		
	const set<UINT32>& l_vGroupIDsSource = g_Joshua.UnitManager().CountryUnitGroups(m_iSource);
	const set<UINT32>& l_vGroupIDsTarget = g_Joshua.UnitManager().CountryUnitGroups(m_iTarget);

	// For each unit for this country, take a decision
   set<UINT32>::const_iterator l_GroupIt = l_vGroupIDsSource.begin();
	while(l_GroupIt != l_vGroupIDsSource.end() )
	{
      SDK::Combat::GUnitGroup* l_UnitSource = g_Joshua.UnitManager().UnitGroup(*l_GroupIt);

		//let's find the coordinate of a target country
		SDK::Combat::GUnitGroup* l_UnitTarget = g_Joshua.UnitManager().UnitGroup(*l_vGroupIDsTarget.begin());		

      vector<SDK::Combat::GUnitGroup*> l_vMoveGroups;
      l_vMoveGroups.push_back(l_UnitSource);

		if(l_UnitTarget)
		{
         g_ServerDCL.UnitMover().MoveUnits(l_vMoveGroups, l_UnitTarget->Position(), EMilitaryAction::Attack);
		}
		//?
		l_GroupIt++;
	}	

}