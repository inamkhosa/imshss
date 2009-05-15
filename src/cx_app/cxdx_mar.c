
#include "cxdx_mar.h"
#include "cxdx_session.h"
#include "cxdx_auth.h"
#include "dmbase_parser.h"
#include "cxdx_app.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "cxdx_msg_api.h"
#include "hss.h"

extern	int	g_nDiamBaseTraceID;

t_CxDxAPIRetVal Cx_ProcessMAR(	HCXAPPCONTEXT 	hAppContext_i,
								t_CxMAR*		poMAR_i,
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
    poSessionObj->poData = (void*)poMAR_i;

    if(CxDx_Retval_Success != Cx_InsertSessionObject(hAppContext_i,
								poMAR_i->oMARDBRequest.ucSessionID, poSessionObj))
    {
    	DMBase_Free(hAppContext_i->hDiamStackContext, (void**)&poSessionObj);
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to Insert session object\n");
    	return CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success != Cx_GenerateMARDBRequest(hAppContext_i, poMAR_i))
    {
    	//Delete Session Table Entry
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Event generation failed\n");
        return DMBase_Retval_Failure;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}

t_CxDxAPIRetVal Cx_GenerateMARDBRequest(HCXAPPCONTEXT hAppContext_i, t_CxMAR *poMAR_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


    //Return value check
    Cx_GenerateDBRequest(	hAppContext_i,
                            CX_MAR_DATABASE_REQUEST_EVENT,
                            (void*)&poMAR_i->oMARDBRequest,
                            sizeof(t_CxMARDBRequest));

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}




t_CxDxAPIRetVal	Cx_ProcessMARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
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

    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext_i->hDiamStackContext;
    t_CxMARDBResponse* poMARDBResponse = (t_CxMARDBResponse*)pucEventData_i;
    t_CxSessionObject* poSessionObject = NULL;
    t_CxDxAPIRetVal	oReturnValue = CxDx_Retval_Success;
    t_CxMAA oMAA;

    DMBase_MemsetZero(&oMAA, sizeof(t_CxMAA));
    if(CxDx_Retval_Success != Cx_LookUpSessionObject(hAppContext_i, poMARDBResponse->ucSessionID, (void**)&poSessionObject) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No Session object found\n ");
        return CxDx_Retval_Failure;
    }

    t_CxMAR* poMAR = (t_CxMAR*)poSessionObject->poData;

    if(NULL == poMAR)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " No MAR data in session object\n ");
        return CxDx_Retval_Null_Pointer;
    }

    oMAA.pucSessionID = poMAR->oMARDBRequest.ucSessionID;

    if(DB_RESULT_NO_PUBLIC_ID == poMARDBResponse->unDBResult ||
    		DB_RESULT_NO_PRIVATE_ID == poMARDBResponse->unDBResult)
    {
    	oMAA.unExpResultCode = DIAMETER_ERROR_USER_UNKNOWN;
    }
    else if(DB_RESULT_NOT_FOUND == poMARDBResponse->unDBResult)
    {
    	oMAA.unExpResultCode = DIAMETER_ERROR_IDENTITIES_DONT_MATCH;
    }
    else if(DB_RESULT_FAILURE == poMARDBResponse->unDBResult)
    {
    	oMAA.unResultCode = DIAMETER_UNABLE_TO_COMPLY;
    }
    else if(0 != strcmp(poMAR->oMARDBRequest.ucAuthScheme, poMARDBResponse->ucAuthScheme))
    {
    	oMAA.unExpResultCode = DIAMETER_ERROR_AUTH_SCHEME_NOT_SUPPORTED;
    }
    else if(CX_USER_STATUS_REGISTERED == poMARDBResponse->usRegisterStatus ||
            CX_USER_STATUS_UNREGISTERED == poMARDBResponse->usRegisterStatus ||
            CX_USER_STATUS_NOT_REGISTERED == poMARDBResponse->usRegisterStatus)
    {
    	oMAA.pucPublicId = poMAR->oMARDBRequest.ucPublicIdentity;
    	oMAA.pucUserName = poMAR->oMARDBRequest.ucUserName;
    	oMAA.unResultCode = DIAMETER_SUCCESS;
    	oMAA.unSIPNumAuthItems = poMAR->unSIPNumnerAuthItems;
    	oMAA.pucAuthScheme = poMAR->oMARDBRequest.ucAuthScheme;

        //TODO: It must configuration driven
        if(oMAA.unSIPNumAuthItems > MAX_AUTH_VECTORS_ALLOWED)
        {
        	oMAA.unSIPNumAuthItems = MAX_AUTH_VECTORS_ALLOWED;
        }

        oMAA.pucPrivateKey = poMARDBResponse->ucPrivateKey;
    }

	//update oamps metrics
	if (DIAMETER_SUCCESS == oMAA.unResultCode )
	{
		(hAppContext_i->oCxPerfMgmtData.unsuccessfulMAAMA)++;
	}
	else
	{
		(hAppContext_i->oCxPerfMgmtData.unfailedMAAMA)++;
	}

    HDIAMETERMESSAGE hDiaMsgMAA = NULL;

    if(CxDx_Retval_Success != Cx_CreateMAADiameterMessage(hAppContext_i, &hDiaMsgMAA, poMAR, &oMAA))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Message\n ");
        oReturnValue = CxDx_Retval_Failure;
    }

    if(CxDx_Retval_Success == oReturnValue)
    {
    	DMBase_SendMessageToPeer(hDiamStackContext, poSessionObject->hPeer, hDiaMsgMAA);
    	DMBase_DestroyDiaMessage(hDiamStackContext, &hDiaMsgMAA);
    }

    DMBase_Free(hDiamStackContext, (void**)&poSessionObject->poData);
    DMBase_Free(hDiamStackContext, (void**)&poSessionObject);
    Cx_RemoveSessionObject(hAppContext_i, poMARDBResponse->ucSessionID);
    //printf("MAR Sent\r\n");
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return oReturnValue;

}



t_CxDxAPIRetVal Cx_CreateMAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgMAA_io,
											t_CxMAR*			poMAR_i,
											t_CxMAA*			poMAA_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hCxAppContext_io || NULL == phDiaMsgMAA_io || NULL == poMAR_i || NULL == poMAA_i)
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
    HDIAMETERMESSAGE hMAADiameterMsg = NULL;

    if(DMBase_Retval_Success != DMBase_CreateDiameterMessage(	hDiamStackContext,
																&hMAADiameterMsg,
																DIAMETER_VERSION_NUMBER,
																CxDx_MSG_CMD_CODE_MAA,
																CxDx_APPLICATION,
																poMAR_i->ucFlags,
																poMAR_i->unHopByHopID,
																poMAR_i->unEndToEndID))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create message\n");
		oReturnValue = CxDx_Retval_Failure;
		goto ExitMe;
    }

    DMBase_SetDiaMsgRFlag(hDiamStackContext, hMAADiameterMsg,0);


	if (DMBase_Retval_Success !=  DMBase_Get_AllocationContext_ForMessage(hDiamStackContext,
			hMAADiameterMsg,&hMsgAllocCtxt))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_Get_AllocationContext_ForMessage Failed , Leaving \n");
		return DMBase_Retval_Failure;
	}

	hTempAVP = NULL;
	if(NULL != poMAA_i->pucSessionID)
	{
	    if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, DMBASE_AVP_SESSION_ID, &hTempAVP,
												poMAA_i->pucSessionID, strlen(poMAA_i->pucSessionID) , hMsgAllocCtxt))
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


     if(0 != poMAA_i->unResultCode)
     {
    	 hTempAVP = NULL;
		if(DMBase_Retval_Success != DMBase_CreateEnumAVP(hDiamStackContext, DMBASE_AVP_RESULT_CODE,
													 &hTempAVP, poMAA_i->unResultCode, hMsgAllocCtxt))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
			oReturnValue = CxDx_Retval_Failure;
			goto ExitMe;
		}
		DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }
     else if(0 != poMAA_i->unExpResultCode)
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
														&hTempAVP, poMAA_i->unExpResultCode, hMsgAllocCtxt))
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


     if(NULL != poMAA_i->pucUserName)
     {
    	 hTempAVP = NULL;
     	if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, DMBASE_AVP_USER_NAME, &hTempAVP,
											poMAA_i->pucUserName, strlen(poMAA_i->pucUserName), hMsgAllocCtxt))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }
     	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poMAA_i->pucPublicId)
     {
    	 hTempAVP = NULL;
     	if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_PUBLIC_IDENTITY, &hTempAVP,
											poMAA_i->pucPublicId, strlen(poMAA_i->pucPublicId), hMsgAllocCtxt))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }
     	DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(0 < poMAA_i->unSIPNumAuthItems && NULL != poMAA_i->pucAuthScheme)
     {
    	 hTempAVP = NULL;
         if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_SIP_NUMBER_AUTH_ITEMS,
                                     &hTempAVP, poMAA_i->unSIPNumAuthItems, hMsgAllocCtxt ))
         {
             DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
             oReturnValue = CxDx_Retval_Failure;
             goto ExitMe;
         }
    	 DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hTempAVP);
     }

     if(NULL != poMAA_i->pucPrivateKey)
     {
		 unsigned char ucPrivateKey[CX_K_SIZE/8] = {0};
		 memcpy(ucPrivateKey, poMAA_i->pucPrivateKey, CX_K_SIZE/8);

		 HAVP hAVP_SIPAuthDataItem = NULL;     //SIP Authentication Data Items
		 HAVP hAVP_SIPItemNumber = NULL;       //SIP Item Number
		 HAVP hAVP_SIPAuthScheme = NULL;       //SIP Auth Scheme
		 HAVP hAVP_SIPAuthenticate  = NULL;    //SIP Authenticate (RAND||ATUN)
		 HAVP hAVP_SIPAuthorization  = NULL;   //SIP Authorization (Expected Response)
		 HAVP hAVP_ConfidentialityKey  = NULL; //Confidentiality Key
		 HAVP hAVP_IntegrityKey = NULL;        //Integrity Key

		  // Creation of N number of Authentication Vectors. N is sent in AVP CX_AVP_SIP_NUMBER_AUTH_ITEMS
		 int nCount = 1;
		 for(; nCount <= poMAA_i->unSIPNumAuthItems; nCount++)
		 {
			 unsigned char ucRandomChallange[CX_RAND_SIZE/8] = {0};
			 unsigned char ucSQN[CX_SQN_SIZE/8] = {0};
			 unsigned char ucNext_SQN[CX_SQN_SIZE/8] = {0};
			 unsigned char ucAMF[CX_AMF_SIZE/8] = {0};
			 unsigned char ucMAC[CX_MAC_SIZE/8] = {0};
			 unsigned char ucXRES[CX_XRES_SIZE/8] = {0};
			 unsigned char ucCK[CX_CK_SIZE/8] = {0};
			 unsigned char ucIK[CX_IK_SIZE/8] = {0};
			 unsigned char ucAK[CX_AK_SIZE/8] = {0};
			 unsigned char ucAUTN[CX_AUTN_SIZE/8] = {0};
			 unsigned char ucAuthenticate[CX_RAND_SIZE/8 + CX_AUTN_SIZE/8] = {0};

			 hAVP_SIPAuthDataItem = NULL;
			 hAVP_SIPItemNumber = NULL;
			 hAVP_SIPAuthScheme = NULL;
			 hAVP_SIPAuthenticate  = NULL;
			 hAVP_SIPAuthorization  = NULL;
			 hAVP_ConfidentialityKey  = NULL;
			 hAVP_IntegrityKey = NULL;

			 getnext_sqn(ucSQN,ucNext_SQN);
			 f1(ucPrivateKey, ucRandomChallange, ucNext_SQN, ucAMF, ucMAC);
			 f2345 ( ucPrivateKey, ucRandomChallange, ucXRES, ucCK, ucIK, ucAK);

			 memcpy(ucAuthenticate, ucRandomChallange, CX_RAND_SIZE/8);
			 memcpy(ucAuthenticate+CX_RAND_SIZE/8, ucAUTN, CX_AUTN_SIZE/8);

			 if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, CX_AVP_SIP_ITEM_NUMBER, &hAVP_SIPItemNumber, nCount, hMsgAllocCtxt))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateUTF8StringAVP(hDiamStackContext, CX_AVP_SIP_AUTHENTICATION_SCHEME, &hAVP_SIPAuthScheme,
										 poMAA_i->pucAuthScheme, strlen(poMAA_i->pucAuthScheme), hMsgAllocCtxt))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_SIP_AUTHENTICATE, &hAVP_SIPAuthenticate,
										 ucAuthenticate , CX_RAND_SIZE/8 + CX_AUTN_SIZE/8 , hMsgAllocCtxt))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_SIP_AUTHORIZATION, &hAVP_SIPAuthorization,
										 ucXRES, CX_XRES_SIZE/8 , hMsgAllocCtxt))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_CONFIDENTIALITY_KEY, &hAVP_ConfidentialityKey,
										 ucCK, CX_CK_SIZE/8, hMsgAllocCtxt ))

			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateOctetStringAVP(hDiamStackContext, CX_AVP_INTEGRITY_KEY, &hAVP_IntegrityKey,
										 ucIK, CX_IK_SIZE/8 , hMsgAllocCtxt))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 if(DMBase_Retval_Success != DMBase_CreateGroupedAVP(hDiamStackContext, &hAVP_SIPAuthDataItem, hMsgAllocCtxt, CX_AVP_SIP_AUTH_DATA_ITEM, 0))
			 {
				 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create AVP\n ");
				 oReturnValue = CxDx_Retval_Failure;
				 goto ExitMe;
			 }

			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_SIPItemNumber);
			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_SIPAuthenticate);
			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_SIPAuthorization);
			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_SIPAuthScheme);
			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_ConfidentialityKey);
			 DMBase_AddAVPToGroup(hDiamStackContext, hAVP_SIPAuthDataItem, hAVP_IntegrityKey);

			 //DMBase_AppendSList(hDiamStackContext, &oParams.poSIPAuthDataItem, (void*)hAVP_SIPAuthDataItem);
			 DMBase_AppendSList(hDiamStackContext, &poAVPList, (void*)hAVP_SIPAuthDataItem);

		 }
     }

     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext, hMAADiameterMsg, poAVPList);
     *phDiaMsgMAA_io = hMAADiameterMsg;

     ExitMe:
	//if failed Destroy Message here
     if(CxDx_Retval_Failure == oReturnValue)
     {
    	 //Also Destroy List
    	 DMBase_DestroyDiaMessage(hDiamStackContext, &hMAADiameterMsg);
     }
     //free origin host and realm
     DMBase_Free(hDiamStackContext, (void**)&pucHostName);
     DMBase_Free(hDiamStackContext, (void**)&pucRealmName);

     DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

     return oReturnValue;
}



