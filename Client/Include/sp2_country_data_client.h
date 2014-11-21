/**************************************************************
*
* golem_countrydataclient.h
*
* Description
* ===========
*  Describes the interface of class GCountryDataClient
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COUNTRYDATACLIENT_H_
#define _GOLEM_COUNTRYDATACLIENT_H_


namespace SP2
{

/*!
 * [Class description]
  **/
class GCountryDataClient : public GCountryDataItf
{
public:

   static GCountryDataItf* New();
   GCountryDataClient();
   ~GCountryDataClient();

   void CopyModifiedData(const GCountryDataClient& in_Original);


   bool RemoveMissileClientSide(SP2::GNuclearMissile& in_Missile);


   bool ReceivedAtLeastOneSync() const;
   void ReceivedAtLeastOneSync(bool in_bVal);

   bool m_bConstitutionalFormDirty;
protected:
   bool m_bReceivedAtLeastOneSync;
private:
};

};
#endif //_GOLEM_COUNTRYDATACLIENT_H_
