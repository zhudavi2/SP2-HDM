/**************************************************************
*
* sp2_game_scenario.h
*
* Description
* ===========
*  Describes the interface of class GGameScenario
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/


// Include Guards
#ifndef _SP2_GAME_SCENARIO_H_
#define _SP2_GAME_SCENARIO_H_

namespace SP2
{
  
   // Foward Declaration
   class GGameObjective;


 
  /*!
   * Defines a game scenario
   **/
   class GGameScenario : public GSerializable
   {


   public:
      GGameScenario();
      ~GGameScenario();

      void Name(const GString& in_Name);
      const GString& Name(void) const;

      void Title(const GString& in_Title);
      const GString& Title(void);

      void Description(const GString& in_Description);
      const GString& Description(void);

      void Tips(const GString& in_Tips);
      const GString& Tips(void);

      void AddObjective(GGameObjective* in_pObjective);
      const list<GGameObjective*>& Objectives(void);


      void CompletedObjective(UINT32 in_iObjectiveId, EObjectiveStatus::Enum in_eObjectiveStatus);
      bool IsComplete(void);

	   bool	Serialize(GIBuffer&) const;
      bool	Unserialize(GOBuffer&);

      void ClearObjectives(void);
      void SetObjectives(vector<GGameObjective>& in_NewObjectives);

      void Country(UINT32 in_iCountryID);
      UINT32 Country(void);


   protected:
   private:

      //! Name of the scenario (as describe in the xml file)
      GString  m_Name;

      //! Title of the scenario
      GString  m_Title;

      //! Description of the Scenario
      GString  m_Description;

      //! Tips to give to the player
      GString  m_Tips;

      //! The contry the player have to play, -1 indicate he can choose is country
      UINT32    m_iCountryToPlay;

      //! A set of Game objective to complete this scenario
      list<GGameObjective*> m_ObjectviesToComplete;

      //! If game is completed
      bool m_bGameCompleted;
   };
}

#endif _SP2_GAME_SCENARIO_H_
