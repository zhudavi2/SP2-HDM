/**************************************************************
*
* sp2_politic_event_handler.cpp
*
* Description
* ===========
*  Used to handle every politic events that happen
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

using namespace SP2::Event;
using namespace SDK::Event;

SP2::GPoliticEventHandler::GPoliticEventHandler()
{
}

SP2::GPoliticEventHandler::~GPoliticEventHandler()
{
}

bool SP2::GPoliticEventHandler::HandleUpdateConstitutionalForm(SDK::GGameEventSPtr in_Event)
{
   //Get the game event and the player from who it's coming     
   SP2::Event::GConstitutionalFormUpdate* l_pConstitutionalFormUpdateEvent = (SP2::Event::GConstitutionalFormUpdate*)in_Event.get();
   
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pConstitutionalFormUpdateEvent->m_iSource);
   if(!l_pPlayer)
      return false;

   if(!l_pConstitutionalFormUpdateEvent->GetData)
   {
      INT32 l_iCountryId = l_pConstitutionalFormUpdateEvent->CountryId;

      UINT32 l_iCapitalId = l_pConstitutionalFormUpdateEvent->CapitalId;
      INT32 l_iGvtType   = l_pConstitutionalFormUpdateEvent->GvtType;

      // TODO : Make sure GvtType is valid
      GCountryData* l_Data = g_ServerDAL.CountryData(l_iCountryId);
      
      if(l_iGvtType != l_Data->GvtType())
		{
			if(l_pPlayer->ModID() != l_iCountryId)
			{
				g_ServerDCL.LiberateRegions(l_pPlayer->ModID(),l_iCountryId);
				
				//MultiMOD
				//We'll also set relations to 100
				g_ServerDAL.RelationBetweenCountries(l_iCountryId, l_pPlayer->ModID(), 100);
				g_ServerDAL.RelationBetweenCountries(l_pPlayer->ModID(), l_iCountryId, 100);
			}
         g_ServerDCL.ChangeGovernmentType(l_iCountryId, (SP2::EGovernmentType::Enum)l_Data->GvtType(), (SP2::EGovernmentType::Enum)l_iGvtType);
		}

      if(l_pConstitutionalFormUpdateEvent->MartialLaw != l_Data->MartialLaw())
         g_ServerDCL.ChangeMartialLaw(l_iCountryId, l_pConstitutionalFormUpdateEvent->MartialLaw);

      // capital is different ??
      if(l_iCapitalId != g_ServerDAL.CapitalID(l_iCountryId))
         g_ServerDCL.ChangeCapitalID(l_iCountryId, l_iCapitalId);

      // this event need an acknowledgement of receipt
      g_ServerDCL.AcknowledgeRequest(l_pConstitutionalFormUpdateEvent->m_iRequestID,
                                     l_pConstitutionalFormUpdateEvent->m_iSource);
   }
   else
   {     
      GCountryData* l_Data = g_ServerDAL.CountryData(l_pConstitutionalFormUpdateEvent->CountryId);

      l_pConstitutionalFormUpdateEvent->NextElection = l_Data->NextElection().Serialize();
      l_pConstitutionalFormUpdateEvent->CapitalId    = g_ServerDAL.CapitalID(l_pConstitutionalFormUpdateEvent->CountryId);
      l_pConstitutionalFormUpdateEvent->GvtType      = l_Data->GvtType();
      l_pConstitutionalFormUpdateEvent->MartialLaw   = l_Data->MartialLaw();
      
      l_pConstitutionalFormUpdateEvent->m_iTarget = l_pConstitutionalFormUpdateEvent->m_iSource;
      l_pConstitutionalFormUpdateEvent->m_iSource = 0;

      g_Joshua.RaiseEvent(in_Event);
   }

   return true;
}

bool SP2::GPoliticEventHandler::HandleUpdateInternalLaws(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GInternalLawsUpdate* l_pUpdate = (SP2::Event::GInternalLawsUpdate*)in_Event.get();
   
   if(l_pUpdate->m_bGetData)
   {
      GCountryData* l_pData = g_ServerDAL.CountryData(l_pUpdate->m_iCountryId);
      
      l_pUpdate->m_bEmigrationClosed      = l_pData->EmigrationClosed();
      l_pUpdate->m_bImmigrationClosed     = l_pData->ImmigrationClosed();
      l_pUpdate->m_fEmigrationLevel       = l_pData->EmigrationLevel();
		gassert(!_isnan(l_pUpdate->m_fEmigrationLevel) && l_pUpdate->m_fEmigrationLevel >= 0.f,"Emigration isnan");
      l_pUpdate->m_fImmigrationLevel      = l_pData->ImmigrationLevel();

      l_pUpdate->m_iGvtType               = (INT16)l_pData->GvtType();

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

      vector<GPoliticalParty> l_vParties = l_pData->PoliticalParty();
      for(UINT32 i=0; i<l_vParties.size(); i++)
      {
         if(l_vParties[i].CountryId() == (ENTITY_ID)l_pUpdate->m_iCountryId)
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

      for(UINT32 i=0; i<EInternalLaws::ItemCount; i++)
      {
         l_pUpdate->m_vbLaws[i] =  l_pData->InternalLaw( (EInternalLaws::Enum)i );
      }

      //! \todo *Add Internal laws*
      l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
      l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

      g_Joshua.RaiseEvent(in_Event);
   }
   else
   {
      if(!g_Joshua.ActivePlayer(l_pUpdate->m_iSource) )
         return false;

      ENTITY_ID l_iCountry = g_Joshua.ActivePlayer(l_pUpdate->m_iSource)->ModID();
      GCountryData* l_pData = g_ServerDAL.CountryData(l_iCountry);

      if(l_pUpdate->m_bEmigrationClosed != l_pData->EmigrationClosed())
         g_ServerDCL.ChangeEmigration(l_iCountry, l_pUpdate->m_bEmigrationClosed);
      
      if(l_pUpdate->m_bImmigrationClosed != l_pData->ImmigrationClosed())
         g_ServerDCL.ChangeImmigration(l_iCountry, l_pUpdate->m_bImmigrationClosed);

      vector<GPoliticalParty> l_vParties = l_pData->PoliticalParty();

      for(UINT32 i=0; i<l_pUpdate->m_viType.size(); i++)
      {
         switch(l_pUpdate->m_viType[i])
         {
         case 1:
            g_ServerDCL.ChangeLanguageStatus(l_iCountry, l_pUpdate->m_viId[i], l_pData->LanguageGetStatus(l_pUpdate->m_viId[i]), l_pUpdate->m_viStatus[i]);
            break;

         case 2:
            g_ServerDCL.ChangeReligionStatus(l_iCountry, l_pUpdate->m_viId[i], l_pData->ReligionGetStatus(l_pUpdate->m_viId[i]), l_pUpdate->m_viStatus[i]);
            break;

         case 3:
            g_ServerDCL.ChangePoliticalPartyStatus(l_iCountry, l_pUpdate->m_viId[i], l_pData->PoliticalPartyGetStatus(l_pUpdate->m_viId[i]), l_pUpdate->m_viStatus[i]);
            break;
         }
      }

      for(UINT32 i=0; i<EInternalLaws::ItemCount; i++)
      {
         g_ServerDCL.ChangeCountryInternalLaw(l_iCountry, (EInternalLaws::Enum)i, l_pUpdate->m_vbLaws[i]); 
      }
      // this event need an acknowledgement of receipt
      g_ServerDCL.AcknowledgeRequest(l_pUpdate->m_iRequestID, l_pUpdate->m_iSource);
   }

   return true;   
}

/*!
 * Handle client politic window data update request
 */
bool SP2::GPoliticEventHandler::HandlePoliticWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GPoliticUpdate* l_pUpdate = (SP2::Event::GPoliticUpdate*)in_pEvent.get();

   l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
   l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

   GCountryData* l_pData = g_ServerDAL.CountryData(l_pUpdate->CountryId);

   l_pUpdate->GvtType            = l_pData->GvtType();
   l_pUpdate->PoliticalIdeology  = l_pData->CountryPoliticalIdeology();
   l_pUpdate->GvtApproval        = l_pData->GvtApproval();
   l_pUpdate->GvtStability       = l_pData->GvtStability();
   l_pUpdate->CorruptionLevel    = l_pData->CorruptionLevel();
	l_pUpdate->Pressure				= l_pData->Pressure();
   l_pUpdate->MartialLaw         = l_pData->MartialLaw();

   g_Joshua.RaiseEvent(in_pEvent);

   return true;
}

bool SP2::GPoliticEventHandler::HandleTreatyAdd(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GTreatyDetailsTreatyAdd* l_pTreatyEvt = (SP2::Event::GTreatyDetailsTreatyAdd*)in_Event.get();

   if(!g_Joshua.ActivePlayer(l_pTreatyEvt->m_iSource) )
      return false;

   l_pTreatyEvt->m_Treaty;
   INT32 l_iCountryID = g_Joshua.ActivePlayer(l_pTreatyEvt->m_iSource)->ModID();
   GString l_sTreatyName = l_pTreatyEvt->m_Treaty.Name();
   g_ServerDCL.CreateNewTreaty(l_iCountryID,
                               l_pTreatyEvt->m_Treaty.MembersSideA(false),
                               l_pTreatyEvt->m_Treaty.MembersSideB(false),
                               l_pTreatyEvt->m_Treaty.MembersPressure(),
                               l_pTreatyEvt->m_Treaty.Type(),
                               l_pTreatyEvt->m_Treaty.Private(),
                               l_sTreatyName,
										 l_pTreatyEvt->m_Treaty.Conditions());

   return true;
}


