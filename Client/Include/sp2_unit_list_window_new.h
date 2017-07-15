/**************************************************************
*
* sp2_unit_list_window_new.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_UnitListWindowNew_H_
#define _CSP2_UnitListWindowNew_H_

#include "sp2_frame_cursor_meter.h"

namespace SP2
{
   namespace EUnitListSortType
   {
      enum Enum
      {
         UnitType,
         UnitQty,
         UnitTraining,
         UnitName,
         UnitStatus,
         UnitUpkeep,
         UnitValue,
         UnitForSale,
      };
   };
   
   struct GUnitListSortData
   {
      multimap< GString,               INT32 >::iterator m_TypeIter;
      multimap< INT32,                 INT32 >::iterator m_QtyIter;
      multimap< ETrainingLevel::Enum,  INT32 >::iterator m_TrainingIter;
      multimap< GString,               INT32 >::iterator m_NameIter;
      multimap< GString,               INT32 >::iterator m_StatusIter;
      multimap< REAL32,                INT32 >::iterator m_UpkeepIter;
      multimap< REAL64,                INT32 >::iterator m_ValueIter;
      multimap< bool,                  INT32 >::iterator m_SaleIter;
   };

   class GUnitListWindowNew : public GBaseWindow, Hector::DesignPattern::GObserver
   {
      friend class GTutorials;
   public:
	   GUnitListWindowNew();
	   ~GUnitListWindowNew();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, SDK::GUI::GBaseObject* in_pCaller = NULL);
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void Update( INT32 in_iFromWhichRank = 0 );
      void UpdateUnitList(UINT16 in_iCountryID, INT32 in_iGroupID, EUnitType::Enum in_eFilter = (EUnitType::Enum)0xFFFFFFFF, bool in_bRefreshWindow = true);
      void Sort( EUnitListSortType::Enum in_eSort );
      void Sort( EUnitListSortType::Enum in_eSort, bool in_bAscending );
      INT32 GetMouseOverRowId( GVector2D<INT32> in_iAbsoluteMousePosition );

      void Iterate();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GFrame*    m_pUnitListFrm;
      GUI::GFrame*    m_pUnitItem[20];
      GUI::GComboBox* m_pUnitTypeCbo;

      GUI::GButton*   m_pObjSortTrainingIcon;
      GUI::GButton*   m_pObjSortType;
      GUI::GButton*   m_pObjSortTypeIcon;
      GUI::GButton*   m_pObjSortQty;
      GUI::GButton*   m_pObjSortQtyIcon;
      GUI::GButton*   m_pObjSortName;
      GUI::GButton*   m_pObjSortNameIcon;
      GUI::GButton*   m_pObjSortStatus;
      GUI::GButton*   m_pObjSortStatusIcon;
      GUI::GButton*   m_pObjSortUpkeep;
      GUI::GButton*   m_pObjSortUpkeepIcon;
      GUI::GButton*   m_pObjSortValue;
      GUI::GButton*   m_pObjSortValueIcon;
      GUI::GButton*   m_pObjSortForSaleIcon;

      //GUI::GButton*   m_pObjSelectAll;
      //GUI::GButton*   m_pObjClearSelection;
      
      GUI::GButton*   m_pObjTrainBtn;
      GUI::GButton*   m_pObjSellBtn;
      GUI::GButton*   m_pObjDisbandBtn;

      GUI::GVScrollBar* m_pObjListScr;

      SP2::GGradientBar* m_pAccuracyMtr;
      GUI::GLabel*       m_pAccuracyLbl;
      GUI::GLabel*       m_pAccuracyTxt;

   private:
      void InitializeUnitTypeCombo();
      void AddUnit( SP2::GUnit* in_pUnit );
      const vector<INT32> & GUnitListWindowNew::GetUnitsToDisplay( INT32 in_iFirstUnitShown, EUnitListSortType::Enum in_eSort, bool in_bAscending );
      void RemoveUnit( INT32 in_iUnitID, bool in_bRemoveForModification = true );
      void DisplayUnit( UINT32 in_iRowID, INT32 in_iUnitID );
      void ClearRow(UINT32 in_iRowID);
      void UpdateIterators();
      void UnitUpdateStatus( INT32 in_iUnitID, INT32 in_iRowID, bool in_bForceUpdate = false );
      void UnitsRefreshStatuses();
      void UpdateModifiedUnitGroups( const hash_map<UINT32, SDK::Combat::GUnitGroup*> & in_ModifiedGroups );
      void UpdateModifiedUnits( const hash_map<UINT32, SDK::Combat::GUnit*> & in_ModifiedUnits );
      void UpdateRemovedUnits( const set<UINT32> & in_RemovedUnits );
      void SelectRow( UINT32 in_iRowID );
      void UnselectRow( UINT32 in_iRowID );
      void SellUnit( UINT32 in_iUnitId );
      REAL32 CalculateUnitUpkeep( INT32 in_iUnitID );
      void EnableUnitButtons( vector<INT32> in_viUnitsID );

      //Sort map
      multimap< GString,               INT32 > m_TypeMap;
      multimap< INT32,                 INT32 > m_QtyMap;
      multimap< ETrainingLevel::Enum,  INT32 > m_TrainingMap;
      multimap< GString,               INT32 > m_NameMap;
      multimap< GString,               INT32 > m_StatusMap;
      multimap< REAL32,                INT32 > m_UpkeepMap;
      multimap< REAL64,                INT32 > m_ValueMap;
      multimap< bool,                  INT32 > m_SaleMap;
      set     < INT32                        > m_UnitsLocked;
		
		Random::GQuick									  m_Rand;
      
      hash_map< INT32, GUnitListSortData > m_UnitsMap;

      vector< INT32 > m_ShownUnits;
      
      INT32 m_iCountryID;
      INT32 m_iGroupID;

      INT32                   m_iFirstRankShown;
      vector<INT32>           m_viSelectedUnits;

      EUnitListSortType::Enum m_eSortType;
      bool                    m_bSortAscending;
      EUnitType::Enum         m_eFilterType;
   };
};

#endif //_CSP2_UnitListWindowNew_H_
