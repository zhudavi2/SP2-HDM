/**************************************************************
*
* sp2_internal_laws_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_internal_laws_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_frame_cursor_meter.h"

const GString GInternalLawsWindow::TypeName(L"Internal_Laws");

/*!
 * MAIN MENU construction function
 **/
GUI::GBaseObject* GInternalLawsWindow::New()
{
	return (GUI::GBaseObject*) new GInternalLawsWindow();
}


GInternalLawsWindow::GInternalLawsWindow() : GBaseWindow()
{
   m_pData       = g_ClientDAL.DataInternalLawsWindow;
   m_pWindowData = &(g_ClientDAL.DataInternalLawsWindow[0]);

   m_pObjInnerFrame = NULL;
   m_pObjMigration = NULL;
   m_pObjTabGroups = NULL;
   m_pObjLstReligions = NULL;
   m_pObjLstLanguages = NULL;
   m_pObjLstParties = NULL;
   m_pObjLstLaws = NULL;
   m_pObjImmigrationCbo = NULL;
   m_pObjEmigrationCbo = NULL;
   m_iGvtType = -1;
   m_bWindowCanDisable = true;
}

GInternalLawsWindow::~GInternalLawsWindow()
{
}

void GInternalLawsWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Politic);
   // remove the request associated with our internal laws window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);

   return __super::OnHide();
}

bool GInternalLawsWindow::Enabled()
{
   return m_bEnabled;
}

void GInternalLawsWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmHumanMigration")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmDemographicGroups")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmLaws")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   ((GUI::GLabel*)m_pObjInnerFrame->Child(L"frmOrganizations")->Child(L"lblTitle"))->Enabled( in_bEnabled );
   
   ((GLabel*)m_pObjMigration->Child(L"txtImmigrationValue"))->Enabled( in_bEnabled );
   ((GLabel*)m_pObjMigration->Child(L"txtEmigrationValue"))->Enabled( in_bEnabled );
   ((GLabel*)m_pObjMigration->Child(L"lblImmigration"))->Enabled( in_bEnabled );
   ((GLabel*)m_pObjMigration->Child(L"lblEmigration"))->Enabled( in_bEnabled );

   m_pObjImmigrationCbo->Enabled( in_bEnabled );
   m_pObjEmigrationCbo->Enabled( in_bEnabled );
}

bool GInternalLawsWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrame        = (GUI::GFrame*)Child(L"frmInner");
   m_pObjMigration         = (GUI::GFrame*)m_pObjInnerFrame->Child(L"frmHumanMigration");
   m_pObjTabGroups         = (GUI::GTabbed*)m_pObjInnerFrame->Child(L"frmDemographicGroups")->Child(L"tabGroups");
   m_pObjLstReligions      = (GUI::GListBox*)m_pObjTabGroups->Child(L"Tab1")->Child(L"lstGroups");
   m_pObjLstLanguages      = (GUI::GListBox*)m_pObjTabGroups->Child(L"Tab2")->Child(L"lstGroups");
   m_pObjLstParties        = (GUI::GListBox*)m_pObjInnerFrame->Child(L"frmOrganizations")->Child(L"lstOrganization");
   m_pObjLstLaws           = (GUI::GListBox*)m_pObjInnerFrame->Child(L"frmLaws")->Child(L"lstLaws");
   m_pObjImmigrationCbo    = (GUI::GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus");
   m_pObjEmigrationCbo     = (GUI::GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus");
     
   m_pObjTabGroups->m_pTabFrame->ColorNormal(GColorRGBInt(20,20,20,255));
   
   m_pObjTabGroups->m_vTabPage[0]->PictureNormal(L"000647");
   m_pObjTabGroups->m_vTabPage[0]->Borders( GBorders(1,1,1,1) );

   m_pObjTabGroups->m_vTabPage[1]->PictureNormal(L"000647");
   m_pObjTabGroups->m_vTabPage[1]->Borders( GBorders(1,1,1,1) );

   m_pObjTabGroups->m_vTabButtons[0]->BringToFront();
   m_pObjTabGroups->m_vTabButtons[1]->BringToFront();

   m_pObjTabGroups->m_vTabPage[0]->TooltipText( g_ClientDAL.GetString(102234) );
   m_pObjTabGroups->m_vTabPage[1]->TooltipText( g_ClientDAL.GetString(102235) );

   m_pObjTabGroups->m_vTabButtons[0]->TooltipText( g_ClientDAL.GetString(102234) );
   m_pObjTabGroups->m_vTabButtons[1]->TooltipText( g_ClientDAL.GetString(102235) );

   // be sure its empty
   m_pData[0].m_vpLawsCbo.clear();

   //Internal laws
   for(INT32 i=100682; i<=100690; i++)
   {
      // new row with the law id as additional info
      GUI::GListBox::GRow* l_pRow = m_pObjLstLaws->Insert_New_Row(i);
      vector<GString> l_sChoices;

      // construct the possible choices list
      BuildLawChoicesList(l_sChoices, i);

      // law name as text
      ((GUI::GLabel*)l_pRow->Object()->Child(L"lblName"))->Text( g_ClientDAL.GetString(i) );
      
      for(UINT32 j=0; j<l_sChoices.size(); j++)
      {
         if(j == 0)
            ((GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Selected_Content( l_sChoices[j] );

         ((GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item( l_sChoices[j] );
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->DropSize( 14 * (j + 1) + 2 );
      }

      m_pData[0].m_vpLawsCbo.push_back( (GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus") );
   }
   m_pObjLstLaws->Update();


   if(Visible())
      Update();


   if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());

	return true;
}

void GInternalLawsWindow::BuildLawChoicesList(vector<GString> &in_sChoices, UINT32 in_iLaw)
{
   // empty the list
   in_sChoices.clear();

   // switch the law, build the selection
   switch(in_iLaw)
   {
   case 100682:
   case 100683:
   case 100684:
   case 100686:
   case 100687:
   case 100688:
   case 100689:
   case 100690:
      in_sChoices.push_back( g_ClientDAL.GetString(EStrId::Permitted) );
      in_sChoices.push_back( g_ClientDAL.GetString(EStrId::NotPermitted) );
      break;
   case 100685:
      in_sChoices.push_back( g_ClientDAL.GetString(EStrId::Unlimited) ); 
      in_sChoices.push_back( g_ClientDAL.GetString(EStrId::Limited) );                  		        
      break;
   }
}

bool GInternalLawsWindow::OnDestroy()
{
   RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

GUI::EEventStatus::Enum GInternalLawsWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch(in_EventId)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:
      break;
   default:
      return GBaseObject::OnCustomEvent(in_EventId, in_EventData, in_pCaller);
   }

   GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
   if(l_pCombo)
   {
      GString l_sBordersAreClosed = g_ClientDAL.GetString(100123);
      m_pLastClickedCombo = l_pCombo;

      //first check if it's a border combo
      if(l_pCombo == m_pObjEmigrationCbo)
      {
         // Emigration closed ?
         if(m_pData[0].m_bEmigrationClosed)
         {
            if(l_pCombo->Selected_Content() != l_sBordersAreClosed)
            {
               m_pData[0].m_bEmigrationClosed = false;
               Dirty(true);
            }
         }
         else
         {
            if(l_pCombo->Selected_Content() == l_sBordersAreClosed)
            {
               m_pData[0].m_bEmigrationClosed = true;
               Dirty(true);
            }
         }

         return EEventStatus::Handled;
      }

      // Immigration closed ?
      if(l_pCombo == m_pObjImmigrationCbo)
      {
         if(m_pData[0].m_bImmigrationClosed)
         {
            if(l_pCombo->Selected_Content() != l_sBordersAreClosed)
            {
               m_pData[0].m_bImmigrationClosed = false;
               Dirty(true);
            }
         }
         else
         {
            if(l_pCombo->Selected_Content() == l_sBordersAreClosed)
            {
               m_pData[0].m_bImmigrationClosed = true;
               Dirty(true);
            }
         }

         return EEventStatus::Handled;
      }

      INT32 i, j = 0;
      bool l_bFound = false;
      for(i=0; i<3; i++)
      {
         for(j=0; j<(INT32)m_pData[i].m_viId.size(); j++)
         {
            if(l_pCombo == m_pData[i].m_vpCombo[j])
            {
               l_bFound = true;
               break;
            }
         }

         if(l_bFound)
            break;
      }

      if(i < 3)
      {
         //If we are a multi party democracy and the user changed the status of political party
         if(g_ClientDAL.DataConstitutionalForm.GvtType == 9 && i == 2)
         {
            //ensure the user didn't made illegal all parties except one if so tell him his about to change his const. form
            UINT32 l_iNbIllegal = 0;
            UINT32 l_iNbTotal   = 0;
            for(INT32 k=0; k<(INT32)m_pData[i].m_vpCombo.size(); k++)
            {
               if(m_pData[i].m_vpCombo[k])
               {
                  if(m_pData[i].m_vpCombo[k]->Selected_Content() == g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(2)))
                     l_iNbIllegal++;

                  l_iNbTotal++;
               }
            }

            if((l_iNbIllegal + 1) == l_iNbTotal)
            {
               //Display confirmation window
               GGenericQuestion l_Question;
               l_Question.m_pParentWindow    = this;
               l_Question.m_pOberserToNotify = this;
               l_Question.m_sMessage         = g_ClientDAL.GetString(100452);
               l_Question.m_sOkCaption        = g_ClientDAL.GetString(100348);
               l_Question.m_sCancelCaption    = g_ClientDAL.GetString(100349);
               g_ClientDDL.GenericQuestion(l_Question);

            }
         }

         //If we are a multi party democracy and the user changed the status of political party
         if(g_ClientDAL.DataConstitutionalForm.GvtType == 8 && i == 2)
         {
            //ensure the user didn't made illegal all parties except one if so tell him his about to change his const. form
            UINT32 l_iNbLegal = 0;
            for(INT32 k=0; k<(INT32)m_pData[i].m_vpCombo.size(); k++)
            {
               if(m_pData[i].m_vpCombo[k])
               {
                  if(m_pData[i].m_vpCombo[k]->Selected_Content() != g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(2)))
                     l_iNbLegal++;
               }
            }

            if(l_iNbLegal > 1)
            {
               //Display confirmation window
               GGenericQuestion l_Question;
               l_Question.m_pParentWindow    = this;
               l_Question.m_pOberserToNotify = this;
               l_Question.m_sMessage         = g_ClientDAL.GetString(100453);
               l_Question.m_sOkCaption        = g_ClientDAL.GetString(100348);
               l_Question.m_sCancelCaption    = g_ClientDAL.GetString(100349);

               g_ClientDDL.GenericQuestion(l_Question);
            }
         }      

         bool l_bAllIllegal = true;
         //ensure everything isn't illegal
         if(m_pData[i].m_vpCombo[j]->Selected_Content() == g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(2) )) //2 for illegal
         {
            for(INT32 k=0; k<(INT32)m_pData[i].m_viId.size(); k++)
            {
               if(j == k)
                  continue;
               
               if(m_pData[i].m_vpCombo[k] && (m_pData[i].m_vpCombo[k]->Selected_Content() != g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(2))))
                  l_bAllIllegal = false;
            }

            if(l_bAllIllegal)
            {
               if(!g_ClientDDL.GenericMessageWindow())
                  g_ClientDDL.GenericMessageWindowSpawn();

               switch(i)
               {
               case 0:
                  g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(100449), this );
                  break;
               case 1:
                  g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(100450), this );
                  break;
               case 2:
                  g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(100451), this );
                  break;
               }

               m_pData[i].m_vpCombo[j]->Selected_Content( g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(m_pData[i].m_viStatus[j]) ) );
            }
         }
      }
      else
      {
         for(UINT32 k=0; k<m_pData[0].m_vpLawsCbo.size(); k++)
         {
            if(l_pCombo == m_pData[0].m_vpLawsCbo[k])
            {
               GString l_sSel = m_pData[0].m_vpLawsCbo[k]->Selected_Content();
               
               if(l_sSel == g_ClientDAL.GetString(EStrId::Permitted) || l_sSel == g_ClientDAL.GetString(EStrId::Unlimited))
               {
                  if(m_pData[0].m_vbLaws[k] != true)
                  {
                     m_pData[0].m_vbLaws[k] = true;
                     Dirty(true);
                  }
               }
               else
               {
                  if(m_pData[0].m_vbLaws[k] != false)
                  {
                     m_pData[0].m_vbLaws[k] = false;
                     Dirty(true);
                  }
               }
            }
         }
      }

      if(l_bFound)
      {
         gassert(i < 3, "Ca chie " );

         for(UINT32 k = 1; k <= 7; k++)
         {
            if(g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(k) ) == l_pCombo->Selected_Content())
            {
               if(m_pData[i].m_viStatus[j] != (INT32)k)
               {
                  m_pData[i].m_vbChanged[j] = true;
                  Dirty(true);
               }
               break;
            }
         }
      }
   }

   return EEventStatus::Handled;
}

void GInternalLawsWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   
   if(in_Notification == c_OnGenericQuestionWindowAnswerCancel)
   {
      // This was comming from old system - > To veryfy!!!
      INT32 i;
      bool l_bFound = false;
      for(i=0; i<(INT32)m_pData[2].m_viId.size(); i++)
      {
         if(m_pLastClickedCombo == m_pData[2].m_vpCombo[i])
         {
            l_bFound = true;
            break;
         }
      }

      if(l_bFound)
      {
         m_pLastClickedCombo->Selected_Content(g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(m_pData[2].m_viStatus[i])));
      }
   }
   else
   {

      // is there any request pending for our const form window, if yes do not notify
      if(!RequestPending())
         if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Politic)
            Update();
   }
}

void GInternalLawsWindow::Update(bool in_bForce)
{  
   // Update only if necessary ...
   if(Dirty() && !in_bForce)
      return;
   
   //Update from country data if it's your country
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      // Retrieve the country information
      SP2::GCountryDataClient* l_pData = &g_ClientDAL.m_PlayerCountryData;

      // Our internal laws update event
      SDK::GGameEventSPtr l_Event                = CREATE_GAME_EVENT(SP2::Event::GInternalLawsUpdate);
      SP2::Event::GInternalLawsUpdate* l_pUpdate = ( SP2::Event::GInternalLawsUpdate*)l_Event.get();

      // Set up em/immigration data
      l_pUpdate->m_bEmigrationClosed      = l_pData->EmigrationClosed();
      l_pUpdate->m_bImmigrationClosed     = l_pData->ImmigrationClosed();
      l_pUpdate->m_fEmigrationLevel       = l_pData->EmigrationLevel();		
      l_pUpdate->m_fImmigrationLevel      = l_pData->ImmigrationLevel();

      // Asserts em/immigration data
      gassert(!_isnan(l_pUpdate->m_fEmigrationLevel) && l_pUpdate->m_fEmigrationLevel >= 0.f,"Emigration is NAN");

      // Build the Languages
      const GLanguageList& l_Languages = l_pData->GetLanguages();
      GLanguageList::const_iterator l_It = l_Languages.begin();
      while(l_It != l_Languages.end())
      {
         l_pUpdate->m_viType.push_back(1);
         l_pUpdate->m_viId.push_back(l_It->first);
         l_pUpdate->m_viStatus.push_back(l_pData->LanguageGetStatus(l_It->first));
         l_pUpdate->m_vfPerc.push_back(l_pData->LanguageGetPourcentage(l_It->first));

         l_It++;
      }

      // Build the Religions
      const GReligionList& l_Religions = l_pData->GetReligions();
      GReligionList::const_iterator l_It2 = l_Religions.begin();
      while(l_It2 != l_Religions.end())
      {
         l_pUpdate->m_viType.push_back(2);
         l_pUpdate->m_viId.push_back(l_It2->first);
         l_pUpdate->m_viStatus.push_back(l_pData->ReligionGetStatus(l_It2->first));
         l_pUpdate->m_vfPerc.push_back(l_pData->ReligionGetPourcentage(l_It2->first));

         l_It2++;
      }

      // Build the political parties
      vector<GPoliticalParty> l_vParties = l_pData->PoliticalParty();
      for(UINT32 i=0; i<l_vParties.size(); i++)
      {
         if(l_vParties[i].CountryId() == (UINT16)g_ClientDAL.ControlledCountryID())
         {
            l_pUpdate->m_viType.push_back(3);
            l_pUpdate->m_viId.push_back(l_vParties[i].Id());

            if(l_vParties[i].InPower())
               l_pUpdate->m_viStatus.push_back(3);
            else
               l_pUpdate->m_viStatus.push_back(l_vParties[i].Status());
            l_pUpdate->m_vfPerc.push_back(l_vParties[i].PercentageValue());
            l_pUpdate->m_viGvtType.push_back((UINT8)l_vParties[i].GvtType());
         }
      }

      // Clear the Internal law data
      GDataInternalLaws* l_pInternalLawsData = g_ClientDAL.DataInternalLawsWindow;
      for(UINT32 i=0; i<3; i++)
      {
         GDataInternalLaws & l_Data = l_pInternalLawsData[i];
         
         l_Data.m_vbChanged.clear();
         l_Data.m_vfPerc.clear();
         l_Data.m_viId.clear();
         l_Data.m_viStatus.clear();
         l_Data.m_vpCombo.clear();
         l_Data.m_viGvtType.clear();
      }
      
      INT32 l_iIndex = 0;
      for(UINT32 i=0; i<l_pUpdate->m_viType.size(); i++)
      {
         GDataInternalLaws & l_Data = l_pInternalLawsData[l_pUpdate->m_viType[i] - 1];
         
         l_Data.m_viId.push_back( l_pUpdate->m_viId[i] );
         l_Data.m_viStatus.push_back( l_pUpdate->m_viStatus[i] );
         l_Data.m_vfPerc.push_back( l_pUpdate->m_vfPerc[i] );
         l_Data.m_vbChanged.push_back(false);
         l_Data.m_vpCombo.push_back(NULL);

         if(l_pUpdate->m_viGvtType.size() > 0 && l_pUpdate->m_viType[i] == 3)
         {
            l_Data.m_viGvtType.push_back(l_pUpdate->m_viGvtType[l_iIndex]);
            l_iIndex++;
         }

         if(i == 0)
         {
            l_Data.m_bEmigrationClosed  = l_pUpdate->m_bEmigrationClosed;
            l_Data.m_bImmigrationClosed = l_pUpdate->m_bImmigrationClosed;
            l_Data.m_fEmigration        = l_pUpdate->m_fEmigrationLevel;
				gassert(!_isnan(l_Data.m_fEmigration) && l_Data.m_fEmigration >= 0.f,"Emigration isnan");
            l_Data.m_fImmigration       = l_pUpdate->m_fImmigrationLevel;
            l_Data.m_iGvtType           = (INT16)g_ClientDAL.m_PlayerCountryData.GvtType();
         }
      }

      GDataInternalLaws & l_ILData = l_pInternalLawsData[0];
      for(INT32 i=0; i<EInternalLaws::ItemCount; i++)
      {
         bool l_bData = l_pData->InternalLaw( (EInternalLaws::Enum)i );
         l_ILData.m_vbLaws[(EInternalLaws::Enum)i] = l_bData;
      }

      for(INT32 i=0; i<m_pObjLstLaws->Get_Nb_Rows(); i++)
      {
         ((GUI::GComboBox*)m_pObjLstLaws->Get_Row_At(i)->Object()->Child(L"cboStatus"))->Enabled(true);
      }
   }
   else
   {
      for(INT32 i=0; i<m_pObjLstLaws->Get_Nb_Rows(); i++)
      {
         ((GUI::GComboBox*)m_pObjLstLaws->Get_Row_At(i)->Object()->Child(L"cboStatus"))->Enabled(false);
      }
   }

   vector<GString> l_sChoices;
   for(UINT32 i=0; i<EInternalLaws::ItemCount; i++)
   {
      GUI::GListBox::GRow* l_pRow = (GUI::GListBox::GRow*)m_pObjLstLaws->Get_Row_At(i);
      GUI::GComboBox* l_pCbo      = (GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus");

      // build the possible choices
      BuildLawChoicesList(l_sChoices, l_pRow->m_iAdditionalId);
      // do we have choices ?
      if(l_sChoices.size() > 0)
      {
         // is the requested law permitted ?
         if(!m_pData[0].m_vbLaws[i])
            // no ? choose content 1 wich is the "illegal" text
            l_pCbo->Selected_Content(l_sChoices[1]);
         else
            // yes ? choos the content 0 wich is the "legal" text
            l_pCbo->Selected_Content(l_sChoices[0]);
      }
   }

   // Languages list update
   {
      bool l_bUpdateLanguages = false;
      for(UINT32 i=0; i<m_pData[0].m_viId.size(); i++)
      {
         GListBox::GRow* l_pRow = NULL;

         if( m_pObjLstLanguages->Get_Nb_Rows() <= (INT32)i )
         {
            l_pRow = m_pObjLstLanguages->Insert_New_Row();
            l_bUpdateLanguages = true;
         }
         else
            l_pRow = m_pObjLstLanguages->Get_Row_At(i);

         ((GFrame*)l_pRow->Object())->ColorNormal(L"RGBA: 0, 0, 0",L"0");
         ((GFrame*)l_pRow->Object())->PictureNormal(L"000587");
         ((GFrame*)l_pRow->Object())->Borders( GBorders(1,1,1,1) );
         ((GLabel*)l_pRow->Object()->Child(L"lblName"))->Text( g_ClientDAL.GetString( g_ClientDAL.StringIdLanguage(m_pData[0].m_viId[i]) ) );
         ((GLabel*)l_pRow->Object()->Child(L"lblPerc"))->Text(GString::FormatNumber(m_pData[0].m_vfPerc[i] * 100.f,L"",L".",L"",L" %",0,1));
         
         if( ( g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() ) && !((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->NbItems() )
         {
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(1)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(2)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(3)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->DropSize(44);
         }
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Selected_Content( g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(m_pData[0].m_viStatus[i]) ) );

         m_pData[0].m_vpCombo[i] = ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"));
      }
      for( INT32 i=0; i<m_pObjLstLanguages->Get_Nb_Rows() - (INT32)m_pData[0].m_viId.size(); i++ )
      {
         m_pObjLstLanguages->RemoveRow( m_pObjLstLanguages->Get_Nb_Rows() - i - 1 );
         l_bUpdateLanguages = true;
      }
      if(l_bUpdateLanguages)
      {
         m_pObjLstLanguages->Update();
      }
   }

   // Religions list update
   {
      bool l_bUpdateReligions = false;
      for(UINT32 i=0; i<m_pData[1].m_viId.size(); i++)
      {
         GListBox::GRow* l_pRow = NULL;

         if( m_pObjLstReligions->Get_Nb_Rows() <= (INT32)i )
         {
            l_pRow = m_pObjLstReligions->Insert_New_Row();
            l_bUpdateReligions = true;
         }
         else
            l_pRow = m_pObjLstReligions->Get_Row_At(i);

         ((GFrame*)l_pRow->Object())->ColorNormal(L"RGBA: 0, 0, 0",L"0");
         ((GFrame*)l_pRow->Object())->PictureNormal(L"000587");
         ((GFrame*)l_pRow->Object())->Borders( GBorders(1,1,1,1) );
         ((GLabel*)l_pRow->Object()->Child(L"lblName"))->Text( g_ClientDAL.GetString( g_ClientDAL.StringIdReligion(m_pData[1].m_viId[i]) ) );
         ((GLabel*)l_pRow->Object()->Child(L"lblPerc"))->Text(GString::FormatNumber(m_pData[1].m_vfPerc[i] * 100.f,L"",L".",L"",L" %",0,1));
         if( ( g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() ) && !((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->NbItems() )
         {
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(1)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(2)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(3)));
            ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->DropSize(44);
         }
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Selected_Content( g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(m_pData[1].m_viStatus[i]) ) );

         m_pData[1].m_vpCombo[i] = ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"));
      }
      for( INT32 i=0; i<m_pObjLstReligions->Get_Nb_Rows() - (INT32)m_pData[1].m_viId.size(); i++ )
      {
         m_pObjLstReligions->RemoveRow( m_pObjLstReligions->Get_Nb_Rows() - i - 1 );
         l_bUpdateReligions = true;
      }
      if(l_bUpdateReligions)
      {
         m_pObjLstReligions->Update();
      }
   }

   INT32 l_iDisplayed = m_pObjLstParties->Get_Nb_Rows();
   INT32 l_iCurrent   = 0;

   for(UINT32 i=0; i<m_pData[2].m_viId.size(); i++)
   {
      if(m_iGvtType != m_pData[0].m_iGvtType)
      {
         m_iGvtType = m_pData[0].m_iGvtType;
         m_pObjLstParties->Clear();
         l_iDisplayed = 0;
      }

      if(m_pData[0].m_iGvtType == 8 || m_pData[0].m_iGvtType == 9)
      {
         if(m_pData[2].m_viGvtType[i] != 9)
            continue;

         if(m_pData[0].m_iGvtType == 8 && m_pData[2].m_viStatus[i] != 3)
            continue;
      }
      else
      {
         if(m_pData[0].m_iGvtType != m_pData[2].m_viGvtType[i])
            continue;
      }      
     
      GListBox::GRow* l_pRow = NULL;

      if( l_iDisplayed <= l_iCurrent )
      {
         l_pRow = m_pObjLstParties->Insert_New_Row();
         l_iDisplayed++;
      }
      else
      {
         l_pRow = m_pObjLstParties->Get_Row_At(l_iCurrent);
      }

      l_iCurrent++;

      ((GFrame*)l_pRow->Object())->ColorNormal(L"RGBA: 0, 0, 0",L"0");
      ((GFrame*)l_pRow->Object())->PictureNormal(L"000587");
      ((GFrame*)l_pRow->Object())->Borders( GBorders(1,1,1,1) );
      ((GLabel*)l_pRow->Object()->Child(L"lblName"))->Text( g_ClientDAL.GetString( g_ClientDAL.StringIdPoliticalParty(m_pData[2].m_viId[i]) ) );
      ((GLabel*)l_pRow->Object()->Child(L"lblPerc"))->Text(GString::FormatNumber(m_pData[2].m_vfPerc[i] * 100.f,L"",L".",L"",L" %",0,1));
      if( ( g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() ) && !((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->NbItems() )
      {
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(1)));
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Add_Item(g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(2)));
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->DropSize(30);
      }
      if(m_pData[2].m_viStatus[i] != 3)
      {
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Selected_Content( g_ClientDAL.GetString( g_ClientDAL.StringIdStatus(m_pData[2].m_viStatus[i]) ) );
      }
      else
      {
         ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"))->Selected_Content( g_ClientDAL.GetString( EStrId::InPower ) );
      }

      m_pData[2].m_vpCombo[i] = ((GComboBox*)l_pRow->Object()->Child(L"cboStatus"));
   }
   for( INT32 i=0; i<m_pObjLstParties->Get_Nb_Rows() - (INT32)m_pData[2].m_viId.size(); i++ )
   {
      m_pObjLstParties->RemoveRow( m_pObjLstParties->Get_Nb_Rows() - i - 1 );
   }
   m_pObjLstParties->Update();

   ((GLabel*)m_pObjMigration->Child(L"txtEmigrationValue"))->Text(GString::FormatNumber(m_pData[0].m_fEmigration * 100.f,L"",L".",L"",L" %",0,1));
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() && !((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->NbItems())
   {
      ((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->Add_Item(g_ClientDAL.GetString(100123));
      ((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->Add_Item(g_ClientDAL.GetString(100124));
   }
   if(m_pData[0].m_bEmigrationClosed == true)
      ((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->Selected_Content(g_ClientDAL.GetString(100123));
   else
      ((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->Selected_Content(g_ClientDAL.GetString(100124));

   ((GLabel*)m_pObjMigration->Child(L"txtImmigrationValue"))->Text(GString::FormatNumber(m_pData[0].m_fImmigration * 100.f,L"",L".",L"",L" %",0,1));
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() && !((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->NbItems())
   {
      ((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->Add_Item(g_ClientDAL.GetString(100123));
      ((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->Add_Item(g_ClientDAL.GetString(100124));
   }
   if(m_pData[0].m_bImmigrationClosed == true)
      ((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->Selected_Content(g_ClientDAL.GetString(100123));
   else
      ((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->Selected_Content(g_ClientDAL.GetString(100124));

   bool l_bEnabled = false;
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      l_bEnabled = true;
   }

   for(INT32 i=0; i<3; i++)
   {
      for(INT32 j=0; j<(INT32)m_pData[i].m_vpCombo.size(); j++)
      {
         if(m_pData[i].m_vpCombo.size() && m_pData[i].m_vpCombo[j])
         {
            m_pData[i].m_vpCombo[j]->Enabled(l_bEnabled);

            if( m_pData[i].m_vpCombo[j]->Selected_Content() == g_ClientDAL.GetString( EStrId::InPower ) )
               m_pData[i].m_vpCombo[j]->Enabled(false);
         }
      }
   }

   ((GComboBox*)m_pObjMigration->Child(L"cboImmigrationStatus"))->Enabled(l_bEnabled);
   ((GComboBox*)m_pObjMigration->Child(L"cboEmigrationStatus"))->Enabled(l_bEnabled);

   Dirty(false);
}

void GInternalLawsWindow::OnShow()
{
   __super::OnShow();

   if(m_pObjInnerFrame)
      Update();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Politic);
}

void GInternalLawsWindow::Save()
{
   if(!Dirty())
      return;
   
   m_fWhenSaved = g_Joshua.GameTime();

   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GInternalLawsUpdate);
   SP2::Event::GInternalLawsUpdate* l_pUpdate = (SP2::Event::GInternalLawsUpdate*)l_pEvent.get();

   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_bGetData = false;
   
   for(INT32 i=0; i<3; i++)
   {
      for(UINT32 j=0; j<m_pData[i].m_viId.size(); j++)
      {
         if(m_pData[i].m_vbChanged[j])
         {
            l_pUpdate->m_viType.push_back(i + 1);
            l_pUpdate->m_viId.push_back(m_pData[i].m_viId[j]);
            
            UINT32 k;
            for(k=1; k<=7; k++)
            {
               if(m_pData[i].m_vpCombo[j]->Selected_Content() == g_ClientDAL.GetString(g_ClientDAL.StringIdStatus(k)))
               {
                  break;
               }
            }

            l_pUpdate->m_viStatus.push_back(k);
         }
      }
   }

   // laws
   for(UINT32 i=0; i<EInternalLaws::ItemCount; i++)
   {
      l_pUpdate->m_vbLaws[i] = m_pData[0].m_vbLaws[i];
   }

   // borders
   l_pUpdate->m_bImmigrationClosed = m_pData[0].m_bImmigrationClosed;
   l_pUpdate->m_bEmigrationClosed  = m_pData[0].m_bEmigrationClosed;

   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdate);

   g_Joshua.RaiseEvent(l_pEvent);
     
   Dirty(false);
}

void GInternalLawsWindow::Clear()
{
   m_pObjLstReligions->Clear();
   m_pObjLstParties->Clear();
   m_pObjLstLanguages->Clear();
}
