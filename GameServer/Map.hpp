#ifndef MAP_HPP_INCLUDED
#define MAP_HPP_INCLUDED

class Map
{
	public:
	// TYPEDEFS
		typedef unordered_map <int, Unit *> UnitStorage;
		typedef unordered_map <int, InteractiveObject *> IObjectStorage;
		typedef vector <Cell> CellStorage;

	// INTIALIZATION
		Map (int ID, const string & date, int w, int h, int x, int y, int subarea, const string & key, const string & data, const string & mobs, int max_mobs, int max_grps, int capacity);
		void load ();
		void parse_map_data (const string & data);

	// PATHFINDING
		int get_cell_from_direction (int cell, char direction);
		pair<int, int> valid_single_path (int pos, const string & path, int & step_nbr);
		int is_valid_path (int cell, string & path);

	// UNITS
		void add_unit (Unit * unit);
		void remove_unit (int ID);

		Unit * get_unit (int id)
		{
			unordered_map<int, Unit *>::iterator it = _units.find (id);
			return it == _units.end() ? NULL : it->second;
		}

	// GETTERS
		bool is_init () const 	{ return _init; }

		int ID () const 		{ return _ID; }
		int width () const 		{ return _width; }
		int height () const 	{ return _height; }
		int subarea () const 	{ return _subarea; }
		int coord_x () const 	{ return _coordonates.first; }
		int coord_y () const 	{ return _coordonates.second; }

		string & key ()  		{ return _key; }
		string & date () 	 	{ return _date; }

		unordered_map<int, Unit *> & units () { return _units; }

		Cell get_cell (int id)
		{
			try
			{
				return _cells.at (id);
			}
			catch (std::exception)
			{
				Cell cell;
				cell._ID = UNDEFINED_CELLID;
				return cell;
			}
		}

	// MISCELLANEOUS

	private:
		bool _init;
		int _ID, _width, _height, _subarea, _capacity, _max_groups, _max_monsters;
		string _date, _key, _monsters, _data;
		pair<int, int> _coordonates;
		int _ioID;

		vector<Cell> _cells;
		unordered_map<int, Unit *> _units;
		unordered_map<int, InteractiveObject *> _iobjects;
		/* TODO : completer Map
		 * Items
		 * Creature_group
		 * Action ?
		 * IObjetc;
		 */
};

#endif // MAP_HPP_INCLUDED
