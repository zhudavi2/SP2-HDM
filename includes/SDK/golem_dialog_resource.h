#include "golem_sdk_api.h"

#ifndef _GOLEM_DIALOG_RESOURCE_H_
#define _GOLEM_DIALOG_RESOURCE_H_

/*!
GDO = GolemDialoGBaseObject

Hierarchy: 

GDOResourceReader
Definitions
Key=>Definition
Properties
Key=>Value           
*/
namespace SDK
{
   namespace GUI
   {
      class GDODefClass;

      typedef stdext::hash_map<GString, GString>            GDOProperties;  // first:prop. name; second: prop. value
      typedef GDOProperties::value_type                     GDOProperty;    // a key/value pair. -> first:prop. name; second: prop. value
      typedef GDOProperties::const_iterator                 GDOPropertiesCItr;
      typedef GDOProperties::iterator                       GDOPropertiesItr;
      typedef GSmartPtr<GDODefClass>                        GDODefClass_SPtr; //Smart pointer to a GDODefClass
      typedef stdext::hash_map<GString, GDODefClass_SPtr>   GDODefinitions;
      typedef GDODefinitions::value_type                    GDODefinition;
      typedef GDODefinitions::const_iterator                GDODefinitionsCItr;
      typedef GDODefinitions::iterator                      GDODefinitionsItr;

      /*!
      *	Defines a GUI object.
      *		A GUI Object has a Name, a Type (which must be known by the GUI Manager so it can
      *		create an instance), a list of properties and a list of Childs.
      */
      class GOLEM_SDK_API GDODefClass
      {   
         friend class GDOResourceReader;
      public:
         GDODefClass();
         ~GDODefClass();

         GString Name;
         GString Type;
         GString Base;
         GString ZOrder;
         GString Id;
         GDOProperties Properties;
         GDODefinitions Childs;


         GDODefClass_SPtr Child(const GString& in_sName)
         {
            GDODefinitionsItr l_It = Childs.find(in_sName);
            if(l_It != Childs.end())
            {
               return l_It->second;
            }
            else
            {
               return GDODefClass_SPtr(NULL);
            }
         }


         GDODefClass_SPtr Ancestor;

         /*!
         * clear the content of the hash map
         **/
         void clear();

      private:
         //! Update the actual class definition properties and Childs with the current object
         bool Update(GDODefClass* in_pUpdateWith);
      };

      /*!
      *	GDOResourceReader
      *		Reads the resource definition file and returns the definitions of the 
      *		objects it finds there.
      */
      class GOLEM_SDK_API GDOResourceReader
      {
         //Members
         GDODefinitions m_definitions;   
      public:
         GDOResourceReader();
         ~GDOResourceReader(void);
         //! Parse the resource file
         bool ParseResources(const GString &filename); 
         //!Get the definitions of the objects found in the resource file
         const GDODefinitions & GetDefinitions() {return m_definitions;}    

         bool reset();

         //!Checks if the base class is one of the generic object
         static bool	IsTypeGeneric(const GString &baseType);


      private:
         //! get the properties for one object in the XML file
         GDOProperties	getObjectProperties(GTreeNode<GXMLNode>* objectNode);
         // get the Childs for one object in the XML file
         GDODefinitions  GetObjectChilds(GTreeNode<GXMLNode> *, stdext::hash_map<GString, GDOProperties*>, GDODefClass_SPtr);
         //!Recursively creates dialog object definition
         GDODefClass*    CreateObject(GTreeNode<GXMLNode>*, GDOProperties*, GDODefClass_SPtr, bool);
      };
   }
}


#endif