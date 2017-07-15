/**************************************************************
*
* golem_combatoverview.h
*
* Description
* ===========
*  Describes the interface of class GCombatOverview
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COMBATOVERVIEW_H_
#define _GOLEM_COMBATOVERVIEW_H_

namespace SP2
{

   /*!
   * Window of the overview of what is happening in a combat.
   **/
   class GCombatOverview : public GUI::GFrame
   {
      friend class GDataDisplayLayer;
   public:
      GCombatOverview();
      ~GCombatOverview();

      //! Set the content of the combat overview window
      bool Set(const UINT32      in_iSide1Soldiers,
         const UINT32            in_iSide1Land,
         const UINT32            in_iSide1Air,
         const UINT32            in_iSide1Naval,
         const UINT32            in_iSide1Overall,
         const vector<UINT32>&   in_vSide1Actors,
         const vector<UINT32>&   in_vSide2Actors);

      static const GString TypeName;

   protected:

      static GUI::GBaseObject* New();
   private:
   };

};//end namespace SP2
#endif //_GOLEM_COMBATOVERVIEW_H_
