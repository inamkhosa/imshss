#include "sh_UDR.h"
#include "sh_app.h"
#include "sh_msg_api.h"
#include "sh_avps.h"
#include "dmbase_ampswrappers.h"
#include "sh_diamretcodes.h"
#include "dmbase_parser.h"
#include "db_cmd_id.h"


//externs
extern	int                     g_nDiamBaseTraceID ;


t_ShAppReturnVal	sh_app_ProcessUDR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
													 unsigned char* pucEvData_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	
    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

    if(NULL == pucEvData_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucEvData_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	t_EventDataDBToSh*	pRcvdEvtDataDBToSh = NULL;
	t_ShSessionObject*	poShSessionObj = NULL;
	t_DiamOctetString   oSessionIDFromDBM = {0};
	pRcvdEvtDataDBToSh = (t_EventDataDBToSh*) pucEvData_i;
	unsigned int		unRetRespCode = 0;

	unRetRespCode = pRcvdEvtDataDBToSh->unRetCodeForShResp;
	

	DMBase_CopyCharArrayToDiamString(&oSessionIDFromDBM,pRcvdEvtDataDBToSh->pucSessionID);

	sh_app_LookUpSessionObject(hShAppContext_io,oSessionIDFromDBM,&poShSessionObj);

    if(NULL == poShSessionObj)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," No entry found in session table for SessionID => %s , SOMETHING IS VERY WRONG \n",pRcvdEvtDataDBToSh->pucSessionID);
            return Sh_Retval_Failure;
    }

	

	//make AVPs and Message;
	t_ShPullRespParams		oShPullResp = {0};
	HDIAMETERMESSAGE		hMsgUDA = NULL;
	t_DiamDictionary		oDiamDict = {0};
	HMSGALLOCATIONCONTEXT	hUDAMsgAllocContext = NULL;

	//now create Diameter Message 
	 DMBase_CreateDiameterMessage(hShAppContext_io->hDiamStackContext, 
						&hMsgUDA, 1, SH_CMD_CODE_UDR, SH_DIAM_APP_ID,0,0,0);
	 DMBase_SetDiaMsgRFlag(hShAppContext_io->hDiamStackContext,hMsgUDA,0);
	 DMBase_Get_AllocationContext_ForMessage(hShAppContext_io->hDiamStackContext
											 ,hMsgUDA,&hUDAMsgAllocContext);

	DMBase_GetDictionaryObjectFromDiameterContext(hShAppContext_io->hDiamStackContext,&oDiamDict);
	
	 

	sh_app_MakeAVPFromGeneralData(hShAppContext_io,
								  &(oShPullResp.hAVPSessionID),
								  DMBASE_AVP_SESSION_ID,
								  hUDAMsgAllocContext,
								  pRcvdEvtDataDBToSh->pucSessionID,
								  strlen(pRcvdEvtDataDBToSh->pucSessionID) );


	//make userData AVP
    if (strlen(pRcvdEvtDataDBToSh->pucUserData) >= 1 ) 
    {
        sh_app_MakeAVPFromGeneralData(hShAppContext_io,
                                      &(oShPullResp.hAVPUserData),
                                      SH_AVP_CODE_USER_DATA,
                                      hUDAMsgAllocContext,
                                      pRcvdEvtDataDBToSh->pucUserData,
                                      strlen(pRcvdEvtDataDBToSh->pucUserData) );
    }
	
	

	//make Auth Session State AVP

	sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &(oShPullResp.hAVPAuthSessionState),
							  DMBASE_AVP_AUTH_SESSION_STATE,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->unAuthSessionState),
							  sizeof ( unsigned int) );


	unsigned char*	pucTempOrigHost = NULL;
	unsigned char*	pucTempOrigRealm = NULL;

	DMBase_GetHostNameFromDiameterContext(hShAppContext_io->hDiamStackContext,(char**)&pucTempOrigHost);
	DMBase_GetRealmNameFromDiameterContext(hShAppContext_io->hDiamStackContext,(char**)&pucTempOrigRealm);

	void*			pTempDataForAVPs = NULL;

	//make Origin Host AVP
	oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_HOST].oAVPDictionaryFunctionPtrs.pfGetStructData(
		hShAppContext_io->hDiamStackContext,(void*)pucTempOrigHost,&pTempDataForAVPs,strlen(pucTempOrigHost),hUDAMsgAllocContext);

	DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPOriginHost),
					 hUDAMsgAllocContext,
					 DMBASE_AVP_ORIGIN_HOST,pTempDataForAVPs,strlen(pucTempOrigHost),0);


	pTempDataForAVPs = NULL;


	// make Origin Realm AVP
	
	oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
		hShAppContext_io->hDiamStackContext, (void*)pucTempOrigRealm,&pTempDataForAVPs,
		strlen(pucTempOrigRealm),hUDAMsgAllocContext);

	DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPOriginRealm),
					 hUDAMsgAllocContext,
					 DMBASE_AVP_ORIGIN_REALM,pTempDataForAVPs,strlen(pucTempOrigRealm),0);


	pTempDataForAVPs = NULL;

    DMBase_Free(hShAppContext_io->hDiamStackContext,(void**) &pucTempOrigHost);
    DMBase_Free(hShAppContext_io->hDiamStackContext,(void**) &pucTempOrigRealm);


	HAVP	hAVPTempVendorID = NULL;
	HAVP	hAVPTempAuthAppID = NULL;
	HAVP	hAVPTempVendorIDForResCode = NULL;
	HAVP	hAVPTempExpResultCode = NULL;

	//make VendorID AVP

	sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &hAVPTempVendorID,
							  DMBASE_AVP_VENDOR_ID,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->oVendorSpecificAppID.unVendorID),
							  sizeof ( unsigned int) );


	//make AuthAppID AVP

	sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &hAVPTempAuthAppID,
							  DMBASE_AVP_AUTH_APPLICATION_ID,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->oVendorSpecificAppID.unAuthApplicationID),
							  sizeof ( unsigned int) );


	//make grouped AVP Vendor Specific ApplicationID from the above two
	DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,
							&(oShPullResp.hAVPVendorSpecificAppID),
							hUDAMsgAllocContext,
							DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,0);

	DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
						 oShPullResp.hAVPVendorSpecificAppID,
						 hAVPTempAuthAppID);

	DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
						 oShPullResp.hAVPVendorSpecificAppID,
						 hAVPTempVendorID);


	//make VendorID AVP again for the grouped AVP Exp-Result

	sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &hAVPTempVendorIDForResCode,
							  DMBASE_AVP_VENDOR_ID,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->oVendorSpecificAppID.unVendorID),
							  sizeof ( unsigned int) );
    

    if (ShRespRetCode_Result == pRcvdEvtDataDBToSh->oShRespRetCodeType ) 
    {

		sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &oShPullResp.hAVPResultCode,
							  DMBASE_AVP_RESULT_CODE,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->unRetCodeForShResp),
							  sizeof ( unsigned int) );

    }
    else if (ShRespRetCode_ExperResult == pRcvdEvtDataDBToSh->oShRespRetCodeType ) 
    {
        //in this case , the result codes are Sh Interface specific
        //and need to be put in the Experimental-Result AVP

        	//make exp result code AVP 
        	

			sh_app_MakeAVPFromGeneralData(hShAppContext_io,
							  &hAVPTempExpResultCode,
							  DMBASE_AVP_EXPERIMENTATION_RESULT_CODE,
							  hUDAMsgAllocContext,
							  &(pRcvdEvtDataDBToSh->unRetCodeForShResp),
							  sizeof ( unsigned int) );

        
        	//make grouped AVP Experimental Result from the above two
        	DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,
        							&(oShPullResp.hAVPExperimentalResult),
									hUDAMsgAllocContext,
        							DMBASE_AVP_EXPERIMENTAL_RESULT,0);
        
        	DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
        						 oShPullResp.hAVPExperimentalResult,
        						 hAVPTempVendorIDForResCode);
        
        	DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
        						 oShPullResp.hAVPExperimentalResult,
        						 hAVPTempExpResultCode);

    }



	Sh_CreateShPullResponse(hShAppContext_io->hDiamStackContext, &hMsgUDA,oShPullResp);

	if ( DMBase_Retval_Success != DMBase_SendMessageToPeer(hShAppContext_io->hDiamStackContext,
							 poShSessionObj->hPeer,hMsgUDA))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_SendMessageToPeer failed  \n");
		
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," DMBase_SendMessageToPeer succeeded  \n");
		if (DIAMETER_SUCCESS != unRetRespCode) 
		{
			(hShAppContext_io->oShPerfMgmtData.unFailedUDADTR)++;

			if (hShAppContext_io->oShPerfMgmtData.unFailedUDADTR >=
				hShAppContext_io->oShFaultMgmtData.unFailedThresholdUDR) 
			{
				//send trap event to oamps module
				t_hssOAMPSTrapEventData oTrapEvData = {0};
				oTrapEvData.oFaultMgmtOid = hssFMOid_failedThresholdUDR;
				oTrapEvData.lValue = hShAppContext_io->oShPerfMgmtData.unFailedUDADTR;

 				if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(hShAppContext_io->hDiamStackContext->hFrameworkHandle,
										  OAMPS_SEND_TRAP_EVENT,
										  &oTrapEvData,sizeof(oTrapEvData) )
					   )
				{
					DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_SEND_TRAP_EVENT ,Leaving \n");
					return ;
				}
			}
		}
		else
		{
			(hShAppContext_io->oShPerfMgmtData.unSuccessfulUDADTR)++;
		}
	}

	DMBase_DestroyDiaMessage(hShAppContext_io->hDiamStackContext,&hMsgUDA);

	(poShSessionObj->ulNumOfOutstandingMsgs)--;
	if ( 0 == poShSessionObj->ulNumOfOutstandingMsgs)
	{
		//remove session object from table and free its memory
		sh_app_RemoveSessionObject(hShAppContext_io,oSessionIDFromDBM);
		DMBase_Free(hShAppContext_io->hDiamStackContext,(void**) &poShSessionObj);
	}

	DMBase_DeleteDiamString(&oSessionIDFromDBM);



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

}

t_ShAppReturnVal	sh_app_ProcessUDRMessage(HSHAPPCONTEXT		hShAppContext_io,
											 HDIAMETERMESSAGE	hUDRMsg_i,
											HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }
    if(NULL == hUDRMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hUDRMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }
    if(NULL == hPeer_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	//extract data out from the UDR message and build event data

	//get data from mandatory AVPs first

	


	HAVP				hAVPTemp1 = NULL;
	HAVP				hAVPTemp2 = NULL;
    HAVP				hTempAVPForSwap = NULL;
	HAVP 				hAVPTemp3 = NULL;
	t_ShAppReturnVal	oRetval = Sh_Retval_Success;
	t_DiamOctetString*	pTempOctStr = NULL;
	t_DiamEnum*			poTempEnum = NULL;
	unsigned int		unTemp= 0; 
	DMBase_SList*		pAVPList = NULL;
	t_EventDataShToDB	oEvtDataShToDB = {0};
	


	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_AUTH_SESSION_STATE,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		poTempEnum = NULL;
		poTempEnum = (t_DiamEnum*) hAVPTemp1->pData;
		oEvtDataShToDB.unAuthSessionState = *(poTempEnum->pnInteger32);
		hAVPTemp1 = NULL;
	}
	

    hAVPTemp1 = NULL;

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_HOST,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1)
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;
		if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		strncpy(oEvtDataShToDB.pucOriginHost,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
        
		hAVPTemp1 = NULL;
	}
	

    hAVPTemp1 = NULL;



	//copy origin realm
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_REALM,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;
        if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		
		strncpy(oEvtDataShToDB.pucOriginRealm ,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
		hAVPTemp1 = NULL;
	}
	

	
    hAVPTemp1 = NULL;
	

	//copy data reference
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_DATA_REFERENCE,
													hUDRMsg_i, 
                                                    &hAVPTemp1);


	if (NULL != hAVPTemp1) 
	{
		poTempEnum = (t_DiamEnum*) hAVPTemp1->pData;
		oEvtDataShToDB.unDataReference = *(poTempEnum->pnInteger32);
		hAVPTemp1 = NULL;
	}
	

    hAVPTemp1 = NULL;

	//if Data reference is PublicIDentity then get IdentitySet AVP as well
	if (SH_DATA_REF_AVP_VAL_IMSPUBLICIDENTITY == oEvtDataShToDB.unDataReference ) 
	{
		DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_IDENTITY_SET,
													hUDRMsg_i, 
                                                    &hAVPTemp1);
		if (NULL != hAVPTemp1) 
		{
			poTempEnum = (t_DiamEnum*) hAVPTemp1->pData;
			oEvtDataShToDB.unIdentitySet = *(poTempEnum->pnInteger32);
		}
		else
		{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," using default identity set of SH_IDENTITYSET_TYPE_ALL_IDENTITIES \n");
				oEvtDataShToDB.unIdentitySet = SH_IDENTITYSET_TYPE_ALL_IDENTITIES;
		}
	
		hAVPTemp1 = NULL;
		
	}
	

	//get session ID
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_SESSION_ID,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;


		if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		
		strncpy(oEvtDataShToDB.pucSessionID,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
		hAVPTemp1 = NULL;

	}//if (NULL != hAVPTemp1) 
	

	
	//get Service Indication
	hAVPTemp1 = NULL;
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_SERVICE_INDICATION,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if ( NULL != hAVPTemp1) 
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;
        if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		strncpy(oEvtDataShToDB.pucServiceIndication,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
		hAVPTemp1 = NULL;
	}
	

    hAVPTemp1 = NULL;

   
	//get SH_AVP_CODE_SERVER_NAME
	hAVPTemp1 = NULL;
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_SERVER_NAME,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;

        if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		
		strncpy(oEvtDataShToDB.pucServerName,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
		

		hAVPTemp1 = NULL;
	}
	

	

	//get DSAI-TAG
	hAVPTemp1 = NULL;
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_DSAI_TAG,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp1->pData;

        if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		
		strncpy(oEvtDataShToDB.pucDSAI_Tag,pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);
	
		hAVPTemp1 = NULL;

	}
	
	
	

	//get user identity
	hAVPTemp1 = NULL;
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_USER_IDENTITY,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL != hAVPTemp1) 
	{
		//this is a grouped AVP , get public identity AVP out of it

		pAVPList = hAVPTemp1->hCollectionOfAVPs;
	
		DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
								(void*) pAVPList,(void**) &hAVPTemp2);

		if (NULL == hAVPTemp2) 
		{
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," SH_AVP_CODE_PUBLIC_IDENTITY not found ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}

	
		pTempOctStr = NULL;
		pTempOctStr	= (t_DiamOctetString*) hAVPTemp2->pData;
	
		if (pTempOctStr->unNumOfOctets >= SH_MAX_SMALL_STRING_SIZE) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}
		
		strncpy(oEvtDataShToDB.oUserIdentity.pucPublicIdentity,
				pTempOctStr->pucOctetString,pTempOctStr->unNumOfOctets);

		hAVPTemp1 = NULL;

	}//end if SH_AVP_CODE_USER_IDENTITY is in UDR Message
	

	


	//get vendor specific application ID
	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,
													hUDRMsg_i, 
                                                    &hAVPTemp1);



	if (NULL != hAVPTemp1) 
	{
		hAVPTemp3 = NULL;
		//this is a grouped AVP , containing two members , get both of them
		//put VendorID in hTempAVP2 and Auth-Application_Id in hAVPTemp3
	
		pAVPList = hAVPTemp1->hCollectionOfAVPs;
	
		
	
		DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
								(void*) pAVPList,(void**) &hAVPTemp2);
	
		hTempAVPForSwap = NULL;
	
		if (DMBASE_AVP_VENDOR_ID == hAVPTemp2->oAVPHeaderInst.unAVPCode) 
		{
			//next one should be Auth-ApplicationID
			DMBase_SListGetNextNode(hShAppContext_io->hDiamStackContext,
									  pAVPList ,  &pAVPList);
	
			DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
								(void*) pAVPList,(void**) &hAVPTemp3);
	
			//order is fine.
	
		}
		else if (DMBASE_AVP_AUTH_APPLICATION_ID == hAVPTemp2->oAVPHeaderInst.unAVPCode) 
		{
			//next one should be VendorID
			DMBase_SListGetNextNode(hShAppContext_io->hDiamStackContext,
									 pAVPList ,  &pAVPList);
	
			DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
								(void*) pAVPList,(void**) &hAVPTemp3);
	
			//swap the AVPs 
			hTempAVPForSwap = hAVPTemp2;
			hAVPTemp2 = hAVPTemp3;
			hAVPTemp3 = hTempAVPForSwap;
	
		}
	
		if (NULL == hAVPTemp2) 
		{
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID not found ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}

		if (NULL == hAVPTemp3) 
		{
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_AVP_AUTH_APPLICATION_ID not found ,leaving \n");
            oRetval = Sh_Retval_Failure;
			goto ExitMe;
		}

		t_DiamUnsigned32*	poTempUnsign32 = NULL;
		poTempUnsign32 = ( t_DiamUnsigned32*)  hAVPTemp2->pData;
		oEvtDataShToDB.oVendorSpecificAppID.unAuthApplicationID = *(poTempUnsign32->punUnsigned32);

         
	
		poTempUnsign32 = ( t_DiamUnsigned32*)  hAVPTemp3->pData;
		oEvtDataShToDB.oVendorSpecificAppID.unVendorID = *(poTempUnsign32->punUnsigned32);
	
		
	}// end if ( NULL != hAvpTemp1) for grouped AVP DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID
	
	
	

	
	hAVPTemp1 = hAVPTemp2 = hAVPTemp3 = hTempAVPForSwap = NULL;
	oEvtDataShToDB.unEvtCommandID =  DB_SH_CMD_CODE_UDR;

	
	
	
	
    if ( AMPS_SUCCESS != AMPS_EvtSysSendIntEvt( 
		hShAppContext_io->hDiamStackContext->hFrameworkHandle, 
		SH_APP_REQUEST_EVENT, &oEvtDataShToDB , sizeof(oEvtDataShToDB) )
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendIntEvt failed for SH_APP_REQUEST_EVENT \n");
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," AMPS_EvtSysSendIntEvt SUCCEEDED for SH_APP_REQUEST_EVENT \n");

	}

	ExitMe:


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return oRetval;

}
