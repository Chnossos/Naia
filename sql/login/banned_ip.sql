CREATE TABLE IF NOT EXISTS `banned_ip` (
	`IP` varchar(20) NOT NULL,
	`Duration` bigint unsigned NOT NULL default '0',
	PRIMARY KEY (`IP`)
);