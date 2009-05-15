#ifndef DB_SH_UDR_H
#define DB_SH_UDR_H

#include "dbmodule.h"



t_Db_HSSReturnVal				db_sh_ProcessUDRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_RepoData(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_AllIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_ImplicitIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_RegistIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_UserState(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_SCSCF_Name(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IFC(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);



t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_ChargingInfo(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_PSI_Activation(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_DSAI(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_AliasRepoData(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i);








#endif //#ifndef DB_SH_UDR_H
