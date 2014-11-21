/**************************************************************
*
* golem_graph_zone.h
*
* Description
* ===========
*  Graph Zone. Use this class to show an histogram or a time graph
*  Many properties helps to show the proper graph. Use GRAPH_CONTINUE
*  for a segmented-line graph, and GRAPH_DISCRETE for an histogram
*  Graph scrolls automatically to the left when full if you use the 
*  AddPoint function. If you want a fixed graph, use the Point function
*  All the properties related to graph display are locked as soon as 
*  a point is added to the graph. Use Clear() to clear the graph and
*  unlock the properties.
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#pragma once
#include "golem_generic_frame.h"

namespace SDK
{
   namespace GUI 
   {
      enum EGraphType
      {
         GRAPH_CONTINUE,
         GRAPH_DISCRETE,
      };

      class GOLEM_SDK_API GGraphZone : public GFrame
      {
      public:

         void GraphType(EGraphType);



         //! Getters for the properties described below
         GString        FontName()      const {return m_FontName;}
         GColorRGBInt   FontColorNormal()     const {return m_FontColorNormal;}
         GColorRGBInt   AxisColor()     const {return m_AxisColor;}
         UINT32         FontSize()      const {return m_FontSize;}
         UINT32         YMax()          const {return m_YMax;}
         UINT32         XRange()        const {return m_XRange;}
         UINT32         YRange()        const {return m_YRange;}
         UINT32         XBandsGap()     const {return m_XBandsGap;}
         INT32          XInterval()     const {return m_XInterval;}
         INT32          XMin()          const {return m_XMin;}
         INT32          YMin()          const {return m_YMin;}
         bool           XShowAxis()     const {return m_XShowAxis;}
         bool           YShowAxis()     const {return m_YShowAxis;}
         //bool           XShowValues()   const {return m_XShowValues;}
         //bool           YShowValues()   const {return m_YShowValues;}      
         bool           YAutoScaleMax() const {return m_YAutoScaleMax;}
         bool           YAutoScaleMin() const {return m_YAutoScaleMin;}
         EGraphType     GraphType()     const {return m_GraphType;};
         bool           XAxisSpecialEffect() const {return m_XAxisSpecialEffect;}

         //! Sets the font to use for the text
         void Font(GString, UINT32, const GColorRGBInt&);

         //! Sets the color of the axis
         void AxisColor(const GColorRGBInt &);

         //! Maximal Y value shown on the graph
         void YMax(INT32);

         //! Interval between the values
         void XInterval(UINT32);

         //! Number of values shown on the graph, including Min & Max (for Y)
         void XRange(UINT32);
         void YRange(UINT32);

         //! Minimal X value shown on the graph. This value is modified when scrolling
         void XMin(INT32);

         //! Minimal Y value shown on the graph
         void YMin(INT32);

         //! If set to true, the Y scale is set to the bigger value
         void YAutoScaleMax(bool);
         void YAutoScaleMin(bool);

         //! Gap between the bands
         void XBandsGap(UINT32);

         //! Show the axis
         void XShowAxis(bool);
         void YShowAxis(bool);

         void XAxisSpecialEffect(bool);

         //! Show the values
         //void XShowValues(bool);
         //void YShowValues(bool);

         //! Add a point to the graph and scroll if necessary
         void AddPoint(vector<INT32> Y, vector<GColorRGBInt> Color);
         void AddPoint(       INT32  Y,  const GColorRGBInt& Color);
         void AddPoint(vector<INT32> Y);
         void AddPoint(       INT32  Y);

         //! Add or modify a point in the graph is its coordinates are inside the range
         void Point(INT32 X, vector<INT32> Y, vector<GColorRGBInt> Color);
         void Point(INT32 X,        INT32  Y,  const GColorRGBInt& Color);
         void Point(INT32 X, vector<INT32> Y);
         void Point(INT32 X,        INT32  Y);

         void Clear();

         bool OnCreate();
         void OnResize();

         static const GString TypeName;

      protected:
         GGraphZone(void);
         ~GGraphZone(void);      
         friend class GUI::GManager;
         static GUI::GBaseObject* New();

      private:
         struct GGraphPoint
         {
            GGraphPoint(INT32 i_Y, const GColorRGBInt & i_Color) 
            {
               Y = i_Y; 
               Color = i_Color;
            }
            INT32 Y;
            GColorRGBInt Color;
         };

         typedef vector <GColorRGBInt> ColorVector;
         typedef vector <GGraphPoint>  PointVector;      
         typedef list   <PointVector>  GraphDataList;

         EGraphType    m_GraphType;
         UINT32        m_XInterval;
         UINT32        m_XRange;
         UINT32        m_YRange;
         INT32         m_YMax;
         INT32         m_XMin;
         INT32         m_YMin;
         bool          m_XShowAxis;
         bool          m_YShowAxis;
         bool          m_XAxisSpecialEffect;
         //bool          m_XShowValues;
         //bool          m_YShowValues;
         bool          m_YAutoScaleMax;
         bool          m_YAutoScaleMin;
         UINT32        m_XBandsGap;
         UINT32        m_NbFacetLists;
         GString       m_FontName;
         GColorRGBInt  m_FontColorNormal;
         GColorRGBInt  m_AxisColor;
         UINT32        m_FontSize;
         ColorVector   m_LastColor;
         GraphDataList m_Values;      

         GFX::GMesh*                m_GraphMesh;
         GFX::GMesh*                m_YAxisMesh;
         GFX::GMesh*                m_XAxisMesh;
         //CGFX_Text_2D_Itf*          m_TextMesh;

         CGFX_Model_Itf*            m_GraphModel;
         CGFX_Model_Itf*            m_YAxisModel;
         CGFX_Model_Itf*            m_XAxisModel;
         //CGFX_Model_Itf*            m_TextModel;

         GFX::GMaterial*            m_GraphMaterial;
         GFX::GMaterial*            m_YAxisMaterial;
         GFX::GMaterial*            m_XAxisMaterial;
         //GFX::GMaterial*            m_TextMaterial;

         CGFX_Transform_2D_Itf*     m_pGraphTransform;

         void Refresh();
         static PointVector GraphPointVector(const vector<INT32> & Y, const vector<GColorRGBInt> & Color);

      };
   }
};