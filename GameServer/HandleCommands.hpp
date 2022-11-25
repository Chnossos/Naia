#ifndef HANDLECOMMANDS_HPP_INCLUDED
#define HANDLECOMMANDS_HPP_INCLUDED

namespace TaskHandler
{
	// -- CHECKERS
	void handle_basic_command (Session *, string);
	void handle_user_command (Session *, string);

	// -- ADMINS
	void handle_command_kick (Session *, string);

	// -- USERS
	void handle_user_command_patchnote (Session *, string);
}

#endif // HANDLECOMMANDS_HPP_INCLUDED
