#ifndef _GOLEM_XML_
#define _GOLEM_XML_
#pragma once
#include "../../includes/hector/hector.h"

class GXMLNode;
class GXMLAttribute;
#include "golem_xml_util.h"
#include "golem_xml_writer.h"

template<class T>
class Hector::GTree;

/**************************
 * XML attribute class
 *      Represents an attribute inside of an xml statement
 *
 *  Ex : <ROGER someText="Mathieu">Bien le bonjour</ROGER>
 *
 *      In that NODE the attribute name is : someText
 *             and the attribute value is  : Mathieu
 **************************/
class GXMLAttribute
    {
public:
    GXMLAttribute();
    GXMLAttribute( const GString& in_sName, const GString& in_sValue );
    
    GString m_sName;
    GString m_value;
    };

/***********************************
 * Class to represent a XML Node
 *
 * Ex:
 *  Ex : <ROGER someText="Mathieu" age=12>Bien le bonjour</ROGER>
 *
 *  Node name : ROGER
 *  Node Value: Bien le bonjour
 *  Node attribute list:
 *          1st attribute : Name = someText  value = Mathieu
 *          2nd attribute : Name = age       value = 12
 *
 ***********************************/
class GXMLNode
    {
public:
    GXMLNode();
    GXMLNode( const GString& in_sName, const GString& in_sValue );

    GString m_sName;
    GString m_value;
    vector<GXMLAttribute> m_attributeList;

    INT32 nbAttributes() const {return (INT32) m_attributeList.size();}
    bool Attribute( const GString& in_sAttributeName, GString& out_sAttributeValue ) const;
    };


class XercesDOMParser;
class DOMNode;
/****************************************************
 * GXMLParser :
 *     Reads a XML documents and builds a tree corresponding to
 *     it.
 ****************************************************/
class GXMLParser
    {
    bool init();
    XercesDOMParser *m_parser;

    bool parseNode(DOMNode* node);
    bool printNode(GTreeNode<GXMLNode>* node);
    bool m_firstNode;
public:
    GXMLParser();
    ~GXMLParser();

    void  printTree(GTree<GXMLNode>* tree);

    /**************************
     * Parse the xml file and returns a GTree 
     *  corresponding to its structure
     *************************/
    GTree<GXMLNode>* Parse(const GString & filename);
    GTree<GXMLNode>* Parse(GFile& in_File);


    void reset()
        {
        m_firstNode = true;
        init();
        }
private:
    };

#ifdef _DEBUG
#   pragma comment(lib,"libxml_d.lib")
#   pragma comment(lib,"xerces-c_2d.lib")
#endif
#ifdef NDEBUG
#   pragma comment(lib,"libxml.lib")
#   pragma comment(lib,"xerces-c_2.lib")
#endif



#endif