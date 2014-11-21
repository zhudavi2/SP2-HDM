/**************************************************************
*
* sp2_event_history_markers.h
*
* Description
* ===========
*  Class SP2::Event::GHistoryMarkers definition, describing the
*  event sent to notify client of the actions taken during
*  one iteration & their importance.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_HISTORY_MARKERS_H_
#define _SP2_EVENT_HISTORY_MARKERS_H_

namespace SP2
{
   namespace Event
   {
      /*!
       * Class SP2::Event::GHistoryMarkers definition, describing the
       * event sent to notify client of the actions taken during
       * one iteration & their importance.
       **/
      class GHistoryMarkers : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:
         bool Serialize(GIBuffer & out_Obj) const;
         bool Unserialize(GOBuffer & in_Obj);

         struct GMarkerInfo
         {
            EHistoryMarkerType::Enum   m_eType;
            REAL32                     m_fValue;
         };

         REAL64               m_fTime;
         vector<GMarkerInfo>  m_vMarkers;
      };

   }

}

#endif // #ifndef _SP2_EVENT_HISTORY_MARKERS_H_