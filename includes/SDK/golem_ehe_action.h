#include "golem_sdk_api.h"
/**************************************************************
*
* golem_ehe_action.h
*
* Description
* ===========
*	Definition and uses of a GAction
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/
#ifndef GAction_H
#define GAction_H

namespace EHE
{

//! a GAction is a definition of an action for the EHE dynamically linked like with data
class GOLEM_SDK_API GAction
{
public:
   GAction();
   ~GAction();

   ACTION_ID   m_iID;									// >0
   GString     m_sName;
   INT64       m_ITimeToExecute;					   // 0 for right now. 
   vector<GDataLink> m_DataLinked;				   // how an action is linked with data
};

class GOLEM_SDK_API GEHEAction
{
private:

public:

   ACTION_ID   m_iID;
   virtual bool Execute(INT32 in_source, INT32 in_target, INT32 in_intensity) = 0;
};

}

#endif