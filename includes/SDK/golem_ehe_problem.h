#include "golem_sdk_api.h"
/**************************************************************
*
* golem_ehe_problem.h
*
* Description
* ===========
*	Definitions and uses of GProblems
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/
#ifndef GEHE_Problem_H
#define GEHE_Problem_H

namespace EHE
{

//! a GEHE_Problem is a definition of a problem for the EHE dynamically linked
class GOLEM_SDK_API GAEHE_Problem
{
private:

public:

   GAEHE_Problem();
   ~GAEHE_Problem();

   PROBLEM_ID m_iID;
   GString m_sName;
   vector<GDataLink> m_DataLinked;				// how a problem is linked with data

   map<INT32, GPlan*> m_EffectivePlans;	// plans to be used for that problem

   // must be instanciated to be used
   virtual float Evaluate(INT32 id) = 0;
   virtual bool Does_Act(INT32 id, REAL32 value) = 0;
};

}

#endif