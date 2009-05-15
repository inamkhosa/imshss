
#ifndef __CX_DX_DB_H__
#define	__CX_DX_DB_H__

//#include "dbmodule.h"
#include "dmbase_data_types.h"
#include "events.h"
#include "dbmodule.h"
#include "hss_oamps.h"

#define     CX_QUERY_SIZE   							2048
#define		CX_GET_SSERVICEPROFILE_RECORD_BY_IMPI		1
#define		CX_GET_IMPU_RECORD_BY_SERVICEPROFILE_ID		2
#define		CX_GET_IFC									3
#define		CX_GET_TRIGERPOINT							4
#define		CX_GET_SPT									5
#define		CX_GET_MEDIAPROFILEID_BY_CNSA_ID			6
#define		CX_Get_SHAREDIFCSET							7
#define		CX_GET_IMPI_DATA							8
#define 	CX_GET_IMPU_DATA							9
#define 	CX_GET_IMPI_BY_IMPU							10
#define		CX_GET_IMPI_IMPU_MAPPING_BY_IMPI			11
#define 	CX_GET_CHARGING_INFO						12
#define 	CX_GET_SERVER_CAPABILITIES					13
#define		CX_GET_IMPI_IMPU_MAPPING					14
#define		CX_GET_PUBLIC_USER_ID						15
#define		CX_GET_SCSCF_BY_IMPI						16
#define		CX_CLEAR_SCSCF_BY_IMPU						17
#define		CX_UPDATE_SCSCF_BY_IMPU						18
#define		CX_UPDATE_AUTH_PENDING_BY_IMPU				19
#define		CX_UPDATE_REG_STATUS_BY_IMPU				20
#define		CX_QUERY_CODE_GETOID_IMS_SUBS_DBU			21
#define		CX_QUERY_CODE_GETOID_PRIV_UID_DBU			22
#define		CX_QUERY_CODE_GETOID_REG_PRIV_ID_DBU		23
#define		CX_QUERY_CODE_GETOID_REG_PUI_ROAM			24
#define		CX_QUERY_CODE_GETOID_UNREG_PUI				25


// Capability Type
#define     MANDATORY_CAPABILITY        604
#define     OPTIONAL_CAPABILITY         605


typedef unsigned long long ULongInt;

typedef struct IMPI
{
    unsigned char   ucIMPI_ID[CX_LARGE_STRING];
    unsigned char   ucIMSU_ID[CX_LARGE_STRING];
    unsigned char   ucSecret_key[16];
    unsigned char   ucAuth_Scheme[CX_LARGE_STRING];
    unsigned char   ucAlgorithm[CX_LARGE_STRING];
    ULongInt        ulSequence_No;

} t_IMPI;

typedef struct IMPU
{
    unsigned char   ucIMPU_ID[CX_LARGE_STRING];
    unsigned short  usReg_Status;
    unsigned char   ucSIP_URI[CX_LARGE_STRING];
    unsigned char   ucTEL_URI[CX_LARGE_STRING];
    unsigned char   ucPSI_ID[CX_LARGE_STRING];
    unsigned int    unServiceProfile_ID;
    unsigned char   ucPublicService_ID[CX_LARGE_STRING];
    unsigned char   ucUserProfile_ID[CX_LARGE_STRING];
    unsigned short  usIsDefaultIMPU;
    unsigned short  usHasUnregisteredStateServices;
    unsigned int    unBarringIndication;
    unsigned int    unIdentityType;
    unsigned char   ucDisplayName[CX_LARGE_STRING];
    unsigned int    unMediaProfileID;
    unsigned int    unRepo_ID;
    unsigned short  usAuth_Pending;
    unsigned char   ucSCSCF_Name[CX_LARGE_STRING];

} t_IMPU;

typedef struct ChargingInfo
{
    unsigned char   ChrgCollecFName[CX_LARGE_STRING];
    unsigned char   PrimaryChrgFName[CX_LARGE_STRING];
    unsigned char   SecondChrgFName[CX_LARGE_STRING];
    unsigned char   PrimaryChrgEventFName[CX_LARGE_STRING];
    unsigned char   SecondChrgFEventName[CX_LARGE_STRING];

}t_ChargingInfo;

typedef struct Capability
{
    unsigned int unCapType;
    unsigned int unCapValue;
}t_Capability;

int CX_GetRecord_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPI *poRecord, unsigned char* pucIMPI);
int CX_GetRecord_By_IMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPU *poRecord, unsigned char *pucIMPU);
int CX_GetIMPI_By_IMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_IMPI *poRecord, unsigned char *pucIMPU);
int CX_GetIMPU_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoIMPUList, unsigned char   *pucIMPI);
int CX_GetIMPI_IMPU_Mapping_By_IMPI(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoIMPUList, unsigned char   *pucIMPI);
int CX_GetChrgInfo(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, t_ChargingInfo *poRecord, unsigned char   *pucIMPU);
int CX_GetServerCaps(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, DMBase_SList **ppoCapsList);
int CX_Check_IMPI_IMPU_Mapping(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char* pucIMPI, unsigned char *pucIMPU);
int CX_Check_IMPU_Existence(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU);
int CX_GetSCSCFByIMPU(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char ucSCSCFName[], unsigned char* pucPublicUserId);
int CX_Update_SCSCF(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char *pucIMPU, unsigned char *pucSCSCF);
int CX_Update_Auth_Pending(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU, unsigned short unFlag);
int CX_Update_Reg_Status(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io, unsigned char   *pucIMPU, unsigned short unFlag);

int Cx_FreeIMPUDataCallback(void** ppvData);
int Cx_FreeCharStringCallback(void** ppvData);
int Cx_FreeCapabilityDataCallback(void** ppvData);
int Cx_FreeDiamOctetStringCallback(void** ppvData);

t_Db_HSSReturnVal Cx_DB_GetCountOID_FromDBMS(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
							   unsigned int	unQueryID_i,
							   unsigned int*	punCount_o);

t_Db_HSSReturnVal Cx_DB_GetValForOID(HDBHSSTHREADCONTEXT hdbHSSThreadCtxt_io,
										t_hssOIDRespType	ohssOIDRespType_i,
										unsigned int		unOID_i,
										long *				plVal_o,
										unsigned char*		pucVal_o,
										unsigned int		unSizeOfValStr_i);


#endif

