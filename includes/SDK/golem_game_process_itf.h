
#include "golem_sdk_api.h"

#ifndef SDK_GAME_PROCESS_ITF
#define SDK_GAME_PROCESS_ITF

namespace SDK
{
   class GJoshua; 

   enum GAME_MSG
   {
      GAME_ERROR = 0,
      GAME_OK,
      GAME_NOT_IMPLEMENTED,
      GAME_FATAL_ERROR,
      GAME_INIT_ERROR,
      GAME_EVENT_FORWARD_ERROR,
      GAME_ERROR_CONNECTION_LOST,
      GAME_UNDEFINED_ERROR
   };

   namespace Event
   {

      /*!
      * Defines a class that can Process events, the add
      * event adds an event to the object event queue, which
      * can be accessed later
      */
      class GOLEM_SDK_API GGameEventProcessor 
      {
         friend class GJoshua;
      protected:

         class GHandler
         {
         public:
            CALLBACK_HANDLER_bool_spGE Function;
            void*                      Instance;
            GGameEventSPtr             Event;
         };

         multimap<INT64,GGameEventProcessor::GHandler> m_EventHandlerQueue;
         GMutex m_mutex;

         GAME_MSG EnqueueEvent(GGameEventSPtr);      

         //! Pop a game event and call its handler
         bool  PopAndCall(INT32 in_iSpecificTarget = -1);

         //! Get a game event from the queue
         bool  PopEvent(GGameEventSPtr& out_Event);
      };
   }

   /*!
   * Defines a game Process, which is an object that must be initialized,
   * and shutted down. It also has a location (Process location, for example the .DLL location)
   **/
   class GOLEM_SDK_API GGameProcessItf : public Event::GGameEventProcessor
   {
   protected:    
      //!path where the dll is located
      GString m_location;

   public:
      /*!
      Initialize(): Inits the game client. 
      Receives a pointer to the game engine so it can
      be used later on.
      */
      virtual GAME_MSG Initialize() = 0;
      virtual GAME_MSG Shutdown()   = 0;	
      virtual GAME_MSG Iterate(void* params) = 0;

      GString location();

      void setLocation(GString locat);

   };
}
#endif