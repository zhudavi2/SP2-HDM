SuperPower 2 Human Development Modification (SP2-HDM) V1

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
- If your covert mission succeeds but you're found out as the perpetrator, then you will be informed as such in the mission outcome window.
- When creating a new covert cell, the default rank setting is Elite instead of Recruit.
- Experimental feature: It's possible to create 10 covert cells at a time. In the cell name field, type a dash ('-') followed by a whole number; this will create 10 cells, named in sequence according to the whole number that you specified. For example, if you type "-21" into the name field and confirm, you will create 10 cells, named "21", "22", "23", ..., "30".
- In the economic health window, your "income index" (between 0.000 and 1.000) will be shown next to your GDP per capita; this is one component used in calculating your HDI. Higher is better.

Fixes to existing SP2 issues:
- High government approval and stability now decrease your emigration rate instead of increasing it.
- A country can no longer (net) export more of a resource than its own production of that resource.

Other changes:
- The world's average human development is now calculated as if the entire world were one country. This is so that small countries don't meaninglessly skew the world average HDI.
- Minimum possible birth and death rates have been lowered slightly.
- When starting a new game, some statistics of each country will be logged out to the console. Something to look at while waiting for the game to load :)

Known issues:
- When starting the mod, instead of the opening movies playing, you'll get a black screen. This is expected; keep pressing Esc until you reach the SP2 loading screen.
- The country information window cannot display LE values greater than 409.5, or MYS and EYS values greater than 102.3; the game knows what the values actually are, it just can't display them. It shouldn't be a problem in most cases, as those kinds of stats aren't achievable until after a few hundred years of game time.

Future changes and "thoughts" in no particular order of priority:
- Have LE, MYS, and EYS affect more than just the HDI. For example, all 3 could have some effect on birth and death rates, and MYS and EYS could affect resource growth.
- In the covert cells window, display how many Elite cells you would need to reach 94.9% security.
- Have resource sector taxes apply to all domestic production of resources, not just imports and exports.
- Have immigration and emigration rates depend on more than just political factors.
- Have interest rate have a larger effect on inflation rate.
- Have wars and occupation directly and negatively affect LE, MYS, and EYS.
