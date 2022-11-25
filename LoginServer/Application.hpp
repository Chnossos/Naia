#ifndef SUPER_HPP_INCLUDED
#define SUPER_HPP_INCLUDED

class GameServer;
class Session;

namespace Naia {

extern Database * sDatabase;

class Application : public Singleton<Application>
{
	friend class Singleton<Application>;
	public:
		bool run ();
		void stop () { _io_service->stop(); }
		void display_uptime (const system::error_code & error);

		void check_config ();
		bool start_up_database ();

	private:
		Application ();
		~Application ();

		io_ptr _io_service;
		signal_set _signals;
		deadline_timer timer;
		unsigned start_time;
};

} // namespace Naia

#endif // SUPER_HPP_INCLUDED
