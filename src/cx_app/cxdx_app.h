
#ifndef __CX_DX_APP_H__
#define	__CX_DX_APP_H__

#include "dmbase_message.h"
#include "dmbase_data_types.h"
#include "dmbase_transport_mgmt.h"
#include "dappcommon_srvrconfload.h"
#include "events.h"
#include "hss_oamps.h"


#define	CXDX_MSG_CMD_CODE_UAR			300
#define	CXDX_MSG_CMD_CODE_UAA			300
#define	CXDX_MSG_CMD_CODE_SAR			301
#define	CXDX_MSG_CMD_CODE_SAA			301
#define	CXDX_MSG_CMD_CODE_LIR			302
#define	CXDX_MSG_CMD_CODE_LIA			302
#define	CXDX_MSG_CMD_CODE_MAR			303
#define	CXDX_MSG_CMD_CODE_MAA			303
#define	CXDX_MSG_CMD_CODE_RTR			304
#define	CXDX_MSG_CMD_CODE_RTA			304
#define	CXDX_MSG_CMD_CODE_PPR			305
#define	CXDX_MSG_CMD_CODE_PPA			305


#define CX_AVP_VISITED_NETWORK_IDENTIFIER                       600
#define CX_AVP_PUBLIC_IDENTITY                                  601
#define CX_AVP_SERVER_NAME                                      602
#define CX_AVP_SERVER_CAPABILITIES                              603
#define CX_AVP_MANDATORY_CAPABILITIES                           604
#define CX_AVP_OPTIONAL_CAPABILITIES                            605
#define CX_AVP_USER_DATA                                        606
#define CX_AVP_SIP_NUMBER_AUTH_ITEMS                            607
#define CX_AVP_SIP_AUTHENTICATION_SCHEME                        608
#define CX_AVP_SIP_AUTHENTICATE                                 609
#define CX_AVP_SIP_AUTHORIZATION                                610
#define CX_AVP_SIP_AUTHORIZATION_CONTEXT                        611
#define CX_AVP_SIP_AUTH_DATA_ITEM                               612
#define CX_AVP_SIP_ITEM_NUMBER                                  613
#define CX_AVP_SERVER_ASSIGNMENT_TYPE                           614
#define CX_AVP_DEREGISTERATION_REASON                           615
#define CX_AVP_REASON_CODE                                      616
#define CX_AVP_REASON_INFO                                      617
#define CX_AVP_CHARGING_INFORMATION                             618
#define CX_AVP_PRIMARY_EVENT_CHARGING_FUNCTION_NAME             619
#define CX_AVP_SECONDARY_EVENT_CHARGING_FUNCTION_NAME           620
#define CX_AVP_PRIMARY_CHARGING_COLLECTION_FUNCTION_NAME        621
#define CX_AVP_SECONDARY_CHARGING_COLLECTION_FUNCTION_NAME      622
#define CX_AVP_USER_AUTHORIZATION_TYPE                          623
#define CX_AVP_USER_DATA_ALREADY_AVAILABLE                      624
#define CX_AVP_CONFIDENTIALITY_KEY                              625
#define CX_AVP_INTEGRITY_KEY                                    626
#define CX_AVP_SUPPORTED_FEATURES                               628
#define CX_AVP_FEATURE_LIST_ID                                  629
#define CX_AVP_FEATURE_LIST                                     630
#define CX_AVP_SUPPORTED_APPLICATIONS                           631
#define CX_AVP_ASSOCIATED_IDENTITIES                            632
#define CX_AVP_ORIGINATING_REQUEST                              633
#define CX_AVP_WILDCARDED_PSI                                   634
#define CX_AVP_SIP_DIGEST_AUTHENTICATE                          635
#define CX_AVP_WILDCARDED_IMPU                                  636


//User Authorization Type
#define REGISTERATION                                           0
#define DE_REGISTERATION                                        1
#define REGISTERATION_AND_CAPABILITIES                          2






//Experimantal Result Codes

//Success
#define DIAMETER_FIRST_REGISTRATION                             2001
#define DIAMETER_SUBSEQUENT_REGISTRATION                        2002
#define DIAMETER_UNREGISTERED_SERVICE                           2003
#define DIAMETER_SUCCESS_SERVER_NAME_NOT_STORED                 2004

//Permanent Failures
#define DIAMETER_ERROR_USER_UNKNOWN                             5001
#define DIAMETER_ERROR_IDENTITIES_DONT_MATCH                    5002
#define DIAMETER_ERROR_IDENTITY_NOT_REGISTERED                  5003
#define DIAMETER_ERROR_ROAMING_NOT_ALLOWED                      5004
#define DIAMETER_ERROR_IDENTITY_ALREADY_REGISTERED              5005
#define DIAMETER_ERROR_AUTH_SCHEME_NOT_SUPPORTED                5006
#define DIAMETER_ERROR_IN_ASSIGNMENT_TYPE                       5007
#define DIAMETER_ERROR_TOO_MUCH_DATA                            5008
#define DIAMETER_ERROR_NOT_SUPPORTED_USER_DATA                  5009
#define DIAMETER_ERROR_FEATURE_UNSUPPORTED                      5011

#define CX_MAX_AVP_META_DATA									1000


#define CX_SESSION_TABLE_MAX_SIZE 								22201

typedef enum CxDxAPIRetVal
{
	CxDx_Retval_Success,
	CxDx_Retval_Failure,
	CxDx_Retval_IncorrectParameters,
	CxDx_Retval_MoreInfoRequired,
	CxDx_Retval_AccessDenied,
	CxDx_Retval_UnableToOpenDevice,
	CxDx_Retval_Insufficient_Buffer,
	CxDx_Retval_Not_Found,
	CxDx_Retval_Not_Supported,
	CxDx_Retval_Null_Pointer,
	CxDx_Retval_Unknown_AVP,
	CxDx_Retval_InvalidAVPData,
	CxDx_Retval_InvalidMsgHeader,
	CxDx_Retval_Limit_Exceed
}t_CxDxAPIRetVal;


typedef struct __CxAVPMetadata
{
	_Bool			bIsPresent;
	unsigned int	unAVPCount;
	unsigned short	usAVPPosition;

}t_CxAVPMetadata;

typedef struct CxPerfMgmtData
{
	unsigned int	unattemptedUARUR ;
	unsigned int	unsuccessfulUAAUR ;
	unsigned int	unfailedUAAUR;
	unsigned int	unattemptedSARUR;
	unsigned int	unsuccessfulSAAUR;
	unsigned int	unfailedSAAUR;
	unsigned int	unattemptedLIALIQ;
	unsigned int	unsuccessfulLIALIQ;
	unsigned int	unfailedLIALIQ;
	unsigned int	unattemptedMARMA;
	unsigned int	unsuccessfulMAAMA;
	unsigned int	unfailedMAAMA;
}t_CxPerfMgmtData;

typedef struct CxFaultMgmtData
{

}t_CxFaultMgmtData;

typedef struct CxAppContext
{
        HDIAMETERSTACKCONTEXT   hDiamStackContext;
        DMBASE_HASH_HANDLE  	hSessionTable;
        t_CxAVPMetadata			oAVPMetadata[CX_MAX_AVP_META_DATA];
		t_CxPerfMgmtData		oCxPerfMgmtData;
		t_CxFaultMgmtData		oCxFaultMgmtData;
} t_CxAppContext;

typedef t_CxAppContext* HCXAPPCONTEXT;



typedef struct IMPUData
{
    t_DiamOctetString*  poPublicUserId;
    t_DiamUTF8String*   poServerName;
    unsigned int        unRegisStatus;
    unsigned int        unAuthPending;

}t_IMPUData;

typedef struct __CxUAR          t_CxUAR;
typedef struct __CxSAR          t_CxSAR;
typedef struct __CxMAR          t_CxMAR;
typedef struct __CxLIR          t_CxLIR;
typedef struct __CxVendorApp    t_CxVendorApp;

struct __CxVendorApp
{
    unsigned int unVendorID;
    unsigned int unApplicationID;
};

struct __CxUAR
{
	unsigned char			ucFlags;
	unsigned int 			unHopByHopID;
	unsigned int 			unEndToEndID;
//    t_DiameterHeader        oDiaHeader;
    t_CxUARDBRequest        oUARDBRequest;
    t_CxVendorApp           oVendorSpecificApp;
    unsigned char           ucVisitedNetworkId[CX_LARGE_STRING];
    unsigned int            unAuthType; //Type of User Authorization being performed
    unsigned int            unAuthSessionState;
    DMBase_SList            *poProxyInfo;

};

struct __CxSAR
{
	unsigned char			ucFlags;
	unsigned int 			unHopByHopID;
	unsigned int 			unEndToEndID;
    //t_DiameterHeader        oDiaHeader;
    t_CxVendorApp           oVendorSpecificApp;
    t_CxSARDBRequest        oSARDBRequest;
    unsigned int            unAuthSessionState;
    DMBase_SList            *poProxyInfo;
};

struct __CxMAR
{
	unsigned char			ucFlags;
	unsigned int 			unHopByHopID;
	unsigned int 			unEndToEndID;
    //t_DiameterHeader        oDiaHeader;
    t_CxVendorApp           oVendorSpecificApp;
    t_CxMARDBRequest        oMARDBRequest;
    unsigned int            unAuthSessionState;
    unsigned int            unSIPNumnerAuthItems;
    DMBase_SList            *poProxyInfo;
};

struct __CxLIR
{
	unsigned char			ucFlags;
	unsigned int 			unHopByHopID;
	unsigned int 			unEndToEndID;
    //t_DiameterHeader        oDiaHeader;
    t_CxVendorApp           oVendorSpecificApp;
    t_CxLIRDBRequest        oLIRDBRequest;
    unsigned int            unAuthSessionState;
    unsigned int            unOriginatingRequest;
    DMBase_SList            *poProxyInfo;
};

#ifdef __cplusplus
extern "C" {
#endif

AMPS_MODULE_HANDLE	cxdx_app_ModuleInit(AMPS_HANDLE hAMPS_HANDLE_i,
										void* 		pvUserData_i);

void 				cxdx_app_ModuleCleanup(	AMPS_HANDLE 		hAMPS_HANDLE_i,
											AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i);

t_DiamAPIReturnVal  Cx_ProcessMessage(	HPEERTABLEENTRY 	hPeer,
										HDIAMETERMESSAGE 	hDiamMsg_i,
										void* 				pvAppContext);

void Cx_PeerCloseEventCallback(void*	pvAppContext_i, HPEERTABLEENTRY hPeer_i);

void                Cx_DBResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
											void* r_pvEventHandle, void* r_pvEventPayload);

t_CxDxAPIRetVal	Cx_InitApplication(t_cx_config_params*	poCxConfParams_i);

void  cx_app_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
					void* r_pvEventHandle, void* r_pvEventPayload);

t_CxDxAPIRetVal 	cx_app_GetValForOID(HCXAPPCONTEXT		hCxAppContext_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i);

t_CxDxAPIRetVal Cx_GenerateDBRequest(HCXAPPCONTEXT hAppContext_i, unsigned int unReqType_i,
									 void* pvData_i, unsigned int unSize_i);


#ifdef __cplusplus
}
#endif


#endif
