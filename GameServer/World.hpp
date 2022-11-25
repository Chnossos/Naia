#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

class World : public Singleton<World>
{
	friend class Singleton<World>;
	public:
	// TYPEDEFS
		typedef unordered_map <int, boost::shared_ptr<CharacterInfo> > CharacterStorage;
		typedef unordered_map <int, boost::shared_ptr<Map> > MapStorage;
		typedef unordered_map <int, boost::shared_ptr<Session> > SessionsStorage;

	// GLOBAL OBJECTS
		Chat chat;

	// MISCELLANEOUS
		bool is_running () const { return _is_running; }

	// LOADING
		bool load ();
		bool load_maps ();
		void load_characters ();

	// COMMUNICATION
		void send_to_map (unsigned ID, const Packet & packet, unsigned bypass_sender = 0);
		void send_to_map (unsigned ID, const Packet & packet, bool GM_only);
		void send_to_all (const Packet & packet);

	// CHARACTERS
		void add_character (CharacterInfo * c);
		void del_character (int ID);
		void update_character_info (Character * character, bool is_online);

		CharacterInfo * get_character (const string & name);
		CharacterStorage get_characters (int accountID);

		unsigned next_characterID ();

		size_t population () const { return _sessions.size(); }

	// MAPS
		void add_map (Map * map);
		Map * get_map (int ID);
		Map * get_map (int x, int y);

	// SESSIONS
		void connect (Session * s);
		void disconnect (Session * s);
		Session * get_session (int ID);
		const SessionsStorage & sessions () const { return _sessions; }

	private:
		World ();
		~World ();

		bool _is_running;

	// CHARACTERS
		unsigned highest_charID;
		CharacterStorage _characters;

	// MAPS
		MapStorage _maps;

	// SESSIONS
		SessionsStorage _sessions;
};

#define world World::instance()

#endif // WORLD_HPP_INCLUDED
