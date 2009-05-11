
#include "cxdx_msgvalidation.h"
#include "dmbase_parser.h"
#include "hss.h"

//extern t_AVPDictionaryElement   g_oDmBase_AVPDictionary[];
//HDIAMETERSTACKCONTEXT    g_hDiameterStackContext;
extern	int                     g_nDiamBaseTraceID ;


t_CxDxAPIRetVal  Cx_ValidateMessage(	HCXAPPCONTEXT 	hAppContext_i,
										void*			poAppMsg_i,
										unsigned int	unCmdCode,
										DMBase_SList**	ppoFailedAVPList)
{

	t_CxDxAPIRetVal oRetvalue = CxDx_Retval_Success;

    switch(unCmdCode)
    {
        case CXDX_MSG_CMD_CODE_UAR:
            oRetvalue = Cx_ValidateUAR(hAppContext_i, (t_CxUAR*)poAppMsg_i, ppoFailedAVPList);
            break;
        case CXDX_MSG_CMD_CODE_MAR:
            oRetvalue = Cx_ValidateMAR(hAppContext_i, (t_CxMAR*)poAppMsg_i, ppoFailedAVPList);
            break;
        case CXDX_MSG_CMD_CODE_SAR:
            oRetvalue = Cx_ValidateSAR(hAppContext_i, (t_CxSAR*)poAppMsg_i, ppoFailedAVPList);
            break;
        case CXDX_MSG_CMD_CODE_LIR:
            oRetvalue = Cx_ValidateLIR(hAppContext_i, (t_CxLIR*)poAppMsg_i, ppoFailedAVPList);
            break;

    }

    return oRetvalue;
}


t_CxDxAPIRetVal  Cx_ValidateUAR(HCXAPPCONTEXT   hAppContext_i,
                                t_CxUAR*        poUAR_i,
                                DMBase_SList**  ppoFailedAVPsList_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o || NULL == poUAR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    static t_CxAVPValidationInfo oUARAVPs[] = {	{DMBASE_AVP_SESSION_ID,TRUE,1},
												{DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,TRUE,1},
												{DMBASE_AVP_AUTH_SESSION_STATE,TRUE,1},
												{DMBASE_AVP_ORIGIN_HOST,TRUE,1},
												{DMBASE_AVP_ORIGIN_REALM,TRUE,1},
												{DMBASE_AVP_DESTINATION_HOST,FALSE,1},
												{DMBASE_AVP_DESTINATION_REALM,FALSE,1},
												{DMBASE_AVP_USER_NAME,TRUE,1},
												{CX_AVP_SUPPORTED_FEATURES,FALSE,200},
												{CX_AVP_PUBLIC_IDENTITY,TRUE,1},
												{CX_AVP_VISITED_NETWORK_IDENTIFIER,TRUE,1},
												{CX_AVP_USER_AUTHORIZATION_TYPE,FALSE,1},
												{DMBASE_AVP_PROXY_INFO,FALSE,200},
												{DMBASE_AVP_ROUTE_RECORD,FALSE,200}
												};

    t_CxDxAPIRetVal oReturnVal;
    oReturnVal = Cx_CheckMissingAVPs(hAppContext_i,oUARAVPs,
									sizeof(oUARAVPs)/sizeof(t_CxAVPValidationInfo),
									ppoFailedAVPsList_o);


    if(CxDx_Retval_Success == oReturnVal)
	{
		//Check for duplication
		oReturnVal = Cx_CheckDuplicateAVPs(hAppContext_i, oUARAVPs,
							sizeof(oUARAVPs)/sizeof(t_CxAVPValidationInfo), ppoFailedAVPsList_o);
	}

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oReturnVal;

}


t_CxDxAPIRetVal  Cx_ValidateMAR(HCXAPPCONTEXT   hAppContext_i,
								t_CxMAR*        poMAR_i,
                                DMBase_SList**  ppoFailedAVPsList_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o || NULL == poMAR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    static t_CxAVPValidationInfo oMARAVPs[] = {	{DMBASE_AVP_SESSION_ID,TRUE,1},
												{DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,TRUE,1},
												{DMBASE_AVP_AUTH_SESSION_STATE,TRUE,1},
												{DMBASE_AVP_ORIGIN_HOST,TRUE,1},
												{DMBASE_AVP_ORIGIN_REALM,TRUE,1},
												{DMBASE_AVP_DESTINATION_HOST,FALSE,1},
												{DMBASE_AVP_DESTINATION_REALM,FALSE,1},
												{DMBASE_AVP_USER_NAME,TRUE,1},
												{CX_AVP_SUPPORTED_FEATURES,FALSE,200},
												{CX_AVP_PUBLIC_IDENTITY,TRUE,1},
												{CX_AVP_SIP_AUTH_DATA_ITEM,TRUE,1},
												{CX_AVP_SIP_NUMBER_AUTH_ITEMS,TRUE,1},
												{CX_AVP_SERVER_NAME,TRUE,1},
												{DMBASE_AVP_PROXY_INFO,FALSE,200},
												{DMBASE_AVP_ROUTE_RECORD,FALSE,200}
										};


    t_CxDxAPIRetVal oReturnVal;
    oReturnVal = Cx_CheckMissingAVPs(hAppContext_i,oMARAVPs,
									sizeof(oMARAVPs)/sizeof(t_CxAVPValidationInfo),
									ppoFailedAVPsList_o);

    if(CxDx_Retval_Success == oReturnVal)
	{
		//Check for duplication
		oReturnVal = Cx_CheckDuplicateAVPs(hAppContext_i, oMARAVPs,
							sizeof(oMARAVPs)/sizeof(t_CxAVPValidationInfo), ppoFailedAVPsList_o);
	}
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oReturnVal;
}

t_CxDxAPIRetVal  Cx_ValidateSAR( HCXAPPCONTEXT   hAppContext_i,
                                 t_CxSAR*        poSAR_i,
                                 DMBase_SList**  ppoFailedAVPsList_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o || NULL == poSAR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    static t_CxAVPValidationInfo oSARAVPs[] = {	{DMBASE_AVP_SESSION_ID,TRUE,1},
												{DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,TRUE,1},
												{DMBASE_AVP_AUTH_SESSION_STATE,TRUE,1},
												{DMBASE_AVP_ORIGIN_HOST,TRUE,1},
												{DMBASE_AVP_ORIGIN_REALM,TRUE,1},
												{DMBASE_AVP_DESTINATION_HOST,FALSE,1},
												{DMBASE_AVP_DESTINATION_REALM,FALSE,1},
												{DMBASE_AVP_USER_NAME,FALSE,1},
												{CX_AVP_SUPPORTED_FEATURES,FALSE,200},
												{CX_AVP_PUBLIC_IDENTITY,FALSE,200},
												{CX_AVP_WILDCARDED_PSI,FALSE,1},
												{CX_AVP_WILDCARDED_IMPU,FALSE,1},
												{CX_AVP_SERVER_NAME,TRUE,1},
												{CX_AVP_SERVER_ASSIGNMENT_TYPE,TRUE,1},
												{CX_AVP_USER_DATA_ALREADY_AVAILABLE,TRUE,1},
												{DMBASE_AVP_PROXY_INFO,FALSE,200},
												{DMBASE_AVP_ROUTE_RECORD,FALSE,200}
											};



    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    t_CxDxAPIRetVal oReturnVal;
    oReturnVal = Cx_CheckMissingAVPs(hAppContext_i,oSARAVPs,
									sizeof(oSARAVPs)/sizeof(t_CxAVPValidationInfo),
									ppoFailedAVPsList_o);

    if(TRUE != hAppContext_i->oAVPMetadata[oSARAVPs[7].unAVPCode].bIsPresent &&
       TRUE != hAppContext_i->oAVPMetadata[oSARAVPs[9].unAVPCode].bIsPresent)
    {
    	t_CxAVPValidationInfo *poAVPInfo = NULL;
		if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext, (void**)&poAVPInfo, sizeof(t_CxAVPValidationInfo)))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to allocate memory\n ");
			return DMBase_Retval_Failure;
		}
		poAVPInfo->unAVPCode = oSARAVPs[7].unAVPCode;
		DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)poAVPInfo);

    	poAVPInfo = NULL;
		if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext, (void**)&poAVPInfo, sizeof(t_CxAVPValidationInfo)))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to allocate memory\n ");
			return DMBase_Retval_Failure;
		}
		poAVPInfo->unAVPCode = oSARAVPs[9].unAVPCode;
		DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)poAVPInfo);
		oReturnVal = CxDx_Retval_MoreInfoRequired;
    }

    if(CxDx_Retval_Success == oReturnVal)
        {
        	//Check for duplication
        	oReturnVal = Cx_CheckDuplicateAVPs(hAppContext_i, oSARAVPs,
    							sizeof(oSARAVPs)/sizeof(t_CxAVPValidationInfo), ppoFailedAVPsList_o);
        }
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oReturnVal;

}



t_CxDxAPIRetVal  Cx_ValidateLIR(HCXAPPCONTEXT   hAppContext_i,
								t_CxLIR*        poLIR_i,
								DMBase_SList**  ppoFailedAVPsList_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o || NULL == poLIR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    static t_CxAVPValidationInfo oLIRAVPs[] = {	{DMBASE_AVP_SESSION_ID,TRUE,1},
												{DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,TRUE,1},
												{DMBASE_AVP_AUTH_SESSION_STATE,TRUE,1},
												{DMBASE_AVP_ORIGIN_HOST,TRUE,1},
												{DMBASE_AVP_ORIGIN_REALM,TRUE,1},
												{DMBASE_AVP_DESTINATION_HOST,FALSE,1},
												{DMBASE_AVP_DESTINATION_REALM,FALSE,1},
												{CX_AVP_ORIGINATING_REQUEST,FALSE,1},
												{CX_AVP_SUPPORTED_FEATURES,FALSE,200},
												{CX_AVP_PUBLIC_IDENTITY,TRUE,1},
												{DMBASE_AVP_PROXY_INFO,FALSE,200},
												{DMBASE_AVP_ROUTE_RECORD,FALSE,200}
												};


    t_CxDxAPIRetVal oReturnVal;
    oReturnVal = Cx_CheckMissingAVPs(hAppContext_i,oLIRAVPs,
									sizeof(oLIRAVPs)/sizeof(t_CxAVPValidationInfo),
									ppoFailedAVPsList_o);

    if(CxDx_Retval_Success == oReturnVal)
    {
    	//Check for duplication
    	oReturnVal = Cx_CheckDuplicateAVPs(hAppContext_i, oLIRAVPs,
							sizeof(oLIRAVPs)/sizeof(t_CxAVPValidationInfo), ppoFailedAVPsList_o);
    }
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oReturnVal;
}


t_CxDxAPIRetVal Cx_CheckMissingAVPs(HCXAPPCONTEXT   		hAppContext_i,
									t_CxAVPValidationInfo 	oMandatoryAVPs_i[],
									unsigned int    		unAVPCount_i,
									DMBase_SList**  		ppoFailedAVPsList_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o)
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " NULL pointer\n ");
    	return DMBase_Retval_Null_Pointer;
    }
    DMBase_SList*   poFailedAVPsList = NULL;

    unsigned int unIndex = 0;
    t_CxAVPValidationInfo *poAVPInfo = NULL;

    for(; unIndex < unAVPCount_i; unIndex++)
    {
    	if(oMandatoryAVPs_i[unIndex].bMandatory)
    	{
			if(TRUE != hAppContext_i->oAVPMetadata[oMandatoryAVPs_i[unIndex].unAVPCode].bIsPresent)
			{
				poAVPInfo = NULL;
				if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext, (void**)&poAVPInfo, sizeof(t_CxAVPValidationInfo)))
				{
					DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to allocate memory\n ");
					return DMBase_Retval_Failure;
				}
				poAVPInfo->unAVPCode = oMandatoryAVPs_i[unIndex].unAVPCode;
				DMBase_AppendSList(hDiamStackContext, &poFailedAVPsList, (void*)poAVPInfo);
			}
    	}
    }

    if(NULL == poFailedAVPsList)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
        return CxDx_Retval_Success;
    }

    *ppoFailedAVPsList_o = poFailedAVPsList;
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_MoreInfoRequired;
}


t_CxDxAPIRetVal Cx_CheckDuplicateAVPs(	HCXAPPCONTEXT   		hAppContext_i,
										t_CxAVPValidationInfo 	oMandatoryAVPs_i[],
										unsigned int    		unAVPCount_i,
										DMBase_SList**  		ppoFailedAVPsList_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == ppoFailedAVPsList_o)
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " NULL pointer\n ");
    	return DMBase_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    DMBase_SList*   poFailedAVPsList = NULL;

    unsigned int unIndex = 0;
    t_CxAVPValidationInfo *poAVPInfo = NULL;

    for(; unIndex < unAVPCount_i; unIndex++)
    {
    	if(1 == oMandatoryAVPs_i[unIndex].unMaxAllowed)
    	{
			if(hAppContext_i->oAVPMetadata[oMandatoryAVPs_i[unIndex].unAVPCode].unAVPCount > oMandatoryAVPs_i[unIndex].unMaxAllowed)
			{
				poAVPInfo = NULL;
				if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext, (void**)&poAVPInfo, sizeof(t_CxAVPValidationInfo)))
				{
					DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to allocate memory\n ");
					return DMBase_Retval_Failure;
				}
				poAVPInfo->unAVPCode = oMandatoryAVPs_i[unIndex].unAVPCode;
				DMBase_AppendSList(hDiamStackContext, &poFailedAVPsList, (void*)poAVPInfo);
			}
    	}
    }

    if(NULL == poFailedAVPsList)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
        return CxDx_Retval_Success;
    }

    *ppoFailedAVPsList_o = poFailedAVPsList;
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Limit_Exceed;
}


t_CxDxAPIRetVal  Cx_AddExpectedAVPToList(	HCXAPPCONTEXT 			hAppContext_i,
											HMSGALLOCATIONCONTEXT 	hMsgAllocContext_io,
											unsigned int 			unAVPCode_i,
											DMBase_SList** 			ppoFailedAVPsList_o)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == ppoFailedAVPsList_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    switch(unAVPCode_i)
    {
		case DMBASE_AVP_SESSION_ID:
		{
			unsigned char ucSessionId[] = "223DAq11A12";
			HAVP hSessionIdAVP = NULL;
			DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID,
										&hSessionIdAVP, ucSessionId, strlen(ucSessionId) , hMsgAllocContext_io);
			DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hSessionIdAVP);
		}
		break;
		case DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID:
		{
			HAVP hVendorAVP = NULL;
			Cx_CreateVendorAppAVP(hAppContext_i,&hVendorAVP,hMsgAllocContext_io);
			DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hVendorAVP);
		}
		break;
        case DMBASE_AVP_AUTH_SESSION_STATE:
        {
            HAVP hAuthSessionState = NULL;
            DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_AUTH_SESSION_STATE,
                    &hAuthSessionState, DMBASE_NO_STATE_MAINTAINED, hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hAuthSessionState);
        }
        break;
        case DMBASE_AVP_ORIGIN_HOST:
        {
            unsigned char ucOrigionHost[] = "seagull.ims.hpintelco.org";
            HAVP hOrigionHostAVP = NULL;
            DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_ORIGIN_HOST,
                                        &hOrigionHostAVP, ucOrigionHost, strlen(ucOrigionHost) , hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hOrigionHostAVP);
        }
            break;
        case DMBASE_AVP_ORIGIN_REALM:
        {
            unsigned char ucOrigionRealm[] = "ims.hpintelco.org";
            HAVP hOrigionRealmAVP = NULL;
            DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_ORIGIN_REALM,
                                        &hOrigionRealmAVP, ucOrigionRealm, strlen(ucOrigionRealm) , hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hOrigionRealmAVP);
        }
            break;
        case CX_AVP_PUBLIC_IDENTITY:
        {
            unsigned char ucPublicID[] = "sip:nasir@voip.com";
            HAVP hPublicIdAVP = NULL;
            DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_PUBLIC_IDENTITY,
                                        &hPublicIdAVP, ucPublicID, strlen(ucPublicID) , hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hPublicIdAVP);
        }
            break;
        case CX_AVP_VISITED_NETWORK_IDENTIFIER:
        {
            unsigned char ucVNetID[] = "44222333";
            HAVP hVNetIDAVP = NULL;
            DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_VISITED_NETWORK_IDENTIFIER,
                                        &hVNetIDAVP, ucVNetID, strlen(ucVNetID) , hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hVNetIDAVP);
        }
            break;
        case DMBASE_AVP_USER_NAME:
        {
            unsigned char ucUserName[] = "asif@bigco.com";
            HAVP hUserNameAVP = NULL;
            DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_USER_NAME,
                                        &hUserNameAVP, ucUserName, strlen(ucUserName) , hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hUserNameAVP);
        }
        break;
        case CX_AVP_SERVER_NAME:
        {
            unsigned char ucServerName[] = "sip:server@domain.com";
            HAVP hServerName = NULL;
            DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SERVER_NAME,
                                        &hServerName, ucServerName, strlen(ucServerName), hMsgAllocContext_io );
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hServerName);
        }
        break;
        case CX_AVP_SIP_NUMBER_AUTH_ITEMS:
        {
            HAVP hSIPNumAuthItems = NULL;
            DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_SIP_NUMBER_AUTH_ITEMS,
											&hSIPNumAuthItems, 3, hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hSIPNumAuthItems);
        }
        break;
        case CX_AVP_SIP_AUTH_DATA_ITEM:
        {
            HAVP hSIPAuthDataItem = NULL;
            HAVP hSIPAuthScheme = NULL;
            unsigned char ucSipAuthScheme[] = "Digest-AKAv1-MD5";
            DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SIP_AUTHENTICATION_SCHEME,
                                        &hSIPAuthScheme, ucSipAuthScheme, strlen(ucSipAuthScheme) , hMsgAllocContext_io);

            DMBase_CreateGroupedAVP(hDiamStackContext,&hSIPAuthDataItem, hMsgAllocContext_io, CX_AVP_SIP_AUTH_DATA_ITEM, 0);
            DMBase_AddAVPToGroup(hDiamStackContext, hSIPAuthDataItem, hSIPAuthScheme);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hSIPAuthDataItem);
        }
        break;
        case CX_AVP_SIP_AUTHENTICATION_SCHEME:
        {
            HAVP hSIPAuthScheme = NULL;
            unsigned char ucSipAuthScheme[] = "Digest-AKAv1-MD5";
            DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SIP_AUTHENTICATION_SCHEME,
                                        &hSIPAuthScheme, ucSipAuthScheme, strlen(ucSipAuthScheme), hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hSIPAuthScheme);
        }
        break;
        case CX_AVP_USER_DATA_ALREADY_AVAILABLE:
        {
            HAVP hUserDataAlreadyAvail = NULL;
            DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_USER_DATA_ALREADY_AVAILABLE,
                    &hUserDataAlreadyAvail, USER_DATA_ALREADY_AVAILABLE, hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hUserDataAlreadyAvail);
        }
        break;
        case CX_AVP_SERVER_ASSIGNMENT_TYPE:
        {
            HAVP hSerAsignType = NULL;
            DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_SERVER_ASSIGNMENT_TYPE,
                    &hSerAsignType, SA_RE_REGISTRATION, hMsgAllocContext_io);
            DMBase_AppendSList(hDiamStackContext, ppoFailedAVPsList_o, (void*)hSerAsignType);
        }
        break;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}



t_CxDxAPIRetVal      Cx_GeneratePermanentFailure(HCXAPPCONTEXT 			hAppContext_i,
                                                 HPEERTABLEENTRY hPeer, HDIAMETERMESSAGE hDiamMsgRcv_i,
                                                 unsigned int unResCode, DMBase_SList* poFailedAVPs_i )
{

    unsigned int unCommandCode = 0;
    HDIAMETERSTACKCONTEXT hDiameterStackContext_io = hAppContext_i->hDiamStackContext;

     if (DMBase_Retval_Success !=  DMBase_GetDiaMsgCmdCodeFromDiamMsg(hDiameterStackContext_io,
									   hDiamMsgRcv_i,&unCommandCode)
	  )
	  {
		  DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_GetDiaMsgCmdCodeFromDiamMsg Failed , Leaving \n");
		  return DMBase_Retval_Failure;
	  }

    unsigned int unAppID = hDiamMsgRcv_i->oDiaHeader.unApplicationID;
    unsigned char ucFlags = hDiamMsgRcv_i->oDiaHeader.ucFlags;

    HDIAMETERMESSAGE hAnsMsg = NULL;
    DMBase_SList*	pAVPList = NULL;
    HAVP hTempAVP = NULL;

    DMBase_CreateDiameterMessage(hDiameterStackContext_io, &hAnsMsg, 1, unCommandCode, unAppID , 0, hDiamMsgRcv_i->oDiaHeader.unHopByHopID, hDiamMsgRcv_i->oDiaHeader.unEndToEndID);
    hAnsMsg->oDiaHeader.ucFlags = ucFlags;
    DMBase_SetDiaMsgRFlag(hDiameterStackContext_io, hAnsMsg,0);

	 HMSGALLOCATIONCONTEXT	hMsgAllocCtxt = NULL;
	 if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiameterStackContext_io,
			 hAnsMsg,&hMsgAllocCtxt)
	  )
	  {
		  DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		  return DMBase_Retval_Failure;
	  }

    HAVP hAVPSessionID = NULL;
    DMBase_GetAVP(hDiameterStackContext_io, DMBASE_AVP_SESSION_ID, hDiamMsgRcv_i, &hAVPSessionID);
    if(NULL != hAVPSessionID)
    {
        t_DiamOctetString* poSessionID = (t_DiamOctetString*)hAVPSessionID->pData;
        DMBase_CreateOctetStringAVP(hDiameterStackContext_io, DMBASE_AVP_SESSION_ID,
                &hTempAVP, poSessionID->pucOctetString, poSessionID->unNumOfOctets,hMsgAllocCtxt);
        DMBase_AppendSList(hDiameterStackContext_io, &pAVPList, (void*)hTempAVP);

    }

    hTempAVP = NULL;
    DMBase_CreateUTF8StringAVP(hDiameterStackContext_io,
            DMBASE_AVP_ORIGIN_HOST,
            &hTempAVP,
            hDiameterStackContext_io->oHostName.pucOctetString,
            hDiameterStackContext_io->oHostName.unNumOfOctets ,hMsgAllocCtxt);
    DMBase_AppendSList(hDiameterStackContext_io, &pAVPList, (void*)hTempAVP);

    hTempAVP = NULL;
    DMBase_CreateUTF8StringAVP(hDiameterStackContext_io,
            DMBASE_AVP_ORIGIN_REALM,
            &hTempAVP,
            hDiameterStackContext_io->oRealmName.pucOctetString,
            hDiameterStackContext_io->oRealmName.unNumOfOctets ,hMsgAllocCtxt );
    DMBase_AppendSList(hDiameterStackContext_io, &pAVPList, (void*)hTempAVP);

    hTempAVP = NULL;
    DMBase_CreateEnumAVP(hDiameterStackContext_io, DMBASE_AVP_RESULT_CODE, &hTempAVP, unResCode,hMsgAllocCtxt);
    DMBase_AppendSList(hDiameterStackContext_io, &pAVPList, (void*)hTempAVP);


    HAVPCOLLECTION hCollOfFailedAVPs = NULL;
    if(NULL != poFailedAVPs_i)
    {
    	DMBase_SList* poTempAVPsList = poFailedAVPs_i;
    	while(NULL != poTempAVPsList)
    	{
            HAVP hFailedAVP = NULL;
            t_CxAVPValidationInfo* poAVPInfo = NULL;
            DMBase_SListGetNodeData(hDiameterStackContext_io, poTempAVPsList, (void**)&poAVPInfo);
            Cx_AddExpectedAVPToList(hAppContext_i, hMsgAllocCtxt, poAVPInfo->unAVPCode, &hCollOfFailedAVPs);
            DMBase_SListGetNextNode(hDiameterStackContext_io, poTempAVPsList, &poTempAVPsList);
    	}

    }

    if(NULL != hCollOfFailedAVPs)
    {
        hTempAVP = NULL;
        DMBase_CreateGroupedAVP(hDiameterStackContext_io,&hTempAVP,hMsgAllocCtxt, DMBASE_AVP_FAILED_AVP, 0);

        DMBase_SList* poTempAVPsList = hCollOfFailedAVPs;

        for(; NULL != poTempAVPsList; DMBase_SListGetNextNode(hDiameterStackContext_io, poTempAVPsList, &poTempAVPsList))
        {
            HAVP hFailedAVP = NULL;
            DMBase_SListGetNodeData(hDiameterStackContext_io, poTempAVPsList, (void**)&hFailedAVP);
            DMBase_AddAVPToGroup(hDiameterStackContext_io, hTempAVP, hFailedAVP);
        }

        DMBase_AppendSList(hDiameterStackContext_io, &pAVPList, (void*)hTempAVP);
    }

    DMBase_AddAVPCollectionToDiameterMessage(hDiameterStackContext_io, hAnsMsg, pAVPList);

    DMBase_SendMessageToPeer(hDiameterStackContext_io, hPeer, hAnsMsg);
    DMBase_DestroyDiaMessage(hDiameterStackContext_io, &hAnsMsg);
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}


int Cx_FreeAVPValidationDataCallback(void** ppvData)
{
    DMBase_Free(NULL,ppvData);
    return AMPS_SUCCESS;
}


