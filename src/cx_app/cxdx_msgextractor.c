
#include "cxdx_msgextractor.h"
#include "dmbase_parser.h"
#include "hss.h"

extern int g_nDiamBaseTraceID ;


t_CxDxAPIRetVal Cx_ExtractAppMsgAttributes(	HCXAPPCONTEXT  		hAppContext_i,
												HDIAMETERMESSAGE    hDiaMsg_i,
												void**				ppAppMsg_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext || NULL == hDiaMsg_i || NULL == ppAppMsg_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;

	unsigned int unCommandCode = 0;//DMBase_GetCmdCodeFromMsg(hDiaMsg_i);
	DMBase_GetDiaMsgCmdCodeFromDiamMsg(hDiamStackContext, hDiaMsg_i, &unCommandCode);

	unsigned int unAppMsgSize = 0;

    switch(unCommandCode)
    {
        case CXDX_MSG_CMD_CODE_UAR:
        	unAppMsgSize = sizeof(t_CxUAR);
            break;
        case CXDX_MSG_CMD_CODE_SAR:
        	unAppMsgSize = sizeof(t_CxSAR);
            break;
        case CXDX_MSG_CMD_CODE_LIR:
        	unAppMsgSize = sizeof(t_CxLIR);
            break;
        case CXDX_MSG_CMD_CODE_MAR:
        	unAppMsgSize = sizeof(t_CxMAR);
            break;
        default:
        	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Invalid Command Code\n ");
        	return CxDx_Retval_Not_Supported;
    }

    if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext, ppAppMsg_o, unAppMsgSize))
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Memory allocation failed\n ");
    	return CxDx_Retval_Failure;
    }

    DMBase_MemsetZero(hAppContext_i->oAVPMetadata, sizeof(t_CxAVPMetadata)*CX_MAX_AVP_META_DATA);

    t_CxDxAPIRetVal oReturnVal;

    switch(unCommandCode)
    {
        case CXDX_MSG_CMD_CODE_UAR:
        	oReturnVal = Cx_ExtractUARAttributes(hAppContext_i,
												 hDiaMsg_i,
												 (t_CxUAR*)*ppAppMsg_o);
            break;
        case CXDX_MSG_CMD_CODE_SAR:
        	oReturnVal = Cx_ExtractSARAttributes(hAppContext_i,
												 hDiaMsg_i,
												 (t_CxSAR*)*ppAppMsg_o);
            break;
        case CXDX_MSG_CMD_CODE_LIR:
        	oReturnVal = Cx_ExtractLIRAttributes(hAppContext_i,
												 hDiaMsg_i,
												 (t_CxLIR*)*ppAppMsg_o);
            break;
        case CXDX_MSG_CMD_CODE_MAR:
        	oReturnVal = Cx_ExtractMARAttributes(hAppContext_i,
												 hDiaMsg_i,
												 (t_CxMAR*)*ppAppMsg_o);
            break;
    }

    if(CxDx_Retval_Success != oReturnVal)
    {
    	DMBase_Free(hDiamStackContext, ppAppMsg_o);
    	*ppAppMsg_o = NULL;
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Failed to extract message\n ");
    	return oReturnVal;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Success;
}


t_CxDxAPIRetVal Cx_ExtractVendorAppAttribute(HCXAPPCONTEXT  hAppContext_i,
                                                HAVP           hVendorAppAVP_i,
                                                t_CxVendorApp *poVendorApp_o)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext ||
                                NULL == hVendorAppAVP_i || NULL == poVendorApp_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    DMBase_SList *poAVPsList = hVendorAppAVP_i->hCollectionOfAVPs;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;
    _Bool bIsVendorIdExist = FALSE;
    _Bool bIsAppIdExist = FALSE;

    while(NULL != poAVPsList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPsList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }
        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case DMBASE_AVP_VENDOR_ID:
                poVendorApp_o->unVendorID = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
                bIsVendorIdExist = TRUE;
                break;
            case DMBASE_AVP_AUTH_APPLICATION_ID:
            case DMBASE_AVP_ACCT_APPLICATION_ID:
                poVendorApp_o->unApplicationID = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
                bIsAppIdExist = TRUE;
                break;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPsList, &poAVPsList);
    }

    t_CxDxAPIRetVal oAVPsExist = (bIsAppIdExist && bIsVendorIdExist ? CxDx_Retval_Success : CxDx_Retval_Failure);
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oAVPsExist;



}

t_CxDxAPIRetVal Cx_ExtractSIPAuthDataItems(HCXAPPCONTEXT  hAppContext_i,
                                              HAVP           hSIPAuthDataItemAVP_i,
                                              unsigned char*  pucAuthScheme)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext ||
                                NULL == hSIPAuthDataItemAVP_i || NULL == pucAuthScheme)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    DMBase_SList *poAVPsList = hSIPAuthDataItemAVP_i->hCollectionOfAVPs;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;
    _Bool bSipAuthSchemeExist = FALSE;

    while(NULL != poAVPsList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPsList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }
        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case CX_AVP_SIP_AUTHENTICATION_SCHEME:
                DMBase_CopyDiamStrToNSizedCharArray(pucAuthScheme,
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                bSipAuthSchemeExist = TRUE;
                break;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPsList, &poAVPsList);
    }

    t_CxDxAPIRetVal oAVPExist = (bSipAuthSchemeExist == TRUE ? CxDx_Retval_Success : CxDx_Retval_Failure);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return oAVPExist;

}


t_CxDxAPIRetVal Cx_ExtractUARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxUAR*            poUAR_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == hDiaMsg_i ||NULL == poUAR_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    DMBase_SList *poAVPList = hDiaMsg_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;
    _Bool bIsExist = TRUE;

    DMBase_GetDiaMsgFlagOctetFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poUAR_o->ucFlags);
    DMBase_GetHopByHopIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poUAR_o->unHopByHopID);
    DMBase_GetEndToEndIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poUAR_o->unEndToEndID);

    poUAR_o->unAuthType = -1;
    unsigned int unAVPPosition = 0;
    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }


        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case DMBASE_AVP_USER_NAME:
                DMBase_CopyDiamStrToNSizedCharArray(poUAR_o->oUARDBRequest.ucUserName,
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;

            case DMBASE_AVP_SESSION_ID:
                DMBase_CopyDiamStrToNSizedCharArray(poUAR_o->oUARDBRequest.ucSessionID,
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;

            case DMBASE_AVP_AUTH_SESSION_STATE:
                poUAR_o->unAuthSessionState = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;

            case DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID:
               if(CxDx_Retval_Success != Cx_ExtractVendorAppAttribute(hAppContext_i, hAVP, &poUAR_o->oVendorSpecificApp))
               {
            	   bIsExist = FALSE;
               }
                break;

            case CX_AVP_PUBLIC_IDENTITY:
                DMBase_CopyDiamStrToNSizedCharArray(poUAR_o->oUARDBRequest.ucPublicIdentity,
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;

            case CX_AVP_USER_AUTHORIZATION_TYPE:
                poUAR_o->unAuthType =*((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;

            case CX_AVP_VISITED_NETWORK_IDENTIFIER:
                DMBase_CopyDiamStrToNSizedCharArray(poUAR_o->ucVisitedNetworkId,
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;

        }

        unAVPPosition++;
        if(bIsExist)
        {
        	Cx_SetAVPMetaData(&hAppContext_i->oAVPMetadata[hAVP->oAVPHeaderInst.unAVPCode], unAVPPosition);
        }
        else
        {
        	bIsExist = TRUE;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal Cx_ExtractSARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxSAR*            poSAR_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == hDiaMsg_i ||NULL == poSAR_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }


    DMBase_SList *poAVPList = hDiaMsg_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;

    unsigned int unAVPPosition = 0;
    _Bool bIsExist = TRUE;

    DMBase_GetDiaMsgFlagOctetFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poSAR_o->ucFlags);
    DMBase_GetHopByHopIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poSAR_o->unHopByHopID);
    DMBase_GetEndToEndIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poSAR_o->unEndToEndID);

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case DMBASE_AVP_SESSION_ID:
                DMBase_CopyDiamStrToNSizedCharArray(poSAR_o->oSARDBRequest.ucSessionID,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID:
                if(CxDx_Retval_Success != Cx_ExtractVendorAppAttribute(hAppContext_i, hAVP, &poSAR_o->oVendorSpecificApp))
                {
             	   bIsExist = FALSE;
                }
                break;
            case DMBASE_AVP_AUTH_SESSION_STATE:
                poSAR_o->unAuthSessionState = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
            case DMBASE_AVP_USER_NAME:
                DMBase_CopyDiamStrToNSizedCharArray(poSAR_o->oSARDBRequest.ucUserName,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case CX_AVP_PUBLIC_IDENTITY:
                if(poSAR_o->oSARDBRequest.unPublicUserIdCount < CX_MAX_PUBLIC_USER_ID)
                {
                    DMBase_CopyDiamStrToNSizedCharArray(
                     poSAR_o->oSARDBRequest.ucPublicIdentity[poSAR_o->oSARDBRequest.unPublicUserIdCount++],
                                CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                }
                break;
            case CX_AVP_SERVER_NAME:
                DMBase_CopyDiamStrToNSizedCharArray(poSAR_o->oSARDBRequest.ucSCSCFName,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case CX_AVP_SERVER_ASSIGNMENT_TYPE:
                poSAR_o->oSARDBRequest.unServerAssignmentType = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
            case CX_AVP_USER_DATA_ALREADY_AVAILABLE:
                poSAR_o->oSARDBRequest.unUserDataAvailable = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
        }

        unAVPPosition++;
        if(bIsExist)
        {
        	Cx_SetAVPMetaData(&hAppContext_i->oAVPMetadata[hAVP->oAVPHeaderInst.unAVPCode], unAVPPosition);
        }
        else
        {
        	bIsExist = TRUE;
        }

        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal Cx_ExtractMARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxMAR*            poMAR_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == hDiaMsg_i ||NULL == poMAR_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    DMBase_SList *poAVPList = hDiaMsg_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;
    _Bool bIsExist = TRUE;
    unsigned int unAVPPosition = 0;

    DMBase_GetDiaMsgFlagOctetFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poMAR_o->ucFlags);
    DMBase_GetHopByHopIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poMAR_o->unHopByHopID);
    DMBase_GetEndToEndIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poMAR_o->unEndToEndID);

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occures while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case DMBASE_AVP_SESSION_ID:
                DMBase_CopyDiamStrToNSizedCharArray(poMAR_o->oMARDBRequest.ucSessionID,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID:
                if(CxDx_Retval_Success != Cx_ExtractVendorAppAttribute(hAppContext_i, hAVP, &poMAR_o->oVendorSpecificApp))
                {
             	   bIsExist = FALSE;
                }
                break;
            case DMBASE_AVP_AUTH_SESSION_STATE:
                poMAR_o->unAuthSessionState = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
            case DMBASE_AVP_USER_NAME:
                DMBase_CopyDiamStrToNSizedCharArray(poMAR_o->oMARDBRequest.ucUserName,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case CX_AVP_PUBLIC_IDENTITY:
                DMBase_CopyDiamStrToNSizedCharArray(poMAR_o->oMARDBRequest.ucPublicIdentity,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case CX_AVP_SIP_NUMBER_AUTH_ITEMS:
                poMAR_o->unSIPNumnerAuthItems = *((t_DiamUnsigned32*)hAVP->pData)->punUnsigned32;
                break;
            case CX_AVP_SIP_AUTH_DATA_ITEM:
                if(CxDx_Retval_Success != Cx_ExtractSIPAuthDataItems(hAppContext_i, hAVP, poMAR_o->oMARDBRequest.ucAuthScheme))
                {
             	   bIsExist = FALSE;
                }
                break;
            case CX_AVP_SERVER_NAME:
                DMBase_CopyDiamStrToNSizedCharArray(poMAR_o->oMARDBRequest.ucSCSCFName,
                                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
        }

        unAVPPosition++;
        if(bIsExist)
        {
        	Cx_SetAVPMetaData(&hAppContext_i->oAVPMetadata[hAVP->oAVPHeaderInst.unAVPCode], unAVPPosition);
        }
        else
        {
        	bIsExist = TRUE;
        }
        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal Cx_ExtractLIRAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxLIR*            poLIR_o)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == hDiaMsg_i ||NULL == poLIR_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    DMBase_SList *poAVPList = hDiaMsg_i->hCollectionOfAVPs;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    HAVP hAVP = NULL;
    t_DiamAPIReturnVal oReturnValue;
    poLIR_o->unOriginatingRequest = 1;
    _Bool bIsExist = TRUE;
    unsigned int unAVPPosition = 0;

    DMBase_GetDiaMsgFlagOctetFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poLIR_o->ucFlags);
    DMBase_GetHopByHopIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poLIR_o->unHopByHopID);
    DMBase_GetEndToEndIDFromDiamMsg(hDiamStackContext, hDiaMsg_i, &poLIR_o->unEndToEndID);

    while(NULL != poAVPList)
    {
        hAVP = NULL;
        oReturnValue = DMBase_SListGetNodeData(hDiamStackContext, poAVPList, (void**)&hAVP);
        if(DMBase_Retval_Success != oReturnValue)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
                        " Problem occurs while getting data from DMBase_SList Node\n ");
            return CxDx_Retval_Failure;
        }

        switch(hAVP->oAVPHeaderInst.unAVPCode)
        {
            case DMBASE_AVP_SESSION_ID:
                DMBase_CopyDiamStrToNSizedCharArray(poLIR_o->oLIRDBRequest.ucSessionID,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID:
                if(CxDx_Retval_Success != Cx_ExtractVendorAppAttribute(hAppContext_i, hAVP, &poLIR_o->oVendorSpecificApp))
                {
             	   bIsExist = FALSE;
                }
                break;
            case DMBASE_AVP_AUTH_SESSION_STATE:
                poLIR_o->unAuthSessionState = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
            case CX_AVP_PUBLIC_IDENTITY:
                DMBase_CopyDiamStrToNSizedCharArray(poLIR_o->oLIRDBRequest.ucPublicIdentity,
                            CX_LARGE_STRING, *((t_DiamUTF8String*)hAVP->pData));
                break;
            case CX_AVP_ORIGINATING_REQUEST:
                poLIR_o->unOriginatingRequest = *((t_DiamEnum*)hAVP->pData)->pnInteger32;
                break;
        }

        unAVPPosition++;
        if(bIsExist)
        {
        	Cx_SetAVPMetaData(&hAppContext_i->oAVPMetadata[hAVP->oAVPHeaderInst.unAVPCode], unAVPPosition);
        }
        else
        {
        	bIsExist = TRUE;
        }
        DMBase_SListGetNextNode(hDiamStackContext, poAVPList, &poAVPList);
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
    return CxDx_Retval_Success;
}

void Cx_SetAVPMetaData(t_CxAVPMetadata* poAVPMetaData_i, unsigned int unPosition_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering\n ");

	poAVPMetaData_i->bIsPresent = TRUE;
	poAVPMetaData_i->unAVPCount++;
	if(0 == poAVPMetaData_i->usAVPPosition)
	{
		poAVPMetaData_i->usAVPPosition = unPosition_i;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n ");
}
