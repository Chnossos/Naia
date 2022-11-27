CREATE TABLE IF NOT EXISTS `accounts` (
	`ID` int(11) NOT NULL AUTO_INCREMENT,
	`Name` text NOT NULL,
	`Password` text NOT NULL,
	`Nickname` text NOT NULL,
	`Secret_Question` text NOT NULL,
	`Secret_Answer` text NOT NULL,
	`Access_Level` int(3) NOT NULL DEFAULT 0,
	`Ticket` varchar(15) NOT NULL,
	`IP_Net` varchar(15) NOT NULL,
	`IP_Loc` varchar(15) NOT NULL,
	`Email` text NOT NULL,
	`Subscribe_Time` bigint(18) NOT NULL DEFAULT '0',
	`Is_Online` tinyint(3) NOT NULL DEFAULT '0',
	`Last_Connection_Date` int UNSIGNED NOT NULL DEFAULT '0',
	`Available_Channels` VARCHAR(15) NOT NULL DEFAULT 'i*?:!',
	`Mute_End_Date` bigint(18) NOT NULL DEFAULT '0',
	PRIMARY KEY (`ID`)
);

INSERT INTO `accounts` VALUE (
	'1', 
	'Admin', 
	'73acd9a5972130b75066c82595a1fae3', 
	'Admin', 
	'DELETE?', 
	'DELETE',
	'1', 
	'', 
	'', 
	'', 
	'', 
	'1356217992',
	DEFAULT,
	DEFAULT,
	DEFAULT,
	DEFAULT
);