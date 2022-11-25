#include "Naia.hpp"

initialize_singleton (GameServer);

map<SSMessage, boost::function<void(string)> > gTasks;

GameServer::GameServer () :
	_ID (0),
	_state (SERVER_OFFLINE),
	_hexid ("default"),
	_io_service (kernel.threadpool.io_service()),
	_socket (new tcp::socket (*_io_service))

{
	gTasks[SSMSG_ON_DISCONNECT] 	= &ServerTaskHandler::on_disconnect;
	gTasks[SSMSG_ON_UPDATE_INFO] 	= &ServerTaskHandler::on_update;
	gTasks[SSMSG_ON_KICK_PLAYER] 	= &ServerTaskHandler::on_kick_player;

	_header[0] = _header[1] = 0;
}

GameServer::~GameServer ()
{
	if (_state != SERVER_OFFLINE && _socket->is_open())
		send (SSMSG_ON_DISCONNECT);
}

bool GameServer::connect ()
{
	kernel.log.print (TBLUE, true,
		"Attempting to connect to Login Server on %s:%d\n",
		kernel.config.get_property<string> (LOGIN_HOST_PARAM, LOGIN_HOST).c_str(),
		kernel.config.get_property<int> (LOGIN_PORT_PARAM, LOGIN_PORT)
	);

	if (_hexid == "default")
	{
		string file_name = PATH_CONFIG + string ("hexid.txt");
		ifstream file (file_name.c_str());
		if (!file)
		{
			kernel.log.print_error ("Failed to read authentification key ... Connection aborted.\n");
			return false;
		}
		else
			file >> _hexid;
	}

	system::error_code ec;
	_socket->connect (tcp::endpoint (address::from_string (kernel.config.get_property<string> (LOGIN_HOST_PARAM, LOGIN_HOST)), kernel.config.get_property<int> (LOGIN_PORT_PARAM, LOGIN_PORT)), ec);
	if (ec)
	{
		_socket->close();
		handle_error (ec);
		return false;
	}
	kernel.log.print (TGREEN, true, "Connection etablished !\n\n");

	_state = SERVER_STARTING;

	if (!authentificate())
		return false;

	kernel.log.print (TGREEN, true, "Authentification complete !\n");

	run();
	return true;
}

bool GameServer::authentificate ()
{
	gameserver.send (SSMSG_ON_CONNECT, gameserver.hexid());

	read (*_socket, asio::buffer (_header)); // On attend le header
	_packet.resize (_header[1]);

	read (*_socket, asio::buffer (_packet)); // On lis le contenu
	string content;
	size_t size = _packet.size();
	content.resize (size);
	memcpy (&content[0], &_packet[0], size);

	kernel.log.print_debug ("[LS] Received: <%u %s> [%d]\n", _header[0], content.c_str(), _header[1]);
	int ID = Naia::from_string<int> (content);
	if (ID == 0)
	{
		kernel.log.print_error ("Authentification denied ...\n");
		return false;
	}
	_ID = ID;
	return true;
}

void GameServer::update_status () const
{
	Packet status;
	status 	<< kernel.config.get_property<string> 	(USER_HOST_PARAM, USER_HOST) << ";"
			<< kernel.config.get_property<int> 		(USER_PORT_PARAM, USER_PORT) << ";"
			<< kernel.config.get_property<bool> 	(MAINTENANCE_PARAM, MAINTENANCE) << ";"
			<< kernel.config.get_property<int> 		(POPULATION_LIMIT_PARAM, POPULATION_LIMIT) << ";"
			<< boolalpha << kernel.config.get_property<bool> (IGNORE_POP_LIMIT_PARAM, IGNORE_POP_LIMIT) << ";" ;
	gameserver.send (SSMSG_ON_UPDATE_INFO, status.to_string());
}

void GameServer::run ()
{
	async_read (*_socket, asio::buffer (_header), bind (&GameServer::read_header, this, boost::asio::placeholders::error));
}

void GameServer::read_header (const system::error_code & e)
{
	if (e)
	{
		handle_error (e);
		return;
	}
	_packet.resize (_header[1]);

	async_read (*_socket, asio::buffer (_packet), bind (&GameServer::read_data, this, boost::asio::placeholders::error));
}

void GameServer::read_data (const system::error_code & e)
{
	if (e)
	{
		handle_error (e);
		return;
	}
	string content;
	size_t size = _packet.size();
	content.resize (size);
	memcpy (&content[0], &_packet[0], size);

	kernel.log.print_debug ("[LS] Received: <%u %s> [%d]\n", _header[0], content.c_str(), _header[1]);
	SSMessage msg = static_cast<SSMessage> (_header[0]);
	if (msg == SSMSG_BASIC_NOTHING)
	{
		run();
		return;
	}
	map<SSMessage, boost::function<void(string)> >::iterator it = gTasks.find (msg);
	if (it == gTasks.end())
	{
		send (SMSG_BASIC_NOTHING);
		run();
		return;
	}
	kernel.threadpool.schedule (bind (it->second, content));
	run();
}

void GameServer::handle_error (const system::error_code & e)
{
	if (!kernel.threadpool.running()) return;
	switch (e.value())
	{
		case 1236:
			// On coupe proprement le GS connecté
		break;
		case 10054: // Connexion perdue avec le LS
			if (_state != SERVER_OFFLINE)
			{
				kernel.log.print_error ("Connexion lost with the Login Server ! Trying to reconnect in 10 seconds...\n");
				_socket->close();
				kernel.threadpool.schedule_general (bind (&GameServer::connect, this), posix_time::seconds (10));
			}
		break;
		case 10061: // Connexion impossible avec le LS
			if (_state != SERVER_OFFLINE)
			{
				kernel.log.print_error ("Attempt fail ... Trying to reconnect in a few seconds ...\n");
				kernel.threadpool.schedule_general (bind (&GameServer::connect, this), posix_time::seconds (10));
			}
			else
				kernel.log.print_error ("Failed to connect to the Login Server :\n%s\n", e.message().c_str());
		break;
		default:
			kernel.log.print_error ("Error %d when etablishing connection with Login Server :\n%s\n", e.value(), e.message().c_str());
			_state = SERVER_OFFLINE;
	}
}

void GameServer::send (uint16_t header, const string & content)
{
	uint16_t size = static_cast<uint16_t> (content.size()); // juste pour eviter les warnings du compilo le static_cast
	vector<uint8_t> rawData (4 + size);
	memcpy (&rawData[0], &header, 2); // on ecrit le header sur deux octets
	memcpy (&rawData[2], &size, 2); // on ecrit ensuite la taille sur deux octets
	memcpy (&rawData[4], &content[0], size); // et on ecrit ensuite le contenu du packet
	system::error_code ec;
	write (*_socket, asio::buffer (rawData.data(), 4 + size), ec); // la methode data() permet d'acceder au pointeur interne du tableau (donc pas besoin de copie)
	if (ec)
	{
		kernel.log.print_error ("[%u] %s\n", ec.value(), ec.message().c_str());
	}
	kernel.log.print_debug ("[LS] Sent: <%u %s> [%u]\n", header, content.c_str(), content.size());
}

void ServerTaskHandler::on_update (string)
{
	gameserver.update_status();
}

void ServerTaskHandler::on_disconnect (string)
{
	gameserver.set_state (SERVER_OFFLINE);
	kernel.log.print_header (TRED, true, "[INFO]", "Login Server requests for shuting down !\n");
	Application::instance().exit (APP_NORMAL);
}

void ServerTaskHandler::on_kick_player (string packet)
{
	vector<string> table = Naia::split (packet, ';');
	int ID = Naia::from_string<int> (table[0]);
	Session * s = world.get_session (ID);
	if (s == NULL)
		return;
	Packet error ("M018|le serveur; Une deuxieme connexion a ete tentee sur ce compte depuis l'ip ");
	error << table[1] << ".";
	s->send (error);
	world.disconnect (s);
}
