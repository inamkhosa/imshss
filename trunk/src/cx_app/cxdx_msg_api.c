
#include "dmbase_ampswrappers.h"
#include "cxdx_msg_api.h"
#include "dmbase_dictionary.h"
#include "dmbase_parser.h"
#include "dmbase_debug.h"
#include "hss.h"


extern int g_nDiamBaseTraceID;
//extern t_AVPDictionaryElement		g_oDmBase_AVPDictionary[];
//extern HDIAMETERSTACKCONTEXT            g_hDiameterStackContext;


t_CxDxAPIRetVal Cx_CreateVendorAppAVP(	HCXAPPCONTEXT	hCxAppContext_io,
										HAVP*			phVendorSpecificApp,
										HMSGALLOCATIONCONTEXT  hMsgAllocContext_io)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    HAVP hSuppAppAVP = NULL;
    HAVP hVendorSpecificAppAVP = NULL;
    HAVP hAppVendorIdAVP = NULL;
    t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;

	if(DMBase_Retval_Success !=	DMBase_CreateUnsigned32AVP( hDiamStackContext,
															DMBASE_AVP_AUTH_APPLICATION_ID,
															&hSuppAppAVP,
															CxDx_APPLICATION,
															hMsgAllocContext_io))
	{
		oReturnVal = CxDx_Retval_Failure;
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Application AVP.\n");
		goto ExitMe;
	}

	if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP( hDiamStackContext,
															DMBASE_AVP_VENDOR_ID,
															&hAppVendorIdAVP,
															CxDx_VENDOR_IDENTIFIER,
															hMsgAllocContext_io))
	{
		oReturnVal = CxDx_Retval_Failure;
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Vendor-Id AVP.\n");
		goto ExitMe;
	}

	if(DMBase_Retval_Success != DMBase_CreateGroupedAVP(hDiamStackContext,
														phVendorSpecificApp,
														hMsgAllocContext_io,
														DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,
														0))
	{
		oReturnVal = CxDx_Retval_Failure;
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to create Vendor-Specific-Application-ID AVP.\n");
		goto ExitMe;
	}

	if(DMBase_Retval_Success != DMBase_AddAVPToGroup(hDiamStackContext, *phVendorSpecificApp, hSuppAppAVP))
	{
		oReturnVal = CxDx_Retval_Failure;
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to add Vendor-Specific-Application-ID AVP.\n");
		goto ExitMe;
	}
	hSuppAppAVP = NULL;

	if(DMBase_Retval_Success != DMBase_AddAVPToGroup(hDiamStackContext, *phVendorSpecificApp, hAppVendorIdAVP))
	{
		oReturnVal = CxDx_Retval_Failure;
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Failed to add Vendor-ID AVP.\n");
		goto ExitMe;
	}
	hAppVendorIdAVP = NULL;


	ExitMe:
/*
	if(CxDx_Retval_Success != oReturnVal)
	{
		DMBase_DestroyAVP(hDiamStackContext,&hSuppAppAVP);
		DMBase_DestroyAVP(hDiamStackContext,&hAppVendorIdAVP);
		DMBase_DestroyAVP(hDiamStackContext,phVendorSpecificApp);
	}*/
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

    return oReturnVal;
}

t_CxDxAPIRetVal Cx_CreateCxQuery_SelectPull_Resp(	HCXAPPCONTEXT 					hCxAppContext_io,
													HDIAMETERMESSAGE 				hDiaMsgUAA_io,
													HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
													t_CxQuery_SelectPull_RespParams oParams_i,
													unsigned int					unAppID_i,
													unsigned char					ucFlags_i,
													unsigned int					unHopByHopID_i,
													unsigned int					unEndToEndID_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;
	DMBase_SList*	pAVPList = NULL;
	t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    HAVP hAVP_VendorID = NULL;
    HAVP hAVP_Exp_Result = NULL;

     if(NULL != oParams_i.hAVP_Session_ID)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Session_ID);
     }
     else
     {
   	    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
   	    oReturnVal = CxDx_Retval_Failure;
    	goto ExitMe;
     }

     HAVP hVendorSpecificAppAVP = NULL;

     if(CxDx_Retval_Success != (oReturnVal = Cx_CreateVendorAppAVP(hCxAppContext_io, &hVendorSpecificAppAVP, hMsgAllocContext_io)))
     {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
     	oReturnVal = CxDx_Retval_Failure;
      	goto ExitMe;
     }

     DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hVendorSpecificAppAVP );


     if(NULL == oParams_i.hAVP_ResCode && NULL == oParams_i.hAVP_Exp_Result_Code ||
        NULL != oParams_i.hAVP_ResCode && NULL != oParams_i.hAVP_Exp_Result_Code)
     {
      	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
      	oReturnVal = CxDx_Retval_Failure;
       	goto ExitMe;
     }
     else if(NULL != oParams_i.hAVP_ResCode)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_ResCode);
     }
     else if(NULL != oParams_i.hAVP_Exp_Result_Code)
     {

        if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hAVP_Exp_Result,hMsgAllocContext_io,
																DMBASE_AVP_EXPERIMENTAL_RESULT, 0))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_VENDOR_ID,
																&hAVP_VendorID, CxDx_VENDOR_IDENTIFIER, hMsgAllocContext_io))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, hAVP_VendorID);
        hAVP_VendorID = NULL;

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, oParams_i.hAVP_Exp_Result_Code);
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hAVP_Exp_Result);
        hAVP_Exp_Result = NULL;
     }

     if(NULL != oParams_i.hAVP_Auth_Session_State)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Auth_Session_State);
     }
     else
     {
       	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
       	oReturnVal = CxDx_Retval_Failure;
        goto ExitMe;
     }


     if(NULL != oParams_i.hAVP_Orig_Host)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Host);
     }
     else
     {
    	 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	 oReturnVal = CxDx_Retval_Failure;
    	 goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_Orig_Realm)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Realm);
     }
     else
     {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	oReturnVal = CxDx_Retval_Failure;
     	goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_SrvName)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_SrvName);
     }

     if(NULL != oParams_i.hAVP_SrvCapab)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_SrvCapab);
     }



     if(NULL != oParams_i.hAVP_Wildcard_IMPU)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Wildcard_IMPU);
     }

     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext,hDiaMsgUAA_io,pAVPList);

     ExitMe:
    /* if(CxDx_Retval_Success != oReturnVal)
     {
    	 DMBase_DestroyDiaMessage(hDiamStackContext, phDiaMsgUAA_io);
     }*/

     DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

     return oReturnVal;
}


t_CxDxAPIRetVal Cx_CreateCxPut_PullResp(	HCXAPPCONTEXT 			hCxAppContext_io,
											HDIAMETERMESSAGE 		hDiaMsgSAA_io,
											HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
											t_CxPut_PullRespParams	oParams_i,
											unsigned int			unAppID_i,
											unsigned char			ucFlags_i,
											unsigned int			unHopByHopID_i,
											unsigned int			unEndToEndID_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");


    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;
    DMBase_SList*	pAVPList = NULL;
	t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    HAVP hAVP_VendorID = NULL;
    HAVP hAVP_Exp_Result = NULL;


     if(NULL != oParams_i.hAVP_Session_ID)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Session_ID);
     }
     else
     {
   	    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
   	    oReturnVal = CxDx_Retval_Failure;
    	goto ExitMe;
     }

     HAVP hVendorSpecificAppAVP = NULL;

     if(CxDx_Retval_Success != (oReturnVal = Cx_CreateVendorAppAVP(hCxAppContext_io, &hVendorSpecificAppAVP, hMsgAllocContext_io)))
     {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
     	oReturnVal = CxDx_Retval_Failure;
      	goto ExitMe;
     }

     DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hVendorSpecificAppAVP );


     if(NULL == oParams_i.hAVP_ResCode && NULL == oParams_i.hAVP_Exp_Result_Code ||
        NULL != oParams_i.hAVP_ResCode && NULL != oParams_i.hAVP_Exp_Result_Code)
     {
      	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
      	oReturnVal = CxDx_Retval_Failure;
       	goto ExitMe;
     }
     else if(NULL != oParams_i.hAVP_ResCode)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_ResCode);
     }
     else if(NULL != oParams_i.hAVP_Exp_Result_Code)
     {

        if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hAVP_Exp_Result,hMsgAllocContext_io,
																DMBASE_AVP_EXPERIMENTAL_RESULT, 0))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_VENDOR_ID,
																&hAVP_VendorID, CxDx_VENDOR_IDENTIFIER, hMsgAllocContext_io))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, hAVP_VendorID);
        hAVP_VendorID = NULL;

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, oParams_i.hAVP_Exp_Result_Code);
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hAVP_Exp_Result);
        hAVP_Exp_Result = NULL;
     }

     if(NULL != oParams_i.hAVP_Auth_Session_State)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Auth_Session_State);
     }
     else
     {
       	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
       	oReturnVal = CxDx_Retval_Failure;
        goto ExitMe;
     }


     if(NULL != oParams_i.hAVP_Orig_Host)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Host);
     }
     else
     {
    	 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	 oReturnVal = CxDx_Retval_Failure;
    	 goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_Orig_Realm)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Realm);
     }
     else
     {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	oReturnVal = CxDx_Retval_Failure;
     	goto ExitMe;
     }




     if(NULL != oParams_i.hAVP_UserData)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_UserData);
     }

     if(NULL != oParams_i.hAVP_UserChargingInfo)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_UserChargingInfo);
     }

     if(NULL != oParams_i.hAVP_UserName)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_UserName);
     }


     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext,hDiaMsgSAA_io,pAVPList);

     ExitMe:

  /*   if(CxDx_Retval_Success != oReturnVal)
     {
    	 DMBase_DestroyDiaMessage(hDiamStackContext, phDiaMsgSAA_io);
     }*/

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

    return oReturnVal;
}



t_CxDxAPIRetVal Cx_CreateCxLocationQueryResp(	HCXAPPCONTEXT		 		hCxAppContext_io,
												HDIAMETERMESSAGE 			hDiaMsgLIA_io,
												HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
												t_CxLocationQueryRespParams	oParams_i,
												unsigned int				unAppID_i,
												unsigned char				ucFlags_i,
												unsigned int				unHopByHopID_i,
												unsigned int				unEndToEndID_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;
    DMBase_SList*	pAVPList = NULL;
	t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    HAVP hAVP_VendorID = NULL;
    HAVP hAVP_Exp_Result = NULL;


     if(NULL != oParams_i.hAVP_Session_ID)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Session_ID);
     }
     else
     {
   	    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
   	    oReturnVal = CxDx_Retval_Failure;
    	goto ExitMe;
     }

     HAVP hVendorSpecificAppAVP = NULL;

     if(CxDx_Retval_Success != (oReturnVal = Cx_CreateVendorAppAVP(hCxAppContext_io, &hVendorSpecificAppAVP, hMsgAllocContext_io)))
     {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
     	oReturnVal = CxDx_Retval_Failure;
      	goto ExitMe;
     }

     DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hVendorSpecificAppAVP );


     if(NULL == oParams_i.hAVP_ResCode && NULL == oParams_i.hAVP_Exp_Result_Code ||
        NULL != oParams_i.hAVP_ResCode && NULL != oParams_i.hAVP_Exp_Result_Code)
     {
      	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
      	oReturnVal = CxDx_Retval_Failure;
       	goto ExitMe;
     }
     else if(NULL != oParams_i.hAVP_ResCode)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_ResCode);
     }
     else if(NULL != oParams_i.hAVP_Exp_Result_Code)
     {

        if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hAVP_Exp_Result,
																hMsgAllocContext_io,
																DMBASE_AVP_EXPERIMENTAL_RESULT, 0))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_VENDOR_ID,
																&hAVP_VendorID, CxDx_VENDOR_IDENTIFIER, hMsgAllocContext_io))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, hAVP_VendorID);
        hAVP_VendorID = NULL;

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, oParams_i.hAVP_Exp_Result_Code);
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hAVP_Exp_Result);
        hAVP_Exp_Result = NULL;
     }

     if(NULL != oParams_i.hAVP_Auth_Session_State)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Auth_Session_State);
     }
     else
     {
       	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
       	oReturnVal = CxDx_Retval_Failure;
        goto ExitMe;
     }


     if(NULL != oParams_i.hAVP_Orig_Host)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Host);
     }
     else
     {
    	 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	 oReturnVal = CxDx_Retval_Failure;
    	 goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_Orig_Realm)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Realm);
     }
     else
     {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	oReturnVal = CxDx_Retval_Failure;
     	goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_SrvCapab)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_SrvCapab);
     }

     if(NULL != oParams_i.hAVP_SrvName)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_SrvName);
     }

     if(NULL != oParams_i.hAVP_WildcardPSI)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_WildcardPSI);
     }

     if(NULL != oParams_i.hAVP_Wildcard_IMPU)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Wildcard_IMPU);
     }


     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext,hDiaMsgLIA_io,pAVPList);

     ExitMe:
/*
     if(CxDx_Retval_Success != oReturnVal)
     {
    	 DMBase_DestroyDiaMessage(hDiamStackContext, hDiaMsgLIA_io);
     }
*/
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

    return oReturnVal;
}



t_CxDxAPIRetVal Cx_CreateCxAVReqResp(	HCXAPPCONTEXT		 	hCxAppContext_io,
										HDIAMETERMESSAGE 		hDiaMsgMAA_io,
										HMSGALLOCATIONCONTEXT		hMsgAllocContext_io,
										t_CxAVReqRespParams		oParams_i,
										unsigned int			unAppID_i,
										unsigned char			ucFlags_i,
										unsigned int			unHopByHopID_i,
										unsigned int			unEndToEndID_i)
{
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    HDIAMETERSTACKCONTEXT hDiamStackContext = hCxAppContext_io->hDiamStackContext;
    DMBase_SList*	pAVPList = NULL;
	t_CxDxAPIRetVal oReturnVal = CxDx_Retval_Success;
    HAVP hAVP_VendorID = NULL;
    HAVP hAVP_Exp_Result = NULL;


     if(NULL != oParams_i.hAVP_Session_ID)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Session_ID);
     }
     else
     {
   	    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
   	    oReturnVal = CxDx_Retval_Failure;
    	goto ExitMe;
     }

     HAVP hVendorSpecificAppAVP = NULL;

     if(CxDx_Retval_Success != (oReturnVal = Cx_CreateVendorAppAVP(hCxAppContext_io, &hVendorSpecificAppAVP,hMsgAllocContext_io)))
     {
     	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
     	oReturnVal = CxDx_Retval_Failure;
      	goto ExitMe;
     }

     DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hVendorSpecificAppAVP );


     if(NULL == oParams_i.hAVP_ResCode && NULL == oParams_i.hAVP_Exp_Result_Code ||
        NULL != oParams_i.hAVP_ResCode && NULL != oParams_i.hAVP_Exp_Result_Code)
     {
      	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
      	oReturnVal = CxDx_Retval_Failure;
       	goto ExitMe;
     }
     else if(NULL != oParams_i.hAVP_ResCode)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_ResCode);
     }
     else if(NULL != oParams_i.hAVP_Exp_Result_Code)
     {

        if(DMBase_Retval_Success != DMBase_CreateGroupedAVP( hDiamStackContext, &hAVP_Exp_Result,
																hMsgAllocContext_io,
																DMBASE_AVP_EXPERIMENTAL_RESULT, 0))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        if(DMBase_Retval_Success != DMBase_CreateUnsigned32AVP(hDiamStackContext, DMBASE_AVP_VENDOR_ID,
																&hAVP_VendorID, CxDx_VENDOR_IDENTIFIER, hMsgAllocContext_io))
        {
         	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Failed to create AVP\n");
         	oReturnVal = CxDx_Retval_Failure;
          	goto ExitMe;
        }

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, hAVP_VendorID);
        hAVP_VendorID = NULL;

        DMBase_AddAVPToGroup(hDiamStackContext, hAVP_Exp_Result, oParams_i.hAVP_Exp_Result_Code);
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hAVP_Exp_Result);
        hAVP_Exp_Result = NULL;
     }

     if(NULL != oParams_i.hAVP_Auth_Session_State)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Auth_Session_State);
     }
     else
     {
       	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
       	oReturnVal = CxDx_Retval_Failure;
        goto ExitMe;
     }


     if(NULL != oParams_i.hAVP_Orig_Host)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Host);
     }
     else
     {
    	 DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	 oReturnVal = CxDx_Retval_Failure;
    	 goto ExitMe;
     }

     if(NULL != oParams_i.hAVP_Orig_Realm)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_Orig_Realm);
     }
     else
     {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Mandatory AVP missing\n");
    	oReturnVal = CxDx_Retval_Failure;
     	goto ExitMe;
     }


     if(NULL != oParams_i.hAVP_PublicID)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_PublicID);
     }

    if(NULL != oParams_i.hAVP_UserName)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_UserName);
     }

     if(NULL != oParams_i.hAVP_SIPNumAuthItems)
     {
        DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)oParams_i.hAVP_SIPNumAuthItems);
     }

     if(NULL != oParams_i.poSIPAuthDataItem)
     {
        DMBase_SList* poTempSIPAuthDataItems = oParams_i.poSIPAuthDataItem;
        HAVP hAVP_SIPAuthDataItem = NULL;
        for(; NULL != poTempSIPAuthDataItems; DMBase_SListGetNextNode(hDiamStackContext, poTempSIPAuthDataItems, &poTempSIPAuthDataItems))
        {
            DMBase_SListGetNodeData(hDiamStackContext, poTempSIPAuthDataItems, (void**)&hAVP_SIPAuthDataItem);
            DMBase_AppendSList(hDiamStackContext, &pAVPList, (void*)hAVP_SIPAuthDataItem);
        }

     }

     DMBase_AddAVPCollectionToDiameterMessage(hDiamStackContext,hDiaMsgMAA_io,pAVPList);

    ExitMe:
/*
    if(CxDx_Retval_Success != oReturnVal)
    {
    	DMBase_DestroyDiaMessage(hDiamStackContext, phDiaMsgMAA_io);
    }
*/
    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");

    return CxDx_Retval_Success;
}

