/**************************************************************
*
* sp2_mini_map.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_MINI_MAP_H_
#define _SP2_MINI_MAP_H_

namespace SP2
{
   struct SEarthState;

   class GMiniMap : public GUI::GFrame
   {
   public:
      GMiniMap();
      ~GMiniMap();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove  (const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp    (const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel (const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      bool UpdateCrosshair(const SEarthState & in_State);
      bool UpdateZoomBar();
      void ToggleMapLook(bool in_bNormalLook);
      void ToggleMagnifier(bool in_nToggle);
      
      static const GString TypeName;

      CGFX_Track_Itf* & AnimationSlidingTrack();

      GButton* ZoomBar() {return m_pObjZoomBar;}
      GButton* Map() {return m_pObjMap;}
      GButton* HomeButton() {return m_pObjHome;}
 

   private:

      GButton* m_pObjHome;
      GToggle* m_pObjMagnifier;
      GButton* m_pObjZoomBar;
      GButton* m_pObjMap;
      GButton* m_pObjCrosshair;
      GButton* m_pObjHidingButton;
      GButton* m_pObjHidingZoom;
      GFrame*  m_pObjLongitudeBar;
      GFrame*  m_pObjLatitudeBar;
      GFrame*  m_pObjRealisticFrm;
      GFrame*  m_pObjStrategicFrm;

      INT16    m_iDragStartX;
      INT16    m_iDragStartY;

      REAL32   m_fZoomInc;

      bool m_bDragging;

      void MiniMapToEarth(GVector2D<INT32>, SEarthState & in_State);

      CGFX_Track_Itf* m_pLookSwapTrack;
      REAL32          m_fAnimLength;
      void CreateTransitionAnimation(REAL32 in_fAnimationTime, REAL32 in_fDelay);
      CGFX_Track_Itf*   m_pSlidingTrack;

   
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   };
};
#endif //_SP2_MINI_MAP_H_
