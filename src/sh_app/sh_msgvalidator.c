#include "sh_msgvalidator.h"
#include "sh_msg_api.h"
#include "dmbase_parser.h"
#include "sh_avps.h"
#include "sh_diamretcodes.h"

//externs
extern	int                     g_nDiamBaseTraceID ;


t_ShAppReturnVal	sh_app_ValidatePURMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i, 
										   HDIAMETERMESSAGE hPURMsg_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL == hPeer_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL \n");
            return Sh_Retval_NullPointer;
    }


	if(NULL == hPURMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPURMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	_Bool	bValidationFail = 0;
	_Bool	bUnableToComply = 0;

	t_DiamDictionary	oDiamDict = {0};
	DMBase_GetDictionaryObjectFromDiameterContext(hShAppContext_io->hDiamStackContext,&oDiamDict);


	//start validation as per PUR ABNF Grammar in 3GPP specs
	HDIAMETERMESSAGE	    hPUAMsg = NULL;
	t_ShUpdateRespParams	oShUpdateResp = {0};
	HAVP				    hAVPTemp1 = NULL;
	HAVP				    hAVPTemp2 = NULL;
	HAVP				    hFailedAVP = NULL;
	HMSGALLOCATIONCONTEXT	hPURMsgAllocCtxt = NULL;
	HMSGALLOCATIONCONTEXT	hPUAMsgAllocCtxt = NULL;

	DMBase_Get_AllocationContext_ForMessage(hShAppContext_io->hDiamStackContext,
											hPURMsg_i,&hPURMsgAllocCtxt);

	//now create PUA Diameter Message that would be needed to sent in case of error
	DMBase_CreateDiameterMessage(hShAppContext_io->hDiamStackContext, 
						&hPUAMsg, 1, SH_CMD_CODE_PUR, SH_DIAM_APP_ID,0,0,0);
	DMBase_SetDiaMsgRFlag(hShAppContext_io->hDiamStackContext,hPUAMsg,0);
	DMBase_Get_AllocationContext_ForMessage(hShAppContext_io->hDiamStackContext,hPUAMsg,&hPUAMsgAllocCtxt);



	DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hFailedAVP,
							hPUAMsgAllocCtxt,DMBASE_AVP_FAILED_AVP,0);

	//check if session ID is the first AVP or not , if not send unable to comply
	DMBase_SList*	pListTemp = hPURMsg_i->hCollectionOfAVPs;
	void*			pData = NULL;
	DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
							 pListTemp, &pData);

	hAVPTemp1 = (HAVP) pData;

	if (DMBASE_AVP_SESSION_ID != hAVPTemp1->oAVPHeaderInst.unAVPCode) 
	{
		//send unable to comply
		bUnableToComply = 1;
		bValidationFail = 1;
		goto ValidationFailCheck;
	}

	hAVPTemp1 = pData = NULL;


	//check sessionID
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_SESSION_ID,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempSessID[] = ";12345;21";
		
		void*			pTempDataForAVPs = NULL;
	
		//make SessionID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_SESSION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempSessID,&pTempDataForAVPs,strlen(pucTempSessID),hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_SESSION_ID,pTempDataForAVPs,strlen(pucTempSessID),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


    //check Vendor Specific Aplication ID
    DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		//this is a grouped AVP
		HAVP hAVPTemp3 = NULL;
		hAVPTemp2 = NULL;


		DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp3,hPUAMsgAllocCtxt,
								DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,0);
		

		unsigned int    unTemp = 11;
		
		void*			pTempDataForAVPs = NULL;

        //make vendor ID
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;
       

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 hAVPTemp3,hAVPTemp2);

        hAVPTemp2 = NULL;


        //make AuthApplicationID
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_APPLICATION_ID,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;
       

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 hAVPTemp3,hAVPTemp2);

        hAVPTemp2 = NULL;



		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp3);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}
    else
     {
        unsigned int    unVendorIdAVPCount = 0;
        unsigned int    unAuthApplicationIdAVPCount = 0;

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,
                                       DMBASE_AVP_VENDOR_ID,hAVPTemp1->hCollectionOfAVPs,
                                       &unVendorIdAVPCount);

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,
                                       DMBASE_AVP_AUTH_APPLICATION_ID,hAVPTemp1->hCollectionOfAVPs,
                                       &unAuthApplicationIdAVPCount);

        if (0 == unVendorIdAVPCount) 
        {
            unsigned int    unTemp = 11;
            unTemp = htonl(unTemp);

            void*			pTempDataForAVPs = NULL;

            //make vendor ID


            oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
                hShAppContext_io->hDiamStackContext,
                (void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt );

            DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
                             DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);


            pTempDataForAVPs = NULL;

            DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
            bValidationFail = 1;
            hAVPTemp1 = NULL;
            hAVPTemp2 = NULL;

        }//end if (0 == unVendorIdAVPCount) 

        if (0 == unAuthApplicationIdAVPCount) 
        {
            //make AuthApplicationID

            unsigned int    unTemp = 11;
            unTemp = htonl(unTemp);

            void*			pTempDataForAVPs = NULL;

            oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
               hShAppContext_io->hDiamStackContext,
                (void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt );

            DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,
                             hPUAMsgAllocCtxt,DMBASE_AVP_AUTH_APPLICATION_ID,
                             pTempDataForAVPs,sizeof(unTemp),0);


            pTempDataForAVPs = NULL;

            DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
            bValidationFail = 1;

            hAVPTemp1 = NULL;
            hAVPTemp2 = NULL;

        }//end if (0 == unAuthApplicationIdAVPCount) 
    }

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


	//check Auth-SessionState

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_AUTH_SESSION_STATE,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned int	unTemp = 1;
		
		void*			pTempDataForAVPs = NULL;
	
		//make AuthSessionState AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_SESSION_STATE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)&unTemp,&pTempDataForAVPs,sizeof( unTemp ),hPUAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_SESSION_STATE,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check OriginHost

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_HOST,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempOrigHost[] = "myhost";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_HOST].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempOrigHost,&pTempDataForAVPs,strlen(pucTempOrigHost) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_HOST,pTempDataForAVPs,strlen(pucTempOrigHost),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check OriginRealm

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_REALM,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempOrigRealm[] = "myrealm";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempOrigRealm,&pTempDataForAVPs,strlen(pucTempOrigRealm) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_REALM,pTempDataForAVPs,strlen(pucTempOrigRealm),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;
    _Bool bIsUserDataPresent = 0;

    //check User Data

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_USER_DATA,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempUserData[] = "myData";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_USER_DATA].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempUserData,&pTempDataForAVPs,strlen(pucTempUserData) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 SH_AVP_CODE_USER_DATA,pTempDataForAVPs,strlen(pucTempUserData),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;
        bIsUserDataPresent = 0;

	}
    else
    {
        bIsUserDataPresent = 1;
    }

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;



	//check DestinationRealm

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_DESTINATION_REALM,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempDestRealm[] = "myrealm";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_DESTINATION_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempDestRealm,&pTempDataForAVPs,strlen(pucTempDestRealm) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_DESTINATION_REALM,pTempDataForAVPs,strlen(pucTempDestRealm),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


    //check user Data

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_USER_DATA,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempUserData[] = "<Sh-Data>myDummyUSerData</Sh-Data>";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_USER_DATA].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempUserData,&pTempDataForAVPs,strlen(pucTempUserData) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 SH_AVP_CODE_USER_DATA,pTempDataForAVPs,strlen(pucTempUserData),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

    if (1 == bIsUserDataPresent)
    {
    
    
        //validate the XML recieved inside the UDR Data against the Sh XML Schema from 3GPP XSD File
        unsigned char   pucPathOfSh_XSDFile[2048] = {0};
    
        getcwd(pucPathOfSh_XSDFile,2048);
    
        strncat(pucPathOfSh_XSDFile,"/XMLSchemas/",2048);
        strncat(pucPathOfSh_XSDFile,SH_XSD_VALIDATION_FILENAME,2048);
        
    
        t_DiamOctetString*  poUserDataFromAVP = NULL;
        poUserDataFromAVP = (t_DiamOctetString*) hAVPTemp1->pData;
    
        t_DappCommonReturnVal   oDappCommRetVal = DappCommon_Retval_Failure;
    
        oDappCommRetVal = DappCommon_ValidateBufferXMLFromXSD(poUserDataFromAVP->pucOctetString,
              poUserDataFromAVP->unNumOfOctets , pucPathOfSh_XSDFile);
    
    
        if (DappCommon_Retval_XML_Valid == oDappCommRetVal) 
        {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_DEBUG," XML Sent in PUR Is Valid  \n");

        }
        else if (DappCommon_Retval_XML_NotValid == oDappCommRetVal) 
        {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," XML Sent in PUR Is NOT Valid as per schema!!  \n");
            //send unable to comply
			bUnableToComply = 1;
			bValidationFail = 1;
			goto ValidationFailCheck;
		}
        else
        {
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DappCommon_ValidateBufferXMLFromXSD FAILED  \n");

        }
    }//else if bIsUserDataPresent ==1
    


	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_DESTINATION_REALM,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempDestRealm[] = "myrealm";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_DESTINATION_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempDestRealm,&pTempDataForAVPs,strlen(pucTempDestRealm) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 DMBASE_AVP_DESTINATION_REALM,pTempDataForAVPs,strlen(pucTempDestRealm),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check user identity

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_USER_IDENTITY,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		//this is a grouped AVP
		HAVP hAVPTemp3 = NULL;
		hAVPTemp2 = NULL;


		DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp3,
								hPUAMsgAllocCtxt,SH_AVP_CODE_USER_IDENTITY,0);
		

		unsigned char	pucTempPubID[] = "sip:me@me.com";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_PUBLIC_IDENTITY].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempPubID,&pTempDataForAVPs,strlen(pucTempPubID) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 SH_AVP_CODE_PUBLIC_IDENTITY,pTempDataForAVPs,strlen(pucTempPubID),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 hAVPTemp3,hAVPTemp2);


		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp3);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}
    else
    {
        //check if public iodentity is present inside the user identity

        unsigned int    unCountOfPublicIDentityAVPs =0;

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,SH_AVP_CODE_PUBLIC_IDENTITY,
                                  hAVPTemp1->hCollectionOfAVPs   , &unCountOfPublicIDentityAVPs);
    
        if (0 == unCountOfPublicIDentityAVPs) 
         {
    		
    		HAVP hAVPTemp3 = NULL;
    		hAVPTemp2 = NULL;
    
    		unsigned char	pucTempPubID[] = "sip:me@me.com";
    		
    		void*			pTempDataForAVPs = NULL;
    	
    		
    		oDiamDict.poDictionaryArray[SH_AVP_CODE_PUBLIC_IDENTITY].oAVPDictionaryFunctionPtrs.pfGetStructData(
    			hShAppContext_io->hDiamStackContext,(void*)pucTempPubID,&pTempDataForAVPs,strlen(pucTempPubID) ,hPUAMsgAllocCtxt);
    	
    		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
    						 SH_AVP_CODE_PUBLIC_IDENTITY,pTempDataForAVPs,strlen(pucTempPubID),0);
    	
    	
    		pTempDataForAVPs = NULL;
    
    		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
    							 hFailedAVP,hAVPTemp2);
    
    
    		hAVPTemp1 = NULL;
    		hAVPTemp2 = NULL;
    		bValidationFail = 1;
    	}//end if (0 == unCountOfPublicIDentityAVPs) 
    }

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


	//check Data Reference

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_DATA_REFERENCE,
													hPURMsg_i, 
                                                    &hAVPTemp1);

	unsigned int unDataRef = 0;

	

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned int	unTemp = 12;
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_DATA_REFERENCE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp) ,hPUAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hPUAMsgAllocCtxt,
						 SH_AVP_CODE_DATA_REFERENCE,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}
	else
	{
		t_DiamUnsigned32*	pTempDiam32 = (t_DiamUnsigned32*) hAVPTemp1->pData;

		unDataRef = *(pTempDiam32->punUnsigned32);
	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	
	ValidationFailCheck:
	if (1 == bValidationFail) 
	{
		//build an error PUA message

		unsigned char	pucTempOrigHost[] = "myhost";
		unsigned char	pucTempOrigRealm[] = "myrealm";
		unsigned char	pucTempSessId[] = "mysessID";
		void*			pTempDataForAVPs = NULL;

		//make Origin Host AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_HOST].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempOrigHost,&pTempDataForAVPs,strlen(pucTempOrigHost),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShUpdateResp.hAVPOriginHost),
						 hPUAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_HOST,pTempDataForAVPs,strlen(pucTempOrigHost),0);


		pTempDataForAVPs = NULL;


		// make Origin Realm AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempOrigRealm,&pTempDataForAVPs,strlen(pucTempOrigRealm),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShUpdateResp.hAVPOriginRealm),
						 hPUAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_REALM,pTempDataForAVPs,strlen(pucTempOrigRealm),0);


		pTempDataForAVPs = NULL;


		// make SessionId AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_SESSION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempSessId,&pTempDataForAVPs,strlen(pucTempSessId),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShUpdateResp.hAVPSessionID),
						 hPUAMsgAllocCtxt,
						 DMBASE_AVP_SESSION_ID,pTempDataForAVPs,strlen(pucTempSessId),0);


		pTempDataForAVPs = NULL;


		// make AuthSessionState
		unsigned int unTemp = 1;
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_SESSION_STATE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShUpdateResp.hAVPAuthSessionState),
						 hPUAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_SESSION_STATE,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;


		//make vendor specific application ID
		HAVP	hAVPTempVendorID = NULL;
		HAVP	hAVPTempAuthAppID = NULL;
		
		//make VendorID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTempVendorID,hPUAMsgAllocCtxt,
						 DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;
		

		//make AuthAppID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTempAuthAppID,hPUAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_APPLICATION_ID,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;
		


		//make grouped AVP Vendor Specific ApplicationID from the above two
		DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,
								&(oShUpdateResp.hAVPVendorSpecificAppID),
								hPUAMsgAllocCtxt,
								DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,0);

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 oShUpdateResp.hAVPVendorSpecificAppID,
							 hAVPTempAuthAppID);

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 oShUpdateResp.hAVPVendorSpecificAppID,
							 hAVPTempVendorID);


		//make resultCode AVP
		if (1==bUnableToComply) 
		{
			unTemp = DIAMETER_UNABLE_TO_COMPLY;
		}
		else
		{
			unTemp = DIAMETER_MISSING_AVP;
		}
		
		unTemp = htonl(unTemp);
	
		oDiamDict.poDictionaryArray[DMBASE_AVP_RESULT_CODE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hPUAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShUpdateResp.hAVPResultCode),
						 hPUAMsgAllocCtxt,
						 DMBASE_AVP_RESULT_CODE,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;

		//add the Failed AVP made so far to the list 
        if (0 == bUnableToComply) 
        {
            
    		DMBase_AppendSList( hShAppContext_io->hDiamStackContext,
    							&(oShUpdateResp.pListOfFailedAVPs),
    							 (void* )hFailedAVP );
        }

		Sh_CreateShUpdateResponse(hShAppContext_io->hDiamStackContext,&hPUAMsg,oShUpdateResp);

		DMBase_SendMessageToPeer(hShAppContext_io->hDiamStackContext,hPeer_i,hPUAMsg);
        DMBase_DestroyDiaMessage(hShAppContext_io->hDiamStackContext,&hPUAMsg);

		return Sh_Retval_Failure;



	}//end if (1 == bValidationFail) 

    DMBase_DestroyDiaMessage(hShAppContext_io->hDiamStackContext,&hPUAMsg);
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");


	return Sh_Retval_Success;
}

t_ShAppReturnVal	sh_app_ValidateUDRMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i, 
										   HDIAMETERMESSAGE hUDRMsg_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL == hPeer_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL \n");
            return Sh_Retval_NullPointer;
    }


	if(NULL == hUDRMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hUDRMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	_Bool	bValidationFail = 0;
	_Bool	bUnableToComply = 0;

	t_DiamDictionary	oDiamDict = {0};
	DMBase_GetDictionaryObjectFromDiameterContext(hShAppContext_io->hDiamStackContext,&oDiamDict);


	//start validation as per UDR ABNF Grammar in 3GPP specs
	HDIAMETERMESSAGE		hUDAMsg = NULL;
	t_ShPullRespParams		oShPullResp = {0};
	HAVP					hAVPTemp1 = NULL;
	HAVP					hAVPTemp2 = NULL;
	HAVP					hFailedAVP = NULL;
	HMSGALLOCATIONCONTEXT	hUDRMsgAllocCtxt = NULL;
	HMSGALLOCATIONCONTEXT	hUDAMsgAllocCtxt = NULL;
	void*					pData = NULL;

	DMBase_Get_AllocationContext_ForMessage(hShAppContext_io->hDiamStackContext,hUDRMsg_i,&hUDRMsgAllocCtxt);

	//now create UDA Diameter Message that would be needed to sent in case of error
	DMBase_CreateDiameterMessage(hShAppContext_io->hDiamStackContext, 
						&hUDAMsg, 1, SH_CMD_CODE_UDR, SH_DIAM_APP_ID,0,0,0);
	DMBase_SetDiaMsgRFlag(hShAppContext_io->hDiamStackContext,hUDAMsg,0);
	DMBase_Get_AllocationContext_ForMessage(hShAppContext_io->hDiamStackContext,hUDAMsg,&hUDAMsgAllocCtxt);



	DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hFailedAVP,
							hUDAMsgAllocCtxt,DMBASE_AVP_FAILED_AVP,0);

	//check if session ID is the first AVP or not , if not send unable to comply
	DMBase_SList*	pListTemp = hUDRMsg_i->hCollectionOfAVPs;
	DMBase_SListGetNodeData(hShAppContext_io->hDiamStackContext,
							 pListTemp, &pData);

	hAVPTemp1 = (HAVP) pData;

	if (DMBASE_AVP_SESSION_ID != hAVPTemp1->oAVPHeaderInst.unAVPCode) 
	{
		//send unable to comply
		bUnableToComply = 1;
		bValidationFail = 1;
		goto ValidationFailCheck;
	}

	hAVPTemp1 = pData = NULL;



	//check sessionID
	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_SESSION_ID,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempSessID[] = ";12345;21";
		
		void*			pTempDataForAVPs = NULL;
	
		//make SessionID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_SESSION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,
			(void*)pucTempSessID,&pTempDataForAVPs,strlen(pucTempSessID),hUDAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,
						 hUDAMsgAllocCtxt,DMBASE_AVP_SESSION_ID,
						 pTempDataForAVPs,strlen(pucTempSessID),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

    //check Vendor Specific Aplication ID
    DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
                                                    DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,
                                                    hUDRMsg_i, 
                                                    &hAVPTemp1);

    if (NULL == hAVPTemp1) 
    {

        //this is a grouped AVP
        HAVP hAVPTemp3 = NULL;
        hAVPTemp2 = NULL;


        DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp3,
								hUDAMsgAllocCtxt,
                                DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,0);


        unsigned int    unTemp = 11;
        unTemp = htonl(unTemp);

        void*			pTempDataForAVPs = NULL;

        //make vendor ID


        oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
            hShAppContext_io->hDiamStackContext,
			(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt );

        DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
                         DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);


        pTempDataForAVPs = NULL;


        DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
                             hAVPTemp3,hAVPTemp2);

        hAVPTemp2 = NULL;


        //make AuthApplicationID


        oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
           hShAppContext_io->hDiamStackContext,
		    (void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt );

        DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,
                         hUDAMsgAllocCtxt,DMBASE_AVP_AUTH_APPLICATION_ID,
						 pTempDataForAVPs,sizeof(unTemp),0);


        pTempDataForAVPs = NULL;


        DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
                             hAVPTemp3,hAVPTemp2);

        hAVPTemp2 = NULL;



        DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp3);



        hAVPTemp1 = NULL;
        hAVPTemp2 = NULL;

        bValidationFail = 1;

    }
    else
    {
        unsigned int    unVendorIdAVPCount = 0;
        unsigned int    unAuthApplicationIdAVPCount = 0;

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,
                                       DMBASE_AVP_VENDOR_ID,hAVPTemp1->hCollectionOfAVPs,
                                       &unVendorIdAVPCount);

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,
                                       DMBASE_AVP_AUTH_APPLICATION_ID,hAVPTemp1->hCollectionOfAVPs,
                                       &unAuthApplicationIdAVPCount);

        if (0 == unVendorIdAVPCount) 
        {
            unsigned int    unTemp = 11;
            unTemp = htonl(unTemp);

            void*			pTempDataForAVPs = NULL;

            //make vendor ID


            oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
                hShAppContext_io->hDiamStackContext,
                (void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt );

            DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
                             DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);


            pTempDataForAVPs = NULL;

            DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
            bValidationFail = 1;
            hAVPTemp1 = NULL;
            hAVPTemp2 = NULL;

        }//end if (0 == unVendorIdAVPCount) 

        if (0 == unAuthApplicationIdAVPCount) 
        {
            //make AuthApplicationID

            unsigned int    unTemp = 11;
            unTemp = htonl(unTemp);

            void*			pTempDataForAVPs = NULL;

            oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
               hShAppContext_io->hDiamStackContext,
                (void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt );

            DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,
                             hUDAMsgAllocCtxt,DMBASE_AVP_AUTH_APPLICATION_ID,
                             pTempDataForAVPs,sizeof(unTemp),0);


            pTempDataForAVPs = NULL;

            DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
            bValidationFail = 1;

            hAVPTemp1 = NULL;
            hAVPTemp2 = NULL;

        }//end if (0 == unAuthApplicationIdAVPCount) 
    }


    hAVPTemp1 = NULL;
    hAVPTemp2 = NULL;


	//check Auth-SessionState

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_AUTH_SESSION_STATE,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if  ( NULL == hAVPTemp1) 
		 
	{

		hAVPTemp2 = NULL;

		unsigned int	unTemp = 1;
		
		void*			pTempDataForAVPs = NULL;
	
		//make SessionID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_SESSION_STATE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof( unTemp ),hUDAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_SESSION_STATE,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check OriginHost

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_HOST,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempOrigHost[] = "myhost";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_HOST].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempOrigHost,&pTempDataForAVPs,strlen(pucTempOrigHost),hUDAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_HOST,pTempDataForAVPs,strlen(pucTempOrigHost),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check OriginRealm

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_ORIGIN_REALM,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempOrigRealm[] = "myrealm";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempOrigRealm,&pTempDataForAVPs,strlen(pucTempOrigRealm),hUDAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_REALM,pTempDataForAVPs,strlen(pucTempOrigRealm),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


	//check DestinationRealm

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													DMBASE_AVP_DESTINATION_REALM,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned char	pucTempDestRealm[] = "myrealm";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[DMBASE_AVP_DESTINATION_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempDestRealm,&pTempDataForAVPs,strlen(pucTempDestRealm),hUDAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 DMBASE_AVP_DESTINATION_REALM,pTempDataForAVPs,strlen(pucTempDestRealm),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	//check user identity

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_USER_IDENTITY,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	if (NULL == hAVPTemp1) 
	{

		//this is a grouped AVP
		HAVP hAVPTemp3 = NULL;
		hAVPTemp2 = NULL;


		DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp3,hUDAMsgAllocCtxt,
								SH_AVP_CODE_USER_IDENTITY,0);
		

		unsigned char	pucTempPubID[] = "sip:me@me.com";
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_PUBLIC_IDENTITY].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempPubID,&pTempDataForAVPs,strlen(pucTempPubID) ,hUDAMsgAllocCtxt);
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 SH_AVP_CODE_PUBLIC_IDENTITY,pTempDataForAVPs,strlen(pucTempPubID),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 hAVPTemp3,hAVPTemp2);


		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp3);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}
    else
    {
        //check if public iodentity is present inside the user identity

        unsigned int    unCountOfPublicIDentityAVPs =0;

        DMBase_GetAVPCountInCollection(hShAppContext_io->hDiamStackContext,SH_AVP_CODE_PUBLIC_IDENTITY,
                                  hAVPTemp1->hCollectionOfAVPs   , &unCountOfPublicIDentityAVPs);
    
        if (0 == unCountOfPublicIDentityAVPs) 
         {
    		
    		HAVP hAVPTemp3 = NULL;
    		hAVPTemp2 = NULL;
    
    		unsigned char	pucTempPubID[] = "sip:me@me.com";
    		
    		void*			pTempDataForAVPs = NULL;
    	
    		
    		oDiamDict.poDictionaryArray[SH_AVP_CODE_PUBLIC_IDENTITY].oAVPDictionaryFunctionPtrs.pfGetStructData(
    			hShAppContext_io->hDiamStackContext,(void*)pucTempPubID,&pTempDataForAVPs,strlen(pucTempPubID) ,hUDAMsgAllocCtxt);
    	
    		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
    						 SH_AVP_CODE_PUBLIC_IDENTITY,pTempDataForAVPs,strlen(pucTempPubID),0);
    	
    	
    		pTempDataForAVPs = NULL;
    
    		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
    							 hFailedAVP,hAVPTemp2);
    
    
    		hAVPTemp1 = NULL;
    		hAVPTemp2 = NULL;
    		bValidationFail = 1;
    	}//end if (0 == unCountOfPublicIDentityAVPs) 
    }

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;


	//check Data Reference

	DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
													SH_AVP_CODE_DATA_REFERENCE,
													hUDRMsg_i, 
                                                    &hAVPTemp1);

	unsigned int unDataRef = 0;

	

	if (NULL == hAVPTemp1) 
	{

		hAVPTemp2 = NULL;

		unsigned int	unTemp = 12;
		
		void*			pTempDataForAVPs = NULL;
	
		
		oDiamDict.poDictionaryArray[SH_AVP_CODE_DATA_REFERENCE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt );
	
		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
						 SH_AVP_CODE_DATA_REFERENCE,pTempDataForAVPs,sizeof(unTemp),0);
	
	
		pTempDataForAVPs = NULL;

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);



		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		bValidationFail = 1;

	}
	else
	{
		t_DiamUnsigned32*	pTempDiam32 = (t_DiamUnsigned32*) hAVPTemp1->pData;

		unDataRef = *(pTempDiam32->punUnsigned32);
	}

	hAVPTemp1 = NULL;
	hAVPTemp2 = NULL;

	switch (unDataRef) 
	{
	case SH_DATA_REF_AVP_VAL_ALIASESREPOSITORYDATA:
	case SH_DATA_REF_AVP_VAL_REPOSITORYDATA: //intentional fallthrough


		//get service indication
		DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
														SH_AVP_CODE_SERVICE_INDICATION,
														hUDRMsg_i, 
														&hAVPTemp1);
	
		if (NULL == hAVPTemp1) 
		{
	
			hAVPTemp2 = NULL;
	
			unsigned char	pucTempSerInd[] = "myindication";
			
			void*			pTempDataForAVPs = NULL;
		
			
			oDiamDict.poDictionaryArray[SH_AVP_CODE_SERVICE_INDICATION].oAVPDictionaryFunctionPtrs.pfGetStructData(
				hShAppContext_io->hDiamStackContext,
				(void*)pucTempSerInd,&pTempDataForAVPs,strlen(pucTempSerInd),hUDAMsgAllocCtxt );
		
			DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
							 SH_AVP_CODE_SERVICE_INDICATION,pTempDataForAVPs,strlen(pucTempSerInd)
							 ,0);
		
		
			pTempDataForAVPs = NULL;
	
			DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
	
	
	
			hAVPTemp1 = NULL;
			hAVPTemp2 = NULL;
	
			bValidationFail = 1;
	
		}

		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;


		break;

	

	case SH_DATA_REF_AVP_VAL_INITIALFILTERCRITERIA:

		//get server name
		DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
														SH_AVP_CODE_SERVER_NAME,
														hUDRMsg_i, 
														&hAVPTemp1);
	
		if (NULL == hAVPTemp1) 
		{
	
			hAVPTemp2 = NULL;
	
			unsigned char	pucTempServerName[] = "myserver";
			
			void*			pTempDataForAVPs = NULL;
		
			
			oDiamDict.poDictionaryArray[SH_AVP_CODE_SERVER_NAME].oAVPDictionaryFunctionPtrs.pfGetStructData(
				hShAppContext_io->hDiamStackContext,(void*)pucTempServerName,&pTempDataForAVPs,strlen(pucTempServerName),hUDAMsgAllocCtxt );
		
			DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
							 SH_AVP_CODE_SERVER_NAME,pTempDataForAVPs,strlen(pucTempServerName),0);
		
		
			pTempDataForAVPs = NULL;
	
			DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
	
	
	
			hAVPTemp1 = NULL;
			hAVPTemp2 = NULL;
	
			bValidationFail = 1;
	
		}

		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;



		break;


	case SH_DATA_REF_AVP_VAL_DSAI:


		//get server name
		DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
														SH_AVP_CODE_SERVER_NAME,
														hUDRMsg_i, 
														&hAVPTemp1);
	
		if (NULL == hAVPTemp1) 
		{
	
			hAVPTemp2 = NULL;
	
			unsigned char	pucTempServerName[] = "myserver";
			
			void*			pTempDataForAVPs = NULL;
		
			
			oDiamDict.poDictionaryArray[SH_AVP_CODE_SERVER_NAME].oAVPDictionaryFunctionPtrs.pfGetStructData(
				hShAppContext_io->hDiamStackContext,(void*)pucTempServerName,&pTempDataForAVPs,strlen(pucTempServerName),hUDAMsgAllocCtxt );
		
			DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
							 SH_AVP_CODE_SERVER_NAME,pTempDataForAVPs,strlen(pucTempServerName),0);
		
		
			pTempDataForAVPs = NULL;
	
			DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
	
	
	
			hAVPTemp1 = NULL;
			hAVPTemp2 = NULL;
	
			bValidationFail = 1;
	
		}

		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		//get DSAI-TAG
		DMBase_GetAVP(hShAppContext_io->hDiamStackContext,
														SH_AVP_CODE_DSAI_TAG,
														hUDRMsg_i, 
														&hAVPTemp1);
	
		if (NULL == hAVPTemp1) 
		{
	
			hAVPTemp2 = NULL;
	
			unsigned char	pucTempDSAITag[] = "myDSAITag";
			
			void*			pTempDataForAVPs = NULL;
		
			
			oDiamDict.poDictionaryArray[SH_AVP_CODE_DSAI_TAG].oAVPDictionaryFunctionPtrs.pfGetStructData(
				hShAppContext_io->hDiamStackContext,(void*)pucTempDSAITag,&pTempDataForAVPs,strlen(pucTempDSAITag),hUDAMsgAllocCtxt );
		
			DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTemp2,hUDAMsgAllocCtxt,
							 SH_AVP_CODE_DSAI_TAG,pTempDataForAVPs,strlen(pucTempDSAITag),0);
		
		
			pTempDataForAVPs = NULL;
	
			DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,hFailedAVP,hAVPTemp2);
	
	
	
			hAVPTemp1 = NULL;
			hAVPTemp2 = NULL;
	
			bValidationFail = 1;
	
		}

		hAVPTemp1 = NULL;
		hAVPTemp2 = NULL;

		break;
	}//end switch(unDataRef)

	ValidationFailCheck:


	if (1 == bValidationFail) 
	{
		//build an error UDA message

		unsigned char	pucTempOrigHost[] = "myhost";
		unsigned char	pucTempOrigRealm[] = "myrealm";
		unsigned char	pucTempSessId[] = "mysessID";
		void*			pTempDataForAVPs = NULL;

		//make Origin Host AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_HOST].oAVPDictionaryFunctionPtrs.pfGetStructData(
		hShAppContext_io->hDiamStackContext,(void*)pucTempOrigHost,&pTempDataForAVPs,strlen(pucTempOrigHost),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPOriginHost),hUDAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_HOST,pTempDataForAVPs,strlen(pucTempOrigHost),0);


		pTempDataForAVPs = NULL;


		// make Origin Realm AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_ORIGIN_REALM].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempOrigRealm,&pTempDataForAVPs,strlen(pucTempOrigRealm),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPOriginRealm),hUDAMsgAllocCtxt,
						 DMBASE_AVP_ORIGIN_REALM,pTempDataForAVPs,strlen(pucTempOrigRealm),0);


		pTempDataForAVPs = NULL;


		// make SessionId AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_SESSION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)pucTempSessId,&pTempDataForAVPs,strlen(pucTempSessId),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPSessionID),hUDAMsgAllocCtxt,
						 DMBASE_AVP_SESSION_ID,pTempDataForAVPs,strlen(pucTempSessId),0);


		pTempDataForAVPs = NULL;


		// make AuthSessionState
		unsigned int unTemp = 1;
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_SESSION_STATE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPAuthSessionState),
						 hUDAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_SESSION_STATE,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;


		//make vendor specific application ID
		HAVP	hAVPTempVendorID = NULL;
		HAVP	hAVPTempAuthAppID = NULL;
		
		//make VendorID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_VENDOR_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTempVendorID,hUDAMsgAllocCtxt,
						 DMBASE_AVP_VENDOR_ID,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;
		

		//make AuthAppID AVP
		oDiamDict.poDictionaryArray[DMBASE_AVP_AUTH_APPLICATION_ID].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&hAVPTempAuthAppID,hUDAMsgAllocCtxt,
						 DMBASE_AVP_AUTH_APPLICATION_ID,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;
		


		//make grouped AVP Vendor Specific ApplicationID from the above two
		DMBase_CreateGroupedAVP(hShAppContext_io->hDiamStackContext,
								&(oShPullResp.hAVPVendorSpecificAppID),
								hUDAMsgAllocCtxt,
								DMBASE_AVP_VENDOR_SPECIFIC_APPLICATION_ID,0);

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 oShPullResp.hAVPVendorSpecificAppID,
							 hAVPTempAuthAppID);

		DMBase_AddAVPToGroup(hShAppContext_io->hDiamStackContext,
							 oShPullResp.hAVPVendorSpecificAppID,
							 hAVPTempVendorID);


		//make resultCode AVP
		if (1 == bUnableToComply ) 
		{
			unTemp = DIAMETER_UNABLE_TO_COMPLY;
		}
		else
		{
			unTemp = DIAMETER_MISSING_AVP;
		}
		
		unTemp = htonl(unTemp);
	
		oDiamDict.poDictionaryArray[DMBASE_AVP_RESULT_CODE].oAVPDictionaryFunctionPtrs.pfGetStructData(
			hShAppContext_io->hDiamStackContext,(void*)&unTemp,&pTempDataForAVPs,sizeof(unTemp),hUDAMsgAllocCtxt);

		DMBase_CreateAVP(hShAppContext_io->hDiamStackContext,&(oShPullResp.hAVPResultCode),
						 hUDAMsgAllocCtxt,
						 DMBASE_AVP_RESULT_CODE,pTempDataForAVPs,sizeof(unTemp),0);


		pTempDataForAVPs = NULL;

		//add the Failed AVP made so far to the list 
        if ( 0  == bUnableToComply) 
        {
            DMBase_AppendSList( hShAppContext_io->hDiamStackContext,
                                &(oShPullResp.pListOfFailedAVPs),
                                 (void* )hFailedAVP );
        }
		
		Sh_CreateShPullResponse(hShAppContext_io->hDiamStackContext,&hUDAMsg,oShPullResp);

		DMBase_SendMessageToPeer(hShAppContext_io->hDiamStackContext,hPeer_i,hUDAMsg);

        DMBase_DestroyDiaMessage(hShAppContext_io->hDiamStackContext,&hUDAMsg);

		return Sh_Retval_Failure;



	}//end if (1 == bValidationFail) 

    DMBase_DestroyDiaMessage(hShAppContext_io->hDiamStackContext,&hUDAMsg);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return Sh_Retval_Success;
}

t_ShAppReturnVal	sh_app_ValidateMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i, 
										   HDIAMETERMESSAGE hDiamMsg_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

	if(NULL == hPeer_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hPeer_i is NULL \n");
            return Sh_Retval_NullPointer;
    }


	if(NULL == hDiamMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiamMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	
    unsigned int unCommandCode = 0;
	DMBase_GetDiaMsgCmdCodeFromDiamMsg(hShAppContext_io->hDiamStackContext,hDiamMsg_i,&unCommandCode);

	switch(unCommandCode)
	{
	
	case	SH_CMD_CODE_UDR:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Received UDR message for validation \n");

		(hShAppContext_io->oShPerfMgmtData.unAttemptedUDRDTR)++;

		if (Sh_Retval_Success !=  sh_app_ValidateUDRMessage(hShAppContext_io,hPeer_i,hDiamMsg_i))
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_ValidateUDRMessage Failed \n");
			(hShAppContext_io->oShPerfMgmtData.unFailedUDADTR)++;
			if (hShAppContext_io->oShPerfMgmtData.unFailedUDADTR >=
				hShAppContext_io->oShFaultMgmtData.unFailedThresholdUDR) 
			{
				//send trap event to oamps module
				t_hssOAMPSTrapEventData oTrapEvData = {0};
				oTrapEvData.oFaultMgmtOid = hssFMOid_failedThresholdUDR;
				oTrapEvData.lValue = hShAppContext_io->oShPerfMgmtData.unFailedUDADTR;
	
				if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(hShAppContext_io->hDiamStackContext->hFrameworkHandle,
										  OAMPS_SEND_TRAP_EVENT,
										  &oTrapEvData,sizeof(oTrapEvData) )
					   )
				{
					DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_SEND_TRAP_EVENT ,Leaving \n");
					return ;
				}
			}
			return Sh_Retval_Failure;
		}

		break;


	case 	SH_CMD_CODE_PUR:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Received   PUR message for validation \n");

		(hShAppContext_io->oShPerfMgmtData.unAttemptedPURDTU)++;


        if (Sh_Retval_Success !=  sh_app_ValidatePURMessage(hShAppContext_io,hPeer_i,hDiamMsg_i))
        {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," sh_app_ValidatePURMessage Failed \n");
			(hShAppContext_io->oShPerfMgmtData.unFailedPUADTU)++;
			if (hShAppContext_io->oShPerfMgmtData.unFailedPUADTU >=
				hShAppContext_io->oShFaultMgmtData.unFailedThresholdPUR) 
			{
				//send trap event to oamps module
				t_hssOAMPSTrapEventData oTrapEvData = {0};
				oTrapEvData.oFaultMgmtOid = hssFMOid_failedThresholdPUR;
				oTrapEvData.lValue = hShAppContext_io->oShPerfMgmtData.unFailedPUADTU;
	
				if ( AMPS_SUCCESS != AMPS_EvtSysSendExtEvt(hShAppContext_io->hDiamStackContext->hFrameworkHandle,
										  OAMPS_SEND_TRAP_EVENT,
										  &oTrapEvData,sizeof(oTrapEvData) )
					   )
				{
					DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," AMPS_EvtSysSendExtEvt Failed for OAMPS_SEND_TRAP_EVENT ,Leaving \n");
					return ;
				}
			}
            return Sh_Retval_Failure;
        }


		break;

	case	SH_CMD_CODE_SNR:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Received unsupported SNR message for validation \n");
		return Sh_Retval_Failure;

		break;


	case	SH_CMD_CODE_PNR:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Received unsupported PNR message for validation \n");
		return Sh_Retval_Failure;

		break;
			
	default:
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," Received unrecognized message for validation \n");
		return Sh_Retval_Failure;
			
			break;
	}



	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return Sh_Retval_Success;
}



t_ShAppReturnVal	sh_app_IsThisAnShMessage(HSHAPPCONTEXT	hShAppContext_io,
											 unsigned int	unDiamMsgCode_i,
											 _Bool*	pbIsShMsg_i)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

    if(NULL == hShAppContext_io)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hShAppContext_io is NULL \n");
            return Sh_Retval_NullPointer;
    }

    if(NULL == pbIsShMsg_i)
    {
            DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pbIsShMsg_i is NULL \n");
            return Sh_Retval_NullPointer;
    }

	

	switch( unDiamMsgCode_i)
	{

		//intentional fallthrough
		case	SH_CMD_CODE_UDR:
		case 	SH_CMD_CODE_PUR:
		case	SH_CMD_CODE_SNR:
		case	SH_CMD_CODE_PNA:
			*pbIsShMsg_i = 1;
			break;
		
	
		default:
			*pbIsShMsg_i = 0;
			break;
	}


	return Sh_Retval_Success;


	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");



}





