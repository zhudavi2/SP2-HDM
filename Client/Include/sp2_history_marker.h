/**************************************************************
*
* sp2_history_marker.h
*
* Description
* ===========
*  Definition of class SP2::GHistoryMarker, which contains data
*  about a history graph marker & is able to convert this data
*  into text.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_HISTORY_MARKER_H_
#define _SP2_HISTORY_MARKER_H_

namespace SP2
{
   class GHistoryMarker
   {
   public:
      UINT32                     m_iTime;

      EHistoryMarkerType::Enum   m_eType;
      REAL32                     m_fValue;
   };
}

#endif // #ifndef _SP2_HISTORY_MARKER_H_
