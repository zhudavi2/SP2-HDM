/**************************************************************
*
* golem_thematicmaplegendwindow.h
*
* Description
* ===========
*  Describes the interface of class GThematicMapLegendWindow
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef SP2THEMATICMAPLEGENDWINDOW_H
#define SP2THEMATICMAPLEGENDWINDOW_H


namespace SP2
{

   namespace EThematicMapLegendType
   {
      enum Enum
      {
         HighLow = 0,
         GoodBad,
         Empire,
         Unknown,
			HumanControlled,
         Count = Unknown
      };
   };

   namespace EThematicMapEmpireColors
   {
      // const for unknown description
      const GColorRGBInt c_EmpireColorUnknown(0, 0, 0, 0);
      // array of know region description colors
      const GColorRGBInt
         c_EmpireColors[EThematicMapRegionDescription::ItemCount] = 
      {
         // PlayerControlled
         GColorRGBInt(129, 185, 117, 255),
         //Allied
         GColorRGBInt(121, 149, 175, 255),
         //Hostile
         GColorRGBInt(142, 28, 37, 255),
         //MilitaryControlled
         GColorRGBInt(213, 129, 62, 255),
         //PoliticallyControlled
         GColorRGBInt(218, 220, 97, 255),
         //TakenBy
         GColorRGBInt(26, 69, 88, 255),
         //TakenFrom
         GColorRGBInt(68, 5, 5, 255),
			//MilitaryAccess
         GColorRGBInt(129, 195, 194, 255)
      };
   };

/*!
 * Thematic map legend window
 **/
class GThematicMapLegendWindow : public GUI::GFrame, public DesignPattern::GObserver
{
   friend class GDataDisplayLayer;
public:
   GThematicMapLegendWindow();
   ~GThematicMapLegendWindow();

   //! OnCreate handler
   bool OnCreate();
   bool OnDestroy();

   //! Updates the thematic map legend window, depending on the legend type, and if the results are by country or by region (results used to create a ranking)
   void Update(EThematicMapLegendType::Enum     in_eLegendType,
               ECharacteristicToHighlight::Enum in_eCharacteristic,
               vector<REAL64>& in_vResults);




protected:
private:
   static const GString TypeName;
   static GBaseObject* New();

   GListBox*      m_pObjCountryList;
   GPictureBox*   m_pObjGradientPicBox;
   GLabel*        m_pObjTopLbl;
   GLabel*        m_pObjMiddleLbl;
   GLabel*        m_pObjBottomLbl;
   GLabel*        m_pObjTitleLbl;


   ECharacteristicToHighlight::Enum m_eCharacteristicToHighlight;
   EThematicMapLegendType::Enum     m_eLegendType;
   vector<UINT32>                   m_vCountryRanks;//Index = countryid-1 , value = rank

   //! Create the country ranking
   void CreateCountryRanking(ECharacteristicToHighlight::Enum in_eCharacteristic,vector<REAL64>& in_vResults);
   void SortByRanking();
   void UpdateSelectedCountryInList();
   void UpdateLegendImageAndLabels();
   void UpdateTitle();
   void UpdateRankTag();
   //! Observer method
   void OnSubjectChange(Hector::DesignPattern::GSubject &,
                        const Hector::DesignPattern::GSubjectNotification &,
                        void *);

   EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID,const SDK::GUI::GEventData& in_EventData,SDK::GUI::GBaseObject* in_pCaller = 0);
   EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
   EEventStatus::Enum OnMouseUp(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller);
   EEventStatus::Enum OnMouseDown(const GEventData &in_EventData, GUI::GBaseObject* in_pCaller);

   void FillListBoxWithCountries(GCountries& in_vCountries);
};

};
#endif //_GOLEM_THEMATICMAPLEGENDWINDOW_H_
