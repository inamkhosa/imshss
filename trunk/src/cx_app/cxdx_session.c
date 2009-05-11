
#include "cxdx_msg_api.h"
#include "cxdx_session.h"
#include "dmbase_ampswrappers.h"
#include "hss.h"

extern	int	g_nDiamBaseTraceID ;

t_CxDxAPIRetVal	Cx_InsertSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i,
										void* 			poSessionObj_i)
{
    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext  ||
    		NULL == hAppContext_i->hSessionTable ||NULL == pucSessionID_i ||
    		NULL == poSessionObj_i)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null pointer not allowed\n");
        return CxDx_Retval_Null_Pointer;
    }

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, pucSessionID_i);

    if(DMBase_Retval_Success != DMBase_HashTableInsert(	hAppContext_i->hDiamStackContext,
														hAppContext_i->hSessionTable,
														poSessionObj_i,
														&oHashTableKey))
    {
    	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Session Table Insertion Failed\n");
        return CxDx_Retval_Failure;
    }

//    static unsigned int nCount = 0;
//    printf("Session Inserted = %u\r\n", ++nCount);

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, " Leaving\n");
    return CxDx_Retval_Success;
}



t_CxDxAPIRetVal Cx_LookUpSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i,
										void** 			ppoSessionObj_o)
{

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

    if(NULL == hAppContext_i || NULL == hAppContext_i->hDiamStackContext  ||
    		NULL == hAppContext_i->hSessionTable ||NULL == pucSessionID_i ||
    		NULL == ppoSessionObj_o)
    {
        DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, " Null pointer not allowed\n");
        return CxDx_Retval_Null_Pointer;
    }

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, pucSessionID_i);

    if ( DMBase_Retval_Success != DMBase_HashTableSearch(hAppContext_i->hDiamStackContext,hAppContext_i->hSessionTable,
															&oHashTableKey, (void**)ppoSessionObj_o ))
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Session Object Not Found\n");
        return CxDx_Retval_Not_Found;
    }

    DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Leaving\n");
    return CxDx_Retval_Success;
}

t_CxDxAPIRetVal Cx_RemoveSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i)
{
	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Entering\n");

    t_DiamHashTableKey oHashTableKey;
    DMBase_SetStringInHashTableKey(&oHashTableKey, pucSessionID_i);

    if ( DMBase_Retval_Success != DMBase_HashTableRemove(hAppContext_i->hDiamStackContext,hAppContext_i->hSessionTable,
															&oHashTableKey ))
    {
        DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR, "Failed to remove Session Object\n");
        return CxDx_Retval_Not_Found;
    }

//    static unsigned int nCount = 0;
//    printf("Session Removed = %u\r\n", ++nCount);

	DMBASE_TRACE( g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO, "Leaving\n");
    return CxDx_Retval_Success;
}




