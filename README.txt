SuperPower 2 Human Development Modification (SP2-HDM) V5

V5 changes:
- Military units that are hostile against each other will start battling each other from a farther distance; this negates the tactic of excessively splitting one's military units to make it artificially difficult for hostile units to engage in battle. Thanks to Fenix for the code suggestion.
- Added a configuration setting to limit the number of covert cells that a human player may assign to any one foreign country. Thanks to Fenix for the suggestion. The limit does not apply to AI-controlled countries.
- Fixed an issue in which if a country is conquered, and then another country changes its name, then the conquered country would reappear in country listings.
- When performing covert actions against a target country, the positive effect on success rate of having additional cells in the target country has been lessened. Thanks to Fenix for the suggestion.
- Added a configuration setting to force the AI to always refuse Assume Debt treaties when an AI country is requested to assume a human player's debt.
- Added a configuration setting to control the effective damage radius of nuclear missiles.
- Added a configuration setting to control how much of a region's resource production is lost upon being annexed. The production loss does not affect regions gained through trade.
- Added a configuration setting for dedicated servers, to choose whether to autosave to the same folder as joshua.exe.
- Added configuration settings for adjusting the expected (not actual) stability levels required for entering and exiting Anarchy.
- Minor modifications to the Server.vcxproj file.
- When a nuclear-armed AI country has enough of its regions under foreign occupation, it will launch nuclear attacks against the occupiers' military, instead of against the occupiers' civilians. Thanks to Fenix for the suggestion.
- Fixed a potential issue in which countries with extremely low revenue wouldn't be able to build or buy non-infantry units.
- A nuclear-armed AI country, in response to a nuclear launch involving another country that it really likes or really dislikes, will have a bigger preference for itself to launch a military nuclear strike rather than civilian. Thanks to Fenix for the code suggestion.
- When starting a new game, each country's population will be logged out to the console in addition to all the stats already being logged.
- When a country conquers another country in which the conquering country has covert cells stationed, then the conquering country will have its national security correctly recalculated immediately.
- When a country gains a region, it will lose stability based on the ratio of the region's population to the gaining country's population. The stability penalty is lower if the region is gained through trade.
- Nuclear strikes against civilians now affect regional populations that are not attached to any specific cities. This effectively increases the amount of people that nuclear strikes can kill at once. Thanks to Fenix for the suggestion.
- Nuclear strikes against civilians now affect telecom development level as well as that of infrastructure.
- The Free Region treaty now only gives relations bonuses between the original owner of the region(s) and the former occupier of the region(s), instead of between everyone and the former occupier. Thanks to Fenix for the suggestion.
- Changed how the game determines whether a covert mission succeeds, whether or not the true perpetrator would be found out, and whether or not the executing cell would be captured in case of mission failure. Previously, the outcome of a mission would always be the same as long as all the mission details (e.g., type, cell training level, complexity, etc.) and the in-game date and time of mission execution were the same.
- When an Assassination mission succeeds against a target country, if the target country has any covert cells stationed in itself, then one of the foreign cell may be removed (assassinated) at random, depending on the executing cell's experience level and the mission's complexity. Thanks to Mr Monday for the suggestion.
- It is no longer possible to perform Espionage missions against one's own country for economic benefit. Thanks to Fenix for the code suggestion.
- Changed the method of creating 10 covert cells at once. In the cell name field, type "MULTIPLE, followed by a space, followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "MULTIPLE 21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30".
- Changed the mod's server list display name to "Human Development Mod V5" from "SuperPower 2 Human Development Mod".
- When the game saves, either via the dedicated server autosave feature or the "save" console command, the game will now be saved correctly even if the "save" folder doesn't already exist. Thanks to Fenix for the suggestion.
- Fixed an issue where the "set_admin_country" console command wasn't recognized.
- Added a "print_players" console command to list all active players. Thanks to Fenix for the suggestion.
- It is no longer possible to move ground units to Antarctica when the Naval Rule is enabled; this prevents circumventing the Naval Rule by moving non-naval units to Antarctica before moving them to their final destination.
- Fixed at least some instances of the issue in which units that are deployed close to their capital may get stuck in deployment status.
- Unemployment now changes more gradually than before.

V4 changes:
- Added new settings to the configuration file, to help set resource state control, export quotas, taxes, etc. quickly. Please see SP2-HDM_Config.xml for details.
- Added a new configuration file setting to make the server autosave the game every time a specified number of minutes have passed.
- Added a new console option, set_admin_country, that allows changing the server's admin to the player controlling the country with the given country ID. (Usage: set_admin_country <1-based country ID>)
- The HDI, LE, MYS, and EYS of every active country will be no longer written to the server console log every 365 days of game time.

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
- Every 365 days of game time, the HDI, LE, MYS, and EYS of every active country will be written to the server console log. (Removed in V4.)
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
- The country information window will display the numerical values of your country's HDI, LE, MYS, and EYS, as well as the world average HDI. (Removed in V2.)

Internal political sphere changes:
- Reduce how much government stability contributes to birth rate. This is partially to prevent highly-developed and stable countries having unrealistic population booms.
- Maximum possible immigration rate has been raised to equal the maximum emigration rate.

Internal military sphere changes:
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
- When creating a new covert cell, the default rank setting is Elite instead of Recruit. (Removed in V2.)
- Experimental feature: It's possible to create 10 covert cells at a time. In the cell name field, type a dash ('-') followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "-21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30". (Changed in V5.)

Fixes to existing SP2 issues:
- High government approval and stability now decrease your emigration rate instead of increasing it.
- A country can no longer (net) export more of a resource than its own production of that resource.

Other changes:
- The world's average human development is now calculated as if the entire world were one country. This is so that small countries don't meaninglessly skew the world average HDI.
- Minimum possible birth and death rates have been lowered slightly.
- When starting a new game, some statistics of each country will be logged out to the console. Something to look at while waiting for the game to load :)

Known issues and possible future changes:
- Please see https://github.com/zhudavi2/SP2-HDM/issues.

Credits:
- Fenix for code suggestions and support.
- Radu and Mr Monday for suggestions and support.
- Many more people on the SP2-HDM discussion thread and the SP2 Discussion forum.
- Jean-René Couture and GolemLabs for releasing the SP2 SDK and supporting the SP2 community.
