CREATE TABLE IF NOT EXISTS `banks` (
	`ID` int(11) NOT NULL,
	`Contents` text NOT NULL,
	`Kamas` int NOT NULL default '0',
	PRIMARY KEY (`ID`)
);