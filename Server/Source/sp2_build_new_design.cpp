#include "golem_pch.hpp"


using namespace EHEActions;


GBuildNewDesign* GBuildNewDesign::Clone() const
{
   return new GBuildNewDesign( *this ); 
}


void GBuildNewDesign::Execute()
{

}