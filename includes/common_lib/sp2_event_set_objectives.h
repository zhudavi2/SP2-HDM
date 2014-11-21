/**************************************************************
*
* golem_setgameobjectives.h
*
* Description
* ===========
*  Describes the interface of class GSetGameObjectives
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SETGAMEOBJECTIVES_H_
#define _GOLEM_SETGAMEOBJECTIVES_H_

namespace SP2
{
namespace Event
{

class GSetGameObjectives : public SDK::Event::GGameEvent
{
   friend class GClient;
   friend class GServer;
   static SDK::GGameEventSPtr New();

public:

   GSetGameObjectives();
   ~GSetGameObjectives();
   bool Serialize(GIBuffer& out_Buffer) const;
   bool Unserialize(GOBuffer& in_Buffer);

   vector<GGameObjective> m_vObjectives;
protected:
private:
};

};
};
#endif //_GOLEM_SETGAMEOBJECTIVES_H_
