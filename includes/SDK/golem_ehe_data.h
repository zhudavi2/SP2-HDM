#include "golem_sdk_api.h"
/**************************************************************
*
* golem_ehe_data.h
*
* Description
* ===========
*	Definitions and uses of GData
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/
#ifndef GData_H
#define GData_H

namespace EHE
{

//! the different kinds of data known to the EHE
enum EDataType
{
   EHE_BOOL,
   EHE_INT,
   EHE_INT64,
   EHE_FLOAT
};

//! what to do with the data
enum EDataGoal
{
   MINIMIZE,
   MAXIMIZE,
   STANDARDIZE,
   NO_OBJECTIVE
};

//! GData is the link between the EHE core logic and the game data. It's dynamically
//! linked in the game through Attach_Data
class GOLEM_SDK_API GData
{
private:

public:
   GData();
   GData(DATA_ID in_iID, GString in_sName, EDataType in_Type, EDataGoal in_Goal);

   DATA_ID m_iID;									// >0
   GString m_sName;

   vector<ACTION_ID> m_ActionsLinked;		// includes only IDs of actions

   EDataType m_Type;							   // type of data
   EDataGoal m_Goal;							   // what to do with this data
};

/*!
* used to link GData with either actions or problems
**/
class GOLEM_SDK_API GDataLink
{
private:

public:
   GDataLink();
   ~GDataLink();
   GDataLink(DATA_ID in_iDataID, INT32 pID);

   INT32 m_iDataID;
   INT32 m_iID;
};

}

#endif