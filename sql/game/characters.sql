CREATE TABLE IF NOT EXISTS characters (
	`ID` int unsigned NOT NULL,
	`AccountID` int unsigned NOT NULL,
	`Access_Level` tinyint unsigned NOT NULL default '0',
	`Name` varchar(20) NOT NULL,
	`Class` tinyint(2) unsigned NOT NULL,
	`Sex` enum('0', '1') NOT NULL,
	`color1` int NOT NULL default '-1',
	`color2` int NOT NULL default '-1',
	`color3` int NOT NULL default '-1',
	`Size` smallint(3) unsigned NOT NULL default '100',
	`GFX` smallint unsigned NOT NULL,
	`XP` bigint unsigned NOT NULL default '0',
	`Kamas` int unsigned NOT NULL default '0',
	`Energy` smallint(5) unsigned NOT NULL default '10000',
	`Capital` smallint(5) unsigned NOT NULL default '0',
	`Spell_Boosts` smallint(4) unsigned NOT NULL default '0',
	`Damages` int unsigned NOT NULL default '0',
	`Vitality` smallint(5) unsigned NOT NULL default '0',
	`Wisdom` smallint(3) unsigned NOT NULL default '0',
	`Strength` smallint(4) unsigned NOT NULL default '0',
	`Intelligence` smallint(4) unsigned NOT NULL default '0',
	`Chance` smallint(4) unsigned NOT NULL default '0',
	`Agility` smallint(4) unsigned NOT NULL default '0',
	`Map` smallint(5) unsigned NOT NULL,
	`Cell` smallint(3) unsigned NOT NULL,
	`Saved_pos` varchar(15) NOT NULL,
	`Zaaps` text NOT NULL,
	`Spells` text NOT NULL,
	`Align_Side` enum('0', '1', '2', '3') NOT NULL default '0',
	`Align_Level` tinyint(2) unsigned NOT NULL default '0',
	`Wings_enabled` enum('true', 'false') NOT NULL default 'false',
	`Honor` smallint(5) unsigned NOT NULL default '0',
	`Disgrace` smallint(5) unsigned NOT NULL default '0',
	`Emotes` text NOT NULL,
	`Jobs` text NOT NULL,
	`Title` tinyint unsigned NOT NULL default '0',
	`Is_Merchant` enum('true', 'false') NOT NULL default 'false',
	`Is_Dead` enum('true', 'false') NOT NULL default 'false',
	`See_Offline_Friends` enum('true', 'false') NOT NULL default 'false',
	PRIMARY KEY (`ID`, `AccountID`)
);
	