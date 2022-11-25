#include "_Common.hpp"

string Naia::lower_case (const string & str)
{
	string buffer;
	buffer.reserve (str.size());
	for (size_t i = 0 ; i < str.size() ; ++i)
	{
		buffer += tolower (str[i]);
	}
	return buffer;
}

string Naia::upper_case (const string & str)
{
	string buffer;
	buffer.reserve (str.size());
	for (size_t i = 0 ; i < str.size() ; ++i)
	{
		buffer += toupper (str[i]);
	}
	return buffer;
}

vector<string> Naia::split (const string & str, char delim)
{
	vector<string> table;
	istringstream iss (str);
	string buffer;
	table.reserve (str.size());
	while (getline (iss, buffer, delim))
		table.push_back (buffer);
	return table;
}

void Naia::set_application_title (string title)
{
#ifdef _WIN32
	title = "TITLE " + title;
	std::system (title.c_str ());
#else
	printf ("\033]0;%s\007\n", title.c_str ());
#endif
}

bool Naia::check_name (const string & name)
{
	if (name.size () < 2 || name.size () >= 20) // Si le nom ne fait pas entre 3 et 20 caractères inclus
		return false;

	for (size_t i = 2 ; i < name.size() ; ++i)
	{
		if (name[i] == name[i-1] && name[i] == name[i-2]) // Si 3 mêmes caractères de suite
			return false;
	}

	{
		string authorized_char = "abcdefghijklmnopqrstuvwxyz-";

		for (size_t i = 0 ; i < name.size() ; ++i) // Vérifie si les tous les caractères sont acceptables
		{
			if (authorized_char.find (tolower(name[i])) == string::npos)
				return false;
		}
	}

	{
		int count = 0;

		for (size_t i = 0 ; i < name.size() ; ++i) // Vérifie le nombre de tiret
		{
			if (name[i] == '-')
				++count;
		}
		if (count > 1) // Si plus d'un tiret
			return false;
	}

	if (name[0] == '-' || name[1] == '-') // Si le tiret se situe en 1ère ou 2ème place
		return false;

	if (name[0] != toupper (name[0])) // Si le premier caractère n'est pas une majuscule
		return false;

	for (size_t i = 0 ; i < name.size () ; ++i)
	{
		if (name[i] != '-' && name[i] == toupper (name[i]) && i > 0 && (name[i - 1] != '-' || i == name.size () - 1))
			// Si le caractère est une majuscule et qu'il n'est pas en 1ere place
			// Si le caractère juste avant n'est pas un tiret ou que c'est le dernier
			return false;
	}

	{
		string prohibed_name [] = {"XELOR", "IOP", "FECA", "ENIRIPSA", "SADIDA", "ECAFLIP", "ENUTROF", "PANDAWA", "SRAM", "CRA", "OSAMODAS", "SACRIEUR", "DROP", "MULE"};
		for (size_t i = 0 ; i < 14 ; ++i) // Si le pseudo appartient à l'un des interdits ...
		{
			if (!upper_case (name).compare (prohibed_name[i]))
				return false;
		}

		string custom_file = PATH_CONFIG + string ("banned_names.txt");
		ifstream file (custom_file.c_str());
		if (file)
		{
			vector<string> custom_name;
			string line, upper_name = upper_case (name);
			while (getline (file, line))
			{
				if (line.empty() || line.find ('#') != string::npos)
					continue;
				if (upper_name.find (line) != string::npos)
					return false;
			}
		}
		else
			cerr << "Impossible d'ouvrir le fichier " << custom_file << "." << endl;
	}

	string vowels = "aeiouy";
	int count = 0;
	for (size_t i = 0 ; i < name.size () ; ++i) // Compte le nombre de voyelle
	{
		if (vowels.find (name[i]) != string::npos)
			++count;
	}
	if (count < 1) // S'il n'y a pas de voyelle
		return false;

	return true;
}

bool Naia::string_to_bool (const string & str)
{
	return (lower_case (str) == "true") ? true : false;
}

string Naia::string_to_hex (const string & str)
{
	string result;
	char buf[5];
	for (size_t i = 0 ; i < str.size () ; ++i)
	{
		snprintf (buf, 5, "%02X", str[i]);
		result += string (buf);
	}
	return result;
}

string Naia::hex_to_string (const string & str)
{
	if (str.size() % 2 != 0)
		return "";

	string result/*, sub*/;
	int current;
	for (size_t i = 0 ; i < str.size() ; i += 2)
	{
		//sub = str.substr (i, 2);
		current = from_string<int> (str.substr (i, 2), true);
		result += current;
	}
	return result;
}

string Naia::create_ticket ()
{
	string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
	string ticket;
	for (short i = 0 ; i < 7 ; ++i)
		ticket += chars.at (kernel.random.random (chars.size() - 1));
	return ticket;
}

string Naia::decrypt_pass (string str, string hashkey)
{
	str = str.substr (2);
	string pass, chaine = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
	char pkey;
	int apass, akey, somme1, somme2;
	unsigned csize = chaine.size();

	for (unsigned a = 0 ; a < str.size() ; a += 2)
	{
		pkey = hashkey[a / 2];
		somme1 = csize + chaine.find (str[a]);
		somme2 = csize + chaine.find (str[a + 1]);
		apass = somme1 - (int)pkey;
		if (apass < 0)
			apass += 64;
		apass *= 16;
		akey = somme2 - (int)pkey;
		if (akey < 0)
			akey += 64;
		pass += (char)(apass + akey);
	}
	return pass;
}
