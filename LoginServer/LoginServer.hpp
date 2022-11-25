#ifndef LOGINSERVER_H_INCLUDED
#define LOGINSERVER_H_INCLUDED

class GameServer;
class Session;

class LoginServer : public Singleton<LoginServer>
{
friend class Singleton<LoginServer>;
	public:
	// Typedefs
		typedef map<int, boost::shared_ptr<GameServer> > GameServerMap;
		typedef unordered_map<int, boost::shared_ptr<Session> > SessionMap;

	// Loading
		bool load_servers ();

	// Storage
		void connect (Session * s);
		void connect (GameServer * g);

		void disconnect (Session * s);
		void disconnect (GameServer * g);

	// Access
		GameServerMap & gameservers () { return _gameservers; }
		SessionMap & sessions () { return _sessions; }

		GameServer * get_server_by_id (int id);
		Session * get_session_by_id (int id);

	// Misc
		void refresh_server_list ();
		void build_server_list (Packet & packet);

		bool running () const { return _running; }

	private:
		LoginServer ();
		~LoginServer ();

		GameServerMap _gameservers;

		SessionMap _sessions;

		bool _running;
};

#define login_server LoginServer::instance()

#endif // LOGINSERVER_H_INCLUDED
