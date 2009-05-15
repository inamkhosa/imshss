

#ifndef	OAMPS_XMLAPI_H
#define	OAMPS_XMLAPI_H

#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "hss.h"
#include "hss_oamps.h"






struct oampCommand
{
    char    opcode[OAMP_OPCODE_LENGTH];
    char    oid[OAMP_OID_LENGTH];
    long    value; 
    char    str_value[OAMP_STR_VALUE_LENGTH];
    long    errorcode;
    char    errordesc[OAMP_ERRDESC_LENGTH];
};
          
                
struct oampCommandTable
{
    char    opcode[OAMP_OPCODE_LENGTH];
    char    table[OAMP_TABLE_NAME_LENGTH];
    char    column[OAMP_COLUMN_NAME_LENGTH];
    int     row;
    long    value; 
    char    str_value[OAMP_STR_VALUE_LENGTH];
    long    errorcode;
    char    errordesc[OAMP_ERRDESC_LENGTH];
};


typedef enum oampsCmdValType
{
	oampsCmdValType_long,
	oampsCmdValType_string
}t_oampsCmdValType;


typedef enum oampsTrapSeverity
{
	oampsTrapLowSeverity,
	oampsTrapMediumSeverity,
	oampsTrapHighSeverity

}t_oampsTrapSeverity;



xmlChar *	oampMakeRequest (char * oid, char * opcode, long value);

xmlChar *	oampMakeRequestString (char * oid, char * opcode, char * value);

void		oampProcessResponseNode (struct oampCommand *pCMD,  unsigned char* pucBuff_i);

void		oampProcessResponseNodeString (struct oampCommand *pCMD,  unsigned char* pucBuff_i);

xmlChar *	oampMakeRequestTable (char * opcode, char * table, char * column, int row, long value);

void		oampProcessResponseNodeStringTable (struct oampCommandTable *pCMD, unsigned char* pucBuff_i);

void		oampProcessResponseNodeTable (struct oampCommandTable *pCMD, unsigned char* pucBuff_i);

xmlChar *	oampMakeResponseTable (char * opcode, char * table, char * column, int row, long value, int errorcode, char * errordesc);
xmlChar *	oampMakeResponseStringTable (char * opcode, char * table, char * column, int row, char * value, int errorcode, char * errordesc);

t_hssOampsReturnVal	oampMakeResponse (HDIAMETERSTACKCONTEXT hDiamStackContext_io,
									 unsigned char * pucOid_i,
								  unsigned  char * pucOpcode_i, 
								  long lValue_i, 
								  unsigned char* pucValue_i,
								  t_oampsCmdValType  oOampsCmdValType_i,
								  int nErrorcode_i, 
								  unsigned char * pucErrorDesc_i,
								  unsigned char** ppucRespBuff_o);

/*
 <OID>listenPortBindFailed</OID>
    <VALUE>1200</VALUE>
    <SEVERITY>low-severity</SEVERITY>
    <ERRORCODE>1</ERRORCODE>
    <ERRORDESC>Test</ERRORDESC>
*/

t_hssOampsReturnVal	oampMakeTrapBuffer (HDIAMETERSTACKCONTEXT hDiamStackContext_io,
									 unsigned char * pucOid_i,
								  t_oampsTrapSeverity	oTrapSeverity_i, 
								  long lValue_i, 
								  int nErrorcode_i, 
								  unsigned char * pucErrorDesc_i,
								  unsigned char** ppucTrapBuff_o);



#endif //#ifndef	OAMPS_XMLAPI_H
