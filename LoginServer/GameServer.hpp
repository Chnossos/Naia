#ifndef GAMESERVER_H_INCLUDED
#define GAMESERVER_H_INCLUDED

class GameServer
{
	public:
	// INITIALIZATION & DESTRUCTION
		explicit GameServer (tcp::socket * socket);
		GameServer (int ID, const string & hexid);
		~GameServer ();

		static void init_task_handlers ();

	// NETWORK
		void run ();
		void read_header 	(const system::error_code & e);
		void read_data 		(const system::error_code & e);
		void handle_error 	(const system::error_code & e);
		void send (uint16_t header, const string & content = "");

	// GETTERS
		const int & ID () const 				{ return _ID; }
		const int & state () const 				{ return _state; }
		const int & connection_port () const 			{ return _user_port; }
		const int & population () const 		{ return _population; }
		const int & population_limit () const 	{ return _population_limit; }
		const int & min_access_level () const 	{ return _min_access_level; }

		const int port () const { return remote_ip() == "127.0.0.1" ? _socket->local_endpoint().port() : _socket->remote_endpoint().port(); }

		const bool & ignore_pop_limit () const 	{ return _ignore_population_limit; }

		const string & hexid () const 			{ return _hexid; }
		const string & connection_ip () const 	{ return _connection_ip; }

		const string local_ip () const 			{ return _socket->local_endpoint().address().to_string(); }
		const string remote_ip () const 		{ return _socket->remote_endpoint().address().to_string(); }

		tcp::socket * socket () 				{ return _socket.get(); }

	// SETTERS
		void set_ID (unsigned ID) 				{ _ID = ID; }
		void set_state (unsigned state) 		{ _state = state; }
		void set_connection_port (unsigned port){ _user_port = port; }
		void set_population (unsigned pop) 		{ _population = pop; }
		void set_population_limit (unsigned max){ _population_limit = max; }
		void set_min_access_level (int min) 	{ _min_access_level = min; }

		void set_ignore_pop_limit (bool ignore) { _ignore_population_limit = ignore; }

		void set_hexid (const string & hexid) 		{ _hexid = hexid; }
		void set_connection_ip (const string & ip) 	{ _connection_ip = ip; }

	// OPERATORS
		GameServer * operator = (const GameServer & gs);

	private:
	// DATA
		int _ID,
			_state,
			_user_port,
			_population,
			_population_limit,
			_min_access_level;

		bool _ignore_population_limit;

		string 	_hexid,
				_connection_ip;

	// NETWORK
		socket_ptr 		_socket;
        vector<uint8_t> _packet;
        boost::array <uint16_t, 2> _header;
};

#endif // GAMESERVER_H_INCLUDED
