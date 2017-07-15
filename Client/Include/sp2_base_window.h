/**************************************************************
*
* sp2_base_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_BaseWindow_H_
#define _CSP2_BaseWindow_H_

namespace SP2
{

   class GBaseWindow : public GUI::GFrame
   {
   public:
	   GBaseWindow();
	   ~GBaseWindow();


	   bool OnCreate();
      bool OnDestroy();
      void OnShow();

	   GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller);

      bool OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller);
      void OnDrag(const GUI::GEventData &, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> &);
      void OnDragEnd();

      bool Moveable();
      void Moveable(bool in_bMoveable);

      bool Dirty();
      void Dirty(bool in_bDirty);

      void Minimize();
      void Maximize();
      bool Minimized();

      bool Enabled();
      void Enabled(bool in_bEnabled);

      virtual bool AcceptEscape() const;
      void Close();

      //! Do we need to validate country for a specific action ?
      bool NeedCountryCheckToSave() const
         {return m_bNeedCountryCheckToSave;};
      void NeedCountryCheckToSave(bool in_NeedCountryCheckToSave)
         {m_bNeedCountryCheckToSave = in_NeedCountryCheckToSave;};

      bool WindowCanDisable() const
         {return m_bWindowCanDisable;};
      void WindowCanDisable(bool in_WindowCanDisable)
         {m_bWindowCanDisable = in_WindowCanDisable;};

      virtual void Save(){return;};

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GBaseObject*  m_pObjFrameTitle;
      GUI::GToggle* m_pObjLockTog;
      GUI::GButton* m_pObjExitBtn;
      GUI::GButton* m_pObjApplyBtn;

      GUI::GPictureBox* m_pObjGradientPic;
      GUI::GPictureBox* m_pObjColorGradientPic;

      GUI::GFrame* m_pObjInnerFrm;

      GDataBaseWindow*  m_pWindowData;
      REAL64 m_fWhenSaved;      

      bool m_bWindowCanDisable;

   private:
      GVector2D<INT32> m_MouseClick;
      
      INT32 m_iLastHeight;
      CGFX_Action_Itf* m_pActionResize;
      
      //bool m_bDragging;
      bool m_bMoveable;
      bool m_bDirty;
      bool m_bMinimized;

      //! Country check before saving ?
      bool m_bNeedCountryCheckToSave;

      //! a click on the exit button
      void ExitWindow();
      void Apply();
   };

};

#endif //_CSP2_BaseWindow_H_
