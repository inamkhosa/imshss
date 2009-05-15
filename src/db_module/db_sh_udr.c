#include "db_sh_udr.h"
#include "sh_app.h"
#include "dbmodule.h"
#include "db_cmd_id.h"
#include "db_sh_querycodes.h"
#include "sh_diamretcodes.h"
#include "dmbase_data_types.h"
#include "dmbase_ampswrappers.h"
#include "hss.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>


//externs
extern	int                     g_nDiamBaseTraceID ;



t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_AliasRepoData(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}





	//the UserIdentity can be only PUI

	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal =   Db_HSS_Retval_NotFound;
        goto ExitMe;
    }



	if (SH_IDENTITY_TYPE_DISTINCT_PUI != unIdentityType)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d , operation not allowed\n",unIdentityType);
        oRetVal = Db_HSS_Retval_OperationNotAllowed;
        goto ExitMe;
	}



	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_UDR_ALIASES_REPO_DATA_BY_PUI].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->pucServiceIndication,
			'\"','\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');





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
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	
	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Extension node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child AliasesRepositoryData node of Extension
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "AliasesRepositoryData");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;
	if (DMBase_Retval_Success !=  DMBase_Malloc(hDBHSSContext->hDiamStackContext,
								&pucTempBuff,
						oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	//write the ServiceData child node of AliasesRepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ServiceData",
										 "%s", pucTempBuff);

	DMBase_Free(hDBHSSContext->hDiamStackContext,&pucTempBuff);
	

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	pucTempBuff = NULL;



	if (DMBase_Retval_Success !=  DMBase_Malloc(hDBHSSContext->hDiamStackContext,
								&pucTempBuff,
						oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

	//write the Sequence Number child node of AliasesRepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SequenceNumber",
										 "%s", pucTempBuff);

	DMBase_Free(hDBHSSContext->hDiamStackContext,&pucTempBuff);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

	//write the ServiceIndication child node of AliasesRepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ServiceIndication",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);


	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"Alias repo data before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);


    ExitMe:


    if (NULL != oXMLbuf)
	{
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return oRetVal;

}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_DSAI(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;
	unsigned int    		unNumOfDSAIs = 0;
    unsigned int    		unCurrDSAI=0;

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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}






	//the UserIdentity can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal = Db_HSS_Retval_NotFound;
        goto ExitMe;
    }

	

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_DSAI_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_DSAI_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType);
        oRetVal =  Db_HSS_Retval_IncorrectParameters;
        goto ExitMe;
	}




	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[unDBShQueryMapIndex].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->pucDSAI_Tag,
			'\"','\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');





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
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
	    oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;
    unNumOfDSAIs = ulNumberOfResults;
    unCurrDSAI=0;


	

	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Sh-IMS-Data node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Extension node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Extension node of Extension
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    for (unCurrDSAI=0 ; unCurrDSAI< unNumOfDSAIs ; unCurrDSAI++)
    {
       	//write the child DSAI node of Extension
    	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "DSAI");
    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

    	//make a temp string from the non-null terminated db results
    	unsigned char*	pucTempBuff = NULL;

    	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][0].nLengthOfField+1);

    	memcpy(pucTempBuff,
    		   oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][0].pchField,
    		   oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][0].nLengthOfField);

    	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][0].nLengthOfField] = '\0';

    	//write the DSAI-Value child node of DSAI

    	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "DSAI-Value",
    										 "%s", pucTempBuff);

    	free (pucTempBuff);
    	pucTempBuff = NULL;

    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

    	//make a temp string from the non-null terminated db results
    	pucTempBuff = NULL;

    	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][1].nLengthOfField+1);

    	memcpy(pucTempBuff,
    		   oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][1].pchField,
    		   oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][1].nLengthOfField);

    	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[unCurrDSAI][1].nLengthOfField] = '\0';

    	//write the DSAI-Tag child node of DSAI

    	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "DSAI-Tag",
    										 "%s", pucTempBuff);

    	free (pucTempBuff);
    	pucTempBuff = NULL;

    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

		//close the DSAI Tag
        nrc = xmlTextWriterEndElement(oXMLwriter);

		if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}


    }//end for on number of DSAIS returned

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);




	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"DSAI before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

    ExitMe:

    if (NULL != oXMLbuf)
	{
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }



	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);



	return oRetVal;

}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_PSI_Activation(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;



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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}




	//the above can only be PSI

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal =  Db_HSS_Retval_NotFound;
        goto ExitMe;
    }



	if (SH_IDENTITY_TYPE_DISTINCT_PSI != unIdentityType)

	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d , operation not allowed\n",unIdentityType);
        oRetVal = Db_HSS_Retval_OperationNotAllowed;
        goto ExitMe;
	}






	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_UDR_PSI_ACTIV_BY_PSI].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');



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
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	
	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Sh-IMS-Data node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child Extension node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	//write the PSIActivation child node of Extension

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "PSIActivation",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);



	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"PSI Activation before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

    ExitMe:

    if (NULL != oXMLbuf)
    {
    	xmlBufferFree(oXMLbuf);
    	oXMLbuf = NULL;
    }




	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);




	return oRetVal;

}

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_ChargingInfo(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}




			//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal = Db_HSS_Retval_NotFound;
        goto ExitMe;
    }

	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_CHARGING_INFO_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_GET_CHARGING_INFO_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType);
        oRetVal = Db_HSS_Retval_IncorrectParameters;
        goto ExitMe;
	}



	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[unDBShQueryMapIndex].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');




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
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	
	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the ChargingInformation child node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "ChargingInformation");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][4].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][4].nLengthOfField] = '\0';

	//write the PrimaryEventChargingFunctionName child node of Charging Info

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "PrimaryEventChargingFunctionName",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//priority

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][5].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][5].nLengthOfField] = '\0';

	//write the SecondaryEventChargingFunctionName of ChargingInformation

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SecondaryEventChargingFunctionName",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//Server Name
	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

	//write the PrimaryChargingCollectionFunctionName Child node of ChargingInfo

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "PrimaryChargingCollectionFunctionName",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//Server Name
	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';

	//write the SecondaryChargingCollectionFunctionName Child node of ChargingInfo

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SecondaryChargingCollectionFunctionName",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);



	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"Charging info before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

    ExitMe:

    if (NULL != oXMLbuf )
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }




	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);




	return oRetVal;

}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IFC(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;
	unsigned int    		unNumOfIFCs = 0;
    unsigned int    		unCurrIFC = 0;



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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}



			//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal = Db_HSS_Retval_NotFound;
        goto ExitMe;
    }

	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_IFC_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_IFC_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType);
        oRetVal = Db_HSS_Retval_IncorrectParameters;
        goto ExitMe;
	}



	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_UDR_IFC_BY_PUI].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
			'\"', '\"',pRcvdEvDataFromSh->pucServerName,'\"');


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
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;
    unNumOfIFCs = ulNumberOfResults;
    unCurrIFC = 0;


	

	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the child node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the IFCs child node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "IFCs");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    for (unCurrIFC=0 ; unCurrIFC< unNumOfIFCs ; unCurrIFC++)
    {
        //write the IFC child node of IFCs
    	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "IFC");
    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

    	//make a temp string from the non-null terminated db results
    	unsigned char*	pucTempBuff = NULL;

    	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

    	memcpy(pucTempBuff,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

    	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

    	//write the TriggerPoint child node of IFC

    	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "TriggerPoint",
    										 "%s", pucTempBuff);

    	free (pucTempBuff);
    	pucTempBuff = NULL;

    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

    	//priority

    	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1);

    	memcpy(pucTempBuff,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

    	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

    	//write the Priority child node of IFC

    	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "Priority",
    										 "%s", pucTempBuff);

    	free (pucTempBuff);
    	pucTempBuff = NULL;

    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}

    	//Server Name
    	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

    	memcpy(pucTempBuff,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
    		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

    	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    	//write the ApplicationServer Child node of IFC
    	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "ApplicationServer");
    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}
    	//write the Server Name node of Aplication Server

    	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ServerName",
    										 "%s", pucTempBuff);

    	free (pucTempBuff);
    	pucTempBuff = NULL;

    	if (nrc < 0)
    	{
    		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
    		oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
    	}


		//end ApplicationServer element
        nrc =  xmlTextWriterEndElement(oXMLwriter);

        if (nrc < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
            oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
        }


       //end IFC element
        nrc =  xmlTextWriterEndElement(oXMLwriter);

        if (nrc < 0)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
            oRetVal = Db_HSS_Retval_Failure;
            goto ExitMe;
        }

    }//end for on Number of IFcs



	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);

	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"IFCs before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

    ExitMe:

    if (NULL != oXMLbuf)
    {
    	xmlBufferFree(oXMLbuf);
    	oXMLbuf = NULL;
    }



	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);




	return oRetVal;

}

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_SCSCF_Name(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetval = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}



		//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetval = Db_HSS_Retval_NotFound;
        goto ExitMe;
    }

	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_SCSCFNAME_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_SCSCFNAME_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType);
        oRetval = Db_HSS_Retval_IncorrectParameters;
        goto ExitMe;
	}






	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[unDBShQueryMapIndex].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');





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
		oRetval =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	
	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
        goto ExitMe;
    }

	//write the child node of Sh-IMS-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	//write the ServiceData child node of Sh-Data

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SCSCFName",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);

    if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetval = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


   	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"SCSCF Name Before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

    ExitMe:


   if (NULL != oXMLbuf)
   {
       xmlBufferFree(oXMLbuf);
       oXMLbuf = NULL;
   }



	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);



	return oRetval;

}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_UserState(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
    t_Db_HSSReturnVal       oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}





		//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal = Db_HSS_Retval_NotFound;
        goto ExitMe;
    }


	if (SH_IDENTITY_TYPE_DISTINCT_PUI != unIdentityType)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d , operation not allowed\n",unIdentityType);
        oRetVal = Db_HSS_Retval_OperationNotAllowed;
        goto ExitMe;
	}



	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_UDR_IMS_USER_STATE_BY_PUI].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');




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
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," IMS User State Before XML Buffer Create");


	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal  =  Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-IMS-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);



    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"IMS User State Before memcpy ");


	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	//write the ServiceData child node of Sh-Data

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "IMSUserState",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);

	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
        goto ExitMe;
	}


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," IMS User State Before final memcpy");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);



    ExitMe:

    if (NULL != oXMLbuf)
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }


   	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"IMS User State Before DB Engine Free ");



	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);




	return oRetVal;
}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_RegistIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i)
{

	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;
	unsigned char*			pucPubIdInputFinal = NULL;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}







	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}


	if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	{
		//get PUI corresponding to this PSI
		//fill in the place holders inside the query
		//

		bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

		snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PSI_ID].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');

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
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (1 != oDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}


		nloopcounterRows = 0;
		nloopcounterColumns = 0;
		ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
		ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
		ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

		pucPubIdInputFinal= NULL;

		pucPubIdInputFinal = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucPubIdInputFinal,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucPubIdInputFinal[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
								   hdbHSSThreadContext_io->hDBEngineHandle,
								   &oDBEngineResult);



	}//end if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	else
	{
		pucPubIdInputFinal = strdup(pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity);
		if (NULL == pucPubIdInputFinal)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strdup failed , leaving\n");
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}
	}




	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PRIV_ID_FOR_PUB_ID].oQueryString.pucOctetString,
			'\"',pucPubIdInputFinal,'\"');

	free(pucPubIdInputFinal);
	pucPubIdInputFinal = NULL;



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

	if (1 != oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	unsigned char* pucPrivID = NULL;

	pucPrivID = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucPrivID,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucPrivID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);

	//make another query to get all the public Identities corresponding to this private
	//identity

	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PRIV_ID].oQueryString.pucOctetString,
			'\"',pucPrivID,'\"');

	free(pucPrivID);
	pucPrivID = NULL;



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
		oRetval =  Db_HSS_Retval_Failure;
	}

	if (0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	
	oXMLbuf = xmlBufferCreate();

	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child Extension node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child AllIdentities node of Extension
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "RegisteredIdentities");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	unsigned int	unPubIdCnt = 0;

	for (unPubIdCnt=0; unPubIdCnt < ulNumberOfResults; unPubIdCnt++)
	{
		//for each public ID ,write its XML node to the XML buffer

		unsigned char * pucTempPubId = NULL;
		pucTempPubId = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField+1);

		memcpy(pucTempPubId,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField);

		pucTempPubId[oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField] = '\0';


		DMBASE_DBEngineResult	oInnerDBEngineResult = {0};
		unsigned char			pucInnerDB_ShQuery[SH_QUERY_SIZE] = {0};

		//fill in the place holders inside the query
		snprintf(pucInnerDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_REG_STATUS_FOR_PUI].oQueryString.pucOctetString,
				'\"',pucTempPubId,'\"');


		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING INNER QUERY \n\n\t%s\n\n",pucInnerDB_ShQuery);
		#endif



		//actually run the query on the DBMS , get the results and build the User Result TLV Record
		if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
												hDBHSSContext->hDiamStackContext,
												hdbHSSThreadContext_io->hDBEngineHandle,
												pucInnerDB_ShQuery,
												strlen(pucInnerDB_ShQuery),
												&oInnerDBEngineResult)
			)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (0 == oInnerDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oInnerDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}




		unsigned char* pucTempRegStatBuff = NULL;

		pucTempRegStatBuff = (unsigned char*) malloc( oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucTempRegStatBuff,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucTempRegStatBuff[oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oInnerDBEngineResult);

		bzero(&oInnerDBEngineResult,sizeof(oInnerDBEngineResult));

		unsigned int unRegStatus = atoi ( pucTempRegStatBuff);

		free (pucTempRegStatBuff);
		pucTempRegStatBuff = NULL;

		/*****************************************/

		//fill in the place holders inside the query
		snprintf(pucInnerDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_BARR_INDIC_FOR_PUI].oQueryString.pucOctetString,
				'\"',pucTempPubId,'\"');


		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING INNER QUERY \n\n\t%s\n\n",pucInnerDB_ShQuery);
		#endif


		//actually run the query on the DBMS , get the results and build the User Result TLV Record
		if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
												hDBHSSContext->hDiamStackContext,
												hdbHSSThreadContext_io->hDBEngineHandle,
												pucInnerDB_ShQuery,
												strlen(pucInnerDB_ShQuery),
												&oInnerDBEngineResult)
			)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (0 == oInnerDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oInnerDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}




		unsigned char* pucTempBarrBuff = NULL;

		pucTempBarrBuff = (unsigned char*) malloc( oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucTempBarrBuff,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucTempBarrBuff[oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oInnerDBEngineResult);

		bzero(&oInnerDBEngineResult,sizeof(oInnerDBEngineResult));

		unsigned int unBarIndic = atoi ( pucTempBarrBuff);

		free (pucTempBarrBuff);
		pucTempBarrBuff = NULL;


		/******************************************/

		if ( (SH_IMSUSER_STATE_REGISTERED == unRegStatus) &&
			  (0 == unBarIndic)
		   )
		{
			//this PUI is registered and non barred, insert its node into XML Buffer

			//write the child IMSPublicIdentity node of RegisteredIdentities node

			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "IMSPublicIdentity",
										 "%s", pucTempPubId);
			if (nrc < 0)
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
				oRetval =  Db_HSS_Retval_Failure;
				goto ExitMe;
			}

			

		}//end if (SH_IMSUSER_STATE_REGISTERED == unRegStatus) && not barred

        free(pucTempPubId);
        pucTempPubId = NULL;


	}//end for

	//close the RegisteredIdentities tag
	nrc =  xmlTextWriterEndElement(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		return Db_HSS_Retval_Failure;
	}

	xmlFreeTextWriter(oXMLwriter);



	if (oXMLbuf->use >= SH_MAX_PUR_USER_DATA_STRING_SIZE)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided \n");
		return Db_HSS_Retval_Failure;
	}

	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		return Db_HSS_Retval_Failure;
	}


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  IMS Public IDs Regist IDs Before final memcpy ");

	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);



	ExitMe:

    if (NULL != oXMLbuf)
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return oRetval;


}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_ImplicitIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i)
{

	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;
	unsigned char*			pucPubIdInputFinal = NULL;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}





	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}

	if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	{
		//get PUI corresponding to this PSI
		//fill in the place holders inside the query
		//

		bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

		snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PSI_ID].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');

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
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (1 != oDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}


		nloopcounterRows = 0;
		nloopcounterColumns = 0;
		ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
		ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
		ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

		pucPubIdInputFinal= NULL;

		pucPubIdInputFinal = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucPubIdInputFinal,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucPubIdInputFinal[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
								   hdbHSSThreadContext_io->hDBEngineHandle,
								   &oDBEngineResult);



	}//end if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	else
	{
		pucPubIdInputFinal = strdup(pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity);
		if (NULL == pucPubIdInputFinal)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strdup failed , leaving\n");
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}
	}




	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PRIV_ID_FOR_PUB_ID].oQueryString.pucOctetString,
			'\"',pucPubIdInputFinal,'\"');

	free(pucPubIdInputFinal);
	pucPubIdInputFinal = NULL;





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

	if (1 != oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	unsigned char* pucPrivID = NULL;

	pucPrivID = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucPrivID,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucPrivID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);

	//make another query to get all the public Identities corresponding to this private
	//identity

	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PRIV_ID].oQueryString.pucOctetString,
			'\"',pucPrivID,'\"');

	free(pucPrivID);
	pucPrivID = NULL;



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
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if (0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	
	oXMLbuf = xmlBufferCreate();

	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child Extension node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child AllIdentities node of Extension
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "ImplicitIdentities");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	unsigned int	unPubIdCnt = 0;

	for (unPubIdCnt=0; unPubIdCnt < ulNumberOfResults; unPubIdCnt++)
	{
		//for each public ID ,write its XML node to the XML buffer

		unsigned char * pucTempPubId = NULL;
		pucTempPubId = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField+1);

		memcpy(pucTempPubId,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField);

		pucTempPubId[oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField] = '\0';


		DMBASE_DBEngineResult	oInnerDBEngineResult = {0};
		unsigned char			pucInnerDB_ShQuery[SH_QUERY_SIZE] = {0};

		//fill in the place holders inside the query
		snprintf(pucInnerDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_BARR_INDIC_FOR_PUI].oQueryString.pucOctetString,
				'\"',pucTempPubId,'\"');



		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING INNER QUERY FOR BARRING ==>  \n\n\t%s\n\n",pucInnerDB_ShQuery);
		#endif



		//actually run the query on the DBMS , get the results and build the User Result TLV Record
		if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
												hDBHSSContext->hDiamStackContext,
												hdbHSSThreadContext_io->hDBEngineHandle,
												pucInnerDB_ShQuery,
												strlen(pucInnerDB_ShQuery),
												&oInnerDBEngineResult)
			)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (0 == oInnerDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oInnerDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}




		unsigned char* pucTempBarrBuff = NULL;

		pucTempBarrBuff = (unsigned char*) malloc( oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucTempBarrBuff,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucTempBarrBuff[oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oInnerDBEngineResult);

		bzero(&oInnerDBEngineResult,sizeof(oInnerDBEngineResult));


		unsigned int unBarIndic = atoi ( pucTempBarrBuff);


		free (pucTempBarrBuff);
		pucTempBarrBuff = NULL;

		if (0 == unBarIndic)
		{
			//this PUI is not barred , insert its node into XML Buffer

			//write the child IMSPublicIdentity node of ImplicitIdentities node

			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "IMSPublicIdentity",
										 "%s", pucTempPubId);
			if (nrc < 0)
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
				oRetval =  Db_HSS_Retval_Failure;
				goto ExitMe;
			}

			

		}//end if (0 == unBarIndic)

        free(pucTempPubId);
        pucTempPubId = NULL;


	}//end for

	//close the ImplicitIdentities tag

	nrc =  xmlTextWriterEndElement(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		return Db_HSS_Retval_Failure;
	}

	xmlFreeTextWriter(oXMLwriter);


	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		return Db_HSS_Retval_Failure;
	}


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"IMS Public Ids Implicit Ids Before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

	ExitMe:

    if (NULL != oXMLbuf)
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return oRetval;


}

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID_AllIDs(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i,
										  unsigned int unIdentityType_i)
{

	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;
	unsigned char*			pucPubIdInputFinal = NULL;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;


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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}





	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	{
		//get PUI corresponding to this PSI
		//fill in the place holders inside the query
		//

		bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

		snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PSI_ID].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');

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
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (1 != oDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}


		nloopcounterRows = 0;
		nloopcounterColumns = 0;
		ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
		ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
		ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

		pucPubIdInputFinal= NULL;

		pucPubIdInputFinal = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucPubIdInputFinal,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucPubIdInputFinal[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
								   hdbHSSThreadContext_io->hDBEngineHandle,
								   &oDBEngineResult);



	}//end if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	else
	{
		pucPubIdInputFinal = strdup(pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity);
		if (NULL == pucPubIdInputFinal)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strdup failed , leaving\n");
			oRetval = Db_HSS_Retval_Failure;
			goto ExitMe;
		}
	}




	//fill in the place holders inside the query

	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PRIV_ID_FOR_PUB_ID].oQueryString.pucOctetString,
			'\"',pucPubIdInputFinal,'\"');


	free(pucPubIdInputFinal);
	pucPubIdInputFinal = NULL;





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
		oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if (1 != oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 1 != oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	unsigned char* pucPrivID = NULL;

	pucPrivID = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucPrivID,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucPrivID[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);

	//make another query to get all the public Identities corresponding to this private
	//identity

	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));

	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PRIV_ID].oQueryString.pucOctetString,
			'\"',pucPrivID,'\"');

	free(pucPrivID);
	pucPrivID = NULL;



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
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if (0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;

	
	oXMLbuf = xmlBufferCreate();

	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child Extension node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Extension");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the child AllIdentities node of Extension
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "AllIdentities");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	unsigned int	unPubIdCnt = 0;

	for (unPubIdCnt=0; unPubIdCnt < ulNumberOfResults; unPubIdCnt++)
	{
		//for each public ID ,write its XML node to the XML buffer

		unsigned char * pucTempPubId = NULL;
		pucTempPubId = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField+1);

		memcpy(pucTempPubId,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField);

		pucTempPubId[oDBEngineResult.ppoAMPSDBEngineTable[unPubIdCnt][0].nLengthOfField] = '\0';


		DMBASE_DBEngineResult	oInnerDBEngineResult = {0};
		unsigned char			pucInnerDB_ShQuery[SH_QUERY_SIZE] = {0};

		//fill in the place holders inside the query
		snprintf(pucInnerDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_BARR_INDIC_FOR_PUI].oQueryString.pucOctetString,
				'\"',pucTempPubId,'\"');


		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING INNER QUERY \n\n\t%s\n\n",pucInnerDB_ShQuery);
		#endif


		//actually run the query on the DBMS , get the results and build the User Result TLV Record
		if (DMBase_Retval_Success !=  DMBASE_DBEngineQuery(
												hDBHSSContext->hDiamStackContext,
												hdbHSSThreadContext_io->hDBEngineHandle,
												pucInnerDB_ShQuery,
												strlen(pucInnerDB_ShQuery),
												&oInnerDBEngineResult)
			)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBASE_DBEngineQuery Failed \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}

		if (0 == oInnerDBEngineResult.ulNumberOfResults )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oInnerDBEngineResult.ulNumberOfResults \n");
			oRetval =  Db_HSS_Retval_Failure;
			goto ExitMe;
		}




		unsigned char* pucTempBarrBuff = NULL;

		pucTempBarrBuff = (unsigned char*) malloc( oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

		memcpy(pucTempBarrBuff,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
			   oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

		pucTempBarrBuff[oInnerDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oInnerDBEngineResult);

		bzero(&oInnerDBEngineResult,sizeof(oInnerDBEngineResult));

		unsigned int unBarIndic = atoi ( pucTempBarrBuff);

		free (pucTempBarrBuff);
		pucTempBarrBuff = NULL;

		if (0 == unBarIndic)
		{
			//this PUI is not barred , insert its node into XML Buffer

			//write the child IMSPublicIdentity node of AllIdentities node

			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "IMSPublicIdentity",
										 "%s", pucTempPubId);
			if (nrc < 0)
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
				oRetval =  Db_HSS_Retval_Failure;
				goto ExitMe;
			}

			

		}//end if (0 == unBarIndic)

        free(pucTempPubId);
        pucTempPubId = NULL;



	}//end for

	//close the AllIdentities tag

	nrc =  xmlTextWriterEndElement(oXMLwriter);
	
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndElement Failed \n");
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		return Db_HSS_Retval_Failure;
	}

	xmlFreeTextWriter(oXMLwriter);


	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		return Db_HSS_Retval_Failure;
	}


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"IMS Public ID All IDS  Before final memcpy ");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

	ExitMe:

    if (NULL != oXMLbuf)
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return oRetval;


}


t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_IMS_PublicID(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	t_Db_HSSReturnVal		oRetval = Db_HSS_Retval_Success;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;

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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}


	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}

	unsigned int	unIdentitySet = pRcvdEvDataFromSh->unIdentitySet;




	//check if the identity set value received in the buffer is valid
	if (!
		(
		 (SH_IDENTITYSET_TYPE_ALL_IDENTITIES == unIdentitySet) ||
		 (SH_IDENTITYSET_TYPE_ALIAS_IDENTITIES == unIdentitySet) ||
		 (SH_IDENTITYSET_TYPE_IMPLICIT_IDENTITIES == unIdentitySet) ||
		 (SH_IDENTITYSET_TYPE_REGISTERED_IDENTITIES == unIdentitySet)
		)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Identity Set Record in the TLV Buffer => %d , is not valid\n",unIdentitySet);
		oRetval =  Db_HSS_Retval_Failure;
		goto ExitMe;
	}



	//the above can be PSI as well as PUI , check its type
	//from DB and handle case accordingly
	//

	unIdentityType = 0;

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetval =  Db_HSS_Retval_NotFound;
		goto ExitMe;
    }

	if ( (SH_IDENTITY_TYPE_WILDCARDED_PUI == unIdentityType) ||
			 (SH_IDENTITY_TYPE_WILCARDED_PSI == unIdentityType)
		   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Wildcarded identities are not supported \n");
		oRetval = Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	switch(unIdentitySet)
	{

	case	SH_IDENTITYSET_TYPE_ALL_IDENTITIES:



			oRetval = db_sh_GiveUserDataForUDR_IMS_PublicID_AllIDs(hdbHSSThreadContext_io,
								pucEvData_i,unEventDataLen_i,pucUserData_o,unLenOfUserData_i,unIdentityType);

		break;

	case	SH_IDENTITYSET_TYPE_REGISTERED_IDENTITIES:

		oRetval = db_sh_GiveUserDataForUDR_IMS_PublicID_RegistIDs(hdbHSSThreadContext_io,
							pucEvData_i,unEventDataLen_i,pucUserData_o,unLenOfUserData_i,unIdentityType);

		break;

	case	SH_IDENTITYSET_TYPE_IMPLICIT_IDENTITIES:

		oRetval = db_sh_GiveUserDataForUDR_IMS_PublicID_ImplicitIDs(hdbHSSThreadContext_io,
				pucEvData_i,unEventDataLen_i,pucUserData_o,unLenOfUserData_i,unIdentityType);

		break;

	case	SH_IDENTITYSET_TYPE_ALIAS_IDENTITIES:

		 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Identity Set SH_IDENTITYSET_TYPE_ALIAS_IDENTITIES not supported \n");
		 oRetval = Db_HSS_Retval_Failure;
		 goto ExitMe;

		break;

	default:

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid value of Identity Set is received   => %d \n",unIdentitySet);
		 oRetval = Db_HSS_Retval_Failure;
		 goto ExitMe;
		break;

	}//end switch

	ExitMe:





	return oRetval;


}

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR_RepoData(
										  HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
{


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	t_Db_HSSReturnVal		oRetVal = Db_HSS_Retval_Success;
	int             		nloopcounterRows = 0;
	int             		nloopcounterColumns = 0;
	unsigned long   		ulNumberOfResults   = 0;
	unsigned long   		ulNumberOfRows      = 0;
	unsigned long   		ulNumberOfColumns   = 0;
	int 					nrc = 0;
	xmlTextWriterPtr		oXMLwriter = {0};
	xmlBufferPtr 			oXMLbuf = {0};
	xmlChar *				potmpXMLChar = NULL;
	unsigned int    		unIdentityType = 0;
	HDBHSSCONTEXT			hDBHSSContext = NULL;
    t_EventDataShToDB*		pRcvdEvDataFromSh = NULL;
	unsigned int			unDBShQueryMapIndex=0;

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

	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}




	//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//

	unIdentityType = 0;
	hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
													pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
													&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        oRetVal = Db_HSS_Retval_NotFound;
		goto ExitMe;
    }

	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_REPO_DATA_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_UDR_GET_REPO_DATA_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType);
        oRetVal = Db_HSS_Retval_IncorrectParameters;
		goto ExitMe;
	}


	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[unDBShQueryMapIndex].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->pucServiceIndication,'\"','\"',
			pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');





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
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	if ( 0 == oDBEngineResult.ulNumberOfResults )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	nloopcounterRows = 0;
	nloopcounterColumns = 0;
	ulNumberOfResults   = oDBEngineResult.ulNumberOfResults;
	ulNumberOfRows      = oDBEngineResult.ulNumberOfRows;
	ulNumberOfColumns   = oDBEngineResult.ulNumberOfColumns;


	
	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the root node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "Sh-Data");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//write the RepositoryData child node of Sh-Data
	nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "RepositoryData");
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	unsigned char*	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';

	//write the ServiceData child node of RepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ServiceData",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';

	//write the Sequence Number child node of RepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SequenceNumber",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	//make a temp string from the non-null terminated db results
	pucTempBuff = NULL;

	pucTempBuff = (unsigned char*) malloc(	oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

	memcpy(pucTempBuff,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
		   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

	pucTempBuff[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

	//write the ServiceIndication child node of RepositoryData

	nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ServiceIndication",
										 "%s", pucTempBuff);

	free (pucTempBuff);
	pucTempBuff = NULL;

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	nrc = xmlTextWriterEndDocument(oXMLwriter);

	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed \n");
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}

	xmlFreeTextWriter(oXMLwriter);



	if (oXMLbuf->use >= unLenOfUserData_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Insufficient buffer provided oXMLbuf->use == %d AND  unLenOfUserData_i == %d\n",oXMLbuf->use, unLenOfUserData_i);
		oRetVal = Db_HSS_Retval_Failure;
		goto ExitMe;
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," IMS Repo Data  Before final memcpy");


	memcpy(pucUserData_o,
		   oXMLbuf->content,
		   oXMLbuf->use);


	ExitMe:


    if (NULL != oXMLbuf)
    {
        xmlBufferFree(oXMLbuf);
        oXMLbuf = NULL;
    }


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);

	return oRetVal;



}

t_Db_HSSReturnVal				db_sh_GiveUserDataForUDR(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										  unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned char* pucUserData_o,
										unsigned unLenOfUserData_i)
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
	if (NULL == pucUserData_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL == pucUserData_o\n");
        return Db_HSS_Retval_NullPointer;
	}

	HDBHSSCONTEXT		hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
	t_EventDataShToDB*	pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}


    if (NULL == hdbHSSThreadContext_io->hDBEngineHandle)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hdbHSSThreadContext_io->hDBEngineHandle is NULL , Leaving\n");
        return Db_HSS_Retval_Failure;
	}





    unsigned int    unIdentityType = 0;

    if ( DappCommon_Retval_Success !=   db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
								pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,&unIdentityType) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        return Db_HSS_Retval_NotFound;
    }


    //check Application Server permissions list

    t_db_sh_AppSrvPermissions   oAppSrvPerms = {0};

    if ( DappCommon_Retval_Success !=  db_sh_GetAppServPermListFromDB(hdbHSSThreadContext_io,
									pRcvdEvDataFromSh->pucOriginHost,
										pRcvdEvDataFromSh->unDataReference,&oAppSrvPerms))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," db_sh_GetAppServPermListFromDB FAILED \n");
        return Db_HSS_Retval_Failure;
    }


    if ( 1 != oAppSrvPerms.bUDR_Allowed)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," UDR Operation is not allowed to this Server for the current Data Ref => %d \n",pRcvdEvDataFromSh->unDataReference);
        return Db_HSS_Retval_PermissionDenied;
    }



	t_Db_HSSReturnVal		oRetVal = Db_HSS_Retval_Failure;



	switch (pRcvdEvDataFromSh->unDataReference)
	{
	case	SH_DATA_REF_AVP_VAL_REPOSITORYDATA:

			oRetVal = db_sh_GiveUserDataForUDR_RepoData(hdbHSSThreadContext_io,pucEvData_i,
														unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

			break;

	case 	SH_DATA_REF_AVP_VAL_IMSPUBLICIDENTITY:


			oRetVal = db_sh_GiveUserDataForUDR_IMS_PublicID(hdbHSSThreadContext_io,pucEvData_i,
														unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

		break;


	case	SH_DATA_REF_AVP_VAL_IMSUSERSTATE:

			oRetVal = db_sh_GiveUserDataForUDR_IMS_UserState(hdbHSSThreadContext_io,pucEvData_i,
															 unEventDataLen_i,pucUserData_o,unLenOfUserData_i);
		break;



	case	SH_DATA_REF_AVP_VAL_S_CSCFNAME:

			oRetVal = db_sh_GiveUserDataForUDR_SCSCF_Name(hdbHSSThreadContext_io,pucEvData_i,
														  unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

		break;

	case	SH_DATA_REF_AVP_VAL_INITIALFILTERCRITERIA:

			oRetVal = db_sh_GiveUserDataForUDR_IFC(hdbHSSThreadContext_io,pucEvData_i,
												   unEventDataLen_i,pucUserData_o,unLenOfUserData_i);


		break;

	case	SH_DATA_REF_AVP_VAL_LOCATIONINFORMATION:


			//not supported
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DataReference SH_DATA_REF_AVP_VAL_LOCATIONINFORMATION is not supported ! \n");
			oRetVal = Db_HSS_Retval_NotSupported;

		break;


	case	SH_DATA_REF_AVP_VAL_USERSTATE:

		//not supported
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DataReference SH_DATA_REF_AVP_VAL_USERSTATE is not supported ! \n");
		oRetVal = Db_HSS_Retval_NotSupported;


		break;

	case	SH_DATA_REF_AVP_VAL_CHARGINGINFORMATION:

		oRetVal = db_sh_GiveUserDataForUDR_ChargingInfo(hdbHSSThreadContext_io,pucEvData_i,
														unEventDataLen_i,pucUserData_o,unLenOfUserData_i);


		break;

	case	SH_DATA_REF_AVP_VAL_MSISDN:

			//not supported
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DataReference SH_DATA_REF_AVP_VAL_MSISDN is not supported ! \n");
			oRetVal = Db_HSS_Retval_NotSupported;

			break;



	case	SH_DATA_REF_AVP_VAL_PSIACTIVATION:

		oRetVal = db_sh_GiveUserDataForUDR_PSI_Activation(hdbHSSThreadContext_io,pucEvData_i,
														  unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

		break;

	case	SH_DATA_REF_AVP_VAL_DSAI:

		oRetVal = db_sh_GiveUserDataForUDR_DSAI(hdbHSSThreadContext_io,pucEvData_i,
												unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

		break;


	case	SH_DATA_REF_AVP_VAL_ALIASESREPOSITORYDATA:

		oRetVal = db_sh_GiveUserDataForUDR_AliasRepoData(hdbHSSThreadContext_io,pucEvData_i,
														 unEventDataLen_i,pucUserData_o,unLenOfUserData_i);

		break;

	default:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Invalid Value of DataReference Record in TLV Buffer\n");
        oRetVal =  Db_HSS_Retval_Failure;
		break;

	}//end switch


	return oRetVal;
}

t_Db_HSSReturnVal				db_sh_ProcessUDRData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io ,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

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


	unsigned int		unDiamRetCode = 0;
	HDBHSSCONTEXT		hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
	unsigned int		unVendIdFromSh = 0;
	unsigned int		unAuthAppIDFromSh = 0;
	unsigned int		unAuthSessStateFromSh = 0;
	t_EventDataDBToSh	oEvtDataDBToSh = {0};
	t_EventDataShToDB*	pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;
	t_DiamOctetString	oTempUsrData = {0};
    t_Db_HSSReturnVal   oRetVal = Db_HSS_Retval_Failure;
	oEvtDataDBToSh.unEvtCommandID = DB_SH_CMD_CODE_UDR;

	oEvtDataDBToSh.oVendorSpecificAppID = pRcvdEvDataFromSh->oVendorSpecificAppID;
	oEvtDataDBToSh.unAuthSessionState = pRcvdEvDataFromSh->unAuthSessionState;

	strncpy(oEvtDataDBToSh.pucSessionID,pRcvdEvDataFromSh->pucSessionID,
			SH_MAX_SMALL_STRING_SIZE);


	//build the User Data
    oRetVal = db_sh_GiveUserDataForUDR(hdbHSSThreadContext_io,pucEvData_i,unEventDataLen_i,
							 oEvtDataDBToSh.pucUserData, SH_MAX_PUR_USER_DATA_STRING_SIZE);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  Returned from db_sh_GiveUserDataForUDR \n");



	if ( Db_HSS_Retval_Success ==  oRetVal)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  Db_HSS_Retval_Success Returned from db_sh_GiveUserDataForUDR \n");
		oEvtDataDBToSh.unRetCodeForShResp = DIAMETER_SUCCESS;
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_Result;
    }
    else if ( Db_HSS_Retval_NotFound ==  oRetVal)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  Db_HSS_Retval_NotFound Returned from db_sh_GiveUserDataForUDR \n");
		oEvtDataDBToSh.unRetCodeForShResp = SH_DIAMETER_ERROR_USER_UNKNOWN;
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
    }
    else if ( Db_HSS_Retval_PermissionDenied ==  oRetVal)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  Db_HSS_Retval_PermissionDenied Returned from db_sh_GiveUserDataForUDR \n");
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_CANNOT_BE_READ;
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
    }
    else if ( Db_HSS_Retval_OperationNotAllowed ==  oRetVal)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  Db_HSS_Retval_OperationNotAllowed Returned from db_sh_GiveUserDataForUDR \n");
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_OPERATION_NOT_ALLOWED;
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
    }
    else
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG,"  sOME Error code => %d Returned from db_sh_GiveUserDataForUDR \n",oRetVal);
		//return unable to comply error code
		oEvtDataDBToSh.unRetCodeForShResp = DIAMETER_UNABLE_TO_COMPLY;
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_Result;
	}



	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadContext_io->hUnitIOAgent,
					DB_RESPONSE_EVENT_FOR_SH_APP, &oEvtDataDBToSh, sizeof(oEvtDataDBToSh) )
		)
	{

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsSendEvt FAILED for DB_RESPONSE_EVENT_FOR_SH_APP \n");

	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," AMPS_IOAgentsSendEvt SUCCEEDED for DB_RESPONSE_EVENT_FOR_SH_APP \n");
    }



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return Db_HSS_Retval_Success;
}
