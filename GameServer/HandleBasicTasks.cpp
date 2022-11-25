#include "Naia.hpp"

void TaskHandler::handle_basic_datetime (Session * s, string packet)
{
	Packet date (SMSG_BASIC_DATE);
	time_t t = time (NULL);
	struct tm * tm = localtime (&t);

	string 	day 	= Naia::to_string<int> (tm->tm_mday),
			month 	= Naia::to_string<int> (tm->tm_mon),
			year 	= Naia::to_string<int> (tm->tm_year);

	if (day.size() == 1)
		day = "0" + day;

	if (month.size() == 1)
		month = "0" + month;

	date << year << ";" << month << ";" << day;
	s->send (date);

	Packet time (SMSG_BASIC_TIME);
	time << (t * 1000);
	s->send (time);
}

void TaskHandler::handle_basic_message (Session * s, string packet)
{
	if (packet[2] == 'Â') return;
	if (world.chat.is_active_channel (packet[2]) > 0)
	{
		s->send (Packet ("Im0113|"), false);
		return;
	}

	time_t mute_duration = s->is_muted();
	if (mute_duration)
	{
		Packet mute (SMSG_INFO_MESSAGE);
		mute << "1124;" << mute_duration;
		s->send (mute, false);
		return;
	}

	Packet info (SMSG_INFO_MESSAGE);

	/*boost::replace_all (packet, "<", "&lt|");
	boost::replace_all (packet, ">", "&gt|");*/

	vector<string> table = Naia::split (packet, '|');

	if (!isalpha (packet[2]) && packet[3] != '|')
	{
		Packet error ("cMEf");
		error << table[0].substr (2);
		s->send (error);
		return;
	}
	switch (packet[2])
	{
		case CHANNEL_MAP: // TODO: in fight
		{
			if (table[1].size() > 1 && table[1][0] == '.')
			{
				TaskHandler::handle_user_command (s, table[1].substr (1));
				return;
			}
		}
		case CHANNEL_PARTY:
		case CHANNEL_TEAM: // TODO: en fight uniquement
		case CHANNEL_GUILD: // TODO: en guild uniquement
		case CHANNEL_MEETIC:
			break;
		case CHANNEL_TRADE:
		{
			if (s->character()->level() < 6)
			{
				info << "0157;6";
				s->send (info);
				return;
			}
			time_t timeout = s->next_trade_date();
			if (timeout > 0)
			{
				info << "0115;" << timeout;
				s->send (info);
				return;
			}
			s->set_next_trade_date (time (NULL) + 60);
			break;
		}
		case CHANNEL_RECRUIT:
		{
			if (s->character()->level() < 6)
			{
				info << "0157;6";
				s->send (info);
				return;
			}
			time_t timeout = s->next_recruit_date();
			if (timeout > 0)
			{
				info << "0115;" << timeout;
				s->send (info);
				return;
			}
			s->set_next_recruit_date (time (NULL) + 60);
			break;
		}
		case CHANNEL_ADMIN:
		{
			if (s->character()->access_level() < 1)
			{
				s->send (Packet (SMSG_BASIC_NOTHING));
				return;
			}
			break;
		}
		case CHANNEL_PVP:
		{
			if (s->character()->align_side() == 0 || !s->character()->has_wings_deployed() || Naia::get_align_rank (s->character()->honor()) < 3)
				// TODO: OU pas de spé métier pvp
			{
				info << "0106";
				s->send (info);
				return;
			}
			if (s->character()->disgrace() > 0)
			{
				info << "183";
				s->send (info);
				return;
			}
			break;
		}
		default:
			world.chat.send_private (s, packet);
		return;
	}

	Packet data (SMSG_CHAT_MESSAGE_SUCCESS);
	data << packet[2] << "|" << s->character()->ID() << "|" << s->character()->name() << "|" << table[1];

	// TODO: logguer le chat
	switch (packet[2])
	{
		case CHANNEL_MAP:
			world.chat.send_to_map (CHANNEL_MAP, s->character()->map()->ID(), data);
			break;
		default:
			world.chat.send_to_all (packet[2], data);
	}
}

void TaskHandler::handle_basic_state (Session * s, string packet)
{
	Packet info (SMSG_INFO_MESSAGE);
	Character & character = *s->character();

	switch (packet[2])
	{
		case 'A':
		{
			switch (character.is_quiet())
			{
				case 0:
				{
					character.set_away (!character.is_away());
					info << (character.is_away() ? "037;" : "038;");
					break;
				}
				case 1:
				{
					character.set_quiet (false);
					character.set_away (true);
					info << "037;";
					break;
				}
			}
			break;
		}
		case 'I':
		{
			switch (character.is_away())
			{
				case 0:
				{
					character.set_quiet (!character.is_quiet());
					info << (character.is_quiet() ? "050;" : "051;");
					break;
				}
				case 1:
				{
					character.set_away (false);
					character.set_quiet (true);
					info << "050;";
					break;
				}
			}
			break;
		}
		default:
			return;
	}
	s->send (info);
}
