/**************************************************************
*
* golem_gui_helper.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/


#include "golem_pch.hpp"
#include "../include/sp2_gui_helper.h"


GUI::GTreeBranch* GUIHelper::PrepareTreeItem(GUI::GTreeBranch* in_pNode, void* in_pNodeData,GString in_sName)
{
   gassert(in_pNode,"Node pointer is invalid");

   // Set Default tree node parameters
   ((GUI::GLabel*)in_pNode->Item())->ColorNormal(GColorRGBInt(0,0,0,0));
   ((GUI::GLabel*)in_pNode->Item())->Font(L"Lucida Sans Unicode",9);
   ((GUI::GLabel*)in_pNode->Item())->TextAlignment(L"MiddleLeft");
   ((GUI::GLabel*)in_pNode->Item())->TextOffset(2,0);
   ((GUI::GLabel*)in_pNode->Item())->FontColorNormal(GColorRGBInt(200,200,200,255));
   ((GUI::GLabel*)in_pNode->Item())->Text(in_sName);

   // Set Node Data
   in_pNode->Value(in_pNodeData);
   
   // return the node
   return in_pNode;
}


GTreeView* GUIHelper::CreateDefaultTreeView(GUI::GBaseObject *in_pParentOfTreeView,GUI::GBaseObject *in_pOwnerOfTreeView )
{
   // Create it
   GTreeView* l_pTreeView = NULL;
   l_pTreeView = CREATE_GUI_OBJECT_NAMED_VISIBLE(GTreeView, L"treeDesigns", in_pParentOfTreeView);   
   l_pTreeView->Owner(in_pOwnerOfTreeView);

   // Set default value for image, position, node size, etc...
   l_pTreeView->ItemType(L"g_def_label");
   l_pTreeView->Position(0,0);
   l_pTreeView->StartNode(0);
   l_pTreeView->ItemIndent(15);
   l_pTreeView->ItemHeight(15);
   l_pTreeView->ImageSize(16,16);
   l_pTreeView->SetImage(GUI::ETI_Branch,     L"images\\new\\tree_branch.RC#000225.tga");
   l_pTreeView->SetImage(GUI::ETI_Collapse,   L"images\\new\\tree_minus.RC#000223.tga");
   l_pTreeView->SetImage(GUI::ETI_Empty,      L"images\\new\\tree_empty.RC#000227.tga");
   l_pTreeView->SetImage(GUI::ETI_Expand,     L"images\\new\\tree_plus.RC#000231.tga");
   l_pTreeView->SetImage(GUI::ETI_Last,       L"images\\new\\tree_end.RC#000229.tga");
   l_pTreeView->SetImage(GUI::ETI_Leaf,       L"images\\new\\tree_leaf.RC#000235.tga");
   l_pTreeView->SetImage(GUI::ETI_Trunk,      L"images\\new\\tree_trunk.RC#000233.tga");
   l_pTreeView->ColorNormal(L"RGBA: 255, 0, 0",L"0");

   // Return this new baby.
   return l_pTreeView;
}   

