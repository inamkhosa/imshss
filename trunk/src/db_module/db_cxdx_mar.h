

#ifndef __DB_CX_DX_MAR_H__
#define	__DB_CX_DX_MAR_H__

#include "dbmodule.h"
#include "events.h"

#ifdef __cplusplus
extern "C" {
#endif

t_Db_HSSReturnVal	Cx_ProcessMARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxMARDBRequest* 	poMARDBRequest_i);

#ifdef __cplusplus
}
#endif


#endif //__DB_CX_DX_MAR_H__

