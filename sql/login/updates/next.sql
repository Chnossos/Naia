ALTER TABLE `accounts` 
	ADD COLUMN `Active_Channels` VARCHAR(15) NOT NULL DEFAULT 'i*#$p%!?:^',
	ADD COLUMN `Mute_End_Date` bigint(18) NOT NULL DEFAULT '0';