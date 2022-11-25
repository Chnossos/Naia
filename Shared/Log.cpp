#include "_Common.hpp"

#ifndef _WIN32
static const char * colorstrings[] = {
"",
"\033[22;31m",
"\033[22;32m",
"\033[01;33m",
//"\033[22;37m",
"\033[0m",
"\033[01;37m",
"\033[22;34m"
};
#endif

Log::Log () :
_stderr_handle (GetStdHandle (STD_ERROR_HANDLE)),
_stdout_handle (GetStdHandle (STD_OUTPUT_HANDLE))
{
	for (int i = 0 ; i < AUDIT_COUNT ; ++i)
		_audit[i]._active = false;
}

void Log::initialize (int target, const string & log_path, const string & config_path)
{
	_log_path = log_path;
	Naia::Config config ("logs.properties");

	switch (target)
	{
		case GAME:
			_audit[AUDIT_ERRORS]._active 		= config.get_property<bool> (LOG_ERROR_PARAM, LOG_ERROR);
			_audit[AUDIT_ERRORS]._path			= "errors/";

			_audit[AUDIT_DEBUG]._active 		= config.get_property<bool> (LOG_DEBUG_PARAM, LOG_DEBUG);

			_audit[AUDIT_GMSHOP]._active 		= config.get_property<bool> (LOG_GMSHOP_PARAM, LOG_GMSHOP);
			_audit[AUDIT_GMSHOP]._path			= "gmshop/";

			_audit[AUDIT_CONNECTION]._active 	= config.get_property<bool> (LOG_ERROR_PARAM, LOG_ERROR);
			_audit[AUDIT_CONNECTION]._path		= "connections/";
		break;

		case LOGIN:
			_audit[AUDIT_ERRORS]._active 		= config.get_property<bool> (LOG_ERROR_PARAM, LOG_ERROR);
			_audit[AUDIT_ERRORS]._path			= "errors/";

			_audit[AUDIT_DEBUG]._active			= config.get_property<bool> (LOG_DEBUG_PARAM, LOG_DEBUG);

			_audit[AUDIT_GMSHOP]._active 		= false;

			_audit[AUDIT_CONNECTION]._active 	= config.get_property<bool> (LOG_ERROR_PARAM, LOG_ERROR);
			_audit[AUDIT_CONNECTION]._path		= "connections/";
		break;
	}
}

void Log::print_time ()
{
	time_t rawtime;
	struct tm * time_info;
	char buf[80];

	time (&rawtime);
	time_info = localtime (&rawtime);
	strftime (buf, 80, "[%H:%M] ", time_info);
    cout << buf;
}

void Log::print_time (const string & file_name)
{
	time_t rawtime;
	struct tm * time_info;
	char buf[80];

	time (&rawtime);
	time_info = localtime (&rawtime);
	strftime (buf, 80, "[%H:%M]", time_info);

    ofstream file (file_name.c_str(), ios::app); // Fin de fichier
    if (!file || file.eof())
        return;

	file.tellp() == 0 ? (file << buf) : (file << '\n' << buf);

    file.close();
}

void Log::print (int color, bool display_time, const char * str, ...)
{
	boost::mutex::scoped_lock lock (_mutex);
	if (display_time) print_time();

	va_list ap;
	va_start (ap, str);
#ifdef _WIN32
	SetConsoleTextAttribute (_stdout_handle, color);
#else
	cout << colorstrings[color];
#endif
	vprintf (str, ap);
	cout << flush;
	va_end (ap);
#ifdef _WIN32
	SetConsoleTextAttribute (_stdout_handle, TNORMAL);
#else
	cout << colorstrings[TNORMAL];
#endif
}

void Log::print_header (int color, bool display_time, const char * header, const char * str, ...)
{
	boost::mutex::scoped_lock lock (_mutex);
	if (display_time) print_time();

	va_list ap;
	va_start (ap, str);
#ifdef _WIN32
	SetConsoleTextAttribute (_stdout_handle, color);
#else
	cout << colorstrings[color];
#endif
	cout << "\b" << header << " ";
#ifdef _WIN32
	SetConsoleTextAttribute (_stdout_handle, TNORMAL);
#else
	cout << colorstrings[TNORMAL];
#endif
	vprintf (str, ap);
	cout << flush;
	va_end (ap);
}

void Log::print_debug (const char * str, ...)
{
	boost::mutex::scoped_lock lock (_mutex);
	if (_audit[AUDIT_DEBUG]._active)
	{
		print_time();

		va_list ap;
		va_start (ap, str);
	#ifdef _WIN32
		SetConsoleTextAttribute (_stdout_handle, TPURPLE);
	#else
		cout << colorstrings[TPURPLE];
	#endif
		cout << "\b[DEBUG] ";
	#ifdef _WIN32
		SetConsoleTextAttribute (_stdout_handle, TNORMAL);
	#else
		cout << colorstrings[TNORMAL];
	#endif
		vprintf (str, ap);
		cout << flush;
		va_end (ap);
	}
}

void Log::print_error (const char * str, ...)
{
	_mutex.lock();
	if (!str)
		return;

	char buffer[32768];
	va_list ap;
	va_start (ap, str);
	vsnprintf (buffer, 32768, str, ap);
	va_end (ap);

	print_time();

#ifdef _WIN32
	SetConsoleTextAttribute (_stderr_handle, TRED);
#else
	cerr << colorstrings[TRED];
#endif
	cerr << "\b[ERROR] ";
#ifdef _WIN32
	SetConsoleTextAttribute (_stderr_handle, TNORMAL);
#else
	cerr << colorstrings[TNORMAL];
#endif
	cerr << buffer;
	cerr << flush;

	_mutex.unlock();
	Log::log (AUDIT_ERRORS, buffer);
}

void Log::print_warning (const char * str, ...)
{
	boost::mutex::scoped_lock lock (_mutex);
	print_time();

	va_list ap;
	va_start (ap, str);
#ifdef _WIN32
	SetConsoleTextAttribute (_stderr_handle, TRED);
#else
	cout << colorstrings[TRED];
#endif
	cout << "\b[WARNING] ";
#ifdef _WIN32
	SetConsoleTextAttribute (_stderr_handle, TNORMAL);
#else
	cout << colorstrings[TNORMAL];
#endif
	vprintf (str, ap);
	cout << flush;
	va_end (ap);
}

void Log::print_separator (const char * separator)
{
	boost::mutex::scoped_lock lock (_mutex);
	cout << endl;
	char header[] = "=[ ]=----";
	int nb_tirets = (77 - (strlen (header) - 1) - (strlen (separator) - 1));
	for (int i = 0 ; i < nb_tirets ; ++i)
		cout << "-";
	cout << "=[ " << Naia::upper_case (separator) << " ]=----" << endl;
}

void Log::log (int type, const char * str, ...)
{
	boost::mutex::scoped_lock lock (_mutex);
	if (!_audit[type]._active) return;

	char buffer[1000];
	va_list ap;
	va_start (ap, str);
	vsnprintf (buffer, 1000, str, ap);
	va_end (ap);

	time_t rawtime;
	struct tm * time_info;
	char buf[80];

	time (&rawtime);
	time_info = localtime (&rawtime);
	strftime (buf, 80, "%Y-%m-%d", time_info);

	string file_name = _log_path + _audit[type]._path + buf + ".log"; // /logs/type/Year-Month-Day.log
	print_time (file_name);

	ofstream file (file_name.c_str(), ios::app);
	if (!file || file.eof())
		return;

	file << " " << buffer;

	string temp = buffer;
	reverse (temp.begin(), temp.end());
	if (temp[0] != '\n')
		file << '\n';
	file.close();
}

void Log::log (int type, const string & file_name, const char * str, ...)
{
	if (!str) return;
	boost::mutex::scoped_lock lock (_mutex);
	if (!_audit[type]._active) return;

	char buffer[500];
	va_list ap;
	va_start (ap, str);
	vsnprintf (buffer, 500, str, ap);
	va_end (ap);

	string log_file_name = _log_path + _audit[type]._path + file_name + ".log";
	if (type != AUDIT_CONNECTION)
		print_time (log_file_name);
	else
	{
		time_t rawtime;
		struct tm * time_info;
		char buf[80];

		time (&rawtime);
		time_info = localtime (&rawtime);
		strftime (buf, 80, "[%x - %H:%M]", time_info);

		ofstream file (log_file_name.c_str(), ios::app); // Fin de fichier
		if (!file || file.eof())
			return;

		file.tellp() == 0 ? (file << buf) : (file << '\n' << buf);
	}

	ofstream file (log_file_name.c_str(), ios::app);
	if (!file || file.eof())
		return;

	file << " " << buffer;

	string temp = buffer;
	if (temp[temp.length() - 1] != '\n')
		file << '\n';
}
