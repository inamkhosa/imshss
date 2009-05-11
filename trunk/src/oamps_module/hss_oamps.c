#include "hss_oamps.h"
#include "dappcommon_helpers.h"
#include "oamps_xmlAPI.h"
#include "hss.h"


//externs
extern	int                     g_nDiamBaseTraceID ;

//globals
HHSSOAMPSCONTEXT				g_hHssOampsContext=NULL;





AMPS_MODULE_HANDLE	hss_oamps_ModuleInit(AMPS_HANDLE r_hAMPS_HANDLE, void* r_pvUserData)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");
	printf("\n\n  Hello world from hss_oamps_ModuleInit \n\n");

	t_snmp_config_params*	poSNMPConfigParams = (t_snmp_config_params*)r_pvUserData;

	HDIAMETERSTACKCONTEXT	hDiamStackContext = poSNMPConfigParams->hDiameterStackContext;

	printf("\n\n\n Printing SNMP Conf PArams \n\n\n");
	printf("\npucListenerIP => %s\n",poSNMPConfigParams->oSNMPConfigParams.pucListenerIP);
	printf("\npucTrapIP => %s\n",poSNMPConfigParams->oSNMPConfigParams.pucTrapIP);
	printf("\nunListenerPort => %d\n",poSNMPConfigParams->oSNMPConfigParams.unListenerPort);
	printf("\nunTrapPort => %d\n",poSNMPConfigParams->oSNMPConfigParams.unTrapPort);

	



    
	if( hssOamps_Retval_Success !=  hss_oamps_CreateMainContext(&g_hHssOampsContext,
											hDiamStackContext)
	  )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_CreateMainContext failed, Leaving\n");
		return NULL;
	}

	g_hHssOampsContext->hDiamStackContext = hDiamStackContext;
	g_hHssOampsContext->oSNMPConfigParams = poSNMPConfigParams->oSNMPConfigParams;



      
	if(hssOamps_Retval_Success != hss_oamps_InitialiseApp(g_hHssOampsContext ))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_InitialiseApp failed, Leaving\n");
		return NULL;
	}

	if(hssOamps_Retval_Success != hss_oamps_RegisterForEvents(g_hHssOampsContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_RegisterForEvents failed, Leaving\n");
		return NULL;
	}
    


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	
	return (AMPS_MODULE_HANDLE) g_hHssOampsContext;
	
}

void hss_oamps_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}


t_hssOampsReturnVal hss_oamps_LookUpSessionObject(HHSSOAMPSCONTEXT	hHssOampsContext_io,
											unsigned char* pucSessionID_i,
											t_hssOampsSessionObject** ppohssOampsSessObj_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_HASHTABLE_KEY_HANDLE	hHashTableKeyHandle=NULL;
	t_DiamOctetString			oSessionID = {0};
	t_hssOampsReturnVal			oRetVal = hssOamps_Retval_Success;

	if ( DMBase_Retval_Success != DMBase_CopyCharArrayToDiamString(&oSessionID,
										pucSessionID_i)
	   )
	{
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CopyCharArrayToDiamString Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }
	
   
   if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hHssOampsContext_io->hDiamStackContext,
														  oSessionID,
														 &hHashTableKeyHandle)
	 )
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }
    
    if ( DMBase_Retval_Not_Found == DMBase_HashTableSearch(hHssOampsContext_io->hDiamStackContext,
														   hHssOampsContext_io->hSessTableHandle, 
														   hHashTableKeyHandle, 
														   (void**)ppohssOampsSessObj_o )
	   )
    {
         
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "DMBase_HashTableSearch did not find value in session table \n");
        oRetVal = hssOamps_Retval_NotFound;
		goto ExitMe;
    }

	ExitMe:
		DMBase_DeleteDiamString(&oSessionID);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");



	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal hss_oamps_InsertSessionObject(HHSSOAMPSCONTEXT hHssOampsContext_io,
											 unsigned char* pucSessionID_i,
											 t_hssOampsSessionObject* pohssOampsSessObj_i )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_HASHTABLE_KEY_HANDLE	hHashTableKeyHandle = NULL;
	t_DiamOctetString			oSessionID = {0};
	t_hssOampsReturnVal			oRetVal = hssOamps_Retval_Success;

	if ( DMBase_Retval_Success != DMBase_CopyCharArrayToDiamString(&oSessionID,
										pucSessionID_i)
	   )
	{
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CopyCharArrayToDiamString Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }
	
   
   if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hHssOampsContext_io->hDiamStackContext,
														  oSessionID,
														 &hHashTableKeyHandle)
	 )
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }

	


	if(DMBase_Retval_Success != DMBase_HashTableInsert(hHssOampsContext_io->hDiamStackContext, 
													   hHssOampsContext_io->hSessTableHandle,
													   (void*) pohssOampsSessObj_i, 
													   hHashTableKeyHandle)
	   )
	{

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_HashTableInsert Failed \n");
		oRetVal = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	ExitMe:
		DMBase_DeleteDiamString(&oSessionID);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal hss_oamps_RemoveSessionObject(HHSSOAMPSCONTEXT hHssOampsContext_io,
											 unsigned char* pucSessionID_i )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_HASHTABLE_KEY_HANDLE	hHashTableKeyHandle = NULL;
	t_DiamOctetString			oSessionID = {0};
	t_hssOampsReturnVal			oRetVal = hssOamps_Retval_Success;

	if ( DMBase_Retval_Success != DMBase_CopyCharArrayToDiamString(&oSessionID,
										pucSessionID_i)
	   )
	{
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CopyCharArrayToDiamString Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }
	
   
   if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hHssOampsContext_io->hDiamStackContext,
														  oSessionID,
														 &hHashTableKeyHandle)
	 )
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         oRetVal = hssOamps_Retval_Failure;
		 goto ExitMe;
    }



	if(DMBase_Retval_Success != DMBase_HashTableRemove(hHssOampsContext_io->hDiamStackContext, 
													   hHssOampsContext_io->hSessTableHandle,
                                                       hHashTableKeyHandle)
	   )
	{

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_HashTableRemove Failed \n");
		oRetVal = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	ExitMe:
		DMBase_DeleteDiamString(&oSessionID);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal hss_oamps_InitialiseApp(HHSSOAMPSCONTEXT		hHssOampsContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");


	if(NULL == hHssOampsContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Error! hHssOampsContext_io is NULL , leaving\n");
		return hssOamps_Retval_NullPointer;
	}
 
	
	if(hssOamps_Retval_Success !=hss_oamps_CreateSessionTable(hHssOampsContext_io))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " A Problem Occurred while Creating Session Table\n");
		return hssOamps_Retval_Failure;
	}


	
	//start listening on UDP port for data
	AMPS_NET_HANDLE	hAMPSNetHandle = NULL;

	if ( AMPS_SUCCESS != AMPS_CreateUDPObject(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
						 AMPS_MEMORY_SCHEME_STATIC,NULL,NULL,
						hHssOampsContext_io->oSNMPConfigParams.unListenerPort,
						hHssOampsContext_io->oSNMPConfigParams.pucListenerIP,
						 hss_oamps_ReceiveMessageOnUDP_CallBack,&hAMPSNetHandle,
						 (AMPS_APP_HANDLE) hHssOampsContext_io,NULL)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " AMPS_CreateUDPObject  failed , leaving\n");
		return hssOamps_Retval_Failure;
	}


	unsigned char*	pucLocalIP = NULL;

	if ( DMBase_Retval_Success != DMBase_GetIPAddressFromDiameterContext(
				hHssOampsContext_io->hDiamStackContext,(char**)&pucLocalIP)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_GetIPAddressFromDiameterContext  failed for creating trap, leaving\n");
		return hssOamps_Retval_Failure;
	}

	//create UDP Object for trap sending
	if ( AMPS_SUCCESS != AMPS_CreateUDPObject(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
						 AMPS_MEMORY_SCHEME_STATIC,NULL,NULL,
						0,//dont care which local port is used
						pucLocalIP,
						 NULL,&(hHssOampsContext_io->hAMPSNetHandleForTraps),
						 NULL,NULL)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " AMPS_CreateUDPObject  failed for creating trap, leaving\n");
		DMBase_Free(hHssOampsContext_io->hDiamStackContext,(void**)&pucLocalIP);
		return hssOamps_Retval_Failure;
	}

	DMBase_Free(hHssOampsContext_io->hDiamStackContext,(void**)&pucLocalIP);


	if ( hssOamps_Retval_Success != hss_oamps_PopulateOID_ToAppMapping(g_hHssOampsContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " hss_oamps_PopulateOID_ToAppMapping  failed , leaving\n");
		return hssOamps_Retval_Failure;
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal hss_oamps_CreateSessionTable(HHSSOAMPSCONTEXT	hHssOampsContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if(NULL == hHssOampsContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Error! hHssOampsContext_io is NULL , leaving\n");
		return hssOamps_Retval_NullPointer;
	}

	DMBASE_HASH_HANDLE       hSessTableHandle   = NULL ;

	if(DMBase_Retval_Success != DMBase_CreateHashTable(hHssOampsContext_io->hDiamStackContext,
													   OAMPS_HSS_SESSION_HASHTAB_MAX_SIZE,
													   &hSessTableHandle,NULL))
	{
		  DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_CreateHashTable Failed ! \n");
		  return hssOamps_Retval_Failure;
	}

	hHssOampsContext_io->hSessTableHandle = hSessTableHandle;


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal	hss_oamps_CreateMainContext(HHSSOAMPSCONTEXT*	pohHssOampsContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i
											  )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == pohHssOampsContext_io) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pohHssOampsContext_io is NULL\n");
		return hssOamps_Retval_NullPointer;
	}
	if (NULL == hDiameterStackContext_i) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiameterStackContext_i is NULL\n");
		return hssOamps_Retval_NullPointer;
	}
	if (NULL != *pohHssOampsContext_io) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL != *pohHssOampsContext_io\n");
		return hssOamps_Retval_IncorrectParameters;
	}


	if (DMBase_Retval_Success !=  DMBase_Malloc( hDiameterStackContext_i ,
				    (void*) pohHssOampsContext_io , sizeof(t_hssOampsContext) )
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed \n");
		return hssOamps_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return hssOamps_Retval_Success;
}

int hss_oamps_ReceiveMessageOnUDP_CallBack( AMPS_HANDLE     r_hAMPS_HANDLE, 
                                AMPS_APP_HANDLE hAMPS_APP_HANDLE_i, 
                                AMPS_NET_HANDLE hAMPS_NET_HANDLE_i, 
                                unsigned char*  pucBuff_i, 
                                int             nBuffLength_i, 
                                int             nPort_i, 
                                char*           pcIPAddress_i)
{
     

   DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

   HHSSOAMPSCONTEXT	hHssOampsContext_io = (HHSSOAMPSCONTEXT)hAMPS_APP_HANDLE_i;

    printf("\n\nData received Size  = %d Bytes\n\n" , nBuffLength_i);

    if (0 == nBuffLength_i) 
    {
            return 1;
    }   

	printf("\n\n Data Received == %s \n\n",pucBuff_i);


	if (0 == strncmp("suicide",pucBuff_i,7)) 
	{
		printf("\n\n suicide command received , stopping AMPS \n\n ");
		AMPS_Stop();
		return 1;
	}

	t_hssOIDDetails 		oCurrOIDDetails = {0};
	struct oampCommand		oRcvdOAMPSCmd={0};
	

	oampProcessResponseNode(&oRcvdOAMPSCmd,pucBuff_i);

	printf("\n\n Received Command is for OID  => %s \n\n",oRcvdOAMPSCmd.oid);
	
	

	if (hssOamps_Retval_Success != hss_oamps_GetOIDDetailsFromMapping(hHssOampsContext_io,
																	oRcvdOAMPSCmd.oid,
																	  &oCurrOIDDetails,
																	  0, OidDetailsByName)
								   
	   ) 
	{
		printf("\n\n Unsupported Command  Received => %s \n\n",oRcvdOAMPSCmd.oid);
		return 1;
		
	}

    

	unsigned char pucUUID[DAPPCOMMON_UUID_SIZE_IN_BYTES] = {0};
	

	DappCommon_GiveNewUUID(pucUUID,DAPPCOMMON_UUID_SIZE_IN_BYTES);

	printf("\n\n The new GUID Generated is => \n\t\t %s \n\n",pucUUID);


	//Make a session object 

	t_hssOampsSessionObject*	poSessObj = NULL;
	if ( DMBase_Retval_Success !=  DMBase_Malloc(hHssOampsContext_io->hDiamStackContext,
							(void**)&poSessObj,sizeof(t_hssOampsSessionObject))
		 )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed ,Leaving \n");
		return 1;
	}

	poSessObj->hHssOampsContext = hHssOampsContext_io;
	poSessObj->hAMPSNetHandleForPeer = hAMPS_NET_HANDLE_i;
	poSessObj->unPeerPort = nPort_i;
	strncpy(poSessObj->pucPeerIPAddr,pcIPAddress_i,OAMPS_HSS_MAX_SMALL_STRING_SIZE);
	strncpy(poSessObj->pcOampsReqOpCode,oRcvdOAMPSCmd.opcode,OAMP_OPCODE_LENGTH);


	printf("\n\n In callback hAMPS_NET_HANDLE_i == %x\n\n",hAMPS_NET_HANDLE_i);
	printf("\n\n In callback nPort_i == %d\n\n",nPort_i);
	printf("\n\n In callback pcIPAddress_i == %s\n\n",pcIPAddress_i);
	printf("\n\n In callback poSessObj->unPeerPort == %d\n\n",poSessObj->unPeerPort);
	

	//insert into session table
	if ( hssOamps_Retval_Success != hss_oamps_InsertSessionObject(
		         hHssOampsContext_io,pucUUID,poSessObj)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_InsertSessionObject Failed ,Leaving \n");
		return 1;
	}

	//prepare event data
	t_hssOAMPSReqDataToMods	oReqDataToMods = {0};
	oReqDataToMods.oOIDDetails = oCurrOIDDetails;
	strncpy(oReqDataToMods.pucUUID,pucUUID,DAPPCOMMON_UUID_SIZE_IN_BYTES);

	//send message to HSS modules
	if ( 
		(hssAppType_Cx == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDDB == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDSh == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDShANDDB == oCurrOIDDetails.oHssOampsAppType)
	   )
	{
		poSessObj->bWaitRespFromCx = 1;
		//send event to Cx module

		if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(r_hAMPS_HANDLE,OAMPS_REQUEST_TO_CX_EVENT,
							  &oReqDataToMods,sizeof(oReqDataToMods) )
		   )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_REQUEST_TO_CX_EVENT ,Leaving \n");
			return 1;
		}
	}

	if ( 
		(hssAppType_Sh == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDShANDDB == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDSh == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_ShANDDB == oCurrOIDDetails.oHssOampsAppType)
	   )
	{
		poSessObj->bWaitRespFromSh = 1;
		//send event to Sh module
		if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(r_hAMPS_HANDLE,OAMPS_REQUEST_TO_SH_EVENT,
							  &oReqDataToMods,sizeof(oReqDataToMods) )
		   )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_REQUEST_TO_SH_EVENT ,Leaving \n");
			return 1;
		}
	}

	if ( 
		(hssAppType_DB == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDDB == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_ShANDDB == oCurrOIDDetails.oHssOampsAppType) ||
		(hssAppType_CxANDShANDDB == oCurrOIDDetails.oHssOampsAppType)
	   )
	{
		poSessObj->bWaitRespFromDb = 1;
		//send event to DB module
		if ( AMPS_SUCCESS != AMPS_EvtSysSendIntEvt(r_hAMPS_HANDLE,OAMPS_REQUEST_TO_DB_EVENT,
							  &oReqDataToMods,sizeof(oReqDataToMods) )
		   )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendIntEvt Failed for OAMPS_REQUEST_TO_DB_EVENT ,Leaving \n");
			return 1;
		}
	}




	
	


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
    return AMPS_SUCCESS;
}

t_hssOampsReturnVal hss_oamps_PopulateOID_ToAppMapping(HHSSOAMPSCONTEXT hHssOampsContext_io )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == hHssOampsContext_io) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
		return hssOamps_Retval_NullPointer;
	}

	unsigned int	unIndex = 0;

	//FaultManagement Oid Settings Begin
	

	//Cx FM oids begin

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdUAR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdUAR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdSAR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdSAR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdMAR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdMAR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdLIR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdLIR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 //Cx FM oids end


	//Sh FM oids begin

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdUDR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdUDR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_failedThresholdPUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedThresholdPUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	//Sh FM oids end

	//CxAndSH FM oids begin

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_CxANDSh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_listenPortBindFailed;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "listenPortBindFailed",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	

	//CxANDSH FM oids end

	 //DB fm OIDs begin
	  hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_FaultMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid = hssFMOid_downDB;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "downDB",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 //DB fm OIDs end





	//FaultManagement Oid Settings End


	//PerfManagement Oid Settings Begin

	
	 //DB PM oids begin
	  hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_imsSubscriptionDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "imsSubscriptionDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_privateUserIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "privateUserIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	  hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_sipPublicUserIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "sipPublicUserIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_telPublicUserIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "telPublicUserIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_privateSrvIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "privateSrvIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_sipPublicSrvIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "sipPublicSrvIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_telPublicSrvIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "telPublicSrvIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_regPublicUserIdRoamingDomainDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "regPublicUserIdRoamingDomainDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_unRegPublicUserIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "unRegPublicUserIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_DB;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_regPrivateUserIdDBU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "regPrivateUserIdDBU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	 //DB PM oids End
	 
	//Cx PM oids begin

	
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedUARUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedUARUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulUAAUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulUAAUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedUAAUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedUAAUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedSARUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedSARUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulSAAUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulSAAUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedSAAUR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedSAAUR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedLIALIQ;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedLIALIQ",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulLIALIQ;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulLIALIQ",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedLIALIQ;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedLIALIQ",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedMARMA;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedMARMA",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulMAAMA;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulMAAMA",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Cx;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedMAAMA;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedMAAMA",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	//Cx PM oids end



	//SH PM oids begin

      

    
  

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedUDRDTR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedUDRDTR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulUDADTR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulUDADTR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedUDADTR;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedUDADTR",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

 

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_attemptedPURDTU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "attemptedPURDTU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_successfulPUADTU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "successfulPUADTU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;

	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oHssOampsAppType = hssAppType_Sh;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oOidType = OidType_PerfMgmt;
	 hHssOampsContext_io->oOIDDetailsArray[unIndex].oPerfMgmtOid = hssPMOid_failedPUADTU;
	 strncpy(hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
			 "failedPUADTU",OAMPS_HSS_MAX_OID_STRING_SIZE);

	 unIndex++;


	//SH PM oids end
	
	//PerfManagement Oid Settings End

	 printf("\n\n Total %d Oids info added to mapping table ",unIndex);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return hssOamps_Retval_Success;
}

t_hssOampsReturnVal hss_oamps_GetOIDDetailsFromMapping(HHSSOAMPSCONTEXT hHssOampsContext_io,
								unsigned char* pucOIDName_i,
								t_hssOIDDetails* poOIDDetails_o ,
								unsigned int	unOidEnum_i,
								t_OidDetailsQueryCriteria oQueryCriteria)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	
	unsigned int	unIndex=0;
	_Bool			bFound=0;

	if (NULL == hHssOampsContext_io) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO,"hHssOampsContext_io is NULL  Leaving \n");
		return hssOamps_Retval_NullPointer;
	}

	if (OidDetailsByName == oQueryCriteria ) 
	{
		if (NULL == pucOIDName_i) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO,"pucOIDName_i is NULL, Leaving \n");
			return hssOamps_Retval_NullPointer;
		}
	}
	

	if (NULL == poOIDDetails_o) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO,"poOIDDetails_o is NULL, Leaving \n");
		return hssOamps_Retval_NullPointer;
	}

	for (unIndex=0; unIndex<OAMPS_HSS_MAX_SUPPORTED_OIDS; unIndex++) 
	{
		if (OidDetailsByName == oQueryCriteria ) 
		{
			if ( 0 == strncmp(pucOIDName_i, 
				hHssOampsContext_io->oOIDDetailsArray[unIndex].pucSupportedOID,
					OAMPS_HSS_MAX_OID_STRING_SIZE)
		       ) 
			{
				*poOIDDetails_o = hHssOampsContext_io->oOIDDetailsArray[unIndex];
				bFound = 1;
				break;
			}
		}//end if by name
		else if (OidDetailsByEnum == oQueryCriteria ) 
		{
			if ( unOidEnum_i == hHssOampsContext_io->oOIDDetailsArray[unIndex].oFaultMgmtOid)
					
			{
				*poOIDDetails_o = hHssOampsContext_io->oOIDDetailsArray[unIndex];
				bFound = 1;
				break;
			}
		}//end if by enum
		
	}//end for

	

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	if (0 == bFound) 
	{
		return hssOamps_Retval_NotFound;
	}
	else
	{
		return hssOamps_Retval_Success;
	}
	
}

void  hss_oamps_ShResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	HHSSOAMPSCONTEXT	hHssOampsContext = (HHSSOAMPSCONTEXT) r_hAMPS_APP_HANDLE;

	if (NULL == hHssOampsContext) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hHssOampsContext is NULL , Leaving \n");
		return;
	}

	t_hssOAMPSRespDataFromMods*		pRespData = (t_hssOAMPSRespDataFromMods*)r_pvEventPayload;
	t_hssOampsSessionObject*		poSessObj = NULL;

	if (NULL == pRespData) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pRespData is NULL , Leaving \n");
		return;
	}

	//lookup session object for this response
	if ( hssOamps_Retval_Success != hss_oamps_LookUpSessionObject(hHssOampsContext,
									pRespData->pucUUID,&poSessObj)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_LookUpSessionObject failed , Leaving \n");
		return;
	}

	//got response so clear flag
	poSessObj->bWaitRespFromSh = 0;

	if (1 == poSessObj->bWaitRespFromDb) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Db so not processing further , Leaving \n");
		return;
	}

	if (1 == poSessObj->bWaitRespFromCx) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Cx so not processing further , Leaving \n");
		return;
	}

	//Make XML response buffer to send to peer
	unsigned char* pucXMLBuffToSend= NULL;

	

	if ( hssOamps_Retval_Success != oampMakeResponse(hHssOampsContext->hDiamStackContext,
					 pRespData->oOIDDetails.pucSupportedOID,
					  poSessObj->pcOampsReqOpCode,
					 pRespData->lResp,
					 NULL,oampsCmdValType_long,200,"OK",&pucXMLBuffToSend)
	    )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oampMakeResponse Failed , Leaving \n");
		return;
	}

	
	printf("\n\n Just before AMPS Send Buffer \n\n ");


	//send buffer 

	printf("\n\n poSessObj->hAMPSNetHandleForPeer == %x",poSessObj->hAMPSNetHandleForPeer);
	printf("\n\n poSessObj->pucPeerIPAddr == %s",poSessObj->pucPeerIPAddr);
	printf("\n\n poSessObj->unPeerPort == %x",poSessObj->unPeerPort);

	

	printf("\n\n Printing the XML generated \n\n ");
	printf("\n\n poXmlBuffToSend == %s\n\n",pucXMLBuffToSend);


	if ( AMPS_SUCCESS != AMPS_SndBuffer(hHssOampsContext->hDiamStackContext->hFrameworkHandle,
							poSessObj->hAMPSNetHandleForPeer,
							pucXMLBuffToSend,strlen(pucXMLBuffToSend),
							poSessObj->unPeerPort,
							poSessObj->pucPeerIPAddr)
       )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_SndBuffer Failed , Leaving \n");
		return;
	}


	printf("\n\n Just after  AMPS Send Buffer \n\n ");

	//remove session object from session table
	if ( hssOamps_Retval_Success != hss_oamps_RemoveSessionObject(hHssOampsContext,
									pRespData->pucUUID)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_RemoveSessionObject Failed for UUID == %s, Leaving \n",pRespData->pucUUID);
		return;
	}


	//Free xml Buffer
	if (NULL != pucXMLBuffToSend) 
	{
		DMBase_Free(hHssOampsContext->hDiamStackContext,(void**) &pucXMLBuffToSend);
	}
	


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}

void  hss_oamps_DbResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	HHSSOAMPSCONTEXT	hHssOampsContext = (HHSSOAMPSCONTEXT) r_hAMPS_APP_HANDLE;

	if (NULL == hHssOampsContext) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hHssOampsContext is NULL , Leaving \n");
		return;
	}

	t_hssOAMPSRespDataFromMods*		pRespData = (t_hssOAMPSRespDataFromMods*)r_pvEventPayload;
	t_hssOampsSessionObject*		poSessObj = NULL;

	if (NULL == pRespData) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pRespData is NULL , Leaving \n");
		return;
	}

	//lookup session object for this response
	if ( hssOamps_Retval_Success != hss_oamps_LookUpSessionObject(hHssOampsContext,
									pRespData->pucUUID,&poSessObj)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_LookUpSessionObject failed , Leaving \n");
		return;
	}

	//got response so clear flag
	poSessObj->bWaitRespFromDb = 0;

	if (1 == poSessObj->bWaitRespFromSh) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Sh so not processing further , Leaving \n");
		return;
	}

	if (1 == poSessObj->bWaitRespFromCx) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Cx so not processing further , Leaving \n");
		return;
	}

	//Make XML response buffer to send to peer
	unsigned char* pucXMLBuffToSend= NULL;

	

	if ( hssOamps_Retval_Success != oampMakeResponse(hHssOampsContext->hDiamStackContext,
					 pRespData->oOIDDetails.pucSupportedOID,
					  poSessObj->pcOampsReqOpCode,
					 pRespData->lResp,
					 NULL,oampsCmdValType_long,200,"OK",&pucXMLBuffToSend)
	    )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oampMakeResponse Failed , Leaving \n");
		return;
	}

	
	printf("\n\n Just before AMPS Send Buffer \n\n ");


	//send buffer 

	printf("\n\n poSessObj->hAMPSNetHandleForPeer == %x",poSessObj->hAMPSNetHandleForPeer);
	printf("\n\n poSessObj->pucPeerIPAddr == %s",poSessObj->pucPeerIPAddr);
	printf("\n\n poSessObj->unPeerPort == %x",poSessObj->unPeerPort);

	

	printf("\n\n Printing the XML generated \n\n ");
	printf("\n\n poXmlBuffToSend == %s\n\n",pucXMLBuffToSend);


	if ( AMPS_SUCCESS != AMPS_SndBuffer(hHssOampsContext->hDiamStackContext->hFrameworkHandle,
							poSessObj->hAMPSNetHandleForPeer,
							pucXMLBuffToSend,strlen(pucXMLBuffToSend),
							poSessObj->unPeerPort,
							poSessObj->pucPeerIPAddr)
       )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_SndBuffer Failed , Leaving \n");
		return;
	}


	printf("\n\n Just after  AMPS Send Buffer \n\n ");

	//remove session object from session table
	if ( hssOamps_Retval_Success != hss_oamps_RemoveSessionObject(hHssOampsContext,
									pRespData->pucUUID)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_RemoveSessionObject Failed for UUID == %s, Leaving \n",pRespData->pucUUID);
		return;
	}


	//Free xml Buffer
	if (NULL != pucXMLBuffToSend) 
	{
		DMBase_Free(hHssOampsContext->hDiamStackContext,(void**) &pucXMLBuffToSend);
	}
	


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}

void  hss_oamps_CxResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	HHSSOAMPSCONTEXT	hHssOampsContext = (HHSSOAMPSCONTEXT) r_hAMPS_APP_HANDLE;

	if (NULL == hHssOampsContext) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hHssOampsContext is NULL , Leaving \n");
		return;
	}

	t_hssOAMPSRespDataFromMods*		pRespData = (t_hssOAMPSRespDataFromMods*)r_pvEventPayload;
	t_hssOampsSessionObject*		poSessObj = NULL;

	if (NULL == pRespData) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pRespData is NULL , Leaving \n");
		return;
	}

	//lookup session object for this response
	if ( hssOamps_Retval_Success != hss_oamps_LookUpSessionObject(hHssOampsContext,
									pRespData->pucUUID,&poSessObj)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_LookUpSessionObject failed , Leaving \n");
		return;
	}

	//got response so clear flag
	poSessObj->bWaitRespFromCx = 0;

	if (1 == poSessObj->bWaitRespFromSh) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Sh so not processing further , Leaving \n");
		return;
	}

	if (1 == poSessObj->bWaitRespFromDb) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Waiting for response from Db so not processing further , Leaving \n");
		return;
	}

	//Make XML response buffer to send to peer
	unsigned char* pucXMLBuffToSend= NULL;

	

	if ( hssOamps_Retval_Success != oampMakeResponse(hHssOampsContext->hDiamStackContext,
					 pRespData->oOIDDetails.pucSupportedOID,
					  poSessObj->pcOampsReqOpCode,
					 pRespData->lResp,
					 NULL,oampsCmdValType_long,200,"OK",&pucXMLBuffToSend)
	    )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oampMakeResponse Failed , Leaving \n");
		return;
	}

	
	printf("\n\n Just before AMPS Send Buffer \n\n ");


	//send buffer 

	printf("\n\n poSessObj->hAMPSNetHandleForPeer == %x",poSessObj->hAMPSNetHandleForPeer);
	printf("\n\n poSessObj->pucPeerIPAddr == %s",poSessObj->pucPeerIPAddr);
	printf("\n\n poSessObj->unPeerPort == %d",poSessObj->unPeerPort);
	

	

	printf("\n\n Printing the XML generated \n\n ");
	printf("\n\n poXmlBuffToSend == %s\n\n",pucXMLBuffToSend);


	if ( AMPS_SUCCESS != AMPS_SndBuffer(hHssOampsContext->hDiamStackContext->hFrameworkHandle,
							poSessObj->hAMPSNetHandleForPeer,
							pucXMLBuffToSend,strlen(pucXMLBuffToSend),
							poSessObj->unPeerPort,
							poSessObj->pucPeerIPAddr)
       )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_SndBuffer Failed , Leaving \n");
		return;
	}


	printf("\n\n Just after  AMPS Send Buffer \n\n ");

	//remove session object from session table
	if ( hssOamps_Retval_Success != hss_oamps_RemoveSessionObject(hHssOampsContext,
									pRespData->pucUUID)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_RemoveSessionObject Failed for UUID == %s, Leaving \n",pRespData->pucUUID);
		return;
	}


	//Free xml Buffer
	if (NULL != pucXMLBuffToSend) 
	{
		DMBase_Free(hHssOampsContext->hDiamStackContext,(void**) &pucXMLBuffToSend);
	}
	


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}

t_hssOampsReturnVal hss_oamps_RegisterForEvents(HHSSOAMPSCONTEXT		hHssOampsContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == hHssOampsContext_io) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"hHssOampsContext_io is NULL  Leaving \n");
		return hssOamps_Retval_NullPointer;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
		  OAMPS_RESPONSE_FROM_CX_EVENT, hss_oamps_CxResponseHandler, hHssOampsContext_io)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"AMPS_EvtSysRegExtEvt Failed For OAMPS_RESPONSE_FROM_CX_EVENT  Leaving \n");
		return hssOamps_Retval_Failure;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
		  OAMPS_RESPONSE_FROM_SH_EVENT, hss_oamps_ShResponseHandler, hHssOampsContext_io)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"AMPS_EvtSysRegExtEvt Failed For OAMPS_RESPONSE_FROM_SH_EVENT  Leaving \n");
		return hssOamps_Retval_Failure;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
		  OAMPS_RESPONSE_FROM_DB_EVENT, hss_oamps_DbResponseHandler, hHssOampsContext_io)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"AMPS_EvtSysRegExtEvt Failed For OAMPS_RESPONSE_FROM_DB_EVENT  Leaving \n");
		return hssOamps_Retval_Failure;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hHssOampsContext_io->hDiamStackContext->hFrameworkHandle,
		  OAMPS_SEND_TRAP_EVENT, hss_oamps_TrapEventHandler, hHssOampsContext_io)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"AMPS_EvtSysRegExtEvt Failed For OAMPS_SEND_TRAP_EVENT  Leaving \n");
		return hssOamps_Retval_Failure;
	}

	

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return hssOamps_Retval_Success;

}

void  hss_oamps_TrapEventHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, 
					void* r_pvEventHandle, void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	HHSSOAMPSCONTEXT	hHssOampsContext = (HHSSOAMPSCONTEXT) r_hAMPS_APP_HANDLE;

	if (NULL == hHssOampsContext) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hHssOampsContext is NULL , Leaving \n");
		return;
	}

	t_hssOAMPSTrapEventData*		pTrapEvData = (t_hssOAMPSTrapEventData*)r_pvEventPayload;
	

	if (NULL == pTrapEvData) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pTrapEvData is NULL , Leaving \n");
		return;
	}

	

	

	//Make XML Trap buffer to send to send
	unsigned char* 		pucXMLBuffToSend= NULL;
	t_hssOIDDetails		oOidDetails = {0};
	
	if ( hssOamps_Retval_Success != hss_oamps_GetOIDDetailsFromMapping(hHssOampsContext,
																	   NULL,
																	   &oOidDetails,
								pTrapEvData->oFaultMgmtOid,OidDetailsByEnum)
	    )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hss_oamps_GetOIDDetailsFromMapping Failed , Leaving \n");
		return;
	}

	

	if ( hssOamps_Retval_Success != oampMakeTrapBuffer(hHssOampsContext->hDiamStackContext,
													   oOidDetails.pucSupportedOID,
                                                       oampsTrapLowSeverity,
													   pTrapEvData->lValue,
													   1,"Test",&pucXMLBuffToSend)
	    )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oampMakeTrapBuffer Failed , Leaving \n");
		return;
	}

	
	printf("\n\n Just before AMPS Send Buffer \n\n ");


	//send buffer 

	

	printf("\n\n Printing the XML generated \n\n ");
	printf("\n\n poXmlBuffToSend == %s\n\n",pucXMLBuffToSend);


	if ( AMPS_SUCCESS != AMPS_SndBuffer(hHssOampsContext->hDiamStackContext->hFrameworkHandle,
							hHssOampsContext->hAMPSNetHandleForTraps,
							pucXMLBuffToSend,strlen(pucXMLBuffToSend),
							hHssOampsContext->oSNMPConfigParams.unTrapPort,
							hHssOampsContext->oSNMPConfigParams.pucTrapIP)
       )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_SndBuffer Failed For Sending TRAP BUFFER, Leaving \n");
		return;
	}


	printf("\n\n Just after  AMPS Send Trap Buffer \n\n ");

	

	//Free xml Buffer
	if (NULL != pucXMLBuffToSend) 
	{
		DMBase_Free(hHssOampsContext->hDiamStackContext,(void**) &pucXMLBuffToSend);
	}
	


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}
