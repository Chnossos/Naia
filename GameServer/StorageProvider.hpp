#ifndef STORAGE_PROVIDER_HPP_INCLUDED
#define STORAGE_PROVIDER_HPP_INCLUDED

typedef boost::function <void(Session *, string)> command_handler;

struct StorageProvider : public Singleton<StorageProvider>
{
	friend class Singleton<StorageProvider>;
	typedef unordered_map<string, command_handler> CommandsStorage;
	// -- INITIALIZATION
	StorageProvider ();

	// -- STORED DATA
	CommandsStorage admin_commands;
	CommandsStorage user_commands;
};

#define storage_provider StorageProvider::instance()

#endif // STORAGE_PROVIDER_HPP_INCLUDED
