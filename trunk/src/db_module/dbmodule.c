#include "AMPS_API.h"
#include "events.h"
#include "dappcommon_helpers.h"
#include "dappcommon_srvrconfload.h"
#include "sh_app.h"
#include "dbmodule.h"
#include "db_cmd_id.h"
#include "dmbase_parser.h"
#include "sh_avps.h"
#include "dmbase_ampswrappers.h"
#include "db_sh_querycodes.h"

#include "sh_diamretcodes.h"
#include "dmbase_data_types.h"
#include "dmbase_ampswrappers.h"

#include "cxdx_db.h"
#include "cxdx_userprofile.h"
#include "db_cxdx_uar.h"
#include "db_cxdx_sar.h"
#include "db_cxdx_mar.h"
#include "db_cxdx_lir.h"

#include "hss.h"
#include "hss_oamps.h"
#include "dappcommon_helpers.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>





//externs
extern	int                     g_nDiamBaseTraceID ;

//globals
HDBHSSCONTEXT					g_hDB_HSSContext = NULL;



void  db_cxdx_AppRequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
								void* r_pvEventHandle, void* r_pvEventPayload)
{


	//static unsigned int nCount = 0;
	//printf("Cx Request Recv = %u\r\n", ++nCount);

	 HDBHSSTHREADCONTEXT hdbHSSThreadCtxt = (HDBHSSTHREADCONTEXT) r_pvEventData;

	 HDBHSSCONTEXT	hDBHSSContext = hdbHSSThreadCtxt->hDBHSSMainContext;

	 unsigned int unEventType = AMPS_EvtSysGetEventType(r_hAMPSHandle, r_pvEventHandle);
	 unsigned int unEventDataSize = AMPS_EvtSysGetEventSize(r_hAMPSHandle, r_pvEventHandle);

	 switch(unEventType)
	 {
		 case CX_UAR_DATABASE_REQUEST_EVENT:
			 Cx_ProcessUARDBRequest(hdbHSSThreadCtxt, (t_CxUARDBRequest*)r_pvEventPayload);
			 break;
		 case CX_SAR_DATABASE_REQUEST_EVENT:
			 Cx_ProcessSARDBRequest(hdbHSSThreadCtxt, (t_CxSARDBRequest*)r_pvEventPayload);
			 break;
		 case CX_MAR_DATABASE_REQUEST_EVENT:
			 Cx_ProcessMARDBRequest(hdbHSSThreadCtxt, (t_CxMARDBRequest*)r_pvEventPayload);
			 break;
		 case CX_LIR_DATABASE_REQUEST_EVENT:
			 Cx_ProcessLIRDBRequest(hdbHSSThreadCtxt, (t_CxLIRDBRequest*)r_pvEventPayload);
			 break;
	 }


}


void  db_ShAppRequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
				void* r_pvEventHandle, void* r_pvEventPayload)
{


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	 HDBHSSTHREADCONTEXT hdbHSSThreadCtxt = (HDBHSSTHREADCONTEXT) r_pvEventData;

	 HDBHSSCONTEXT	hDBHSSContext = hdbHSSThreadCtxt->hDBHSSMainContext;






	 unsigned int		unDBCmdId = ((t_EventDataShToDB*) r_pvEventPayload )->unEvtCommandID;

	 switch(unDBCmdId)
	 {
	 case DB_SH_CMD_CODE_UDR:
		 db_sh_ProcessUDRData(hdbHSSThreadCtxt,
							  (unsigned char*) r_pvEventPayload,sizeof(t_EventDataShToDB));
		 break;

	 case DB_SH_CMD_CODE_PNR:
		 db_sh_ProcessPNRData(hdbHSSThreadCtxt,
						(unsigned char*)r_pvEventPayload,sizeof(t_EventDataShToDB));
		 break;

	 case DB_SH_CMD_CODE_PUR:
		 db_sh_ProcessPURData(hdbHSSThreadCtxt,
							(unsigned char*)r_pvEventPayload,sizeof(t_EventDataShToDB));
		 break;

	 case DB_SH_CMD_CODE_SNR:
		 db_sh_ProcessSNRData(hdbHSSThreadCtxt,
							  (unsigned char*)r_pvEventPayload,sizeof(t_EventDataShToDB));
		 break;
	 }//end switch

	 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

}

int	Event1ValidationCallback(AMPS_APP_HANDLE r_hAMPS_APP_HANDLE, int r_nEventID,
							  void* r_pvEventPayload)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," LEaving \n");

	return AMPS_SUCCESS;
}



AMPS_MODULE_HANDLE	hss_dbmodule_ModuleInit(AMPS_HANDLE r_hAMPS_HANDLE, void* r_pvUserData)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	t_hss_db_config_params*	poHSS_DBConfParams = (t_hss_db_config_params*) r_pvUserData;

	HDIAMETERSTACKCONTEXT	hDiamStackContext = poHSS_DBConfParams->hDiameterStackContext;


	db_HSS_CreateDBHSSContext(&g_hDB_HSSContext,hDiamStackContext);

	g_hDB_HSSContext->hDiamStackContext = hDiamStackContext;
	g_hDB_HSSContext->oHSS_DB_ConfigParams = *poHSS_DBConfParams;


	AMPS_EvtSysSetEvtHandleValidationCallback(r_hAMPS_HANDLE, Event1ValidationCallback);




	g_hDB_HSSContext->hIOAgentsHandle = AMPS_IOAgentsInit(hDiamStackContext->hFrameworkHandle,
					 DB_HSS_NUMBER_OF_THREADS,
				 NULL, db_HSS_ThreadInit,  db_HSS_ThreadCleanup, g_hDB_HSSContext);

    if (NULL == g_hDB_HSSContext->hIOAgentsHandle)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsInit failed , leaving \n");
        return NULL;
    }

	printf("\n\n\n Database Module started with => %d <= threads\n\n\n",DB_HSS_NUMBER_OF_THREADS);


	//registering for events


	if ( Db_HSS_Retval_Success != db_hss_RegisterForCxEvents(g_hDB_HSSContext))
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," db_hss_RegisterForCxEvents failed , leaving \n");
        return NULL;
    }

	if ( Db_HSS_Retval_Success != db_hss_RegisterForShEvents(g_hDB_HSSContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," db_hss_RegisterForShEvents failed , leaving \n");
        return NULL;
    }

	if ( Db_HSS_Retval_Success != db_hss_RegisterForOampsEvents(g_hDB_HSSContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," db_hss_RegisterForOampsEvents failed , leaving \n");
        return NULL;
    }




	t_DiamOctetString oShDBQueryFile = {0};
    t_DiamOctetString oCxDBQueryFile = {0};
    char              pchCWD[1024]= {0};
	getcwd(pchCWD,1024);


	DMBase_CopyCharArrayToDiamString(&oShDBQueryFile,DATADIR);
	DMBase_AppendCharStringToDiamString(&oShDBQueryFile, "db_sh_queries.xml");

	if ( DappCommon_Retval_Success != DappCommon_LoadDBQueryConfigFromFile
										 (oShDBQueryFile,
										 g_hDB_HSSContext->oDBQueryMapForSh,
										 DB_SH_QUERY_MAP_SIZE))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DappCommon_LoadDBQueryConfigFromFile failed for file -> %s , LEAVING\n",oShDBQueryFile.pucOctetString);
		return NULL;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," DappCommon_LoadDBQueryConfigFromFile SUCCEEDED for SH QUERY file \n");
	}

	DMBase_CopyCharArrayToDiamString(&oCxDBQueryFile,DATADIR);
	DMBase_AppendCharStringToDiamString(&oCxDBQueryFile, "db_cx_queries.xml");
	


	if ( DappCommon_Retval_Success != DappCommon_LoadDBQueryConfigFromFile(oCxDBQueryFile,
                                                                               g_hDB_HSSContext->oDBQueryMapForCx,
                                                                               DB_CX_QUERY_MAP_SIZE))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DappCommon_LoadDBQueryConfigFromFile failed for file -> %s , LEAVING\n",oCxDBQueryFile.pucOctetString);
		return NULL;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," DappCommon_LoadDBQueryConfigFromFile SUCCEEDED for CX QUERY file \n");
	}




	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return (AMPS_MODULE_HANDLE)g_hDB_HSSContext;


}



void hss_dbmodule_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");


}

t_Db_HSSReturnVal	db_HSS_CreateDBHSSContext(HDBHSSCONTEXT* phdbHSSContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == phdbHSSContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," phdbHSSContext_io is NULL\n");
		return Db_HSS_Retval_NullPointer;
	}
	if (NULL == hDiameterStackContext_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiameterStackContext_i is NULL\n");
		return Db_HSS_Retval_NullPointer;
	}
	if (NULL != *phdbHSSContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL != *phdbHSSContext_io\n");
		return Db_HSS_Retval_IncorrectParameters;
	}


	DMBase_Malloc( hDiameterStackContext_i ,
					(void*) phdbHSSContext_io , sizeof(t_db_HSSAppContext) );


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return Db_HSS_Retval_Success;
}




t_Db_HSSReturnVal				db_sh_ProcessSNRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i)
{
	if (NULL == hdbHSSThreadContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == hdbHSSThreadContext_io\n");
		return Db_HSS_Retval_NullPointer;
	}
	if (NULL == pucEvData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucEvData_i\n");
		return Db_HSS_Retval_NullPointer;
	}

	return Db_HSS_Retval_Success;
}

t_Db_HSSReturnVal				db_sh_ProcessPNRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i)
{
	if (NULL == hdbHSSThreadContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == hdbHSSThreadContext_io\n");
		return Db_HSS_Retval_NullPointer;
	}
	if (NULL == pucEvData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucEvData_i\n");
		return Db_HSS_Retval_NullPointer;
	}

	return Db_HSS_Retval_Success;
}



t_Db_HSSReturnVal				db_sh_GetUserIdentityTypeFromDB(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
                                          unsigned char * pucUserIdentity_i,
                                          unsigned int* punIdentType_o
										  )
{

    if (NULL == hdbHSSThreadContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == hdbHSSThreadContext_io\n");
        return Db_HSS_Retval_NullPointer;
	}
	if (NULL == pucUserIdentity_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserIdentity_i\n");
        return Db_HSS_Retval_NullPointer;
	}
	if (NULL == punIdentType_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == punIdentType\n");
        return Db_HSS_Retval_NullPointer;
	}




    unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;



	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_IDENTITY_TYPE].oQueryString.pucOctetString,
			'\"',pucUserIdentity_i,'\"');



	//temp for debug
	#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
		printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
	#endif


	//actually run the query on the DBMS , get the results and build the User Result TLV Record
	if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
											hDBHSSContext->hDiamStackContext,
											hdbHSSThreadContext_io->hDBEngineHandle,
											pucDB_ShQuery,
											strlen(pucDB_ShQuery),
											&oDBEngineResult)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
		return Db_HSS_Retval_Failure;
	}

	int             nloopcounterRows = 0;
	int             nloopcounterColumns = 0;
	unsigned long   ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	unsigned long   ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	unsigned long   ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	 if (0 == oDBEngineResult.ulNumberOfResults)
	 {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oDBEngineResult.ulNumberOfResults returned 0 \n");
		return Db_HSS_Retval_NotFound;
	 }

     if (1  != oDBEngineResult.ulNumberOfResults)
     {
         //there should be one and only one entry for a given identity in the
         // identity_type_info table else something is wrong in DB Table setup

        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR! more than one entries for the user Identity \n => %s found in the identity_type_info table \n",pucUserIdentity_i);
        return Db_HSS_Retval_Failure;

     }

     unsigned char* pucTempBuff = NULL;

     pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';



    *punIdentType_o = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;

     DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
                                hdbHSSThreadContext_io->hDBEngineHandle,
                                &oDBEngineResult);


    return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal				db_sh_GetAppServPermListFromDB(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
                                          unsigned char* pucServerAddress_i,
										  unsigned int	unDataRef_i,
                                          t_db_sh_AppSrvPermissions*	poAppSrvPerm_o
										  )
{

    if (NULL == hdbHSSThreadContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == hdbHSSThreadContext_io\n");
        return Db_HSS_Retval_NullPointer;
	}
	if (NULL == pucServerAddress_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucServerAddress_i\n");
        return Db_HSS_Retval_NullPointer;
	}
	if (NULL == poAppSrvPerm_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == poAppSrvPerm_o\n");
        return Db_HSS_Retval_NullPointer;
	}



    unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;




	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_APSRV_ID_FROM_AS_ADR].oQueryString.pucOctetString,
			'\"',pucServerAddress_i,'\"');




	//temp for debug
	#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
		printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
	#endif


	//actually run the query on the DBMS , get the results
	if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
											hDBHSSContext->hDiamStackContext,
											hdbHSSThreadContext_io->hDBEngineHandle,
											pucDB_ShQuery,
											strlen(pucDB_ShQuery),
											&oDBEngineResult)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
        oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	int             nloopcounterRows = 0;
	int             nloopcounterColumns = 0;
	unsigned long   ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	unsigned long   ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	unsigned long   ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	 if (0 == oDBEngineResult.ulNumberOfResults)
	 {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oDBEngineResult.ulNumberOfResults returned 0 \n");
		oRetval = Db_HSS_Retval_NotFound;
		goto ExitMe;
	 }


     unsigned char* pucTempBuff = NULL;

     pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';



    unsigned int    unAppSrv_Id = 0;

    unAppSrv_Id  = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;


   DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
                                hdbHSSThreadContext_io->hDBEngineHandle,
                                &oDBEngineResult);



   //now run the second query

    bzero(pucDB_ShQuery ,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_APSRV_PERM_FROM_ASID].oQueryString.pucOctetString,
			unAppSrv_Id , unDataRef_i);


	//temp for debug
	#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
		printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
	#endif


	//actually run the query on the DBMS , get the results
	if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
											hDBHSSContext->hDiamStackContext,
											hdbHSSThreadContext_io->hDBEngineHandle,
											pucDB_ShQuery,
											strlen(pucDB_ShQuery),
											&oDBEngineResult)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
		oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

    ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
    ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
    ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	 if (0 == oDBEngineResult.ulNumberOfResults)
	 {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," oDBEngineResult.ulNumberOfResults returned 0 \n");
		oRetval = Db_HSS_Retval_NotFound;
		goto ExitMe;
	 }

     //getting permissions from the query result and returning in the supplied pointer to struct


     unsigned int    unUDRPerm = 0;
     unsigned int    unPURPerm = 0;
     unsigned int    unPNRPerm = 0;
     unsigned int    unSNRPerm = 0;

     //geting UDR Perm

    pucTempBuff = NULL;

    pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';


    unUDRPerm  = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;

    //geting PUR Perm

    pucTempBuff = NULL;

    pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';


    unPURPerm  = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;

    //geting PNR Perm

    pucTempBuff = NULL;

    pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';


    unPNRPerm  = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;

    //geting SNR Perm

    pucTempBuff = NULL;

    pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';


    unSNRPerm  = atoi(pucTempBuff);
    free ( pucTempBuff );
    pucTempBuff = NULL;


	ExitMe:


    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
                                hdbHSSThreadContext_io->hDBEngineHandle,
                                &oDBEngineResult);
	if (NULL != pucTempBuff)
	{
		free ( pucTempBuff );
		pucTempBuff = NULL;
	}

    // stuff the results
    poAppSrvPerm_o->unDataRef = unDataRef_i;
    poAppSrvPerm_o->bUDR_Allowed = ( 0 != unUDRPerm);
    poAppSrvPerm_o->bPUR_Allowed = ( 0 != unPURPerm);
    poAppSrvPerm_o->bPNR_Allowed = ( 0 != unPNRPerm);
    poAppSrvPerm_o->bSNR_Allowed = ( 0 != unSNRPerm);


    return oRetval;
}















void* db_HSS_ThreadInit(AMPS_HANDLE r_hAMPSHandle,
						AMPS_UIOAGENT_HANDLE r_hUIOAgentsHandle, void* hdbHSSContext_io)
{
    HDBHSSTHREADCONTEXT	hDBHSS_ThreadCtxt = NULL;
	HDBHSSCONTEXT		hDBHSSContext = (HDBHSSCONTEXT)hdbHSSContext_io;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if ( DMBase_Retval_Success != DMBase_Malloc(hDBHSSContext->hDiamStackContext,
									(void**)&hDBHSS_ThreadCtxt,sizeof(t_db_HSS_ThreadContext))
		)


    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc failed for current thread ,Leaving \n");
        return NULL;
    }

	hDBHSS_ThreadCtxt->hDBHSSMainContext = hDBHSSContext;
	hDBHSS_ThreadCtxt->hUnitIOAgent = r_hUIOAgentsHandle;


    //open connection to DB
	if ( DMBase_Retval_Success != DMBASE_DBEngin_Init( hDBHSSContext->hDiamStackContext,
						 hDBHSSContext->oHSS_DB_ConfigParams.oDBMSConfigParams.pucHostName,
						 hDBHSSContext->oHSS_DB_ConfigParams.oDBMSConfigParams.pucUserName,
						 hDBHSSContext->oHSS_DB_ConfigParams.oDBMSConfigParams.pucPassword,
						 hDBHSSContext->oHSS_DB_ConfigParams.oDBMSConfigParams.pucDatabaseName,
						 &(hDBHSS_ThreadCtxt->hDBEngineHandle))
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngin_Init failed for current thread ,Leaving \n");
        return NULL;
    }

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

    return hDBHSS_ThreadCtxt;
}

void db_HSS_ThreadCleanup(void* hDBHSS_ThreadCtxt_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	HDBHSSTHREADCONTEXT	hDBHSS_ThreadCtxt = hDBHSS_ThreadCtxt_io;
	HDBHSSCONTEXT		hdbHSSContext = hDBHSS_ThreadCtxt->hDBHSSMainContext;

	DMBASE_DBEngin_Cleanup(hdbHSSContext->hDiamStackContext,hDBHSS_ThreadCtxt->hDBEngineHandle);
	DMBase_Free(hdbHSSContext->hDiamStackContext,(void**)hDBHSS_ThreadCtxt);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}


t_Db_HSSReturnVal				db_hss_RegisterForShEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	AMPS_IOAGENTS_HANDLE hIOAgentsHandle = hdbHSSContext_io->hIOAgentsHandle;



	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle,
											SH_APP_REQUEST_EVENT,
											db_ShAppRequestHandler))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for SH_APP_REQUEST_EVENT \n");
		return Db_HSS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return Db_HSS_Retval_Success;
}

t_Db_HSSReturnVal				db_hss_RegisterForCxEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	AMPS_IOAGENTS_HANDLE hIOAgentsHandle = hdbHSSContext_io->hIOAgentsHandle;

	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle,
											CX_UAR_DATABASE_REQUEST_EVENT,
											db_cxdx_AppRequestHandler))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for CX_UAR_DATABASE_REQUEST_EVENT \n");
		return Db_HSS_Retval_Failure;
	}

	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle, CX_SAR_DATABASE_REQUEST_EVENT, db_cxdx_AppRequestHandler))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for CX_SAR_DATABASE_REQUEST_EVENT \n");
		return Db_HSS_Retval_Failure;
	}


	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle, CX_MAR_DATABASE_REQUEST_EVENT, db_cxdx_AppRequestHandler))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for CX_MAR_DATABASE_REQUEST_EVENT \n");
		return Db_HSS_Retval_Failure;
	}


	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle, CX_LIR_DATABASE_REQUEST_EVENT, db_cxdx_AppRequestHandler))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for CX_LIR_DATABASE_REQUEST_EVENT \n");
		return Db_HSS_Retval_Failure;
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return Db_HSS_Retval_Success;
}



void  db_hss_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPSHandle, void* r_pvEventData,
								void* r_pvEventHandle, void* r_pvEventPayload)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	printf("\n\n\n\n OAMPS Request Handler Called IN DB MODULE \n\n\n\n");

	if ( NULL == r_pvEventPayload)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," r_pvEventPayload is NULL , Leaving\n");
		return;
	}

	 HDBHSSTHREADCONTEXT hdbHSSThreadCtxt = (HDBHSSTHREADCONTEXT) r_pvEventData;

	 HDBHSSCONTEXT	hDBHSSContext = hdbHSSThreadCtxt->hDBHSSMainContext;



	if ( NULL == hDBHSSContext)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDBHSSContext is NULL , Leaving\n");
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

	if ( Db_HSS_Retval_Success != Cx_DB_GetValForOID(hdbHSSThreadCtxt,hssOIDRespType_Long,
							  unOID,&(oRespDataToSend.lResp),NULL,0)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Cx_DB_GetValForOID failed for OID => %s , Leaving\n",pRcvdEvDataFromOamps->oOIDDetails.pucSupportedOID);
		return;
	}

	//oRespDataToSend.lResp = ++lDummyReturn;
	oRespDataToSend.oOIDDetails = pRcvdEvDataFromOamps->oOIDDetails;
	oRespDataToSend.oRespDataType = hssOIDRespType_Long;


	//send response event to oamps module
	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadCtxt->hUnitIOAgent,
					OAMPS_RESPONSE_FROM_DB_EVENT, &oRespDataToSend, sizeof(oRespDataToSend) )
		)
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for OAMPS_RESPONSE_FROM_DB_EVENT \n\n ");
	}
	else
	{
		printf("\n\n AMPS_IOAgentsSendEvt SUCCEEDED for OAMPS_RESPONSE_FROM_DB_EVENT \n\n ");
	}



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
}


t_Db_HSSReturnVal				db_hss_RegisterForOampsEvents(
										  HDBHSSCONTEXT hdbHSSContext_io
										  )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

	AMPS_IOAGENTS_HANDLE hIOAgentsHandle = hdbHSSContext_io->hIOAgentsHandle;

	if (NULL == hdbHSSContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hdbHSSContext_io is NULL, Leaving\n");
        return Db_HSS_Retval_Failure;
	}

	if (AMPS_SUCCESS != AMPS_IOAgentsRegEvt(hIOAgentsHandle,
											OAMPS_REQUEST_TO_DB_EVENT,
											db_hss_OAMPS_RequestHandler))
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsRegEvt failed for OAMPS_REQUEST_TO_DB_EVENT, Leaving\n");
		return Db_HSS_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	return Db_HSS_Retval_Success;
}
