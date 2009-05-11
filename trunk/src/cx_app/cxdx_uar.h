

#ifndef __CX_DX_UAR_H__
#define	__CX_DX_UAR_H__

#include "cxdx_app.h"
#include "dmbase_data_types.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct __CxUAA
{
	unsigned char*	pucSCSCFName;
	unsigned char*	pucSessionID;
	unsigned int 	unResultCode;
	unsigned int 	unExpResultCode;
	unsigned int*	poServerMandatoryCaps;
	unsigned int	unMandatoryCapsCount;
	unsigned int*	poServerOptionalCaps;
	unsigned int	unOptionalCapsCount;

}t_CxUAA;

t_CxDxAPIRetVal Cx_ProcessUAR(	HCXAPPCONTEXT	hAppContext_i,
								t_CxUAR*		poUAR_i,
								HPEERTABLEENTRY hPeer_i);

t_CxDxAPIRetVal Cx_GenerateUARDBRequest(HCXAPPCONTEXT 	hAppContext_i,
										t_CxUAR*		poUAR_i);

t_CxDxAPIRetVal	Cx_ProcessUARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char*	pucEventData_i,
										unsigned int 	unEventDataSize_i);

t_CxDxAPIRetVal Cx_CreateUAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgUAA_io,
											t_CxUAR*			poUAR_i,
											t_CxUAA*			poUAA_i);
#ifdef __cplusplus
}
#endif


#endif //__CX_DX_UAR_H__


