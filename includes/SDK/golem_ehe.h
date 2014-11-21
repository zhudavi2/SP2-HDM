#include "golem_sdk_api.h"
/**************************************************************
*
* golem_ehe.h
*
* Description
* ===========
*	Main file for the EHE. Include this one to use it, it includes the others
*
* Owner
* =====
*	Jean-René Couture
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/
#ifndef EHE_h
#define EHE_h

#pragma warning( disable : 4786 )				// Disable warning messages 4786

using namespace std;

namespace EHE
{

// these are system problems functions and must exist on the client side
class GOLEM_SDK_API GAEHE_Gameplay
{
public:

   virtual float EHESystemEvaluatePoli(int id) = 0;
   virtual float EHESystemEvaluateEcon(int id) = 0;
   virtual float EHESystemEvaluateMili(int id) = 0;
   virtual bool EHESystemActPoli(int id, float value) = 0;
   virtual bool EHESystemActEcon(int id, float value) = 0;
   virtual bool EHESystemActMili(int id, float value) = 0;
};

//! return values for the EHE functions
enum EEHE_Return
{
   EHE_OK,										// normal error-free return
   EHE_NOT_INITIALIZED,						// not yet initialized and trying to do something
   EHE_ALREADY_INITIALIZED,					// trying to init a second time
   EHE_CLOSED,									// EHE closed and trying to do something
   EHE_ARGUMENT_OUT_OF_BOUND,					// an argument was out of bound
   EHE_COULDNT_CREATE_PLAN,					// used by Do_Something to say it couldn't think of anything to do
   EHE_CANT_ECHO,								// used in the DEBUG_Echo functions
   EHE_FILE_ERROR,								// can't read or write in a file
   EHE_ILLOGICAL_ERROR							// everything else
};

//! used in echo to determine what to do with the problem
enum EEHE_DEBUG_Echo_Types
{
   EHE_ECHO_DEV,
   EHE_ECHO_WARN,
   EHE_ECHO_ERROR
};

//! GEHE_Logic is the entry-way to the EHE by the program. It's the main module containing
//! the contexts.
class GOLEM_SDK_API GEHE_Logic
{
private:
   // these are used to determine if the EHE outputs stuff
   bool m_bDEBUGDevEcho;
   bool m_bDEBUGWarnEcho;
   bool m_bDEBUGErrorEcho;
   GString m_SDEBUGEchoFilename;
   bool m_bDEBUGScreenEcho;

   bool m_bInitialized;
   bool m_bSubjectivity;
   bool m_bUsingRelationProblems;

   INT32 m_iHighestAvailableGDataLinkID;

   map<INT32, GData> m_GameData;			   // links to actual data in the game
   map<INT32, GAction> m_GameAction;		   // links to actions in the game
   map<INT32, GAEHE_Problem *> m_GameProblem;   // links to problems in the game

   map<INT32, GPlan> m_KnownPlans;       // all known plans. Then, the problems have pointers to
                                             // plans that do something.

public:
   // General Functions
   GEHE_Logic();
   ~GEHE_Logic();

   EEHE_Return Init();							// initializes the engine
   EEHE_Return Shutdown();						// must be called to clean up

   // Context functions
   EEHE_Return Iterate(GContext* context, bool relation);
   EEHE_Return Do_Something(GContext *context, INT32 ProbID, REAL32 ProbValue);
   EEHE_Return Build_Plan(GContext *context, GPlan *plan); 

   // Sets the subjectivity values
   EEHE_Return SetSubjectivity(bool val);
   EEHE_Return SetRelationProblem(bool val);

   // GData functions
   // adds a new data to the list
   EEHE_Return Attach_Data(INT32 id, GData Data);	
   EEHE_Return Remove_Data(INT32 id);		// removes the data link
   int Return_Number_Of_Data();				// return the number of elements linked in

   // GAction functions
   // adds a new action to the list
   EEHE_Return Attach_Action(INT32 id, GAction Action);	
   EEHE_Return Remove_Action(INT32 id);		// removes the action link
   int Return_Number_Of_Action();				// return the number of actions linked in
   EEHE_Return Attach_Data_To_Action(INT32 id, GDataLink data);
   EEHE_Return Build_Data_Actions_Link();

   map<INT32, GEHEAction *> m_PerformActions;   // links to actions to execute (raise taxes hack)

   // GEHE_Problems functions
   // adds a new problem to the list
   EEHE_Return Attach_Problem(INT32 id, GAEHE_Problem *Problem);	
   EEHE_Return Remove_Problem(INT32 id);		// removes the problem link
   int Return_Number_Of_Problem();				// return the number of problems linked in
   // tells how a problem is computed
   EEHE_Return Attach_Data_To_Problem(INT32 id, GDataLink data);	

   // GPlan functions

   EEHE_Return Load_Plan(FILE *stream);
   EEHE_Return Save_Plans(FILE *stream);
   EEHE_Return Load_Plans(FILE *stream);
   EEHE_Return Add_Known_Plan(INT32 ID, GPlan plan);

   // Training functions

   EEHE_Return Generate_Known_Plans();
   EEHE_Return Generate_Experience_Data();
   EEHE_Return Build_Effective_Plans_Map();

   // utils functions
   INT32 Get_Next_Available_DataID();

   // DEBUG code
   EEHE_Return DEBUG_Set_Dev_Echo(bool value);
   EEHE_Return DEBUG_Set_Warn_Echo(bool value);
   EEHE_Return DEBUG_Set_Error_Echo(bool value);

   EEHE_Return DEBUG_Set_Console(bool screen, GString filename);
   EEHE_Return DEBUG_Echo(EEHE_DEBUG_Echo_Types type, const char *str, ...);

   EEHE_Return DEBUG_Print_Program_Status();	// print out status of linked game elements
};

}

#endif