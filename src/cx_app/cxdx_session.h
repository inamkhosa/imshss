

#ifndef __CX_DX_SESSION_H__
#define	__CX_DX_SESSION_H__

#include "dmbase_data_types.h"
#include "cxdx_app.h"

typedef struct __SessionObject
{
    HCXAPPCONTEXT 	hAppContext;
    HPEERTABLEENTRY hPeer;
    void*			poData;
}t_CxSessionObject;

#ifdef __cplusplus
extern "C" {
#endif

t_CxDxAPIRetVal	Cx_InsertSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i,
										void* 			poSessionObj_i);

t_CxDxAPIRetVal Cx_LookUpSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i,
										void** 			ppoSessionObj_o);

t_CxDxAPIRetVal Cx_RemoveSessionObject(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char 	*pucSessionID_i);


#ifdef __cplusplus
}
#endif


#endif //__CX_DX_SESSION_H__

