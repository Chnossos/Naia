#ifndef HANDLEGAMETASKS_HPP_INCLUDED
#define HANDLEGAMETASKS_HPP_INCLUDED

namespace TaskHandler
{
	void handle_game_creation (Session *, string);
	void handle_game_info_map (Session *, string);
	void handle_game_create_action (Session *, string);
	void handle_game_finish_action (Session *, string);
}

#endif // HANDLEGAMETASKS_HPP_INCLUDED
