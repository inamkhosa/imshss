
#include "db_cxdx_sar.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "events.h"
#include "cxdx_db.h"
#include "hss.h"

extern int g_nDiamBaseTraceID ;

t_Db_HSSReturnVal	Cx_ProcessSARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxSARDBRequest* 	poSARDBRequest_i)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hdbHSSThreadCtxt_io || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext
                               || NULL == poSARDBRequest_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return Db_HSS_Retval_NullPointer;
    }

    HDBHSSCONTEXT hDBHSSContext_i = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    t_CxSARDBResponse oSARDBResponse;
    DMBase_MemsetZero(&oSARDBResponse, sizeof(t_CxSARDBResponse));

    strcpy(oSARDBResponse.ucSessionID, poSARDBRequest_i->ucSessionID);

    HDIAMETERSTACKCONTEXT hDiamStackContext = hDBHSSContext_i->hDiamStackContext;
    unsigned int unDBResult = DB_RESULT_SUCCESS;
    int nIMPUCount = -1;
    t_IMPI oIMPI;
    t_IMPU oIMPU;

    DMBase_MemsetZero(&oIMPI,sizeof(t_IMPI));
    DMBase_MemsetZero(&oIMPU,sizeof(t_IMPU));

    DMBase_SList* poPUIDList = NULL;
    DMBase_SList* poTempPUIDList = NULL;

    unsigned int unIndex;
    unsigned char* pucPublicUserId;
    for(unIndex = 0; unIndex< poSARDBRequest_i->unPublicUserIdCount; unIndex++)
    {
    	pucPublicUserId = NULL;
    	DMBase_Malloc(hDBHSSContext_i->hDiamStackContext, (void**)&pucPublicUserId,
    			strlen(poSARDBRequest_i->ucPublicIdentity[unIndex])+1);
    	strcpy(pucPublicUserId, poSARDBRequest_i->ucPublicIdentity[unIndex]);
    	DMBase_AppendSList(hDBHSSContext_i->hDiamStackContext, &poPUIDList, pucPublicUserId);
    }

    unsigned int unServerAssignmentType = poSARDBRequest_i->unServerAssignmentType;

    if(poSARDBRequest_i->unPublicUserIdCount > 1 && (SA_NO_ASSIGNMENT == unServerAssignmentType ||
       SA_REGISTRATION == unServerAssignmentType || SA_RE_REGISTRATION == unServerAssignmentType ||
       SA_UNREGISTERED_USER  == unServerAssignmentType || SA_AUTHENTICATION_FAILURE == unServerAssignmentType ||
       SA_AUTHENTICATION_TIMEOUT  == unServerAssignmentType))
    {
        unDBResult = DB_RESULT_TOO_MUCH_DATA;
    }

    if(DB_RESULT_SUCCESS == unDBResult && 0 < poSARDBRequest_i->unPublicUserIdCount)
    {
        unDBResult = CX_GetRecord_By_IMPU(hdbHSSThreadCtxt_io, &oIMPU, poSARDBRequest_i->ucPublicIdentity[0]);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PUBLIC_ID;
        }
    }

    if(DB_RESULT_SUCCESS == unDBResult && 0 < strlen(poSARDBRequest_i->ucUserName))
    {
        unDBResult = CX_GetRecord_By_IMPI(hdbHSSThreadCtxt_io, &oIMPI, poSARDBRequest_i->ucUserName);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PRIVATE_ID;
        }
    }
    else if(DB_RESULT_SUCCESS == unDBResult && 0 == strlen(poSARDBRequest_i->ucUserName))
    {
        unDBResult = CX_GetIMPI_By_IMPU(hdbHSSThreadCtxt_io, &oIMPI, poSARDBRequest_i->ucPublicIdentity[0]);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PRIVATE_ID;
        }
    }

    if(DB_RESULT_SUCCESS == unDBResult && 0 < strlen(oIMPI.ucIMPI_ID) && 0 < poSARDBRequest_i->unPublicUserIdCount)
    {
        unDBResult = CX_Check_IMPI_IMPU_Mapping(hdbHSSThreadCtxt_io, oIMPI.ucIMPI_ID, poSARDBRequest_i->ucPublicIdentity[0]);
    }

    unsigned char *pucUserProfile = NULL;
    t_Db_HSSReturnVal oProfileCreationResultValue;

    if(DB_RESULT_SUCCESS == unDBResult)
    {
        if(SA_REGISTRATION == unServerAssignmentType ||
        		SA_RE_REGISTRATION == unServerAssignmentType)
        {
            if(USER_DATA_NOT_AVAILABLE == poSARDBRequest_i->unUserDataAvailable)
            {
                oProfileCreationResultValue = CxDx_GetUserProfile(hdbHSSThreadCtxt_io, oIMPI.ucIMPI_ID, &pucUserProfile);

                if(Db_HSS_Retval_Success == oProfileCreationResultValue && NULL != pucUserProfile )
                {

                    t_ChargingInfo oChrgInfo;
                    unsigned int unRetValue = CX_GetChrgInfo(hdbHSSThreadCtxt_io, &oChrgInfo, poSARDBRequest_i->ucPublicIdentity[0]);
                    if(DB_RESULT_SUCCESS == unRetValue)
                    {
                        if(strlen(oChrgInfo.PrimaryChrgFName) != 0)
                        {
                        	strcpy(oSARDBResponse.ucPCCFuncName, oChrgInfo.PrimaryChrgFName);
                        }

                        if(strlen(oChrgInfo.SecondChrgFName) != 0)
                        {
                        	strcpy(oSARDBResponse.ucSCCFuncName, oChrgInfo.SecondChrgFName);
                        }

                        if(strlen(oChrgInfo.PrimaryChrgEventFName) != 0)
                        {
                        	strcpy(oSARDBResponse.ucPECFuncName, oChrgInfo.PrimaryChrgEventFName);
                        }

                        if(strlen(oChrgInfo.SecondChrgFEventName) != 0)
                        {
                        	strcpy(oSARDBResponse.ucSECFuncName, oChrgInfo.SecondChrgFEventName);
                        }

                    }
                    else if(DB_RESULT_FAILURE == unRetValue)
                    {
                        unDBResult = DB_RESULT_FAILURE;
                    }
                }
                else
                {
                    unDBResult = DB_RESULT_FAILURE;
                }
            }
            //Check result code value
            CX_Update_Auth_Pending(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0], 0);
            CX_Update_Reg_Status(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0], CX_USER_STATUS_REGISTERED);
        }
        else if(SA_UNREGISTERED_USER == unServerAssignmentType)
        {
            unDBResult =CX_Update_SCSCF(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0],
														poSARDBRequest_i->ucSCSCFName);

            if(DB_RESULT_SUCCESS == unDBResult)
            {
                unDBResult = CX_Update_Reg_Status(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0], CX_USER_STATUS_UNREGISTERED);
            }

        }
        else if(SA_TIMEOUT_DEREGISTRATION == unServerAssignmentType||
                SA_USER_DEREGISTRATION == unServerAssignmentType||
                SA_DEREGISTRATION_TOO_MUCH_DATA == unServerAssignmentType||
                SA_ADMINISTRATIVE_DEREGISTRATION == unServerAssignmentType)
        {
             unsigned int unFlag = 0;
            if(NULL == poPUIDList)
            {
                unDBResult = CX_GetIMPI_IMPU_Mapping_By_IMPI(hdbHSSThreadCtxt_io, &poPUIDList, oIMPI.ucIMPI_ID);
                unFlag = 1;
            }

            if(DB_RESULT_SUCCESS == unDBResult)
            {
                unsigned char *pucPUId;
                poTempPUIDList = poPUIDList;

                 while( NULL != poTempPUIDList && DB_RESULT_SUCCESS == unDBResult)
                {
                	 pucPUId = NULL;

                    DMBase_SListGetNodeData(hDiamStackContext, poTempPUIDList, (void**)&pucPUId);
                    unDBResult = CX_Check_IMPU_Existence(hdbHSSThreadCtxt_io, pucPUId);
                    if(DB_RESULT_SUCCESS == unDBResult)
                    {
                        unDBResult = CX_Check_IMPI_IMPU_Mapping(hdbHSSThreadCtxt_io, oIMPI.ucIMPI_ID, pucPUId);
                    }
                    DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext, poTempPUIDList,
                                                                    &poTempPUIDList);
                }

                poTempPUIDList = poPUIDList;
                while( NULL != poTempPUIDList && DB_RESULT_SUCCESS == unDBResult)
                {
                	pucPUId = NULL;

                    DMBase_SListGetNodeData(hDiamStackContext, poTempPUIDList, (void**)&pucPUId);
                    unDBResult = CX_Update_SCSCF(hdbHSSThreadCtxt_io, pucPUId, NULL);
                    if(DB_RESULT_SUCCESS == unDBResult)
                    {
                        unDBResult = CX_Update_Reg_Status(hdbHSSThreadCtxt_io, pucPUId,
                                                    CX_USER_STATUS_NOT_REGISTERED);
                    }
                    DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext,
                                    poTempPUIDList, &poTempPUIDList);
                }
            }
             if(unFlag)
             {
                 DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poPUIDList, Cx_FreeDiamOctetStringCallback);
             }



        }
        else if(SA_TIMEOUT_DEREGISTRATION_STORE_SERVER_NAME == unServerAssignmentType ||
                SA_USER_DEREGISTRATION_STORE_SERVER_NAME == unServerAssignmentType)
        {
            unsigned int unFlag = 0;
            if(NULL == poPUIDList)
            {
                unDBResult = CX_GetIMPI_IMPU_Mapping_By_IMPI(hdbHSSThreadCtxt_io, &poPUIDList,
                                                                        oIMPI.ucIMPI_ID);
                unFlag = 1;
            }

            if(DB_RESULT_SUCCESS == unDBResult)
            {
                unsigned char    *pucPUId;
                poTempPUIDList = poPUIDList;

                 while(NULL != poTempPUIDList && DB_RESULT_SUCCESS == unDBResult)
                 {
                	 pucPUId = NULL;

                    DMBase_SListGetNodeData(hDiamStackContext, poTempPUIDList, (void**)&pucPUId);
                    unDBResult = CX_Check_IMPU_Existence(hdbHSSThreadCtxt_io, pucPUId);
                    if(DB_RESULT_SUCCESS == unDBResult)
                    {
                        unDBResult = CX_Check_IMPI_IMPU_Mapping(hdbHSSThreadCtxt_io, oIMPI.ucIMPI_ID,
																				pucPUId);
                    }
                    DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext, poTempPUIDList,
                                                                    &poTempPUIDList);
                 }

                poTempPUIDList = poPUIDList;
                while(NULL != poTempPUIDList && DB_RESULT_SUCCESS == unDBResult)
                {
                	pucPUId = NULL;

                    DMBase_SListGetNodeData(hDiamStackContext, poTempPUIDList, (void**)&pucPUId);
                    unDBResult = CX_Update_SCSCF(hdbHSSThreadCtxt_io, pucPUId, NULL);
                    if(DB_RESULT_SUCCESS == unDBResult)
                    {
                        unDBResult = CX_Update_Reg_Status(hdbHSSThreadCtxt_io, pucPUId,
                                                        CX_USER_STATUS_NOT_REGISTERED);
                    }
                    DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext, poTempPUIDList,
                                            &poTempPUIDList);
                }
            }
            if(unFlag)
             {
                 DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poPUIDList, Cx_FreeDiamOctetStringCallback);
             }
        }
        else if(SA_NO_ASSIGNMENT == unServerAssignmentType)
        {
            if(USER_DATA_NOT_AVAILABLE == poSARDBRequest_i->unUserDataAvailable)
            {
                oProfileCreationResultValue = CxDx_GetUserProfile(hdbHSSThreadCtxt_io,
                                                    oIMPI.ucIMPI_ID, &pucUserProfile);
                unDBResult = DB_RESULT_FAILURE;
                if(Db_HSS_Retval_Success != oProfileCreationResultValue || NULL == pucUserProfile )
                {
                    unDBResult = DB_RESULT_FAILURE;
                }
            }
            if(DB_RESULT_SUCCESS == unDBResult)
            {
            	strcpy(oSARDBResponse.ucSCSCFName, oIMPU.ucSCSCF_Name);
            }

        }
        else if(SA_AUTHENTICATION_FAILURE == unServerAssignmentType ||
                SA_AUTHENTICATION_TIMEOUT == unServerAssignmentType)
        {

            if(CX_USER_STATUS_NOT_REGISTERED == oIMPU.usReg_Status)
            {
                unDBResult = CX_Update_SCSCF(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0], NULL);
            }
            if(DB_RESULT_SUCCESS == unDBResult)
            {
                unDBResult = CX_Update_Auth_Pending(hdbHSSThreadCtxt_io, poSARDBRequest_i->ucPublicIdentity[0], 0);
            }
        }

    }

    if(DB_RESULT_SUCCESS == unDBResult)
    {
    	strcpy(oSARDBResponse.ucUserName, oIMPI.ucIMPI_ID);
    }

    oSARDBResponse.unDBResult = unDBResult;

    unsigned int unUserProfileLength = 0;
    if(NULL != pucUserProfile)
    {
    	unUserProfileLength = strlen(pucUserProfile);
    }

    unsigned int unBufferSize = sizeof(t_CxSARDBResponse) + unUserProfileLength;
    unsigned char *pucBuffer = NULL;

    DMBase_Malloc(hDBHSSContext_i->hDiamStackContext, (void**)&pucBuffer, unBufferSize);
    memcpy(pucBuffer, &oSARDBResponse, sizeof(t_CxSARDBResponse));

    if(NULL != pucUserProfile)
    {
    	memcpy(pucBuffer+sizeof(t_CxSARDBResponse), pucUserProfile, unUserProfileLength);
    }

//    AMPS_EvtSysSendIntEvt(hDBHSSContext_i->hDiamStackContext->hFrameworkHandle, CX_SAR_DATABASE_RESPONSE_EVENT, (void* ) pucBuffer,
//																			unBufferSize );

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadCtxt_io->hUnitIOAgent,
			CX_SAR_DATABASE_RESPONSE_EVENT, (void* ) pucBuffer, unBufferSize ))
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for CX_SAR_DATABASE_RESPONSE_EVENT \n\n ");
	}
    //Free Memory

    DMBase_Free(hDBHSSContext_i->hDiamStackContext, (void**)&pucBuffer);
    if(NULL != pucUserProfile)
    {
    	DMBase_Free(hDBHSSContext_i->hDiamStackContext, (void**)&pucUserProfile);
    }

    DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poPUIDList, Cx_FreeDiamOctetStringCallback);

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return Db_HSS_Retval_Success;
}

