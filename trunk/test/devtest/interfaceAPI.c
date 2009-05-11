
/***************************************************************************************
** Written by Muhammad Zeeshan
**
** Copyright (C) 2009 AdvancedIMS
**
** All rights reserved.
** 
****************************************************************************************
** FILE NAME      		: interfaceAPI.c
** DESCRIPTION    		: The file contains xml interface API code.
** PROJECT NAME   		: IMS
** TYPE           		: ANSI C
** CONTRIBUTOR(S) 		: Muhammad Zeeshan
****************************************************************************************
****************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>

#include "interfaceAPI.h"



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

xmlChar * oampMakeRequest (char * oid, char * opcode, long value)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlDocPtr doc;

    xmlNodePtr cur;
    
    xmlChar *xmlbuff=NULL;
    int buffersize;

    /* Create a new XmlWriter, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (NULL == writer) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }


    /* Start an element named "REQUEST".*/
    rc = xmlTextWriterStartElement(writer, BAD_CAST "REQUEST");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    
        /* Start an element named "VARIABLE" as child of REQUEST. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    

    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }


    /* Write an element named "OID" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OID",
                                         "%s", oid);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%ld", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Here we could close all the elements. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    
        /*
     * Dump the document to a buffer and print it
     * for demonstration purposes.
     */
    xmlFreeTextWriter(writer);

    //xmlSaveFileEnc(file, doc, MY_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    //printf("\n\n\n****** Printing Buffer\n\n\n");
    //printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}



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

xmlChar * oampMakeRequestString (char * oid, char * opcode, char * value)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlDocPtr doc;

    xmlNodePtr cur;
    
    xmlChar *xmlbuff=NULL;
    int buffersize;

    /* Create a new XmlWriter, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (NULL == writer) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }


    /* Start an element named "REQUEST".*/
    rc = xmlTextWriterStartElement(writer, BAD_CAST "REQUEST");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    
        /* Start an element named "VARIABLE" as child of REQUEST. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }


    /* Write an element named "OID" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OID",
                                         "%s", oid);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%s", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Here we could close all the elements. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    
        /*
     * Dump the document to a buffer and print it
     * for demonstration purposes.
     */
    xmlFreeTextWriter(writer);

    //xmlSaveFileEnc(file, doc, MY_ENCODING);

    //dumping the doc into a buffer
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    //verification code
    //printf("\n\n\n****** Printing Buffer\n\n\n");
    //printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}



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

void oampProcessResponseNode (struct oampCommand *pCMD, xmlDocPtr doc, xmlNodePtr cur) 
{
        
        xmlChar *key_oid=NULL, *key_value=NULL, *key_opcode=NULL, *key_errordesc=NULL, *key_errorcode=NULL;
        int size;
        
        cur = cur->xmlChildrenNode;
                
        //printf("\n****Indise Process Node\n");
        while (NULL != cur) {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"OPCODE"))) 
	    {
                            key_opcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_opcode);
                            strncpy (pCMD->opcode, key_opcode, size);
                            //printf("OPCODE Inside ProcessNode: %s\n", pCMD->opcode);
                            xmlFree(key_opcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"OID"))) 
	    {
                            key_oid = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_oid);
                            strncpy (pCMD->oid, key_oid, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->oid);
                            xmlFree(key_oid);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VALUE"))) 
	    {
                            key_value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->value = atol(key_value);
                            //printf("VALUE Inside ProcessNode: %ld\n", pCMD->value);
                            xmlFree(key_value);
            }  
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORCODE"))) 
	    {
                            key_errorcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->errorcode = atol(key_errorcode);
                            //printf("ERRORCODE Inside ProcessNode: %d\n", pCMD->errorcode);
                            xmlFree(key_errorcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORDESC"))) 
	    {
                            key_errordesc = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_errordesc);
                            strncpy (pCMD->errordesc, key_errordesc, size);
                            //printf("ERRORDESC Inside ProcessNode: %s\n", pCMD->errordesc);
                            xmlFree(key_errordesc);
            }
	    else
	    {
               //unexpected tag
	       //printf("ERROR: The following tag was found unexpectedly=> %s\n", cur->name);
            }

        cur = cur->next;
        }
        //printf("\n****Leaving Process Node\n");
}



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

void oampProcessResponseNodeString (struct oampCommand *pCMD, xmlDocPtr doc, xmlNodePtr cur)
{
        xmlChar *key_oid=NULL, *key_value=NULL, *key_opcode=NULL, *key_errordesc=NULL, *key_errorcode=NULL;
        int size;
        cur = cur->xmlChildrenNode;
        
        //printf("\n****Indise Process Node\n");
        while (NULL != cur) {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"OPCODE"))) 
	    {
                            key_opcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_opcode);
                            strncpy (pCMD->opcode, key_opcode, size);
                            //printf("OPCODE Inside ProcessNode: %s\n", pCMD->opcode);
                            xmlFree(key_opcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"OID"))) 
	    {
                            key_oid = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_oid);
                            strncpy (pCMD->oid, key_oid, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->oid);
                            xmlFree(key_oid);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VALUE"))) 
	    {
                            key_value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_value);
                            strncpy (pCMD->str_value, key_value, size);
                            xmlFree(key_value);
            }  
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORCODE"))) 
	    {
                            key_errorcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->errorcode = atol(key_errorcode);
                            //printf("ERRORCODE Inside ProcessNode: %d\n", pCMD->errorcode);
                            xmlFree(key_errorcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORDESC"))) 
	    {
                            key_errordesc = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_errordesc);
                            strncpy (pCMD->errordesc, key_errordesc, size);
                            //printf("ERRORDESC Inside ProcessNode: %s\n", pCMD->errordesc);
                            xmlFree(key_errordesc);
            }
	    else
	    {
               //unexpected tag
	       //printf("ERROR: The following tag was found unexpectedly=> %s\n", cur->name);
            }

        cur = cur->next;
        }
        //printf("\n****Leaving Process Node\n");
}




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

xmlChar * oampMakeRequestTable (char * opcode, char * table, char * column, int row, long value)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlDocPtr doc;

    xmlNodePtr cur;
    
    xmlChar *xmlbuff=NULL;
    int buffersize;

    /* Create a new XmlWriter, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (NULL == writer) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }


    /* Start an element named "REQUEST".*/
    rc = xmlTextWriterStartElement(writer, BAD_CAST "REQUEST");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    
        /* Start an element named "VARIABLE" as child of REQUEST. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    

    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }
    
    
    /* Write an element named "TABLE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "TABLE",
                                         "%s", table);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }


    /* Write an element named "COLUMN" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLUMN",
                                         "%s", column);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Write an element named "ROW" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ROW",
                                         "%d", row);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%ld", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Here we could close all the elements. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    
        /*
     * Dump the document to a buffer and print it
     * for demonstration purposes.
     */
    xmlFreeTextWriter(writer);

    //xmlSaveFileEnc(file, doc, MY_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    printf("\n\n\n****** Printing Buffer\n\n\n");
    printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}



/***************************************************************************************
** Function Name: 
**		MakeRequestStringTable
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

xmlChar * MakeRequestStringTable (char * opcode, char * table, char * column, int row, char * value)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlDocPtr doc;

    xmlNodePtr cur;
    
    xmlChar *xmlbuff=NULL;
    int buffersize;

    /* Create a new XmlWriter, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (NULL == writer) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    /* Start the document with the xml default for the version,
     * encoding ISO 8859-1 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }


    /* Start an element named "REQUEST".*/
    rc = xmlTextWriterStartElement(writer, BAD_CAST "REQUEST");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    
        /* Start an element named "VARIABLE" as child of REQUEST. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    

    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }
    
    
    /* Write an element named "TABLE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "TABLE",
                                         "%s", table);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }


    /* Write an element named "COLUMN" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLUMN",
                                         "%s", column);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Write an element named "ROW" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ROW",
                                         "%d", row);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%s", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    
    /* Here we could close all the elements. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    
        /*
     * Dump the document to a buffer and print it
     * for demonstration purposes.
     */
    xmlFreeTextWriter(writer);

    //xmlSaveFileEnc(file, doc, MY_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    printf("\n\n\n****** Printing Buffer\n\n\n");
    printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}





/***************************************************************************************
** Function Name: 
**		oampProcessResponseNodeStringTable
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

void oampProcessResponseNodeStringTable (struct oampCommandTable *pCMD, xmlDocPtr doc, xmlNodePtr cur)
{
        xmlChar *key_opcode=NULL, *key_table=NULL, *key_row=NULL, *key_column=NULL, *key_errordesc=NULL, *key_errorcode=NULL, *key_value=NULL;
        int size;
        cur = cur->xmlChildrenNode;
        
        //printf("\n****Indise Process Node\n");
        while (NULL != cur) {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"OPCODE"))) 
	    {
                            key_opcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_opcode);
                            strncpy (pCMD->opcode, key_opcode, size);
                            //printf("OPCODE Inside ProcessNode: %s\n", pCMD->opcode);
                            xmlFree(key_opcode);
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"TABLE")))
	    {
                            key_table = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_table);
                            strncpy (pCMD->table, key_table, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->table);
                            xmlFree(key_table);
            }
            
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"COLUMN")))
	    {
                            key_column = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_column);
                            strncpy (pCMD->column, key_column, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->column);
                            xmlFree(key_column);
            }
            
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ROW"))) 
	    {
                            key_row = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->row = atoi(key_row);
                            //printf("ROW Inside ProcessNode: %d\n", pCMD->row);
                            xmlFree(key_row);
            }
            
                 
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VALUE"))) 
	    {
                            key_value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_value);
                            strncpy (pCMD->str_value, key_value, size);
                            //printf("VALUE Inside ProcessNode: %s\n", pCMD->str_value);
                            xmlFree(key_value);
            }  
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORCODE"))) 
	    {
                            key_errorcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->errorcode = atol(key_errorcode);
                            //printf("ERRORCODE Inside ProcessNode: %d\n", pCMD->errorcode);
                            xmlFree(key_errorcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORDESC"))) 
	    {
                            key_errordesc = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_errordesc);
                            strncpy (pCMD->errordesc, key_errordesc, size);
                            //printf("ERRORDESC Inside ProcessNode: %s\n", pCMD->errordesc);
                            xmlFree(key_errordesc);
            }
	    else
	    {
               //unexpected tag
	       //printf("ERROR: The following tag was found unexpectedly=> %s\n", cur->name);
            }

        cur = cur->next;
        }
        //printf("\n****Leaving Process Node\n");
}



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
**                          This parsing handles the commands for Integer OIDs.             
**		 
** 
***************************************************************************************
** Created By: 		: Muhammad Zeeshan
***************************************************************************************
***************************************************************************************
*/

void oampProcessResponseNodeTable (struct oampCommandTable *pCMD, xmlDocPtr doc, xmlNodePtr cur)
{
        xmlChar *key_opcode=NULL, *key_table=NULL, *key_row=NULL, *key_column=NULL, *key_errordesc=NULL, *key_errorcode=NULL, *key_value=NULL;
        int size;
        cur = cur->xmlChildrenNode;
        
        //printf("\n****Indise Process Node\n");
        while (NULL != cur) {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"OPCODE"))) 
	    {
                            key_opcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_opcode);
                            strncpy (pCMD->opcode, key_opcode, size);
                            //printf("OPCODE Inside ProcessNode: %s\n", pCMD->opcode);
                            xmlFree(key_opcode);
            }
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"TABLE")))
	    {
                            key_table = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_table);
                            strncpy (pCMD->table, key_table, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->table);
                            xmlFree(key_table);
            }
            
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"COLUMN")))
	    {
                            key_column = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_column);
                            strncpy (pCMD->column, key_column, size);
                            //printf("OID Inside ProcessNode: %s\n", pCMD->column);
                            xmlFree(key_column);
            }
            
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ROW"))) 
	    {
                            key_row = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->row = atoi(key_row);
                            //printf("ROW Inside ProcessNode: %d\n", pCMD->row);
                            xmlFree(key_row);
            }
            
                 
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VALUE"))) 
	    {
                            key_value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->value = atol(key_value);
                            //printf("VALUE Inside ProcessNode: %s\n", pCMD->str_value);
                            xmlFree(key_value);
            }  
            else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORCODE"))) 
	    {
                            key_errorcode = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            pCMD->errorcode = atol(key_errorcode);
                            //printf("ERRORCODE Inside ProcessNode: %d\n", pCMD->errorcode);
                            xmlFree(key_errorcode);
            }
	    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"ERRORDESC"))) 
	    {
                            key_errordesc = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                            size = strlen(key_errordesc);
                            strncpy (pCMD->errordesc, key_errordesc, size);
                            //printf("ERRORDESC Inside ProcessNode: %s\n", pCMD->errordesc);
                            xmlFree(key_errordesc);
            }
	    else
	    {
               //unexpected tag
	       //printf("ERROR: The following tag was found unexpectedly=> %s\n", cur->name);
            }

        cur = cur->next;
        }
        //printf("\n****Leaving Process Node\n");
}





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

xmlChar * oampMakeResponse (char * oid, char * opcode, long value, int errorcode, char * errordesc)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    xmlDocPtr doc;
    int size;

    xmlNodePtr cur;
    char key_opcode[OAMP_OPCODE_LENGTH], key_oid[OAMP_OID_LENGTH], key_errordesc[OAMP_ERRDESC_LENGTH];
    
    xmlChar *xmlbuff;
    int buffersize;
    
    /* Create a new XmlWriter for DOM, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }

    /* Start an element named "RESPONSE". */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "RESPONSE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    /* Start an element named "VARIABLE" as child of RESPONSE. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    size = strlen(opcode);
    strncpy(key_opcode, opcode, OAMP_OPCODE_LENGTH);
    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", key_opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(oid);
    strncpy(key_oid, oid, OAMP_OID_LENGTH);
    /* Write an element named "OID" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OID",
                                         "%s", key_oid);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%ld", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORCODE",
                                         "%d", errorcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(errordesc);
    strncpy(key_errordesc, errordesc, OAMP_ERRDESC_LENGTH);
    /* Write an element named "ERRORDESC" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORDESC",
                                         "%s", key_errordesc);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    //printf((char *) xmlbuff);
    
    return xmlbuff;
}



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

xmlChar * oampMakeResponseString (char * oid, char * opcode, char * value, int errorcode, char * errordesc)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    xmlDocPtr doc;
    int size;

    xmlNodePtr cur;
    char key_opcode[OAMP_OPCODE_LENGTH], key_oid[OAMP_OID_LENGTH], key_errordesc[OAMP_ERRDESC_LENGTH], key_value[OAMP_STR_VALUE_LENGTH];

    xmlChar *xmlbuff;
    int buffersize;

    /* Create a new XmlWriter for DOM, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }

    /* Start an element named "RESPONSE". */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "RESPONSE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Start an element named "VARIABLE" as child of RESPONSE. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    size = strlen(opcode);
    strncpy(key_opcode, opcode, OAMP_OPCODE_LENGTH);
    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", key_opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(oid);
    strncpy(key_oid, oid, OAMP_OID_LENGTH);
    /* Write an element named "OID" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OID",
                                         "%s", key_oid);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(value);
    strncpy(key_value, value, OAMP_STR_VALUE_LENGTH);
    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%s", key_value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORCODE",
                                         "%d", errorcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(errordesc);
    strncpy(key_errordesc, errordesc, OAMP_ERRDESC_LENGTH);
    /* Write an element named "ERRORDESC" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORDESC",
                                         "%s", key_errordesc);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    //printf((char *) xmlbuff);
    
    return xmlbuff;
}



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

xmlChar * oampMakeResponseTable (char * opcode, char * table, char * column, int row, long value, int errorcode, char * errordesc)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    xmlDocPtr doc;
    int size;

    xmlNodePtr cur;
char key_opcode[OAMP_OPCODE_LENGTH], key_table[OAMP_TABLE_NAME_LENGTH], key_errordesc[OAMP_ERRDESC_LENGTH], key_column[OAMP_COLUMN_NAME_LENGTH];

    xmlChar *xmlbuff;
    int buffersize;      

    /* Create a new XmlWriter for DOM, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }

    /* Start an element named "RESPONSE". */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "RESPONSE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }    
    
    /* Start an element named "VARIABLE" as child of RESPONSE. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    size = strlen(opcode);
    strncpy(key_opcode, opcode, size);
    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(table);
    strncpy(key_table, table, size);
    /* Write an element named "TABLE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "TABLE",
                                         "%s", table);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(column);
    strncpy(key_column, column, size);
    /* Write an element named "COLUMN" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLUMN",
                                         "%s", key_column);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }
    
    /* Write an element named "ROW" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ROW",
                                         "%d", row);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%ld", value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORCODE",
                                         "%d", errorcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(errordesc);
    strncpy(key_errordesc, errordesc, size);
    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORCODE",
                                         "%s", key_errordesc);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    //printf((char *) xmlbuff);
    
    return xmlbuff;
}


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

xmlChar * oampMakeResponseStringTable (char * opcode, char * table, char * column, int row, char * value, int errorcode, char * errordesc)
{
    int rc;
    xmlTextWriterPtr writer;
    xmlChar *tmp;
    xmlDocPtr doc;
    int size;

    xmlNodePtr cur;
    char key_opcode[OAMP_OPCODE_LENGTH], key_table[OAMP_TABLE_NAME_LENGTH], key_value[OAMP_STR_VALUE_LENGTH], key_errordesc[OAMP_ERRDESC_LENGTH], key_column[OAMP_COLUMN_NAME_LENGTH];

    xmlChar *xmlbuff;
    int buffersize;
      
    /* Create a new XmlWriter for DOM, with no compression. */
    writer = xmlNewTextWriterDoc(&doc, 0);
    if (writer == NULL) {
        printf("testXmlwriterDoc: Error creating the xml writer\n");
        return;
    }

    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartDocument\n");
        return;
    }

    /* Start an element named "RESPONSE". */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "RESPONSE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    /* Start an element named "VARIABLE" as child of RESPONSE. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "VARIABLE");
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterStartElement\n");
        return;
    }
    
    size = strlen(opcode);
    strncpy(key_opcode, opcode, size);
    /* Write an element named "OPCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "OPCODE",
                                         "%s", opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(table);
    strncpy(key_table, table, size);
    /* Write an element named "TABLE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "TABLE",
                                         "%s", table);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(column);
    strncpy(key_column, column, size);
    /* Write an element named "COLUMN" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "COLUMN",
                                         "%s", key_column);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }
    
    /* Write an element named "ROW" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ROW",
                                         "%d", row);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(value);
    strncpy(key_value, value, size);
    /* Write an element named "VALUE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "VALUE",
                                         "%s", key_value);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORCODE",
                                         "%d", errorcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(errordesc);
    strncpy(key_errordesc, errordesc, size);
    /* Write an element named "ERRORDESC" as child of VARIABLE. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ERRORDESC",
                                         "%s", key_errordesc);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    //printf((char *) xmlbuff);
    
    return xmlbuff;
}
