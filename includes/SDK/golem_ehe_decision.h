#include "golem_sdk_api.h"
/**************************************************************
*
* golem_ehe_decision.h
*
* Description
* ===========
*	A GDecision is a complete decision (plan) involving one of more
*  actions. Defined and used here
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/
#ifndef GDecision_H
#define GDecision_H

namespace EHE
{

/*!
* structure used to compare a problem modifier with the modifier median
**/
struct GStepProblems
{
   REAL32 m_fModif;
   REAL32 m_fMedian;
};

/*!
* A decision to be converted in Game_Event form
**/
class GOLEM_SDK_API GDecision
{
private:
   bool        m_bValid;
   bool        m_bProcessed;
   ENTITY_ID   m_iSourceID;
   ENTITY_ID   m_iTargetID;
   ACTION_ID   m_iActionID;
   INT64       m_iExecutionTime;
   REAL32      m_fIntensity;
   REAL32      m_fEfficiency;

public:
   GDecision();
   ~GDecision();

   bool        SourceID(ENTITY_ID in_iValue);
   ENTITY_ID   SourceID();
   bool        TargetID(ENTITY_ID in_iValue);
   ENTITY_ID   TargetID();
   bool        ActionID(ACTION_ID in_iValue);
   ACTION_ID   ActionID();
   bool        Intensity(REAL32 in_fValue);
   REAL32      Intensity();
   bool        ExecutionTime(INT64 in_iValue);
   INT64       ExecutionTime();
   bool        Efficiency(REAL32 in_fValue);
   REAL32      Efficiency();

   bool IsValid();
   bool SetProcessed();		// automatically sets to true, can't revert to false
   bool IsProcessed();
};

/*!
* a step is one level containing one or more decisions
**/
class GOLEM_SDK_API GDecisionStep
{
private:

public:
   GDecisionStep();
   ~GDecisionStep();

   bool EvaluateProblemsModif();

   vector<GDecision>                m_Decisions;
   map<PROBLEM_ID, GStepProblems>   m_Problems;
};

/*!
* a complete plan contains one or more steps
**/
class GOLEM_SDK_API GPlan
{
private:
   UINT32      m_iScore;
   PROBLEM_ID  m_iProblemToSolve;

public:
   GPlan();
   ~GPlan();

   vector<GDecisionStep> DecisionTree;
   UINT32 m_iNbRequests;

   bool Load(FILE * io_pFile);
   bool Save(FILE * io_pFile);

   PROBLEM_ID ProblemToSolve();
   bool ProblemToSolve(PROBLEM_ID in_iProblemID);
   bool EvaluateScore(UINT32 in_iLevels);
   UINT32 Score();
   // returns true if the plan should be stopped.
   bool ObjectivesMet();
};

}

#endif