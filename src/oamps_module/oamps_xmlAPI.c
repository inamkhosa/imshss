

#include <stdio.h>
#include <stdlib.h>

#include "oamps_xmlAPI.h"
#include "hss.h"
//externs
extern	int                     g_nDiamBaseTraceID ;



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
    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

    //xmlSaveFileEnc(file, doc, HSS_OAMPS_XML_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    //printf("\n\n\n****** Printing Buffer\n\n\n");
    //printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}


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
    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

    //xmlSaveFileEnc(file, doc, HSS_OAMPS_XML_ENCODING);

    //dumping the doc into a buffer
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    //verification code
    //printf("\n\n\n****** Printing Buffer\n\n\n");
    //printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}




void oampProcessResponseNode (struct oampCommand *pCMD,  unsigned char* pucBuff_i)
{
        
        xmlChar *key_oid=NULL, *key_value=NULL, *key_opcode=NULL, *key_errordesc=NULL, *key_errorcode=NULL;
        int size;
		xmlDocPtr doc;
		xmlNodePtr cur;
		doc = xmlParseMemory(pucBuff_i, strlen(pucBuff_i));
		cur = xmlDocGetRootElement(doc);
		_Bool	bFoundNode =0;

		cur = cur->xmlChildrenNode;
		
	
		while (NULL != cur) 
		{
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"VARIABLE"))) 
			{
				bFoundNode = 1;
				break;
			}
			cur = cur->next;
		}//end while

		if (0 == bFoundNode) 
		{
            xmlFreeDoc(doc);
            xmlCleanupParser();
			return;


		}

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
		 xmlFreeDoc(doc);
            xmlCleanupParser();
}



void oampProcessResponseNodeString (struct oampCommand *pCMD,  unsigned char* pucBuff_i)
{
        xmlChar *key_oid=NULL, *key_value=NULL, *key_opcode=NULL, *key_errordesc=NULL, *key_errorcode=NULL;
        int size;
		xmlDocPtr doc;
		xmlNodePtr cur;
		doc = xmlParseMemory(pucBuff_i, strlen(pucBuff_i));
		cur = xmlDocGetRootElement(doc);
		_Bool	bFoundNode =0;
		
	
		while (NULL != cur) 
		{
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"VARIABLE"))) 
			{
				bFoundNode = 1;
				break;
			}
			cur = cur->next;
		}//end while

		if (0 == bFoundNode) 
		{
			return;
		}


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
    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

    //xmlSaveFileEnc(file, doc, HSS_OAMPS_XML_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    printf("\n\n\n****** Printing Buffer\n\n\n");
    printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}


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
    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

    //xmlSaveFileEnc(file, doc, HSS_OAMPS_XML_ENCODING);
    
    //dumping the document in a buffer to return
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    
    // verification test code
    printf("\n\n\n****** Printing Buffer\n\n\n");
    printf((char *) xmlbuff);
    //printf("\n****** Printing Buffer\n\n\n");
    
    return xmlbuff;
}


void oampProcessResponseNodeStringTable (struct oampCommandTable *pCMD, unsigned char* pucBuff_i)
{
        xmlChar *key_opcode=NULL, *key_table=NULL, *key_row=NULL, *key_column=NULL, *key_errordesc=NULL, *key_errorcode=NULL, *key_value=NULL;
        int size;
		xmlDocPtr doc;
		xmlNodePtr cur;
		doc = xmlParseMemory(pucBuff_i, strlen(pucBuff_i));
		cur = xmlDocGetRootElement(doc);
		_Bool	bFoundNode =0;
		
	
		while (NULL != cur) 
		{
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"VARIABLE"))) 
			{
				bFoundNode = 1;
				break;
			}
			cur = cur->next;
		}//end while

		if (0 == bFoundNode) 
		{
			return;
		}

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





void oampProcessResponseNodeTable (struct oampCommandTable *pCMD, unsigned char* pucBuff_i)
{
        xmlChar *key_opcode=NULL, *key_table=NULL, *key_row=NULL, *key_column=NULL, *key_errordesc=NULL, *key_errorcode=NULL, *key_value=NULL;
        int size;
		xmlDocPtr doc;
		xmlNodePtr cur;
		doc = xmlParseMemory(pucBuff_i, strlen(pucBuff_i));
		cur = xmlDocGetRootElement(doc);
		_Bool	bFoundNode =0;
		
	
		while (NULL != cur) 
		{
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"VARIABLE"))) 
			{
				bFoundNode = 1;
				break;
			}
			cur = cur->next;
		}//end while

		if (0 == bFoundNode) 
		{
			return;
		}

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






xmlChar * oampOLDMakeResponse (char * oid, char * opcode, long value, int errorcode, char * errordesc)
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

    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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
                                         "%s", key_opcode);
    if (rc < 0) {
        printf
            ("testXmlwriterDoc: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    size = strlen(oid);
    strncpy(key_oid, oid, size);
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

	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    xmlFreeTextWriter(writer);
    
    //printf((char *) xmlbuff);
    
    return xmlbuff;
}



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

    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

    rc = xmlTextWriterStartDocument(writer, NULL, HSS_OAMPS_XML_ENCODING, NULL);
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

t_hssOampsReturnVal	oampMakeResponse (HDIAMETERSTACKCONTEXT hDiamStackContext_io,
									 unsigned char * pucOid_i,
								  unsigned  char * pucOpcode_i, 
								  long lValue_i, 
								  unsigned char* pucValue_i,
								  t_oampsCmdValType  oOampsCmdValType_i,
								  int nErrorcode_i, 
								  unsigned char * pucErrorDesc_i,
								  unsigned char** ppucRespBuff_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == hDiamStackContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiamStackContext_io is NULL\n");
		return hssOamps_Retval_NullPointer;
	}

	if (oampsCmdValType_string == oOampsCmdValType_i  ) 
	{
		if (NULL == pucValue_i)
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucValue_i is NULL\n");
			return hssOamps_Retval_NullPointer;
		}
	}

	if (NULL == pucOid_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucOid_i is NULL\n");
		return hssOamps_Retval_NullPointer;
	}

	if (NULL == pucOpcode_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucOpcode_i is NULL\n");
		return hssOamps_Retval_NullPointer;	
	}

	if (NULL == pucErrorDesc_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucErrorDesc_i is NULL\n");
		return hssOamps_Retval_NullPointer;	
	}

	if (NULL == ppucRespBuff_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ppucRespBuff_o is NULL\n");
		return hssOamps_Retval_NullPointer;	
	}

	if (NULL != (*ppucRespBuff_o) )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," *ppucRespBuff_o is Not NULL\n");
		return hssOamps_Retval_IncorrectParameters;	
	}

	int 				nrc = 0;
	xmlTextWriterPtr	oXMLwriter = NULL;
	xmlBufferPtr 		oXMLbuf = NULL;
	t_hssOampsReturnVal	oRetval = hssOamps_Retval_Success;
	unsigned int		unSizeOfOid=0;
	unsigned int		unSizeOfErrorDesc=0;
	unsigned int		unSizeOfOpcode=0;

	unSizeOfOid = strlen(pucOid_i);
	unSizeOfErrorDesc = strlen(pucErrorDesc_i);
	unSizeOfOpcode  = strlen(pucOpcode_i);


	//do size checks
	if ( unSizeOfOid >= OAMP_OID_LENGTH) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strlen(pucOid_i) >= OAMP_OID_LENGTH , leaving\n");
		return hssOamps_Retval_IncorrectParameters;
	}

	if ( unSizeOfErrorDesc >= OAMP_ERRDESC_LENGTH) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strlen(pucErrorDesc_i) >= OAMP_ERRDESC_LENGTH , leaving\n");
		return hssOamps_Retval_IncorrectParameters;
	}

	if (unSizeOfOpcode >= OAMP_OPCODE_LENGTH) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strlen(pucOpcode_i) >= OAMP_OPCODE_LENGTH , leaving\n");
		return hssOamps_Retval_IncorrectParameters;
	}

 

	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_OAMPS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

    

    /* Start an element named "RESPONSE". */
    nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "RESPONSE");
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}
    
    /* Start an element named "VARIABLE" as child of RESPONSE. */
    nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "VARIABLE");
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}
    
    
    /* Write an element named "OPCODE" as child of VARIABLE. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "OPCODE",
                                         "%s", pucOpcode_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

 
    /* Write an element named "OID" as child of VARIABLE. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "OID",
                                         "%s", pucOid_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

    /* Write an element named "VALUE" as child of VARIABLE. */
	if (oampsCmdValType_string == oOampsCmdValType_i  ) 
	{
		nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "VALUE",
											 "%s", pucValue_i);
		if (nrc < 0) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
			oRetval = hssOamps_Retval_Failure;
			goto ExitMe;
		}
	}
	else
	{
		nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "VALUE",
											 "%ld", lValue_i);
		if (nrc < 0) 
		{
			DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
			oRetval = hssOamps_Retval_Failure;
			goto ExitMe;
		}
	}

    /* Write an element named "ERRORCODE" as child of VARIABLE. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ERRORCODE",
                                         "%d", nErrorcode_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

   
    /* Write an element named "ERRORDESC" as child of VARIABLE. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ERRORDESC",
                                         "%s", pucErrorDesc_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

    nrc = xmlTextWriterEndDocument(oXMLwriter);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	
	

	if ( DMBase_Retval_Success !=  DMBase_Malloc(hDiamStackContext_io,
						(void**)ppucRespBuff_o,oXMLbuf->use +1)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	memcpy(*ppucRespBuff_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

	ExitMe:

	if (NULL != oXMLwriter)
	{
		xmlFreeTextWriter(oXMLwriter);
		oXMLwriter = NULL;
	}

	if (NULL != oXMLbuf) 
	{
		xmlBufferFree(oXMLbuf);
		oXMLbuf = NULL;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return oRetval;
  
    
}


t_hssOampsReturnVal	oampMakeTrapBuffer (HDIAMETERSTACKCONTEXT hDiamStackContext_io,
									 unsigned char * pucOid_i,
								  t_oampsTrapSeverity	oTrapSeverity_i, 
								  long lValue_i, 
								  int nErrorcode_i, 
								  unsigned char * pucErrorDesc_i,
								  unsigned char** ppucTrapBuff_o)
{
	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Entering \n");

	if (NULL == hDiamStackContext_io)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," hDiamStackContext_io is NULL\n");
		return hssOamps_Retval_NullPointer;
	}


	if (NULL == pucOid_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucOid_i is NULL\n");
		return hssOamps_Retval_NullPointer;
	}


	if (NULL == pucErrorDesc_i)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," pucErrorDesc_i is NULL\n");
		return hssOamps_Retval_NullPointer;	
	}

	if (NULL == ppucTrapBuff_o)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," ppucTrapBuff_o is NULL\n");
		return hssOamps_Retval_NullPointer;	
	}

	if (NULL != (*ppucTrapBuff_o) )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," *ppucTrapBuff_o is Not NULL\n");
		return hssOamps_Retval_IncorrectParameters;	
	}

	int 				nrc = 0;
	xmlTextWriterPtr	oXMLwriter = NULL;
	xmlBufferPtr 		oXMLbuf = NULL;
	t_hssOampsReturnVal	oRetval = hssOamps_Retval_Success;
	unsigned int		unSizeOfOid=0;
	unsigned int		unSizeOfErrorDesc=0;

	unSizeOfOid = strlen(pucOid_i);
	unSizeOfErrorDesc = strlen(pucErrorDesc_i);


	//do size checks
	if ( unSizeOfOid >= OAMP_OID_LENGTH) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strlen(pucOid_i) >= OAMP_OID_LENGTH , leaving\n");
		return hssOamps_Retval_IncorrectParameters;
	}

	if ( unSizeOfErrorDesc >= OAMP_ERRDESC_LENGTH) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," strlen(pucErrorDesc_i) >= OAMP_ERRDESC_LENGTH , leaving\n");
		return hssOamps_Retval_IncorrectParameters;
	}

	

 

	oXMLbuf = xmlBufferCreate();
	if ( NULL == oXMLbuf)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlBufferCreate Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	oXMLwriter = xmlNewTextWriterMemory(oXMLbuf, 0);
	if ( NULL == oXMLwriter)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlNewTextWriterMemory Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	nrc = xmlTextWriterStartDocument(oXMLwriter, NULL, HSS_OAMPS_XML_ENCODING, NULL);
	if (nrc < 0)
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartDocument Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	/*

    <NOTIFICATION>
  <TRAP>
    <OID>listenPortBindFailed</OID>
    <VALUE>1200</VALUE>
    <SEVERITY>low-severity</SEVERITY>
    <ERRORCODE>1</ERRORCODE>
    <ERRORDESC>Test</ERRORDESC>
  </TRAP>
</NOTIFICATION>
		*/

    /* Start an element named "NOTIFICATION". */
    nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "NOTIFICATION");
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}
    
    /* Start an element named "TRAP" as child of NOTIFICATION. */
    nrc = xmlTextWriterStartElement(oXMLwriter, BAD_CAST "TRAP");
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterStartElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}
    
    
    /* Write an element named "OID" as child of TRAP. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "OID",
                                         "%s", pucOid_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	

    /* Write an element named "VALUE" as child of TRAP. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "VALUE",
                                         "%d", lValue_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

    /* Write an element named "SEVERITY" as child of TRAP. */
	switch (oTrapSeverity_i) 
	{
		case oampsTrapHighSeverity:
			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SEVERITY",
												 "%s", "high-severity");
			
			break;
		
		case oampsTrapLowSeverity:
			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SEVERITY",
													 "%s", "low-severity");
				break;
		
		case oampsTrapMediumSeverity:
			nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "SEVERITY",
													 "%s", "medium-severity");
				break;
	}//end switch

	if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	
	

    /* Write an element named "ERRORCODE" as child of TRAP. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ERRORCODE",
                                         "%d", nErrorcode_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

   
    /* Write an element named "ERRORDESC" as child of VARIABLE. */
    nrc = xmlTextWriterWriteFormatElement(oXMLwriter, BAD_CAST "ERRORDESC",
                                         "%s", pucErrorDesc_i);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterWriteFormatElement Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

    nrc = xmlTextWriterEndDocument(oXMLwriter);
    if (nrc < 0) 
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," xmlTextWriterEndDocument Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}


	
	

	if ( DMBase_Retval_Success !=  DMBase_Malloc(hDiamStackContext_io,
						(void**)ppucTrapBuff_o,oXMLbuf->use +1)
	   )
	{
		DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_ERROR," DMBase_Malloc Failed , leaving\n");
		oRetval = hssOamps_Retval_Failure;
		goto ExitMe;
	}

	memcpy(*ppucTrapBuff_o,
		   oXMLbuf->content,
		   oXMLbuf->use);

	ExitMe:

	if (NULL != oXMLwriter)
	{
		xmlFreeTextWriter(oXMLwriter);
		oXMLwriter = NULL;
	}

	if (NULL != oXMLbuf) 
	{
		xmlBufferFree(oXMLbuf);
		oXMLbuf = NULL;
	}

	DMBASE_TRACE(g_nDiamBaseTraceID, DMBASE_TRACE_LEVEL_INFO," Leaving \n");

	return oRetval;
  
    
}
