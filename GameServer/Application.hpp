#ifndef SUPER_HPP_INCLUDED
#define SUPER_HPP_INCLUDED

#define TIMER 30

class GameServer;
class Session;

namespace Naia {

enum AppState
{
	APP_NORMAL,
	APP_RESTART,
	APP_ERROR,
};

extern Database * lsDatabase;
extern Database * gsDatabase;
extern Connector<Session> * uListener;

class Application : public Singleton<Application>
{
	friend class Singleton<Application>;
	public:
		bool run ();
		void stop () { _io_service->stop(); }
		void exit (const AppState & good);
		void display_uptime (const system::error_code & error);

		AppState state () const { return _state; }

		void check_config ();
		bool start_up_database ();

	private:
		Application ();
		~Application ();

		AppState _state;

		io_ptr _io_service;
		signal_set _signals;
		deadline_timer timer;

		unsigned start_time;
};

inline void Application::exit (const AppState & state)
{
	_state = state;
	raise (SIGINT);
}

} // namespace Naia

#endif // SUPER_HPP_INCLUDED
