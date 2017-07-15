/**************************************************************
*
* sp2_meter_double.h
*
* Description
* ===========
*  Describes the interface of class GUpdatableValue
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_METER_DOUBLE_H_
#define _SP2_METER_DOUBLE_H_

const UINT32 c_OnMeterDoubleChange = 2143486737;

namespace SP2
{
   class GMeterDouble : public GUI::GFrame
   {
   public:
      static const GString TypeName;

      GMeterDouble();
      ~GMeterDouble();

      bool OnCreate();
      bool OnDestroy();
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      REAL64 Value();
      void   Value(REAL64 in_fValue, bool in_bCallEvent = true);

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GPictureBox* m_pObjRedBg;
      GUI::GPictureBox* m_pObjGreenBg;
      GUI::GPictureBox* m_pObjRedMeter;
      GUI::GPictureBox* m_pObjGreenMeter;
      GUI::GPictureBox* m_pObjMiddle;

      REAL64 m_fValue;

      bool   m_bDragging;
      bool   m_bCallEvent;
   };
};

#endif //_SP2_METER_DOUBLE_H_