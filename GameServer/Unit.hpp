#ifndef UNIT_HPP_INCLUDED
#define UNIT_HPP_INCLUDED

enum UnitType
{
	UNIT_CHARACTER,
	UNIT_NPC,
	UNIT_TAX_COLLECTOR,
};

class Map;

class Unit
{
	public:
		Unit (int type, int ID, int color1, int color2, int color3, int sex, int gfx, int size, int orientation, Map * map, int cell) :
			_ID (ID),
			_color1 (color1),
			_color2 (color2),
			_color3 (color3),
			_sex (sex),
			_type (type),
			_gfx (gfx),
			_size (size),
			_orientation (orientation),
			_cell (cell),
			_map (map) {}

		virtual string pack () = 0;

	// GETTERS
		const int & ID () const 			{ return _ID; }
		const int & color1 () const 		{ return _color1; }
		const int & color2 () const 		{ return _color2; }
		const int & color3 () const 		{ return _color3; }
		const int & sex () const 			{ return _sex; }
		const int & type () const 			{ return _type; }
		const int & gfx () const 			{ return _gfx; }
		const int & size () const 			{ return _size; }
		const int & orientation () const 	{ return _orientation; }
		const int & cell () const 			{ return _cell; }
		Map * map () 				{ return _map; }

	// SETTERS
		void set_gfx (int gfx) 		{ _gfx = gfx; }
		void set_size (int size) 	{ _size = size; }
		void set_orientation(int i) { _orientation = i; }
		void set_cell (int cell) 	{ _cell = cell; }
		void set_map (Map * map) 	{ _map = map; }

	protected:
		int _ID;
		int _color1, _color2, _color3;
		int _sex, _type;
		int _gfx, _size, _orientation;
		int _cell;
		Map * _map;
};

#endif // UNIT_HPP_INCLUDED
