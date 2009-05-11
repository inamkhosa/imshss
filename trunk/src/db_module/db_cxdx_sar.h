

#ifndef __DB_CX_DX_SAR_H__
#define	__DB_CX_DX_SAR_H__

#include "dbmodule.h"
#include "events.h"
#ifdef __cplusplus
extern "C" {
#endif

t_Db_HSSReturnVal	Cx_ProcessSARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxSARDBRequest* 	poSARDBRequest_i);

#ifdef __cplusplus
}
#endif


#endif //__DB_CX_DX_SAR_H__

