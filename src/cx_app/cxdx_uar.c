
#include "cxdx_uar.h"
#include "cxdx_session.h"
#include "dmbase_parser.h"
#include "cxdx_app.h"
#include "dappcommon_helpers.h"
#include "dmbase_utility.h"
#include "dmbase_ampswrappers.h"
#include "cxdx_msg_api.h"
#include "hss.h"

extern	int	g_nDiamBaseTraceID ;

t_CxDxAPIRetVal Cx_ProcessUAR(	HCXAPPCONTEXT hAppContext_i,
								t_CxUAR *poUAR_i,
								HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == hPeer_i ||NULL == poUAR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    t_CxSessionObject *poSessionObj = NULL;


    if(DMBase_Retval_Success != DMBase_Malloc(hAppContext_i->hDiamStackContext,
									(void**)&poSessionObj,sizeof(t_CxSessionObject)))
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create session object\n");
    	return CxDx_Retval_Failure;
    }
    poSessionObj->hAppContext = hAppContext_i;
    poSessionObj->hPeer = hPeer_i;
    poSessionObj->poData = (void*)poUAR_i;

    if(CxDx_Retval_Success != Cx_InsertSessionObject(hAppContext_i,
								poUAR_i->oUARDBRequest.ucSessionID, poSessionObj))
    {
    	DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&poSessionObj);
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to Insert session object\n");
    	return CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success != Cx_GenerateUARDBRequest(hAppContext_i, poUAR_i))
    {
    	//Delete Session Table Entry
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Event generation failed\n");
        return DMBase_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}

t_CxDxAPIRetVal Cx_GenerateUARDBRequest(HCXAPPCONTEXT hAppContext_i, t_CxUAR *poUAR_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == poUAR_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    //Return value check

    Cx_GenerateDBRequest(	hAppContext_i,
                            CX_UAR_DATABASE_REQUEST_EVENT,
                            (void*)&poUAR_i->oUARDBRequest,
                            sizeof(t_CxUARDBRequest));

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal	Cx_ProcessUARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
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

    t_CxUARDBResponse* poUARDBResponse = (t_CxUARDBResponse*)pucEventData_i;
    t_CxSessionObject* poSessionObject = NULL;

    if(CxDx_Retval_Success != Cx_LookUpSessionObject(hAppContext_i, poUARDBResponse->ucSessionID, (void**)&poSessionObject) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No Session object found\n ");
        return CxDx_Retval_Failure;
    }

    t_CxUAR* poUAR = (t_CxUAR*)poSessionObject->poData;

    if(NULL == poUAR)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No UAR data in session object\n ");
        return CxDx_Retval_Null_Pointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;

    t_CxDxAPIRetVal oReturnValue = CxDx_Retval_Success;

    t_CxUAA oUAA;
    DMBase_MemsetZero(&oUAA, sizeof(t_CxUAA));

    oUAA.pucSessionID = poUAR->oUARDBRequest.ucSessionID;


    if(DB_RESULT_NO_PRIVATE_ID == poUARDBResponse->unDBResult ||
    		DB_RESULT_NO_PUBLIC_ID == poUARDBResponse->unDBResult)
    {
    	oUAA.unExpResultCode = DIAMETER_ERROR_USER_UNKNOWN;
    }
    else if(DB_RESULT_NOT_FOUND == poUARDBResponse->unDBResult)
    {
    	oUAA.unExpResultCode = DIAMETER_ERROR_IDENTITIES_DONT_MATCH;
    }
    else if(DB_RESULT_SUCCESS == poUARDBResponse->unDBResult)
    {
        if(poUARDBResponse->usBarringInd)
        {
        	oUAA.unResultCode = DIAMETER_AUTHORIZATION_REJECTED;
        }
        else if(REGISTERATION_AND_CAPABILITIES == poUAR->unAuthType)
        {
			if(0 < poUARDBResponse->unMandatoryCapsCount )
			{
				oUAA.unMandatoryCapsCount = poUARDBResponse->unMandatoryCapsCount;
				oUAA.poServerMandatoryCaps = poUARDBResponse->unMandatoryCaps;
			}

			if(0 < poUARDBResponse->unOptionalCapsCount)
			{
				oUAA.unOptionalCapsCount = poUARDBResponse->unOptionalCapsCount;
				oUAA.poServerOptionalCaps = poUARDBResponse->unOptionalCaps;
			}

			oUAA.unResultCode = DIAMETER_SUCCESS;
        }
        else
        {
            if(CX_USER_STATUS_REGISTERED == poUARDBResponse->usRegisterStatus)
            {
                if(-1 == poUAR->unAuthType || REGISTERATION == poUAR->unAuthType)
                {
                	oUAA.unExpResultCode = DIAMETER_SUBSEQUENT_REGISTRATION;
                }
                else if (DE_REGISTERATION == poUAR->unAuthType)
                {
                	oUAA.unResultCode = DIAMETER_SUCCESS;
                }

                oUAA.pucSCSCFName = poUARDBResponse->ucSCSCFName;
            }
            else if(CX_USER_STATUS_UNREGISTERED == poUARDBResponse->usRegisterStatus)
            {
                if(-1 == poUAR->unAuthType || REGISTERATION == poUAR->unAuthType)
                {
                	oUAA.unExpResultCode = DIAMETER_SUBSEQUENT_REGISTRATION;
                }
                else if (DE_REGISTERATION == poUAR->unAuthType)
                {
                	oUAA.unResultCode = DIAMETER_SUCCESS;
                }
                oUAA.pucSCSCFName = poUARDBResponse->ucSCSCFName;
            }
            else if(CX_USER_STATUS_NOT_REGISTERED == poUARDBResponse->usRegisterStatus)
            {
                if(DE_REGISTERATION == poUAR->unAuthType)
                {
                	oUAA.unExpResultCode = DIAMETER_ERROR_IDENTITY_NOT_REGISTERED;
                }
                else if(-1 == poUAR->unAuthType || REGISTERATION == poUAR->unAuthType)
                {
                    if(0 < strlen(poUARDBResponse->ucSCSCFName))
                    {
                    	oUAA.unExpResultCode = DIAMETER_SUBSEQUENT_REGISTRATION;
                    	oUAA.pucSCSCFName = poUARDBResponse->ucSCSCFName;

                    }
                    else
                    {
            			if(0 < poUARDBResponse->unMandatoryCapsCount )
            			{
            				oUAA.unMandatoryCapsCount = poUARDBResponse->unMandatoryCapsCount;
            				oUAA.poServerMandatoryCaps = poUARDBResponse->unMandatoryCaps;
            			}

            			if(0 < poUARDBResponse->unOptionalCapsCount)
            			{
            				oUAA.unOptionalCapsCount = poUARDBResponse->unOptionalCapsCount;
            				oUAA.poServerOptionalCaps = poUARDBResponse->unOptionalCaps;
            			}

            			oUAA.unExpResultCode = DIAMETER_FIRST_REGISTRATION;
                    }
                }
            }
        }
    }
    else
    {
    	oUAA.unResultCode = DIAMETER_UNABLE_TO_COMPLY;
    }

	//update oamps metrics
	if (DIAMETER_SUCCESS == oUAA.unResultCode )
	{
		(hAppContext_i->oCxPerfMgmtData.unsuccessfulUAAUR)++;
	}
	else
	{
		(hAppContext_i->oCxPerfMgmtData.unfailedUAAUR)++;
	}

    HDIAMETERMESSAGE hDiaMsgUAA = NULL;

    if(CxDx_Retval_Success != Cx_CreateUAADiameterMessage(hAppContext_i, &hDiaMsgUAA, poUAR, &oUAA))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Message\n ");
        oReturnValue = CxDx_Retval_Failure;
    }


    if(CxDx_Retval_Success == oReturnValue)
    {
    	DMBase_SendMessageToPeer(hDiamStackContext, poSessionObject->hPeer, hDiaMsgUAA);
    	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiaMsgUAA);
    }

    //Free Memory
    DMBase_Free(hDiamStackContext, (void**)&poSessionObject->poData);
    DMBase_Free(hDiamStackContext, (void**)&poSessionObject);

    Cx_RemoveSessionObject(hAppContext_i, poUARDBResponse->ucSessionID);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return oReturnValue;

}



t_CxDxAPIRetVal Cx_CreateUAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgUAA_io,
											t_CxUAR*			poUAR_i,
											t_CxUAA*			poUAA_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hCxAppContext_io || NULL == phDiaMsgUAA_io || NULL == poUAR_i || NULL == poUAA_i)
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
    HDIAMETERMESSAGE hUAADiameterMsg = NULL;
    unsigned char*	pucHostName = NULL;
    unsigned char*	pucRealmName = NULL;

    if(DMBase_Retval_Success != DMBase_CreateDiameterMessage(	hDiamStackContext,
																&hUAADiameterMsg,
																DIAMETER_VERSION_NUMBER,
																CxDx_MSG_CMD_CODE_UAA,
																CxDx_APPLICATION,
																poUAR_i->ucFlags,
																poUAR_i->unHopByHopID,
																poUAR_i->unEndToEndID))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create message\n");
		oReturnValue = CxDx_Retval_Failure;
		goto ExitMe;
    }

    DMBase_SetDiaMsgRFlag(hDiamStackContext, hUAADiameterMsg,0);


	if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiamStackContext,
															hUAADiameterMsg,&hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		return DMBase_Retval_Failure;
	}

	hTempAVP = NULL;
	if(NULL != poUAA_i->pucSessionID)
	{
	    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID, &hTempAVP,
												poUAA_i->pucSessionID, strlen(poUAA_i->pucSessionID) , hMsgAllocCtxt))
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


     if(0 != poUAA_i->unResultCode)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
													 &hTempAVP, poUAA_i->unResultCode, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }
     else if(0 != poUAA_i->unExpResultCode)
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
														&hTempAVP, poUAA_i->unExpResultCode, hMsgAllocCtxt))
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


     if(NULL != poUAA_i->pucSCSCFName)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SERVER_NAME, &hTempAVP,
																poUAA_i->pucSCSCFName, strlen(poUAA_i->pucSCSCFName),
																hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
        DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poUAA_i->poServerMandatoryCaps || NULL != poUAA_i->poServerOptionalCaps)
     {
    	HAVP hServerCaps = NULL;
		if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hServerCaps, hMsgAllocCtxt,
																CX_AVP_SERVER_CAPABILITIES, 0))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}

		 if(0 <  poUAA_i->unMandatoryCapsCount )
		 {
			 unsigned int unIndex = 0;
			 HAVP hMandatoryCapAVP = NULL;

			 while(unIndex < poUAA_i->unMandatoryCapsCount )
			 {
				 hMandatoryCapAVP = NULL;
				if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_MANDATORY_CAPABILITIES, &hMandatoryCapAVP,
																		poUAA_i->poServerMandatoryCaps[unIndex++], hMsgAllocCtxt))
				 {
					 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
					 oReturnValue = CxDx_Retval_Failure;
					 goto ExitMe;
				 }

				 DMBase_AddAVPToGroup(hDiamStackContext, hServerCaps, hMandatoryCapAVP);
			 }
		 }

		 if(0 < poUAA_i->unOptionalCapsCount)
		 {
			unsigned int unIndex = 0;
			 HAVP hOptionalCapAVP = NULL;

			 while(unIndex < poUAA_i->unOptionalCapsCount)
			 {
				 hOptionalCapAVP = NULL;
				if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_OPTIONAL_CAPABILITIES,
								 &hOptionalCapAVP, poUAA_i->poServerOptionalCaps[unIndex++], hMsgAllocCtxt))
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


     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext, hUAADiameterMsg, poAVPList);

     *phDiaMsgUAA_io = hUAADiameterMsg;

     ExitMe:
	//if failed Destroy Message here
     if(CxDx_Retval_Failure == oReturnValue)
     {
    	 //Also Destroy List
    	 DMBase_DestroyDiaMessage(hDiamStackContext, &hUAADiameterMsg);
     }
     //free origin host and realm
     DMBase_Free(hDiamStackContext, (void**)&pucHostName);
     DMBase_Free(hDiamStackContext, (void**)&pucRealmName);

     DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

     return oReturnValue;
}

