/**************************************************************
*
* sp2_history_marker_window.cpp
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

#include "../include/sp2_history_marker_window.h"

const GString GHistoryMarkerWindow::TypeName = L"History_Marker";

GUI::GBaseObject* GHistoryMarkerWindow::New()
{
   return new GHistoryMarkerWindow();
}

bool GHistoryMarkerWindow::OnCreate()
{
   ClearMarkers();

   return __super::OnCreate();
}

EEventStatus::Enum GHistoryMarkerWindow::OnMouseOver(const GEventData &, GBaseObject*)
{
   return EEventStatus::Callback;
}

EEventStatus::Enum GHistoryMarkerWindow::OnMouseOut(const GEventData &, GBaseObject*)
{
   return EEventStatus::Callback;
}

UINT32 GHistoryMarkerWindow::CountMarkers() const
{
   return m_iCurrentCount;
}

UINT32 GHistoryMarkerWindow::MarkerType(UINT32 in_iIndex) const
{
   gassert(in_iIndex < m_iCurrentCount, "Invalid marker reference index");

   return m_pMarkers[in_iIndex].m_iType;
}

REAL32 GHistoryMarkerWindow::MarkerValue(UINT32 in_iIndex) const
{
   gassert(in_iIndex < m_iCurrentCount, "Invalid marker reference index");

   return m_pMarkers[in_iIndex].m_fValue;
}

void GHistoryMarkerWindow::InsertMarker(UINT32 in_iType, REAL32 in_fValue)
{
   // Quickly insert reference when no marker is already present
   if(m_iCurrentCount == 0)
   {
      m_iCurrentCount = 1;
      m_pMarkers[0].m_iType = in_iType;
      m_pMarkers[0].m_fValue = in_fValue;
   }
   else
   {
      // Try to merge the value
      bool l_bInserted = false;
      if(c_HistoryMarkerIsMergeable[in_iType] )
      {
         INT32 l_Id = 0;
         for(;(l_Id < (INT32) m_iCurrentCount) && !l_bInserted;l_Id ++)
         {
            if(m_pMarkers[l_Id].m_iType == in_iType)
            {
               l_bInserted = true;
               m_pMarkers[l_Id].m_fValue += in_fValue;
            }
         }
      }

      if(!l_bInserted)
      {
         // Quickly skip reference insertion when its importance is lower than the last one
         if( !( (m_iCurrentCount >= c_iMaxNbMarkerInfo) && (m_pMarkers[c_iMaxNbMarkerInfo - 1].m_iType <= in_iType) ) )
         {
            // Find the insertion point of the incoming marker
            UINT32 l_iInsert = 0;
            while( (m_pMarkers[l_iInsert].m_iType <= in_iType) && (l_iInsert < m_iCurrentCount) )
            {
               l_iInsert ++;
            }

            gassert(l_iInsert < c_iMaxNbMarkerInfo, "Last marker reference should have a lower importance than incoming importance");
            m_iCurrentCount = min(m_iCurrentCount + 1, c_iMaxNbMarkerInfo);

            // Move the marker references after the insertion
            if(c_iMaxNbMarkerInfo - l_iInsert - 1 > 0)
            {
               Memory::Move(m_pMarkers + l_iInsert + 1, m_pMarkers + l_iInsert, c_iMaxNbMarkerInfo - l_iInsert - 1);
            }

            // Insert the new reference
            m_pMarkers[l_iInsert].m_iType = in_iType;
            m_pMarkers[l_iInsert].m_fValue = in_fValue;
         }
      }
   }
}

void GHistoryMarkerWindow::ClearMarkers()
{
   m_iCurrentCount = 0;
}
