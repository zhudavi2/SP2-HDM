/**************************************************************
*
* sp2_event_log_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_event_log_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_text_overlay.h"

const GString GEventLogWindow::TypeName(L"Event_Log");

GUI::GBaseObject* GEventLogWindow::New()
{
	return (GUI::GBaseObject*) new GEventLogWindow();
}


GEventLogWindow::GEventLogWindow() : GUI::GFrame()
{
   m_iCounter = 0;
   m_bDragging = false;
}

GEventLogWindow::~GEventLogWindow()
{
}

bool GEventLogWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjEventsLst         = (GUI::GListBox*)Child(L"frmInner")->Child(L"lstEvents");
   m_pObjEventsLst->ItemColorNormal( GColorRGBInt(0,0,0,0) );

   m_pObjCategoryToggle[ESphere::Politic]        = (GUI::GToggle*)Child(L"frmInner")->Child(L"togFilterPolitic");
   m_pObjCategoryToggle[ESphere::Military]       = (GUI::GToggle*)Child(L"frmInner")->Child(L"togFilterMilitary");
   m_pObjCategoryToggle[ESphere::Economic]       = (GUI::GToggle*)Child(L"frmInner")->Child(L"togFilterEconomic");
   m_pObjCategoryToggle[ESphere::Demographic]    = (GUI::GToggle*)Child(L"frmInner")->Child(L"togFilterDemographic");

   m_pObjGotoBtn           = (GUI::GButton*)Child(L"frmInner")->Child(L"btnGoto");

   Position( g_ClientDDL.MainBar()->Left() + 190, g_ClientDDL.MainBar()->Top() - g_ClientDDL.MainBar()->Height() - Height() + 10);

	return true;
}

bool GEventLogWindow::OnDestroy()
{
   return __super::OnDestroy();
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GEventLogWindow::OnShow()
{
   __super::OnShow();

   BringToFront();  

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

void GEventLogWindow::OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative)
{
   __super::OnDrag(in_EventData, in_MouseOffset, in_MouseRelative);

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   
   if((Top() + Height()) > mode.m_Resolution.y)
   {
      Top( mode.m_Resolution.y - Height() );
   }
}

GUI::EEventStatus::Enum GEventLogWindow::OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller)
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

GUI::EEventStatus::Enum GEventLogWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{  
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //Check if the caller is one of the toggle
         if((in_pCaller == m_pObjCategoryToggle[ESphere::Politic])       || 
            (in_pCaller == m_pObjCategoryToggle[ESphere::Military])      ||
            (in_pCaller == m_pObjCategoryToggle[ESphere::Economic])      ||
            (in_pCaller == m_pObjCategoryToggle[ESphere::Demographic])
         )
         {
            Update();
         }
         else if(in_pCaller == m_pObjGotoBtn)
         {
            GVector2D<REAL32> l_NewsPosition;

            vector<INT32> l_viItem = m_pObjEventsLst->Get_Selected_Item();
            
            if(!l_viItem.size())
               return GUI::EEventStatus::Handled;
            
            vector< pair<UINT32,UINT32> >::iterator l_It;
            for(l_It = m_ShownEvents.begin(); l_It != m_ShownEvents.end(); l_It++)
            {
               if((UINT32)l_viItem[0] == (*l_It).first)
                  break;
            }
               
            if(l_It == m_ShownEvents.end())
               return GUI::EEventStatus::Handled;

            map<UINT32, SEventLogData>::iterator l_DemographicIt = m_Events[ESphere::Demographic].find((*l_It).second);
            map<UINT32, SEventLogData>::iterator l_EconomicIt    = m_Events[ESphere::Economic]   .find((*l_It).second);
            map<UINT32, SEventLogData>::iterator l_PoliticIt     = m_Events[ESphere::Politic]    .find((*l_It).second);
            map<UINT32, SEventLogData>::iterator l_MilitaryIt    = m_Events[ESphere::Military]   .find((*l_It).second);


            if(l_DemographicIt != m_Events[ESphere::Demographic].end())
            {
               l_NewsPosition = l_DemographicIt->second.m_Position;
            }
            else if(l_EconomicIt != m_Events[ESphere::Economic].end())
            {
               l_NewsPosition = l_EconomicIt->second.m_Position;
            }
            else if(l_PoliticIt != m_Events[ESphere::Politic].end())
            {
               l_NewsPosition = l_PoliticIt->second.m_Position;
            }
            else if(l_MilitaryIt != m_Events[ESphere::Military].end())
            {
               l_NewsPosition = l_MilitaryIt->second.m_Position;
            }

            g_ClientDDL.BackgroundEarth()->CreateCameraAnimation( &l_NewsPosition );
            g_ClientDDL.BackgroundEarth()->PlayCameraAnimation();      
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GEventLogWindow::OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   INT32 x = in_EventData.X();
   INT32 y = in_EventData.Y();
   
   if( (x >= m_pObjEventsLst->Left()) && (x <= (m_pObjEventsLst->Left() + m_pObjEventsLst->Width())) && (y >= m_pObjEventsLst->Top()) && (y <= (m_pObjEventsLst->Top() + m_pObjEventsLst->Height())) )
   {
      DisplayOverlay();
   }

   return GUI::EEventStatus::Handled;
}

void GEventLogWindow::DisplayOverlay()
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
   if( (x >= m_pObjEventsLst->Left()) && (x <= (m_pObjEventsLst->Left() + m_pObjEventsLst->Width())) && (y >= m_pObjEventsLst->Top()) && (y <= (m_pObjEventsLst->Top() + m_pObjEventsLst->Height())) )
   {
      y = y - m_pObjEventsLst->Top();

      INT32 l_iOverRowID = (INT32)floor( (REAL32)y / (REAL32)m_pObjEventsLst->RowHeight() );
      GUI::GListBox::GRow* l_pRow = m_pObjEventsLst->Get_Row_At(l_iOverRowID + m_pObjEventsLst->FirstRowShown());
      if(l_pRow)
      {
         GUI::GLabel* l_pEvent = (GUI::GLabel*)l_pRow->Object()->Child(L"txtEvent");
         
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

void GEventLogWindow::HideOverlay()
{
   g_ClientDDL.TextOverlay()->Hide();
}

EEventStatus::Enum GEventLogWindow::OnMouseOut(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   //if(in_pCaller == m_pObjOverlayLbl)
   //{
   //   DisplayOverlay();
   //}
   DisplayOverlay();

   return EEventStatus::Handled;
}

EEventStatus::Enum GEventLogWindow::OnMouseLeave(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(!in_pCaller)
      HideOverlay();

   return GUI::EEventStatus::Handled;
}

bool GEventLogWindow::OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller)
{
   BringToFront();  

   if(g_ClientDDL.MainBar() && g_ClientDDL.MainBar()->Visible())
      g_ClientDDL.MainBar()->BringToFront();

   if(g_ClientDDL.MiniMap() && g_ClientDDL.MiniMap()->Visible())      
      g_ClientDDL.MiniMap()->BringToFront();
   
   return false;
}

void GEventLogWindow::AddEvent(GString in_sEvent, SP2::ESphere::Enum in_eType, GVector2D<REAL32> in_Position, UINT32 in_iCountryA, UINT32 in_iCountryB)
{
   //Fill the filters array
   bool l_bFilters[ESphere::Count];
   {
      l_bFilters[ESphere::Politic]     = m_pObjCategoryToggle[ESphere::Politic]     ->ToggleState();
      l_bFilters[ESphere::Economic]    = m_pObjCategoryToggle[ESphere::Economic]    ->ToggleState();
      l_bFilters[ESphere::Military]    = m_pObjCategoryToggle[ESphere::Military]    ->ToggleState();
      l_bFilters[ESphere::Demographic] = m_pObjCategoryToggle[ESphere::Demographic] ->ToggleState();
   }

   bool l_bToDisplay = false;

   SEventLogData       l_Data;
   l_Data.m_sMail      = in_sEvent;
   l_Data.m_Position   = in_Position;
   l_Data.m_eType      = in_eType;
   l_Data.m_iCountryA  = in_iCountryA;
	l_Data.m_iCountryB  = in_iCountryB;   

   //If there are too many news present, must remove one.
   if(m_Events[l_Data.m_eType].size()  >= c_iNewsMax)
   {
      //Remove the oldest event of the given type, which is the 1st in the list
      map<UINT32, SEventLogData>::iterator l_OldestEventIt = m_Events[l_Data.m_eType].begin();
      
      //Check if the oldest event is in the shown list
      for(vector< pair<UINT32, UINT32> >::iterator l_ShownEventsIt = m_ShownEvents.begin(); 
          l_ShownEventsIt != m_ShownEvents.end(); 
          l_ShownEventsIt++)
      {
         //If the oldest event is shown, remove it
         if((*l_ShownEventsIt).second == l_OldestEventIt->first)
         {
            UINT32 l_iTempID = (*l_ShownEventsIt).first;
            
            m_pObjEventsLst->RemoveRow( (*l_ShownEventsIt).first );
            m_ShownEvents.erase(l_ShownEventsIt);

            //Reset the event ids correctly (decrementing the number by 1)
            for(vector< pair<UINT32, UINT32> >::iterator l_ShownEvents2It = m_ShownEvents.begin(); 
                l_ShownEvents2It != m_ShownEvents.end(); 
                l_ShownEvents2It++)
            {
               if( l_iTempID < (*l_ShownEvents2It).first )
                  (*l_ShownEvents2It).first--;
            }
            break;
         }
      }
      //Remove the oldest event from the list
      m_Events[l_Data.m_eType].erase(l_OldestEventIt);
   }
   //Insert the new event
   m_Events[l_Data.m_eType].insert( make_pair( m_iCounter, l_Data ) ); 
   if(l_bFilters[l_Data.m_eType])
      l_bToDisplay = true;

   if(!l_bFilters[ESphere::Economic] && !l_bFilters[ESphere::Politic]&& !l_bFilters[ESphere::Military] && !l_bFilters[ESphere::Demographic])
      l_bToDisplay = true;

   if(l_bToDisplay)
   {
      AddItem(l_Data);
      map<UINT32, SEventLogData>::reverse_iterator l_It = m_Events[in_eType].rbegin();
      m_ShownEvents.push_back( make_pair( m_pObjEventsLst->Get_Nb_Rows() - 1, l_It->first) );
      
      if(Visible())
         DisplayOverlay();
   }
   m_pObjEventsLst->Update();
   m_iCounter++;
}

void GEventLogWindow::AddItem(SEventLogData in_Data)
{
   GUI::GListBox::GRow* l_pRow = m_pObjEventsLst->Insert_New_Row();
   
   GUI::GPictureBox* l_pPic			= (GUI::GPictureBox*)l_pRow->Object()->Child(L"picIcon");
   GUI::GLabel*      l_pLabel			= (GUI::GLabel*)l_pRow->Object()->Child(L"txtEvent");
   GUI::GLabel*      l_pCode			= (GUI::GLabel*)l_pRow->Object()->Child(L"frmCountry")->Child(L"txtCode");
   GUI::GPictureBox* l_pFlag			= (GUI::GPictureBox*)l_pRow->Object()->Child(L"frmCountry")->Child(L"picFlag");
	//GUI::GFrame*		l_pSecondFlag	= (GUI::GFrame*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"frmCountrySmall", L"frmSecondFlag", l_pRow->Object(), false, false);
	GString				l_sSecondString = L"";
	

   gassert(in_Data.m_iCountryA,"Should always have a country on side A");
	if(in_Data.m_iCountryA)
   {
      const GCountry & l_Country = g_ClientDAL.Country(in_Data.m_iCountryA);
      l_pCode->Text( l_Country.Code() );

      l_pFlag->PictureBackground(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);		
      l_pRow->Object()->Child(L"frmCountry")->TooltipText( l_Country.Name() );
   }

	
	if(in_Data.m_iCountryB)
	{
		const GCountry & l_Country = g_ClientDAL.Country(in_Data.m_iCountryB);
		l_sSecondString = L" " + l_Country.Name();
		/*
		
		((GUI::GPictureBox*)l_pSecondFlag->Child(L"picFlag"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
		((GUI::GLabel*)l_pSecondFlag->Child(L"txtCode"))->Text( l_Country.Code() );

      l_pSecondFlag->TooltipText( l_Country.Name() );
		*/
	}
	

   switch(in_Data.m_eType)
   {
   case ESphere::Demographic:
      l_pPic->PictureNormal(GString(L"000834"));
      l_pPic->PictureBackground( l_pPic->PictureNormal() );
      l_pLabel->FontColorNormal(GColorRGBInt(115, 116, 190, 255));
      break;
   case ESphere::Economic:
      l_pPic->PictureNormal(GString(L"000835"));
      l_pPic->PictureBackground( l_pPic->PictureNormal() );
      l_pLabel->FontColorNormal(GColorRGBInt(115, 190, 122, 255));
      break;
   case ESphere::Politic:
      l_pPic->PictureNormal(GString(L"000837"));
      l_pPic->PictureBackground( l_pPic->PictureNormal() );
      l_pLabel->FontColorNormal(GColorRGBInt(200, 201, 127, 255));
      break;
   case ESphere::Military:
      l_pPic->PictureNormal(GString(L"000836"));
      l_pPic->PictureBackground( l_pPic->PictureNormal() );
      l_pLabel->FontColorNormal(GColorRGBInt(190, 115, 115, 255));
      break;
   }

   l_pLabel->Text(in_Data.m_sMail + l_sSecondString);
	/*
	l_pSecondFlag->Left(l_pLabel->Left() + l_pLabel->Text2D()->Width() + 7);
	l_pSecondFlag->Top(l_pFlag->Top() - 2);
	l_pSecondFlag->Show();
	l_pSecondFlag->BringToFront();
	*/

   m_pObjEventsLst->Update();
   UINT32 l_iLastRowShown = (UINT32)m_pObjEventsLst->FirstRowShown() + m_pObjEventsLst->NbVisibleObjects();
   
   if( l_iLastRowShown == (UINT32)m_pObjEventsLst->Get_Nb_Rows() )
      m_pObjEventsLst->ScrollTo( m_pObjEventsLst->Get_Nb_Rows() - 1 );
}

bool GEventLogWindow::Update()
{
   //Clear the already existing lists
   {
      m_ShownEvents.clear();
      m_pObjEventsLst->Clear();
   }

   bool l_bMustSkipCategory   [ESphere::Count];

   //If all the buttons toggle state is off, it is as if they were all pressed
   bool l_bAllToggledOff = true;


   //Compute which categories must be skipped and which must be processed
   for(UINT32 i = ESphere::Economic ; i < ESphere::Count ; i++)
   {
      //If category is not toggled in or there are no events in it, skip the category
      if(!m_pObjCategoryToggle[i]->ToggleState())
         l_bMustSkipCategory[i]  = true;
      else
      {
         l_bMustSkipCategory[i]  = false;      
         l_bAllToggledOff = false;
      }

      //If all the buttons toggle state is off, it is as if they were all pressed
/*
      if(m_pObjCategoryToggle[i]->ToggleState())
      {
         l_bAllToggledOff = false;
      }
      //If category is not toggled in or there are no events in it, skip the category
      if(!m_pObjCategoryToggle[i]->ToggleState() || !m_Events[i].size())
         l_bMustSkipCategory[i]  = true;
      else
         l_bMustSkipCategory[i]  = false;      
*/
   }

   //If all the buttons toggle state is off, it is as if they were all pressed, skip no category
   if(l_bAllToggledOff)
   {
      for(UINT32 i = 0; i < ESphere::Count ; i++)
      {
         l_bMustSkipCategory[i] = false;
      }
   }


   //Display the events in historical order (by ID and not by category)
   {
      //Count the number of items and set the iterators
      UINT32                               l_iItemCount = 0;
      map<UINT32, SEventLogData>::iterator l_EventsIt[ESphere::Count];
      for(UINT32 i = 0 ; i < ESphere::Count; i++)
      {
         if(!l_bMustSkipCategory[i])
         {
            l_iItemCount += m_Events[i].size();         
            l_EventsIt[i] = m_Events[i].begin();
         }
      }
     
      //While there are items to display
      while(l_iItemCount)
      {
         //Find the lowest id among the iterators
         INT32 l_iLowestIdSphere = -1;
         INT32 l_iLowestID       = -1;

         for(UINT32 i = 0 ; i < ESphere::Count ; i++)
         {
            if(!l_bMustSkipCategory[i])
            {
               //If the iterator is valid
               if(l_EventsIt[i] != m_Events[i].end())
               {
                  //If there are no chosen item yet
                  if(l_iLowestIdSphere == -1)
                  {
                     l_iLowestIdSphere = i;
                     l_iLowestID       = l_EventsIt[i]->first;
                  }
                  //If the id is lower
                  else if(l_EventsIt[i]->first < (UINT32)l_iLowestIdSphere)
                  {
                     l_iLowestIdSphere = i;
                     l_iLowestID       = l_EventsIt[i]->first;
                  }
               }
            }
         }

         if(l_iLowestID != -1)
         {
            //Item to display is identified by in l_iLowestID and l_iLowestIDSphere
            AddItem(l_EventsIt[l_iLowestIdSphere]->second);
            m_ShownEvents.push_back(make_pair( m_pObjEventsLst->Get_Nb_Rows() - 1, l_EventsIt[l_iLowestIdSphere]->first) );
            l_EventsIt[l_iLowestIdSphere]++;
            l_iItemCount--;
         }
         else
         {
            gassert(l_iItemCount == 0,"Incoherence in event log window::Update");
            break;
         }
      }
   }
   m_pObjEventsLst->Update();

   return true;
}