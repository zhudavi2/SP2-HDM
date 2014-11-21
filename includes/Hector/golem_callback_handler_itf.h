/**************************************************************
*
* golem_callback_handler_itf.h
*
* Description
* ===========
*  A generic callback handler interface with most used callback defined
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CALLBACK_HANDLER_
#define _GOLEM_CALLBACK_HANDLER_

namespace Hector
{

   /*!
   * Class template to declare function pointers to the same class
   **/
   class GCallbackHandlerItf
   {
   };

   typedef void    (GCallbackHandlerItf::*CALLBACK_HANDLER_void_void)(void);
   typedef bool    (GCallbackHandlerItf::*CALLBACK_HANDLER_bool_void)(void);
   typedef bool    (GCallbackHandlerItf::*CALLBACK_HANDLER_bool_crGS)(const GString &);
   typedef GString (GCallbackHandlerItf::*CALLBACK_HANDLER_GS_crGS_cvrGS)(const GString &, const vector<GString> &);
   typedef void    (GCallbackHandlerItf::*CALLBACK_HANDLER_void_uint)(UINT32);   
   typedef bool    (GCallbackHandlerItf::*CALLBACK_HANDLER_bool_pvoid)(void *);
   typedef void    (GCallbackHandlerItf::*CALLBACK_HANDLER_void_pvoid)(void *);
   typedef void    (GCallbackHandlerItf::*CALLBACK_HANDLER_void_int_pvoid)(int, void*);   

   #define VALID_HANDLER(Instance, FuncName) (Instance && FuncName)
   #define CALL_HANDLER(Instance, FuncName) (((GCallbackHandlerItf*)Instance)->*FuncName)
}

#endif
