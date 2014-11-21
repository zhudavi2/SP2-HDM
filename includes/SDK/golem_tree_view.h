/**************************************************************
*
* golem_tree_view.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#pragma once

#pragma warning (push)
#pragma warning (disable : 4355) // 'this' : used in base member initializer list

namespace SDK
{
   namespace GUI
   {      
      class GOLEM_SDK_API GTreeBranches : protected vector <class GTreeBranch *>
      {
      private:
         friend class GTreeBranch;

         GTreeBranches(GTreeBranch * node) 
         {
            m_node = node;
         }
         GTreeBranches(){;}

         GTreeBranch* m_node;

         GTreeBranches(const GTreeBranches &) {;}
         GTreeBranches & operator = (const GTreeBranches &) {return *this;}
      
      public:
         typedef vector<class GTreeBranch *>::const_iterator const_iterator;
         typedef vector<class GTreeBranch *>::iterator iterator;

         GTreeBranches::iterator begin()               { return __super::begin(); }
         GTreeBranches::iterator end()                 { return __super::end(); }
         GTreeBranches::const_iterator begin() const   { return __super::begin(); }
         GTreeBranches::const_iterator end()   const   { return __super::end(); }

         void add(GTreeBranch*, GTreeBranch* InsertBefore = NULL);    
         void erase(GTreeBranch*);        
         void erase_at(difference_type);
         void clear();
         //! Find a GTreeBranch with a specific value, NULL is returned if the value couldn't be found.
         GTreeBranch* FindNodeWithValue(GTreeBranch* in_pRootSearch,void * in_pValue);

         reference operator [](difference_type idx)
         {
            return vector<GTreeBranch*>::operator[](idx);
         }

         size_type size()
         {
            return __super::size();
         }
      };


      class GOLEM_SDK_API GTreeBranch
      {
      private:
         friend class GTreeView;
         friend class GTreeBranches;

         GTreeBranch(const GTreeBranch &) {;}
         void operator = (const GTreeBranch &) {;}

         void ExpandCollapseInternal(bool in_Expand, bool in_Recurse);

         GTreeView         * m_View;
         GTreeBranch       * m_pParentNode;
         void              * m_pValue;
         GUI::GBaseObject  * m_Item;

         bool           m_Expanded;
         GTreeBranches  m_Nodes;

      protected:
         // private constructors
         GTreeBranch(GUI::GBaseObject * in_Item) 
            : m_Nodes(this), m_Item(in_Item) {}
         GTreeBranch(GUI::GBaseObject * in_Item, GTreeView * in_View) 
            : m_Nodes(this), m_Item(in_Item), m_pParentNode(NULL), m_View(in_View), m_pValue(NULL) {}

         void Modified();

      public:
         GUI::GBaseObject * Item() {return m_Item;};
         virtual ~GTreeBranch();

         const GTreeBranches & Nodes() const {return m_Nodes;}
         GTreeBranches & Nodes() {return m_Nodes;}

         void Collapse(bool in_Recurse = false);
         void Expand(bool in_Recurse = false);   
         void Toggle();
         bool Expanded() {return m_Expanded;};
         
         void * Value();
         void Value(void* in_pValue);
         
         GTreeBranch* ParentBranch();
         
         GTreeView* View() {return m_View;};
      };      

      enum ETreeImages
      {
         ETI_Empty,
         ETI_Expand,
         ETI_Collapse,
         ETI_Leaf,
         ETI_Branch,
         ETI_Trunk,
         ETI_Last
      };


      class GOLEM_SDK_API GTreeView : public GUI::GFrame, protected GTreeBranch
      {
      private:
         friend class GTreeBranch;
         friend class GUI::GManager;

         static GUI::GBaseObject* New();

         void Collapse(bool recurse = false) {;};
         void Expand  (bool recurse = false) {;};
         bool Expanded() {return true;};
         void Toggle() {;};

         void DoLayout(GTreeBranch * BaseNode, UINT16 & r_LayoutTop, UINT16 indent, bool parentExpanded, vector<ETreeImages> & in_States);

         GTreeBranch * FindNode(UINT32 & TTL, GTreeBranch * = NULL);

         GFX::GMeshComplex *        m_TreeMesh;
         CGFX_Model_Itf *           m_TreeModel;
         CGFX_Texture_Itf *         m_Textures[ETI_Last+1];
         CGFX_Transform_2D_Itf *    m_Transform;
         GFX::GMaterial *           m_Material;
         GString                    m_Images[ETI_Last+1];
         GString                    m_NodeType;
         GTreeBranch *              m_SelectedNode;         
         GVector2D<UINT32>          m_ImageSize;
         INT16                      m_ItemHeight;
         INT16                      m_ItemIndent;
         INT16                      m_iNbVisibleNode;
         INT16                      m_iStartNode;
         POINT                      m_GridSize;
         bool                       m_ShowPlusMinus;            
         bool                       m_bDestroyingObject;
         bool                       m_bStretchImageX;
         bool                       m_bStretchImageY;    
         bool                       m_bFlatTreeView;

      protected:
         GTreeView();           

      public:      
         static const GString TypeName;

         GTreeBranch *  CreateNode();
         INT32          CountVisible();
         bool           OnCreate();
         bool           OnDestroy();
         void           CountVisibleRecursive(INT32 & in_iNb, GTreeBranch* in_pTreeNode);
         void           FlatTreeView(bool in_bEnabled);
         void           ImageSize(INT32 in_iX, INT32 in_iY);
         void           ItemHeight(INT16);
         void           ItemIndent(INT16);
         void           ItemType(const GString&);         
         void           NbVisibleNode(INT16);
         void           OnResize();
         void           SelectedNode(GTreeBranch *,bool in_bNodeDoubleClicked = false);
         void           Show();
         void           ShowPlusMinus(bool);
         void           StartNode(INT16);                        
         void           StretchImage(bool in_bX, bool in_bY);
         bool           Update();                         
                         
         void           SetImage(ETreeImages img, const GString & file);

         EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData &, GBaseObject *);

         GTreeBranches &   Nodes()           {return GTreeBranch::Nodes();};
         INT16             ItemHeight()      {return m_ItemHeight;};
         INT16             ItemIndent()      {return m_ItemIndent;};
         INT16             NbVisibleNode()   {return m_iNbVisibleNode;}
         INT16             StartNode()       {return m_iStartNode;}
         bool              ShowPlusMinus()   {return m_ShowPlusMinus;};
         GTreeBranch *     SelectedNode()    {return m_SelectedNode;};
         bool              DestroyingObject(){return m_bDestroyingObject;};


      };
   }
}
#pragma warning (pop)
