Human Development Modification (HDM) V10.3

Human development changes:
- (V1) If a country takes over part of another country, and the two countries have different LE, MYS, and/or EYS values, then the enlarged country's LE, MYS, and/or EYS will be affected according to the now-smaller country's LE, MYS, and/or EYS.
- Human development, or human development index (HDI), is now dependent on 4 stats: Life expectancy (in years; LE), mean years of schooling (MYS), expected years of schooling (EYS), and GNI per capita. This is to match the United Nations Development Programme's method of calculating human development, but with a $60,000 maximum GNI per capita for the mod's purposes. Also for the mod's purposes, GDP and GNI are treated as the same thing.
- LE, MYS, and EYS increase and decrease depending on your country's stability and budgetary spending.
- The world's average human development is now calculated as if the entire world were one country. This is so that small countries don't meaninglessly skew the world average HDI.

Client state changes:
- (V8) You may force a country to become your client state by signing a military trespassing treaty with the target country on side B, with the treaty's name starting with the letters "CLIENT". 80% of the target country's population must be living in regions under your military control, and it must be significantly weaker than you are, both economically and militarily. You will lose military control of your client state's regions, but your military will have access to your client state's territory, and vice versa.
- (V8) Your client state's name will be appended with "client of", followed by your country's name.
- (V8) Your client state will start off with the same diplomatic relations with the rest of the world as you.
- (V8) Your client state will immediately exit all of its existing wars, and will automatically join any present or future wars on your side.
- (V8) Your client state cannot build or buy so military units that it would rival you in military strength.
- (V8) Your client state will pay a fraction of its GDP as tribute to you; the tribute will be higher if the client state's HDI is high.
- (V8) Your client state can't create treaties on its own.
- (V8) You may free your client state by leaving the associated military trespassing treaty.
- (V8) You may free a client state of another country by occupying enough regions that account for 80% of the client state's population, then signing a free region treaty with the client state, with the treaty's name starting with the letters "CLIENT".
- (V8) Your client state will be able to unilaterally free itself from you if its military strength exceeds yours (e.g., if you lose so many units that your military strength drops below your client state's).
- (V8) A country that economically fails, or is conquered, automatically releases its client states.

Political changes:
- (V10.1) A player can set their country to have no capital. Create a new covert cell with the name, "NOCAPITAL"; a new covert cell won't actually be created when doing this. The Constitutional Form window can be used to reinstate a capital. Thanks to IceyVVind for the suggestion. (Issue #84)
- (V10) A country's budget spending level has more effect on its approval when unemployment is high.
- (V10) A country's stability scales with its unemployment rate.
- (V10) Send a chat message to all players upon the beginning of any annexation, including treaty annexation and civil-war-related annexation.
- (V9) A player who fails to create a treaty (i.e., not that other parties don't accept the treaty, but the treaty itself can't be created) will receive a chat notification of that fact.
- (V9) A chat message is broadcast to every player when a country changes its government type. Thanks to memento mori for initial code.
- (V5) When a country gains a region, it will lose stability based on the ratio of the region's population to the gaining country's population. The stability penalty is lower if the region is gained through trade.
- (V5) The Free Region treaty now only gives relations bonuses between the original owner of the region(s) and the former occupier of the region(s), instead of between everyone and the former occupier. Thanks to Fenix for the suggestion.
- (V5) It's possible to create 10 covert cells at a time. In the cell name field, type "MULTIPLE" followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "MULTIPLE 21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30".
- Reduce how much government stability contributes to birth rate. This is partially to prevent highly-developed and stable countries having unrealistic population booms.
- Maximum possible immigration rate has been raised to equal the maximum emigration rate.

Military changes:
- (V9) A player who executes a covert mission while trying to frame someone else, but is found out as the true attacker, will receive a chat notification of that fact.
- (V9) A chat message is broadcast to every player when a country joins a war. Thanks to memento mori for initial code.
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

Economic changes:
- (V10) A country whose economy fails will experience an unemployment surge.
- (V10) 0% economic health can lead to 100% unemployment.
- (V10) Base unemployment rate partially on growth of GDP and 15-65 population.
- (V10) Reduce domestic consumption when PIT is high. Consumption of food resources decreases less than that of other resources, and decreases less if government spending is high.
- (V9) A region's resource production growth now depends on whether or not a battle is occurring in the region and whether or not it's foreign-occupied. Better relations with occupier leads to more normal growth.
- (V9) A country's ability to trade now depends on how much of its production is in foreign-occupied regions. Better relations with occupiers leads to more normal trading ability.
- (V9) A region keeps its original level of resource demand when it becomes foreign-occupied.
- (V9) For a country whose economy is at least partially state-controlled, its approval, stability, and food resource demand satisfaction will affect its tax revenue.
- (V9) Removed government type modifiers for resource production and GDP, so GDP itself is no longer dependent on government type.
- (V9) Removed imports and exports from contributing to GDP, as they weren't consistent with the GDP calculation method otherwise used in the game.
- (V9) Domestic demand is no longer capped by HDI.
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

Configurable settings:
- hdm_cfg_server.xml is the configuration XML file for the server side.
- (V10.3) Allow players to play past election losses.
- (V10.3) Control whether or not to use new economy mechanics, which grants ountries an advantage in exporting a resource if its GDP per capita is high and it has a high market share in the resource, or the resource makes up a large percentage of its GDP. Countries, whose production satisfies their domestic demand for non-service resources, won't be able to export as much of service resources.
- (V10.1) Control whether countries must have regions in order to continue existing in the game. (Issue #15)
- (V10) Control whether to limit death rate to 0.2% when a country's LE and over-65 population are high enough to yield a death rate higher than 0.2%.
- (V10) Control production modifiers per government type. This is the same as the V1.5.1 feature that applies multipliers to resource production based on government type, but the specific multiplier values are now configurable.
- (V10) Set a stability criterion such that a country with less than the specified stability value would have a chance of immediately falling into anarchy, with that chance also being configurable.
- (V10) Enable or disable human players playing past their own economic failure.
- (V10) Enable or disable "civil war" and related options. A country that falls into anarchy might have some of its regions automatically fall under occupation, or begin becoming annexed, by the designated "rebel" country (country ID 194, Rebels, for default V1.5.1 database); native military units in the affected regions will become owned by the rebel country.
- (V9) Enable or disable automatic covert mission execution. Covert cells, when given a mission, will continuously prepare and execute the given mission until cell capture, cell disbanding, or mission cancellation; some restrictions apply. Thanks to memento mori for initial code.
- (V9) Adjust relations loss upon annexing regions.
- (V9) Adjust military unit upkeep, as a percentage of default, based on training level. Thanks to Fenix for the suggestion
- (V9) Adjust how much tribute a client state must give as a percentage of GDP. Thanks to Mr Monday for the suggestion.
- (V9) Enable or disable logging of country bankruptcies.
- (V8) Enable or disable human-controlled countries from becoming client states. This is in response to some players reporting crashes when a human-controlled country becomes a client state.
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

Other changes:
- (V10.4) Added a console command, load_hdm_cfg, to reload the HDM config file in the middle of a game. Thanks to -Jesse- for the suggestion.
- (V10.3) Added a console command, set_password, to set server password. Thanks to IceyVVind for the suggestion. (#127)
- (V10.3) Added a console command, print_server_name, to log out current server name.
- (V10.3) Broadcast chat message to all players when a war treaty is created, to allow players to know if someone sponsored a war that they're not participating in. Thanks to Tannman and Alex2142 for the suggestion. (Issue #101)
- (V10.3) Added a console command, set_max_players, to set the maximum number of players allowed in the server. Thanks to IceyVVind for the suggestion. (Issue #59)
- (V10.3) Added a console command, print_max_players, to log out the maximum number of players allowed in the server.
- (V10.3) Display HDM version in main menu.
- (V10.2) Intro movies no longer play when starting the game.
- (V10) Prevent some displayed percentages and rates from going negative, and make such displays more accurate, by exponentially annualizing and de-annualizing growth rates rather than multiplicatively doing so.
- (V10) If LE is increasing while LE and the percentage of population over 65 are high enough, then the death rate will initially be limited to prevent the percentage of population over 65 from decreasing (increasing LE should lead to aging population), but in the long term, the death rate will rise again (as the over-65 population becomes so high that much of the population is dying of old age).
- (V10) Upon game start or load, each region will be checked so that its population breakdowns by age, religion, and language are consistent, taking breakdown by age as correct.
- (V9) Death rate now depends on LE instead of HDI as a whole.
- (V9) Birth rate is now sharply reduced when LE is under 20.
- (V9) Expected birth rate now depends on EYS and percentage of population over 65.
- (V9) Added a console command, print_military_control, to list all regions under a country's military control.
- (V9) Added a console command, print_foreign_presence, to list all foreign military units in a country.
- (V9) Added a console command, print_regions_in_area, to list all regions in an area bounded by given latitudes and longitudes.
- (V9) Added a console command, declare_peace, to end all current wars. Thanks to p2 for the suggestion.
- (V9) The game will detect and fix database errors as it finds them.
- (V8) A successful military removal treaty now only improves relations between the parties to the treaty, instead of affecting relations between the military unit mover and the rest of the world.
- (V6) Added a console command, print_players, to list all active players. Thanks to Fenix for the suggestion.
- (V6) Added a console command, set_admin_country, that sets the server's admin player by country ID.
- (V6) Added a console command, set_admin_player, that sets the server's admin player by player ID.
- (V1) You can change your country's name. Just create a new covert cell with the name, "NAME", followed by a space, followed by your desired new name; a new covert cell won't actually be created when doing this. For example, if you type "NAME New Name" in the covert cell name blank and click "Confirm", then instead of creating a new covert cell, your country's name will be changed to "New Name". However, the new name will not automatically update in the any in-game windows you have open, unless you close and reopen those windows.
- Minimum possible birth and death rates have been lowered slightly.
- When starting a new game, some statistics of each country will be logged out to the console. Something to look at while waiting for the game to load :)

Changes to previous HDM releases:
- (V10.3) Fixed unintentional volatility of PIT's and budgets' effects on domestic consumption.
- (V10.3) Config file renamed from SP2-HDM_Config.xml to hdm_cfg_server.xml.
- (V10.3) HDM custom client now compatible with non-HDM servers. Thanks to IceyVVind for the suggestion. (Issue #125)
- (V10.3) print_players now logs out each player's relative ping.
- (V10.2) Fixed conflict between country name changes and player name changes. Removed country-name-change-related config option.
- (V10.0.2) Fixed an occasional crash while annexations are occurring in contested regions. (Issue #94)
- (V10.0.2) During civil war, units in deployment or transit will no longer defect; fixes some related crashes.
- (V10.0.2) Fixed an occasional crash upon a country leaving anarchy when stability-based anarchy is enabled. (Issue #91)
- (V10.0.1) Fixed an issue where upon loading the game, your resources window would be blank.
- (V10) Fixed a crash that would occur in certain situations when creating a client state.
- (V10) Fixed a crash related to calculation of nuclear military unit upkeep.
- (V9) A player who tries and fails to attack a country, because they're fighting a non-offensive war against that country, will receive a chat notification of that fact.
- (V9) When a country changes its government type, its income tax rate will be automatically set to no more than its new government type's tax limit, if configured and if necessary. Thanks to memento mori for initial code.
- (V9) MYS now automatically trends towards EYS, instead of being affected directly by other factors that affect HDI components.
- (V9) Estimate HDI components more accurately at game start.
- (V9) Fixed an issue where client state relationships would persist after loading a game that didn't have them.
- (V6) When performing covert actions against a target country, the positive effect on success rate of having additional cells in the target country has been lessened, further than it was in V5. Thanks to Fenix for the suggestion.
- (V6) When performing covert actions against a target country, the positive effect on success rate of having additional cells in the target country has been lessened, further than it was in V5. Thanks to Fenix for the suggestion.
- (V5) Fixed an issue in which if a country is conquered, and then another country changes its name, then the conquered country would reappear in country listings.
- (V5) Fixed a potential issue in which countries with extremely low revenue wouldn't be able to build or buy non-infantry units.

Fixes to SP2 V1.5.1 issues:
- (V10.3) Disallow state-controlled economies from importing more than their domestic consumption. Thanks to memento mori for reporting. (Issue #79)
- (V10.2) Prevent modified clients from connecting to password-protected servers. To secure a non-password-protected server in this way, as a workaround, you could create a password-protected server that mentions its password in its name.
- (V10) Population growth thematic map is no longer misleading.
- (V10) Increase covert mission result randomness so that multiple missions wouldn't always all yield the same result if they were all executed around the same game time.
- (V10) Fixed regions still appearing to be bombarded when bombardment has actually already stopped.
- (V9) The Big-time Friend achievement is now obtainable.
- (V9) Fixed an issue where the AI would unintentionally execute covert missions against the conqueror of the originally-intended target.
- (V9) Fixed an issue where covert cells could travel to and execute missions in a conquered country.
- (V8) Fixed an issue where cells in the covert cell window may appear to "overwrite" each other when being created. (Issue #10)
- (V8) Fixed an issue where deploying units to a capital, while having other units ready to be deployed or while having other units deployed in the capital, would sometimes cause the in-deployment units to disappear after deployment. (Issue #46)
- (V8) Fixed an issue where a military removal treaty would boost diplomatic relations even if no units moved countries as a result. (Issue #37)
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

Other notes:
- Saved games created in SP2 V1.5.1 are not compatible with HDM, and saved games created in HDM are not compatible with SP2 V1.5.1.

Known issues and possible future changes:
- Please see https://github.com/zhudavi2/SP2-HDM/issues.

SDK notes:
- The source code is at https://github.com/zhudavi2/SP2-HDM.
- The includes/RGAPI and lib folders, that come with the SDK, are not on GitHub; you must provide your own. The RGAPI and lib files were not modified in making the mod.
- The "Debug" and "Release" build configurations are known to build correctly and produce working DLLs.
- A Post-Build Event will automatically copy the built DLL to a default SP2 mod folder location. You may disable the step, or change it to point to your own desired folder.
- Starting with V10.2, for security, the code to prevent modified clients from connecting to password-protected servers is not available in the public source. I hope to make the full code (excluding the RGAPI and lib folders) available again a later date.
- The Release Client and Server DLLs are included with each commit for those interested in testing out the latest changes. Starting with V10.2, the included Client DLL contains the code to prevent modified clients from connecting to password-protected servers, so the included Client DLL is not exactly equivalent to any Client DLL built from the public source.
- Added a new class, GDZDebug, which offers assert and logging logic. Please see https://github.com/zhudavi2/SP2-HDM/wiki/DZDebug.
- Changed debug console commands: print_relations
- New debug console commands: build_amds, print_amds, force_occupy, research_nuclear, print_nuclear_research, declare_war_2v2, force_anarchy, set_stability
- Please credit the below people and entities, and me, if you release your own work that's based on this mod. Thanks!

Credits:
- Fenix/memento mori for code suggestions, support, and testing help.
- awolfska for testing help.
- BlueS3xy`=;], IceyVVind, Illuminati, -Jesse-, Mr Monday, p2, Paco Jiangxi Dandong, Radu, Slajavck, Tannman, The Patriot, thereversedguy, and ZDisket for suggestions and support.
- Many more people on the HDM discussion thread and the SP2 Discussion forum.
- Jean-René Couture and GolemLabs for releasing the SP2 SDK and supporting the SP2 community.
