/**************************************************************
*
* sp2_design_variations_holder.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

CWSTR c_sGlobalDefaultModelNode = L"DEFAULT_MODEL";
CWSTR c_sTypeDefaultModelNode = L"DEFAULT_MODEL";
CWSTR c_sTypeIdNode = L"ID";
CWSTR c_sNameNode   = L"NAME";
CWSTR c_sTypePieceNode = L"PIECE";
CWSTR c_sPieceParentIdNode = L"PARENTID";
CWSTR c_sPieceColorizeNode = L"COLORIZE";
CWSTR c_sPieceParentNameNode = L"PARENTNAME";
CWSTR c_sPieceVariationNode = L"VARIATION";
CWSTR c_sVariationModelNode = L"MODEL";
CWSTR c_sVariationDefTxtrNode = L"DEFAULTTEXTURE";
CWSTR c_sVariationValidChildNode = L"VALIDCHILD";
CWSTR c_sVariationColorizeTxtrNode = L"COLORIZETEXTURE";

GDesignVariationHolder::GType::GPiece::GPiece()
{
   ParentID = c_iRootParentId;
}

GDesignVariationHolder::GType::GType()
{
   Memory::Clear(Pieces, c_iMaxDesignPieceCount);
}

GDesignVariationHolder::~GDesignVariationHolder()
{
   for (hash_map<UINT32, GType>::iterator itr = m_Types.begin(); itr != m_Types.end(); itr++)
   {
      SAFE_DELETE(itr->second.Pieces[0]);
      SAFE_DELETE(itr->second.Pieces[1]);
      SAFE_DELETE(itr->second.Pieces[2]);
   }
}

bool GDesignVariationHolder::Load(GFile& in_File)
{
   // Parse the unit design variation file
   CXML_Util l_XMLUtil;
   GXMLParser l_Parser;
   GTree<GXMLNode>* l_pTree = l_Parser.Parse(in_File);
   IF_RETURN(!l_pTree,false);

   CTreeXMLNode* l_Root = l_pTree->Root();
   CTreeXMLNode* l_pTypeNode = NULL;
   CTreeXMLNode* l_pPieceNode = NULL;
   CTreeXMLNode* l_pVarNode = NULL;

   for(UINT32 i0 = 0 ; i0 < l_Root->NbChilds() ; i0++)
   {
      if (l_Root->Child(i0)->Data().m_sName == c_sGlobalDefaultModelNode)
      {
         g_Joshua.FileManager()->File(l_Root->Child(i0)->Data().m_value, m_DefaultModel);
      }
      else
      {
         l_pTypeNode = l_Root->Child(i0);
         GDesignVariationHolder::GType l_CurType;
         INT32 l_iTypeID = -1;
         UINT32 l_PieceIdx = 0;
         for(UINT32 i1 = 0 ; i1 < l_pTypeNode->NbChilds() ; i1++)
         {
            if (l_pTypeNode->Child(i1)->Data().m_sName == c_sTypeDefaultModelNode)
            {
               g_Joshua.FileManager()->File(l_pTypeNode->Child(i1)->Data().m_value, l_CurType.DefaultModel);
            }
            else if (l_pTypeNode->Child(i1)->Data().m_sName == c_sTypeIdNode)
            {
               l_iTypeID = l_pTypeNode->Child(i1)->Data().m_value.ToINT32();
            }
            else if (l_pTypeNode->Child(i1)->Data().m_sName == c_sTypePieceNode)
            {
               if(l_PieceIdx < c_iMaxDesignPieceCount)
               {
                  l_pPieceNode = l_pTypeNode->Child(i1);
                  l_CurType.Pieces[l_PieceIdx] = new GDesignVariationHolder::GType::GPiece;
                  GDesignVariationHolder::GType::GPiece & l_CurPiece = *l_CurType.Pieces[l_PieceIdx];

                  for(UINT32 i2 = 0 ; i2 < l_pPieceNode->NbChilds() ; i2++)
                  {
                     if(l_pPieceNode->Child(i2)->Data().m_sName == c_sNameNode)
                     {
                        l_CurPiece.Name = g_ClientDAL.GetString(l_pPieceNode->Child(i2)->Data().m_value.ToINT32());
                     }
                     if (l_pPieceNode->Child(i2)->Data().m_sName == c_sPieceParentIdNode)
                     {
                        l_CurPiece.ParentID = l_pPieceNode->Child(i2)->Data().m_value.ToINT32() - 1;
                     }
                     if (l_pPieceNode->Child(i2)->Data().m_sName == c_sPieceColorizeNode)
                     {
                        l_CurPiece.Colorize = l_pPieceNode->Child(i2)->Data().m_value.ToINT32();
                     }
                     else if (l_pPieceNode->Child(i2)->Data().m_sName == c_sPieceParentNameNode)
                     {
                        l_CurPiece.ParentName = l_pPieceNode->Child(i2)->Data().m_value;
                     }
                     else if (l_pPieceNode->Child(i2)->Data().m_sName == c_sPieceVariationNode)
                     {
                        l_pVarNode = l_pPieceNode->Child(i2);
                        GDesignVariationHolder::GType::GPiece::GVariation l_Variation;
                        GString l_Path;
                        for(UINT32 i3 = 0 ; i3 < l_pVarNode->NbChilds() ; i3++)
                        {
                           if (l_pVarNode->Child(i3)->Data().m_sName == c_sVariationModelNode)
                           {
                              GString& l_ModelPath = l_pVarNode->Child(i3)->Data().m_value;
                              g_Joshua.FileManager()->File(l_ModelPath, l_Variation.Model);

                              INT32 l_iLastIndex = (INT32) l_ModelPath.size() - 1;
                              while(l_iLastIndex >= 0)
                              {
                                 WCHAR& l_CurChar = l_ModelPath[l_iLastIndex];
                                 if( (l_CurChar == '\\') || (l_CurChar == '/') )
                                 {
                                    l_Path = l_ModelPath.substr(0, l_iLastIndex + 1);
                                    break;
                                 }
                                 l_iLastIndex --;
                              }
                           }
                           else if (l_pVarNode->Child(i3)->Data().m_sName == c_sVariationDefTxtrNode)
                           {
                              l_Variation.DefaultTexture = l_pVarNode->Child(i3)->Data().m_value;
                           }
                           else if (l_pVarNode->Child(i3)->Data().m_sName == c_sVariationValidChildNode)
                           {
                              l_Variation.Connectable.push_back(l_pVarNode->Child(i3)->Data().m_value.ToINT32() - 1);
                           }
                        }
                        for(UINT32 i3 = 0 ; i3 < l_pVarNode->NbChilds() ; i3++)
                        {
                           if (l_pVarNode->Child(i3)->Data().m_sName == c_sVariationColorizeTxtrNode)
                           {
                              GString& l_TxtrPath = l_pVarNode->Child(i3)->Data().m_value;
                              if( (l_TxtrPath[0] != '\\') && (l_TxtrPath[0] != '/') )
                              {
                                 l_TxtrPath = l_Path + l_TxtrPath;
                              }
                              if(!g_Joshua.FileManager()->File(l_TxtrPath, l_Variation.ColorizeTexture) )
                              {
#ifdef GOLEM_DEBUG
                                 g_Joshua.Log(GString(L"Texture cannot be loaded ") + l_TxtrPath);
#endif
                              }
                           }
                        }
                        l_CurPiece.Variations.push_back(l_Variation);
                     }
                  }
                  l_PieceIdx++;
               }
            }
         }
         if(l_iTypeID != -1)
         {
            m_Types[l_iTypeID] = l_CurType;
         }
      }
   }
   return true;
}

GDesignVariationHolder::GType* GDesignVariationHolder::Type(UINT32 in_iTypeID)
{
   hash_map<UINT32, GType>::iterator l_It = m_Types.find(in_iTypeID);
   if(l_It != m_Types.end() )
   {
      return &(l_It->second);
   }

   return NULL;
}
