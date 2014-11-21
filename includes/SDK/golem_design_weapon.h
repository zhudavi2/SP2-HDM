/**************************************************************
*
* golem_weapondesign.h
*
* Description
* ===========
*  Describes the interface of class GWeaponDesign
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_WEAPONDESIGN_H_
#define _GOLEM_WEAPONDESIGN_H_


namespace SDK
{
   namespace Combat
   {
      namespace Design
      {


         /*!
         * Defines the characteristics of a weapon
         **/
         class GOLEM_SDK_API GWeapon
         {
         public:
            GWeapon();
            ~GWeapon();
            //! Set the details of the weapon design
            void Set(const UINT32            in_iID,
               const REAL32            in_fRange,
               const REAL32            in_fPower,
               const REAL32            in_fAccuracy,
               const GString&          in_sName,
               const vector<UINT32>&   in_vPossibleTargets =  vector<UINT32>());

            //! Get the details about the weapon design
            void Get(UINT32&                 out_iID,
               REAL32&                 out_fRange,
               REAL32&                 out_fPower,
               REAL32&                 out_fAccuracy,
               GString&                out_sName,
               vector<UINT32>&         out_vPossibleTargets) const;

            UINT32 Id() const;

            // Set / Get the Range
            REAL32 Range() const;
            void   Range(const REAL32 in_fRange);
            //! Get the accuracy
            REAL32 Accuracy() const;
            //! Get the power
            REAL32 Power() const;

            const GString& Name() const;
            const vector<UINT32> & PossibleTargets() const;


         protected:
         private:
            UINT32         m_iId;       //Unique identifier
            REAL32         m_fRange;    //Weapon range
            REAL32         m_fPower;    //Weapon power
            REAL32         m_fAccuracy; //Weapon accuracy
            GString        m_sName;     //Weapon name
            vector<UINT32> m_vPossibleTargets; //IDs of the possible Unit types to target with that weapon

         };//End class GWeapon
      };//End namespace design
   };//End namespace combat
}
#endif //_GOLEM_WEAPONDESIGN_H_
