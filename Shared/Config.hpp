#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED

namespace Naia {

class Config
{
	public:
		Config (const std::string & file_name = "", const std::string & path = PATH_CONFIG);
		Config (const std::vector <std::string> & files_name, const std::string & path = PATH_CONFIG);

		bool parse (const std::string & file_name);
		bool parse (const std::vector <std::string> & files_name);

		template <typename T> T get_property (const std::string & name, const T & default_value);

		template <typename T> T get_property (const std::string & file_name, const std::string & name, const T & default_value);

		const std::vector <std::string> & unparsed () const { return _unparsed; }

	private:
		std::string _path;
		std::vector <std::string> _unparsed;
		std::unordered_map <std::string, std::string> _properties;
};

template <typename T> inline T Config::get_property (const std::string & name, const T & default_value)
{
	std::unordered_map <std::string, std::string>::const_iterator it = _properties.find (name);
	if (it == _properties.end())
		return default_value;

	return from_string <T> (it->second);
}

template <> inline std::string Config::get_property <std::string> (const std::string & name, const std::string & default_value)
{
	std::unordered_map<string, string>::const_iterator it = _properties.find (name);
	if (it == _properties.end())
		return default_value;

	return it->second;
}

template <> inline bool Config::get_property <bool> (const std::string & name, const bool & default_value)
{
	std::unordered_map <std::string, std::string>::const_iterator it = _properties.find (name);
	if (it == _properties.end())
		return default_value;

	return Naia::lower_case (it->second) == "true" ? true : false;
}

template <typename T> inline T Config::get_property (const std::string & file_name, const std::string & name, const T & default_value)
{
	string fname = _path + file_name;
	std::ifstream file (fname.c_str());
	if (!file)
	{
		cerr << "Unable to open file " << file_name << endl;
		return default_value;
	}

	std::string line, property, value;
	std::vector <std::string> table;

	while (std::getline (file, line))
	{
		if (line.empty() || line[0] == '#' || line.find ('=') == std::string::npos)
			continue;

		table        = Naia::split (line, '=');
		property     = table[0];
		value        = table.size() < 1 ? "" : table[1];

		boost::trim (property);
		boost::trim (value);

		if (name == property)
			return from_string<T> (value);
	}
	return default_value;
}

} // namespace Naia

#endif // CONFIG_HPP_INCLUDED
