#ifndef DB_SH_PUR_H
#define DB_SH_PUR_H

#include "dbmodule.h"


t_Db_HSSReturnVal				db_sh_ProcessPURData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i);




t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i);


t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_DSAI(
										HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 );

t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_PSI_Activation(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 );

t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_RepoData(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 );

t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_AliasRepoData(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 );



#endif //#ifndef DB_SH_PUR_H
