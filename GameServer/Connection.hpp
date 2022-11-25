#ifndef CONNECTION_HPP_INCLUDED
#define CONNECTION_HPP_INCLUDED

class GameServer : public Singleton<GameServer>
{
	friend class Singleton<GameServer>;
	public:
	// NETWORK
		bool connect ();
		bool authentificate ();
		void update_status () const;
		void run ();
		void read_header (const system::error_code & e);
		void read_data (const system::error_code & e);
		void handle_error (const system::error_code & e);
		void send (uint16_t id, const string & content = "");

	// GETTERS
		const int & ID () const 		{ return _ID; }
		const int & state () const 		{ return _state; }

		const string & hexid () const 	{return _hexid; }

	// SETTERS
		void set_ID (unsigned ID) 		{ _ID = ID; }
		void set_state (unsigned state) { _state = state; }

		void set_hexid (const string & hexid) { _hexid = hexid; }

	private:
		GameServer ();
		~GameServer ();

	// DATA
		int _ID,
			_state;

		string _hexid;

	// NETWORK
		io_ptr _io_service;
		socket_ptr _socket;
        vector<uint8_t> _packet;
		vector<uint8_t> _content;
        boost::array <uint16_t, 2> _header;
};

#define gameserver GameServer::instance()

namespace ServerTaskHandler
{
	void on_update (string);
	void on_disconnect (string);
	void on_kick_player (string);
}

#endif // CONNECTION_HPP_INCLUDED
