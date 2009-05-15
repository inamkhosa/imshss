//#include "AMPS_Defines.h"

#include "cxdx_db.h"
#include "dmbase_ampswrappers.h"
#include "hss.h"

extern	int                     g_nDiamBaseTraceID ;
int CX_GetRecord_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPI *poRecord, unsigned char* pucIMPI)
{

    if(NULL == hdbHSSThreadCtxt_io || NULL == poRecord || NULL == pucIMPI ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    unsigned int 	unQueryLength;
    unsigned char 	ucDBQuery[CX_QUERY_SIZE] = {0};
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPI_DATA].oQueryString);

    sprintf(ucDBQuery, ucQueryStr, pucIMPI);
    unQueryLength = strlen(ucDBQuery);
    t_AMPSDBEngineResult         oDBEngineResult ;

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\r\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }

    strncpy(poRecord->ucIMPI_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);
    poRecord->ucIMPI_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

    strncpy(poRecord->ucIMSU_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);
    poRecord->ucIMSU_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

    strncpy(poRecord->ucSecret_key, oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);
    poRecord->ucSecret_key[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    strncpy(poRecord->ucAuth_Scheme, oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);
    poRecord->ucAuth_Scheme[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';

    strncpy(poRecord->ucAlgorithm, oDBEngineResult.ppoAMPSDBEngineTable[0][4].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField);
    poRecord->ucAlgorithm[oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField] = '\0';

    unsigned char ucTempBuffer[25];
    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][5].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField] = '\0';


    poRecord->ulSequence_No = atoll(ucTempBuffer);

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );

    return DB_RESULT_SUCCESS;
}

int CX_GetRecord_By_IMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPU *poRecord, unsigned char *pucIMPU)
{

    if(NULL == hdbHSSThreadCtxt_io || NULL == poRecord || NULL == pucIMPU ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPU_DATA].oQueryString);

    sprintf(ucDBQuery, ucQueryStr, pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
												hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }

    strncpy(poRecord->ucIMPU_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);
    poRecord->ucIMPU_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

    unsigned char ucTempBuffer[25];
    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';
    poRecord->usReg_Status = atoi(ucTempBuffer);

    strncpy(poRecord->ucSIP_URI, oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);
    poRecord->ucSIP_URI[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    strncpy(poRecord->ucTEL_URI, oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);
    poRecord->ucTEL_URI[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';

    strncpy(poRecord->ucPSI_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][4].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField);
    poRecord->ucPSI_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField] = '\0';

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][5].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField] = '\0';
    poRecord->unServiceProfile_ID = atoi(ucTempBuffer);

    strncpy(poRecord->ucPublicService_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][6].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][6].nLengthOfField);
    poRecord->ucPublicService_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][6].nLengthOfField] = '\0';

    strncpy(poRecord->ucUserProfile_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][7].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][7].nLengthOfField);
    poRecord->ucUserProfile_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][7].nLengthOfField] = '\0';

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][8].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][8].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][8].nLengthOfField] = '\0';
    poRecord->usIsDefaultIMPU = atoi(ucTempBuffer);

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][9].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][9].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][9].nLengthOfField] = '\0';
    poRecord->usHasUnregisteredStateServices = atoi(ucTempBuffer);

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][10].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][10].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][10].nLengthOfField] = '\0';
    poRecord->unBarringIndication = atoi(ucTempBuffer);

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][11].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][11].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][11].nLengthOfField] = '\0';
    poRecord->unIdentityType = atoi(ucTempBuffer);

    strncpy(poRecord->ucDisplayName, oDBEngineResult.ppoAMPSDBEngineTable[0][12].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][12].nLengthOfField);
    poRecord->ucDisplayName[oDBEngineResult.ppoAMPSDBEngineTable[0][12].nLengthOfField] = '\0';

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][13].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][13].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][13].nLengthOfField] = '\0';
    poRecord->unMediaProfileID = atoi(ucTempBuffer);

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][14].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][14].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][14].nLengthOfField] = '\0';
    poRecord->unRepo_ID = atoi(ucTempBuffer);

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][15].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][15].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][15].nLengthOfField] = '\0';
    poRecord->usAuth_Pending = atoi(ucTempBuffer);

    strncpy(poRecord->ucSCSCF_Name, oDBEngineResult.ppoAMPSDBEngineTable[0][16].pchField,
                        oDBEngineResult.ppoAMPSDBEngineTable[0][16].nLengthOfField);
    poRecord->ucSCSCF_Name[oDBEngineResult.ppoAMPSDBEngineTable[0][16].nLengthOfField] = '\0';

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );

    return DB_RESULT_SUCCESS;
}

int CX_GetIMPI_By_IMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPI *poRecord, unsigned char* pucIMPU)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == poRecord || NULL == pucIMPU ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
//    unsigned char ucPublicID[CX_STANDARED_STRING] = {0};
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPI_BY_IMPU].oQueryString);


    sprintf(ucDBQuery, ucQueryStr , pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
														hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }

    strncpy(poRecord->ucIMPI_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);
    poRecord->ucIMPI_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

    strncpy(poRecord->ucIMSU_ID, oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);
    poRecord->ucIMSU_ID[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

    strncpy(poRecord->ucSecret_key, oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);
    poRecord->ucSecret_key[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    strncpy(poRecord->ucAuth_Scheme, oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);
    poRecord->ucAuth_Scheme[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';

    strncpy(poRecord->ucAlgorithm, oDBEngineResult.ppoAMPSDBEngineTable[0][4].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField);
    poRecord->ucAlgorithm[oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField] = '\0';

    unsigned char ucTempBuffer[25];

    strncpy(ucTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][5].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField);
    ucTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField] = '\0';

    poRecord->ulSequence_No = atoll(ucTempBuffer);

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );

    return DB_RESULT_SUCCESS;
}

int CX_GetIMPU_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoIMPURecList, unsigned char   *pucIMPI)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == pucIMPI || NULL == ppoIMPURecList ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unDBResult = DB_RESULT_SUCCESS;
    DMBase_SList *poIMPUList = NULL;
    if(DB_RESULT_SUCCESS != (unDBResult = CX_GetIMPI_IMPU_Mapping_By_IMPI(hdbHSSThreadCtxt_io, &poIMPUList, pucIMPI)))
    {
        goto ExitMe;
    }

    DMBase_SList *poTempIMPUList = poIMPUList;
    t_IMPU *poIMPU = NULL;
    unsigned char *pucNodeData = NULL;

    for(; NULL != poTempIMPUList; DMBase_SListGetNextNode(hDBHSSContext->hDiamStackContext, poTempIMPUList, &poTempIMPUList))
    {
        poIMPU = NULL;
        pucNodeData = NULL;
        DMBase_SListGetNodeData(hDBHSSContext->hDiamStackContext, poTempIMPUList, (void**)&pucNodeData);

        DMBase_Malloc(hDBHSSContext->hDiamStackContext, (void**)&poIMPU, sizeof(t_IMPU));

        if(DB_RESULT_SUCCESS != (unDBResult = CX_GetRecord_By_IMPU(hdbHSSThreadCtxt_io, poIMPU, pucNodeData)))
        {
            goto ExitMe;
        }

        DMBase_AppendSList(hDBHSSContext->hDiamStackContext, ppoIMPURecList, poIMPU);
    }

    ExitMe:
		if(NULL != poIMPUList)
		{
			DMBase_SListFree(hDBHSSContext->hDiamStackContext, &poIMPUList, Cx_FreeCharStringCallback);
		}
    //return DB_RESULT_SUCCESS;
    return unDBResult;
}

int CX_GetIMPI_IMPU_Mapping_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoIMPUList, unsigned char   *pucIMPI)
{

    if(NULL == hdbHSSThreadCtxt_io || NULL == pucIMPI || NULL == ppoIMPUList ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    unsigned int unQueryLength;
 //   unsigned char ucPublicID[CX_STANDARED_STRING] = {0};
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPI_IMPU_MAPPING_BY_IMPI].oQueryString);

    sprintf(ucDBQuery, ucQueryStr, pucIMPI);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }



    t_DiamOctetString* poPublicUserId = NULL;

    int nIndex = 0;
    unsigned char* pucPublicUserId = NULL;
    for(; nIndex<oDBEngineResult.ulNumberOfResults; nIndex++ )
    {
    	pucPublicUserId = NULL;

        DMBase_Malloc(NULL, (void**)&pucPublicUserId,
                oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField+1);
        memcpy(pucPublicUserId, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].pchField,
                oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField);
        pucPublicUserId[oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField] = '\0';

        DMBase_AppendSList(hDBHSSContext->hDiamStackContext, ppoIMPUList, pucPublicUserId);
    }
    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );
    return DB_RESULT_SUCCESS;
}


int CX_GetChrgInfo(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_ChargingInfo *poRecord, unsigned char   *pucIMPU)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == poRecord || NULL == pucIMPU ||
    		NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_CHARGING_INFO].oQueryString);

    sprintf(ucDBQuery, ucQueryStr, pucIMPU);
    unQueryLength = strlen(ucDBQuery);
    t_AMPSDBEngineResult         oDBEngineResult ;

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }

    strncpy(poRecord->ChrgCollecFName, oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);
    poRecord->ChrgCollecFName[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

    strncpy(poRecord->PrimaryChrgFName, oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);
    poRecord->PrimaryChrgFName[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    strncpy(poRecord->SecondChrgFName, oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);
    poRecord->SecondChrgFName[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';

    strncpy(poRecord->PrimaryChrgEventFName, oDBEngineResult.ppoAMPSDBEngineTable[0][4].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField);
    poRecord->PrimaryChrgEventFName[oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField] = '\0';

    strncpy(poRecord->SecondChrgFEventName, oDBEngineResult.ppoAMPSDBEngineTable[0][5].pchField,
            oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField);
    poRecord->SecondChrgFEventName[oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField] = '\0';

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );
    return DB_RESULT_SUCCESS;
}

int CX_GetServerCaps(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoCapsList)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == ppoCapsList || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE];
    unsigned int unDBResult = DB_RESULT_SUCCESS;
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_SERVER_CAPABILITIES].oQueryString);

    sprintf(ucDBQuery, ucQueryStr);
    unQueryLength = strlen(ucDBQuery);
    t_AMPSDBEngineResult oDBEngineResult ;

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
										hdbHSSThreadCtxt_io->hDBEngineHandle,
                                      ucDBQuery,
                                      unQueryLength,
                                      &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        unDBResult = DB_RESULT_FAILURE;
    }

    unsigned int unNumberOfResults   = oDBEngineResult.ulNumberOfResults;
    unsigned int unNumberOfRows      = oDBEngineResult.ulNumberOfRows;
    unsigned int unNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;
    int nRowCount = 0;
    int nColumnCount = 0;
    char cTempBuffer[20];
    t_Capability *poCapability = NULL;

    if(DB_RESULT_SUCCESS == unDBResult && 0 < unNumberOfResults)
    {
        for(nRowCount = 0 ; nRowCount<unNumberOfResults; nRowCount++)
        {

            if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][0].nLengthOfField)
            {
                poCapability = NULL;
                DMBase_Malloc(hDBHSSContext->hDiamStackContext, (void**)&poCapability, sizeof(t_Capability));
                strncpy(cTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][0].pchField,
                                    oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][0].nLengthOfField);

                cTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';
                poCapability->unCapType = MANDATORY_CAPABILITY;
                poCapability->unCapValue = atoi(cTempBuffer);
                DMBase_AppendSList(hDBHSSContext->hDiamStackContext, ppoCapsList, poCapability);

            }

            if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][1].nLengthOfField)
            {
                poCapability = NULL;
                DMBase_Malloc(hDBHSSContext->hDiamStackContext, (void**)&poCapability, sizeof(t_Capability));
                strncpy(cTempBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][1].pchField,
                                    oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][1].nLengthOfField);

                cTempBuffer[oDBEngineResult.ppoAMPSDBEngineTable[nRowCount][1].nLengthOfField] = '\0';
                poCapability->unCapType = OPTIONAL_CAPABILITY;
                poCapability->unCapValue = atoi(cTempBuffer);
                DMBase_AppendSList(hDBHSSContext->hDiamStackContext, ppoCapsList, poCapability);
            }
        }

    }
    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );
    return unDBResult;
}

int CX_Check_IMPI_IMPU_Mapping(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char* pucIMPI, unsigned char *pucIMPU)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == pucIMPI || NULL == pucIMPU || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPI_IMPU_MAPPING].oQueryString);


    sprintf(ucDBQuery, ucQueryStr, pucIMPI, pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NOT_FOUND;
    }

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );
    return DB_RESULT_SUCCESS;


}

int CX_Check_IMPU_Existence(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == pucIMPU)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_PUBLIC_USER_ID].oQueryString);

    sprintf(ucDBQuery, ucQueryStr, pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    if(0 == oDBEngineResult.ulNumberOfResults)
    {
        return DB_RESULT_NO_PUBLIC_ID;
    }

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hdbHSSThreadCtxt_io->hDBEngineHandle,&oDBEngineResult );
    return DB_RESULT_SUCCESS;
}

int CX_GetSCSCFByIMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char ucSCSCFName[], unsigned char* pucPublicUserId)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    t_IMPI oIMPI;
    DMBase_MemsetZero(oIMPI, sizeof(t_IMPI));
    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned unDBResult = CX_GetIMPI_By_IMPU(hdbHSSThreadCtxt_io, &oIMPI, pucPublicUserId);

    if(DB_RESULT_SUCCESS == unDBResult)
    {

        unsigned int unQueryLength;
        unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};

        unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

        DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_SCSCF_BY_IMPI].oQueryString);

        sprintf(ucDBQuery, ucQueryStr, oIMPI.ucIMPI_ID);

        unQueryLength = strlen(ucDBQuery);
         t_AMPSDBEngineResult         oDBEngineResult ;
        if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
																hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                              ucDBQuery,
                                                              unQueryLength,
                                                              &oDBEngineResult))
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
            unDBResult = DB_RESULT_FAILURE;
        }

        if(DB_RESULT_SUCCESS == unDBResult && 0 != oDBEngineResult.ulNumberOfResults)
        {
            strncpy(ucSCSCFName, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
                                oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);
            ucSCSCFName[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

            DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
            		hdbHSSThreadCtxt_io->hDBEngineHandle, &oDBEngineResult );
        }
        else if(DB_RESULT_SUCCESS == unDBResult && 0 == oDBEngineResult.ulNumberOfResults)
        {
        	unDBResult = DB_RESULT_NOT_FOUND;
        }

    }

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return unDBResult;
}

int CX_Update_SCSCF(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char *pucIMPU, unsigned char *pucSCSCF)
{
    if(NULL == hdbHSSThreadCtxt_io || NULL == pucIMPU || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }


    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};


    if(NULL == pucSCSCF)
    {
    	DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_CLEAR_SCSCF_BY_IMPU].oQueryString);
        sprintf(ucDBQuery, ucQueryStr, pucIMPU);
    }
    else
    {
    	DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_UPDATE_SCSCF_BY_IMPU].oQueryString);
        sprintf(ucDBQuery, ucQueryStr ,pucSCSCF, pucIMPU);
    }

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
													hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    return DB_RESULT_SUCCESS;
}

int CX_Update_Auth_Pending(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU, unsigned short usFlag)
{
    if(NULL == hdbHSSThreadCtxt_io ||NULL == pucIMPU || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_UPDATE_AUTH_PENDING_BY_IMPU].oQueryString);

    sprintf(ucDBQuery,ucQueryStr , usFlag, pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
															hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    return DB_RESULT_SUCCESS;
}

int CX_Update_Reg_Status(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU, unsigned short usFlag)
{
    if(NULL == hdbHSSThreadCtxt_io ||NULL == pucIMPU || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL Pointer Error\r\n");
        return DB_RESULT_FAILURE;
    }

    HDBHSSCONTEXT hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    unsigned int unQueryLength;
    unsigned char ucDBQuery[CX_QUERY_SIZE] = {0};
    unsigned char 	ucQueryStr[CX_QUERY_SIZE] = {0};

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_UPDATE_REG_STATUS_BY_IMPU].oQueryString);

    sprintf(ucDBQuery, ucQueryStr , usFlag, pucIMPU);

    unQueryLength = strlen(ucDBQuery);
     t_AMPSDBEngineResult         oDBEngineResult ;
    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
														hdbHSSThreadCtxt_io->hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return DB_RESULT_FAILURE;
    }

    return DB_RESULT_SUCCESS;
}



int Cx_FreeIMPUDataCallback(void** ppvData)
{
    DMBase_Free(NULL,ppvData);
    return AMPS_SUCCESS;
}

int Cx_FreeCharStringCallback(void** ppvData)
{
    DMBase_Free(NULL,ppvData);
    return AMPS_SUCCESS;
}

int Cx_FreeCapabilityDataCallback(void** ppvData)
{
    DMBase_Free(NULL,ppvData);
    return AMPS_SUCCESS;
}

int Cx_FreeDiamOctetStringCallback(void** ppvData)
{
//    t_DiamOctetString* poData = (t_DiamOctetString*)*ppvData;
 //   DMBase_Free(NULL, poData->pucOctetString);
    DMBase_Free(NULL,ppvData);
    return AMPS_SUCCESS;
}

t_Db_HSSReturnVal Cx_DB_GetCountOID_FromDBMS(
							   HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
							   unsigned int	unQueryID_i,
							   unsigned int*	punCount_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;
	HDBHSSCONTEXT			hDBMainContext = NULL;
	unsigned char			pucDBQuery[CX_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};

	if (NULL == hdbHSSThreadCtxt_io)
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hdbHSSThreadCtxt_io is NULL, Leaving\n");
        return Db_HSS_Retval_NullPointer;
    }

	if (NULL == punCount_o)
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," punCount_o is NULL, Leaving\n");
        return Db_HSS_Retval_NullPointer;
    }

	hDBMainContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;
	//fill in the place holders inside the query
	snprintf(pucDBQuery,CX_QUERY_SIZE,
			hDBMainContext->oDBQueryMapForCx[unQueryID_i].oQueryString.pucOctetString);

	//temp for debug
	printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDBQuery);


	//actually run the query on the DBMS , get the results and build the User Result TLV Record
	if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
											hDBMainContext->hDiamStackContext,
											hdbHSSThreadCtxt_io->hDBEngineHandle,
											pucDBQuery,
											strlen(pucDBQuery),
											&oDBEngineResult)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
		oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if ( 1 != oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
		oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	*punCount_o = atoi( oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField);



	ExitMe:

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	DMBASE_DBEngineFreeResults(hDBMainContext->hDiamStackContext,
								   hdbHSSThreadCtxt_io->hDBEngineHandle,
								   &oDBEngineResult);



	return oRetval;
}

t_Db_HSSReturnVal Cx_DB_GetValForOID(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");
	unsigned int		unQueryId = 0;

	if(NULL == hdbHSSThreadCtxt_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hdbHSSThreadCtxt_io is NULL \n");
		return Db_HSS_Retval_NullPointer;
	}

	if (hssOIDRespType_Long == ohssOIDRespType_i)
	{
		if(NULL == plVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," plVal_o is NULL \n");
			return Db_HSS_Retval_NullPointer;
		}
	}
	else if (hssOIDRespType_String == ohssOIDRespType_i)
	{
		if(NULL == pucVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucVal_o is NULL \n");
			return Db_HSS_Retval_NullPointer;
		}
	}

	if (hssPMOid_imsSubscriptionDBU == unOID_i)
	{
		unQueryId = CX_QUERY_CODE_GETOID_IMS_SUBS_DBU;
	}
	else if (hssPMOid_privateUserIdDBU == unOID_i)
	{
		unQueryId = CX_QUERY_CODE_GETOID_PRIV_UID_DBU;
	}
	else if (hssPMOid_regPublicUserIdRoamingDomainDBU == unOID_i)
	{
		unQueryId = CX_QUERY_CODE_GETOID_REG_PUI_ROAM;
	}
	else if (hssPMOid_unRegPublicUserIdDBU == unOID_i)
	{
		unQueryId = CX_QUERY_CODE_GETOID_UNREG_PUI;
	}
	else if (hssPMOid_regPrivateUserIdDBU == unOID_i)
	{
		unQueryId = CX_QUERY_CODE_GETOID_REG_PRIV_ID_DBU;
	}
	else
	{
		//not supported
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Unsupported Value of unOID = %d received , leaving \n",unOID_i);
		return Db_HSS_Retval_IncorrectParameters;
	}

	if ( Db_HSS_Retval_Success != Cx_DB_GetCountOID_FromDBMS(hdbHSSThreadCtxt_io,
									unQueryId, (unsigned int* ) plVal_o)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Cx_DB_GetCountOID_FromDBMS Failed , leaving \n");
		return Db_HSS_Retval_IncorrectParameters;
	}



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	return Db_HSS_Retval_Success;

}
