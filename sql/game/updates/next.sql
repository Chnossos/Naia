ALTER TABLE access_levels
	ADD COLUMN Title tinyint(3) unsigned NOT NULL DEFAULT '0' AFTER Name;