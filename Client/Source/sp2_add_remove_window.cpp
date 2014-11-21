/**************************************************************
*
* sp2_add_remove_window.cpp
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
#include "golem_pch.hpp"

#include "../include/sp2_add_remove_window.h"

const GString GAddRemoveWindow::TypeName(L"Add_Remove_Window");

GUI::GBaseObject* SP2::GAddRemoveWindow::New()
{
	return (GUI::GBaseObject*) new GAddRemoveWindow();
}


GAddRemoveWindow::GAddRemoveWindow() : SP2::GBaseWindow()
{
   m_pCaller = NULL;
   m_bSingle = false;
}

GAddRemoveWindow::~GAddRemoveWindow()
{
}

bool GAddRemoveWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjSelectedALst   = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmItems")->Child(L"frmSideA")->Child(L"lstSelectedA");
   m_pObjSelectedBLst   = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmItems")->Child(L"frmSideB")->Child(L"lstSelectedB");
   m_pObjUnselectedLst  = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmItems")->Child(L"frmUnselected")->Child(L"lstUnselected");
   m_pObjUnselectedLst->LabelText("txtName");
   m_pObjSelectedBLst->LabelText("txtName");
   m_pObjUnselectedLst->LabelText("txtName");

   m_pObjLeftA          = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnLeftA");
   m_pObjRightA         = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnRightA");
   m_pObjLeftB          = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnLeftB");
   m_pObjRightB         = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnRightB");
   m_pObjLeftAB         = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnLeftAB");
   m_pObjRightAB        = (GUI::GButton*)Child(L"frmInner")->Child(L"frmItems")->Child(L"btnRightAB");

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Left(m_pObjLockTog->Left());
   m_pObjSelectedALst->BringToFront();

	return true;
}

void GAddRemoveWindow::OnShow()
{
   __super::OnShow();
   
   m_pObjApplyBtn->Enabled(true);   
}

GUI::EEventStatus::Enum GAddRemoveWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjExitBtn)
      {
         Hide();
         return EEventStatus::Handled;
      }
      
      if(in_pCaller == m_pObjApplyBtn)
      {
         m_pvSideA->clear();
         m_pvSideB->clear();

         for(INT32 i=0; i<m_pObjSelectedALst->Get_Nb_Rows(); i++)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjSelectedALst->Get_Row_At(i);
            const GCountry& l_Ctry = g_ClientDAL.Country(((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );
            m_pvSideA->push_back(l_Ctry.Id());
         }
         for(INT32 i=0; i<m_pObjSelectedBLst->Get_Nb_Rows(); i++)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjSelectedBLst->Get_Row_At(i);
            const GCountry& l_Ctry = g_ClientDAL.Country(((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );
            m_pvSideB->push_back(l_Ctry.Id());
         }
         
         SDK::GUI::GEventData l_Data;
         l_Data.Data = this;
         
         if(m_pCaller)
            m_pCaller->OnCustomEvent(c_OnAddRemoveCountrySelection, l_Data, this);

         Hide();

         return EEventStatus::Handled;
      }

      if(in_pCaller == m_pObjRightA)
         SwapRows(m_pObjUnselectedLst, m_pObjSelectedALst);

      if(in_pCaller == m_pObjRightB)
         SwapRows(m_pObjUnselectedLst, m_pObjSelectedBLst);
      
      if(in_pCaller == m_pObjRightAB)
         SwapRows(m_pObjSelectedALst, m_pObjSelectedBLst);
      
      if(in_pCaller == m_pObjLeftA)
         SwapRows(m_pObjSelectedALst, m_pObjUnselectedLst);
      
      if(in_pCaller == m_pObjLeftB)
         SwapRows(m_pObjSelectedBLst, m_pObjUnselectedLst);
      
      if(in_pCaller == m_pObjLeftAB)
         SwapRows(m_pObjSelectedBLst, m_pObjSelectedALst);
   }

   return GUI::EEventStatus::Handled;
}

void GAddRemoveWindow::SwapRows(GUI::GListBox* in_pFrom, GUI::GListBox* in_pTo)
{
   vector<INT32> l_vSel = in_pFrom->Get_Selected_Item();

   while(l_vSel.size() > 0)
   {
      if(m_bSingle)
      {
         if( in_pTo == m_pObjSelectedALst || in_pTo == m_pObjSelectedBLst )
         {
            if( (UINT32)in_pTo->Get_Nb_Rows() >= 1)
            {
               return;
            }
         }
      }

      if(l_vSel[0] == -1)
         return;

      GUI::GListBox::GRow* l_pRow = in_pFrom->Get_Row_At(l_vSel[0]);
      GUI::GListBox::GRow* l_pNew = in_pTo->Insert_New_Row();

      ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));
      
      const GCountry& l_Ctry = g_ClientDAL.Country(((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() );
      
      ((GUI::GLabel*)l_pNew->Object()->Child(L"txtName"))->Text(l_Ctry.Name());
      ((GUI::GPictureBox*)l_pNew->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
      
      if( !m_bPonctual && ( m_psIncluded->find( (UINT32)l_Ctry.Id() ) == m_psIncluded->end() ) )
         ((GUI::GLabel*)l_pNew->Object()->Child(L"txtName"))->FontColor( GColorRGBInt(200,0,0,255) );

      in_pFrom->RemoveRow(l_vSel[0]);
      l_vSel = in_pFrom->Get_Selected_Item();
   }

   in_pFrom->Update();
   in_pTo->Update();  

   Sort(in_pFrom);
   Sort(in_pTo);
   
   in_pFrom->Update();
   in_pTo->Update();  
}

GUI::EEventStatus::Enum GAddRemoveWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return EEventStatus::Handled;
}

void GAddRemoveWindow::Update(vector<INT32> & in_viSideA, vector<INT32> & in_viSideB, set<UINT32> & in_sIncluded, bool in_bTwoSide, bool in_bSingle, bool in_bPonctual)
{
   m_pvSideA = &in_viSideA;
   m_pvSideB = &in_viSideB;
   m_psIncluded = &in_sIncluded;
   m_bSingle = in_bSingle;
   m_bPonctual = in_bPonctual;

   if(m_bSingle)
   {
      m_pObjSelectedALst->SelectionMode(SDK::LIST_SELECT_MODE_ONE);
      m_pObjSelectedBLst->SelectionMode(SDK::LIST_SELECT_MODE_ONE);
      m_pObjUnselectedLst->SelectionMode(SDK::LIST_SELECT_MODE_ONE);

      if( m_pvSideA->size() > 1 )
      {
         INT32 l_iValue = m_pvSideA->at(0);
         m_pvSideA->clear();
         m_pvSideA->push_back( l_iValue );
      }
      
      if( m_pvSideB->size() > 1 )
      {
         INT32 l_iValue = m_pvSideB->at(0);
         m_pvSideB->clear();
         m_pvSideB->push_back( l_iValue );
      }
   }
   else
   {
      m_pObjSelectedALst->SelectionMode(SDK::LIST_SELECT_MODE_MULTI_EXTENDED);
      m_pObjSelectedBLst->SelectionMode(SDK::LIST_SELECT_MODE_MULTI_EXTENDED);
      m_pObjUnselectedLst->SelectionMode(SDK::LIST_SELECT_MODE_MULTI_EXTENDED);
   }

   if(in_bTwoSide)
   {
      m_pObjSelectedALst->Owner()->Height(170);
      m_pObjSelectedALst->Height(168);
      m_pObjLeftB->Show();
      m_pObjRightB->Show();
   }
   else
   {
      m_pObjSelectedALst->Owner()->Height(m_pObjUnselectedLst->Owner()->Height());
      m_pObjSelectedALst->Height(m_pObjUnselectedLst->Height());
      m_pObjLeftB->Hide();
      m_pObjRightB->Hide();

      m_pvSideB->clear();
   }
   
   SP2::GCountries & l_Ctries = g_ClientDAL.Countries();

   m_pObjSelectedALst->Clear();
   m_pObjSelectedBLst->Clear();
   m_pObjUnselectedLst->Clear();
   
   for(UINT32 i=0; i<l_Ctries.size(); i++)
   {
      if(!l_Ctries[i].IsActive())
         continue;

      GUI::GListBox::GRow* l_pRow = NULL;
      vector<INT32>::iterator l_It = find(m_pvSideA->begin(), m_pvSideA->end(), l_Ctries[i].Id());
      if(l_It != m_pvSideA->end())
      {
         l_pRow = m_pObjSelectedALst->Insert_New_Row();
      }
      else
      {
         l_It = find(m_pvSideB->begin(), m_pvSideB->end(), l_Ctries[i].Id());
         if(l_It != m_pvSideB->end())
         {
            l_pRow = m_pObjSelectedBLst->Insert_New_Row();
         }
         else
         {
            l_pRow = m_pObjUnselectedLst->Insert_New_Row();
         }
      }

      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(l_Ctries[i].Name());
      
      if(!m_bPonctual && m_psIncluded->find((UINT32)l_Ctries[i].Id()) == m_psIncluded->end())
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColor(GColorRGBInt(200,0,0,255));

      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctries[i].Id()]);
   }

   Sort(m_pObjSelectedALst);
   Sort(m_pObjSelectedBLst);
   Sort(m_pObjUnselectedLst);
}

void GAddRemoveWindow::Sort(GUI::GListBox* in_pList)
{
   for(INT32 i=in_pList->Get_Nb_Rows() - 1; i>0; i--)
   {
      GUI::GLabel* l_pLabel1 = (GUI::GLabel*)in_pList->Get_Row_At(i)->Object()->Child(L"txtName");
      GUI::GLabel* l_pLabel2 = NULL;
      GUI::GLabel* l_pLabelCmp = l_pLabel1;
      INT32 l_iBiggestID = i;

      for(INT32 j=0; j<i; j++)
      {
         GUI::GListBox::GRow* l_pRow = in_pList->Get_Row_At(j);
         
         l_pLabel2 = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");

         //Ensure the empty rows gets to the end of the listbox
         if(l_pLabel1->Text() != L"")
         {
            if(l_pLabel2->Text() == L"")
            {
               l_iBiggestID = j;
               break;
            }
         }
         else
         {
            break;
         }
         
         //Check which one is the smaller and if it's a new one keep it
         if(l_pLabel1->Text() < l_pLabel2->Text() && l_pLabelCmp->Text() < l_pLabel2->Text())
         {
            l_iBiggestID = j;
            l_pLabelCmp = l_pLabel2;
         }
      }

      if(l_iBiggestID != i)
      {
         //Swap the 2 rows
         in_pList->SwapRows(i, l_iBiggestID);
      }
   }

   in_pList->Update();
}

void GAddRemoveWindow::Caller(GUI::GBaseObject* in_pCaller)
{
   m_pCaller = in_pCaller;
}

GUI::EEventStatus::Enum GAddRemoveWindow::OnKeyDown(const GEventData &in_pEventData, GBaseObject* in_pObj)
{
   return m_pObjUnselectedLst->OnKeyDown(in_pEventData, in_pObj);
}
