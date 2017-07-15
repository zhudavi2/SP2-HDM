/**************************************************************
*
* sp2_progress_meter.h
*
* Description
* ===========
*  Describes the interface of class GProgressMeter
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_PROGRESSMETER_H_
#define _GOLEM_PROGRESSMETER_H_

namespace SP2
{
   /*!
    * Defines the progress meter, in percentage from 0 to 100.
    *
    * The progress meter can be used hierarchically
    * by using splits. A split indicates that a certain
    * portion of the progress will be updated using
    * values from 0 to 100.
    *
    * Ex.:  The table below shows the consequences of a
    *       sequence of operations on the progress
    *
    *        Call(Value)      | Percentage
    *       ------------------------------------
    *        Percentage(20)   | 20
    *        Split(20)        | 20
    *        Percentage(40)   | 28
    *        Percentage(50)   | 30
    *        Split(50)        | 30
    *        Percentage(30)   | 33
    *        Percentage(70)   | 37
    *        EndSplit()       | 40
    *        EndSplit()       | 40
    *        Percentage(56)   | 56
    *        Split(4)         | 56
    *        Percentage(25)   | 57
    *        EndSplit()       | 60
    *        Percentage(75)   | 75
    **/
   class GProgressMeter : public GUI::GFrame
   {
   public:
      bool OnCreate();

      void  Percentage(UINT8 in_iPercentage);
      void  PercentageUpdate(UINT8 in_iPercentage);
      UINT8 Percentage();

      //! Begin a hierarchical split of the progress
      void  Split(UINT8 in_iPercentage);

      //! Fetch the current hierarchical split amount
      UINT8 SplitPercentage();

      //! End the current hierarchical split
      void  EndSplit();

      bool Update();
      void SetText(const GString & in_sText);
      static const GString TypeName;

   protected:
      GProgressMeter();

      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GLabel* m_pTextLbl;

   private:
      struct GSplitInfo
      {
         REAL32   m_fOffset;
         REAL32   m_fScale;
         INT32    m_iLength;
         INT32    m_iStart;
      };

      vector<GSplitInfo>   m_vSplits;

      INT32  m_iActualPercentage;
      INT32  m_iHundredPercentLength;
   };

}

#endif //_GOLEM_PROGRESSMETER_H_
