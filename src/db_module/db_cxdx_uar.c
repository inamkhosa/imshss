
#include "db_cxdx_uar.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "events.h"
#include "cxdx_db.h"
#include "hss.h"


extern int g_nDiamBaseTraceID ;

t_Db_HSSReturnVal	Cx_ProcessUARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxUARDBRequest* 	poUARDBRequest_i)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hdbHSSThreadCtxt_io || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext
                               || NULL == poUARDBRequest_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return Db_HSS_Retval_NullPointer;
    }

    HDBHSSCONTEXT hDBHSSContext_i = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    t_CxUARDBResponse oUARDBResponse;
    DMBase_MemsetZero(&oUARDBResponse, sizeof(t_CxUARDBResponse));

    strcpy(oUARDBResponse.ucSessionID, poUARDBRequest_i->ucSessionID);


    unsigned int unDBResult = DB_RESULT_SUCCESS;
    t_IMPI oIMPI;
    t_IMPU oIMPU;

    DMBase_MemsetZero(&oIMPI, sizeof(t_IMPI));
    DMBase_MemsetZero(&oIMPU, sizeof(t_IMPU));

    if(DB_RESULT_SUCCESS == unDBResult)
    {
    	unDBResult = CX_GetRecord_By_IMPI(hdbHSSThreadCtxt_io, &oIMPI, poUARDBRequest_i->ucUserName);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
        	unDBResult = DB_RESULT_NO_PRIVATE_ID;
        }

    }


    if(DB_RESULT_SUCCESS == unDBResult)
    {
        unDBResult = CX_GetRecord_By_IMPU(hdbHSSThreadCtxt_io, &oIMPU, poUARDBRequest_i->ucPublicIdentity);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PUBLIC_ID;
        }
    }

    if(DB_RESULT_SUCCESS == unDBResult)
    {
        if(DB_RESULT_SUCCESS != (unDBResult = CX_Check_IMPI_IMPU_Mapping(hdbHSSThreadCtxt_io,
                                                    poUARDBRequest_i->ucUserName, poUARDBRequest_i->ucPublicIdentity)))
        {
            unDBResult = DB_RESULT_NOT_FOUND;
        }
    }

    unsigned int unBarred = oIMPU.unBarringIndication;
    if(DB_RESULT_SUCCESS == unDBResult && oIMPU.unBarringIndication)
    {
        DMBase_SList *poIMPURecList = NULL;
        if(DB_RESULT_SUCCESS == (unDBResult = CX_GetIMPU_By_IMPI(hdbHSSThreadCtxt_io,
                                               &poIMPURecList, poUARDBRequest_i->ucUserName)))
        {
            DMBase_SList *poTempIMPUList = poIMPURecList;
            t_IMPU *poIMPU = NULL;
            while(NULL != poTempIMPUList)
            {
                poIMPU = NULL;
                DMBase_SListGetNodeData(hDBHSSContext_i->hDiamStackContext, poTempIMPUList, (void**)&poIMPU);
                if(NULL != poIMPU && !poIMPU->unBarringIndication)
                {
                    unBarred = 0;
                    break;
                }
                DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext,
                                        poTempIMPUList, &poTempIMPUList);
            }
        }
        DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poIMPURecList, Cx_FreeIMPUDataCallback);
    }


    if(DB_RESULT_SUCCESS == unDBResult)
    {
        oUARDBResponse.usRegisterStatus = oIMPU.usReg_Status;
        oUARDBResponse.usBarringInd = unBarred;

        if(0 != strlen(oIMPU.ucSCSCF_Name))
        {
        	strcpy(oUARDBResponse.ucSCSCFName, oIMPU.ucSCSCF_Name);
        }
        else
        {
            DMBase_SList *poIMPURecList = NULL;

            if(DB_RESULT_SUCCESS == (unDBResult = CX_GetIMPU_By_IMPI(hdbHSSThreadCtxt_io,
                                                   &poIMPURecList, poUARDBRequest_i->ucUserName)))
            {
                DMBase_SList *poTempIMPUList = poIMPURecList;
                t_IMPU *poIMPU = NULL;
                t_IMPU *poNodeData = NULL;
                for(; NULL != poTempIMPUList; DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext,
                                                                        poTempIMPUList, &poTempIMPUList))
                {
                    poNodeData = NULL;
                    DMBase_SListGetNodeData(hDBHSSContext_i->hDiamStackContext, poTempIMPUList, (void**)&poNodeData);
                    if(0 != strlen(poNodeData->ucSCSCF_Name))
                    {
                        if(CX_USER_STATUS_REGISTERED == poNodeData->usReg_Status)
                        {
                            poIMPU = poNodeData;
                            break;
                        }
                        else if(CX_USER_STATUS_UNREGISTERED == poNodeData->usReg_Status &&
                                (NULL == poIMPU  || NULL != poIMPU && CX_USER_STATUS_UNREGISTERED != poIMPU->usReg_Status))
                        {
                            poIMPU = poNodeData;
                        }
                        else if(NULL == poIMPU && poNodeData->usAuth_Pending)
                        {
                            poIMPU = poNodeData;
                        }
                    }
                }

                if(NULL != poIMPU)
                {
                	strcpy(oUARDBResponse.ucSCSCFName, poIMPU->ucSCSCF_Name);
                }
            }
            DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poIMPURecList, Cx_FreeIMPUDataCallback);
        }


        DMBase_SList *poCapsList = NULL;
        t_Capability *poCapability = NULL;
        if(DB_RESULT_SUCCESS == (unDBResult = CX_GetServerCaps(hdbHSSThreadCtxt_io, &poCapsList)))
        {
            DMBase_SList *poTempList = poCapsList;
            for(; NULL != poTempList; DMBase_SListGetNextNode(hDBHSSContext_i->hDiamStackContext,
                                                                poTempList, &poTempList))
            {
                poCapability = NULL;
                DMBase_SListGetNodeData(hDBHSSContext_i->hDiamStackContext, poTempList, (void**)&poCapability);
                if(OPTIONAL_CAPABILITY == poCapability->unCapType)
                {
                	oUARDBResponse.unOptionalCaps[oUARDBResponse.unOptionalCapsCount++] = poCapability->unCapValue;
                }
                else if(MANDATORY_CAPABILITY == poCapability->unCapType)
                {
                	oUARDBResponse.unMandatoryCaps[oUARDBResponse.unMandatoryCapsCount++] = poCapability->unCapValue;
                }
            }
        }
        DMBase_SListFree(hDBHSSContext_i->hDiamStackContext, &poCapsList, Cx_FreeCapabilityDataCallback);

    }
    oUARDBResponse.unDBResult = unDBResult;

    //AMPS_EvtSysSendIntEvt(hDBHSSContext_i->hDiamStackContext->hFrameworkHandle, 
	// CX_UAR_DATABASE_RESPONSE_EVENT, (void* )&oUARDBResponse, sizeof(t_CxUARDBResponse) );

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadCtxt_io->hUnitIOAgent,
					CX_UAR_DATABASE_RESPONSE_EVENT, &oUARDBResponse, sizeof(t_CxUARDBResponse) )
		)
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for CX_UAR_DATABASE_RESPONSE_EVENT \n\n ");
	}


    //Free Memory

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return Db_HSS_Retval_Success;

}





