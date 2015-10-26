SuperPower 2 Human Development Modification (SP2-HDM) V3

V3 changes:
- Configuration XML file added, SP2-HDM_Config.xml. Modifiable settings include:
 - Global tax limit
 - Income tax limits per government type
 - Server message to be displayed to players who join
 - "Naval Rule" disablement
 - Percentage of regions occupied in order to be able to use nuclear weapons
 - Resource tax limit
 - HDI component (LE, MYS, EYS) viewing through the country info window
- When a country creates a war treaty and is on the attacking side, in the war list window it will be listed as the leader of the attacking side. Previously, the leader of the attacking side wouldn't always be the treaty creator even if the creator were on the attacking side.
- Multiple covert cell creation is fixed from V2.

V2 changes:
- Internet multiplayer now works. However, some minor UI-related features had to be removed in order to get it to work.
- Every 365 days of game time, the HDI, LE, MYS, and EYS of every active country will be written to the server console log.
- Fixed an issue where during the logging out of stats to the server console at the beginning of the game, countries with 0 population would have their GDP per capita calculations messed up; such countries would now be listed as having $0 GDP per capita.

Removed features in V2:
- In the country information window, your numerical HDI, LE, MYS, and EYS will no longer be shown.
- In the economic health window, your "income index" will no longer be shown.
- If your covert mission succeeds but you're found out as the perpetrator, then you will no longer be informed as such in the mission outcome window.

V1 changes:
- Experimental feature: You can change your country's name. Just create a new covert cell with the name, "NAME", followed by a space, followed by your desired new name; a new covert cell won't actually be created when doing this. For example, if you type "NAME New Name" in the covert cell name blank and click "Confirm", then instead of creating a new covert cell, your country's name will be changed to "New Name". However, the new name will not automatically update in the any in-game windows you have open, unless you close and reopen those windows.
- Full compatibility for database-only mods; for each country, the game now makes estimates of LE, MYS, and EYS based on the country's HDI as specified in database.
- The Psilon achievement is now obtainable.
- When a country succeeds a covert mission while framing itself, its government approval will decrease.
- If a country takes over part of another country, and the two countries have different LE, MYS, and/or EYS values, then the enlarged country's LE, MYS, and/or EYS will be affected according to the now-smaller country's LE, MYS, and/or EYS.

Human development changes:
- Human development, or human development index (HDI), is now dependent on 4 stats: Life expectancy (in years; LE), mean years of schooling (MYS), expected years of schooling (EYS), and GNI per capita. This is to match the United Nations Development Programme's method of calculating human development (but with a $60,000 maximum for GNI per capita for the mod's purposes).
- For the mod's purposes, GDP and GNI are treated as the same thing.
- LE, MYS, and EYS increase and decrease depending on your country's stability and budgetary spending.
- The country information window will display the numerical values of your country's HDI, LE, MYS, and EYS, as well as the world average HDI.

Internal political sphere changes:
- Reduce how much government stability contributes to birth rate. This is partially to prevent highly-developed and stable countries having unrealistic population booms.
- Maximum possible immigration rate has been raised to equal the maximum emigration rate.

Internal mlitary sphere changes:
- The number of covert cells that a country needs, in order to maintain a constant security level, increases more gradually along with population.
- A country's covert mission will have a higher chance of succeeding if it has other active cells in the target country.

Internal economic sphere changes:
- Income tax revenue relative to GDP is more in line with what such a ratio would be in the real world. The most noticeable effect is that your budget will be easier to balance, especially for highly-developed countries.
- A country's domestic demand level for resources is no longer dependent on its government type.
- Setting higher resource sector taxes still negatively affect sector growth, but not as much as before.
- A country's tourism revenue only drops if battles or bombardments are occurring in some of its regions.

AI changes:
- When an AI country invites other countries to join in a war declaration treaty, it will invite all other countries that may be interested, not just enough countries to defeat the enemy.
- An AI country will be more willing to come to the defense of another country that it likes, and its willingness no longer depends on the AI aggressiveness level.
- An AI country will be more willing to join an existing war on the attacking side if it hates the defending side, but its willingness will now be dependent on the AI aggressiveness level.
- The number of military units that an AI country builds or buys now depends on how high the country's revenue is.

UI changes:
- When creating a new covert cell, the default rank setting is Elite instead of Recruit.
- Experimental feature: It's possible to create 10 covert cells at a time. In the cell name field, type a dash ('-') followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "-21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30".

Fixes to existing SP2 issues:
- High government approval and stability now decrease your emigration rate instead of increasing it.
- A country can no longer (net) export more of a resource than its own production of that resource.

Other changes:
- The world's average human development is now calculated as if the entire world were one country. This is so that small countries don't meaninglessly skew the world average HDI.
- Minimum possible birth and death rates have been lowered slightly.
- When starting a new game, some statistics of each country will be logged out to the console. Something to look at while waiting for the game to load :)

Known issues:
- There's a possible issue where if a country is occupied for a long time, releasing all its regions would make the country extremely rich in resources momentarily. (Existed in SP2 V1.5.1)

Future changes and "thoughts" in no particular order of priority:
- Have LE, MYS, and EYS affect more than just the HDI. For example, all 3 could have some effect on birth and death rates, and MYS and EYS could affect resource growth.
- Have resource sector taxes apply to all domestic production of resources, not just imports and exports.
- Have immigration and emigration rates depend on more than just political factors.
- Have interest rate have a larger effect on inflation rate.
- Have wars and occupation directly and negatively affect LE, MYS, and EYS.
- Find out some other way to show LE, MYS, and EYS to the player.
- If your country isn't meeting your domestic consumption of food-related resources, then it should have detrimental effects on your LE.
