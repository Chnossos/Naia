#include "Naia.hpp"

initialize_singleton (LoginServer);

LoginServer::LoginServer ()
{
	GameServer::init_task_handlers();
	Session::init_task_handlers();
}

LoginServer::~LoginServer ()
{
	_running = false;

	if (!_sessions.empty())
	{
		for (SessionMap::iterator it = _sessions.begin() ; it != _sessions.end() ; ++it)
		{
			sDatabase->execute ("UPDATE accounts SET IP_Loc='' WHERE ID='%u';", it->second->ID());
			it->second->send (Packet ("M018|le serveur lors de son extinction;"), false);
			boost::this_thread::sleep (posix_time::milliseconds (25));
			_sessions.erase(it);
		}
		kernel.log.print_header (TBLUE, true, "[Server]", "\tAll users have been kicked successfully.\n");
	}

	bool at_least_one = false;
	for (GameServerMap::iterator git = _gameservers.begin() ; git != _gameservers.end() ; ++git)
	{
		if (git->second->state() == SERVER_ONLINE)
		{
			at_least_one = true;
			git->second->send (SSMSG_ON_DISCONNECT);
			boost::this_thread::sleep (posix_time::milliseconds (75));
		}
		_gameservers.erase(git);
	}
	if (at_least_one)
		kernel.log.print_header (TBLUE, true, "[Server]", "\tAll Game Servers have been shut down successfully.\n");
}

bool LoginServer::load_servers ()
{
	kernel.log.print (TBLUE, true, "Loading registered Game Server(s) ...");
	QueryResult * QR = sDatabase->query ("SELECT * FROM gameservers;");
	if (QR)
	{
		ProgressBar * bar = new ProgressBar (QR->get_row_count());
		do
		{
			Field * fields = QR->fetch();

			int id = fields[0].get_value<int>();
			_gameservers[id] = boost::shared_ptr<GameServer> (new GameServer (id, fields[1].get_value<string>()));

			bar->step();
		}
		while (QR->next_row());

		delete bar;
		delete QR;
		_gameservers.size() == 1
		? kernel.log.print (TGREEN, true, "Loaded 1 Game Server !\n")
		: kernel.log.print (TGREEN, true, "Loaded %u Game Servers !\n", _gameservers.size());
		return _running = true;
	}
	return false;
}

void LoginServer::refresh_server_list ()
{
	Packet packet ("AH");
	build_server_list (packet);
	for (SessionMap::iterator it = _sessions.begin() ; it != _sessions.end() ; ++it)
		it->second->send (packet);
}

void LoginServer::build_server_list (Packet & packet)
{
	for (GameServerMap::iterator it = _gameservers.begin() ; it != _gameservers.end() ; ++it)
	{
		if (packet.to_string() != "AH")
			packet << "|";
		packet << it->second->ID() << ";" << it->second->state() << ";110;1";
	}
}

void LoginServer::connect (Session * s)
{
	Packet success (ACCOUNT_AUTH_SUCCESS);
	success << (s->access_level() > 0 ? 1 : 0);
	s->send (success);

	if (kernel.config.get_property<bool> (MULTI_ACCOUNTS_PARAM, MULTI_ACCOUNTS) == false && s->access_level() == 0)
	{
		QueryResult * QR = sDatabase->query ("SELECT * from accounts WHERE IP_Loc='%s';", s->local_ip().c_str());
		if (QR)
		{
			s->send (Packet ("M018|le serveur de connexion car le multi-comptes est globalement interdit;"), false);
			delete s;
			delete QR;
			return;
		}
	}

	ostringstream oss, oss2;
	oss << "["
		<< s->name() << "]["
		<< s->access_level() << "]["
		<< s->remote_ip() << ":"
		<< s->socket()->remote_endpoint().port() << "]["
		<< s->local_ip() << ":"
		<< s->socket()->local_endpoint().port() << "]" << endl;
	oss2 << s->ID();
	kernel.log.log (AUDIT_CONNECTION, oss2.str(), "L: %s", oss.str().c_str());

	sDatabase->execute ("UPDATE accounts SET Nickname='%s', IP_Loc='%s' WHERE ID='%u';",
						s->nickname().c_str(),
						s->local_ip().c_str(),
						s->ID());

	kernel.log.print_debug ("New user: %s", oss.str().c_str());
	_sessions[s->ID()] = boost::shared_ptr<Session> (s);

	s->load_data();
	s->run();
}

void LoginServer::connect (GameServer * gs)
{
	int ID = gs->ID();
	GameServerMap::iterator it = _gameservers.find (ID);
	if (it == _gameservers.end() && gs != NULL)
	{
		gs->send (SSMSG_HELLO_CONNECTION, Naia::to_string<int> (0));
		delete gs;
		return;
	}
	gs->send (SSMSG_HELLO_CONNECTION, Naia::to_string<int> (ID));
	_gameservers[ID].reset (gs);

	kernel.log.print_debug ("Game Server [ID: %u][%s:%u] successfully connected.\n",
		ID,
		gs->remote_ip().c_str(),
		gs->port()
	);
	refresh_server_list();
}

void LoginServer::disconnect (Session * s)
{
	if (s == NULL || !_running) return;
	int id = s->ID();
	if (id == 0)
	{
		delete s;
		return;
	}
	SessionMap::iterator it = _sessions.find (id);
	if (it == _sessions.end()) // Si la session n'existe pas
	{
		delete s;
		return;
	}
	s->ticket().empty()
	? sDatabase->execute ("UPDATE accounts SET IP_Loc='', Last_Connection_Date='%u' WHERE ID='%u';", time (NULL), id)
	: sDatabase->execute ("UPDATE accounts SET IP_Loc='' WHERE ID='%u';", id);
	_sessions.erase (it);
	kernel.log.print_debug ("User [%s] disconnected.\n", s->name().c_str());
}

void LoginServer::disconnect (GameServer * gs)
{
	if (gs == NULL || !_running) return;
	int ID = gs->ID();
	if (ID == 0)
	{
		delete gs;
		return;
	}
	GameServerMap::iterator it = _gameservers.find (ID);
	if (it == _gameservers.end()) // Si le server n'est pas enregistré
	{
		delete gs;
		return;
	}
	if (it->second->socket()->is_open())
		it->second->socket()->close();

	it->second->set_state (SERVER_OFFLINE);
	refresh_server_list();
	kernel.log.print_debug ("GameServer [ID: %u] disconnected.\n", ID);
}

Session * LoginServer::get_session_by_id (int id)
{
	SessionMap::iterator it = _sessions.find (id);
	return it != _sessions.end() ? it->second.get() : NULL;
}

GameServer * LoginServer::get_server_by_id (int id)
{
	GameServerMap::iterator it = _gameservers.find (id);
	return it != _gameservers.end() ? it->second.get() : NULL;
}
