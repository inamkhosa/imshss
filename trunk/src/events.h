/*
 * File:   events.h
 * Author: root
 *
 * Created on September 19, 2008, 2:29 PM
 */

#ifndef _EVENTS_H
#define	_EVENTS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "dmbase_data_types.h"



#define CX_EVENT_DATA_PUBLIC_IDENTITY           6001
#define CX_EVENT_DATA_PRIVATE_IDENTITY          6002
#define CX_EVENT_DATA_SERVER_NAME               6003
#define CX_EVENT_DATA_REG_STATUS                6004
#define CX_EVENT_DATA_MANDATORY_CAPABILITIES    6005
#define CX_EVENT_DATA_OPTIONAL_CAPABILITIES     6006
#define CX_EVENT_DATA_SERVICES_STATE            6007
#define CX_EVENT_DATA_SESSION_ID                6008
#define CX_EVENT_DATA_RESULT_CODE               6009
#define CX_EVENT_DATA_USER_PROFILE              6010
#define CX_EVENT_DATA_PEC_FUNCTION_NAME         6011
#define CX_EVENT_DATA_SEC_FUNCTION_NAME         6012
#define CX_EVENT_DATA_PCC_FUNCTION_NAME         6013
#define CX_EVENT_DATA_SCC_FUNCTION_NAME         6014
#define CX_EVENT_DATA_PRIVATE_KEY               6015
#define CX_EVENT_DATA_DERIGISTERATION_REASON    6016
#define CX_EVENT_DATA_AUTHENTICATION_SCHEME     6017
#define CX_EVENT_DATA_AUTH_PENDING              6018
#define CX_EVENT_DATA_SERVER_ASGN_TYPE          6019
#define CX_EVENT_DATA_USER_DATA_ALREADY_AVL     6020
#define CX_EVENT_DATA_BARRING_INDICATION        6021

#define CX_USER_STATUS_REGISTERED                  0
#define CX_USER_STATUS_UNREGISTERED                1
#define CX_USER_STATUS_NOT_REGISTERED              2

#define DB_CX_CMD_USER_AUTHORIZATION            8001
#define DB_CX_CMD_SERVER_ASSIGNMENT             8002
#define DB_CX_CMD_LOCATION_INFO                 8003
#define DB_CX_CMD_MULTIMEDIA_AUTH               8004
#define DB_CX_CMD_REGISTERATION_TERMINATION     8005
#define DB_CX_CMD_PUSH_PROFILE                  8006


#define DB_RESULT_SUCCESS                       8100
#define DB_RESULT_FAILURE                       8101
#define DB_RESULT_NOT_FOUND                     8102
#define DB_RESULT_NO_PUBLIC_ID                  8103
#define DB_RESULT_NO_PRIVATE_ID                 8104
#define DB_RESULT_INFO_MISSING                  8105
#define DB_RESULT_TOO_MUCH_DATA                 8106


//Deregisteration Reason
#define PERMANENT_TERMINATION                      0
#define NEW_SERVER_ASSIGNED                        1
#define SERVER_CHANGE                              2
#define REMOVE_S_CSCF                              3

//  Server-Assignment-Type
#define SA_NO_ASSIGNMENT                                        0
#define SA_REGISTRATION                                         1
#define SA_RE_REGISTRATION                                      2
#define SA_UNREGISTERED_USER                                    3
#define SA_TIMEOUT_DEREGISTRATION                               4
#define SA_USER_DEREGISTRATION                                  5
#define SA_TIMEOUT_DEREGISTRATION_STORE_SERVER_NAME             6
#define SA_USER_DEREGISTRATION_STORE_SERVER_NAME                7
#define SA_ADMINISTRATIVE_DEREGISTRATION                        8
#define SA_AUTHENTICATION_FAILURE                               9
#define SA_AUTHENTICATION_TIMEOUT                               10
#define SA_DEREGISTRATION_TOO_MUCH_DATA                         11

//User-Data-Already Available

#define USER_DATA_NOT_AVAILABLE                                 0
#define USER_DATA_ALREADY_AVAILABLE                             1

//String size

#define     CX_SMALL_STRING                          32
#define     CX_STANDARED_STRING                      64
#define     CX_LARGE_STRING                         128
#define     CX_EXTRA_LARGE_STRING                   256

#define     CX_MAX_OPTIONAL_CAPABILITIES              8
#define     CX_MAX_MANDATORY_CAPABILITIES             8
#define     CX_MAX_PUBLIC_USER_ID                    16


typedef	struct __CxUARDBRequest
{
    unsigned char   ucSessionID[CX_LARGE_STRING]; //UAR Session ID
    unsigned char   ucUserName[CX_LARGE_STRING]; //User Name(Private User Identity)
    unsigned char   ucPublicIdentity[CX_LARGE_STRING]; //Public User Identity

}t_CxUARDBRequest;

typedef struct __CxUARDBResponse
{
    unsigned char   ucSessionID[CX_LARGE_STRING]; //UAR Session ID
    unsigned char   ucSCSCFName[CX_LARGE_STRING]; //SCSCF Name , Stored in Database
    unsigned int    unDBResult; //Database result code
    unsigned short  usBarringInd; //Public User Identity's Barring Indication
    unsigned short  usRegisterStatus; //Public User Identity's Registration Status
    unsigned int    unOptionalCaps[CX_MAX_OPTIONAL_CAPABILITIES]; //List of Server's Optional Capabilities
    unsigned int    unOptionalCapsCount;  //Optional Capabilities Count
    unsigned int    unMandatoryCaps[CX_MAX_MANDATORY_CAPABILITIES]; //List of Server's Mandatory Capabilities
    unsigned int    unMandatoryCapsCount; //Mandatory Capabilities Count
}t_CxUARDBResponse;

typedef struct __CxSARDBRequest
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//SAR Session ID
    unsigned char   ucUserName[CX_LARGE_STRING]; //User Name(Private User Identity)
    unsigned char   ucSCSCFName[CX_LARGE_STRING]; //SCSCF Name
    unsigned int    unServerAssignmentType; //Server Assignment Type
    unsigned int    unUserDataAvailable;//User Data Already Available
    unsigned char   ucPublicIdentity[CX_MAX_PUBLIC_USER_ID][CX_LARGE_STRING]; //List of Public User Identity
    unsigned int    unPublicUserIdCount;//Public User Identities Count


}t_CxSARDBRequest;

typedef struct __CxSARDBResponse
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//SAR Session ID
    unsigned int    unDBResult;//Database result code
    unsigned char   ucUserName[CX_LARGE_STRING]; //User Name(Private User Identity)
    unsigned char   ucSCSCFName[CX_LARGE_STRING]; //SCSCF Name , Stored in Database
    unsigned char   ucPublicIdentity[CX_MAX_PUBLIC_USER_ID][CX_LARGE_STRING]; //List of Public User Identity
    unsigned int    unPublicUserIdCount;//Public User Identities Count
    unsigned char   ucPCCFuncName[CX_LARGE_STRING]; //Primary Charging Collection Function Name
    unsigned char   ucPECFuncName[CX_LARGE_STRING]; //Primary Event Charging Function Name
    unsigned char   ucSCCFuncName[CX_LARGE_STRING]; //Secondary Charging Collection Function Name
    unsigned char   ucSECFuncName[CX_LARGE_STRING]; //Secondary Event Charging Function Name

}t_CxSARDBResponse;

typedef struct __CxLIRDBRequest
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//LIR Session ID
    unsigned char   ucPublicIdentity[CX_LARGE_STRING]; //Public User Identity
}t_CxLIRDBRequest;

typedef struct __CxLIRDBResponse
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//LIR Session ID
    unsigned int    unDBResult;//Database result code
    unsigned short  usRegisterStatus; //Registration Status of User
    unsigned short  usHasUnRegisteredStateServices; //UnRegistered State Service Flag
    unsigned char   ucSCSCFName[CX_LARGE_STRING]; //SCSCF Name , Stored in Database
    unsigned int    unOptionalCaps[CX_MAX_OPTIONAL_CAPABILITIES]; //List of Server's Optional Capabilities
    unsigned int    unOptionalCapsCount;  //Optional Capabilities Count
    unsigned int    unMandatoryCaps[CX_MAX_MANDATORY_CAPABILITIES]; //List of Server's Mandatory Capabilities
    unsigned int    unMandatoryCapsCount; //Mandatory Capabilities Count

}t_CxLIRDBResponse;

typedef struct __CxMARDBRequest
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//MAR Session ID
    unsigned char   ucUserName[CX_LARGE_STRING]; //User Name(Private User Identity)
    unsigned char   ucPublicIdentity[CX_LARGE_STRING]; //Public User Identity
    unsigned char   ucSCSCFName[CX_LARGE_STRING]; //SCSCF Name
    unsigned char   ucAuthScheme[CX_LARGE_STRING]; //Authentication Scheme
}t_CxMARDBRequest;

typedef struct __CxMARDBResponse
{
    unsigned char   ucSessionID[CX_LARGE_STRING];//MAR Session ID
    unsigned int    unDBResult;//Database result code
    unsigned short  usRegisterStatus; //Registration Status of User
    unsigned char   ucAuthScheme[CX_LARGE_STRING]; //Authentication Scheme
    unsigned char   ucPrivateKey[CX_LARGE_STRING]; //Authentication Scheme

}t_CxMARDBResponse;



typedef struct EventDataHeader
{
    int nTotalLength;
    int nCommandID;

}t_EventDataHeader;

int InsertInt(unsigned char* ucBuffer, int nOffset, int nSize, int nValue);
int InsertString(unsigned char* ucBuffer, int nOffset, int nSize, char* pcValue);

int GetInt(unsigned char* ucBuffer, int nOffset, int* pnValue);
int GetString(unsigned char* ucBuffer, int nOffset,  int nSize, char** ppcValue);

#ifdef	__cplusplus
}
#endif

#endif	/* _EVENTS_H */

