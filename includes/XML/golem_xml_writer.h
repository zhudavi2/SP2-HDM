#ifndef _GOLEM_XML_WRITER___
#define _GOLEM_XML_WRITER___

#include "golem_xml.h"
#include "../../includes/hector/hector.h"

class GXMLWriter
    {
    FILE* m_pFilePtr;
public:
    GXMLWriter();
    ~GXMLWriter();

    /*********************************
     * Writes a XML file from a GTreeNode<GXMLNode>
     *
     *  It writes the node and all its Childrens into the 
     *  file pointed by the filename X.
     *
     *  If the file exists, it is overwritten
     *
     *  The fact that the param is a nodePtr instead of a treePtr
     *  allows to write partial parts of an XML file into another 
     *  xml file
     *********************************/
    bool write(const GString & filename,GTreeNode<GXMLNode>* nodePtr);

private:
    void writeNode(GTreeNode<GXMLNode>* nodePtr);

    };
#endif