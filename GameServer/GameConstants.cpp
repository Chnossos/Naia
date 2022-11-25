#include "Naia.hpp"

pair<int, int> Naia::get_start_location (int classID)
{
	if (kernel.config.get_property<bool> (CUSTOM_START_PARAM, CUSTOM_START))
		return make_pair (kernel.config.get_property<int> (CUSTOM_START_MAP_PARAM, CUSTOM_START_MAP), kernel.config.get_property<int> (CUSTOM_START_CELL_PARAM, CUSTOM_START_CELL));
	else
		switch (classID)
		{
			case 1: // FECA
				return make_pair (10300, 323);
			case 2: // OSAMODAS
				return make_pair (10284, 385);
			case 3: // ENU
				return make_pair (10299, 299);
			case 4: // SRAM
				return make_pair (10285, 264);
			case 5: // XELOR
				return make_pair (10298, 314);
			case 6: // ECAFLIP
				return make_pair (10276, 310);
			case 7: // ENIRIPSA
				return make_pair (10283, 298);
			case 8: // IOP
				return make_pair (10294, 295);
			case 9: // CRÂ
				return make_pair (10292, 298);
			case 10: // SADIDA
				return make_pair (10279, 298);
			case 11: // SACRIEUR
				return make_pair (10296, 256);
			case 12: // PANDAWA
				return make_pair (10289, 265);
			default:
				return make_pair (0, 0);
		}
}

int Naia::hash_to_int (char c)
{
	for (size_t i = 0 ; i < sizeof (HASH) ; ++i)
	{
		if (HASH[i] == c)
			return i;
	}
	return -1;
}

int Naia::cell_code_to_ID (const string & code)
{
	char a = code[0], b = code[1];
	int code1 = 0, code2 = 0;
	size_t i = 0;
	while (i++ < sizeof (HASH))
	{
		if (HASH[i] == a)
			code1 = i * 64;

		if (HASH[i] == b)
			code2 = i;
	}
	return code1 + code2;
}

int Naia::get_align_rank (int honor)
{
	if (honor < 500)
		return 1;
	else if (honor < 1500)
		return 2;
	else if (honor < 3000)
		return 3;
	else if (honor < 5000)
		return 4;
	else if (honor < 7500)
		return 5;
	else if (honor < 10000)
		return 6;
	else if (honor < 12500)
		return 7;
	else if (honor < 15000)
		return 8;
	else if (honor < 17500)
		return 9;
	return 10;
}
