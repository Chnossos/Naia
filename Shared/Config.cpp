#include "_Common.hpp"

Naia::Config::Config (const std::string & file_name, const std::string & path) : _path (path)
{
	if (!file_name.empty())
		parse (file_name);
}

Naia::Config::Config (const std::vector <std::string> & files_name, const std::string & path) : _path (path)
{
	for (size_t i = 0 ; i < files_name.size() ; ++i)
		parse (files_name[i]);
}

bool Naia::Config::parse (const std::vector <std::string> & files_name)
{
	for (size_t i = 0 ; i < files_name.size() ; ++i)
		parse (files_name[i]);
	return _unparsed.empty();
}

bool Naia::Config::parse (const std::string & file_name)
{
	std::string name = _path + file_name;
	std::ifstream file (name.c_str());
	if (!file)
	{
		_unparsed.push_back (name);
		return false;
	}

	std::string line, property, value;
	std::vector <std::string> table;

	while (std::getline (file, line))
	{
		/// Si la ligne est vide, commence par un '#' ou n'a pas de signe '=' on ignore
		if (line.empty() || line[0] == '#' || line.find ('=') == std::string::npos)
			continue;

		table        = Naia::split (line, '=');
		property     = table[0];
		value        = table.size() < 1 ? "" : table[1];

		boost::trim (property);
		boost::trim (value);

		//cout << property << " : " << value << endl;
		_properties [property] = value;

		table.clear();
		property.clear();
		value.clear();
	}
	return true;
}
