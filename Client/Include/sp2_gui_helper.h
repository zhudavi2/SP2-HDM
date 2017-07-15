/**************************************************************
*
* sp2_gui_helper.h
*
* Description
* ===========
* Implement some helper function for eaiser gui management.
*
* Owner
* =====
*  Jonathan Mecier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Include Guards
#ifndef _SP2_GUI_HELPER_H
#define _SP2_GUI_HELPER_H

namespace SP2
{

   /*!
   * Different helpers to share between system that uses UI elements.
   **/
   namespace GUIHelper
   {

      /*!
       * Build a tree view with all the default parameter
       * @param   in_pParentOfTreeView    : The parent of the new tree view
       * @param   in_pOwnerOfTreeView     : The ownder of the new tree view
       * @return  GUI::GTreeView*         : The new tree view
       **/
      GUI::GTreeView* CreateDefaultTreeView(GUI::GBaseObject *in_pParentOfTreeView,GUI::GBaseObject *in_pOwnerOfTreeView );

      
      /*!
       * Prepare a tree item given a node, pointer and string
       * @param   in_pNode       : The node to format or prepare
       * @param   in_pNodeData   : A pointer to the associate data for this node
       * @param   in_sName       : The name to be display on the node.
       * @return  GTreeBranch*   : The 'formated' node passed in parameter
       **/
      GUI::GTreeBranch* PrepareTreeItem(GUI::GTreeBranch* in_pNode, void* in_pNodeData,GString in_sName);

   }
}

#endif // _SP2_GUI_HELPER_H