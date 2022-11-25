#include "Naia.hpp"

void TaskHandler::packet_format_queue (Packet & packet, Session * s)
{
}

/* ******************
 * SERVER FUNCTIONS *
 * *****************/

void TaskHandler::handle_connection (GameServer * gs, string packet) // <HC auth_key>
{
	LoginServer::GameServerMap GSM = login_server.gameservers();
	for (LoginServer::GameServerMap::iterator it = GSM.begin() ; it != GSM.end() ; ++it)
	{
		if (packet == it->second->hexid())
		{
			/*gs->set_ID (it->second->ID());
			gs->set_hexid (packet);*/
			*gs = *(it->second);
			login_server.connect (gs);
			return;
		}
	}
	gs->send (SSMSG_HELLO_CONNECTION, Naia::to_string<int> (0));
}

void TaskHandler::handle_disconnection (GameServer * gs, string packet) // <Di>
{
	gs->set_state (SERVER_OFFLINE);
}

void TaskHandler::handle_update_info (GameServer * gs, string packet) // <UI MIN_ACCESS_LVL;POPULATION;MAX_POPULATION>
{
	vector<string> table = Naia::split (packet, ';');

	gs->set_connection_ip (table[0]);
	gs->set_connection_port (from_string<int> (table[1]));
	gs->set_min_access_level (from_string<int> (table[2]));
	gs->set_population_limit (from_string<int> (table[3]));
	gs->set_ignore_pop_limit (from_string<bool>(table[4]));
	gs->set_state (SERVER_ONLINE);
	login_server.refresh_server_list();
}

void TaskHandler::handle_update_pop (GameServer * gs, string packet)
{
	gs->set_population (from_string<int> (packet));
}

/* ******************
 * SESSION FUNTIONS *
 * *****************/

void TaskHandler::handle_account_version (Session * s, string packet)
{
	string version = kernel.config.get_property<string> (CLIENT_VERSION_PARAM, CLIENT_VERSION);
	if (!kernel.config.get_property<bool> (IGNORE_VERSION_PARAM, IGNORE_VERSION) && packet != version)
	{
		Packet data (REQUIRED_VERSION);
		data << version;
		s->send (data);
		delete s;
	}
	else
		s->run();
}

void TaskHandler::handle_account_password (Session * s, string packet)
{
	Packet error (ACCOUNT_LOGIN_ERROR);

	if (packet.substr (0, 2) != "#1")
	{
		error << "f";
		s->send (error);
		delete s;
		return;
	}

	QueryResult * QR = sDatabase->query ("SELECT * FROM banned_ip WHERE IP='%s' LIMIT 1;", s->remote_ip().c_str());
	if (QR)
	{
		Field * fields = QR->fetch();

		if (fields[1].get_value<int>() >= time (NULL) || fields[1].get_value<int>() == 0)
		{
			error << "b";  // Compte incorrect (banni)
			s->send (error);
			delete s;
			return;
		}
		else
			sDatabase->query ("DELETE FROM banned_ip WHERE IP='%s';", s->remote_ip().c_str());

		delete QR;
	}

	string base_pass 	= Naia::decrypt_pass (packet, s->hash_key());
	md5_byte_t * digest = kernel.md5.hash (base_pass);
	string md5_pass 	= kernel.md5.hash_to_string (digest);

	int is_connected = 0;
	QR = sDatabase->query ("SELECT * FROM accounts WHERE Name='%s' AND Password='%s' LIMIT 1;", s->name().c_str(), md5_pass.c_str());
	if (QR)
	{
		Field * fields = QR->fetch();

		s->set_ID 					(fields[0].get_value<int>());
		s->set_nickname 			(fields[3].get_value<string>());
		s->set_question 			(fields[4].get_value<string>());
		s->set_access_level 		(fields[6].get_value<int>());
		s->set_subscribe_end_date 	(fields[11].get_value<time_t>());
		is_connected 				= fields[12].get_value<int>();

		delete QR;
	}
	else
	{
		QR = sDatabase->query ("SELECT * FROM accounts WHERE Name='%s';", s->name().c_str());
		if (!QR) // Si ce n'est pas une erreur de password
		{
			QueryResult * QR2 = sDatabase->query ("SELECT * FROM accounts;");
			s->set_ID (QR2 ? (QR2->get_row_count() + 1) : 1);
			s->set_question ("DELETE?");

			ostringstream query;
			query 	<< "INSERT INTO accounts VALUE ("
					<< "'" << s->ID() << "', "
					<< "'" << s->name() << "', "
					<< "'" << md5_pass << "', "
					<< "'" << s->name() << "', "
					<< "'" << s->question() << "', "
					<< "'" << "DELETE" << "', "
					<< "'" << s->access_level() << "', "
					<< "'', '', '', '', " // Ticket, IP_NET, IP_LOC, email,
					<< "'" << 0 << "', " // Subscribe time
					<< "'" << 0 << "', " // is online
					<< "'" << 0 << "', " // last connection date
					<< "'" << "i*#$p%!?:^" << "', "
					<< "'" << 0 << "');"; // mute_time
			sDatabase->execute (query.str().c_str());

			error << "r";
			s->send (error);
			s->wait_for_nickname();
			delete QR2;
			return;
		}
		error << "f"; // nom de compte ou mot de passe incorrect
		s->send (error);
		delete QR;
		delete s;
		return;
	}

	if (s->access_level() < 0)
	{
		if (s->access_level() == -1)
		{
			error << "b";  // Compte incorrect (banni)
			s->send (error);
			delete s;
			return;
		}
		time_t duration = abs (s->access_level()) - time (NULL);
		if (duration > 0)
		{
			error << "k"; // banni pendant X j, Y h, W min
			time_t 	min 	= duration / 60,
					hours 	= duration / (60 * 60),
					days 	= duration / (24 * 60 * 60);
			error << days << "|" << hours % 24 << "|" << min % 60;
			s->send (error);
			delete s;
			return;
		}
		sDatabase->execute ("UPDATE accounts SET Access_Level='0' WHERE ID='%u';", s->ID());
	}
	Session * other = login_server.get_session_by_id (s->ID());
	if (other) // Déjà quelqu'un de connecté sur le sevreur de connexion avec le compte
	{
		if (s->local_ip() == other->local_ip()) // Est-ce un idiot ?
		{
			error << "a";
			s->send (error);
		}
		else
		{
			// On dégage l'intrus
			Packet error2 ("M018|le serveur; Une deuxieme connexion a ete tentee sur ce compte depuis l'ip ");
			error2 << s->remote_ip() << ".";
			other->send (error2, false);

			login_server.disconnect (other);

			error << "d"; // Vous venez de déconnecter un personnage utilisant déjà ce compte
			s->send (error);
		}
		delete s;
		return;
	}
	if (is_connected) // Si le compte est sur un serveur de jeu
	{
		GameServer * GS = login_server.get_server_by_id (is_connected);
		if (GS)
		{
			Packet disconnect;
			disconnect << s->ID() << ";" << s->remote_ip();
			GS->send (SSMSG_ON_KICK_PLAYER, disconnect.to_string()); // On demande la déco de l'user distant

			error << "d"; // Vous venez de déconnecter un personnage utilisant déjà ce compte
			s->send (error);
			delete s;
			return;
		}
	}
	login_server.connect (s);
}

void TaskHandler::handle_account_queue (Session * s, string packet)
{
	if(s->queue_pos() == NULL)
	{
		if (!s->nickname().empty())
		{
			Packet data1 (ACCOUNT_DOFUS_PSEUDO);
			data1 << s->nickname();
			s->send (data1);
		}

		Packet data2 (ACCOUNT_COMMUNITY);
		data2 << 0; // fr
		s->send (data2);

		Packet data3 (ACCOUNT_QUESTION); // question
		string question = s->question();
		boost::algorithm::replace_all (question, " ", "+");
		data3 << question;
		s->send (data3);

		Packet data4 (ACCOUNT_HOSTS); // hosts
		login_server.build_server_list (data4);
		s->send (data4);
	}
	else
	{
		Packet data (ACCOUNT_WAITQUEUE);
		TaskHandler::packet_format_queue (data, s);
		s->send (data);
	}
}

void TaskHandler::handle_account_characters (Session * s, string packet)
{
	Packet data (ACCOUNT_CHARACTERS);
	time_t subscribe = s->is_subscriber();

	subscribe ? (data << subscribe << "000") : (data << 0);
	data << s->characters_distribution();

	s->send (data);
}

void TaskHandler::handle_account_gameservers (Session * s, string packet)
{
	GameServer * GS = login_server.get_server_by_id (Naia::from_string<int> (packet.substr (2)));
	if (!GS)
	{
		s->send (Packet (SMSG_BASIC_NOTHING));
		return;
	}

	Packet error (ACCOUNT_SERVER_ERROR);
	if (GS->state() == SERVER_OFFLINE)
	{
		error << "d"; // Serveur down
		s->send (error);
		return;
	}
	if (GS->state() == SERVER_STARTING || s->access_level() < GS->min_access_level())
	{
		error << "r"; // Serveur innaccessible (save/gmOnly)
		s->send (error);
		return;
	}
	// Si le serveur est plein, que les accès sont insuffisants, qu'il n'est pas abonné
	if (GS->population() >= GS->population_limit()
		&& s->access_level() < 1
		&& (s->is_subscriber() == 0 || !kernel.config.get_property<bool> (RETAIL_SUBSCRIPTION_POLICY_PARAM, RETAIL_SUBSCRIPTION_POLICY)))
	{
		// TODO: if GS->gameplay() == SERVER_HARDCORE
		error << (GS->ID() == 22 ? "F" : "f"); // Serveur plein
		int a = 0;
		LoginServer::GameServerMap & gs = login_server.gameservers();
		for (LoginServer::GameServerMap::iterator it = gs.begin() ; it != gs.end() ; ++it)
		{
			if (it->second->state() == SERVER_ONLINE && it->second->population() < it->second->population_limit())
				a++ ? (error << it->second->ID()) : (error << "|" << it->second->ID());
		}
		s->send (error);
		return;
	}
	Packet data (ACCOUNT_SERVER_SUCCESS);
	data << (GS->remote_ip() == "127.0.0.1" && s->remote_ip() == GS->connection_ip() ? "127.0.0.1" : GS->connection_ip()) << ":" << GS->connection_port() << ";";

	string ticket = Naia::create_ticket();
	if(!sDatabase->execute ("UPDATE accounts SET Ticket='%s' WHERE ID='%u';", ticket.c_str(), s->ID())) // sauvegarde du ticket, avec renvoi d'exception
	{
		kernel.log.print_error ("[S:%u] Echec de la sauvegarde du ticket !\n", s->ID());
		s->send (Packet (SMSG_BASIC_NOTHING));
		return;
	}
	s->set_ticket (ticket);
	data << ticket;
	s->send (data);
	GS->set_population (GS->population() + 1);
}

void TaskHandler::handle_nickname_search (Session * s, string packet)
{
	string nickname = packet.substr (2);

	QueryResult * QR = sDatabase->query ("SELECT * from accounts WHERE Nickname='%s' LIMIT 1;", nickname.c_str());
	if (!QR)
	{
		s->send (Packet (SMSG_BASIC_NOTHING));
		return;
	}

	int ID = QR->fetch()[0].get_value<int>();
	delete QR;

	Packet found ("AF");
	LoginServer::GameServerMap & gs = login_server.gameservers();
	for (LoginServer::GameServerMap::iterator it = gs.begin() ; it != gs.end() ; ++it)
	{
		int count = 0, serverID = it->second->ID();
		QR = sDatabase->query ("SELECT * FROM account_data WHERE AccountID='%u' AND ServerID='%u';", ID, serverID);
		if (QR)
		{
			count = QR->get_row_count();
			delete QR;
		}
		if (count != 0)
			found.to_string() != "AF" ? (found << ";" << serverID << ";" << count) : (found << serverID << ";" << count);
	}
	found.to_string() != "AF" ? s->send (found) : s->send (Packet (SMSG_BASIC_NOTHING));
}
