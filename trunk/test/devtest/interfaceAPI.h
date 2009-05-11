
/******************************************************************************
** Written by Muhammad Zeeshan
**
** Copyright (C) 2009 AdvancedIMS
**
** All rights reserved.
** 
*******************************************************************************
** FILE NAME      		: interfaceAPI.h
** DESCRIPTION    		: The file contains xml interface API defines.
** PROJECT NAME   		: IMS
** TYPE           		: ANSI C
** CONTRIBUTOR(S) 		: Muhammad Zeeshan
*******************************************************************************
*******************************************************************************
*/

#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>



#define     MY_ENCODING                 "UTF-8"
#define     OAMP_OPCODE_LENGTH          20
#define     OAMP_OID_LENGTH             50
#define     OAMP_STR_VALUE_LENGTH       300
#define     OAMP_ERRDESC_LENGTH         500    
#define     OAMP_TABLE_NAME_LENGTH      50
#define     OAMP_COLUMN_NAME_LENGTH     50

/******************************************************************************
** Structure Name: 
**		oampCommand
**  
** Includes:
**          char    opcode[20]      for operation code
**          char    oid[50]         for oid            
**          long    value           for integer value 
**          char    str_value[300]  for strung value
**          long    errorcode       for error code 
**          char    errordesc[500]  for error description
**		 
** 
*******************************************************************************
** Created By: 		: Muhammad Zeeshan
*******************************************************************************
*******************************************************************************
*/

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



/***************************************************************************************
** Function Name: 
**		oampMakeRequest
**  
** Parameters:
**              char    oid
**                      type: input pointer
**                      detail : this oid contain string specifying OID contained in NE.
**              char    opcode
**                      type: input
**                      detail: this oid contain string specifying OP-CODE.
**              long    value
**                      type: input
**                      detail: contains the actual Integer value to be get or set. 
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Request to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeRequest (char * oid, char * opcode, long value);



/***************************************************************************************
** Function Name: 
**		oampMakeRequestString
**  
** Parameters:
**              char    oid
**                      type: input pointer
**                      detail : this oid contain string specifying OID contained in NE.
**              char    opcode
**                      type: input
**                      detail: this oid contain string specifying OP-CODE.
**              char    value
**                      type: input
**                      detail: contains the actual String value to be get or set. 
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Request to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeRequestString (char * oid, char * opcode, char * value);



/***************************************************************************************
** Function Name: 
**		oampProcessResponseNode
**  
** Parameters:
**              struct      oampCommand
**                          type: input/output structure pointer
**                          detail : this straucture contains oampCommand information.
**              xmlDocPtr   doc
**                          type: input xml document pointer
**                          detail: this is xml document pointer.
**              xmlNodePtr  cur
**                          type: input xml node pointer
**                          detail: this is xml current node pointer. 
**
** Return:      void
**
** Functionality:           Parsing the xml buffer and extracting the oampCommand details.
**                          This parsing handles the commands for Integer OIDs.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

void oampProcessResponseNode (struct oampCommand *pCMD, xmlDocPtr doc, xmlNodePtr cur);



/***************************************************************************************
** Function Name: 
**		oampProcessResponseNodeString
**  
** Parameters:
**              struct      oampCommand
**                          type: input/output structure pointer
**                          detail : this straucture contains oampCommand information.
**              xmlDocPtr   doc
**                          type: input xml document pointer
**                          detail: this is xml document pointer.
**              xmlNodePtr  cur
**                          type: input xml node pointer
**                          detail: this is xml current node pointer. 
**
** Return:      void
**
** Functionality:           Parsing the xml buffer and extracting the oampCommand details.
**                          This parsing handles the commands for String OIDs.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

void oampProcessResponseNodeString (struct oampCommand *pCMD, xmlDocPtr doc, xmlNodePtr cur);





/***************************************************************************************
** Function Name: 
**		oampMakeRequestTable
**  
** Parameters:
**              char    opcode
**                      type: input pointer
**                      detail : contain string specifying opcode contained in NE.
**              char    table
**                      type: input pointer
**                      detail: contain string specifying table name.
**              char    column 
**                      type: input pointer
**                      detail: contain string specifying column name.
**              int     row
**                      type: input integer
**                      detail: contains the row number in the table. 
**              long    value
**                      type: input
**                      detail: contains the actual Integer value to be get or set. 
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Request to send for get or set for table.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeRequestTable (char * opcode, char * table, char * column, int row, long value);




/***************************************************************************************
** Function Name: 
**		oampProcessResponseNodeStringTable
**  
** Parameters:
**              char    opcode
**                      type: input pointer
**                      detail : contain string specifying opcode contained in NE.
**              char    table
**                      type: input pointer
**                      detail: contain string specifying table name.
**              char    column 
**                      type: input pointer
**                      detail: contain string specifying column name.
**              int     row
**                      type: input integer
**                      detail: contains the row number in the table. 
**              char    value
**                      type: input pointer
**                      detail: contains the actual string value to be get or set. 
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Request to send for get or set for table.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

void oampProcessResponseNodeStringTable (struct oampCommandTable *pCMD, xmlDocPtr doc, xmlNodePtr cur);




/***************************************************************************************
** Function Name: 
**		oampProcessResponseNodeTable
**  
** Parameters:
**              struct      oampCommandTable
**                          type: input/output structure pointer
**                          detail : this straucture contains oampCommand information.
**              xmlDocPtr   doc
**                          type: input xml document pointer
**                          detail: this is xml document pointer.
**              xmlNodePtr  cur
**                          type: input xml node pointer
**                          detail: this is xml current node pointer. 
**
** Return:      void
**
** Functionality:           Parsing the xml buffer and extracting the oampCommand details.
**                          This parsing handles the commands for String OIDs.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

void oampProcessResponseNodeTable (struct oampCommandTable *pCMD, xmlDocPtr doc, xmlNodePtr cur);





/***************************************************************************************
** Function Name: 
**		oampMakeResponse
**  
** Parameters:
**              char    oid
**                      type: input pointer
**                      detail : this oid contain string specifying OID contained in NE.
**              char    opcode
**                      type: input pointer
**                      detail: this oid contain string specifying OP-CODE.
**              long    value
**                      type: input
**                      detail: contains the actual Integer value to be get or set. 
**              int     errorcode
**                      type: input
**                      detail: contains the error code. 
**              char    errordesc
**                      type: input pointer
**                      detail: this description of the error occured.
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Response to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeResponse (char * oid, char * opcode, long value, int errorcode, char * errordesc);





/***************************************************************************************
** Function Name: 
**		oampMakeResponseString
**  
** Parameters:
**              char    oid
**                      type: input pointer
**                      detail : this oid contain string specifying OID contained in NE.
**              char    opcode
**                      type: input pointer
**                      detail: this oid contain string specifying OP-CODE.
**              char    value
**                      type: input pointer
**                      detail: contains the actual String value to be get or set. 
**              int     errorcode
**                      type: input
**                      detail: contains the error code. 
**              char    errordesc
**                      type: input pointer
**                      detail: this description of the error occured.
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Response to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeResponseString (char * oid, char * opcode, char * value, int errorcode, char * errordesc);




/***************************************************************************************
** Function Name: 
**		oampMakeResponseTable
**  
** Parameters:
**              char    opcode
**                      type: input pointer
**                      detail: this oid contain string specifying OP-CODE.
**              char    table
**                      type: input pointer
**                      detail : this table contain string specifying table name.
**              char    column
**                      type: input pointer
**                      detail : this column contain string specifying column name.
**              int     row
**                      type: input
**                      detail: contains the rown number of the same table.
**              long    value
**                      type: input
**                      detail: contains the actual Integer value to be get or set. 
**              int     errorcode
**                      type: input
**                      detail: contains the error code. 
**              char    errordesc
**                      type: input pointer
**                      detail: this description of the error occured.
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Response to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeResponseTable (char * opcode, char * table, char * column, int row, long value, int errorcode, char * errordesc);





/***************************************************************************************
** Function Name: 
**		oampMakeResponseStringTable
**  
** Parameters:
**              char    opcode
**                      type: input pointer
**                      detail: this oid contain string specifying OP-CODE.
**              char    table
**                      type: input pointer
**                      detail : this table contain string specifying table name.
**              char    column
**                      type: input pointer
**                      detail : this column contain string specifying column name.
**              int     row
**                      type: input
**                      detail: contains the rown number of the same table.
**              long    value
**                      type: input
**                      detail: contains the actual Integer value to be get or set. 
**              int     errorcode
**                      type: input
**                      detail: contains the error code. 
**              char    errordesc
**                      type: input pointer
**                      detail: this description of the error occured.
**
** Return:      buffer  xmlChar*
**                      detail: return contains the xml buffer.
**
** Functionality:       Making the Response to send for get or set             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

xmlChar * oampMakeResponseStringTable (char * opcode, char * table, char * column, int row, char * value, int errorcode, char * errordesc);








