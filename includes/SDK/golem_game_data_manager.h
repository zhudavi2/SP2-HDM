/**************************************************************
*
* golem_game_data_manager.h
*
* Description
* ===========
*  Description of class GGameDataManager, which is responsible
*  for save and load of game data to/from disk.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GAME_DATA_MANAGER_H_
#define _GOLEM_GAME_DATA_MANAGER_H_

namespace SDK
{
   // Forward declarations
   class GGameDataNode;

   namespace EGameDataError
   {
      enum Enum
      {
         NoError,
         CantOpenFile,
         CantCreateFile,
         InvalidGameType,
         InvalidDataVersion,
         NewGameError,
         LoadGameError,
         SaveGameError,
         ItemCount,
      };
   }

   /*!
    * Class that manages the save and load of game data.
    * Save and load is done via game data nodes, which are
    * serialized to or from  a file in the order they are registered
    * to this manager.
    **/
   class GOLEM_SDK_API GGameDataManager
   {
   public:
      GGameDataManager();
      ~GGameDataManager();

      //! Add a game data container (node) that must be loaded/saved.
      void RegisterNode(GGameDataNode* in_pNewNode);

      //! Remove a game data container (node).
      bool UnregisterNode(GGameDataNode* in_pNode);

      //! Save the current game content to a file.
      EGameDataError::Enum SaveGame(const GString& in_sSaveGameFileName, UINT32 in_iGameType, UINT32 in_iDataVersion);

      //! Load the game content from a file.
      EGameDataError::Enum LoadGame(const GString& in_sLoadGameFileName, UINT32 in_iGameType, UINT32 in_iDataVersion);

      //! Create a new game from a database.
      EGameDataError::Enum NewGame(GDatabase* in_pDatabase);

      //! Clean all game data.
      EGameDataError::Enum CleanGame();

   private:
      vector<GGameDataNode*>        m_RegisteredNodes;
   };

   class GOLEM_SDK_API GGameDataNode
   {
   public:
      //! Serialize the game data content to an output buffer
      virtual bool OnSave(GIBuffer& io_Buffer);

      //! Unserialize the game data content from an input buffer
      virtual bool OnLoad(GOBuffer& io_Buffer);

      //! Load the game data content from the engine database
      virtual bool OnNew(GDatabase* in_pDatabase);

      //! Clean game data
      virtual void OnClean();
   };

}

#endif // #ifndef _GOLEM_GAME_DATA_MANAGER_H_
