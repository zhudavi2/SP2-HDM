/**************************************************************
*
* sp2_strategic_target_country_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_strategic_target_country_window.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_strategic_launch_window.h"
#include "../include/sp2_strategic_missile_list_window.h"
#include "../include/sp2_strategic_select_target_window.h"

const GString GStrategicTargetCountryWindow::TypeName(L"Strategic_Target_Country_Window");

GUI::GBaseObject* SP2::GStrategicTargetCountryWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicTargetCountryWindow();
}


GStrategicTargetCountryWindow::GStrategicTargetCountryWindow() : GFrame()
{
   m_iNbMissileInRange   = 0;
   m_iNbMissileUsed      = 0;
   m_bMeterChanged       = false;
   m_pSlidingTrack       = NULL;
   m_bDragging           = false;
   m_bSpecificTargetMode = false;
	m_bProcessOfChoosingSpecificTarget = false;
   m_TargetType          = ENuclearTargetType::Undefined;
   m_pObjSuggestedBtn    = NULL;
   m_pObjMissileTypeCbo  = NULL;
   m_eMissileTypeToUse   = ENuclearMissileType::None;

   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Strategic Target Country Window");
}

GStrategicTargetCountryWindow::~GStrategicTargetCountryWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Strategic Target Country Window");
}

bool GStrategicTargetCountryWindow::OnCreate()
{
	__super::OnCreate();

   m_bSpecificTargetMode = false;
	m_bProcessOfChoosingSpecificTarget = false;

   //Init frequently used objects
   {
      GUI::GFrame* l_pFrmAttackStrength= (GUI::GFrame*)              Child("frmAttackStrength");
      m_pObjSuggestedBtn               = (GUI::GButton*)             l_pFrmAttackStrength->Child(L"btnSuggested");
      m_pObjIncreaseMissileUseBtn      = (GUI::GButton*)             l_pFrmAttackStrength->Child(L"btnArrowIncrease1");
      m_pObjDecreaseMissileUseBtn      = (GUI::GButton*)             l_pFrmAttackStrength->Child(L"btnArrowDecrease1");
      m_pObjAvailableMissiles          = (GUI::GLabel*)              l_pFrmAttackStrength->Child(L"txtAvailMissiles");
      m_pObjMissilesUsed               = (GUI::GLabel*)              l_pFrmAttackStrength->Child(L"txtMissilesUsed");
      m_pObjProjCasualities            = (GUI::GLabel*)              l_pFrmAttackStrength->Child(L"txtNbTargets");
      m_pObjMissileTypeCbo             = (GUI::GComboBox*)           l_pFrmAttackStrength->Child(L"cboMissileType");
      m_pObjAttackCursor               = (GProgressMeterYellowValue*)l_pFrmAttackStrength->Child("frmAttackStrengthValue");
      m_pObjPrepareLaunchBtn           = (GUI::GButton*)             Child(L"btnPrepare");
      m_pObjReturnBtn                  = (GUI::GButton*)             Child(L"btnReturn");
      m_pObjCountry                    = (GComboBox*)                Child("frmCountry")->Child(L"cboCountry");
      m_pObjAttackType                 = (GComboBox*)                Child("frmAttackType")->Child(L"cboAttackType");
      m_pObjAttackStrengthValue        = m_pObjAttackCursor->TextValueObj();
   }

   //Preset the missile type combo box (icbm, submarine based, both)
   {
      m_pObjMissileTypeCbo->Add_Item(g_ClientDAL.GetString(100698));
      m_pObjMissileTypeCbo->Add_Item(g_ClientDAL.GetString(100699));
      m_pObjMissileTypeCbo->Add_Item(g_ClientDAL.GetString(100700));
      m_pObjMissileTypeCbo->Enabled(false);
   }

   //Initialize the Attack cursor
   {
      m_pObjAttackCursor->CursorDragable()[0] = true;
      m_pObjAttackCursor->Enabled(false);
      m_pObjAttackCursor->Minimum()[0] = 0;
      m_pObjAttackCursor->Minimum()[1] = 0;
      m_pObjAttackCursor->Maximum()[0] = 100;
      m_pObjAttackCursor->Maximum()[1] = 100;
      m_pObjAttackCursor->Value(0,0);
      m_pObjAttackCursor->MeterBtn()[0]->Width(0);
      m_pObjAttackCursor->Owner(this);
      m_pObjAttackCursor->Enabled(false);
   }

   //Populate country list without adding your country if it's the first pass before adding an item add specific target
   {
      m_pObjCountry->Add_Item(g_ClientDAL.GetString(100087));
      g_ClientDDL.ComboboxSetCountries(g_ClientDAL.Countries(), m_pObjCountry);
   }
 
   //Preset the Attack Type combo box
   {
      for(INT32 i=100088; i<=100089; i++)
      {
         GString l_sAttackType = g_ClientDAL.GetString(i);
         m_pObjAttackType->Add_Item(l_sAttackType);
         m_vAttackType.push_back(l_sAttackType);
      }
      m_pObjAttackType->Update();
   }

   //Position the window at the bottom left
   {
      CGFX_Renderer_Init_Mode mode;
      g_Joshua.Renderer()->Get_Current_Mode(&mode);
      Position(0,(INT16)mode.m_Resolution.y - Height());
   }

   m_pSlidingTrack = g_ClientDDL.AnimationCreateSliding(0.5f, 0.51f, Top() + Height(), Top(), Model()->Get_Transform());

   Initialize();
	return true;
}

void GStrategicTargetCountryWindow::Initialize()
{
   m_eMissileTypeToUse = ENuclearMissileType::None;
   m_iTargetCountryId  = 0;
   m_pObjMissileTypeCbo->Selected_Content(g_ClientDAL.GetString(100697));
   m_pObjMissileTypeCbo->Enabled(false);
   m_pObjCountry->Selected_Content(g_ClientDAL.GetString(100287));   
   m_pObjAttackStrengthValue->Text(g_ClientDAL.GetString(100106));
   m_pObjAttackType->Enabled(false);
   m_pObjAttackCursor->Value(0,0);
   m_pObjAttackCursor->Enabled(false);
   m_iNbMissileUsed                    = 0;
   m_iNbMissileInRange                 = 0;
   m_NewTargetsMissilesList.clear();
   m_MissilesUsed.clear();
   m_pObjMissilesUsed->Text(L"");
   m_pObjAvailableMissiles->Text(L"");
   m_pObjProjCasualities->Text(L"");
   m_pObjAttackType->Selected_Content(g_ClientDAL.GetString(100288));
}

void GStrategicTargetCountryWindow::OnShow()
{
   __super::OnShow();
}

GUI::EEventStatus::Enum GStrategicTargetCountryWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      m_bDragging = true;

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicTargetCountryWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      //On target multiple country button
      //if(in_pCaller == m_pObjTargetMultipleBtn)
      //{
      //   Hide();
      //   g_ClientDDL.StrategicTargetMultipleCountriesWindow()->Show();
      //}

      //On missile list button (check if the window is not already opened
      //if(in_pCaller == m_pObjMissileListBtn && !g_ClientDDL.StrategicMissileListWindow()->Visible())
      //{
      //   g_ClientDDL.StrategicMissileListWindow()->Show();
      //   g_ClientDDL.StrategicMissileListWindow()->Initialize(true);
      //   g_ClientDDL.AnimationPlayRectangle(m_pObjMissileListBtn, g_ClientDDL.StrategicMissileListWindow());
      //   g_ClientDDL.TimedVisibilityUpdate(g_ClientDDL.StrategicMissileListWindow(), true, 0.5f);         
      //   this->Enabled(false);
      //}

      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //Increase missile list button
         if(in_pCaller == m_pObjIncreaseMissileUseBtn)
         {
            if(m_iNbMissileUsed < m_MissilesFull.size() && m_MissilesFull.size())
            {
               m_iNbMissileUsed++;                       
               OnNbMissilesUsedChange();
            }
         }
         //Decrease missile list button
         else if(in_pCaller == m_pObjDecreaseMissileUseBtn)
         {
            if(m_iNbMissileUsed > 0 && m_MissilesFull.size())
            {
               m_iNbMissileUsed--;
               m_iNbMissileUsed = min(m_iNbMissileUsed,m_MissilesFull.size());               
               OnNbMissilesUsedChange();
            }
         }
         //On prepare launch button
         else if(in_pCaller == m_pObjPrepareLaunchBtn)
         {
            if(m_eMissileTypeToUse == ENuclearMissileType::None)
               return GUI::EEventStatus::Handled;

            //If the player didn't chose any missiles don't do anything
            if(m_iNbMissileUsed <= 0)
               return GUI::EEventStatus::Handled;

            //Spawn strategic launch window and reassing targets
            if(!g_ClientDDL.StrategicLaunchWindow())
               g_ClientDDL.StrategicLaunchWindowSpawn();

            g_ClientDDL.StrategicLaunchWindow()->Show();

            //If we are not in specific target mode
            if(!m_bSpecificTargetMode)
               m_TargetsMissilesList = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(m_MissilesFull, m_iTargetCountryId,m_TargetType,m_iNbMissileUsed,false);

            g_ClientDDL.StrategicLaunchWindow()->Initialize(true);
         }
         //On return to main button
         else if(in_pCaller == m_pObjReturnBtn)
            g_ClientDDL.SwapToMilitary();
         else if(in_pCaller == m_pObjSuggestedBtn)
         {
            // If we want a suggestion, change target type to military if we were at a Specific location.
            if(m_TargetType == ENuclearTargetType::Specific || m_TargetType == ENuclearTargetType::Undefined )
               m_TargetType = ENuclearTargetType::Military;

            if(m_eMissileTypeToUse != ENuclearMissileType::None)
            {
               FillWindowAndMeters();
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicTargetCountryWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      //If the cursor is dragged update the value of the cursor
      if((in_pCaller->Name() == L"btnFill" || in_pCaller->Name() == L"btnUnfilled") && m_bDragging)
      {
         REAL32  l_fValue = (REAL32) m_pObjAttackCursor->Value(0);
         GString l_sValue = GString::FormatNumber((REAL64)l_fValue,L"",L"",L"",L"%",3,0);
         m_pObjAttackStrengthValue->Text(l_sValue);
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GStrategicTargetCountryWindow::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      m_bDragging = false;

      if(m_bMeterChanged)
      {
         //Process the nukes and assign them
         OnNbMissilesUsedChange();
         m_bMeterChanged = false;
      }
   }

   return GUI::EEventStatus::Handled;
}

bool GStrategicTargetCountryWindow::SpecificTargetOn() const
{	
	return m_bProcessOfChoosingSpecificTarget;	
}

void GStrategicTargetCountryWindow::ChangeSelectedCountryToNuke(UINT32 in_iCountryID)
{
	if(m_pObjCountry && in_iCountryID)
	{
		const GCountry& l_Country = g_ClientDAL.Country(in_iCountryID);
		m_iTargetCountryId = in_iCountryID;
		m_pObjCountry->Selected_Content(l_Country.Name());
		m_bSpecificTargetMode = false;
		m_bProcessOfChoosingSpecificTarget = false;
      m_pObjAttackType->Selected_Content(g_ClientDAL.GetString(100288));
      m_pObjAttackType->Enabled(true);
      m_pObjSuggestedBtn->Enabled(true);
	}
}

GUI::EEventStatus::Enum GStrategicTargetCountryWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch(in_EventId)
   {
      // On Meter Change event
      case c_OnMeterChangeEvent:
      {
         if(m_pObjAttackCursor->Enabled())
         {
            //Update the nb of missiles used
            ((SP2::GProgressMeterYellowValue*)in_pCaller)->TextValueObj()->Text(GString::FormatNumber(((GOnMeterChangeData*)in_EventData.Data)->m_fBtnValue,L"",L".",L"",L"%",0,0));
            m_iNbMissileUsed = (UINT32)((m_pObjAttackCursor->Value(0) / 100) * m_iNbMissileInRange);
            m_pObjMissilesUsed->Text(GString(m_iNbMissileUsed));
            m_bMeterChanged = true;
         }
         break;
      }

      // On Combo Box selection event
      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;

         //Select the missile type to use (only ground, submarines etc.)
         //-------------------------------------------------------------
         if(l_pCombo == m_pObjMissileTypeCbo)
         {   
            if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100698))
            {
               m_eMissileTypeToUse = ENuclearMissileType::Standard;
            }
            else if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100699))
            {
               m_eMissileTypeToUse = ENuclearMissileType::OnSubmarines;
            }
            else if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100700))
            {
               m_eMissileTypeToUse = ENuclearMissileType::All;
            }
            FillWindowAndMeters();       
         }

         // select the location (specific or country)
         if(l_pCombo == m_pObjCountry)
         {
            //Reinit country combo box
            GString l_sTemp = l_pCombo->Selected_Content();
            Initialize();
            l_pCombo->Selected_Content(l_sTemp);

            //if we choose a specific target
            if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100087))
            {
               m_bSpecificTargetMode = true;
					m_bProcessOfChoosingSpecificTarget = true;

               // will return the mouse position after the mouseclick event on the earth
               g_ClientDDL.BackgroundEarth()->m_ReturnClickedPosition = (ReturnClickedPosition)&SP2::GStrategicTargetCountryWindow::OnClickFromEarth;
               g_ClientDDL.BackgroundEarth()->m_pReturnWindow = this;
               
               // display the specific target selection window
               if(!g_ClientDDL.StrategicSelectTargetWindow())
                  g_ClientDDL.StrategicSelectTargetWindowSpawn();
               g_ClientDDL.StrategicSelectTargetWindow()->Show();
                              
               l_pCombo->Selected_Content(g_ClientDAL.GetString(100287));
               // hide our StrategicTargetCountryWindow for now
               Hide();
               m_pObjAttackType->Enabled(false);
               m_pObjMissileTypeCbo->Enabled(true);
               m_pObjSuggestedBtn->Enabled(false);
            }
            else
            {
               // not a specific target, just set up the country and default values
               const GCountry& l_Country = g_ClientDAL.Country(l_sTemp);
               m_iTargetCountryId = l_Country.Id();

               m_bSpecificTargetMode = false;
					m_bProcessOfChoosingSpecificTarget = false;
               m_pObjAttackType->Selected_Content(g_ClientDAL.GetString(100288));
               m_pObjAttackType->Enabled(true);
               m_pObjSuggestedBtn->Enabled(true);
            }
         }

         // select the attack type
         if(l_pCombo == m_pObjAttackType)
         {
            //If the player chose Country-wide(military)
            if(m_pObjAttackType->Selected_Content() == g_ClientDAL.GetString(100088))
            {
               m_TargetType = SP2::ENuclearTargetType::Military;
            }

            //If the player chose Country-wide(civilian)
            if(m_pObjAttackType->Selected_Content() == g_ClientDAL.GetString(100089))
            {
               m_TargetType = SP2::ENuclearTargetType::Civil;
            }
            m_pObjMissileTypeCbo->Enabled(true);

            if(m_eMissileTypeToUse != ENuclearMissileType::None)
               FillWindowAndMeters();
         }
         break;
      }
      default:
      {
         return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);      
      }
   }

   return GUI::EEventStatus::Handled;
}

void GStrategicTargetCountryWindow::OnClickFromEarth(const GVector2D<REAL32> in_Position)
{	
   if(g_ClientDDL.StrategicSelectTargetWindow()->Visible())
   {
		m_bProcessOfChoosingSpecificTarget = false;
      g_ClientDDL.StrategicSelectTargetWindow()->Hide();
      g_ClientDDL.StrategicSelectTargetWindowKill();
      Show();
      
		//Get the region id where we clicked
      UINT32 l_iID      = g_ClientDCL.EarthLocateRegion(in_Position.x,in_Position.y);

		//Best Position
		GVector2D<REAL32> l_FinalPosition = in_Position;


		//Find the closest combat, enemy, or city
		SP2::GArenaInfo* l_pClosestArena = NULL;
		REAL32 l_fMinDistanceArena = FLT_MAX;
		{
			for(stdext::hash_map<UINT32, SDK::Combat::GArenaInfo*>::const_iterator l_It = g_Joshua.UnitManager().ArenaInfos().begin();
				l_It != g_Joshua.UnitManager().ArenaInfos().end();
				l_It++)
			{
				SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)l_It->second;				
				REAL32 l_fDistance = g_ClientDAL.DistanceBetween2PointsLatLong(in_Position, l_pArenaInfo->m_Coordinate);
				if( l_fDistance < l_fMinDistanceArena )
				{
					l_fMinDistanceArena = l_fDistance;
					l_pClosestArena = l_pArenaInfo;
				}
			}
		}
		//Find the closest unit group
		const hash_map<UINT32, SDK::Combat::GUnitGroup*>& l_UnitGroups =  g_Joshua.UnitManager().UnitGroups();
		SDK::Combat::GUnitGroup* l_pClosestGroup = NULL;
		REAL32 l_fMinDistanceGroup = FLT_MAX;
		{
			for(hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator it = l_UnitGroups.begin();
				it != l_UnitGroups.end(); it++)
			{
				SDK::Combat::GUnitGroup* l_pUnitGroup = it->second;				
				REAL32 l_fDistance = g_ClientDAL.DistanceBetween2PointsLatLong(in_Position, l_pUnitGroup->Position());
				if( l_fDistance < l_fMinDistanceGroup )
				{
					l_fMinDistanceGroup = l_fDistance;
					l_pClosestGroup = l_pUnitGroup;
				}
			}
		}


		if(l_pClosestArena && l_fMinDistanceArena <= SP2::c_fDistanceMaxWhenSpecificTarget)
		{
			l_FinalPosition = l_pClosestArena->m_Coordinate;
		}
		else if(l_pClosestGroup && l_fMinDistanceGroup <= SP2::c_fDistanceMaxWhenSpecificTarget)
		{
			l_FinalPosition = l_pClosestGroup->Position();
		}

      m_pObjAttackType->Selected_Content(L"Location: (" + GString::FormatNumber(l_FinalPosition.x,2) + L", " + GString::FormatNumber(l_FinalPosition.y,2) + L")");

      //If the selected area is invalid set text to N/A
      if(l_iID == 0) 
      {
         m_pObjCountry->Selected_Content(g_ClientDAL.GetString(100106));
         m_iTargetCountryId = 0;
         m_pObjAttackType->Selected_Content(g_ClientDAL.GetString(100288));   
      }
      else
      {
         //Get the owner of the country and set country as text
         UINT32 l_iCountryID = g_ClientDAL.RegionControl(l_iID).m_iMilitary;
         if(l_iCountryID != 0)
         {
            const GCountry& l_Country = g_ClientDAL.Country(l_iCountryID);
            m_pObjCountry->Selected_Content(l_Country.Name());
            m_iTargetCountryId = (INT32)l_Country.Id(); 
            // Select the country in the main bar combo
            g_ClientDCL.SelectedCountrySet(l_Country);
         }
      }
      //Create the specific target
      GNukeTarget l_Target;
      {
         l_Target.m_TargetType = SP2::ENuclearTargetType::Specific;
         l_Target.m_Position.x = l_FinalPosition.x;
         l_Target.m_iUnitGroupID = 0;
         l_Target.m_iTargetScore = 0;
         l_Target.m_Position.y = l_FinalPosition.y;
         l_Target.m_iTargetCountryID = m_iTargetCountryId;
      }
      m_SpecificTarget = l_Target;
      m_TargetType     = SP2::ENuclearTargetType::Specific;
		

/*
      #pragma warning here
      //m_TargetsMissilesList = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(g_ClientDAL.MissileList(),m_iTargetCountryId,m_TargetType,g_ClientDAL.MissileList().size());

      //Clear and fill the vector of missiles used
      //m_MissilesFull.clear();
      //m_MissilesFull = GenerateMissilesUsedList(m_TargetsMissilesList);
      m_MissilesUsed.clear();

      INT32 l_iNbMissiles = m_MissilesFull.size();

      m_pObjAvailableMissiles->Text(GString(l_iNbMissiles));
      m_iNbMissileInRange = l_iNbMissiles;

      //If the nb of missiles in range is greater than 0 enable attack cursor
      if(m_TargetsMissilesList.size() > 0 && m_TargetsMissilesList[0].second.size() > 0)
      {
         m_pObjAttackCursor->Enabled(true);
         m_pObjAttackCursor->TextValueObj()->Text(L"0%");
      }
      else
      {
         m_pObjAttackCursor->Enabled(false);
         m_pObjAttackCursor->TextValueObj()->Text(g_ClientDAL.GetString(100106));
      }

      m_pObjProjCasualities->Text(g_ClientDAL.GetString(100106));
*/
   }
   else
   {
      //Get the region id where we clicked
      UINT32 l_iID      = g_ClientDCL.EarthLocateRegion(in_Position.x,in_Position.y);

      //If the selected area is invalid set text to N/A
      if(l_iID == 0) 
      {
         m_pObjCountry->Selected_Content(g_ClientDAL.GetString(100106));
         m_iTargetCountryId = 0;
         m_pObjAttackType->Enabled(false);
      }
      else
      {
         //Get the owner of the country and set country as text
         UINT32 l_iCountryID = g_ClientDAL.RegionControl(l_iID).m_iMilitary;
         if(l_iCountryID != 0)
         {
            const GCountry& l_Country = g_ClientDAL.Country(l_iCountryID);
            m_pObjCountry->Selected_Content(l_Country.Name());
            m_iTargetCountryId = (INT32)l_Country.Id(); 
            m_pObjAttackType->Enabled(true);
            g_ClientDCL.SelectedCountrySet(l_Country);
         }
      }
   }
   //reassign function ptr
   g_ClientDDL.BackgroundEarth()->m_pReturnWindow = NULL;
}

vector<GNuclearMissile> GStrategicTargetCountryWindow::GenerateMissilesUsedList(const vector< pair<GNukeTarget, vector<GNuclearMissile> > > &in_TargetsMissilesList)
{
   vector<GNuclearMissile> l_UsedMissileList;
   for(UINT32 i=0; i< in_TargetsMissilesList.size(); i++)
   {
      for(UINT32 j=0; j< in_TargetsMissilesList[i].second.size(); j++)
      {
         l_UsedMissileList.push_back(in_TargetsMissilesList[i].second[j]);
      }
   }
   return l_UsedMissileList;
}

vector<GNuclearMissile> GStrategicTargetCountryWindow::GenerateMissilesUsedListFromSpec(const multimap<UINT32,GNuclearMissile> &in_TargetsMissilesList)
{
   vector<GNuclearMissile> l_UsedMissileList;
   for(multimap<UINT32,GNuclearMissile>::const_iterator l_it = in_TargetsMissilesList.begin(); 
       l_it != in_TargetsMissilesList.end(); 
       l_it++)
   {
      l_UsedMissileList.push_back(l_it->second);
   }
   return l_UsedMissileList;
}


bool GStrategicTargetCountryWindow::OnDestroy()
{
   __super::OnDestroy();

   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(m_pSlidingTrack);

   SAFE_RELEASE(m_pSlidingTrack);

   if(g_ClientDDL.BackgroundEarth())
   {
      g_ClientDDL.BackgroundEarth()->m_pReturnWindow = NULL;
   }

   return true;
}

void GStrategicTargetCountryWindow::OnHide()
{
   if(g_ClientDDL.BackgroundEarth() && g_ClientDDL.StrategicSelectTargetWindow() && !g_ClientDDL.StrategicSelectTargetWindow()->Visible())
      g_ClientDDL.BackgroundEarth()->m_pReturnWindow = NULL;

   __super::OnHide();
}

CGFX_Track_Itf* & GStrategicTargetCountryWindow::AnimationSlidingTrack()
{
   return m_pSlidingTrack;
}


/*!
 * Updates the target list, the list of missiles used, the casualties count and consequently the interface
 **/
void GStrategicTargetCountryWindow::UpdateTargetsMissilesAndCasualties(const vector<GNuclearMissile>& in_vMissilesAvailable,
                                                                       UINT32                         in_iTargetCountryID,
                                                                       SP2::ENuclearTargetType::Enum  in_eTargetType,
                                                                       const UINT32                   in_iNbMissilesToUse)
{
   //Cant do for undefined target type and if there are at least 1 missile in range
   if(in_eTargetType == ENuclearTargetType::Undefined || m_iNbMissileInRange == 0)
   {
      // can't nuke if not a valid target
      CantNuke();
      return;
   }

   m_TargetType             = in_eTargetType;
   m_NewTargetsMissilesList = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(in_vMissilesAvailable,in_iTargetCountryID,in_eTargetType,in_iNbMissilesToUse,false);
   m_MissilesUsed           = GenerateMissilesUsedList(m_NewTargetsMissilesList);
   m_iNbMissileUsed         = m_MissilesUsed.size();

   //Set the attack cursor
   {
      REAL32  l_fPercentage = (REAL32)100*m_iNbMissileUsed/m_iNbMissileInRange;
      GString l_sPercentage = GString::FormatNumber((REAL64)l_fPercentage,L"",L"",L"",L"%",3,0);
      m_pObjAttackCursor->Value(l_fPercentage,0);      
      m_pObjAttackStrengthValue->Text(l_sPercentage);
   }
   //Write the number of missiles used
   m_pObjMissilesUsed->Text(GString(m_iNbMissileUsed));

	//Calculate the casualities for the amount of missiles used and display them
   {      
      UINT32 l_iCasualties = g_ClientDCL.EstimateNuclearCasualties(m_NewTargetsMissilesList);

	   if(in_eTargetType == SP2::ENuclearTargetType::Military)
         m_pObjProjCasualities->Text(L"$ " + GString(l_iCasualties));
      else if((m_TargetType == SP2::ENuclearTargetType::Civil) || (m_TargetType == SP2::ENuclearTargetType::Specific))
		   m_pObjProjCasualities->Text(L"Pop: " + GString(l_iCasualties));
   }

   return;
}

void GStrategicTargetCountryWindow::FillWindowAndMeters()
{
   //If the target country is != 0, fill the window and meters
   if(m_iTargetCountryId != 0)
   {
      //Clear and fill the target list
      m_TargetsMissilesList.clear();

      //Count the number of missiles that could be sent on the given target
      vector<GNuclearMissile> l_vWantedMissiles;
      {         
         switch(m_eMissileTypeToUse)
         {
            case ENuclearMissileType::Standard:
               l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
               break;
            case ENuclearMissileType::OnSubmarines:
               l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles();
               break;
            case ENuclearMissileType::All:
            {
               l_vWantedMissiles = g_ClientDAL.m_PlayerCountryData.GroundBasedNuclearMissiles();
               l_vWantedMissiles.insert(l_vWantedMissiles.begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().begin(),g_ClientDAL.m_PlayerCountryData.SubmarineBasedNuclearMissiles().end());
               break;
            }
            default:
               gassert(0,"Strategic window, on click from earth: Unhandled case");
         }          
      }

      // clear the missile used vectors
      m_MissilesFull.clear();
      m_MissilesUsed.clear();

      // specific target ?
      if(m_bSpecificTargetMode)
      {                                                                
         m_SpecificTargetMissilesByRange = g_ClientDCL.MissilesInRange(l_vWantedMissiles, m_SpecificTarget);
         //fill the vector of missiles used  
         m_MissilesFull                  = GenerateMissilesUsedListFromSpec(m_SpecificTargetMissilesByRange); 
      }
      else
      {
         m_TargetsMissilesList = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(l_vWantedMissiles,m_iTargetCountryId,m_TargetType,l_vWantedMissiles.size(),false);
         //fill the vector of missiles used         
         m_MissilesFull        = GenerateMissilesUsedList(m_TargetsMissilesList);  
      }  

      if(!m_MissilesFull.empty())
      {
         m_pObjAttackCursor->Enabled(true);
      }

      UINT32 l_iNbMissiles = m_MissilesFull.size();
      m_pObjAvailableMissiles->Text(GString(l_iNbMissiles));
      m_iNbMissileInRange = l_iNbMissiles;

      //If the nb of missiles in range is greater than 0 enable attack cursor otherwise make sure it's disabled
      if(m_iNbMissileInRange > 0)
      {
         m_pObjAttackCursor->Enabled(true);
         m_iNbMissileUsed = 0;
         //Update the meters
         OnNbMissilesUsedChange();
      }
      else
      {
         // can't nuke if no missile are in range
         CantNuke();
      }

      //Now, compute the optimal missile assignment
      if(!m_MissilesFull.empty())
      {
         vector< pair<GNukeTarget, vector<GNuclearMissile> > > l_vOptimalAssignment = g_ClientDCL.SelectNuclearTargetsAndAssignMissiles(m_MissilesFull,m_iTargetCountryId,m_TargetType,m_MissilesFull.size(),true);
         vector<GNuclearMissile> l_vMissiles = GenerateMissilesUsedList(l_vOptimalAssignment);
         UpdateTargetsMissilesAndCasualties(m_MissilesFull,m_iTargetCountryId,m_TargetType,l_vMissiles.size());
      } 
   }
   else
   {
      // Cant attack if not a valid country id
      CantNuke();
   }
} 

void GStrategicTargetCountryWindow::OnNbMissilesUsedChange()
{
   //Update everything
   UpdateTargetsMissilesAndCasualties(m_MissilesFull,m_iTargetCountryId,m_TargetType,m_iNbMissileUsed);
}

void GStrategicTargetCountryWindow::CantNuke()
{
   m_pObjAttackCursor->Enabled(false);
   m_pObjAttackCursor->Value(0,0);
   m_pObjAttackCursor->TextValueObj()->Text(g_ClientDAL.GetString(100106));
   m_iNbMissileUsed    = 0;
   m_iNbMissileInRange = 0;
   m_pObjMissilesUsed->Text(L"");
}