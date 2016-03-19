SuperPower 2 Human Development Modification (SP2-HDM) V6

Human development changes:
- (V1) If a country takes over part of another country, and the two countries have different LE, MYS, and/or EYS values, then the enlarged country's LE, MYS, and/or EYS will be affected according to the now-smaller country's LE, MYS, and/or EYS.
- Human development, or human development index (HDI), is now dependent on 4 stats: Life expectancy (in years; LE), mean years of schooling (MYS), expected years of schooling (EYS), and GNI per capita. This is to match the United Nations Development Programme's method of calculating human development, but with a $60,000 maximum GNI per capita for the mod's purposes. Also for the mod's purposes, GDP and GNI are treated as the same thing.
- LE, MYS, and EYS increase and decrease depending on your country's stability and budgetary spending.
- The world's average human development is now calculated as if the entire world were one country. This is so that small countries don't meaninglessly skew the world average HDI.

Internal political sphere changes:
- (V5) When a country gains a region, it will lose stability based on the ratio of the region's population to the gaining country's population. The stability penalty is lower if the region is gained through trade.
- (V5) The Free Region treaty now only gives relations bonuses between the original owner of the region(s) and the former occupier of the region(s), instead of between everyone and the former occupier. Thanks to Fenix for the suggestion.
- Reduce how much government stability contributes to birth rate. This is partially to prevent highly-developed and stable countries having unrealistic population booms.
- Maximum possible immigration rate has been raised to equal the maximum emigration rate.

Internal military sphere changes:
- (V6) When a country declares war on another country, and both countries are in the same Weapons Trade treaty, then the declarer will now automatically leave the treaty.
- (V6) When a country declares war on another country, any military units in progress, that the declarer requested from the other country or vice versa, are now automatically cancelled.
- (V5) Nuclear strikes against civilians now affect regional populations that are not attached to any specific cities. This effectively increases the amount of people that nuclear strikes can kill at once. Thanks to Fenix for the suggestion.
- (V5) Nuclear strikes against civilians now affect telecom development level as well as that of infrastructure.
- (V5) Changed how the game determines whether a covert mission succeeds, whether or not the true perpetrator would be found out, and whether or not the executing cell would be captured in case of mission failure. Previously, the outcome of a mission would always be the same as long as all the mission details (e.g., type, cell training level, complexity, etc.) and the in-game date and time of mission execution were the same.
- (V5) When an Assassination mission succeeds against a target country, if the target country has any covert cells stationed in itself, then one of the foreign cell may be removed (assassinated) at random, depending on the executing cell's experience level and the mission's complexity. Thanks to Mr Monday for the suggestion.
- (V2) When a country creates a war treaty and is on the attacking side, in the war list window it will be listed as the leader of the attacking side. Previously, the leader of the attacking side wouldn't always be the treaty creator even if the creator were on the attacking side.
- (V1) When a country succeeds a covert mission while framing itself, its government approval will decrease.
- The number of covert cells that a country needs, in order to maintain a constant security level, increases more gradually along with population.
- A country's covert mission will have a higher chance of succeeding if it has other active cells in the target country.

Internal economic sphere changes:
- (V5) Unemployment now changes more gradually than before.
- Income tax revenue relative to GDP is more in line with what such a ratio would be in the real world. The most noticeable effect is that budgets will be easier to balance, especially for highly-developed countries.
- A country's domestic demand level for resources is no longer dependent on its government type.
- Setting higher resource sector taxes still negatively affect sector growth, but not as much as before.
- A country's tourism revenue only drops if battles or bombardments are occurring in its own regions.

AI changes:
- (V5) When a nuclear-armed AI country has enough of its regions under foreign occupation, it will launch nuclear attacks against the occupiers' military, instead of against the occupiers' civilians. Thanks to Fenix for the suggestion.
- (V5) A nuclear-armed AI country, in response to a nuclear launch involving another country that it really likes or really dislikes, will have a bigger preference for itself to launch a military nuclear strike rather than civilian. Thanks to Fenix for the code suggestion.
- When an AI country invites other countries to join in a war declaration treaty, it will invite all other countries that may be interested, not just enough countries to defeat the enemy.
- An AI country will be more willing to come to the defense of another country that it likes, and its willingness no longer depends on the AI aggressiveness level.
- An AI country will be more willing to join an existing war on the attacking side if it hates the defending side, but its willingness will now be dependent on the AI aggressiveness level.
- The number of military units that an AI country builds or buys now depends on how high the country's revenue is.

UI changes:
- (V5) It's possible to create 10 covert cells at a time. In the cell name field, type "MULTIPLE" followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "MULTIPLE 21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30".

Configurable settings:
- SP2-HDM_Config.xml is the configuration XML file.
- (V7) Enable or disable country name changing. However, due to SDK limitations, in order to prevent a country name change from affecting player nicknames, player nicknames can only be changed via the covert window once in the game; this is done in the same way as changing a country name, just with "PLAYER" and a space, followed by the desired name, instead of "NAME" and a space, followed by the desired name.
- (V6) Control whether to disallow a country on the defending side of a war from occupying the home territory of a country on the attacking side, unless the occupying country is also currently in an offensive war against the owning country.
- (V6) Approximate maximum distance, in degrees of latitude and longitude, that must lie between hostile units in order for them to engage in battle; this negates the tactic of excessively splitting one's military units to make it artificially difficult for hostile units to engage in battle. Thanks to Fenix for the code suggestion.
- (V6) Control disablement of AMDS research, and disbanding any existing AMDS, when a country becomes fully occupied.
- (V6) Control upkeep fee, as a percentage of default, for each military unit category (e.g., Infantry, Ground, Air, etc.).
- (V6) Control disablement nuclear research, and reduction of existing nuclear tech levels, upon full military occupation. If the country doesn't already have all nuclear tech levels at 1 or greater, then it will have to restart nuclear research from the beginning.
- (V5) Number of covert cells that a human player may assign to any one foreign country. Thanks to Fenix for the suggestion. The limit does not apply to AI-controlled countries.
- (V5) Control whether an AI should always refuse to assume a human player's debt.
- (V5) Effective damage radius of nuclear missiles.
- (V5) How much of a region's resource production is lost upon being annexed. Does not affect regions gained through trade.
- (V5) Choose if the dedicated server autosave file should be saved to the save folder, or the same folder as joshua.exe.
- (V5) Expected (not actual) stability levels required for entering and exiting Anarchy.
- (V4) Quickly set resource state control, import/export quotas, and taxes.
- (V4) Dedicated server autosave period.
- (V3) Global tax limit.
- (V3) Income tax limits per government type.
- (V3) Server message to be displayed to players who join.
- (V3) Toggle Naval Rule.
- (V3) Percentage of regions occupied in order to be able to use nuclear weapons.
- (V3) Resource tax limit.
- (V3) HDI component (LE, MYS, EYS) viewing through the country info window.

Experimental changes:
- (V1) You can change your country's name. Just create a new covert cell with the name, "NAME", followed by a space, followed by your desired new name; a new covert cell won't actually be created when doing this. For example, if you type "NAME New Name" in the covert cell name blank and click "Confirm", then instead of creating a new covert cell, your country's name will be changed to "New Name". However, the new name will not automatically update in the any in-game windows you have open, unless you close and reopen those windows.

Changes to previous SP2-HDM releases:
- (V6) When performing covert actions against a target country, the positive effect on success rate of having additional cells in the target country has been lessened, further than it was in V5. Thanks to Fenix for the suggestion.
- (V5) Fixed an issue in which if a country is conquered, and then another country changes its name, then the conquered country would reappear in country listings.
- (V5) Fixed a potential issue in which countries with extremely low revenue wouldn't be able to build or buy non-infantry units.

Fixes to SP2 V1.5.1 issues:
- (V6) Fixed an issue where a player could enroll a large amount of infantry (more than its manpower should make available) and then cancel them, resulting in an artificial population boost. Thanks to Fenix for pointing it out.
- (V6) Fixed an issue in which a country's nuclear units would not always be automatically disbanded when the country becomes fully occupied.
- (V5) Fixed at least some instances of the issue in which units that are deployed close to their capital may get stuck in deployment status.
- (V5) When the game saves, either via the dedicated server autosave feature or the "save" console command, the game will now be saved correctly even if the "save" folder doesn't already exist. Thanks to Fenix for the suggestion.
- (V5) When a country conquers another country in which the conquering country has covert cells stationed, then the conquering country will have its national security correctly recalculated immediately.
- (V5) It is no longer possible to perform Espionage missions against one's own country for economic benefit. Thanks to Fenix for the code suggestion.
- (V5) It is no longer possible to move ground units to Antarctica when the Naval Rule is enabled; this prevents circumventing the Naval Rule by moving non-naval units to Antarctica before moving them to their final destination.
- (V1) The Psilon achievement is now obtainable.
- High government approval and stability now decrease your emigration rate instead of increasing it.
- A country can no longer (net) export more of a resource than its own production of that resource.

Other changes:
- (V6) Added a console command, print_players, to list all active players. Thanks to Fenix for the suggestion.
- (V6) Added a console command, set_admin_country, that sets the server's admin player by country ID.
- (V6) Added a console command, set_admin_player, that sets the server's admin player by player ID.
- Minimum possible birth and death rates have been lowered slightly.
- When starting a new game, some statistics of each country will be logged out to the console. Something to look at while waiting for the game to load :)

Known issues and possible future changes:
- Please see https://github.com/zhudavi2/SP2-HDM/issues.

SDK notes:
- The source code is at https://github.com/zhudavi2/SP2-HDM.
- The includes/RGAPI folder, that comes with the SDK, is not on GitHub; you must provide your own. The RGAPI files were not modified in making SP2-HDM.
- The "Debug_In_Release" and "Release" build configurations are known to build correctly and produce working DLLs.
- A Post-Build Event will automatically copy the built DLL to a default SP2 mod folder location. You may disable the step, or change it to point to your own desired folder. 
- The Release DLL is included with each commit for those interested in testing out the latest changes.
- Added a new class, GDZDebug, which offers debug logging logic.
- Changed debug console commands: print_relations
- New debug console commands: build_amds, print_amds, force_occupy, research_nuclear, print_nuclear_research
- Please credit the below people and entities, and me, if you release your own work that's based on this mod. Thanks!

Credits:
- Fenix for code suggestions and support.
- Illuminati, Mr Monday, Radu, and thereversedguy for suggestions and support.
- Many more people on the SP2-HDM discussion thread and the SP2 Discussion forum.
- Jean-René Couture and GolemLabs for releasing the SP2 SDK and supporting the SP2 community.
