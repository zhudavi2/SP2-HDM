/**************************************************************
*
* sp2_party_selection_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_party_selection_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GPartySelectionWindow::TypeName(L"Party_Selection_Window");

GUI::GBaseObject* GPartySelectionWindow::New()
{
	return (GUI::GBaseObject*) new GPartySelectionWindow();
}


GPartySelectionWindow::GPartySelectionWindow() : GBaseWindow()
{
   m_iSelectedRowID = -1;
}

GPartySelectionWindow::~GPartySelectionWindow()
{
}

bool GPartySelectionWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   if(m_pObjApplyBtn)
      m_pObjApplyBtn->Hide();

   if(m_pObjLockTog)
      m_pObjLockTog->Hide();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

   m_pObjPartiesLst  = (GUI::GListBox*)m_pObjInnerFrm->Child(L"frmParties")->Child(L"lstParties");
   m_pObjConfirmBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancelBtn   = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

   for(INT32 i=0; i<8; i++)
      m_pObjPartiesLst->Insert_New_Row();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GPartySelectionWindow::OnShow()
{
   __super::OnShow();

   if( m_iSelectedRowID >= 0)
      m_pObjConfirmBtn->Enabled(true);
   else
      m_pObjConfirmBtn->Enabled(false);

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GPartySelectionWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn || in_pCaller == m_pObjCancelBtn)
         {
            Hide();
         }
         else if(in_pCaller == m_pObjConfirmBtn)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjPartiesLst->Get_Row_At(m_iSelectedRowID);
            hash_map<GString, INT32>::iterator l_It = m_mParties.find( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );
            
            GUI::GEventData l_Data;
            l_Data.Data = (void*)l_It->second;
            m_pCaller->FireCustomEvent( c_iOnPartySelection, l_Data, false, this );
            
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GPartySelectionWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch(in_iEventID)
   {
   case ECustomEvents::OnListItemSelected:
      {
         if( m_pObjPartiesLst->Get_Selected_Item().size() )
         {
            GUI::GListBox::GRow* l_pRow = m_pObjPartiesLst->Get_Row_At( m_pObjPartiesLst->Get_Selected_Item()[0] );
            GUI::GLabel* l_pName = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
            if( l_pName->Text() != L"" )
            {
               if( m_iSelectedRowID >= 0)
               {
                  l_pRow = m_pObjPartiesLst->Get_Row_At( m_iSelectedRowID );
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal( GString(L"000587") );
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->PictureNormal( GString(L"000587") );
               }
               
               m_iSelectedRowID = m_pObjPartiesLst->Get_Selected_Item()[0];
               l_pRow = m_pObjPartiesLst->Get_Row_At( m_iSelectedRowID );
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal( GString(L"000647") );
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->PictureNormal( GString(L"000647") );

               m_pObjConfirmBtn->Enabled(true);
            }
         }  
      }
      break;
   }


   return GUI::EEventStatus::Handled;
}

void GPartySelectionWindow::Show(const vector<GPoliticalParty> & in_vParties, INT32 in_iActivePartyID, GUI::GBaseObject* in_pCaller)
{  
   m_pCaller = in_pCaller;
   
   for(INT32 i=(INT32)m_pObjPartiesLst->Get_Nb_Rows() - 1; i>=0; i--)
   {
      if(i >= 8)
         m_pObjPartiesLst->RemoveRow( i );
      else
      {
         GUI::GListBox::GRow* l_pRow = m_pObjPartiesLst->Get_Row_At(i);
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text( L"" );
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->Text( L"" );
      }
   }

   if( m_iSelectedRowID >= 0)
   {
      // be sure the row exists ...
      if(m_iSelectedRowID < m_pObjPartiesLst->Get_Nb_Rows())
      {
         GUI::GListBox::GRow* l_pRow = m_pObjPartiesLst->Get_Row_At( m_iSelectedRowID );
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal( GString(L"000587") );
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->PictureNormal( GString(L"000587") );
      }
      else
         m_iSelectedRowID = -1;
   }
   
   for(UINT32 i=0; i<in_vParties.size(); i++)
   {
      GUI::GListBox::GRow* l_pRow = NULL;
      
      if(i >= 8)
      {
         l_pRow = m_pObjPartiesLst->Insert_New_Row();
      }
      else
      {
         l_pRow = m_pObjPartiesLst->Get_Row_At(i);
      }

      if( in_iActivePartyID == in_vParties[i].Id() )
      {
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal( GString(L"000647") );
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->PictureNormal( GString(L"000647") );
         m_iSelectedRowID = i;
      }

      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text( g_ClientDAL.GetString( in_vParties[i].NameId() ) );
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtApproval"))->Text( GString::FormatNumber( in_vParties[i].PercentageValue() * 100, L"", L".", L"", L" %", 0, 1, false ) );
      
      m_mParties.insert( make_pair( g_ClientDAL.GetString( in_vParties[i].NameId() ), in_vParties[i].Id() ) );
   }

   __super::Show();
}