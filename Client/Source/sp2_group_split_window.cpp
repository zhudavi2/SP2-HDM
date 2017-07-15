/**************************************************************
*
* sp2_group_split_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_group_split_window.h"
#include "../include/sp2_unit_split_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GGroupSplitWindow::TypeName(L"Split_Group_Window");

GUI::GBaseObject* GGroupSplitWindow::New()
{
	return (GUI::GBaseObject*) new GGroupSplitWindow();
}


GGroupSplitWindow::GGroupSplitWindow() : GBaseWindow()
{
   m_iOriginalLastSelectedRowID = -1;
   m_iNewLastSelectedRowID      = -1;
   m_pObjSplitEquallyBtn        = NULL;
   m_pObjToRightBtn             = NULL;
   m_pObjToLeftBtn              = NULL;
   m_pObjNewLst                 = NULL;
   m_pObjOriginalLst            = NULL;
   m_pObjInnerFrm               = NULL;
}

GGroupSplitWindow::~GGroupSplitWindow()
{
}

/*!
 * Called when the object iscreated
 **/
bool GGroupSplitWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   m_pObjOriginalLst     = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmOriginal")->Child(L"lstGroup");
   m_pObjNewLst          = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmNew")->Child(L"lstGroup");
   m_pObjToLeftBtn       = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnLeft");
   m_pObjToRightBtn      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnRight");
   m_pObjSplitEquallyBtn = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnSplitEqually");

	return true;
}

/*!
 * Called when the object is displayed
 **/
void GGroupSplitWindow::OnShow()
{
   __super::OnShow();

   // disable buttons until the user selectes something
   m_pObjToLeftBtn->Enabled(false);
   m_pObjToRightBtn->Enabled(false);
   // set up our apply button
   m_pObjApplyBtn->Enabled(false);

   if(!g_ClientDDL.UnitSplitWindow())
      g_ClientDDL.UnitSplitWindowSpawn();

   RegisteredModalWindow.push_back( g_ClientDDL.UnitSplitWindow() );

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

/*!
 * Called when the object is hidden
 **/
void GGroupSplitWindow::OnHide()
{
   RegisteredModalWindow.clear();

   __super::OnHide();
}

GUI::EEventStatus::Enum GGroupSplitWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
            return GUI::EEventStatus::Handled;
         }
         else if(in_pCaller == m_pObjToLeftBtn)
         {           
            const vector<INT32> & l_Selection = m_pObjNewLst->Get_Selected_Item();
            if(l_Selection.size() && l_Selection[0] >= 0)
            {
               hash_map<INT32, pair<INT32, INT32> >::iterator l_It = m_New.find( l_Selection[0] );
               if(l_It != m_New.end())
               {
                  m_bFromLeft = false;

                  if(!g_ClientDDL.UnitSplitWindow())
                     g_ClientDDL.UnitSplitWindowSpawn();

                  g_ClientDDL.UnitSplitWindow()->Initialize( this, l_It->second.second );
                  g_ClientDDL.UnitSplitWindow()->Show();                  
               }
               else
               {
                  m_pObjToLeftBtn->Enabled(false);
                  m_pObjToRightBtn->Enabled(false);
               }
            }
         }
         else if(in_pCaller == m_pObjToRightBtn)
         {
            const vector<INT32> & l_Selection = m_pObjOriginalLst->Get_Selected_Item();
            if(l_Selection.size() && l_Selection[0] >= 0)
            {
               hash_map<INT32, pair<INT32, INT32> >::iterator l_It = m_Original.find( l_Selection[0] );
               if(l_It != m_Original.end())
               {
                  m_bFromLeft = true;

                  if(!g_ClientDDL.UnitSplitWindow())
                     g_ClientDDL.UnitSplitWindowSpawn();

                  g_ClientDDL.UnitSplitWindow()->Initialize( this, l_It->second.second );
                  g_ClientDDL.UnitSplitWindow()->Show();                  
               }
               else
               {
                  m_pObjToLeftBtn->Enabled(false);
                  m_pObjToRightBtn->Enabled(false);
               }
            }
         }
         // Split the group equally in 2
         else if(in_pCaller == m_pObjSplitEquallyBtn)
         {
            // reupdate, but now split in 2
            Update(m_iGroupID, true);
         }
         else if(in_pCaller == m_pObjApplyBtn)
         {
            // only apply the split if we have something
            if(CanSplit())
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventGroupSplit);
               SP2::Event::GEventGroupSplit* l_pUpdate = (SP2::Event::GEventGroupSplit*)l_Event.get();
               
               l_pUpdate->m_iGroupToSplit = m_iGroupID;
               
               hash_map<INT32, INT32> l_Units;
               hash_map<INT32, pair<INT32, INT32> >::iterator l_It = m_New.begin();
               while(l_It != m_New.end())
               {
                  l_Units.insert( l_It->second );
                  l_It++;
               }
               
               l_pUpdate->m_UnitsToSplit = l_Units;
               
               l_pUpdate->m_iSource = g_SP2Client->Id();
               l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

               g_Joshua.RaiseEvent( l_Event );
               
               Hide();
            }
         }
      }
   }
   // set up our apply button
   m_pObjApplyBtn->Enabled(CanSplit());
   return GUI::EEventStatus::Handled;
}

void GGroupSplitWindow::SwapRow( GUI::GListBox* in_pFrom, GUI::GListBox* in_pTo, INT32 in_iQtyToSwap )
{
   if(!in_pFrom || !in_pTo || !in_iQtyToSwap)
      return;

   if( m_iOriginalLastSelectedRowID >= 0 )
   {
      GUI::GListBox::GRow* l_pRow = m_pObjOriginalLst->Get_Row_At( m_iOriginalLastSelectedRowID );
      ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
      ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000587");
   }

   if( m_iNewLastSelectedRowID >= 0 )
   {
      GUI::GListBox::GRow* l_pRow = m_pObjNewLst->Get_Row_At( m_iNewLastSelectedRowID );
      ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
      ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000587");
   }   

   //If there is something selected in the listbox and is valid
   const vector<INT32> & l_From = in_pFrom->Get_Selected_Item();
   if(l_From.size() && l_From[0] >= 0)
   {
      hash_map<INT32, pair<INT32, INT32> >* l_pFrom;
      hash_map<INT32, pair<INT32, INT32> >* l_pTo;
      
      hash_map<INT32, pair<INT32, INT32> >::iterator l_FromIt;
      hash_map<INT32, pair<INT32, INT32> >::iterator l_ToIt;
      bool l_bUnitExistInDestination = false;

      //Get the selected unit according to which list it came
      if(in_pFrom == m_pObjNewLst)
      {
         l_pFrom = &m_New;
         l_pTo   = &m_Original;
      }
      else
      {
         l_pFrom = &m_Original;
         l_pTo   = &m_New;
      }

      l_FromIt = l_pFrom->find( l_From[0] );
      if(l_FromIt == l_pFrom->end())
         return;

      //Check if the unit exist at the destination so we do not add an unused row
      l_ToIt = l_pTo->begin(); 
      while(l_ToIt != l_pTo->end())
      {
         if( l_ToIt->second.first == l_FromIt->second.first )
         {
            l_bUnitExistInDestination = true;
            break;
         }

         l_ToIt++;
      }

      GUI::GListBox::GRow* l_pDestRow = NULL;
      GUI::GListBox::GRow* l_pFromRow = in_pFrom->Get_Row_At( l_FromIt->first );
      
      l_FromIt->second.second -= in_iQtyToSwap;

      if(l_ToIt->second.second)
      {
         if(l_bUnitExistInDestination)
         {
            l_pDestRow = in_pTo->Get_Row_At( l_ToIt->first );
            l_ToIt->second.second += in_iQtyToSwap;
         }
         else
         {
            l_pDestRow = in_pTo->Insert_New_Row();
            l_pDestRow->Object()->Child(L"picTraining")->BringToFront();

            l_pTo->insert( make_pair( l_pDestRow->m_iId, make_pair(l_FromIt->second.first, in_iQtyToSwap) ) );
            l_ToIt = l_pTo->find( l_pDestRow->m_iId );
         }

         ((GUI::GLabel*)l_pDestRow->Object()->Child(L"txtName"))->Text( ((GUI::GLabel*)l_pFromRow->Object()->Child(L"txtName"))->Text() );
         ((GUI::GLabel*)l_pDestRow->Object()->Child(L"txtQty"))->Text( GString::FormatNumber((REAL64)l_ToIt->second.second,   L" ", L"", L"", L"", 3, 0, false) );

         SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit( l_ToIt->second.first );

         switch( l_pUnit->Training() )
         {
         case ETrainingLevel::Recruit:
            ((GUI::GPictureBox*)l_pDestRow->Object()->Child(L"picTraining"))->PictureNormal( L"000159" );
            break;
         case ETrainingLevel::Regular:
            ((GUI::GPictureBox*)l_pDestRow->Object()->Child(L"picTraining"))->PictureNormal( L"000825" );
            break;
         case ETrainingLevel::Veteran:
            ((GUI::GPictureBox*)l_pDestRow->Object()->Child(L"picTraining"))->PictureNormal( L"000826" );
            break;
         case ETrainingLevel::Elite:
            ((GUI::GPictureBox*)l_pDestRow->Object()->Child(L"picTraining"))->PictureNormal( L"000827" );
            break;
         default:
            gassert(0,"Unhandled case in unit training");
            break;
         }
      }

      //If we need to set a quantity to the giver
      if(l_FromIt->second.second)
      {
         ((GUI::GLabel*)l_pFromRow->Object()->Child(L"txtQty"))->Text( GString::FormatNumber((REAL64)l_FromIt->second.second, L" ", L"", L"", L"", 3, 0, false) );
      }
      //else we must remove the row since it's not used anymore
      else
      {        
         hash_map< INT32, pair< INT32, INT32 > > l_NewMap;
         INT32 l_iRefID = l_FromIt->first;

         l_pFrom->erase( l_FromIt );
         in_pFrom->RemoveRow( l_pFromRow );

         //We must reajust every row after the deleted one in the map
         hash_map< INT32, pair< INT32, INT32 > >::iterator l_It = l_pFrom->begin();
         while( l_It != l_pFrom->end() )
         {
            if( l_It->first >= l_iRefID )
            {
               l_NewMap.insert( make_pair( l_It->first - 1, make_pair( l_It->second.first, l_It->second.second ) ) );
            }
            else
            {
               l_NewMap.insert( make_pair( l_It->first, make_pair( l_It->second.first, l_It->second.second ) ) );
            }
            
            l_It++;
         }

         *l_pFrom = l_NewMap;
      }
   }

   in_pFrom->Update();
   in_pTo->Update();

   m_iOriginalLastSelectedRowID = -1;
   m_iNewLastSelectedRowID = -1;

   m_pObjToRightBtn->Enabled(false);
   m_pObjToLeftBtn->Enabled(false);
}

EEventStatus::Enum GGroupSplitWindow::OnCustomEvent(UINT32 in_iEventID, const GEventData & in_Data, GBaseObject* in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnListItemSelected)
   {
      if(in_pCaller == m_pObjOriginalLst)
      {
         m_pObjToRightBtn->Enabled(true);

         if( m_iOriginalLastSelectedRowID >= 0 )
         {
            GUI::GListBox::GRow* l_pRow = m_pObjOriginalLst->Get_Row_At( m_iOriginalLastSelectedRowID );
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000587");
         }

         if(m_pObjOriginalLst->Get_Selected_Item().size() && m_pObjOriginalLst->Get_Selected_Item()[0] >= 0)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjOriginalLst->Get_Row_At( m_pObjOriginalLst->Get_Selected_Item()[0] );
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000647");
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000647");
         
            m_iOriginalLastSelectedRowID = m_pObjOriginalLst->Get_Selected_Item()[0];
         }
      }
      else if(in_pCaller == m_pObjNewLst)
      {
         m_pObjToLeftBtn->Enabled(true);
         if( m_iNewLastSelectedRowID >= 0 )
         {
            GUI::GListBox::GRow* l_pRow = m_pObjNewLst->Get_Row_At( m_iNewLastSelectedRowID );
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000587");
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000587");
         }

         if(m_pObjNewLst->Get_Selected_Item().size() && m_pObjNewLst->Get_Selected_Item()[0] >= 0)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjNewLst->Get_Row_At( m_pObjNewLst->Get_Selected_Item()[0] );
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(L"000647");
            ((GUI::GFrame*)l_pRow->Object()->Child(L"txtQty"))->PictureNormal(L"000647");

            m_iNewLastSelectedRowID = m_pObjNewLst->Get_Selected_Item()[0];
         }
      }
   }
   else if(in_iEventID == c_iOnSplitWindowClose)
   {
      if(m_bFromLeft)
      {
         SwapRow(m_pObjOriginalLst, m_pObjNewLst, g_ClientDDL.UnitSplitWindow()->QuantityInGroupB());
      }
      else
      {
         SwapRow(m_pObjNewLst, m_pObjOriginalLst, g_ClientDDL.UnitSplitWindow()->QuantityInGroupA());
      }
   }
   // set up our apply button
   m_pObjApplyBtn->Enabled(CanSplit());
   // handled
   return GUI::EEventStatus::Handled;
}

/*!
 * Update the splits window from what we have in the unit manager
 **/
void GGroupSplitWindow::Update(INT32 in_iGroupID, bool in_bSplitEqually)
{
   // retrieve the group from the unit manager through our member id
   SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(in_iGroupID);
   // validate the returned group
   if(!l_pGroup)
   {
      m_iGroupID = -1;
      if(Visible())
         Hide();
      return;
   }
   else
   {
      m_iGroupID = in_iGroupID;
   }
   // Disable gui stuff
   m_iOriginalLastSelectedRowID = -1;
   m_iNewLastSelectedRowID      = -1;
   m_pObjToRightBtn->Enabled(false);
   m_pObjToLeftBtn ->Enabled(false);
   m_pObjApplyBtn  ->Enabled(false); 
   // empty the listboxes
   m_pObjOriginalLst->Clear();
   m_pObjNewLst->Clear();
   // empty the maps between rows and units
   m_Original.clear();
   m_New.clear();
   // loop into every units and add them into our list box and map
   const vector<SDK::Combat::GUnit*> l_pUnits = l_pGroup->Units();
   for(UINT32 i=0; i<l_pUnits.size(); i++)
   {
      // retrieve total qty
      UINT32  l_iQtyTotal     = ((SP2::GUnit*)l_pUnits[i])->Qty();
      // retrieve qty to add in original 
      UINT32 l_iQtyInOriginal = in_bSplitEqually ? l_iQtyTotal / 2 : l_iQtyTotal;
      // retrieve qty to add in new 
      UINT32 l_iQtyInNew      = in_bSplitEqually ? l_iQtyTotal - l_iQtyInOriginal : 0;
      // add original row if we have a quantity
      if(l_iQtyInOriginal > 0)
      {
         // insert original row
         GUI::GListBox::GRow* l_pRowInOriginal = m_pObjOriginalLst->Insert_New_Row();
         // set up our unit row information
         SetUpRowWithUnit(l_pRowInOriginal, l_pUnits[i], l_iQtyInOriginal);
         // add it into our map
         m_Original.insert( make_pair(l_pRowInOriginal->m_iId, make_pair(l_pUnits[i]->Id(), l_iQtyInOriginal) ) );
      }
      // add new group row if we have a quantity
      if(l_iQtyInNew > 0)
      {
         // insert row in new
         GUI::GListBox::GRow* l_pRowInNew = m_pObjNewLst->Insert_New_Row();
         // set up its info
         SetUpRowWithUnit(l_pRowInNew, l_pUnits[i], l_iQtyInNew);
         // add it into our map
         m_New.insert( make_pair(l_pRowInNew->m_iId, make_pair(l_pUnits[i]->Id(), l_iQtyInNew) ) );
      }
   }
   // refresh time !
   m_pObjNewLst     ->Update();
   m_pObjOriginalLst->Update();
   // set up our apply button
   m_pObjApplyBtn->Enabled(CanSplit());
}

/*!
 * Set up a split window unit row looks
 **/
void GGroupSplitWindow::SetUpRowWithUnit(GUI::GListBox::GRow* in_pRow, SDK::Combat::GUnit* in_pUnit, 
                                         UINT32 in_QtyToDisplay)
{
   // show training pic
   in_pRow->Object()->Child(L"picTraining")->BringToFront();
   // set up nam and qty to display
   GString l_sName = ((SP2::GUnitDesign*)in_pUnit->Design())->Name();
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtName"))->Text( l_sName );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtQty"))->Text( GString::FormatNumber((REAL64)in_QtyToDisplay, L" ", L"", L"", L"", 3, 0, false) );
   // display the trainig pictures
   switch( ((SP2::GUnit*)in_pUnit)->Training() )
   {
   case ETrainingLevel::Recruit:
      ((GUI::GPictureBox*)in_pRow->Object()->Child(L"picTraining"))->PictureNormal( L"000159" );
      break;
   case ETrainingLevel::Regular:
      ((GUI::GPictureBox*)in_pRow->Object()->Child(L"picTraining"))->PictureNormal( L"000825" );
      break;
   case ETrainingLevel::Veteran:
      ((GUI::GPictureBox*)in_pRow->Object()->Child(L"picTraining"))->PictureNormal( L"000826" );
      break;
   case ETrainingLevel::Elite:
      ((GUI::GPictureBox*)in_pRow->Object()->Child(L"picTraining"))->PictureNormal( L"000827" );
      break;
   default:
      gassert(0,"Unhandled case in unit training");
   }
}

/*!
 * Set up the window if we can aplit
 **/
bool GGroupSplitWindow::CanSplit()
{
   return (m_New.size() > 0 && m_Original.size() > 0);
}