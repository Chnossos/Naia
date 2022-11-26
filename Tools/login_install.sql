-- These queries are meant to delete any loginserver table you may have from
-- previous Naia installations.
--
-- Queries that create these tables and/or populate them apropiately are
-- available in the sql/ folder.

DROP TABLE IF EXISTS accounts;
DROP TABLE IF EXISTS account_data;
DROP TABLE IF EXISTS gameservers;
DROP TABLE IF EXISTS banned_ip;

