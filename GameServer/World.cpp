#include "Naia.hpp"

initialize_singleton (World);

World::World () :
	_is_running (false),
	highest_charID (0)
{

}

World::~World ()
{
	_is_running = false;

	if (!_sessions.empty())
	{
		for (SessionsStorage::iterator it = _sessions.begin() ; it != _sessions.end() ; ++it)
		{
			it->second->send (Packet (SMSG_SERVER_WILL_DISCONNECT), false);
			Application::instance().state() == APP_RESTART
			? it->second->send (Packet ("M018|le serveur lors de son redemarrage;"), false)
			: it->second->send (Packet ("M018|le serveur lors de son extinction;"), false);
			_sessions.erase (it);
		}
		kernel.log.print_header (TBLUE, true, "[Server]", "\tAll users have been kicked successfully.\n");
	}
}

/* *******************
 * LOADING FUNCTIONS *
 * ******************/

bool World::load ()
{
	Session::load_tasks();

	if (!LevelManager::instance().load())
		return false;

	if (!load_maps())
		return false;

	load_characters();

	return _is_running = true;
}

bool World::load_maps ()
{
	time_t start = clock();
	QueryResult * QR = gsDatabase->query ("SELECT * FROM maps;");
	if (!QR)
	{
		kernel.log.print_error ("There is no map to load ...\n");
		return false;
	}
	do
	{
		Field * fields = QR->fetch();
		int id = fields[0].get_value<int>();
		_maps[id] = boost::shared_ptr<Map> (new Map (
			id,
			fields[8].get_value<string>(),
			fields[3].get_value<int>(),
			fields[4].get_value<int>(),
			fields[1].get_value<int>(),
			fields[2].get_value<int>(),
			fields[5].get_value<int>(),
			fields[7].get_value<string>(),
			fields[6].get_value<string>(),
			fields[9].get_value<string>(),
			fields[10].get_value<int>(),
			fields[11].get_value<int>(),
			fields[12].get_value<int>()
		));
	}
	while (QR->next_row());
	kernel.log.print_header (TWHITE, false, "[World]", "Loaded %u maps successfully in %ums.\n", _maps.size(), clock() - start);
	delete QR;
	return true;
}

void World::load_characters ()
{
	QueryResult * QR = gsDatabase->query ("SELECT * FROM characters;");
	if (!QR)
		return;
	do
	{
		Field * fields = QR->fetch();
		unsigned id = fields[0].get_value<unsigned>();
		_characters[id] = boost::shared_ptr<CharacterInfo> (new CharacterInfo (
			fields[3].get_value<string>(),
			id,
			fields[1].get_value<int>(), // accountID
			fields[4].get_value<int>(), // breed
			fields[6].get_value<int>(), // color1
			fields[7].get_value<int>(), // color2
			fields[8].get_value<int>(), // color3
			fields[5].get_value<int>(), // sex
			fields[10].get_value<int>(), // gfx
			LevelManager::instance().get_level (XP_CHARACTER, fields[11].get_value<uint64_t>()), // LEVEL
			fields[28].get_value<int>(), // align_side
			fields[37].get_value<bool>(), // dead
			fields[36].get_value<bool>() // merchant
		));
		if (id > highest_charID)
			highest_charID = id;
	}
	while (QR->next_row());
	delete QR;
	kernel.log.print_header (TWHITE, false, "[World]", "Loaded %u character(s).\n", _characters.size());
}

/* *************************
 * COMMUNICATION FUNCTIONS *
 * ************************/

void World::send_to_map (unsigned ID, const Packet & packet, unsigned bypass_sender)
{
	Map::UnitStorage & _units = _maps[ID]->units();
	for (Map::UnitStorage::iterator it = _units.begin() ; it != _units.end() ; ++it)
	{
		if (it->second->type() == UNIT_CHARACTER)
		{
			Character * ch = (Character *)it->second;

			if (bypass_sender && ch->ID() == bypass_sender)
				continue;

			ch->session()->send (packet);
		}
	}
}

void World::send_to_map (unsigned ID, const Packet & packet, bool GM_only)
{
	Map::UnitStorage & _units = _maps[ID]->units();
	for (Map::UnitStorage::iterator it = _units.begin() ; it != _units.end() ; ++it)
	{
		if (it->second->type() == UNIT_CHARACTER)
		{
			Character * ch = (Character *)it->second;

			if (GM_only && ch->access_level() < 1)
				continue;

			ch->session()->send (packet);
		}
	}
}

void World::send_to_all (const Packet & packet)
{
	for (SessionsStorage::iterator it = _sessions.begin() ; it != _sessions.end() ; ++it)
	{
		if (it->second->character() != NULL)
			it->second->send (packet);
	}
}

/* **********************
 * CHARACTERS FUNCTIONS *
 * *********************/

void World::add_character (CharacterInfo * c)
{
	_characters[c->ID] = boost::shared_ptr<CharacterInfo> (c);
}

void World::del_character (int ID)
{
	_characters.erase (ID);
}

void World::update_character_info (Character * character, bool is_online)
{
	CharacterInfo * CI = get_character (character->name());
	if (CI == NULL)
		return;

	CI->ID 			= character->ID();
	CI->accountID 	= character->accountID();
	CI->color1 		= character->color1();
	CI->color2 		= character->color2();
	CI->color3 		= character->color3();
	CI->breed 		= character->breed();
	CI->sex 		= character->sex();
	CI->name 		= character->name();
	CI->level 		= character->level();
	CI->gfx 		= character->gfx();
	CI->dead 		= character->is_dead();
	CI->merchant 	= character->is_merchant();

	if (!is_online)
	{
		CI->online_character = NULL;
		CI->update = clock();
	}
	else
		CI->online_character = character;
}

CharacterInfo * World::get_character (const string & name)
{
	string lower_name = Naia::lower_case (name);
	for (CharacterStorage::iterator it = _characters.begin() ; it != _characters.end() ; ++it)
	{
		if (Naia::lower_case (it->second->name) == lower_name)
			return it->second.get();
	}
	return NULL;
}

World::CharacterStorage World::get_characters (int accountID)
{
	CharacterStorage characters;
	for (CharacterStorage::iterator it = _characters.begin() ; it != _characters.end() ; ++it)
	{
		if (it->second->accountID == accountID)
			characters[it->first] = it->second;
	}
	return characters;
}

unsigned World::next_characterID ()
{
	return ++highest_charID;
}

/* ****************
 * MAPS FUNCTIONS *
 *****************/

void World::add_map (Map * map)
{
	_maps[map->ID()] = boost::shared_ptr<Map> (map);
}

Map * World::get_map (int ID)
{
	MapStorage::iterator it = _maps.find (ID);
	return it != _maps.end() ? it->second.get() : NULL;
}

/* *******************
 * SESSION FUNCTIONS *
 * ******************/

void World::connect (Session * s)
{
	ostringstream oss;
	oss << "["
		<< s->name() << "] ["
		<< s->access_level() << "] ["
		<< s->remote_ip() << " : "
		<< s->socket()->remote_endpoint().port() << "] ["
		<< s->local_ip() << " : "
		<< s->socket()->local_endpoint().port() << "]" << endl;

	kernel.log.log (AUDIT_CONNECTION, Naia::to_string<int> (s->ID()), "[%u]: %s\n", gameserver.ID(), oss.str().c_str());
	kernel.log.print_debug ("New user: %s", oss.str().c_str());

	_sessions[s->ID()] = boost::shared_ptr<Session> (s);
	gsDatabase->execute ("UPDATE accounts SET Ticket='', IP_Loc='%s', Is_Online='%u' WHERE ID='%u';",
						s->local_ip().c_str(),
						gameserver.ID(),
						s->ID());
}

void World::disconnect (Session * s)
{
	if (!s || !_is_running) return;

	int id = s->ID();
	if (id == 0 && s != NULL)
	{
		delete s;
		return;
	}
	SessionsStorage::iterator it = _sessions.find (id);
	if (it == _sessions.end() && s != NULL) // Si la session n'existe pas
	{
		delete s;
		return;
	}
	_sessions.erase (it);
	kernel.log.print_debug ("User [%s] disconnected.\n", s->name().c_str());
	gameserver.send (SSMSG_ON_UPDATE_POP, Naia::to_string<unsigned> (_sessions.size()));
}

Session * World::get_session (int ID)
{
	SessionsStorage::iterator it = _sessions.find (ID);
	return it != _sessions.end() ? it->second.get() : NULL;
}
