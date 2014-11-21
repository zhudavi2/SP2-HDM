#include "golem_pch.hpp"


using namespace EHEActions;


GMoveToDefend* GMoveToDefend::Clone() const
{
   return new GMoveToDefend( *this ); 
}


void GMoveToDefend::Execute()
{

}