#ifndef THREADPOOL_HPP_INCLUDED
#define THREADPOOL_HPP_INCLUDED

template <typename T> class Connector;

class ThreadPool
{
	typedef boost::function<void()> Callback;
	typedef boost::shared_ptr<deadline_timer> timer_ptr;

	public:
	// INITIALIZATION
		ThreadPool () :
			_io_service (new boost::asio::io_service),
			_work (new io_service::work (*_io_service)),
			_running (true) {}

		~ThreadPool ();
		void run ();
		void shutdown ();

		void schedule (const Callback & task);
		//void schedule (const string & thread, const Callback & task);
		void schedule_general (const Callback & callback, const deadline_timer::duration_type & initial);
		void schedule_general_time (const Callback & callback, const string & time);
		Callback schedule_repeat (const Callback & callback, const deadline_timer::duration_type & delay);
		Callback schedule_repeat_time (const Callback & callback, const string & time);
		void cancel_timer (timer_ptr timer, Callback * callback);

		void wait ();
		void notify ();

		io_ptr io_service () { return _io_service; }
		bool running () const { return _running; }

	private:
		void handle_wait (timer_ptr timer, const deadline_timer::duration_type & duration, const system::error_code & error, Callback * callback);

		unordered_map<string, thread_ptr> 	_threads;
		io_ptr 				_io_service;
		work_ptr 			_work;
		bool 				_running;
};

#endif // THREADPOOL_HPP_INCLUDED
