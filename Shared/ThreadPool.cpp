#include "_Common.hpp"

#define USER_THREAD "user"

void basic_worker (const io_ptr & io_service)
{
	io_service->run();
}

ThreadPool::~ThreadPool ()
{
	for (unordered_map<string, thread_ptr>::iterator it = _threads.begin() ; it != _threads.end() ; ++it)
	// Wait for all threads in the pool to exit.
	for (size_t i = 0; i < _threads.size(); ++i)
		it->second->join();
}

void ThreadPool::shutdown ()
{
	_running = false;
	_io_service->stop();
}

void ThreadPool::run ()
{
	_threads[USER_THREAD] = thread_ptr (new boost::thread (boost::bind (&basic_worker, _io_service)));
}

void ThreadPool::schedule (const Callback & task)
{
	if (!_running) return;
	_io_service->post (task);
}

void ThreadPool::schedule_general (const Callback & callback, const deadline_timer::duration_type & initial)
{
	if (!_running) return;
	timer_ptr timer (new deadline_timer (*_io_service, initial));
	timer->async_wait (boost::bind (&ThreadPool::handle_wait, this, timer, deadline_timer::duration_type(), boost::asio::placeholders::error, new Callback (callback)));
}

ThreadPool::Callback ThreadPool::schedule_repeat (const Callback & callback, const deadline_timer::duration_type & delay)
{
	if (!_running) return ThreadPool::Callback();
	timer_ptr timer (new deadline_timer (*_io_service, delay));
	Callback * pCallback = new Callback (callback); // create a separate callback on the heap to avoid shutdown crashes
	timer->async_wait (boost::bind (&ThreadPool::handle_wait, this, timer, delay, boost::asio::placeholders::error, pCallback));
	return boost::bind (&ThreadPool::cancel_timer, this, timer, pCallback);
}

void ThreadPool::schedule_general_time (const Callback & callback, const string & time)
{
	schedule_general (callback, posix_time::duration_from_string (time));
}

ThreadPool::Callback ThreadPool::schedule_repeat_time (const Callback & callback, const string & time)
{
	return schedule_repeat (callback, posix_time::duration_from_string (time));
}

void ThreadPool::handle_wait (timer_ptr timer, const deadline_timer::duration_type & duration, const system::error_code & error, Callback * callback)
{
	bool run_once = duration.ticks();
	if (!error)
	{
		if (run_once)
		{
			timer->expires_at (timer->expires_at() + duration);
			timer->async_wait (boost::bind (&ThreadPool::handle_wait, this, timer, duration, boost::asio::placeholders::error, callback));
		}
		schedule (*callback);
	}

	if (!run_once)
		delete callback;
}

void ThreadPool::cancel_timer (timer_ptr timer, Callback * callback)
{
	if (timer.get())
	{
		system::error_code ec;
		timer->cancel (ec);
	}
	delete callback;
}
