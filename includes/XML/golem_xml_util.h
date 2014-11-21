#ifndef _GOLEM_XML_UTIL_
#define _GOLEM_XML_UTIL_

#include "../../includes/hector/hector.h"
#include "golem_xml.h"

typedef GTreeNode<GXMLNode> CTreeXMLNode;

class CXML_Util
	{
	bool init();
	_int32 LevelDepth;
public:
    CXML_Util();
    ~CXML_Util();


    /***********************************************
     * Print_Tree
     ***********************************************/
    bool Print_Tree(const CTreeXMLNode* node);
    bool Print_TreeChild(const CTreeXMLNode* ChildNode,INT16 place);



    /***********************************************
     * Search_Node
     ************************************************/
	const CTreeXMLNode* Search_Const_Node(const CTreeXMLNode* TreeNode, 
                                         const GString & element_name,
                                         const GString & element_value,
                                         const vector<GXMLAttribute>* attrVector,
                                         INT32 maxLevelsearch);

   CTreeXMLNode* Search_Node(CTreeXMLNode* TreeNode, 
                             const GString & element_name,
                             const GString & element_value,
                             const vector<GXMLAttribute>* attrVector,
                             INT32 maxLevelsearch);


/*!*********************************************************
 * Gets the value of a XML Node
 *	The function looks inside the @node_to_search@
 *
 *	returns false if the node doesnt exists
 ***********************************************************/
	bool Get_XML_Node_Value(const GTreeNode<GXMLNode>* node_to_search,
						    GString nodeName,
							GString &outValue);



private:
    //Recursive function that Process the tree 
   const  CTreeXMLNode*  recursedSearch(const CTreeXMLNode* node,
                                        const GString & elementName,
                                        const GString & element_value, 
                                        const vector<GXMLAttribute>* attrVector,
                                        INT32 deepLevel);


    //Compare one node
    bool            compareNode(const CTreeXMLNode* node,
                                const GString & elementName,
                                const GString & element_value, 
                                const vector<GXMLAttribute>* attrVector);

                        

    //Search all the Childs of one node for the given values
    const CTreeXMLNode*  search1Level(const CTreeXMLNode* node,
                                      const GString & elementName,
                                      const GString & element_value,
                                      const vector<GXMLAttribute>* attrVector);

	bool checkElemName(const GString & elementName, 
					       const CTreeXMLNode* ChildNode);
	
	bool checkElemValue(const GString & elementValue,
		                 const CTreeXMLNode* ChildNode);

	bool checkAttrList(const vector<GXMLAttribute>* attrVector, 
					       const CTreeXMLNode* ChildNode, 
					       INT32 nbVectorAttr,
					       INT32 nbNodeAttr);




	};

#endif




