#include "Naia.hpp"

Map::Map (int ID, const string & date, int w, int h, int x, int y, int subarea, const string & key, const string & data, const string & mobs, int max_mobs, int max_grps, int capacity) :
	_init (false),
	_ID (ID),
	_width (w),
	_height (h),
	_subarea (subarea),
	_capacity (capacity),
	_max_groups (max_grps),
	_max_monsters (max_mobs),
	_date (date),
	_key (key),
	_monsters (mobs),
	_data (data),
	_coordonates (x, y)
{
	parse_map_data (_data);
	_data.clear();
	//GenerateCreatureGroups();
	_init = true;
}

/* *******************
 * LOADING FUNCTIONS *
 * ******************/

void Map::parse_map_data (const string & data)
{
	int type, layerObject;
	bool los, interactive;
	size_t size = data.size();
	if (size % 10 != 0)
	{
		int cell_number = _width * _height;
		_cells.resize (cell_number, Cell());
		for (int i = 0 ; i < cell_number ; ++i)
		{
			_cells[i]._ID = i;
			_cells[i]._los = false;
			_cells[i]._walkable = false;
		}
		return;
	}
	_cells.resize (size / 10, Cell());
	int cells[10];
	for (size_t i = 0 ; i < size ; i += 10)
	{
		for (int j = 0 ; j < 10 ; ++j)
			cells[j] = Naia::hash_to_int (data.at (i + j));
		type = (cells[2] & 56) >> 3;
		los = cells[0] & 1;
		interactive = (cells[7] & 2) >> 1;
		layerObject = ((cells[0] & 2) << 12) + ((cells[7] & 1) << 12) + (cells[8] << 6) + cells[9];
		_cells[i / 10]._ID = i / 10;
		_cells[i / 10]._los = los;
		_cells[i / 10]._walkable = type != 0;
		if (interactive)
		{
			InteractiveObject * IO = new InteractiveObject;
			IO->_state = Naia::FULL;
			//IO->_template = world.get_IOTemplate (layerObject);
			_iobjects[i / 10] = IO;
		}
	}
}

/* ***********************
 * PATHFINDING FUNCTIONS *
 * **********************/

int Map::get_cell_from_direction (int cell, char direction)
{
	int cellID = -1;
	switch (direction)
	{
		case 'a':
			cellID = cell + 1;
		break;
		case 'b':
			cellID = cell + _width;
		break;
		case 'c':
			cellID = cell + (_width * 2 - 1);
		break;
		case 'd':
			cellID = cell + (_width - 1);
		break;
		case 'e':
			cellID = cell - 1;
		break;
		case 'f':
			cellID = cell - _width;
		break;
		case 'g':
			cellID = cell - (_width * 2 - 1);
		break;
		case 'h':
			cellID = cell - _width + 1;
		break;
	}
	return cellID;
}

pair<int, int> Map::valid_single_path (int pos, const string & path, int & step_nbr)
{
	char dir = path[0];
	int dir_cellID = Naia::cell_code_to_ID (path.substr (1));
	int last_pos = pos;
	for (step_nbr = 1 ; step_nbr <= 64 ; ++step_nbr)
	{
		if (get_cell_from_direction (last_pos, dir) == dir_cellID)
		{
			IObjectStorage::iterator it = _iobjects.find (dir_cellID);
			bool iowalkable = true;
			if (it != _iobjects.end())
				iowalkable = it->second->_template._walkable;
			if (get_cell (dir_cellID)._walkable && iowalkable)
				return make_pair (1, 0);
			else
			{
				--step_nbr;
				return make_pair (0, last_pos);
			}
		}
		else
			last_pos = get_cell_from_direction (last_pos, dir);
	}
	return make_pair (2, 0);
}

int Map::is_valid_path (int cell, string & path)
{
	int step_nbr, step = 0, new_pos = cell;
	string new_path;
	for (size_t i = 0 ; i < path.size() ; i += 3)
	{
		string small_path = path.substr (i, 3);
		char dir = small_path[0];
		int dir_cellID = Naia::cell_code_to_ID (small_path.substr (1));
		step_nbr = 0;
		pair<int, int> path_infos = valid_single_path (new_pos, small_path, step_nbr);
		switch (path_infos.first)
		{
			case 0:
				new_pos = path_infos.second;
				new_path += dir + Naia::cellID_to_code (new_pos);
				step += step_nbr;
				path = new_path;
				return -step;
			break;
			case 1:
				new_pos = dir_cellID;
				step += step_nbr;
			break;
			default:
				path = new_path;
				return -1000;
			break;
		}
		new_path += dir + Naia::cellID_to_code (new_pos);
	}
	path = new_path;
	return step;
}

/* *****************
 * UNITS FUNCTIONS *
 * ****************/

void Map::add_unit (Unit * unit)
{
	_units[unit->ID()] = unit;
	Packet data (SMSG_GAME_MOVEMENT);
	data << "|+" << unit->pack();

	((Character *)unit)->is_invisible() ? world.send_to_map (_ID, data, true) : world.send_to_map (_ID, data);
}

void Map::remove_unit (int ID)
{
	if (!world.is_running())
		return;

	UnitStorage::iterator it = _units.find (ID);
	if (it == _units.end())
		return;

	Packet data (SMSG_GAME_MOVEMENT);
	data << "|-" << it->second->pack();
	world.send_to_map (_ID, data);
	_units.erase (it);
}
