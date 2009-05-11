#ifndef HSS_H
#define	HSS_H


#define     HSS_OAMPS_XML_ENCODING      "UTF-8"
#define     OAMP_OPCODE_LENGTH          50
#define     OAMP_OID_LENGTH             50
#define     OAMP_STR_VALUE_LENGTH       200
#define     OAMP_ERRDESC_LENGTH         200    
#define     OAMP_TABLE_NAME_LENGTH      50
#define     OAMP_COLUMN_NAME_LENGTH     50
#define		HSS_MAX_NUM_INTERNAL_EVENTS		20
#define		HSS_MAX_NUM_EXTERNAL_EVENTS		20

//the define below is used to write final query to console before running it
//#define		HSS_ENABLE_SH_QUERY_CONSOLE_DUMP

typedef enum hssOampsReturnVal
{
    hssOamps_Retval_Success,
    hssOamps_Retval_Failure,
    hssOamps_Retval_IncorrectParameters,
	hssOamps_Retval_NullPointer,
	hssOamps_Retval_NotFound,
	hssOamps_Retval_InsufficientBuffer

} t_hssOampsReturnVal;


typedef enum HSS_InternalEvents
{
		//Cx Database Request events
	CX_UAR_DATABASE_REQUEST_EVENT =   2,
	CX_SAR_DATABASE_REQUEST_EVENT =  3,
	CX_MAR_DATABASE_REQUEST_EVENT =  4,
	CX_LIR_DATABASE_REQUEST_EVENT = 5,

	//Sh Database Request events
	SH_APP_REQUEST_EVENT	=	6,

	//OAMPS Database Request events
	OAMPS_REQUEST_TO_DB_EVENT = 7
	
}t_HSS_InternalEvents;

typedef enum HSS_ExternalEvents
{

	//Cx Database Response events
	DB_RESPONSE_EVENT_FOR_SH_APP = 2,
	
	//Cx Database Response events
	CX_UAR_DATABASE_RESPONSE_EVENT = 3,
	CX_SAR_DATABASE_RESPONSE_EVENT = 4,
	CX_MAR_DATABASE_RESPONSE_EVENT = 5,
	CX_LIR_DATABASE_RESPONSE_EVENT = 6,

	//OAMPS request events
	OAMPS_REQUEST_TO_SH_EVENT = 7,
	OAMPS_REQUEST_TO_CX_EVENT = 8,
	

	//OAMPS response events
	OAMPS_RESPONSE_FROM_SH_EVENT = 9,
	OAMPS_RESPONSE_FROM_CX_EVENT = 10,
	OAMPS_RESPONSE_FROM_DB_EVENT = 11,

	OAMPS_SEND_TRAP_EVENT = 12

}t_HSS_ExternalEvents;






#endif //#ifndef HSS_H
