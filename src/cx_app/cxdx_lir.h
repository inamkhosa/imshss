

#ifndef __CX_DX_LIR_H__
#define	__CX_DX_LIR_H__

#include "cxdx_app.h"
#include "dmbase_data_types.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct __CxLIA
{
	unsigned char*	pucSCSCFName;
	unsigned char*	pucSessionID;
	unsigned int 	unResultCode;
	unsigned int 	unExpResultCode;
	unsigned int*	poServerMandatoryCaps;
	unsigned int	unMandatoryCapsCount;
	unsigned int*	poServerOptionalCaps;
	unsigned int	unOptionalCapsCount;

}t_CxLIA;

t_CxDxAPIRetVal Cx_ProcessLIR(	HCXAPPCONTEXT hAppContext_i,
								t_CxLIR *poLIR_i,
								HPEERTABLEENTRY hPeer_i);

t_CxDxAPIRetVal Cx_GenerateLIRDBRequest(	HCXAPPCONTEXT 	hAppContext_i,
											t_CxLIR 		*poLIR_i);

t_CxDxAPIRetVal	Cx_ProcessLIRDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char*	pucEventData_i,
										unsigned int 	unEventDataSize_i);

t_CxDxAPIRetVal Cx_CreateLIADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgLIA_io,
											t_CxLIR*			poLIR_i,
											t_CxLIA*			poLIA_i);

#ifdef __cplusplus
}
#endif


#endif //__CX_DX_LIR_H__

