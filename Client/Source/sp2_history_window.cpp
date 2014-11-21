/**************************************************************
*
* sp2_history_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_history_window.h"

#include "../include/sp2_graph.h"
#include "../include/sp2_history_marker_window.h"
#include "../include/sp2_history_marker_info_window.h"
#include "../include/sp2_main_bar.h"

using namespace DesignPattern;

const GVector2D<INT32> c_MarkerOffset(-3, -3);
const GVector2D<INT32> c_MarkerSize(7, 7);
const INT32 c_iMarkerInfoOffsetY = 16;

const GString GHistoryWindow::TypeName = L"History_Window";

GUI::GBaseObject* GHistoryWindow::New()
{
   return new GHistoryWindow();
}

bool GHistoryWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   // Fill value type combo box
   m_pHistoryCombo = (GComboBox*) Child(L"frmInner")->Child(L"frmHistory")->Child(L"cboDisplay");

   for(UINT32 i = 0;i < EHistoryValues::TotalCount;i ++)
   {
      m_ValueTypeNames[i] = g_ClientDAL.GetString( (INT32) EStrId::c_HistoryValues[i] );
#ifdef GOLEM_DEBUG
      GListBox::GRow* l_pTemp = 
#endif

         m_pHistoryCombo->Add_Item(m_ValueTypeNames[i] );

#ifdef GOLEM_DEBUG
      gassert( (UINT32) l_pTemp->m_iId == i, "Incoherent Combo Box Item ID");
#endif
   }

   // Select first history value
   m_pHistoryCombo->Selected_Content(m_ValueTypeNames[EHistoryValues::Population] );
   m_eCurrentSelection = EHistoryValues::Population;

   // Create history graph
   m_pMarkerParent = Child(L"frmInner")->Child(L"frmHistory")->Child(L"frmGraph");
   m_pGraph = new GGraph(m_pMarkerParent->Child(L"Container")->Model() );
   m_pGraph->Size(m_pMarkerParent->Child(L"Container")->Size() );

   m_pGraph->LineWidth(1);
   m_pGraph->LineColor(GColorRGBInt(255, 0, 0, 255) );
   m_pGraph->AxisColor(GColorRGBInt(40, 40, 40, 255) );
   m_pGraph->NbValues(300);

   m_pHistoryMarkerInfo = (GHistoryMarkerInfoWindow*) 
      CREATE_GUI_OBJECT(GHistoryMarkerInfoWindow::TypeName, 
                        L"Marker_Info", 
                        this, 
                        false, 
                        false);

   // Fetch axis value objects
   m_pMinimumVerticalValue = (GUI::GLabel*) m_pMarkerParent->Child(L"txtV0");
   m_pMaximumVerticalValue = (GUI::GLabel*) m_pMarkerParent->Child(L"txtV1");

   m_vHorizontalValues.resize(6);
   for(UINT32 i = 0;i < 6;i ++)
   {
      m_vHorizontalValues[i].m_pValue = (GUI::GLabel*) m_pMarkerParent->Child(GString(L"txtH") + GString(i) );
      m_vHorizontalValues[i].m_pTrait = (GUI::GFrame*) m_pMarkerParent->Child(GString(L"frmH") + GString(i) );
   }

   // retrieve the scale toggles and labels
   m_pTog1Year  = (GUI::GToggle*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"tog1Year");
   m_pTog1Month = (GUI::GToggle*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"tog1Month");
   m_pTog2Year  = (GUI::GToggle*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"tog2Year");
   m_pTog6Month = (GUI::GToggle*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"tog6Month");
   m_pLbl1Year  = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"txtTransparentLabel2");
   m_pLbl1Month = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"txtTransparentLabel1");
   m_pLbl2Year  = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"txtTransparentLabel4");
   m_pLbl6Month = (GUI::GLabel*)Child(L"frmInner")->Child(L"frmHistory")->Child(L"txtTransparentLabel3");

   // hide for now
   m_pTog1Year->Hide();  m_pLbl1Year->Hide();
   m_pTog1Month->Hide(); m_pLbl1Month->Hide();
   m_pTog2Year->Hide();  m_pLbl2Year->Hide();
   m_pTog6Month->Hide(); m_pLbl6Month->Hide();

   // Make sure will be notified when the graph content is modified
   g_ClientDAL.Attach(this, ENotifyWindowMsg::HistoryValuesAndMarkers);
   g_ClientDAL.Attach(this, ENotifyWindowMsg::HistoryValuesOnly);
   g_ClientDAL.Attach(this, ENotifyWindowMsg::HistoryMarkersOnly);

   return true;
}

bool GHistoryWindow::OnDestroy()
{
   g_ClientDAL.Detach(this);

   SAFE_DELETE(m_pGraph);

   return __super::OnDestroy();
}

void GHistoryWindow::OnResize()
{
   m_pGraph->Size(m_pMarkerParent->Child(L"Container")->Size() );
}

void GHistoryWindow::OnShow()
{
   ChangeSelection(m_eCurrentSelection);
}

void GHistoryWindow::OnHide()
{
   if(g_ClientDDL.MainBar() )
   {
      GEventData l_Data;
      g_ClientDDL.MainBar()->OnCustomEvent(c_iCloseWindowEvent, l_Data, this);
   }

   return __super::OnHide();
}

EEventStatus::Enum GHistoryWindow::OnMouseOver(const GEventData &in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_pCaller->Parent() == m_pMarkerParent)
      {
         ShowMarkerInfo(in_pCaller);
         return EEventStatus::Handled;
      }
   }

   return __super::OnMouseOver(in_EventData, in_pCaller);
}

EEventStatus::Enum GHistoryWindow::OnMouseOut(const GEventData &in_EventData, GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_pCaller->Parent() == m_pMarkerParent)
      {
         m_pHistoryMarkerInfo->Hide();
         return EEventStatus::Handled;
      }
   }

   return __super::OnMouseOver(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GHistoryWindow::OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if (in_EventId != ECustomEvents::OnComboBoxSelectionEvent)
      return __super::OnCustomEvent(in_EventId, in_EventData, in_pCaller);

   GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
   if(l_pCombo == m_pHistoryCombo)
   {
      // Check if selection has changed
      if(l_pCombo->Selected_Content() != m_ValueTypeNames[m_eCurrentSelection] )
      {
         // Find the new value selection
         UINT32 l_iNewSelection = 0;
         while(m_ValueTypeNames[l_iNewSelection] != l_pCombo->Selected_Content() )
         {
            l_iNewSelection ++;
            gassert(l_iNewSelection < EHistoryValues::TotalCount, "Cannot find history value type");
         }

         ChangeSelection(l_iNewSelection);
      }
   }

   return GUI::EEventStatus::Handled;
}

void GHistoryWindow::OnSubjectChange(GSubject& in_Subject, 
                                     const GSubjectNotification& in_Notification, 
                                     void* in_pAdditionalMsg)
{
   // Make sure notification is a graph or marker change from DAL
   // Also, no need to update when we are hidden
   if( (&in_Subject == &g_ClientDAL) && Visible() )
   {
      gassert( (m_eCurrentSelection >= 0) && (m_eCurrentSelection < EHistoryValues::TotalCount),
               "History graph selection is invalid");

      // Different actions depending on notification type
      switch(in_Notification)
      {
      case ENotifyWindowMsg::HistoryValuesAndMarkers:
      case ENotifyWindowMsg::HistoryValuesOnly:
         {
            m_pGraph->Update(g_ClientDAL.HistoryValues(m_eCurrentSelection) );

            UpdateAxis();

            // Markers need an update since the graph is different
            UpdateMarkers();
         }
         break;
      case ENotifyWindowMsg::HistoryMarkersOnly:
         {
            UpdateMarkers();
         }
         break;
      }
   }
}

void GHistoryWindow::ChangeSelection(UINT32 in_iNewSelection)
{
   m_eCurrentSelection = (EHistoryValues::Enum) in_iNewSelection;
   m_pGraph->AxisVisibility(true);
   m_pGraph->Update(g_ClientDAL.HistoryValues(m_eCurrentSelection) );

   UpdateAxis();
   UpdateMarkers();
}

void GHistoryWindow::UpdateMarkers()
{
   GVector2D<INT32> l_Offset = m_pMarkerParent->Child(L"Container")->Position();
   INT32 l_iCurrentMarker = -1;

   // Fetch current marker data
   const vector<REAL32>& l_vValues = g_ClientDAL.HistoryValues(m_eCurrentSelection);
   const vector<GHistoryMarker>& l_vMarkers = g_ClientDAL.HistoryMarkers(m_eCurrentSelection);
   INT32 l_iTimeOffset = g_ClientDAL.HistoryMarkerOffset(m_eCurrentSelection);

   // Create markers
   GVector2D<INT32> l_Pos;
   for(UINT32 i = 0;i < l_vMarkers.size();i ++)
   {
      const GHistoryMarker& l_Marker = l_vMarkers[i];
      UINT32 l_ValueID = l_Marker.m_iTime - l_iTimeOffset;

      bool l_bClipped;
      GVector2D<INT32> l_NewPos = m_pGraph->PositionAtValue(l_ValueID, l_vValues[l_ValueID], l_bClipped);
      if( (!l_bClipped) && (l_ValueID < l_vValues.size() ) )
      {
         bool l_bMerge = false;

         // First marker will always be displayed "properly" (no merge)
         if(l_iCurrentMarker >= 0)
         {
            // Following markes can be merged if too close from (or equal to) previous marker
            if( (l_NewPos.x - l_Pos.x) <= (c_MarkerSize.x + 1) )
            {
               l_bMerge = true;
            }
         }

         if(!l_bMerge)
         {
            // Show a new marker at the current marker position
            l_iCurrentMarker ++;
            if(l_iCurrentMarker < (INT32) m_vMarkers.size() )
            {
               // A marker window already exist, simply unhide it
               m_vMarkers[l_iCurrentMarker]->Show();

               // Make sure it does not contain previous marker data
               m_vMarkers[l_iCurrentMarker]->ClearMarkers();
            }
            else
            {
               // Create a new marker window

               // Name it with its index so we can know what marker callbacks us in mouse events
               m_vMarkers.push_back( (GHistoryMarkerWindow*) 
                                    CREATE_GUI_OBJECT(GHistoryMarkerWindow::TypeName,
                                                      GString(l_iCurrentMarker),
                                                      m_pMarkerParent, 
                                                      true, 
                                                      false) );
               m_vMarkers[l_iCurrentMarker]->Size(c_MarkerSize);
            }

            // Update the marker position
            m_vMarkers[l_iCurrentMarker]->Position(l_NewPos + l_Offset + c_MarkerOffset);
            l_Pos = l_NewPos;
         }

         gassert( (l_iCurrentMarker >= 0) && (l_iCurrentMarker < (INT32) m_vMarkers.size() ), 
                  "A marker object should have been created");

         // Remember that this marker window references or merge if using previous marker
         m_vMarkers[l_iCurrentMarker]->InsertMarker(l_Marker.m_eType, l_Marker.m_fValue);
      }
   }

   // Hide remaining markers (if they exist)
   for(l_iCurrentMarker = l_iCurrentMarker + 1;l_iCurrentMarker < (INT32) m_vMarkers.size();l_iCurrentMarker ++)
   {
      m_vMarkers[l_iCurrentMarker]->Hide();
   }

   GBaseObject* l_pObj = g_Joshua.GUIManager()->FindTargetObject(g_Joshua.GUIManager()->MousePosition(), this);

   if(l_pObj)
   {
      vector<GHistoryMarkerWindow*>::iterator l_It = 
         find(m_vMarkers.begin(), m_vMarkers.end(), l_pObj);

      if(l_It != m_vMarkers.end() )
      {
         ShowMarkerInfo(l_pObj);
      }
      else
      {
         m_pHistoryMarkerInfo->Hide();
      }
   }
   else
   {
      m_pHistoryMarkerInfo->Hide();
   }
}

void GHistoryWindow::ShowMarkerInfo(GBaseObject* in_pObj)
{
   m_pHistoryMarkerInfo->Show();

   // Retrieve the marker index from its name
   INT32 l_iMarkerID = in_pObj->Name().ToINT32();

   GString l_sInfo;

   for(UINT32 i = 0;i < m_vMarkers[l_iMarkerID]->CountMarkers();i ++)
   {
      l_sInfo += g_ClientDAL.HistoryMarkerToText(m_vMarkers[l_iMarkerID]->MarkerType(i), m_vMarkers[l_iMarkerID]->MarkerValue(i) ) + GString(L"\n");
   }

   m_pHistoryMarkerInfo->Label()->Size(Size().x, ( (GBaseObject*) (m_pHistoryMarkerInfo->Label() ) )->Size().y);
   m_pHistoryMarkerInfo->Label()->Text(l_sInfo);
   m_pHistoryMarkerInfo->Size(m_pHistoryMarkerInfo->Label()->Text2D()->WidthMax() + 8, m_pHistoryMarkerInfo->Label()->Text2D()->Height() + 8);
   m_pHistoryMarkerInfo->Label()->Size(m_pHistoryMarkerInfo->Size().x - 8, m_pHistoryMarkerInfo->Size().y - 8);

   // Position window so it is located around the marker
   GVector2D<INT32> l_CornerPos = m_pHistoryMarkerInfo->Parent()->AbsoluteToRelative(in_pObj->RelativeToAbsolute(GVector2D<INT32>(0, 0) ) );
   GVector2D<INT32> l_NewInfoPos = l_CornerPos;
   if(l_NewInfoPos.y + c_iMarkerInfoOffsetY + m_pHistoryMarkerInfo->Size().y < m_pHistoryMarkerInfo->Parent()->Size().y)
   {
      l_NewInfoPos.y += c_iMarkerInfoOffsetY;
   }
   else
   {
      l_NewInfoPos.y -= c_iMarkerInfoOffsetY + m_pHistoryMarkerInfo->Size().y - in_pObj->Size().y;
   }

   if(l_NewInfoPos.x + m_pHistoryMarkerInfo->Size().x > m_pHistoryMarkerInfo->Parent()->Size().x)
   {
      l_NewInfoPos.x = m_pHistoryMarkerInfo->Parent()->Size().x - m_pHistoryMarkerInfo->Size().x;
   }

   m_pHistoryMarkerInfo->Position(l_NewInfoPos);
}

void GHistoryWindow::UpdateAxis()
{
   // right align label depending on value type
   bool l_bRightAlign = true;

   // Update vertical values
   REAL32 l_fMinVal = m_pGraph->MinValue();
   REAL32 l_fMaxVal = m_pGraph->MaxValue();

   GString l_sMinValueText;
   GString l_sMaxValueText;

   switch(c_HistoryValueTypes[m_eCurrentSelection])
   {
   case EHistoryValueType::NoUnit:
      l_sMinValueText = GString::FormatNumber(l_fMinVal, " ", ".", "", "", 3, 0, true);
      l_sMaxValueText = GString::FormatNumber(l_fMaxVal, " ", ".", "", "", 3, 0, true);
      break;
   case EHistoryValueType::Money:
      l_sMinValueText = GString::FormatNumber(l_fMinVal, " ", ".", "$ ", "", 3, 0, true);
      l_sMaxValueText = GString::FormatNumber(l_fMaxVal, " ", ".", "$ ", "", 3, 0, true);
      break;
   case EHistoryValueType::Percent:
      l_sMinValueText = GString::FormatNumber(l_fMinVal * 100.f, " ", ".", "", "%", 3, 1, false);
      l_sMaxValueText = GString::FormatNumber(l_fMaxVal * 100.f, " ", ".", "", "%", 3, 1, false);
      break;
   case EHistoryValueType::Relation:
      if(l_fMinVal == 0)
      {
         l_sMinValueText = g_ClientDAL.GetString(EStrId::NeutralRelation);
      }
      else
      {
         l_sMinValueText = g_ClientDAL.GetString(EStrId::Bad);
      }
      if(l_fMaxVal == 0)
      {
         l_sMaxValueText = g_ClientDAL.GetString(EStrId::Neutral);
      }
      else
      {
         l_sMaxValueText = g_ClientDAL.GetString(EStrId::Good);
      }
      break;
   case EHistoryValueType::Research:
      l_bRightAlign   = false;
      l_sMinValueText =L"";
      {
         UINT32 l_iResearchLevel = 0;
         while( (UINT32) (l_fMaxVal * 10.f / 408.f) > l_iResearchLevel)
         {
            l_iResearchLevel ++;
         }
         gassert(l_iResearchLevel <= 10, "Research level too high?");
         l_sMaxValueText = g_ClientDAL.GetString(EStrId::c_ResearhLevel[l_iResearchLevel] );
      }
      break;
   default:
      gassert(0, "Invalid history graph selection");
   }
   m_pMinimumVerticalValue->Text(l_sMinValueText);
   m_pMaximumVerticalValue->Text(l_sMaxValueText);

   // check for our axis min and max values labels
   if(l_bRightAlign)
   {
      m_pMinimumVerticalValue->TextAlignment("MiddleRight");
      m_pMaximumVerticalValue->TextAlignment("MiddleRight");
   }
   else
   {
      m_pMinimumVerticalValue->TextAlignment("MiddleLeft");
      m_pMaximumVerticalValue->TextAlignment("MiddleLeft");
   }
   // multilined only if left align
   m_pMinimumVerticalValue->Multiline(!l_bRightAlign);
   m_pMaximumVerticalValue->Multiline(!l_bRightAlign);
   m_pMinimumVerticalValue->Update();
   m_pMaximumVerticalValue->Update();

   // Update horizontal values & position
   GVector2D<INT32> l_Offset = m_pMarkerParent->Child(L"Container")->Position();
   const vector<REAL32>& l_vValues = g_ClientDAL.HistoryValues(m_eCurrentSelection);

   INT32 l_iTimeOffset = g_ClientDAL.HistoryMarkerOffset(m_eCurrentSelection);
   INT32 l_iLastTime = (INT32) l_vValues.size() + l_iTimeOffset;

   // Convert time into date
   GDateTime l_DateLast = g_ClientDAL.DateFromTime( 0.5 + (REAL64) l_iLastTime);
   l_DateLast.Hours(0);
   l_DateLast.Minutes(0);
   l_DateLast.Seconds(0);
   l_DateLast.Tenths(0);
   GDateTime l_DateCur(l_DateLast);

   // Place markers at each month until there is no more place on the history graph
   UINT32 l_iAxisID = 0;
   bool l_bClipped;
   GVector2D<INT32> l_NewPos(0,0);

   // Place a marker at each month beginning
   l_DateCur.Month(1 + ( ( (l_DateCur.Month() - 1) >> 1) << 1) );
   l_DateCur.Day(1);
   do
   {
      // Convert current date into history value ID
      GTimeSpan l_Span = l_DateLast - l_DateCur;
      INT32 l_iAxisTime = RoundToInt(l_Span.Days() / c_fHistoryUpdatePeriod);
      INT32 l_ValueID = (l_iLastTime - l_iAxisTime) - l_iTimeOffset;

      // Make sure history value exists
      if(l_ValueID >= 0)
      {
         // Compute x position of history value
         l_NewPos = m_pGraph->PositionAtValue(l_ValueID, 0, l_bClipped);

         // Make sure position is inside current graph
         if(l_NewPos.x >= 0)
         {
            // Show an axis element & value at the appropriate position
            ShowAxisElement(l_iAxisID, l_NewPos.x + l_Offset.x, l_DateCur);

            l_iAxisID ++;
         }

         // Go to previous month
         if(l_DateCur.Month() > 0)
         {
            l_DateCur.Month(l_DateCur.Month() - 2);
         }
         else
         {
            l_DateCur.Month(MAXMONTH - 2);
            l_DateCur.Year(l_DateCur.Year() - 1);
         }
      }
      else
      {
         // Force a loop exit
         l_NewPos.x = -1;
      }

   // Loop until a month is found before the current graph begin
   } while(l_NewPos.x >= 0);

   // Continue place axis values on months following the last graph value
   l_DateCur = l_DateLast;
   l_DateCur.Month(1 + ( ( (l_DateCur.Month() - 1) >> 1) << 1) );
   l_DateCur.Day(1);
   do
   {
      // Go to next month
      if(l_DateCur.Month() < MAXMONTH - 2)
      {
         l_DateCur.Month(l_DateCur.Month() + 2);
      }
      else
      {
         l_DateCur.Year(l_DateCur.Year() + 1);
         l_DateCur.Month(1);
      }

      // Convert current date into history value ID
      GTimeSpan l_Span = l_DateCur - l_DateLast;
      INT32 l_iAxisTime = RoundToInt(l_Span.Days() / c_fHistoryUpdatePeriod);
      UINT32 l_ValueID = (l_iLastTime + l_iAxisTime) - l_iTimeOffset;

      // Compute x position of history value
      l_NewPos = m_pGraph->PositionAtValue(l_ValueID, 0, l_bClipped);

      // Make sure position is inside current graph
      if(l_NewPos.x < m_pGraph->Size().x)
      {
         // Show an axis element & value at the appropriate position
         ShowAxisElement(l_iAxisID, l_NewPos.x + l_Offset.x, l_DateCur);

         l_iAxisID ++;
      }

   } while(l_NewPos.x < m_pGraph->Size().x);

   // Hide all non updated axis elements
   for(;l_iAxisID < 6;l_iAxisID ++)
   {
      GHorizontalValue& l_HorizValue = m_vHorizontalValues[l_iAxisID];
      l_HorizValue.m_pValue->Hide();
      l_HorizValue.m_pTrait->Hide();
   }
}

void GHistoryWindow::ShowAxisElement(UINT32 in_iAxisID, INT32 in_NewPosX, const GDateTime& in_Date)
{
   GHorizontalValue& l_HorizValue = m_vHorizontalValues[in_iAxisID];

   // Only show & update axis value & position if it is completely visible inside graph
   if( (in_NewPosX + (l_HorizValue.m_pValue->Width() >> 1) ) < m_pMarkerParent->Width() )
   {
      l_HorizValue.m_pValue->Show();
      l_HorizValue.m_pValue->Position(in_NewPosX - (l_HorizValue.m_pValue->Width() >> 1), ( (GUI::GBaseObject*) (l_HorizValue.m_pValue) )->Position().y);
      l_HorizValue.m_pValue->Text(in_Date.ToString(L"YYYY/MM") );
   }
   else
   {
      // do not show the marker label until we can really display it properly in the graph
      l_HorizValue.m_pValue->Hide();
   }

   // Update axis trait position
   l_HorizValue.m_pTrait->Show();
   l_HorizValue.m_pTrait->Position(in_NewPosX, ( (GUI::GBaseObject*) (l_HorizValue.m_pTrait) )->Position().y);
}
