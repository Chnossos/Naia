#ifndef UTIL_HPP
#define UTIL_HPP

#include <sstream>
#include <string>

template <typename T> T from_string (const std::string & str, bool hex_convert = false)
{
	std::istringstream iss (str);
	T t;

	hex_convert ? (iss >> std::hex >> t) : (iss >> t);
	return t;
}

template <typename T> std::string to_string (const T & t)
{
	std::ostringstream oss;
	oss << t;
	oss.flush();
	return oss.str ();
}

#endif // UTIL_HPP
