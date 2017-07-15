/**************************************************************
*
* sp2_mail_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_mail_window.h"
#include "../include/sp2_text_overlay.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GMailWindow::TypeName(L"Mail_Window");

GUI::GBaseObject* GMailWindow::New()
{
	return (GUI::GBaseObject*) new GMailWindow();
}


GMailWindow::GMailWindow() : GUI::GFrame()
{
   m_iNextMailID = 1;
}

GMailWindow::~GMailWindow()
{
}

bool GMailWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjMailsLst = (GUI::GListBox*)Child(L"frmInner")->Child(L"lstMail");
   m_pObjMailsLst->ItemColorNormal( GColorRGBInt(0,0,0,0) );

   Position( g_ClientDDL.MainBar()->Left() + 190, g_ClientDDL.MainBar()->Top() - g_ClientDDL.MainBar()->Height() - Height()  + 10 );

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GMailWindow::OnShow()
{
   __super::OnShow();

   BringToFront();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   Focus();
}

EEventStatus::Enum GMailWindow::OnMouseLeave(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(!in_pCaller)
      HideOverlay();

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMailWindow::OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left == in_EventData.Mouse.Down.Bits.Left)
   {
      if(!in_pCaller)
      {
         HideOverlay();
         Drag();
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GMailWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{  
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller)
      {
         // View one mail
         if(in_pCaller->Name() == L"btnView")
         {
            INT32 l_iRowID = m_pObjMailsLst->MouseOverWhichRow();

            if(l_iRowID < 0)
               return GUI::EEventStatus::Handled;
            
            ShowMail( l_iRowID );
            RemoveMail( m_vMailID[ l_iRowID ] );
         }
         // Delete one mail
         else if(in_pCaller->Name() == L"btnDelete")
         {
            INT32 l_iRowID = m_pObjMailsLst->MouseOverWhichRow();

            if(l_iRowID < 0)
               return GUI::EEventStatus::Handled;

            RemoveMail( m_vMailID[ l_iRowID ] );                       
         }
         // Delete all mail
         else if(in_pCaller->Name() == L"btnDeleteAll")
         {
            RemoveAllMails();
         }
         in_pCaller = NULL;
      }
   }

   return GUI::EEventStatus::Handled;
}

bool GMailWindow::OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller)
{
   BringToFront();  

   if(g_ClientDDL.MainBar() && g_ClientDDL.MainBar()->Visible())
      g_ClientDDL.MainBar()->BringToFront();

   if(g_ClientDDL.MiniMap() && g_ClientDDL.MiniMap()->Visible())      
      g_ClientDDL.MiniMap()->BringToFront();
   
   return false;
}

INT32 GMailWindow::AddMail(GString in_sMail, void (*in_sFunction)(void*), void* in_sParam )
{
	GUI::GListBox::GRow* l_pRow = m_pObjMailsLst->Insert_New_Row();
   ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Text(in_sMail);

   m_vFunctions.push_back( in_sFunction );
   m_vParams.push_back( in_sParam );
   m_vMailID.push_back(m_iNextMailID);
   m_vRowID.push_back( l_pRow->m_iId );

   if(in_sFunction == NULL)
   {
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width() + ((GUI::GButton*)l_pRow->Object()->Child(L"btnView"))->Width() );
      ((GUI::GButton*)l_pRow->Object()->Child(L"btnView"))->Hide();
   }

   m_pObjMailsLst->Update();

   // Play a sound only for first mail.
   if(m_vMailID.size() == 1)
      g_SP2Client->PlaySound(ESoundEventID::IncomingMail);

   m_iNextMailID++;
   if(m_iNextMailID == 0)
      m_iNextMailID++;

   return m_vMailID.back();
}

void GMailWindow::OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative)
{
   __super::OnDrag(in_EventData, in_MouseOffset, in_MouseRelative);

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   
   if((Top() + Height()) > mode.m_Resolution.y)
   {
      Top( mode.m_Resolution.y - Height() );
   }
}

bool GMailWindow::ChangeMail(INT32 in_iID, GString in_sMail, void (*in_sFunction)(void*), void* in_sParam)
{
   for(INT32 i=0; i<(INT32)m_vFunctions.size(); i++)
   {
      if(m_vMailID[i] == in_iID)
      {
         GUI::GListBox::GRow* l_pRow = m_pObjMailsLst->Get_Row_At(m_vRowID[i]);
         if(l_pRow)
         {
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Text(in_sMail);
            // could we view the mail before
            bool l_bCouldView = (m_vFunctions[i] != NULL);
            // assign new function and params
            m_vFunctions[i] = in_sFunction;
            m_vParams[i]    = in_sParam;
            // retrieve our button width
            INT32 l_iBtnViewWidth = ((GUI::GButton*)l_pRow->Object()->Child(L"btnView"))->Width();
            // set up mails buttons hence what we can do
            if(in_sFunction == NULL)
            {
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width( ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width() + (l_bCouldView ? l_iBtnViewWidth : 0));
               ((GUI::GButton*)l_pRow->Object()->Child(L"btnView"))->Hide();
            }
            else
            {
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width(((GUI::GLabel*)l_pRow->Object()->Child(L"txtMail"))->Width() - (l_bCouldView ? 0: l_iBtnViewWidth));
               ((GUI::GButton*)l_pRow->Object()->Child(L"btnView"))->Show();
            }
            return true;
         }
      }
   }
   
   return false;
}

void GMailWindow::ShowMail(INT32 in_iID)
{
   (*m_vFunctions[in_iID])(m_vParams[in_iID]);
}

void GMailWindow::RemoveAllMails()
{
   // remove every mail entry in our vector
   while((INT32)m_vMailID.size())
   {
      RemoveMail(m_vMailID[0]);
   }
}

void GMailWindow::RemoveMail(INT32 in_iMailID)
{
   gassert(m_vFunctions.size() == m_vParams.size() && m_vFunctions.size() == m_vMailID.size(),"Incoherence");

   vector<void (*)(void*) >::iterator l_It   = m_vFunctions.begin();
   vector<void*>::iterator l_It2             = m_vParams.begin();
   vector<INT32>::iterator l_It3             = m_vMailID.begin();
   vector<INT32>::iterator l_It4             = m_vRowID.begin();
   UINT32 i;

   for(i=0;  i < m_vMailID.size() ; i++)
   {
      if(in_iMailID == *l_It3)
         break;
      
      l_It++;
      l_It2++;
      l_It3++;
      l_It4++;
   }

   //If the mail was found in the list, remove from the list, else ignore the event
   gassert(l_It3 != m_vMailID.end(),"Mail not found");

   if(l_It3 != m_vMailID.end())
   {
      //Erase the element
      m_vFunctions.erase(l_It);
      m_vParams.erase(l_It2);
      m_vMailID.erase(l_It3);
      m_vRowID.erase(l_It4);
   
      //Remove the row
      m_pObjMailsLst->RemoveRow(i);
      m_pObjMailsLst->Update();

      for(INT32 j=0; j<(INT32)m_vRowID.size(); j++)
      {
         if(j >= (INT32)i)
            m_vRowID[j] = m_vRowID[j] - 1;
      }
   
      //If no more rows, hide the window, untoggle
      if(!m_pObjMailsLst->Get_Nb_Rows())
      {
         Close();
      }
   }

   //We must check if the mail is a treaty mail, if so ensure it doesn't exist anymore in the treaties mail map
   for( hash_map<UINT32, INT32>::iterator l_It = g_ClientDAL.m_TreatiesMail.begin(); l_It != g_ClientDAL.m_TreatiesMail.end(); l_It++  )
   {
      if( l_It->second == in_iMailID )
      {
         g_ClientDAL.m_TreatiesMail.erase( l_It );
         break;
      }
   }

   //We must check if the mail is a treaty mail, if so ensure it doesn't exist anymore in the treaties mail map
   for( hash_map<UINT32, INT32>::iterator l_It = g_ClientDAL.m_CellsIdReady.begin(); l_It != g_ClientDAL.m_CellsIdReady.end(); l_It++  )
   {
      if( l_It->second == in_iMailID )
      {
         g_ClientDAL.m_CellsIdReady.erase( l_It );
         break;
      }
   }

	// Remove associated treaty refusal mail
   g_ClientDAL.RemoveTreatyRefusalMail(in_iMailID);
   // Remove associated trade mail
   g_ClientDAL.RemoveTradeMail(in_iMailID);
   // Remove associated combat over mail
   g_ClientDAL.RemoveCombatOverMail(in_iMailID);
}


UINT32 GMailWindow::NbPendingMessages() const
{
   gassert(m_vMailID.size() == m_vParams.size() && m_vParams.size() == m_vFunctions.size(),L"GMailWindow::NbPendingMessages() Incoherence, all vectors should have same size");
   return m_vMailID.size();
}

bool GMailWindow::MailExists(UINT32 in_iMailID) const
{
   if(find(m_vMailID.begin(),m_vMailID.end(),in_iMailID) == m_vMailID.end())
      return false;
   else
      return true;
}

EEventStatus::Enum GMailWindow::OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   INT32 x = in_EventData.X();
   INT32 y = in_EventData.Y();
   
   if( (x >= m_pObjMailsLst->Left()) && (x <= (m_pObjMailsLst->Left() + m_pObjMailsLst->Width())) && (y >= m_pObjMailsLst->Top()) && (y <= (m_pObjMailsLst->Top() + m_pObjMailsLst->Height())) )
   {
      DisplayOverlay();
   }

   return GUI::EEventStatus::Handled;
}

void GMailWindow::DisplayOverlay()
{
   if(!Visible())
   {
      HideOverlay();    
      return;
   }

   //This code will handle the overlay appearing on news that are too long
   bool  l_bMustDisplayOverlay = false;

   GVector2D<INT32> l_Pos = AbsoluteToRelative( g_Joshua.GUIManager()->MousePosition() );
   
   INT32 x = l_Pos.x;
   INT32 y = l_Pos.y;

   //If the user is in the Event listbox zone
   if( (x >= m_pObjMailsLst->Left()) && (x <= (m_pObjMailsLst->Left() + m_pObjMailsLst->Width())) && (y >= m_pObjMailsLst->Top()) && (y <= (m_pObjMailsLst->Top() + m_pObjMailsLst->Height())) )
   {
      y = y - m_pObjMailsLst->Top();

      INT32 l_iOverRowID = (INT32)floor( (REAL32)y / (REAL32)m_pObjMailsLst->RowHeight() );
      GUI::GListBox::GRow* l_pRow = m_pObjMailsLst->Get_Row_At(l_iOverRowID + m_pObjMailsLst->FirstRowShown());
      if(l_pRow)
      {
         GUI::GLabel* l_pEvent = (GUI::GLabel*)l_pRow->Object()->Child(L"txtMail");
         
         if( (x >= l_pEvent->Left()) && (x <= (l_pEvent->Left() + l_pEvent->Width())) )
         {
            //If the text do not fit the box, position and set text to the overlay
            if(l_pEvent->Width() < (INT32)l_pEvent->Text2D()->Width())
            {
               g_ClientDDL.TextOverlay()->Update( l_pEvent->Text(), l_pEvent );
               l_bMustDisplayOverlay = true;
            }
         }
      }
   }

   if(l_bMustDisplayOverlay)
   {
      g_ClientDDL.TextOverlay()->Show();
   }
   else
   {
      g_ClientDDL.TextOverlay()->Hide();
   }
}

void GMailWindow::HideOverlay()
{
   g_ClientDDL.TextOverlay()->Hide();
}

EEventStatus::Enum GMailWindow::OnMouseOut(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   DisplayOverlay();
   return EEventStatus::Handled;
}

/*!
 * on key down event handler
 **/
GUI::EEventStatus::Enum GMailWindow::OnKeyDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Key())
   {
      // escape key pressed ?
      case VK_ESCAPE:
      {
         // close our window
         Close();
         return EEventStatus::Handled;
      }
      default:
      {
         // check if we dont have any owner but we have a parent
         if(!Owner() && Parent())
         {
            // broadcast our key down event to the parent
            return Parent()->OnKeyDown(in_EventData, in_pCaller); 
         }
      }
   }
   return EEventStatus::Callback;
}

/*!
 * Close our mail window
 **/
void GMailWindow::Close()
{
   Hide();
   g_ClientDDL.MainBar()->m_pObjMail->PictureNormal(L"000813");
   g_ClientDDL.MainBar()->m_pObjMail->ToggleState(false);
}