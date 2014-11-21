/**************************************************************
*
* sp2_ehe_support.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jean-René Couture
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"
#include "../include/sp2_ehe_support.h"
#include "../include/sp2_ehe_problems.h"
#include "../include/sp2_ehe_actions.h"

/*!
 * Gets the unique GEHE_Support's instance
 **/
GEHE_Support& GEHE_Support::Instance()
{
   static GEHE_Support l_Instance;
   return l_Instance;
}


GEHE_Support::GEHE_Support()
{
}

GEHE_Support::~GEHE_Support()
{
}

/*!
 * Initializes the gameplay elements of SP2 related to the EHE and how it sees the world
  * @return  true if successful
 **/
bool GEHE_Support::Initialize()
{
   // local variables
	GAction l_EAction;
	GContextActionWeight l_WeightData;

   // put to blank for now. Later to be built anyway with the training
	l_WeightData.Efficiency = 1.0f;

	// set if problem perception is subject to entity personal modifications
	g_Joshua.EHE()->SetSubjectivity(true);
	// to know if the freudian relationships between entities causing problems should be
	// used
	g_Joshua.EHE()->SetRelationProblem(true);

   // **************************************************************
   // create data
   // **************************************************************

   // Data 1. financial situation (now and near future)
   g_Joshua.EHE()->Attach_Data(EHE_ID_FINANCIAL_DATA, GData(EHE_ID_FINANCIAL_DATA, "Finances", EHE_INT, MAXIMIZE));
   // Data 2. population support
   g_Joshua.EHE()->Attach_Data(EHE_ID_POPSUPPORT_DATA, GData(EHE_ID_POPSUPPORT_DATA, "PopSupport", EHE_FLOAT, MAXIMIZE));

   // **************************************************************
   // create problems
   // **************************************************************

   // Problem 1. Government popular
   Entity_Problem_GVT_Popular *tp1 = new Entity_Problem_GVT_Popular();
   tp1->m_iID = EHE_ID_GVT_POPULAR_PROB;
   tp1->m_sName = "GvtPopular";
   g_Joshua.EHE()->Attach_Problem(tp1->m_iID, tp1);

   // Problem 2. Has Money
   Entity_Problem_Money *tp2 = new Entity_Problem_Money();
   tp2->m_iID = EHE_ID_HAS_MONEY_PROB;
   tp2->m_sName = "HasMoney";
   g_Joshua.EHE()->Attach_Problem(tp2->m_iID, tp2);

   // link the problems and the behaviors in the contexts
   for(INT32 c1 = 1; c1 <= g_Joshua.ActivePlayers(); c1++)
   {
      // for every problem
      for(INT32 c2 = 1; c2 <= g_Joshua.EHE()->Return_Number_Of_Problem(); c2++)
      {
         // register the problem in the context
         g_Joshua.ActivePlayer(c1)->Context()->CreateProblemsArray(c2);
         // set the subjectivity modifier for the context
         // todo - load from file
         g_Joshua.ActivePlayer(c1)->Context()->SetSubjectivityModifier(c2, 1.0f);
      }
   }

   // **************************************************************
   // system problems
   // **************************************************************
	Entity_Problem_Poli *kpoli = new Entity_Problem_Poli();
	kpoli->m_iID = SYSTEM_PROBLEM_POLI;
	kpoli->m_sName = "Poli";
	g_Joshua.EHE()->Attach_Problem(kpoli->m_iID, kpoli);

	Entity_Problem_Econ *kecon = new Entity_Problem_Econ();
	kecon->m_iID = SYSTEM_PROBLEM_ECON;
	kecon->m_sName = "Econ";
	g_Joshua.EHE()->Attach_Problem(kecon->m_iID, kecon);

	Entity_Problem_Mili *kmili = new Entity_Problem_Mili();
	kmili->m_iID = SYSTEM_PROBLEM_MILI;
	kmili->m_sName = "Mili";
	g_Joshua.EHE()->Attach_Problem(kmili->m_iID, kmili);

   // **************************************************************
   // link data to problems
   // **************************************************************
   
   // Problem 1. Government popular
	g_Joshua.EHE()->Attach_Data_To_Problem(EHE_ID_GVT_POPULAR_PROB, 
      GDataLink(EHE_ID_FINANCIAL_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));
	g_Joshua.EHE()->Attach_Data_To_Problem(EHE_ID_GVT_POPULAR_PROB, 
      GDataLink(EHE_ID_POPSUPPORT_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));

   // Problem 2. Has Money
	g_Joshua.EHE()->Attach_Data_To_Problem(EHE_ID_HAS_MONEY_PROB, 
      GDataLink(EHE_ID_FINANCIAL_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));
	g_Joshua.EHE()->Attach_Data_To_Problem(EHE_ID_HAS_MONEY_PROB, 
      GDataLink(EHE_ID_POPSUPPORT_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));

   // **************************************************************
   // create actions
   // **************************************************************

   // Action 1. Raise tax
	l_EAction.m_iID = EHE_ID_RAISE_TAX_ACT;
	l_EAction.m_sName = "RaiseTax";
	l_EAction.m_ITimeToExecute = EHE_ID_RAISE_TAX_TIME;
	g_Joshua.EHE()->Attach_Action(l_EAction.m_iID, l_EAction);

   EHE_Action_Raise_Tax *ta1 = new EHE_Action_Raise_Tax();
   ta1->m_iID = EHE_ID_RAISE_TAX_ACT;
   g_Joshua.EHE()->m_PerformActions.insert(make_pair(ta1->m_iID, ta1));

   // Action 2. Lower taxes
	l_EAction.m_iID = EHE_ID_LOWER_TAX_ACT;
	l_EAction.m_sName = "LowerTax";
	l_EAction.m_ITimeToExecute = EHE_ID_LOWER_TAX_TIME;
	g_Joshua.EHE()->Attach_Action(l_EAction.m_iID, l_EAction);

   EHE_Action_Lower_Tax *ta2 = new EHE_Action_Lower_Tax();
   ta2->m_iID = EHE_ID_LOWER_TAX_ACT;
   g_Joshua.EHE()->m_PerformActions.insert(make_pair(ta2->m_iID, ta2));

   // link the actions with the contextes
   for(c1 = 1; c1 <= g_Joshua.ActivePlayers(); c1++)
   {
      // for every problem
      for(INT32 c2 = 1; c2 <= g_Joshua.EHE()->Return_Number_Of_Action(); c2++)
      {
         // adds relations and a blank weight
         g_Joshua.ActivePlayer(c1)->Context()->AddAction(c2, l_WeightData);
      }
   }

   // **************************************************************
   // link data to actions
   // **************************************************************

   // Action 1. Raise taxes
	g_Joshua.EHE()->Attach_Data_To_Action(EHE_ID_RAISE_TAX_ACT, 
      GDataLink(EHE_ID_FINANCIAL_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));
	g_Joshua.EHE()->Attach_Data_To_Action(EHE_ID_RAISE_TAX_ACT, 
      GDataLink(EHE_ID_POPSUPPORT_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));

   // Action 2. Lower taxes
	g_Joshua.EHE()->Attach_Data_To_Action(EHE_ID_LOWER_TAX_ACT, 
      GDataLink(EHE_ID_FINANCIAL_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));
	g_Joshua.EHE()->Attach_Data_To_Action(EHE_ID_LOWER_TAX_ACT, 
      GDataLink(EHE_ID_POPSUPPORT_DATA, g_Joshua.EHE()->Get_Next_Available_DataID()));

   // **************************************************************
   // build data actions link
   // **************************************************************

   g_Joshua.EHE()->Build_Data_Actions_Link();

   return true;
}

/*!
 * This is the function that is passed as pointer to contextes to have an evaluation of their relations
 * @param   in_Param    : in_fromid is the from ID country
 *                      : in_toid is the to ID country
 * @return  Return value: the description of the relation between from and to
 **/
GEntityDescription GEHE_Support::EHESystemEvaluateRelations(INT32 in_fromid, INT32 in_toid)
{
   GEntityDescription l_ret;

   //! \todo 
   l_ret.Poli = 0.5f;
   l_ret.Econ = 0.5f;
   l_ret.Mili = 0.5f;

   return l_ret;
}

INT32 GEHE_Support::EHESystemReturnRecursiveLevels(INT32 id)
{
   //! \todo 
   return 2;
}
