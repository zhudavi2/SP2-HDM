/**************************************************************
*
* sp2_treaties_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_treaties_window.h"
#include "../include/sp2_treaties_detailed_window.h"
#include "../include/sp2_generic_message_window.h"

const GString GTreatiesWindow::TypeName(L"Treaties_Window");

GUI::GBaseObject* GTreatiesWindow::New()
{
	return (GUI::GBaseObject*) new GTreatiesWindow();
}

GTreatiesWindow::GTreatiesWindow() : SP2::GBaseWindow()
{
   m_sFilterMember = L"";
   m_eFilterType   = ETreatyType::Undefined;
   
   m_iLastSelectedTreaty       = -1;
   m_iLastSelectedTreatyId     = -1;
   m_bLastSelectedTreatyActive = false;
}

GTreatiesWindow::~GTreatiesWindow()
{
}

void GTreatiesWindow::SetFilter(GString in_sCountryName, ETreatyType::Enum in_eTreatyType, bool in_bUpdateWindow)
{
   FilterMember(in_sCountryName, false);
   FilterType(in_eTreatyType, false);

   if(in_bUpdateWindow)
      Update();
}

void GTreatiesWindow::FilterMember(GString in_sCountryName, bool in_bUpdate)
{
   m_sFilterMember = in_sCountryName;
   m_pObjFilterMember->Selected_Content(m_sFilterMember);

   if(m_sFilterMember == g_ClientDAL.GetString(EStrId::NoFilter))
      m_sFilterMember = L"";

   if(in_bUpdate)
      Update();
}

GString GTreatiesWindow::FilterMember()
{
   return m_sFilterMember;
}

void GTreatiesWindow::FilterType(ETreatyType::Enum in_eTreatyType, bool in_bUpdate)
{
   m_eFilterType = in_eTreatyType;

   GString l_sType;
   if(m_eFilterType == ETreatyType::Undefined)
      l_sType = g_ClientDAL.GetString(EStrId::NoFilter);
   else
   {
      if(m_eFilterType == ETreatyType::Alliance)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyAlliance);
      else if(m_eFilterType == ETreatyType::Annexation)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyAnnexation);
      else if(m_eFilterType == ETreatyType::CommonMarket)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyCommonMarket);
      else if(m_eFilterType == ETreatyType::CulturalExchanges)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges);
      else if(m_eFilterType == ETreatyType::EconomicAid)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicAid);
      else if(m_eFilterType == ETreatyType::EconomicEmbargo)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo);
      else if(m_eFilterType == ETreatyType::EconomicPartnership)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership);
      else if(m_eFilterType == ETreatyType::AssumeForeignDebt)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt);
      else if(m_eFilterType == ETreatyType::FreeRegion)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyFreeRegion);
      else if(m_eFilterType == ETreatyType::HumanDevelopmentCollaboration)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration);
      else if(m_eFilterType == ETreatyType::MilitaryAccess)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess);
      else if(m_eFilterType == ETreatyType::NobleCause)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyNobleCause);
      else if(m_eFilterType == ETreatyType::RequestMilitaryPresenceRemoval)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval);
      else if(m_eFilterType == ETreatyType::ResearchPartnership)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyResearchPartnership);
      else if(m_eFilterType == ETreatyType::War)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyWar);
      else if(m_eFilterType == ETreatyType::WeaponTrade)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyWeaponTrade);
      else if(m_eFilterType == ETreatyType::WeaponTradeEmbargo)
         l_sType = g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo);
   }

   m_pObjFilterType->Selected_Content(l_sType);

   if(in_bUpdate)
      Update();
}

ETreatyType::Enum GTreatiesWindow::FilterType()
{
   return m_eFilterType;
}

GUI::EEventStatus::Enum GTreatiesWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //On view selected button
         if(in_pCaller == m_pObjViewSelected)
         {
            if(!g_ClientDDL.TreatyDetailsWindow())
               g_ClientDDL.TreatyDetailsWindowSpawn();

            g_ClientDDL.TreatyDetailsWindow()->Update(m_iLastSelectedTreatyId);
            
            if(!g_ClientDDL.TreatyDetailsWindow()->Visible())
               g_ClientDDL.TreatyDetailsWindow()->Show();
         }
         //On new treaty button
         else if(in_pCaller == m_pObjNewTreaty)
         {
            if(!g_ClientDDL.GenericMessageWindow())
               g_ClientDDL.GenericMessageWindowSpawn();

            //If your country is in anarchy you can't do a new treaty
            if(g_ClientDAL.m_PlayerCountryData.GvtType() == 1)
            {
               g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(100938), this );
               return GUI::EEventStatus::Handled;
            }
            
            if(!g_ClientDDL.TreatyDetailsWindow())
               g_ClientDDL.TreatyDetailsWindowSpawn();

            g_ClientDDL.TreatyDetailsWindow()->Update(-1);
            
            //Unselect the last selected treaty
            if(m_iLastSelectedTreaty != -1)
            {
               GUI::GListBox::GRow* l_pRow = NULL;
               
               if(m_bLastSelectedTreatyActive)
                  l_pRow = m_pObjActiveTreatiesLst->Get_Row_At(m_iLastSelectedTreaty);
               else
                  l_pRow = m_pObjPendingTreatiesLst->Get_Row_At(m_iLastSelectedTreaty);

               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
               
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));

               m_iLastSelectedTreaty = -1;
               m_pObjViewSelected->Enabled(false);
            }

            if(!g_ClientDDL.TreatyDetailsWindow()->Visible())
               g_ClientDDL.TreatyDetailsWindow()->Show();
         }
      }
   }

	return GUI::EEventStatus::Handled;
}

void GTreatiesWindow::Sort()
{
   for(INT32 i=m_pObjActiveTreatiesLst->Get_Nb_Rows() - 1; i>0; i--)
   {
      GUI::GLabel* l_pLabel1 = (GUI::GLabel*)m_pObjActiveTreatiesLst->Get_Row_At(i)->Object()->Child(L"txtName");
      GUI::GLabel* l_pLabel2 = NULL;
      GUI::GLabel* l_pLabelCmp = l_pLabel1;
      INT32 l_iBiggestID = i;

      for(INT32 j=0; j<i; j++)
      {
         l_pLabel2 = (GUI::GLabel*)m_pObjActiveTreatiesLst->Get_Row_At(j)->Object()->Child(L"txtName");

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
         m_pObjActiveTreatiesLst->SwapRows(i, l_iBiggestID);
         UINT32 l_iTemp = m_TreatyActiveId[l_iBiggestID];
         m_TreatyActiveId[l_iBiggestID] = m_TreatyActiveId[i];
         m_TreatyActiveId[i] = l_iTemp;
      }
   }
   m_pObjActiveTreatiesLst->Update();

   for(INT32 i=m_pObjPendingTreatiesLst->Get_Nb_Rows() - 1; i>0; i--)
   {
      GUI::GLabel* l_pLabel1 = (GUI::GLabel*)m_pObjPendingTreatiesLst->Get_Row_At(i)->Object()->Child(L"txtName");
      GUI::GLabel* l_pLabel2 = NULL;
      GUI::GLabel* l_pLabelCmp = l_pLabel1;
      INT32 l_iBiggestID = i;

      for(INT32 j=0; j<i; j++)
      {
         l_pLabel2 = (GUI::GLabel*)m_pObjPendingTreatiesLst->Get_Row_At(j)->Object()->Child(L"txtName");

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
         m_pObjPendingTreatiesLst->SwapRows(i, l_iBiggestID);
         UINT32 l_iTemp = m_TreatyPendingId[l_iBiggestID];
         m_TreatyPendingId[l_iBiggestID] = m_TreatyPendingId[i];
         m_TreatyPendingId[i] = l_iTemp;
      }
   }
   m_pObjPendingTreatiesLst->Update();

   //If there is a selected treaty, ensure that it stays selected when rows swap
   if(m_iLastSelectedTreaty != -1)
   {
      GUI::GListBox* l_pListbox;
      
      if(m_bLastSelectedTreatyActive)
         l_pListbox = m_pObjActiveTreatiesLst;
      else
         l_pListbox = m_pObjPendingTreatiesLst;

      const vector<INT32>& l_viItem = l_pListbox->Get_Selected_Item();

      if(l_viItem.size() > 0 && l_viItem[0] < l_pListbox->Get_Nb_Rows() )
      {
         UINT32 l_iTreatyID;
         if(m_bLastSelectedTreatyActive)
         {
            l_iTreatyID = m_TreatyActiveId[ l_viItem[0] ];
         }
         else
         {
            l_iTreatyID = m_TreatyPendingId[ l_viItem[0] ];
         }

         m_iLastSelectedTreaty   = l_viItem[0];
         m_iLastSelectedTreatyId = l_iTreatyID;
      }
   }
}

GUI::EEventStatus::Enum GTreatiesWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseDown(in_EventData, in_pCaller);
   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GTreatiesWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:

      if(in_EventData.Data == m_pObjFilterMember)
      {
         m_sFilterMember = m_pObjFilterMember->Selected_Content();

         if(m_sFilterMember == g_ClientDAL.GetString(EStrId::NoFilter))
            m_sFilterMember = L"";
      }

      //If a filter was set check which one was chosen
      if(in_EventData.Data == m_pObjFilterType)
      {
         GString l_sType = m_pObjFilterType->Selected_Content();

         if(l_sType == g_ClientDAL.GetString(EStrId::NoFilter))
            m_eFilterType = ETreatyType::Undefined;
         else
         {
            if(l_sType == g_ClientDAL.GetString(EStrId::TreatyAlliance))
               m_eFilterType = ETreatyType::Alliance;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyAnnexation))
               m_eFilterType = ETreatyType::Annexation;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyCommonMarket))
               m_eFilterType = ETreatyType::CommonMarket;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges))
               m_eFilterType = ETreatyType::CulturalExchanges;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyEconomicAid))
               m_eFilterType = ETreatyType::EconomicAid;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo))
               m_eFilterType = ETreatyType::EconomicEmbargo;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership))
               m_eFilterType = ETreatyType::EconomicPartnership;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt))
               m_eFilterType = ETreatyType::AssumeForeignDebt;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyFreeRegion))
               m_eFilterType = ETreatyType::FreeRegion;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration))
               m_eFilterType = ETreatyType::HumanDevelopmentCollaboration;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess))
               m_eFilterType = ETreatyType::MilitaryAccess;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyNobleCause))
               m_eFilterType = ETreatyType::NobleCause;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval))
               m_eFilterType = ETreatyType::RequestMilitaryPresenceRemoval;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyResearchPartnership))
               m_eFilterType = ETreatyType::ResearchPartnership;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyWar))
               m_eFilterType = ETreatyType::War;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyWeaponTrade))
               m_eFilterType = ETreatyType::WeaponTrade;
            else if(l_sType == g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo))
               m_eFilterType = ETreatyType::WeaponTradeEmbargo;
         }
      }

      //Apply filter
      Update(m_sFilterMember, m_eFilterType);
      break;

   case ECustomEvents::OnListItemSelected:
      {
         //If the user selected an empty row do nothing
         GUI::GListBox* l_pListbox = (GUI::GListBox*)in_EventData.Data;
         GUI::GListBox::GRow* l_pRow;
         vector<INT32> l_viItem = l_pListbox->Get_Selected_Item();
         if(l_viItem.size() > 0)
         {
            l_pRow = l_pListbox->Get_Row_At(l_viItem[0]);
            if(((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text() == L"")
            {
               break;
            }
         }

         //If there was a selected treaty unselect it
         if(m_iLastSelectedTreaty != -1)
         {
            if(m_bLastSelectedTreatyActive)
               l_pRow = m_pObjActiveTreatiesLst->Get_Row_At(m_iLastSelectedTreaty);
            else
               l_pRow = m_pObjPendingTreatiesLst->Get_Row_At(m_iLastSelectedTreaty);

            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
            
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));
         }
         else
         {
            //If there wasn't any selected treaty enable the view treaty button
            if(!m_pObjViewSelected->Enabled())
               m_pObjViewSelected->Enabled(true);
         }

         //check if the selected treaty is active or pending
         if(in_EventData.Data == m_pObjActiveTreatiesLst)
         {
            m_bLastSelectedTreatyActive = true;
         }

         if(in_EventData.Data == m_pObjPendingTreatiesLst)
         {
            m_bLastSelectedTreatyActive = false;
         }

         //Get the selected item in the list box and do the selection
         l_viItem.clear();
         l_viItem = l_pListbox->Get_Selected_Item();
         if(l_viItem.size() > 0)
         {
            m_iLastSelectedTreaty = l_viItem[0];
         }

         l_pRow = l_pListbox->Get_Row_At(m_iLastSelectedTreaty);
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000776"));
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000776"));

         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(255,255,255,255));
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(255,255,255,255));


         //Find the selected treaty id
         if(m_bLastSelectedTreatyActive)
            m_iLastSelectedTreatyId = m_TreatyActiveId[ m_iLastSelectedTreaty ];
         else
            m_iLastSelectedTreatyId = m_TreatyPendingId[ m_iLastSelectedTreaty ];
      }
      break;

   case ECustomEvents::OnListItemDoubleClicked:
      {
         if( (m_bLastSelectedTreatyActive && (in_pCaller == m_pObjActiveTreatiesLst)) || (!m_bLastSelectedTreatyActive && (in_pCaller == m_pObjPendingTreatiesLst)))
         {
            if(m_iLastSelectedTreatyId >= 0)
            {
               if(!g_ClientDDL.TreatyDetailsWindow())
                  g_ClientDDL.TreatyDetailsWindowSpawn();

               g_ClientDDL.TreatyDetailsWindow()->Update(m_iLastSelectedTreatyId);
               
               if(!g_ClientDDL.TreatyDetailsWindow()->Visible())
                  g_ClientDDL.TreatyDetailsWindow()->Show();
            }
         }
      }
      break;

   default:
      return GBaseObject::OnCustomEvent(in_iEventID, in_EventData, in_pCaller);
   }

   return GUI::EEventStatus::Handled;
}


bool GTreatiesWindow::OnCreate()
{
	__super::OnCreate();

   m_pData       = &(g_ClientDAL.m_DataTreatiesWindow);
   m_pWindowData = &(g_ClientDAL.m_DataTreatiesWindow);
   
   m_pObjApplyBtn->Hide();

   m_pObjActiveTreatiesLst  = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmActiveTreaties")->Child(L"lstTreaties");
   m_pObjPendingTreatiesLst = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmPendingTreaties")->Child(L"lstTreaties");
   m_pObjFilterMember       = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmFilters")->Child(L"cboCountry");
   m_pObjFilterType         = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmFilters")->Child(L"cboType");
   m_pObjNewTreaty          = (GUI::GButton*)Child(L"frmInner")->Child(L"btnNew");
   m_pObjViewSelected       = (GUI::GButton*)Child(L"frmInner")->Child(L"btnView");

   m_pObjViewSelected->Enabled(false);
   
   m_pObjFilterMember->Add_Item(g_ClientDAL.GetString(EStrId::NoFilter));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::NoFilter));

   m_pObjFilterMember->Selected_Content(g_ClientDAL.GetString(EStrId::NoFilter));
   m_pObjFilterType->Selected_Content(g_ClientDAL.GetString(EStrId::NoFilter));

   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAlliance));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAnnexation));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyCommonMarket));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicAid));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyFreeRegion));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyNobleCause));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyResearchPartnership));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWar));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWeaponTrade));
   m_pObjFilterType->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo));

   FillCountries();

   if(!g_ClientDDL.TreatyDetailsWindow())
      g_ClientDDL.TreatyDetailsWindowSpawn();

   if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());

	return true;
}

void GTreatiesWindow::FillCountries()
{
   m_pObjFilterMember->Clear();
   m_pObjFilterMember->Add_Item(g_ClientDAL.GetString(EStrId::NoFilter));
   g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), m_pObjFilterMember);  
}

bool GTreatiesWindow::OnDestroy()
{
   RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

void GTreatiesWindow::OnShow()
{
   __super::OnShow();
   
   g_ClientDAL.Attach(this, ENotifyWindowMsg::Treaties);

   Update(m_sFilterMember, m_eFilterType);
}

void GTreatiesWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Treaties);
   
   __super::OnHide();
}

void GTreatiesWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Treaties)
   {
      GTreatyNotifyData* l_pNotifyData = (GTreatyNotifyData*)in_pAdditionalMsg;
		if(!l_pNotifyData)
			return;

      //Try to find the treaty modified in the list
      hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().find(l_pNotifyData->m_iTreatyID);

      //If we can't find it we must erase it
      //if(l_It == g_ClientDAL.Treaties().end())
		if(l_pNotifyData->m_bToRemove)
      {
         //erase the treaty if it's in the pending section
         for(UINT32 i=0; i<m_TreatyPendingId.size(); i++)
         {
            if(m_TreatyPendingId[i] == (UINT32)l_pNotifyData->m_iTreatyID)
            {
               /*
               if(m_pObjPendingTreatiesLst->Get_Nb_Rows() > 5)
               {
               }
               else
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjPendingTreatiesLst->Get_Row_At( i );
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(L"");
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(L"");
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
						((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));
               }
*/
               if((INT32)m_TreatyPendingId[i] == m_iLastSelectedTreatyId)
               {
                  m_iLastSelectedTreaty   = -1;
                  m_iLastSelectedTreatyId = -1;
                  m_pObjViewSelected->Enabled(false);
               }

               m_pObjPendingTreatiesLst->RemoveRow( i );

               m_TreatyPendingId.erase( find(m_TreatyPendingId.begin(), m_TreatyPendingId.end(), m_TreatyPendingId[i]) );

               if(m_pObjPendingTreatiesLst->Get_Nb_Rows() < 5)
               {
                  m_pObjPendingTreatiesLst->Insert_New_Row();
                  m_pObjPendingTreatiesLst->Update();
               }

               break;
            }
         }

         for(UINT32 i=0; i<m_TreatyActiveId.size(); i++)
         {
            if(m_TreatyActiveId[i] == (UINT32)l_pNotifyData->m_iTreatyID)
            {
               /*
               if(m_pObjActiveTreatiesLst->Get_Nb_Rows() > 5)
               {
                  m_pObjActiveTreatiesLst->RemoveRow( i );
                  m_pObjActiveTreatiesLst->Update();
               }
               else
               {
                  GUI::GListBox::GRow* l_pRow = m_pObjActiveTreatiesLst->Get_Row_At( i );
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(L"");
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(L"");
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
						((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                  ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));
               }
               */

               m_pObjActiveTreatiesLst->RemoveRow( i );

               if((INT32)m_TreatyActiveId[i] == m_iLastSelectedTreatyId)
               {
                  m_iLastSelectedTreaty   = -1;
                  m_iLastSelectedTreatyId = -1;
                  m_pObjViewSelected->Enabled(false);
               }

               m_TreatyActiveId.erase( find(m_TreatyActiveId.begin(), m_TreatyActiveId.end(), m_TreatyActiveId[i]) );

               if(m_pObjActiveTreatiesLst->Get_Nb_Rows() < 5)
               {
                  m_pObjActiveTreatiesLst->Insert_New_Row();
                  m_pObjActiveTreatiesLst->Update();
               }

               break;
            }
         }
      }
      else
      {
         //If we found it add it
         for(UINT32 i=0; i<m_TreatyPendingId.size(); i++)
         {
            if(m_TreatyPendingId[i] == (UINT32)l_pNotifyData->m_iTreatyID)
            {
               hash_map<UINT32, SP2::GTreaty>::const_iterator l_It2 = g_ClientDAL.Treaties().find(l_pNotifyData->m_iTreatyID);
               if(l_It2 != g_ClientDAL.Treaties().end())
               {
                  //if the treaty passed from pending to active
                  if(l_It2->second.Active())
                  {/*
                     //Remove it in the pending list
                     if(m_pObjPendingTreatiesLst->Get_Nb_Rows() > 5)
                     {
                        m_pObjPendingTreatiesLst->Update();
                     }
                     else
                     {
                        GUI::GListBox::GRow* l_pRow = m_pObjPendingTreatiesLst->Get_Row_At( i );
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(L"");
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(L"");
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
								((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                     }
*/
                     if((INT32)m_TreatyPendingId[i] == m_iLastSelectedTreatyId)
                     {
                        m_iLastSelectedTreaty   = -1;
                        m_iLastSelectedTreatyId = -1;
                        m_pObjViewSelected->Enabled(false);
                     }

                     m_pObjPendingTreatiesLst->RemoveRow( i );
                     m_TreatyPendingId.erase( find(m_TreatyPendingId.begin(), m_TreatyPendingId.end(), m_TreatyPendingId[i]) );
                     
                     if(m_pObjPendingTreatiesLst->Get_Nb_Rows() < 5)
                     {
                        m_pObjPendingTreatiesLst->Insert_New_Row();
                        m_pObjPendingTreatiesLst->Update();
                     }

                     //Add it in the active list
                     AddTreaty(l_pNotifyData->m_iTreatyID);
                     
                     Sort();
                     m_pObjActiveTreatiesLst->Update();
                  }
               }
               return;
            }
         }

         //Add it in the active list
         for(UINT32 i=0; i<m_TreatyActiveId.size(); i++)
         {
            //If the modified treaty is actually shown somewhere in the list, ensure we are still in that treaty
            if(m_TreatyActiveId[i] == (UINT32)l_pNotifyData->m_iTreatyID)
            {
               //if there is a member filter be sure it's applied
               if(m_sFilterMember != L"")
               {
                  const GCountry& l_CtryFilter = g_ClientDAL.Country(m_sFilterMember);

                  bool l_bFound = false;
                  hash_map<UINT32, SP2::GTreaty>::const_iterator l_It2 = g_ClientDAL.Treaties().find(l_pNotifyData->m_iTreatyID);
                  const GTreaty* l_pTreaty = &(l_It2->second);
                  
                  l_bFound = l_pTreaty->MembersSideA(true).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideA(true).end();
                  l_bFound = l_bFound || (l_pTreaty->MembersSideB(true).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideB(true).end());
                  l_bFound = l_bFound || (l_pTreaty->MembersSideA(false).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideA(false).end());
                  l_bFound = l_bFound || (l_pTreaty->MembersSideB(false).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideB(false).end());
                  l_bFound = l_bFound || (l_pTreaty->MembersPressure().find(l_CtryFilter.Id()) != l_pTreaty->MembersPressure().end());
                  
                  //If my filter country wasn't found in the treaty remove it
                  if(!l_bFound)
                  {
                     m_pObjActiveTreatiesLst->RemoveRow( i );
                     m_pObjActiveTreatiesLst->Update();
/*
                     if(m_pObjActiveTreatiesLst->Get_Nb_Rows() > 5)
                     {
                     }
                     else
                     {
                        GUI::GListBox::GRow* l_pRow = m_pObjActiveTreatiesLst->Get_Row_At( i );
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(L"");
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(L"");
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->PictureNormal(GString(L"000587"));
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->PictureNormal(GString(L"000587"));
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                        ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->FontColorNormal(GColorRGBInt(200,200,200,255));
                     }
                     */
                     
                     m_TreatyActiveId.erase( find(m_TreatyActiveId.begin(), m_TreatyActiveId.end(), m_TreatyActiveId[i]) );
                     
                     m_iLastSelectedTreaty       = -1; 
                     m_iLastSelectedTreatyId     = -1;
                     m_bLastSelectedTreatyActive = false;
                     m_pObjViewSelected->Enabled(false);

                     //Sort();
                     if(m_pObjActiveTreatiesLst->Get_Nb_Rows() < 5)
                     {
                        m_pObjActiveTreatiesLst->Insert_New_Row();
                        m_pObjActiveTreatiesLst->Update();
                     }
   
                     return;
                  }
               }
               
               return;
            }
            else
               l_It++;
         }
         
         AddTreaty(l_pNotifyData->m_iTreatyID);
         Sort();
         m_pObjActiveTreatiesLst->Update();
         m_pObjPendingTreatiesLst->Update();
      }
   }
}

void GTreatiesWindow::AddTreaty(UINT32 in_iID)
{
   //Try to find the treaty to add in the list
   hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().find(in_iID);

   if(l_It == g_ClientDAL.Treaties().end())
      return;

   //if there is a member filter be sure it's applied
   if(m_sFilterMember != L"")
   {
      const GCountry& l_CtryFilter = g_ClientDAL.Country(m_sFilterMember);
      bool l_bFound = false;

      const GTreaty* l_pTreaty = &(l_It->second);
      
      l_bFound = l_pTreaty->MembersSideA(true).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideA(true).end();
      l_bFound = l_bFound || (l_pTreaty->MembersSideB(true).find(l_CtryFilter.Id())  != l_pTreaty->MembersSideB(true).end());
      l_bFound = l_bFound || (l_pTreaty->MembersSideA(false).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideA(false).end());
      l_bFound = l_bFound || (l_pTreaty->MembersSideB(false).find(l_CtryFilter.Id()) != l_pTreaty->MembersSideB(false).end());
      l_bFound = l_bFound || (l_pTreaty->MembersPressure().find(l_CtryFilter.Id())   != l_pTreaty->MembersPressure().end());
      l_bFound = l_bFound || (l_pTreaty->MembersOriginalsA().find(l_CtryFilter.Id()) != l_pTreaty->MembersOriginalsA().end());
      l_bFound = l_bFound || (l_pTreaty->MembersOriginalsB().find(l_CtryFilter.Id()) != l_pTreaty->MembersOriginalsB().end());
      
      if(!l_bFound)
      {
         return;
      }
   }
   
   //Check if there isn't any filter on the type
   if(m_eFilterType != l_It->second.Type() && m_eFilterType != ETreatyType::Undefined)
   {
      return;
   }

   //Get the treaty name
   GString l_sName   = l_It->second.Name();
   bool    l_bActive = l_It->second.Active();
   GString l_sType;

   l_sType = g_ClientDAL.FindTreatyName(l_It->second.Type());

   GUI::GListBox::GRow* l_pRow = NULL;

   //Add the item to active or pending depending of the treaty status
   if(l_bActive)
   {
      bool l_bFound = false;
      if(m_pObjActiveTreatiesLst->Get_Nb_Rows() <= 5)
      {
         for(INT32 i=0; i<m_pObjActiveTreatiesLst->Get_Nb_Rows(); i++)
         {
            GUI::GListBox::GRow* l_pRow2 = m_pObjActiveTreatiesLst->Get_Row_At(i);
            if(((GUI::GLabel*)l_pRow2->Object()->Child(L"txtName"))->Text() == L"")
            {
               l_pRow = l_pRow2;
               l_bFound = true;
               break;
            }
         }
      }
      if(!l_bFound)
         l_pRow = m_pObjActiveTreatiesLst->Insert_New_Row();

      m_TreatyActiveId.push_back( l_It->second.ID() );
   }
   else
   {
      bool l_bFound = false;
      if(m_pObjPendingTreatiesLst->Get_Nb_Rows() <= 5)
      {
         for(INT32 i=0; i<m_pObjPendingTreatiesLst->Get_Nb_Rows(); i++)
         {
            GUI::GListBox::GRow* l_pRow2 = m_pObjPendingTreatiesLst->Get_Row_At(i);
            if(((GUI::GLabel*)l_pRow2->Object()->Child(L"txtName"))->Text() == L"")
            {
               l_pRow = l_pRow2;
               l_bFound = true;
               break;
            }
         }
      }
      if(!l_bFound)
         l_pRow = m_pObjPendingTreatiesLst->Insert_New_Row();

      m_TreatyPendingId.push_back( l_It->second.ID() );
   }

   ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(l_sName);
   ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(l_sType);
}

bool GTreatiesWindow::Update()
{
   Update(m_sFilterMember, m_eFilterType);

   return true;
}

void GTreatiesWindow::Update(GString in_sMemberName, ETreatyType::Enum in_TypeFilter)
{
   UINT32 l_iNbActive  = 0;
   UINT32 l_iNbPending = 0;

   m_pObjActiveTreatiesLst->Clear();
   m_pObjPendingTreatiesLst->Clear();
   m_TreatyActiveId.clear();
   m_TreatyPendingId.clear();

   m_iLastSelectedTreaty = -1;

   const hash_map<UINT32, GTreaty>* l_pData = &(g_ClientDAL.Treaties());
   hash_map<UINT32, GTreaty>::const_iterator l_It = l_pData->begin();

   const GCountry* l_pCtryFilter = NULL;
   if(in_sMemberName != L"")
   {
      l_pCtryFilter = &(g_ClientDAL.Country(in_sMemberName) );
   }
   
   //Cycle through each treaty
   while(l_It != l_pData->end())
   {
      //if there is a member filter be sure it's applied
      if(in_sMemberName != L"")
      {
         bool l_bFound = false;

         const GTreaty* l_pTreaty = &(l_It->second);
         
         l_bFound = l_pTreaty->MembersSideA(true).find(l_pCtryFilter->Id()) != l_pTreaty->MembersSideA(true).end();
         l_bFound = l_bFound || (l_pTreaty->MembersSideB(true).find(l_pCtryFilter->Id()) != l_pTreaty->MembersSideB(true).end());
         l_bFound = l_bFound || (l_pTreaty->MembersSideA(false).find(l_pCtryFilter->Id()) != l_pTreaty->MembersSideA(false).end());
         l_bFound = l_bFound || (l_pTreaty->MembersSideB(false).find(l_pCtryFilter->Id()) != l_pTreaty->MembersSideB(false).end());
         l_bFound = l_bFound || (l_pTreaty->MembersPressure().find(l_pCtryFilter->Id()) != l_pTreaty->MembersPressure().end());

			if(l_pTreaty->Ponctual())
			{
				l_bFound = l_bFound || (l_pTreaty->MembersOriginalsA().find(l_pCtryFilter->Id()) != l_pTreaty->MembersOriginalsA().end());
				l_bFound = l_bFound || (l_pTreaty->MembersOriginalsB().find(l_pCtryFilter->Id()) != l_pTreaty->MembersOriginalsB().end());
			}

         if(!l_bFound)
         {
            l_It++;
            continue;
         }
      }
      
      //Check if there isn't any filter on the type
      if(m_eFilterType != l_It->second.Type() && m_eFilterType != ETreatyType::Undefined)
      {
         l_It++;
         continue;
      }

      //Get the treaty name
      GString l_sName   = l_It->second.Name();
      bool    l_bActive = l_It->second.Active();
      GString l_sType;

      l_sType = g_ClientDAL.FindTreatyName(l_It->second.Type());

      GUI::GListBox::GRow* l_pRow = NULL;

      //Add the item to active or pending depending of the treaty status
      if(l_bActive)
      {
         l_pRow = m_pObjActiveTreatiesLst->Insert_New_Row();
         l_iNbActive++;
         m_TreatyActiveId.push_back( l_It->second.ID() );
      }
      else
      {
         l_pRow = m_pObjPendingTreatiesLst->Insert_New_Row();
         l_iNbPending++;
         m_TreatyPendingId.push_back( l_It->second.ID() );
      }

      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtName"))->Text(l_sName);
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtType"))->Text(l_sType);
      
      l_It++;
   }

   //Be sure there is always 5 items created (art request)
   if(l_iNbActive < 5)
      for(UINT32 i=0; i<(5-l_iNbActive); i++)
         m_pObjActiveTreatiesLst->Insert_New_Row();

   if(l_iNbPending < 5)
      for(UINT32 i=0; i<(5-l_iNbPending); i++)
         m_pObjPendingTreatiesLst->Insert_New_Row();

   Sort();
   
   //Update each list box
   m_pObjActiveTreatiesLst->Update();
   m_pObjPendingTreatiesLst->Update();

   m_pObjViewSelected->Enabled(false);
}