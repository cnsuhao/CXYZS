/*
Navicat MySQL Data Transfer

Source Server         : 127.0.0.1
Source Server Version : 50528
Source Host           : localhost:3306
Source Database       : game1

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2015-10-27 10:07:40
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `applyinfo`
-- ----------------------------
DROP TABLE IF EXISTS `applyinfo`;
CREATE TABLE `applyinfo` (
  `ID` int(11) NOT NULL,
  `applyID` int(11) DEFAULT '0' COMMENT '申请人ID',
  `applyType` int(11) DEFAULT '0' COMMENT '申请类型',
  `targetID` int(11) DEFAULT '0' COMMENT '目标ID',
  `applyTime` datetime DEFAULT NULL COMMENT '申请时间',
  `backupInfo` varchar(50) DEFAULT NULL COMMENT '备注',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of applyinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `ganginfo`
-- ----------------------------
DROP TABLE IF EXISTS `ganginfo`;
CREATE TABLE `ganginfo` (
  `ID` int(11) NOT NULL,
  `gangName` varchar(20) NOT NULL COMMENT '公会名',
  `gangLevel` int(11) DEFAULT '1' COMMENT '公会等级',
  `gangShopLevel` int(11) DEFAULT '1' COMMENT '公会商店等级',
  `headerID` int(11) NOT NULL COMMENT '会长ID',
  `headerName` varchar(20) NOT NULL COMMENT '会长名',
  `gangActive` int(11) DEFAULT '0' COMMENT '活跃度',
  `gangAnnouncement` varchar(50) DEFAULT '' COMMENT '公会公告',
  `viceHeader1` int(10)  COMMENT '副会长ID1',
  `viceHeader2` int(10)  COMMENT '副会长ID2',
  `starFlex` blob  COMMENT '堂主',
  `createTime` datetime  COMMENT '创建时间',
  `copyID` int(11)  COMMENT '副本ID',
  `copyStartTime` datetime  COMMENT '副本开始时间',
  `members` blob  COMMENT '帮派成员',
  `backupInfo` varchar(50) DEFAULT NULL COMMENT '备注',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of ganginfo
-- ----------------------------

-- ----------------------------
-- Table structure for `guid`
-- ----------------------------
DROP TABLE IF EXISTS `guid`;
CREATE TABLE `guid` (
  `item` bigint(20) DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into guid values(0);
-- ----------------------------
-- Records of guid
-- ----------------------------

-- ----------------------------
-- Table structure for `player`
-- ----------------------------
DROP TABLE IF EXISTS `player`;
CREATE TABLE `player` (
  `playerID` bigint(11) unsigned NOT NULL COMMENT '玩家id 用账号id来填充',
  `playerName` varchar(64) NOT NULL,
  `sex` tinyint(2) DEFAULT '0' COMMENT '性别 0女 1男',
  `headID` smallint(8) DEFAULT '0' COMMENT '头像ID',
  `offlineTime` bigint(11) DEFAULT '0' COMMENT '离线时间',
  `level` int(8) DEFAULT '1' COMMENT '等级',
  `exp` int(11) DEFAULT '0' COMMENT '经验',
  `money` int(11) DEFAULT '0' COMMENT '游戏币',
  `gold` int(11) DEFAULT '0' COMMENT '充值货币（rmb）',
  `bindGold` int(11) DEFAULT '0' COMMENT '绑定充值货币',
  `rechargGoldSum` int(11) DEFAULT '0' COMMENT '充值货币总额',
  `arenaScore` int(11) DEFAULT '0' COMMENT '竞技场积分',
  `arenaMoney` int(11) DEFAULT '0' COMMENT '竞技场货币',
  `battlefieldScore` int(11) DEFAULT '0' COMMENT '战场积分',
  `battlefieldMoney` int(11) DEFAULT '0' COMMENT '战场货币',
  `mapID` int(11) DEFAULT '0' COMMENT '地图id',
  `posx` int(11) DEFAULT '0' COMMENT '位置x',
  `posy` int(11) DEFAULT '0' COMMENT '位置y',
  `hp` int(11) DEFAULT '0',
  `mp` int(11) DEFAULT '0',
  `xp` int(11) DEFAULT '0',
  `societyID` int(11) DEFAULT '0' COMMENT '公会id',
  `realmLevel` int(11) DEFAULT '0' COMMENT '境界等级',
  `realmExp` int(8) DEFAULT '0' COMMENT '境界经验',
  `activeSkillTreeId` int(11) DEFAULT '0' COMMENT '激活任务树ID',
  `skillInfo` blob COMMENT '功法信息',
  `wingInfo` blob COMMENT '翅膀信息',
  `realmupInfo` blob COMMENT '法宝信息',
  `mountInfo` blob COMMENT '器灵信息',
  `buffInfo` blob COMMENT 'buff信息',
  `copy` blob COMMENT '副本信息',
  `daily` blob COMMENT '日常信息',
  `task` blob COMMENT '任务信息',
  `equip` blob COMMENT '身上装备',
  `bag` blob COMMENT '背包物品',
  `friendsList` blob COMMENT '好友列表',
  `blackList` blob COMMENT '黑名单列表',
  `choseskill` blob COMMENT '玩家选择要使用的技能',
  PRIMARY KEY (`playerID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of player
-- ----------------------------
INSERT INTO `player` VALUES ('107', 'az', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '101', 0x898A01000100000000000000718E01000100000000000000, '', '', '', '', '', '', 0x010000000100000002000000, 0x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000, '', '', '', 0x29460F00B1590F0051690F00F1780F00E1C81000C9CC100081D8100000000000);
