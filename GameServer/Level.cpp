#include "Naia.hpp"

initialize_singleton (LevelManager);

bool LevelManager::load ()
{
	QueryResult * QR = gsDatabase->query ("SELECT * FROM experience;");
	if (!QR)
	{
		kernel.log.print_error ("There is no level to load !\n");
		return false;
	}
	do
	{
		Field * fields = QR->fetch();
		unsigned level = fields[0].get_value<int>();

		for (size_t i = XP_CHARACTER ; i < XP_COUNT ; ++i)
		{
			uint64_t xp = fields[i + 1].get_value<uint64_t>();
			if (xp || level == 1)
				_container[i][level] = xp;
		}
	}
	while (QR->next_row());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Characters \t: %u levels loaded.\n", _container[XP_CHARACTER].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Jobs \t\t: %u levels loaded.\n", _container[XP_JOB].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "DragoTurkeys \t: %u levels loaded.\n", _container[XP_TURKEY].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Guilds \t\t: %u levels loaded.\n", _container[XP_GUILD].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Livitinems \t: %u levels loaded.\n", _container[XP_LIVITINEM].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Incarnations \t: %u levels loaded.\n", _container[XP_INCARNATION].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "Bandits \t\t: %u levels loaded.\n", _container[XP_BANDIT].size());
	kernel.log.print_header (TWHITE, false, "[LevelManager]", "PvP \t\t: %u levels loaded.\n", _container[XP_HONOR].size());
	delete QR;
	return true;
}

uint64_t LevelManager::get_xp (XPType type, unsigned level) const
{
	const map<unsigned, uint64_t> & xp_type = _container[type];
	map<unsigned, uint64_t>::const_iterator it = xp_type.find (level);
	return it == xp_type.end() ? 0 : it->second;
}

unsigned LevelManager::get_level (XPType type, uint64_t xp) const
{
	if (xp == 0)
		return 1;
	map<unsigned, uint64_t> xp_type = _container[type];
	for (map<unsigned, uint64_t>::const_iterator it = xp_type.begin() ; it != xp_type.end() ; ++it)
	{
		if (xp >= xp_type[it->first] && (it->first + 1 <= xp_type.size() && xp < xp_type[it->first + 1]))
			return it->first;
	}
	return get_max_level (type);
}

/* *************
 * CLASS LEVEL *
 **************/

Level::Level (XPType type, unsigned level, uint64_t xp) :
	_type (type), _level (level), _xp (xp)
{
	if (_type == XP_CHARACTER && level == 1 && xp == 0)
	{
		if (kernel.config.get_property<bool> (START_LEVEL_LIMIT_PARAM, START_LEVEL_LIMIT) && level < _level) // On est descendu en dessous du niveau de départ
		{
			_level = kernel.config.get_property<unsigned> (START_LEVEL_PARAM, START_LEVEL);
			_xp = LevelManager::instance().get_xp (type, _level);
		}
	}
}

void Level::set_level (XPType type, unsigned level)
{
	int base_lvl = _level;
	unsigned min_level = kernel.config.get_property<unsigned> (START_LEVEL_PARAM, START_LEVEL);
	unsigned max_level = LevelManager::instance().get_max_level (type);

	if (type == XP_CHARACTER && max_level == 201)
		max_level = 200;

	if (kernel.config.get_property<bool> (START_LEVEL_LIMIT_PARAM, START_LEVEL_LIMIT) && level < min_level) // On est descendu en dessous du niveau de départ
		_level = min_level;
	else if (level < 1) // Sous le niveau 1
		_level = 1;
	else if (level > max_level) // Au-dessus du niveau max
		_level = max_level;
	else // on est bon
		_level = level;

	uint64_t base_xp = LevelManager::instance().get_xp (_type, base_lvl);
	uint64_t curr_xp = LevelManager::instance().get_xp (type, _level);

	if (base_xp < curr_xp)
		add_xp (curr_xp - base_xp);
	if (base_xp > curr_xp)
		add_xp (base_xp - curr_xp);
}

void Level::add_xp (int64_t xp)
{
	if (xp == 0)
		return;

	else if (xp < 0 && _xp < abs(xp)) // Si on retire plus d'xp qu'il n'en a
		set_level (_type, 1);

	_xp += xp;

	uint64_t stage_xp = LevelManager::instance().get_xp (_type, _level);
	int level = LevelManager::instance().get_level (_type, _xp);
	int diff = level - _level;

	if (_xp > stage_xp)
		on_level_up (abs(diff));
	else if (_xp < stage_xp)
		on_level_down (abs(diff));
}
