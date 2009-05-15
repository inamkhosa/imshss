
#include "db_cxdx_lir.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "events.h"
#include "cxdx_db.h"
#include "hss.h"

extern int g_nDiamBaseTraceID ;

t_Db_HSSReturnVal	Cx_ProcessLIRDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxLIRDBRequest* 	poLIRDBRequest_i)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hdbHSSThreadCtxt_io || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext
								   || NULL == poLIRDBRequest_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return Db_HSS_Retval_NullPointer;
    }

    HDIAMETERSTACKCONTEXT hDiamStackContext = hdbHSSThreadCtxt_io->hDBHSSMainContext->hDiamStackContext;
    t_CxLIRDBResponse oLIRDBResponse;
    DMBase_MemsetZero(&oLIRDBResponse, sizeof(t_CxLIRDBResponse));

    strcpy(oLIRDBResponse.ucSessionID, poLIRDBRequest_i->ucSessionID);

    unsigned int unDBResult = DB_RESULT_SUCCESS;
//    unsigned char ucSCSCFName[70] = {0};
    t_IMPU oIMPU;

    DMBase_MemsetZero(&oIMPU, sizeof(t_IMPU));

    unDBResult = CX_GetRecord_By_IMPU(hdbHSSThreadCtxt_io, &oIMPU, poLIRDBRequest_i->ucPublicIdentity);

    if(DB_RESULT_NOT_FOUND == unDBResult)
    {
        unDBResult = DB_RESULT_NO_PUBLIC_ID;
    }
    else if(DB_RESULT_SUCCESS == unDBResult)
    {
    	oLIRDBResponse.usRegisterStatus = oIMPU.usReg_Status;
    	oLIRDBResponse.usHasUnRegisteredStateServices = oIMPU.usHasUnregisteredStateServices;

        if(0 < strlen(oIMPU.ucSCSCF_Name))
        {
        	strcpy(oLIRDBResponse.ucSCSCFName, oIMPU.ucSCSCF_Name);
        }

    }

    if(DB_RESULT_SUCCESS == unDBResult && CX_USER_STATUS_NOT_REGISTERED == oIMPU.usReg_Status)
    {

        unsigned char ucSCSCFName[CX_LARGE_STRING] = {0};
        unDBResult = CX_GetSCSCFByIMPU(hdbHSSThreadCtxt_io, ucSCSCFName, poLIRDBRequest_i->ucPublicIdentity);


        if(DB_RESULT_SUCCESS == unDBResult)
        {
        	strcpy(oLIRDBResponse.ucSCSCFName, ucSCSCFName);
        }
        else if(DB_RESULT_FAILURE != unDBResult)
        {

            DMBase_SList *poCapsList = NULL;
            t_Capability *poCapability = NULL;
            if(DB_RESULT_SUCCESS == (unDBResult = CX_GetServerCaps(hdbHSSThreadCtxt_io, &poCapsList)))
            {
                DMBase_SList *poTempList = poCapsList;
                for(; NULL != poTempList; DMBase_SListGetNextNode(hDiamStackContext,
                                                                    poTempList, &poTempList))
                {
                    poCapability = NULL;
                    DMBase_SListGetNodeData(hDiamStackContext, poTempList, (void**)&poCapability);
                    if(OPTIONAL_CAPABILITY == poCapability->unCapType)
                    {
                    	oLIRDBResponse.unOptionalCaps[oLIRDBResponse.unOptionalCapsCount++] = poCapability->unCapValue;
                    }
                    else if(MANDATORY_CAPABILITY == poCapability->unCapType)
                    {
                    	oLIRDBResponse.unMandatoryCaps[oLIRDBResponse.unMandatoryCapsCount++] = poCapability->unCapValue;
                    }
                }
            }
            DMBase_SListFree(hDiamStackContext, &poCapsList, Cx_FreeCapabilityDataCallback);
        }
    }

    oLIRDBResponse.unDBResult = unDBResult;

 //   AMPS_EvtSysSendIntEvt(hDiamStackContext->hFrameworkHandle, CX_LIR_DATABASE_RESPONSE_EVENT, (void* ) &oLIRDBResponse,
 //                                           sizeof(t_CxLIRDBResponse) );

	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadCtxt_io->hUnitIOAgent,
			CX_LIR_DATABASE_RESPONSE_EVENT, (void* ) &oLIRDBResponse, sizeof(t_CxLIRDBResponse) )
		)
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for CX_LIR_DATABASE_RESPONSE_EVENT \n\n ");
	}
    //Free Memory

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return Db_HSS_Retval_Success;

}

