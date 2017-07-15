/**************************************************************
*
* sp2_mail_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_MAIL_WINDOW_H_
#define _CSP2_MAIL_WINDOW_H_

namespace SP2
{

   class GMailWindow : public GUI::GFrame
   {
   public:
	   GMailWindow();
	   ~GMailWindow();

      bool MailExists(UINT32 in_iMailID) const;
      INT32 AddMail(GString in_sMail, void (*in_sFunction)(void*), void* in_sParam);
      bool  ChangeMail(INT32 in_iID, GString in_sMail, void (*in_sFunction)(void*), void* in_sParam);
      void  RemoveMail(INT32 in_iMailID);
      void  RemoveAllMails();

      INT32 NextMailID(){return m_iNextMailID;}

      UINT32 NbPendingMessages() const;
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjMailsLst;

	   bool OnCreate();
      void OnShow();

      void Close();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller);
      void OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative);
      bool OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller);
      EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseLeave(const GEventData & in_EventData, GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller);

   private:
      void ShowMail(INT32 in_iID);
      void DisplayOverlay();
      void HideOverlay();

      vector< void (*)(void*) > m_vFunctions;
      vector<void*> m_vParams;
      vector<INT32> m_vMailID;
      vector<INT32> m_vRowID;

      INT32 m_iNextMailID;
   };
};

#endif //_CSP2_MAIL_WINDOW_H_
