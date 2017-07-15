/**************************************************************
*
* sp2_treaties_detailed_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_treaties_detailed_window.h"
#include "../include/sp2_add_remove_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_data_access_layer.h"

const GString GTreatyDetailsWindow::TypeName(L"Treaty_Details_Window");

GUI::GBaseObject* GTreatyDetailsWindow::New()
{
	return (GUI::GBaseObject*) new GTreatyDetailsWindow();
}

GTreatyDetailsWindow::GTreatyDetailsWindow() : SP2::GBaseWindow()
{
   m_pCurrentTreaty = NULL;
   m_bSideADirty = false;
   m_bSideBDirty = false;
   m_bTreatySameName = false;
   m_bCountryNotEligible = false;
   m_bSingleOnSide = false;
   m_bSkipEligibleWarning = false;
   m_bWaitForEligibleAnswer = false;
   m_bTreatyIsActive = false;
   m_bIsEditingName = false;

   m_iTreatyID = -1;
}

GTreatyDetailsWindow::~GTreatyDetailsWindow()
{
}

GUI::EEventStatus::Enum GTreatyDetailsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);
   
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      //If the caller is the modify button
      if(in_pCaller == m_pObjModifyBtn)
      {
         //If the button text was modify
         if(m_pObjModifyBtn->Text() == g_ClientDAL.GetString(EStrId::Modify))
         {
            //Display the country selection add remove window
            if(!g_ClientDDL.AddRemoveWindow())
               g_ClientDDL.AddRemoveWindowSpawn();
               
            ETreatyType::Enum l_eType = FindTreatyType(m_pObjTypeCbo->Selected_Content());
            bool l_bIsPonctual = false;

            if(l_eType != ETreatyType::Undefined)
               l_bIsPonctual = GTreaty::Ponctual(l_eType);
            
            //Initialize the add remove window depending if it's a two sided treaty
            if(m_bTwoSidesTreaty)
            {
               g_ClientDDL.AddRemoveWindow()->Update(m_viSideA, m_viSideB, m_siEligibleCountries, true, m_bSingleOnSide, l_bIsPonctual);
               m_bSideBDirty = true;
            }
            else
            {
               g_ClientDDL.AddRemoveWindow()->Update(m_viSideA, m_viSideB, m_siEligibleCountries, false, m_bSingleOnSide, l_bIsPonctual);
               m_bSideADirty = true;
            }
            
            //Set the window caller
            g_ClientDDL.AddRemoveWindow()->Caller(this);
            g_ClientDDL.AddRemoveWindow()->Show();
         }
         else
         {
            //JOIN CASE
            if(m_pObjModifyBtn->Text() == g_ClientDAL.GetString(EStrId::Join))
            {
               //Send the join event to the server
               SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyJoin);
               SP2::Event::GTreatyJoin* l_pTreaty = (SP2::Event::GTreatyJoin*)l_pEvent.get();

               vector<INT32>::iterator l_It2 = find(m_viSideB.begin(), m_viSideB.end(), g_ClientDAL.ControlledCountryID());
               if(l_It2 != m_viSideB.end())
                  l_pTreaty->Side = 2;
               else
                  l_pTreaty->Side = 1;
               
               l_pTreaty->m_iSource = g_SP2Client->Id();
               l_pTreaty->m_iTarget = SDK::Event::ESpecialTargets::Server;
               l_pTreaty->NewMemberID = g_ClientDAL.ControlledCountryID();
               l_pTreaty->TreatyID = m_pCurrentTreaty->ID();

               g_Joshua.RaiseEvent(l_pEvent);

               m_pObjRefuseBtn->Enabled(false);
               m_pObjModifyBtn->Enabled(false);

            }
            //LEAVE CASE
            else if(m_pObjModifyBtn->Text() == g_ClientDAL.GetString(EStrId::Leave))
            {
               //Send the leave event to the server
               SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyLeave);
               SP2::Event::GTreatyLeave* l_pTreaty = (SP2::Event::GTreatyLeave*)l_pEvent.get();

               l_pTreaty->m_iSource = g_SP2Client->Id();
               l_pTreaty->m_iTarget = SDK::Event::ESpecialTargets::Server;
               l_pTreaty->OldMemberID = g_ClientDAL.ControlledCountryID();
               l_pTreaty->TreatyID = m_pCurrentTreaty->ID();

               g_Joshua.RaiseEvent(l_pEvent);

               m_pObjRefuseBtn->Enabled(false);
               m_pObjModifyBtn->Enabled(false);

            }
         }
      }

      //If the user refused to join a treaty send the event to the server
      if(in_pCaller == m_pObjRefuseBtn)
      {
         SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyLeave);
         SP2::Event::GTreatyLeave* l_pTreaty = (SP2::Event::GTreatyLeave*)l_pEvent.get();

         l_pTreaty->m_iSource = g_SP2Client->Id();
         l_pTreaty->m_iTarget = SDK::Event::ESpecialTargets::Server;
         l_pTreaty->OldMemberID = g_ClientDAL.ControlledCountryID();
         l_pTreaty->TreatyID = m_pCurrentTreaty->ID();

         g_Joshua.RaiseEvent(l_pEvent);

         m_pObjRefuseBtn->Enabled(false);
         m_pObjModifyBtn->Enabled(false);
      }
      
      //on Confirm button clicked
      if(in_pCaller == m_pObjConfirmBtn)
      {
         //The first thing to do is disable the confirm button so the user will not be able to create the same treaty twice (DUMB ASS PROOF :P hehe)
         m_pObjConfirmBtn->Enabled(false);
         
         //Ensure the treaty name is not already used
         GString l_sTreatyName = m_pObjNameEdt->Text();
         {
            if(!g_ClientDDL.GenericMessageWindow())
               g_ClientDDL.GenericMessageWindowSpawn();

            hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().begin();
            while(l_It != g_ClientDAL.Treaties().end())
            {
               if(l_It->second.Name() == l_sTreatyName)
               {
                  m_bTreatySameName = true;
                  g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::TreatyWithSameName), this );
                  m_pObjConfirmBtn->Enabled(true);
                  return EEventStatus::Handled;
               }
               else
               {
                  l_It++;
               }
            }
         }

         //Create treaty
         GTreaty l_Treaty;

         //Set treaty name
         m_sTreatyName = l_sTreatyName;
         l_Treaty.Name(l_sTreatyName);
         
         //Set treaty type
         ETreatyType::Enum l_eTreatyType = FindTreatyType(m_pObjTypeCbo->Selected_Content());
         l_Treaty.Type(l_eTreatyType);
         
         //Fill in conditions
         l_Treaty.AddCondition(ETreatyConditions::EconomicStrengthSimilitude, ConditionGetEconomicStrengthSimilitude(m_pObjCondEconomicStrength->Selected_Content()) );
         l_Treaty.AddCondition(ETreatyConditions::GeographicProximity,        ConditionGetGeographicProximity(m_pObjCondGeographicProximity->Selected_Content())     );
         l_Treaty.AddCondition(ETreatyConditions::MilitaryStrengthSimilitude, ConditionGetMilitaryStrengthSimilitude(m_pObjCondMilitaryStrength->Selected_Content()) );
         l_Treaty.AddCondition(ETreatyConditions::PoliticalSimilitude,        ConditionGetPoliticalSimilitude(m_pObjCondPoliticalStrength->Selected_Content())       );
         l_Treaty.AddCondition(ETreatyConditions::RelationsNotAtWar,          ConditionGetRelationsCloseness(m_pObjCondRelations->Selected_Content())                );
         l_Treaty.AddCondition(ETreatyConditions::ResearchLevelSimilitude,    ConditionGetResearchLevelSimilitude(m_pObjCondResearchLevel->Selected_Content())       );

         //Add each member on side A
         for(INT32 i=0; i<(INT32)m_viSideA.size(); i++)
         {
            //If the treaty has only one country per side be sure we do not add any more countries
            if(i >= 1 && m_bSingleOnSide)
               break;

            //If we are adding the controlled country already set the member as active
            if(m_viSideA[i] == g_ClientDAL.ControlledCountryID())
               l_Treaty.AddMemberSideA(m_viSideA[i], true, true);
            else
               l_Treaty.AddMemberSideA(m_viSideA[i], false, true);
         }

         //Add each member on side B
         for(INT32 i=0; i<(INT32)m_viSideB.size(); i++)
         {
            //If the treaty has only one country per side be sure we do not add any more countries
            if(i >= 1 && m_bSingleOnSide)
               break;

            //If we are adding the controlled country already set the member as active
            if(m_viSideB[i] == g_ClientDAL.ControlledCountryID())
               l_Treaty.AddMemberSideB(m_viSideB[i], true, true);
            else
               l_Treaty.AddMemberSideB(m_viSideB[i], false, true);
         }
         
         //Set the treaty status
         l_Treaty.Private(!m_pObjTreatyOpenTog->ToggleState());

         //It we do not find the creator of the treaty on one of the two sides add it on pressure
         vector<INT32>::iterator l_It  = find(m_viSideA.begin(), m_viSideA.end(), g_ClientDAL.ControlledCountryID());
         if(l_It == m_viSideA.end())
         {
            vector<INT32>::iterator l_It2 = find(m_viSideB.begin(), m_viSideB.end(), g_ClientDAL.ControlledCountryID());
            if(l_It2 == m_viSideB.end())
               l_Treaty.AddMemberPressure(g_ClientDAL.ControlledCountryID());
         }

         //Raise the treaty add event
         SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyDetailsTreatyAdd);
         SP2::Event::GTreatyDetailsTreatyAdd* l_pTreatyAddEvt = (SP2::Event::GTreatyDetailsTreatyAdd*)l_pEvent.get();
         l_pTreatyAddEvt->m_iSource = g_Joshua.Client()->Id();
         l_pTreatyAddEvt->m_Treaty  = l_Treaty;
         l_pTreatyAddEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;

         g_Joshua.RaiseEvent(l_pEvent);

         m_pObjConfirmBtn->Enabled(false);
      }

      //If the user want to sponsor/unsponsor a treaty
      if(in_pCaller == m_pObjSponsorBtn)
      {
         //If he want's to sponsor
         if(m_pObjSponsorBtn->Text() == g_ClientDAL.GetString(EStrId::SponsorTreaty))
         {
            SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyJoin);
            SP2::Event::GTreatyJoin* l_pTreaty = (SP2::Event::GTreatyJoin*)l_pEvent.get();
           
            l_pTreaty->m_iSource = g_SP2Client->Id();
            l_pTreaty->m_iTarget = SDK::Event::ESpecialTargets::Server;
            l_pTreaty->NewMemberID = g_ClientDAL.ControlledCountryID();
            l_pTreaty->TreatyID = m_pCurrentTreaty->ID();
            l_pTreaty->Side = 3;

            g_Joshua.RaiseEvent(l_pEvent);

            m_pObjSponsorBtn->Enabled(false);
         }
         else
         {
            SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTreatyLeave);
            SP2::Event::GTreatyLeave* l_pTreaty = (SP2::Event::GTreatyLeave*)l_pEvent.get();

            l_pTreaty->m_iSource = g_SP2Client->Id();
            l_pTreaty->m_iTarget = SDK::Event::ESpecialTargets::Server;
            l_pTreaty->OldMemberID = g_ClientDAL.ControlledCountryID();
            l_pTreaty->TreatyID = m_pCurrentTreaty->ID();

            g_Joshua.RaiseEvent(l_pEvent);
            
            m_pObjSponsorBtn->Enabled(false);
         }
      }

      //On cancel button simply hide the window
      if(in_pCaller == m_pObjCancelBtn)
      {
         Hide();
      }

      if(in_pCaller == m_pObjRemoveNotEligibleBtn)
      {
         for(UINT32 i=0; i<m_viSideA.size(); i++)
         {
            set<UINT32>::iterator l_It = m_siEligibleCountries.find( m_viSideA[i] );
            if( l_It == m_siEligibleCountries.end() )
            {
               m_viSideA.erase( find(m_viSideA.begin(), m_viSideA.end(), m_viSideA[i]) );
               i--;
               m_bSideADirty = true;
            }
         }

         for(UINT32 i=0; i<m_viSideB.size(); i++)
         {
            set<UINT32>::iterator l_It = m_siEligibleCountries.find( m_viSideB[i] );
            if( l_It == m_siEligibleCountries.end() )
            {
               m_viSideB.erase( find(m_viSideB.begin(), m_viSideB.end(), m_viSideB[i]) );
               i--;
               m_bSideBDirty = true;
            }
         }
         
         UpdateListBoxes();

         if(m_bTwoSidesTreaty)
         {
            if(m_viSideA.size() && m_viSideB.size() && m_sName.size() > 0)
               m_pObjConfirmBtn->Enabled(true);
         }
         else
         {
            if(m_viSideA.size() >= 2 && m_sName.size() > 0)
               m_pObjConfirmBtn->Enabled(true);
         }
      }
   }

	return GUI::EEventStatus::Handled;
}

//Find the country ID with the country code
INT32 GTreatyDetailsWindow::FindCountryIdFromCode(GString in_sCode)
{
   if(in_sCode == L"")
      return -1;

   return g_ClientDAL.CountryID(in_sCode);
}

GUI::EEventStatus::Enum GTreatyDetailsWindow::InitializeTreaty(ETreatyType::Enum in_eTreatyType)
{

	 //Check if the treaty is one country per side
   if(in_eTreatyType == ETreatyType::Annexation || 
      in_eTreatyType == ETreatyType::FreeRegion ||
      in_eTreatyType == ETreatyType::MilitaryAccess )
   {
      m_bSingleOnSide = true;
   }
   else
   {
      m_bSingleOnSide = false;
   }

   //If the treaty type wasn't found do nothing (should never happen but well ...)
   if(in_eTreatyType == ETreatyType::Undefined)
      return EEventStatus::Handled;

   //Check if the treaty is two sided
   m_bTwoSidesTreaty = IsTreatyTwoSided(in_eTreatyType);

   //Enable modify button so we can add country
   m_pObjModifyBtn->Enabled(true);

   //Enable and show stuff depending it's a two sided treaty or not
   if(m_bTwoSidesTreaty)
   {
      if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusItem")
      {
         m_pObjSideALst->Width(141);
         m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusItem");
         m_pObjSideALbl->Show();
         m_pObjSideBLbl->Show();
         m_pObjSideALst->Clear();
      }
   }
   else
   {
      if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusDoubleItem")
      {
         m_pObjSideALst->Width(283);
         m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusDoubleItem");
         m_pObjSideALbl->Hide();
         m_pObjSideBLbl->Hide();
         m_pObjSideALst->Clear();
      }
   }

   if(GTreaty::Ponctual(in_eTreatyType))
   {
      EnableConditions(false);
      m_pObjEligibleLst->Clear();
      m_siEligibleCountries.clear();
   }
   else
   {               
      if(in_eTreatyType != ETreatyType::Alliance)
      {
         m_pObjCondRelations->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
      }

      EnableConditions(true);

      if(in_eTreatyType == ETreatyType::Alliance)
      {
         m_pObjCondRelations->Selected_Content(g_ClientDAL.GetString(EStrId::NotAtWar));
         m_pObjCondRelations->Enabled(false);
      }

      FetchNewAvailableCountryList();
   }

   //Fill treaty description
   FillDescription(in_eTreatyType);
   
   //Generate a dummy treaty name
   const GCountry& l_Country = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID() );
   m_pObjNameEdt->Text(g_ClientDAL.TreatyName(in_eTreatyType, l_Country.Name()));

   //Force the side update
   m_bSideADirty = true;
   m_bSideBDirty = true;
   
   //Update the sides lstbox
   UpdateListBoxes();

	return EEventStatus::NotHandled;
}

GUI::EEventStatus::Enum GTreatyDetailsWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
   case c_iOnEditBoxEditBegin:
      {
         if(in_pCaller == m_pObjNameEdt)
         {
            if( !m_bIsEditingName ) 
            {
               m_pObjNameOverEdt->Text( m_pObjNameEdt->Text() );
               m_pObjNameEdt->Text(L"");
               m_pObjNameOverEdt->Show();
               g_Joshua.GUIManager()->Focus( m_pObjNameOverEdt );
               m_bIsEditingName = true;
            }
         }
      }
      break;
   case c_iOnEditBoxEditEnd:
      {
         if(in_pCaller == m_pObjNameOverEdt)
         {
            if( m_bIsEditingName )
            {
               m_bIsEditingName = false;
               m_pObjNameEdt->Text( m_pObjNameOverEdt->Text() );
               m_pObjNameOverEdt->Hide();
            }
         }
      }
      break;
   case ECustomEvents::OnScrollVertical:
      {
         //If we are scrolling the treaty description text
         if(in_pCaller == m_pObjDescriptionScr)
         {
            m_pObjDescriptionLbl->Top(-(INT16)in_EventData.Data);
         }
      }
      break;

   case c_OnGenericMessageWindowAnswerEvent:
      {
         if(m_bWaitForClose)
         {
            Hide();
            return EEventStatus::Handled;
         }

         if(m_bCountryNotEligible)
            break;
         
         if(!m_bTreatySameName)
            Hide();
         else
            m_bTreatySameName = false;
      }
      break;

   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;

         //On treaty type selection
         if(l_pCombo == m_pObjTypeCbo)
			{
            ETreatyType::Enum l_eType = FindTreatyType(m_pObjTypeCbo->Selected_Content());

				if (InitializeTreaty(l_eType) == GUI::EEventStatus::Handled)
				{
					return GUI::EEventStatus::Handled;
				}
         }

         //If the user changed a condition fetch the new available countries list
         if(l_pCombo == m_pObjCondRelations || 
            l_pCombo == m_pObjCondGeographicProximity || 
            l_pCombo == m_pObjCondPoliticalStrength || 
            l_pCombo == m_pObjCondEconomicStrength || 
            l_pCombo == m_pObjCondMilitaryStrength || 
            l_pCombo == m_pObjCondResearchLevel)
         {
            if(m_pObjCondGeographicProximity->Enabled())
            {
               ETreatyType::Enum l_eType = FindTreatyType(m_pObjTypeCbo->Selected_Content());

               if( l_eType != ETreatyType::Undefined )
               {
                  if( !GTreaty::Ponctual( l_eType ) )
                     FetchNewAvailableCountryList();
               }
            }
         }
      }
      break;

   //If the user changed his country selection
   case c_OnAddRemoveCountrySelection:
      {
         OnAddRemoveCountrySelection();			
      }
      break;

   }

   return GUI::EEventStatus::Handled;
}

void GTreatyDetailsWindow::OnAddRemoveCountrySelection()
{
	//Force the side update
   m_bSideADirty = true;
   m_bSideBDirty = true;

   //Update the sides
   UpdateListBoxes();

   //Enable stuff depending it's a two sided treaty
   if(m_bTwoSidesTreaty)
   {
      if(m_viSideA.size() && m_viSideB.size() && !m_bCountryNotEligible && m_sName.size() > 0)
         m_pObjConfirmBtn->Enabled(true);
      else
         m_pObjConfirmBtn->Enabled(false);
   }
   else
   {
      if(m_viSideA.size() && !m_bCountryNotEligible && m_sName.size() > 0)
         m_pObjConfirmBtn->Enabled(true);
      else
         m_pObjConfirmBtn->Enabled(false);
   }
}

//Send an event to the server so we can get a list of country that respect the treaty conditions
void GTreatyDetailsWindow::FetchNewAvailableCountryList()
{
   UINT32 l_Conditions[ETreatyConditions::ItemCount];

   if( m_pObjCondRelations->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit) )
      l_Conditions[ETreatyConditions::RelationsNotAtWar] = 0;
   else if( m_pObjCondRelations->Selected_Content() == g_ClientDAL.GetString(EStrId::NotAtWar))
      l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;

   ETreatyType::Enum l_eSelectedTreatyType = FindTreatyType( m_pObjTypeCbo->Selected_Content() );

   vector<INT32> l_iAlreadyMembers;
   if(m_bTwoSidesTreaty)
   {
      if(l_eSelectedTreatyType == ETreatyType::War ||
         l_eSelectedTreatyType == ETreatyType::EconomicEmbargo ||
         l_eSelectedTreatyType == ETreatyType::WeaponTradeEmbargo ||
         l_eSelectedTreatyType == ETreatyType::Annexation)
      {
         if(!m_viSideA.size())
         {
            l_Conditions[ETreatyConditions::RelationsNotAtWar] = 0;
         }
         else
         {
            l_iAlreadyMembers = m_viSideA;
         }
      }
      else
      {
         if(!m_viSideA.size() && !m_viSideB.size())
         {
            l_Conditions[ETreatyConditions::RelationsNotAtWar] = 0;
         }
         else
         {
            l_iAlreadyMembers = m_viSideA;

            for(UINT32 i=0; i<m_viSideB.size(); i++)
            {
               l_iAlreadyMembers.push_back( m_viSideB[i] );
            }
         }
      }
   }
   else
   {
      if(m_viSideA.size())
      {
         l_Conditions[ETreatyConditions::RelationsNotAtWar] = 0;
      }
      else
      {
         l_iAlreadyMembers = m_viSideA;
      }
   }

   if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit))
      l_Conditions[ETreatyConditions::GeographicProximity] = 0;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::Africa))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Africa;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::Europe))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Europe;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::America))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::America;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::Oceania))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Oceania;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::Asia))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Asia;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::CentralAmerica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::CentralAmerica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::EasternAfrica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAfrica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::EasternAsia))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAsia;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::EasternEurope))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternEurope;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::FarEast))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::FarEast;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::MiddleEast))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::MiddleEast;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::NorthernAfrica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAfrica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::NorthernAmerica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAmerica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::PacificIslands))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::PacificIslands;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::SouthernAfrica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAfrica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::SouthernAmerica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAmerica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::SouthernOceania))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernOceania;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::WesternAfrica))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternAfrica;
   else if( m_pObjCondGeographicProximity->Selected_Content() == g_ClientDAL.GetString(EStrId::WesternEurope))
      l_Conditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternEurope;

   if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = 0;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::AllDemocracies))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::AllDemocracies;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::CommunistState))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::Communism;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::MilitaryDictatorship))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::MilitaryDictatorship;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Monarchy))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::Monarchy;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::MultiPartyDemocracy))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::MultiParty;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::SinglePartyDemocracy))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::SingleParty;
   else if( m_pObjCondPoliticalStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Theocracy))
      l_Conditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusPolitical::Theocracy;

   if( m_pObjCondEconomicStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit))
      l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] = 0;
   else if( m_pObjCondEconomicStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top10))
      l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::Top10;
   else if( m_pObjCondEconomicStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top25))
      l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::Top25;
   else if( m_pObjCondEconomicStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top50))
      l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::Top50;
   else if( m_pObjCondEconomicStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top100))
      l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::Top100;

   if( m_pObjCondMilitaryStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit))
      l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] = 0;
   else if( m_pObjCondMilitaryStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top10))
      l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::Top10;
   else if( m_pObjCondMilitaryStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top25))
      l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::Top25;
   else if( m_pObjCondMilitaryStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top50) )
      l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::Top50;
   else if( m_pObjCondMilitaryStrength->Selected_Content() == g_ClientDAL.GetString(EStrId::Top100))
      l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::Top100;

   if( m_pObjCondResearchLevel->Selected_Content() == g_ClientDAL.GetString(EStrId::NoLimit))
      l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = 0;
	else if( m_pObjCondResearchLevel->Selected_Content() == g_ClientDAL.GetString(EStrId::ConditionLow))
      l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::Low;
	else if( m_pObjCondResearchLevel->Selected_Content() == g_ClientDAL.GetString(EStrId::ConditionAverage))
      l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::Average;
	else if( m_pObjCondResearchLevel->Selected_Content() == g_ClientDAL.GetString(EStrId::ConditionHigh))
      l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::High;

   //Once the array is done send the request to the server
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GRequestTreatyConditionsCountries);
   SP2::Event::GRequestTreatyConditionsCountries* l_pUpdate = (SP2::Event::GRequestTreatyConditionsCountries*)l_pEvent.get();

   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   
   for(INT32 i=0; i<ETreatyConditions::ItemCount; i++)
      l_pUpdate->m_iConditions[i] = l_Conditions[i];

   for(UINT32 i=0; i<l_iAlreadyMembers.size(); i++)
   {
      l_pUpdate->m_CountryList.insert((UINT32)l_iAlreadyMembers[i]);
   }

   m_bWaitForEligibleAnswer = true;

   g_Joshua.RaiseEvent(l_pEvent);
}

void GTreatyDetailsWindow::QuickTreatyWindow(ETreatyType::Enum in_eTreatyType,
															const GString& in_sTypeName,
															UINT32 in_iSideA,
															UINT32 in_iSideB)
{	
	//Reset all window objects
   m_pObjNameEdt->Enabled(true);
   m_pObjTypeCbo->Enabled(true);
   m_pObjTreatyOpenTog->Enabled(true);   
   m_pObjConfirmBtn->Enabled(false);
   m_pObjTreatyOpenTog->ToggleState(false);

	m_pCurrentTreaty = NULL;
	m_sTreatyName    = g_ClientDAL.FindTreatyName(in_eTreatyType);

	m_pObjNameEdt->Text(m_sTreatyName);
   m_pObjDescriptionLbl->Text(L"");
   m_pObjTypeCbo->Selected_Content(in_sTypeName);
   m_pObjRefuseBtn->Hide();

   EnableConditions(false);

   m_pConditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::NoLimit;
   m_pConditions[ETreatyConditions::GeographicProximity]        = EConditionStatusGeo::NoLimit;
   m_pConditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::NoLimit;
   m_pConditions[ETreatyConditions::PoliticalSimilitude]        = EConditionStatusRank::NoLimit;
   m_pConditions[ETreatyConditions::RelationsNotAtWar]          = EConditionStatusRelation::NoLimit;
   m_pConditions[ETreatyConditions::ResearchLevelSimilitude]    = EConditionStatusResearch::NoLimit;

	m_pObjEligibleLst->Clear();

   m_pObjStatusLbl->Text( g_ClientDAL.GetString(EStrId::Pending) );
   m_pObjStatusPic->PictureNormal(GString(L"000865"));

   m_pObjSideALst->Clear();
   m_pObjSideBLst->Clear();
   m_pObjPressureLst->Clear();

   //Enable and show stuff depending it's a two sided treaty or not
   m_bTwoSidesTreaty = false;
   if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusDoubleItem")
   {
      m_pObjSideALst->Width(283);
      m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusDoubleItem");
      m_pObjSideALbl->Hide();
      m_pObjSideBLbl->Hide();
      m_pObjSideALst->Clear();
   }

   m_pObjModifyBtn->Enabled(false);
   m_pObjSponsorBtn->Enabled(false);

   m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Modify));
   m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102236) );

   m_viSideA.clear();
   m_viSideB.clear();
   m_siEligibleCountries.clear();

	if(in_eTreatyType == ETreatyType::Annexation ||
		in_eTreatyType == ETreatyType::FreeRegion ||
		in_eTreatyType == ETreatyType::EconomicEmbargo ||
		in_eTreatyType == ETreatyType::RequestMilitaryPresenceRemoval ||
		in_eTreatyType == ETreatyType::WeaponTradeEmbargo ||
      in_eTreatyType == ETreatyType::War)
	{
		m_viSideA.push_back( in_iSideA );
		m_viSideB.push_back( in_iSideB );		
	}
	else if(in_eTreatyType == ETreatyType::AssumeForeignDebt ||
			  in_eTreatyType == ETreatyType::EconomicAid ||
			  in_eTreatyType == ETreatyType::MilitaryAccess)
			
	{
		m_viSideA.push_back( in_iSideB );
		m_viSideB.push_back( in_iSideA );
	}
	else
	{
		m_viSideA.push_back( in_iSideA );
		m_viSideA.push_back( in_iSideB );
	}     

   //Force the side update
   m_bSideADirty = true;
	m_bSideBDirty = true;
   m_bSkipEligibleWarning = false;

	InitializeTreaty(in_eTreatyType);

	OnAddRemoveCountrySelection();
}

void GTreatyDetailsWindow::Update(INT32 in_iTreatyID)
{
   if(m_bWaitForClose)
      return;

   //If it's a new treaty
   if(in_iTreatyID == -1)
   {
      //Reset all window objects
      m_pObjNameEdt->Enabled(true);
      m_pObjTypeCbo->Enabled(true);
      m_pObjTreatyOpenTog->Enabled(true);   
      m_pObjConfirmBtn->Enabled(false);
      m_pObjTreatyOpenTog->ToggleState(false);

      m_iTreatyID = -1;

      m_pCurrentTreaty = NULL;
      m_sTreatyName    = L"";
      
      m_pObjNameEdt->Text(L"");
      m_pObjDescriptionLbl->Text(L"");
      m_pObjTypeCbo->Selected_Content(g_ClientDAL.GetString(EStrId::SelectTreatyType));
      m_pObjRefuseBtn->Hide();

      EnableConditions(false);

      m_pConditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::NoLimit;
      m_pConditions[ETreatyConditions::GeographicProximity]        = EConditionStatusGeo::NoLimit;
      m_pConditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::NoLimit;
      m_pConditions[ETreatyConditions::PoliticalSimilitude]        = EConditionStatusRank::NoLimit;
      m_pConditions[ETreatyConditions::RelationsNotAtWar]          = EConditionStatusRelation::NoLimit;
      m_pConditions[ETreatyConditions::ResearchLevelSimilitude]    = EConditionStatusResearch::NoLimit;

      m_pObjEligibleLst->Clear();

      m_pObjStatusLbl->Text( g_ClientDAL.GetString(EStrId::Pending) );
      m_pObjStatusPic->PictureNormal(GString(L"000865"));

      m_pObjSideALst->Clear();
      m_pObjSideBLst->Clear();
      m_pObjPressureLst->Clear();
      m_pObjPressureBar->Value(0.f,0,false,false);

      //Enable and show stuff depending it's a two sided treaty or not
      m_bTwoSidesTreaty = false;
      if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusDoubleItem")
      {
         m_pObjSideALst->Width(283);
         m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusDoubleItem");
         m_pObjSideALbl->Hide();
         m_pObjSideBLbl->Hide();
         m_pObjSideALst->Clear();
      }

      m_pObjModifyBtn->Enabled(false);
      m_pObjSponsorBtn->Enabled(false);

      m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Modify));
      m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102236) );

      m_viSideA.clear();
      m_viSideB.clear();
      m_siEligibleCountries.clear();

      m_viSideA.push_back( g_ClientDAL.ControlledCountryID() );
      m_siEligibleCountries.insert( g_ClientDAL.ControlledCountryID() );
      //Force the side update
      m_bSideADirty = true;
      m_bSkipEligibleWarning = false;

      //Update the sides
      UpdateListBoxes();
   }
   else
   {
      //Prepare the window to receive data
      //m_pObjSideALst->Clear();
      //m_pObjSideBLst->Clear();

      //Disable stuff that we can't change once the treaty created
      m_pObjNameEdt->Enabled(false);
      m_pObjTypeCbo->Enabled(false);
      m_pObjTreatyOpenTog->Enabled(false);
      m_pObjModifyBtn->Enabled(false);
      m_pObjConfirmBtn->Enabled(false);
      m_pObjRefuseBtn->Hide();

      EnableConditions(false);

      hash_map<UINT32, GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().find(in_iTreatyID);
      
      //If the treaty is not existant anymore display a message to user
      if(l_It == g_ClientDAL.Treaties().end())
      {
         if(!g_ClientDDL.GenericMessageWindow())
            g_ClientDDL.GenericMessageWindowSpawn();

         if(!g_ClientDDL.GenericMessageWindow()->Visible())
            g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::TreatyHasBeenRemoved), this );

         m_pCurrentTreaty = NULL;
         m_iTreatyID = -1;
         return;
      }
      else
      {
         //Get the current displayed treaty
         m_pCurrentTreaty = (SP2::GTreaty*)&l_It->second;
         m_iTreatyID = m_pCurrentTreaty->ID();
      }
      //else
      //{
         //If the treaty is ponctual
         //if(l_It->second.Ponctual())
         //{
            //If the treaty is accepted
         //   if(l_It->second.MembersOriginalsA().size() == l_It->second.MembersSideA(true).size() && 
         //      l_It->second.MembersOriginalsB().size() == l_It->second.MembersSideB(true).size())
         //   {
               //Send a notification to Treaty details window if it's currently showing that treaty
         //      if( DisplayedTreatyID() == (INT32)l_It->second.ID() )
         //      {
         //         g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::TreatyIsAccepted), this );
         //      }
         //   }
         //}
      //}
      
      //Set treaty initial data
      bool l_bPrivate = m_pCurrentTreaty->Private();
      m_pObjTreatyOpenTog->ToggleState(!l_bPrivate);
      m_pObjNameEdt->Text(m_pCurrentTreaty->Name());
      m_pObjTypeCbo->Selected_Content(g_ClientDAL.FindTreatyName(m_pCurrentTreaty->Type()));
      
      if(m_pCurrentTreaty->ShouldBeActive())
      {
         m_pObjStatusLbl->Text( g_ClientDAL.GetString(EStrId::Active) );
         m_pObjStatusPic->PictureNormal(GString(L"000866"));
         m_bTreatyIsActive = true;
      }
      else
      {
         m_pObjStatusLbl->Text( g_ClientDAL.GetString(EStrId::Pending) );
         m_pObjStatusPic->PictureNormal(GString(L"000865"));
         m_bTreatyIsActive = false;
      }

      if( !m_pCurrentTreaty->Ponctual() )
      {
         //Fill the conditions
         FillConditions(m_pCurrentTreaty->Conditions());
         //Apply the country conditions to the countries currently in the treaty
         ApplyCountryConditions( m_pCurrentTreaty->EligibleCountries() );
      }
      else
      {
         FillSides(m_pCurrentTreaty);
      }

      //Fill description
      FillDescription(m_pCurrentTreaty->Type());

      //Fill pressure with countries
      FillPressure(m_pCurrentTreaty);

      //Check the player country status in that treaty
      bool l_bPlayerIsIn      = false;
      bool l_bPlayerActive    = false;
      bool l_bPlayerOriginal  = false;
      bool l_bPlayerPressure  = false;
      bool l_bPlayerInnactive = false;
      bool l_bPlayerNotInConditions = false;

      INT32 l_iCountryID = g_ClientDAL.ControlledCountryID();

      l_bPlayerActive    = m_pCurrentTreaty->MembersSideA(true).find(l_iCountryID) != m_pCurrentTreaty->MembersSideA(true).end();
      l_bPlayerActive    = l_bPlayerActive || (m_pCurrentTreaty->MembersSideB(true).find(l_iCountryID)  != m_pCurrentTreaty->MembersSideB(true).end());
      l_bPlayerInnactive = l_bPlayerInnactive || (m_pCurrentTreaty->MembersSideA(false).find(l_iCountryID) != m_pCurrentTreaty->MembersSideA(false).end());
      l_bPlayerInnactive = l_bPlayerInnactive || (m_pCurrentTreaty->MembersSideB(false).find(l_iCountryID) != m_pCurrentTreaty->MembersSideB(false).end());
      l_bPlayerPressure  = l_bPlayerPressure || (m_pCurrentTreaty->MembersPressure().find(l_iCountryID)   != m_pCurrentTreaty->MembersPressure().end());
      l_bPlayerOriginal  = l_bPlayerOriginal || (m_pCurrentTreaty->MembersOriginalsA().find(l_iCountryID) != m_pCurrentTreaty->MembersOriginalsA().end());
      l_bPlayerOriginal  = l_bPlayerOriginal || (m_pCurrentTreaty->MembersOriginalsB().find(l_iCountryID) != m_pCurrentTreaty->MembersOriginalsB().end());
      
      //if the player do not meet the condition
      if(m_pCurrentTreaty->EligibleCountries().find((INT16)l_iCountryID) == m_pCurrentTreaty->EligibleCountries().end())
         l_bPlayerNotInConditions = true;

      //If the player is in the treaty
      if(l_bPlayerOriginal || l_bPlayerActive || l_bPlayerInnactive)
         l_bPlayerIsIn = true;

      //Is the treaty active
      bool l_bActive  = m_pCurrentTreaty->Active();

      if(l_bActive)
      {
         //if it's active we can't sponsor anymore
         m_pObjPressureLst->Clear();
         m_pObjSponsorBtn->Enabled(false);
         m_pObjPressureBar->Value(0,0,false,false);
         m_pObjPressureTxt->Text(g_ClientDAL.GetString(EStrId::NA));
      }
      else
      {
         //if it's not active enable sponsorship
         m_pObjSponsorBtn->Enabled(true);
      }

      //If the player is in the treaty
      if(l_bPlayerIsIn)
      {
         //Enable the modify button
         m_pObjModifyBtn->Enabled(true);

         //Disable sponsorship, a player in the treaty can't sponsor the same treaty
         m_pObjSponsorBtn->Enabled(false);
   
         //If the treaty is not active
         if(!l_bActive)
         {
            //Set the modify button to JOIN
            m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
            m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );
            m_pObjRefuseBtn->Show();
            
            //If the player is active in the treaty we want him to be able to leave the treaty
            if(l_bPlayerActive)
            {
               m_pObjModifyBtn->Enabled(true);
               m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Leave));
               m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102240) );
               m_pObjRefuseBtn->Hide();
            }
            else
            {
               //if the player is innactive but is an original we want him to be able to either join/refuse the treaty
               if(l_bPlayerInnactive && l_bPlayerOriginal)
               {
                  m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
                  m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );
                  m_pObjRefuseBtn->Show();
                  m_pObjModifyBtn->Enabled(true);
                  m_pObjRefuseBtn->Enabled(true);
               }
               else
               {
                  //otherwise the user has to be able to join the treaty
                  m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
                  m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );
                  m_pObjModifyBtn->Enabled(true);
                  m_pObjRefuseBtn->Hide();               
               }
            }

            //The player must not be able to join if he do not meet the treaty requirements
            if(l_bPlayerNotInConditions && !m_pCurrentTreaty->Ponctual())
               m_pObjModifyBtn->Enabled(false);
         }
         else
         {
            //If the treaty is active arrange button so the user can either join or leave the treaty depending if he is active or not
            m_pObjModifyBtn->Enabled(true);
            if(l_bPlayerActive)
            {
               m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Leave));
               m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102240) );
               m_pObjRefuseBtn->Hide();
               m_pObjRefuseBtn->Enabled(false);
            }
            else
            {
               m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
               m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );
               
               if(l_bPlayerInnactive)
               {
                  m_pObjRefuseBtn->Show();
                  m_pObjRefuseBtn->Enabled(true);
               }
               else
               {
                  m_pObjRefuseBtn->Hide();
                  m_pObjRefuseBtn->Enabled(false);
               }
            }

            //The player must not be able to join if he do not meet the treaty requirements (Ensure that the text is not Leave so the player can leave the treaty
            if(l_bPlayerNotInConditions && !m_pCurrentTreaty->Ponctual() && (m_pObjModifyBtn->Text() != g_ClientDAL.GetString(EStrId::Leave)) )
               m_pObjModifyBtn->Enabled(false);

         }
      }
      else
      {
         //If the player is not in the treaty
         m_pObjRefuseBtn->Hide();
         
         //if the treaty is private
         if(l_bPrivate)
         {
            //allow the user to join only if he is an original member
            m_pObjModifyBtn->Enabled(false);
            m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
            m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );

            if(l_bPlayerOriginal)
            {
               m_pObjModifyBtn->Enabled(true);
            }
         }
         else
         {
            //if it's public the user can join whenever he wants
            m_pObjModifyBtn->Enabled(true);
            m_pObjModifyBtn->Text(g_ClientDAL.GetString(EStrId::Join));
            m_pObjModifyBtn->TooltipText( g_ClientDAL.GetString(102239) );
         }

         //The player must not be able to join if he do not meet the treaty requirements
         if(l_bPlayerNotInConditions && !m_pCurrentTreaty->Ponctual())
            m_pObjModifyBtn->Enabled(false);

         //Check if the player can sponsor or stop to sponsor the treaty
         if(m_pObjSponsorBtn->Enabled())
         {
            if(l_bPlayerPressure)
               m_pObjSponsorBtn->Text( g_ClientDAL.GetString( EStrId::RemoveSponsorship ) );
            else
               m_pObjSponsorBtn->Text( g_ClientDAL.GetString( EStrId::SponsorTreaty ) );
         }
         else
         {
            m_pObjSponsorBtn->Text( g_ClientDAL.GetString( EStrId::SponsorTreaty ) );
         }
      }
   }

   //If the button is leave, ensure the country can really leave the treaty (side B)
   if(m_pCurrentTreaty && m_pObjModifyBtn->Text() == g_ClientDAL.GetString(EStrId::Leave) && m_pCurrentTreaty->Sides() == 2)
   {
      if( m_pCurrentTreaty->MembersSideB(true).find( g_ClientDAL.ControlledCountryID() ) != m_pCurrentTreaty->MembersSideB(true).end() )
      {
         if(!m_pCurrentTreaty->SideBCanLeave())
         {
            m_pObjModifyBtn->Enabled(false);
         }
         else
         {
            m_pObjModifyBtn->Enabled(true);
         }
      }
   }
}

void GTreatyDetailsWindow::FillPressure(GTreaty* in_pTreaty)
{
   //Clear pressure listbox
   m_pObjPressureLst->Clear();

   const set<UINT32> & l_Ctries = in_pTreaty->MembersPressure();

   //Check the total units value through the world
   REAL64 l_fTotalUnitsValue    = 0.f;
   REAL64 l_fTotalPressureValue = 0.f;
	UINT32 l_iNbCountries = (UINT32)g_ClientDAL.NbCountry();
   for(UINT32 i=1; i<=l_iNbCountries; i++)
   {
      l_fTotalUnitsValue += g_ClientDAL.TotalUnitsValue(i, EUnitCategory::ItemCount);
   }

	l_fTotalUnitsValue = max(1.f,l_fTotalUnitsValue);
   //For each pressure country we have
   for(set<UINT32>::const_iterator l_It = l_Ctries.begin(); l_It != l_Ctries.end(); l_It++)
   {
      //Add it and set it color correctly
      GUI::GListBox::GRow* l_pRow = m_pObjPressureLst->Insert_New_Row();
      ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));

      const GCountry& l_Ctry = g_ClientDAL.Country(*l_It);
      
      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]) ;
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->Text( GString( l_Ctry.Code() ) );

      l_pRow->Object()->Child(L"picFlag")->TooltipText( l_Ctry.Name() );
      l_pRow->Object()->Child(L"txtCode")->TooltipText( l_Ctry.Name() );
      
      //Set it's value to the proportion it have compared to all units in the world
      REAL64 l_fCtryValue = g_ClientDAL.TotalUnitsValue(*l_It, EUnitCategory::ItemCount);
      l_fTotalPressureValue += l_fCtryValue;
      REAL64 l_fValue = l_fCtryValue / l_fTotalUnitsValue;
      
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtValue"))->Text( GString::FormatNumber(l_fValue * 100, L"", L".", L"", L" %", 0, 1, false ) );
      ((SP2::GGradientBar*)l_pRow->Object()->Child(L"frmGradient"))->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_RED));
      ((SP2::GGradientBar*)l_pRow->Object()->Child(L"frmGradient"))->Value(l_fValue, 0, false, false);
   }
   SortPressure();

   //Set the total pressure value
   REAL64 l_fValue = l_fTotalPressureValue / l_fTotalUnitsValue;
   m_pObjPressureTxt->Text( GString::FormatNumber(l_fValue * 100, L"", L".", L"", L" %", 0, 1, false ) );
   m_pObjPressureBar->Value(l_fValue, 0, false, false);
}

void GTreatyDetailsWindow::SortPressure()
{
   for(INT32 i=m_pObjPressureLst->Get_Nb_Rows() - 1; i>0; i--)
   {
      GUI::GLabel* l_pLabel1 = (GUI::GLabel*)m_pObjPressureLst->Get_Row_At(i)->Object()->Child(L"txtCode");
      GUI::GLabel* l_pLabel2 = NULL;
      GUI::GLabel* l_pLabelCmp = l_pLabel1;
      INT32 l_iBiggestID = i;

      for(INT32 j=0; j<i; j++)
      {
         l_pLabel2 = (GUI::GLabel*)m_pObjPressureLst->Get_Row_At(j)->Object()->Child(L"txtCode");

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
         m_pObjPressureLst->SwapRows(i, l_iBiggestID);
      }
   }

   m_pObjPressureLst->Update();
}


//This function will fill the two listbox containing countries
void GTreatyDetailsWindow::UpdateListBoxes()
{
   GUI::GListBox::GRow* l_pRow = NULL;
   m_bCountryNotEligible = false;
   bool l_bNotEligible = false;

   //If the side A has changed update it
   if(m_bSideADirty)
   {
      //m_pObjSideALst->Clear();
      sort(m_viSideA.begin(), m_viSideA.end());
	  INT32 i;
      for(i=0; i<(INT32)m_viSideA.size(); i++)
      {
         //Only add one country if it's one country per side
         if(i >= 1 && m_bSingleOnSide)
            break;

         ETreatyType::Enum l_eType = FindTreatyType(m_pObjTypeCbo->Selected_Content());
         bool l_bIsPonctual = false;
         if( l_eType != ETreatyType::Undefined )
         {
            l_bIsPonctual = GTreaty::Ponctual( l_eType );
         }
         
         //Check if the country is eligible to the treaty
         if( !l_bIsPonctual && !m_bWaitForEligibleAnswer && (m_siEligibleCountries.find(m_viSideA[i]) == m_siEligibleCountries.end() ))
         {
            l_bNotEligible = true;
            m_bCountryNotEligible = true;
         }

         const GCountry& l_Country = g_ClientDAL.Country(m_viSideA[i] );

         //If it's a single side treaty we have two country per row so we must check that case here
         if((!m_bTwoSidesTreaty && !(i % 2)) || m_bTwoSidesTreaty)
         {
            if(m_bTwoSidesTreaty)
            {
               if( m_pObjSideALst->Get_Nb_Rows() > i )
                  l_pRow = m_pObjSideALst->Get_Row_At(i);
               else
                  l_pRow = m_pObjSideALst->Insert_New_Row();
            }
            else
            {
               if( m_pObjSideALst->Get_Nb_Rows() > (INT32)floor( (REAL32)(i / 2) ) )
                  l_pRow = m_pObjSideALst->Get_Row_At( (INT32)floor( (REAL32)(i / 2) ) );
               else
                  l_pRow = m_pObjSideALst->Insert_New_Row();
            }
            
            ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));
            ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->Text(l_Country.Code());         

            l_pRow->Object()->Child(L"picFlag")->TooltipText( l_Country.Name() );
            l_pRow->Object()->Child(L"txtCode")->TooltipText( l_Country.Name() );

            //If the country was not eligible we set its text in red
            if(l_bNotEligible)
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->FontColor(GColorRGBInt(200,0,0,255));
            else
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->FontColor(GColorRGBInt(200,200,200,255));
         }

         if((!m_bTwoSidesTreaty && (i % 2)))
         {
            //l_pRow = m_pObjSideALst->Get_Row_At( m_pObjSideALst->Get_Nb_Rows() - 1 );

            ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode1"))->Text(l_Country.Code());         

            //If the country was not eligible we set its text in red
            if(l_bNotEligible)
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode1"))->FontColor(GColorRGBInt(200,0,0,255));
            else
               ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode1"))->FontColor(GColorRGBInt(200,200,200,255));

            l_pRow->Object()->Child(L"picFlag1")->TooltipText( l_Country.Name() );
            l_pRow->Object()->Child(L"txtCode1")->TooltipText( l_Country.Name() );
         }

         l_bNotEligible = false;
      }

      if(m_bTwoSidesTreaty)
      {
         if(!m_viSideA.size())
            m_pObjSideALst->Clear();
         else
         {
            INT32 l_iNb = m_pObjSideALst->Get_Nb_Rows() - 1;
            for(INT32 j=l_iNb; j>=(INT32)m_viSideA.size(); j--)
            {
               m_pObjSideALst->RemoveRow( j );
            }
         }
      }
      else
      {
         if(!m_viSideA.size())
            m_pObjSideALst->Clear();
         else
         {      
            INT32 l_iNb = m_pObjSideALst->Get_Nb_Rows() - 1;
            for(INT32 j=l_iNb; j>=(INT32)ceil((REAL64)m_viSideA.size() / 2.f); j--)
            {
               m_pObjSideALst->RemoveRow( j );
            } 
         }

         if( (i % 2) )
         {
            l_pRow = m_pObjSideALst->Get_Row_At( m_pObjSideALst->Get_Nb_Rows() - 1 );

            ( (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1")   )->PictureNormal(GString(L"000159"));
            ( (GUI::GLabel*)     l_pRow->Object()->Child(L"txtCode1")   )->Text(L"");       
            ( (GUI::GLabel*)     l_pRow->Object()->Child(L"txtStatus1") )->Text(L"");     

            l_pRow->Object()->Child(L"picFlag1")->TooltipText( L"" );
            l_pRow->Object()->Child(L"txtCode1")->TooltipText( L"" );
         }
      }

      m_pObjSideALst->Update();
   }

   //Get the treaty type
   ETreatyType::Enum l_eSelectedTreatyType = FindTreatyType( m_pObjTypeCbo->Selected_Content() );

   //Fill the side B if it's dirty
   if(m_bSideBDirty)
   {
      //m_pObjSideBLst->Clear();
      sort(m_viSideB.begin(), m_viSideB.end());
      for(INT32 i=0; i<(INT32)m_viSideB.size(); i++)
      {
         //Only add one country if it's one country per side
         if(i >= 1 && m_bSingleOnSide)
            break;

         ETreatyType::Enum l_eType = FindTreatyType(m_pObjTypeCbo->Selected_Content());
         bool l_bIsPonctual = false;
         if( l_eType != ETreatyType::Undefined )
         {
            l_bIsPonctual = GTreaty::Ponctual( l_eType );
         }

         //Check if it's a two side treaty and if the country is eligible to the treaty
         if(m_bTwoSidesTreaty)
         {
            if(l_eSelectedTreatyType != ETreatyType::War &&
               l_eSelectedTreatyType != ETreatyType::EconomicEmbargo &&
               l_eSelectedTreatyType != ETreatyType::WeaponTradeEmbargo &&
               l_eSelectedTreatyType != ETreatyType::Annexation)
            {
               if( !l_bIsPonctual && !m_bWaitForEligibleAnswer && (m_siEligibleCountries.find(m_viSideB[i]) == m_siEligibleCountries.end() ))
               {
                  l_bNotEligible = true;
                  m_bCountryNotEligible = true;
               }
            }
         }

         //Add the country if everything's ok
         const GCountry& l_Country = g_ClientDAL.Country(m_viSideB[i] );

         if( m_pObjSideBLst->Get_Nb_Rows() > i )
            l_pRow = m_pObjSideBLst->Get_Row_At(i);
         else
            l_pRow = m_pObjSideBLst->Insert_New_Row();

         ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));

         ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
         ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->Text(l_Country.Code());         

         if(l_bNotEligible)
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->FontColor(GColorRGBInt(200,0,0,255));
         else
            ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode"))->FontColor(GColorRGBInt(200,200,200,255));

         l_pRow->Object()->Child(L"picFlag")->TooltipText( l_Country.Name() );
         l_pRow->Object()->Child(L"txtCode")->TooltipText( l_Country.Name() );

         l_bNotEligible = false;
      }

      if(!m_viSideB.size())
         m_pObjSideBLst->Clear();
      else
      {
         INT32 l_iNb = m_pObjSideBLst->Get_Nb_Rows() - 1;
         for(INT32 j=l_iNb; j>=(INT32)m_viSideB.size(); j--)
         {
            m_pObjSideBLst->RemoveRow( j );
         }
      }

      m_pObjSideBLst->Update();
   }

   //If there was a country not eligible warn the user and disable the confirm button
   if(m_bCountryNotEligible)
   {
      m_pObjConfirmBtn->Enabled(false);
      m_pObjRemoveNotEligibleBtn->Enabled(true);

      if(!g_ClientDDL.GenericMessageWindow())
         g_ClientDDL.GenericMessageWindowSpawn();

      if(!m_bSkipEligibleWarning && !m_bWaitForEligibleAnswer)
      {
		 g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(102361), this );
         m_bSkipEligibleWarning = true;
      }
   }
   else
   {
      m_pObjRemoveNotEligibleBtn->Enabled(false);
   }

   m_bSideADirty = false;
   m_bSideBDirty = false;
}

void GTreatyDetailsWindow::FillSides(GTreaty* in_pTreaty)
{
   m_viSideA.clear();
   m_viSideB.clear();

   bool l_bDualSides = false;
   if(in_pTreaty->Sides() == 1)
   {
      if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusDoubleItem")
      {
         m_pObjSideALst->Width(283);
         m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusDoubleItem");
         m_pObjSideALst->Clear();
         m_pObjSideALbl->Hide();
         m_pObjSideBLbl->Hide();     
      }
      
      l_bDualSides = false;
   }
   else
   {
      if(m_pObjSideALst->RowType() != L"lstCountryFlagCodeStatusItem")
      {
         m_pObjSideALst->Width(141);
         m_pObjSideALst->RowType(L"lstCountryFlagCodeStatusItem");
         m_pObjSideALst->Clear();
         m_pObjSideALbl->Show();
         m_pObjSideBLbl->Show();
      }

      l_bDualSides = true;
   }
   
   const set<UINT32> * l_pMembersActive;
   const set<UINT32> * l_pMembersInnactive;
   const set<UINT32> * l_pMembersOriginals;

   //Map that will be used to generate country list
   //First digit is country id
   //Second digit is status : 1=active 2=pending 3=refused 4=suspended
   map<UINT32, UINT8> l_CountriesSideA;
   map<UINT32, UINT8> l_CountriesSideB;
   map<UINT32, UINT8> * l_CountriesSide;

   vector<INT32> * l_viSide;

   GUI::GListBox* l_pListBox;

   //We must do that twice (for the 2 sides)
   for(UINT8 i=0; i<2; i++)
   {
      //If it's the first iteration
      if(!i)
      {
         //Set information for Side A
         l_pMembersActive    = &in_pTreaty->MembersSideA(true);
         l_pMembersInnactive = &in_pTreaty->MembersSideA(false);
         l_pMembersOriginals = &in_pTreaty->MembersOriginalsA();
         l_CountriesSide     = &l_CountriesSideA;
         l_pListBox          = m_pObjSideALst;
         l_viSide            = &m_viSideA;
      }
      else
      {
         //Set information for Side B
         l_pMembersActive    = &in_pTreaty->MembersSideB(true);
         l_pMembersInnactive = &in_pTreaty->MembersSideB(false);
         l_pMembersOriginals = &in_pTreaty->MembersOriginalsB();
         l_CountriesSide     = &l_CountriesSideB;
         l_pListBox          = m_pObjSideBLst;
         l_viSide            = &m_viSideB;
      }

      //Run through each member original on side A and add it to the country list with refused status
      for(set<UINT32>::const_iterator l_It = l_pMembersOriginals->begin(); l_It != l_pMembersOriginals->end(); l_It++)
      {
         l_CountriesSide->insert(pair<UINT32, UINT8>(*l_It, 3));
      }

      //Run through each member on side A
      for(set<UINT32>::const_iterator l_It = l_pMembersInnactive->begin(); l_It != l_pMembersInnactive->end(); l_It++)
      {
         //If we can find the country on side A that means the country hasn't answered yet, set it to pending
         map<UINT32, UINT8>::iterator l_ItCmp = l_CountriesSide->find(*l_It);
         if(l_ItCmp != l_CountriesSide->end())
         {
            l_ItCmp->second = 2;
         }
         else
         {
            //If we can't find the country we must add it
            l_CountriesSide->insert(pair<UINT32, UINT8>(*l_It, 2));
         }
      }

      //Run through each active member on side A
      for(set<UINT32>::const_iterator l_It = l_pMembersActive->begin(); l_It != l_pMembersActive->end(); l_It++)
      {
         //If we can find the country on active side A that means the country has answered and set it to active
         map<UINT32, UINT8>::iterator l_ItCmp = l_CountriesSide->find(*l_It);
         if(l_ItCmp != l_CountriesSide->end())
         {
            l_ItCmp->second = 1;
         }
      }   

      //Run through each added country
      for(map<UINT32, UINT8>::iterator l_It = l_CountriesSide->begin(); l_It != l_CountriesSide->end(); l_It++)
      {
         //Check if the treaty is suspended, if so set it to suspended
         if(in_pTreaty->IsSuspended(l_It->first))
         {
            l_It->second = 4;
         }
      }

      //Run through each added country
      for(map<UINT32, UINT8>::iterator l_It = l_CountriesSide->begin(); l_It != l_CountriesSide->end(); l_It++)
      {
         //If the treaty is active erase countries that has refused
         if(in_pTreaty->Active() && (l_It->second == 3) )
         {
            l_It = l_CountriesSide->erase(l_It);
            l_It--;
         }
      }

      INT32 l_iCounter = 1;
      GUI::GListBox::GRow* l_pRow = NULL;
      GString l_sChildFlag;
      GString l_sChildCode;
      GString l_sChildStatus;

      //Run through each country to add them in the window listbox side A
      for(map<UINT32, UINT8>::iterator l_It = l_CountriesSide->begin(); l_It != l_CountriesSide->end(); l_It++)
      {     
         //Get the country
         const GCountry& l_Ctry = g_ClientDAL.Country(l_It->first);
         
         //If it's the first country on the line
         if(((l_iCounter % 2) != 0 && !l_bDualSides) || l_bDualSides)
         {
            //Insert a new row and set it's color
            if(l_bDualSides)
            {
               if( l_pListBox->Get_Nb_Rows() < l_iCounter)
                  l_pRow = l_pListBox->Insert_New_Row();
               else
                  l_pRow = l_pListBox->Get_Row_At( l_iCounter - 1 );
            }
            else
            {
               if( (l_pListBox->Get_Nb_Rows() * 2) < l_iCounter)
                  l_pRow = l_pListBox->Insert_New_Row();
               else
                  l_pRow = l_pListBox->Get_Row_At( (INT32)floor(((REAL32)l_iCounter - 1) / 2.f) );
            }

            ((GUI::GFrame*)l_pRow->Object())->ColorNormal(GColorRGBInt(20,20,20,255));

            l_sChildFlag   = L"picFlag";
            l_sChildCode   = L"txtCode";
            l_sChildStatus = L"txtStatus";
         }
         else
         {
            l_sChildFlag   = L"picFlag1";
            l_sChildCode   = L"txtCode1";
            l_sChildStatus = L"txtStatus1";
         }

         //Set the country information
         ( (GUI::GPictureBox*)l_pRow->Object()->Child(l_sChildFlag)   )->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Ctry.Id()]);
         ( (GUI::GLabel*)     l_pRow->Object()->Child(l_sChildCode)   )->Text( GString( l_Ctry.Flag() ).substr(0, 3) );         
         
         l_pRow->Object()->Child(l_sChildFlag)->TooltipText( l_Ctry.Name() );
         l_pRow->Object()->Child(l_sChildCode)->TooltipText( l_Ctry.Name() );

         EStrId::Enum l_Status = EStrId::Pending;
         switch(l_It->second)
         {
         case 1:
            l_Status = EStrId::Active;
            break;
         case 2:
            l_Status = EStrId::Pending;
            break;
         case 3:
            l_Status = EStrId::Refused;
            break;
         case 4:
            l_Status = EStrId::Suspended;
            break;
         }      
         ( (GUI::GLabel*)l_pRow->Object()->Child(l_sChildStatus) )->Text( g_ClientDAL.GetString( l_Status ) );

         l_iCounter++;
         l_viSide->push_back(l_It->first);
      }

      //Insert a new row and set it's color
      if(l_bDualSides)
      {
         if(!m_viSideA.size())
            m_pObjSideALst->Clear();
         else
         {
            INT32 l_iNb = m_pObjSideALst->Get_Nb_Rows();
            for(INT32 i=l_iNb; i>(INT32)m_viSideA.size(); i--)
            {
               m_pObjSideALst->RemoveRow( i - 1 );
            }
         }

         if(!m_viSideB.size())
            m_pObjSideBLst->Clear();
         else
         {
            INT32 l_iNb = m_pObjSideBLst->Get_Nb_Rows();
            for(INT32 i=l_iNb; i>(INT32)m_viSideB.size(); i--)
            {
               m_pObjSideBLst->RemoveRow( i - 1 );
            }
         }
      }
      else
      {
         if( !(l_iCounter % 2) )
         {
            l_pRow = m_pObjSideALst->Get_Row_At( (INT32)floor(((REAL32)l_iCounter - 2) / 2.f) );

            ( (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1")   )->PictureNormal(GString(L"000159"));
            ( (GUI::GLabel*)     l_pRow->Object()->Child(L"txtCode1")   )->Text(L"");         
            ( (GUI::GLabel*)     l_pRow->Object()->Child(L"txtStatus1") )->Text(L"");     

            l_pRow->Object()->Child(l_sChildFlag)->TooltipText( L"" );
            l_pRow->Object()->Child(l_sChildCode)->TooltipText( L"" );
         }

         if(!m_viSideA.size())
            m_pObjSideALst->Clear();
         else
         {
            INT32 l_iNb = m_pObjSideALst->Get_Nb_Rows();
            for(INT32 i=l_iNb; i>(INT32)ceil((REAL32)m_viSideA.size() / 2.f); i--)
            {
               m_pObjSideALst->RemoveRow( i - 1 );
            } 
         }
      }
   }
   
   m_pObjSideALst->Update();
   m_pObjSideBLst->Update();
}

bool GTreatyDetailsWindow::OnCreate()
{
	__super::OnCreate();

   m_pData       = &(g_ClientDAL.m_DataTreatiesWindow);
   m_pWindowData = &(g_ClientDAL.m_DataTreatyDetailsWindow);

   m_pObjApplyBtn->Hide();

   GUI::GFrame* l_pDescriptionFrm = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmMain");
   
   m_pObjNameEdt        = (GUI::GEditBox*)l_pDescriptionFrm->Child(L"edtName");
   m_pObjNameOverEdt    = (GUI::GEditBox*)l_pDescriptionFrm->Child(L"edtNameOver");
   m_pObjNameOverEdt->Hide();

   m_pObjTypeCbo        = (GUI::GComboBox*)l_pDescriptionFrm->Child(L"cboType");
   m_pObjSideALst       = (GUI::GListBox*)l_pDescriptionFrm->Child(L"lstCountriesA");
   m_pObjSideBLst       = (GUI::GListBox*)l_pDescriptionFrm->Child(L"lstCountriesB");
   m_pObjSideALbl       = (GUI::GLabel*)l_pDescriptionFrm->Child(L"lblSideA");
   m_pObjSideBLbl       = (GUI::GLabel*)l_pDescriptionFrm->Child(L"lblSideB");

   m_pObjModifyFrm   = (GUI::GFrame*)l_pDescriptionFrm->Child(L"frmModify");
   m_pObjTreatyOpenTog  = (GUI::GToggle*)l_pDescriptionFrm->Child(L"togOpen");
   m_pObjDescriptionFrm = (GUI::GFrame*)l_pDescriptionFrm->Child(L"frmDescription");
   m_pObjDescriptionLbl = (GUI::GLabel*)m_pObjDescriptionFrm->Child(L"txtDetail");
   m_pObjDescriptionScr = (GUI::GVScrollBar*)l_pDescriptionFrm->Child(L"scrDescription");
   m_pObjConfirmBtn     = (GUI::GButton*)l_pDescriptionFrm->Child(L"btnConfirm");
   m_pObjCancelBtn      = (GUI::GButton*)l_pDescriptionFrm->Child(L"btnCancel");
   m_pObjPressureLst    = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmPressure")->Child(L"lstPressure");
   m_pObjPressureTxt    = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmPressure")->Child(L"txtValue");
   m_pObjPressureBar    = (SP2::GGradientBar*)Child(L"frmInner")->Child(L"frmPressure")->Child(L"frmGradient");
   m_pObjRemoveNotEligibleBtn = (GUI::GButton*)Child(L"frmInner")->Child(L"frmEligibleCountries")->Child(L"btnRemoveNotEligible");
   m_pObjRemoveNotEligibleBtn->Enabled(false);

   m_pObjEligibleLst    = (GUI::GListBox*)Child(L"frmInner")->Child(L"frmEligibleCountries")->Child(L"lstEligibleCountries");

   m_pObjPressureBar->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_RED));
   m_pObjPressureBar->Value(0.f,0,false,false);
   m_pObjSponsorBtn     = (GUI::GButton*)Child(L"frmInner")->Child(L"frmPressure")->Child(L"btnSponsor");
   m_pObjSponsorBtn->Text(g_ClientDAL.GetString(EStrId::SponsorTreaty));

   m_pObjCondRelations           = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboRelations");
   m_pObjCondGeographicProximity = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboGeographicProximity");
   m_pObjCondPoliticalStrength   = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboPoliticalStrength");
   m_pObjCondEconomicStrength    = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboEconomicStrength");
   m_pObjCondMilitaryStrength    = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboMilitaryStrength");
   m_pObjCondResearchLevel       = (GUI::GComboBox*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"cboResearchLevel");

   m_pObjCondRelationsLbl           = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblRelations");
   m_pObjCondGeographicProximityLbl = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblGeographicProximity");
   m_pObjCondPoliticalStrengthLbl   = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblPoliticalStr");
   m_pObjCondEconomicStrengthLbl    = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblEconomicStr");
   m_pObjCondMilitaryStrengthLbl    = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblMilitaryStr");
   m_pObjCondResearchLevelLbl       = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmConditions")->Child(L"lblResearchLvl");

   m_pObjStatusPic   = (GUI::GPictureBox*)l_pDescriptionFrm->Child(L"picStatus");
   m_pObjStatusLbl   = (GUI::GLabel*)     l_pDescriptionFrm->Child(L"txtStatus");
   m_pObjStatusPic->BringToFront();

   m_pObjModifyBtn     = (GUI::GButton*)m_pObjModifyFrm->Child(L"btnModify");
   m_pObjRefuseBtn      = (GUI::GButton*)m_pObjModifyFrm->Child(L"btnRefuse");
   m_pObjRefuseBtn->Hide();

   ((GFX::GWindow*)m_pObjDescriptionFrm->Model()->Get_Primitive())->ClipChildren(true);
   m_pObjConfirmBtn->Enabled(false);

   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAlliance));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAnnexation));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyCommonMarket));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicAid));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyFreeRegion));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyNobleCause));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyResearchPartnership));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWar));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWeaponTrade));
   m_pObjTypeCbo->Add_Item(g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo));

   m_pObjCondRelations->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondRelations->Add_Item(g_ClientDAL.GetString(EStrId::NotAtWar));
   m_pObjCondRelations->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::Africa));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::Europe));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::America));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::Oceania));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::Asia));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::CentralAmerica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::EasternAfrica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::EasternAsia));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::EasternEurope));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::FarEast));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::MiddleEast));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::NorthernAfrica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::NorthernAmerica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::PacificIslands));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::SouthernAfrica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::SouthernAmerica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::SouthernOceania));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::WesternAfrica));
   m_pObjCondGeographicProximity->Add_Item(g_ClientDAL.GetString(EStrId::WesternEurope));
   m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::AllDemocracies));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::CommunistState));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::MilitaryDictatorship));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::Monarchy));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::MultiPartyDemocracy));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::SinglePartyDemocracy));
   m_pObjCondPoliticalStrength->Add_Item(g_ClientDAL.GetString(EStrId::Theocracy));
   m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjCondEconomicStrength->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondEconomicStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top10));
   m_pObjCondEconomicStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top25));
   m_pObjCondEconomicStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top50));
   m_pObjCondEconomicStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top100));
   m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjCondMilitaryStrength->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondMilitaryStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top10));
   m_pObjCondMilitaryStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top25));
   m_pObjCondMilitaryStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top50));
   m_pObjCondMilitaryStrength->Add_Item(g_ClientDAL.GetString(EStrId::Top100));
   m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjCondResearchLevel->Add_Item(g_ClientDAL.GetString(EStrId::NoLimit));
   m_pObjCondResearchLevel->Add_Item(g_ClientDAL.GetString(EStrId::ConditionLow));
   m_pObjCondResearchLevel->Add_Item(g_ClientDAL.GetString(EStrId::ConditionAverage));
   m_pObjCondResearchLevel->Add_Item(g_ClientDAL.GetString(EStrId::ConditionHigh));
   m_pObjCondResearchLevel->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));

   m_pObjDescriptionScr->Initialize(this, 0, 0, 0, 0, 0);

   if(!g_ClientDDL.AddRemoveWindow())
      g_ClientDDL.AddRemoveWindowSpawn();
   
   if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.AddRemoveWindow());
   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());

   m_bWaitForClose = false;

	return true;
}

bool GTreatyDetailsWindow::OnDestroy()
{
   RegisteredModalWindow.clear();

   if(g_ClientDDL.AddRemoveWindow())
      g_ClientDDL.AddRemoveWindowKill();

   return __super::OnDestroy();
}

void GTreatyDetailsWindow::OnShow()
{
   __super::OnShow();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Treaties);
}

void GTreatyDetailsWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Treaties);

   m_bWaitForClose = false;

   __super::OnHide();
}

void GTreatyDetailsWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Treaties)
   {
      m_bSideADirty = true;
      if(m_bTwoSidesTreaty)
         m_bSideBDirty = true;
      
      SP2::GTreatyNotifyData* l_Data = (SP2::GTreatyNotifyData*)in_pAdditionalMsg;
      
      if(m_pCurrentTreaty)
      {
         /*
         hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().begin();
         while(l_It != g_ClientDAL.Treaties().end())
         {
            if( (INT32)l_It->second.ID() == l_iUpdateID )
            {
               Update(m_pCurrentTreaty->ID());
               return;
            }

            l_It++;
         }
         */

         hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().find( (UINT32)l_Data->m_iTreatyID );
         if( l_It != g_ClientDAL.Treaties().end() )
         {
            if(!l_Data->m_bToRemove)
            {
               if( m_iTreatyID == l_Data->m_iTreatyID )
               {
                  Update(m_pCurrentTreaty->ID());
                  return;
               }
            }
            else
            {
				   if( ( m_iTreatyID == l_Data->m_iTreatyID ) )
				   {
                  if(!g_ClientDDL.GenericMessageWindow())
                     g_ClientDDL.GenericMessageWindowSpawn();

					   if(l_It->second.ShouldBeActive())
                     g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::TreatyIsAccepted), this );
                  else
                     g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::TreatyHasBeenRemoved), this );

					   m_bWaitForClose = true;
                  m_iTreatyID = -1;
                  m_pCurrentTreaty = NULL;

                  m_pObjNameEdt->Enabled(false);
                  m_pObjTypeCbo->Enabled(false);
                  m_pObjTreatyOpenTog->Enabled(false);
                  m_pObjModifyBtn->Enabled(false);
                  m_pObjConfirmBtn->Enabled(false);
                  m_pObjRefuseBtn->Hide();

                  EnableConditions(false);
				   }
            }
         }
      }
      else
      {
         hash_map<UINT32, SP2::GTreaty>::const_iterator l_It = g_ClientDAL.Treaties().begin();
         while(l_It != g_ClientDAL.Treaties().end())
         {
            if((l_It->second.Name() == m_sTreatyName) && (m_sTreatyName != L""))
            {
               Update(l_It->second.ID());
               break;
            }

            l_It++;
         }
      }
   }
}

void GTreatyDetailsWindow::FillDescription(ETreatyType::Enum in_eType)
{
   GString l_sText;

   switch(in_eType)
   {
   case ETreatyType::Alliance:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionAlliance);
      break;
   case ETreatyType::War:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionWar);
      break;
   case ETreatyType::MilitaryAccess:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionMilitaryAccess);
      break;
	case ETreatyType::EconomicPartnership:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionEconomicPartnership);
      break;
	case ETreatyType::CommonMarket:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionCommonMarket);
      break;
	case ETreatyType::EconomicAid:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionEconomicAid);
      break;
	case ETreatyType::AssumeForeignDebt:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionAssumeForeignDebt);
      break;
	case ETreatyType::EconomicEmbargo:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionEconomicEmbargo);
      break;
	case ETreatyType::WeaponTrade:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionWeaponTrade);
      break;
	case ETreatyType::WeaponTradeEmbargo:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionWeaponTradeEmbargo);
      break;
	case ETreatyType::HumanDevelopmentCollaboration:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionHumanDevelopmentCollaboration);
      break;
	case ETreatyType::CulturalExchanges:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionCulturalExchanges);
      break;
	case ETreatyType::ResearchPartnership:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionResearchPartnership);
      break;
	case ETreatyType::RequestMilitaryPresenceRemoval:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionRequestMilitaryPresenceRemoval);
      break;
	case ETreatyType::NobleCause:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionNobleCause);
      break;
	case ETreatyType::Annexation:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionAnnexation);
      break;
	case ETreatyType::FreeRegion:
      l_sText = g_ClientDAL.GetString(EStrId::TreatyDescriptionFreeRegion);
      break;
   }

   UINT32 l_iPos = l_sText.find(L"\\n");
   while (l_iPos != string::npos)
   {
      l_sText.replace(l_iPos, 2, L"\n");
      l_iPos = l_sText.find(L"\\n", l_iPos);
   }

   m_pObjDescriptionLbl->Text(l_sText);
   m_pObjDescriptionLbl->Height(m_pObjDescriptionLbl->Text2D()->Height());
   m_pObjDescriptionLbl->Top(0);
   m_pObjDescriptionScr->Initialize(this, 0, max(0, m_pObjDescriptionLbl->Height() - m_pObjDescriptionFrm->Height()), 1, 10, 0);
}

INT32 GTreatyDetailsWindow::DisplayedTreatyID()
{
   if(m_pCurrentTreaty)
      return m_pCurrentTreaty->ID();
   else
      return -1;
}

//Once we received the result from the FetchNewAvailableCountryList function check the two sides and fill the eligible countries
void GTreatyDetailsWindow::ApplyCountryConditions(const set<UINT32> & in_Countries)
{
   m_bWaitForEligibleAnswer = false;

   m_siEligibleCountries.clear();
   
   //Get the new eligible countries list
   if(in_Countries.size())
      m_siEligibleCountries = in_Countries;
   
   GUI::GListBox::GRow* l_pRow  = NULL;
   GUI::GPictureBox*    l_pPic  = NULL;
   GUI::GLabel*         l_pCode = NULL;

   INT32 i=0;

   map<GString, INT32> l_Countries;
   
   //Refill the eligible countries
   for(set<UINT32>::iterator l_It = m_siEligibleCountries.begin(); l_It != m_siEligibleCountries.end(); l_It++)
   {
      l_Countries.insert( make_pair( GString(g_ClientDAL.Country(*l_It).Code()), INT32(g_ClientDAL.Country(*l_It).Id()) ) );
   }

   //m_pObjEligibleLst->Clear();
   //For each country
   for(map<GString, INT32>::iterator l_It = l_Countries.begin(); l_It != l_Countries.end(); l_It++)
   {
      //Check if we have enough row to add the country if not create a new row
      if( i + 1 <= (m_pObjEligibleLst->Get_Nb_Rows() * 5) )
      {
         l_pRow = m_pObjEligibleLst->Get_Row_At( (INT32)floor((REAL32)(i) / 5.f) );
      }
      else
      {
         l_pRow = m_pObjEligibleLst->Insert_New_Row();
      }

      
      //Check in which part we are suppose to set this country
      if( !((i - 4) % 5) )
      {
         l_pPic = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag4");
         l_pCode = (GUI::GLabel*)l_pRow->Object()->Child(L"txtCode4");
      }
      else if( !((i - 3) % 5) )
      {
         l_pPic = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag3");
         l_pCode = (GUI::GLabel*)l_pRow->Object()->Child(L"txtCode3");
      }
      else if( !((i - 2) % 5) )
      {
         l_pPic = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag2");
         l_pCode = (GUI::GLabel*)l_pRow->Object()->Child(L"txtCode2");
      }
      else if( !((i - 1) % 5) )
      {
         l_pPic = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1");
         l_pCode = (GUI::GLabel*)l_pRow->Object()->Child(L"txtCode1");
      }
      else if( !(i % 5) )
      {
         l_pPic = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag");
         l_pCode = (GUI::GLabel*)l_pRow->Object()->Child(L"txtCode");
      }

      //Set the country info
      l_pPic->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_It->second]);
      l_pCode->Text( l_It->first );

      l_pPic->TooltipText( g_ClientDAL.CountryFromCode(l_It->first).Name() );
      l_pCode->TooltipText( g_ClientDAL.CountryFromCode(l_It->first).Name() );


      i++;
   }   

   //Get the last row
   l_pRow = m_pObjEligibleLst->Get_Row_At( (INT32)floor((REAL32)(i) / 5.f) );
   
   //Check up to where we added countries to remove the flag and code where there is no country
   switch(i % 5)
   {
   case 1:
      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag1"))->PictureNormal(GString(L"000159"));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode1"))->Text(L"");
      l_pRow->Object()->Child(L"picFlag1")->TooltipText( L"" );
      l_pRow->Object()->Child(L"txtCode1")->TooltipText( L"" );
   case 2:
      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag2"))->PictureNormal(GString(L"000159"));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode2"))->Text(L"");
      l_pRow->Object()->Child(L"picFlag2")->TooltipText( L"" );
      l_pRow->Object()->Child(L"txtCode2")->TooltipText( L"" );
   case 3:
      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag3"))->PictureNormal(GString(L"000159"));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode3"))->Text(L"");
      l_pRow->Object()->Child(L"picFlag3")->TooltipText( L"" );
      l_pRow->Object()->Child(L"txtCode3")->TooltipText( L"" );
   case 4:
      ((GUI::GPictureBox*)l_pRow->Object()->Child(L"picFlag4"))->PictureNormal(GString(L"000159"));
      ((GUI::GLabel*)l_pRow->Object()->Child(L"txtCode4"))->Text(L"");
      l_pRow->Object()->Child(L"picFlag4")->TooltipText( L"" );
      l_pRow->Object()->Child(L"txtCode4")->TooltipText( L"" );
      break;
   default:
      break;
   }

   //Remove every rows that we're not filled
   INT32 l_iNb = m_pObjEligibleLst->Get_Nb_Rows();
   for(INT32 j=l_iNb; j>(INT32)ceil((REAL32)(i) / 5.f); j--)
   {
      m_pObjEligibleLst->RemoveRow( j - 1 );
   }

   //Update the eligible list
   m_pObjEligibleLst->Update();

   //If we are checking a treaty that already exist fill the sides 
   if(m_pCurrentTreaty)
   {
      FillSides(m_pCurrentTreaty);

      //Check if the user can't join the treaty
      if( m_siEligibleCountries.find((UINT32)g_ClientDAL.ControlledCountryID()) == m_siEligibleCountries.end() )
      {
         //Make sure the user can always leave the treaty
         if( m_pObjModifyBtn->Text() != g_ClientDAL.GetString(EStrId::Leave) )
            m_pObjModifyBtn->Enabled(false);
         else
            m_pObjModifyBtn->Enabled(true);
      }
      else
      {
         m_pObjModifyBtn->Enabled(true);
      }
   }
   else
   {
      //If it's a new treaty fill the sides
      m_bSideADirty = true;
      m_bSideBDirty = true;
      UpdateListBoxes();

      //Check if there is countries not eligible and disable the confirm button
      if(m_bCountryNotEligible || m_pCurrentTreaty)
         m_pObjConfirmBtn->Enabled(false);
      else
      {
         if(m_bTwoSidesTreaty)
         {
            if(m_viSideA.size() && m_viSideB.size() && m_sName.size() > 0)
               m_pObjConfirmBtn->Enabled(true);
         }
         else
         {
            if(m_viSideA.size() >= 2 && m_sName.size() > 0)
               m_pObjConfirmBtn->Enabled(true);
         }
      }
   }

   return;
}

void GTreatyDetailsWindow::EnableConditions(bool in_bEnabled)
{
   if(in_bEnabled == m_pObjCondEconomicStrength->Enabled())
   {
      if(!in_bEnabled)
      {
         m_pObjCondEconomicStrength->Selected_Content(L"");
         m_pObjCondGeographicProximity->Selected_Content(L"");
         m_pObjCondMilitaryStrength->Selected_Content(L"");
         m_pObjCondPoliticalStrength->Selected_Content(L"");
         m_pObjCondRelations->Selected_Content(L"");
         m_pObjCondResearchLevel->Selected_Content(L"");
      }
   }

   m_pObjCondEconomicStrength->Enabled(in_bEnabled);
   m_pObjCondGeographicProximity->Enabled(in_bEnabled);
   m_pObjCondMilitaryStrength->Enabled(in_bEnabled);
   m_pObjCondPoliticalStrength->Enabled(in_bEnabled);
   m_pObjCondRelations->Enabled(in_bEnabled);
   m_pObjCondResearchLevel->Enabled(in_bEnabled);

   m_pObjCondRelationsLbl->Enabled(in_bEnabled);
   m_pObjCondGeographicProximityLbl->Enabled(in_bEnabled);
   m_pObjCondPoliticalStrengthLbl->Enabled(in_bEnabled);
   m_pObjCondEconomicStrengthLbl->Enabled(in_bEnabled);
   m_pObjCondMilitaryStrengthLbl->Enabled(in_bEnabled);
   m_pObjCondResearchLevelLbl->Enabled(in_bEnabled);
   
   if(!in_bEnabled)
   {
      if( m_pObjCondEconomicStrength->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::EconomicStrengthSimilitude] = ConditionGetEconomicStrengthSimilitude( m_pObjCondEconomicStrength->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::EconomicStrengthSimilitude] = EConditionStatusRank::NoLimit;

      if( m_pObjCondGeographicProximity->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::GeographicProximity] = ConditionGetGeographicProximity( m_pObjCondGeographicProximity->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NoLimit;

      if( m_pObjCondMilitaryStrength->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::MilitaryStrengthSimilitude] = ConditionGetMilitaryStrengthSimilitude( m_pObjCondMilitaryStrength->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::MilitaryStrengthSimilitude] = EConditionStatusRank::NoLimit;

      if( m_pObjCondPoliticalStrength->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::PoliticalSimilitude] = ConditionGetPoliticalSimilitude( m_pObjCondPoliticalStrength->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::PoliticalSimilitude] = EConditionStatusRank::NoLimit;

      if( m_pObjCondRelations->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::RelationsNotAtWar] = ConditionGetRelationsCloseness( m_pObjCondRelations->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NoLimit;

      if( m_pObjCondResearchLevel->Selected_Content() != L"" )
         m_pConditions[ETreatyConditions::ResearchLevelSimilitude] = ConditionGetResearchLevelSimilitude( m_pObjCondResearchLevel->Selected_Content() );
      else
         m_pConditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::NoLimit;

      m_pObjCondEconomicStrength->Selected_Content(L"");
      m_pObjCondGeographicProximity->Selected_Content(L"");
      m_pObjCondMilitaryStrength->Selected_Content(L"");
      m_pObjCondPoliticalStrength->Selected_Content(L"");
      m_pObjCondRelations->Selected_Content(L"");
      m_pObjCondResearchLevel->Selected_Content(L"");
   }
   else
   {
      FillConditions(m_pConditions);
   }
}

void GTreatyDetailsWindow::RemoveTreaty(INT32 in_iTreatyID)
{
   if((INT32)m_pCurrentTreaty->ID() == in_iTreatyID)
   {
      m_pCurrentTreaty = NULL;
      m_sTreatyName    = L"";
   }
}

bool GTreatyDetailsWindow::IsTreatyTwoSided(ETreatyType::Enum in_eType)
{
   switch(in_eType)
   {
   case ETreatyType::Alliance:
      return false;
   case ETreatyType::War:
      return true;
   case ETreatyType::MilitaryAccess:
      return true;
	case ETreatyType::EconomicPartnership:
      return false;
	case ETreatyType::CommonMarket:
      return false;
	case ETreatyType::EconomicAid:
      return true;
	case ETreatyType::AssumeForeignDebt:
      return true;
	case ETreatyType::EconomicEmbargo:
      return true;
	case ETreatyType::WeaponTrade:
      return false;
	case ETreatyType::WeaponTradeEmbargo:
      return true;
	case ETreatyType::HumanDevelopmentCollaboration:
      return false;
	case ETreatyType::CulturalExchanges:
      return false;
	case ETreatyType::ResearchPartnership:
      return false;
	case ETreatyType::RequestMilitaryPresenceRemoval:
      return true;
	case ETreatyType::NobleCause:
      return false;
	case ETreatyType::Annexation:
      return true;
	case ETreatyType::FreeRegion:
      return true;
   default:
      return false;
   }
}

ETreatyType::Enum GTreatyDetailsWindow::FindTreatyType(GString in_sTreatyName)
{
   if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyAlliance))
      return ETreatyType::Alliance;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyAnnexation))
      return ETreatyType::Annexation;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyCommonMarket))
      return ETreatyType::CommonMarket;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges))
      return ETreatyType::CulturalExchanges;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyEconomicAid))
      return ETreatyType::EconomicAid;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo))
      return ETreatyType::EconomicEmbargo;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership))
      return ETreatyType::EconomicPartnership;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt))
      return ETreatyType::AssumeForeignDebt;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyFreeRegion))
      return ETreatyType::FreeRegion;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration))
      return ETreatyType::HumanDevelopmentCollaboration;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess))
      return ETreatyType::MilitaryAccess;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyNobleCause))
      return ETreatyType::NobleCause;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval))
      return ETreatyType::RequestMilitaryPresenceRemoval;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyResearchPartnership))
      return ETreatyType::ResearchPartnership;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyWar))
      return ETreatyType::War;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyWeaponTrade))
      return ETreatyType::WeaponTrade;
   else if(in_sTreatyName == g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo))
      return ETreatyType::WeaponTradeEmbargo;
   else
      return ETreatyType::Undefined;
}

//Fill the treaty conditions with a treaty
void GTreatyDetailsWindow::FillConditions(const UINT32* in_Conditions)
{
   const UINT32* & l_Conditions = in_Conditions;

   if( l_Conditions[ETreatyConditions::RelationsNotAtWar] == 0 )
      m_pObjCondRelations->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::RelationsNotAtWar] == EConditionStatusRelation::NotAtWar )
      m_pObjCondRelations->Selected_Content(g_ClientDAL.GetString(EStrId::NotAtWar));

   if( l_Conditions[ETreatyConditions::GeographicProximity] == 0 )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::Africa )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::Africa));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::Europe )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::Europe));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::America )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::America));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::Oceania )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::Oceania));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::Asia )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::Asia));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::CentralAmerica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::CentralAmerica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::EasternAfrica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::EasternAfrica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::EasternAsia )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::EasternAsia));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::EasternEurope )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::EasternEurope));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::FarEast )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::FarEast));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::MiddleEast )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::MiddleEast));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::NorthernAfrica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::NorthernAfrica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::NorthernAmerica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::NorthernAmerica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::PacificIslands )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::PacificIslands));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::SouthernAfrica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::SouthernAfrica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::SouthernAmerica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::SouthernAmerica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::SouthernOceania )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::SouthernOceania));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::WesternAfrica )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::WesternAfrica));
   else if( l_Conditions[ETreatyConditions::GeographicProximity] == EConditionStatusGeo::WesternEurope )
      m_pObjCondGeographicProximity->Selected_Content(g_ClientDAL.GetString(EStrId::WesternEurope));

   if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == 0 )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::AllDemocracies )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::AllDemocracies));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::Communism )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::CommunistState));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::MilitaryDictatorship )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::MilitaryDictatorship));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::Monarchy )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Monarchy));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::MultiParty )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::MultiPartyDemocracy));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::SingleParty )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::SinglePartyDemocracy));
   else if( l_Conditions[ETreatyConditions::PoliticalSimilitude] == EConditionStatusPolitical::Theocracy )
      m_pObjCondPoliticalStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Theocracy));

   if( l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] == 0 )
      m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] == EConditionStatusRank::Top10 )
      m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top10));
   else if( l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] == EConditionStatusRank::Top25 )
      m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top25));
   else if( l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] == EConditionStatusRank::Top50 )
      m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top50));
   else if( l_Conditions[ETreatyConditions::EconomicStrengthSimilitude] == EConditionStatusRank::Top100 )
      m_pObjCondEconomicStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top100));

   if( l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] == 0 )
      m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] == EConditionStatusRank::Top10 )
      m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top10));
   else if( l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] == EConditionStatusRank::Top25 )
      m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top25));
   else if( l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] == EConditionStatusRank::Top50 )
      m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top50));
   else if( l_Conditions[ETreatyConditions::MilitaryStrengthSimilitude] == EConditionStatusRank::Top100 )
      m_pObjCondMilitaryStrength->Selected_Content(g_ClientDAL.GetString(EStrId::Top100));

   if( l_Conditions[ETreatyConditions::ResearchLevelSimilitude] == EConditionStatusResearch::NoLimit )
      m_pObjCondResearchLevel->Selected_Content(g_ClientDAL.GetString(EStrId::NoLimit));
   else if( l_Conditions[ETreatyConditions::ResearchLevelSimilitude] == EConditionStatusResearch::Average )
      m_pObjCondResearchLevel->Selected_Content(g_ClientDAL.GetString(EStrId::ConditionAverage));
   else if( l_Conditions[ETreatyConditions::ResearchLevelSimilitude] == EConditionStatusResearch::High )
      m_pObjCondResearchLevel->Selected_Content(g_ClientDAL.GetString(EStrId::ConditionHigh));
   else if( l_Conditions[ETreatyConditions::ResearchLevelSimilitude] == EConditionStatusResearch::Low )
      m_pObjCondResearchLevel->Selected_Content(g_ClientDAL.GetString(EStrId::ConditionLow));
}

EConditionStatusRank::Enum GTreatyDetailsWindow::ConditionGetEconomicStrengthSimilitude(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::NoLimit) )
      return EConditionStatusRank::NoLimit;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top10) )
      return EConditionStatusRank::Top10;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top25) )
      return EConditionStatusRank::Top25;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top50) )
      return EConditionStatusRank::Top50;
   else
      return EConditionStatusRank::Top100;
}

EConditionStatusRank::Enum GTreatyDetailsWindow::ConditionGetMilitaryStrengthSimilitude(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::NoLimit) )
      return EConditionStatusRank::NoLimit;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top10) )
      return EConditionStatusRank::Top10;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top25) )
      return EConditionStatusRank::Top25;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Top50) )
      return EConditionStatusRank::Top50;
   else
      return EConditionStatusRank::Top100;
}

EConditionStatusPolitical::Enum GTreatyDetailsWindow::ConditionGetPoliticalSimilitude(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::AllDemocracies) )
		return EConditionStatusPolitical::AllDemocracies;
   else if( in_sText == g_ClientDAL.GetString(EStrId::CommunistState) )
      return EConditionStatusPolitical::Communism;
   else if( in_sText == g_ClientDAL.GetString(EStrId::MilitaryDictatorship) )
      return EConditionStatusPolitical::MilitaryDictatorship;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Monarchy) )
      return EConditionStatusPolitical::Monarchy;
	else if( in_sText == g_ClientDAL.GetString(EStrId::MultiPartyDemocracy) )
      return EConditionStatusPolitical::MultiParty;
	else if( in_sText == g_ClientDAL.GetString(EStrId::SinglePartyDemocracy) )
      return EConditionStatusPolitical::SingleParty;
	else if( in_sText == g_ClientDAL.GetString(EStrId::Theocracy) )
      return EConditionStatusPolitical::Theocracy;
   else
		return EConditionStatusPolitical::NoLimit;
}

EConditionStatusGeo::Enum GTreatyDetailsWindow::ConditionGetGeographicProximity(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::NoLimit) )
      return EConditionStatusGeo::NoLimit;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Africa) )
      return EConditionStatusGeo::Africa;
   else if( in_sText == g_ClientDAL.GetString(EStrId::America) )
      return EConditionStatusGeo::America;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Europe) )
      return EConditionStatusGeo::Europe;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Asia) )
      return EConditionStatusGeo::Asia;
   else if( in_sText == g_ClientDAL.GetString(EStrId::Oceania) )
      return EConditionStatusGeo::Oceania;
   else if( in_sText == g_ClientDAL.GetString(EStrId::WesternEurope) )
      return EConditionStatusGeo::WesternEurope;
   else if( in_sText == g_ClientDAL.GetString(EStrId::CentralAmerica) )
      return EConditionStatusGeo::CentralAmerica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::SouthernAmerica) )
      return EConditionStatusGeo::SouthernAmerica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::EasternEurope) )
      return EConditionStatusGeo::EasternEurope;
   else if( in_sText == g_ClientDAL.GetString(EStrId::EasternAsia) )
      return EConditionStatusGeo::EasternAsia;
   else if( in_sText == g_ClientDAL.GetString(EStrId::FarEast) )
      return EConditionStatusGeo::FarEast;
   else if( in_sText == g_ClientDAL.GetString(EStrId::MiddleEast) )
      return EConditionStatusGeo::MiddleEast;
   else if( in_sText == g_ClientDAL.GetString(EStrId::NorthernAfrica) )
      return EConditionStatusGeo::NorthernAfrica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::WesternAfrica) )
      return EConditionStatusGeo::WesternAfrica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::EasternAfrica) )
      return EConditionStatusGeo::EasternAfrica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::SouthernAfrica) )
      return EConditionStatusGeo::SouthernAfrica;
   else if( in_sText == g_ClientDAL.GetString(EStrId::PacificIslands) )
      return EConditionStatusGeo::PacificIslands;
   else if( in_sText == g_ClientDAL.GetString(EStrId::NorthernAmerica) )
      return EConditionStatusGeo::NorthernAmerica;
   else
      return EConditionStatusGeo::SouthernOceania;
}

EConditionStatusRelation::Enum GTreatyDetailsWindow::ConditionGetRelationsCloseness(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::NoLimit) )
      return EConditionStatusRelation::NoLimit;
   else
      return EConditionStatusRelation::NotAtWar;
}

EConditionStatusResearch::Enum GTreatyDetailsWindow::ConditionGetResearchLevelSimilitude(const GString & in_sText)
{
   if( in_sText == g_ClientDAL.GetString(EStrId::NoLimit) )
      return EConditionStatusResearch::NoLimit;
   else if( in_sText == g_ClientDAL.GetString(EStrId::ConditionLow) )
      return EConditionStatusResearch::Low;
   else if( in_sText == g_ClientDAL.GetString(EStrId::ConditionAverage) )
      return EConditionStatusResearch::Average;
   else
      return EConditionStatusResearch::High;
}