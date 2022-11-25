#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

enum RequiredData
{
	NONE = 0,
	HAVE_ACCOUNT,
	HAVE_LIST,
};

class Session
{
	public:
	// INITIALIZATION
		explicit Session (tcp::socket * socket);
		~Session ();
		static void init_task_handlers ();

		void load_data ();

	// NETWORK/COMMUNICATION
		void run ();
		void parse_packet (const system::error_code & e);
		void handle_error (const system::error_code & e);
		void handle_timer (const system::error_code & e);

		void send (const Packet & packet, bool print = true) const
		{
			boost::asio::write (*_socket, boost::asio::buffer (packet.to_string() + (char) 0x00));
			if (print)
				kernel.log.print_debug ("[S:%u] Sent <%s>\n", _ID, packet.to_string().c_str());
			//boost::this_thread::sleep (boost::posix_time::milliseconds (75));
		}

	// GETTERS
		const int & ID () const 			{ return _ID; }
		const int & access_level () const 	{ return _access_level; }

		int * queue_pos () 					{ return _queue_pos; }

		const bool is_waiting_for_nickname () { return _waiting_for_nickname; }

		time_t is_subscriber () const;

		const string & name () const 			{ return _name; }
		const string & nickname () const 		{ return _nickname; }
		const string & question () const 		{ return _secret_question; }
		const string & hash_key () const 		{ return _hash_key; }
		const string & ticket () const 			{ return _ticket; }
		const string & characters_distribution () const { return _characters_distribution; }

		const string local_ip () const 		{ return _socket->local_endpoint().address().to_string(); }
		const string remote_ip () const 	{ return _socket->remote_endpoint().address().to_string(); }

		tcp::socket * socket () 			{ return _socket.get(); }

	// SETTERS
		void set_ID (int ID)			{ _ID = ID; }
		void set_access_level (int lvl) { _access_level = lvl; }

		void wait_for_nickname () { _waiting_for_nickname = true; run(); }

		void set_subscribe_end_date (time_t end_date) { _subscribe_end_date = end_date; }

		void set_name (const string & name) 		{ _name = name; }
		void set_nickname (const string & nick) 	{ _nickname = nick; }
		void set_question (const string & question) { _secret_question = question; }
		void set_hash_key (const string & hash_key) { _hash_key = hash_key; }
		void set_ticket (const string & ticket) 	{ _ticket = ticket; }

	private:
	// DATA
		int 	_ID,
				_access_level;

		int * 	_queue_pos;

		bool	_waiting_for_nickname;

		time_t 	_subscribe_end_date;

		string 	_name,
				_nickname,
				_secret_question,
				_hash_key,
				_ticket,
				_characters_distribution;

	// NETWORK
		socket_ptr _socket;
		boost::asio::streambuf _buffer;
		int _packet_count;
		deadline_timer _timer;
};

#endif // SESSION_H_INCLUDED
