/**************************************************************
*
* sp2_strategic_missile_list_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_strategic_missile_list_window.h"

#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_strategic_target_country_window.h"
#include "../include/sp2_strategic_target_multiple_countries_window.h"

const GString GStrategicMissileListWindow::TypeName(L"Strategic_Missile_List_Window");

GUI::GBaseObject* SP2::GStrategicMissileListWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicMissileListWindow();
}


GStrategicMissileListWindow::GStrategicMissileListWindow() : GBaseWindow()
{
   m_bFromTargetSingleCountry = false;
   m_iInternalID = 0;
}

GStrategicMissileListWindow::~GStrategicMissileListWindow()
{
}

bool GStrategicMissileListWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjMissileLst     = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmList")->Child(L"lstMissileList");
   m_pObjSelectAllBtn   = (GUI::GButton*)Child(L"frmInner")->Child(L"btnSelectAll");
   m_pObjSelectNoneBtn  = (GUI::GButton*)Child(L"frmInner")->Child(L"btnSelectNone");
   m_pObjConfirmBtn     = (GUI::GButton*)Child(L"frmInner")->Child(L"btnConfirm");

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

	return true;
}

void GStrategicMissileListWindow::Initialize(bool in_bFromTargetSingleCountry)
{
   m_bFromTargetSingleCountry = in_bFromTargetSingleCountry;
   
   vector<GNuclearMissile>* l_pMissilesInRange;
   vector<GNuclearMissile>* l_pMissilesUsed;

   //if the caller is the target single country
   if(m_bFromTargetSingleCountry)
   {
      //get a pointer to missiles vectors
      l_pMissilesInRange = &(g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesFull);
      l_pMissilesUsed    = &(g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed);
   }
   else
   {
      //get the current selected item id
      const vector<INT32>& l_viSel = g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_pObjAttackList->Get_Selected_Item();
      if(l_viSel.size())
         m_iInternalID = l_viSel[0];
      else
         gassert(l_viSel.size(), "There was no selected missile");

      //get a pointer to missiles vectors
      l_pMissilesInRange = &(g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesFull[m_iInternalID]);
      l_pMissilesUsed    = &(g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesUsed[m_iInternalID]);
   }

   multimap<REAL32, pair<UINT32,UINT32> > l_MissilesUsed;      

   INT32 i, j, l_iNbAdded;
   l_iNbAdded = 0;
   GUI::GListBox::GRow* l_pRow = NULL;
   
   m_pObjMissileLst->Clear();
   
   //for each missile in range
   for(i=0; i<(INT32)l_pMissilesInRange->size(); i++)
   {
      //get the missile design
      SDK::Combat::Design::GUnit* l_pDesign = g_Joshua.UnitManager().UnitDesign(l_pMissilesInRange->at(i).m_iDesignID);

      //create the new row and set is properties
      l_pRow = m_pObjMissileLst->Insert_New_Row();
      ((GUI::GLabel*)l_pRow->Object()->Child(L"lblMissileName"))->Text(l_pDesign->Name());
      ((GUI::GLabel*)l_pRow->Object()->Child(L"lblYield"))->Text(GString(l_pMissilesInRange->at(i).Megatons()));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"lblLocation"))->Text(L"Long: " + GString(GString::FormatNumber(l_pMissilesInRange->at(i).Position().x,2)) + L", Lat:" + GString(GString::FormatNumber(l_pMissilesInRange->at(i).Position().y,2)));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"lblTarget"))->Text(g_ClientDAL.GetString(100106));
      ((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState(false);
      ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(30,30,30,255));

      //push the missile range, id and the index for further use
      l_MissilesUsed.insert( multimap<REAL32, pair<UINT32,UINT32> >::value_type(l_pMissilesInRange->at(i).RangeKMs(), pair<UINT32,UINT32>(l_pMissilesInRange->at(i).m_iID,l_iNbAdded)));
      l_iNbAdded++;
   }

   //if there are any missiles used
   if(l_pMissilesUsed->size() > 0)
   {
      //for each missile used
      for(i=0; i<(INT32)l_pMissilesUsed->size(); i++)
      {
         //find the data corresponding to the missile to know if it's activated or not
         multimap<REAL32, pair<UINT32,UINT32> >::iterator l_It = l_MissilesUsed.begin();
         for(j=0; j<(INT32)l_MissilesUsed.size(); j++)
         {
            if(l_It->second.first == l_pMissilesUsed->at(i).m_iID)
            {
               GUI::GListBox::GRow* l_pRow = m_pObjMissileLst->Get_Row_At(l_It->second.second);
               ((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState(true);
            }

            l_It++;
         }   
      }
   }/*
   else
   {
      INT32 l_iNbIt = 0;
      if(m_bFromTargetSingleCountry)
      {
         l_iNbIt = g_ClientDDL.StrategicTargetCountryWindow()->m_iNbMissileUsed;
      }
      else
      {

      }

      for(i=0; i<l_iNbIt; i++)
      {
         //find the data corresponding to the missile to know if it's activated or not
         multimap<REAL32, pair<UINT32,UINT32> >::iterator l_It = l_MissilesUsed.begin();
         GUI::GListBox::GRow* l_pRow = m_pObjMissileLst->Get_Row_At(l_It->second.second);
         ((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState(true);

         l_It++;
      }
   }*/

   m_pObjMissileLst->Update();
}

void GStrategicMissileListWindow::OnShow()
{
   __super::OnShow();
}

GUI::EEventStatus::Enum GStrategicMissileListWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller->Name() == L"togSelect")
         {
            
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicMissileListWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.StrategicMissileListWindow()->Hide();

            if(m_bFromTargetSingleCountry)
               g_ClientDDL.StrategicTargetCountryWindow()->Enabled(true);
            else
               g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Enabled(true);
         }
         //on select all button
         else if(in_pCaller == m_pObjSelectAllBtn)
         {
            //for each row toggle it
            g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed.clear();

            for(INT32 i=0; i<m_pObjMissileLst->Get_Nb_Rows(); i++)
            {
               GUI::GListBox::GRow* l_pRow = m_pObjMissileLst->Get_Row_At(i);
               
               if(!((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState())
                  ((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState(true);
            }
         }
         //on select none btn
         else if(in_pCaller == m_pObjSelectNoneBtn)
         {
            //for each row untoggle it
            for(INT32 i=0; i<m_pObjMissileLst->Get_Nb_Rows(); i++)
            {
               GUI::GListBox::GRow* l_pRow = m_pObjMissileLst->Get_Row_At(i);
               
               if(((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState())
                  ((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState(false);
            }
         }
         //on confirm button
         else if(in_pCaller == m_pObjConfirmBtn)
         {
            //update the missile count, kill the window and enable target country windows
            UpdateMissileCount();
            g_ClientDDL.StrategicMissileListWindow()->Hide();

            if(g_ClientDDL.StrategicTargetCountryWindow())
               g_ClientDDL.StrategicTargetCountryWindow()->Enabled(true);
            else
               g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Enabled(true);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicMissileListWindow::On_List_Items_Selected(GUI::GListBox* in_pList)
{
   return GUI::EEventStatus::Handled;
}

//! Update the quantity of missile used 
void GStrategicMissileListWindow::UpdateMissileUsed()
{
   vector<GNuclearMissile>* l_pMissilesInRange;

   if(m_bFromTargetSingleCountry)
   {
      l_pMissilesInRange = &(g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesFull);
      g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed.clear();
   }
   else
   {
      l_pMissilesInRange = &(g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesFull[m_iInternalID]);
      g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesUsed[m_iInternalID].clear();
   }

   for(INT32 i=0; i<m_pObjMissileLst->Get_Nb_Rows(); i++)
   {
      GUI::GListBox::GRow* l_pRow = m_pObjMissileLst->Get_Row_At(i);
      if(((GUI::GToggle*)l_pRow->Object()->Child(L"togSelect"))->ToggleState())
      {
         if(m_bFromTargetSingleCountry)
            g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed.push_back(l_pMissilesInRange->at(i));
         else
            g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesUsed[m_iInternalID].push_back(l_pMissilesInRange->at(i));
      }
   }
}

void GStrategicMissileListWindow::UpdateMissileCount()
{
   UpdateMissileUsed();
   
   INT32 l_iMissileCount = 0;
   INT32 l_iMissileMax   = 0;

   if(m_bFromTargetSingleCountry)
   {
      l_iMissileCount = (INT32)g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesUsed.size();
      l_iMissileMax = (INT32)g_ClientDDL.StrategicTargetCountryWindow()->m_MissilesFull.size();
      if(l_iMissileMax > 0)
         g_ClientDDL.StrategicTargetCountryWindow()->m_pObjAttackCursor->Value(ceil(((REAL32)l_iMissileCount / (REAL32)l_iMissileMax) * 100),0,true);
      else
         g_ClientDDL.StrategicTargetCountryWindow()->m_pObjAttackCursor->Value(0,0,true);

   }
   else
   {
      l_iMissileCount = (INT32)g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesUsed[m_iInternalID].size();
      l_iMissileMax = (INT32)g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_MissilesFull[m_iInternalID].size();
      if(l_iMissileMax > 0)
      {
         g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_pObjAttackStrengthValue[m_iInternalID]->Value(ceil(((REAL32)l_iMissileCount / (REAL32)l_iMissileMax) * 100),0,true);
      }
      else
      {
         g_ClientDDL.StrategicTargetMultipleCountriesWindow()->m_pObjAttackStrengthValue[m_iInternalID]->Value(0,0,true);
      }
   }

}


