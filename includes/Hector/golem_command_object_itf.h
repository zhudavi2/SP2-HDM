/******************************************************

golem_command_object_itf.h

Owner : Germain Sauve

*******************************************************/

#ifndef _GOLEM_COMMAND_OBJECT_ITF_
#define _GOLEM_COMMAND_OBJECT_ITF_

#include "golem_string.h"

#define k_Command_Type_None         0x00
#define k_Command_Type_Function     0x01
#define k_Command_Type_Proc         0x02


typedef class CGolem_Command_Manafer *CGolem_Command_Manafer_Ptr;

typedef class CGolem_Command_object_Itf
    {
public:
    virtual bool Command( long pComandType, GString pCommand, GString pReturn ) = 0;
    virtual bool Register( CGolem_Command_Manafer_Ptr pManager )= 0;
    virtual bool Unregister() = 0;
    } *CGolem_Command_object_Itf_Ptr;

#endif //#ifndef _GOLEM_COMMAND_OBJECT_ITF_