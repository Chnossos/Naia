#ifndef CONFIGDEFAULT_H_INCLUDED
#define CONFIGDEFAULT_H_INCLUDED

/* ************
 * FILES NAME *
 * ***********/

#define CONFIG_FILE_CHARACTER		"character.properties"
#define CONFIG_FILE_NETWORK			"network.properties"
#define CONFIG_FILE_SERVER			"server.properties"
#define CONFIG_FILE_STAFF 			"staff.properties"

/* ********************
 * NETWORK.PROPERTIES *
 * *******************/

// Game
#define DB_GAME_HOST_PARAM				"GameDatabaseHost"
#define DB_GAME_HOST					"127.0.0.1"

#define DB_GAME_NAME_PARAM				"GameDatabaseName"
#define DB_GAME_NAME					"naia"

#define DB_GAME_USER_PARAM				"GameUserName"
#define DB_GAME_USER					"root"

#define DB_GAME_PASS_PARAM				"GameDatabasePassword"
#define DB_GAME_PASS					""

#define DB_GAME_PORT_PARAM				"GameDatabasePort"
#define DB_GAME_PORT					3306

// Login
#define DB_LOGIN_HOST_PARAM				"LoginDatabaseHost"
#define DB_LOGIN_HOST					"127.0.0.1"

#define DB_LOGIN_NAME_PARAM				"LoginDatabaseName"
#define DB_LOGIN_NAME					"naia"

#define DB_LOGIN_USER_PARAM				"LoginUserName"
#define DB_LOGIN_USER					"root"

#define DB_LOGIN_PASS_PARAM				"LoginDatabasePassword"
#define DB_LOGIN_PASS					""

#define DB_LOGIN_PORT_PARAM				"LoginDatabasePort"
#define DB_LOGIN_PORT					3306

// Network
#define LOGIN_HOST_PARAM				"LoginHost"
#define LOGIN_HOST						"127.0.0.1"

#define LOGIN_PORT_PARAM				"LoginPort"
#define LOGIN_PORT						444

#define USER_PORT_PARAM					"UserPort"
#define USER_PORT						442

#define USER_HOST_PARAM					"RemoteHost"
#define USER_HOST						"127.0.0.1"

/* ********************
 * SERVER.PROPERTIES *
 * *******************/

#define POPULATION_LIMIT_PARAM			"PopulationLimit"
#define POPULATION_LIMIT				500

#define IGNORE_POP_LIMIT_PARAM			"IgnorePopulationLimit"
#define IGNORE_POP_LIMIT				true

#define MAINTENANCE_PARAM				"Maintenance"
#define MAINTENANCE						false

#define MAX_CHARACTERS_COUNT_PARAM		"MaxCharactersCount"
#define MAX_CHARACTERS_COUNT			5

#define MULTI_ACCOUNTS_PARAM			"MultiAccounts"
#define MULTI_ACCOUNTS					true

/* **********************
 * CHARACTER.PROPERTIES *
 * *********************/

// Character statistics
#define MAX_AP_PARAM			"MaxAP"
#define MAX_AP					12

#define MAX_MP_PARAM			"MaxMP"
#define MAX_MP					6

#define MAX_RANGE_PARAM			"MaxRange"
#define MAX_RANGE				9

// Character creation settings
#define PANDAWA_FOR_ALL_PARAM	"PandawaForAll"
#define PANDAWA_FOR_ALL			false

#define START_LEVEL_PARAM		"StartLevel"
#define START_LEVEL				1

#define START_LEVEL_LIMIT_PARAM "StartLevelLimit"
#define START_LEVEL_LIMIT		false

#define START_KAMAS_PARAM		"StartKamas"
#define START_KAMAS				0

#define FULL_SCROLLED_PARAM		"FullScrolled"
#define FULL_SCROLLED			false

#define WEIGHT_MULTIPLIER_PARAM	"WeightMultiplier"
#define WEIGHT_MULTIPLIER		1

#define CUSTOM_START_PARAM		"CustomStart"
#define CUSTOM_START			false

#define CUSTOM_START_MAP_PARAM	"CustomStartMap"
#define CUSTOM_START_MAP		10354

#define CUSTOM_START_CELL_PARAM	"CustomStartCell"
#define CUSTOM_START_CELL		311

/* **************************
 * STAFF MEMBERS PROPERTIES *
 * *************************/

#define GM_STARTUP_INVISIBLE_PARAM 	"GMStartupInvisible"
#define GM_STARTUP_INVISIBLE 		false

#define GM_STARTUP_SILENCE_PARAM 	"GMStartupSilence"
#define GM_STARTUP_SILENCE 			false

#define GM_STARTUP_AUTOLIST_PARAM 	"GMStartupAutoList"
#define GM_STARTUP_AUTOLIST 		false

#define GM_STARTUP_DIET_MODE_PARAM 	"GMStartupDietMode"
#define GM_STARTUP_DIET_MODE 		false

#endif // CONFIGDEFAULT_H_INCLUDED
