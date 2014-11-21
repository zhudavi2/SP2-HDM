#include "golem_pch.hpp"


using namespace EHEActions;


GTrainUnits* GTrainUnits::Clone() const
{
   return new GTrainUnits( *this ); 
}


void GTrainUnits::Execute()
{

}