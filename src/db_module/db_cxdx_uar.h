

#ifndef __DB_CX_DX_UAR_H__
#define	__DB_CX_DX_UAR_H__

#include "dbmodule.h"
#include "events.h"

#ifdef __cplusplus
extern "C" {
#endif


t_Db_HSSReturnVal	Cx_ProcessUARDBRequest(	HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
											t_CxUARDBRequest* 	poUARDBRequest_i);

#ifdef __cplusplus
}
#endif


#endif //__DB_CX_DX_UAR_H__

