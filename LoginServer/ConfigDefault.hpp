#ifndef CONFIGDEFAULT_H_INCLUDED
#define CONFIGDEFAULT_H_INCLUDED

/* ************
 * FILES NAME *
 * ***********/

#define CONFIG_FILE_NETWORK		"network.properties"
#define CONFIG_FILE_SERVER		"server.properties"

/* ********************
 * NETWORK PROPERTIES *
 * *******************/

#define GAME_PORT_PARAM				"GamePort"
#define GAME_PORT					444

#define USER_PORT_PARAM				"UserPort"
#define USER_PORT					443

/* *********************
 * DATABASE PROPERTIES *
 * ********************/

#define DB_HOST_PARAM				"DatabaseHost"
#define DB_HOST						"127.0.0.1"

#define DB_NAME_PARAM				"DatabaseName"
#define DB_NAME						"naia"

#define DB_USER_PARAM				"UserName"
#define DB_USER						"root"

#define DB_PASS_PARAM				"DatabasePassword"
#define DB_PASS						""

#define DB_PORT_PARAM				"DatabasePort"
#define DB_PORT						3306

/* *******************
 * SERVER PROPERTIES *
 * ******************/

#define IGNORE_VERSION_PARAM				"IgnoreVersion"
#define IGNORE_VERSION						false

#define CLIENT_VERSION_PARAM				"ClientVersion"
#define CLIENT_VERSION						"1.29.1"

#define MULTI_ACCOUNTS_PARAM				"MultiAccounts"
#define MULTI_ACCOUNTS						true

#define RETAIL_SUBSCRIPTION_POLICY_PARAM 	"RetailSubscriptionPolicy"
#define RETAIL_SUBSCRIPTION_POLICY 			true

#endif // CONFIGDEFAULT_H_INCLUDED
