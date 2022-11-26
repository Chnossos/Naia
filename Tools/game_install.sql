-- These queries are meant to delete any gameserver table you may have from
-- previous Naia installations.
--
-- Queries that create these tables and/or populate them apropiately are
-- available in the sql/ folder.

DROP TABLE IF EXISTS access_levels;
DROP TABLE IF EXISTS admin_commands;
DROP TABLE IF EXISTS area_data;
DROP TABLE IF EXISTS banks;
DROP TABLE IF EXISTS char_creation_items;
DROP TABLE IF EXISTS char_relations;
DROP TABLE IF EXISTS char_templates;
DROP TABLE IF EXISTS characters;
DROP TABLE IF EXISTS containers;
DROP TABLE IF EXISTS experience;
DROP TABLE IF EXISTS maps;
DROP TABLE IF EXISTS subarea_data;
DROP TABLE IF EXISTS zaaps;