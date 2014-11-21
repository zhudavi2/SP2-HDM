/**************************************************************
*
* sp2_graph.h
*
* Description
* ===========
*  Description & implementation of class SP2::GGraph
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_GRAPH_H_
#define _SP2_GRAPH_H_

namespace SP2
{
   /*!
    * Graph object that can display line of data inside a
    * parent model.
    **/
   class GGraph
   {
   public:
      GGraph(CGFX_Model_Itf* in_pParent);
      ~GGraph();

      //! Change the graph size
      void Size(const GVector2D<INT32>& in_NewSize);

      //! Retrieve the graph size
      GVector2D<INT32> Size() const;

      //! Change the graph position
      void Position(const GVector2D<INT32>& in_NewPosition);

      //! Retrieve the graph position
      GVector2D<INT32> Position() const;

      //! Change the width of the graph line
      void LineWidth(UINT32 in_iNewWidth);

      //! Retrieve the current graph line width
      UINT32 LineWidth() const;

      //! Change the color of the graph line
      void LineColor(const GColorRGBInt& in_NewColor);

      //! Retrieve the current graph line color
      GColorRGBInt LineColor() const;

      //! Change the visibility of the horizontal axis (value 0)
      void AxisVisibility(bool in_bVisible);

      //! Retrieve the visibility of the horizontal axis (value 0)
      bool AxisVisibility() const;

      //! Change the color of the graph axis
      void AxisColor(const GColorRGBInt& in_NewColor);

      //! Retrieve the current graph line axis
      GColorRGBInt AxisColor() const;

      //! Change the maximum number of values that can be displayed in the graph
      void NbValues(UINT32 in_iNewNbValues);

      //! Retrieve the maximum number of values that can be displayed in the graph
      UINT32 NbValues();

      void AutomaticValueScale(bool in_bNewState);

      bool AutomaticValueScale() const;

      void MinValue(REAL32 in_fNewMinValue);

      REAL32 MinValue() const;

      void MaxValue(REAL32 in_fNewMaxValue);

      REAL32 MaxValue() const;

      //! Update the content of the graph using the specified data
      void Update(vector<REAL32> in_Values, UINT32 in_iStep = 1);

      //! Retrieve the location of the specified graph value
      GVector2D<INT32> PositionAtValue(UINT32 in_iValueIndex, REAL32 in_Vfalue, bool& out_bClipped);

   private:
      CGFX_Model_Itf*            m_pModel;
      GFX::GMaterial*            m_pMaterial;
      CGFX_Transform_2D_Itf*     m_pTransform;
      GFX::GMesh*                m_pMesh;
      GFX::GCascadePixelShader*  m_pPS;
      GFX::GOverridePSCascade*   m_pOverride;
      GFX::GOverridePSCascade*   m_pOverrideValue;

      UINT32                     m_iLineWidth;
      UINT32                     m_iNbValues;
      REAL32                     m_fMinValue;
      REAL32                     m_fMaxValue;
      UINT32                     m_iOffset;
      bool                       m_bAutoScale;
   };
}

#endif // #ifndef _SP2_GRAPH_H_
