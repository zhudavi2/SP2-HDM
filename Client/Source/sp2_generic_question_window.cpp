/**************************************************************
*
* sp2_generic_message_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_generic_question_window.h"

const GString GGenericQuestionWindow::TypeName(L"Generic_Question_Window");
list<GGenericQuestionWindow*> GGenericQuestionWindow::m_OpenQuestionWindow;

GUI::GBaseObject* GGenericQuestionWindow::New()
{
	return (GUI::GBaseObject*) new GGenericQuestionWindow();
}


GGenericQuestionWindow::GGenericQuestionWindow() : GBaseWindow()
{
}

GGenericQuestionWindow::~GGenericQuestionWindow()
{
}

bool GGenericQuestionWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjOkBtn = (GUI::GButton*)Child(L"btnOk");
   m_pObjCancelBtn = (GUI::GButton*)Child(L"btnCancel");
   m_pObjTextLbl = (GUI::GLabel*)Child(L"lblMessage");

	return true;
}

void GGenericQuestionWindow::KillMe(void)
{
   // Detach, notification was sent
   Detach(m_Question.m_pOberserToNotify,c_OnGenericQuestionWindowAnswerCancel);
   Detach(m_Question.m_pOberserToNotify,c_OnGenericQuestionWindowAnswerOk);

   // Remove from modal list
   if(m_Question.m_pParentWindow)
   {
      vector<GUI::GBaseObject*>::iterator l_It = find(m_Question.m_pParentWindow->RegisteredModalWindow.begin(), m_Question.m_pParentWindow->RegisteredModalWindow.end(), (SDK::GUI::GBaseObject*)this);
      gassert(l_It != RegisteredModalWindow.end(),"We attached ourselve in the show...it has to be there");
      m_Question.m_pParentWindow->RegisteredModalWindow.erase(l_It);
   }

   // Kill it
   g_Joshua.GUIManager()->ReleaseObjectAsync(this);

   m_bEventSent = true;
}

GUI::EEventStatus::Enum GGenericQuestionWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(!in_pCaller)
      return GUI::EEventStatus::Handled;
   
   if((in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left) &&  m_bEventSent == false)
   {

      if(in_pCaller == m_pObjOkBtn)
      {
         //Notify any potential observer
         NotifyObserver(c_OnGenericQuestionWindowAnswerOk,this);

         // Make some noise
         g_SP2Client->PlaySound(ESoundEventID::ConfirmOrder);
      }
      else if(in_pCaller == m_pObjCancelBtn)
      {
         //Notify any potential observer
         NotifyObserver(c_OnGenericQuestionWindowAnswerCancel,this);
      }

      if(in_pCaller == m_pObjCancelBtn || in_pCaller == m_pObjOkBtn)
      {
         m_OpenQuestionWindow.remove(this);
         KillMe();
      }
   }

   return GUI::EEventStatus::Handled;
}

void GGenericQuestionWindow::Show(GGenericQuestion  &in_Question)
{
   // Get Question
   m_Question = in_Question;
   m_bEventSent = false;

   // Compose message
   m_Question.m_sMessage = Hector::Util::ReplaceEndLinesByRealEndLines(m_Question.m_sMessage);

   UINT32 l_iPos = m_Question.m_sMessage.find(L"\\n");
   while (l_iPos != string::npos)
   {
      m_Question.m_sMessage.replace(l_iPos, 2, L"\n");
      l_iPos = m_Question.m_sMessage.find(L"\\n", l_iPos);
   }
   
   m_pObjTextLbl->Text(m_Question.m_sMessage);

   if(m_Question.m_sOkCaption != L"")
      m_pObjOkBtn->Text(m_Question.m_sOkCaption);

   if(m_Question.m_sCancelCaption != L"")
      m_pObjCancelBtn->Text(m_Question.m_sCancelCaption);

   if(m_pObjTextLbl->Height() != (INT32)m_pObjTextLbl->Text2D()->Height())
   {
      UINT32 l_iDiff = m_pObjTextLbl->Text2D()->Height() - m_pObjTextLbl->Height();
      m_pObjTextLbl->Height(m_pObjTextLbl->Height() + l_iDiff + 4);
      m_pObjOkBtn->Top(m_pObjTextLbl->Height() + m_pObjTextLbl->Top() + 12);
      m_pObjCancelBtn->Top(m_pObjOkBtn->Top());
      Height(m_pObjOkBtn->Top() + m_pObjOkBtn->Height() + 10);
   }

   if(m_Question.m_pParentWindow)
      m_Question.m_pParentWindow->RegisteredModalWindow.push_back(this);
   
   // Attach observer to notification system
   Attach(m_Question.m_pOberserToNotify,c_OnGenericQuestionWindowAnswerCancel);
   Attach(m_Question.m_pOberserToNotify,c_OnGenericQuestionWindowAnswerOk);

   // Play and advisor warning sound event.
   g_SP2Client->PlaySound(ESoundEventID::AdvisorWarning);

   m_OpenQuestionWindow.push_back(this);

   __super::Show();
}

bool GGenericQuestionWindow::OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller)
{
   if((in_pFocusTo != this) && (in_pFocusTo != m_pObjOkBtn) && (in_pFocusTo != m_pObjCancelBtn) && Visible())
   {
      g_Joshua.GUIManager()->Focus(this);
      BringToFront();
   }

   return true;
}

bool GGenericQuestionWindow::OnGotFocus(const GBaseObject* in_pFocusFrom, GBaseObject* in_pCaller)
{
   BringToFront();
   return true;
}

void* GGenericQuestionWindow::OkValue() const
{
   return m_Question.m_pOnOkValue;
}
void* GGenericQuestionWindow::CancelValue() const
{
   return m_Question.m_pOnCancelValue;
}


void GGenericQuestionWindow::CleanQuestionWindow(void)
{
   // Kill all remaning question
   list<GGenericQuestionWindow*>::iterator l_It = m_OpenQuestionWindow.end();
   while(l_It != m_OpenQuestionWindow.begin())
   {
      
      (*l_It)->KillMe();
      l_It++;
   }

   m_OpenQuestionWindow.clear();
}