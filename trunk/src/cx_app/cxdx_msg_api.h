/*
 * File:   cxdx_msg_api.h
 * Author: root
 *
 * Created on July 28, 2008, 4:22 PM
 */

#ifndef _CXDX_MSG_API_H
#define	_CXDX_MSG_API_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "dmbase_avp.h"
#include "dmbase_message.h"
#include "cxdx_app.h"

#define CxDx_VENDOR_IDENTIFIER      10415
#define CxDx_APPLICATION            16777216
#define CxDx_MSG_CMD_CODE_UAR       300
#define CxDx_MSG_CMD_CODE_UAA       300
#define CxDx_MSG_CMD_CODE_SAR       301
#define CxDx_MSG_CMD_CODE_SAA       301
#define CxDx_MSG_CMD_CODE_LIR       302
#define CxDx_MSG_CMD_CODE_LIA       302
#define CxDx_MSG_CMD_CODE_MAR       303
#define CxDx_MSG_CMD_CODE_MAA       303
#define CxDx_MSG_CMD_CODE_RTR       304
#define CxDx_MSG_CMD_CODE_RTA       304
#define CxDx_MSG_CMD_CODE_PPR       305
#define CxDx_MSG_CMD_CODE_PPA       305


typedef struct CxQueryParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_PublicIdentity;
    HAVP    hAVP_VisitedNetworkID;
    HAVP    hAVP_UserAuthType;
    HAVP    hAVP_UserName;
    HAVP    hAVP_DestHostOrRealm;

}t_CxQueryParams;

typedef struct CxSelectPullParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_PublicIdentity;
    HAVP    hAVP_VisitedNetworkID;
    HAVP    hAVP_UserAuthType;
    HAVP    hAVP_UserName;
    HAVP    hAVP_DestHostOrRealm;

}t_CxSelectPullParams;


typedef struct CxQuery_SelectPull_RespParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_Orig_Host;
    HAVP    hAVP_Orig_Realm;
    HAVP    hAVP_Auth_Session_State;
    HAVP    hAVP_ResCode;
    HAVP    hAVP_Exp_Result_Code;
    HAVP    hAVP_SrvCapab;
    HAVP    hAVP_SrvName;
    HAVP    hAVP_Wildcard_IMPU;
}t_CxQuery_SelectPull_RespParams;




typedef struct CxPut_PullRespParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_Orig_Host;
    HAVP    hAVP_Orig_Realm;
    HAVP    hAVP_UserName;
    HAVP    hAVP_ResCode;
    HAVP    hAVP_Exp_Result_Code;
    HAVP    hAVP_Auth_Session_State;
    HAVP    hAVP_UserData;
    HAVP    hAVP_UserChargingInfo;
    HAVP    hAVP_AssocIDs;

}t_CxPut_PullRespParams;

typedef struct CxDeregisterParams
{
    HAVP    hAVP_Session_ID;
    DMBase_SList *poPublicUserIds;
    HAVP    hAVP_AuthSession_State;
    HAVP    hAVP_UserName;
    HAVP    hAVP_DeregReason;
    HAVP    hAVP_DestHost;
    HAVP    hAVP_AssocIDs;

}t_CxDeregisterParams;

typedef struct CxDeregisterRespParams
{
    HAVP hAVP_Session_ID;
    HAVP hAVP_Auth_Session_State;
    HAVP hAVP_ResCode;
    HAVP hAVP_Exp_Result_Code;
    HAVP hAVP_AssocIDs;

}t_CxDeregisterRespParams;


typedef struct CxLocationQueryParams
{
    HAVP hAVP_Session_ID;
    HAVP hAVP_Auth_Session_State;
    HAVP hAVP_PublicID;
    HAVP hAVP_DestHostOrRealm;
    HAVP hAVP_OrigRequest;

}t_CxLocationQueryParams;

typedef struct CxLocationQueryRespParams
{
    HAVP hAVP_Session_ID;
    HAVP hAVP_Orig_Host;
    HAVP hAVP_Orig_Realm;
    HAVP hAVP_ResCode;
    HAVP hAVP_Exp_Result_Code;
    HAVP hAVP_Auth_Session_State;
    HAVP hAVP_SrvName;
    HAVP hAVP_SrvCapab;
    HAVP hAVP_WildcardPSI;
    HAVP hAVP_Wildcard_IMPU;

}t_CxLocationQueryRespParams;

typedef struct CxAVReqParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_Auth_Session_State;
    HAVP    hAVP_PublicID;
    HAVP    hAVP_UserName;
    HAVP    hAVP_SIPNumAuthItems;
    HAVP    hAVP_SIPAuthDataItem;
    HAVP    hAVP_SrvName;
    HAVP    hAVP_DestHost;
    HAVP    hAVP_Wildcarded_IMPU;

}t_CxAVReqParams;

typedef struct CxAVReqRespParams
{
    HAVP    hAVP_Session_ID;
    HAVP    hAVP_Orig_Host;
    HAVP    hAVP_Orig_Realm;
    HAVP    hAVP_Auth_Session_State;
    HAVP    hAVP_PublicID;
    HAVP    hAVP_UserName;
    HAVP    hAVP_SIPNumAuthItems;
    DMBase_SList *poSIPAuthDataItem;
    HAVP    hAVP_ResCode;
    HAVP    hAVP_Exp_Result_Code;

}t_CxAVReqRespParams;

typedef struct CxUpdateSubscrDataParams
{
    HAVP hAVP_UserName;
    HAVP hAVP_Session_ID;
    HAVP hAVP_Auth_Session_State;
    HAVP hAVP_UserData;
    HAVP hAVP_ChargingInfo;
    HAVP hAVP_SIPAuthDataItem;
    HAVP hAVP_DestHost;


}t_CxUpdateSubscrDataParams;


t_CxDxAPIRetVal Cx_CreateVendorAppAVP(	HCXAPPCONTEXT	hCxAppContext_io,
										HAVP*			phVendorSpecificApp,
										HMSGALLOCATIONCONTEXT  hMsgAllocContext_io);

t_CxDxAPIRetVal Cx_CreateCxQuery_SelectPull_Resp(	HCXAPPCONTEXT 					hCxAppContext_io,
													HDIAMETERMESSAGE 				hDiaMsgUAA_io,
													HMSGALLOCATIONCONTEXT			hMsgAllocContext_io,
													t_CxQuery_SelectPull_RespParams oParams_i,
													unsigned int					unAppID_i,
													unsigned char					ucFlags_i,
													unsigned int					unHopByHopID_i,
													unsigned int					unEndToEndID_i);

t_CxDxAPIRetVal Cx_CreateCxPut_PullResp(	HCXAPPCONTEXT 			hCxAppContext_io,
											HDIAMETERMESSAGE  		hDiaMsgSAA_io,
											HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
											t_CxPut_PullRespParams	oParams_i,
											unsigned int			unAppID_i,
											unsigned char			ucFlags_i,
											unsigned int			unHopByHopID_i,
											unsigned int			unEndToEndID_i);


t_CxDxAPIRetVal Cx_CreateCxAVReqResp(	HCXAPPCONTEXT		 	hCxAppContext_io,
										HDIAMETERMESSAGE 		hDiaMsgMAA_io,
										HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
										t_CxAVReqRespParams		oParams_i,
										unsigned int			unAppID_i,
										unsigned char			ucFlags_i,
										unsigned int			unHopByHopID_i,
										unsigned int			unEndToEndID_i);


t_CxDxAPIRetVal Cx_CreateCxLocationQueryResp(	HCXAPPCONTEXT		 		hCxAppContext_io,
												HDIAMETERMESSAGE 			hDiaMsgLIA_io,
												HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
												t_CxLocationQueryRespParams	oParams_i,
												unsigned int				unAppID_i,
												unsigned char				ucFlags_i,
												unsigned int				unHopByHopID_i,
												unsigned int				unEndToEndID_i);


#ifdef	__cplusplus
}
#endif

#endif	/* _CXDX_MSG_API_H */

