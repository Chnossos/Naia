#include "Naia.hpp"

map <CMessage, pair<Session::Task, RequiredData> > tasks;

Session::Session (tcp::socket * socket) :
	_socket (socket),
	_timer (*(kernel.threadpool.io_service()), posix_time::minutes (USER_TIMEOUT)),
	_packet_count (0),
	_ID (0),
	_access_level (0),
	_next_recruit_date (0),
	_next_trade_date (0),
	_mute_end_date (0),
	_character (NULL)
{
	_timer.async_wait (bind (&Session::handle_timer, this, boost::asio::placeholders::error));
}

Session::~Session ()
{
	lsDatabase->execute ("UPDATE accounts SET IP_Loc='', Is_Online='0' WHERE ID='%u';", _ID);
	if (_character != NULL)
	{
		if (world.is_running())
		{
			_character->map()->remove_unit (_character->ID());
			world.update_character_info (_character, false);
		}
		_character->save();
		delete _character;
	}
	// TODO: sauvegarder les amis
	_socket->close();
}

void Session::load_tasks ()
{
	tasks[CMSG_ACCOUNT_TICKET].first 			= &TaskHandler::handle_account_ticket;
	tasks[CMSG_ACCOUNT_REGIONAL_VERSION].first 	= &TaskHandler::handle_account_regional_version;
	tasks[CMSG_ACCOUNT_CHARS_LIST].first 		= &TaskHandler::handle_account_characters_list;
	tasks[CMSG_ACCOUNT_WAIT_QUEUE].first 		= &TaskHandler::handle_account_wait_queue;
	tasks[CMSG_ACCOUNT_GIFTS_LIST].first 		= &TaskHandler::do_nothing;
	tasks[CMSG_ACCOUNT_IDENTITY].first 			= &TaskHandler::do_nothing;
	tasks[CMSG_ACCOUNT_KEY].first 				= &TaskHandler::do_nothing;
	tasks[CMSG_ACCOUNT_ADD_CHAR].first 			= &TaskHandler::handle_account_add_character;
	tasks[CMSG_ACCOUNT_DEL_CHAR].first			= &TaskHandler::handle_account_del_character;
	tasks[CMSG_ACCOUNT_RANDOM_NAME].first		= &TaskHandler::handle_account_generate_name;
	tasks[CMSG_ACCOUNT_SELECT_CHAR].first 		= &TaskHandler::handle_account_select_char;
	tasks[CMSG_ACCOUNT_MIGRATION].first 		= &TaskHandler::handle_account_confirm_migration;

	tasks[CMSG_BASIC_DATETIME].first 		= &TaskHandler::handle_basic_datetime;
	tasks[CMSG_BASIC_MESSAGE].first 		= &TaskHandler::handle_basic_message;
	tasks[CMSG_BASIC_STATE].first 			= &TaskHandler::handle_basic_state;

	tasks[CMSG_GAME_CREATE].first 			= &TaskHandler::handle_game_creation;
	tasks[CMSG_GAME_INFOMAP].first 			= &TaskHandler::handle_game_info_map;
	tasks[CMSG_GAME_ACTION].first 			= &TaskHandler::handle_game_create_action;
	tasks[CMSG_GAME_FINISH_ACTION].first 	= &TaskHandler::handle_game_finish_action;

	tasks[CMSG_BASIC_ADMIN].first 			= &TaskHandler::handle_basic_command;
}

void Session::handle_timer (const system::error_code & e)
{
	if (!e) // Le temps est écoulé
	{
		send (Packet ("M01"), false); // Message serveur 1 : inactivité
		world.disconnect (this);
	}

	else if (e != boost::asio::error::operation_aborted)
		kernel.log.print_error ("[S:%u] Uptime timer could not be launched: %s\n", _ID, e.message().c_str());
}

void Session::handle_error (const system::error_code & e)
{
	switch (e.value())
	{
		case 2:
			// EOF ==> Survient si déco normale par l'user
		break;
		case 1236:
			// Double connexion sur un même compte & timer écoulé
			// Opération entrée/sortie annulée ...
		case 995:
			// The I/O operation has been aborted because of either thread exit or an application request.
		return;

		default:
			kernel.log.print_error ("[S:%u](%u) %s", _ID, e.value(), e.message().c_str());
	}
	world.disconnect (this);
}

void Session::run ()
{
	if (!world.is_running())
	{
		send (Packet ("M018|le serveur; Le demarrage est en cours : connexion impossible."), false);
		delete this;
		return;
	}
	else if (!_packet_count)
	{
		send (Packet (SMSG_HELLO_GAME));
		++_packet_count;
	}
	async_read_until (*_socket, _buffer, '\n', bind (&Session::parse_packet, this, boost::asio::placeholders::error));
}

void Session::parse_packet (const system::error_code & e)
{
	if (e)
	{
		handle_error (e);
		return;
	}
	istream is (&_buffer);
	string packet;
	getline (is, packet);

	if (packet.empty())
	{
		run();
		return;
	}
	if (packet[0] == '\0')
		packet = packet.substr (1);

	kernel.log.print_debug ("[S:%u] Received <%s> [%u]\n", _ID, packet.c_str(), packet.size());

	if (packet == "ping")
	{
		send (Packet ("pong"));
		return;
	}
	else if (packet == "qping")
	{
		send (Packet ("qpong"));
		return;
	}

	if (_timer.expires_from_now (posix_time::minutes (USER_TIMEOUT)) > 0)
		_timer.async_wait (bind (&Session::handle_timer, this, boost::asio::placeholders::error));
	else
	{
		send (Packet ("M018|le serveur; Probleme de timer interne."), false);
		world.disconnect (this);
		return;
	}
	map<CMessage, pair<Session::Task, RequiredData> >::iterator it;
	for (int i = 2 ; i < 4 ; ++i)
	{
		string dest = Naia::string_to_hex (packet.substr (0, i));
		int result  = Naia::from_string<int> (dest, true);
		it = tasks.find (static_cast<CMessage> (result));
		if (it == tasks.end())
		{
			if (i == 3)
			{
				send (Packet (SMSG_BASIC_NOTHING));
				run();
				return;
			}
			continue;
		}
		break;
	}

	bool can_send_this_packet = false;
	switch (it->second.second)
	{
		case NONE:
			can_send_this_packet = true;
		break;

		case HAVE_ACCOUNT:
			if (_ID)
				can_send_this_packet = true;
		break;

		case HAVE_CHARACTER:
			if (_character != NULL)
				can_send_this_packet = true;
		break;

		/*case HAVE_LIST:
			if (_queue_pos == NULL)
				can_send_this_packet = true;
		break;*/

		default:
			can_send_this_packet = true;
	}
	if (!can_send_this_packet)
	{
		send (Packet ("M07"), false); // "Cette opération n'est pas autorisée ici."
		world.disconnect (this);
		return;
	}
	kernel.threadpool.schedule (boost::bind (it->second.first, this, packet));
	run();
}

/* *************
 * PROCEEDINGS *
 **************/

time_t Session::is_subscriber () const
{
	time_t subscribe = _subscribe_end_date - time (NULL);
	return subscribe <= 60 ? 0 : subscribe;
}

time_t Session::is_muted () const
{
	if (_mute_end_date == -1) return 1;
	time_t duration = _mute_end_date - time (NULL);
	return duration <= 0 ? 0 : duration;
}

time_t Session::next_trade_date () const
{
	time_t timeout = _next_trade_date - time (NULL);
	return timeout > 0 ? timeout : 0;
}

time_t Session::next_recruit_date () const
{
	time_t timeout = _next_recruit_date - time (NULL);
	return timeout > 0 ? timeout : 0;
}
