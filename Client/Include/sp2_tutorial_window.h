/**************************************************************
*
* golem_tutorialwindow.h
*
* Description
* ===========
*  Describes the interface of class GTutorialWindow
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TUTORIALWINDOW_H_
#define _GOLEM_TUTORIALWINDOW_H_

#include "sp2_tooltip_window.h"

/*!
* [Class description]
**/

namespace SP2
{
   class G2DComponent;
   class GTutorials;
   class GTutorialWindow : public GTooltipWindow
   {
      friend class GTutorials;
   public:
      GTutorialWindow();
      ~GTutorialWindow();

      GUI::EEventStatus::Enum OnMouseClick(const GUI::GEventData &, GUI::GBaseObject *);
      void OnShow();
      void OnHide();
      bool OnCreate();
      bool OnDestroy();
      void CenterAndResize();
      void MoveToTop();
	  void MoveToBottom();
      void SetupStandardSize();

      static const GString TypeName;
   protected:
   private:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GLabel* m_pLabel;
      GFrame * m_pArrow;
      
      void Text(const GString & in_Text) {m_pLabel->Text(in_Text);}
      void PointTo(GBaseObject *);
      void PointTo(GVector2D<INT32> &);
      void PointToLatLong(const GVector2D<REAL32>& in_Coord);
      void PointTo(GVector2D<REAL32> in_Position,GBaseObject* in_pParent);
      void HideArrow();
   };
}
#endif //_GOLEM_TUTORIALWINDOW_H_
