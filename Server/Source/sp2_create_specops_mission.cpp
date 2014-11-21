#include "golem_pch.hpp"


using namespace EHEActions;


GCreateSpecopsMission* GCreateSpecopsMission::Clone() const
{
   return new GCreateSpecopsMission( *this ); 
}


void GCreateSpecopsMission::Execute()
{

}