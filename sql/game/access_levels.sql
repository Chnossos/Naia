-- How to configure the SQL based access level system :

-- There are two tables.

-- First one is named acess_levels and it's meant to define the different possible
-- groups a GM can belong to.

-- You can see in this tablefive predefined GM groups. Each group has a different
-- Access_Level, and GM's access level in the characters table should match with
-- one of these. You could define as many groups as needed and give them whatever
-- number you wanted to, from 1 to 255.

-- In our predefined set of examples, access_level=1 is for the highest admin,
-- and access_level=4 is for Event GMs.

-- The rest of the access_levels table columns are expected to be self explanatory.

-- And there is a second table named admin_commands and in this table
-- administrators should add every command they wanted GMs to use.

-- We left just one query here to show how commands should be added to the table:

-- INSERT IGNORE INTO `admin_command_access_rights` VALUES ('admin_admin','6');

-- If an administrator wanted to grant his GMs from groups 2 and 3 the usage of the teleport
-- command, he should just copy our example and replace values like this:

-- INSERT IGNORE INTO `admin_command_access_rights` VALUES ('admin_teleport','2,3');

-- So on, for each command there should be a record in this table. And it would be
-- advisable to use one query per command to avoid messups ;)

CREATE TABLE IF NOT EXISTS `access_levels` (
  `Access_Level` MEDIUMINT(9) NOT NULL,  
  `Name` VARCHAR(255) NOT NULL DEFAULT '',
  `Title` tinyint(3) NOT NULL DEFAULT '0',
  `Allow_Transaction` ENUM('true', '0', 'false') NOT NULL DEFAULT 'false',
  PRIMARY KEY (`Access_Level`)
);

INSERT INTO `access_levels` VALUES
(1, 'Admin', '0', 'true'),
(2, 'Head GM', '0', 'false'),
(3, 'General GM', '0', 'false'),
(4, 'Event GM', '0', 'true'),
(5, 'Chat Moderator', '0', 'true');
  