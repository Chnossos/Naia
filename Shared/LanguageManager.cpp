#include "_Common.hpp"

initialize_singleton (LanguageManager);

LanguageManager::LanguageManager()
{
	string line, file_name = PATH_LANG + kernel.config.get_property <string> (CONFIG_FILE_LOGS, LANGUAGE_PARAM, LANGUAGE) + string (".lang");
	ifstream lang_file (file_name.c_str());
	if (!lang_file)
	{
		cerr << "Fail opening " << file_name << endl;
		return;
	}
	int count = 1;
	while (getline (lang_file, line))
	{
		if (line[0] == '0')
			kernel.log.print (TBLUE, true, "%s", Naia::split (line, '\t')[1].c_str());
		++count;
	}
	ProgressBar bar (count);
	vector<string> table;
	while (getline (lang_file, line))
	{
		table = Naia::split (line, '\t');
		_msg[Naia::from_string<int>(table[0])] = table[1];
		bar.step();
	}
}

string LanguageManager::get_message (int msgID)
{
	std::unordered_map<int, string>::const_iterator it = _msg.find (msgID);
	if (it == _msg.end())
		return string();

	return it->second;
}

const char * gettext (int msgID, ...)
{
	string str = LanguageManager::instance().get_message (msgID);

	if (str.empty())
	{
		kernel.log.print_error ("Missing message translation ! [requested ID: %u]\n", msgID);
		return "";
	}

	vector <string> table = Naia::split (str, '%');

	if (table.size() == 1)
		return str.c_str();

	tr1::unordered_map<int, string> args;

	va_list va;
	va_start (va, msgID);

	for (size_t i = 1 ; i < table.size() ; ++i)
		args[i] = va_arg (va, char *);

	va_end (va);

	size_t pos = str.find ('%');
	while (pos != string::npos)
	{
		if (pos + 2 > str.size())
			break;
		else if (!isdigit (str[pos + 1]))
		{
			pos = str.substr(pos + 1).find ('%');
			continue;
		}
		str.replace (
			pos,
			args.size() >= 10 && pos + 3 < str.size() && isdigit (str[pos + 2])
				? 3
				: 2,
			args[Naia::from_string <int> (str.substr (pos + 1, pos + 2 <= str.size() ? 2 : 1))]
		);
		pos = str.find ('%');
	}

	return str.c_str();
}
