#include "Naia.hpp"

initialize_singleton (StorageProvider);

void test (Session * s, string packet)
{
	Packet test (SMSG_INFO_MESSAGE);
	test << packet;
	s->send (test);
}

StorageProvider::StorageProvider ()
{
	admin_commands["kick"] = TaskHandler::handle_command_kick;

	user_commands["patchnote"] = TaskHandler::handle_user_command_patchnote;
	user_commands["test"] = test;
}
