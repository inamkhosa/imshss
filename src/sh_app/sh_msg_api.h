#ifndef	SH_MSG_API_H
#define SH_MSG_API_H

#include "dmbase_avp.h"
#include "dmbase_message.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_data_types.h"


#define	SH_3GPP_VENDOR_ID	10415
#define	SH_DIAM_APP_ID		16777217
#define	SH_CMD_CODE_UDR		306
#define	SH_CMD_CODE_UDA		306
#define	SH_CMD_CODE_PUR		307
#define	SH_CMD_CODE_PUA		307
#define	SH_CMD_CODE_SNR		308
#define	SH_CMD_CODE_SNA		308
#define	SH_CMD_CODE_PNR		309
#define	SH_CMD_CODE_PNA		309



typedef enum ShAppReturnVal
{
    Sh_Retval_Success,
    Sh_Retval_Failure,
    Sh_Retval_IncorrectParameters,
	Sh_Retval_NullPointer,
	Sh_Retval_NotFound
} t_ShAppReturnVal;

typedef	struct	ShPullParams
{
	//actually a UDR 
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPDestinationHost;
	HAVP			hAVPDestinationRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPUserIdentity;
	HAVP			hAVPWildcardedPSI;
	HAVP			hAVPServerName;
	DMBase_SList*	pListOfServiceIndicationAVPs;
	DMBase_SList*	pListOfDataReferenceAVPs;
	DMBase_SList*	pListOfIdentitySetAVPs;
	HAVP			hAVPRequestedDomain;
	HAVP			hAVPCurrentLocation;
	HAVP			hAVPDSAI_Tag;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;
}t_ShPullParams;

typedef	struct	ShPullRespParams
{
	//actually a UDA
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPResultCode;
	HAVP			hAVPExperimentalResult;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPWildcardedPSI;
	HAVP			hAVPUserData;
	DMBase_SList*	pListOfFailedAVPs;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;
}t_ShPullRespParams;


typedef struct ShUpdateParams
{
	//actually a PUR

	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPDestinationHost;
	HAVP			hAVPDestinationRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPUserIdentity;
	HAVP			hAVPWildcardedPSI;
	HAVP			hAVPDataReference;
	HAVP			hAVPUserData;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

} t_ShUpdateParams;

typedef struct ShUpdateRespParams
{
	//actually a PUA

	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPResultCode;
	HAVP			hAVPExperimentalResult;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPWildcardedPSI;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	DMBase_SList*	pListOfFailedAVPs;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

} t_ShUpdateRespParams;

typedef struct ShSubsNotifParams
{
	//actually SNR 
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPDestinationHost;
	HAVP			hAVPDestinationRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPUserIdentity;
	HAVP			hAVPWildcardedPSI;
	DMBase_SList*	pListOfServiceIndicationAVPs;
	HAVP			hAVPSendDataIndication;
	HAVP			hAVPServerName;
	HAVP			hAVPSubsReqType;
	DMBase_SList*	pListOfDataReferenceAVPs;
	HAVP			hAVPIdentitySet;
	HAVP			hAVPExpiryTime;
	HAVP			hAVPDSAI_Tag;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

} t_ShSubsNotifParams;


typedef struct ShSubsNotifRespParams
{
	//actually SNA
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPResultCode;
	HAVP			hAVPExperimentalResult;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPWildcardedPSI;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPUserData;
	HAVP			hAVPExpiryTime;
	DMBase_SList*	pListOfFailedAVPs;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

}t_ShSubsNotifRespParams;

typedef struct ShNotifParams
{
	//actually PNR 
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	HAVP			hAVPDestinationHost;
	HAVP			hAVPDestinationRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	HAVP			hAVPUserIdentity;
	HAVP			hAVPWildcardedPSI;
	HAVP			hAVPUserData;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

} t_ShNotifParams;

typedef struct ShNotifRespParams
{
	//actually PNA 
	HAVP			hAVPSessionID;
	HAVP			hAVPVendorSpecificAppID;
	HAVP			hAVPResultCode;
	HAVP			hAVPExperimentalResult;
	HAVP			hAVPAuthSessionState;
	HAVP			hAVPOriginHost;
	HAVP			hAVPOriginRealm;
	DMBase_SList*	pListOfSupportedFeaturesAVPs;
	DMBase_SList*	pListOfFailedAVPs;
	DMBase_SList*	pListOfProxyInfoAVPs;
	DMBase_SList*	pListOfRouteRecordAVPs;

} t_ShNotifRespParams;

t_ShAppReturnVal Sh_StuffAVPsFromSrcToDestList( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											 DMBase_SList* pAVPListDest_o,
								    		DMBase_SList* pAVPListSrc_i );


t_ShAppReturnVal Sh_CreateShPullRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										 HDIAMETERMESSAGE*	phMsgUDR_o,
										  t_ShPullParams	oShPullParams_i 
										);

t_ShAppReturnVal Sh_CreateShPullResponse( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										  HDIAMETERMESSAGE*	phMsgUDA_o,
										  t_ShPullRespParams	oShPullRespParams_i 
										);


t_ShAppReturnVal Sh_CreateShUpdateRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										   HDIAMETERMESSAGE*	phMsgPUR_o,
										  t_ShUpdateParams	oShUpdateParams_i 
										);

t_ShAppReturnVal Sh_CreateShUpdateResponse(HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										    HDIAMETERMESSAGE*	phMsgPUA_o,
										  t_ShUpdateRespParams	oShUpdateRespParams_i 
										);

t_ShAppReturnVal Sh_CreateShSubsNotifRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											  HDIAMETERMESSAGE*	phMsgSNR_o,
										  t_ShSubsNotifParams	oShSubsNotifParams_i 
										);

t_ShAppReturnVal Sh_CreateShSubsNotifResponse(HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											   HDIAMETERMESSAGE*	phMsgSNA_o,
										  t_ShSubsNotifRespParams	oShSubsNotifRespParams_i 
										);


t_ShAppReturnVal Sh_CreateShNotifRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										  HDIAMETERMESSAGE*	phMsgPNR_o,
										  t_ShNotifParams	oShNotifParams_i 
										);

t_ShAppReturnVal Sh_CreateShNotifResponse(HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										   HDIAMETERMESSAGE*	phMsgPNA_o,
										  t_ShNotifRespParams	oShNotifRespParams_i 
										);


#endif // #ifndef	SH_MSG_API_H
