#ifndef LANGUAGEMANAGER_HPP_INCLUDED
#define LANGUAGEMANAGER_HPP_INCLUDED

class LanguageManager : public Singleton<LanguageManager>
{
	friend class Singleton<LanguageManager>;
	public:
		LanguageManager();
		string get_message (int msgID);
		tr1::unordered_map <int, string> messages () { return _msg; }

	private:
		tr1::unordered_map <int, string> _msg;
};

const char * gettext (int msgID, ...);

#endif // LANGUAGEMANAGER_HPP_INCLUDED
