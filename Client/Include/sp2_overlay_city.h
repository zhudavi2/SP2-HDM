/**************************************************************
*
* sp2_overlay_city.h
*
* Description
* ===========
*  Description & implementation of class SP2::GOverlayCity,
*  which displays city icons as an overlay on the earth
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_OVERLAY_CITY_H_
#define _SP2_OVERLAY_CITY_H_

namespace SP2
{
   const INT32 c_iNbCityTypes = 4;

   /*!
    * Display city icons as an overlay on the earth
    **/
   class GOverlayCity
   {
   public:
      GOverlayCity(class GBackgroundEarth* in_pBGEarth);
      ~GOverlayCity();

      //! Initialize the city display
      void InitCities();

      void HideCities();
      void ShowCities();

      //! Update the city display (when camera has moved for example)
      void UpdateCities();

      //! Display the name of a city located at the specified mouse location
      void ShowCityName(const GVector2D<INT32>& in_MousePos);

   private:
      GBackgroundEarth*       m_pBGEarth;

      CGFX_Model_Itf*         m_pCitiesMasterModel;
      GFX::GMesh*             m_pCityMeshes[c_iNbCityTypes + 1];
      GFX::GOverrideTSampler* m_pCityTSamplers[c_iNbCityTypes + 1];

      CGFX_Model_Itf*         m_pCityNameModel;
      CGFX_Transform_2D_Itf*  m_pCityNameTfx;
      CGFX_Text_2D_Itf*       m_pCityNameText;

      GVector2D<UINT32>       m_CitySize[c_iNbCityTypes + 1];
      GVector2D<UINT32>       m_CitySizeHalf[c_iNbCityTypes + 1];

      UINT32 CityPopToType(UINT32 in_iPopulation) const;
   };
}

#endif // #ifndef _SP2_OVERLAY_CITY_H_