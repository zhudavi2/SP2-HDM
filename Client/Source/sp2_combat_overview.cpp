/**************************************************************
*
* sp2_combat_overview.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_combat_overview.h"

CWSTR FRAME_BORDER = L"frmBorder";
CWSTR FRAME_BALANCE= L"frmBalance";
CWSTR SOLDIERS_BAR = L"frmSoldiersBar";
CWSTR LAND_BAR     = L"frmLandBar";
CWSTR AIR_BAR      = L"frmAirBar";
CWSTR NAVAL_BAR    = L"frmNavalBar";
CWSTR OVERALL_BAR  = L"frmOverallBar";
CWSTR BLUE_BAR     = L"frmBlue";
CWSTR RED_BAR      = L"frmRed";
CWSTR VERSUS_BAR   = L"lblVs";


const GString GCombatOverview::TypeName(L"Battle_Progress_Window");

GUI::GBaseObject* GCombatOverview::New()
{
   return new GCombatOverview();
}


GCombatOverview::GCombatOverview() : GFrame()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Combat Overview");
}

GCombatOverview::~GCombatOverview() 
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Combat Overview");
}

/*!
 * Set the content of the combat overview window
 *
 * We only need to set the side 1 winning chances since the side2 chances are 100 - side1chances
 *    @param in_iSide1Soldiers: Winning chances of the side 1 soldiers ( 0 to 100)
 *    @param in_iSide1Soldiers: Winning chances of the side 1 land units ( 0 to 100)
 *    @param in_iSide1Soldiers: Winning chances of the side 1 land air units ( 0 to 100)
 *    @param in_iSide1Soldiers: Winning chances of the side 1 land naval units( 0 to 100)
 *    @param in_iSide1Soldiers: Winning chances of the side 1 overall ( 0 to 100)
 *    @return true if set successfully, false otherwise
 **/
const INT32 c_iFlagIconSizeX = 22;
const INT32 c_iFlagIconSizeY = 13;

bool GCombatOverview::Set(const UINT32             in_iSide1Soldiers,
                          const UINT32             in_iSide1Land,
                          const UINT32             in_iSide1Air,
                          const UINT32             in_iSide1Naval,
                          const UINT32             in_iSide1Overall,
                          const vector<UINT32>&    in_vSide1Actors,
                          const vector<UINT32>&    in_vSide2Actors)
{
   //Fetch the pointers to the bars
   GUI::GFrame* l_pSoldiersBar = (GUI::GFrame*)Child(FRAME_BORDER)->Child(FRAME_BALANCE)->Child(SOLDIERS_BAR);
   GUI::GFrame* l_pLandBar     = (GUI::GFrame*)Child(FRAME_BORDER)->Child(FRAME_BALANCE)->Child(LAND_BAR);
   GUI::GFrame* l_pAirBar      = (GUI::GFrame*)Child(FRAME_BORDER)->Child(FRAME_BALANCE)->Child(AIR_BAR);
   GUI::GFrame* l_pNavalBar    = (GUI::GFrame*)Child(FRAME_BORDER)->Child(FRAME_BALANCE)->Child(NAVAL_BAR);
   GUI::GFrame* l_pOverallBar  = (GUI::GFrame*)Child(FRAME_BORDER)->Child(FRAME_BALANCE)->Child(OVERALL_BAR);
   assert(l_pSoldiersBar && l_pLandBar && l_pAirBar && l_pNavalBar && l_pOverallBar);

   GUI::GFrame* l_pSide1 = NULL;
   GUI::GFrame* l_pSide2 = NULL;

   //Set the soldier overall bar
   {
      INT16 l_iSide1Size = INT16(in_iSide1Soldiers * l_pSoldiersBar->Width() / 100) ;
      l_pSide1 = (GUI::GFrame*)l_pSoldiersBar->Child(BLUE_BAR);
      assert(l_pSide1);
      l_pSide1->Position(0,0);
      l_pSide1->Width(l_iSide1Size);

      l_pSide2 = (GUI::GFrame*)l_pSoldiersBar->Child(RED_BAR);
      l_pSide2->Position(l_iSide1Size,0);
      l_pSide2->Width(l_pSoldiersBar->Width() - l_pSide1->Width());
   }

   //Set the land units overall bar
   {
      INT16 l_iSide1Size = INT16(in_iSide1Land * l_pLandBar->Width() / 100) ;
      l_pSide1 = (GUI::GFrame*)l_pLandBar->Child(BLUE_BAR);
      assert(l_pSide1);
      l_pSide1->Position(0,0);
      l_pSide1->Width(l_iSide1Size);

      l_pSide2 = (GUI::GFrame*)l_pLandBar->Child(RED_BAR);
      l_pSide2->Position(l_iSide1Size,0);
      l_pSide2->Width(l_pLandBar->Width() - l_pSide1->Width());
   }

   //Set the air units overall bar
   {
      INT16 l_iSide1Size = INT16(in_iSide1Air * l_pAirBar->Width() / 100) ;
      l_pSide1 = (GUI::GFrame*)l_pAirBar->Child(BLUE_BAR);
      assert(l_pSide1);
      l_pSide1->Position(0,0);
      l_pSide1->Width(l_iSide1Size);

      l_pSide2 = (GUI::GFrame*)l_pAirBar->Child(RED_BAR);
      l_pSide2->Position(l_iSide1Size,0);
      l_pSide2->Width(l_pAirBar->Width() - l_pSide1->Width());   
   }

   //Set the Naval units overall bar
   {
      INT16 l_iSide1Size = INT16(in_iSide1Naval * l_pNavalBar->Width() / 100) ;
      l_pSide1 = (GUI::GFrame*)l_pNavalBar->Child(BLUE_BAR);
      assert(l_pSide1);
      l_pSide1->Position(0,0);
      l_pSide1->Width(l_iSide1Size);

      l_pSide2 = (GUI::GFrame*)l_pNavalBar->Child(RED_BAR);
      l_pSide2->Position(l_iSide1Size,0);
      l_pSide2->Width(l_pNavalBar->Width() - l_pSide1->Width());   
   }

   //Set the Overall units overall bar
   {
      INT16 l_iSide1Size = INT16(in_iSide1Overall * l_pOverallBar->Width() / 100) ;
      l_pSide1 = (GUI::GFrame*)l_pOverallBar->Child(BLUE_BAR);
      assert(l_pSide1);
      l_pSide1->Position(0,0);
      l_pSide1->Width(l_iSide1Size);

      l_pSide2 = (GUI::GFrame*)l_pOverallBar->Child(RED_BAR);
      l_pSide2->Position(l_iSide1Size,0);
      l_pSide2->Width(l_pOverallBar->Width() - l_pSide1->Width());   
   }


   //Display the correct flags
   {
      GUI::GFrame* l_pFlagParent = (GUI::GFrame*)Child(FRAME_BORDER);
      GUI::GLabel* l_pVersusLbl  = (GUI::GLabel*)Child(FRAME_BORDER)->Child(VERSUS_BAR);
      UINT32 l_iTotalActors = in_vSide1Actors.size() + in_vSide2Actors.size();
      INT16  l_fPositionIncrement = (INT16)(l_pVersusLbl->Width()/l_iTotalActors);
      for(UINT32 i = 0 ; i < in_vSide1Actors.size() ; i++)
      {
         GString l_sFlag = g_ClientDAL.CountryFlagSmallPath(in_vSide1Actors[i]);
         //Create a small frame
         GUI::GFrame* l_pFrame = 
            CREATE_GUI_OBJECT_NAMED_VISIBLE(GFrame, GString(L"Flag" + GString(i)), l_pFlagParent);
         l_pFrame->Size(c_iFlagIconSizeX,c_iFlagIconSizeY);
         l_pFrame->Set_Background_Picture(l_sFlag,false);
         l_pFrame->Position((INT16)(l_pVersusLbl->Left()+l_fPositionIncrement*i),
                                l_pVersusLbl->Top());
      }
      for(UINT32 i = 0 ; i < in_vSide2Actors.size() ;i++)
      {
         GString l_sFlag = g_ClientDAL.CountryFlagSmallPath(in_vSide2Actors[i]);

         //Create a small frame
         GUI::GFrame* l_pFrame = CREATE_GUI_OBJECT_NAMED_VISIBLE(GFrame, GString(L"Flag" + GString(i)), l_pFlagParent);
         l_pFrame->Size(c_iFlagIconSizeX,c_iFlagIconSizeY);
         l_pFrame->Set_Background_Picture(l_sFlag,false);

         l_pFrame->Position((INT16)(l_pVersusLbl->Left()+l_pVersusLbl->Width()-c_iFlagIconSizeX-(l_fPositionIncrement*i)),
                                l_pVersusLbl->Top());         
      }
   }
   return true;
}