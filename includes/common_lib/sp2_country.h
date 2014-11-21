/**************************************************************
*
* golem_country.h
*
* Description
* ===========
*  Describes the interface of class GCountry
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COUNTRY_H_
#define _GOLEM_COUNTRY_H_

namespace SP2  
{
   //Constants
   const INT32 COUNTRY_CODE_SIZE = 4;
   const INT32 COUNTRY_FLAG_SIZE = 8;


/*!
 * Class to represent a country in Superpower 2.
 *  Contains all useful information about a country
 **/
class GCountry
{
public:
   GCountry();
   ~GCountry();

   //Id Get/Set
   void           Id(INT16 in_iID);
   INT16          Id() const;
   //Name Get/Set
   void           Name(const GString& in_sName);
   const GString& Name() const;

   //Code Get/Set
   void           Code(WCHAR*  in_sCode);
   void           Code(GString in_sCode);
   const WCHAR*   Code() const;
   //Flag Get/Set
   void           Flag(WCHAR*  in_sFlag);
   void           Flag(GString in_sFlag);
   const WCHAR*   Flag() const; 
   //! Player Get/Set
   SDK::GPlayer*       Player();
   void           Player(SDK::GPlayer* in_pPlayer);

   bool IsActive();
   bool IsActive() const;
   void IsActive(bool in_bActive);

protected:
private:
   INT16          m_iId;
   GString        m_sName;
   WCHAR          m_sCode[COUNTRY_CODE_SIZE];
   WCHAR          m_sFlag[COUNTRY_FLAG_SIZE];
   SDK::GPlayer*       m_pPlayer; 
   bool           m_bIsActive;
};


typedef vector<GCountry> GCountries;

}; //End namespace SP2

#endif //_GOLEM_COUNTRY_H_

