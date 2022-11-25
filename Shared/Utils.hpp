#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

namespace Naia
{
	const char HASH[] = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
		'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
	};

	string lower_case (const string & str);
	string upper_case (const string & str);

	vector<string> split (const string & str, char delim);

    void set_application_title (string title);

	bool check_name (const string & name);

    bool string_to_bool (const string & str);

	string string_to_hex (const string & str);

	string hex_to_string (const string & str);

	string create_ticket ();
	string decrypt_pass (string, string);


	template <typename T> T from_string (const string & str, bool hex_convert = false)
	{
		istringstream iss (str);
		T t;

		hex_convert ? (iss >> hex >> t) : (iss >> t);
		return t;
	}

	template <typename T> string to_string (const T & t)
	{
		ostringstream oss;
		oss << t;
		oss.flush();
		return oss.str ();
	}

	template <typename T> string to_hex_string (const T & t, bool show_base = false)
	{
		ostringstream oss;
		oss << hex;
		show_base ? (oss << showbase << t) : (oss << t);
		return oss.str ();
	}

} // namespace Naia

#endif // UTILS_HPP_INCLUDED
