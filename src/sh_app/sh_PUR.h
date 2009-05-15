
#ifndef SH_PUR_H
#define SH_PUR_H

#include "sh_app.h"
#include "dmbase_data_types.h"



t_ShAppReturnVal	sh_app_ProcessPURMessage(HSHAPPCONTEXT		hShAppContext_io,
											 HDIAMETERMESSAGE	hPURMsg_i,
											HPEERTABLEENTRY hPeer_i);




t_ShAppReturnVal	sh_app_ProcessPUR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
													 unsigned char* pucEvData_i);







#endif //#ifndef SH_PUR_H

