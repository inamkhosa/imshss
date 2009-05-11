#include "db_sh_pur.h"
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




t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_AliasRepoData
										(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 )
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


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    t_EventDataShToDB*		pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}



	//parse the XML received and get data out of it
	xmlDocPtr 			poXmlDoc = NULL;
	xmlNodePtr 			poCurrXMLNode = NULL;
	xmlNodePtr 			poCurrInnerXMLNode = NULL;
	_Bool				bIsServiceDataPresent = 0;

	poXmlDoc	= xmlParseMemory(pRcvdEvDataFromSh->pucUserData,
								 strlen(pRcvdEvDataFromSh->pucUserData));

	if (NULL == poXmlDoc)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlParseMemory Failed ! \n");
		return Db_HSS_Retval_Failure;
	}

	poCurrXMLNode = xmlDocGetRootElement(poXmlDoc);

	if (NULL == poCurrXMLNode)
	{
		xmlFreeDoc(poXmlDoc);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlDocGetRootElement Failed ! \n");
		return Db_HSS_Retval_Failure;
	}


	poCurrXMLNode = poCurrXMLNode->xmlChildrenNode;

	xmlChar *			poXMLKey = NULL;
	t_DiamOctetString	oTempServiceData = {0};
	t_DiamOctetString	oTempSequenceNumber = {0};
	t_DiamOctetString	oTempServiceIndication = {0};

	while (poCurrXMLNode != NULL)
	{
		if ((!xmlStrcmp(poCurrXMLNode->name, (const xmlChar *)"AliasesRepositoryData")))
		{
			poCurrInnerXMLNode = poCurrXMLNode->xmlChildrenNode;
			while (poCurrInnerXMLNode != NULL)
			{
				if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"ServiceData")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempServiceData,poXMLKey);
					xmlFree(poXMLKey);
					bIsServiceDataPresent = 1;
				}
				else if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"SequenceNumber")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempSequenceNumber,poXMLKey);
					xmlFree(poXMLKey);
				}
				else if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"ServiceIndication")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempServiceIndication,poXMLKey);
					xmlFree(poXMLKey);
				}
				poCurrInnerXMLNode = poCurrInnerXMLNode->next;
			}//end while for children of AliasesRepositoryData

		}//endif

		poCurrXMLNode = poCurrXMLNode->next;
	}//end while



	xmlFreeDoc(poXmlDoc);
	poXMLKey = NULL;
	poXmlDoc = NULL;
	poCurrXMLNode = NULL;
	poCurrInnerXMLNode = NULL;



	if (SH_IDENTITY_TYPE_DISTINCT_PUI != unIdentityType_i)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d, operation not allowed\n",unIdentityType_i);
        return Db_HSS_Retval_OperationNotAllowed;
	}


	unsigned char*	pucServDataFromMsg = NULL;
	unsigned char*	pucServIndicFromMsg = NULL;
	unsigned char*	pucSeqNumFromMsg = NULL;
	unsigned int	unSeqNumFromMsg = 0;


	 pucSeqNumFromMsg = (unsigned char*) malloc(oTempSequenceNumber.unNumOfOctets+1);
	memcpy(pucSeqNumFromMsg,oTempSequenceNumber.pucOctetString,oTempSequenceNumber.unNumOfOctets);
	pucSeqNumFromMsg[oTempSequenceNumber.unNumOfOctets] = '\0';

	unSeqNumFromMsg = atoi(pucSeqNumFromMsg);

	free (pucSeqNumFromMsg);
	pucSeqNumFromMsg = NULL;

	pucServIndicFromMsg = (unsigned char*) malloc(oTempServiceIndication.unNumOfOctets+1);
	memcpy(pucServIndicFromMsg,oTempServiceIndication.pucOctetString,oTempServiceIndication.unNumOfOctets);
	pucServIndicFromMsg[oTempServiceIndication.unNumOfOctets] = '\0';

	if (1 == bIsServiceDataPresent)
	{
		pucServDataFromMsg = (unsigned char*) malloc(oTempServiceData.unNumOfOctets+1);
		memcpy(pucServDataFromMsg,oTempServiceData.pucOctetString,oTempServiceData.unNumOfOctets);
		pucServDataFromMsg[oTempServiceData.unNumOfOctets] = '\0';
	}


	//GET repoID first from the first query





	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_GET_REPOID_BY_PUI_AND_SRV_IND].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,
			'\"','\"',pucServIndicFromMsg,'\"');



	//temp for debug
	#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
		printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
	#endif

	_Bool	bWasDataPresent = 0;



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
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," oDBEngineResult.ulNumberOfResults returned 0 i.e. Repo Data was not present\n");
		bWasDataPresent = 0;
	 }
	 else
	 {
		 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," oDBEngineResult.ulNumberOfResults returned NON 0 i.e. Repo Data Was present \n");
		 bWasDataPresent = 1;
	 }

	 if ( 1 == bWasDataPresent)
	 {
		 //need to handle update and deletion cases
		 unsigned int unRepoID = 0;
		 unRepoID = atoi(oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField);

		 unsigned char* pucServIndicFromDB = NULL;
		 unsigned char*	pucSeqNumFromDB = NULL;
		 unsigned char* pucServDataFromDB = NULL;
		 unsigned int	unSeqNumFromDB = 0;


		 pucServIndicFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1);

		memcpy(pucServIndicFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

		pucServIndicFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';



		pucServDataFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

		memcpy(pucServDataFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

		pucServDataFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';


		pucSeqNumFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField+1);

		memcpy(pucSeqNumFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);

		pucSeqNumFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';


		unSeqNumFromDB = atoi(pucSeqNumFromDB);

		free(pucSeqNumFromDB);
		pucSeqNumFromDB = NULL;



		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
								   hdbHSSThreadContext_io->hDBEngineHandle,
								   &oDBEngineResult);

		if (0 == unSeqNumFromMsg)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: 0 == unSeqNumFromMsg \n");
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}

		if ((unSeqNumFromMsg-1) != (unSeqNumFromDB % 65535) )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: (unSeqNumFromMsg-1) != (unSeqNumFromDB % 65535) \n");
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}

		if ( 1 == bIsServiceDataPresent)
		{
			if (oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS )
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS \n");
				return Db_HSS_Retval_TooMuchData;
			}

			//make and execute the update Query

			bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


			//fill in the place holders inside the query
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
					hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_UPDATE_ALIAS_REPO_DATA].oQueryString.pucOctetString,
					'\"',pucServDataFromMsg,'\"',unSeqNumFromMsg, '\"',pucServIndicFromMsg,'\"',unRepoID);

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

			DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);

		}//end if ServiceData is present
		else
		{
			//have to remove the record
			bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


			//fill in the place holders inside the query
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
					hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_PUR_DEL_REPO_DATA_REC_BY_REPOID].oQueryString.pucOctetString,
					unRepoID);

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

			DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);


		}//end else i.e. case where serviceData is not present

	 }//end if Data was present in dB Already
	 else
	 {
		 //need to handle addition ,i.e. insertion case

		 //remove old DB Engine Results
		 DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									hdbHSSThreadContext_io->hDBEngineHandle,
									&oDBEngineResult);


		if (0 != unSeqNumFromMsg)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: For insertion case, 0 != unSeqNumFromMsg \n");
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}


		if (0 == bIsServiceDataPresent)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: For insertion case, 0 != unSeqNumFromMsg \n");
			return Db_HSS_Retval_OperationNotAllowed;
		}

		if (oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS \n");
			return Db_HSS_Retval_TooMuchData;
		}


		//make the query



		//now actually add data
		bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


		//fill in the place holders inside the query

		snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_PUR_INSERT_REC_INTO_REPO_DATA].oQueryString.pucOctetString,
				'\"',pucServIndicFromMsg,'\"','\"',pucServDataFromMsg,'\"',
				unSeqNumFromMsg,'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');


		//temp for debug
		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
		#endif


		//actually run the query on the DBMS
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

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);




	 }//end else ,i.e.Data was not present in dB

	return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_RepoData
										(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 )
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


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    t_EventDataShToDB*		pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}


	//parse the XML received and get data out of it , TODO !! Use XQuery Later
	xmlDocPtr poXmlDoc = NULL;
	xmlNodePtr poCurrXMLNode = NULL;
	xmlNodePtr poCurrInnerXMLNode = NULL;

	poXmlDoc	= xmlParseMemory(pRcvdEvDataFromSh->pucUserData,
								 strlen(pRcvdEvDataFromSh->pucUserData));

	if (NULL == poXmlDoc)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlParseMemory Failed ! \n");
		return Db_HSS_Retval_Failure;
	}

	poCurrXMLNode = xmlDocGetRootElement(poXmlDoc);

	if (NULL == poCurrXMLNode)
	{
		xmlFreeDoc(poXmlDoc);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlDocGetRootElement Failed ! \n");
		return Db_HSS_Retval_Failure;
	}


	poCurrXMLNode = poCurrXMLNode->xmlChildrenNode;

	xmlChar *			poXMLKey = NULL;
	t_DiamOctetString	oTempServiceData = {0};
	t_DiamOctetString	oTempSequenceNumber = {0};
	t_DiamOctetString	oTempServiceIndication = {0};

	_Bool				bIsServiceDataPresent = 0;

	while (poCurrXMLNode != NULL)
	{
		if ((!xmlStrcmp(poCurrXMLNode->name, (const xmlChar *)"RepositoryData")))
		{
			poCurrInnerXMLNode = poCurrXMLNode->xmlChildrenNode;
			while (poCurrInnerXMLNode != NULL)
			{
				if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"ServiceData")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempServiceData,poXMLKey);
					xmlFree(poXMLKey);
					bIsServiceDataPresent = 1;
				}
				else if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"SequenceNumber")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempSequenceNumber,poXMLKey);
					xmlFree(poXMLKey);
				}
				else if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"ServiceIndication")))
				{
					poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode->xmlChildrenNode, 1);
					DMBase_CopyCharArrayToDiamString( &oTempServiceIndication,poXMLKey);
					xmlFree(poXMLKey);
				}
				poCurrInnerXMLNode = poCurrInnerXMLNode->next;
			}//end while for children of RepositoryData

		}//endif

		poCurrXMLNode = poCurrXMLNode->next;
	}//end while



	xmlFreeDoc(poXmlDoc);
	poXMLKey = NULL;
	poXmlDoc = NULL;
	poCurrXMLNode = NULL;
	poCurrInnerXMLNode = NULL;

	t_DiamOctetString	oUserIdentity = {0};

	unsigned char*	pucServDataFromMsg = NULL;
	unsigned char*	pucServIndicFromMsg = NULL;
	unsigned char*	pucSeqNumFromMsg = NULL;
	unsigned int	unSeqNumFromMsg = 0;


	 pucSeqNumFromMsg = (unsigned char*) malloc(oTempSequenceNumber.unNumOfOctets+1);
	memcpy(pucSeqNumFromMsg,oTempSequenceNumber.pucOctetString,oTempSequenceNumber.unNumOfOctets);
	pucSeqNumFromMsg[oTempSequenceNumber.unNumOfOctets] = '\0';

	unSeqNumFromMsg = atoi(pucSeqNumFromMsg);

	free (pucSeqNumFromMsg);
	pucSeqNumFromMsg = NULL;

	pucServIndicFromMsg = (unsigned char*) malloc(oTempServiceIndication.unNumOfOctets+1);
	memcpy(pucServIndicFromMsg,oTempServiceIndication.pucOctetString,oTempServiceIndication.unNumOfOctets);
	pucServIndicFromMsg[oTempServiceIndication.unNumOfOctets] = '\0';

	if (1 == bIsServiceDataPresent)
	{
		pucServDataFromMsg = (unsigned char*) malloc(oTempServiceData.unNumOfOctets+1);
		memcpy(pucServDataFromMsg,oTempServiceData.pucOctetString,oTempServiceData.unNumOfOctets);
		pucServDataFromMsg[oTempServiceData.unNumOfOctets] = '\0';
	}




	//GET repoID first from the first query

    //the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//


	unsigned int	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType_i)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_PUR_GET_REPOID_BY_PUI_AND_SRV_IND;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_PUR_GET_REPOID_BY_PSI_AND_SRV_IND;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType_i);
        return Db_HSS_Retval_IncorrectParameters;
	}




	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[unDBShQueryMapIndex].oQueryString.pucOctetString,
			'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"','\"',pucServIndicFromMsg,'\"');



	//temp for debug
	#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
		printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
	#endif

	_Bool	bWasDataPresent = 0;



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
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," oDBEngineResult.ulNumberOfResults returned 0 i.e. Repo Data was not present\n");
		bWasDataPresent = 0;
	 }
	 else
	 {
		 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," oDBEngineResult.ulNumberOfResults returned NON 0 i.e. Repo Data Was present \n");
		 bWasDataPresent = 1;
	 }

	 if ( 1 == bWasDataPresent)
	 {
		 //need to handle update and deletion cases
		 unsigned int unRepoID = 0;
		 unRepoID = atoi(oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField);

		 unsigned char* pucServIndicFromDB = NULL;
		 unsigned char*	pucSeqNumFromDB = NULL;
		 unsigned char* pucServDataFromDB = NULL;
		 unsigned int	unSeqNumFromDB = 0;


		 pucServIndicFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField+1);

		memcpy(pucServIndicFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

		pucServIndicFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';



		pucServDataFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField+1);

		memcpy(pucServDataFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

		pucServDataFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';


		pucSeqNumFromDB = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField+1);

		memcpy(pucSeqNumFromDB,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][3].pchField,
			   oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField);

		pucSeqNumFromDB[oDBEngineResult.ppoAMPSDBEngineTable[0][3].nLengthOfField] = '\0';


		unSeqNumFromDB = atoi(pucSeqNumFromDB);

		free(pucSeqNumFromDB);
		pucSeqNumFromDB = NULL;



		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
								   hdbHSSThreadContext_io->hDBEngineHandle,
								   &oDBEngineResult);

		if (0 == unSeqNumFromMsg)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: 0 == unSeqNumFromMsg \n");
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}

		if ((unSeqNumFromMsg-1) != (unSeqNumFromDB % 65535) )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: (unSeqNumFromMsg-1)= %d != (unSeqNumFromDB % 65535) = %d , unSeqNumFromDB = %d\n",unSeqNumFromMsg-1, unSeqNumFromDB % 65535 , unSeqNumFromDB );
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}

		if ( 1 == bIsServiceDataPresent)
		{
			if (oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS )
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS \n");
				return Db_HSS_Retval_TooMuchData;
			}

			//make and execute the update Query

			bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


			//fill in the place holders inside the query
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
					hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_UPDATE_ALIAS_REPO_DATA].oQueryString.pucOctetString,
					'\"',pucServDataFromMsg,'\"',unSeqNumFromMsg, '\"',pucServIndicFromMsg,'\"',unRepoID);

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

			DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);

		}//end if ServiceData is present
		else
		{
			//have to remove the record
			bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


			//fill in the place holders inside the query
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
					hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_PUR_DEL_REPO_DATA_REC_BY_REPOID].oQueryString.pucOctetString,
					unRepoID);

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

			DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);


		}//end else i.e. case where serviceData is not present

	 }//end if Data was present in dB Already
	 else
	 {
		 //need to handle addition ,i.e. insertion case

		 //remove old DB Engine Results
		 DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									hdbHSSThreadContext_io->hDBEngineHandle,
									&oDBEngineResult);


		if (0 != unSeqNumFromMsg)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: For insertion case, 0 != unSeqNumFromMsg \n");
			return Db_HSS_Retval_TranspDataOutOfSynch;
		}


		if (0 == bIsServiceDataPresent)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: For insertion case, 0 != unSeqNumFromMsg \n");
			return Db_HSS_Retval_OperationNotAllowed;
		}

		if (oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS )
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR: oTempServiceData.unNumOfOctets > DB_SH_VARHCAR_MAX_SIZE_IN_DBMS \n");
			return Db_HSS_Retval_TooMuchData;
		}


		//make the query

		unsigned char* pucPUIToAddIfPSI = NULL;

		if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
		{
			//first get the mapping PUI from the DBMS by Query

			bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


			//fill in the place holders inside the query
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
					hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_GET_PUB_ID_FOR_PSI_ID].oQueryString.pucOctetString,
					'\"' , pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');

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

			if (0 == oDBEngineResult.ulNumberOfResults )
			{
				DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," 0 == oDBEngineResult.ulNumberOfResults \n");
				return Db_HSS_Retval_Failure;
			}



			pucPUIToAddIfPSI = (unsigned char*) malloc( oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField+1);

			memcpy(pucPUIToAddIfPSI,
				   oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
				   oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

			pucPUIToAddIfPSI[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';



			DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);

		}//end if PSI



		//now actually add data
		bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


		//fill in the place holders inside the query
		if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
		{
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_PUR_INSERT_REC_INTO_REPO_DATA].oQueryString.pucOctetString,
				'\"' ,pucServIndicFromMsg,'\"','\"',pucServDataFromMsg,'\"',
					unSeqNumFromMsg,'\"',pucPUIToAddIfPSI,'\"');
		}
		else
		{
			snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
				hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_SH_PUR_INSERT_REC_INTO_REPO_DATA].oQueryString.pucOctetString,
				'\"',pucServIndicFromMsg,'\"','\"',pucServDataFromMsg,'\"',
					unSeqNumFromMsg,'\"',pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,'\"');
		}


		//temp for debug
		#ifdef HSS_ENABLE_SH_QUERY_CONSOLE_DUMP
			printf("\n\n PRINTING QUERY \n\n\t%s\n\n",pucDB_ShQuery);
		#endif


		//actually run the query on the DBMS
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

		DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
									   hdbHSSThreadContext_io->hDBEngineHandle,
									   &oDBEngineResult);




	 }//end else ,i.e.Data was not present in dB






	return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_PSI_Activation
										(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 )
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

	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    t_EventDataShToDB*		pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}




	if (SH_IDENTITY_TYPE_DISTINCT_PSI != unIdentityType_i)
    {
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d, operation not allowed\n",unIdentityType_i);
        return Db_HSS_Retval_OperationNotAllowed;
	}




	//parse the XML received and get data out of it
	xmlDocPtr poXmlDoc = NULL;
	xmlNodePtr poCurrXMLNode = NULL;
	xmlNodePtr poCurrInnerXMLNode = NULL;
	xmlNodePtr poCurrInnerXMLNode2 = NULL;

	poXmlDoc	= xmlParseMemory( pRcvdEvDataFromSh->pucUserData,
								 strlen(pRcvdEvDataFromSh->pucUserData));

	if (NULL == poXmlDoc)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlParseMemory Failed ! \n");
		return Db_HSS_Retval_Failure;
	}

	poCurrXMLNode = xmlDocGetRootElement(poXmlDoc);

	if (NULL == poCurrXMLNode)
	{
		xmlFreeDoc(poXmlDoc);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlDocGetRootElement Failed ! \n");
		return Db_HSS_Retval_Failure;
	}


	poCurrXMLNode = poCurrXMLNode->xmlChildrenNode;

	xmlChar *			poXMLKey = NULL;
	t_DiamOctetString	oTempPSIActivVal = {0};


	while (poCurrXMLNode != NULL)
	{
		if ((!xmlStrcmp(poCurrXMLNode->name, (const xmlChar *)"Sh-IMS-Data")))
		{
			poCurrInnerXMLNode = poCurrXMLNode->xmlChildrenNode;
			while (poCurrInnerXMLNode != NULL)
			{
				if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"Extension")))
				{
					poCurrInnerXMLNode2 = poCurrInnerXMLNode->xmlChildrenNode;

					while (poCurrInnerXMLNode2 != NULL)
					{
						if ((!xmlStrcmp(poCurrInnerXMLNode2->name, (const xmlChar *)"PSIActivation")))
						{
							poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode2->xmlChildrenNode, 1);
							DMBase_CopyCharArrayToDiamString( &oTempPSIActivVal,poXMLKey);
							xmlFree(poXMLKey);
						}

						poCurrInnerXMLNode2 = poCurrInnerXMLNode2->next;
					}//end while for children of extension

				}
				poCurrInnerXMLNode = poCurrInnerXMLNode->next;
			}//end while for children of Sh-IMS-Data

		}//endif

		poCurrXMLNode = poCurrXMLNode->next;
	}//end while for children of Sh-Data



	xmlFreeDoc(poXmlDoc);
	poXMLKey = NULL;
	poXmlDoc = NULL;
	poCurrXMLNode = NULL;
	poCurrInnerXMLNode = NULL;
	poCurrInnerXMLNode2 = NULL;

	//GET data ExtensionID first from the first query
	unsigned int	unDataExtID = 0;




	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_GET_DATAEXTID_BY_PSI].oQueryString.pucOctetString,
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
		return Db_HSS_Retval_Failure;
	 }


	 unDataExtID = atoi(oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField);


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	//now run the next query based on the data extensionID we just got from above.
	unsigned char* pucTemp1 = NULL;



	pucTemp1 = (unsigned char*) malloc(oTempPSIActivVal.unNumOfOctets+1);
	memcpy(pucTemp1,oTempPSIActivVal.pucOctetString,oTempPSIActivVal.unNumOfOctets);
	pucTemp1[oTempPSIActivVal.unNumOfOctets] = '\0';


	unsigned int unPSIActVal = atoi(pucTemp1);

	free(pucTemp1);
	pucTemp1 = NULL;


	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_UPD_PSI_ACT_BY_DATAEXID].oQueryString.pucOctetString,
			unPSIActVal,unDataExtID);

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

	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return Db_HSS_Retval_Success;
}


t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB_DSAI(
										 HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i,
										 unsigned int	unIdentityType_i
										 )
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


	unsigned char			pucDB_ShQuery[SH_QUERY_SIZE] = {0};
	DMBASE_DBEngineResult	oDBEngineResult = {0};
	HDBHSSCONTEXT			hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
    t_EventDataShToDB*		pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;

	if (sizeof(t_EventDataShToDB) != unEventDataLen_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Event Data Size mismatch , Leaving\n");
        return Db_HSS_Retval_Failure;
	}





	//parse the XML received and get data out of it
	xmlDocPtr poXmlDoc = NULL;
	xmlNodePtr poCurrXMLNode = NULL;
	xmlNodePtr poCurrInnerXMLNode = NULL;
	xmlNodePtr poCurrInnerXMLNode1 = NULL;
	xmlNodePtr poCurrInnerXMLNode2 = NULL;
	xmlNodePtr poCurrInnerXMLNode3 = NULL;

	poXmlDoc	= xmlParseMemory(pRcvdEvDataFromSh->pucUserData,
								 strlen(pRcvdEvDataFromSh->pucUserData));

	if (NULL == poXmlDoc)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlParseMemory Failed ! \n");
		return Db_HSS_Retval_Failure;
	}

	poCurrXMLNode = xmlDocGetRootElement(poXmlDoc);

	if (NULL == poCurrXMLNode)
	{
		xmlFreeDoc(poXmlDoc);
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlDocGetRootElement Failed ! \n");
		return Db_HSS_Retval_Failure;
	}


	poCurrXMLNode = poCurrXMLNode->xmlChildrenNode;

	xmlChar *			poXMLKey = NULL;
	t_DiamOctetString	oTempDSAI_Tag = {0};
	t_DiamOctetString	oTempDSAI_Val = {0};


	while (poCurrXMLNode != NULL)
	{
		if ((!xmlStrcmp(poCurrXMLNode->name, (const xmlChar *)"Sh-IMS-Data")))
		{
			poCurrInnerXMLNode = poCurrXMLNode->xmlChildrenNode;
			while (poCurrInnerXMLNode != NULL)
			{
				if ((!xmlStrcmp(poCurrInnerXMLNode->name, (const xmlChar *)"Extension")))
				{
					poCurrInnerXMLNode1 = poCurrInnerXMLNode->xmlChildrenNode;
					while (poCurrInnerXMLNode1 != NULL)
					{
						if ((!xmlStrcmp(poCurrInnerXMLNode1->name, (const xmlChar *)"Extension")))
						{
							poCurrInnerXMLNode2 = poCurrInnerXMLNode1->xmlChildrenNode;
							while (poCurrInnerXMLNode2 != NULL)
							{
								if ((!xmlStrcmp(poCurrInnerXMLNode2->name, (const xmlChar *)"DSAI")))
								{
									poCurrInnerXMLNode3 = poCurrInnerXMLNode2->xmlChildrenNode;

									while (poCurrInnerXMLNode3 != NULL)
									{
										if ((!xmlStrcmp(poCurrInnerXMLNode3->name, (const xmlChar *)"DSAI-Tag")))
										{
											poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode3->xmlChildrenNode, 1);
											DMBase_CopyCharArrayToDiamString( &oTempDSAI_Tag,poXMLKey);
											xmlFree(poXMLKey);
										}
										else if ((!xmlStrcmp(poCurrInnerXMLNode3->name, (const xmlChar *)"DSAI-Value")))
										{
											poXMLKey = xmlNodeListGetString(poXmlDoc, poCurrInnerXMLNode3->xmlChildrenNode, 1);
											DMBase_CopyCharArrayToDiamString( &oTempDSAI_Val,poXMLKey);
											xmlFree(poXMLKey);
										}

										poCurrInnerXMLNode3 = poCurrInnerXMLNode3->next;
									}//end while for children of DSAI
								}
								poCurrInnerXMLNode2 = poCurrInnerXMLNode2->next;
							}//end while for children of extension->extension

						}
						poCurrInnerXMLNode1 = poCurrInnerXMLNode1->next;
					}//end while for children of extension

				}

				poCurrInnerXMLNode = poCurrInnerXMLNode->next;
			}//end while for children of RepositoryData

		}//endif

		poCurrXMLNode = poCurrXMLNode->next;
	}//end while



	xmlFreeDoc(poXmlDoc);
	poXMLKey = NULL;
	poXmlDoc = NULL;
	poCurrXMLNode = NULL;
	poCurrInnerXMLNode = NULL;
	poCurrInnerXMLNode1 = NULL;
	poCurrInnerXMLNode2 = NULL;
	poCurrInnerXMLNode3 = NULL;


	//the above can be PSI as well as PUI , check its type
	//from DB and make query accordingly
	//


	unsigned int	unDBShQueryMapIndex=0;

	if (SH_IDENTITY_TYPE_DISTINCT_PUI == unIdentityType_i)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_PUR_GET_TDSI_ID_AND_DSAI_INFO_BY_PUI;
	}
	else if (SH_IDENTITY_TYPE_DISTINCT_PSI == unIdentityType_i)
	{
		unDBShQueryMapIndex = DB_SH_QUERY_PUR_GET_TDSI_ID_AND_DSAI_INFO_BY_PSI;
	}
	else
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR : Invalid or unsupported Identity type => %d\n",unIdentityType_i);
        return Db_HSS_Retval_IncorrectParameters;
	}




	unsigned char* pucTemp1 = NULL;
	unsigned char* pucTemp2 = NULL;




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
		return Db_HSS_Retval_Failure;
	 }



    unsigned char* pucTDSI_IDTemp = NULL;
    unsigned char* pucDSAIValTemp  = NULL;
    unsigned char* pucDSAITagTemp = NULL;

	unsigned int unTDSI_ID = 0;


    pucTDSI_IDTemp = malloc(oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField +1);
    memcpy(pucTDSI_IDTemp,
           oDBEngineResult.ppoAMPSDBEngineTable[0][0].pchField,
           oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField);

    pucTDSI_IDTemp[oDBEngineResult.ppoAMPSDBEngineTable[0][0].nLengthOfField] = '\0';



	unTDSI_ID = atoi(pucTDSI_IDTemp);
    free(pucTDSI_IDTemp);
    pucTDSI_IDTemp = NULL;

    pucDSAIValTemp = malloc(oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField +1);
    memcpy(pucDSAIValTemp,
           oDBEngineResult.ppoAMPSDBEngineTable[0][1].pchField,
           oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField);

    pucDSAIValTemp[oDBEngineResult.ppoAMPSDBEngineTable[0][1].nLengthOfField] = '\0';


    pucDSAITagTemp = malloc(oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField +1);
    memcpy(pucDSAITagTemp,
           oDBEngineResult.ppoAMPSDBEngineTable[0][2].pchField,
           oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField);

    pucDSAITagTemp[oDBEngineResult.ppoAMPSDBEngineTable[0][2].nLengthOfField] = '\0';

    pucTemp1 = NULL;

    pucTemp2 = NULL;

    pucTemp1 = (unsigned char*) malloc(oTempDSAI_Tag.unNumOfOctets+1);
    memcpy(pucTemp1,oTempDSAI_Tag.pucOctetString,oTempDSAI_Tag.unNumOfOctets);
    pucTemp1[oTempDSAI_Tag.unNumOfOctets] = '\0';

    pucTemp2 = (unsigned char*) malloc(oTempDSAI_Val.unNumOfOctets+1);
    memcpy(pucTemp2,oTempDSAI_Val.pucOctetString,oTempDSAI_Val.unNumOfOctets);
    pucTemp2[oTempDSAI_Val.unNumOfOctets] = '\0';

    unsigned int unDSAIVal = atoi(pucTemp2);

    free(pucTemp2);
    pucTemp2 = NULL;



    if ( 0 != strcmp(pucDSAITagTemp ,pucTemp1) )
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR ! DSAI not available \n");
		return Db_HSS_Retval_DSAINotAvailable;
    }


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	//now run the next query based on the unTDSI_ID we just got from above.





	bzero(pucDB_ShQuery,sizeof(pucDB_ShQuery));


	//fill in the place holders inside the query
	snprintf(pucDB_ShQuery,SH_QUERY_SIZE,
			hDBHSSContext->oDBQueryMapForSh[DB_SH_QUERY_PUR_UPDATE_DSAI_BY_TDSI_ID].oQueryString.pucOctetString,
			unDSAIVal, '\"',pucTemp1,'\"',unTDSI_ID);

	free(pucTemp1);
	pucTemp1 = NULL;

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


	DMBASE_DBEngineFreeResults(hDBHSSContext->hDiamStackContext,
							   hdbHSSThreadContext_io->hDBEngineHandle,
							   &oDBEngineResult);


	return Db_HSS_Retval_Success;
}



t_Db_HSSReturnVal				db_sh_WritePUR_UserData_ToDB(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
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


    unsigned int    unIdentityType = 0;
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

    t_Db_HSSReturnVal   oRetValFromGetIDType = Db_HSS_Retval_Success;

    oRetValFromGetIDType = db_sh_GetUserIdentityTypeFromDB(hdbHSSThreadContext_io,
								pRcvdEvDataFromSh->oUserIdentity.pucPublicIdentity,&unIdentityType);

    if ( DappCommon_Retval_Success !=    oRetValFromGetIDType)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ERROR in getting Identity Type for the identity \n");
        return oRetValFromGetIDType;
    }


	//for debugging , dump the data xml received


    t_db_sh_AppSrvPermissions   oAppSrvPerms = {0};

    if ( DappCommon_Retval_Success !=  db_sh_GetAppServPermListFromDB(hdbHSSThreadContext_io,
										pRcvdEvDataFromSh->pucOriginHost,
                                                 pRcvdEvDataFromSh->unDataReference,&oAppSrvPerms))
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," db_sh_GetAppServPermListFromDB FAILED \n");
        return Db_HSS_Retval_Failure;
    }


    if ( 1 != oAppSrvPerms.bPUR_Allowed)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," PUR Operation is not allowed to this Server for the current Data Ref => %d \n",pRcvdEvDataFromSh->unDataReference);
        return Db_HSS_Retval_PermissionDenied;
    }

	t_Db_HSSReturnVal		oRetVal = Db_HSS_Retval_Failure;



	switch (pRcvdEvDataFromSh->unDataReference)
	{
	case	SH_DATA_REF_AVP_VAL_REPOSITORYDATA:

			oRetVal = db_sh_WritePUR_UserData_ToDB_RepoData(hdbHSSThreadContext_io,pucEvData_i,
												  unEventDataLen_i,unIdentityType);


			break;

	case	SH_DATA_REF_AVP_VAL_PSIACTIVATION:

			oRetVal = db_sh_WritePUR_UserData_ToDB_PSI_Activation(hdbHSSThreadContext_io,pucEvData_i,
												  unEventDataLen_i,unIdentityType);
		break;


	case	SH_DATA_REF_AVP_VAL_DSAI:



			oRetVal = db_sh_WritePUR_UserData_ToDB_DSAI(hdbHSSThreadContext_io,pucEvData_i,
												  unEventDataLen_i,unIdentityType);


		break;


	case	SH_DATA_REF_AVP_VAL_ALIASESREPOSITORYDATA:



			oRetVal = db_sh_WritePUR_UserData_ToDB_AliasRepoData(hdbHSSThreadContext_io,
										pucEvData_i,unEventDataLen_i,unIdentityType);

		break;


        //intentional falltrhough!!
    case SH_DATA_REF_AVP_VAL_IMSPUBLICIDENTITY:
    case SH_DATA_REF_AVP_VAL_IMSUSERSTATE:
    case SH_DATA_REF_AVP_VAL_S_CSCFNAME:
    case SH_DATA_REF_AVP_VAL_INITIALFILTERCRITERIA:
    case SH_DATA_REF_AVP_VAL_LOCATIONINFORMATION:
    case SH_DATA_REF_AVP_VAL_USERSTATE:
    case SH_DATA_REF_AVP_VAL_CHARGINGINFORMATION:
    case SH_DATA_REF_AVP_VAL_MSISDN:

        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," PUR operation not allowed for DataReference => %d\n",pRcvdEvDataFromSh->unDataReference);
        oRetVal = Db_HSS_Retval_OperationNotAllowed;

        break;



	default:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Invalid Value of DataReference = %d\n",pRcvdEvDataFromSh->unDataReference);
        oRetVal = Db_HSS_Retval_Failure;
		break;

	}//end switch

	return oRetVal;
}




t_Db_HSSReturnVal				db_sh_ProcessPURData(HDBHSSTHREADCONTEXT hdbHSSThreadContext_io,
										 unsigned char* pucEvData_i ,
										 unsigned int unEventDataLen_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering");

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




	HDBHSSCONTEXT		hDBHSSContext = hdbHSSThreadContext_io->hDBHSSMainContext;
	t_EventDataDBToSh	oEvtDataDBToSh = {0};
	t_EventDataShToDB*	pRcvdEvDataFromSh = (t_EventDataShToDB*)pucEvData_i;
    t_Db_HSSReturnVal   oRetVal = Db_HSS_Retval_Failure;

	oEvtDataDBToSh.unEvtCommandID = DB_SH_CMD_CODE_PUR;
	oEvtDataDBToSh.oVendorSpecificAppID = pRcvdEvDataFromSh->oVendorSpecificAppID;
	oEvtDataDBToSh.unAuthSessionState = pRcvdEvDataFromSh->unAuthSessionState;
	strncpy(oEvtDataDBToSh.pucSessionID,pRcvdEvDataFromSh->pucSessionID,
			SH_MAX_SMALL_STRING_SIZE);


    oRetVal = db_sh_WritePUR_UserData_ToDB(hdbHSSThreadContext_io,pucEvData_i,unEventDataLen_i);




	if ( Db_HSS_Retval_Success == oRetVal  )
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_Result;
		oEvtDataDBToSh.unRetCodeForShResp = DIAMETER_SUCCESS;
	}
    else if ( Db_HSS_Retval_NotFound == oRetVal  )
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
		oEvtDataDBToSh.unRetCodeForShResp = SH_DIAMETER_ERROR_USER_UNKNOWN;
	}
    else if ( Db_HSS_Retval_PermissionDenied ==  oRetVal)
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_USER_DATA_CANNOT_BE_MODIFIED;
	}
	else if ( Db_HSS_Retval_OperationNotAllowed ==  oRetVal)
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_OPERATION_NOT_ALLOWED;
	}
	else if ( Db_HSS_Retval_TooMuchData ==  oRetVal)
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_TOO_MUCH_DATA;
	}
	else if ( Db_HSS_Retval_TranspDataOutOfSynch ==  oRetVal)
    {
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
		oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_TRANSPARENT_DATA_OUT_OF_SYNC;
	}
    else if ( Db_HSS_Retval_DSAINotAvailable ==  oRetVal)
    {
        oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_ExperResult;
        oEvtDataDBToSh.unRetCodeForShResp = SH_PERMFAIL_EXPRES_DIAMETER_ERROR_DSAI_NOT_AVAILABLE;
    }

    else
	{
		oEvtDataDBToSh.oShRespRetCodeType = ShRespRetCode_Result;
		oEvtDataDBToSh.unRetCodeForShResp = DIAMETER_UNABLE_TO_COMPLY;
	}

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadContext_io->hUnitIOAgent,
					DB_RESPONSE_EVENT_FOR_SH_APP, &oEvtDataDBToSh, sizeof(oEvtDataDBToSh) )
	   )
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_IOAgentsSendEvt failed for DB_RESPONSE_EVENT_FOR_SH_APP \n");
	}
	else
	{
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," AMPS_IOAgentsSendEvt SUCCEEDED for DB_RESPONSE_EVENT_FOR_SH_APP \n");
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," LEaving");

	return Db_HSS_Retval_Success;
}

