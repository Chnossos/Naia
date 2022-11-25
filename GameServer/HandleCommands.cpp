#include "Naia.hpp"

void TaskHandler::handle_basic_command (Session * s, string packet)
{
	string command = packet.substr (0, packet.find (' '));
	StorageProvider::CommandsStorage::iterator it = storage_provider.admin_commands.find (command);
	if (it == storage_provider.admin_commands.end())
	{
		Packet msg (SMSG_INFO_MESSAGE);
		msg << "116;Error~Command '" << command << "' does not exists !";
		s->send (msg);
		return;
	}
	(it->second)(s, packet);
}

void TaskHandler::handle_user_command (Session * s, string packet)
{
	string command;
	size_t pos = packet.find (' ');
	pos == string::npos
	? command = packet
	: command = packet.substr (0, pos);
	StorageProvider::CommandsStorage::iterator it = storage_provider.user_commands.find (command);
	if (it == storage_provider.user_commands.end())
	{
		Packet msg (SMSG_INFO_MESSAGE);
		msg << "116;Error~Command '" << command << "' does not exists !";
		s->send (msg);
		return;
	}
	(it->second)(s, packet.substr (packet.find (' ') + 1));
}

void TaskHandler::handle_command_kick (Session * s, string packet)
{

}

/* ***********************************************
 * 					USER COMMANDS				 *
 * **********************************************/

void TaskHandler::handle_user_command_patchnote (Session * s, string packet)
{
	Packet msg (SMSG_CHAT_SERVER_MESSAGE);
	msg 	<< "- Les commandes /invisible et /away sont fonctionnelles (/invisible a le meme effet que /away jusqu'a la mise en place des amis).\n"
			<< "- Le systeme de commande est a l'essai.";
	s->send (msg);
}
