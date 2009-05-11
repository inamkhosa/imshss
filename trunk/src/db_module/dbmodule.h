#ifndef	DBMODULE_H
#define	DBMODULE_H



#include "dmbase_dictionary.h"
#include "dmbase_ampswrappers.h"
#include "dappcommon_dbqueryload.h"
#include "dappcommon_srvrconfload.h"

//#define CX_QUERY_SIZE   				2000
#define SH_QUERY_SIZE   				2000
#define	DB_SH_QUERY_MAP_SIZE            100
#define	DB_CX_QUERY_MAP_SIZE            100
#define	DB_SH_VARHCAR_MAX_SIZE_IN_DBMS	70
#define	DB_HSS_NUMBER_OF_THREADS		10


#define HSS_XML_ENCODING 				"ISO-8859-1"


typedef struct db_HSSAppContext
{
    HDIAMETERSTACKCONTEXT		hDiamStackContext;
    t_Dappcommon_DBQueryMap		oDBQueryMapForSh[DB_SH_QUERY_MAP_SIZE];
    t_Dappcommon_DBQueryMap		oDBQueryMapForCx[DB_CX_QUERY_MAP_SIZE];
	AMPS_IOAGENTS_HANDLE		hIOAgentsHandle;
	t_hss_db_config_params		oHSS_DB_ConfigParams;
} t_db_HSSAppContext;

typedef t_db_HSSAppContext*		HDBHSSCONTEXT;

typedef struct db_HSS_ThreadContext
{
    HDBHSSCONTEXT				hDBHSSMainContext;
	DMBASE_DBE_HANDLE			hDBEngineHandle;
	AMPS_UIOAGENT_HANDLE		hUnitIOAgent;
} t_db_HSS_ThreadContext;

typedef t_db_HSS_ThreadContext*		HDBHSSTHREADCONTEXT;


typedef enum Db_HSSReturnVal
{
    Db_HSS_Retval_Success,
    Db_HSS_Retval_Failure,
    Db_HSS_Retval_IncorrectParameters,
    Db_HSS_Retval_NullPointer,
    Db_HSS_Retval_NotFound,
    Db_HSS_Retval_NotSupported,
    Db_HSS_Retval_OperationNotAllowed,
    Db_HSS_Retval_TooMuchData,
    Db_HSS_Retval_TranspDataOutOfSynch,
    Db_HSS_Retval_PermissionDenied,
    Db_HSS_Retval_QueryFailed,
    Db_HSS_Retval_DSAINotAvailable

} t_Db_HSSReturnVal;



typedef struct db_sh_AppSrvPermissions
{
	unsigned int	unDataRef;
	_Bool			bUDR_Allowed;
	_Bool			bPUR_Allowed;
	_Bool			bPNR_Allowed;
	_Bool			bSNR_Allowed;
}t_db_sh_AppSrvPermissions;


t_Db_HSSReturnVal				db_HSS_CreateDBHSSContext(HDBHSSCONTEXT* phdbHSSContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i);


t_Db_HSSReturnVal				db_sh_ProcessSNRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i);

t_Db_HSSReturnVal				db_sh_ProcessPNRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i);


t_Db_HSSReturnVal				db_sh_GetUserIdentityTypeFromDB(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
                                          unsigned char * pucUserIdentity_i,
                                          unsigned int* punIdentType_o
										  );

t_Db_HSSReturnVal				db_sh_GetAppServPermListFromDB(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
                                          unsigned char* pucServerAddress_i,
										  unsigned int	unDataRef_i,
                                          t_db_sh_AppSrvPermissions*	poAppSrvPerm_o
										  );

t_Db_HSSReturnVal				db_hss_RegisterForShEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  );

t_Db_HSSReturnVal				db_hss_RegisterForCxEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  );

t_Db_HSSReturnVal				db_hss_RegisterForOampsEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  );

void 							db_HSS_ThreadCleanup(void* hDBHSS_ThreadCtxt_io);

void* 							db_HSS_ThreadInit(AMPS_HANDLE r_hAMPSHandle,
									AMPS_UIOAGENT_HANDLE r_hUIOAgentsHandle, void* hdbHSSContext_io);

void  							db_ShAppRequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
										void* r_pvEventHandle, void* r_pvEventPayload);

void  							db_hss_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
								void* r_pvEventHandle, void* r_pvEventPayload);



#endif //#ifndef	DBMODULE_H
