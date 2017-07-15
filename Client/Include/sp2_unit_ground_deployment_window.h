/**************************************************************
*
* sp2_unit_ground_deployment_window.h
*
* Description
* ===========
*  Describes the interface of class GUnitGroundDeploymentWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


// Include Guards
#ifndef _SP2_UNIT_GROUND_DEPLOY_WINDOW_H_
#define _SP2_UNIT_GROUND_DEPLOY_WINDOW_H_

namespace SP2
{
   class GProgressMeterYellowValue;

   /*!
   * Unit Deployement window
   * Used to drag and drop new units 
   **/
   class GUnitGroundDeploymentWindow : public SP2::GBaseWindow, 
                                       public Hector::DesignPattern::GObserver
   {
      friend class GTutorials;
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GUnitGroundDeploymentWindow();
      ~GUnitGroundDeploymentWindow();

      //UI Functions
      void OnShow();
      bool OnCreate();

      void IterateTrainingDisplay();

   protected:
      friend class GDataDisplayLayer;

   private:

      //! Clean selection in listbox 
      void CleanSelection();

      //! return true if the unit is in training and sets its progress
      bool GetUnitTrainingProgress(UINT32 l_inUnitId,REAL32& out_fProgress);

      //! Update the list of units that are ready to be deployed.
      void UpdateReadyToDeployGroup(void);

      //! Build the tree view with the list of new units.
      void BuildTreeView(void);

      //! Ask the server to deploy a unit to a specific mouse position
      void DeployUnit(GVector2D<INT32> in_MousePos);

      //! Ask the server to deploy a unit to a specific earth location
      void DeployUnitToEarthLocation(GVector2D<REAL32> in_EarthPosition);

      //! Callback when the unit manager group changes
      void OnSubjectChange(DesignPattern::GSubject &in_Subject,const DesignPattern::GSubjectNotification &in_Notification,void* in_pAdditionalMsg );
      
      //! Function that calculate if we can drop a specific units at the specific locations.
      bool CanDropUnitsAtPosition(GVector2D<INT32> in_MousePos);

      //! Splits units with information given by the split window.
      //void SplitUnits(void);

      //! Iterate over units in training to remove them from the training list
      void IterateUnitsInTraining(void);

      // UI Events
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);


      // UI Flags and data
      bool              m_bMouseIsDragging;
      GVector2D<INT32>  m_StartDragCoord;
      vector<INT32>     m_SelectedRow;
      GFile             m_DropCursorFile;
      GFile             m_InvalidCursorFile;
      GString           m_UnitDesignIDLogo[SP2::EUnitCategory::ItemCount];
      GString           m_UnitTrainingIDLogo[SP2::ETrainingLevel::ItemCount];

      // An assosiative container that assosiate Row* to GUnit*
      hash_map<GListBox::GRow*,GUnit*>    m_UnitByRow;

      // Unit Selection
      set<SP2::GUnit*> m_SelectedUnit;

      // UI Elements
      GUI::GTabbed*     m_objTabGroup;
      GUI::GButton*     m_pObjDeployToCapitalBtn;
      GUI::GButton*     m_pObjTrainingBtn;
      GUI::GListBox*    m_pObjGroundList;
      GUI::GListBox*    m_pObjAirList;
      GUI::GListBox*    m_pObjNavalList;
      GUI::GListBox*    m_pObjNuclearList;
      GUI::GListBox*    m_pSelectedList;
      GUI::GLabel*      m_pObjQuantityByCategory[4];

      hash_set<UINT32>  m_HasBeenTrained;

      vector<pair<UINT32, GProgressMeterYellowValue*> > m_vTrainingUnitProgress;
   };

};//end namespace SP2
#endif //_SP2_UNIT_GROUND_DEPLOY_WINDOW_H_
