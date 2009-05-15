

#ifndef __DB_CX_DX_LIR_H__
#define	__DB_CX_DX_LIR_H__

#include "dbmodule.h"
#include "events.h"

#ifdef __cplusplus
extern "C" {
#endif

t_Db_HSSReturnVal	Cx_ProcessLIRDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxLIRDBRequest* 	poLIRDBRequest_i);

#ifdef __cplusplus
}
#endif


#endif //__DB_CX_DX_LIR_H__

