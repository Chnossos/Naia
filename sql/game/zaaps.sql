CREATE TABLE IF NOT EXISTS `zaaps` (
  `MapID` smallint(5) unsigned NOT NULL,
  `CellID` smallint(3) unsigned NOT NULL,
  `Super_Area` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `Known_At_Start` ENUM('true', 'false') NOT NULL DEFAULT 'false',
  PRIMARY KEY (`MapID`)
);

INSERT INTO `zaaps` VALUES ('164', '193', '0', 'false');
INSERT INTO `zaaps` VALUES ('528', '126', '0', 'false');
INSERT INTO `zaaps` VALUES ('844', '212', '0', 'false');
INSERT INTO `zaaps` VALUES ('935', '295', '0', 'false');
INSERT INTO `zaaps` VALUES ('951', '126', '0', 'false');
INSERT INTO `zaaps` VALUES ('1158', '340', '0', 'false');
INSERT INTO `zaaps` VALUES ('1242', '323', '0', 'false');
INSERT INTO `zaaps` VALUES ('1841', '150', '0', 'false');
INSERT INTO `zaaps` VALUES ('2191', '200', '0', 'false');
INSERT INTO `zaaps` VALUES ('3022', '186', '0', 'false');
INSERT INTO `zaaps` VALUES ('3250', '165', '0', 'false');
INSERT INTO `zaaps` VALUES ('4263', '170', '0', 'false');
INSERT INTO `zaaps` VALUES ('4739', '354', '0', 'false');
INSERT INTO `zaaps` VALUES ('5295', '561', '0', 'false');
INSERT INTO `zaaps` VALUES ('6137', '104', '0', 'false');
INSERT INTO `zaaps` VALUES ('6954', '238', '0', 'false');
INSERT INTO `zaaps` VALUES ('7411', '311', '0', 'false');
INSERT INTO `zaaps` VALUES ('8037', '249', '0', 'false');
INSERT INTO `zaaps` VALUES ('8088', '208', '0', 'false');
INSERT INTO `zaaps` VALUES ('8785', '253', '0', 'false');
INSERT INTO `zaaps` VALUES ('9454', '268', '0', 'false');
INSERT INTO `zaaps` VALUES ('10114', '254', '3', 'false');
INSERT INTO `zaaps` VALUES ('10297', '229', '3', 'false');
INSERT INTO `zaaps` VALUES ('10304', '395', '3', 'false');
INSERT INTO `zaaps` VALUES ('10317', '167', '0', 'false');
INSERT INTO `zaaps` VALUES ('10349', '297', '3', 'false');
INSERT INTO `zaaps` VALUES ('10643', '269', '0', 'false');
INSERT INTO `zaaps` VALUES ('11170', '326', '0', 'false');
INSERT INTO `zaaps` VALUES ('11210', '401', '0', 'false');
