#include "Naia.hpp"
#include <csignal>

initialize_singleton (Application);
Database * Naia::sDatabase = NULL;
//Connector<GameServer> * Naia::gListener = NULL;
//Connector<Session> * Naia::uListener = NULL;

Application::Application () :
	_io_service (new io_service),
	_signals (*_io_service),
	timer (*_io_service),
	start_time (clock())
{
	_signals.add (SIGABRT);
	_signals.add (SIGINT);
	_signals.add (SIGTERM);
#ifdef SIGBREAK
	_signals.add (SIGBREAK);
#endif
	_signals.async_wait (bind (&Application::stop, this));

	setlocale (LC_ALL, "");

	string title = "Naia Loginserver Release v." + string (SERVER_VERSION);
    set_application_title (title);
	ostringstream header;
	header << "\t\t\t___________________________________"	<< '\n';
	header << "\t\t\t--    -- ,------, ,------. ,------,"	<< '\n';
	header << "\t\t\t||\\   || | ,--, | `--||--' | ,--, |"	<< '\n';
	header << "\t\t\t|| \\  || | |  | |    ||    | |  | |"	<< '\n';
	header << "\t\t\t||  \\ || | '--' |    ||    | '--' |"	<< '\n';
	header << "\t\t\t||   \\|| | ,--, | .--||--, | ,--, |"	<< '\n';
	header << "\t\t\t--    -- '-'  '-' `------` '-'  '-'"	<< '\n';
	header << "\t\t\t-----------------------------------"	<< '\n';
	kernel.log.print (TBLUE, false, header.str().c_str());
	cout << "\t\t\t-=[ By Nemesis for Dofus 1.29.1 ]=-\n" << endl;
	kernel.log.print (TYELLOW, true, "Starting up services ...");
}

bool Application::run ()
{
	kernel.log.print_separator ("LOADING SYSTEM");

	kernel.log.print (TBLUE, true, "Activation of log system ...");
	{
		ProgressBar bar (1);
		kernel.log.initialize (LOGIN);
		bar.step();
	}
	kernel.log.print (TGREEN, true, "Log system successsfully activated !\n\n");

	vector<string> config_files;
	config_files.push_back (CONFIG_FILE_NETWORK);
	config_files.push_back (CONFIG_FILE_SERVER);
	kernel.config.parse (config_files);

	check_config();

	kernel.log.print (TBLUE, true, "Trying to connect to the database ...");

	if (!start_up_database())
		return false;

	kernel.log.print (TGREEN, true, "Connection etablished successfully !\n\n");

	kernel.log.print (TBLUE, true, "Starting up the Thread Pool ...");
	{
		ProgressBar bar (0);
	}
	kernel.threadpool.run();
	kernel.log.print (TGREEN, true, "ThreadPool started !\n");

	kernel.log.print_separator ("LOADING DATA");

	if (!login_server.load_servers())
	{
		kernel.log.print_error ("Any gameserver registered ... Please use GSRegister to add a gameserver and try again.\n");
		return false;
	}

	kernel.log.print_separator ("SYSTEM STARTED UP");

	Connector<Session> uListener (kernel.threadpool.io_service(), kernel.config.get_property<int> (USER_PORT_PARAM, USER_PORT));
	if (!uListener.is_open())
	{
		//kernel.log.print_error ("Error while launching user connection listener !\n");
		return false;
	}
	uListener.run();
	kernel.log.print (TWHITE, true, "Listening for user connections on port: %d\n", kernel.config.get_property<int> (USER_PORT_PARAM, USER_PORT));

	Connector<GameServer> gListener (kernel.threadpool.io_service(), kernel.config.get_property<int> (GAME_PORT_PARAM, GAME_PORT));
	if (!gListener.is_open())
	{
		//kernel.log.print_error ("Error while launching game connection listener !\n");
		return false;
	}
	gListener.run();
	kernel.log.print (TWHITE, true, "Listening for gameserver connections on port: %d\n\n", kernel.config.get_property<int> (GAME_PORT_PARAM, GAME_PORT));

	kernel.log.print_header (TYELLOW, true, "[INFO]", "Please type [ CTRL + C ] to safely shutdown the server.\n");
	kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u ms\n\n", clock() - start_time);

	// On démarre le timer d'affichage de l'uptime
	timer.expires_from_now (posix_time::minutes (APP_TIMEOUT));
	timer.async_wait (bind (&Application::display_uptime, this, asio::placeholders::error));

	_io_service->run();

	return true;
}

Application::~Application ()
{
	kernel.log.print_header (TYELLOW, true, "[INFO]", "Shutdown in progress ...\n\n");

	if (sDatabase != NULL)
	{
		kernel.threadpool.shutdown();
		kernel.log.print_header (TBLUE, true, "[Kernel]", "\tThreadPool shut down successfully.\n");
	}

	LoginServer::release();

	if (sDatabase != NULL)
		kernel.log.print_header (TBLUE, true, "[Database]", "\tDatabase closed successfully.\n");

	kernel.log.print_header (TBLUE, true, "[Kernel]", "\tLog system turned off.\n\n");
	kernel.log.print_header (TGREEN, true, "[INFO]", "Shutdown successfully !\n");

	kernel.log.print_separator ("CONTACT & SUPPORT");

	cout << "Naia Login Server, version " << SERVER_VERSION << " for Dofus 1.29.1, by Nemesis.\n" << '\n'
		<< "Thanks for using our server. Any suggestion is welcome.\n" << '\n'
		<< "For any question, advice, issue, and for more informations\n" << '\n'
		<< "about Naia, please visit the project website : \n" << '\n'
		<< "[ http://naia-project.niloo.fr/ ]" << endl;

	Kernel::release();
}

void Application::check_config ()
{
	kernel.log.print (TBLUE, true, "Checking configuration ...");
	{
		ProgressBar bar (0);
	}

	if (!kernel.config.unparsed().empty())
	{
		kernel.log.print (TRED, true, "Checking done ...\n\n");
		kernel.log.print_error ("Following file(s) could not be read :\n");
		vector <string> error = kernel.config.unparsed();
		for (size_t i = 0 ; i < error.size() ; ++i)
			cerr << error[i] << '\n';
		cerr << endl;
		return;
	}
	else
		kernel.log.print (TGREEN, true, "Checking done ...\n\n");

	if (kernel.config.get_property<string> (DB_USER_PARAM, DB_USER) == "root")
		kernel.log.print_warning ("root user name is not secure, change it to prevent intrusion.\n");

	if (kernel.config.get_property<string> (DB_PASS_PARAM, DB_PASS).empty())
		kernel.log.print_warning ("Please remember that an empty database password is not secure.\n\n");
}

bool Application::start_up_database ()
{
	sDatabase = new Database (3);
	{
		ProgressBar bar (1);
		if (!sDatabase->connect (
			kernel.config.get_property<string> (DB_HOST_PARAM, DB_HOST),
			kernel.config.get_property<int> (DB_PORT_PARAM, DB_PORT),
			kernel.config.get_property<string> (DB_NAME_PARAM, DB_NAME),
			kernel.config.get_property<string> (DB_USER_PARAM, DB_USER),
			kernel.config.get_property<string> (DB_PASS_PARAM, DB_PASS)
		))
		{
			delete sDatabase;
			sDatabase = NULL;
			return false;
		}
		bar.step();
	}
	return true;
}

void Application::display_uptime (const system::error_code & error)
{
	if (!error)
	{
		time_t 	uptime 	= clock() - start_time,
				sec 	= uptime / 1000,
				min 	= sec / 60,
				hours 	= sec / (60 * 60),
				days 	= sec / (24 * 60 * 60);

		if (min <= 0)
			kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u s\n", sec);
		else if (hours <= 0)
			kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u min %u s\n", min, sec % 60);
		else if (days <= 0)
			hours == 1
			? kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : 1 hour %u min %u s\n", min % 60, sec % 60)
			: kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u hours %u min %u s\n", hours, min % 60, sec % 60);
		else
			days == 1
			? kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : 1 day %u hours %u min %u s\n", hours % 24, min % 60, sec % 60)
			: kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u days %u hours %u min %u s\n", days, hours % 24, min % 60, sec % 60);

		timer.expires_from_now (posix_time::minutes (APP_TIMEOUT));
		timer.async_wait (bind (&Application::display_uptime, this, asio::placeholders::error));
	}
	else
		kernel.log.print_error ("Uptime timer could not be launched: %s\n", error.message().c_str());
}
