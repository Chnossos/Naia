#ifndef TIMER_HPP_INCLUDED
#define TIMER_HPP_INCLUDED

#ifdef _WIN32
inline unsigned get_ms_time () { return GetTickCount(); }
#else
inline unsigned get_ms_time ()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

#endif // TIMER_HPP_INCLUDED
