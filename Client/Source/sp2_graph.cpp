/**************************************************************
*
* sp2_graph.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_graph.h"

GGraph::GGraph(CGFX_Model_Itf* in_pParent)
: m_iLineWidth(1)
, m_iNbValues(2)
, m_fMinValue(0)
, m_fMaxValue(1)
, m_iOffset(0)
, m_bAutoScale(true)
{
   // Build the gfx object hierarchy
   m_pModel = g_Joshua.Renderer()->Create_Model();
   m_pMaterial = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                                       GFX::PSTYPE_DEF_FACTOR,
                                                                       NULL,
                                                                       &m_pPS);
   m_pTransform = g_Joshua.Renderer()->Create_Transform_2D();
   m_pMesh = g_Joshua.Renderer()->CreateMesh();

   in_pParent->Add_Child(m_pModel);
   m_pModel->Set_Transform(m_pTransform);
   m_pModel->Set_Material(m_pMaterial);
   m_pModel->Set_Primitive(m_pMesh);

   m_pTransform->Position().X = 0;
   m_pTransform->Position().Y = 0;

   m_pTransform->Size().X = 0;
   m_pTransform->Size().Y = 0;

   m_pPS->ColorFactor() = GColorRGBReal(1, 1, 1, 1);

   m_pOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
   m_pOverride->UseColorFactorOverride(true);
   m_pOverride->ColorFactorOverride() = GColorRGBReal(0.5f, 0.5f, 0.5f, 1);

   m_pOverrideValue = g_Joshua.Renderer()->CreateOverridePSCascade();
   m_pOverrideValue->UseColorFactorOverride(true);
   m_pOverrideValue->ColorFactorOverride() = GColorRGBReal(1.f, 1.f, 1.f, 1.f);

   m_pMesh->NbFacetGroups(2);
   m_pMesh->FacetGroup(0).AddOverride(m_pOverride);
   m_pMesh->FacetGroup(0).Visibility() = 0.f;
   m_pMesh->FacetGroup(0).Facets().Type(FACETS_LINE, false);
   m_pMesh->FacetGroup(0).Facets().Start() = 0;
   m_pMesh->FacetGroup(0).Facets().Count() = 2;
   m_pMesh->FacetGroup(1).AddOverride(m_pOverrideValue);

   Update(vector<REAL32>() );
}

GGraph::~GGraph()
{
   SAFE_RELEASE(m_pModel);
   SAFE_RELEASE(m_pMaterial);
   SAFE_RELEASE(m_pTransform);
   SAFE_RELEASE(m_pMesh);
   SAFE_RELEASE(m_pPS);

   SAFE_RELEASE(m_pOverride);
   SAFE_RELEASE(m_pOverrideValue);
}

void GGraph::Size(const GVector2D<INT32>& in_NewSize)
{
   m_pTransform->Size() = (GVector2D<REAL32> )in_NewSize;
}

GVector2D<INT32> GGraph::Size() const
{
   return (GVector2D<REAL32> ) m_pTransform->Size();
}

void GGraph::Position(const GVector2D<INT32>& in_NewPosition)
{
   m_pTransform->Position() = (GVector2D<REAL32> )in_NewPosition;
}

GVector2D<INT32> GGraph::Position() const
{
   return (GVector2D<REAL32> ) m_pTransform->Position();
}

void GGraph::LineWidth(UINT32 in_iNewWidth)
{
//   m_iLineWidth = max(in_iNewWidth, 1);
}

UINT32 GGraph::LineWidth() const
{
   return m_iLineWidth;
}

void GGraph::LineColor(const GColorRGBInt& in_NewColor)
{
   m_pOverrideValue->ColorFactorOverride() = GColorRGBReal(in_NewColor);
}

GColorRGBInt GGraph::LineColor() const
{
   return GColorRGBInt( (GColorRGBReal) m_pOverrideValue->ColorFactorOverride() );
}

void GGraph::AxisVisibility(bool in_bVisible)
{
   m_pMesh->FacetGroup(0).Visibility() = in_bVisible ? 1.f : 0.f;
}

bool GGraph::AxisVisibility() const
{
   return m_pMesh->FacetGroup(0).Visibility() != 0.f;
}

void GGraph::AxisColor(const GColorRGBInt& in_NewColor)
{
   m_pOverride->ColorFactorOverride() = GColorRGBReal(in_NewColor);
}

GColorRGBInt GGraph::AxisColor() const
{
   return GColorRGBInt( (GColorRGBReal) m_pOverride->ColorFactorOverride() );
}

void GGraph::NbValues(UINT32 in_iNewNbValues)
{
   m_iNbValues = max(2, in_iNewNbValues);
}

UINT32 GGraph::NbValues()
{
   return m_iNbValues;
}

void GGraph::AutomaticValueScale(bool in_bNewState)
{
   m_bAutoScale = in_bNewState;
}

bool GGraph::AutomaticValueScale() const
{
   return m_bAutoScale;
}

void GGraph::MinValue(REAL32 in_fNewMinValue)
{
   m_fMinValue = in_fNewMinValue;
}

REAL32 GGraph::MinValue() const
{
   return m_fMinValue;
}

void GGraph::MaxValue(REAL32 in_fNewMaxValue)
{
   m_fMaxValue = in_fNewMaxValue;
}

REAL32 GGraph::MaxValue() const
{
   return m_fMaxValue;
}

void GGraph::Update(vector<REAL32> in_Values, UINT32 in_iStep)
{
   // Clear the current mesh
   GShape& l_Shape = m_pMesh->Shape();

   l_Shape.ClearAll();

   // No line to display with only one value
   if(in_Values.size() > 1)
   {
      // Update the maximum & minimum values if needed
      if(m_bAutoScale)
      {
         m_fMinValue = FLT_MAX;
         m_fMaxValue = -FLT_MAX;

         if(AxisVisibility() )
         {
            m_fMinValue = 0;
            m_fMaxValue = 0;
         }

         vector<REAL32>::iterator l_It;
         UINT32 i;
         for(i = 0, l_It = in_Values.end();
            (i < m_iNbValues) && (l_It != in_Values.begin() );
            i ++, -- l_It)
         {
            REAL32 l_fCurValue = *(l_It - 1);
            m_fMinValue = min(m_fMinValue, l_fCurValue);
            m_fMaxValue = max(m_fMaxValue, l_fCurValue);
         }

         if(m_fMaxValue == m_fMinValue)
         {
            if(m_fMinValue != 0)
            {
               m_fMaxValue += m_fMinValue * 0.5f;
               m_fMinValue -= m_fMinValue * 0.5f;
            }
            else
            {
               m_fMaxValue = 0.5f;
               m_fMinValue = -0.5f;
            }
         }
      }

      // Update the displayed line
      m_pMesh->NbFacetGroups(m_iLineWidth + 1);
      UINT32 l_iNbElems = min(m_iNbValues, in_Values.size() );

      l_Shape.NbPositions(l_iNbElems * m_iLineWidth + 2);

      REAL32 l_fXInc = 1.f / (REAL32) (m_iNbValues - 1);
      REAL32 l_fYInc = 1.f / (m_fMaxValue - m_fMinValue);
      REAL32 l_fSizeYInv = 1.f / m_pTransform->Size().Y;

      l_Shape.Position(0).x = 0;
      l_Shape.Position(0).y = 1 + m_fMinValue * l_fYInc;
      l_Shape.Position(0).z = 0;

      l_Shape.Position(1).x = 1;
      l_Shape.Position(1).y = l_Shape.Position(0).y;
      l_Shape.Position(1).z = 0;

      m_iOffset = in_Values.size() - l_iNbElems;
      for(UINT32 l = 0;l < m_iLineWidth;l ++)
      {
         m_pMesh->FacetGroup(l + 1).Facets().Type(FACETS_LINE_STRIP, false);
         GLineStrip& l_LineStrip = m_pMesh->FacetGroup(l + 1).Facets().LineStrip();

         l_LineStrip.Start() = l * l_iNbElems + 2;
         l_LineStrip.Count() = l_iNbElems;

         REAL32 l_fLineOffset = ( (REAL32) l - (REAL32) (m_iLineWidth - 1) * 0.5f) * l_fSizeYInv;

         for(UINT32 i = 0;i < l_iNbElems;i ++)
         {
            l_Shape.Position(l_LineStrip.Start() + i).x = i * l_fXInc;
            l_Shape.Position(l_LineStrip.Start() + i).y = 1 - (l_fLineOffset + ( (in_Values[i + m_iOffset] - m_fMinValue) * l_fYInc) );
            l_Shape.Position(l_LineStrip.Start() + i).z = 0;
         }
      }
   }
   else
   {
      if(m_bAutoScale)
      {
         l_Shape.NbPositions(2);
         l_Shape.Position(0).x = 0.f;
         l_Shape.Position(0).y = 0.5f;
         l_Shape.Position(0).z = 0.f;
         l_Shape.Position(1).x = 1.f;
         l_Shape.Position(1).y = 0.5f;
         l_Shape.Position(1).z = 0.f;
      }
      else
      {
         l_Shape.Position(0).x = 0;
         l_Shape.Position(0).y = 1 + (m_fMinValue / (m_fMaxValue - m_fMinValue) );
         l_Shape.Position(0).z = 0;

         l_Shape.Position(1).x = 1;
         l_Shape.Position(1).y = l_Shape.Position(0).y;
         l_Shape.Position(1).z = 0;
      }
   }
}

GVector2D<INT32> GGraph::PositionAtValue(UINT32 in_iValueIndex, REAL32 in_fValue, bool& out_bClipped)
{
   if( (in_iValueIndex < m_iOffset) || (in_iValueIndex >= m_iNbValues + m_iOffset) )
   {
      out_bClipped = true;
   }
   else
   {
      out_bClipped = false;
   }

   return GVector2D<INT32>( RoundToInt( ( (REAL32) (in_iValueIndex - m_iOffset) * m_pTransform->Size().X / (REAL32) m_iNbValues) ), 
                            RoundToInt( m_pTransform->Size().Y * (1 - ( (in_fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue) ) ) ) );
}
