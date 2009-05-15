

#ifndef __CX_DX_SAR_H__
#define	__CX_DX_SAR_H__

#include "cxdx_app.h"
#include "dmbase_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __CxSAA
{
	unsigned char*	pucUserName;
	unsigned char*	pucUserData;
	unsigned int	unUserDataLength;
	unsigned char*	pucSessionID;
	unsigned int 	unResultCode;
	unsigned int 	unExpResultCode;
	unsigned char*	pucPEC;
	unsigned char*	pucSEC;
	unsigned char*	pucPCC;
	unsigned char*	pucSCC;

}t_CxSAA;

t_CxDxAPIRetVal Cx_ProcessSAR(	HCXAPPCONTEXT 	hAppContext_i,
								t_CxSAR*		poSAR_i,
								HPEERTABLEENTRY hPeer_i);

t_CxDxAPIRetVal Cx_GenerateSARDBRequest(HCXAPPCONTEXT 	hAppContext_i,
										t_CxSAR*		poSAR_i);

t_CxDxAPIRetVal	Cx_ProcessSARDBRespose(	HCXAPPCONTEXT 	hAppContext_i,
										unsigned char*	pucEventData_i,
										unsigned int 	unEventDataSize_i);

t_CxDxAPIRetVal Cx_CreateSAADiameterMessage(HCXAPPCONTEXT		hCxAppContext_io,
											HDIAMETERMESSAGE* 	phDiaMsgSAA_io,
											t_CxSAR*			poSAR_i,
											t_CxSAA*			poSAA_i);
#ifdef __cplusplus
}
#endif


#endif //__CX_DX_SAR_H__

