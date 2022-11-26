CREATE TABLE IF NOT EXISTS `char_relations` (
	`CharID` int(10) unsigned NOT NULL,
	`OtherID` int(10) unsigned NOT NULL,
	`Relation` tinyint(1) unsigned NOT NULL default '1',
	PRIMARY KEY (`charID`, `otherID`)
);