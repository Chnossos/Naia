#ifndef CELL_HPP_INCLUDED
#define CELL_HPP_INCLUDED

#define UNDEFINED_CELLID 50000

struct Cell
{
	int _ID;
	bool _walkable, _los;
};

struct IOTemplate
{
	int _type;
	int _respawn_time;
	int _duration;
	int _unknown;
	bool _walkable;
};

struct InteractiveObject // objet interactif basique, � revoir quand j'aurai enti�rement compris le fonctionnement
{
	Naia::IOState _state;
	IOTemplate _template;
};

#endif // CELL_HPP_INCLUDED
