/**************************************************************
*
* sp2_strategic_target_multiple_countries_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_strategic_target_multiple_countries_window.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_strategic_launch_window.h"
#include "../include/sp2_strategic_missile_list_window.h"
#include "../include/sp2_strategic_select_target_window.h"
#include "../include/sp2_strategic_target_country_window.h"

const GString GStrategicTargetMultipleCountriesWindow::TypeName(L"Strategic_Target_Multiple_Countries_Window");

GUI::GBaseObject* SP2::GStrategicTargetMultipleCountriesWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicTargetMultipleCountriesWindow();
}


GStrategicTargetMultipleCountriesWindow::GStrategicTargetMultipleCountriesWindow() : GBaseWindow()
{
   m_bMeterChanged = false;
   m_bSkipModifyRow = false;
   m_bDeleteSelectedRow = false;
}

GStrategicTargetMultipleCountriesWindow::~GStrategicTargetMultipleCountriesWindow()
{
}

bool GStrategicTargetMultipleCountriesWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjLockTog->Hide();

   g_ClientDDL.StrategicTargetCountryWindow()->Hide();

   GUI::GFrame* l_pFrame = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmList");
   m_pObjPrepareLaunchBtn = (GUI::GButton*)l_pFrame->Child(L"btnPrepareLaunch");
   m_pObjAttackList       = (GUI::GListBox*)l_pFrame->Child(L"lstAttackList");
   m_pObjProjCasualities  = (GUI::GLabel*)l_pFrame->Child(L"txtCasualities");
   m_pObjMissileListBtn   = (GUI::GButton*)l_pFrame->Child(L"btnMissileList");

   for(INT32 i=100088; i<=100089; i++)
   {
      GString l_sAttackType = g_ClientDAL.GetString(i);
      m_vAttackType.push_back(l_sAttackType);
   }

	return true;
}

bool GStrategicTargetMultipleCountriesWindow::OnDestroy()
{
   return __super::OnDestroy();
}

void GStrategicTargetMultipleCountriesWindow::OnShow()
{
   if(!m_bSkipModifyRow)
      AddRow();

   __super::OnShow();
}

void GStrategicTargetMultipleCountriesWindow::OnHide()
{
   __super::OnHide();

   if(!m_bSkipModifyRow)
   {
      if(g_ClientDDL.StrategicTargetCountryWindow() && !g_ClientDDL.StrategicTargetCountryWindow()->Visible())
         g_ClientDDL.StrategicTargetCountryWindow()->Show();

      for(INT32 i=0; i<m_pObjAttackList->Get_Nb_Rows(); i++)
      {
         EraseRow(0);
      }
   }
}

GUI::EEventStatus::Enum GStrategicTargetMultipleCountriesWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   return __super::OnMouseDown(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GStrategicTargetMultipleCountriesWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjPrepareLaunchBtn)
         {
            INT32 l_iNbMissilesUsed = 0;
            //If the player didn't chose any missiles don't do anything
            for(INT32 i=0; i<(INT32)m_iNbMissileUsed.size(); i++)
            {
               l_iNbMissilesUsed += m_iNbMissileUsed[i];
            }

            if(l_iNbMissilesUsed <= 0)
               return GUI::EEventStatus::Handled;

            //Spawn strategic launch window and reassing targets
            g_ClientDDL.StrategicLaunchWindow()->Show();
            
            vector<GNuclearMissile> l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
            l_vWantedMissiles.insert(l_vWantedMissiles.begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().end());
            for(INT32 i=0; i<(INT32)m_TargetsMissilesList.size(); i++)
            {
               m_TargetsMissilesList[i] = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(l_vWantedMissiles,m_iTargetCountryId[i],m_TargetType[i],l_vWantedMissiles.size());
            }

            m_bSkipModifyRow = false;
            g_ClientDDL.StrategicLaunchWindow()->Initialize(false);
         }
         else if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.StrategicTargetCountryWindow()->Show();
            g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Hide();
            m_bSkipModifyRow = false;
         }
         else if(in_pCaller == m_pObjMissileListBtn)
         {
            g_ClientDDL.StrategicMissileListWindow()->Show();
            g_ClientDDL.StrategicMissileListWindow()->Initialize(false);
            this->Enabled(false);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

/*****************************************************
   AddRow()
   This function add and prepare a list item
   for the attack list, it also resize the data 
   vectors to be ready to receive data
*/
void GStrategicTargetMultipleCountriesWindow::AddRow()
{
   //Get new listbox row
   GUI::GListBox::GRow* l_pRow = m_pObjAttackList->Insert_New_Row();
   ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(30,30,30,255));
   
   //Push row data
   m_pObjCountry.push_back((GUI::GComboBox*)l_pRow->Object()->Child(L"cboCountry"));
   m_pObjAttackType.push_back((GUI::GComboBox*)l_pRow->Object()->Child(L"cboAttackType"));
   m_pObjAttackStrengthValue.push_back((SP2::GProgressMeterYellowValueSmall*)l_pRow->Object()->Child(L"frmPotentialTargets"));
   m_pObjAvailableMissiles.push_back((GUI::GLabel*)l_pRow->Object()->Child(L"txtAvailableMissiles"));
   m_pObjMissilesUsed.push_back((GUI::GLabel*)l_pRow->Object()->Child(L"txtMissileUsed"));

   //Get the index of the last added
   INT32 i = (INT32)m_pObjCountry.size() - 1;

   //resize the vectors that no value get pushed in
   m_iTargetCountryId.resize(i + 1);
   m_iNbMissileUsed.resize(i + 1);
   m_iNbMissileInRange.resize(i + 1);
   m_TargetType.resize(i + 1);
   m_TargetsMissilesList.resize(i + 1);
   m_NewTargetsMissilesList.resize(i + 1);
   m_TargetList.resize(i + 1);
   m_MissilesFull.resize(i + 1);
   m_MissilesUsed.resize(i + 1);
   
   //initialize the meter
   m_pObjAttackStrengthValue[i]->Enabled(false);
   m_pObjAttackStrengthValue[i]->CursorDragable()[0] = true;
   m_pObjAttackStrengthValue[i]->Minimum()[0] = 0;
   m_pObjAttackStrengthValue[i]->Minimum()[1] = 0;
   m_pObjAttackStrengthValue[i]->Maximum()[0] = 100;
   m_pObjAttackStrengthValue[i]->Maximum()[1] = 100;
   m_pObjAttackStrengthValue[i]->Value(0,0);
   m_pObjAttackStrengthValue[i]->MeterBtn()[0]->Width(0);
   m_pObjAttackStrengthValue[i]->Owner(this);
   
   //get our country id
   INT16 l_iControlledCountryID = g_ClientDAL.ControlledCountryID();
   
   GString l_sTemp;
   
   //Set combo box content to Select Location
   l_sTemp = g_ClientDAL.GetString(100287);
   m_pObjCountry[i]->Selected_Content(l_sTemp);
   
   //Add remove from list in combo
   l_sTemp = g_ClientDAL.GetString(100289);
   m_pObjCountry[i]->Add_Item(l_sTemp);

   //for each country
   for(INT32 j=1; j<=g_ClientDAL.NbCountry(); j++)
   {
      const GCountry& l_Country = g_ClientDAL.Country(j);
      //check if it's not our country
      if(j != l_iControlledCountryID)
      {
         //if it's the first pass, add Specific Location
         if(j == 1)
            m_pObjCountry[i]->Add_Item(g_ClientDAL.GetString(100087));

         //add the country name
         m_pObjCountry[i]->Add_Item(l_Country.Name());
      }
   }

   //set content of attack type combo box to Select attack type
   l_sTemp = g_ClientDAL.GetString(100288);
   m_pObjAttackType[i]->Selected_Content(l_sTemp);

   //add all attack type
   for(INT32 j=0; j<(INT32)m_vAttackType.size(); j++)
   {
      m_pObjAttackType[i]->Add_Item(m_vAttackType[j]);
   }

   //update attack list
   m_pObjAttackList->Update();
}

void GStrategicTargetMultipleCountriesWindow::EraseRow(INT16 in_iPosition)
{
   //initialize all the needed iterators to run through every vector to erase the good item
   vector<GUI::GComboBox*>::iterator l_It1 = m_pObjCountry.begin();
   vector<GUI::GComboBox*>::iterator l_It2 = m_pObjAttackType.begin();
   vector<SP2::GProgressMeterYellowValueSmall*>::iterator    l_It3 = m_pObjAttackStrengthValue.begin();
   vector<GUI::GLabel*>::iterator    l_It4 = m_pObjAvailableMissiles.begin();
   vector<GUI::GLabel*>::iterator    l_It5 = m_pObjMissilesUsed.begin();

   vector<INT32>::iterator                     l_It6 =  m_iTargetCountryId.begin();           
   vector<UINT32>::iterator                    l_It7 =  m_iNbMissileUsed.begin();
   vector<UINT32>::iterator                    l_It8 =  m_iNbMissileInRange.begin();
   vector<ENuclearTargetType::Enum>::iterator  l_It9 =  m_TargetType.begin();

   vector< vector< pair<GNukeTarget, vector<GNuclearMissile> > > >::iterator	l_It10 = m_TargetsMissilesList.begin();
   vector< vector< pair<GNukeTarget, vector<GNuclearMissile> > > >::iterator  l_It11 = m_NewTargetsMissilesList.begin();
   vector< multimap<UINT32,GNukeTarget> >::iterator					   l_It12 = m_TargetList.begin();
   vector< vector<GNuclearMissile> >::iterator						      l_It13 = m_MissilesFull.begin();
   vector< vector<GNuclearMissile> >::iterator						      l_It14 = m_MissilesUsed.begin();

   //Iterate for each row before the wanted position
   l_It1 += in_iPosition;
   l_It2 += in_iPosition;
   l_It3 += in_iPosition;
   l_It4 += in_iPosition;
   l_It5 += in_iPosition;
   l_It6 += in_iPosition;
   l_It7 += in_iPosition;
   l_It8 += in_iPosition;
   l_It9 += in_iPosition;
   l_It10 += in_iPosition;
   l_It11 += in_iPosition;
   l_It12 += in_iPosition;
   l_It13 += in_iPosition;
   l_It14 += in_iPosition;

   //Erase every value related to the row we are currently removing
   m_pObjCountry.erase(l_It1);
   m_pObjAttackType.erase(l_It2);
   m_pObjAttackStrengthValue.erase(l_It3);
   m_pObjAvailableMissiles.erase(l_It4);
   m_pObjMissilesUsed.erase(l_It5);
   
   m_iTargetCountryId.erase(l_It6); 
   m_iNbMissileUsed.erase(l_It7);
   m_iNbMissileInRange.erase(l_It8);
   m_TargetType.erase(l_It9);
   m_TargetsMissilesList.erase(l_It10);
   m_NewTargetsMissilesList.erase(l_It11);
   m_TargetList.erase(l_It12);
   m_MissilesFull.erase(l_It13);
   m_MissilesUsed.erase(l_It14);

   //Remove the row itself
   m_pObjAttackList->RemoveRow(in_iPosition);

   //Update the list box
   m_pObjAttackList->Update();
}

GUI::EEventStatus::Enum GStrategicTargetMultipleCountriesWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(m_bDeleteSelectedRow)
      {
         EraseRow((INT16)m_pObjAttackList->Get_Selected_Item()[0]);
         m_bDeleteSelectedRow = false;
      }

      if(m_bMeterChanged)
      {
         INT16 l_ID = (INT16)m_pObjAttackList->Get_Selected_Item()[0];

         /*---------------------------------------------*/
         //From the full list of missile generate the new target & missiles list     
         m_NewTargetsMissilesList[l_ID] = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(m_MissilesFull[l_ID],m_iTargetCountryId[l_ID],m_TargetType[l_ID],m_iNbMissileUsed[l_ID]);
         
         //From that newly acquired list get the list of missiles used for that attack
         m_MissilesUsed[l_ID] = GenerateMissilesUsedList(m_NewTargetsMissilesList[l_ID]);

         m_bMeterChanged = false;
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicTargetMultipleCountriesWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   //Set the default handler
   switch(in_EventId)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:
   case c_OnMeterChangeEvent:
      break;
   default:
      return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);
   }

   //Get the selected item in the attack list box
   INT16 l_ID = (INT16)m_pObjAttackList->Get_Selected_Item()[0];

   switch(in_EventId)
   {
   // On Meter Change event
   case c_OnMeterChangeEvent:
      {
         //Set the text of the meter to actual value
         ((SP2::GProgressMeterYellowValue*)in_pCaller)->TextValueObj()->Text(GString::FormatNumber(((GOnMeterChangeData*)in_EventData.Data)->m_fBtnValue,L"",L".",L"",L" %",0,1));

         //From the meter value calculate the amount of missiles used and set it to it's textbox
         m_iNbMissileUsed[l_ID] = (UINT32)((m_pObjAttackStrengthValue[l_ID]->Value(0) / 100) * m_iNbMissileInRange[l_ID]);
         m_pObjMissilesUsed[l_ID]->Text(GString(m_iNbMissileUsed[l_ID]));
         
         m_bMeterChanged = true;
      }
      break;

   // On Combo Box selection event
   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         //Get pointer to combo box in event data
         GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
         
         //if the combo is the country combo
         if(l_pCombo == m_pObjCountry[l_ID])
         {
            //If the combo text is Remove from list
            if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100289))
            {
               //Erase the row and quit the event handle
               if(m_pObjAttackList->Get_Nb_Rows() > 1)
                  m_bDeleteSelectedRow = true;

               break;
            }
            else
            {
               bool l_bFound = false;
               
               //get Select location string
               GString l_sCheck = g_ClientDAL.GetString(100287);

               //for each row in the list item
               for(INT32 i=0; i<m_pObjAttackList->Get_Nb_Rows(); i++)
               {
                  //get cboCountry text
                  GString l_sCboContent = ((GUI::GComboBox*)m_pObjAttackList->Get_Row_At(i)->Object()->Child(L"cboCountry"))->Selected_Content();
                  
                  //if the text is Specific location, there is still one free to choose so do not add rows
                  if(l_sCheck == l_sCboContent)
                  {
                     l_bFound = true;
                     break;
                  }
               }

               //if no one was found add the row
               if(!l_bFound && l_pCombo->Selected_Content() != g_ClientDAL.GetString(100087))
                  AddRow();
            }

            //if we choose a specific target
            if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100087))
            {
               //Spawn select target window
               g_ClientDDL.StrategicSelectTargetWindow()->Show();

               l_pCombo->Selected_Content(g_ClientDAL.GetString(100287));

               //Hide this current window
               m_bSkipModifyRow = true;
               Hide();

               //Set the background earth to forward the next click to this window
               g_ClientDDL.BackgroundEarth()->m_ReturnClickedPosition = (ReturnClickedPosition)&SP2::GStrategicTargetMultipleCountriesWindow::OnClickFromEarth;
               g_ClientDDL.BackgroundEarth()->m_pReturnWindow = this;
               
               //Disable the attack type combo
               m_pObjAttackType[l_ID]->Enabled(false);
            }
            else
            {
               //Set attack type combo content to Select Attack type
               m_pObjAttackType[l_ID]->Selected_Content(g_ClientDAL.GetString(100288));
               
               //Enable attack type combo
               m_pObjAttackType[l_ID]->Enabled(true);
            }
         }

         //if the combo is the attack type combo
         if(l_pCombo == m_pObjAttackType[l_ID])
         {
            //If the player chose Country-wide(military)
            if(m_pObjAttackType[l_ID]->Selected_Content() == g_ClientDAL.GetString(100088))
            {
               m_TargetType[l_ID] = SP2::ENuclearTargetType::Military;
            }

            //If the player chose Country-wide(civilian)
            if(m_pObjAttackType[l_ID]->Selected_Content() == g_ClientDAL.GetString(100089))
            {
               m_TargetType[l_ID] = SP2::ENuclearTargetType::Civil;
            }


            //Get the selected country
            const GCountry& l_Country = g_ClientDAL.Country(m_pObjCountry[l_ID]->Selected_Content() );
            m_iTargetCountryId[l_ID] = (INT32)l_Country.Id();
            
            //Clear and fill the target list
            m_TargetList[l_ID].clear();            
            

            vector<GNuclearMissile> l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
            l_vWantedMissiles.insert(l_vWantedMissiles.begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().end());
            //Clear and fill the list of assigned missiles per target
            m_TargetsMissilesList[l_ID].clear();
            m_TargetsMissilesList[l_ID] = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(l_vWantedMissiles,m_iTargetCountryId[l_ID],m_TargetType[l_ID],l_vWantedMissiles.size());

            //Clear and fill the vector of missiles used
            m_MissilesFull[l_ID].clear();
            m_MissilesFull[l_ID] = GenerateMissilesUsedList(m_TargetsMissilesList[l_ID]);
            m_MissilesUsed[l_ID].clear();
   
            UINT32 l_iCasualities = 0;
            //REAL32 l_fCasualitiesValue = 0.f;
            UINT32 l_iNbMissiles = m_MissilesFull[l_ID].size();

            //Set the qty of available missiles (missiles in range available)
            m_pObjAvailableMissiles[l_ID]->Text(GString(l_iNbMissiles));
            m_iNbMissileInRange[l_ID] = l_iNbMissiles;

            //If the nb of missiles in range is greater than 0 enable attack strength meter otherwise make sure it's disabled
            if(m_iNbMissileInRange[l_ID] > 0)
            {
               m_pObjAttackStrengthValue[l_ID]->Enabled(true);
               m_pObjAttackStrengthValue[l_ID]->Value(0,0);
               m_pObjAttackStrengthValue[l_ID]->TextValueObj()->Text(L"0%");
               m_iNbMissileUsed[l_ID] = 0;
               m_pObjMissilesUsed[l_ID]->Text(L"0");
            }
            else
            {
               m_pObjAttackStrengthValue[l_ID]->Enabled(false);
               
               //set text to N/A
               m_pObjAttackStrengthValue[l_ID]->Value(0,0);
               m_pObjAttackStrengthValue[l_ID]->TextValueObj()->Text(g_ClientDAL.GetString(100106));
               m_iNbMissileUsed[l_ID] = 0;
               m_pObjMissilesUsed[l_ID]->Text(L"");
            }

            //Calculate the casualities for the amount of missiles used
            for(INT32 i=0; i<(INT32)m_TargetsMissilesList[l_ID].size(); i++)
            {
               for(INT32 j=0; j<(INT32)m_TargetsMissilesList[l_ID][i].second.size(); j++)
               {                 
                  l_iCasualities += g_ClientDCL.EstimateNuclearCasualties(m_TargetsMissilesList[l_ID][i].first,m_TargetsMissilesList[l_ID][i].second[j]);
               }
            }
            m_pObjProjCasualities->Text(GString(l_iCasualities));

         }
      }
      
      break;
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicTargetMultipleCountriesWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return __super::OnMouseMove(in_EventData, in_pCaller);
}

vector<GNuclearMissile> GStrategicTargetMultipleCountriesWindow::GenerateMissilesUsedList(const vector< pair<GNukeTarget, vector<GNuclearMissile> > >& in_TargetsMissilesList)
{
   vector<GNuclearMissile> l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
   l_vWantedMissiles.insert(l_vWantedMissiles.begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().end());
   vector<GNuclearMissile> l_vUsedMissileList;

   for(UINT32 i = 0 ; i < in_TargetsMissilesList.size() ; i++)
   {
      for(UINT32 j = 0 ; j < in_TargetsMissilesList[i].second.size() ; j++)
      {
         l_vUsedMissileList.push_back(in_TargetsMissilesList[i].second[j]);      
      }
   }

   return l_vUsedMissileList;
}

void GStrategicTargetMultipleCountriesWindow::OnClickFromEarth(const GVector2D<REAL32> in_Position)
{
   //Show this window
   Show();
   m_bSkipModifyRow = false;

   //Kill select target window
   g_ClientDDL.StrategicSelectTargetWindow()->Hide();
   
   //Set the background earth return window to null so it don't forward events here
   g_ClientDDL.BackgroundEarth()->m_pReturnWindow = NULL;
   
   //get the current selected item id
   INT16 l_ID = (INT16)m_pObjAttackList->Get_Selected_Item()[0];
   
   //set attack type combo text to "Location: (Long., Lat.)"
   m_pObjAttackType[l_ID]->Selected_Content(L"Location: (" + GString::FormatNumber(in_Position.x,2) + L", " + GString::FormatNumber(in_Position.y,2) + L")");

   //Get the region id where we clicked
   UINT32 l_iRegionID      = g_ClientDCL.EarthLocateRegion(in_Position.x,in_Position.y);

   //If the selected area is invalid
   if(l_iRegionID == 0) 
   {
      //Set country text to N/A
      m_pObjCountry[l_ID]->Selected_Content(g_ClientDAL.GetString(100106));

      //Set target country to null value
      m_iTargetCountryId[l_ID] = 0;

      //disable attack type combo box
      m_pObjAttackType[l_ID]->Enabled(false);
   }
   else
   {
      //Get the owner of the country and set country as text
      UINT32 l_iCountryID = g_ClientDAL.RegionControl(l_iRegionID).m_iMilitary;
      if(l_iCountryID != 0)
      {
         const GCountry& l_Country = g_ClientDAL.Country(l_iCountryID);
         m_pObjCountry[l_ID]->Selected_Content(l_Country.Name());
         
         //Set the targeted country
         m_iTargetCountryId[l_ID] = (INT32)l_Country.Id(); 

         //Enable attack type combo box
         m_pObjAttackType[l_ID]->Enabled(true);
      }
   }

   //Create target to assign nuke to that target
   GNukeTarget l_Target;

   //set the target type
   l_Target.m_TargetType = SP2::ENuclearTargetType::Specific;
   l_Target.m_iTargetCountryID = m_iTargetCountryId[l_ID];
   m_TargetType[l_ID] = SP2::ENuclearTargetType::Specific;

   //Verify if these are truly mixed up
   l_Target.m_Position.x = in_Position.x;
   l_Target.m_Position.y = in_Position.y;

   //clear the target list and insert the previously made target
   m_TargetList[l_ID].clear();
   m_TargetList[l_ID].insert(multimap<UINT32,GNukeTarget>::value_type(1,l_Target));
                  
   //assign the nukes to the target
   vector<GNuclearMissile> l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
   l_vWantedMissiles.insert(l_vWantedMissiles.begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().end());
   m_TargetsMissilesList[l_ID] = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(l_vWantedMissiles,m_iTargetCountryId[l_ID],m_TargetType[l_ID],l_vWantedMissiles.size());

   //Clear and fill the vector of missiles used
   m_MissilesFull[l_ID].clear();
   m_MissilesFull[l_ID] = GenerateMissilesUsedList(m_TargetsMissilesList[l_ID]);
   m_MissilesUsed[l_ID].clear();

   INT32 l_iNbMissiles = m_MissilesFull[l_ID].size();

   m_pObjAvailableMissiles[l_ID]->Text(GString(l_iNbMissiles));
   m_iNbMissileInRange[l_ID] = l_iNbMissiles;

   m_pObjAttackStrengthValue[l_ID]->Enabled(false);
   m_pObjAttackStrengthValue[l_ID]->TextValueObj()->Text(g_ClientDAL.GetString(100106));

   //If the nb of missiles in range is greater than 0 enable attack cursor
   if((INT32)m_TargetsMissilesList.size() > 0)
   {
      for(INT32 i=0; i<(INT32)m_TargetsMissilesList[l_ID].size(); i++)
      {
         if(m_TargetsMissilesList[l_ID][i].second.size() > 0)
         {
            m_pObjAttackStrengthValue[l_ID]->Enabled(true);
            m_pObjAttackStrengthValue[l_ID]->TextValueObj()->Text(L"0%");
            break;
         }
      }
   }

   m_pObjProjCasualities->Text(g_ClientDAL.GetString(100106));
   AddRow();
}