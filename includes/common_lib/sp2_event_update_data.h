/**************************************************************
*
* sp2_event_update_data.h
*
* Description
* ===========
*  Describes the interface of class GUpdateData
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_UPDATEDATA_H_
#define _GOLEM_UPDATEDATA_H_


namespace SP2
{
   class GClient;
   class GServer;
namespace Event
{
/*!
 * Game event used to send updates on data from the server to the
 * client so the client will be able to cache the data instead of
 * always request it.
 **/
class GUpdateData :  public SDK::Event::GGameEvent
{
   friend GClient;//so they have access to New
   friend GServer;//so they have access to New
public:
   //! List of regions (first) and their new military controler (second)
   vector<pair<UINT32, UINT32> > m_vRegionMilitaryChanges;

   //! List of regions (first) and their new political controler (second)
   vector<pair<UINT32, UINT32> > m_vRegionPoliticalChanges;

   //! List of city changes
   vector<SCityInfo>             m_vCityChanges;

   //! List of country (first) that has changed capital (second)
   vector<pair<UINT32, UINT32> > m_vCapitalChanges;

   //! List of annexed region changes
   vector<GRegionAnnex>          m_vAnnexChanges;

   //! List of annexed region removed
   vector<UINT32>                m_vAnnexRemoved;

   //! List of war declaration
   vector<SWarPair>              m_vWarList;

   //! List of peace declaration
   vector<SWarPair>              m_vPeaceList;

	//! List of Wars that need to be removed
	set<UINT32>							m_WarsToRemove;

	//! List of war changes
	vector<GWar>						m_vWarChanges;

protected:
   static SDK::GGameEventSPtr New();

   bool Serialize(GIBuffer&) const;
   bool Unserialize(GOBuffer&);
};

};//End namespace Event
};//End namespace SP2
#endif //_GOLEM_UPDATEDATA_H_
