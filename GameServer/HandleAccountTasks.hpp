#ifndef HANDLEACCOUNTTASKS_HPP_INCLUDED
#define HANDLEACCOUNTTASKS_HPP_INCLUDED

class Session;

namespace TaskHandler
{
	void do_nothing (Session *, string);

	void handle_account_ticket (Session *, string);
	void handle_account_regional_version (Session *, string);
	void handle_account_characters_list (Session *, string);
	void handle_account_wait_queue (Session *, string);
	void handle_account_add_character (Session *, string);
	void handle_account_del_character (Session *, string);
	void handle_account_generate_name (Session *, string);
	void handle_account_select_char (Session *, string);
	void handle_account_confirm_migration (Session *, string);
}

#endif // HANDLEACCOUNTTASKS_HPP_INCLUDED
