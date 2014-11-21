/**************************************************************
*
* sp2_event_send_data.h
*
* Description
* ===========
*  Describes the interface of class GSendData
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _SP2_EVENT_SEND_DATA_H
#define _SP2_EVENT_SEND_DATA_H


namespace SP2
{
   class GClient;
   class GServer;
namespace Event
{
/*!
 * Game event used to send first set of data from the server to the
 * client so the client will be able to cache the data instead of
 * always request it.
 **/
class GSendData :  public SDK::Event::GGameEvent
{
   friend GClient;//so they have access to New
   friend GServer;//so they have access to New
public:
   bool Serialize(GIBuffer&) const;
   bool Unserialize(GOBuffer&);

   //! List of all regions and their control
   vector<GRegionControl>*    m_vRegionControlArray;
   vector<UINT32> m_vRegionNames;
	vector<EContinent::Enum>  m_vRegionContinent;
	vector<EGeoGroups::Enum>  m_vRegionGeoGroup;  
	
	//! List of all relations with other countries
	vector<REAL32>					m_vRelations;

   multimap<UINT32,vector<GVector2D<REAL32> > > m_vBombardedRegionsAndPoints;

   //! List of all cities
   vector<SCityInfo>*         m_vCities;	

   //! List of country capitals
   vector<UINT32>*            m_vCapitals;

   //! List of country annexes
   hash_map<UINT32, GRegionAnnex>* m_Annexes;

   //! List of conquered country
   bool*                      m_pConqueredCountries;

   //! List of treaties
   hash_map<UINT32, GTreaty>* m_Treaties;

   //! List of war declarations
   hash_set<SWarPair>*        m_Wars;

	//! List of curent wars
	hash_map<UINT32, GWar>*		m_CurrentWars;

	//! List of units in training 
   list<GUnitInTraining>*     m_ListOfUnitsInTraining;

   //! This flag is set to false when pointers must not be deleted by event destruction.
   bool m_bOwnsPointers;

   //! Our game options
   GGameOptions m_GameOptions;

protected:
   GSendData();
   ~GSendData();

   void DeletePointers();

   static SDK::GGameEventSPtr New();
};

};//End namespace Event
};//End namespace SP2
#endif //_SP2_EVENT_SEND_DATA_H
