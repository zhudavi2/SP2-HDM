/**************************************************************
*
* sp2_research_information_holder.h
*
* Description
* ===========
*  Contains all the data for research management
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef __RESEARCH_INFO_HOLDER__
#define __RESEARCH_INFO_HOLDER__

namespace SP2
{
   class GResearchInformationHolder
   {
   private:
      //! Returns the new progression in the given area

   public:
      GResearchInformationHolder();
      void Update();


      GResearchInformationHolder& operator =(const GResearchInformationHolder& in_Original);



      //1st level Budget Information
      REAL64 m_fBudgetGround;
      REAL64 m_fBudgetAir;
      REAL64 m_fBudgetNaval;
      REAL64 m_fBudgetNuclear;
      //Detailed budget information
      REAL64 m_fBudget   [EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
      //Maximum values for the country research, 
      //we use a REAL64 cause we put the research progression in 
      //this variable directly (5.1 means 10% of progression between 5 and 6)
      REAL32 m_fMaxValues[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];



   };
};

#endif //__RESEARCH_INFO_HOLDER__