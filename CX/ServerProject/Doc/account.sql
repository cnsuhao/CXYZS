/*
Navicat MySQL Data Transfer

Source Server         : localhost
Source Server Version : 50027
Source Host           : localhost:3306
Source Database       : account

Target Server Type    : MYSQL
Target Server Version : 50027
File Encoding         : 65001

Date: 2015-07-25 19:52:55
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for account
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `accountId` bigint(10) unsigned NOT NULL auto_increment COMMENT '账号id，一个账号没区最多有10个角色',
  `accountName` varchar(64) default NULL COMMENT '账号',
  `pwd` varchar(64) default NULL COMMENT '账号密码',
  `userName` varchar(64) default NULL COMMENT '真实姓名',
  `idCard` varchar(20) default NULL COMMENT '身份证',
  `mobile` varchar(20) default NULL COMMENT '电话号码',
  `email` varchar(255) default NULL COMMENT '邮箱地址',
  `lastServerId` smallint(5) unsigned default '0' COMMENT '默认服务器id',
  `accountState` tinyint(5) default NULL COMMENT '账号状态',
  PRIMARY KEY  (`accountId`),
  UNIQUE KEY `account` (`accountName`)
) ENGINE=MyISAM AUTO_INCREMENT=107 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO `account` VALUES ('99', 'test00', '111111', 'dfg', '456456', '456456', '456456', '8', null);
INSERT INTO `account` VALUES ('102', 'name112', '111111', null, null, null, null, '0', null);
INSERT INTO `account` VALUES ('103', 'name122', '111111', null, null, null, null, '0', null);
INSERT INTO `account` VALUES ('104', 'name222', '111111', null, null, null, null, '0', null);
INSERT INTO `account` VALUES ('105', 'test99', '111111', null, null, null, null, '0', null);
INSERT INTO `account` VALUES ('106', 'test88', '111111', null, null, null, null, '0', null);
