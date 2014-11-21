/**************************************************************
*
* golem_thematic_map_legend_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_thematic_map_legend_window.h"
#include "../include/sp2_background_earth.h"
const GString GThematicMapLegendWindow::TypeName(L"Thematic_Map_Legend");


const GString c_sThematicLegendImage[EThematicMapLegendType::Count] = 
   {
      L"000844",
      L"000843",
      L"000846"		
   };


GUI::GBaseObject* GThematicMapLegendWindow::New()
{
	return (GUI::GBaseObject*) new GThematicMapLegendWindow();
}


GThematicMapLegendWindow::GThematicMapLegendWindow() : GUI::GFrame()
{
   m_eLegendType                    = EThematicMapLegendType::Unknown;
   m_eCharacteristicToHighlight     = ECharacteristicToHighlight::Geographic;
}

GThematicMapLegendWindow::~GThematicMapLegendWindow()
{
}


/*!
 * Destroy handler
 **/
bool GThematicMapLegendWindow::OnDestroy()
{
   //We do not want to be notified anymore when the selected country has changed
   g_ClientDCL.Detach(this,c_NotificationOnSelectedCountryChange);

   return __super::OnDestroy();


}

/*!
 * OnCreate handler
 **/
bool GThematicMapLegendWindow::OnCreate()
{
   __super::OnCreate();

   //Fetch pointer to the objects
   {
      m_pObjCountryList    = (GUI::GListBox*)   Child(L"lstCountries");
      m_pObjGradientPicBox = (GUI::GPictureBox*)Child(L"frmLegend")->Child(L"picGradient");
      m_pObjTopLbl         = (GUI::GLabel*)     Child(L"frmLegend")->Child(L"lblTop");
      m_pObjMiddleLbl      = (GUI::GLabel*)     Child(L"frmLegend")->Child(L"lblMiddle");
      m_pObjBottomLbl      = (GUI::GLabel*)     Child(L"frmLegend")->Child(L"lblBottom");
      m_pObjTitleLbl       = (GUI::GLabel*)     Child(L"txtTitle");
   }

   //Fill the listbox with the countries
   FillListBoxWithCountries(g_ClientDAL.Countries());

   //We want to be notified when the selected country has changed
   g_ClientDCL.Attach(this,c_NotificationOnSelectedCountryChange);

   //Position the window to the top right of the screen
   Left(g_ClientDDL.BackgroundEarth()->Width()-Width()-2);
   Top(40);

   return true;
}


/*!
 * Updates the thematic map legend window, depending on the legend type, 
 * and if the results are by country or by region (results used to create a ranking)
 * @param in_eLegendType: Type of legend to display
 * @param in_vResults:    Results to created a country ranking with (either 1 result by region or 1 by country, see next param)
 * @param in_bResultsByCountry: true if 1 result (in in_vResults) by country, false if they are by region
 **/
void GThematicMapLegendWindow::Update(EThematicMapLegendType::Enum      in_eLegendType,
                                      ECharacteristicToHighlight::Enum  in_eCharacteristic,
                                      vector<REAL64>&                   in_vResults)
{
   gassert(in_eLegendType != EThematicMapLegendType::Unknown,"GThematicMapLegendWindow::Update Invalid thematic legend type to display");

   g_ClientDAL.FetchCountriesRelations();

   //If we had the empire legend type displayed, we must refill the country list
   if(m_eLegendType == EThematicMapLegendType::Empire && in_eLegendType != EThematicMapLegendType::Empire)
   {
      FillListBoxWithCountries(g_ClientDAL.Countries());
   }
	//If we had the empire legend type displayed, we must refill the country list
   else if(m_eLegendType == EThematicMapLegendType::HumanControlled && in_eLegendType != EThematicMapLegendType::HumanControlled)
   {
      FillListBoxWithCountries(g_ClientDAL.Countries());
   }

   m_eLegendType                = in_eLegendType;
   m_eCharacteristicToHighlight = in_eCharacteristic;

	if(m_eCharacteristicToHighlight == ECharacteristicToHighlight::HumanControlledCountries)
	{
		vector<GCountry> l_CountriesToDisplay;
		for(UINT32 i = 0 ; i < in_vResults.size() ; i++)
      {
			if(in_vResults[i] > 0.f)
			{
				l_CountriesToDisplay.push_back(g_ClientDAL.Country(i+1));
			}
		}
		FillListBoxWithCountries(l_CountriesToDisplay);
	}
   else if(in_eLegendType != EThematicMapLegendType::Empire)
   {
      //Create the country ranking
      CreateCountryRanking(in_eCharacteristic,in_vResults);
      //Sort the entries by rank
      SortByRanking();
   }
   else
   {
      //In the empire only put the allied countries and the selected country in the listbox
      //results are by region
      set<UINT32> l_AlliedCountries;

      //List the allied countries cause they are the only one (with the selected country)
      // that will appear in the country list
      for(UINT32 i = 0 ; i < in_vResults.size() ; i++)
      {
         l_AlliedCountries.insert((UINT32)in_vResults[i]);
      }
      
      vector<GCountry> l_CountriesToDisplay;
      //Add the selected country to the list
      l_AlliedCountries.insert(g_ClientDCL.SelectedCountryID());
      //Add all the allies to the list
      for(set<UINT32>::iterator l_It = l_AlliedCountries.begin();
          l_It != l_AlliedCountries.end();
          l_It++)
      {
         l_CountriesToDisplay.push_back(g_ClientDAL.Country(*l_It));
      }
      FillListBoxWithCountries(l_CountriesToDisplay);
   }

   //Update the actually selected country in the list
   UpdateSelectedCountryInList();
   //Update the legend and the labels on the right
   UpdateLegendImageAndLabels();
   //Update the title bar
   UpdateTitle();
   //Update rankings tags
   UpdateRankTag();

   return;
}

/*!
 * Create the country ranking
 **/
void GThematicMapLegendWindow::CreateCountryRanking(ECharacteristicToHighlight::Enum in_eCharacteristic,
                                                    vector<REAL64>&                  in_vResults)
{
   //Prepare the results so they can be sorted
   //hash_map<UINT32,REAL64> l_ValueByCountry;


   gassert(in_vResults.size() == g_ClientDAL.NbCountry(),"GThematicMapLegendWindow::CreateCountryRanking Incoherence, not enough results (country)");

   //At this point, we now have all the information to perform a country ranking
   //Perform a binary heap sort and fill the country ranks vector
   {
      //Compute the average and perform a heap sort
      GBinaryHeap<REAL64,UINT32> l_Heap(in_vResults.size(),EOrderMethod::OrderDecreasing);

      for(UINT32 i = 0; i < in_vResults.size() ; i++)
      {
         if(g_ClientDAL.Country(i + 1).IsActive() )
         {
            l_Heap.AddNode(in_vResults[i],i+1);
         }
      }

      REAL64 l_fValue;
      UINT32 l_iCountry;
      m_vCountryRanks.resize(g_ClientDAL.NbCountry(),0);
      gassert(m_vCountryRanks.size() == g_ClientDAL.NbCountry(),"GThematicMapLegendWindow::CreateCountryRanking Lists should be of same size");
      UINT32 k = 0;
      //The structure of the vector is [countryid-1] = rank
      while(l_Heap.PopTopNode(l_fValue,l_iCountry))
      {
         m_vCountryRanks[l_iCountry-1] = k++;
      }
   }
   return;
}


void GThematicMapLegendWindow::SortByRanking()
{
   UINT32 l_iNbItems = m_pObjCountryList->Get_Nb_Rows();

   UINT32 l_iLow = 0;
   UINT32 l_iHigh = l_iNbItems - 1;

   // Variation of bubble sort
   while(l_iHigh > l_iLow)
   {
      // Find the maximum element in the current sub-array
      UINT32 l_iMax = l_iLow;
      SDK::GUI::GListBox::GRow* l_pRow = m_pObjCountryList->Get_Row_At(l_iMax);
      UINT32 l_iMaxRank = m_vCountryRanks[l_pRow->m_iAdditionalId - 1];

      for(UINT32 p = l_iLow + 1; p <= l_iHigh;p ++)
      {
         l_pRow = m_pObjCountryList->Get_Row_At(p);
         UINT32 l_iCompareRank = m_vCountryRanks[l_pRow->m_iAdditionalId - 1];
         if(l_iCompareRank > l_iMaxRank)
         {
            l_iMax = p;
            l_iMaxRank = l_iCompareRank;
         }

      }

      // Swap last  element with higher element value
      m_pObjCountryList->SwapRows(l_iHigh, l_iMax);

      // Next iteration will iterate in sub-array without last element
      l_iHigh --;
   }
	//Run through the arrow and change the relation label
	for(UINT32 i=0; i < l_iNbItems; i++)
	{
		SDK::GUI::GListBox::GRow* l_pRow = m_pObjCountryList->Get_Row_At(i);

		//Set the relations status
		{
			GUI::GLabel* l_plblRelations = (GUI::GLabel*)l_pRow->m_pObject->Child(L"frmCountry")->Child(L"lblRelations");				
			UINT32 l_iCountry = l_pRow->m_iAdditionalId;
			if(l_iCountry != (UINT32)g_ClientDAL.ControlledCountryID())
			{
				REAL32 l_fRelations = g_ClientDAL.RelationWithCountry(l_iCountry);
				l_plblRelations->Text( GString::FormatNumber( l_fRelations,0) );
				REAL32 l_fR = 25.f;
				REAL32 l_fG = 25.f;
				REAL32 l_fB = 25.f;
				if(l_fRelations > 0.f)
				{
					l_fG = 240.f;
					l_fR = 240.f - ((l_fRelations/100.f) * 215.f);
				}
				else
				{
					l_fG = 240.f - ((-l_fRelations/100.f) * 215.f);
					l_fR = 240.f;
				}
				l_plblRelations->FontColor(GColorRGBInt((UINT8)l_fR,(UINT8)l_fG,(UINT8)l_fB,255));
			}
			else
				l_plblRelations->Text(L"");
		}

	}
}

void GThematicMapLegendWindow::UpdateSelectedCountryInList()
{
   UINT32 l_iActuallySelectedCountry = g_ClientDCL.SelectedCountryID();
   bool l_bMustUpdate = false;
   //Find the row that represent that country
   for(UINT32 i = 0 ; i < (UINT32)m_pObjCountryList->Get_Nb_Rows() ; i++)
   {
      SDK::GUI::GListBox::GRow* l_pRow = m_pObjCountryList->Get_Row_At(i);
      if( l_pRow->m_iAdditionalId == l_iActuallySelectedCountry)
      {
         vector<INT32> l_vItemToSelect;
         l_vItemToSelect.push_back(l_pRow->m_iId);
         m_pObjCountryList->Select_Items(l_vItemToSelect);
         m_pObjCountryList->ScrollTo(max(0,(INT32)i-4));
         l_bMustUpdate = true;
         break;
      }
   }
   if(l_bMustUpdate)
      m_pObjCountryList->Update();
   return;
}

void GThematicMapLegendWindow::UpdateLegendImageAndLabels()
{
   gassert(m_eLegendType != EThematicMapLegendType::Unknown,"GThematicMapLegendWindow::UpdateLegendImageAndLabels, unknown legend type");
	if(m_eLegendType != EThematicMapLegendType::HumanControlled)
	{
		GString l_sLegendImage = c_sThematicLegendImage[m_eLegendType];
		m_pObjGradientPicBox->PictureNormal(l_sLegendImage);	
		m_pObjGradientPicBox->Show();
	}
	else
	{
		m_pObjGradientPicBox->Hide(); 
	}

   //Set the labels
   switch(m_eLegendType)
   {
      case EThematicMapLegendType::Empire:
      {
         m_pObjTopLbl->Text(g_ClientDAL.GetString(EStrId::Ally));
         m_pObjMiddleLbl->Text(g_ClientDAL.GetString(EStrId::Selected));
         m_pObjBottomLbl->Text(g_ClientDAL.GetString(EStrId::Enemy));
         break;
      }
      case EThematicMapLegendType::GoodBad:
      {
         m_pObjTopLbl->Text(g_ClientDAL.GetString(EStrId::Good));
         m_pObjMiddleLbl->Text(L"");
         m_pObjBottomLbl->Text(g_ClientDAL.GetString(EStrId::Bad));
         break;
      }
      case EThematicMapLegendType::HighLow:
      {
         m_pObjTopLbl->Text(g_ClientDAL.GetString(EStrId::High));
         m_pObjMiddleLbl->Text(L"");
         m_pObjBottomLbl->Text(g_ClientDAL.GetString(EStrId::Low));
         break;
      }
		case EThematicMapLegendType::HumanControlled:
		{
			m_pObjTopLbl->Text(L"");
         m_pObjMiddleLbl->Text(L"");
         m_pObjBottomLbl->Text(L"");
         break;
		}
   }

}

void GThematicMapLegendWindow::UpdateTitle()
{
   //Set the title as the thematic map currently being updated
   GString l_sTitleText;
   switch(m_eCharacteristicToHighlight)
   {
      case ECharacteristicToHighlight::Geographic:
         l_sTitleText = g_ClientDAL.GetString(101108);
         break;
      case ECharacteristicToHighlight::CountryEconomicalStrength:
         l_sTitleText = g_ClientDAL.GetString(101109);
         break;
      case ECharacteristicToHighlight::CountryEmpire:
         l_sTitleText = g_ClientDAL.GetString(101110);
         break;
		case ECharacteristicToHighlight::PoliticalControl:
         l_sTitleText = g_ClientDAL.GetString(101661);
         break;
      case ECharacteristicToHighlight::CountryRelations:
         l_sTitleText = g_ClientDAL.GetString(101111);
         break;
      case ECharacteristicToHighlight::CountryExceedingResources:
         l_sTitleText = g_ClientDAL.GetString(101112);
         break;
      case ECharacteristicToHighlight::CountryGrossDomesticProduct:
         l_sTitleText = g_ClientDAL.GetString(101113);
         break;
      case ECharacteristicToHighlight::CountryGrossDomesticProductGrowth:
         l_sTitleText = g_ClientDAL.GetString(101114);
         break;
      case ECharacteristicToHighlight::CountryHumanDevelopment:
         l_sTitleText = g_ClientDAL.GetString(101115);
         break;
      case ECharacteristicToHighlight::CountryMilitaryStrength:
         l_sTitleText = g_ClientDAL.GetString(101116);
         break;
      case ECharacteristicToHighlight::CountryNuclearStrength:
         l_sTitleText = g_ClientDAL.GetString(101117);
         break;
      case ECharacteristicToHighlight::CountryPoliticalStrength:
         l_sTitleText = g_ClientDAL.GetString(101118);
         break;
      case ECharacteristicToHighlight::CountryPopulationGrowth:
         l_sTitleText = g_ClientDAL.GetString(101119);
         break;
      case ECharacteristicToHighlight::CountryPopulationStabilty:
         l_sTitleText = g_ClientDAL.GetString(101120);
         break;
      case ECharacteristicToHighlight::CountryPopulationSupport:
         l_sTitleText = g_ClientDAL.GetString(101121);
         break;
      case ECharacteristicToHighlight::HumanControlledCountries:
         l_sTitleText = g_ClientDAL.GetString(101122);
         break;
      case ECharacteristicToHighlight::RegionPopulationDensity:
         l_sTitleText = g_ClientDAL.GetString(101123);
         break;
      case ECharacteristicToHighlight::RegionPopulationTotal:      
         l_sTitleText = g_ClientDAL.GetString(101124);
         break;
      case ECharacteristicToHighlight::RegionResourcesProduction:
         l_sTitleText = g_ClientDAL.GetString(101125);
         break;
      //Specific resource production
      default:
      {
         gassert(m_eCharacteristicToHighlight > ECharacteristicToHighlight::RegionResourcesProduction,"GThematicMapLegendWindow::Update Unknown thematic maps are supposed to be resources thematic maps");
         //Preparing the legend name
         l_sTitleText = g_ClientDAL.GetString(101477);
         EResources::Enum l_eResource = (EResources::Enum)((INT32)m_eCharacteristicToHighlight-(INT32)ECharacteristicToHighlight::RegionResourcesProduction-1);
         l_sTitleText += g_ClientDAL.GetString(g_ClientDAL.StringIdResource(l_eResource));
         break;
      }
   }
   m_pObjTitleLbl->Text(l_sTitleText);
}

/*!
 * Observer method
 **/
void GThematicMapLegendWindow::OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                                               const Hector::DesignPattern::GSubjectNotification& in_Notification,
                                               void*)
{
   if(&in_Subject == &g_ClientDCL)
   {
      if(in_Notification == c_NotificationOnSelectedCountryChange)
      {
         UpdateSelectedCountryInList();
      }
   }
}

void GThematicMapLegendWindow::FillListBoxWithCountries(GCountries& in_vCountries)
{
   //Clear the already existing items
   m_pObjCountryList->Clear();
	GString l_sToolTipRelations = g_ClientDAL.GetString(101111);

   //Fill the listbox with the countries
   for(UINT32 i = 0 ; i < in_vCountries.size() ; i++)
   {
      if(in_vCountries[i].IsActive() )
      {
         //Set the row additional id as the country id
         GUI::GListBox::GRow* l_pRow = m_pObjCountryList->Insert_New_Row(in_vCountries[i].Id());
         //Set the flag picture
         {
            GUI::GPictureBox* l_pFlag = (GUI::GPictureBox*)l_pRow->m_pObject->Child(L"frmCountry")->Child(L"picFlag");
            l_pFlag->PictureNormal(g_ClientDDL.SmallFlagsTextures()[in_vCountries[i].Id()]);
         }
         //Set the country name
         {
            GUI::GLabel* l_pTxtCode = (GUI::GLabel*)l_pRow->m_pObject->Child(L"frmCountry")->Child(L"txtCode");				
            l_pTxtCode->Text(in_vCountries[i].Code());
         }	
			//Set the relations status
			{
				GUI::GLabel* l_plblRelations = (GUI::GLabel*)l_pRow->m_pObject->Child(L"frmCountry")->Child(L"lblRelations");				
				if(in_vCountries[i].Id() != g_ClientDAL.ControlledCountryID())
				{
					REAL32 l_fRelations = g_ClientDAL.RelationWithCountry(in_vCountries[i].Id());
					l_plblRelations->Text( GString::FormatNumber( l_fRelations,0) );
					REAL32 l_fR = 25.f;
					REAL32 l_fG = 25.f;
					REAL32 l_fB = 25.f;
					if(l_fRelations > 0.f)
					{
						l_fG = 240.f;
						l_fR = 240.f - ((l_fRelations/100.f) * 215.f);
					}
					else
					{
						l_fG = 240.f - ((-l_fRelations/100.f) * 215.f);
						l_fR = 240.f;
					}
					l_plblRelations->FontColor(GColorRGBInt((UINT8)l_fR,(UINT8)l_fG,(UINT8)l_fB,255));
				}
				else
					l_plblRelations->Text(L"");
				l_plblRelations->TooltipText(l_sToolTipRelations);
			}

         l_pRow->Object()->TooltipText( in_vCountries[i].Name() );
      }
   }
   m_pObjCountryList->Update();
}

void GThematicMapLegendWindow::UpdateRankTag()
{
   for(UINT32 i = 0 ; i < (UINT32)m_pObjCountryList->Get_Nb_Rows()  ; i++)
   {
      GUI::GListBox::GRow* l_pRow = m_pObjCountryList->Get_Row_At(i);
      //Write the rank into the row label
      GUI::GLabel* l_pLabel = (GUI::GLabel*)l_pRow->m_pObject->Child(L"txtRank");
      l_pLabel->Text(GString(i+1));
      l_pLabel->TextOffset(4,0);
   }
}

EEventStatus::Enum GThematicMapLegendWindow::OnCustomEvent(UINT32 in_iEventID,
                                                           const SDK::GUI::GEventData& in_EventData,
                                                           SDK::GUI::GBaseObject* in_pCaller)
{
   //If a new item was selected in the list
   if(in_iEventID == ECustomEvents::OnListItemSelected)
   {      
      //Find the selected row
      const vector<INT32>& l_vSelectedRows = m_pObjCountryList->Get_Selected_Item();
      gassert(l_vSelectedRows.size() > 0, "GThematicMapLegendWindow::OnCustomEvent Received an event to select an item but no item selected");
      GListBox::GRow* l_pRow = m_pObjCountryList->Get_Row_At(l_vSelectedRows[0]);
      UINT32 l_iCountryID = l_pRow->m_iAdditionalId;
      g_ClientDCL.SelectedCountrySetLookAt(g_ClientDAL.Country(l_iCountryID));
   }
   return EEventStatus::Handled;
}

EEventStatus::Enum GThematicMapLegendWindow::OnMouseClick(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller)
{
   return EEventStatus::Handled;
}

EEventStatus::Enum GThematicMapLegendWindow::OnMouseUp(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller)
{
   return EEventStatus::Handled;
}

EEventStatus::Enum GThematicMapLegendWindow::OnMouseDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller)
{
   return EEventStatus::Handled;
}