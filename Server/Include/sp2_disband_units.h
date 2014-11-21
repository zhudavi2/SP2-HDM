#ifndef _EHE_ACTIONS_DISBAND_UNITS
#define _EHE_ACTIONS_DISBAND_UNITS


namespace EHEActions
{


class GDisbandUnits : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandUnits* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif