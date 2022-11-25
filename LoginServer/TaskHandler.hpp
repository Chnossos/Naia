#ifndef TASKSHANDLER_HPP_INCLUDED
#define TASKSHANDLER_HPP_INCLUDED

#define REQUIRED_VERSION "AlEv"
#define ACCOUNT_LOGIN_ERROR "AlE"
#define GAMESERVER_LOGOUT_PLAYER "KP"
#define ACCOUNT_DOFUS_PSEUDO "Ad"
#define ACCOUNT_COMMUNITY "Ac"
#define ACCOUNT_AUTH_SUCCESS "AlK"
#define ACCOUNT_QUESTION "AQ"
#define ACCOUNT_HOSTS "AH"
#define ACCOUNT_WAITQUEUE "Af"
#define ACCOUNT_CHARACTERS "AxK"
#define ACCOUNT_SERVER_SUCCESS "AYK"
#define ACCOUNT_SERVER_ERROR "AYE"
#define ACCOUNT_MAINTENANCE "AlEm"
#define HELLO_CONNECTION "HC"
#define NICKNAME_SEARCH "AF"

class Session;
class GameServer;

namespace TaskHandler
{
		/* ****************
		 * MISC FUNCTIONS *
		 * ***************/
	void packet_format_queue (Packet &, Session *);

		/* ******************
		 * SERVER FUNCTIONS *
		 * *****************/

	void handle_connection (GameServer *, string packet);
	void handle_disconnection (GameServer *, string packet);
	void handle_update_info (GameServer *, string packet);
	void handle_update_pop (GameServer *, string packet);

		/* ******************
		 * SESSION FUNTIONS *
		 * *****************/

	void handle_account_version (Session *, string);
	void handle_account_password (Session *, string);
	void handle_account_queue (Session *, string);
	void handle_account_gameservers (Session *, string);
	void handle_account_characters (Session *, string);
	void handle_nickname_search (Session *, string);
}

#endif // TASKSHANDLER_HPP_INCLUDED
