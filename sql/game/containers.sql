CREATE TABLE IF NOT EXISTS `containers` (
	`ID` smallint(4) unsigned NOT NULL,
	`ItemID` int(10) unsigned NOT NULL,
	`Amount` smallint(4) unsigned NOT NULL,
	PRIMARY KEY (`ID`)
);