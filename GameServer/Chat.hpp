#ifndef CHAT_HPP_INCLUDED
#define CHAT_HPP_INCLUDED

struct AutoAnnouncement
{
	int 	ID;
	string 	text,
			initial,
			delay;
};

class Chat
{
	public:
		Chat ();

		void announce (const string & msg, const string & name = "") const;

		void send_to_all (char channel, const Packet & packet) const;
		void send_to_map (char channel, unsigned ID, const Packet & packet) const;
		void send_private (Session * sender, const string & packet) const;

	// GETTERS
		const vector<string> & annoucements () const { return _announcements; }

		time_t is_active_channel (char channel) const;

	private:
		unordered_map<char, time_t> _channels;
		vector<string> _announcements;
		vector<AutoAnnouncement> _auto_announcements;
};

#endif // CHAT_HPP_INCLUDED
