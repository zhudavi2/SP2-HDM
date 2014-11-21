/************************************************
 * File:
 * Owner:
 * Description:
 *
 *
 * History:
 * ========
 *
 *************************************************/

#ifndef _CSP2_ADVISOR_ADVISE_WINDOW_H_
#define _CSP2_ADVISOR_ADVISE_WINDOW_H_

namespace SP2
{

   class GAdvisorAdviseWindow : public GFrame
   {
   public:
	   GAdvisorAdviseWindow();
	   ~GAdvisorAdviseWindow();

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      void Set(GString in_sText, GUI::GBaseObject* in_pObject);
      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      void SetText(GString in_sText);
      void AttachTo(GUI::GBaseObject* in_pCaller);
   };
};

#endif //_CSP2_ADVISOR_ADVISE_WINDOW_H_
