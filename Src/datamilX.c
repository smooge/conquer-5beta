/*
 * datamilX.c - Military Unit Data Management System
 *
 * This file defines the complete military unit system for the Conquer game,
 * including army units, naval vessels, unit traits, combat statuses, and
 * military class classifications. All military unit balance, capabilities,
 * and game mechanics are configured through the data structures in this file.
 *
 * The military system encompasses:
 * - Leader units (rulers and special commanders)
 * - Combat units (infantry, cavalry, archers, siege equipment)
 * - Special units (scouts, spies, agents, monsters)
 * - Naval vessels (warships, merchants, galleys, barges)
 * - Unit traits and special abilities
 * - Combat and deployment statuses
 *
 * Data Structure Organization:
 * - ainfo_list[]: Complete army unit definitions with stats and abilities
 * - ninfo_list[]: Naval vessel specifications and capabilities
 * - stat_info[]: Army status definitions affecting behavior and combat
 * - traits_list[]: Human-readable descriptions of unit special abilities
 * - ainfo_clist[]: Military classification categories
 * - Array sizing variables for dynamic memory management
 *
 * Game Balance: All unit costs, combat values, movement rates, and special
 * abilities are carefully balanced through the numeric parameters in these
 * data structures. Modifications to this file directly affect game balance.
 *
 * conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
 */
#define DATA_DECLARE
#include "dataX.h"
#undef DATA_DECLARE
#include "armyX.h"
#include "navyX.h"
#include "magicX.h"
#include "statusX.h"

/*
 * ainfo_list[] - Complete Army Unit Definitions
 *
 * This array contains the complete specification for every military unit type
 * available in the game, organized by military role and power level. Each entry
 * defines a unit's combat capabilities, costs, special abilities, and game balance.
 *
 * Structure: AINFO_STRUCT entries containing:
 * - Unit names (full, short, display forms)
 * - Descriptive text explaining the unit's role and characteristics
 * - Special abilities flags (UP_* constants defining unique capabilities)
 * - Unit classification (AC_* constants for organizational grouping)
 * - Combat statistics (hit points, attack, defense, movement)
 * - Economic costs (recruit cost, upkeep cost, buy cost)
 * - Unit size and capacity limits
 * - Monster/magic compatibility flags for summoning and control
 *
 * Organization:
 * 1. Leader Units (18 types): Rulers and commanders with special abilities
 * 2. Spell Casters (2 types): Magical units with spellcasting capabilities
 * 3. Monster Units (17 types): Conjurable creatures and fantastic beasts
 * 4. Normal Army Units (46+ types): Standard military forces and specialists
 * 5. Scout/Agent Units (8 types): Information gathering and espionage
 *
 * Game Balance Notes:
 * - Unit costs scale with power level and special abilities
 * - Leaders provide free supply and unique national characteristics
 * - Monsters offer powerful abilities but high costs
 * - Normal units form the backbone of military forces
 * - Scouts and agents provide essential intelligence capabilities
 *
 * If adding or deleting any types, change DEFAULT_ settings in armyX.h
 */
AINFO_STRUCT ainfo_list[] = {

  /*
   * Leader Units (18 types) - Rulers and Special Commanders
   *
   * Leaders are unique units that define the character and capabilities of
   * a nation. Most leaders have UP_RULER status, allowing them to control
   * territory and provide free supply (UP_FREESUPPLY) to other units.
   * Different leader types represent various governmental and power structures:
   *
   * - Feudal: King/Baron (hereditary nobility)
   * - Imperial: Emperor/Prince (imperial hierarchy)
   * - Magical: Wizard/Mage (arcane power structure)
   * - Religious: Pope/Cardinal (theocratic system)
   * - Naval: Admiral/Captain (maritime power)
   * - Military: Warlord/Lord (military dictatorship)
   * - Evil: Demon/Devil, Dragyn/Wyrm, Shadow/Nazgul (monstrous rulers)
   *
   * Special Abilities:
   * - UP_RULER: Can control territory and establish capitals
   * - UP_FREESUPPLY: Provides free supply to nearby units
   * - UP_SPELLCAST/UP_FULLCASTER: Magical abilities (Wizard/Mage types)
   * - UP_UNDEAD: Immunity to certain magical effects (Shadow/Nazgul)
   *
   * Game Balance: Leaders have high hit points and moderate combat ability,
   * but their primary value is strategic rather than tactical.
   */
  {"King", "King", "King",
     "A noble ruler, whose population follows his hereditary decisions",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     100, 30, 30, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Baron", "Baron", "Baron",
     "This noble leader supports the King in all endeavors",
     UP_FREESUPPLY,
     AC_LEADER,
     50, 20, 20, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Emperor", "Empr", "Emperor",
     "Ruling over a vast realm is the dream of every Emperor",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     100, 30, 30, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Prince", "Prnc", "Prince",
     "Striving to please ones emperor is of highest importance",
     UP_FREESUPPLY,
     AC_LEADER,
     50, 20, 20, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Wizard", "Wzrd", "Wizard",
     "Magic powerful enough to rule a realm",
     UP_RULER | UP_FREESUPPLY | UP_SPELLCAST | UP_FULLCASTER,
     AC_LEADER,
     250, 30, 30, 20, 0L, 0L, 25L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Mage", "Mage", "Mage",
     "Dutiously studying the arcane arts in support of Wizardly Master",
     UP_FREESUPPLY | UP_SPELLCAST,
     AC_LEADER,
     50, 20, 20, 20, 0L, 0L, 25L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Pope", "Pope", "Pope",
     "A great religious leader, the spiritual essense of his nation",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     100, 30, 30, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Cardinal", "Cdnl", "Cardinal",
     "Dedication to the Pope is of the highest honor and duty",
     UP_FREESUPPLY,
     AC_LEADER,
     50, 20, 20, 20, 0L, 0L, 50L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Admiral", "Adml", "Admiral",
     "Knowledge of the seas formed the source of power of this nation ruler",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     100, 30, 30, 20, 0L, 0L, 33L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Captain", "Capt", "Captain",
     "Sailing the seas to expand the reach of the Admiral",
     UP_FREESUPPLY,
     AC_LEADER,
     50, 20, 20, 20, 0L, 0L, 33L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Warlord", "Wrld", "Warlord",
     "Among the elite of warriors, fighting prowess veteran builds a nation",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     250, 35, 35, 20, 0L, 0L, 25L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Lord", "Lord", "Lord",
     "Skilled in armed combat, this soldier knows how to serve the Warlord",
     UP_FREESUPPLY,
     AC_LEADER,
     125, 30, 30, 20, 0L, 0L, 25L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Demon", "Demn", "Demon",
     "Spoken of in hushed tones among the populous, a fearsome ruler indeed",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     250, 50, 50, 20, 0L, 0L, 10L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Devil", "Devl", "Devil",
     "Worshipful of the Demon above all others in the realm",
     UP_FREESUPPLY,
     AC_LEADER,
     75, 20, 20, 20, 0L, 0L, 10L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Dragyn", "Dgyn", "Dragyn",
     "Powerful and awe-inspiring, this monster wields might wherever possible",
     UP_RULER | UP_FREESUPPLY,
     AC_LEADER,
     500, 50, 50, 20, 0L, 0L, 10L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Wyrm", "Wyrm", "Wyrm",
     "Ever striving to serve the powerful Dragyn",
     UP_FREESUPPLY,
     AC_LEADER,
     100, 40, 40, 20, 0L, 0L, 10L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Shadow", "Shdw", "Shadow",
     "Cold, dark and deadly, this ruler is ever searching for more power",
     UP_RULER | UP_FREESUPPLY | UP_UNDEAD,
     AC_LEADER,
     250, 50, 50, 20, 0L, 0L, 5L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Nazgul", "Nzgl", "Nazgul",
     "Deadly in their own right, Shadow's servants bring power to their lord",
     UP_FREESUPPLY | UP_UNDEAD,
     AC_LEADER,
     125, 40, 40, 20, 0L, 0L, 5L, 1, 1,
     {0x0L, 0x0L, 0x0L} },

  /*
   * Spell Caster Units (2 types) - Dedicated Magical Support
   *
   * These units are specialized magic users that provide spellcasting support
   * to military forces without the territorial control abilities of Wizard leaders.
   * They serve as dedicated magical assets for armies.
   *
   * - Sorcerer: UP_FULLCASTER allows casting of all spell types with maximum power
   * - Magician: UP_SPELLCAST provides basic magical support with limited abilities
   *
   * Both units have UP_FREESUPPLY, making them valuable force multipliers that
   * can support other units while providing magical capabilities. Their low
   * combat statistics make them vulnerable in direct combat.
   *
   * Classification: AC_SPELLCASTER separates them from leaders and normal troops.
   */
  {"Sorcerer", "Sorc", "Sorcerer",
     "An extremely skilled spell caster, a master of the arts",
     UP_FREESUPPLY | UP_FULLCASTER | UP_SPELLCAST,
     AC_SPELLCASTER,
     100, 10, 10, 10, 0L, 0L, 0L, 1, 1,
     {0x0L, 0x0L, 0x0L} },
  {"Magician", "Magn", "Magician",
     "A spellcaster who loyally provides magical skills for a nation",
     UP_FREESUPPLY | UP_SPELLCAST,
     AC_SPELLCASTER,
     50, 0, 0, 10, 0L, 0L, 0L, 1, 1,
     {0x0L, 0x0L, 0x0L} },

  /*
   * Monster Units (17 types) - Conjurable Creatures and Fantastic Beasts
   *
   * Monsters are powerful supernatural units that can be summoned through magic
   * or recruited through special means. They offer unique abilities and high
   * combat power but at significant cost. Monster units are balanced around
   * their magical nature and special capabilities.
   *
   * Power Tiers:
   * 1. Basic Monsters: Spirit, Assassin, Efreet, Gargoyle, Wraith
   * 2. Intermediate: Hero, Centaur, Lich, Giant, SuperHero, Mummy
   * 3. Advanced: Earthmental, Minotaur, Daemon
   * 4. Ultimate: Balrog, Dragon
   *
   * Special Ability Categories:
   * - Flight: UP_FLIGHT for aerial movement (Spirit, Efreet, Gargoyle, Balrog)
   * - Undead: UP_UNDEAD for magical immunity (Wraith, Lich, Mummy)
   * - Elemental: UP_FIRETYPE, UP_EARTHTYPE for elemental affinities
   * - Combat: UP_FORTDAMAGE for siege capabilities
   * - Magic: UP_SPELLCAST for magical abilities (Lich, Dragon)
   *
   * Classification: AC_MONSTER with varying Monster Magic (MM_*) and
   * Magic Weakness (MW_*) compatibility for summoning control.
   */
  {"Spirit", "spir", "spirit",
     "Not among the strongest of monsters, the spirit is conjurable by many",
     UP_FLIGHT,
     AC_MONSTER,
     50, 0, 0, 10, 1800L, 1L, 1000L, 150, 100,
     {0x0L, 0x0L, 0x0L} },
  {"Assassin", "Assn", "Assassin",
     "Deadly and swift, this monster is feared by much of mankind",
     UP_SIGHT | UP_SLIPPERY,
     AC_MONSTER,
     50, 20, 20, 10, 1400L, 2L, 1000L, 250, 50,
     {MM_NINJA, 0x0L, 0x0L }},
  {"Efreet", "efr", "efreet",
     "Conjured from and composed of Air, this spirit is a helpful ally",
     UP_FLIGHT,
     AC_MONSTER,
     50, 10, 10, 15, 1200L, 2L, 800L, 200, 100,
     {0x0L, 0x0L, MW_AIR }},
  {"Gargoyle", "garg", "gargoyle",
     "Skulking and devious, the fiendish gargoyle can be a powerful enemy",
     UP_FLIGHT,
     AC_MONSTER,
     75, 10, 10, 10, 2000L, 2L, 1150L, 170, 120,
     {MM_ORC, 0x0L, 0x0L }},
  {"Wraith", "Wra", "Wraith",
     "This fearsome spectre is a bane of all that is good",
     UP_UNDEAD,
     AC_MONSTER,
     75, 10, 10, 10, 2000L, 3L, 1300L, 250, 100,
     {0x0L, 0x0L, MW_VAMPIRE | MW_THEVOID }},
  {"Hero", "Hero", "Hero",
     "A powerful warrior who comes to serve the worthy",
     0x0L,
     AC_MONSTER,
     100, 0, 0, 10, 1600L, 3L, 800L, 300, 200,
     {MM_WARRIOR, 0x0L, 0x0L} },
  {"Centaur", "Cent", "Centaur",
     "Half human, half horse, this creature proves a valient fighter",
     UP_ANTIAIR,
     AC_MONSTER,
     75, 10, 10, 15, 1700L, 3L, 800L, 250, 150,
     {MM_EQUINE, 0x0L, 0x0L} },
  {"Lich", "Lich", "@Lich",
     "An undead wizard whose life force is sustained by its own magic",
     UP_SPELLCAST | UP_FULLCASTER | UP_UNDEAD | UP_NODRAFT,
     AC_MONSTER,
     100, 0, 0, 10, 0L, 0L, 1200L, 300, 100,
     {0x0L, 0x0L, MW_VAMPIRE} },
  {"Giant", "Gnt", "Giant",
     "Large in stature and powerful in strength; a formidable opponent",
     UP_FORTDAMAGE,
     AC_MONSTER,
     150, 0, 0, 10, 2500L, 5L, 2100L, 500, 300,
     {0x0L, 0x0L, 0x0L} },
  {"SuperHero", "SHro", "SuperHero",
     "Superb fighting skills cause consternation among enemies",
     0x0L,
     AC_MONSTER,
     150, 15, 15, 10, 2600L, 5L, 1200L, 500, 300,
     {MM_WARRIOR | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Mummy", "Mumm", "Mummy",
     "Though in the midst of a full body wrap, fighting skills go unhindered",
     UP_UNDEAD,
     AC_MONSTER,
     150, 15, 15, 10, 2300L, 5L, 1600L, 400, 150,
     {0x0L, 0x0L, MW_VAMPIRE} },
  {"Earthmental", "Elem", "Earth Elem",
     "Formed from dirt and rocks, this creature's might is well respected",
     UP_EARTHTYPE | UP_FORTDAMAGE,
     AC_MONSTER,
     175, 5, 5, 15, 2500L, 6L, 2000L, 600, 200,
     {0x0L, 0x0L, MW_EARTH} },
  {"Minotaur", "mino", "minotaur",
     "Half Bull, Half Human; no one dares to say this creature is full of it",
     0x0L,
     AC_MONSTER,
     150, 20, 20, 10, 2500L, 8L, 1800L, 500, 250,
     {0x0L, 0x0L, MW_WYZARD} },
  {"Daemon", "daem", "daemon",
     "Full of might and power, a force to be feared",
     0x0L,
     AC_MONSTER,
     500, 50, 50, 10, 10000L, 10L, 6000L, 1300, 400,
     {MM_ORC, 0x0L, MW_WYZARD | MW_SORCERER} },
  {"Balrog", "Bal", "Balrog",
     "Demonic and full of Fire, a Bane of all life",
     UP_FIRETYPE | UP_FLIGHT,
     AC_MONSTER,
     500, 40, 40, 15, 12000L, 12L, 6000L, 1500, 400,
     {MM_ORC | MM_OGRE, 0x0L, MW_WYZARD | MW_SORCERER | MW_FIRE} },
  {"Dragon", "Drgn", "Dragon",
     "Perhaps the greatest of all beasts, this creature is most feared",
     UP_FIRETYPE | UP_FLIGHT | UP_FORTDAMAGE | UP_SPELLCAST,
     AC_MONSTER,
     1000, 50, 50, 20, 20000L, 15L, 10000L, 3000, 500,
     {MM_DRAGON | MM_OGRE | MM_ORC, 0x0L, MW_WYZARD | MW_SORCERER} },

  /*
   * Normal Army Units (46+ types) - Standard Military Forces
   *
   * This is the largest category, encompassing the conventional military units
   * that form the backbone of most armies. Units are organized by tactical role
   * and power level, providing diverse strategic options.
   *
   * Unit Classifications:
   * - AC_NORMAL: Standard infantry and elite troops
   * - AC_CAVALRY: Mounted units with enhanced mobility
   * - AC_ARCHERS: Ranged units with ballistic capabilities
   * - AC_SAILORS: Naval-capable units for amphibious operations
   * - AC_ORCISH: Monstrous humanoid forces
   * - AC_MERCS: Mercenary units requiring payment instead of population
   * - AC_UNIQUE: Specialized units with unique capabilities
   * - AC_SCOUT: Information gathering and reconnaissance
   * - AC_AGENT: Espionage and intelligence operations
   *
   * Power Progression:
   * 1. Basic Units: Militia, Goblins, Orcs, Infantry, Sailors
   * 2. Trained Units: Marines, Assault, Archers, Ninjas, Longbowmen
   * 3. Elite Units: Phalanx formations, Legionaries, Cavalry
   * 4. Specialized: Siege equipment, Flying units, Undead forces
   * 5. Advanced: Knights, Griffons, Elephants, Engineers
   *
   * Special Mechanics:
   * - Minimum Unit Requirements: UP_NEEDMIN for formation units
   * - Special Training: UP_SPCLTRAIN for elite capabilities
   * - Naval Operations: UP_UNLOAD, UP_NAVALTAKE for amphibious warfare
   * - Reconnaissance: UP_SIGHT, UP_SLIPPERY for scouting
   * - Economic: UP_PAYOFF for mercenary units
   */
  {"Militia", "mlta", "mlta",
     "Not very well trained or supplied, but when it is all you've got...",
     UP_HALFRECRUIT,
     AC_NORMAL,
     1, -40, -25, 0, 0L, 50L, 20L, 5, 8,
     {0x0L, 0x0L, 0x0L} },
  {"Goblins", "gob", "gbln",
     "Ever eager to die in the name of their horribly monstrous leader",
     0x0L,
     AC_ORCISH,
     1, -15, -15, 10, 80L, 70L, 20L, 8, 7,
     { MM_ORC, 0x0L, 0x0L } },
  {"Orcs", "orc", "orc",
     "Fearsome in mere numbers, these units provide capable soldiers",
     0x0L,
     AC_ORCISH,
     1, 0, 0, 10, 80L, 85L, 50L, 10, 8,
     { MM_ORC, 0x0L, 0x0L } },
  {"Infantry", "inf", "infy",
     "The basic military unit, given normal training and supplies",
     0x0L,
     AC_NORMAL,
     1, 0, 0, 10, 100L, 100L, 50L, 10, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Sailors", "Sail", "Sail",
     "Normal sea transported troops, these units may only take unowned land",
     UP_UNLOAD,
     AC_SAILORS,
     1, 0, 0, 0, 100L, 100L, 50L, 10, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Marines", "Marn", "XMrn",
     "Skilled naval based units, these units take and hold beach heads",
     UP_UNLOAD | UP_NAVALTAKE,
     AC_SAILORS,
     1, 5, 0, 0, 100L, 100L, 50L, 11, 11,
     {0x0L, MC_SAILOR, 0x0L} },
  {"Assault", "Aslt", "Aslt",
     "These elite naval based units are used to expand beach heads outward",
     UP_UNLOAD | UP_NAVALTAKE,
     AC_SAILORS,
     1, 10, 5, 5, 120L, 125L, 60L, 12, 11,
     {0x0L, MC_MARINE, 0x0L} },
  {"Archers", "arch", "arch",
     "Trained in the use of archery, these units provide excellent defense",
     UP_ANTIAIR | UP_BALLISTICS,
     AC_ARCHERS,
     1, 0, 10, 10, 100L, 100L, 50L, 11, 10,
     {MM_ARCHERY, 0x0L, 0x0L} },
  {"Uruk-Hai", "Uruk", "Uruk",
     "These skilled troops are better trained and supplied than normal Orcs",
     0x0L,
     AC_ORCISH,
     1, 5, 5, 10, 150L, 125L, 50L, 11, 10,
     { MM_OGRE | MM_ORC, 0x0L, 0x0L } },
  {"Ninjas", "Nnja", "Nnja",
     "Trained in scouting and combat, these units make excellent vanguards",
     UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN,
     AC_NORMAL,
     1, 20, 0, 10, 150L, 125L, 50L, 10, 10,
     {MM_NINJA, 0x0L, 0x0L} },
  {"Longbowmen", "Bmen", "bmen",
     "Archers armed with longbows provide an even more devastating barrage",
     UP_ANTIAIR | UP_BALLISTICS,
     AC_ARCHERS,
     1, 5, 15, 10, 150L, 150L, 65L, 11, 10,
     {MM_ARCHERY | MM_WARRIOR, 0x0L, 0x0L} },
  {"Phalanx", "Phax", "Plnx",
     "Trained to fight as a whole, such troops make good central units",
     UP_NEEDMIN,
     AC_NORMAL,
     500, 10, 10, 10, 150L, 150L, 60L, 11, 10,
     {MM_WARRIOR | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Bow_Phalanx", "Blnx", "BPlnx",
     "A well trained legion of bowmen these archers work best in unison",
     UP_ANTIAIR | UP_BALLISTICS | UP_NEEDMIN,
     AC_ARCHERS,
     500, 20, 20, 10, 160L, 170L, 80L, 11, 10,
     {MM_ARCHERY | MM_WARRIOR | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Olog-Hai", "Olog", "Olog",
     "These elite Orcish units form excellent fighting troops",
     0x0L,
     AC_ORCISH,
     1, 15, 15, 10, 150L, 180L, 75L, 11, 11,
     {MM_DRAGON | MM_OGRE | MM_ORC, MC_BREEDER, 0x0L} },
  {"Legionaries", "Legn", "Lgn",
     "These elite infantry troops are trained to fight as one",
     UP_NEEDMIN,
     AC_NORMAL,
     1000, 20, 20, 10, 150L, 180L, 80L, 11, 11,
     {MM_WARRIOR | MM_CAPTAIN | MM_WARLORD, 0x0L, 0x0L} },
  {"Dragoons", "Dgon", "Dgoon",
     "Mobile and well supplied, these troops perform fast powerful strikes",
     0x0L,
     AC_CAVALRY,
     1, 10, 10, 15, 100L, 200L, 150L, 11, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Full_Bowmen", "Fbow", "Fbow",
     "A well trained legion of bowmen these archers work best in unison",
     UP_ANTIAIR | UP_BALLISTICS | UP_NEEDMIN,
     AC_ARCHERS,
     1000, 30, 30, 10, 160L, 200L, 110L, 12, 10,
     {MM_ARCHERY | MM_WARRIOR | MM_WARLORD | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Crossbows", "Cbow", "Cbow",
     "Crossbow archers provide a powerful offense, but are easy to attack",
     UP_ANTIAIR | UP_BALLISTICS,
     AC_ARCHERS,
     1, 40, 0, 10, 200L, 200L, 130L, 12, 10,
     {MM_ARCHERY | MM_WARRIOR | MM_WARLORD | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Mercenaries", "Merc", "Merc",
     "Soldiers for hire, they need only be paid to do their work",
     UP_PAYOFF | UP_ANYDISB,
     AC_MERCS,
     1, 0, 0, 10, 0L, 225L, 100L, 10, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Arch", "MArc", "archers",
     "Archers for hire, they need only be paid to do their work",
     UP_PAYOFF | UP_ANYDISB | UP_ANTIAIR | UP_BALLISTICS,
     AC_MERCS,
     1, 0, 10, 10, 0L, 250L, 110L, 10, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Asslt", "MAst", "Assault",
     "Pirates for hire, they need only be paid to do their work",
     UP_PAYOFF | UP_ANYDISB | UP_UNLOAD | UP_NAVALTAKE,
     AC_MERCS,
     1, 10, 5, 5, 0L, 275L, 120L, 10, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Drag", "Mdgn", "Dragoons",
     "Dragoon fighters who are available at a price",
     UP_PAYOFF | UP_ANYDISB,
     AC_MERCS,
     1, 10, 10, 15, 0L, 300L, 160L, 11, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Lt_Cav", "Mlcv", "Lt Cavalry",
     "Lighly armored cavalry, they are available at the right price",
     UP_PAYOFF | UP_ANYDISB,
     AC_MERCS,
     1, 20, 20, 20, 0L, 350L, 200L, 11, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Hv_Cav", "Mhcv", "Hvy Cavalry",
     "Armored cavalry, they are available for the right price",
     UP_PAYOFF | UP_ANYDISB,
     AC_MERCS,
     1, 30, 30, 18, 0L, 500L, 275L, 12, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Merc_Rocs", "MRoc", "Rocs",
     "Flying mercenary troops are the top of the line paid troops",
     UP_PAYOFF | UP_ANYDISB | UP_FLIGHT | UP_ARROWWEAK,
     AC_MERCS,
     1, 20, 30, 10, 0L, 750L, 400L, 12, 9,
     {0x0L, 0x0L, 0x0L} },
  {"Trolls", "Trol", "Trll",
     "Strong magical beasts enlisted to serve within the Orcish military",
     0x0L,
     AC_UNIQUE,
     1, 25, 15, 10, 200L, 225L, 100L, 13, 12,
     {MM_DRAGON | MM_OGRE | MM_ORC, 0x0L, MW_WYZARD} },
  {"Inf. Elite", "IElt", "IElt",
     "Well trained and supplied, these troops provide swifter foot soldiers",
     0x0L,
     AC_NORMAL,
     1, 20, 20, 13, 200L, 225L, 100L, 13, 11,
     {MM_ARMOR, 0x0L, 0x0L }},
  {"Hvy_Bowmen", "Hbow", "Hbow",
     "Toughly armored, and heavily supplied, these bowmen are formidable",
     UP_ANTIAIR | UP_BALLISTICS | UP_NEEDMIN,
     AC_ARCHERS,
     2000, 40, 30, 10, 225L, 240L, 120L, 13, 10,
     {MM_ARCHERY | MM_ARMOR | MM_WARRIOR | MM_WARLORD | MM_CAPTAIN, 0x0L, 0x0L }},
  {"Lt_Cavalry", "lCav", "lCav",
     "Lightly armored cavalry troops operate as a very fast military force",
     0x0L,
     AC_CAVALRY,
     1, 20, 20, 20, 100L, 300L, 175L, 13, 10,
     {MM_EQUINE | MM_WARRIOR, 0x0L, 0x0L} },
  {"Hv_Cavalry", "hCav", "hCav",
     "Sacrificing some speed for protection, these horsemen are well supplied",
     0x0L,
     AC_CAVALRY,
     1, 30, 30, 18, 300L, 450L, 225L, 14, 11,
     {MM_EQUINE | MM_WARRIOR | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Catapults", "cat", "cplt",
     "Powerful catapults are useful in or about fortified positions",
     UP_BALLISTICS | UP_COVERBONUS | UP_SAPPERUNIT,
     AC_UNIQUE,
     500, -20, -20, 5, 800L, 600L, 250L, 16, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Siege_Engs", "sge", "sieg",
     "Troop platforms and battering rams, useful in assaulting fortifications",
     UP_FORTDAMAGE | UP_COVERBONUS | UP_SAPPERUNIT,
     AC_UNIQUE,
     400, -20, -20, 5, 800L, 600L, 250L, 18, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Rocs", "Roc", "Roc",
     "Riding large powerful birds, soldiers make excellent support units",
     UP_FLIGHT | UP_ARROWWEAK,
     AC_UNIQUE,
     1, 20, 30, 10, 300L, 600L, 250L, 13, 10,
     {MM_AVIAN | MM_WARRIOR, 0x0L, 0x0L} },
  {"Knights", "Kni", "Kngt",
     "Heavily armored and supplied, lords on horseback make good fighters",
     UP_ARROWWEAK,
     AC_CAVALRY,
     1, 40, 40, 20, 600L, 600L, 250L, 15, 12,
     {MM_ARMOR | MM_EQUINE | MM_WARLORD | MM_WARRIOR | MM_CAPTAIN, 0x0L, 0x0L} },
  {"Griffons", "Grif", "Grfn",
     "Highly skilled aerial troops are a formidable weapon",
     UP_FLIGHT | UP_ARROWWEAK,
     AC_UNIQUE,
     1, 40, 50, 15, 400L, 800L, 250L, 15, 10,
     {MM_AVIAN | MM_WARRIOR | MM_CAPTAIN | MM_WARLORD, 0x0L, 0x0L} },
  {"Elephants", "ele", "elpnt",
     "Large powerful beasts create a fearsome mobile force",
     0x0L,
     AC_UNIQUE,
     1, 50, 50, 5, 600L, 600L, 250L, 20, 13,
     {0x0L, MC_DERVISH, 0x0L} },
  {"Engineers", "Eng", "Engr",
     "Engineers skilled at building things, or even destroying them",
     UP_FORTDAMAGE | UP_SAPPERUNIT,
     AC_UNIQUE,
     1, 10, 10, 8, 500L, 500L, 200L, 13, 25,
     {0x0L, 0x0L, 0x0L} },
  {"Ghosts", "Gho", "@Ghst",
     "The spirits of undead creatures, ghosts can still entrap living souls",
     UP_UNDEAD | UP_FREESUPPLY | UP_ANYDISB | UP_NODRAFT,
     AC_UNIQUE,
     1, -55, -55, 5, 100L, 100L, 0L, 2, 1,
     {0x0L, 0x0L, MW_VAMPIRE} },
  {"Skeletons", "Ske", "%Sktn",
     "Walking bones, these units are frightful by their actions",
     UP_UNDEAD | UP_FREESUPPLY | UP_DECAY | UP_ANYDISB | UP_NODRAFT,
     AC_UNIQUE,
     1, -35, -35, 5, 100L, 100L, 0L, 6, 3,
     {0x0L, 0x0L, MW_VAMPIRE} },
  {"Zombies", "zom", "zomb",
     "Undead, and able to absorb losses, such units chill enemy troops",
     UP_UNDEAD | UP_FREESUPPLY | UP_DECAY | UP_ANYDISB,
     AC_UNIQUE,
     1, -25, -25, 5, 100L, 100L, 0L, 8, 2,
     {0x0L, 0x0L, MW_VAMPIRE} },
  {"Scout", "Scout", "1Sct",
     "Not skilled enough to provide detail, scouts still give useful info",
     UP_SLIPPERY | UP_FREESUPPLY | UP_ANYDISB,
     AC_SCOUT,
     1, -30, -30, 10, 0L, 100L, 100L, 0, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Spy", "Spy", "2Spy",
     "Specialy trained scouts, skilled in surveillance tactics",
     UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN | UP_ANYDISB,
     AC_SCOUT,
     1, -30, -30, 10, 0L, 10000L, 2000L - ARMYUNITCOST, 0, 10,
     {0x0L, 0x0L, 0x0L} },
  {"Cavalry Scout", "CScout", "3CSct",
     "Mounted scouts provide even swifter surveillance",
     UP_SLIPPERY | UP_FREESUPPLY | UP_ANYDISB,
     AC_SCOUT,
     1, -15, -15, 20, 150L, 1000L, 1000L, 0, 10,
     { MM_EQUINE, 0x0L, 0x0L } },
  {"Winged Scout", "WScout", "4WSct",
     "Flying scouts provide the swifted and most maneuverable informants",
     UP_SLIPPERY | UP_FREESUPPLY | UP_FLIGHT | UP_ANYDISB,
     AC_SCOUT,
     1, 0, 0, 20, 200L, 1500L, 1500L, 0, 10,
     { MM_AVIAN, 0x0L, 0x0L }},
  {"Mounted Spy", "MSpy", "5MSpy",
     "A spy on horseback.  What a bargain!",
     UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN | UP_ANYDISB,
     AC_SCOUT,
     1, -20, -20, 20, 0L, 15000L, 3000L - ARMYUNITCOST, 0, 10,
     { MM_EQUINE, 0x0L, 0x0L } },
  {"Winged Spy", "WSpy", "6WSpy",
     "A spy on riding on a winged steed.  Can't be beat!",
     UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN | UP_FLIGHT | UP_ANYDISB,
     AC_SCOUT,
     1, -10, -10, 20, 0L, 20000L, 4000L - ARMYUNITCOST, 0, 10,
     { MM_AVIAN, 0x0L, 0x0L }},
  {"Informant", "Inform", "7Infmt",
     "Well paid informants provide some views of neighbor's lands",
     UP_PAYOFF | UP_SLIPPERY | UP_ENLISTAWAY | UP_ANYDISB,
     AC_AGENT,
     1, -30, -30, 5, 0L, 6000L, 2000L - ARMYUNITCOST, 0, 5,
     {0x0L, 0x0L, 0x0L} },
  {"Agent", "Agent", "8Agnt",
     "Very well paid agents provide much needed information",
     UP_PAYOFF | UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN |
       UP_ENLISTAWAY | UP_ANYDISB,
     AC_AGENT,
     1, -30, -30, 5, 0L, 60000L, 30000L - ARMYUNITCOST, 0, 5,
     {0x0L, 0x0L, 0x0L} },
  {"Surveyor", "Srvyr", "0Srvyr",
     "A highly skilled scout and land evaluation expert",
     UP_SIGHT | UP_SLIPPERY | UP_SPCLTRAIN | UP_MAPPING,
     AC_SCOUT,
     1, -30, -30, 4, 0L, 10000L, 5000L - ARMYUNITCOST, 750, 10,
     {0x0L, 0x0L, 0x0L} }
};

/*
 * traits_list[] - Human-Readable Unit Trait Descriptions
 *
 * This array provides descriptive names for all unit special abilities and
 * traits used throughout the military system. These descriptions correspond
 * to the UP_* bit flags defined in armyX.h and used in the ainfo_list[].
 *
 * The traits represent special capabilities, limitations, and characteristics
 * that modify unit behavior beyond basic combat statistics:
 *
 * Command Traits: "Ruler" (territorial control)
 * Movement Traits: "Slippery" (evasion), "Flight" (aerial movement)
 * Nature Traits: "Undead" (magical immunity), elemental types
 * Combat Traits: "Anti-Air", "Balistics", "Damaging", "Arrowweak"
 * Naval Traits: "Beachhead", "Assault" (amphibious capabilities)
 * Special Traits: "Sight" (reconnaissance), "NeedMin" (formation requirements)
 * Economic Traits: "Payoff" (mercenary), "Free-Support" (no upkeep)
 * Advanced Traits: "Spellcaster", "Fullcaster" (magical abilities)
 * Utility Traits: "Mapping", "Sapper", "Coverbonus"
 *
 * Usage: These strings are used in user interfaces to display unit capabilities
 * and in help systems to explain unit special abilities to players.
 *
 * Defines corresponding to bit flags are located in armyX.h
 */
char *traits_list[] = {
  "Ruler", "Slippery", "Flight", "Undead", "Anti-Air", "Balistics",
  "Beachhead", "Assault", "Sight", "NeedMin", "Damaging",
  "Arrowweak", "Payoff", "Coverbonus", "Disb-Always",
  "1/2-Recruits", "Trained", "Fire", "Water", "Earth",
  "Free-Support", "Decays", "Spellcaster", "Fullcaster",
  "Remote-Enlist", "Sapper", "Mapping", "Nodraft"
};

/*
 * ainfo_clist[] - Military Unit Classification Names
 *
 * This array provides human-readable names for the military unit classification
 * system used to organize and categorize different types of army units. Each
 * entry corresponds to an AC_* constant defined in armyX.h.
 *
 * Classifications organize units by tactical role and command structure:
 * - "Leader": Command units with territorial control capabilities
 * - "Caster": Magical support units with spellcasting abilities
 * - "Monster": Supernatural creatures with unique powers
 * - "Normal": Standard infantry and conventional military forces
 * - "Scout": Reconnaissance and information gathering units
 * - "Agent": Espionage and intelligence operations specialists
 * - "Merc": Mercenary forces requiring payment rather than population
 * - "Cavalry": Mounted units with enhanced mobility and combat power
 * - "Sailors": Naval-capable units for amphibious and maritime operations
 * - "Orcish": Monstrous humanoid forces with special characteristics
 * - "Archers": Ranged combat specialists with ballistic weapons
 * - "Unique": Specialized units with distinctive capabilities
 *
 * Usage: These classifications are used throughout the game engine for:
 * - Unit recruitment and availability restrictions
 * - Combat calculation modifiers
 * - User interface organization and display
 * - AI decision-making and unit evaluation
 *
 * Definitions are located in armyX.h with AC_* constants
 */
char *ainfo_clist[] = {
  "Leader", "Caster", "Monster", "Normal", "Scout", "Agent",
  "Merc", "Cavalry", "Sailors", "Orcish", "Archers", "Unique"
};

/*
 * ninfo_list[] - Naval Vessel Specifications
 *
 * This array defines the complete naval system, specifying the capabilities
 * and characteristics of all ship types available in the game. Naval vessels
 * serve as transport and combat platforms for maritime operations.
 *
 * Structure: NINFO_STRUCT entries containing:
 * - Vessel name and single-character abbreviation
 * - Combat strength and cargo capacity
 * - Construction and maintenance costs
 * - Base combat effectiveness rating
 *
 * Ship Types and Roles:
 * - "Warship": Primary naval combat vessel with high fighting strength
 * - "Merchants": Cargo-focused ships with maximum transport capacity
 * - "Galleys": Balanced combat and transport capabilities
 * - "Barges": Economical transport with minimal combat ability
 *
 * Game Balance:
 * - Warships excel in naval combat but have limited cargo space
 * - Merchants maximize cargo capacity at the cost of combat effectiveness
 * - Galleys provide versatile medium capabilities for balanced fleets
 * - Barges offer economical transport for peaceful expansion
 *
 * Cargo capacity determines how many army units each ship can transport,
 * critical for amphibious operations and overseas expansion.
 *
 * NSHP_NUMBER constant defines the total number of naval vessel types
 */
NINFO_STRUCT ninfo_list[NSHP_NUMBER] = {
  { "Warship", "w", 30, NAVY_HOLD,
      20000L, 1000L, 95 },
  { "Merchants", "m", 20, NAVY_HOLD,
      25000L, 1500L, 50 },
  { "Galleys", "g", 25, NAVY_HOLD,
      22000L, 1100L, 75 },
  { "Barges", "b", 10, NAVY_HOLD,
      18000L, 500L, 75 }
};

/*
 * stat_info[] - Army Status Definitions and Behavioral Modifiers
 *
 * This array defines the complete set of army statuses that control unit
 * behavior, combat effectiveness, and available actions during gameplay.
 * Each status represents a tactical or strategic state that modifies how
 * units operate within the game engine.
 *
 * Structure: STATUS_STRUCT entries containing:
 * - Status names (full, abbreviated, command forms)
 * - Priority level for status conflict resolution
 * - Status flags (SPST_* constants) defining behavioral restrictions/bonuses
 * - Combat modifiers (attack and defense percentage adjustments)
 *
 * Status Categories:
 *
 * 1. Combat Statuses:
 * - "Sortie": Aggressive attack from fortified position
 * - "Ambush": Hidden defensive position with attack bonus
 * - "Attack": Standard offensive combat stance
 * - "Engage": Lock combat with specific enemy force
 * - "Defend": Defensive posture with defense bonus
 * - "Siege": Laying siege to fortified enemy position
 *
 * 2. Garrison/Fortification:
 * - "Garrison": Fortified defensive position
 * - "Sieged": Under siege by enemy forces
 * - "Reserve": Protected rear-area positioning
 *
 * 3. Naval Operations:
 * - "Onboard": Transported on naval vessels
 * - "Onbrd Sppt": Providing supply support while onboard
 * - "OnbSelfSppt": Self-supporting while onboard
 *
 * 4. Support Operations:
 * - "Support": Providing supply to other units
 * - "Self Sppt": Self-sustaining without external supply
 * - "Carry": Transporting supplies or equipment
 *
 * 5. Special Operations:
 * - "Grouped": Combined unit formations
 * - "Sweep": Area patrol and reconnaissance
 * - "WorkCrew": Construction and engineering tasks
 * - "Traded": Commercial activity
 * - "Lure": Deception and misdirection
 * - "Repair": Maintenance and restoration
 * - "Rover": Independent patrol and exploration
 *
 * Status flags control movement restrictions, combat eligibility, supply
 * requirements, and special abilities. Priority determines which status
 * takes precedence when multiple statuses could apply.
 */
STATUS_STRUCT stat_info[] = {
  { "Sortie", "Sortie", "Attack", 1,
      SPST_ATT | SPST_UNCOMB | SPST_UNSWITCH | SPST_UNMOVE |
	SPST_SIEGED | SPST_SPLYINSECT | SPST_RISKY | SPST_ASTATUS,
	50, 20 },
  { "Ambush", "Ambush", "Ambush", 1,
      SPST_ATT | SPST_HIDDEN | SPST_UNMOVE | SPST_SORTIE |
	SPST_GROUNDED | SPST_SPLYINSECT | SPST_USABLE | SPST_ASTATUS,
      30, 30 },
  { "Attack", "Att", "attack", 1,
      SPST_ATT | SPST_CAPT | SPST_USABLE | SPST_ASTATUS | SPST_NSTATUS,
      20, 0 },
  { "Engage", "Eng", "Engage", 1,
      SPST_ATT | SPST_CAPT | SPST_DIST | SPST_UNMOVE |
	SPST_FORT | SPST_USABLE | SPST_ASTATUS | SPST_NSTATUS,
      20, 0 },
  { "Defend", "Dfd", "defend", 1,
      SPST_USABLE | SPST_ASTATUS,
      0, 20 },
  { "Garrison", "Grsn", "garrison", 1,
      SPST_UNMOVE | SPST_FORT | SPST_SIEGED | SPST_SORTIE |
	SPST_HOLDIT | SPST_USABLE | SPST_ASTATUS,
      0, 30 },
  { "Onboard", "Onbrd", "Onboard", 1,
      SPST_UNCOMB | SPST_NOLIKE | SPST_UNSWITCH | SPST_UNSPLIT | SPST_ONSHIP |
	SPST_UNMOVE | SPST_NOHOLD | SPST_SPLYINSECT | SPST_DIESLAST |
	SPST_NOCAST | SPST_ASTATUS | SPST_VSTATUS,
      0, 30 },
  { "Siege", "Siege", "Lay Siege", 1,
      SPST_CAPT | SPST_UNMOVE | SPST_USABLE | SPST_ASTATUS,
      0, 20 },
  { "Grouped", "Grp", "Group", 1,
      SPST_UNSWITCH | SPST_NOCAST | SPST_ASTATUS,
      0, 0 },
  { "Sieged", "Sieged", "Sieged", 1,
      SPST_SIEGED | SPST_SORTIE | SPST_UNSWITCH | SPST_UNMOVE |
	SPST_UNCOMB | SPST_HOLDIT | SPST_FORT | SPST_SPLYINSECT |
	SPST_ASTATUS | SPST_NSTATUS | SPST_VSTATUS,
      0, -20 },
  { "Reserve", "Resrv", "Reserve", 1,
      SPST_SORTIE | SPST_UNMOVE | SPST_FORT | SPST_HOLDIT |
	SPST_DIESLAST | SPST_USABLE | SPST_ASTATUS,
      0, 0 },
  { "Sweep", "Swp", "Sweep", 2,
      SPST_ATT | SPST_CAPT | SPST_SWEEP | SPST_USABLE | SPST_ASTATUS,
      0, 0 },
  { "WorkCrew", "Wcrew", "Wcrew", 2,
      SPST_SIEGED | SPST_FORT | SPST_UNMOVE | SPST_UNCOMB | SPST_NOCAST |
	SPST_NOLIKE | SPST_UNSPLIT | SPST_UNSWITCH | SPST_ITEMINFO |
	SPST_ASTATUS | SPST_NSTATUS,
      0, -40 },
  { "Traded", "Trade", "Trade", 1,
      SPST_SIEGED | SPST_FORT | SPST_UNMOVE | SPST_UNCOMB | SPST_NOCAST |
	SPST_NOLIKE | SPST_UNSPLIT | SPST_UNSWITCH | SPST_ITEMINFO |
	SPST_ASTATUS | SPST_NSTATUS | SPST_VSTATUS,
      0, 0 },
  { "Carry", "Car", "Carry", 1,
      SPST_USABLE | SPST_NSTATUS | SPST_VSTATUS,
      10, 10 },
  { "Lure", "Lur", "Lure",  1,
      SPST_DECOY | SPST_USABLE | SPST_NSTATUS | SPST_VSTATUS,
      0, 20 },
  { "Support", "Sup", "Support", 3,
      SPST_SUPPLY | SPST_USABLE | SPST_NSTATUS | SPST_VSTATUS,
      0, 0 },
  { "Repair", "Rpr", "Repair", 1,
      SPST_UNMOVE | SPST_UNSWITCH | SPST_UNCOMB | SPST_NSTATUS |
	SPST_VSTATUS,
      0, -30 },
  { "Onbrd Sppt", "ObS", "OnbSppt", 1,
      SPST_UNCOMB | SPST_NOLIKE | SPST_UNSWITCH | SPST_UNSPLIT |
	SPST_SUPPLY | SPST_UNMOVE | SPST_SPLYINSECT | SPST_ONSHIP |
	SPST_VSTATUS,
      0, 20 },
  { "Self Sppt", "SSp", "SelfSppt", 4,
      SPST_SSUPPLY | SPST_USABLE | SPST_NSTATUS | SPST_VSTATUS,
      0, 0 },
  { "OnbSelfSppt", "OSS", "OnbSSppt", 1,
      SPST_UNCOMB | SPST_NOLIKE | SPST_UNSWITCH | SPST_UNSPLIT |
	SPST_SUPPLY | SPST_UNMOVE | SPST_SPLYINSECT | SPST_ONSHIP |
	SPST_VSTATUS | SPST_SSUPPLY,
      0, 20 },
  { "Rover", "Rvr", "Rover", 3,
      SPST_ROVER | SPST_ASTATUS | SPST_CAPT | SPST_ATT | SPST_USABLE,
      10, 0 }
};

/*
 * Dynamic Array Sizing Variables
 *
 * These variables provide runtime calculation of array sizes for the military
 * data structures. They enable dynamic memory management and bounds checking
 * throughout the game engine without requiring hardcoded constants.
 *
 * The sizeof() calculations ensure that adding or removing entries from the
 * data arrays automatically updates the corresponding size variables, making
 * the system maintainable and reducing the risk of array bounds errors.
 *
 * Variables:
 * - num_armytypes: Total number of army unit types in ainfo_list[]
 * - num_atraits: Total number of unit traits in traits_list[]
 * - num_aclasses: Total number of army classifications in ainfo_clist[]
 * - num_statuses: Total number of army statuses in stat_info[]
 *
 * Usage: These variables are used throughout the codebase for:
 * - Loop bounds checking when iterating through arrays
 * - Dynamic memory allocation for unit-related data structures
 * - Validation of array indices in unit lookup operations
 * - User interface enumeration of available options
 *
 * The automatic sizing ensures that modifications to the data arrays
 * don't require manual updates to size constants throughout the code.
 */
int num_armytypes = (sizeof(ainfo_list) / sizeof(AINFO_STRUCT));
int num_atraits = (sizeof(traits_list) / sizeof(char *));
int num_aclasses = (sizeof(ainfo_clist) / sizeof(char *));
int num_statuses = (sizeof(stat_info) / sizeof(STATUS_STRUCT));
