#include "golem_pch.hpp"


using namespace EHEActions;


GAskForHelp* GAskForHelp::Clone() const
{
   return new GAskForHelp( *this ); 
}


void GAskForHelp::Execute()
{

}