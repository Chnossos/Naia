#include "Naia.hpp"

Chat::Chat ()
{
	_channels[CHANNEL_MAP] 		= 0;
	_channels[CHANNEL_PVP] 		= 0;
	_channels[CHANNEL_RECRUIT] 	= 0;
	_channels[CHANNEL_TRADE] 	= 0;
	_channels[CHANNEL_MEETIC] 	= 0;

	ifstream file ("./data/announcements.txt");
	if (file)
	{
		string line;
		while (getline (file, line))
		{
			if (!line.empty() && line[0] != '#')
				_announcements.push_back (line);
		}
	}
}

void Chat::announce (const string & msg, const string & name) const
{
	Packet announce (SMSG_CHAT_SERVER_MESSAGE);
	if (name.empty())
		announce << msg;
	else
		announce << "<a href='asfunction:onHref,ShowPlayerPopupMenu," << name << "'>[" << name << "]</a> : " << msg;

	send_to_all (CHANNEL_INFO, announce);
}

void Chat::send_to_all (char channel, const Packet & packet) const
{
	if (_channels.find (channel) != _channels.end() && _channels.at (channel) - time (NULL) > 0)
		return;

	World::SessionsStorage sessions = world.sessions();
	Character * character = NULL;
	for (World::SessionsStorage::iterator it = sessions.begin() ; it != sessions.end() ; ++it)
	{
		character = it->second->character();
		if (character != NULL)
		{
			switch (channel)
			{
				case CHANNEL_ADMIN:
				{
					if (character->access_level() > 0)
						it->second->send (packet, false);
					break;
				}
				default:
					it->second->send (packet, false);
			}
		}
	}
}

void Chat::send_to_map (char channel, unsigned ID, const Packet & packet) const
{
	if (_channels.find (channel) != _channels.end() && _channels.at (channel) - time (NULL) > 0)
		return;

	Map::UnitStorage units = world.get_map(ID)->units();
	for (Map::UnitStorage::iterator it = units.begin() ; it != units.end() ; ++it)
	{
		if (it->second->type() == UNIT_CHARACTER)
		{
			Character * ch = (Character *)it->second;
			ch->session()->send (packet, false);
		}
	}
}

void Chat::send_private (Session * sender, const string & packet) const
{
	Packet info (SMSG_INFO_MESSAGE);
	Packet error (SMSG_CHAT_MESSAGE_ERROR);
	string authorized_char = "abcdefghijklmnopqrstuvwxyz-";

	vector<string> table = Naia::split (packet, '|');
	string target = table[0].substr (2);

	for (size_t i = 0 ; i < target.size() ; ++i) // Vérifie si tous les caractères sont acceptables
	{
		if (authorized_char.find (tolower(target[i])) == string::npos)
		{
			error << "S"; // Erreur de syntaxe : nom impossible
			sender->send (error);
			return;
		}
	}

	CharacterInfo * CI = world.get_character (target);
	if (!CI || CI->online_character == NULL)
	{
		error << "f" << target; // User pas co
		sender->send (error);
		return;
	}

	if (sender->character()->is_away() || sender->character()->is_quiet() /* TODO: OR quiet mais target n'est pas ami*/)
	{
		info << "072;"; // Vous etes actuellement absent, ...
		sender->send (info);
		return;
	}

	Character & receiver = *CI->online_character;

	if (receiver.is_away() || receiver.is_quiet()/* TODO: OR est ennemi/ignoré ; OR quiet mais pas ami*/)
	{
		info << "114;" << receiver.name(); // Joueur NAME absent
		sender->send (info);
		return;
	}

	Packet from (SMSG_CHAT_MESSAGE_SUCCESS), to (SMSG_CHAT_MESSAGE_SUCCESS);
	from << "F|" << sender->character()->ID() << "|" << sender->character()->name() << "|" << table[1];
	to << "T|" << receiver.ID() << "|" << receiver.name() << "|" << table[1];

	sender->send (to);
	receiver.session()->send (from);
}

time_t Chat::is_active_channel (char channel) const
{
	if (_channels.find (channel) == _channels.end())
		return 0;

	time_t duration = _channels.at(channel) - time (NULL);
	return duration <= 0 ? 0 : duration;
}
