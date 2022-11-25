#include "Naia.hpp"

typedef void (*server_task) (GameServer *, string);
map<SSMessage, server_task> gTasks;

GameServer::GameServer (tcp::socket * socket) :
	_ID (0),
	_state (SERVER_STARTING),
	_population (0),
	_population_limit (500),
	_min_access_level (0),
	_socket (socket)
{

}

GameServer::GameServer (int ID, const string & hexid) :
	_ID (ID),
	_state (SERVER_OFFLINE),
	_population (0),
	_population_limit (500),
	_min_access_level (0),
	_hexid (hexid)
{

}

void GameServer::init_task_handlers ()
{
	gTasks[SSMSG_ON_CONNECT] 		= &TaskHandler::handle_connection;
	gTasks[SSMSG_ON_DISCONNECT] 	= &TaskHandler::handle_disconnection;
	gTasks[SSMSG_ON_UPDATE_INFO] 	= &TaskHandler::handle_update_info;
	gTasks[SSMSG_ON_UPDATE_POP]		= &TaskHandler::handle_update_pop;
}

GameServer::~GameServer ()
{
}

GameServer * GameServer::operator = (const GameServer & gs)
{
	_ID = gs.ID();
	_hexid = gs.hexid();
	return this;
}

void GameServer::handle_error (const system::error_code & e)
{
	switch (e.value())
	{
		case 2:
			// EOF ==> Survient si déco normale par l'user
		break;
		case 1236:
			// Double connexion sur un même compte
		return;
		case 10054: // Crash après connexion d'un GS
			boost::this_thread::sleep (posix_time::milliseconds (50));
			if (_socket->is_open())
			{
				if (_state != SERVER_OFFLINE) // Si le serveur était en marche ou démarrait
					kernel.log.print_error ("Game Server [%u] has encountered problems during its execution and crashed.\n", _ID);
				sDatabase->execute ("UPDATE accounts SET Ticket='', IP_Loc='', Is_Online='0' WHERE Is_Online='%u';", _ID);
			}
		break;
		default:
			kernel.log.print_error ("[GS:%u](%u) %s\n", _ID, e.value(), e.message().c_str());
	}
	login_server.disconnect (this);
}

void GameServer::run ()
{
	_packet.clear();

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
	memcpy(&content[0], &_packet[0], size);

	kernel.log.print_debug ("[GS:%u] Received <%s %s> [%d]\n", _ID, Naia::to_string<int>(_header[0]).c_str(), content.c_str(), _header[1]);
	SSMessage msg_header = static_cast<SSMessage> (_header[0]);
	if (msg_header == SSMSG_BASIC_NOTHING)
	{
		run();
		return;
	}
	map<SSMessage, server_task>::iterator it = gTasks.find (msg_header);
	if (it == gTasks.end())
	{
		send (SSMSG_BASIC_NOTHING);
		run();
		return;
	}
	kernel.threadpool.schedule (bind (it->second, this, content));
	run();
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
		kernel.log.print_error ("[GS:%u](%u) %s\n", _ID, ec.value(), ec.message().c_str());
	}
	kernel.log.print_debug ("[GS:%u] Sent : <%s %s> [%u]\n", _ID, Naia::to_string<int>(header).c_str(), content.c_str(), content.size());
}
