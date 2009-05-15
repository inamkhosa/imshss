
#include "cxdx_userprofile.h"
#include "cxdx_db.h"
#include "hss.h"

//externs
extern	int     g_nDiamBaseTraceID ;

t_Db_HSSReturnVal CxDx_GetUserProfile(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                      unsigned char *pucUserID,
                                      unsigned char **ppucUserProfile)
{

    DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
	HDBHSSCONTEXT	hDBHSSContext = hdbHSSThreadCtxt_io->hDBHSSMainContext;

    LIBXML_TEST_VERSION

    xmlTextWriterPtr pWriter;
    xmlBufferPtr pBuffer;
    int nResultCode;

    pBuffer = xmlBufferCreate();

    if (NULL == pBuffer)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR,"Error creating the xml buffer\n");
        return Db_HSS_Retval_QueryFailed;
    }

    pWriter = xmlNewTextWriterMemory(pBuffer, 0);
    if (NULL == pWriter)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Error creating the xml writer\n");
        return Db_HSS_Retval_Failure;
    }

    nResultCode = xmlTextWriterStartDocument(pWriter, NULL, USER_PROFILE_ENCODING, NULL);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating xml document\n");
        return Db_HSS_Retval_Failure;
    }

    xmlTextWriterSetIndent(pWriter, 1);

    nResultCode = xmlTextWriterStartElement(pWriter, TAG_IMS_SUBSCRIPTION );
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
        return Db_HSS_Retval_Failure;
    }

    nResultCode = xmlTextWriterWriteFormatElement(pWriter, TAG_PRIVATE_ID, "%s", pucUserID);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
        return Db_HSS_Retval_Failure;
    }

    CxDx_AddServiceProfileTags(hdbHSSThreadCtxt_io, pWriter, pucUserID);

    nResultCode = xmlTextWriterEndDocument(pWriter);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
        return Db_HSS_Retval_Failure;
    }

    xmlFreeTextWriter(pWriter);
    int unContentLength = strlen(pBuffer->content);
    DMBase_Malloc(hDBHSSContext->hDiamStackContext, (void**)ppucUserProfile, unContentLength + 1);//malloc(unContentLength + 1);
    strcpy(*ppucUserProfile, pBuffer->content);
    xmlBufferFree(pBuffer);
    return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal CxDx_AddServiceProfileTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                             xmlTextWriterPtr pWriter,
                                             unsigned char    *pucUserID)
{
    DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;
    unsigned char        ucQueryStr[CX_QUERY_SIZE];

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_SSERVICEPROFILE_RECORD_BY_IMPI].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, pucUserID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }


    int nRows = oDBEngineResult.ulNumberOfRows;
    int nIndex;
    unsigned char ucBuffer[20];

    for(nIndex = 0; nIndex < nRows; nIndex++)
    {
        nResultCode = xmlTextWriterStartElement(pWriter, TAG_SERVICE_PROFILE);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            return Db_HSS_Retval_Failure;
        }
        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].pchField,
                            oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField);
        CxDx_AddPublicIdTags(hdbHSSThreadCtxt_io, pWriter, atoi(ucBuffer));
        CxDx_AddIFCTags(hdbHSSThreadCtxt_io, pWriter, atoi(ucBuffer));

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField)
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].pchField,
                                oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField);
            CxDx_AddCoreNetworkServiceAuthTag(hdbHSSThreadCtxt_io,  pWriter, atoi(ucBuffer));
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField)
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].pchField,
                                oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField);
            CxDx_AddExtensionTag(hdbHSSThreadCtxt_io,  pWriter, atoi(ucBuffer));
        }

        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            return Db_HSS_Retval_Failure;
        }

     }


     //Now free results
    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal CxDx_AddPublicIdTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                       xmlTextWriterPtr pWriter,
                                       unsigned int     unServiceProfile_ID)
{
	DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;
    unsigned char        ucQueryStr[CX_QUERY_SIZE];

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IMPU_RECORD_BY_SERVICEPROFILE_ID].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unServiceProfile_ID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }


    int nRows = oDBEngineResult.ulNumberOfRows;
    int nIndex;
    unsigned char ucBuffer[70];

    t_Db_HSSReturnVal oReturnValue = Db_HSS_Retval_Success;

    for(nIndex = 0; nIndex<nRows; nIndex++)
    {
        nResultCode = xmlTextWriterStartElement(pWriter, TAG_PUBLIC_IDENTITY);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].pchField,
                                   oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField);

        if(0 != atoi(ucBuffer))
        {
            nResultCode = xmlTextWriterWriteFormatElement(pWriter, TAG_BARING_INDICATION, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].pchField,
                                   oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter, TAG_IDENTITY, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        nResultCode = xmlTextWriterStartElement(pWriter, TAG_EXTENSION);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].pchField,
                                   oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter, TAG_IDENTITY_TYPE, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        //Closing Extension Tag
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        //Closing Public Identity Tag
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

    }

    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return oReturnValue;
}

t_Db_HSSReturnVal CxDx_AddIFCTags(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                  xmlTextWriterPtr pWriter,
                                  unsigned int     unServiceProfile_ID)
{
	DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;
    unsigned char        ucQueryStr[CX_QUERY_SIZE];

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_IFC].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unServiceProfile_ID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }


    int nRows = oDBEngineResult.ulNumberOfRows;
    int nIndex;
    unsigned char ucBuffer[70];
    t_Db_HSSReturnVal oReturnValue = Db_HSS_Retval_Success;

    for(nIndex = 0; nIndex < nRows; nIndex++)
    {

        nResultCode = xmlTextWriterStartElement(pWriter, TAG_IFC);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].pchField,
                                      oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_PRIORITY, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField);
            CxDx_AddTriggerPointTag(hdbHSSThreadCtxt_io, pWriter, atoi(ucBuffer));
        }

        nResultCode = xmlTextWriterStartElement(pWriter, TAG_APPLICATION_SERVER);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].pchField,
                                      oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_SERVER_NAME, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].pchField,
                                          oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_DEFAULT_HANDLING, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }

        //Closing Application Server Tag
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        //Closing InitialFilterCriteria Tag
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

    }

    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return oReturnValue;



}

t_Db_HSSReturnVal CxDx_AddTriggerPointTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                          xmlTextWriterPtr pWriter,
                                          unsigned int     unTriggerPoint_ID)
{
	DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;
    unsigned char        ucBuffer[70];
    unsigned char        ucQueryStr[CX_QUERY_SIZE];

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_TRIGERPOINT].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unTriggerPoint_ID);
    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }

    nResultCode = xmlTextWriterStartElement(pWriter, TAG_TRIGGER_POINT);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
        return Db_HSS_Retval_Failure;
    }

    DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
                                  oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

    nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_CONDITION_TYPE_CNF, "%s", ucBuffer);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
        return Db_HSS_Retval_Failure;
    }

    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    //Add SPTs

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_SPT].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unTriggerPoint_ID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }

    int nRows = oDBEngineResult.ulNumberOfRows;
    t_Db_HSSReturnVal oReturnValue;
    int nIndex;

    for(nIndex = 0; nIndex < nRows; nIndex++)
    {
        nResultCode = xmlTextWriterStartElement(pWriter, TAG_SPT);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].pchField,
                                          oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_CONDITION_NEGATED, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].pchField,
                                      oDBEngineResult.ppoAMPSDBEngineTable[nIndex][1].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_GROUP, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][2].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_REQUEST_URI, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }
        else if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][3].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_METHOD, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }
        else if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][4].nLengthOfField &&
                0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][5].nLengthOfField  )
        {
            nResultCode = xmlTextWriterStartElement(pWriter, TAG_SIP_HEADER);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                return Db_HSS_Retval_Failure;
            }

            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][4].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][4].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_HEADER, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }

            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][5].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][5].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_CONTENT, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }

            nResultCode = xmlTextWriterEndElement(pWriter);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }
        else if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][6].nLengthOfField )
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][6].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][6].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_SESSION_CASE, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }
        else if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][7].nLengthOfField)
        {
            nResultCode = xmlTextWriterStartElement(pWriter, TAG_SESSION_DESCRIPTION);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                return Db_HSS_Retval_Failure;
            }

            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][7].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][7].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_LINE, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }

            if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][8].nLengthOfField)
            {
                DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][8].pchField,
                                  oDBEngineResult.ppoAMPSDBEngineTable[nIndex][8].nLengthOfField);

                nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_CONTENT, "%s", ucBuffer);
                if (nResultCode < 0)
                {
                    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                    oReturnValue = Db_HSS_Retval_Failure;
                    break;
                }
            }

            nResultCode = xmlTextWriterEndElement(pWriter);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }

        if(0 < oDBEngineResult.ppoAMPSDBEngineTable[nIndex][9].nLengthOfField)
        {
            nResultCode = xmlTextWriterStartElement(pWriter, TAG_EXTENSION);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                return Db_HSS_Retval_Failure;
            }

            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][9].pchField,
                              oDBEngineResult.ppoAMPSDBEngineTable[nIndex][9].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_REGISTRATION_TYPE, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }

            nResultCode = xmlTextWriterEndElement(pWriter);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
                oReturnValue = Db_HSS_Retval_Failure;
                break;
            }
        }

        //Closing SPT
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            oReturnValue = Db_HSS_Retval_Failure;
            break;
        }
    }

    //Closing TriggerPoint Tag
    nResultCode = xmlTextWriterEndElement(pWriter);
    if (nResultCode < 0)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
        oReturnValue = Db_HSS_Retval_Failure;
    }

    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return oReturnValue;
}


t_Db_HSSReturnVal CxDx_AddCoreNetworkServiceAuthTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                                    xmlTextWriterPtr pWriter,
                                                    unsigned int     unCNSA_ID)
{
	DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;
    unsigned char        ucBuffer[70];
    unsigned char        ucQueryStr[CX_QUERY_SIZE];

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_GET_MEDIAPROFILEID_BY_CNSA_ID].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unCNSA_ID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }

    if(0 < oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField)
    {
        nResultCode = xmlTextWriterStartElement(pWriter, TAG_CORE_NETWORK_SERVICE_AUTH);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            return Db_HSS_Retval_Failure;
        }

        DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
                                      oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

        nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_SUBSCRIBED_MEDIA_PRO_ID, "%s", ucBuffer);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            return Db_HSS_Retval_Failure;
        }


        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            return Db_HSS_Retval_Failure;
        }
    }


    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return Db_HSS_Retval_Success;
}

t_Db_HSSReturnVal CxDx_AddExtensionTag(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
                                       xmlTextWriterPtr pWriter,
                                       unsigned int     unExt_ID)
{
	DMBASE_DBE_HANDLE   hDBEngineHandle = hdbHSSThreadCtxt_io->hDBEngineHandle;
    HDBHSSCONTEXT hDBHSSContext= hdbHSSThreadCtxt_io->hDBHSSMainContext;

    t_AMPSDBEngineResult oDBEngineResult ;
    unsigned char        ucDBQuery[CX_QUERY_SIZE];
    unsigned char        ucQueryStr[CX_QUERY_SIZE];
    unsigned int         unQueryLength;
    int                  nResultCode;

    DMBase_CopyDiamStrToCharArray(ucQueryStr, hDBHSSContext->oDBQueryMapForCx[CX_Get_SHAREDIFCSET].oQueryString);
    sprintf(ucDBQuery, ucQueryStr, unExt_ID);

    unQueryLength = strlen(ucDBQuery);

    if(DMBase_Retval_Success != DMBASE_DBEngineQuery(hDBHSSContext->hDiamStackContext,
                                                          hDBEngineHandle,
                                                          ucDBQuery,
                                                          unQueryLength,
                                                          &oDBEngineResult))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Database Query failed , Leaving\n");
        return Db_HSS_Retval_QueryFailed;
    }

    int nRows = oDBEngineResult.ulNumberOfRows;
    int nIndex;
    unsigned char ucBuffer[20];

    if(0 < nRows)
    {
        nResultCode = xmlTextWriterStartElement(pWriter, TAG_EXTENSION);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
            return Db_HSS_Retval_Failure;
        }

        for(nIndex = 0; nIndex < nRows; nIndex++)
        {
            DMBase_CopyNChar(ucBuffer, oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].pchField,
                                          oDBEngineResult.ppoAMPSDBEngineTable[nIndex][0].nLengthOfField);

            nResultCode = xmlTextWriterWriteFormatElement(pWriter,TAG_SHARED_IFC_SET_ID, "%s", ucBuffer);
            if (nResultCode < 0)
            {
                DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in creating element \n");
                return Db_HSS_Retval_Failure;
            }

        }
        nResultCode = xmlTextWriterEndElement(pWriter);
        if (nResultCode < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Error in closing element \n");
            return Db_HSS_Retval_Failure;
        }
    }


    //Now free results

    DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext, hDBEngineHandle ,&oDBEngineResult );

    return Db_HSS_Retval_Success;
}


