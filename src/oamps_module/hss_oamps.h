#ifndef 	HSS_OAMPS_H
#define 	HSS_OAMPS_H

#include "dmbase_ampswrappers.h"
#include "dappcommon_helpers.h"
#include "oamps_xmlAPI.h"
#include "AMPS_API.h"
#include "dappcommon_srvrconfload.h"

#define	OAMPS_HSS_SESSION_HASHTAB_MAX_SIZE		2003//prime number close to 2000
#define	OAMPS_HSS_MAX_SMALL_STRING_SIZE			128
#define	OAMPS_HSS_MAX_MEDIUM_STRING_SIZE		256
#define	OAMPS_HSS_MAX_LARGE_STRING_SIZE			512
#define	OAMPS_HSS_MAX_OID_STRING_SIZE			50
#define	OAMPS_HSS_MAX_SUPPORTED_OIDS			47

typedef enum	hssOampsAppType
{
	hssAppType_Cx,
	hssAppType_Sh,
	hssAppType_DB,
	hssAppType_CxANDSh,
	hssAppType_CxANDDB,
	hssAppType_CxANDShANDDB,
	hssAppType_ShANDDB
}t_hssOampsAppType;

typedef enum	OidDetailsQueryCriteria
{
	OidDetailsByName,
	OidDetailsByEnum
}t_OidDetailsQueryCriteria;

typedef enum hssOampsFaultMgmtOid
{
	//cx
	hssFMOid_failedThresholdUAR,
	hssFMOid_failedThresholdSAR,
	hssFMOid_failedThresholdMAR,
	hssFMOid_failedThresholdLIR,

	//sh 
	hssFMOid_failedThresholdUDR,
	hssFMOid_failedThresholdPUR,

	//db
	hssFMOid_downDB,

	//both
	hssFMOid_listenPortBindFailed
	    

}t_hssOampsFaultMgmtOid;


typedef enum hssOampsPerfMgmtOid
{
	

	//cx
    hssPMOid_imsSubscriptionDBU, 
	hssPMOid_privateUserIdDBU ,  
	hssPMOid_sipPublicUserIdDBU, 
	hssPMOid_telPublicUserIdDBU ,
	hssPMOid_privateSrvIdDBU,    
	hssPMOid_sipPublicSrvIdDBU,  
	hssPMOid_telPublicSrvIdDBU , 
	hssPMOid_regPublicUserIdRoamingDomainDBU,    
	hssPMOid_unRegPublicUserIdDBU,       
	hssPMOid_regPrivateUserIdDBU,        
	hssPMOid_attemptedUARUR,     
	hssPMOid_successfulUAAUR,    
	hssPMOid_failedUAAUR,        
	hssPMOid_attemptedSARUR,     
	hssPMOid_successfulSAAUR,    
	hssPMOid_failedSAAUR,        
	hssPMOid_attemptedLIALIQ,    
	hssPMOid_successfulLIALIQ,   
	hssPMOid_failedLIALIQ,       
	hssPMOid_attemptedMARMA,     
	hssPMOid_successfulMAAMA,    
	hssPMOid_failedMAAMA ,


	//sh
	hssPMOid_attemptedUDRDTR, 
	hssPMOid_successfulUDADTR,
	hssPMOid_failedUDADTR,
	hssPMOid_attemptedPURDTU,
	hssPMOid_successfulPUADTU,
	hssPMOid_failedPUADTU    

	
}t_hssOampsPerfMgmtOid;

typedef enum hssOampsConfMgmtOid
{

	hssCMOid_Dummy1,
	hssCMOid_Dummy2,
	hssCMOid_Dummy3

}t_hssOampsConfMgmtOid;


typedef enum	hssOampsOidType
{
	OidType_PerfMgmt,
	OidType_FaultMgmt,
	OidType_ConfMgmt,
}t_hssOampsOidType;


typedef struct hssOIDDetails
{
	unsigned char		pucSupportedOID[OAMPS_HSS_MAX_OID_STRING_SIZE];
	t_hssOampsAppType	oHssOampsAppType;
	t_hssOampsOidType	oOidType;
	union
	{
		t_hssOampsConfMgmtOid  oConfMgmtOid;
		t_hssOampsFaultMgmtOid oFaultMgmtOid;
		t_hssOampsPerfMgmtOid  oPerfMgmtOid;
	};
}t_hssOIDDetails;

typedef enum hssOIDRespType
{
	hssOIDRespType_Long,
	hssOIDRespType_String
}t_hssOIDRespType;

typedef struct hssOAMPSReqDataToMods
{
	t_hssOIDDetails	oOIDDetails;
	unsigned char	pucUUID[DAPPCOMMON_UUID_SIZE_IN_BYTES];
}t_hssOAMPSReqDataToMods;

typedef struct hssOAMPSRespDataFromMods
{
	t_hssOIDDetails		oOIDDetails;
	unsigned char		pucUUID[DAPPCOMMON_UUID_SIZE_IN_BYTES];
	t_hssOIDRespType	oRespDataType;
	long				lResp;
	unsigned char		pucStrResp[OAMP_STR_VALUE_LENGTH];
}t_hssOAMPSRespDataFromMods;

typedef struct hssOAMPSTrapEventData
{
	t_hssOampsFaultMgmtOid	oFaultMgmtOid;
	long					lValue;
}t_hssOAMPSTrapEventData;



typedef struct hssOampsContext
{
	DMBASE_HASH_HANDLE      		hSessTableHandle;
	HDIAMETERSTACKCONTEXT			hDiamStackContext;
	t_hssOIDDetails					oOIDDetailsArray[OAMPS_HSS_MAX_SUPPORTED_OIDS];
	t_DappCommon_SNMPConfigParams	oSNMPConfigParams;
	AMPS_NET_HANDLE					hAMPSNetHandleForTraps;
} t_hssOampsContext;

typedef t_hssOampsContext*		HHSSOAMPSCONTEXT;

typedef struct hssOampsSessionObject
{
	HHSSOAMPSCONTEXT		hHssOampsContext;
	unsigned int			unPeerPort;
	unsigned char			pucPeerIPAddr[OAMPS_HSS_MAX_SMALL_STRING_SIZE];
	AMPS_NET_HANDLE			hAMPSNetHandleForPeer;
	_Bool					bWaitRespFromSh;
	_Bool					bWaitRespFromCx;
	_Bool					bWaitRespFromDb;
	char    				pcOampsReqOpCode[OAMP_OPCODE_LENGTH];
}t_hssOampsSessionObject;






AMPS_MODULE_HANDLE	hss_oamps_ModuleInit(AMPS_HANDLE r_hAMPS_HANDLE, 
										 void* r_pvUserData);

void 				hss_oamps_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, 
											AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE);



t_hssOampsReturnVal hss_oamps_LookUpSessionObject(HHSSOAMPSCONTEXT	hHssOampsContext_io,
											unsigned char* pucSessionID_i,
											t_hssOampsSessionObject** ppohssOampsSessObj_o);

t_hssOampsReturnVal hss_oamps_InsertSessionObject(HHSSOAMPSCONTEXT hHssOampsContext_io,
											 unsigned char* pucSessionID_i,
											 t_hssOampsSessionObject* pohssOampsSessObj_i );

t_hssOampsReturnVal hss_oamps_InitialiseApp(HHSSOAMPSCONTEXT		hHssOampsContext_io);

t_hssOampsReturnVal hss_oamps_RegisterForEvents(HHSSOAMPSCONTEXT		hHssOampsContext_io);

t_hssOampsReturnVal hss_oamps_CreateSessionTable(HHSSOAMPSCONTEXT	hHssOampsContext_io);

t_hssOampsReturnVal	hss_oamps_CreateMainContext(HHSSOAMPSCONTEXT*	pohHssOampsContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i
											  );

t_hssOampsReturnVal hss_oamps_RemoveSessionObject(HHSSOAMPSCONTEXT hHssOampsContext_io,
											 unsigned char* pucSessionID_i );

int					 hss_oamps_ReceiveMessageOnUDP_CallBack( AMPS_HANDLE     r_hAMPS_HANDLE, 
                                AMPS_APP_HANDLE hAMPS_APP_HANDLE_i, 
                                AMPS_NET_HANDLE hAMPS_NET_HANDLE_i, 
                                unsigned char*  pucBuff_i, 
                                int             nBuffLength_i, 
                                int             nPort_i, 
                                char*           pcIPAddress_i);

t_hssOampsReturnVal hss_oamps_PopulateOID_ToAppMapping(HHSSOAMPSCONTEXT hHssOampsContext_io );


t_hssOampsReturnVal hss_oamps_GetOIDDetailsFromMapping(HHSSOAMPSCONTEXT hHssOampsContext_io,
								unsigned char* pucOIDName_i,
								t_hssOIDDetails* poOIDDetails_o ,
								unsigned int	unOidEnum_i,
								t_OidDetailsQueryCriteria oQueryCriteria);

void  hss_oamps_ShResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload);


void  hss_oamps_DbResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload);


void  hss_oamps_CxResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload);


void  hss_oamps_TrapEventHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload);








#endif //#ifndef 	HSS_OAMPS_H
