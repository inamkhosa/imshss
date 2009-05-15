#include "sh_app.h"
#include "sh_msg_api.h"
#include "sh_avps.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "sh_diamretcodes.h"
#include "dmbase_parser.h"
#include "events.h"
#include "db_cmd_id.h"
#include "dappcommon_xmlutils.h"
#include "dappcommon_srvrconfload.h"
#include "hss.h"
#include "hss_oamps.h"

//externs
extern	int                     g_nDiamBaseTraceID ;

//globals
HSHAPPCONTEXT				   g_hSh_AppContext;

t_ShAppReturnVal	sh_app_CreateShAppContext(HSHAPPCONTEXT*	pohSh_AppContext_io,
											  HDIAMETERSTACKCONTEXT	hDiameterStackContext_i
											  )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == pohSh_AppContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pohSh_AppContext_io is NULL\n");
		return Sh_Retval_NullPointer;
	}
	if (NULL == hDiameterStackContext_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiameterStackContext_i is NULL\n");
		return Sh_Retval_NullPointer;
	}
	if (NULL != *pohSh_AppContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," NULL != *pohSh_AppContext_io\n");
		return Sh_Retval_IncorrectParameters;
	}


	DMBase_Malloc( hDiameterStackContext_i ,
				    (void*) pohSh_AppContext_io , sizeof(t_ShAppContext) );




	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return Sh_Retval_Success;
}


AMPS_MODULE_HANDLE	sh_app_ModuleInit(AMPS_HANDLE r_hAMPS_HANDLE, void* r_pvUserData)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");


	t_sh_config_params*	poShConfParams = (t_sh_config_params*)r_pvUserData;


    HDIAMETERSTACKCONTEXT	hDiamStackContext = poShConfParams->hDiameterStackContext;


	sh_app_CreateShAppContext(&g_hSh_AppContext,hDiamStackContext);
	g_hSh_AppContext->hDiamStackContext = hDiamStackContext;

	//temp for testing
	g_hSh_AppContext->oShFaultMgmtData.unFailedThresholdPUR = 5;
	g_hSh_AppContext->oShFaultMgmtData.unFailedThresholdUDR = 5;

	//temp for testing end
	

	if ( DMBase_Retval_Success !=  DMBase_RegisterApplicationCallback(hDiamStackContext,
			SH_DIAM_APP_ID, sh_app_MessageProcessor, NULL, (void*) g_hSh_AppContext ))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_RegisterApplicationCallback failed, Leaving\n");
		return NULL;
	}



	if(Sh_Retval_Success != sh_app_InitialiseApp(g_hSh_AppContext ))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_InitialiseApp failed, Leaving\n");
		return NULL;
	}

	if ( Sh_Retval_Success != sh_app_RegisterForEvents(g_hSh_AppContext))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_RegisterForEvents failed, Leaving\n");
		return NULL;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return g_hSh_AppContext;

}

t_ShAppReturnVal sh_app_InitialiseApp(HSHAPPCONTEXT		hShAppContext_io)

 {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

        if(NULL == hShAppContext_io)
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Error! hShAppContext_io is NULL , leaving\n");
            return Sh_Retval_NullPointer;
        }


		if(Sh_Retval_Success !=sh_app_CreateSessionTable(hShAppContext_io))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " A Problem Occurred while Creating Session Table\n");
			return Sh_Retval_Failure;
		}


       DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
       return Sh_Retval_Success;

 }

t_ShAppReturnVal sh_app_CreateSessionTable(HSHAPPCONTEXT	hShAppContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    DMBASE_HASH_HANDLE       hSessTableHandle   = NULL ;

    if(DMBase_Retval_Success != DMBase_CreateHashTable(hShAppContext_io->hDiamStackContext,
                                                       SH_SESSION_HASHTAB_MAX_SIZE,
                                                       &hSessTableHandle,NULL))
    {
          DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_CreateHashTable Failed ! \n");
          return Sh_Retval_Failure;
    }
    hShAppContext_io->hSessTableHandle = hSessTableHandle;

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return Sh_Retval_Success;

}


void sh_app_ModuleCleanup(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_MODULE_HANDLE r_hAMPS_MODULE_HANDLE)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
}

void  sh_app_DBResponseHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
					void* r_pvEventHandle, void* r_pvEventPayload)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");




	if ( NULL == r_pvEventPayload)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," r_pvEventPayload is NULL , Leaving\n");
		return;
	}

	HSHAPPCONTEXT	hShAppContext = NULL;
	hShAppContext = (HSHAPPCONTEXT) r_hAMPS_APP_HANDLE;

	if ( NULL == hShAppContext)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext is NULL , Leaving\n");
		return;
	}

	unsigned char*	pucEvData = (unsigned char*) r_pvEventPayload ;

	unsigned int	unDBCmdID = 0;
	unDBCmdID = ((t_EventDataDBToSh*) r_pvEventPayload)->unEvtCommandID;

	 switch(unDBCmdID)
	 {
	 case DB_SH_CMD_CODE_UDR:
		 sh_app_ProcessUDR_ResponseFromDB(hShAppContext, pucEvData);
		 break;

	 case DB_SH_CMD_CODE_PNR:
		 sh_app_ProcessPNR_ResponseFromDB(hShAppContext,pucEvData);
		 break;

	 case DB_SH_CMD_CODE_PUR:
		 sh_app_ProcessPUR_ResponseFromDB(hShAppContext,pucEvData);
		 break;

	 case DB_SH_CMD_CODE_SNR:
		 sh_app_ProcessSNR_ResponseFromDB(hShAppContext,pucEvData);
		 break;
	 }//end switch


    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");



}



t_DiamAPIReturnVal sh_app_MessageProcessor(HPEERTABLEENTRY hPeer_i, HDIAMETERMESSAGE hDiamMsg_i, void* pvAppContext_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

	if (NULL == hPeer_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL , Leaving \n");
        return Sh_Retval_NullPointer;
	}

	if (NULL == hDiamMsg_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiamMsg_i is NULL , Leaving \n");
        return Sh_Retval_NullPointer;
	}

	if (NULL == pvAppContext_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pvAppContext_i is NULL , Leaving \n");
        return Sh_Retval_NullPointer;
	}


	HSHAPPCONTEXT	hShAppContext = (HSHAPPCONTEXT) pvAppContext_i;


    unsigned int unCommandCode =0;

	DMBase_GetDiaMsgCmdCodeFromDiamMsg(hShAppContext->hDiamStackContext,hDiamMsg_i,&unCommandCode);

	_Bool	bIsThisShMsg = 0;
	sh_app_IsThisAnShMessage(hShAppContext,unCommandCode,&bIsThisShMsg);
	if (0 == bIsThisShMsg)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Error , not a supported Sh Message \n");
        return Sh_Retval_IncorrectParameters;
	}

	//validate the message received
	if ( Sh_Retval_Success !=   sh_app_ValidateMessage(hShAppContext,hPeer_i,hDiamMsg_i))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_ValidateMessage did not validate the diameter message received \n");
        return Sh_Retval_Failure;
	}



        // Get Session ID from Received message
    HAVP hSessIDAVPFromMsg     = NULL;


        if( DMBase_Retval_Success != DMBase_GetAVP(hShAppContext->hDiamStackContext,
													DMBASE_AVP_SESSION_ID,
                                                      hDiamMsg_i,
                                                      &hSessIDAVPFromMsg) )
         {
              DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "DMBase_GetAVP Fails , Leaving \n");
              return Sh_Retval_Failure;
         }



		t_DiamOctetString* poSessionID = NULL;
		DMBase_Malloc(hShAppContext->hDiamStackContext,
					  (void**) &poSessionID,sizeof(t_DiamOctetString));
		DMBase_CopyDiamStringAssumeNoNullTerm(poSessionID,
							  (t_DiamOctetString*) hSessIDAVPFromMsg->pData);


		t_ShSessionObject* poShSessionObj = NULL;



		sh_app_LookUpSessionObject(hShAppContext, *poSessionID, &poShSessionObj);



		if(NULL != poShSessionObj)
		{
			poShSessionObj->hPeer = hPeer_i;
			poShSessionObj->hMsg = hDiamMsg_i;
			(poShSessionObj->ulNumOfOutstandingMsgs)++;
		}
		else
		{
			DMBase_Malloc(hShAppContext->hDiamStackContext,
						   (void **)&poShSessionObj,
						  sizeof(t_ShSessionObject));

			poShSessionObj->hPeer = hPeer_i;
			poShSessionObj->hMsg = hDiamMsg_i;
			poShSessionObj->hShAppContext = hShAppContext;
			(poShSessionObj->ulNumOfOutstandingMsgs)++;
			sh_app_InsertSessionObject(hShAppContext, *poSessionID, poShSessionObj);
		}





	switch(unCommandCode)
	{
		case SH_CMD_CODE_UDR:

			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received Command code UDR \n");
			sh_app_ProcessUDRMessage(hShAppContext,hDiamMsg_i,hPeer_i);


		break;

		case SH_CMD_CODE_PUR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received Command code PUR \n");
			sh_app_ProcessPURMessage(hShAppContext,hDiamMsg_i,hPeer_i);

		break;

		case SH_CMD_CODE_SNR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received Command code SNR \n");

		break;


		case SH_CMD_CODE_PNR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received Command code PNR \n");

		break;


	default:
		break;

	}

	DMBase_DeleteDiamString(poSessionID);
    DMBase_Free(hShAppContext->hDiamStackContext,(void**) &poSessionID);
	poSessionID = NULL;

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return DMBase_Retval_Success;
}


t_ShAppReturnVal sh_app_LookUpSessionObject(HSHAPPCONTEXT	hShAppContext_io,
										   t_DiamOctetString oSessionID_i,
										   t_ShSessionObject** ppoShSessionObj_o)
{

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }
	if(NULL == ppoShSessionObj_o)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ppoShSessionObj_o is NULL \n");
			return Sh_Retval_NullPointer;
	}
	if(NULL == hShAppContext_io->hSessTableHandle)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io->hSessTableHandle is NULL \n");
			return Sh_Retval_NullPointer;
	}


   DMBASE_HASHTABLE_KEY_HANDLE   hHashTableKeyHandle=NULL;

   if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hShAppContext_io->hDiamStackContext,
														  oSessionID_i,
														 &hHashTableKeyHandle)
	 )
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         return Sh_Retval_Failure;
    }

    if ( DMBase_Retval_Not_Found == DMBase_HashTableSearch(hShAppContext_io->hDiamStackContext,
														   hShAppContext_io->hSessTableHandle,
														   hHashTableKeyHandle,
														   (void**)ppoShSessionObj_o )
	   )
    {

        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "DMBase_HashTableSearch did not find value in session table \n");
        DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);
        return Sh_Retval_NotFound;
    }
    DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);
}

t_ShAppReturnVal sh_app_RemoveSessionObject(HSHAPPCONTEXT hShAppContext_io,
											 t_DiamOctetString oSessionID_i)
{

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL == hShAppContext_io->hSessTableHandle)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io->hSessTableHandle is NULL \n");
			return Sh_Retval_NullPointer;
	}

    DMBASE_HASHTABLE_KEY_HANDLE   hHashTableKeyHandle = NULL;

    if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hShAppContext_io->hDiamStackContext,
														  oSessionID_i,
														  &hHashTableKeyHandle))
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         return Sh_Retval_Failure;
    }


	if(DMBase_Retval_Success != DMBase_HashTableRemove(hShAppContext_io->hDiamStackContext,
													   hShAppContext_io->hSessTableHandle,
													   hHashTableKeyHandle)
	   )
	{

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_HashTableRemove Failed \n");
        DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);
		return Sh_Retval_Failure;
	}

    DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);

}

t_ShAppReturnVal sh_app_InsertSessionObject(HSHAPPCONTEXT hShAppContext_io,
											 t_DiamOctetString oSessionID_i,
											 t_ShSessionObject* poSessionObj_i )
{

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }
	if(NULL == poSessionObj_i)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," poSessionObj_i is NULL \n");
			return Sh_Retval_NullPointer;
	}
	if(NULL == hShAppContext_io->hSessTableHandle)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io->hSessTableHandle is NULL \n");
			return Sh_Retval_NullPointer;
	}

    DMBASE_HASHTABLE_KEY_HANDLE   hHashTableKeyHandle = NULL;

    if(DMBase_Retval_Success != DMBase_CreateHashTableKey(hShAppContext_io->hDiamStackContext,
														  oSessionID_i,
														  &hHashTableKeyHandle))
    {
         DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_CreateHashTableKey Failed \n");
         return Sh_Retval_Failure;
    }


	if(DMBase_Retval_Success != DMBase_HashTableInsert(hShAppContext_io->hDiamStackContext,
													   hShAppContext_io->hSessTableHandle,
													   (void*) poSessionObj_i,
													   hHashTableKeyHandle)
	   )
	{

		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " DMBase_HashTableInsert Failed \n");
        DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);
		return Sh_Retval_Failure;
	}

    DMBase_Free(hShAppContext_io->hDiamStackContext,&hHashTableKeyHandle);

}






t_ShAppReturnVal	sh_app_ProcessSNRMessage(HSHAPPCONTEXT		hShAppContext_io,
											 HDIAMETERMESSAGE	hSNRMsg_i,
											HPEERTABLEENTRY hPeer_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }
    if(NULL == hSNRMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hSNRMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }
    if(NULL == hPeer_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL \n");
            return Sh_Retval_NullPointer;
    }




	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

}




t_ShAppReturnVal	sh_app_ProcessPNR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
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

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

}





t_ShAppReturnVal	sh_app_ProcessSNR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
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

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

}







t_ShAppReturnVal	sh_app_MakeAVPFromGeneralData(HSHAPPCONTEXT				hShAppContext_io,
												  HAVP*						phAVP_o,
												  unsigned int				unAVPCode_i,
												  HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
												  void*						pData_i,
												  unsigned int				unDataSize_i
										   )
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL == phAVP_o)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," phAVP_o is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL != (*phAVP_o))
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," (*phAVP_o) is NOT NULL \n");
            return Sh_Retval_IncorrectParameters;
    }

	if(NULL == pData_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pData_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	_Bool	bIsAVPGrouped = 0;
    _Bool   bIsAVPOctStringDerived = 0;



	if ( DMBase_Retval_Success != DMBase_IsAVPGrouped(hShAppContext_io->hDiamStackContext,
													  unAVPCode_i,&bIsAVPGrouped))
	{
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_IsAVPGrouped Failed ,Leaving \n");
            return Sh_Retval_Failure;
    }

    if ( DMBase_Retval_Success != DMBase_IsTypeOctetStringOrDerived(hShAppContext_io->hDiamStackContext,
													  unAVPCode_i,&bIsAVPOctStringDerived))
	{
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_IsTypeOctetStringOrDerived Failed ,Leaving \n");
            return Sh_Retval_Failure;
    }

    if (0 == bIsAVPOctStringDerived)
    {
        //todo !! correct for 64 bit when needed
        *(unsigned long*) pData_i = htonl(*(unsigned long*) pData_i);
    }

	if (1 == bIsAVPGrouped)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Grouped AVP passed,Leaving \n");
		return Sh_Retval_IncorrectParameters;
	}

	t_DiamDictionary	oDiamDict = {0};
	void*				pStructDataForAVP = NULL;

	if ( DMBase_Retval_Success != DMBase_GetDictionaryObjectFromDiameterContext(
		hShAppContext_io->hDiamStackContext,&oDiamDict))
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_GetDictionaryObjectFromDiameterContext failed\n");
		return Sh_Retval_Failure;
	}

	if ( 0 == DMBase_IsSafeToAccessDictElement(oDiamDict.poDictionaryArray,unAVPCode_i) )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Not safe to access dictionary element for this aVP code\n");
		return Sh_Retval_Failure;
	}

	//Make Data in correct Form
	if ( DMBase_Retval_Success !=  oDiamDict.poDictionaryArray[unAVPCode_i].oAVPDictionaryFunctionPtrs.pfGetStructData(
							hShAppContext_io->hDiamStackContext,
							pData_i,
							&pStructDataForAVP,unDataSize_i,hMsgAllocContext_io)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pfGetStructData failed\n");
		return Sh_Retval_Failure;
	}



	//make the AVP
	if ( DMBase_Retval_Success !=  DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,phAVP_o,
					 hMsgAllocContext_io, unAVPCode_i,
					 pStructDataForAVP,unDataSize_i, 0)
		)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_CreateAVP failed\n");
		return Sh_Retval_Failure;
	}


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return Sh_Retval_Success;

}

t_ShAppReturnVal 	sh_app_RegisterForEvents(HSHAPPCONTEXT		hShAppContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if(NULL == hShAppContext_io)
	{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
			return Sh_Retval_NullPointer;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hShAppContext_io->hDiamStackContext->hFrameworkHandle,
		  DB_RESPONSE_EVENT_FOR_SH_APP, sh_app_DBResponseHandler, hShAppContext_io))
    {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysRegExtEvt Failed for  DB_RESPONSE_EVENT_FOR_SH_APP , leaving \n");
			return Sh_Retval_Failure;
	}

	if ( AMPS_SUCCESS != AMPS_EvtSysRegExtEvt(hShAppContext_io->hDiamStackContext->hFrameworkHandle,
		  OAMPS_REQUEST_TO_SH_EVENT, sh_app_OAMPS_RequestHandler, hShAppContext_io))
    {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysRegExtEvt Failed for  OAMPS_REQUEST_TO_SH_EVENT , leaving \n");
			return Sh_Retval_Failure;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");
	return Sh_Retval_Success;

}

void  sh_app_OAMPS_RequestHandler(AMPS_HANDLE r_hAMPS_HANDLE, AMPS_APP_HANDLE r_hAMPS_APP_HANDLE,
					void* r_pvEventHandle, void* r_pvEventPayload)
{

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	printf("\n\n\n\n OAMPS Request Handler Called IN SH APP \n\n\n\n");

	if ( NULL == r_pvEventPayload)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," r_pvEventPayload is NULL , Leaving\n");
		return;
	}

	HSHAPPCONTEXT	hShAppContext = NULL;
	hShAppContext = (HSHAPPCONTEXT) r_hAMPS_APP_HANDLE;

	if ( NULL == hShAppContext)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext is NULL , Leaving\n");
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

	if ( Sh_Retval_Success != sh_app_GetValForOID(hShAppContext,hssOIDRespType_Long,
							  unOID,&(oRespDataToSend.lResp),NULL,0)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_GetValForOID failed for OID => %s , Leaving\n",pRcvdEvDataFromOamps->oOIDDetails.pucSupportedOID);
		return;
	}


	oRespDataToSend.oOIDDetails = pRcvdEvDataFromOamps->oOIDDetails;
	oRespDataToSend.oRespDataType = hssOIDRespType_Long;


	//send response event to oamps module
	if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(hShAppContext->hDiamStackContext->hFrameworkHandle,
							  OAMPS_RESPONSE_FROM_SH_EVENT,
							  &oRespDataToSend,sizeof(oRespDataToSend) )
		   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_RESPONSE_FROM_SH_EVENT ,Leaving \n");
		return ;
	}



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
}

t_ShAppReturnVal 	sh_app_GetValForOID(HSHAPPCONTEXT		hShAppContext_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");
	t_ShAppReturnVal	oRetval = Sh_Retval_Success;

	if(NULL == hShAppContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
		return Sh_Retval_NullPointer;
	}

	if (hssOIDRespType_Long == ohssOIDRespType_i)
	{
		if(NULL == plVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," plVal_o is NULL \n");
			return Sh_Retval_NullPointer;
		}
	}
	else if (hssOIDRespType_String == ohssOIDRespType_i)
	{
		if(NULL == pucVal_o)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucVal_o is NULL \n");
			return Sh_Retval_NullPointer;
		}
	}

	switch(unOID_i)
	{
		
	case hssFMOid_failedThresholdUDR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssFMOid_failedThresholdUDR OID \n");
			*plVal_o = hShAppContext_io->oShFaultMgmtData.unFailedThresholdUDR;
		break;

	case hssFMOid_failedThresholdPUR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssFMOid_failedThresholdPUR OID \n");
			*plVal_o = hShAppContext_io->oShFaultMgmtData.unFailedThresholdPUR;
		break;


	case hssPMOid_attemptedUDRDTR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedUDRDTR OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unAttemptedUDRDTR;
		break;

	case hssPMOid_successfulUDADTR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulUDADTR OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unSuccessfulUDADTR;
		break;

	case hssPMOid_failedUDADTR:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedUDADTR OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unFailedUDADTR;
		break;

        
	case hssPMOid_attemptedPURDTU:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_attemptedPURDTU OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unAttemptedPURDTU;
		break;


	case hssPMOid_successfulPUADTU:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_successfulPUADTU OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unSuccessfulPUADTU;
		break;

	case hssPMOid_failedPUADTU:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received hssPMOid_failedPUADTU OID \n");
			*plVal_o = hShAppContext_io->oShPerfMgmtData.unFailedPUADTU;
		break;
	
	default:
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Unsupported OID Val => %d Received  \n",unOID_i);
			oRetval = Sh_Retval_IncorrectParameters;
		break;


	}//end switch(unOID_i)

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	return oRetval;

}
