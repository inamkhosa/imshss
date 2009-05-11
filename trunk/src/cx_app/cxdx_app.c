
#include "cxdx_app.h"
#include "db_cmd_id.h"
#include "dmbase_ampswrappers.h"
#include "db_cmd_id.h"
#include "cxdx_msg_api.h"
#include "dmbase_dictionary.h"
#include "dmbase_parser.h"
#include "dmbase_transport_mgmt.h"
#include "dappcommon_helpers.h"
#include "dmbase_avp.h"
#include "cxdx_auth.h"
#include "cxdx_uar.h"
#include "cxdx_sar.h"
#include "cxdx_mar.h"
#include "cxdx_lir.h"
#include "cxdx_session.h"
#include "cxdx_msgvalidation.h"
#include "hss.h"
#include "hss_oamps.h"
#include "dappcommon_helpers.h"


extern	int g_nDiamBaseTraceID ;

AMPS_MODULE_HANDLE	cxdx_app_ModuleInit(AMPS_HANDLE hAMPS_HANDLE_i,
										void* 		pvUserData_i)
{

	printf("\n Entering the Initializer function of CX \r\n");

	if(CxDx_Retval_Success != Cx_InitApplication((t_cx_config_params*)pvUserData_i))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Failed to initialize Cx Application\n");
		//appropriate return value
	}

	return (AMPS_MODULE_HANDLE)0xABCDEF;


}

void cxdx_app_ModuleCleanup(AMPS_HANDLE 		hAMPS_HANDLE_i,
							AMPS_MODULE_HANDLE 	hAMPS_MODULE_HANDLE_i)
{
	printf("\n Entering the cleanup function of cx_dx");
}

t_CxDxAPIRetVal	Cx_InitApplication(t_cx_config_params*	poCxConfParams_i)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering.\n");

	//printf("\n\n CX MOd Ininit => Param 1 == %s", poCxConfParams_i->pucParam1);
	//printf("\n\n CX MOd Ininit => Param 2 == %s\n\n", poCxConfParams_i->pucParam2);

	HDIAMETERSTACKCONTEXT	hDiamStackContext = poCxConfParams_i->hDiameterStackContext;

    HCXAPPCONTEXT hAppContext = NULL;

    if(DMBase_Retval_Success != DMBase_Malloc(hDiamStackContext ,(void**)&hAppContext,
																sizeof(t_CxAppContext)))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
								" Application Context creation Failed\n");
        return CxDx_Retval_Failure;
    }

    if(DMBase_Retval_Success != DMBase_CreateHashTable(hDiamStackContext, CX_SESSION_TABLE_MAX_SIZE,
														&hAppContext->hSessionTable, NULL))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Session Table creation failed\r\n");
        return CxDx_Retval_Failure;
    }

    hAppContext->hDiamStackContext = hDiamStackContext;

    //Register application callback
	if(DMBase_Retval_Success != DMBase_RegisterApplicationCallback(hDiamStackContext,CxDx_APPLICATION,
															Cx_ProcessMessage, Cx_PeerCloseEventCallback,
															(void*)hAppContext))
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Registration of Cx Application callback failed\r\n");
        return CxDx_Retval_Failure;
	}

	//Database Response Event Registration
	/**/
	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,	CX_UAR_DATABASE_RESPONSE_EVENT,
													Cx_DBResponseHandler, (void*)hAppContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" CX_UAR_DATABASE_RESPONSE_EVENT event registration  failed\r\n");
        return CxDx_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,CX_SAR_DATABASE_RESPONSE_EVENT,
													Cx_DBResponseHandler, (void*)hAppContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" CX_SAR_DATABASE_RESPONSE_EVENT event registration  failed\r\n");
        return CxDx_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,CX_MAR_DATABASE_RESPONSE_EVENT,
												Cx_DBResponseHandler, (void*)hAppContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" CX_MAR_DATABASE_RESPONSE_EVENT event registration  failed\r\n");
        return CxDx_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,CX_LIR_DATABASE_RESPONSE_EVENT,
														Cx_DBResponseHandler, (void*)hAppContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" CX_LIR_DATABASE_RESPONSE_EVENT event registration  failed\r\n");
        return CxDx_Retval_Failure;
    }

	if ( DMBase_Retval_Success != DMBase_EvtSysRegExtEvt(hDiamStackContext,
										OAMPS_REQUEST_TO_CX_EVENT,
											cx_app_OAMPS_RequestHandler, (void*)hAppContext))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,
        		" OAMPS_REQUEST_TO_CX_EVENT event registration  failed\r\n");
        return CxDx_Retval_Failure;
    }

	Cx_InitSeed(NULL);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}


t_DiamAPIReturnVal Cx_ProcessMessage(HPEERTABLEENTRY hPeer_i, HDIAMETERMESSAGE hDiamMsg_i, void* pvAppContext)
{
    DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Entering.\n");

//    printf("Message Received --- for CX\r\n");


    HCXAPPCONTEXT hAppContext = (HCXAPPCONTEXT)pvAppContext;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hAppContext->hDiamStackContext;

 //   DMBase_Debug_DumpDiamMsgContents(hDiamStackContext, NULL,hDiamMsg_i);
    unsigned int unCommandCode = 0;//DMBase_GetCmdCodeFromMsg(hDiamMsg_i);

    DMBase_GetDiaMsgCmdCodeFromDiamMsg(hDiamStackContext, hDiamMsg_i, &unCommandCode);

    t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    DMBase_SList* poFailedAVPList = NULL;
    void* poAppMsg = NULL;

    if(CxDx_Retval_Success != (oReturnVal =
    	Cx_ExtractAppMsgAttributes(hAppContext, hDiamMsg_i, &poAppMsg)))
    {
    	if(CxDx_Retval_Not_Supported == oReturnVal)
    	{
            DMBase_GenerateProtocolErr(hDiamStackContext, hPeer_i,
                    hDiamMsg_i, DIAMETER_COMMAND_UNSUPPORTED, NULL);
            DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_ERROR, " Invalid Command Code\r\n");
    	}
    	else
    	{
    		DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_INFO,
    				" Problem occurred while extracting message attributes.\r\n");
    	}

    	goto ExitMe;
    }



    if((oReturnVal = Cx_ValidateMessage(hAppContext, poAppMsg, unCommandCode,  &poFailedAVPList)) != CxDx_Retval_Success)
    {
    	if(CxDx_Retval_MoreInfoRequired == oReturnVal)
    	{
    		Cx_GeneratePermanentFailure(hAppContext,hPeer_i, hDiamMsg_i,
    				DIAMETER_MISSING_AVP, poFailedAVPList );
    	}
    	else if(CxDx_Retval_Limit_Exceed == oReturnVal)
    	{
    		Cx_GeneratePermanentFailure(hAppContext,hPeer_i, hDiamMsg_i,
    				DIAMETER_AVP_OCCURS_TOO_MANY_TIMES, poFailedAVPList );
    	}

    	//Free failed AVP list
    	DMBase_SListFree(hAppContext->hDiamStackContext, &poFailedAVPList, Cx_FreeAVPValidationDataCallback);

    	DMBASE_TRACE( g_nDiamBaseTraceID, AMPS_TRACE_LEVEL_ERROR, " Validation not Successful.\n");

    	goto ExitMe;
    }




    switch(unCommandCode)
    {
        case CXDX_MSG_CMD_CODE_UAR:
        	oReturnVal = Cx_ProcessUAR(hAppContext, (t_CxUAR*)poAppMsg, hPeer_i);
			(hAppContext->oCxPerfMgmtData.unattemptedUARUR)++;
            break;
        case CXDX_MSG_CMD_CODE_SAR:
        	oReturnVal = Cx_ProcessSAR(hAppContext, (t_CxSAR*)poAppMsg, hPeer_i);
			(hAppContext->oCxPerfMgmtData.unattemptedSARUR)++;
            break;
        case CXDX_MSG_CMD_CODE_LIR:
        	oReturnVal = Cx_ProcessLIR(hAppContext, (t_CxLIR*)poAppMsg, hPeer_i);
			(hAppContext->oCxPerfMgmtData.unattemptedLIALIQ)++;
            break;
        case CXDX_MSG_CMD_CODE_MAR:
        	oReturnVal = Cx_ProcessMAR(hAppContext, (t_CxMAR*)poAppMsg, hPeer_i);
			(hAppContext->oCxPerfMgmtData.unattemptedMARMA)++;
            break;
    }

    ExitMe:

    if(CxDx_Retval_Success != oReturnVal)
    {
    	DMBase_Free(hAppContext->hDiamStackContext, (void**)&poAppMsg);
    	printf("Failed\r\n");
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}


void Cx_PeerCloseEventCallback(void*	pvAppContext_i, HPEERTABLEENTRY hPeer_i)
{
	HCXAPPCONTEXT hAppContext = (HCXAPPCONTEXT)pvAppContext_i;
//	printf("Connection Closing Call back for Cx\r\n");
//	printf("%s\r\n", hPeer_i->oHostIdentity.pucOctetString);
}


void  Cx_DBResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
		void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    //static unsigned int nCount = 0;
   // printf("DB Response Recv = %u\r\n", ++nCount);

	HCXAPPCONTEXT hCxAppContext = (HCXAPPCONTEXT)r_hAMPS_APP_HANDLE;
  //  printf("DBResHandler\r\n");

    unsigned int unEventType = 0;
    DMBase_EvtSysGetEventType(hCxAppContext->hDiamStackContext, r_pvEventHandle, &unEventType);

    unsigned int unEventDataSize = 0;//AMPS_EvtSysGetEventSize(r_hAMPS_HANDLE, r_pvEventHandle);
    DMBase_EvtSysGetEventSize(hCxAppContext->hDiamStackContext, r_pvEventHandle, &unEventDataSize);

    switch(unEventType)
    {
        case CX_UAR_DATABASE_RESPONSE_EVENT:
				Cx_ProcessUARDBRespose(	hCxAppContext,
										(unsigned char*)r_pvEventPayload,
										unEventDataSize);
            break;
        case CX_SAR_DATABASE_RESPONSE_EVENT:
			Cx_ProcessSARDBRespose(	hCxAppContext,
									(unsigned char*)r_pvEventPayload,
									unEventDataSize);
            break;
        case CX_MAR_DATABASE_RESPONSE_EVENT:
			Cx_ProcessMARDBRespose(	hCxAppContext,
									(unsigned char*)r_pvEventPayload,
									unEventDataSize);
            break;
        case CX_LIR_DATABASE_RESPONSE_EVENT:
			Cx_ProcessLIRDBRespose(	hCxAppContext,
									(unsigned char*)r_pvEventPayload,
									unEventDataSize);
            break;
    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
}

void  cx_app_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
					void* r_pvEventHandle, void* r_pvEventPayload)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");



	printf("\n\n\n\n OAMPS Request Handler Called IN CX APP \n\n\n\n");


	if ( NULL == r_pvEventPayload)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," r_pvEventPayload is NULL , Leaving\n");
		return;
	}

	HCXAPPCONTEXT	hCxAppContext = NULL;
	hCxAppContext = (HCXAPPCONTEXT) r_hAMPS_APP_HANDLE;

	if ( NULL == hCxAppContext)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hCxAppContext is NULL , Leaving\n");
		return;
	}

	t_hssOAMPSReqDataToMods*	pRcvdEvDataFromOamps = (t_hssOAMPSReqDataToMods*) r_pvEventPayload ;
	t_hssOAMPSRespDataFromMods	oRespDataToSend = {0};

	strncpy(oRespDataToSend.pucUUID,pRcvdEvDataFromOamps->pucUUID,DAPPCOMMON_UUID_SIZE_IN_BYTES);

	unsigned int		unOID =0;

	if (OidType_PerfMgmt == pRcvdEvDataFromOamps->oOIDDetails.oOidType)
	{
		unOID = pRcvdEvDataFromOamps->oOIDDetails.oPerfMgmtOid;
	}
	else if (OidType_FaultMgmt == pRcvdEvDataFromOamps->oOIDDetails.oOidType)
	{
		unOID = pRcvdEvDataFromOamps->oOIDDetails.oFaultMgmtOid;
	}

	if ( CxDx_Retval_Success != cx_app_GetValForOID(hCxAppContext,hssOIDRespType_Long,
							  unOID,&(oRespDataToSend.lResp),NULL,0)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," cx_app_GetValForOID failed for OID => %s , Leaving\n",pRcvdEvDataFromOamps->oOIDDetails.pucSupportedOID);
		return;
	}



	oRespDataToSend.oOIDDetails = pRcvdEvDataFromOamps->oOIDDetails;
	oRespDataToSend.oRespDataType = hssOIDRespType_Long;


	//send response event to oamps module
	if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(hCxAppContext->hDiamStackContext->hFrameworkHandle,
							  OAMPS_RESPONSE_FROM_CX_EVENT,
							  &oRespDataToSend,sizeof(oRespDataToSend) )
		   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_RESPONSE_FROM_CX_EVENT ,Leaving \n");
		return ;
	}



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
}




t_CxDxAPIRetVal 	cx_app_GetValForOID(HCXAPPCONTEXT		hCxAppContext_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");
	t_CxDxAPIRetVal	oRetval = CxDx_Retval_Success;

	if(NULL == hCxAppContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hCxAppContext_io is NULL \n");
		return CxDx_Retval_Null_Pointer;
	}

	if (hssOIDRespType_Long == ohssOIDRespType_i)
	{
		if(NULL == plVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," plVal_o is NULL \n");
			return CxDx_Retval_Null_Pointer;
		}
	}
	else if (hssOIDRespType_String == ohssOIDRespType_i)
	{
		if(NULL == pucVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucVal_o is NULL \n");
			return CxDx_Retval_Null_Pointer;
		}
	}


	switch(unOID_i)
	{
		case hssPMOid_attemptedUARUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedUARUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unattemptedUARUR;
			break;

		case hssPMOid_successfulUAAUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulUAAUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unsuccessfulUAAUR;
			break;

		case hssPMOid_failedUAAUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedUAAUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unfailedUAAUR;
			break;

		case hssPMOid_attemptedSARUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedSARUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unattemptedSARUR;
			break;

		case hssPMOid_successfulSAAUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulSAAUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unsuccessfulSAAUR;
			break;

		case hssPMOid_failedSAAUR:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedSAAUR OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unfailedSAAUR;
			break;

		case hssPMOid_attemptedLIALIQ:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedLIALIQ OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unattemptedLIALIQ;
			break;

		case hssPMOid_successfulLIALIQ:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulLIALIQ OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unsuccessfulLIALIQ;
			break;

		case hssPMOid_failedLIALIQ:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedLIALIQ OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unfailedLIALIQ;
			break;

		case hssPMOid_attemptedMARMA:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedMARMA OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unattemptedMARMA;
			break;

		case hssPMOid_successfulMAAMA:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulMAAMA OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unsuccessfulMAAMA;
			break;

		case hssPMOid_failedMAAMA:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedMAAMA OID \n");
				*plVal_o = hCxAppContext_io->oCxPerfMgmtData.unfailedMAAMA;
			break;


		default:
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Unsupported OID Val => %d Received  \n",unOID_i);
				oRetval = CxDx_Retval_IncorrectParameters;
			break;


	}//end switch(unOID_i)

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	return oRetval;
}



t_CxDxAPIRetVal Cx_GenerateDBRequest(HCXAPPCONTEXT hAppContext_i, unsigned int unReqType_i,
									 void* pvData_i, unsigned int unSize_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext
                             || NULL == pvData_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return CxDx_Retval_Null_Pointer;
    }

    //Return value check

    int nRetVal = AMPS_EvtSysSendIntEvt(hAppContext_i->hDiamStackContext->hFrameworkHandle,
							unReqType_i,
							pvData_i,
							unSize_i);

   /* if(AMPS_SUCCESS == nRetVal)
    {
        static unsigned int nCount = 0;
        printf("Success Events = %u\r\n", ++nCount);
    }
    else
    {
        static unsigned int nCount = 0;
        printf("Failed Events = %u\r\n", ++nCount);
    }*/

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return CxDx_Retval_Success;
}

