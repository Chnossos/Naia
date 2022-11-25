#include "Naia.hpp"

void TaskHandler::handle_game_creation (Session * s, string packet)
{
	Packet data (SMSG_GAME_CREATE_SUCCESS);

	data << "|" << gameserver.ID() << "|" << s->character()->name();
	s->send (data);

	Packet stats (SMSG_ACCOUNT_STATS);
	// TODO format stats
	s->send (stats);

	Packet pods (SMSG_OBJECT_WEIGHT);
	pods << 0 /* TODO: pods utilisés */ << "|" << 1000 * kernel.config.get_property<int> (WEIGHT_MULTIPLIER_PARAM, WEIGHT_MULTIPLIER) /* pods max */ ;
	s->send (pods);

	Map * m = s->character()->map();

	Packet map (SMSG_GAME_MAP_DATA);
	map << "|" << m->ID() << "|" << m->date() << "|" << m->key();
	s->send (map);

	if (s->character()->access_level() > 0 && kernel.config.get_property<bool> (GM_STARTUP_INVISIBLE_PARAM, GM_STARTUP_INVISIBLE))
		s->character()->set_invisible (true);

	m->add_unit (s->character());

	if (s->character()->is_sitting()) // devrait toujours être debout
		s->character()->set_sitting (false);
}

void TaskHandler::handle_game_info_map (Session * s, string packet)
{
	Packet movements (SMSG_GAME_MOVEMENT);
	Map::UnitStorage & units = s->character()->map()->units();
	for (Map::UnitStorage::iterator it = units.begin() ; it != units.end() ; ++it)
		movements << "|+" << it->second->pack();
	s->send (movements);

	Packet creatures (SMSG_GAME_MOVEMENT);
	// TODO: charger les mobs
	s->send (creatures);

	Packet io (SMSG_GAME_MAP_FRAMEOBJECT2);
	// TODO: charger les objets intéractifs
	s->send (io);

	Packet mapitems (SMSG_GAME_MAP_CELLOBJECT);
	mapitems << "+"; // TODO: charger les objets au sol
	s->send (mapitems);

	// TODO: charger les enclos

	s->send (Packet (SMSG_GAME_MAP_SUCCESS));
}

void TaskHandler::handle_game_create_action (Session * s, string packet)
{
	if (s->character()->is_paralyzed())
		return;

	int type = Naia::from_string<int> (packet.substr (2, 3));
	int next = 1;
	Session::ActionStorage & actions = s->actions();

	if (!actions.empty())
	{
		for (Session::ActionStorage::iterator it = actions.begin() ; it != actions.end() ; ++it)
			if (it->second->ID > next)
				next = it->second->ID;
		++next;
	}

	Session::action_ptr g (new GameAction);
	g->type = type;
	g->ID = next;
	g->packet = packet;

	Character * character = s->character();

	switch (type)
	{
		case 1: // Déplacement
		{
			string path = packet.substr (5);
			// TODO: si on est trop lourd + diet
			Packet GA (SMSG_GAME_ACTION);
			int result = character->map()->is_valid_path (character->cell(), path);
			if (result == 0)
			{
				GA << ";" << 0 << ";;";
				s->send (GA);
				return;
			}
			if (result != -1000 && result < 0)
				result = -result;
			if (result == -1000)
				path = Naia::HASH[character->orientation()] + Naia::cellID_to_code (character->cell());
			g->args = path;

			GA << g->ID << ";" << 1 << ";" << character->ID() << ";" << "a" + Naia::cellID_to_code (character->cell()) + path/*, character->map()*/;

			character->is_invisible()
			? world.send_to_map (character->map()->ID(), GA, true)
			: world.send_to_map (character->map()->ID(), GA);

			actions[next] = g;
			if (character->is_sitting())
				character->set_sitting (false);
			character->set_busy (true);
		}
		break;
		case 500: // Game action
		{
			s->send (Packet (SMSG_BASIC_NOTHING));
			return;
			string game_action = packet.substr (5);
			vector<string> table = Naia::split (game_action, ';');
			int cellID = Naia::from_string<int> (table[0]),
				action = Naia::from_string<int> (table[1]);

			if (character->map()->get_cell(cellID)._ID == UNDEFINED_CELLID)
				return;

			g->args = Naia::to_string<int> (cellID) + ";" + Naia::to_string<int> (action);

			// TODO: can_perform_action pour les IO_Object

			actions[next] = g;
			//character->map()->start_action (g, s);
		}
		break;
	}
}

void TaskHandler::handle_game_finish_action (Session * s, string packet)
{
	vector<string> table = Naia::split (packet.substr (3), '|');
	int ID = Naia::from_string<int> (table[0]);
	Character & character = *s->character();
	Session::ActionStorage & actions = s->actions();

	Session::ActionStorage::iterator it = actions.find (ID);
	if (it == actions.end())
		return;
	Session::action_ptr GA = it->second;
	bool is_ok = packet[2] =='K';
	switch (GA->type)
	{
		case 1: // déplacement
		{
			if (is_ok)
			{
				Cell next_cell 	= character.map()->get_cell (Naia::cell_code_to_ID (GA->args.substr (GA->args.size() - 2)));
				Cell target 	= character.map()->get_cell (Naia::cell_code_to_ID (GA->packet.substr (GA->packet.size() - 2)));
				character.set_orientation (Naia::hash_to_int (GA->args[GA->args.size() - 3]));
				if (next_cell._ID == UNDEFINED_CELLID)
					break;
				character.set_cell (next_cell._ID);
				character.set_busy (false);
				s->send (Packet (SMSG_BASIC_NOTHING));
				// TODO : ramasser l'objet sur la case
			}
			else
			{
				Cell next_cell = character.map()->get_cell (Naia::from_string<int> (table[1]));
				if (next_cell._ID == UNDEFINED_CELLID)
					return;
				character.set_cell (next_cell._ID);
				character.set_orientation (Naia::hash_to_int (GA->args[GA->args.size() - 3]));
				s->send (Packet (SMSG_BASIC_NOTHING));
			}
		}
		break;
		case 500: // game Action
			// TODO : character.map().finish_action (GA, s);
		break;
	}
	actions.erase (it);
}
