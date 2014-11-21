/**************************************************************
*
* sp2_strategic_warfare.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_StrategicWarfareWindow_H_
#define _CSP2_StrategicWarfareWindow_H_

namespace SP2
{
   class GProgressMeterYellowValue;

   class GStrategicWarfareWindow : public GBaseWindow
   {
   public:
	   GStrategicWarfareWindow();
	   ~GStrategicWarfareWindow();

	   bool OnCreate();
      void OnShow();

      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      GUI::GFrame* m_pObjInnerFrame;
      GProgressMeterYellowValue* m_pObjAttackCursor;
      GUI::GComboBox*   m_pObjCountry;
      GUI::GComboBox*   m_pObjAttackType;
      GUI::GLabel*      m_pObjAttackStrengthValue;

      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();
   private:
      vector<GString> m_vAttackType;
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateStrategicWarfareWindow();
};

#endif //_CSP2_StrategicWarfareWindow_H_
