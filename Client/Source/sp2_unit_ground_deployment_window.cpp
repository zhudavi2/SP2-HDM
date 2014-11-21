/**************************************************************
*
* sp2_unit_ground_deployment_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes
#include "golem_pch.hpp"
#include "../include/sp2_unit_ground_deployment_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_gui_helper.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_generic_question_window.h"

// consts
const UINT32 c_iItemHeight = 15;
const GString c_sDropCursor = L"/data/cursors/deploy.ani";
const GString c_sNoCursor  = L"/data/cursors/MoveNo.cur";
const GString GUnitGroundDeploymentWindow::TypeName(L"Unit_Ground_Deployment_Window");


GUI::GBaseObject* GUnitGroundDeploymentWindow::New()
{
   return new GUnitGroundDeploymentWindow();
}

GUnitGroundDeploymentWindow::GUnitGroundDeploymentWindow() : SP2::GBaseWindow()
{
   // Attach to unit manager to get notify when groups are deleted or moved
   g_Joshua.UnitManager().Attach(this, Combat::c_notificationUnitGroupModified);

   m_bMouseIsDragging   = false;
   g_Joshua.FileManager()->File(c_sDropCursor,m_DropCursorFile);
   g_Joshua.FileManager()->File(c_sNoCursor,m_InvalidCursorFile);

   // Setup logo 
   m_UnitDesignIDLogo[SP2::EUnitCategory::Infantry]= "000823";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Air]     = "000778";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Ground]  = "000779";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Naval]   = "000780";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Nuclear] = "000781";

   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Regular] = "000825";
   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Veteran] = "000826";
   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Elite]   = "000827";

   m_pSelectedList = NULL;
}

GUnitGroundDeploymentWindow::~GUnitGroundDeploymentWindow()
{
   g_Joshua.UnitManager().Detach(this, Combat::c_notificationUnitGroupModified);
}

bool GUnitGroundDeploymentWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjApplyBtn->Hide();
   
   // Collect gui items
   m_objTabGroup              = (GUI::GTabbed*)Child(L"frmInner")->Child(L"tabGroups");
   m_pObjGroundList           = (GUI::GListBox*)m_objTabGroup->m_vTabPage[0]->Child("lstDeploy");
   m_pObjAirList              = (GUI::GListBox*)m_objTabGroup->m_vTabPage[1]->Child("lstDeploy");
   m_pObjNavalList            = (GUI::GListBox*)m_objTabGroup->m_vTabPage[2]->Child("lstDeploy");
   m_pObjNuclearList          = (GUI::GListBox*)m_objTabGroup->m_vTabPage[3]->Child("lstDeploy");
   m_pObjDeployToCapitalBtn   = (GUI::GButton*)Child(L"frmInner")->Child(L"btnDeploy");
   m_pObjTrainingBtn          = (GUI::GButton*)Child(L"frmInner")->Child(L"btnTrain");

   m_objTabGroup->m_vTabPage[0]->Borders( GBorders(1, 1, 1, 1) );
   m_objTabGroup->m_vTabPage[1]->Borders( GBorders(1, 1, 1, 1) );
   m_objTabGroup->m_vTabPage[2]->Borders( GBorders(1, 1, 1, 1) );
   m_objTabGroup->m_vTabPage[3]->Borders( GBorders(1, 1, 1, 1) );

   (GUI::GListBox*)m_objTabGroup->m_vTabPage[0]->PictureNormal("000647");
   (GUI::GListBox*)m_objTabGroup->m_vTabPage[1]->PictureNormal("000647");
   (GUI::GListBox*)m_objTabGroup->m_vTabPage[2]->PictureNormal("000647");
   (GUI::GListBox*)m_objTabGroup->m_vTabPage[3]->PictureNormal("000647");

   m_pObjQuantityByCategory[0] = (GUI::GLabel*)Child(L"frmInner")->Child(L"txtQty");
   m_pObjQuantityByCategory[1] = (GUI::GLabel*)Child(L"frmInner")->Child(L"txtQty1");
   m_pObjQuantityByCategory[2] = (GUI::GLabel*)Child(L"frmInner")->Child(L"txtQty2");
   m_pObjQuantityByCategory[3] = (GUI::GLabel*)Child(L"frmInner")->Child(L"txtQty3");

   return true;
}

void GUnitGroundDeploymentWindow::OnShow()
{
   __super::OnShow();

   // nothing selected at first
   m_pSelectedList = NULL;
   CleanSelection();
   // Update units list
   UpdateReadyToDeployGroup();
}

//! Clean selection in listbox 
void GUnitGroundDeploymentWindow::CleanSelection()
{
   // Clear selection
   m_SelectedUnit.clear();
   m_SelectedRow.clear();
   // clear list box selection
   if(m_pSelectedList)
   {
      m_pSelectedList->ClearSelection();
      m_pSelectedList = NULL;
   }
   // Disable Button since there is nothing selected
   m_pObjDeployToCapitalBtn->Enabled(false);
   // Can we deploy missiles ?
   m_objTabGroup->m_vTabButtons[3]->Enabled(g_ClientDAL.GameOptions().NuclearAllowed());
}


void GUnitGroundDeploymentWindow::BuildTreeView(void)
{
   // Those total are used for optimizing the listbox (we are not destroying or creating unnecessary list item)
   INT32 l_iTotalGroundRow   = 0;
   INT32 l_iTotalAirRow      = 0;
   INT32 l_iTotalNavalRow    = 0;
   INT32 l_iTotalStrategicRow= 0;
   
   // Clear corespondance between row and Unit
   m_UnitByRow.clear();
   m_vTrainingUnitProgress.clear();

   hash_set<UINT32>  l_CopyOfHasBeenTrained = m_HasBeenTrained;
   m_HasBeenTrained.clear();

   // Find group that contain unit production queue
   const set<UINT32>& l_GroupId = g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID());
   set<UINT32>::const_iterator  l_GroupIdIterator = l_GroupId.begin();
   while(l_GroupIdIterator != l_GroupId.end()) 
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup *)g_Joshua.UnitManager().UnitGroup(*l_GroupIdIterator);
      if(l_pGroup->Status() == EMilitaryStatus::ReadyToDeploy && l_pGroup->Units().size() >0)
      {
         // Get vector of units.
         vector<SDK::Combat::GUnit*> l_UnitVector =  l_pGroup->Units();
         vector<SDK::Combat::GUnit*>::iterator l_UnitIterator =l_UnitVector.begin();
         while(l_UnitIterator != l_UnitVector.end())
         {
            // Get Units information
            SP2::GUnit* l_pUnit = (SP2::GUnit*)*l_UnitIterator;
            GListBox::GRow*   l_pRow = NULL;
            SP2::GUnitDesign* l_pUnitDesign =(SP2::GUnitDesign*) l_pUnit->Design();
   
            if(g_ClientDAL.IsUnitForSale(l_pUnit->Id()))
            {
               l_UnitIterator++;
               continue;
            }

            switch(l_pUnitDesign->Type()->Category())
            {
               case EUnitCategory::Air :
                  l_pRow = m_pObjAirList->Get_Row_At(l_iTotalAirRow);
                  if(l_pRow == NULL)
                  {
                     l_pRow = m_pObjAirList->Insert_New_Row();
                  }
                  l_iTotalAirRow++;
                  break;
               case EUnitCategory::Ground:
               case EUnitCategory::Infantry:
                  l_pRow = m_pObjGroundList->Get_Row_At(l_iTotalGroundRow);
                  if(l_pRow == NULL)
                  {
                     l_pRow = m_pObjGroundList->Insert_New_Row();
                  }
                  l_iTotalGroundRow++;
                  break;
               case EUnitCategory::Naval:
                  l_pRow = m_pObjNavalList->Get_Row_At(l_iTotalNavalRow);
                  if(l_pRow == NULL)
                  {
                     l_pRow = m_pObjNavalList->Insert_New_Row();
                  }
                  l_iTotalNavalRow++;
                  break;
               case EUnitCategory::Nuclear:
                  l_pRow = m_pObjNuclearList->Get_Row_At(l_iTotalStrategicRow);
                  if(l_pRow == NULL)
                  {
                     l_pRow = m_pObjNuclearList->Insert_New_Row();
                  }
                  l_iTotalStrategicRow++;
                  break;
               default:
                  gassert(false,"Where the hell am I suppose to insert this unit");
            }
            
            GUI::GPictureBox*                l_pPictureBox     = (GUI::GPictureBox*)l_pRow->Object()->Child("picType");
            GUI::GLabel*                     l_pUnitName       = (GUI::GLabel*)l_pRow->Object()->Child("txtName");
            GUI::GPictureBox*                l_pTrainingLevel  = (GUI::GPictureBox*) l_pRow->Object()->Child("picTraining");
            GProgressMeterYellowValue*       l_pProgressBar    = (GProgressMeterYellowValue*)l_pRow->Object()->Child("frmProgress");

            REAL32 l_fTrainingProgress;
            if(GetUnitTrainingProgress(l_pUnit->Id(),l_fTrainingProgress))
            {
               if(l_fTrainingProgress >1)
               {
                  l_pProgressBar->TextValueObj()->Text(g_ClientDAL.GetString(102137));
                  l_pProgressBar->Value(1,0,false,false);
               }
               else
               {
                  m_vTrainingUnitProgress.push_back(make_pair(l_pUnit->Id(), l_pProgressBar) );
                  l_pProgressBar->TextValueObj()->Text(g_ClientDAL.GetString(102138));
                  l_pProgressBar->Value(l_fTrainingProgress,0,false,false);
               }
            }
            else if(l_CopyOfHasBeenTrained.count(l_pUnit->Id()) > 0)
            {
               l_pProgressBar->TextValueObj()->Text(g_ClientDAL.GetString(102137));
               l_pProgressBar->Value(1,0,false,false);
            }
            else
            {
               l_pProgressBar->TextValueObj()->Text("");
               l_pProgressBar->Value(0,0,false,false);
            }

            if(l_pUnit->Training() > ETrainingLevel::Recruit)
            {
               l_pTrainingLevel->PictureNormal(m_UnitTrainingIDLogo[l_pUnit->Training()]);
               l_pTrainingLevel->BringToFront();
            }
            l_pPictureBox->PictureNormal(m_UnitDesignIDLogo[l_pUnitDesign->Type()->Category()]);
            l_pUnitName->Text(GString(l_pUnit->Qty()) + L" " + l_pUnitDesign->Name());
            ((GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));

            m_UnitByRow.insert(make_pair(l_pRow,l_pUnit));

            // Copy trained units that are still present in the current deploy group
            if(l_CopyOfHasBeenTrained.count(l_pUnit->Id() ) > 0)
            {
               m_HasBeenTrained.insert(l_pUnit->Id() );
            }

            l_UnitIterator++;
         }

         // We are done here, there is usually one group of this kind.
         break;
      }
      l_GroupIdIterator++;
   }

   // Remove last row until we have all the necessary row shown on the screen
   while(m_pObjGroundList->Get_Nb_Rows() > l_iTotalGroundRow)
      m_pObjGroundList->RemoveRow(m_pObjGroundList->Get_Nb_Rows()-1);
   while(m_pObjAirList->Get_Nb_Rows() > l_iTotalAirRow)
      m_pObjAirList->RemoveRow(m_pObjAirList->Get_Nb_Rows()-1);
   while(m_pObjNavalList->Get_Nb_Rows() > l_iTotalNavalRow)
      m_pObjNavalList->RemoveRow(m_pObjNavalList->Get_Nb_Rows()-1);
   while(m_pObjNuclearList->Get_Nb_Rows() > l_iTotalStrategicRow)
      m_pObjNuclearList->RemoveRow(m_pObjNuclearList->Get_Nb_Rows()-1);

   // Reselect Everything
   if(m_pSelectedList)
      m_pSelectedList->Select_Items(m_SelectedRow);

   // Update list box to reselect properly and show updated stuff
   m_pObjAirList->Update();
   m_pObjNavalList->Update();
   m_pObjGroundList->Update();
   m_pObjNuclearList->Update();

   m_pObjQuantityByCategory[0]->Text(GString(l_iTotalGroundRow));
   m_pObjQuantityByCategory[1]->Text(GString(l_iTotalAirRow));
   m_pObjQuantityByCategory[2]->Text(GString(l_iTotalNavalRow));
   m_pObjQuantityByCategory[3]->Text(GString(l_iTotalStrategicRow));
}

void GUnitGroundDeploymentWindow::UpdateReadyToDeployGroup(void)
{
   // Insert data 
   BuildTreeView();
   // Iterate training units
   IterateUnitsInTraining();
}

GUI::EEventStatus::Enum GUnitGroundDeploymentWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch (in_EventID)
   {
   case ECustomEvents::OnListItemSelected:
      {
         // retrieve last the combo
         m_pSelectedList = (GListBox*)in_pCaller;

         // Clean last Selection
         m_SelectedUnit.clear();         
         // Transfert new selection
         m_SelectedRow = m_pSelectedList->Get_Selected_Item();

         // Update new Selection
         for(UINT32 i=0;i<m_SelectedRow.size();i++)
         {
            GListBox::GRow* l_pRow = m_pSelectedList->Get_Row_At(m_SelectedRow[i]);
            m_SelectedUnit.insert(m_UnitByRow[l_pRow]);
         }   

		   // Enable deploy to capital button only when the category is not naval
		   if(m_SelectedUnit.size() >= 1 && (m_objTabGroup->ActiveTab() != 3))
         {
            m_pObjDeployToCapitalBtn->Enabled(true);
         }
         else
         {
            m_pObjDeployToCapitalBtn->Enabled(false);
         }
      }
      break;
 
   default: 
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);      
   }

   return GUI::EEventStatus::Handled;
}


GUI::EEventStatus::Enum GUnitGroundDeploymentWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   // Just in case, reset mouse pointer
   g_ClientDDL.ResetMousePointer();

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         // deploy button clicked
         if(in_pCaller->Name() == m_pObjDeployToCapitalBtn->Name())
         {
            if(m_SelectedUnit.size() > 0)
            {            
               // Get Capital Position
               GVector2D<REAL32> l_CapitalLocation = g_ClientDAL.CapitalPosition(g_ClientDAL.ControlledCountryID());

               // Deploy selection there
               DeployUnitToEarthLocation(l_CapitalLocation);
            }

         }
         // one of the tab button clicked
         else if( in_pCaller->Name() == m_objTabGroup->m_vTabButtons[0]->Name() || 
                  in_pCaller->Name() == m_objTabGroup->m_vTabButtons[1]->Name() ||
                  in_pCaller->Name() == m_objTabGroup->m_vTabButtons[2]->Name() ||
                  in_pCaller->Name() == m_objTabGroup->m_vTabButtons[3]->Name())
         {
            // clean what was selected
            CleanSelection();
         }
         // one of the tab button clicked
         else if(in_pCaller == m_pObjTrainingBtn)
         {
            if(m_SelectedUnit.size() > 0)
            {
               list<UINT32> l_UnitIDToTrain;
               set<SP2::GUnit*>::iterator l_SelectedUnitIterator= m_SelectedUnit.begin();
               while(l_SelectedUnitIterator != m_SelectedUnit.end())
               {
                  REAL32 l_fNotUsed;
                  // If this selected unit is not in training
                  if(GetUnitTrainingProgress((*l_SelectedUnitIterator)->Id(),l_fNotUsed) == false)
                  {
                     // Add it to the training list
                     l_UnitIDToTrain.push_back((*l_SelectedUnitIterator)->Id());
                  }

                  l_SelectedUnitIterator++;
               }
               if(l_UnitIDToTrain.size() >0)
               {
                  g_ClientDCL.TrainUnits(l_UnitIDToTrain, this,this);
               }
            }
         }
      }
   } 

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitGroundDeploymentWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseDown(in_EventData, in_pCaller);  

   // Start the drag coordinate
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      m_StartDragCoord = in_EventData.Mouse.Position;

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitGroundDeploymentWindow::OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseMove(in_EventData, in_pCaller);  

   if(in_EventData.Mouse.Down.Bits.Left)
   {
      if (!m_bMouseIsDragging)
      {
         // If we go over the threshold, we start draggin
         if ((abs(m_StartDragCoord.x - in_EventData.X()) > GetSystemMetrics(SM_CXDRAG)) || ((abs(m_StartDragCoord.y - in_EventData.Y()) > GetSystemMetrics(SM_CYDRAG))))
         {   
            if(m_SelectedUnit.size()>0 && in_pCaller && in_pCaller->Type() == "g_def_lstbx" )
               m_bMouseIsDragging = true;   
         }
      }
   }
   else
   {
      m_bMouseIsDragging = false;
   }

   // If we are dragging validate drop point position and set proper cursor
   if(m_bMouseIsDragging)
   {

      if(CanDropUnitsAtPosition(in_EventData.Mouse.Position))
      {
         g_Joshua.GUIManager()->SetMousePointer(m_DropCursorFile);
      }
      else
      {
         g_Joshua.GUIManager()->SetMousePointer(m_InvalidCursorFile);
      }
   }
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitGroundDeploymentWindow::OnMouseUp(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseUp(in_EventData, in_pCaller);  

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      // When draging is over, we try to deploy the units at the mouse position
      if(m_bMouseIsDragging)
      {
         // Reset mouse cursor
         g_ClientDDL.ResetMousePointer();
       
         if(m_SelectedUnit.size()>0 && CanDropUnitsAtPosition(in_EventData.Mouse.Position))
		      DeployUnit(in_EventData.Mouse.Position);
      }

      m_bMouseIsDragging = false;
   }

   return GUI::EEventStatus::Handled;
}


void GUnitGroundDeploymentWindow::OnSubjectChange(DesignPattern::GSubject& in_Subject,const DesignPattern::GSubjectNotification &in_Notification,void* in_pAdditionalMsg )
{

   if( (&in_Subject == &g_Joshua.UnitManager()) && 
       (in_Notification == Combat::c_notificationUnitGroupModified))
   {
      // Update only if we are visible and we are not dragging units.
      if(Visible() == true && m_bMouseIsDragging == false)
      {
         UpdateReadyToDeployGroup();
      }
   }
   else
   {
      // Assuming we only ask this kind of question!!!
      if(in_Notification == c_OnGenericQuestionWindowAnswerOk)
      {
         // Fire Event
         g_ClientDCL.RequestTraining();
      }
   }
}

/*!
* Try to deploy a unit at the given mouse position
* @param in_MousePos : The drop point (in relative mouse coordinate from the window
**/
void GUnitGroundDeploymentWindow::DeployUnit(GVector2D<INT32> in_MousePos)
{
	in_MousePos.x+=7;
	in_MousePos.y+=5;

   // Get longitude and latitude from mouse position
   GVector2D<REAL32> l_vLongLatOfMousePosition;
   g_ClientDDL.BackgroundEarth()->ScreenToLongLat( RelativeToAbsolute(in_MousePos),l_vLongLatOfMousePosition);
   DeployUnitToEarthLocation(l_vLongLatOfMousePosition);
}

void GUnitGroundDeploymentWindow::DeployUnitToEarthLocation(GVector2D<REAL32> in_EarthPosition)
{  
   // Deploy units.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GDeployUnits);
   SP2::Event::GDeployUnits* l_pDeployUnitsEvent = (SP2::Event::GDeployUnits*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pDeployUnitsEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pDeployUnitsEvent->m_iSource = g_SP2Client->Id();
   l_pDeployUnitsEvent->m_DeployementCoordinate = in_EarthPosition;


   set<SP2::GUnit*>          l_UnitsToRemove;
   set<GUI::GListBox::GRow*> l_RowsToRemove;
   // loop into the selected units and deploy only the ones that are not training
   for(set<SP2::GUnit*>::const_iterator l_It = m_SelectedUnit.begin(); 
       l_It != m_SelectedUnit.end(); 
       l_It++)
   {
      REAL32 l_fNotUsed;
      // If this selected unit is not in training
      if(GetUnitTrainingProgress((*l_It)->Id(),l_fNotUsed) == false)
      {
         // Add it to the deploy list
         l_pDeployUnitsEvent->m_UnitListToDeploy.push_back((*l_It)->Id());
         // add as a unit to remove
         l_UnitsToRemove.insert((*l_It));
      }
   }	

   // will we remove something ?
   if(l_UnitsToRemove.size())
   {
      // Fire event
      g_Joshua.RaiseEvent(l_pEvent);

      // Remove every selected row to reflect deployed units 
      for(INT32 i = (INT32)m_pSelectedList->Get_Nb_Rows() - 1; i >= 0; i--)     
      {
         if(m_pSelectedList->IsRowSelected(i))
         {
            // a ptr to our selected row
            GUI::GListBox::GRow* l_pRow = m_pSelectedList->Get_Any_Row_At(i);
            // retrieve the unit associated with the row
            hash_map<GListBox::GRow*,GUnit*>::const_iterator l_ItUnitByRow = m_UnitByRow.find(l_pRow);
            // we found it ?
            if(l_ItUnitByRow != m_UnitByRow.end())
            {
               // unit will be removed ?
               set<SP2::GUnit*>::const_iterator l_It = l_UnitsToRemove.find(l_ItUnitByRow->second);
               if(l_It != l_UnitsToRemove.end())
               {
                  // row will need to be removed for our set
                  l_RowsToRemove.insert(l_pRow);
                  // erase our link
                  m_UnitByRow.erase(l_pRow);
               }
            }
         }
      }

      // remove the rows wich contains deployed units
      for(set<GUI::GListBox::GRow*>::const_iterator l_It = l_RowsToRemove.begin(); 
         l_It != l_RowsToRemove.end(); 
         l_It++)
      {
         m_pSelectedList->RemoveRow(*l_It);
      }

      l_UnitsToRemove.clear();
      l_RowsToRemove.clear();
   }
   m_SelectedUnit.clear();
   m_SelectedRow.clear();
   // Update Deployement window
   m_pSelectedList->Update();
}


bool  GUnitGroundDeploymentWindow::CanDropUnitsAtPosition(GVector2D<INT32> in_MousePos)
{
	// Big hack de fin de projet
   in_MousePos.x+=7;
   in_MousePos.y+=5;
   GVector2D<INT32> l_realMousePosition = RelativeToAbsolute(in_MousePos);
   // First make sure the mouse is not over the windows
   if( l_realMousePosition.y > Top() && l_realMousePosition.x > Left() && l_realMousePosition.y < (Top()+Height()) && l_realMousePosition.x < (Left() +Width()))
      return false;
      
   // Can we deploy the units at the given mouse position.
   // Get longitude and latitude from mouse position
   GVector2D<REAL32> l_vLongLatOfMousePosition;
   if(!g_ClientDDL.BackgroundEarth()->ScreenToLongLat( l_realMousePosition,l_vLongLatOfMousePosition))
      return false;


   bool l_bIsNaval = (*m_SelectedUnit.begin())->IsNaval();
   return g_ClientDCL.IsDeployGroupAtPositionPossible(g_ClientDAL.ControlledCountryID(),l_vLongLatOfMousePosition,l_bIsNaval);
}

bool GUnitGroundDeploymentWindow::GetUnitTrainingProgress(UINT32 l_inUnitId,REAL32& out_fProgress)
{
   out_fProgress = 0;
   hash_map<UINT32,GUnitInTraining>::iterator l_UnitFound = g_ClientDAL.m_UnitInTraining.find(l_inUnitId);
   
   if(l_UnitFound != g_ClientDAL.m_UnitInTraining.end())
   {
      GUnitInTraining l_UnitInTraining = l_UnitFound->second;
      out_fProgress =(REAL32)( (g_Joshua.GameTime() - l_UnitInTraining.m_fTrainingStartTime) / (l_UnitInTraining.m_fTrainingCompletionTime - l_UnitInTraining.m_fTrainingStartTime));
      return true;
   }

   return false;
}


void GUnitGroundDeploymentWindow::IterateUnitsInTraining(void)
{
   list<UINT32> l_ListOfUnitToRemove;

   hash_map<UINT32,GUnitInTraining>::iterator l_UnitIterator = g_ClientDAL.m_UnitInTraining.begin();
   while(l_UnitIterator != g_ClientDAL.m_UnitInTraining.end())
   {
      GUnit* l_pUnit = (GUnit*)g_Joshua.UnitManager().Unit(l_UnitIterator->first);
      if(l_pUnit == NULL || l_pUnit->Training() != l_UnitIterator->second.m_ePreviousTrainingLevel)
      {
         // remove this one.
         l_ListOfUnitToRemove.push_back(l_UnitIterator->first);
      }

      l_UnitIterator++;
   }

   list<UINT32>::iterator l_UnitIDIt = l_ListOfUnitToRemove.begin();
   while(l_UnitIDIt != l_ListOfUnitToRemove.end())
   {
      g_ClientDAL.m_UnitInTraining.erase(*l_UnitIDIt);
      m_HasBeenTrained.insert(*l_UnitIDIt);
      l_UnitIDIt++;
   }
}

void GUnitGroundDeploymentWindow::IterateTrainingDisplay()
{
   if(Visible() )
   {
      for(vector<pair<UINT32, GProgressMeterYellowValue*> >::const_iterator it = m_vTrainingUnitProgress.begin();
         it != m_vTrainingUnitProgress.end();++ it)
      {
         SP2::GUnit* l_pUnit = (SP2::GUnit*) g_Joshua.UnitManager().Unit(it->first);

         if(l_pUnit)
         {
            REAL32 l_fTrainingProgress;
            if(GetUnitTrainingProgress(l_pUnit->Id(),l_fTrainingProgress))
            {
               if(l_fTrainingProgress > 1)
               {
                  it->second->TextValueObj()->Text(g_ClientDAL.GetString(102137));
                  it->second->Value(1,0,false,false);
               }
               else
               {
                  it->second->TextValueObj()->Text(g_ClientDAL.GetString(102138));
                  it->second->Value(l_fTrainingProgress,0,false,false);
               }
            }
            else
            {
//               gassert(0, "Trying to update a unit that should no longuer be in training");
            }
         }
      }
   }
}
