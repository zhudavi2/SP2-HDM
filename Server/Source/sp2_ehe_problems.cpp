/**************************************************************
*
* sp2_ehe_problems.cpp
*
* Description
* ===========
*	See sp2_ehe_problems.h
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/

#include "golem_pch.hpp"

// Government is popular problem definition
REAL32 Entity_Problem_GVT_Popular::Evaluate(INT32 id)
{
   INT32 l_iprob = rand()%1000;

   return (REAL32)(l_iprob)/1000.0f;
}

bool Entity_Problem_GVT_Popular::Does_Act(INT32 id, REAL32 value)
{
   if(value >= 0.95)
      return true;

   return false;
}

// Government has money problem definition
REAL32 Entity_Problem_Money::Evaluate(INT32 id)
{
   INT32 l_iprob = rand()%1000;

   return (REAL32)(l_iprob)/1000.0f;
}

bool Entity_Problem_Money::Does_Act(INT32 id, REAL32 value)
{
   if(value >= 0.95)
      return true;

   return false;
}

//*******************************************************
//! system problems, must be defined if used

REAL32 Entity_Problem_Poli::Evaluate(INT32 id)
{
   return 0.0f;
}

bool Entity_Problem_Poli::Does_Act(INT32 id, REAL32 value)
{
   return false;
}

REAL32 Entity_Problem_Econ::Evaluate(INT32 id)
{
   return 0.0f;
}

bool Entity_Problem_Econ::Does_Act(INT32 id, REAL32 value)
{
   return false;
}

REAL32 Entity_Problem_Mili::Evaluate(INT32 id)
{
   return 0.0f;
}

bool Entity_Problem_Mili::Does_Act(INT32 id, REAL32 value)
{
   return false;
}
