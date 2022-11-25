#ifndef SESSION_HPP_INCLUDED
#define SESSION_HPP_INCLUDED

class Character;

struct GameAction
{
	int ID, type;
	string args, packet;
};

enum RequiredData
{
	NONE = 0,
	HAVE_ACCOUNT,
	HAVE_CHARACTER,
	HAVE_LIST,
};

class Session
{
	public:
	// TYPEDEFS
		typedef boost::function <void(Session *, string)> Task;
		typedef boost::shared_ptr<GameAction> action_ptr;
		typedef unordered_map<int, action_ptr> ActionStorage;

	// INITIALIZATION / DESTRUCTION
		explicit Session (tcp::socket * socket);
		~Session ();
		static void load_tasks ();

	// NETWORK
		void run ();
		void handle_timer (const system::error_code & e);
		void handle_error (const system::error_code & e);
		void parse_packet (const system::error_code & e);
		void send (const Packet & packet, bool print = true) const
		{
			boost::asio::write (*_socket, boost::asio::buffer (packet.to_string() + (char) 0x00));
			if (print)
				kernel.log.print_debug ("[S:%u] Sent <%s>\n", _ID, packet.to_string().c_str());
			boost::this_thread::sleep (boost::posix_time::milliseconds (75));
		}

	// GETTERS
		const int & ID () const 			{ return _ID; }
		const int & access_level () const 	{ return _access_level; }

		const time_t & last_connection_date () const 	{ return _last_connection_date; }
		const time_t & subscribe_end_date () const 		{ return _subscribe_end_date; }

		const string & name () const 		{ return _name; }
		const string & nickname () const 	{ return _nickname; }
		const string & answer () const 		{ return _answer; }
		const string & last_ip () const 	{ return _last_ip; }
		const string remote_ip () const 	{ return _socket->remote_endpoint().address().to_string(); }
		const string local_ip () const 		{ return _socket->local_endpoint().address().to_string(); }
		const string & active_channels () const { return _active_channels; }

		const socket_ptr & socket () const 	{ return _socket; }

		Character * character () 			{ return _character; }
		ActionStorage & actions ()			{ return _actions; }

	// SETTERS
		void set_ID (unsigned ID) 			{ _ID = ID; }
		void set_access_level (int lvl) 	{ _access_level = lvl; }

		void set_subscribe_end_date (time_t date) 	{ _subscribe_end_date = date; }
		void set_last_connection_date (time_t date) { _last_connection_date = date; }
		void set_next_recruit_date (time_t date) 	{ _next_recruit_date = date; }
		void set_next_trade_date (time_t date) 		{ _next_trade_date = date; }
		void set_mute_end_date (time_t date) 		{ _mute_end_date = date; }

		void set_name (const string & name ) 				{ _name = name; }
		void set_nickname (const string & name) 			{ _nickname = name; }
		void set_answer (const string & answer) 			{ _answer = answer; }
		void set_last_ip (const string & IP) 				{ _last_ip = IP; }
		void set_active_channels (const string & channels) 	{ _active_channels = channels; }

		void set_character (Character * c) 	{ _character = c; }

	// PROCEEDINGS
		time_t is_subscriber () const;
		time_t is_muted () const;
		time_t next_trade_date () const;
		time_t next_recruit_date () const;

	private:
	// NETWORK
		socket_ptr 		_socket;
		deadline_timer 	_timer;
		asio::streambuf _buffer;
		int 			_packet_count;

	// DATA
		int 	_ID,
				_access_level;

		time_t 	_subscribe_end_date,
				_last_connection_date,
				_next_recruit_date,
				_next_trade_date,
				_mute_end_date;

		string 	_name,
				_nickname,
				_answer,
				_last_ip,
				_active_channels;

		Character * 	_character;
		ActionStorage 	_actions;
};

#endif // SESSION_HPP_INCLUDED
