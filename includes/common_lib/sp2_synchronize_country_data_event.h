/**************************************************************
*
* golem_synchronizecountrydata.h
*
* Description
* ===========
*  Describes the interface of class GSynchronizeCountryData
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SYNCHRONIZECOUNTRYDATA_H_
#define _GOLEM_SYNCHRONIZECOUNTRYDATA_H_

namespace SP2
{
namespace Event
{

/*!
 * GSynchronizeCountryData
 **/
class GSynchronizeCountryData : public SDK::Event::GGameEvent
{
   friend class GClient;
   friend class GServer;
   static SDK::GGameEventSPtr New();
public:
   GSynchronizeCountryData();
   ~GSynchronizeCountryData();

   bool Serialize(GIBuffer&)const;
   bool Unserialize(GOBuffer&);

   GCountryDataItf* CountryData();
   void             CountryData(GCountryDataItf* in_pCountryData);

protected:
   GCountryDataItf* m_pCountryData;
   bool             m_bMustFreeCountryData;

private:
};

};
};
#endif //_GOLEM_SYNCHRONIZECOUNTRYDATA_H_
