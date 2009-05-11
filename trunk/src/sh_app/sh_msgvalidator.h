#ifndef	SH_MSGVALIDATOR_H
#define SH_MSGVALIDATOR_H

#include "sh_app.h"


t_ShAppReturnVal	sh_app_IsThisAnShMessage(HSHAPPCONTEXT	hShAppContext_io,
											 unsigned int	unDiamMsgCode_i,
											 _Bool*	pbIsShMsg_i);



t_ShAppReturnVal	sh_app_ValidateMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i,
										   HDIAMETERMESSAGE hDiamMsg_i);

t_ShAppReturnVal	sh_app_ValidateUDRMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i,
										   HDIAMETERMESSAGE hUDRMsg_i);

t_ShAppReturnVal	sh_app_ValidatePURMessage(HSHAPPCONTEXT		hShAppContext_io,
										   HPEERTABLEENTRY hPeer_i,
										   HDIAMETERMESSAGE hPURMsg_i);


#endif //#ifndef	SH_MSGVALIDATOR_H
