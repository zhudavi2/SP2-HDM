/**************************************************************
*
* sp2_tutorials.h
*
* Description
* ===========
*  Describes the interface of class GTutorials
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "..\include\sp2_tooltip_window.h"

#ifndef _GOLEM_TUTORIALS_H_
#define _GOLEM_TUTORIALS_H_

namespace SP2
{
   //Interface tutorial strings
   const UINT32 c_iInterfaceTutorialNbSteps = 54;
   const UINT32 c_iInterfaceTutorialStringId[c_iInterfaceTutorialNbSteps] = 
   {
      101164,
      101165,
      101166,
      101167,
      101168,
      101169,
      101170,
      101171,
      101172,
      101173,
      101174,
      101175,
      101176,
      101177,
      101178,
      101179,
      101180,
      101181,
      101182,
      101183,
      101184,
      101185,
      101186,
      101187,
      101188,
      101189,
      101190,
      101191,
      101192,
      101193,
      101194,
      101195,
      101196,
      101197,
      101198,
      101199,
      101200,
      101201,
      101202,
      101203,
      101204,
      101205,
      101206,
      101207,
      101208,
      101209,
      101210,
      101211,
      101212,
      101213,
      101214,
      101215,
      101216,
      101217,
   };

   //Military tutorial strings
   const UINT32 c_iMilitaryTutorialNbSteps = 63;
   const UINT32 c_iMilitaryTutorialStringId[c_iMilitaryTutorialNbSteps] = 
   {
            101223,
            101224,
            101225,
            101226,
            101227,
            101230,
            101231,
            101232,
            101233,
            101234,
            101235,
            101236,
            101237,
            101238,
            101239,
            101240,
            101241,
            101242,
            101243,
            101244,
            101245,
            101246,
            101247,
            101248,
            101249,
            101250,
            101251,
            101252,
            101253,
            101254,
            101255,
            101256,
            101257,
            101258,
            101259,
            101260,  //step 36 : Open the military design window
            101261,  //step 37 : Arrow on the Unit Types combo box. Click to continue
            101262,  //step 38 : Arrow on the Designs combo box. Click to continue
            101263,  //step 39 : Arrow on Unit Type description text, Click to continue
            101264,  //step 40 : Arrow on ‘New design’ button Click to continue
            101265,  //step 41 : Arrow on the design name field Input name and press ENTER to continue to continue
            101266,  //step 42 : Arrow on the Technology Dots frame Click in Tutorial window to continue
            101267,  //step 43 : Arrow on the 3D parts frame, Click to continue
            101268,
            101269,
            101270,
            101271,
            101272,
            101273,
            101274,
            101275,
            101276,
            101277,
            101278,
            101279,
            101280,
            101281,
            101282,
            101283,
            101284,
            101285,
            101286,
            101287,
   };

   const UINT32 c_iClickAnywhereToContinueStrId      = 101218;
   const UINT32 c_iClickHereToProceedToNextStepStrId = 101219;

   const UINT32               c_iMilitaryTutorialCountryToControl          =  184;//USA
   const UINT32               c_iMilitaryTutorialTargetUnitGroup           = 1353;
   const UINT32               c_iMilitaryTutorialTargetCountry             =   114;// 44
   const SP2::EUnitType::Enum c_eMilitaryTutorialTargetUnitType            = EUnitType::AttackHelicopter;
   const UINT32               c_iMilitaryTutorialResearchPageProgressTabId =    2;
   const UINT32               c_iMilitaryTutorialResearchPageFinancingTabId=    3;
   const UINT32               c_iMilitaryTutorialDesignToProduce           =  259;
   const UINT32               c_iMilitaryTutorialNuclearTargetCountry      =   37;
   const REAL32               c_fMilitaryTutorialZoomLevelAttack           = 0.35f;
   const REAL32               c_fMilitaryTutorialZoomLevelSelect           = 0.20f;
   
   // Forward declaration
   class GTutorialWindow;

   // Wich tutorial
   namespace ETutorials 
   { 
      enum Enum 
      {
         None,
         InterfaceTutorial,
         MilitaryTutorial
      };
   }

   class GTutorials : GUI::GHookItf
   {
   public:
      friend class SP2::GDataDisplayLayer;
      static GTutorials & Instance();

      void StartTutorial(ETutorials::Enum);      
      void StopTutorial();  
      ETutorials::Enum StartedTutorial() {return m_StartedTutorial;}

   protected:
      bool CanFireEvent(EKeyAndMouseEvents::Enum, GUI::GBaseObject *& in_pTarget, GEventData &);
      bool CanFireHotkey(const GEventData &, GBaseObject*);
      bool CanGiveFocusTo(GBaseObject* in_pObject);
      bool CanBringToFront(GBaseObject* in_pObject);
      GFile m_LeftClickMousePointer;     

      //Interface tutorial methods
      void IterateInterfaceTutorial();
      bool InterfaceTutorialCanObjectRecvEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,GEventData & in_EventData);
      bool ClickingTutorialWindowOnSpecificGoalAdvancesInterfaceTutorial();

      //Military tutorial methods
      void IterateMilitaryTutorial();
      bool MilitaryTutorialCanObjectRecvEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,GEventData & in_EventData);
      bool ClickingTutorialWindowOnSpecificGoalAdvancesMilitaryTutorial();

   private:

      GTutorials();
      GTutorials(const GTutorials &) {};
      GTutorials & operator = (const GTutorials &) {};
      ~GTutorials();

      void TutorialIterate();
      ETutorials::Enum m_StartedTutorial;
      GTutorialWindow* m_pTutorialWindow;
      REAL64 m_fNextTimeToProceed;
      SP2::GUnitGroup* m_pMilitaryTargetGrp;
      bool m_bGoalReached;
      bool m_bSpecificGoal;         
      set<GBaseObject *> m_ValidEventTargets;
      set<GBaseObject *> m_AlwaysValidEventTargets;
      int m_State;

      //!Checks if the military conditions for a status change are met
      void IterateMilitaryTutorialConditions();

      //!Checks if the interface conditions for a status change are met
      void IterateInterfaceTutorialConditions();

      //! Init the military tutorial properly
      void SetupMilitaryTutorial();

      //! Setup the tutorial window text
      void SetupTutorialWindowText(const GString &in_sBegin, GTutorialWindow* in_pTutWindow);

      //! Check if its an earth rotating event
      bool EarthRotatingEvent(EKeyAndMouseEvents::Enum in_eKeyOrMouseEvent,GEventData & in_EventData);

      SP2::GArenaInfo* m_pTargetArenaInfo;

      // Earth looking functions
      void SetupEarthView(const GVector2D<REAL32> &in_vPos, const REAL32 in_fZoom);
      void SetupEarthViewCompleted();

      // Enable to rewind in the tutorials steps
      bool m_bStateRollBack;
   };

}

#endif //_GOLEM_TUTORIALS_H_
