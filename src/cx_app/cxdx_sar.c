
#include "cxdx_sar.h"
#include "cxdx_session.h"
#include "dmbase_parser.h"
#include "cxdx_app.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "cxdx_msg_api.h"
#include "hss.h"

extern	int	g_nDiamBaseTraceID ;

t_CxDxAPIRetVal Cx_ProcessSAR(	HCXAPPCONTEXT 	hAppContext_i,
								t_CxSAR*		poSAR_i,
								HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    t_CxSessionObject *poSessionObj = NULL;

    if(DMBase_Retval_Success != DMBase_Malloc(hAppContext_i->hDiamStackContext,
									(void**)&poSessionObj,sizeof(t_CxSessionObject)))
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create session object\n");
    	return CxDx_Retval_Failure;
    }
    poSessionObj->hAppContext = hAppContext_i;
    poSessionObj->hPeer = hPeer_i;
    poSessionObj->poData = (void*)poSAR_i;

    if(CxDx_Retval_Success != Cx_InsertSessionObject(hAppContext_i,
								poSAR_i->oSARDBRequest.ucSessionID, poSessionObj))
    {
    	DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&poSessionObj);
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to Insert session object\n");
    	return CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success != Cx_GenerateSARDBRequest(hAppContext_i, poSAR_i))
    {
    	//Delete Session Table Entry
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Event generation failed\n");
        return DMBase_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}

t_CxDxAPIRetVal Cx_GenerateSARDBRequest(HCXAPPCONTEXT hAppContext_i, t_CxSAR *poSAR_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


    //Return value check
    Cx_GenerateDBRequest(	hAppContext_i,
                            CX_SAR_DATABASE_REQUEST_EVENT,
                            (void*)&poSAR_i->oSARDBRequest,
                            sizeof(t_CxSARDBRequest));

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}


t_CxDxAPIRetVal	Cx_ProcessSARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char*	pucEventData_i,
										unsigned int 	unEventDataSize_i)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == pucEventData_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    t_CxSARDBResponse* poSARDBResponse = (t_CxSARDBResponse*)pucEventData_i;
    t_CxSessionObject* poSessionObject = NULL;

    if(CxDx_Retval_Success != Cx_LookUpSessionObject(hAppContext_i, poSARDBResponse->ucSessionID, (void**)&poSessionObject) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No Session object found\n ");
        return CxDx_Retval_Failure;
    }

    t_CxSAR* poSAR = (t_CxSAR*)poSessionObject->poData;

    if(NULL == poSAR)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No SAR data in session object\n ");
        return CxDx_Retval_Null_Pointer;
    }

    t_CxDxAPIRetVal oReturnValue = CxDx_Retval_Success;
    t_CxSAA oSAA;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    unEventDataSize_i -= 8;
    unsigned int unUserProfileSize = unEventDataSize_i - sizeof(t_CxSARDBResponse);

    unsigned char *pucUserProfile = NULL;
    if(0 < unUserProfileSize)
    {
    	pucUserProfile = pucEventData_i + sizeof(t_CxSARDBResponse);
    }

    DMBase_MemsetZero(&oSAA, sizeof(t_CxSAA));


    oSAA.pucSessionID = poSAR->oSARDBRequest.ucSessionID;

    int nIMPUCount = 0;

    if(DB_RESULT_NO_PRIVATE_ID == poSARDBResponse->unDBResult ||
    		DB_RESULT_NO_PUBLIC_ID == poSARDBResponse->unDBResult)
    {
    	oSAA.unExpResultCode = DIAMETER_ERROR_USER_UNKNOWN;
    }
    else if( DB_RESULT_NOT_FOUND == poSARDBResponse->unDBResult)
    {
    	oSAA.unExpResultCode = DIAMETER_ERROR_IDENTITIES_DONT_MATCH;
    }
    else if(DB_RESULT_TOO_MUCH_DATA == poSARDBResponse->unDBResult)
    {
    	oSAA.unResultCode = DIAMETER_AVP_OCCURS_TOO_MANY_TIMES;
    }
    else if(DB_RESULT_FAILURE == poSARDBResponse->unDBResult)
    {
    	oSAA.unResultCode = DIAMETER_UNABLE_TO_COMPLY;
    }
    else if(SA_REGISTRATION == poSAR->oSARDBRequest.unServerAssignmentType ||
    		SA_RE_REGISTRATION == poSAR->oSARDBRequest.unServerAssignmentType)
    {
        if(USER_DATA_NOT_AVAILABLE == poSAR->oSARDBRequest.unUserDataAvailable)
        {
        	oSAA.pucUserData = pucUserProfile;
        	oSAA.unUserDataLength = unUserProfileSize;

            if(0 < strlen(poSARDBResponse->ucPCCFuncName))
            {
            	oSAA.pucPCC = poSARDBResponse->ucPCCFuncName;
            }

            if(0 < strlen(poSARDBResponse->ucSCCFuncName))
            {
            	oSAA.pucSCC = poSARDBResponse->ucSCCFuncName;
            }
            if(0 < strlen(poSARDBResponse->ucPECFuncName))
            {
            	oSAA.pucPEC = poSARDBResponse->ucPECFuncName;
            }
            if(0 < strlen(poSARDBResponse->ucSECFuncName))
            {
            	oSAA.pucSEC = poSARDBResponse->ucSECFuncName;
            }
        }

		oSAA.unResultCode = DIAMETER_SUCCESS;
    }
    else if(SA_UNREGISTERED_USER == poSAR->oSARDBRequest.unServerAssignmentType)
    {
    	oSAA.unResultCode = DIAMETER_SUCCESS;
    }
    else if(SA_TIMEOUT_DEREGISTRATION == poSAR->oSARDBRequest.unServerAssignmentType||
            SA_USER_DEREGISTRATION == poSAR->oSARDBRequest.unServerAssignmentType||
            SA_DEREGISTRATION_TOO_MUCH_DATA == poSAR->oSARDBRequest.unServerAssignmentType||
            SA_ADMINISTRATIVE_DEREGISTRATION == poSAR->oSARDBRequest.unServerAssignmentType)
    {
    	oSAA.unResultCode = DIAMETER_SUCCESS;
    }
    else if(SA_TIMEOUT_DEREGISTRATION_STORE_SERVER_NAME == poSAR->oSARDBRequest.unServerAssignmentType ||
            SA_USER_DEREGISTRATION_STORE_SERVER_NAME == poSAR->oSARDBRequest.unServerAssignmentType)
    {
    	oSAA.unExpResultCode = DIAMETER_SUCCESS_SERVER_NAME_NOT_STORED;
    }
    else if(SA_NO_ASSIGNMENT == poSAR->oSARDBRequest.unServerAssignmentType)
    {

        if(strcmp(poSAR->oSARDBRequest.ucSCSCFName, poSARDBResponse->ucSCSCFName) == 0)
        {
        	if(USER_DATA_NOT_AVAILABLE == poSAR->oSARDBRequest.unUserDataAvailable)
            {
        		oSAA.pucUserData = pucUserProfile;
        		oSAA.unUserDataLength = unUserProfileSize;
            }
        	oSAA.unResultCode = DIAMETER_SUCCESS;
        }
        else
        {
        	oSAA.unResultCode = DIAMETER_UNABLE_TO_COMPLY;
        }

    }
    else if(SA_AUTHENTICATION_FAILURE == poSAR->oSARDBRequest.unServerAssignmentType ||
            SA_AUTHENTICATION_TIMEOUT == poSAR->oSARDBRequest.unServerAssignmentType)
    {
    	oSAA.unResultCode = DIAMETER_SUCCESS;
    }

    if(0 < strlen(poSARDBResponse->ucUserName) && !(SA_UNREGISTERED_USER == poSAR->oSARDBRequest.unServerAssignmentType &&
                                                DB_RESULT_NO_PUBLIC_ID == poSARDBResponse->unDBResult))
    {
    	oSAA.pucUserName = poSARDBResponse->ucUserName;
    }

	//update oamps metrics
	if (DIAMETER_SUCCESS == oSAA.unResultCode )
	{
		(hAppContext_i->oCxPerfMgmtData.unsuccessfulSAAUR)++;
	}
	else
	{
		(hAppContext_i->oCxPerfMgmtData.unfailedSAAUR)++;
	}

    HDIAMETERMESSAGE hDiaMsgSAA = NULL;
    if(CxDx_Retval_Success != Cx_CreateSAADiameterMessage(hAppContext_i, &hDiaMsgSAA, poSAR, &oSAA))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Message\n ");
        oReturnValue = CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success == oReturnValue)
    {
    	DMBase_SendMessageToPeer(hDiamStackContext, poSessionObject->hPeer, hDiaMsgSAA);
    	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiaMsgSAA);
    }

    DMBase_Free(hDiamStackContext, (void**)&poSessionObject->poData);
    DMBase_Free(hDiamStackContext, (void**)&poSessionObject);

    Cx_RemoveSessionObject(hAppContext_i, poSARDBResponse->ucSessionID);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return oReturnValue;

}

t_CxDxAPIRetVal Cx_CreateSAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgSAA_io,
											t_CxSAR*			poSAR_i,
											t_CxSAA*			poSAA_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hCxAppContext_io || NULL == phDiaMsgSAA_io || NULL == poSAR_i || NULL == poSAA_i)
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Null pointer\n");
    	return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;
	DMBase_SList*	poAVPList = NULL;
	t_CxDxAPIRetVal oReturnValue = CxDx_Retval_Success;
    HAVP hAVP_VendorID = NULL;
    HAVP hAVP_Exp_Result = NULL;
    HAVP hTempAVP = NULL;
    HMSGALLOCATIONCONTEXT	hMsgAllocCtxt = NULL;
    unsigned char*	pucHostName = NULL;
    unsigned char*	pucRealmName = NULL;
    HDIAMETERMESSAGE hSAADiameterMsg = NULL;

    if(DMBase_Retval_Success != DMBase_CreateDiameterMessage(	hDiamStackContext,
																&hSAADiameterMsg,
																DIAMETER_VERSION_NUMBER,
																CxDx_MSG_CMD_CODE_SAA,
																CxDx_APPLICATION,
																poSAR_i->ucFlags,
																poSAR_i->unHopByHopID,
																poSAR_i->unEndToEndID))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create message\n");
		oReturnValue = CxDx_Retval_Failure;
		goto ExitMe;
    }

    DMBase_SetDiaMsgRFlag(hDiamStackContext, hSAADiameterMsg,0);


	if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiamStackContext,
																hSAADiameterMsg,&hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		return DMBase_Retval_Failure;
	}

	hTempAVP = NULL;
	if(NULL != poSAA_i->pucSessionID)
	{
	    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID, &hTempAVP,
												poSAA_i->pucSessionID, strlen(poSAA_i->pucSessionID) , hMsgAllocCtxt))
	    {
	        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
	        oReturnValue = CxDx_Retval_Failure;
	        goto ExitMe;
	    }
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
		oReturnValue = CxDx_Retval_Failure;
		goto ExitMe;
	}

     hTempAVP = NULL;
     if(CxDx_Retval_Success == (oReturnValue = Cx_CreateVendorAppAVP(hCxAppContext_io, &hTempAVP,
																	 hMsgAllocCtxt)))
     {
    	 DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP );
     }
     else
     {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
     	oReturnValue = CxDx_Retval_Failure;
      	goto ExitMe;
     }


     if(0 != poSAA_i->unResultCode)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
													 &hTempAVP, poSAA_i->unResultCode, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }
     else if(0 != poSAA_i->unExpResultCode)
     {

    	 HAVP hVendorIDAVP = NULL;
        if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hVendorIDAVP,hMsgAllocCtxt,
																DMBASE_AVP_EXPERIMENTAL_RESULT, 0))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnValue = CxDx_Retval_Failure;
          	goto ExitMe;
        }

    	hTempAVP = NULL;
        if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_VENDOR_ID,
																&hTempAVP, CxDx_VENDOR_IDENTIFIER, hMsgAllocCtxt))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnValue = CxDx_Retval_Failure;
          	goto ExitMe;
        }
        DMBase_AddAVPToGroup(hDiamStackContext, hVendorIDAVP, hTempAVP);

        hTempAVP = NULL;
    	if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_EXPERIMENTATION_RESULT_CODE,
														&hTempAVP, poSAA_i->unExpResultCode, hMsgAllocCtxt))
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
            oReturnValue = CxDx_Retval_Failure;
            goto ExitMe;
        }
    	DMBase_AddAVPToGroup(hDiamStackContext, hVendorIDAVP, hTempAVP);

        DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hVendorIDAVP);

     }

     hTempAVP = NULL;
     if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_AUTH_SESSION_STATE,
												&hTempAVP, DMBASE_NO_STATE_MAINTAINED, hMsgAllocCtxt))

     {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
         oReturnValue = CxDx_Retval_Failure;
         goto ExitMe;
     }
     DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);


     hTempAVP = NULL;
     DMBase_GetHostNameFromDiameterContext(hDiamStackContext, (char**)&pucHostName);
     if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext,
															 DMBASE_AVP_ORIGIN_HOST,
															 &hTempAVP,
															 pucHostName,
															 strlen(pucHostName),
															 hMsgAllocCtxt ))
     {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
         oReturnValue = CxDx_Retval_Failure;
         goto ExitMe;
     }
     DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);

     hTempAVP = NULL;
     DMBase_GetRealmNameFromDiameterContext(hDiamStackContext, (char**)&pucRealmName);

     if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext,
															 DMBASE_AVP_ORIGIN_REALM,
															 &hTempAVP,
															 pucRealmName,
															 strlen(pucRealmName) ,
															 hMsgAllocCtxt ))
     {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
         oReturnValue = CxDx_Retval_Failure;
         goto ExitMe;
     }
     DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);


     if(NULL != poSAA_i->pucUserName)
     {
    	 hTempAVP = NULL;
     	if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, DMBASE_AVP_USER_NAME, &hTempAVP,
											poSAA_i->pucUserName, strlen(poSAA_i->pucUserName), hMsgAllocCtxt))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }
     	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poSAA_i->pucUserData)
     {
    	 hTempAVP = NULL;
     	if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_USER_DATA, &hTempAVP,
     			poSAA_i->pucUserData, poSAA_i->unUserDataLength, hMsgAllocCtxt))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }
     	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poSAA_i->pucPCC || NULL != poSAA_i->pucPEC || NULL != poSAA_i->pucSCC || NULL != poSAA_i->pucSEC)
     {
    	 HAVP hUserChargingInfo = NULL;
     	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP(hDiamStackContext, &hUserChargingInfo, hMsgAllocCtxt,
                                                      CX_AVP_CHARGING_INFORMATION, 0))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }

         if(NULL != poSAA_i->pucPCC)
         {
             HAVP hAVP_PCC = NULL;
             if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_PRIMARY_CHARGING_COLLECTION_FUNCTION_NAME,
                                                 &hAVP_PCC, poSAA_i->pucPCC, strlen(poSAA_i->pucPCC), hMsgAllocCtxt))
             {
                 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
                 oReturnValue = CxDx_Retval_Failure;
                 goto ExitMe;
             }

             DMBase_AddAVPToGroup(hDiamStackContext, hUserChargingInfo, hAVP_PCC);
         }

         if(NULL != poSAA_i->pucSCC)
         {
             HAVP hAVP_SCC = NULL;
             if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_SECONDARY_CHARGING_COLLECTION_FUNCTION_NAME,
                                                   &hAVP_SCC, poSAA_i->pucSCC, strlen(poSAA_i->pucSCC), hMsgAllocCtxt))
             {
                 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
                 oReturnValue = CxDx_Retval_Failure;
                 goto ExitMe;
             }

             DMBase_AddAVPToGroup(hDiamStackContext, hUserChargingInfo, hAVP_SCC);
         }
         if(NULL != poSAA_i->pucPEC)
         {
             HAVP hAVP_PEC = NULL;
             if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_PRIMARY_EVENT_CHARGING_FUNCTION_NAME,
                                        &hAVP_PEC, poSAA_i->pucPEC, strlen(poSAA_i->pucPEC), hMsgAllocCtxt))
             {
                 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
                 oReturnValue = CxDx_Retval_Failure;
                 goto ExitMe;
             }

             DMBase_AddAVPToGroup(hDiamStackContext, hUserChargingInfo, hAVP_PEC);
         }
         if(NULL != poSAA_i->pucSEC)
         {
             HAVP hAVP_SEC = NULL;
             if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_SECONDARY_EVENT_CHARGING_FUNCTION_NAME,
                                          &hAVP_SEC, poSAA_i->pucSEC, strlen(poSAA_i->pucSEC), hMsgAllocCtxt))
             {
                 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
                 oReturnValue = CxDx_Retval_Failure;
                 goto ExitMe;
             }

             DMBase_AddAVPToGroup(hDiamStackContext, hUserChargingInfo, hAVP_SEC);
         }

         DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hUserChargingInfo);
     }

     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext, hSAADiameterMsg, poAVPList);

     *phDiaMsgSAA_io = hSAADiameterMsg;

     ExitMe:
	//if failed Destroy Message here
     if(CxDx_Retval_Failure == oReturnValue)
     {
    	 //Also Destroy List
    	 DMBase_DestroyDiaMessage(hDiamStackContext, &hSAADiameterMsg);
     }
     //free origin host and realm
     DMBase_Free(hDiamStackContext, (void**)&pucHostName);
     DMBase_Free(hDiamStackContext, (void**)&pucRealmName);


     DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

     return oReturnValue;
}



