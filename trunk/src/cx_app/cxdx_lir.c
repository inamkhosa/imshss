
#include "cxdx_lir.h"
#include "cxdx_session.h"
#include "dmbase_parser.h"
#include "cxdx_app.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "dmbase_utility.h"
#include "cxdx_msg_api.h"
#include "hss.h"

extern	int	g_nDiamBaseTraceID ;

t_CxDxAPIRetVal Cx_ProcessLIR(	HCXAPPCONTEXT hAppContext_i,
								t_CxLIR *poLIR_i,
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
    poSessionObj->poData = (void*)poLIR_i;

    if(CxDx_Retval_Success != Cx_InsertSessionObject(hAppContext_i,
								poLIR_i->oLIRDBRequest.ucSessionID, poSessionObj))
    {
    	DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&poSessionObj);
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to Insert session object\n");
    	return CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success != Cx_GenerateLIRDBRequest(hAppContext_i, poLIR_i))
    {
    	//Delete Session Table Entry
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Event generation failed\n");
        return DMBase_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}

t_CxDxAPIRetVal Cx_GenerateLIRDBRequest(HCXAPPCONTEXT hAppContext_i, t_CxLIR *poLIR_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


    //Return value check
    Cx_GenerateDBRequest(	hAppContext_i,
                            CX_LIR_DATABASE_REQUEST_EVENT,
                            (void*)&poLIR_i->oLIRDBRequest,
                            sizeof(t_CxLIRDBRequest));


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}


t_CxDxAPIRetVal	Cx_ProcessLIRDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
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

    t_CxLIRDBResponse* poLIRDBResponse = (t_CxLIRDBResponse*)pucEventData_i;
    t_CxSessionObject* poSessionObject = NULL;

    if(CxDx_Retval_Success != Cx_LookUpSessionObject(hAppContext_i, poLIRDBResponse->ucSessionID, (void**)&poSessionObject) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No Session object found\n ");
        return CxDx_Retval_Failure;
    }

    t_CxLIR* poLIR = (t_CxLIR*)poSessionObject->poData;

    if(NULL == poLIR)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No LIR data in session object\n ");
        return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    t_CxDxAPIRetVal	oReturnValue = CxDx_Retval_Success;
    t_CxLIA oLIA;

    DMBase_MemsetZero(&oLIA, sizeof(t_CxLIA));

    oLIA.pucSessionID = poLIR->oLIRDBRequest.ucSessionID;


    if(DB_RESULT_NO_PUBLIC_ID == poLIRDBResponse->unDBResult)
    {
    	oLIA.unExpResultCode = DIAMETER_ERROR_USER_UNKNOWN;
    }
    else if(DB_RESULT_FAILURE == poLIRDBResponse->unDBResult)
    {
    	oLIA.unResultCode = DIAMETER_UNABLE_TO_COMPLY;
    }
    else if(CX_USER_STATUS_REGISTERED == poLIRDBResponse->usRegisterStatus)
    {
    	oLIA.pucSCSCFName = poLIRDBResponse->ucSCSCFName;
    	oLIA.unResultCode = DIAMETER_SUCCESS;
    }
    else if(CX_USER_STATUS_UNREGISTERED == poLIRDBResponse->usRegisterStatus &&
            (1 == poLIRDBResponse->usHasUnRegisteredStateServices || 0 == poLIR->unOriginatingRequest))
    {
    	oLIA.pucSCSCFName = poLIRDBResponse->ucSCSCFName;
    	oLIA.unResultCode = DIAMETER_SUCCESS;
    }
    else if(CX_USER_STATUS_NOT_REGISTERED == poLIRDBResponse->usRegisterStatus &&
            (1 == poLIRDBResponse->usHasUnRegisteredStateServices || 0 == poLIR->unOriginatingRequest))
    {

        if(strlen(poLIRDBResponse->ucSCSCFName) > 0)
        {
        	oLIA.pucSCSCFName = poLIRDBResponse->ucSCSCFName;
        	oLIA.unResultCode = DIAMETER_SUCCESS;

        }
        else
        {
			if(0 < poLIRDBResponse->unMandatoryCapsCount )
			{
				oLIA.unMandatoryCapsCount = poLIRDBResponse->unMandatoryCapsCount;
				oLIA.poServerMandatoryCaps = poLIRDBResponse->unMandatoryCaps;
			}

			if(0 < poLIRDBResponse->unOptionalCapsCount)
			{
				oLIA.unOptionalCapsCount = poLIRDBResponse->unOptionalCapsCount;
				oLIA.poServerOptionalCaps = poLIRDBResponse->unOptionalCaps;
			}

            oLIA.unExpResultCode = DIAMETER_UNREGISTERED_SERVICE;

        }

    }
    else if(CX_USER_STATUS_NOT_REGISTERED == poLIRDBResponse->usRegisterStatus ||
    		CX_USER_STATUS_UNREGISTERED == poLIRDBResponse->usRegisterStatus)
    {
    	oLIA.unExpResultCode = DIAMETER_ERROR_IDENTITY_NOT_REGISTERED;
    }

	//update oamps metrics
	if (DIAMETER_SUCCESS == oLIA.unResultCode )
	{
		(hAppContext_i->oCxPerfMgmtData.unsuccessfulLIALIQ)++;
	}
	else
	{
		(hAppContext_i->oCxPerfMgmtData.unfailedLIALIQ)++;
	}


    HDIAMETERMESSAGE hDiaMsgLIA = NULL;

    if(CxDx_Retval_Success != Cx_CreateLIADiameterMessage(hAppContext_i, &hDiaMsgLIA, poLIR, &oLIA))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Message\n ");
        oReturnValue = CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success == oReturnValue)
    {
    	DMBase_SendMessageToPeer(hDiamStackContext, poSessionObject->hPeer, hDiaMsgLIA);
    	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiaMsgLIA);
    }

    DMBase_Free(hDiamStackContext, (void**)&poSessionObject->poData);
    DMBase_Free(hDiamStackContext, (void**)&poSessionObject);
    Cx_RemoveSessionObject(hAppContext_i, poLIRDBResponse->ucSessionID);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return oReturnValue;

}


t_CxDxAPIRetVal Cx_CreateLIADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgLIA_io,
											t_CxLIR*			poLIR_i,
											t_CxLIA*			poLIA_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hCxAppContext_io || NULL == phDiaMsgLIA_io || NULL == poLIR_i || NULL == poLIA_i)
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

    HDIAMETERMESSAGE hLIRDiameterMsg = NULL;

    if(DMBase_Retval_Success != DMBase_CreateDiameterMessage(	hDiamStackContext,
																&hLIRDiameterMsg,
																DIAMETER_VERSION_NUMBER,
																CxDx_MSG_CMD_CODE_LIA,
																CxDx_APPLICATION,
																poLIR_i->ucFlags,
																poLIR_i->unHopByHopID,
																poLIR_i->unEndToEndID))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create message\n");
		oReturnValue = CxDx_Retval_Failure;
		goto ExitMe;
    }

    DMBase_SetDiaMsgRFlag(hDiamStackContext, hLIRDiameterMsg,0);


	if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiamStackContext,
															hLIRDiameterMsg,&hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		return DMBase_Retval_Failure;
	}

	hTempAVP = NULL;
	if(NULL != poLIA_i->pucSessionID)
	{
	    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID, &hTempAVP,
												poLIA_i->pucSessionID, strlen(poLIA_i->pucSessionID) , hMsgAllocCtxt))
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


     if(0 != poLIA_i->unResultCode)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
													 &hTempAVP, poLIA_i->unResultCode, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }
     else if(0 != poLIA_i->unExpResultCode)
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
														&hTempAVP, poLIA_i->unExpResultCode, hMsgAllocCtxt))
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


     if(NULL != poLIA_i->pucSCSCFName)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SERVER_NAME, &hTempAVP,
																poLIA_i->pucSCSCFName, strlen(poLIA_i->pucSCSCFName),
																hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
        DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poLIA_i->poServerMandatoryCaps || NULL != poLIA_i->poServerOptionalCaps)
     {
    	HAVP hServerCaps = NULL;
		if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hServerCaps, hMsgAllocCtxt,
																CX_AVP_SERVER_CAPABILITIES, 0))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}

		 if(0 <  poLIA_i->unMandatoryCapsCount )
		 {
			 unsigned int unIndex = 0;
			 HAVP hMandatoryCapAVP = NULL;

			 while(unIndex < poLIA_i->unMandatoryCapsCount )
			 {
				 hMandatoryCapAVP = NULL;
				if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_MANDATORY_CAPABILITIES, &hMandatoryCapAVP,
																		poLIA_i->poServerMandatoryCaps[unIndex++], hMsgAllocCtxt))
				 {
					 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
					 oReturnValue = CxDx_Retval_Failure;
					 goto ExitMe;
				 }

				 DMBase_AddAVPToGroup(hDiamStackContext, hServerCaps, hMandatoryCapAVP);
			 }
		 }

		 if(0 < poLIA_i->unOptionalCapsCount)
		 {
			unsigned int unIndex = 0;
			 HAVP hOptionalCapAVP = NULL;

			 while(unIndex < poLIA_i->unOptionalCapsCount)
			 {
				 hOptionalCapAVP = NULL;
				if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_OPTIONAL_CAPABILITIES,
								 &hOptionalCapAVP, poLIA_i->poServerOptionalCaps[unIndex++], hMsgAllocCtxt))
				 {
					 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
					 oReturnValue = CxDx_Retval_Failure;
					 goto ExitMe;
				 }

				 DMBase_AddAVPToGroup(hDiamStackContext, hServerCaps, hOptionalCapAVP);
			 }
		 }
		 DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hServerCaps);
     }


     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext, hLIRDiameterMsg , poAVPList);
     *phDiaMsgLIA_io = hLIRDiameterMsg;

     ExitMe:
	//if failed Destroy Message here
     if(CxDx_Retval_Failure == oReturnValue)
     {
    	 //Also Destroy List
    	 DMBase_DestroyDiaMessage(hDiamStackContext, &hLIRDiameterMsg);
     }
     //free origin host and realm
     DMBase_Free(hDiamStackContext, (void**)&pucHostName);
     DMBase_Free(hDiamStackContext, (void**)&pucRealmName);


     DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

     return oReturnValue;
}


