/**************************************************************
*
* golem_ehe_globals.h
*
* Description
* ===========
*  Utility functions and constants used by the EHE.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _EHE_GLOBALS
#define _EHE_GLOBALS

#include "../includes/xml/golem_xml.h"
#include "golem_sdk_api.h"

using namespace stdext;

namespace EPlanStatus
{
	enum Enum
	{
		Ready,
		Cancel,
		Hold,
		Undefined
	};
};

namespace SDK
{
   namespace EHE
   {

      //---------------------------------------------------------
      //  Macros
      //---------------------------------------------------------

      //
      // conditional inclusion of a group of statements
      //
#ifdef _DEBUG
#define EXEC_DEBUG( statements ) statements
#else
#define EXEC_DEBUG( statements )
#endif

      //---------------------------------------------------------
      //  Type definitions
      //---------------------------------------------------------

      typedef  UINT32   ACTION_ID;
      typedef  UINT32   ENTITY_ID;
      typedef  UINT32   OBJECTIVE_ID;

      typedef  REAL64   TIME;                // game clock time
      typedef  REAL64   TIME_SPAN;           // time interval in game clock units

      typedef  REAL32   EXPERIENCE_LEVEL;    // measures an entity's experience for a given action-objective pair
      typedef  REAL32   STRENGTH;            // mesaures the strength of an action
      typedef  REAL32   OBJECTIVE_LEVEL;     // satisfaction level for an objective ( -1 < value < 1 )
      typedef  REAL32   INERTIA_LEVEL;       // measures an entity's reluctance to update its experience
      typedef  REAL32   PRIORITY_LEVEL;      // measures the importance accorded to an objective


      //---------------------------------------------------------
      //  Constants
      //---------------------------------------------------------

      //
      // XML tags
      //
      namespace XMLTags
      {
         const GString  EHE_ENTITIES               =  "EHE_ENTITIES";
         const GString  DEFAULT_SETTINGS           =  "DEFAULT_SETTINGS";
         const GString  ENTITY_LIST                =  "ENTITY_LIST";
         const GString  ENTITY                     =  "ENTITY";
         const GString  OBJECTIVE_REVIEW_PERIOD    =  "OBJECTIVE_REVIEW_PERIOD";
         const GString  PRIORITY_LIST              =  "PRIORITY_LIST";
         const GString  PRIORITY                   =  "PRIORITY";
         const GString  ID                         =  "ID";
         const GString  NAME                       =  "NAME";
         const GString  OBJECTIVE                  =  "OBJECTIVE";
         const GString  ACTION                     =  "ACTION";
         const GString  INERTIA                    =  "INERTIA";
         const GString  EXPERIENCE_LIST            =  "EXPERIENCE_LIST";
         const GString  EXPERIENCE                 =  "EXPERIENCE";
         const GString  MAX_PLAN_DEPTH             =  "MAX_PLAN_DEPTH";
      }


      //
      // pre-defined strings used when logging
      //
      const GString  NEWLINE  =  "\r\n\t";
      const GString  TAB      =  "\t";
      const GString  COMMA    =  ", ";


      //---------------------------------------------------------
      //  Structures
      //---------------------------------------------------------

      struct GOLEM_SDK_API GCondition    // represents a prerequisite to the triggering of an action
      {
         GCondition();
         GCondition( OBJECTIVE_ID in_iObjective, OBJECTIVE_LEVEL in_fLevel );

         OBJECTIVE_ID      ObjectiveID;
         OBJECTIVE_LEVEL   Level;
      };


      //---------------------------------------------------------
      //  Templates
      //---------------------------------------------------------

      //
      // Deep copy of a basic container of pointers
      //
      template <class ContainerType, class ItemType>
         void DeepCopyBasicContainer( ContainerType& out_Destination, const ContainerType& in_Source )
      {
         out_Destination.clear();
         out_Destination.insert( out_Destination.end(), in_Source.begin(), in_Source.end() );

         for ( ContainerType::iterator i = out_Destination.begin(); i != out_Destination.end(); ++i )
         {
            *i = new ItemType( **i );
         }
      }

      //
      // Deep copy of a vector of pointers
      //
      template <class ItemType>
         void DeepCopyVector( vector<ItemType*>& out_Destination, const vector<ItemType*>& in_Source )
      {
         DeepCopyBasicContainer<vector<ItemType*>, ItemType>( out_Destination, in_Source );
      }

      //
      // Deep copy of a list of pointers
      //
      template <class ItemType>
         void DeepCopyList( list<ItemType*>& out_Destination, const list<ItemType*>& in_Source )
      {
         DeepCopyBasicContainer<list<ItemType*>, ItemType>( out_Destination, in_Source );
      }

      //
      // Deep cleaning of a basic container of pointers
      //
      template <class ContainerType>
         void DeepCleanBasicContainer( ContainerType& io_ContainerToClean )
      {
         for ( ContainerType::iterator i = io_ContainerToClean.begin(); i != io_ContainerToClean.end(); ++i )
         {
            delete *i;
         }
         io_ContainerToClean.clear();
      }

      //
      // Deep copy of a keyed container of pointers
      //
      template <class ContainerType, class KeyType, class ItemType>
         void DeepCopyKeyedContainer( ContainerType& out_Destination, const ContainerType& in_Source )
      {
         out_Destination.clear();

         for ( ContainerType::const_iterator i = in_Source.begin(); i != in_Source.end(); ++i )
         {
            out_Destination[ i->first ] = new ItemType( *( i->second ) );
         }
      }

      //
      // Deep copy of a hash_map of pointers
      //
      template <class KeyType, class ItemType>
         void DeepCopyHashMap( hash_map<KeyType, ItemType*>& out_Destination, const hash_map<KeyType, ItemType*>& in_Source )
      {
         DeepCopyKeyedContainer<hash_map<KeyType, ItemType*>, KeyType, ItemType>( out_Destination, in_Source );
      }

      //
      // Deep copy of a map of pointers
      //
      template <class KeyType, class ItemType>
         void DeepCopyMap( map<KeyType, ItemType*>& out_Destination, const hash_map<KeyType, ItemType*>& in_Source )
      {
         DeepCopyKeyedContainer<map<KeyType, ItemType*>, KeyType, ItemType>( out_Destination, in_Source );
      }

      //
      // Deep cleaning of a keyed container of pointers
      //
      template <class ContainerType>
         void DeepCleanKeyedContainer( ContainerType& io_ContainerToClean )
      {
         for ( ContainerType::iterator i = io_ContainerToClean.begin(); i != io_ContainerToClean.end(); ++i )
         {
            delete i->second;
         }
         io_ContainerToClean.clear();
      }

      //
      // Bound quantity within specified limits
      //
      template <class Type>
         Type Bound( const Type& in_Argument, const Type& in_LowerLimit, const Type& in_UpperLimit )
      {
         if ( in_Argument < in_LowerLimit )
         {
            return in_LowerLimit;
         }
         else if ( in_Argument > in_UpperLimit )
         {
            return in_UpperLimit;
         }
         else
         {
            return in_Argument;
         }
      }


      //---------------------------------------------------------
      //  Functions
      //---------------------------------------------------------

      //
      // bound input values
      //
      GOLEM_SDK_API OBJECTIVE_LEVEL BoundObjectiveLevel( OBJECTIVE_LEVEL in_fObjectiveLevel );
      GOLEM_SDK_API INERTIA_LEVEL BoundInertiaLevel( INERTIA_LEVEL in_fInertiaLevel );
      GOLEM_SDK_API PRIORITY_LEVEL BoundPriorityLevel( PRIORITY_LEVEL in_fPriorityLevel );

   }//end namespace EHE
}; //end namespace SDK

#endif