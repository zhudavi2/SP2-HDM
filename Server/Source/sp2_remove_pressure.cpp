#include "golem_pch.hpp"


using namespace EHEActions;


GRemovePressure* GRemovePressure::Clone() const
{
   return new GRemovePressure( *this ); 
}


void GRemovePressure::Execute()
{	
	multimap<REAL32,UINT32> l_mListOfPossibilities;
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();		
	REAL32 l_fSideARel = 0.f;
	REAL32 l_fSideBRel = 0.f;
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;
		if(l_Treaty.CountrySide(m_iSource) == 3)
		{
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

			if(l_Treaty.Sides() == 2)
				l_mListOfPossibilities.insert(make_pair(min(l_fSideBRel,l_fSideARel),l_Treaty.ID()));	
			else
				l_mListOfPossibilities.insert(make_pair(l_fSideARel,l_Treaty.ID()));				
		}
			
	}

	//For now, let's just take the worst relation treaties
	if(l_mListOfPossibilities.size() > 0)
		g_ServerDCL.LeaveTreaty(m_iSource,l_mListOfPossibilities.begin()->second);
	
	/* //! \todo Run through the possible countries
	for(multimap<REAL32,UINT32>::const_iterator l_Itr = l_mListOfPossibilities.begin();
		 l_Itr != l_mListOfPossibilities.end(); l_Itr++)
	{		
	}*/
}