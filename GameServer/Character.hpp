#ifndef CHARACTER_HPP_INCLUDED
#define CHARACTER_HPP_INCLUDED

class Character;
class Map;
struct Cell;

struct CharacterInfo
{
	CharacterInfo (const string & name_, int ID_, int accountID_, int breed_, int color1_, int color2_, int color3_, int sex_, int gfx_, int level_, int align_,
					bool dead_, bool merchant_, Character * online_character_ = NULL) :
		name (name_),
		ID (ID_),
		accountID (accountID_),
		breed (breed_),
		color1 (color1_),
		color2 (color2_),
		color3 (color3_),
		sex (sex_),
		gfx (gfx_),
		level (level_),
		align (align_),
		dead (dead_),
		merchant (merchant_),
		update (clock()),
		online_character (online_character_) {}
	string pack ();

	string name;
	int ID,
		accountID,
		breed,
		color1,
		color2,
		color3,
		sex,
		gfx,
		level,
		align;
	bool dead, merchant;
	time_t update;
	Character * online_character;
};

class Character : public Unit, public Level
{
	public:
	// INITIALIZATION
		Character (unsigned ID, int accountID, int access_level, const string & name, unsigned breed, int sex, int color1, int color2, int color3, unsigned level, uint64_t xp,
					int gfx, int size, Map * map, int cell, int align_side, bool wings_deployed, int aligne_level, int honor, int disgrace, int title,
					bool dead, bool merchant, int last_seating, pair<int, int> save_pos, const string & zaaps, const string & emotes, int dammages, Session * session);

	// EVENTS
		void on_level_up (int count);
		void on_level_down (int count);
		string pack ();
		void save ();

	// GETTERS
		const int & accountID () const 		{ return _accountID; }
		const int & access_level () const 	{ return _access_level; }
		const int & breed () const 			{ return _breed; }
		const int & align_side () const 	{ return _align_side; }
		const int & honor () const 			{ return _honor; }
		const int & disgrace () const 		{ return _disgrace; }
		const int & mount_xp () const 		{ return _mount_xp; }
		const int & title () const 			{ return _title; }

		const bool & is_riding () const 	{ return _riding; }
		const bool & is_busy () const 		{ return _busy; }
		const bool & is_dead () const 		{ return _dead; }
		const bool & is_sitting () const 	{ return _sitting; }
		const bool & is_merchant () const 	{ return _merchant; }
		const bool & is_paralyzed () const 	{ return _paralyzed; }
		const bool & has_wings_deployed () const { return _wings_deployed; }
		const bool & is_away () const 		{ return _away; }
		const bool & is_quiet () const 		{ return _quiet; }
		const bool & is_invisible () const 	{ return _invisible; }

		const string & name () const		{ return _name; }

		const pair<int, int> & save_pos () const {return _save_pos; }

		Session * session () 	{ return _session; }

		const list<int> & zaaps () const { return _zaaps; }

	// SETTERS
		void set_away (bool b) 			{ _away = b; }
		void set_busy (bool b) 			{ _busy = b; }
		void set_invisible (bool b) 	{ _invisible = b; }
		void set_quiet (bool b) 		{ _quiet = b; }
		void set_sitting (bool b)		{ _sitting = b; }

		void set_title (unsigned ID) 	{ _title = ID; }

	private:
		string _name;

		bool _busy,
			_dead,
			_riding,
			_sitting,
			_merchant,
			_wings_deployed,
			_paralyzed,
			_away,
			_quiet,
			_invisible;

		int _breed,
			_accountID,
			_access_level,
			_title,
			_align_side,
			_align_level,
			_honor,
			_disgrace,
			_trading,
			_talking,
			_spouse,
			_mount_xp,
			_last_seating;

		set <int> _emotes;
		list<int> _zaaps;
		pair<int, int> _save_pos;
		Session * _session;
};

#endif // CHARACTER_HPP_INCLUDED
