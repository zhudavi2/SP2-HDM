/**************************************************************
*
* sp2_ehe_problems.h
*
* Description
* ===========
*	Problems definitions for the EHE
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/

#ifndef _SP2_EHE_PROBLEMS_H
#define _SP2_EHE_PROBLEMS_H

using namespace EHE;

// government is popular
class Entity_Problem_GVT_Popular : public GAEHE_Problem
	{
	public:

	REAL32 Evaluate(INT32 id);
	bool Does_Act(INT32 id, REAL32 value);
	};

// government has money
class Entity_Problem_Money : public GAEHE_Problem
	{
	public:

	REAL32 Evaluate(INT32 id);
	bool Does_Act(INT32 id, REAL32 value);
	};

//*******************************************************

//! system problems, must be defined if used
class Entity_Problem_Poli : public GAEHE_Problem
	{
	public:

	REAL32 Evaluate(INT32 id);
	bool Does_Act(INT32 id, REAL32 value);
	};

class Entity_Problem_Econ : public GAEHE_Problem
	{
	public:

	REAL32 Evaluate(INT32 id);
	bool Does_Act(INT32 id, REAL32 value);
	};

class Entity_Problem_Mili : public GAEHE_Problem
	{
	public:

	REAL32 Evaluate(INT32 id);
	bool Does_Act(INT32 id, REAL32 value);
	};

#endif