/**************************************************************
*
* sp2_tutorials.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"
#include "../include/sp2_tutorials.h"
#include "../include/sp2_tutorial_window.h"
#include "../include/sp2_unit_research_window.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_military_window.h"
#include "../include/sp2_design_window.h"
#include "../include/sp2_create_design_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_unit_attack_confirm_window.h"
#include "../include/sp2_battle_overview_window.h"
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_unit_production_window.h"
#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_unit_ground_deployment_window.h"
#include "../include/sp2_strategic_target_country_window.h"
#include "../include/sp2_strategic_launch_window.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_country_information_window.h"
#include "../include/sp2_relations_window.h"
#include "../include/sp2_connecting_window.h"
#include "../include/sp2_system_bar_window.h"
#include "../include/sp2_game_options_window.h"
#include "../include/sp2_credits_window.h"

using namespace SP2;

CWSTR c_LeftClickMousePointer   = L"/data/cursors/mouse-leftclick.ani";
CWSTR c_RightClickMousePointer  = L"/data/cursors/mouse-rightclick.ani";
CWSTR c_MiddleClickMousePointer = L"/data/cursors/mouse-middleclick.ani";
const INT32 c_iChinaId          = 37;
const INT32 c_iCanadaId         = 32;

/*!
 * constructor
 **/
GTutorials::GTutorials()
{   
   // load the left click blinking cursor
   g_Joshua.FileManager()->File(c_LeftClickMousePointer, m_LeftClickMousePointer);
   m_pTutorialWindow    = NULL;  
   m_pTargetArenaInfo   = NULL;
   m_pMilitaryTargetGrp = NULL;
   m_fNextTimeToProceed = 0.f;
   m_bStateRollBack     = false;
}

/*!
 * destructor
 **/
GTutorials::~GTutorials()
{   
   SAFE_DELETE(m_pTutorialWindow);
}

/*!
 * global static instance of our tutorial class
 **/
GTutorials & GTutorials::Instance()
{
   static GTutorials s_TutorialInstance;
   return s_TutorialInstance;   
}

/*!
 * begins tutorial mode
 **/
void GTutorials::StartTutorial(ETutorials::Enum in_Tutorial)
{   
   // Render our tutorial window
   if (!m_pTutorialWindow)
   {      
      CGFX_Renderer_Init_Mode l_Mode;
      g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
      m_pTutorialWindow = CREATE_GUI_OBJECT_HIDDEN(GTutorialWindow, (GBaseObject*)g_ClientDDL.BackgroundEarth());
      m_pTutorialWindow->Owner(NULL);
   }          

   // Setup our always valid events 
   m_AlwaysValidEventTargets.clear();
   m_AlwaysValidEventTargets.insert(m_pTutorialWindow);   
   m_AlwaysValidEventTargets.insert((GBaseObject*)g_SP2Client->DDL().MainMenu());
   m_AlwaysValidEventTargets.insert((GBaseObject*)g_SP2Client->DDL().ConsoleWindow());   
   // create the connection window
   if(!g_SP2Client->DDL().ConnectingWindow())
   {
      g_SP2Client->DDL().ConnectingWindowSpawn();
      g_SP2Client->DDL().ConnectingWindow()->Hide();
   }
   // game options
   if(!g_SP2Client->DDL().GameOptionsWindow())
   {
      g_SP2Client->DDL().GameOptionsWindowSpawn();
      g_SP2Client->DDL().GameOptionsWindow()->Hide();      
   }
   // game credits
   if(!g_SP2Client->DDL().CreditsWindow())
   {
      g_SP2Client->DDL().CreditsWindowSpawn();
      g_SP2Client->DDL().CreditsWindow()->Hide();      
   }
   // allow to cancel connection ...
   m_AlwaysValidEventTargets.insert((GBaseObject*)g_SP2Client->DDL().ConnectingWindow());
   // allow game options
   m_AlwaysValidEventTargets.insert((GBaseObject*)g_SP2Client->DDL().GameOptionsWindow());
   // allo credit
   m_AlwaysValidEventTargets.insert((GBaseObject*)g_SP2Client->DDL().CreditsWindow());

   g_Joshua.GUIManager()->InstallGUIHook(this);
 
   // Tutorials defaults vars 
   m_bGoalReached       = true;
   m_bSpecificGoal      = true;
   m_pMilitaryTargetGrp = NULL;
   m_fNextTimeToProceed = 0.f;
   m_bStateRollBack     = false;
   m_State              = 0;
   m_StartedTutorial    = in_Tutorial; 
   m_pTargetArenaInfo   = NULL;

   // no unit can move in tutorials ...
   g_ClientDCL.LimitMovementTo(EUnitMovementPermissions::c_iNoUnitCanMove, 
                               EUnitMovementPermissions::c_iUnitCantMoveToAnyCountry);
}

/*!
 * Stops tutorial mode
 **/
void GTutorials::StopTutorial()
{
   m_pTutorialWindow->Hide();   
   m_State = 0;
   m_StartedTutorial = ETutorials::None;      

   // Any units can move outside the tutorials
   g_ClientDCL.LimitMovementTo(EUnitMovementPermissions::c_iAnyUnitCanMove, 
                               EUnitMovementPermissions::c_iUnitCanMoveAnywhere);
   g_ClientDDL.BackgroundEarth()->UnlockEarthPosition();

   g_ClientDDL.ResetMousePointer();
   g_Joshua.GUIManager()->UninstallGUIHook();
   g_SP2Client->StopGame();
}

/*!
 * Set Tutorial window text
 **/
void GTutorials::SetupTutorialWindowText(const GString &in_sBegin, GTutorialWindow* in_pTutWindow)
{
   GString l_sText = in_sBegin;

   // if no specific goal, display "left click to continue"
   if (!m_bSpecificGoal)
   {
      l_sText += g_ClientDAL.GetString(c_iClickAnywhereToContinueStrId);
      g_Joshua.GUIManager()->SetMousePointer(m_LeftClickMousePointer);
   }
   else
   {
      if (m_ValidEventTargets.find(in_pTutWindow) != m_ValidEventTargets.end())
         l_sText += g_ClientDAL.GetString(c_iClickHereToProceedToNextStepStrId);
      g_ClientDDL.ResetMousePointer();
   }

   //Make sure the \\n have been replaced by real newlines
   l_sText = Util::ReplaceEndLinesByRealEndLines(l_sText);

   // set up the tutorial window text
   in_pTutWindow->Text(l_sText);
   in_pTutWindow->SetupStandardSize();
}

void GTutorials::SetupEarthView(const GVector2D<REAL32> &in_vPos, const REAL32 in_fZoom)
{
   // Earth will move so unlock it
   g_ClientDDL.BackgroundEarth()->UnlockEarthPosition();
   // Zoom
   g_ClientDCL.m_EarthState.m_Zoom = in_fZoom;
   g_ClientDDL.MiniMap()->UpdateZoomBar(); 
   // Center the tutorial window
   g_ClientDDL.BackgroundEarth()->CreateCameraAnimation(&in_vPos);
   m_fNextTimeToProceed = g_ClientDDL.BackgroundEarth()->PlayCameraAnimation() + g_Joshua.Clock();  
}

void GTutorials::SetupEarthViewCompleted()
{
   // Earth has stop moved, so lock it
   g_ClientDDL.BackgroundEarth()->LockEarthPosition();
   m_fNextTimeToProceed = 0.f;
}

bool GTutorials::CanFireEvent(EKeyAndMouseEvents::Enum in_EventProc, GUI::GBaseObject *& in_pTarget, GEventData & in_EventData)
{
   // if special moving, no movements available
   if (g_Joshua.Clock() < m_fNextTimeToProceed)
      return false;

   GBaseObject * l_Target = in_pTarget;
   // Loop for always valid events target
   while(l_Target)
   {
      //If target is in the always valid events
      if (m_AlwaysValidEventTargets.find(l_Target) != m_AlwaysValidEventTargets.end())
      {
         if ((in_EventProc == EKeyAndMouseEvents::OnMouseOver) && (l_Target != m_pTutorialWindow))
            g_ClientDDL.ResetMousePointer();
         
         if ((in_EventProc == EKeyAndMouseEvents::OnMouseClick) && (l_Target == m_pTutorialWindow))
         {
            //Right click on the Tutorials window = Moving back 
            // DShooner : not really well supported ... m_bStateRollBack is a patch
            if (in_EventData.Mouse.Actor.Bits.Right && m_bStateRollBack)
            {
               m_State = max(0, m_State - 2);
               m_bGoalReached = true;
               m_ValidEventTargets.clear();
            }
            
            //Left clicking on the tutorial window generally advances
            //the tutorial
            else if (in_EventData.Mouse.Actor.Bits.Left)
            {
               //If not a specific goal, advance the tutorial
               if (!m_bSpecificGoal)
               {
                  m_bGoalReached = true;
                  return false;
               }
               //Specific goal, in some case, clicking the tut window advances the tutorial
               switch (m_StartedTutorial)
               {
                  case ETutorials::InterfaceTutorial:
                     m_bGoalReached = ClickingTutorialWindowOnSpecificGoalAdvancesInterfaceTutorial();
                     break;
                  case ETutorials::MilitaryTutorial:
                     m_bGoalReached = ClickingTutorialWindowOnSpecificGoalAdvancesMilitaryTutorial();
               }
            }
         }
         // Always valid will always receive events ...
         return true;
      }
      l_Target = l_Target->Owner();
   }

   //Set a specific mouse pointer (mouse left click) when a general goal
   if (in_EventProc == EKeyAndMouseEvents::OnMouseOver)
   {
      if (!m_bSpecificGoal)
         g_Joshua.GUIManager()->SetMousePointer(m_LeftClickMousePointer);
   }

   //Click anywhere to continue
   if (in_EventData.Mouse.Actor.Bits.Left && (in_EventProc == EKeyAndMouseEvents::OnMouseClick) && !m_bSpecificGoal)
   {
      m_bGoalReached = true;
      return false;
   }

   // no double-click in backgroud earth
   if(in_EventData.Mouse.IsDoubleClick && 
      (m_ValidEventTargets.find(g_ClientDDL.BackgroundEarth()) != m_ValidEventTargets.end()))
      return false;

   //Check if target is in the ValidEventTarget list, and if so, check if condition
   //was reached
   l_Target = in_pTarget;
   while(l_Target)
   {
      // no event to the main bar
      if(l_Target == g_ClientDDL.MainBar() || l_Target == g_ClientDDL.MiniMap() || 
         l_Target == g_ClientDDL.SystemBarWindow())
         break;

      //If the object is in the valid event target list, check if we can advance tutorial specific goal
      if (m_ValidEventTargets.find(l_Target) != m_ValidEventTargets.end())
      {
         switch (m_StartedTutorial)
         {
            case ETutorials::InterfaceTutorial:
            { 
               return InterfaceTutorialCanObjectRecvEvent(in_EventProc,in_EventData);
            }
            case ETutorials::MilitaryTutorial:
            {
               return MilitaryTutorialCanObjectRecvEvent(in_EventProc,in_EventData);
            }
         }  
      }
      else
      {//Not in the listCheck its owner
         l_Target = l_Target->Owner();                     
      }
   }

   // Cannot fire events ...
   return false;
}

bool GTutorials::CanFireHotkey(const GEventData & in_EventData, GBaseObject*)
{
   switch (in_EventData.Key() )
   {
   case VK_F9:
   case VK_F10:
      return true;
   case VK_HOME:
      if ((m_StartedTutorial == ETutorials::InterfaceTutorial) && (m_State == 19))
         return true;
   default:
      return false;
   }
}

bool GTutorials::CanGiveFocusTo(GBaseObject* in_pObject)
{
   GBaseObject * l_Target = in_pObject;
   while(l_Target)
   {
      if (m_AlwaysValidEventTargets.find(l_Target) != m_AlwaysValidEventTargets.end())
         return true;
      l_Target = l_Target->Owner();
   }

   l_Target = in_pObject;
   while(l_Target)
   {
      if (m_ValidEventTargets.find(l_Target) != m_ValidEventTargets.end())
         return true;
      l_Target = l_Target->Owner();
   }
   return false;
}

bool GTutorials::CanBringToFront(GBaseObject* in_pObject)
{
   GBaseObject *l_Target = in_pObject;
   while(l_Target)
   {
      if (m_AlwaysValidEventTargets.find(l_Target) != m_AlwaysValidEventTargets.end())
         return true;
      l_Target = l_Target->Owner();
   }

   l_Target = in_pObject;
   while(l_Target)
   {
      if (m_ValidEventTargets.find(l_Target) != m_ValidEventTargets.end())
         return true;
      l_Target = l_Target->Owner();
   }
   return false;
}


/*!
 * Iterates the tutorial
 **/
void GTutorials::TutorialIterate()
{
   //If no tutorial started
   if (m_StartedTutorial == ETutorials::None)
      return;

   //If we are not actually playing but the tutorial is started
   //# Keep tutorial alive until game is started...
   if (g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
   {
      if (m_State)
         StopTutorial();
      return;
   }

   // if special moving, no movements available
   if (g_Joshua.Clock() < m_fNextTimeToProceed)
      m_bGoalReached = false;
   else
   {
      // need to lock the Earth ?
      if(m_fNextTimeToProceed != 0.f)
         SetupEarthViewCompleted();

      //Always iterate the tutorial conditions
      if(m_StartedTutorial == ETutorials::MilitaryTutorial)
         IterateMilitaryTutorialConditions();
      else if(m_StartedTutorial == ETutorials::InterfaceTutorial)
         IterateInterfaceTutorialConditions();
   }

   //If the tutorial window is hidden, display it
   if (!m_pTutorialWindow->Visible()) 
      m_pTutorialWindow->Show();

   //If the goal wasnt reached, dont advance to the next step
   if (!m_bGoalReached)
      return;

   //Goal was reached, advance to the next step
   ++m_State;

   //Reset the tutorial specific values
   {
      m_ValidEventTargets.clear();
      m_pTutorialWindow->HideArrow();
      m_bGoalReached  = false;   
      m_bSpecificGoal = false;   
   }
      
   // can roll back by default ...
   m_bStateRollBack = true;
   // iterate hence tutorial 
   switch (m_StartedTutorial)
   {
   case ETutorials::InterfaceTutorial:
      IterateInterfaceTutorial();
      break;
   case ETutorials::MilitaryTutorial:
      IterateMilitaryTutorial();
   }

   m_pTutorialWindow->OnResize();
   m_pTutorialWindow->OnShow(); 
}


//          Interface tutorial methods (begin)
//-------------------------------------------------------

/*!
 * Iterates the interface tutorial
 * @returns the String to display in the tutorial window
 **/
void GTutorials::IterateInterfaceTutorial()
{
   GString l_sText;
   GTutorialWindow * l_pTutWin = (GTutorialWindow *)m_pTutorialWindow;

   // Set the text for the step
   if(m_State > 0 && m_State <= c_iInterfaceTutorialNbSteps)
   {
      // Tutorials strings are in the string table 
      l_sText = g_ClientDAL.GetString(c_iInterfaceTutorialStringId[m_State-1]);
   }

   switch (m_State)
   {
   // Welcome 
   case 1:
      l_pTutWin->CenterAndResize();
   // Explaining ESC key and right-click
   case 2:
   // Encourage to start campaign mode after tutorial
   case 3:
   // What is SuperPower 2 ?
   case 4:
   // Moving around the globe is important
   case 5:
      break;
   // Two ways of navigating: 1 - the map
   case 6:
      l_pTutWin->PointTo(g_ClientDDL.MiniMap()->Map());         
      break;
   // try to use the navigation map with a click
   case 7:
      l_pTutWin->PointTo(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;
      break;
   // Selected target on the map will be the main view
   case 8:
      break;
   // try to use the navigation map with a drag
   case 9:
      l_pTutWin->PointTo(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;         
      break;
   // Right-click to rotate the globe
   case 10:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;         
      break;
   // View is blocked at the poles to prevent from globe from turning upside down
   case 11:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());  
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(l_pTutWin);
      break;
   // Two ways of zooming
   case 12:
      break;
   // First by using the zoom bar
   case 13:
      l_pTutWin->PointTo(g_ClientDDL.MiniMap()->ZoomBar());      
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Child(L"btnHiding"));
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Child(L"btnZoomRect"));
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->ZoomBar());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;         
      break;
   // Second by using the mouse wheel
   case 14:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;
      break;
   // Notice as you move closer, more details
   case 15:   
      g_ClientDCL.SelectHomeCountry();
      g_ClientDCL.m_EarthState.m_Zoom = 0.9f;
		g_ClientDDL.MiniMap()->UpdateZoomBar();      
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      break;
   // Select anything by left clicking on it
   case 16:
      g_ClientDCL.m_EarthState.m_Zoom = 0;
		g_ClientDDL.MiniMap()->UpdateZoomBar();
      m_ValidEventTargets.insert(l_pTutWin);
      break;
   // Select China for the purpose of the scenario
   case 17:
      g_ClientDCL.SelectedCountrySetLookAt(g_ClientDAL.Country(c_iChinaId));
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"cboCountry")); 
      m_ValidEventTargets.insert(l_pTutWin);
      m_bStateRollBack = false;
      break;
   // To continue select the United States
   case 18:
      g_ClientDCL.SelectedCountrySetLookAt(g_ClientDAL.Country(c_iChinaId));
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_bSpecificGoal = true;
      break; 
   // home key is cooler to select your country
   case 19:
      g_ClientDCL.SelectedCountrySetLookAt(g_ClientDAL.Country(c_iChinaId));
      l_pTutWin->PointTo(g_ClientDDL.MiniMap()->HomeButton());   
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->HomeButton());  
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_bSpecificGoal = true;
      break;
   // Last important thing is the thematic maps
   case 20:
      break;
   // Right click brings the contextual menu
   case 21:
      break;
   // Secrets of thematic maps revelead after this
   case 22:
      break;
   // Select the economic strength through contextual menu
   case 23:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());   
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_bSpecificGoal = true;
      break;
   // Explaining colors in Economic strength thematic map
   case 24:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      break;
   // Reselect the Geographic map
   case 25:
      m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
      m_ValidEventTargets.insert(g_ClientDDL.MiniMap()->Map());
      m_bSpecificGoal = true;
      break;
   // Main toolbar explained
   case 26:
      break;
   // The game is divided in 3 concepts, military, politic, economic
   case 27:
      break;
   // Theses spheres are accessible through their respective button
   case 28:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnMilitary"));
      break;
   // Explaining window levels and informations
   case 29:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnMilitary"));
      break;
   // Purpose of political window
   case 30:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child("btnPolitic"));         
      break;
   // Purpose of military window
   case 31:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnMilitary"));
      break;
   // Purpose of economic window
   case 32:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnEconomic"));         
      break;
   case 33:
      // REMOVED, proceed to next step !
      m_bGoalReached = true;
      return;
   // Explaining the progress bars under the spheres buttons
   case 34:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnMilitary"));         
      break;
   // Currently selected country with its flag
   case 35:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"cboCountry"));         
      break;
   // The country name is in a combo box !
   case 36:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"cboCountry"));
      break;
   // Select the Canada in the box to continue ...
   case 37:
      g_ClientDCL.SelectHomeCountry();
      g_ClientDCL.m_EarthState.m_Zoom = 0;
		g_ClientDDL.MiniMap()->UpdateZoomBar();
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"cboCountry"));
      m_ValidEventTargets.insert(g_ClientDDL.MainBar()->Child(L"cboCountry"));
      m_bSpecificGoal = true;
      break;
   // Diplomatic relations instead of the objectives button
   case 38:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnStatus"));
      m_bStateRollBack = false;
      break;
   // Click on this button to view Diplomatic relation window (????)
   case 39:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnRelations"));
      m_ValidEventTargets.insert(g_ClientDDL.MainBar()->Child("btnRelations"));
      m_bSpecificGoal = true;
      break;
   // Explaining the diplomatic window
   case 40:
      l_pTutWin->MoveToTop();
      // be really really sure its visible
      if(!g_ClientDDL.RelationsWindow())
         g_ClientDDL.RelationsWindowSpawn();
      g_ClientDDL.RelationsWindow()->Show();
      m_ValidEventTargets.insert(g_ClientDDL.RelationsWindow());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;
      break;
   // Can drag window to reposition information
   case 41:
      // be really really sure its visible
      if(!g_ClientDDL.RelationsWindow())
         g_ClientDDL.RelationsWindowSpawn();
      g_ClientDDL.RelationsWindow()->Show();
      m_ValidEventTargets.insert(g_ClientDDL.RelationsWindow());
      m_ValidEventTargets.insert(l_pTutWin);
      m_bSpecificGoal = true;    
      break;
   // Close this window through the close button
   case 42:
      // be really really sure its visible
      if(!g_ClientDDL.RelationsWindow())
         g_ClientDDL.RelationsWindowSpawn();
      g_ClientDDL.RelationsWindow()->Show();
      m_ValidEventTargets.insert(g_ClientDDL.RelationsWindow());
      m_bSpecificGoal = true;
      break;
   // More info button for our country
   case 43:
      l_pTutWin->CenterAndResize();
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnMoreInfo"));
      m_ValidEventTargets.insert(g_ClientDDL.MainBar()->Child(L"btnMoreInfo"));
      m_bSpecificGoal = true;
      break;
   // Explaining the more info window, close it through the close button
   case 44:
      l_pTutWin->MoveToTop();
      // be really really sure its visible
      if(!g_ClientDDL.CountryInformationWindow())
         g_ClientDDL.CountryInformationWindowSpawn();
      g_ClientDDL.CountryInformationWindow()->Show();
      m_ValidEventTargets.insert(g_ClientDDL.CountryInformationWindow());
      m_bSpecificGoal = true;
      break;
   // Explaining the advisor window
   case 45:
      l_pTutWin->CenterAndResize();
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"togAdvisor"));
      break;
   // Explaining the history graph
   case 46:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"togHistory"));
      break;
   // Pending mails
   case 47:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->m_pObjMail);
      break;
   // Differents events happening world wide in the news bar
   case 48:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"frmNews"));
      break;
   // Differents events happening world wide in the news bar
   case 49:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"frmNews"));
      break;
   // Events filter can be activated
   case 50:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"frmNews"));
      break;
   // Contains the last 200 events ...
   case 51:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"togNews"));
      break;
   // Displaying current game date and time
   case 52:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"lblDate"));
      break;
   // Time flows at a constant rate, but game speed can be adjusted
   case 53:
      l_pTutWin->PointTo(g_ClientDDL.MainBar()->Child(L"btnTime"));
      break;
   // Left click to return to main menu
   case 54:
      break;
   default:
      StopTutorial();
      return;
      break;
   }   
   // display tutorial window
   SetupTutorialWindowText(l_sText, l_pTutWin);
}

bool GTutorials::InterfaceTutorialCanObjectRecvEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,
                                                     GEventData& in_EventData)
{
   switch (m_State)
   {
   // all these states are able to rotate earth
   case 7: 
   case 9: 
   case 13:     
      return true;
   case 10: 
   case 11: 
   case 14: 
   case 15: 
   case 24:
      if (EarthRotatingEvent(in_eKeyOrMouseEvent, in_EventData))
         return true;
      break;
   // pick the states through the map
   case 18:
      if (g_ClientDCL.SelectedCountryID() == c_iMilitaryTutorialCountryToControl)
         m_bGoalReached = true;
      // earth rotation ?
      if (EarthRotatingEvent(in_eKeyOrMouseEvent, in_EventData))
         return true;
      // left click ?  
      return true;
   // select economic strength menu
   case 23:
      if (g_ClientDCL.CurrentThematicMap == ECharacteristicToHighlight::CountryEconomicalStrength)
         m_bGoalReached = true;
      // earth rotation ?
      if (EarthRotatingEvent(in_eKeyOrMouseEvent, in_EventData))
         return true;
      return true;
   // select the geographic menu
   case 25:
      if (g_ClientDCL.CurrentThematicMap == ECharacteristicToHighlight::Geographic)
         m_bGoalReached = true;
      // earth rotation ?
      if (EarthRotatingEvent(in_eKeyOrMouseEvent, in_EventData))
         return true;
      return true;
   // select the canada in the country combo box
   case 37:               
      if (g_ClientDCL.SelectedCountryID() == c_iCanadaId)
         m_bGoalReached = true;
      return true;
   case 19:// pick the states through the home key
      //Only allow keyboard input
      if(in_EventData.Data == g_ClientDDL.MiniMap()->HomeButton())
         return true;
      if(in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnKeyDown || in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnKeyUp )
         return true;
   case 39:// open the relations window 
   case 40:
   case 41://play with the relations window
   case 42:// close the relations window
   case 43:// open the more info window               
   case 44:// close the more info window                
      return true;
   }
   // event wont be fired
   return false;
}

bool GTutorials::EarthRotatingEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,GEventData & in_EventData)
{
   return ((in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnMouseWheel) || 
           ((in_eKeyOrMouseEvent != EKeyAndMouseEvents::OnMouseClick) && 
            in_EventData.Mouse.Down.Bits.Right));
}

/*!
 * Checks if the interface conditions for a status change are met
 **/
void GTutorials::IterateInterfaceTutorialConditions()
{
   // switch states, check for conditions
   switch (m_State)
   {
   // pick the states through the home key
   case 19:
      if (g_ClientDCL.SelectedCountryID() == c_iMilitaryTutorialCountryToControl)
         m_bGoalReached = true;
      break;
   // open the relations window 
   case 39:      
      if(g_ClientDDL.RelationsWindow() && g_ClientDDL.RelationsWindow()->Visible())
         m_bGoalReached = true;
      break;
   // close the relations window
   case 42:
      if(g_ClientDDL.RelationsWindow() && g_ClientDDL.RelationsWindow()->Visible())
         m_pTutorialWindow->PointTo(g_ClientDDL.RelationsWindow()->Child(L"frmTitle")->Child(L"btnExit"));
      if(!g_ClientDDL.RelationsWindow() || !g_ClientDDL.RelationsWindow()->Visible())
         m_bGoalReached = true;
      break;
   // open the more info window
   case 43:               
      if(g_ClientDDL.CountryInformationWindow() && g_ClientDDL.CountryInformationWindow()->Visible())
         m_bGoalReached = true;
      break;
   // close the more info window
   case 44:            
      if(g_ClientDDL.CountryInformationWindow() && g_ClientDDL.CountryInformationWindow()->Visible())
         m_pTutorialWindow->PointTo(g_ClientDDL.CountryInformationWindow()->Child(L"frmTitle")->Child(L"btnExit"));
      if(!g_ClientDDL.CountryInformationWindow() || !g_ClientDDL.CountryInformationWindow()->Visible())
         m_bGoalReached = true;
      break;
   default:
      return;
   }
}

/*!
 * Specifies when clicking directly on the tutorial window advances the tutorial
 * 
 **/
bool GTutorials::ClickingTutorialWindowOnSpecificGoalAdvancesInterfaceTutorial()
{
   switch (m_State)
   {
      case 7: 
      case 9: 
      case 10: 
      case 13: 
      case 14:
      case 40:
      case 41:
         return true;
      default:
         return false;
   }
}
//---------end of interface tutorial methods --------------


//          Military tutorial methods (begin)
//-------------------------------------------------------

/*!
 * Init the tutorial mode on first step
 **/
void GTutorials::SetupMilitaryTutorial()
{   
   // center tutorial window
   m_pTutorialWindow->CenterAndResize();
   // setup the view
   SetupEarthView(g_ClientDAL.CapitalPosition(g_ClientDAL.Country(c_iMilitaryTutorialCountryToControl).Id()), 
                  c_fMilitaryTutorialZoomLevelSelect);
}

/*!
 * Iterates the military tutorial
 * @returns the String to display in the tutorial window
 **/
void GTutorials::IterateMilitaryTutorial()
{
   GTutorialWindow* l_pTutWin       = (GTutorialWindow *)m_pTutorialWindow;
   GString l_sText                  = "";
   
   //Set the text for the step
   if(m_State > 0 && m_State <= c_iMilitaryTutorialNbSteps)
   {
      // Get the text from the string table
      l_sText = g_ClientDAL.GetString(c_iMilitaryTutorialStringId[m_State-1]);
   }
                           
   switch (m_State)
   {
      // Military Intro
      case 1:                           
         // init tutorial
         SetupMilitaryTutorial();
      // What are the units groups ? Color signification
      case 2:     
         // Is the tutorial group valid ?
         m_pMilitaryTargetGrp = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(c_iMilitaryTutorialTargetUnitGroup);
         gassert(m_pMilitaryTargetGrp,"GTutorials::IterateMilitaryTutorial Should be a valid target group");
         // Locate it !
         SetupEarthView(m_pMilitaryTargetGrp->Position(), c_fMilitaryTutorialZoomLevelSelect);
         break;
      // Select this unit group by left clicking on it
      case 3:
         m_bSpecificGoal = true;                                                        
         // Point the group
         l_pTutWin->MoveToTop();
         l_pTutWin->PointToLatLong(m_pMilitaryTargetGrp->Position());
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth()); 
         // only this group can be selected
         g_ClientDCL.LimitMovementTo(c_iMilitaryTutorialTargetUnitGroup, 
                                     EUnitMovementPermissions::c_iUnitCantMoveToAnyCountry);
         break;
      // Explaining units status
      case 4:
         m_bSpecificGoal = true;
         m_ValidEventTargets.insert(l_pTutWin);
      case 5:
         break;
      // Try to change the unit status by right clicking on it
      case 6:
         m_bSpecificGoal = true;            
         // Put an arrow by the unit group and enable it
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());         
         l_pTutWin->PointToLatLong(m_pMilitaryTargetGrp->Position());
         break;
      // Progress bar for status change
      case 7:
         m_bSpecificGoal = true;
         break;
      // Lets see how we move it
      case 8:        
         //Select the controlled country and unzoom
         SetupEarthView(g_ClientDAL.CapitalPosition(g_ClientDAL.Country(c_iMilitaryTutorialCountryToControl).Id()) - GVector2D<REAL32>(20.0f, 0.0f),
                        c_fMilitaryTutorialZoomLevelSelect);  
         m_bStateRollBack = false;
         break;
      // 3 ways of moving a unit
      case 9:
         break;
      // Choose prefered methode to move it to Cuba
      case 10:
         m_bSpecificGoal = true;
         // pad the enemy country name
         l_sText += g_ClientDAL.Country(c_iMilitaryTutorialTargetCountry).Name();     
         //Put an arrow by the unit group
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
         l_pTutWin->PointToLatLong(m_pMilitaryTargetGrp->Position());
         g_ClientDCL.LimitMovementTo(c_iMilitaryTutorialTargetUnitGroup, c_iMilitaryTutorialTargetCountry);
         break;
      // Confirm the attack of a neutral country
      case 11:
      {
         m_bSpecificGoal = true;
         // Ptr to confirm attack window
         GUnitAttackConfirmWindow* l_pAttackConfirmWindow = g_ClientDDL.UnitAttackConfirmWindow(c_iMilitaryTutorialTargetCountry);
         // Add the ok button to the list of valid targets
         l_pTutWin->MoveToTop();
         l_pTutWin->PointTo(l_pAttackConfirmWindow->ButtonYes());
         m_ValidEventTargets.insert(l_pAttackConfirmWindow->ButtonYes());
         m_bStateRollBack = false;
         break;
      }
      // Attacked country units turned red
      case 12:
         //Pause the game
         l_pTutWin->CenterAndResize();
         g_ClientDCL.RequestGameSpeedChange(c_fSP2GamePausedSpeed);
         m_bStateRollBack = false;
         break;
      // Preparation is influenced by bigger armies and infrastructure
      case 13:
         break;
      // Notice as your unit arrive that it will be attacked
      case 14:
         m_bSpecificGoal = true;
         g_ClientDCL.RequestGameSpeedChange(c_fSP2HighTimeSpeed);
         //Select the target country and zoom a bit
         SetupEarthView(g_ClientDAL.CapitalPosition(g_ClientDAL.Country(c_iMilitaryTutorialTargetCountry).Id()),
                        c_fMilitaryTutorialZoomLevelAttack);   
         m_bStateRollBack = false;
         break;
      // Click the combat icon to enter the fight !
      case 15:
         m_bSpecificGoal = true;
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
         gassert(m_pTargetArenaInfo,"GTutorials::IterateMilitaryTutorial Should be a target arena info here");
         //Put an arrow beside the arena target
         l_pTutWin->PointToLatLong(m_pTargetArenaInfo->m_Coordinate);
		 l_pTutWin->MoveToBottom();
         m_bStateRollBack = false;
         break;
      // Battle has been paused
      case 16:
		 l_pTutWin->CenterAndResize();
         m_bStateRollBack = false;
         break;
      // We can see the units firing and getting hit
      case 17:
         m_bSpecificGoal = true;
         //Put the arrow beside the icon on the ally side
         {
            gassert(g_ClientDDL.BattleOverviewWindow(),"GTutorials::IterateMilitaryTutorial, there should be a battle overview window");
            SP2::GBattleScene& l_Scene = g_ClientDDL.BattleOverviewWindow()->m_BattleScene;
            SP2::GBattleOverviewUnit* l_pUnit = l_Scene.GetUnit(c_eMilitaryTutorialTargetUnitType,true);
            gassert(l_pUnit,"GTutorials::IterateMilitaryTutorial, unit should exist");
            GVector2D<REAL32> l_Position = l_pUnit->Position();
            l_Position.x +=   l_pUnit->Width()/2;
            l_pTutWin->PointTo(l_Position,g_ClientDDL.BattleOverviewWindow()->m_pBattleFrame);
            m_ValidEventTargets.insert(g_ClientDDL.BattleOverviewWindow()->m_pBattleFrame);
         }         
         break;
      // Selected unit position is displayed in this section
      case 18:
         l_pTutWin->PointTo(g_ClientDDL.BattleOverviewWindow()->m_pMapFrame);
         break;
      // We can make the units attack or defend
      case 19:
         //Put the arrow beside the attack defend icon
         {
            gassert(g_ClientDDL.BattleOverviewWindow(),"GTutorials::IterateMilitaryTutorial, there should be a battle overview window");
            SP2::GBattleScene& l_Scene = g_ClientDDL.BattleOverviewWindow()->m_BattleScene;
            SP2::GBattleOverviewUnit* l_pUnit = l_Scene.GetUnit(c_eMilitaryTutorialTargetUnitType,true);
            gassert(l_pUnit,"GTutorials::IterateMilitaryTutorial, unit should exist");
            l_pTutWin->PointTo(l_pUnit->Position() + l_pUnit->m_StatusIcon.Position(),g_ClientDDL.BattleOverviewWindow()->m_pBattleFrame);
         }
         break;
      // Differents units are more efficients than others against a certain unit type
      case 20:
         break;
      // Unit quantity explained
      case 21:
         //Put the arrow beside the quantity
         {
            gassert(g_ClientDDL.BattleOverviewWindow(),"GTutorials::IterateMilitaryTutorial, there should be a battle overview window");
            SP2::GBattleScene& l_Scene = g_ClientDDL.BattleOverviewWindow()->m_BattleScene;
            SP2::GBattleOverviewUnit* l_pUnit = l_Scene.GetUnit(c_eMilitaryTutorialTargetUnitType,true);
            gassert(l_pUnit,"GTutorials::IterateMilitaryTutorial, unit should exist");
            l_pTutWin->PointTo(l_pUnit->Position() + l_pUnit->m_UnitHP.Position(),g_ClientDDL.BattleOverviewWindow()->m_pBattleFrame);
         }
         break;
      // Describes the ally flags
      case 22:
         l_pTutWin->PointTo(g_ClientDDL.BattleOverviewWindow()->m_pAllyFlag);
         break;
      // Describes the who is winning bar ?
      case 23:
         l_pTutWin->PointTo((GUI::GBaseObject*)g_ClientDDL.BattleOverviewWindow()->m_pObjOverviewMeter);
         break;
      // Describes the strategy combo box utility
      case 24:
         l_pTutWin->PointTo(g_ClientDDL.BattleOverviewWindow()->m_pStrategies);
         break;
      // Describes the retreat button
      case 25:
         l_pTutWin->PointTo(g_ClientDDL.BattleOverviewWindow()->m_pRetreatButton);
         break;
      // Let battles unfold !
      case 26:
         m_bSpecificGoal = true;
         g_ClientDCL.RequestGameSpeedChange(c_fSP2HighestTimeSpeed);         
         m_bStateRollBack = false;
         break;
      // When battle is over, battle summery window is displayed
      case 27:
         l_pTutWin->BringToFront();
         l_pTutWin->MoveToTop();
         m_bStateRollBack = false;
         break;
      // Press the Tab key to see you have currently military control 
      case 28:  
         m_bSpecificGoal = true;       
         // No more battle summary
         if(g_ClientDDL.BattleSummaryWindow())
            g_ClientDDL.BattleSummaryWindowKill();
         // Be sure the overview is not here anymore
         if(g_ClientDDL.BattleOverviewWindow())            
            g_ClientDDL.BattleOverviewWindow()->Hide();              
         l_sText = l_sText.ReplaceNextPattern(g_ClientDAL.Country(c_iMilitaryTutorialTargetCountry).Name(),
                                              L"[country]");         
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
         m_ValidEventTargets.insert(l_pTutWin);
         g_ClientDDL.BackgroundEarth()->Focus();
         //Slow down the time
         g_ClientDCL.RequestGameSpeedChange(c_fSP2NormalTimeSpeed);
         break;
      // Only Annexation treaty can gives you political control
      case 29:
         // no need to show units anymore
         g_ClientDDL.BackgroundEarth()->HideAllCountryUnits();
         l_pTutWin->CenterAndResize();
         g_ClientDCL.m_EarthState.m_Zoom = c_fMilitaryTutorialZoomLevelSelect;
         g_ClientDDL.MiniMap()->UpdateZoomBar();
         g_ClientDCL.SelectedCountrySetLookAt(g_ClientDAL.Country(c_iMilitaryTutorialCountryToControl),true); 
         m_bStateRollBack = false;
         break;
      // Lets have a look at the military research window
      case 30:
         m_bSpecificGoal = true;
         // Make sure Military window is visible
         g_ClientDDL.MilitaryWindowSpawn();
         g_ClientDDL.MilitaryWindow()->Show();
         // Enable research button 
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjResearchBtn);
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjResearchBtn);  
         m_bSpecificGoal = true;
         break;
      // Military research in the budget is managed here
      case 31:    
         // enable first tab
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjGroundTog);
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjNavalTog);
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjStrategicTog);
         g_ClientDDL.UnitResearchWindow()->m_pObjGroundTog->BringToFront();
         g_ClientDDL.UnitResearchWindow()->m_pObjNavalTog->BringToFront();
         g_ClientDDL.UnitResearchWindow()->m_pObjStrategicTog->BringToFront();
         m_ValidEventTargets.clear();
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[0]->ToggleState(true);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]->ToggleState(false);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]->ToggleState(false);
         l_pTutWin->PointTo((GUI::GBaseObject*)g_ClientDDL.UnitResearchWindow()->m_vMainFinancing[0]);  
         m_bStateRollBack = false;
         break;
      // Click progress tab in unit research
      case 32:
         m_bSpecificGoal = true;          
         // Enable the progress tab  
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[0]->ToggleState(true);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]->ToggleState(false);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]->ToggleState(false);
         l_pTutWin->MoveToTop();
         l_pTutWin->PointTo(g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]);
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]);
         break;
      // This tab displays progress for each technology, click on the financing tab
      case 33:
         m_bSpecificGoal = true;            
         // Enable the finance tab  
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[0]->ToggleState(false);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]->ToggleState(true);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]->ToggleState(false);
         l_pTutWin->PointTo(g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]);
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]);
         break;
      // Fine tune distribution of the research money
      case 34:
         m_bSpecificGoal = true;
         // Enable the research tab  
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[0]->ToggleState(false);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[1]->ToggleState(false);
         g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->m_vTabButtons[2]->ToggleState(true);
         // Enable the exit button
         l_pTutWin->PointTo(g_ClientDDL.UnitResearchWindow()->m_pObjExitBtn);
         m_ValidEventTargets.insert(g_ClientDDL.UnitResearchWindow()->m_pObjExitBtn);
         break;
      // Lets have a look at the unit design
      case 35:
         m_bSpecificGoal = true;         
         //Make sure the tutorial country is actually selected
         g_ClientDCL.SelectedCountrySet(g_ClientDAL.Country(c_iMilitaryTutorialCountryToControl),true);         
         //Make sure military window is visible
         g_ClientDDL.MilitaryWindowSpawn();
         g_ClientDDL.MilitaryWindow()->Show();
         // Enable the design button
         l_pTutWin->CenterAndResize();
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjDesignBtn);
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjDesignBtn);
         m_bStateRollBack = false;
         break;
      // This window allows you to design new military units.
      // When new technologies are discovered, you will most likely be willing to 
      // include them in new designs that you will then be able to produce.
      case 36:
         l_pTutWin->MoveToTop();
         break;
      // This list lets you select within from the available unit types.
      case 37:
         // Point to unit type
         l_pTutWin->PointTo(g_ClientDDL.DesignWindow()->m_pObjUnitTypeCbo);  
         break;
      // This list lets you browse through your country’s existing Military Units 
      // military unit designs. They are regrouped grouped by type.
      case 38:
         // Point to unit name
         l_pTutWin->PointTo(g_ClientDDL.DesignWindow()->m_pObjUnitNameCbo); 
         break;
      // Finally, this text provides you with general information about the selected unit type. 
      // It explains the strengths and weaknesses that you need to know in order to be able to 
      // manage your units wisely on the battlefield.
      // Click the 'New design' button at the bottom of the window to continue.
      case 39:
         m_bSpecificGoal = true;
         // Point to unit  description
         l_pTutWin->PointTo(g_ClientDDL.DesignWindow()->m_pObjDescTxtLbl);
         // must click on new button to continue
         m_ValidEventTargets.insert(g_ClientDDL.DesignWindow()->m_pObjCreateDesignBtn);
         break;
      // Let’s create a new tank design.
      // Select ‘Tank’ from the Unit Type.
      case 40:
         m_bSpecificGoal = true; 
         // close the design window
         if(g_ClientDDL.DesignWindow())
            g_ClientDDL.DesignWindow()->Hide();
         // Enable the type cbo
         l_pTutWin->CenterAndResize();         
         l_pTutWin->PointTo(g_ClientDDL.CreateDesignWindow()->m_pDesignTypeCbo);         
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pDesignTypeCbo); 
         m_bStateRollBack = false;
         break;
      // Give a name to your new design and press ENTER.
      case 41:
         m_bSpecificGoal = true;
         // Enable the edit name button
         l_pTutWin->CenterAndResize();         
         l_pTutWin->PointTo(g_ClientDDL.CreateDesignWindow()->m_pObjNameEdt);         
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pObjNameEdt); 
         g_ClientDDL.CreateDesignWindow()->m_pObjNameEdt->Focus();
         m_bStateRollBack = false;
         break;
      // Now, assign technology dots to your design by clicking the dots in this area. 
      // Note that you can only include technologies that have been previously discovered 
      // using through research, espionage or acquired by trading with other countries.
      // The design’s unit cost is displayed at the bottom of the window.
      // Obviously, the price increases as you include more advanced technologies.
      // Once you are done assigning technology dots, click in the Tutorial window to continue.
      case 42:
         m_bSpecificGoal = true;       
         l_pTutWin->Position(0,0);
         l_pTutWin->PointTo(g_ClientDDL.CreateDesignWindow()->m_pTechnologyMeters[EUnitDesignCharacteristics::GunRange]);
         // Enable techno meters
         m_ValidEventTargets.insert(l_pTutWin);
         for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount ; i++)
         {
            m_ValidEventTargets.insert((GUI::GBaseObject*)g_ClientDDL.CreateDesignWindow()->m_pTechnologyMeters[i]);
         }       
         m_bStateRollBack = false;
         break;
      // You can select the parts that are used for the Units unit’s 3D representation above.
      // You can select the part types from this list and browse through the different 
      // pieces by using the arrows on the right side.
      // You can also change the camouflage color you wish to use for this representation.
      //Once you are done, click the ‘Confirm’ button to continue.
      case 43:
         m_bSpecificGoal = true;
         // valid targets for objects parts
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pObjLeftBtn);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pObjRightBtn);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pObjPieceTypeCbo);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pPartLeftFrm);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pPartMiddleFrm);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pPartRightFrm);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->Child(L"frmBuild"));
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->Child(L"frmBuild")->Child(L"frmParts"));
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->Child(L"frmBuild")->Child(L"frmTextures"));
         for(UINT32 i = 0 ; i < g_ClientDDL.CreateDesignWindow()->m_vpObjColorBtn.size() ; i++)
         {
            m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_vpObjColorBtn[i]);
            g_ClientDDL.CreateDesignWindow()->m_vpObjColorBtn[i]->BringToFront();
         }
         for(UINT32 i = 0 ; i < g_ClientDDL.CreateDesignWindow()->m_vpObjPartBtn.size() ; i++)
         {
            m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_vpObjPartBtn[i]);
            g_ClientDDL.CreateDesignWindow()->m_vpObjPartBtn[i]->BringToFront();
         }         
         // Enable the confirm creation button
         l_pTutWin->PointTo(g_ClientDDL.CreateDesignWindow()->m_pObjConfirmCreationBtn);
         m_ValidEventTargets.insert(g_ClientDDL.CreateDesignWindow()->m_pObjConfirmCreationBtn); 
         break;         
      // Now, let’s see how to build Units using those designs.
      // Open the Unit Production window by clicking on the ‘Build’ button.
      case 44:
      {
         m_bSpecificGoal = true;
         // Enable the build button
         l_pTutWin->CenterAndResize();         
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjBuildBtn);
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjBuildBtn);
         //Request the production of a couple units
         g_ClientDCL.RequestUnitProduction(c_iMilitaryTutorialDesignToProduce,
                                           1000, 0, c_iMilitaryTutorialCountryToControl,
                                           EUnitProductionPriority::Normal);
         m_bStateRollBack = false;
         break;
      }
      // Here, you can select Unit Designs, set a quantity and start the production.
      case 45:
         l_pTutWin->MoveToTop();
         gassert(g_ClientDDL.UnitProductionWindow() && g_ClientDDL.UnitProductionWindow()->Visible(),"unit prod window should exist and be visible");
         l_pTutWin->PointTo(g_ClientDDL.UnitProductionWindow()->m_pObjDesignTree);
         m_bStateRollBack = false;
         break;
      // Each country has a predetermined Production Capacity, calculated using the country’s 
      // Gross Domestic Product (GDP).
      // You can put as much Units as you wish in the Production Queue. However, orders that 
      // exceed the country’s Production Capacity are put on hold until other productions are completed.
      case 46:
         l_pTutWin->PointTo(g_ClientDDL.UnitProductionWindow()->m_pObjProductionList);
         break;
      // You can adjust the Priority Level of your productions here. 
      // It determines in which order your productions are started.
      case 47:
         // if the unit is currently in the list, point on it
         if(g_ClientDDL.UnitProductionWindow()->m_pObjProductionList->Get_Nb_Rows() > 0)
            l_pTutWin->PointTo(g_ClientDDL.UnitProductionWindow()->m_pObjProductionList->Get_Row_At(0)->m_pObject->Child(L"cboPriority"));
         // if not, (maybe the user left clicked too fast ...) point on the list
         else
            l_pTutWin->PointTo(g_ClientDDL.UnitProductionWindow()->m_pObjProductionList);
         break;
      // Once the productions are completed, they are put in the Deployment List, 
      // waiting to be placed on the globe. Close this window to continue.
      case 48:         
         m_bSpecificGoal = true;
         // Enable the close button
         l_pTutWin->CenterAndResize();
         l_pTutWin->PointTo(g_ClientDDL.UnitProductionWindow()->m_pObjExitBtn);
         m_ValidEventTargets.insert(g_ClientDDL.UnitProductionWindow()->m_pObjExitBtn);
         break;
      // Open the Unit List by clicking the ‘More’ button here.
      case 49:         
         m_bSpecificGoal = true;
         //Make sure military window is visible
         g_ClientDDL.MilitaryWindowSpawn();
         g_ClientDDL.MilitaryWindow()->Show();    
         // Enable the More button
         l_pTutWin->Position(0,0);
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjConventionalForcesMoreBtn);
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjConventionalForcesMoreBtn);
         m_bStateRollBack = false;
         break;
      // This list contains all of your military units and provides information about them.
      // Click the ‘Deploy’ button to access the Deployment List.
      case 50:
         m_bSpecificGoal = true;
         // Enable the Deploy button
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjDeployBtn);
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjDeployBtn);
         break;
      // From here, you can simply drag & drop the units on the globe to deploy them.
      // When they have just been produced, the units have a very low training level. 
      // They come out as Recruits and suffer efficiency penalties in combat.
      // They can be used as is, but it’s recommended to train them before deploying them.
      case 51:
         m_bStateRollBack = false;
         break;
      // There are four different training levels: Recruit, Regular, Veteran and Elite.
      // You can train your units by clicking the ‘Train’ button here.
      // Training takes time and cost money, especially for the higher levels of training.
      // It can be a good strategic advantage, but it comes at a price.
      // It’s recommended to train your units to Regular so they suffer no penalties.
      case 52:
         l_pTutWin->PointTo(g_ClientDDL.UnitGroundDeploymentWindow()->m_pObjTrainingBtn);
         break;
      // Units that have already been deployed can still be trained by using the 
      // ‘Train’ button from the Unit List window.
      case 53:
         break;
      // Close both the Deployment List and the Unit List windows to continue
      case 54:
         m_bSpecificGoal = true;
         // Enable the 2 exits buttons
         m_ValidEventTargets.insert(g_ClientDDL.UnitGroundDeploymentWindow()->m_pObjExitBtn);
         m_ValidEventTargets.insert(g_ClientDDL.UnitListWindowNew()->m_pObjExitBtn);
         m_bStateRollBack = false;
         break;
      // This section displays the status of your nuclear program. This is where, provided you have 
      // the necessary funds, you can develop an Anti-Missile Defense System (AMDS) 
      // which offers a certain level of protection against incoming enemy strategic missiles.
      // Note that Strategic Warfare is only available when the Nuclear Program research has been completed.
      // Since the United States already possess a nuclear arsenal, click on this button to access the
      // Strategic Warfare mode.
      case 55:
         m_bSpecificGoal = true;
         // Enable Strategic Warfare button
         m_ValidEventTargets.insert(g_ClientDDL.MilitaryWindow()->m_pObjStrategicWarfareBtn);
         l_pTutWin->PointTo(g_ClientDDL.MilitaryWindow()->m_pObjStrategicWarfareBtn);
         break;
      // Launch nuclear attack on china
      case 56:
         m_bSpecificGoal = true;
         // list box exists and more than one row ??
         if(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry->GetListBox() &&
            g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry->GetListBox()->Get_Row_At(0))
            g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry->GetListBox()->Get_Row_At(0)->Object()->Enabled(false);
         // Enable country cbo
         l_pTutWin->PointTo(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry);
         m_ValidEventTargets.insert(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry); 
         m_bStateRollBack = false;
         break;
      // Select attack type
      case 57:
         m_bSpecificGoal = true;
         // Enable attack type cbo
         l_pTutWin->PointTo(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjAttackType);
         m_ValidEventTargets.insert(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjAttackType);
         break;
      // Select missile type
      case 58:
         m_bSpecificGoal = true;
         // Enable missile type cbo
         l_pTutWin->PointTo(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjMissileTypeCbo);
         m_ValidEventTargets.insert(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjMissileTypeCbo);         
         break;    
      // Show quantity can be modified
      case 59:
         l_pTutWin->PointTo(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjAttackCursor);
         break;
      // Prepare the nuclear launch
      case 60:
         m_bSpecificGoal = true;
         // Enable prepare for launch button
         l_pTutWin->PointTo(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjPrepareLaunchBtn);
         m_ValidEventTargets.insert(g_ClientDDL.StrategicTargetCountryWindow()->m_pObjPrepareLaunchBtn);
         m_bStateRollBack = false;
         break;
      // Launch the missiles
      case 61:
         m_bSpecificGoal = true;
         // Enable launch button
         l_pTutWin->PointTo(g_ClientDDL.StrategicLaunchWindow()->m_pObjLaunchFrm);
         m_ValidEventTargets.insert(g_ClientDDL.StrategicLaunchWindow()->m_pObjLaunchFrm);
         break;
      // See the results
      case 62:
         // Earth will be unlocked from now on
         g_ClientDDL.BackgroundEarth()->UnlockEarthPosition();
         // Can rotate the globe
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
         break;
      // See China retaliate
      case 63:
         // Can rotate the globe
         m_ValidEventTargets.insert(g_ClientDDL.BackgroundEarth());
         break;
      // End military tutorial
      default:
         StopTutorial();
         return;
   };
   // display tutorial window
   SetupTutorialWindowText(l_sText, l_pTutWin);
}

/*!
 * Checks if we can advance the military tutorial
 **/
bool GTutorials::MilitaryTutorialCanObjectRecvEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,GEventData & in_EventData)
{
  switch (m_State)
   {
      //Only allow left click to select something
      case 3: //must select unit group
      case 15://The battle is started, must select the battle icon
      {
         if((in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnMouseDown) && (in_EventData.Mouse.Actor.Bits.Left))
            return true;
         break;
      }
      //Allow input on any target that is in the Valid Target List
      case 6: //Right click and set status to ready
      case 10://Move to Cuba
      case 11:
      case 17:
      case 26:      
      case 30:
      case 32://Click the progress tab (research window)
      case 33://Click the financing tab (research window)
      case 34://Must close the window to continue (research window)
      case 35://Make sure design window is open to continue
      case 39://Make sure Create design window is opened to continue (click new design button)
      case 40://Tank type      
      case 42://Allow to modify the technology dots and the pieces of the design
      case 43://Make sure the design window is closed to continue     
      case 44://Open the build window by pressing the build button      
      case 48://Wait for the unit production window to be closed to continue
      case 49://Wait for the unit list to open      
      case 50://Wait until deploy window is opened      
      case 54://Wait until ground deployment window and unit list window are closed to continue      
      case 55://Must wait until the strategic warfare view switch is complete
      case 56://Wait until the country : China is selected in the strategic warfare country combo box      
      case 57://Wait until military is selected from the attack type combo box
      case 58://Wait until the missile type is set to ICBM only
      case 60://Wait until the confirm launch window is displayed
      case 61://Wait for the missiles to be launched
      case 62://Watch fro attack results
      case 63://See China retaliate
         return true;
         break;
      case 41://Enter a name for your new design press enter to continue
         //Only allow keyboard input
         if(in_EventData.Key() == VK_RETURN)
         {
            m_bGoalReached = true;
         }
         return true;
         break;
      case 28:
         //Only allow keyboard input
         if(in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnKeyDown || in_eKeyOrMouseEvent == EKeyAndMouseEvents::OnKeyUp )
            return true;
         break;
      //Dont allow input
      default:
         return false;
         break;
   }
   return false;
}

/*!
 * Says if clicking the tutorial window on specific goal advances military tutorial
 **/
bool GTutorials::ClickingTutorialWindowOnSpecificGoalAdvancesMilitaryTutorial()
{
   switch(m_State)
   {
      case 4:
      case 28:
      case 42:
         return true;
      default:
         return false;
   }
}

/*!
 * Checks if the military conditions for a status change are met
 **/
void GTutorials::IterateMilitaryTutorialConditions()
{
   // switch states, check for conditions
   switch (m_State)
   {
      case 3:
      {
         //Check if the unit is selected
         const set<UINT32>& l_SelectedUnits = g_ClientDAL.SelectedUnitsID();
         if( (l_SelectedUnits.size() == 1) && 
             (*l_SelectedUnits.begin() == c_iMilitaryTutorialTargetUnitGroup))
         {
            m_bGoalReached = true;
         }
         break;
      }
      case 6:
      {
         //If the group next status is ready, objective met         
         if(m_pMilitaryTargetGrp->NextStatus() == EMilitaryStatus::Ready)
         {
            m_bGoalReached = true;
            g_ClientDCL.RequestGameSpeedChange(c_fSP2HighestTimeSpeed);         
         }
         break;
      }
      case 7:
      {
         //If the group next status is ready, objective met        
         //If the next status is ready and the time for the next status was reached
         if( (m_pMilitaryTargetGrp->NextStatus() == EMilitaryStatus::Ready) &&
             (m_pMilitaryTargetGrp->StatusChangeEndTime() <= g_Joshua.GameTime() ))
         {
            m_bGoalReached = true;
            g_ClientDCL.RequestGameSpeedChange(c_fSP2NormalTimeSpeed);
         }
         break;
      }
      case 10:
      {
         //Check if the unit is selected
         const set<UINT32>& l_SelectedUnits = g_ClientDAL.SelectedUnitsID();
         if( (l_SelectedUnits.size() == 1) && 
             (*l_SelectedUnits.begin() == c_iMilitaryTutorialTargetUnitGroup))
         {
            // Point to enemy capital
            m_pTutorialWindow->PointToLatLong(g_ClientDAL.CapitalPosition(
               g_ClientDAL.Country(c_iMilitaryTutorialTargetCountry).Id()));
         }
         else
         {
            // point to unit to select
            m_pTutorialWindow->PointToLatLong(m_pMilitaryTargetGrp->Position());
         }

         //only allow to advance if the given unit group requested a move
         //to mexico
         if(g_ClientDDL.UnitAttackConfirmWindow(c_iMilitaryTutorialTargetCountry))
         {
            m_bGoalReached = true;
         }
         else if(g_ClientDDL.UnitAttackConfirmWindowsSize())
         {
            //Asked to attack another country, not the good one, cancel the move automatically
            g_ClientDDL.UnitAttackConfirmWindowKillAll();
         }
         break;
      }
      case 11:
      {
         //If the window is no longer present, it means the goal was reached
         if(!g_ClientDDL.UnitAttackConfirmWindow(c_iMilitaryTutorialTargetCountry))
         {
            m_bGoalReached = true;            
         }
         break;
      }

      //A battle should begin, wait until battle begins
      case 14:
      {
         //There should be a combat arena somewhere in mexico
         const stdext::hash_map<UINT32, Combat::GArenaInfo*>& l_ArenaInfos = g_Joshua.UnitManager().ArenaInfos();
         //For each arena, check if its in mexico
         for(hash_map<UINT32,Combat::GArenaInfo*>::const_iterator l_It = l_ArenaInfos.begin();
             l_It != l_ArenaInfos.end();
             l_It++)
         {
            SP2::GArenaInfo* l_pInfo = (SP2::GArenaInfo*)l_It->second;
            const set<UINT32>& l_TargetCountryRegions = g_ClientDAL.CountryPoliticalControl(c_iMilitaryTutorialTargetCountry);
            if(find(l_TargetCountryRegions.begin(),l_TargetCountryRegions.end(),l_pInfo->m_iRegionID) != l_TargetCountryRegions.end())
            {
               m_pTargetArenaInfo = (SP2::GArenaInfo*)l_It->second;
               m_bGoalReached = true;
               g_ClientDCL.RequestGameSpeedChange(c_fSP2GamePausedSpeed);
               break;
            }
         }
         break;
      }
      //The battle is started, must select the battle icon
      case 15:
      {
         //Check if the unit is selected
         gassert(m_pTargetArenaInfo,"GTutorials::CanAdvanceMilitaryTutorial Should be a valid target arena info");
         UINT32 l_iSelectedCombat = g_ClientDAL.SelectedCombatID();
         if(l_iSelectedCombat == m_pTargetArenaInfo->m_iId)
         {
            m_bGoalReached = true;
         }
         else
         {
            // Hack, be sure we clicked on the good arena
            if(g_ClientDDL.BattleOverviewWindow() && g_ClientDDL.BattleOverviewWindow()->Visible())
               g_ClientDDL.BattleOverviewWindow()->Hide();
         }
         break;
      }
      case 17:
      {
         SP2::GBattleOverviewUnit* l_pSelectedUnit = g_ClientDDL.BattleOverviewWindow()->m_pSelectedUnit;
         if(l_pSelectedUnit  && ((l_pSelectedUnit->m_iTypeID+1) == c_eMilitaryTutorialTargetUnitType))
         {
            m_bGoalReached = true;
         }
         break;
      }

      case 26:
      {        
         // allow to close only if battle is over !
         if(g_ClientDDL.BattleSummaryWindow() && g_ClientDDL.BattleSummaryWindow()->Visible())
         {
            // goal has been reached
            m_bGoalReached = true;
         }
         break;
      }
      case 30:
      {
         if(g_ClientDDL.UnitResearchWindow() &&
            g_ClientDDL.UnitResearchWindow()->Visible())
         {
            m_bGoalReached = true;
            break;
         }
      }
      case 32:
      {
         //Check if the condition was reached (if the progress tab is active)
         if(g_ClientDDL.UnitResearchWindow() && g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->ActiveTab() == c_iMilitaryTutorialResearchPageProgressTabId)
         {
            m_bGoalReached = true;            
         }
         break;
      }
      case 33:
      {
         //Check if the condition was reached (if the progress tab is active)
         if(g_ClientDDL.UnitResearchWindow() && g_ClientDDL.UnitResearchWindow()->m_pObjResearchTab->ActiveTab() == c_iMilitaryTutorialResearchPageFinancingTabId)
         {
            m_bGoalReached = true;
         }
         break;
      }

      //Must close the window to continue
      case 34: 
      {
         //Check if the research window has been closed
         if(!g_ClientDDL.UnitResearchWindow() || !g_ClientDDL.UnitResearchWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }

      //Make sure design window is open to continue
      case 35:
      {
         if(g_ClientDDL.DesignWindow() && g_ClientDDL.DesignWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Make sure the create design window is visible to continue
      case 39:
      {
         // is our new design window openend ?
         if(g_ClientDDL.CreateDesignWindow() && g_ClientDDL.CreateDesignWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      // make sure tank is selected
      case 40:
      {
         // make sure tank is selected
         if(g_ClientDDL.CreateDesignWindow()->m_pDesignTypeCbo->SelectedRow() && 
            g_ClientDDL.CreateDesignWindow()->m_pDesignTypeCbo->SelectedRow()->m_iAdditionalId == (UINT32)EUnitType::Tank)            
         {
            m_bGoalReached = true;
         }
         break;         
      }
      //Make sure the design window is closed to continue
      case 43:
      {
         if(!g_ClientDDL.CreateDesignWindow() || !g_ClientDDL.CreateDesignWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Open the build window by pressing the build button
      case 44:
      {
         if(g_ClientDDL.UnitProductionWindow() && g_ClientDDL.UnitProductionWindow()->Visible())
         {
            m_bGoalReached = true;
            //Speed up the time to make sure we'll have units in the deployment list
            g_ClientDCL.RequestGameSpeedChange(c_fSP2HighestTimeSpeed);
         }
         break;
      }
      //Wait for the unit production window to be closed to continue
      case 48:
      {
         if(!g_ClientDDL.UnitProductionWindow() || !g_ClientDDL.UnitProductionWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait for the unit list to open
      case 49:
      {
         //Hack, bring the more button to front
         if(g_ClientDDL.MilitaryWindow())
            g_ClientDDL.MilitaryWindow()->m_pObjConventionalForcesMoreBtn->BringToFront();

         if(g_ClientDDL.UnitListWindowNew() && g_ClientDDL.UnitListWindowNew()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until deploy window is opened
      case 50:
      {
         //Hack, send the unit list to back
         if(g_ClientDDL.UnitListWindowNew())
         {
            g_ClientDDL.UnitListWindowNew()->SendToBack();
            g_ClientDDL.BackgroundEarth()->SendOverlayToBack();
         }

         if(g_ClientDDL.UnitGroundDeploymentWindow() && g_ClientDDL.UnitGroundDeploymentWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until ground deployment window and unit list window are closed to continue
      case 54:
      {
         bool l_bDeploymentClosed = false;
         bool l_bUnitListClosed   = false;

         if(!g_ClientDDL.UnitGroundDeploymentWindow() || !g_ClientDDL.UnitGroundDeploymentWindow()->Visible())
         {
            l_bDeploymentClosed = true;
         }
         if(!g_ClientDDL.UnitListWindowNew() || !g_ClientDDL.UnitListWindowNew()->Visible())
         {
            l_bUnitListClosed = true;
         }
         if(l_bDeploymentClosed && l_bUnitListClosed)
         {
            m_bGoalReached = true;
            //slow done the time again to the normal time
            g_ClientDCL.RequestGameSpeedChange(c_fSP2NormalTimeSpeed);
         }
         break;
      }
      //Must wait until the strategic warfare view switch is complete
      case 55:
      {
         if(g_ClientDDL.StrategicTargetCountryWindow() && g_ClientDDL.StrategicTargetCountryWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until the country : China is selected in the strategic warfare country combo box
      case 56:
      {
         const GCountry& l_TargetNuclearCountry = g_ClientDAL.Country(c_iMilitaryTutorialNuclearTargetCountry);
         if(g_ClientDDL.StrategicTargetCountryWindow() && g_ClientDDL.StrategicTargetCountryWindow()->m_pObjCountry->Selected_Content() == l_TargetNuclearCountry.Name())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until military is selected from the attack type combo box
      case 57:
      {
         if(g_ClientDDL.StrategicTargetCountryWindow()->m_TargetType == ENuclearTargetType::Military)
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until the missile type is set to ICBM only
      case 58:
      {
         if(g_ClientDDL.StrategicTargetCountryWindow()->m_eMissileTypeToUse == ENuclearMissileType::Standard)
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait until the confirm launch window is displayed
      case 60:
      {
         if(g_ClientDDL.StrategicLaunchWindow() && g_ClientDDL.StrategicLaunchWindow()->Visible())
         {
            m_bGoalReached = true;
         }
         break;
      }
      //Wait for the missiles to be launched
      case 61:
      {
         //Hack, window was not in front
         if(g_ClientDDL.StrategicLaunchWindow())
         {
            g_ClientDDL.StrategicLaunchWindow()->BringToFront();
         }
         if(!g_ClientDDL.StrategicLaunchWindow() || !g_ClientDDL.StrategicLaunchWindow()->Visible())
         {//window is closed
            m_bGoalReached = true;
            //Zoom out
            g_ClientDCL.m_EarthState.m_Zoom = 0.1f;
            if(g_ClientDDL.MiniMap())
		         g_ClientDDL.MiniMap()->UpdateZoomBar();
         }
         break;
      }
      default:
      {
         return;
      }
   }

}