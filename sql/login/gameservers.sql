CREATE TABLE IF NOT EXISTS `gameservers` (
  `ID` smallint(4) unsigned NOT NULL default '0',
  `Auth_key` varchar(50) NOT NULL default '',
  PRIMARY KEY (`ID`)
);