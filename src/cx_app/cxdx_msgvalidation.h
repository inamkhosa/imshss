
#ifndef __CX_DX_MSGVALIDATION_H__
#define	__CX_DX_MSGVALIDATION_H__


#include "dmbase_data_types.h"
#include "cxdx_app.h"
#include "cxdx_msg_api.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct __CxAVPValidationInfo
{
	unsigned int 	unAVPCode;
	_Bool			bMandatory;
	unsigned int  	unMaxAllowed;

}t_CxAVPValidationInfo;

t_CxDxAPIRetVal  Cx_ValidateMessage(	HCXAPPCONTEXT 	hAppContext_i,
										void*			poAppMsg_i,
										unsigned int	unCmdCode,
										DMBase_SList**	ppoFailedAVPList);

t_CxDxAPIRetVal  Cx_ValidateUAR(	HCXAPPCONTEXT   hAppContext_i,
									t_CxUAR*        poUAR_i,
                                    DMBase_SList**  ppoFailedAVPsList_o);

t_CxDxAPIRetVal  Cx_ValidateMAR( 	HCXAPPCONTEXT   hAppContext_i,
                                    t_CxMAR*        poMAR_i,
                                    DMBase_SList**  ppoFailedAVPsList_o);

t_CxDxAPIRetVal  Cx_ValidateSAR( 	HCXAPPCONTEXT   hAppContext_i,
                                    t_CxSAR*        poSAR_i,
                                    DMBase_SList**  ppoFailedAVPsList_o);

t_CxDxAPIRetVal  Cx_ValidateLIR( 	HCXAPPCONTEXT   hAppContext_i,
                                    t_CxLIR*        poLIR_i,
                                    DMBase_SList**  ppoFailedAVPsList_o);

t_CxDxAPIRetVal Cx_CheckMissingAVPs(HCXAPPCONTEXT   		hAppContext_i,
									t_CxAVPValidationInfo 	oMandatoryAVPs_i[],
									unsigned int    		unAVPCount_i,
									DMBase_SList**  		ppoFailedAVPsList_o);

t_CxDxAPIRetVal Cx_CheckDuplicateAVPs(	HCXAPPCONTEXT   		hAppContext_i,
										t_CxAVPValidationInfo 	oMandatoryAVPs_i[],
										unsigned int    		unAVPCount_i,
										DMBase_SList**  		ppoFailedAVPsList_o);

t_CxDxAPIRetVal  Cx_AddExpectedAVPToList(	HCXAPPCONTEXT 			hAppContext_i,
											HMSGALLOCATIONCONTEXT 	hMsgAllocContext_io,
											unsigned int 			unAVPCode_i,
											DMBase_SList** 			ppoFailedAVPsList_o);

t_CxDxAPIRetVal      Cx_GeneratePermanentFailure(HCXAPPCONTEXT 			hAppContext_i,
                                                 HPEERTABLEENTRY hPeer, HDIAMETERMESSAGE hDiamMsgRcv_i,
                                                 unsigned int unResCode, DMBase_SList* poFailedAVPs_i );

int Cx_FreeAVPValidationDataCallback(void** ppvData);


#ifdef __cplusplus
}
#endif

#endif //__CX_DX_MSGVALIDATION_H__



