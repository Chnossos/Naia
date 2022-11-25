#include "Naia.hpp"

initialize_singleton (Application);
Database * Naia::lsDatabase = NULL;
Database * Naia::gsDatabase = NULL;
Connector<Session> * Naia::uListener = NULL;

Application::Application () :
	_state (APP_NORMAL),
	_io_service (new io_service),
	_signals (*_io_service),
	timer (*_io_service),
	start_time (clock())
{
	_signals.add (SIGABRT);
	_signals.add (SIGINT);
	_signals.add (SIGTERM);
	_signals.add (SIGSEGV);
	_signals.add (SIGILL);
	_signals.add (SIGFPE);
#ifdef SIGBREAK
	_signals.add (SIGBREAK);
#elif defined (SIGBUS)
	_signals.add (SIGBUS);
#endif
	_signals.async_wait (bind (&Application::stop, this));

	setlocale (LC_ALL, "");

	string title = "Naia GameServer Release v." + string (SERVER_VERSION);
    set_application_title (title);
	ostringstream header;
	header << "\t\t\t___________________________________"	<< endl;
	header << "\t\t\t--    -- ,------, ,------. ,------,"	<< endl;
	header << "\t\t\t||\\   || | ,--, | --||--' | ,--, |"	<< endl;
	header << "\t\t\t|| \\  || | |  | |    ||    | |  | |"	<< endl;
	header << "\t\t\t||  \\ || | '--' |    ||    | '--' |"	<< endl;
	header << "\t\t\t||   \\|| | ,--, | .--||--, | ,--, |"	<< endl;
	header << "\t\t\t--    -- '-'  '-' `------` '-'  '-'"	<< endl;
	header << "\t\t\t-----------------------------------"	<< endl;
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
		kernel.log.initialize (GAME);
		bar.step();
	}
	kernel.log.print (TGREEN, true, "Log system successsfully activated !\n\n");

	vector<string> config_files;
	config_files.push_back (CONFIG_FILE_CHARACTER);
	config_files.push_back (CONFIG_FILE_NETWORK);
	config_files.push_back (CONFIG_FILE_SERVER);
	config_files.push_back (CONFIG_FILE_STAFF);

	kernel.config.parse (config_files);
	check_config();

	if (!start_up_database())
		return false;

	kernel.log.print (TBLUE, true, "Starting up the Thread Pool ...");
	{
		ProgressBar bar (0);
	}
	kernel.threadpool.run();
	kernel.log.print (TGREEN, true, "ThreadPool started !\n\n");

	if (!gameserver.connect ())
		return false;

	kernel.log.print_separator ("LOADING DATA");

	if (!world.load())
		return false;

	cout << endl;
	gameserver.update_status();

	kernel.log.print_separator ("SYSTEM STARTED UP");

	uListener = new Connector<Session> (kernel.threadpool.io_service(), kernel.config.get_property<int> (USER_PORT_PARAM, USER_PORT));
	if (!uListener->is_open())
	{
		//kernel.log.print_error ("Error while launching user connection listener !\n");
		return false;
	}
    uListener->run();
//	kernel.threadpool.add_connector ("user_listener", uListener);
	kernel.log.print (TWHITE, true, "Listening for user connections on port: %d\n\n", kernel.config.get_property<int> (USER_PORT_PARAM, USER_PORT));

	kernel.log.print_header (TYELLOW, true, "[INFO]", "Please type [ CTRL + C ] to safely shutdown the server.\n");
	kernel.log.print_header (TYELLOW, true, "[INFO]", "Uptime : %u ms\n\n", clock() - start_time);
	timer.expires_from_now (posix_time::minutes (TIMER));
	timer.async_wait (bind (&Application::display_uptime, this, asio::placeholders::error));

	_io_service->run();

	return true;
}

Application::~Application ()
{
	kernel.log.print_header (TYELLOW, true, "[INFO]", "Shutdown in progress ...\n\n");

	if (lsDatabase != NULL && gsDatabase != NULL)
	{
		GameServer::release();
		kernel.log.print_header (TBLUE, true, "[Server]", "\tDisconnected successfully from Login Server.\n");

		kernel.threadpool.shutdown();
		kernel.log.print_header (TBLUE, true, "[Kernel]", "\tThreadPool shut down successfully.\n");
	}

	if (uListener != NULL)
	{
		delete uListener;
		kernel.log.print_header (TBLUE, true, "[Listener]", "\tSession listener turned off.\n");
	}

	if (lsDatabase != NULL && gsDatabase != NULL)
	{
		World::release();
		kernel.log.print_header (TBLUE, true, "[World]", "\t\tWorld released successfully.\n");
	}

	if (lsDatabase != NULL)
	{
		delete lsDatabase;
		kernel.log.print_header (TBLUE, true, "[Database]", "\tLogin Database closed successfully.\n");
	}

	if (gsDatabase != NULL)
	{
		delete gsDatabase;
		kernel.log.print_header (TBLUE, true, "[Database]", "\tGame Database closed successfully.\n");
	}

	kernel.log.print_header (TBLUE, true, "[Kernel]", "\tLog system turned off.\n\n");
	kernel.log.print_header (TGREEN, true, "[INFO]", "Shutdown successfully !\n");

	if (_state != APP_RESTART)
	{
		kernel.log.print_separator ("CONTACT & SUPPORT");

		cout << "Naia Login Server, version " << SERVER_VERSION << " for Dofus 1.29.1, by Nemesis.\n" << '\n'
			<< "Thanks for using our server. Any suggestion is welcome.\n" << '\n'
			<< "For any question, advice, issue, and for more informations\n" << '\n'
			<< "about Naia, please visit the project website : \n" << '\n'
			<< "[ http://naia-project.niloo.fr/ ]" << endl;
	}
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

	if (kernel.config.get_property<string> (DB_GAME_USER_PARAM, DB_GAME_USER) == "root"
		|| kernel.config.get_property<string> (DB_LOGIN_USER_PARAM, DB_LOGIN_USER) == "root")
		kernel.log.print_warning ("root user name is not secure, change it to prevent intrusion.\n");

	if (kernel.config.get_property<string> (DB_GAME_PASS_PARAM, DB_GAME_PASS).empty()
		|| kernel.config.get_property<string> (DB_LOGIN_PASS_PARAM, DB_LOGIN_PASS).empty())
		kernel.log.print_warning ("Please remember that an empty database password is not secure.\n\n");
}

bool Application::start_up_database ()
{
	lsDatabase = new Database (3);
	kernel.log.print (TBLUE, true, "Trying to connect to the login database ...");
	{
		ProgressBar bar (1);
		if (!lsDatabase->connect (
			kernel.config.get_property<string> (DB_LOGIN_HOST_PARAM, DB_LOGIN_HOST),
			kernel.config.get_property<int> (DB_LOGIN_PORT_PARAM, DB_LOGIN_PORT),
			kernel.config.get_property<string> (DB_LOGIN_NAME_PARAM, DB_LOGIN_NAME),
			kernel.config.get_property<string> (DB_LOGIN_USER_PARAM, DB_LOGIN_USER),
			kernel.config.get_property<string> (DB_LOGIN_PASS_PARAM, DB_LOGIN_PASS)
		))
		{
			kernel.log.print_error ("Failed to connect to the login database ... Please check your setup and try again later.\n");
			delete lsDatabase;
			lsDatabase = NULL;
			return false;
		}
		bar.step();
	}
	kernel.log.print (TGREEN, true, "Connection etablished successfully !\n\n");

	gsDatabase = new Database (3);
	kernel.log.print (TBLUE, true, "Trying to connect to the game database ...");
	{
		ProgressBar bar (1);
		if (!gsDatabase->connect (
			kernel.config.get_property<string> (DB_GAME_HOST_PARAM, DB_GAME_HOST),
			kernel.config.get_property<int> (DB_GAME_PORT_PARAM, DB_GAME_PORT),
			kernel.config.get_property<string> (DB_GAME_NAME_PARAM, DB_GAME_NAME),
			kernel.config.get_property<string> (DB_GAME_USER_PARAM, DB_GAME_USER),
			kernel.config.get_property<string> (DB_GAME_PASS_PARAM, DB_GAME_PASS)
		))
		{
			kernel.log.print_error ("Failed to connect to the game database ... Please check your setup and try again later.\n");
			delete gsDatabase;
			gsDatabase = NULL;
			return false;
		}
		bar.step();
	}
	kernel.log.print (TGREEN, true, "Connection etablished successfully !\n\n");
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

		kernel.log.print_header (TYELLOW, true, "[INFO]", "Total online players : %u\n\n", world.population());
		timer.expires_from_now (boost::posix_time::minutes (TIMER));
		timer.async_wait (bind (&Application::display_uptime, this, boost::asio::placeholders::error));
	}
	else
		kernel.log.print_error ("Uptime timer could not be launched: %s\n", error.message().c_str());
}
