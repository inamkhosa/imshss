/*
MySQL Data Transfer
Source Host: localhost
Source Database: HSS
Target Host: localhost
Target Database: HSS
Date: 2/2/2009 1:51:35 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for ActiveRegisterations
-- ----------------------------
DROP TABLE IF EXISTS `ActiveRegisterations`;
CREATE TABLE `ActiveRegisterations` (
  `ActiveRegId` int(10) unsigned NOT NULL,
  `RegisterationStatus` int(10) NOT NULL,
  `CSCFName` varchar(70) NOT NULL,
  `ASName` varchar(70) NOT NULL,
  `DiameterClientAddress` varchar(70) NOT NULL,
  `UNRI` varchar(70) NOT NULL,
  `UNRR` varchar(70) NOT NULL,
  `IMPU_ID` varchar(70) NOT NULL,
  `IMPI_ID` varchar(70) default NULL,
  PRIMARY KEY  (`ActiveRegId`),
  KEY `IMPU_ID` (`IMPU_ID`),
  CONSTRAINT `ActiveRegisterations_ibfk_1` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for Application_Server
-- ----------------------------
DROP TABLE IF EXISTS `Application_Server`;
CREATE TABLE `Application_Server` (
  `AppServer_ID` int(10) unsigned NOT NULL auto_increment,
  `AppServer_Name` varchar(70) NOT NULL,
  `AppServer_Address` varchar(70) NOT NULL,
  `Default_Handling` int(10) NOT NULL,
  `Service_Info` varchar(70) default NULL,
  PRIMARY KEY  (`AppServer_ID`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for ApplicationServerPermList
-- ----------------------------
DROP TABLE IF EXISTS `ApplicationServerPermList`;
CREATE TABLE `ApplicationServerPermList` (
  `APPSVR_PERM_ID` int(10) unsigned NOT NULL auto_increment,
  `APPServer_ID` int(10) unsigned NOT NULL,
  `Data_Reference` int(10) NOT NULL,
  `Allow_UDR` tinyint(4) NOT NULL,
  `Allow_PUR` tinyint(4) NOT NULL,
  `Allow_PNR` tinyint(4) NOT NULL,
  `Allow_SNR` tinyint(4) NOT NULL,
  PRIMARY KEY  (`APPSVR_PERM_ID`,`APPServer_ID`,`Data_Reference`),
  KEY `APPServer_ID` (`APPServer_ID`),
  CONSTRAINT `ApplicationServerPermList_ibfk_1` FOREIGN KEY (`APPServer_ID`) REFERENCES `Application_Server` (`AppServer_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for CAMEL_Data
-- ----------------------------
DROP TABLE IF EXISTS `CAMEL_Data`;
CREATE TABLE `CAMEL_Data` (
  `CDI` int(10) unsigned NOT NULL,
  `OIMCSI` varchar(70) NOT NULL,
  `VTIMCSI` varchar(70) NOT NULL,
  `DIMCSI` varchar(70) NOT NULL,
  `gsmSCFAddr` varchar(70) NOT NULL,
  `IMSSFAddr` varchar(70) NOT NULL,
  PRIMARY KEY  (`CDI`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for Charging_Function_Info
-- ----------------------------
DROP TABLE IF EXISTS `Charging_Function_Info`;
CREATE TABLE `Charging_Function_Info` (
  `Chrg_Info_Id` int(10) unsigned NOT NULL auto_increment,
  `Chrg_Collection_FN_Name` varchar(70) NOT NULL,
  `Primary_Chrg_FN_Name` varchar(70) default NULL,
  `Secondary_Chrg_FN_Name` varchar(70) default NULL,
  `Primary_Chrg_Event_FN_Name` varchar(70) default NULL,
  `Secondary_Chrg_Event_FN_Name` varchar(70) default NULL,
  `IMPU_ID` varchar(70) NOT NULL,
  PRIMARY KEY  (`Chrg_Info_Id`),
  KEY `IMPU_ID` (`IMPU_ID`),
  CONSTRAINT `Charging_Function_Info_ibfk_3` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for CommunicationsServicesIdentifiers
-- ----------------------------
DROP TABLE IF EXISTS `CommunicationsServicesIdentifiers`;
CREATE TABLE `CommunicationsServicesIdentifiers` (
  `CSID` int(10) unsigned NOT NULL,
  `CSI` varchar(70) NOT NULL,
  PRIMARY KEY  (`CSID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for DSAI
-- ----------------------------
DROP TABLE IF EXISTS `DSAI`;
CREATE TABLE `DSAI` (
  `TDSI_ID` int(10) unsigned NOT NULL auto_increment,
  `DSAI_Value` int(10) NOT NULL,
  `DSAI_Tag` varchar(70) NOT NULL,
  PRIMARY KEY  (`TDSI_ID`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for Identity_Type_Info
-- ----------------------------
DROP TABLE IF EXISTS `Identity_Type_Info`;
CREATE TABLE `Identity_Type_Info` (
  `Identity` varchar(70) NOT NULL,
  `Ident_type` tinyint(4) NOT NULL,
  PRIMARY KEY  (`Identity`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for IFC_ServiceProfile_Mapping
-- ----------------------------
DROP TABLE IF EXISTS `IFC_ServiceProfile_Mapping`;
CREATE TABLE `IFC_ServiceProfile_Mapping` (
  `IFC_ID` int(10) unsigned NOT NULL auto_increment,
  `ServiceProfile_ID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`IFC_ID`,`ServiceProfile_ID`),
  KEY `IFC_FK1` (`ServiceProfile_ID`),
  CONSTRAINT `IFC_FK1` FOREIGN KEY (`ServiceProfile_ID`) REFERENCES `ServiceProfiles` (`ServiceProfile_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `IFC_ServiceProfile_Mapping_ibfk_1` FOREIGN KEY (`IFC_ID`) REFERENCES `InitialFilterCriteria` (`IFC_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for IMS_Data_Extension
-- ----------------------------
DROP TABLE IF EXISTS `IMS_Data_Extension`;
CREATE TABLE `IMS_Data_Extension` (
  `Data_Extension_ID` int(10) unsigned NOT NULL auto_increment,
  `PSI_Activation` int(10) unsigned NOT NULL,
  `TDSI_ID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`Data_Extension_ID`),
  KEY `TDSI_ID` (`TDSI_ID`),
  CONSTRAINT `IMS_Data_Extension_ibfk_1` FOREIGN KEY (`TDSI_ID`) REFERENCES `DSAI` (`TDSI_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for InitialFilterCriteria
-- ----------------------------
DROP TABLE IF EXISTS `InitialFilterCriteria`;
CREATE TABLE `InitialFilterCriteria` (
  `IFC_ID` int(10) unsigned NOT NULL,
  `TriggerPoint_ID` int(10) unsigned NOT NULL,
  `AppServer_ID` int(10) unsigned NOT NULL,
  `IFC_Name` varchar(70) NOT NULL,
  `Priority` int(10) NOT NULL,
  PRIMARY KEY  (`IFC_ID`),
  KEY `TriggerPoint_ID` (`TriggerPoint_ID`),
  KEY `AppServer_ID` (`AppServer_ID`),
  CONSTRAINT `InitialFilterCriteria_ibfk_1` FOREIGN KEY (`TriggerPoint_ID`) REFERENCES `TriggerPoint` (`Trigger_Point_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `InitialFilterCriteria_ibfk_2` FOREIGN KEY (`AppServer_ID`) REFERENCES `Application_Server` (`AppServer_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for MediaProfiles
-- ----------------------------
DROP TABLE IF EXISTS `MediaProfiles`;
CREATE TABLE `MediaProfiles` (
  `MediaProfileID` int(10) unsigned NOT NULL,
  `MediaDescriptors` varchar(70) NOT NULL,
  PRIMARY KEY  (`MediaProfileID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PrivateUID_PublicUID_Mapping
-- ----------------------------
DROP TABLE IF EXISTS `PrivateUID_PublicUID_Mapping`;
CREATE TABLE `PrivateUID_PublicUID_Mapping` (
  `IMPI_ID` varchar(70) NOT NULL,
  `IMPU_ID` varchar(70) NOT NULL,
  PRIMARY KEY  (`IMPI_ID`,`IMPU_ID`),
  KEY `IM_PI_PU_MAP2` (`IMPU_ID`),
  CONSTRAINT `IM_PI_PU_MAP` FOREIGN KEY (`IMPI_ID`) REFERENCES `PrivateUserIdentities` (`IMPI_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `IM_PI_PU_MAP2` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PrivateUserIdentities
-- ----------------------------
DROP TABLE IF EXISTS `PrivateUserIdentities`;
CREATE TABLE `PrivateUserIdentities` (
  `IMPI_ID` varchar(70) NOT NULL,
  `IMSU_ID` varchar(70) NOT NULL,
  `Secret_key` varchar(16) NOT NULL,
  `Auth_Scheme` varchar(70) NOT NULL,
  `Algorithm` varchar(70) NOT NULL,
  `ServiceProfileID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`IMPI_ID`),
  KEY `PUI_FK1` (`ServiceProfileID`),
  CONSTRAINT `PUI_FK1` FOREIGN KEY (`ServiceProfileID`) REFERENCES `ServiceProfiles` (`ServiceProfile_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PSI_IMPU_Mapping
-- ----------------------------
DROP TABLE IF EXISTS `PSI_IMPU_Mapping`;
CREATE TABLE `PSI_IMPU_Mapping` (
  `PSI_ID` varchar(70) NOT NULL,
  `IMPU_ID` varchar(70) NOT NULL,
  PRIMARY KEY  (`PSI_ID`,`IMPU_ID`),
  KEY `IMPU_ID` (`IMPU_ID`),
  CONSTRAINT `PSI_IMPU_Mapping_ibfk_1` FOREIGN KEY (`PSI_ID`) REFERENCES `PublicServiceIdentity` (`PSI_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `PSI_IMPU_Mapping_ibfk_2` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PublicServiceIdentity
-- ----------------------------
DROP TABLE IF EXISTS `PublicServiceIdentity`;
CREATE TABLE `PublicServiceIdentity` (
  `PSI_ID` varchar(70) NOT NULL,
  `Template_ID` int(10) unsigned NOT NULL,
  `Name` varchar(70) NOT NULL,
  `WildCard` varchar(70) NOT NULL,
  `IMPU_ID` varchar(70) NOT NULL,
  `Repo_ID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`PSI_ID`),
  KEY `PSI_FK1` (`Template_ID`),
  KEY `Repo_ID` (`Repo_ID`),
  CONSTRAINT `PSI_FK1` FOREIGN KEY (`Template_ID`) REFERENCES `PublicServiceIdentityTemplate` (`Template_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `PublicServiceIdentity_ibfk_1` FOREIGN KEY (`PSI_ID`) REFERENCES `Identity_Type_Info` (`Identity`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `PublicServiceIdentity_ibfk_2` FOREIGN KEY (`Repo_ID`) REFERENCES `Repository_Data` (`Repo_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PublicServiceIdentityTemplate
-- ----------------------------
DROP TABLE IF EXISTS `PublicServiceIdentityTemplate`;
CREATE TABLE `PublicServiceIdentityTemplate` (
  `Template_ID` int(10) unsigned NOT NULL,
  `AppServer_ID` int(10) unsigned NOT NULL,
  `Template_Name` varchar(70) NOT NULL,
  `User_Name` varchar(70) NOT NULL,
  `Host_Name` varchar(70) NOT NULL,
  PRIMARY KEY  (`Template_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for PublicUserIdentities
-- ----------------------------
DROP TABLE IF EXISTS `PublicUserIdentities`;
CREATE TABLE `PublicUserIdentities` (
  `IMPU_ID` varchar(70) NOT NULL,
  `Reg_Status` int(10) unsigned NOT NULL,
  `SIP_URI` varchar(70) NOT NULL,
  `TEL_URI` varchar(70) NOT NULL,
  `PSI_ID` varchar(70) NOT NULL,
  `ServiceProfile_ID` varchar(70) NOT NULL,
  `PublicService_ID` varchar(70) NOT NULL,
  `UserProfile_ID` varchar(70) NOT NULL,
  `IsDefaultIMPU` tinyint(1) NOT NULL,
  `HasUnregisteredStateServices` tinyint(1) NOT NULL,
  `BarringIndication` int(10) NOT NULL,
  `IdentityType` varchar(70) NOT NULL,
  `DisplayName` varchar(70) NOT NULL,
  `MediaProfileID` int(10) unsigned NOT NULL,
  `Repo_ID` int(10) unsigned NOT NULL,
  `Auth_Pending` tinyint(1) NOT NULL,
  `SCSCF_Name` varchar(70) default NULL,
  PRIMARY KEY  (`IMPU_ID`),
  KEY `MediaProfileID` (`MediaProfileID`),
  KEY `Repo_ID` (`Repo_ID`),
  CONSTRAINT `PublicUserIdentities_ibfk_2` FOREIGN KEY (`MediaProfileID`) REFERENCES `MediaProfiles` (`MediaProfileID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `PublicUserIdentities_ibfk_4` FOREIGN KEY (`IMPU_ID`) REFERENCES `Identity_Type_Info` (`Identity`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for Repository_Data
-- ----------------------------
DROP TABLE IF EXISTS `Repository_Data`;
CREATE TABLE `Repository_Data` (
  `Repo_ID` int(10) unsigned NOT NULL auto_increment,
  `Service_Indication` varchar(70) NOT NULL,
  `Service_Data` varchar(70) default NULL,
  `Seq_No` int(10) unsigned NOT NULL,
  `IMPU_ID` varchar(70) default NULL,
  PRIMARY KEY  (`Repo_ID`),
  KEY `IMPU_ID` (`IMPU_ID`),
  CONSTRAINT `Repository_Data_ibfk_3` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for RoamingPartners
-- ----------------------------
DROP TABLE IF EXISTS `RoamingPartners`;
CREATE TABLE `RoamingPartners` (
  `RoamingPartnerId` int(10) unsigned NOT NULL,
  `Name` varchar(70) NOT NULL,
  `Address` varchar(70) NOT NULL,
  PRIMARY KEY  (`RoamingPartnerId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for SCSCF_CAPS
-- ----------------------------
DROP TABLE IF EXISTS `SCSCF_CAPS`;
CREATE TABLE `SCSCF_CAPS` (
  `CAPS_ID` int(10) unsigned NOT NULL,
  `WildCardPSI` varchar(70) default NULL,
  `OrigUnregSPT` varchar(70) default NULL,
  `SharediFCSet` varchar(70) default NULL,
  `DisplayName` varchar(70) default NULL,
  `Mandatory_Cap` int(10) default NULL,
  `Optional_Cap` int(10) default NULL,
  PRIMARY KEY  (`CAPS_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for SCSCFLIST
-- ----------------------------
DROP TABLE IF EXISTS `SCSCFLIST`;
CREATE TABLE `SCSCFLIST` (
  `SCSCFID` int(10) unsigned NOT NULL,
  `SCSCFName` varchar(70) NOT NULL,
  `SCSCFAddress` varchar(70) NOT NULL,
  `CAPS_ID` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`SCSCFID`),
  KEY `CAPS_ID` (`CAPS_ID`),
  CONSTRAINT `SCSCFLIST_ibfk_1` FOREIGN KEY (`CAPS_ID`) REFERENCES `SCSCF_CAPS` (`CAPS_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for ServicePointTrigger
-- ----------------------------
DROP TABLE IF EXISTS `ServicePointTrigger`;
CREATE TABLE `ServicePointTrigger` (
  `SPT_ID` int(10) unsigned NOT NULL auto_increment,
  `TriggerPoint_ID` int(10) unsigned NOT NULL,
  `SIP_Method` varchar(70) default NULL,
  `SIP_Header` varchar(70) default NULL,
  `Request_URI` varchar(70) default NULL,
  `Session_Case` tinyint(4) default NULL,
  `Group` tinyint(4) NOT NULL,
  `ConditionNegated` int(10) NOT NULL,
  `SessionDesc_Content` varchar(70) default NULL,
  `Reg_Type` tinyint(4) NOT NULL,
  `SIP_Header_Content` varchar(70) default NULL,
  `SessionDesc_Line` varchar(70) default NULL,
  PRIMARY KEY  (`SPT_ID`),
  KEY `TriggerPoint_ID` (`TriggerPoint_ID`),
  CONSTRAINT `ServicePointTrigger_ibfk_1` FOREIGN KEY (`TriggerPoint_ID`) REFERENCES `TriggerPoint` (`Trigger_Point_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for ServiceProfiles
-- ----------------------------
DROP TABLE IF EXISTS `ServiceProfiles`;
CREATE TABLE `ServiceProfiles` (
  `ServiceProfile_ID` int(10) unsigned NOT NULL,
  `ServiceProfile_Name` varchar(70) NOT NULL,
  PRIMARY KEY  (`ServiceProfile_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for Sh_IMS_Data
-- ----------------------------
DROP TABLE IF EXISTS `Sh_IMS_Data`;
CREATE TABLE `Sh_IMS_Data` (
  `SCSCF_Name` varchar(70) NOT NULL,
  `IFC_ID` int(10) unsigned NOT NULL,
  `IMS_User_State` int(10) unsigned NOT NULL,
  `Charging_info_ID` int(10) unsigned NOT NULL,
  `Data_Extension_ID` int(10) unsigned NOT NULL,
  `Sh_IMS_Data_ID` int(10) unsigned NOT NULL auto_increment,
  `IMPU_ID` varchar(70) NOT NULL,
  PRIMARY KEY  (`Sh_IMS_Data_ID`),
  KEY `IFC_ID` (`IFC_ID`),
  KEY `Charging_info_ID` (`Charging_info_ID`),
  KEY `Data_Extension_ID` (`Data_Extension_ID`),
  KEY `IMPU_ID` (`IMPU_ID`),
  CONSTRAINT `Sh_IMS_Data_ibfk_1` FOREIGN KEY (`IFC_ID`) REFERENCES `IFC_ServiceProfile_Mapping` (`IFC_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `Sh_IMS_Data_ibfk_2` FOREIGN KEY (`Charging_info_ID`) REFERENCES `Charging_Function_Info` (`Chrg_Info_Id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `Sh_IMS_Data_ibfk_3` FOREIGN KEY (`Data_Extension_ID`) REFERENCES `IMS_Data_Extension` (`Data_Extension_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `Sh_IMS_Data_ibfk_4` FOREIGN KEY (`IMPU_ID`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for SVP2IMPU_Mapping
-- ----------------------------
DROP TABLE IF EXISTS `SVP2IMPU_Mapping`;
CREATE TABLE `SVP2IMPU_Mapping` (
  `ServiceProfileID` int(10) unsigned NOT NULL,
  `IMPU` varchar(70) NOT NULL,
  PRIMARY KEY  (`ServiceProfileID`,`IMPU`),
  KEY `SV2IMPU_MAP2` (`IMPU`),
  CONSTRAINT `SV2IMPU_MAP1` FOREIGN KEY (`ServiceProfileID`) REFERENCES `ServiceProfiles` (`ServiceProfile_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `SVP2IMPU_Mapping_ibfk_1` FOREIGN KEY (`IMPU`) REFERENCES `PublicUserIdentities` (`IMPU_ID`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for TriggerPoint
-- ----------------------------
DROP TABLE IF EXISTS `TriggerPoint`;
CREATE TABLE `TriggerPoint` (
  `Trigger_Point_ID` int(10) unsigned NOT NULL auto_increment,
  `Trigger_Point_Name` varchar(70) NOT NULL,
  `ConjuctiveNormalForm` tinyint(4) NOT NULL,
  PRIMARY KEY  (`Trigger_Point_ID`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for VisitedNetworkIDs
-- ----------------------------
DROP TABLE IF EXISTS `VisitedNetworkIDs`;
CREATE TABLE `VisitedNetworkIDs` (
  `VNID` int(10) unsigned NOT NULL,
  `VisitedNetworkID` varchar(70) NOT NULL,
  `Name` varchar(70) NOT NULL,
  `Address` varchar(70) NOT NULL,
  PRIMARY KEY  (`VNID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `Application_Server` VALUES ('1', 'Great Application Server', 'Server1.com', '2', 'info');
INSERT INTO `Application_Server` VALUES ('2', 'IMS App Server', 'Serveradd.com', '1', 'New Info');
INSERT INTO `Application_Server` VALUES ('3', 'Server2', 'Server2.com', '4', 'Info3');
INSERT INTO `ApplicationServerPermList` VALUES ('1', '1', '2', '1', '0', '1', '1');
INSERT INTO `ApplicationServerPermList` VALUES ('2', '2', '18', '0', '0', '1', '1');
INSERT INTO `ApplicationServerPermList` VALUES ('3', '1', '18', '1', '1', '0', '1');
INSERT INTO `Charging_Function_Info` VALUES ('1', 'Charging Function 1', 'New Func3', 'New Func4', 'New Func1', 'New Func2', 'inam@advancedims.com');
INSERT INTO `Charging_Function_Info` VALUES ('2', 'Charging Function 2', 'pcf1@advancedims.com', 'scf1@advancedims.com', 'pcef1@advancedims.com', 'scef1@advancedims.com', 'mansoor@advancedims.com');
INSERT INTO `DSAI` VALUES ('1', '5', 'New Tag');
INSERT INTO `Identity_Type_Info` VALUES ('1', '1');
INSERT INTO `Identity_Type_Info` VALUES ('inam@advancedims.com', '0');
INSERT INTO `Identity_Type_Info` VALUES ('mansoor@advancedims.com', '0');
INSERT INTO `IFC_ServiceProfile_Mapping` VALUES ('1', '1');
INSERT INTO `IFC_ServiceProfile_Mapping` VALUES ('2', '1');
INSERT INTO `IMS_Data_Extension` VALUES ('1', '3', '1');
INSERT INTO `InitialFilterCriteria` VALUES ('1', '1', '1', 'I am a Great IFC', '4');
INSERT INTO `InitialFilterCriteria` VALUES ('2', '1', '1', 'IFC2', '2');
INSERT INTO `MediaProfiles` VALUES ('1', 'ulaw,alaw,g711,g723');
INSERT INTO `PrivateUID_PublicUID_Mapping` VALUES ('sip:inam@advancedims.com', 'inam@advancedims.com');
INSERT INTO `PrivateUID_PublicUID_Mapping` VALUES ('sip:mansoor@advancedims.com', 'mansoor@advancedims.com');
INSERT INTO `PrivateUserIdentities` VALUES ('sip:inam@advancedims.com', '', 'mansoor', 'IMSAKA', 'md5', '1');
INSERT INTO `PrivateUserIdentities` VALUES ('sip:mansoor@advancedims.com', '', 'mansoor', 'IMSAKA', 'md5', '1');
INSERT INTO `PSI_IMPU_Mapping` VALUES ('1', 'inam@advancedims.com');
INSERT INTO `PublicServiceIdentity` VALUES ('1', '1', 'DummyName', 'DummyWildcard', '1', '1');
INSERT INTO `PublicServiceIdentityTemplate` VALUES ('1', '1', 'DummyTemplate', 'DummyUser', 'DummyHost');
INSERT INTO `PublicUserIdentities` VALUES ('inam@advancedims.com', '2', 'sip:inam@advancedims.com', '+921221322122@advancedims.com', '1', '1', '1', '1', '0', '0', '1', '1', 'Inam', '1', '1', '0', 'scscf.com');
INSERT INTO `PublicUserIdentities` VALUES ('mansoor@advancedims.com', '0', 'sip:mansoor@advancedims.com', '+921222211534@advancedims.com', '1', '1', '1', '1', '0', '1', '1', '1', 'mansoor', '1', '1', '0', 'scscf.com');
INSERT INTO `Repository_Data` VALUES ('1', 'New Indication', 'ABC', '2', 'inam@advancedims.com');
INSERT INTO `SCSCF_CAPS` VALUES ('1', null, null, null, null, '100', '200');
INSERT INTO `SCSCF_CAPS` VALUES ('2', null, null, null, null, '101', '201');
INSERT INTO `SCSCFLIST` VALUES ('1', 'AA', 'BB', '2');
INSERT INTO `SCSCFLIST` VALUES ('2', 'AA', '', '2');
INSERT INTO `ServicePointTrigger` VALUES ('1', '1', null, null, 'sip://uri.com', null, '0', '0', null, '0', null, null);
INSERT INTO `ServicePointTrigger` VALUES ('2', '1', 'INVITE', null, null, null, '0', '0', null, '1', null, null);
INSERT INTO `ServicePointTrigger` VALUES ('3', '1', null, 'From', null, null, '0', '0', null, '1', 'Ali', null);
INSERT INTO `ServicePointTrigger` VALUES ('4', '1', null, null, 'Isip://newuri.com', null, '0', '0', null, '1', null, null);
INSERT INTO `ServicePointTrigger` VALUES ('5', '1', null, null, null, '2', '0', '0', null, '1', null, null);
INSERT INTO `ServicePointTrigger` VALUES ('6', '1', null, null, null, null, '0', '0', 'SD_Content2', '1', null, 'SD_Line2');
INSERT INTO `ServiceProfiles` VALUES ('1', 'serviceprofile1');
INSERT INTO `Sh_IMS_Data` VALUES ('scscf1.com', '1', '5', '1', '1', '1', 'inam@advancedims.com');
INSERT INTO `SVP2IMPU_Mapping` VALUES ('1', 'inam@advancedims.com');
INSERT INTO `TriggerPoint` VALUES ('1', 'DummyTrigger', '3');
INSERT INTO `TriggerPoint` VALUES ('2', 'TP2', '3');
