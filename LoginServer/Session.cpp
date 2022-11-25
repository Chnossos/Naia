#include "Naia.hpp"

typedef void (*session_task) (Session *, string);
map<CMessage, pair<session_task, RequiredData> > sTasks;

Session::Session (tcp::socket * socket) :
	_ID (0),
	_access_level (0),
	_queue_pos (NULL),
	_waiting_for_nickname (false),
	_socket (socket),
	_packet_count (0),
	_timer (*(kernel.threadpool.io_service()), posix_time::minutes (USER_TIMEOUT))
{
	_timer.async_wait (bind (&Session::handle_timer, this, boost::asio::placeholders::error));
}

Session::~Session ()
{
	_socket->close();
	delete _queue_pos;
}

void Session::init_task_handlers ()
{
	sTasks[CMSG_ACCOUNT_WAIT_QUEUE].first 		= &TaskHandler::handle_account_queue;
	sTasks[CMSG_ACCOUNT_WAIT_QUEUE].second		= HAVE_ACCOUNT;

	sTasks[CMSG_ACCOUNT_GAME_SERVER].first		= &TaskHandler::handle_account_gameservers;
	sTasks[CMSG_ACCOUNT_GAME_SERVER].second		= HAVE_LIST;

	sTasks[CMSG_ACCOUNT_SERVERS_LIST].first		= &TaskHandler::handle_account_characters;
	sTasks[CMSG_ACCOUNT_SERVERS_LIST].second	= HAVE_LIST;

	sTasks[CMSG_ACCOUNT_NICK_SEARCH].first 		= &TaskHandler::handle_nickname_search;
}

void Session::load_data ()
{
	LoginServer::GameServerMap & gs = login_server.gameservers();
	QueryResult * QR = NULL;

	for (LoginServer::GameServerMap::iterator it = gs.begin() ; it != gs.end() ; ++it)
	{
		int character_count = 0;
		QR = sDatabase->query ("SELECT * FROM account_data WHERE AccountID='%u' AND ServerID='%u';", _ID, it->second->ID());
		if (QR)
		{
			character_count = QR->get_row_count();
			delete QR;
		}
		_characters_distribution += "|" + Naia::to_string (it->second->ID()) + "," + Naia::to_string (character_count);
	}
}

void Session::run ()
{
	if (_packet_count == 0)
	{
		++_packet_count;
		Packet packet (HELLO_CONNECTION);
		string alphabet = "abcdefghijklmnopqrstuvwxyz", hashkey;

		for (int i = 0 ; i < 32 ; ++i)
			hashkey += alphabet[kernel.random.random (alphabet.size() - 1)];

		packet << hashkey;
		_hash_key = hashkey;
		send (packet);
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
	getline (is, packet, '\n');

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
		run();
		return;
	}
	else if (packet == "qping")
	{
		send (Packet ("qpong"));
		run();
		return;
	}
	switch (_packet_count)
	{
		case 1:
			kernel.threadpool.schedule (bind (TaskHandler::handle_account_version, this, packet));
			++_packet_count;
			return;

		case 2:
			_name = packet;
			++_packet_count;
			run();
			return;

		case 3:
			kernel.threadpool.schedule (bind (TaskHandler::handle_account_password, this, packet));
			++_packet_count;
			return;
	}

	if (_timer.expires_from_now (posix_time::minutes (USER_TIMEOUT)) > 0)
		_timer.async_wait (bind (&Session::handle_timer, this, boost::asio::placeholders::error));
	else
	{
		send (Packet ("M018|le serveur; Probleme de timer interne."), false);
		login_server.disconnect (this);
		return;
	}

	string dest = Naia::string_to_hex (packet.substr (0, 2));
	int result  = Naia::from_string<int> (dest, true);
	map<CMessage, pair<session_task, RequiredData> >::iterator it = sTasks.find (static_cast<CMessage> (result));
	if (it == sTasks.end())
	{
		if (_waiting_for_nickname)
		{
			QueryResult * QR = sDatabase->query ("SELECT * FROM accounts WHERE Nickname='%s';", packet.c_str());
			if (QR)
			{
				delete QR;
				send (Packet ("AlEs"));
				run();
				return;
			}
			_waiting_for_nickname = false;
			_nickname = packet;
			Packet nickname (ACCOUNT_DOFUS_PSEUDO);
			nickname << _nickname;
			send (nickname);
			login_server.connect (this);
		}
		else
		{
			kernel.log.print_debug ("Packet %s introuvable !\n", packet.substr(0, 2).c_str());
			send (Packet (SMSG_BASIC_NOTHING));
			send (Packet ("M018|le serveur; Action inconnue du serveur."), false); // Message serveur 18 : kick
			login_server.disconnect (this);
		}
		return;
	}

	bool can_send_this_packet = false;
	switch (it->second.second)
	{
		case NONE:
			can_send_this_packet = true;
			break;

		case HAVE_ACCOUNT:
			if (_ID != 0)
				can_send_this_packet = true;
			break;

		case HAVE_LIST:
			if (_queue_pos == NULL)
				can_send_this_packet = true;
			break;

		default:
			can_send_this_packet = true;
	}
	if (!can_send_this_packet)
	{
		send (Packet ("M07"), false); // "Cette opération n'est pas autorisée ici."
		login_server.disconnect (this);
		return;
	}
	kernel.threadpool.schedule (bind (it->second.first, this, packet));
	run();
}

void Session::handle_error (const system::error_code & e)
{
	switch (e.value())
	{
		case 2:
			// EOF ==> Survient si déco normale par l'user
			break;

		case 995:
			// The I/O operation has been aborted because of either thread exit or an application request.
			return;

		case 1236:
			// Double connexion sur un même compte ou timer écoulé
			return;

		case 10054:
			// Une connexion existante a dû être fermée par l'hôte distant
			break;

		default:
			_ID == 0
			? kernel.log.print_error ("[%s](%u) %s\n", remote_ip().c_str(), e.value(), e.message().c_str())
			: kernel.log.print_error ("[S:%u](%u) %s\n", _ID, e.value(), e.message().c_str());
	}
	login_server.disconnect (this);
}

void Session::handle_timer (const system::error_code & e)
{
	if (!e) // Le temps est écoulé
	{
		send (Packet ("M01"), false); // Message serveur 1 : inactivité
		login_server.disconnect (this);
	}
	else if (e != boost::asio::error::operation_aborted)
		kernel.log.print_error ("[S:%u] Uptime timer could not be launched: %s\n", _ID, e.message().c_str());
}


time_t Session::is_subscriber () const
{
	time_t subscribe = _subscribe_end_date - time (NULL);
	return subscribe <= 60 ? 0 : subscribe;
}
