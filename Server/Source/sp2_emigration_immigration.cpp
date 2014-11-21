#include "golem_pch.hpp"


using namespace EHEActions;


GOpenImmigration* GOpenImmigration::Clone() const
{
   return new GOpenImmigration( *this ); 
}


void GOpenImmigration::Execute()
{
	g_ServerDCL.ChangeImmigration(m_iSource,false);
}

bool GOpenImmigration::CheckHardConditions() const
{	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() > 0.3f)
		return false;
	
	if(g_ServerDAL.CountryData(m_iSource)->ImmigrationClosed())
		return true;
	return false;
}

GCloseImmigration* GCloseImmigration::Clone() const
{
   return new GCloseImmigration( *this ); 
}


void GCloseImmigration::Execute()
{
	g_ServerDCL.ChangeImmigration(m_iSource,true);
}

bool GCloseImmigration::CheckHardConditions() const
{	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() > 0.3f)
		return false;

	if(!g_ServerDAL.CountryData(m_iSource)->ImmigrationClosed())
		return true;
	return false;
}

GOpenEmigration* GOpenEmigration::Clone() const
{
   return new GOpenEmigration( *this ); 
}


void GOpenEmigration::Execute()
{
	g_ServerDCL.ChangeEmigration(m_iSource,false);
}

bool GOpenEmigration::CheckHardConditions() const
{	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() > 0.3f)
		return false;

	if(g_ServerDAL.CountryData(m_iSource)->EmigrationClosed())
		return true;
	return false;
}

GCloseEmigration* GCloseEmigration::Clone() const
{
   return new GCloseEmigration( *this ); 
}


void GCloseEmigration::Execute()
{
	g_ServerDCL.ChangeEmigration(m_iSource,true);
}

bool GCloseEmigration::CheckHardConditions() const
{	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() > 0.3f)
		return false;

	if(!g_ServerDAL.CountryData(m_iSource)->EmigrationClosed())
		return true;
	return false;
}