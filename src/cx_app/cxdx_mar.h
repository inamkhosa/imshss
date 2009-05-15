

#ifndef __CX_DX_MAR_H__
#define	__CX_DX_MAR_H__

#define MAX_AUTH_VECTORS_ALLOWED	5

#include "cxdx_app.h"
#include "dmbase_data_types.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct __CxMAA
{

	unsigned char*	pucUserName;
	unsigned char*	pucPublicId;
	unsigned char*	pucSessionID;
	unsigned int 	unResultCode;
	unsigned int 	unExpResultCode;
	unsigned int	unSIPNumAuthItems;
	unsigned char*	pucPrivateKey;
	unsigned char*	pucAuthScheme;

}t_CxMAA;

t_CxDxAPIRetVal Cx_ProcessMAR(	HCXAPPCONTEXT 	hAppContext_i,
								t_CxMAR*		poMAR_i,
								HPEERTABLEENTRY hPeer_i);

t_CxDxAPIRetVal Cx_GenerateMARDBRequest(HCXAPPCONTEXT 	hAppContext_i,
										t_CxMAR*		poMAR_i);

t_CxDxAPIRetVal	Cx_ProcessMARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char*	pucEventData_i,
										unsigned int 	unEventDataSize_i);

t_CxDxAPIRetVal Cx_CreateMAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgMAA_io,
											t_CxMAR*			poMAR_i,
											t_CxMAA*			poMAA_i);

#ifdef __cplusplus
}
#endif


#endif //__CX_DX_MAR_H__

