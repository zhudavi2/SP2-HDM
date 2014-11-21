/**************************************************************
*
* golem_clientai.h
*
* Description
* ===========
*  Represents an AI Client on the server side, so the AI can receive game events as the other
*  clients
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CLIENTAI_H_
#define _GOLEM_CLIENTAI_H_


namespace SDK
{

   /*!
 * Represents an AI client.  Equivalent to the GGenericNetworkClient for a human player on the
 * server side
 **/
class GOLEM_SDK_API GClientAI : public GGameClientItf
{
public:
   GClientAI();
   ~GClientAI();


   //! Initialize(): Inits the game client
   GAME_MSG Initialize();
   //! Iterates, called every iteration
   GAME_MSG Iterate(void* param);
   //! shutdown(): Shuts down the game client
   GAME_MSG Shutdown();

protected:
private:
};
};
#endif //_GOLEM_CLIENTAI_H_
