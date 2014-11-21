// wrapper for steam

#ifndef _Steam_Achievement_H_
#define _Steam_Achievement_H_

#pragma warning(disable : 4996)
#include "../../SteamSDK/public/steam/steam_api.h"
#pragma comment(lib, "steam_api.lib")

#define _ACH_ID( id, name ) { id, #id, name, "", 0 }
struct Achievement_t
{
	int m_eAchievementID;
	GSString m_pchAchievementID;
	GSString m_rgchName;
	GSString m_rgchDescription;
	bool m_bAchieved;
};

class CSteamAchievements
{
private:
   INT64 m_iAppID; // Our current AppID
   Achievement_t *m_pAchievements; // Achievements data
   int m_iNumAchievements; // The number of Achievements
   bool m_bInitialized; // Have we called Request stats and received the callback?

public:
   CSteamAchievements(Achievement_t *Achievements, int NumAchievements);
   ~CSteamAchievements()
   {
   }

   bool RequestStats();
   bool SetAchievement(const GSString &ID);

   STEAM_CALLBACK( CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived );
   STEAM_CALLBACK( CSteamAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored );
   STEAM_CALLBACK( CSteamAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored );
};


// local variables
#define NB_ACHIEVEMENTS    14

// Defining our achievements
enum EAchievements
{
	ACHIEVEMENT_1 = 0,
	ACHIEVEMENT_2 = 1,
	ACHIEVEMENT_3 = 2,
	ACHIEVEMENT_4 = 3,
	ACHIEVEMENT_5 = 4,
	ACHIEVEMENT_6 = 5,
	ACHIEVEMENT_7 = 6,
	ACHIEVEMENT_8 = 7,
	ACHIEVEMENT_9 = 8,
	ACHIEVEMENT_10 = 9,
	ACHIEVEMENT_11 = 10,
	ACHIEVEMENT_12 = 11,
	ACHIEVEMENT_13 = 12,
	ACHIEVEMENT_14 = 13,
};

class GSteamWrapper
{
private:
   bool mInitiated;
   bool mSteamPresent;   

public:
   GSteamWrapper();
   ~GSteamWrapper();

   GSString mLanguage;   // leveled down to those we support

   void Init();
   void Shutdown();
   void Iterate();

   void PassAchievement(int inID);

   bool Initiated() {return mInitiated;}
   bool SteamPresent() {return mSteamPresent;}
};

class GSteamWrapperServer
{
private:
   bool mAchievementState[NB_ACHIEVEMENTS];
public:
   GSteamWrapperServer()
   {
      for(int c = 0; c < NB_ACHIEVEMENTS; c++)
         mAchievementState[c] = false;
   }

   ~GSteamWrapperServer() {}

   void SetAchievementState(int inID, bool inState) {mAchievementState[inID] = inState;}
   bool GetAchievementState(int inID) {return mAchievementState[inID];}
};

#pragma warning(default: 4996)

#endif //_Steam_Achievement_H_