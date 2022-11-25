#include "Naia.hpp"

void TaskHandler::do_nothing (Session *, string) {}

void TaskHandler::handle_account_ticket (Session * s, string packet)
{
	QueryResult * QR = NULL;
	if (kernel.config.get_property<bool> (MULTI_ACCOUNTS_PARAM, MULTI_ACCOUNTS) == false)
	{
		QR = lsDatabase->query (
			"SELECT * from accounts WHERE IP_Loc='%s' AND Is_Online='%u';",
			s->socket()->local_endpoint().address().to_string().c_str(),
			gameserver.ID()
		);
		if (QR)
		{
			s->send (Packet ("M018|le serveur que vous tentiez de rejoindre car le multi-comptes y est interdit; "), false);
			delete s;
			delete QR;
			return;
		}
	}

	string ticket = packet.substr (2);
	QR = lsDatabase->query ("SELECT * FROM accounts WHERE Ticket='%s' LIMIT 1;", ticket.c_str());
	if (!QR)
	{
		s->send (Packet (SMSG_ACCOUNT_TICKET_ERROR));
		delete s;
		return;
	}
	Field * fields = QR->fetch();
	s->set_ID (fields[0].get_value<int>());
	s->set_name (fields[1].get_value<string>());
	s->set_nickname (fields[3].get_value<string>());
	s->set_answer (fields[5].get_value<string>());
	s->set_access_level (fields[6].get_value<int>());
	s->set_last_ip (fields[8].get_value<string>());
	s->set_subscribe_end_date (fields[11].get_value<time_t>());
	s->set_last_connection_date (fields[13].get_value<time_t>());
	s->set_active_channels (fields[14].get_value<string>());
	s->set_mute_end_date (fields[15].get_value<time_t>());

	delete QR;


	world.connect (s);
	Packet data (SMSG_ACCOUNT_TICKET_SUCCESS);
	data << 0;
	s->send (data);
}

void TaskHandler::handle_account_regional_version (Session * s, string packet)
{
	Packet data (SMSG_ACCOUNT_REGIONAL_VERSION);
	data << 0;
	s->send (data);
}

void TaskHandler::handle_account_characters_list (Session * s, string packet)
{
	// TODO: si a du crédit et des personnages sur d'autres serveurs
	Packet data;

	if (packet.size() > 2 && packet[2] == 'f' /* TODO: OU n'a pas de crédit ni d'autres personnages */)
		data << SMSG_ACCOUNT_LIST_SUCCESS;
	else
		data << "AM";

	const World::CharacterStorage & characters = world.get_characters (s->ID());

	time_t subscribe = s->is_subscriber();
	subscribe ? (data << subscribe << "000") : (data << "0");
	data << "|" << characters.size();

	for (World::CharacterStorage::const_iterator it = characters.begin() ; it != characters.end() ; ++it)
	{
		CharacterInfo * CI = it->second.get();
		data 	<< "|" << CI->ID << ";" << CI->name << ";" << CI->level << ";" << CI->gfx << ";"
				<< (CI->color1 == -1 ? "-1" : Naia::to_hex_string<int>(CI->color1)) << ";"
				<< (CI->color2 == -1 ? "-1" : Naia::to_hex_string<int>(CI->color2)) << ";"
				<< (CI->color3 == -1 ? "-1" : Naia::to_hex_string<int>(CI->color3)) << ";"
				<< CI->pack() << ";"
				<< (CI->merchant ? 1 : 0) << ";" << gameserver.ID() << ";" << (CI->dead ? 1 : 0) << ";" << "2;25"; // deathCount;lvlMax (pour heroique)
	}
	s->send (data);
}

void TaskHandler::handle_account_wait_queue (Session * s, string packet)
{
	int pos = 1;
	int aboSize = 0;
	int discSize = 0;

	Packet data(SMSG_ACCOUNT_WAIT_QUEUE);
	data << pos << "|" << aboSize << "|" << discSize << "|" << (s->is_subscriber() > 0 ? 1 : 0) << "|" << gameserver.ID();
	s->send (data);
}

void TaskHandler::handle_account_add_character (Session * s, string packet)
{
	Packet error (SMSG_ACCOUNT_ADD_CHAR_ERROR);
	Packet success (SMSG_ACCOUNT_ADD_CHAR_SUCCESS);
	vector<string> table 	= Naia::split (packet.substr(2), '|');
	string name 			= table[0];
	int breed 				= Naia::from_string<int>(table[1]);
	int sex 				= Naia::from_string<int> (table[2]);

	if (breed == 12 && !kernel.config.get_property<bool> (PANDAWA_FOR_ALL_PARAM, PANDAWA_FOR_ALL) && !s->is_subscriber())
	{
		error << "s";
		s->send (error);
		return;
	}
	if (!Naia::check_name (name))
	{
		error << "n";
		s->send (error);
		return;
	}
	if (world.get_character (name) != NULL)
	{
		error << "a";
		s->send (error);
		return;
	}

	QueryResult * QR = lsDatabase->query ("SELECT * FROM account_data WHERE AccountID='%u';", s->ID(), gameserver.ID());
	if (QR && QR->get_row_count() >= kernel.config.get_property<int> (MAX_CHARACTERS_COUNT_PARAM, MAX_CHARACTERS_COUNT))
	{
		error << "f";
		s->send (error);
		delete QR;
		return;
	}
	delete QR;

	pair<int, int> start_loc = Naia::get_start_location (breed);
	Map * map = world.get_map (start_loc.first);
	if (map == NULL || map->get_cell(start_loc.second)._ID == UNDEFINED_CELLID)
	{
		kernel.log.print_debug ("Map<%u>|Cell<%u> not found ...\n", start_loc.first, start_loc.second);
		s->send (error);
		return;
	}

	unordered_map<int, int> base_stats;
	bool full_scrolled = kernel.config.get_property<bool> (FULL_SCROLLED_PARAM, FULL_SCROLLED);
	if (full_scrolled)
	{
		base_stats[Naia::VITALITY] 		= 101;
		base_stats[Naia::STRENGTH] 		= 101;
		base_stats[Naia::AGILITY] 		= 101;
		base_stats[Naia::INTELLIGENCE] 	= 101;
		base_stats[Naia::CHANCE] 		= 101;
		base_stats[Naia::WISDOM] 		= 101;
	}

	string zaaps;
	QR = gsDatabase->query ("SELECT * from zaaps WHERE Known_At_Start='true';");
	if (QR)
	{
		do
		{
			Field * fields = QR->fetch();
			zaaps = zaaps + fields[0].get_value<string>() + ",";
		}
		while (QR->next_row());
	}

	unsigned start_level = kernel.config.get_property<int> (START_LEVEL_PARAM, START_LEVEL);

	Character * character = new Character (
		world.next_characterID(),
		s->ID(),
		0,
		name,
		breed,
		sex,
		Naia::from_string<int> (table[3]),
		Naia::from_string<int> (table[4]),
		Naia::from_string<int> (table[5]),
		start_level,
		LevelManager::instance().get_xp (XP_CHARACTER, start_level),
		breed * 10 + sex, 	// gfx
		100,				// size
		map,
		start_loc.second,	// cell
		0,					// align_side
		false, 				// wings_deployed
		0, 					// align_level
		0,					// honor
		0,					// disgrace
		0,					// title
		false,				// dead
		false,				// merchant
		time (NULL),		// last_seated
		start_loc,			// save_pos
		zaaps,
		"",					// emotes
		0,					// damages
		s
	);

	string save_pos = Naia::to_string<int> (start_loc.first) + string (";") + Naia::to_string<int> (start_loc.second);

	// TODO : Spells, Start Items
	ostringstream query;
	query 	<< "INSERT INTO characters VALUES ("
			<< "'" << character->ID() << "',"
			<< "'" << s->ID() << "',"
			<< "'" << 0 << "',"
			<< "'" << character->name() << "',"
			<< "'" << breed << "',"
			<< "'" << sex << "',"
			<< "'" << character->color1() << "',"
			<< "'" << character->color2() << "',"
			<< "'" << character->color3() << "',"
			<< "'" << 100 << "',"
			<< "'" << breed * 10 + sex << "',"
			<< "'" << character->xp() << "',"
			<< "'" << kernel.config.get_property<int> (START_KAMAS_PARAM, START_KAMAS) << "',"
			<< "'" << 10000 << "',"
			<< "'" << (character->level() - 1) * 5 << "',"
			<< "'" << (character->level() - 1) << "',"
			<< "'" << 0 << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << (full_scrolled ? 101 : 0) << "',"
			<< "'" << start_loc.first << "',"
			<< "'" << start_loc.second << "',"
			<< "'" << save_pos << "',"
			<< "'" << zaaps << "',"
			<< "'" << "'," 							// Spells
			<< "'" << 0 << "',"
			<< "'" << 0 << "',"
			<< "'" << "false" << "',"
			<< "'" << 0 << "',"
			<< "'" << 0 << "',"
			<< "'" << "',"								// Emotes
			<< "'" << "',"								// Jobs
			<< "'" << 0 << "',"
			<< "'" << "false" << "',"
			<< "'" << "false" << "',"
			<< "'" << "false" << "');";

	if (!gsDatabase->execute (query.str().c_str()))
	{
		delete character;
		error << "F";
		s->send (error);
		return;
	}

	ostringstream query2;
	query2	<< "INSERT INTO account_data VALUES ("
			<< "'" << s->ID() << "',"
			<< "'" << gameserver.ID() << "');";

	if (!lsDatabase->execute (query2.str().c_str()))
	{
		gsDatabase->execute ("DELETE FROM characters WHERE ID='%u' LIMIT 1;", character->ID());
		delete character;
		error << "F";
		s->send (error);
		return;
	}

	CharacterInfo * CI = new CharacterInfo (
		character->name(),
		character->ID(),
		character->accountID(),
		character->breed(),
		character->color1(),
		character->color2(),
		character->color3(),
		character->sex(),
		character->gfx(),
		character->level(),
		character->align_side(),
		character->is_dead(),
		character->is_merchant()
	);

	world.add_character (CI);
	s->send (success);
	delete character;

	handle_account_characters_list (s, "");

	s->send (Packet (SMSG_TUTORIAL_BEGIN));
}

void TaskHandler::handle_account_del_character (Session * s, string packet)
{
	Packet error (SMSG_ACCOUNT_DEL_CHAR_ERROR);

	if (packet[0] == '-')
	{

	}
	else
	{		vector<string> table = Naia::split (packet.substr (2), '|');
		int ID = Naia::from_string<int> (table[0]);
		World::CharacterStorage characters = world.get_characters (s->ID());
		World::CharacterStorage::iterator it = characters.find (ID);
		if (it == characters.end())
		{
			s->send (error);
			return;
		}
		if (it->second->level >= 20)
		{
			if (table.size() == 1)
			{
				s->send (error);
				return;
			}
			string response = table[1];
			if (Naia::lower_case (s->answer()) != Naia::lower_case (response))
			{
				s->send (error);
				return;
			}
		}
		if (!gsDatabase->execute ("DELETE FROM characters WHERE ID='%u';", ID))
		{
			s->send (error);
			return;
		}
		if (!lsDatabase->execute (
				"DELETE FROM account_data WHERE AccountID='%u' AND ServerID='%u' LIMIT 1;",
				s->ID(),
				gameserver.ID()
		))
		{
			s->send (error);
			return;
		}

		world.del_character (ID);
	}
	TaskHandler::handle_account_characters_list (s, "");
}

void TaskHandler::handle_account_generate_name (Session * s, string packet)
{
	Packet data (SMSG_ACCOUNT_GENERATED_NAME_SUCCESS);

	string 	name,
			chars = "bcdfghjklmnpqrstvwxz",
			vowels = "aeiouy";

	float random = (kernel.random.random (RAND_MAX) / RAND_MAX);
	int B = kernel.random.random (RAND_MAX) % (10 - 6) + 6;
	int vorc = (int)floor (random);

	for (int i = 0 ; i < B ; ++i)
	{
		if (i % 2 == vorc)
		{
			float rand3 = (kernel.random.random (RAND_MAX) / (double)RAND_MAX) * 6;
			int a = (int)floor (rand3);
			name += vowels.at (a);
		}
		else
		{
			float rand3 = (kernel.random.random (RAND_MAX) / (double)RAND_MAX) * 20;
			int a = (int)floor (rand3);
			name += chars.at (a);
		}

		if (!i)
			name = Naia::upper_case (name);
	}

	data << name;
	s->send (data);
}

void TaskHandler::handle_account_select_char (Session * s, string packet)
{
	Packet error (SMSG_ACCOUNT_SELECT_CHAR_ERROR);

	int ID = Naia::from_string<int> (packet.substr (2));
	World::CharacterStorage characters = world.get_characters (s->ID());

	World::CharacterStorage::iterator it = characters.find (ID);
	if (it == characters.end())
	{
		s->send (error);
		return;
	}
	CharacterInfo * character = it->second.get();

	QueryResult * QR = gsDatabase->query ("SELECT * FROM characters WHERE ID='%u' LIMIT 1;", ID);
	if (!QR)
	{
		s->send (error);
		return;
	}
	Field * fields = QR->fetch();

	unordered_map<int, int> stats;
	stats[Naia::VITALITY] 		= fields[17].get_value<int>();
	stats[Naia::WISDOM] 		= fields[18].get_value<int>();
	stats[Naia::STRENGTH] 		= fields[19].get_value<int>();
	stats[Naia::INTELLIGENCE]	= fields[20].get_value<int>();
	stats[Naia::CHANCE] 		= fields[21].get_value<int>();
	stats[Naia::AGILITY] 		= fields[22].get_value<int>();

	vector<string> table = Naia::split (fields[25].get_value<string>(), ';');

	Character * ch = new Character (
		fields[0].get_value<int>(),
		s->ID(),
		fields[2].get_value<int>(), 	// access_level
		fields[3].get_value<string>(),	// name
		fields[4].get_value<int>(),		// class
		fields[5].get_value<int>(), 	// sex
		fields[6].get_value<int>(), 	// color 1
		fields[7].get_value<int>(), 	// color 2
		fields[8].get_value<int>(), 	// color 3
		LevelManager::instance().get_level (XP_CHARACTER, fields[11].get_value<uint64_t>()),
		fields[11].get_value<uint64_t>(), // XP
		fields[10].get_value<int>(), 	// GFX
		fields[9].get_value<int>(), 	// size
		world.get_map (fields[23].get_value<int>()), 	// map
		fields[24].get_value<int>(), 	// cell
		fields[28].get_value<int>(),	// align_side
		fields[30].get_value<bool>(),	// wings_deployed
		fields[29].get_value<int>(),	// align_level
		fields[31].get_value<int>(),	// honor
		fields[32].get_value<int>(),	// disgrace
		fields[35].get_value<int>(),	// title
		fields[37].get_value<bool>(),	// dead
		fields[36].get_value<bool>(),	// merchant
		character->update,				// last_seated
		make_pair (Naia::from_string<int> (table[0]), Naia::from_string<int> (table[1])),
		fields[26].get_value<string>(),	// zaaps
		fields[33].get_value<string>(),	// name
		fields[16].get_value<int>(),	// damages
		s // Session
	);
	s->set_character (ch);
	// TODO: s->character()->load_items();
	world.update_character_info (s->character(), true);

	Packet data (SMSG_ACCOUNT_SELECT_CHAR_SUCCESS);
	data 	<< "|" << ch->ID() << "|" << ch->name() << "|" << ch->level() << "|"
			<< ch->breed() << "|" << ch->sex() << "|" << ch->gfx() << "|"
			<< (ch->color1() == -1 ? "-1" : Naia::to_hex_string<int>(ch->color1())) << "|"
			<< (ch->color2() == -1 ? "-1" : Naia::to_hex_string<int>(ch->color2())) << "|"
			<< (ch->color3() == -1 ? "-1" : Naia::to_hex_string<int>(ch->color3())) << "|";
			// TODO : ADD ITEMS

	s->send (data);

	// TODO : Drago

	// Envoie des canaux de chat
	Packet channels (SMSG_CHAT_CHANNELS);
	channels << "+" << s->active_channels() << (s->character()->access_level() > 0 ? "^@" : "^");
	s->send (channels);

	Packet see_friends (SMSG_FRIEND_ONLINE);
	see_friends << (fields[38].get_value<bool>() ? "+" : "-");
	s->send (see_friends);

	Packet spells (SMSG_SPELL_LIST);
	s->send (spells);

	Packet alignment (SMSG_SPECIALIZATION_SET);
	alignment << ch->align_side();
	s->send (alignment);

	// Envoie des emotes
	Packet emotes (SMSG_EMOTE_LIST);
	s->send (emotes);

	// ????
	Packet restrictions (SMSG_ACCOUNT_RESTRICTIONS);
	restrictions << "6bk";
	s->send (restrictions);

	// Message "Bienvenue sur DOFUS [...]"
	Packet welcome (SMSG_INFO_MESSAGE);
	welcome << "189";
	s->send (welcome);

	// TODO : GUILD

	if (s->last_connection_date() != 0 && !s->last_ip().empty())
	{
		struct tm * tm = localtime (&s->last_connection_date());
		string 	day 		= Naia::to_string<int> (tm->tm_mday),
				month 		= Naia::to_string<int> (tm->tm_mon + 1),
				year 		= Naia::to_string<int> (tm->tm_year + 1900),
				hour 		= Naia::to_string<int> (tm->tm_hour),
				minute 		= Naia::to_string<int> (tm->tm_min),
				second 		= Naia::to_string<int> (tm->tm_sec);

		if (day.size() == 1)
			day = "0" + day;

		if (month.size() == 1)
			month = "0" + month;

		if (hour.size() == 1)
			hour = "0" + hour;

		if (minute.size() == 1)
			minute = "0" + minute;

		string time = year + "~" + month + "~" + day + "~" + hour + "~" + minute;

		Packet Im (SMSG_INFO_MESSAGE);
		Im << "0152;" << time << "~" << s->last_ip();
		s->send (Im);
	}

	Packet Im (SMSG_INFO_MESSAGE);
	Im << "0153;" << s->remote_ip();
	s->send (Im);

	Packet Im2 (SMSG_INFO_MESSAGE);
	Im2 << "1225;";
	s->send (Im2, false);

	vector<string> announcements = world.chat.annoucements();
	for (size_t i = 0 ; i < announcements.size() ; ++i)
	{
		Packet announcement (SMSG_CHAT_SERVER_MESSAGE);;
		announcement << announcements[i];
		s->send (announcement);
	}

	lsDatabase->execute ("UPDATE accounts SET IP_Net='%s', Last_Connection_Date='%u' WHERE ID='%u';", s->remote_ip().c_str(), time (NULL), s->ID());
	s->set_last_connection_date (time (NULL));
}

void TaskHandler::handle_account_confirm_migration (Session * s, string packet)
{
	if (packet[2] == '-')
	{
		handle_account_del_character (s, packet.substr (2));
		return;
	}
	vector<string> table = Naia::split (packet, ';');
	int character_ID = Naia::from_string<int> (table[0]);
	string new_name  = table[1];
}

