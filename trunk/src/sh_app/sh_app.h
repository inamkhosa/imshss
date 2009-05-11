
#ifndef		SH_APP_H
#define		SH_APP_H


#include "dmbase_initconfig.h"
#include "dmbase_transport_mgmt.h"
#include "dmbase_message.h"
#include "AMPS_API.h"
#include "dmbase_dictionary.h"
#include "sh_msg_api.h"
#include "dmbase_ampswrappers.h"
#include "hss_oamps.h"


#define	SH_XSD_VALIDATION_FILENAME					"ShDataType_Rel7.xsd"



#define	SH_SESSION_HASHTAB_MAX_SIZE					2003//prime number close to 2000

#define	SH_IDENTITY_TYPE_DISTINCT_PUI				0
#define	SH_IDENTITY_TYPE_DISTINCT_PSI				1
#define	SH_IDENTITY_TYPE_WILDCARDED_PUI				2
#define	SH_IDENTITY_TYPE_WILCARDED_PSI				3


#define	SH_IDENTITYSET_TYPE_ALL_IDENTITIES			0
#define	SH_IDENTITYSET_TYPE_REGISTERED_IDENTITIES	1
#define	SH_IDENTITYSET_TYPE_IMPLICIT_IDENTITIES		2
#define	SH_IDENTITYSET_TYPE_ALIAS_IDENTITIES		3



#define	SH_IMSUSER_STATE_NOT_REGISTERED				0
#define	SH_IMSUSER_STATE_REGISTERED					1
#define	SH_IMSUSER_STATE_REGISTERED_UNREG_SERVICES	2
#define	SH_IMSUSER_STATE_AUTHENTICATION_PENDING		3



//todo: Move this and similar tihngs in Cx to Diameter base protocol header file
#define SH_DIAMETER_ERROR_USER_UNKNOWN     5001

#define	SH_MAX_SMALL_STRING_SIZE				256
#define	SH_MAX_MEDIUM_STRING_SIZE				512
#define	SH_MAX_LARGE_STRING_SIZE				1024
#define	SH_MAX_PUR_USER_DATA_STRING_SIZE		2*1024//thread crashes on anytihng more than 3k


typedef struct ShPerfMgmtData
{
	unsigned int unAttemptedUDRDTR;
	unsigned int unSuccessfulUDADTR;
	unsigned int unFailedUDADTR;
	unsigned int unAttemptedPURDTU;
	unsigned int unSuccessfulPUADTU;
	unsigned int unFailedPUADTU; 
}t_ShPerfMgmtData;

typedef struct ShFaultMgmtData
{
    unsigned int unFailedThresholdUDR;
	unsigned int unFailedThresholdPUR;
}t_ShFaultMgmtData;

typedef struct ShAppContext
{
	HDIAMETERSTACKCONTEXT	hDiamStackContext;
	DMBASE_HASH_HANDLE      hSessTableHandle;
	t_ShPerfMgmtData		oShPerfMgmtData;
	t_ShFaultMgmtData		oShFaultMgmtData;	
} t_ShAppContext;

typedef t_ShAppContext*		HSHAPPCONTEXT;

typedef struct ShSessionObject
{
    HPEERTABLEENTRY		hPeer;
    HDIAMETERMESSAGE	hMsg;
	HSHAPPCONTEXT		hShAppContext;
	unsigned int		unAuthSessionState;
	unsigned long		ulNumOfOutstandingMsgs;//when this reaches zero session will be removed from table
}t_ShSessionObject;

typedef struct	Sh_DB_CorrID
{
	DMBASE_HASHTABLE_KEY_HANDLE		hHashKeyHandle;
} t_Sh_DB_CorrID;


typedef	struct	Sh_User_Identity
{
	unsigned char	pucPublicIdentity[SH_MAX_SMALL_STRING_SIZE];
}t_Sh_User_Identity;

typedef	struct	Sh_VendorSpecificAppID
{
	unsigned int	unVendorID;
	unsigned int	unAuthApplicationID;
}t_Sh_VendorSpecificAppID;

typedef enum ShRespRetCodeType
{
	ShRespRetCode_ExperResult,
	ShRespRetCode_Result
} t_ShRespRetCodeType;

typedef struct EventDataShToDB
{
    unsigned int				unEvtCommandID;
	//below are for UDR and PUR Both
	unsigned int				unAuthSessionState;
	unsigned char				pucOriginHost[SH_MAX_SMALL_STRING_SIZE];
	unsigned char				pucOriginRealm[SH_MAX_SMALL_STRING_SIZE];
	unsigned int				unDataReference;
	unsigned char				pucSessionID[SH_MAX_SMALL_STRING_SIZE];
	unsigned char				pucServiceIndication[SH_MAX_SMALL_STRING_SIZE];
	unsigned char				pucServerName[SH_MAX_SMALL_STRING_SIZE];
	unsigned char				pucDSAI_Tag[SH_MAX_SMALL_STRING_SIZE];
	unsigned int				unIdentitySet;
	t_Sh_User_Identity			oUserIdentity;
	t_Sh_VendorSpecificAppID	oVendorSpecificAppID;
	//below are for PUR only
	unsigned char				pucUserData[SH_MAX_PUR_USER_DATA_STRING_SIZE];
} t_EventDataShToDB;

typedef struct EventDataDBToSh
{
    unsigned int				unEvtCommandID;
	//below are for PUR and UDR both
	unsigned int				unAuthSessionState;
    t_Sh_VendorSpecificAppID	oVendorSpecificAppID;
	unsigned char				pucSessionID[SH_MAX_SMALL_STRING_SIZE];
	t_ShRespRetCodeType			oShRespRetCodeType;
	unsigned int				unRetCodeForShResp;
	//below is for UDR only
	unsigned char				pucUserData[SH_MAX_PUR_USER_DATA_STRING_SIZE];

} t_EventDataDBToSh;



t_ShAppReturnVal sh_app_LookUpSessionObject(HSHAPPCONTEXT	hShAppContext_io,
											t_DiamOctetString oSessionID_i,
											t_ShSessionObject** ppoShSessionObj_o);

t_ShAppReturnVal sh_app_InsertSessionObject(HSHAPPCONTEXT hShAppContext_io,
											 t_DiamOctetString oSessionID_i,
											 t_ShSessionObject* poSessionObj_i );

t_ShAppReturnVal sh_app_RemoveSessionObject(HSHAPPCONTEXT hShAppContext_io,
											 t_DiamOctetString oSessionID_i );

t_ShAppReturnVal sh_app_InitialiseApp(HSHAPPCONTEXT		hShAppContext_io);
t_ShAppReturnVal sh_app_CreateSessionTable(HSHAPPCONTEXT	hShAppContext_io);

t_ShAppReturnVal	sh_app_CreateShAppContext(HSHAPPCONTEXT*	pohSh_AppContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i
											  );


t_DiamAPIReturnVal	sh_app_MessageProcessor(HPEERTABLEENTRY hPeer_i, HDIAMETERMESSAGE hDiamMsg_i,
									    void* pvAppContext);

void  				sh_app_DBResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
										void* r_pvEventHandle, void* r_pvEventPayload);






t_ShAppReturnVal	sh_app_ProcessSNRMessage(HSHAPPCONTEXT		hShAppContext_io,
											 HDIAMETERMESSAGE	hSNRMsg_i,
											HPEERTABLEENTRY hPeer_i);





t_ShAppReturnVal	sh_app_ProcessPNR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
													 unsigned char* pucEvData_i);
t_ShAppReturnVal	sh_app_ProcessSNR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
													 unsigned char* pucEvData_i);



t_ShAppReturnVal	sh_app_MakeAVPFromGeneralData(HSHAPPCONTEXT				hShAppContext_io,
												  HAVP*						phAVP_o,
												  unsigned int				unAVPCode_i,
												  HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
												  void*						pData_i,
												  unsigned int				unDataSize_i
										   );

t_ShAppReturnVal 	sh_app_RegisterForEvents(HSHAPPCONTEXT		hShAppContext_io);

void  				sh_app_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
									void* r_pvEventHandle, void* r_pvEventPayload);

t_ShAppReturnVal 	sh_app_GetValForOID(HSHAPPCONTEXT		hShAppContext_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i);













#endif //#ifndef		SH_APP_H
