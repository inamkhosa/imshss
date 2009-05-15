

#include "db_cxdx_mar.h"
#include "dappcommon_helpers.h"
#include "dmbase_ampswrappers.h"
#include "events.h"
#include "cxdx_db.h"
#include "hss.h"

extern int g_nDiamBaseTraceID ;

t_Db_HSSReturnVal	Cx_ProcessMARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxMARDBRequest* 	poMARDBRequest_i)
{

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering\n");

    if(NULL == hdbHSSThreadCtxt_io || NULL == hdbHSSThreadCtxt_io->hDBHSSMainContext
                               || NULL == poMARDBRequest_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null Pointer Not Allowed\n ");
        return Db_HSS_Retval_NullPointer;
    }

    HDBHSSCONTEXT hDBHSSContext_i = hdbHSSThreadCtxt_io->hDBHSSMainContext;
    t_CxMARDBResponse oMARDBResponse;
    DMBase_MemsetZero(&oMARDBResponse, sizeof(t_CxMARDBResponse));

    strcpy(oMARDBResponse.ucSessionID, poMARDBRequest_i->ucSessionID);

    unsigned int unDBResult = DB_RESULT_SUCCESS;
    t_IMPI oIMPI;
    t_IMPU oIMPU;

    DMBase_MemsetZero(&oIMPI, sizeof(t_IMPI));
    DMBase_MemsetZero(&oIMPU, sizeof(t_IMPU));

    if(DB_RESULT_SUCCESS == unDBResult)
    {
        unDBResult = CX_GetRecord_By_IMPI(hdbHSSThreadCtxt_io, &oIMPI, poMARDBRequest_i->ucUserName);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PRIVATE_ID;
        }

    }


    if(DB_RESULT_SUCCESS == unDBResult)
    {
        unDBResult = CX_GetRecord_By_IMPU(hdbHSSThreadCtxt_io, &oIMPU, poMARDBRequest_i->ucPublicIdentity);
        if(DB_RESULT_NOT_FOUND == unDBResult)
        {
            unDBResult = DB_RESULT_NO_PUBLIC_ID;
        }
    }

    if(DB_RESULT_SUCCESS == unDBResult)
    {
        if(DB_RESULT_SUCCESS != (unDBResult = CX_Check_IMPI_IMPU_Mapping(hdbHSSThreadCtxt_io,
												poMARDBRequest_i->ucUserName,
												poMARDBRequest_i->ucPublicIdentity)))
        {
            unDBResult = DB_RESULT_NOT_FOUND;
        }
    }


    if(DB_RESULT_SUCCESS == unDBResult && strcmp(poMARDBRequest_i->ucAuthScheme, oIMPI.ucAuth_Scheme) == 0)
    {

        if(CX_USER_STATUS_REGISTERED == oIMPU.usReg_Status)
        {
            if(strcmp(poMARDBRequest_i->ucSCSCFName, oIMPU.ucSCSCF_Name) != 0)
            {
            	unDBResult = CX_Update_SCSCF(hdbHSSThreadCtxt_io, poMARDBRequest_i->ucPublicIdentity,
															poMARDBRequest_i->ucSCSCFName);
                if(DB_RESULT_SUCCESS == unDBResult)
                {
                    unDBResult = CX_Update_Auth_Pending(hdbHSSThreadCtxt_io, poMARDBRequest_i->ucPublicIdentity, 1);
                }
            }
        }
        else if(CX_USER_STATUS_NOT_REGISTERED == oIMPU.usReg_Status ||
                CX_USER_STATUS_UNREGISTERED == oIMPU.usReg_Status)
        {

            if(strlen(oIMPU.ucSCSCF_Name) == 0 ||
                    strcmp(poMARDBRequest_i->ucSCSCFName, oIMPU.ucSCSCF_Name) != 0)
            {
                unDBResult = CX_Update_SCSCF(hdbHSSThreadCtxt_io, poMARDBRequest_i->ucPublicIdentity,
															poMARDBRequest_i->ucSCSCFName);

            }
            if(DB_RESULT_SUCCESS == unDBResult)
            {
                unDBResult = CX_Update_Auth_Pending(hdbHSSThreadCtxt_io, poMARDBRequest_i->ucPublicIdentity, 1);
            }
         }
    }

    if(DB_RESULT_SUCCESS == unDBResult)
    {
    	strcpy(oMARDBResponse.ucAuthScheme, oIMPI.ucAuth_Scheme);
    	strcpy(oMARDBResponse.ucPrivateKey, oIMPI.ucSecret_key);
    	oMARDBResponse.usRegisterStatus = oIMPU.usReg_Status;
    }

    oMARDBResponse.unDBResult = unDBResult;

//    AMPS_EvtSysSendIntEvt(hDBHSSContext_i->hDiamStackContext->hFrameworkHandle, CX_MAR_DATABASE_RESPONSE_EVENT,
//											(void*)&oMARDBResponse, sizeof(t_CxMARDBResponse));
	if ( AMPS_SUCCESS != AMPS_IOAgentsSendEvt( hdbHSSThreadCtxt_io->hUnitIOAgent,
			CX_MAR_DATABASE_RESPONSE_EVENT, (void*)&oMARDBResponse, sizeof(t_CxMARDBResponse) ))
	{
		printf("\n\n AMPS_IOAgentsSendEvt failed for CX_MAR_DATABASE_RESPONSE_EVENT \n\n ");
	}
    //Free Memory

    DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving\n");
    return Db_HSS_Retval_Success;
}

