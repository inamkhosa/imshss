#include "sh_msg_api.h"



int g_nShTraceID;

t_ShAppReturnVal Sh_StuffAVPsFromSrcToDestList( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
												DMBase_SList* pAVPListDest_o,
								    		DMBase_SList* pAVPListSrc_i )
{

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*		pCurrentNode = pAVPListSrc_i;

	if( NULL == pAVPListDest_o )
    {
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  pAVPListDest_o is NULL \n");
        return Sh_Retval_NullPointer ;   
    }

	if( NULL == pAVPListSrc_i )
    {
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  pAVPList_i is NULL \n");
        return Sh_Retval_NullPointer ;   
    }

	HAVP	hCurrAVP = NULL;

	while ( NULL != pCurrentNode ) 
	{
		  DMBase_SListGetNodeData(hDiamStackContext_io, pCurrentNode, (void *)&hCurrAVP);
		  DMBase_AppendSList(hDiamStackContext_io, &pAVPListDest_o, (void*)hCurrAVP);
		  DMBase_SListGetNextNode( hDiamStackContext_io , pCurrentNode , &pCurrentNode);

	} //end while   

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;

}



t_ShAppReturnVal Sh_CreateShPullRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										 HDIAMETERMESSAGE*	phMsgUDR_o,
										  t_ShPullParams	oShPullParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgUDR_o) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgUDR_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShPullParams_i.hAVPSessionID) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPSessionID);
	}

	if (NULL == oShPullParams_i.hAVPVendorSpecificAppID) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPVendorSpecificAppID);
	}


	if (NULL == oShPullParams_i.hAVPAuthSessionState) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShPullParams_i.hAVPOriginHost) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPOriginHost);
	}


	if (NULL == oShPullParams_i.hAVPOriginRealm) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPOriginRealm);
	}


	if (NULL == oShPullParams_i.hAVPUserIdentity) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserIdentity is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPUserIdentity);
	}


	if (NULL == oShPullParams_i.hAVPDestinationRealm) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDestinationRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPDestinationRealm);
	}

	if ( NULL == oShPullParams_i.pListOfDataReferenceAVPs) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory list of dataReference AVPs is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfDataReferenceAVPs);
	}

	//now adding optional AVPs if present
	if (NULL != oShPullParams_i.hAVPDestinationHost) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPDestinationHost);
	}

	if (NULL != oShPullParams_i.hAVPWildcardedPSI) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPWildcardedPSI);
	}

	if (NULL != oShPullParams_i.hAVPServerName) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPServerName);
	}

	if (NULL != oShPullParams_i.hAVPRequestedDomain) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPRequestedDomain);
	}

	if (NULL != oShPullParams_i.hAVPCurrentLocation) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPCurrentLocation);
	}

	if (NULL != oShPullParams_i.hAVPDSAI_Tag) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullParams_i.hAVPDSAI_Tag);
	}

	if ( NULL != oShPullParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShPullParams_i.pListOfServiceIndicationAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfServiceIndicationAVPs);
	}

	if ( NULL != oShPullParams_i.pListOfIdentitySetAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfIdentitySetAVPs);
	}

	if ( NULL != oShPullParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShPullParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullParams_i.pListOfRouteRecordAVPs);
	}



	 DMBase_AddAVPCollectionToDiameterMessage( hDiamStackContext_io , *phMsgUDR_o , pListOfAVPs);
	
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}

t_ShAppReturnVal Sh_CreateShPullResponse(HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										  HDIAMETERMESSAGE*	phMsgUDA_o,
										  t_ShPullRespParams	oShPullRespParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgUDA_o) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgUDA_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShPullRespParams_i.hAVPSessionID) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPSessionID);
	}

	if (NULL == oShPullRespParams_i.hAVPVendorSpecificAppID) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPVendorSpecificAppID);
	}

	//one and only one of Result-Code and Experimental-Result AVP MUST be present
	if ( (NULL != oShPullRespParams_i.hAVPResultCode) &&
		 (NULL != oShPullRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are present \n");
		return Sh_Retval_IncorrectParameters;
	}
	else if ( (NULL == oShPullRespParams_i.hAVPResultCode) &&
		 (NULL == oShPullRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are absent \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		if (NULL != oShPullRespParams_i.hAVPResultCode)
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPResultCode);
		}

		if (NULL != oShPullRespParams_i.hAVPExperimentalResult) 
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPExperimentalResult);
		}
	}


	if (NULL == oShPullRespParams_i.hAVPAuthSessionState) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShPullRespParams_i.hAVPOriginHost) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPOriginHost);
	}


	if (NULL == oShPullRespParams_i.hAVPOriginRealm) 
	{
        DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPOriginRealm);
	}
	
	//now adding optional AVPs if present
	if (NULL != oShPullRespParams_i.hAVPWildcardedPSI) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPWildcardedPSI);
	}

	if (NULL != oShPullRespParams_i.hAVPUserData) 
	{
  		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShPullRespParams_i.hAVPUserData);
	}

	if ( NULL != oShPullRespParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullRespParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShPullRespParams_i.pListOfFailedAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullRespParams_i.pListOfFailedAVPs);
	}

	if ( NULL != oShPullRespParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullRespParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShPullRespParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShPullRespParams_i.pListOfRouteRecordAVPs);
	}


	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgUDA_o , pListOfAVPs);
	
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}



t_ShAppReturnVal Sh_CreateShUpdateRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										   HDIAMETERMESSAGE*	phMsgPUR_o,
										  t_ShUpdateParams	oShUpdateParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgPUR_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgPUR_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShUpdateParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPSessionID);
	}

	if (NULL == oShUpdateParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPVendorSpecificAppID);
	}


	if (NULL == oShUpdateParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShUpdateParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPOriginHost);
	}


	if (NULL == oShUpdateParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPOriginRealm);
	}

	if (NULL == oShUpdateParams_i.hAVPDestinationRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDestinationRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPDestinationRealm);
	}

	if (NULL == oShUpdateParams_i.hAVPUserIdentity) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserIdentity is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPUserIdentity);
	}

	if (NULL == oShUpdateParams_i.hAVPDataReference) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDataReference is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPDataReference);
	}

	if (NULL == oShUpdateParams_i.hAVPUserData) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserData is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPUserData);
	}


	//now adding optional AVPs if present
	if (NULL != oShUpdateParams_i.hAVPDestinationHost) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPDestinationHost);
	}

	if (NULL != oShUpdateParams_i.hAVPWildcardedPSI) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateParams_i.hAVPWildcardedPSI);
	}

	if ( NULL != oShUpdateParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateParams_i.pListOfSupportedFeaturesAVPs);
	}

	
	if ( NULL != oShUpdateParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShUpdateParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateParams_i.pListOfRouteRecordAVPs);
	}


	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgPUR_o , pListOfAVPs);

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}

t_ShAppReturnVal Sh_CreateShUpdateResponse( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											HDIAMETERMESSAGE*	phMsgPUA_o,
										  t_ShUpdateRespParams	oShUpdateRespParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgPUA_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgPUA_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShUpdateRespParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPSessionID);
	}

	if (NULL == oShUpdateRespParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPVendorSpecificAppID);
	}

	//one and only one of Result-Code and Experimental-Result AVP MUST be present
	if ( (NULL != oShUpdateRespParams_i.hAVPResultCode) &&
		 (NULL != oShUpdateRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are present \n");
		return Sh_Retval_IncorrectParameters;
	}
	else if ( (NULL == oShUpdateRespParams_i.hAVPResultCode) &&
		 (NULL == oShUpdateRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are absent \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		if (NULL != oShUpdateRespParams_i.hAVPResultCode)
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPResultCode);
		}

		if (NULL != oShUpdateRespParams_i.hAVPExperimentalResult) 
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPExperimentalResult);
		}
	}


	if (NULL == oShUpdateRespParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShUpdateRespParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPOriginHost);
	}


	if (NULL == oShUpdateRespParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPOriginRealm);
	}

    //now adding optional AVPs if present
	if (NULL != oShUpdateRespParams_i.hAVPWildcardedPSI) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShUpdateRespParams_i.hAVPWildcardedPSI);
	}

	if ( NULL != oShUpdateRespParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateRespParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShUpdateRespParams_i.pListOfFailedAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateRespParams_i.pListOfFailedAVPs);
	}

	if ( NULL != oShUpdateRespParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateRespParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShUpdateRespParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShUpdateRespParams_i.pListOfRouteRecordAVPs);
	}

	
	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgPUA_o , pListOfAVPs);

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}

t_ShAppReturnVal Sh_CreateShSubsNotifRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											  HDIAMETERMESSAGE*	phMsgSNR_o,
										  t_ShSubsNotifParams	oShSubsNotifParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgSNR_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgSNR_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShSubsNotifParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPSessionID);
	}

	if (NULL == oShSubsNotifParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPVendorSpecificAppID);
	}


	if (NULL == oShSubsNotifParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShSubsNotifParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPOriginHost);
	}


	if (NULL == oShSubsNotifParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPOriginRealm);
	}

	if (NULL == oShSubsNotifParams_i.hAVPDestinationRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDestinationRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPDestinationRealm);
	}

	if (NULL == oShSubsNotifParams_i.hAVPUserIdentity) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserIdentity is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPUserIdentity);
	}

	if (NULL == oShSubsNotifParams_i.hAVPSubsReqType) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSubsReqType is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPSubsReqType);
	}

	if ( NULL != oShSubsNotifParams_i.pListOfDataReferenceAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifParams_i.pListOfDataReferenceAVPs);
	}
	else
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP List pListOfDataReferenceAVPs is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	

	//now adding optional AVPs if present
	if (NULL != oShSubsNotifParams_i.hAVPDestinationHost) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPDestinationHost);
	}

	if (NULL != oShSubsNotifParams_i.hAVPWildcardedPSI) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPWildcardedPSI);
	}

	if (NULL != oShSubsNotifParams_i.hAVPSendDataIndication) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPSendDataIndication);
	}

	if (NULL != oShSubsNotifParams_i.hAVPServerName) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPServerName);
	}

	if (NULL != oShSubsNotifParams_i.hAVPIdentitySet) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPIdentitySet);
	}

	if (NULL != oShSubsNotifParams_i.hAVPExpiryTime) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPExpiryTime);
	}

	if (NULL != oShSubsNotifParams_i.hAVPDSAI_Tag) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifParams_i.hAVPDSAI_Tag);
	}

	if ( NULL != oShSubsNotifParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShSubsNotifParams_i.pListOfServiceIndicationAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifParams_i.pListOfServiceIndicationAVPs);
	}

	if ( NULL != oShSubsNotifParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShSubsNotifParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifParams_i.pListOfRouteRecordAVPs);
	}

	
	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgSNR_o , pListOfAVPs);


	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}

t_ShAppReturnVal Sh_CreateShSubsNotifResponse( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
											   HDIAMETERMESSAGE*	phMsgSNA_o,
										  t_ShSubsNotifRespParams	oShSubsNotifRespParams_i 
										)
{

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgSNA_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgSNA_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShSubsNotifRespParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPSessionID);
	}

	if (NULL == oShSubsNotifRespParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPVendorSpecificAppID);
	}

	//one and only one of Result-Code and Experimental-Result AVP MUST be present
	if ( (NULL != oShSubsNotifRespParams_i.hAVPResultCode) &&
		 (NULL != oShSubsNotifRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are present \n");
		return Sh_Retval_IncorrectParameters;
	}
	else if ( (NULL == oShSubsNotifRespParams_i.hAVPResultCode) &&
		 (NULL == oShSubsNotifRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are absent \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		if (NULL != oShSubsNotifRespParams_i.hAVPResultCode)
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPResultCode);
		}

		if (NULL != oShSubsNotifRespParams_i.hAVPExperimentalResult) 
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPExperimentalResult);
		}
	}


	if (NULL == oShSubsNotifRespParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShSubsNotifRespParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPOriginHost);
	}


	if (NULL == oShSubsNotifRespParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPOriginRealm);
	}

	//now adding optional AVPs if present
	if (NULL != oShSubsNotifRespParams_i.hAVPWildcardedPSI) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPWildcardedPSI);
	}

	if (NULL != oShSubsNotifRespParams_i.hAVPUserData) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPUserData);
	}

	if (NULL != oShSubsNotifRespParams_i.hAVPExpiryTime) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShSubsNotifRespParams_i.hAVPExpiryTime);
	}

	if ( NULL != oShSubsNotifRespParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifRespParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShSubsNotifRespParams_i.pListOfFailedAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifRespParams_i.pListOfFailedAVPs);
	}

	if ( NULL != oShSubsNotifRespParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifRespParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShSubsNotifRespParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShSubsNotifRespParams_i.pListOfRouteRecordAVPs);
	}

	
	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgSNA_o , pListOfAVPs);

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;

}


t_ShAppReturnVal Sh_CreateShNotifRequest( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										  HDIAMETERMESSAGE*	phMsgPNR_o,
										  t_ShNotifParams	oShNotifParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgPNR_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgPNR_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShNotifParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPSessionID);
	}

	if (NULL == oShNotifParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPVendorSpecificAppID);
	}


	if (NULL == oShNotifParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShNotifParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPOriginHost);
	}


	if (NULL == oShNotifParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPOriginRealm);
	}

	if (NULL == oShNotifParams_i.hAVPDestinationHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDestinationHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPDestinationHost);
	}

	if (NULL == oShNotifParams_i.hAVPDestinationRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPDestinationRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPDestinationRealm);
	}

	if (NULL == oShNotifParams_i.hAVPUserIdentity) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserIdentity is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPUserIdentity);
	}

	if (NULL == oShNotifParams_i.hAVPUserData) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPUserData is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPUserData);
	}

	//now adding optional AVPs if present

	if (NULL != oShNotifParams_i.hAVPWildcardedPSI) 
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifParams_i.hAVPWildcardedPSI);
	}

	if ( NULL != oShNotifParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShNotifParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShNotifParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifParams_i.pListOfRouteRecordAVPs);
	}

	
	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgPNR_o , pListOfAVPs);

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}

t_ShAppReturnVal Sh_CreateShNotifResponse( HDIAMETERSTACKCONTEXT hDiamStackContext_io,
										   HDIAMETERMESSAGE*	phMsgPNA_o,
										  t_ShNotifRespParams	oShNotifRespParams_i 
										)
{
	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

	DMBase_SList*	pListOfAVPs = NULL;

	if (NULL == phMsgPNA_o) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  phMsgPNA_o is NULL \n");
		return Sh_Retval_NullPointer;
	}

	//checking for mandatory AVPs first
	if (NULL == oShNotifRespParams_i.hAVPSessionID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPSessionID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPSessionID);
	}

	if (NULL == oShNotifRespParams_i.hAVPVendorSpecificAppID) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPVendorSpecificAppID is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPVendorSpecificAppID);
	}

	//one and only one of Result-Code and Experimental-Result AVP MUST be present
	if ( (NULL != oShNotifRespParams_i.hAVPResultCode) &&
		 (NULL != oShNotifRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are present \n");
		return Sh_Retval_IncorrectParameters;
	}
	else if ( (NULL == oShNotifRespParams_i.hAVPResultCode) &&
		 (NULL == oShNotifRespParams_i.hAVPExperimentalResult)
	   ) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  Error , both Result-Code and Experimental-Result AVP are absent \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		if (NULL != oShNotifRespParams_i.hAVPResultCode)
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPResultCode);
		}

		if (NULL != oShNotifRespParams_i.hAVPExperimentalResult) 
		{
			DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPExperimentalResult);
		}
	}


	if (NULL == oShNotifRespParams_i.hAVPAuthSessionState) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPAuthSessionState is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPAuthSessionState);
	}


	if (NULL == oShNotifRespParams_i.hAVPOriginHost) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginHost is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPOriginHost);
	}


	if (NULL == oShNotifRespParams_i.hAVPOriginRealm) 
	{
		DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_ERROR, "  mandatory AVP hAVPOriginRealm is NULL \n");
		return Sh_Retval_IncorrectParameters;
	}
	else
	{
		DMBase_AppendSList(hDiamStackContext_io, &pListOfAVPs, (void*)oShNotifRespParams_i.hAVPOriginRealm);
	}

	//now adding optional AVPs if present
	
	if ( NULL != oShNotifRespParams_i.pListOfSupportedFeaturesAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifRespParams_i.pListOfSupportedFeaturesAVPs);
	}

	if ( NULL != oShNotifRespParams_i.pListOfFailedAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifRespParams_i.pListOfFailedAVPs);
	}

	if ( NULL != oShNotifRespParams_i.pListOfProxyInfoAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifRespParams_i.pListOfProxyInfoAVPs);
	}

	if ( NULL != oShNotifRespParams_i.pListOfRouteRecordAVPs) 
	{
		Sh_StuffAVPsFromSrcToDestList(hDiamStackContext_io,pListOfAVPs,oShNotifRespParams_i.pListOfRouteRecordAVPs);
	}

	
	 DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext_io , *phMsgPNA_o , pListOfAVPs);

	DMBASE_TRACE(g_nShTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
	
	return Sh_Retval_Success;
}
