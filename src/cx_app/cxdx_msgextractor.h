
#ifndef __CX_DX_MESSAGE_PARSER_H__
#define	__CX_DX_MESSAGE_PARSER_H__

#include "events.h"
#include "cxdx_app.h"
#include "dmbase_data_types.h"


#ifdef __cplusplus
extern "C" {
#endif

t_CxDxAPIRetVal Cx_ExtractAppMsgAttributes(	HCXAPPCONTEXT  		hAppContext_i,
												HDIAMETERMESSAGE    hDiaMsg_i,
												void**				ppAppMsg_o);

t_CxDxAPIRetVal Cx_ExtractVendorAppAttribute( HCXAPPCONTEXT	hAppContext_i,
												 HAVP           hVendorAppAVP_i,
												 t_CxVendorApp 	*poVendorApp_o);

t_CxDxAPIRetVal Cx_ExtractSIPAuthDataItems(HCXAPPCONTEXT  hAppContext_i,
                                              HAVP           hSIPAuthDataItemAVP_i,
                                              unsigned char*  pucAuthScheme);

t_CxDxAPIRetVal Cx_ExtractUARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxUAR*            poUAR_o);

t_CxDxAPIRetVal Cx_ExtractSARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxSAR*            poSAR_o);

t_CxDxAPIRetVal Cx_ExtractMARAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxMAR*            poMAR_o);

t_CxDxAPIRetVal Cx_ExtractLIRAttributes(HCXAPPCONTEXT       hAppContext_i,
                                           HDIAMETERMESSAGE    hDiaMsg_i,
                                           t_CxLIR*            poLIR_o);

void Cx_SetAVPMetaData(t_CxAVPMetadata* poAVPMetaData_i, unsigned int unPosition_i);


#ifdef __cplusplus
}
#endif


#endif //__CX_DX_MESSAGE_PARSER_H__

