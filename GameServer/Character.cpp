#include "Naia.hpp"

Character::Character (unsigned ID, int accountID, int access_level, const string & name, unsigned breed, int sex, int color1, int color2, int color3, unsigned level, uint64_t xp,
					int gfx, int size, Map * map, int cell, int align_side, bool wings_deployed, int align_level, int honor, int disgrace, int title,
					bool dead, bool merchant, int last_seating, pair<int, int> save_pos, const string & zaaps, const string & emotes, int dammages, Session * session) :
	Unit (XP_CHARACTER, ID, color1, color2, color3, sex, gfx, size, 1, map, cell),
	Level (XP_CHARACTER, level, xp),
	_name (name),
	_busy (false),
	_dead (dead),
	_riding (false),
	_sitting (false),
	_merchant (merchant),
	_wings_deployed (false),
	_paralyzed (false),
	_away (false),
	_quiet (false),
	_invisible (false),
	_breed (breed),
	_accountID (accountID),
	_access_level (access_level),
	_title (title),
	_align_side (align_side),
	_align_level (align_level),
	_honor (honor),
	_disgrace (disgrace),
	_last_seating (last_seating),
	_save_pos (save_pos),
	_session (session)
{
	if (!zaaps.empty())
	{
		vector<string> table = Naia::split (zaaps, ',');
		for (size_t i = 0 ; i < table.size() ; ++i)
			_zaaps.push_back (Naia::from_string<int> (table[i]));
	}

	if (!emotes.empty())
	{
		vector<string> table = Naia::split (emotes, ',');
		for (size_t i = 0 ; i < table.size() ; ++i)
			_emotes.insert (Naia::from_string<int> (table[i]));
	}

	// TODO Stat, Guild, job, Mount
}

/* ******************
 * EVENTS FUNCTIONS *
 * *****************/

void Character::on_level_up (int count)
{

}

void Character::on_level_down (int count)
{

}

string Character::pack ()
{

	ostringstream data;
	data	<< _cell << ";" << _orientation << ";" << 0 << ";" << _ID << ";" << _name << ";" << _breed
			<< (_title > 0 ? "," + Naia::to_string<int> (_title) : "") << ";" << _gfx << "^" << _size << ";"
			<< _sex << ";" << _align_side << "," << 0 << "," << (_wings_deployed ? 1 : 0) << "," << _ID << ";"
			<< (_color1 == -1 ? "-1" : Naia::to_hex_string<int> (_color1)) << ";" << (_color2 == -1 ? "-1" : Naia::to_hex_string<int> (_color2)) << ";"
			<< (_color3 == -1 ? "-1" : Naia::to_hex_string<int> (_color3)) << ";" << ",,,," << ";" // TODO: ",,,," ==> parse_accessories
			<< (_level > 99 ? (_level > 199 ? 2 : 1) : 0) << ";;;" // TODO: emote + emote_timer
			<< ";;" // TODO guild
			<< 0 << ";" << ";;";

	return data.str();
}

void Character::save ()
{
	string zaaps;
	for (list<int>::iterator it = _zaaps.begin() ; it != _zaaps.end() ; ++it)
	{
		if (it != _zaaps.begin())
			zaaps += ",";
		zaaps += Naia::to_string<int> (*it);
	}

	// TODO : spells + emotes

	ostringstream query;
	query 	<< "UPDATE characters SET "
			<< "Access_Level='" << _access_level << "', "
			<< "Name='" 		<< _name 	<< "', "
			<< "Sex='" 			<< _sex 	<< "', "
			<< "color1='" 		<< _color1 	<< "', "
			<< "color2='" 		<< _color2 	<< "', "
			<< "color3='" 		<< _color3 	<< "', "
			<< "Size='" 		<< _size 	<< "', "
			<< "GFX='" 			<< _gfx 	<< "', "
			<< "XP='" 			<< _xp 		<< "', "
			/*<< "Kamas='" 		<< 0 		<< "', "
			<< "Energy='" 		<< 10000 	<< "', "
			<< "Capital='" 		<< 0 		<< "', "
			<< "SpellBoosts='" 	<< 0 		<< "', "
			<< "Damages='" 		<< 0 		<< "', "*/
			<< "Map='" 			<< _map->ID() << "', "
			<< "Cell='" 		<< _cell 	<< "', "
			<< "Saved_pos='" 	<< _save_pos.first << ";" << _save_pos.second << "', "
			<< "Zaaps='" 		<< zaaps 	<< "' "
			<< "WHERE ID='" 	<< _ID 		<< "' LIMIT 1;";

	gsDatabase->execute (query.str().c_str());
	lsDatabase->execute ("UPDATE accounts SET Active_Channels='%s' WHERE ID='%u';", _session->active_channels().c_str(), _session->ID());
}

/* **************************
 * CHARACTER_INFO FUNCTIONS *
 * *************************/

string CharacterInfo::pack ()
{
	return ",,,,";
}
