#ifndef GAMECONSTANTS_HPP_INCLUDED
#define GAMECONSTANTS_HPP_INCLUDED

#define CHANNEL_INFO 	'i'
#define CHANNEL_MAP 	'*'
#define CHANNEL_PARTY 	'$'
#define CHANNEL_TEAM 	'#'
#define CHANNEL_PRIVATE 'p'
#define CHANNEL_GUILD 	'%'
#define CHANNEL_PVP 	'!'
#define CHANNEL_RECRUIT ':'
#define CHANNEL_TRADE 	'?'
#define CHANNEL_MEETIC 	'^'
#define CHANNEL_ADMIN 	'@'

namespace Naia
{
	/* ***************
	 * MAP FUNCTIONS *
	 ****************/

	pair<int, int> get_start_location (int classID);
	int hash_to_int (char c);
	int cell_code_to_ID (const string & code);
	inline string cellID_to_code (int cellID) { return to_string<char> (HASH[cellID / 64]) + to_string<char> (HASH[cellID % 64]); }

	/* *********************
	 * ALIGNMENT FUNCTIONS *
	 **********************/

	int get_align_rank (int honor);

	/* **************
	 * ENUMERATIONS *
	 * *************/

	enum Stat
	{
		VITALITY,
		WISDOM,
		STRENGTH,
		INTELLIGENCE,
		CHANCE,
		AGILITY,
		LIFE, // obsolete
		AP,
		MP,
		RANGE,
		INVOCATIONS,
		INITIATIVE,
		PROSPECTING,
		POD,

		DAMAGES,
		MASTERY,
		PDAMAGES,
		MULTIPLY_DAMAGES, // obsolete
		TRAP_DAMAGES,
		TRAP_PDAMAGES,
		HEAL,
		REFLECT_DAMAGES,
		CRITICAL_HIT,
		CRITICAL_MISS,
		AP_LOSS_RESISTANCE,
		MP_LOSS_RESISTANCE,

		NEUTRAL_RESISTANCE,
		NEUTRAL_PRESISTANCE,
		EARTH_RESISTANCE,
		EARTH_PRESISTANCE,
		FIRE_RESISTANCE,
		FIRE_PRESISTANCE,
		WATER_RESISTANCE,
		WATER_PRESISTANCE,
		AIR_RESISTANCE,
		AIR_PRESISTANCE,

		NEUTRAL_RESISTANCE_PVP,
		NEUTRAL_PRESISTANCE_PVP,
		EARTH_RESISTANCE_PVP,
		EARTH_PRESISTANCE_PVP,
		FIRE_RESISTANCE_PVP,
		FIRE_PRESISTANCE_PVP,
		WATER_RESISTANCE_PVP,
		WATER_PRESISTANCE_PVP,
		AIR_RESISTANCE_PVP,
		AIR_PRESISTANCE_PVP,

		WATER_DAMAGES_REDUCTION,
		AIR_DAMAGES_REDUCTION,
		FIRE_DAMAGES_REDUCTION,
		EARTH_DAMAGES_REDUCTION,

	};

	enum IOState
	{
		FULL = 1,
		EMPTYING,
		EMPTY,
		EMPTY2,
		REPLENISHING,
	};
}

#endif // GAMECONSTANTS_HPP_INCLUDED
