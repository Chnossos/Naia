#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED

enum XPType
{
	XP_CHARACTER = 0,
	XP_JOB,
	XP_TURKEY,
	XP_GUILD,
	XP_LIVITINEM,
	XP_INCARNATION,
	XP_BANDIT,
	XP_HONOR,
	XP_COUNT
};

class LevelManager : public Singleton<LevelManager>
{
	friend class Singleton<LevelManager>;
	public:
		bool load ();
		uint64_t get_xp (XPType type, unsigned level) const;
		unsigned get_level (XPType type, uint64_t xp) const;
		size_t get_max_level (XPType type) const { return _container[type].size(); }
	private:
		LevelManager () {}
		map<unsigned, uint64_t> _container[XP_COUNT]; // <niveau, xp_min>
};

class Level
{
	public:
		Level (XPType type, unsigned level, uint64_t xp);
		Level (XPType type, unsigned level) : _type (type), _level (level), _xp (LevelManager::instance().get_xp (type, level)) {}

	// GETTERS
		unsigned level () const 		{ return _level; }
		uint64_t xp () const 	{ return _xp; }

	// SETTERS
		void set_level (XPType type, unsigned level);

	// EVENTS
		void add_xp (int64_t xp);
		virtual void on_level_up (int count) = 0;
		virtual void on_level_down (int count) = 0;

	protected:
		XPType _type;
		unsigned _level;
		uint64_t _xp;
};

#endif // LEVEL_HPP_INCLUDED
