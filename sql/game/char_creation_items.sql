-- Note : Creation kamas' amount has to be set in config/character.properties file

CREATE TABLE IF NOT EXISTS `char_creation_items` (
	`ItemID` smallint(5) NOT NULL,
	`Amount` smallint(4) NOT NULL DEFAULT '1',
	`Equipped` enum('true','false') NOT NULL DEFAULT 'false',
	PRIMARY KEY (`ItemID`)
);