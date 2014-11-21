/************************************************
 * File: sp2_add_remove_window.h
 * Owner: Francois Durand
 * Description:
 * Country list selection
 *
 * History:
 * ========
 *
 *************************************************/

#ifndef _CSP2_AddRemoveWindow_H_
#define _CSP2_AddRemoveWindow_H_

const UINT32 c_OnAddRemoveCountrySelection = 2143486734;

namespace SP2
{
   class GAddRemoveWindow : public SP2::GBaseWindow
   {
   public:
	   GAddRemoveWindow();
	   ~GAddRemoveWindow();

	   bool OnCreate();
      void OnShow();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      GUI::EEventStatus::Enum OnKeyDown(const GEventData &, GBaseObject* );

      void Sort(GUI::GListBox* in_pList);
      void Update(vector<INT32> & in_viSideA, vector<INT32> & in_viSideB, set<UINT32> & in_sIncluded, bool in_bTwoSide = true, bool in_bSingle = false, bool in_bPonctual = false);

      void Caller(GUI::GBaseObject* in_pCaller);

      void SwapRows(GUI::GListBox* in_pFrom, GUI::GListBox* in_pTo);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjSelectedALst;
      GUI::GListBox* m_pObjSelectedBLst;
      GUI::GListBox* m_pObjUnselectedLst;

      GUI::GButton*  m_pObjLeftA;
      GUI::GButton*  m_pObjRightA;

      GUI::GButton*  m_pObjLeftB;
      GUI::GButton*  m_pObjRightB;

      GUI::GButton*  m_pObjLeftAB;
      GUI::GButton*  m_pObjRightAB;
   private:
      GUI::GBaseObject* m_pCaller;

      vector<INT32>* m_pvSideA;
      vector<INT32>* m_pvSideB;

      set<UINT32>*   m_psIncluded;

      bool m_bSingle;
      bool m_bPonctual;
   };
};

#endif //_CSP2_AddRemoveWindow_H_
