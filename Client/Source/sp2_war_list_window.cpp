/**************************************************************
*
* sp2_war_list_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_war_list_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GWarListWindow::TypeName(L"War_List_Window");

GUI::GBaseObject* GWarListWindow::New()
{
	return (GUI::GBaseObject*) new GWarListWindow();
}


GWarListWindow::GWarListWindow() : GBaseWindow()
{
	m_iCurrentWarID = -1;
   m_iSelectedRowID = -1;
}

GWarListWindow::~GWarListWindow()
{
}

bool GWarListWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   m_pObjWarLst      = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmWarList")->Child(L"lstWar");
   
   GUI::GFrame* l_pFrame = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmWarDetails");
   
   m_pObjSideALst    = (GUI::GListBox*)l_pFrame->Child(L"lstWar");
   m_pObjSideBLst    = (GUI::GListBox*)l_pFrame->Child(L"lstWar1");

   m_pObjJoinABtn    = (GUI::GButton*)l_pFrame->Child(L"btnJoin");
   m_pObjJoinBBtn    = (GUI::GButton*)l_pFrame->Child(L"btnJoin1");

   m_pObjPeaceATog   = (GUI::GToggle*)l_pFrame->Child(L"togPeace");
   m_pObjPeaceBTog   = (GUI::GToggle*)l_pFrame->Child(L"togPeace1");
   m_pObjWarATog     = (GUI::GToggle*)l_pFrame->Child(L"togWar");
   m_pObjWarBTog     = (GUI::GToggle*)l_pFrame->Child(L"togWar1");

   m_pObjMasterALbl  = (GUI::GLabel*)l_pFrame->Child(L"frmMasterAttacker")->Child(L"txtName");
   m_pObjMasterBLbl  = (GUI::GLabel*)l_pFrame->Child(L"frmMasterDefender")->Child(L"txtName1");

   m_pObjMasterAPic  = (GUI::GPictureBox*)l_pFrame->Child(L"frmMasterAttacker")->Child(L"picFlag");
   m_pObjMasterBPic  = (GUI::GPictureBox*)l_pFrame->Child(L"frmMasterDefender")->Child(L"picFlag1");

   m_pObjStatusAPic  = (GUI::GPictureBox*)l_pFrame->Child(L"frmMasterAttacker")->Child(L"picState");
   m_pObjStatusBPic  = (GUI::GPictureBox*)l_pFrame->Child(L"frmMasterDefender")->Child(L"picState1");

	m_pObjStatusAPic->PictureNormal(L"");
	m_pObjStatusBPic->PictureNormal(L"");

	//Hide all buttons
	m_pObjJoinABtn->Hide();
	m_pObjJoinBBtn->Hide();
	m_pObjPeaceATog->Hide();
	m_pObjPeaceBTog->Hide();
	m_pObjWarATog->Hide();
	m_pObjWarBTog->Hide();

   if(m_pObjApplyBtn)
      m_pObjApplyBtn->Hide();

	return true;
}

void GWarListWindow::OnShow()
{
   __super::OnShow();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::War);

   m_pObjSideALst->Clear();
   m_pObjSideBLst->Clear();
   m_pObjSideALst->Update();
   m_pObjSideBLst->Update();
   
	UpdateWarList();
	if( g_ClientDAL.CurrentWars().size() )
	{
		vector<INT32> l_viWar;
		l_viWar.push_back(0);
		m_pObjWarLst->Select_Items( l_viWar, true );
	}

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

void GWarListWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::War);

   __super::OnHide();
}

GUI::EEventStatus::Enum GWarListWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.WarListWindow()->Hide();
            return GUI::EEventStatus::Handled;
         }
			else if(in_pCaller == m_pObjJoinABtn)
			{
				SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			   SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				l_pUpdate->m_iSource = g_SP2Client->Id();
				l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
				
				l_pUpdate->m_iWarID = m_iCurrentWarID;
				l_pUpdate->m_bJoinWar = true;
				l_pUpdate->m_bAttacker = true;
				l_pUpdate->m_bWantPeace = false;
	
				g_Joshua.RaiseEvent(l_pEvent);
			}
			else if(in_pCaller == m_pObjJoinBBtn)
			{           
            SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			   SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				l_pUpdate->m_iSource = g_SP2Client->Id();
				l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

				l_pUpdate->m_iWarID = m_iCurrentWarID;
				l_pUpdate->m_bJoinWar = true;
				l_pUpdate->m_bAttacker = false;
				l_pUpdate->m_bWantPeace = false;

				g_Joshua.RaiseEvent(l_pEvent);
			}
			else if(in_pCaller == m_pObjPeaceATog)
			{
				bool l_bMustSendEvent = false;
            bool l_bWantsPeace;
            //Check if the current state of the war has changed
            hash_map< UINT32, bool >::iterator l_It = m_ChangedWars.find( m_iCurrentWarID );
            
            //If it has changed
            if( l_It != m_ChangedWars.end() )
            {
               //If the user idea changed (wanted peace and now war) rechange the state again
               if( !l_It->second )
               {
                  l_bMustSendEvent = true;
                  l_It->second     = true;
               }
            }
            else
            {
               //If we couldn't find that the war state has changed check the actual status
               if( ActualWarStatus(true, l_bWantsPeace) )
               {
                  if( !l_bWantsPeace )
                  {
                     l_bMustSendEvent = true;
                     m_ChangedWars[m_iCurrentWarID] = true;
                  }
               }
            }

            m_pObjStatusAPic->PictureNormal( GString(L"000862") );

            if(l_bMustSendEvent)
            {
               SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			      SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				   l_pUpdate->m_iSource = g_SP2Client->Id();
				   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

				   l_pUpdate->m_iWarID = m_iCurrentWarID;
				   l_pUpdate->m_bJoinWar = false;
				   l_pUpdate->m_bAttacker = true;
				   l_pUpdate->m_bWantPeace = true;

				   g_Joshua.RaiseEvent(l_pEvent);
            }
			}
			else if(in_pCaller == m_pObjPeaceBTog)
			{
				bool l_bMustSendEvent = false;
            bool l_bWantsPeace;
            //Check if the current state of the war has changed
            hash_map< UINT32, bool >::iterator l_It = m_ChangedWars.find( m_iCurrentWarID );
            
            //If it has changed
            if( l_It != m_ChangedWars.end() )
            {
               //If the user idea changed (wanted peace and now war) rechange the state again
               if( !l_It->second )
               {
                  l_bMustSendEvent = true;
                  l_It->second     = true;
               }
            }
            else
            {
               //If we couldn't find that the war state has changed check the actual status
               if( ActualWarStatus(false, l_bWantsPeace) )
               {
                  if( !l_bWantsPeace )
                  {
                     l_bMustSendEvent = true;
                     m_ChangedWars[m_iCurrentWarID] = true;
                  }
               }
            }
            
   		   m_pObjStatusBPic->PictureNormal( GString(L"000862") );

            if(l_bMustSendEvent)
            {
               SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			      SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				   l_pUpdate->m_iSource = g_SP2Client->Id();
				   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

				   l_pUpdate->m_iWarID = m_iCurrentWarID;
				   l_pUpdate->m_bJoinWar = false;
				   l_pUpdate->m_bAttacker = false;
				   l_pUpdate->m_bWantPeace = true;
   	
				   g_Joshua.RaiseEvent(l_pEvent);
            }
			}
			else if(in_pCaller == m_pObjWarATog)
			{
				bool l_bMustSendEvent = false;
            bool l_bWantsPeace;
            //Check if the current state of the war has changed
            hash_map< UINT32, bool >::iterator l_It = m_ChangedWars.find( m_iCurrentWarID );
            
            //If it has changed
            if( l_It != m_ChangedWars.end() )
            {
               //If the user idea changed (wanted peace and now war) rechange the state again
               if( l_It->second )
               {
                  l_bMustSendEvent = true;
                  l_It->second     = false;
               }
            }
            else
            {
               //If we couldn't find that the war state has changed check the actual status
               if( ActualWarStatus(true, l_bWantsPeace) )
               {
                  if( l_bWantsPeace )
                  {
                     l_bMustSendEvent = true;
                     m_ChangedWars[m_iCurrentWarID] = false;
                  }
               }
            }
            
     		   m_pObjStatusAPic->PictureNormal( GString(L"000863") );

            if(l_bMustSendEvent)
            {
				   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			      SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				   l_pUpdate->m_iSource = g_SP2Client->Id();
				   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

				   l_pUpdate->m_iWarID = m_iCurrentWarID;
				   l_pUpdate->m_bJoinWar = false;
				   l_pUpdate->m_bAttacker = true;
				   l_pUpdate->m_bWantPeace = false;
   	
				   g_Joshua.RaiseEvent(l_pEvent);
            }
			}
			else if(in_pCaller == m_pObjWarBTog)
			{
				bool l_bMustSendEvent = false;
            bool l_bWantsPeace;
            //Check if the current state of the war has changed
            hash_map< UINT32, bool >::iterator l_It = m_ChangedWars.find( m_iCurrentWarID );
            
            //If it has changed
            if( l_It != m_ChangedWars.end() )
            {
               //If the user idea changed (wanted peace and now war) rechange the state again
               if( l_It->second )
               {
                  l_bMustSendEvent = true;
                  l_It->second     = false;
               }
            }
            else
            {
               //If we couldn't find that the war state has changed check the actual status
               if( ActualWarStatus(false, l_bWantsPeace) )
               {
                  if( l_bWantsPeace )
                  {
                     l_bMustSendEvent = true;
                     m_ChangedWars[m_iCurrentWarID] = false;
                  }
               }
            }

            m_pObjStatusBPic->PictureNormal( GString(L"000863") );

            if(l_bMustSendEvent)
            {
				   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventUpdateWarState);
			      SP2::Event::GEventUpdateWarState* l_pUpdate = (SP2::Event::GEventUpdateWarState*)l_pEvent.get();

				   l_pUpdate->m_iSource = g_SP2Client->Id();
				   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

				   l_pUpdate->m_iWarID = m_iCurrentWarID;
				   l_pUpdate->m_bJoinWar = false;
				   l_pUpdate->m_bAttacker = false;
				   l_pUpdate->m_bWantPeace = false;
   	
				   g_Joshua.RaiseEvent(l_pEvent);
            }
			}
      }
   }

   return GUI::EEventStatus::Handled;
}

bool GWarListWindow::ActualWarStatus(bool in_bAttacker, bool & out_bWantsPeace)
{
   hash_map<UINT32, GWar>::const_iterator l_It = g_ClientDAL.CurrentWars().find( m_iCurrentWarID );
   if( l_It != g_ClientDAL.CurrentWars().end() )
   {
      if(in_bAttacker)
      {
         out_bWantsPeace = l_It->second.MasterAttackingWantsPeace();
      }
      else
      {
         out_bWantsPeace = l_It->second.MasterDefendingWantsPeace();
      }

      return true;
   }
   else
   {
      return false;
   }
}

void GWarListWindow::AddDirtyWar(UINT32 in_iWarID, bool in_bAtWar)
{
   m_ChangedWars[ in_iWarID ] = in_bAtWar;
}

void GWarListWindow::RemoveDirtyWar(UINT32 in_iWarID)
{
   hash_map< UINT32, bool >::iterator l_WarIt = m_ChangedWars.find(in_iWarID);
   if(l_WarIt != m_ChangedWars.end())
   {
      m_ChangedWars.erase(l_WarIt);
   }
}

bool GWarListWindow::IsWarDirty(UINT32 in_iWarID, bool out_bAtWar)
{
   hash_map< UINT32, bool >::iterator l_WarIt = m_ChangedWars.find(in_iWarID);
   if(l_WarIt != m_ChangedWars.end())
   {
      out_bAtWar = l_WarIt->second;
      return true;
   }
   else
   {
      return false;
   }
}

EEventStatus::Enum GWarListWindow::OnCustomEvent(UINT32 in_EventID, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
	if(in_EventID == ECustomEvents::OnListItemSelected)
	{
		if(in_pCaller == m_pObjWarLst)
		{
			vector<INT32> l_viSel = m_pObjWarLst->Get_Selected_Item();
			if( l_viSel.size() && ( l_viSel[0] >= 0 ) )
			{
				m_iCurrentWarID = m_ShownWars[ l_viSel[0] ];
				UpdateSides( m_iCurrentWarID );
			}

         if( m_iSelectedRowID >= 0 )
         {
            GUI::GListBox::GRow* l_pRow = m_pObjWarLst->Get_Row_At(m_iSelectedRowID);
            if(l_pRow)
            {
               ((GUI::GFrame*)l_pRow->Object())->PictureNormal(L"000587");
            }
         }

         if( m_pObjWarLst->Get_Selected_Item().size() )
         {
            GUI::GListBox::GRow* l_pRow = m_pObjWarLst->Get_Row_At( m_pObjWarLst->Get_Selected_Item()[0] );
            if(l_pRow)
            {
               m_iSelectedRowID = m_pObjWarLst->Get_Selected_Item()[0];
               ((GUI::GFrame*)l_pRow->Object())->PictureNormal(L"000647");
            }
         }
		}
	}
	
	return GUI::EEventStatus::Handled;
}

void GWarListWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::War)
   {
      //Get the war that has changed
      UINT32 l_iWarID = (UINT32)in_pAdditionalMsg;

      //Try to find if the war is currently displayed
      hash_map<UINT32, UINT32>::iterator l_It = m_ShownWars.begin();
      while( l_It != m_ShownWars.end() )
      {
         //If we found the war
         if( l_It->second == l_iWarID )
         {
            //Get the war that has changed
            hash_map<UINT32, SP2::GWar>::const_iterator l_ItWar = g_ClientDAL.CurrentWars().find( l_iWarID );
            //If we found it, it means that we want to update their qty of allies
            if( l_ItWar != g_ClientDAL.CurrentWars().end() )
            {
               //Update the number of allies
               GUI::GListBox::GRow* l_pRow = m_pObjWarLst->Get_Row_At( l_It->first );
               GUI::GLabel* l_pNb     = (GUI::GLabel*)l_pRow->Object()->Child(L"txtNbAllies");
		         GUI::GLabel* l_pNb1    = (GUI::GLabel*)l_pRow->Object()->Child(L"txtNbAllies1");

               //Fill in the textboxes
               l_pNb->Text( GString(l_ItWar->second.AttackingSide().size()) );
               l_pNb1->Text( GString(l_ItWar->second.DefendingSide().size()) );

               //If the war currently displayed is the one modified we must update the bottom
               if( m_iCurrentWarID == (INT32)l_iWarID )
               {  
                  UpdateSides( m_iCurrentWarID );
               }
            }
            //If we didn't found it it means it has been erased, so we must do the same
            else
            {
               UINT32 l_iRowID = l_It->first;

               //If the row to be deleted was previously selected, unselect it
               if( m_iSelectedRowID == (INT32)l_It->first )
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjWarLst->Get_Row_At(m_iSelectedRowID);
                  if(l_pRow)
                  {
                     ((GUI::GFrame*)l_pRow->Object())->PictureNormal(L"000587");
                     m_iSelectedRowID = -1;
                  }                  
               }
               
               //Erase the elements
               RemoveDirtyWar( l_It->second );
               m_pObjWarLst->RemoveRow( l_It->first );
               m_ShownWars.erase( l_It );
               m_pObjWarLst->Update();

               //Once it's done we must repopulate the shown wars map with the new id's
               hash_map<UINT32, UINT32> l_CopyShownWars = m_ShownWars;
               m_ShownWars.clear();
               
               hash_map<UINT32, UINT32>::iterator l_CopyIt = l_CopyShownWars.begin();
               while( l_CopyIt != l_CopyShownWars.end() )
               {
                  //If the row id was bigger than the one erased we must put it one upper row
                  if( l_iRowID < l_CopyIt->first )
                     m_ShownWars.insert( make_pair( l_CopyIt->first - 1, l_CopyIt->second ) );
                  else
                     m_ShownWars.insert( make_pair( l_CopyIt->first, l_CopyIt->second ) );

                  l_CopyIt++;
               }

               //If the war was already selected clear it
               if( m_iCurrentWarID == (INT32)l_iWarID )
               {
                  m_iCurrentWarID = -1;
                  UpdateSides( m_iCurrentWarID );
               }
            }

            //If we found the war we can break the loop
            break;
         }
         
         l_It++;
      }

      //If the war was not shown, it means it's a new war we must add it to the list
      if( l_It == m_ShownWars.end() )
      {
         hash_map<UINT32, SP2::GWar>::const_iterator l_ItWar = g_ClientDAL.CurrentWars().find( l_iWarID );

         AddWar( l_ItWar->second );
      }
   }
}

void GWarListWindow::PlayerState(const INT32 in_iWarID, bool & io_bPlayerIsIn, bool & io_bPlayerIsMaster, bool & io_bPlayerAttacker)
{
	//Get the war
	hash_map<UINT32, GWar>::const_iterator l_WarIt = g_ClientDAL.CurrentWars().find(in_iWarID);
	const GWar & l_War = l_WarIt->second;

	//Get My country id
	UINT32 l_iMyCountry = (UINT32)g_ClientDAL.ControlledCountryID();

	//Check if i'm the master attacking
	if(l_iMyCountry == l_War.MasterAttacking())
	{
		io_bPlayerIsIn = true;
		io_bPlayerIsMaster = true;
		io_bPlayerAttacker = true;
		return;
	}
	//Check if i'm the master defending
	else if(l_iMyCountry == l_War.MasterDefending())
	{
		io_bPlayerIsIn = true;
		io_bPlayerIsMaster = true;
		io_bPlayerAttacker = false;
		return;
	}
	//If i'm not the master
	else
	{
		//Check if i'm on side A
		set<UINT32>::const_iterator l_It = l_War.AttackingSide().find(l_iMyCountry);
		if(l_It != l_War.AttackingSide().end())
		{
			io_bPlayerIsIn = true;
			io_bPlayerIsMaster = false;
			io_bPlayerAttacker = true;
			return;
		}

		//If not check if i'm on side B
		l_It = l_War.DefendingSide().find(l_iMyCountry);
		if(l_It != l_War.DefendingSide().end())
		{
			io_bPlayerIsIn = true;
			io_bPlayerIsMaster = false;
			io_bPlayerAttacker = false;
			return;
		}
	}

	//Otherwise i'm not involved in the war
	io_bPlayerIsIn = false;
	io_bPlayerIsMaster = false;
	io_bPlayerAttacker = false;
}

// Update the war listbox
void GWarListWindow::UpdateWarList()
{
	//Clear the shown wars
	m_ShownWars.clear();
	m_pObjWarLst->Clear();
	
	//Get current wars
	const hash_map<UINT32, GWar> & l_Wars = g_ClientDAL.CurrentWars();
	hash_map<UINT32, GWar>::const_iterator l_It = l_Wars.begin();

	//For each wars
	while(l_It != l_Wars.end())
	{
		AddWar( l_It->second );

		l_It++;
	}

	m_pObjWarLst->Update();
}

void GWarListWindow::AddWar(const SP2::GWar & in_War)
{
   //Check if the war does not already exists
   for(hash_map<UINT32,UINT32>::const_iterator l_WarIt = m_ShownWars.begin();
       l_WarIt != m_ShownWars.end();
       l_WarIt++)
   {
      if(l_WarIt->second == in_War.ID())
      {
         gassert(0,"A war should not be added twice");
         return;
      }
   }

   GUI::GListBox::GRow* l_pRow = NULL;

	//Check if we need to insert a new row
   l_pRow = m_pObjWarLst->Insert_New_Row();

	//Get pointer to row objects
	GUI::GPictureBox* l_pFlag  = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag");
	GUI::GPictureBox* l_pFlag1 = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1");
	GUI::GLabel* l_pName   = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
	GUI::GLabel* l_pName1  = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName1");
	GUI::GLabel* l_pNb     = (GUI::GLabel*)l_pRow->Object()->Child(L"txtNbAllies");
	GUI::GLabel* l_pNb1    = (GUI::GLabel*)l_pRow->Object()->Child(L"txtNbAllies1");

	//Get the Master attacking country
	const GCountry & l_Ctry = g_ClientDAL.Country((INT32)in_War.MasterAttacking());
   
	//Set his information
	l_pFlag->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
	l_pName->Text( l_Ctry.Name() );
	l_pNb->Text( GString(in_War.AttackingSide().size()) );

	//Get the Master defending country
	const GCountry & l_Ctry1 = g_ClientDAL.Country((INT32)in_War.MasterDefending());
   
	//Set his information
	l_pFlag1->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry1.Id()]);
	l_pName1->Text( l_Ctry1.Name() );
	l_pNb1->Text( GString(in_War.DefendingSide().size()) );

	//Insert the war into Shown Wars (ROWID, WARID)
   m_ShownWars.insert( make_pair(l_pRow->m_iId, in_War.ID()) );
}

//Fill the war info section
void GWarListWindow::UpdateSides(const INT32 in_iWarID)
{
   if( in_iWarID < 0 )
   {
      m_pObjSideALst->Clear();
      m_pObjSideBLst->Clear();
      m_pObjMasterALbl->Text( L"" );
      m_pObjMasterBLbl->Text( L"" );
      m_pObjMasterAPic->PictureNormal( GString(L"000159") );
      m_pObjMasterBPic->PictureNormal( GString(L"000159") );
      m_pObjMasterAPic->PictureNormal( GString(L"000159") );
      m_pObjMasterBPic->PictureNormal( GString(L"000159") );
      m_pObjStatusAPic->PictureNormal( GString(L"000159") );
      m_pObjStatusBPic->PictureNormal( GString(L"000159") );

      m_pObjJoinABtn->Hide();
      m_pObjJoinBBtn->Hide();
      m_pObjPeaceATog->Hide();
      m_pObjPeaceBTog->Hide();
      m_pObjWarATog->Hide();
      m_pObjWarBTog->Hide();

      return;
   }

	//Get the current war
	hash_map<UINT32, GWar>::const_iterator l_WarIt = g_ClientDAL.CurrentWars().find(in_iWarID);

   if( l_WarIt == g_ClientDAL.CurrentWars().end() )
   {
      gassert( 0,"WTF !" );
      return;
   }

	const GWar & l_War = l_WarIt->second;

	//bool to see the last one when a country is added on a side
   bool l_bShowLast = false;
   
   //Get the masters
	UINT32 l_iMasterAttacker = l_War.MasterAttacking();
	UINT32 l_iMasterDefender = l_War.MasterDefending();

	const GCountry & l_MasterAttacker = g_ClientDAL.Country(l_iMasterAttacker);
	const GCountry & l_MasterDefender = g_ClientDAL.Country(l_iMasterDefender);

	//Set the masters information
	m_pObjMasterALbl->Text( l_MasterAttacker.Name() );
	m_pObjMasterBLbl->Text( l_MasterDefender.Name() );

	m_pObjMasterAPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_MasterAttacker.Id()]);
	m_pObjMasterBPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_MasterDefender.Id()]);

	//Get the two sides
	const set<UINT32> & l_SideA = l_War.AttackingSide();
	const set<UINT32> & l_SideB = l_War.DefendingSide();

	set<UINT32>::const_iterator l_It = l_SideA.begin();

	//For each buddy on side A
	UINT32 i=0;
	while(l_It != l_SideA.end())
	{
      //If it's the master country we must skip it
      if( (*l_It) == l_War.MasterAttacking() )
      {
         l_It++;
         continue;
      }

      GUI::GListBox::GRow* l_pRow = NULL;

      if( (m_pObjSideALst->FirstRowShown() + 1 + m_pObjSideALst->NbVisibleRows()) >= m_pObjSideALst->Get_Nb_Rows() )
      {
         l_bShowLast = true;
      }

		//Check if i need to insert a new row
		if(i >= (UINT32)m_pObjSideALst->Get_Nb_Rows())
		{
			l_pRow = m_pObjSideALst->Insert_New_Row();
		}
		else
		{
			l_pRow = m_pObjSideALst->Get_Row_At(i);
		}

		//Get the country of the one on side A
		const GCountry & l_Ctry = g_ClientDAL.Country( (INT32)(*l_It) );
		
		//Set it's info
		((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
		((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text( l_Ctry.Name() );

		i++;
		l_It++;
	}

   //We must erase the rows that has not be used ( - 1 stands for the master country )
   for(INT32 j=m_pObjSideALst->Get_Nb_Rows(); j>((INT32)l_SideA.size() - 1); j--)
   {
      m_pObjSideALst->RemoveRow( j - 1 );
   }

   if(l_bShowLast)
   {
      m_pObjSideALst->ScrollTo( m_pObjSideALst->Get_Nb_Rows() );
      l_bShowLast = false;
   }

	m_pObjSideALst->Update();

	//For each buddy on side B
	i = 0;
	l_It = l_SideB.begin();
	while(l_It != l_SideB.end())
	{
      //If it's the master country we must skip it
      if( (*l_It) == l_War.MasterDefending() )
      {
         l_It++;
         continue;
      }
      
      GUI::GListBox::GRow* l_pRow = NULL;

      if( (m_pObjSideBLst->FirstRowShown() + 1 + m_pObjSideBLst->NbVisibleRows()) >= m_pObjSideBLst->Get_Nb_Rows() )
      {
         l_bShowLast = true;
      }

		//Check if i need to insert a new row
		if(i >= (UINT32)m_pObjSideBLst->Get_Nb_Rows())
		{
			l_pRow = m_pObjSideBLst->Insert_New_Row();
		}
		else
		{
			l_pRow = m_pObjSideBLst->Get_Row_At(i);
		}

		//Get the country of the one on side B
		const GCountry & l_Ctry = g_ClientDAL.Country( (INT32)(*l_It) );
		
		//Set it's info
		((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
		((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text( l_Ctry.Name() );

		i++;
		l_It++;
	}
 
   //We must erase the rows that has not be used ( - 1 stands for the master country )
   for(INT32 j=m_pObjSideBLst->Get_Nb_Rows(); j>((INT32)l_SideB.size() - 1); j--)
   {
      m_pObjSideBLst->RemoveRow( j - 1 );
   }

   if(l_bShowLast)
   {
      m_pObjSideBLst->ScrollTo( m_pObjSideBLst->Get_Nb_Rows() );
      l_bShowLast = false;
   }

	m_pObjSideBLst->Update();

	//Get my own status
	bool l_bPlayerIsIn,
		  l_bPlayerIsAttacker,
		  l_bPlayerIsMaster;

	PlayerState(in_iWarID, l_bPlayerIsIn, l_bPlayerIsMaster, l_bPlayerIsAttacker);

	//Get the masters intentions
	bool l_bAttackerIntention = l_War.MasterAttackingWantsPeace();
	bool l_bDefenderIntention = l_War.MasterDefendingWantsPeace();

	//Set the toggle accordingly if they wants peace or war
   //---------------------------------------------------------------------------
   
   //Check if the user already changed his war state
   hash_map< UINT32, bool >::iterator l_ChangedIt = m_ChangedWars.find(in_iWarID);
   
   //If he didn't do it as it should
   if( l_ChangedIt == m_ChangedWars.end() )
   {
      m_pObjPeaceATog->ToggleState(l_bAttackerIntention);
	   m_pObjWarATog->ToggleState(!l_bAttackerIntention);

	   m_pObjPeaceBTog->ToggleState(l_bDefenderIntention);
	   m_pObjWarBTog->ToggleState(!l_bDefenderIntention);

      if(l_bAttackerIntention)
		   m_pObjStatusAPic->PictureNormal( GString(L"000862") );
	   else
		   m_pObjStatusAPic->PictureNormal( GString(L"000863") );

	   if(l_bDefenderIntention)
		   m_pObjStatusBPic->PictureNormal( GString(L"000862") );
	   else
		   m_pObjStatusBPic->PictureNormal( GString(L"000863") );
   }
   else
   {     
      if(l_bPlayerIsAttacker)
      {
         m_pObjPeaceATog->ToggleState(l_ChangedIt->second);
	      m_pObjWarATog->ToggleState(!l_ChangedIt->second);
         
         if(l_ChangedIt->second)
		      m_pObjStatusAPic->PictureNormal( GString(L"000862") );
	      else
		      m_pObjStatusAPic->PictureNormal( GString(L"000863") );

         if( l_ChangedIt->second == l_bAttackerIntention )
         {
            RemoveDirtyWar( l_ChangedIt->first )   ;
         }

	      m_pObjPeaceBTog->ToggleState(l_bDefenderIntention);
	      m_pObjWarBTog->ToggleState(!l_bDefenderIntention);
      }
      else
      {
	      m_pObjPeaceBTog->ToggleState(l_ChangedIt->second);
	      m_pObjWarBTog->ToggleState(!l_ChangedIt->second);

	      if(l_ChangedIt->second)
		      m_pObjStatusBPic->PictureNormal( GString(L"000862") );
	      else
		      m_pObjStatusBPic->PictureNormal( GString(L"000863") );

         if( l_ChangedIt->second == l_bDefenderIntention )
         {
            RemoveDirtyWar( l_ChangedIt->first )   ;
         }

         m_pObjPeaceATog->ToggleState(l_bAttackerIntention);
	      m_pObjWarATog->ToggleState(!l_bAttackerIntention);
      }
   }

   //---------------------------------------------------------------------------
   
   m_pObjPeaceATog->Enabled( l_bPlayerIsAttacker );
   m_pObjWarATog->Enabled( l_bPlayerIsAttacker );

   m_pObjPeaceBTog->Enabled( !l_bPlayerIsAttacker );
   m_pObjWarBTog->Enabled( !l_bPlayerIsAttacker );

	//Hide all buttons
	m_pObjJoinABtn->Hide();
	m_pObjJoinBBtn->Hide();
	m_pObjPeaceATog->Hide();
	m_pObjPeaceBTog->Hide();
	m_pObjWarATog->Hide();
	m_pObjWarBTog->Hide();

	//Depending of the player status check which button must be shown and enabled
	if(l_bPlayerIsIn)
	{
		m_pObjPeaceATog->Show();
		m_pObjWarATog->Show();
		m_pObjPeaceBTog->Show();
		m_pObjWarBTog->Show();

		if(l_bPlayerIsAttacker)
		{
			m_pObjPeaceATog->Enabled(l_bPlayerIsMaster);
			m_pObjWarATog->Enabled(l_bPlayerIsMaster);
		}
		else
		{
			m_pObjPeaceBTog->Enabled(l_bPlayerIsMaster);
			m_pObjWarBTog->Enabled(l_bPlayerIsMaster);
		}
	}
	else
	{
		m_pObjJoinABtn->Show();
		m_pObjJoinBBtn->Show();
	}
}