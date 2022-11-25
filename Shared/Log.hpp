#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#ifdef _WIN32

#define TRED    FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN  FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE  TNORMAL | FOREGROUND_INTENSITY
#define TBLUE   FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TPURPLE FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY

#else

#define TRED    1
#define TGREEN  2
#define TYELLOW 3
#define TNORMAL 4
#define TWHITE  5
#define TBLUE   6
#define TPURPLE 7

#endif

enum AuditType
{
	AUDIT_ERRORS = 0,
	AUDIT_DEBUG,
	AUDIT_GMSHOP,
	AUDIT_CONNECTION,
	AUDIT_COUNT,
};

struct Audit
{
	string _path;
	bool _active;
};

class Log
{
	public:
		Log ();
		void initialize (int, const string & log_path = PATH_LOGS, const string & config_path = PATH_CONFIG);
		void print (int color, bool display_time, const char * str, ...);
		void print_header (int color, bool display_time, const char * header, const char * str, ...);
		void print_debug (const char * str, ...);
		void print_error (const char * str, ...);
		void print_warning (const char * str, ...);
		void print_separator (const char * separator);
		void log (int type, const char * str, ...);
		void log (int type, const string & file_name, const char * str, ...);

	private:
		void print_time ();
		void print_time (const string & file_name);

		string _log_path;
		Audit _audit[AUDIT_COUNT];
		HANDLE _stderr_handle, _stdout_handle;
		boost::mutex _mutex;
};

#endif // LOG_HPP_INCLUDED
