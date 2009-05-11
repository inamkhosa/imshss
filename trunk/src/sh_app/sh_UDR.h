#ifndef SH_UDR_H
#define SH_UDR_H

#include "sh_app.h"
#include "dmbase_data_types.h"


t_ShAppReturnVal	sh_app_ProcessUDRMessage(HSHAPPCONTEXT		hShAppContext_io,
											 HDIAMETERMESSAGE	hUDRMsg_i,
											HPEERTABLEENTRY hPeer_i);


t_ShAppReturnVal	sh_app_ProcessUDR_ResponseFromDB(HSHAPPCONTEXT		hShAppContext_io,
													 unsigned char* pucEvData_i);



#endif //#ifndef SH_UDR_H
