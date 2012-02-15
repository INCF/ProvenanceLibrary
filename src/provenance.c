//
//  provenance.c
//  libprov
//
//  This library is being developed based on the draft model of the
//  W3C provenance working group.
//  http://dvcs.w3.org/hg/prov/raw-file/0f43d8bc798c/model/ProvenanceModel.html
//  https://github.com/lucmoreau/ProvToolbox/blob/master/xml/src/main/resources
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_SAX1_ENABLED)

#include "provenance.h"

typedef struct {
    xmlDocPtr doc;
} PrivateProv;

typedef PrivateProv *PrivateProvPtr;

typedef struct{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
} XPathQuery;

typedef XPathQuery *XPathQueryPtr;

/* Private U=utility routines */

/*
Query the document for elements using xpath
*/
static XPathQueryPtr query_xpath(const xmlDocPtr doc, const xmlChar* xpathExpr)
{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        return(NULL);
    }
    xpathCtx->namespaces = xmlGetNsList(doc, xmlDocGetRootElement(doc));
    xpathCtx->nsNr = 0;
    if (xpathCtx->namespaces != NULL) {
	while (xpathCtx->namespaces[xpathCtx->nsNr] != NULL)
	    xpathCtx->nsNr++;
    }

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx);
        return(NULL);
    }
    XPathQueryPtr p_xpquery = (XPathQueryPtr)malloc(sizeof(XPathQuery));
    p_xpquery->xpathCtx = xpathCtx;
    p_xpquery->xpathObj = xpathObj;
    return p_xpquery;
}

static int free_xpquery(XPathQueryPtr p_xpquery){
    xmlXPathFreeObject(p_xpquery->xpathObj);
    xmlXPathFreeContext(p_xpquery->xpathCtx);
    free(p_xpquery);
    return(0);
}

/*
  Get the namespace prefix associated with the node
*/
static xmlNsPtr getNs(xmlNodePtr p_node, const char *prefix)
{
    xmlNsPtr ns = NULL;
    xmlNodePtr root_node = xmlDocGetRootElement(p_node->doc);
    if (prefix == NULL)
	ns = xmlSearchNs(p_node->doc, root_node, BAD_CAST "prov");
    else
	ns = xmlSearchNs(p_node->doc, root_node, BAD_CAST prefix);
    if (ns == NULL) {
	if (prefix == NULL)
	    fprintf(stderr, "Prov namespace not defined.\n");
	else
	    fprintf(stderr, "Namespace %s not defined. Use AddNamespace().\n", prefix);
    }
    /*
    else
	fprintf(stderr, "Namespace: %s\n", ns->prefix);
    */
    return(ns);
}

/*
Generic element adding routine
*/
static xmlNodePtr add_element(xmlNodePtr p_record,
                              const char* root,
                              const char* element_name,
                              const char* id_prefix)
{
    xmlNodePtr p_node, root_node;
    xmlChar id[255];
    int size1, size2;
    xmlChar xpathExpr[255];

    if (root == NULL)
	sprintf(xpathExpr, "//prov:%s", element_name);
    else
	sprintf(xpathExpr, "//prov:%s/prov:%s", root, element_name);
    if (id_prefix != NULL){
	strcat(xpathExpr, "[@prov:id]");
    }
    //fprintf(stderr, "Query: %s\n", xpathExpr);
    XPathQueryPtr p_xpquery = query_xpath(p_record->doc,
                                          xpathExpr);

    size1 = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    free_xpquery(p_xpquery);

    if (root != NULL){
	sprintf(xpathExpr, "//prov:%s", root);
        p_xpquery = query_xpath(p_record->doc, xpathExpr);

        size2 = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
        if (size2 == 0)
            root_node = xmlNewChild(p_record, NULL, BAD_CAST root, NULL);
        else
            root_node = p_xpquery->xpathObj->nodesetval->nodeTab[0];
        free_xpquery(p_xpquery);
    }
    else
        root_node = p_record;

    xmlNsPtr ns = getNs(root_node, NULL);
    p_node = xmlNewChild(root_node, ns, BAD_CAST element_name, NULL);
    if (id_prefix != NULL){
        sprintf(id, "%s_%d", id_prefix, size1);
	//fprintf(stderr, "id: %s\n", id);
	xmlNewNsProp(p_node, ns, BAD_CAST "id", BAD_CAST id);
    }
    return p_node;
}

/* Add a records element to a node
*/
static RecordPtr newRecord(ProvPtr p_prov)
{
    assert(p_prov);
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    XPathQueryPtr p_xpquery = query_xpath(p_priv->doc, "/prov:container");

    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    if (size != 1){
        fprintf(stderr, "No container element found or multiple elements found\n");
        return NULL;
    }
    return (RecordPtr)add_element(p_xpquery->xpathObj->nodesetval->nodeTab[0], NULL, BAD_CAST "records", NULL);
}


/*Initialization and Cleanup routines*/
/*
Create a provenance object that can be used by a client to generate an
instance of a provenance schema
*/
ProvPtr newProvenanceFactory(const char* id)
{
    //fprintf(stdout, "Creating provenance object [BEGIN]\n");
    LIBXML_TEST_VERSION;
    ProvPtr p_prov = (ProvPtr)malloc(sizeof(Provenance));
    if(p_prov == NULL) {
        fprintf(stderr, "Error: unable to create Provenance struct\n");
        return(NULL);
    }
    assert(p_prov);
    assert(id);
    p_prov->id = strdup(id);
    p_prov->private = (void *)malloc(sizeof(PrivateProv));

    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    p_priv->doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "container");
    xmlDocSetRootElement(p_priv->doc, root_node);
    root_node->ns = xmlNewNs(root_node, "http://openprovenance.org/prov-xml#", "prov");
    xmlNewNs(root_node, "http://www.w3.org/2001/XMLSchema-instance", "xsi");
    xmlNewNs(root_node, "http://www.w3.org/2001/XMLSchema", "xsd");
    xmlNewNsProp(root_node, root_node->ns, BAD_CAST "id", BAD_CAST id);

    //fprintf(stdout, "Creating provenance object [END]\n");
    p_prov->p_record = (void *)newRecord(p_prov);
    return(p_prov);
}

/* Read a provenance document from a file */
ProvPtr newProvenanceFactoryFromFile(const char* filename)
{
    LIBXML_TEST_VERSION;
    ProvPtr p_prov = (ProvPtr)malloc(sizeof(Provenance));
    if(p_prov == NULL) {
        fprintf(stderr, "Error: unable to create Provenance struct\n");
        return(NULL);
    }
    assert(p_prov);
    p_prov->private = (void *)malloc(sizeof(PrivateProv));

    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    p_priv->doc = xmlReadFile(filename, NULL, 0);
    if (p_priv->doc == NULL) {
        fprintf(stderr, "error: could not parse file %s\n", filename);
        free(p_prov->private);
        free(p_prov);
        return(NULL);
    }
    xmlNodePtr root_node = xmlDocGetRootElement(p_priv->doc);
    p_prov->id = strdup(xmlGetProp(root_node, BAD_CAST "id"));
    XPathQueryPtr p_xpquery = query_xpath(p_priv->doc, "/prov:container/prov:records");
    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    if (size != 1){
        fprintf(stderr, "No records element found\n");
        free(p_prov->private);
        free(p_prov);
        free_xpquery(p_xpquery);
        return NULL;
    }
    p_prov->p_record = (void *)p_xpquery->xpathObj->nodesetval->nodeTab[0];
    free_xpquery(p_xpquery);
    return(p_prov);
}

/* Construct tree from a provenance model in memory */
ProvPtr newProvenanceFactoryFromMemoryBuffer(const char* buffer, int bufferSize)
{
    LIBXML_TEST_VERSION;
    ProvPtr p_prov = (ProvPtr)malloc(sizeof(Provenance));
    if(p_prov == NULL) {
        fprintf(stderr, "Error: unable to create Provenance struct\n");
        return(NULL);
    }
    assert(p_prov);
    p_prov->private = (void *)malloc(sizeof(PrivateProv));

    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    p_priv->doc = xmlReadMemory((xmlChar*)buffer, bufferSize, "noname.xml", NULL, 0);
    if (p_priv->doc == NULL) {
        fprintf(stderr, "error: could not parse memory buffer\n");
        free(p_prov->private);
        free(p_prov);
        return(NULL);
    }
    //fprintf(stdout, "printing doc\n");
    //xmlSaveFormatFileEnc("-", p_priv->doc, "UTF-8", 1);
    xmlNodePtr root_node = xmlDocGetRootElement(p_priv->doc);
    assert(root_node);
    xmlChar* p_id = xmlGetProp(root_node, BAD_CAST "id");
    assert(p_id);
    p_prov->id = strdup(p_id);
    XPathQueryPtr p_xpquery = query_xpath(p_priv->doc, "/prov:container/prov:records");
    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    if (size != 1){
        fprintf(stderr, "%d records element found in container id[%s] \n", size, p_prov->id);
        free(p_prov->id);
        free(p_prov->private);
        free(p_prov);
        free_xpquery(p_xpquery);
        return NULL;
    }
    p_prov->p_record = (void *)p_xpquery->xpathObj->nodesetval->nodeTab[0];
    free_xpquery(p_xpquery);
    return(p_prov);
}

/*
Clean up the memory used by the xml document
*/
int delProvenanceFactory(ProvPtr p_prov)
{
    assert(p_prov);
    //fprintf(stdout, "Destroying provenance object [BEGIN]\n");
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);

    /*free the document */
    xmlFreeDoc(p_priv->doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    free(p_priv);
    free(p_prov->id);
    free(p_prov);
    //fprintf(stdout, "Destroying provenance object [END]\n");

    return(0);
}

int addNamespace(ProvPtr p_prov, const char* href, const char* prefix)
{
    assert(p_prov);
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    xmlNodePtr root_node = xmlDocGetRootElement(p_priv->doc);
    xmlNewNs(root_node, href, prefix);
    return(0);
}

/*
print provenance info to stdout (filename==NULL) or to a file
*/
void print_provenance(ProvPtr p_prov, const char *filename)
{
    assert(p_prov);
    //fprintf(stdout, "Printing provenance\n");
    //fprintf(stdout, "ID: %d\n", p_prov->id);

    /*Get the root element node */
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    if (filename == NULL)
        xmlSaveFormatFileEnc("-", p_priv->doc, "UTF-8", 1);
    else
        xmlSaveFormatFileEnc(BAD_CAST filename, p_priv->doc, "UTF-8", 1);
}

/*
return provenance as a string buffer
*/
void dumpToMemoryBuffer(ProvPtr p_prov, char** buffer, int* p_buffer_size)
{
    assert(p_prov);

    /*Get the root element node */
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    xmlDocDumpFormatMemory(p_priv->doc, (xmlChar**)buffer, p_buffer_size, 1);
}

/*
free the memory buffer
*/
void freeMemoryBuffer(char* buffer)
{
    assert(buffer);
    xmlFree((xmlChar*)buffer);
}

/* Record creation routines */

RecordPtr newAccount(RecordPtr p_record, const char* asserter)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, NULL, "account", "ac");
    if (asserter != NULL)
        xmlNewChild(p_node, NULL, BAD_CAST "asserter", BAD_CAST asserter);
    return (RecordPtr)add_element(p_node, NULL, BAD_CAST "records", NULL);
}

IDREF newEntity(RecordPtr p_record)
{
    return(xmlGetProp(add_element((xmlNodePtr)p_record, NULL, "entity", "e"), "id"));
}

IDREF newActivity(RecordPtr p_record, const char* recipeLink,
                   const char* startTime, const char* endTime)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, NULL, "activity", "a");
    if (startTime != NULL)
        xmlNewChild(p_node, NULL, BAD_CAST "startTime", BAD_CAST startTime);
    if (endTime != NULL)
        xmlNewChild(p_node, NULL, BAD_CAST "endTime", BAD_CAST endTime);
    IDREF id = (IDREF)xmlGetProp(p_node, BAD_CAST "id");
    assert(id);
    return(id);
}

IDREF newAgent(RecordPtr p_record)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, NULL, "agent", "ag");
    return(xmlGetProp(p_node, "id"));
}

IDREF newNote(RecordPtr p_record)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, NULL, "note", "n");
    return(xmlGetProp(p_node, "id"));
}

/* add relations */
IDREF newUsedRecord(RecordPtr p_record, IDREF activity, IDREF entity, const char* time)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "used", "u");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "activity", NULL), ns, BAD_CAST "ref", BAD_CAST activity);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity", NULL), ns, BAD_CAST "ref", BAD_CAST entity);
    if (time != NULL)
	xmlNewChild(p_node, ns, BAD_CAST "time", BAD_CAST time);
    return(xmlGetProp(p_node, "id"));
}

IDREF newGeneratedByRecord(RecordPtr p_record, IDREF entity, IDREF activity, const char* time)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "wasGeneratedBy", "wgb");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity", NULL), ns, BAD_CAST "ref", BAD_CAST entity);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "activity", NULL), ns, BAD_CAST "ref", BAD_CAST activity);
    if (time != NULL)
	xmlNewChild(p_node, ns, BAD_CAST "time", BAD_CAST time);
    return(xmlGetProp(p_node, "id"));
}

IDREF newAssociatedWithRecord(RecordPtr p_record, IDREF activity, IDREF agent, const char* startTime, const char* endTime)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "wasAssociatedWith", "waw");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "activity", NULL), ns, BAD_CAST "ref", BAD_CAST activity);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "agent", NULL), ns, BAD_CAST "ref", BAD_CAST agent);
    return(xmlGetProp(p_node, "id"));
}

IDREF newControlledByRecord(RecordPtr p_record, IDREF activity, IDREF agent, const char* startTime, const char* endTime)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "wasControlledBy", "wcb");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "effect", NULL), ns, BAD_CAST "ref", BAD_CAST activity);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "cause", NULL), ns, BAD_CAST "ref", BAD_CAST agent);
    if (startTime != NULL)
	xmlNewChild(p_node, ns, BAD_CAST "startTime", BAD_CAST startTime);
    if (endTime != NULL)
	xmlNewChild(p_node, ns, BAD_CAST "endTime", BAD_CAST endTime);
    return(xmlGetProp(p_node, "id"));
}

IDREF newDerivedFromRecord(RecordPtr p_record, IDREF entity_effect, IDREF entity_cause)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "wasDerivedFrom", "wdf");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "effect", NULL), ns, BAD_CAST "ref", BAD_CAST entity_effect);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "cause", NULL), ns, BAD_CAST "ref", BAD_CAST entity_cause);
    return(xmlGetProp(p_node, "id"));
}

IDREF newInformedByRecord(RecordPtr p_record, IDREF activity_effect, IDREF activity_cause, const char* time)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "wasInformedBy", "wib");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "effect", NULL), ns, BAD_CAST "ref", BAD_CAST activity_effect);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "cause", NULL), ns, BAD_CAST "ref", BAD_CAST activity_cause);
    if (time != NULL)
	xmlNewChild(p_node, ns, BAD_CAST "time", BAD_CAST time);
    return(xmlGetProp(p_node, "id"));
}

IDREF newAlternateOfRecord(RecordPtr p_record, IDREF entity1, IDREF entity2)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "alternateOf", "ao");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity1", NULL), ns, BAD_CAST "ref", BAD_CAST entity1);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity2", NULL), ns, BAD_CAST "ref", BAD_CAST entity2);
    return(xmlGetProp(p_node, "id"));
}

IDREF newSpecializationOfRecord(RecordPtr p_record, IDREF entity1, IDREF entity2)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "specializationOf", "so");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity1", NULL), ns, BAD_CAST "ref", BAD_CAST entity1);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "entity2", NULL), ns, BAD_CAST "ref", BAD_CAST entity2);
    return(xmlGetProp(p_node, "id"));
}

IDREF newHasAnnotationRecord(RecordPtr p_record, IDREF thing, IDREF note)
{
    xmlNodePtr p_node = add_element((xmlNodePtr)p_record, "dependencies", "hasAnnotation", "ha");
    xmlNsPtr ns = getNs(p_node, NULL);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "thing", NULL), ns, BAD_CAST "ref", BAD_CAST thing);
    xmlNewNsProp(xmlNewChild(p_node, ns, BAD_CAST "note", NULL), ns, BAD_CAST "ref", BAD_CAST note);
    return(xmlGetProp(p_node, "id"));
}

/* Record manipulation routines */

/*
Add key value elements with given id
*/
int addAttribute(RecordPtr p_record, IDREF id, const char * prefix, const char* type, const char* localName, const char* value)
{
    xmlChar xpathExpr[128];
    assert(id);
    sprintf(xpathExpr, "//*[@prov:id='%s']", id);
    XPathQueryPtr p_xpquery = query_xpath(((xmlNodePtr)p_record)->doc,
                                          xpathExpr);
    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    //fprintf(stdout, "xpath[%s]:numnodes[%d]\n", xpathExpr, size);
    if (size==0){
       fprintf(stderr, "Element with id=%s not found.\n", id);
       /* Cleanup */
       free_xpquery(p_xpquery);
       return(1);
    }
    //fprintf(stdout, "=====%s=====\n", id);
    if (size>1){
       fprintf(stderr, "addAttribute: Multiple elements found [n=%d]. Doc invalid. [id=%s]\n", size, id);
       /* Cleanup */
       free_xpquery(p_xpquery);
       return(1);
    }
    xmlNodePtr p_node = p_xpquery->xpathObj->nodesetval->nodeTab[0];
    assert(localName);
    assert(value);
    xmlNsPtr ns = getNs(p_node, prefix);
    if (ns == NULL) {
	free_xpquery(p_xpquery);
	return(1);
    }
    xmlNodePtr p_child = xmlNewChild(p_node, ns, BAD_CAST localName, BAD_CAST value);
    if (type != NULL)
	xmlNewProp(p_child, "xsi:type", type);
    //fprintf(stderr,"%s=%s\n", localName, value);
    //xmlSaveFormatFileEnc("-", ((xmlNodePtr)p_record)->doc, "UTF-8", 1);
    /* Cleanup */
    free_xpquery(p_xpquery);
    return(0);
}

int changeID(RecordPtr p_record, IDREF id, const char* new_id)
{
    xmlChar xpathExpr[128];
    sprintf(xpathExpr, "//*[@prov:id='%s']", id);
    XPathQueryPtr p_xpquery = query_xpath(((xmlNodePtr)p_record)->doc,
                                          xpathExpr);
    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    //fprintf(stdout, "xpath[%s]:numnodes[%d]\n", xpathExpr, size);
    if (size==0){
       fprintf(stderr, "Element with id=%s not found.\n", id);
       /* Cleanup */
       free_xpquery(p_xpquery);
       return(1);
    }
    if (size>1){
       fprintf(stderr, "changeID: Multiple elements found. Doc invalid.\n");
       /* Cleanup */
       free_xpquery(p_xpquery);
       return(1);
    }
    xmlNodePtr p_node = p_xpquery->xpathObj->nodesetval->nodeTab[0];
    assert(new_id);
    xmlSetProp(p_node, BAD_CAST "id", BAD_CAST new_id);

    /* Cleanup */
    free_xpquery(p_xpquery);
    return(0);
}

static void change_refs(xmlNodePtr p_node, char *prefix)
{
    xmlNodePtr p_curnode = NULL;
    char newid[255];

    for (p_curnode = p_node; p_curnode; p_curnode = p_curnode->next) {
        if (p_curnode->type == XML_ELEMENT_NODE) {
	    if (xmlHasProp(p_curnode, BAD_CAST "id") != NULL){
		sprintf(newid, "%s_%s", prefix, xmlGetProp(p_curnode, BAD_CAST"id"));
		xmlSetProp(p_curnode, BAD_CAST "prov:id", BAD_CAST newid);
            }
	    if (xmlHasProp(p_curnode, BAD_CAST "ref") != NULL){
		sprintf(newid, "%s_%s", prefix, xmlGetProp(p_curnode, BAD_CAST"ref"));
		xmlSetProp(p_curnode, BAD_CAST "prov:ref", BAD_CAST newid);
            }
        }
        change_refs(p_curnode->children, prefix);
    }
}

int addProvAsAccount(RecordPtr p_record, const ProvPtr p_prov, const char *prefix)
{
    // create new account record
    RecordPtr p_newrecord = newAccount(p_record, NULL);
    xmlNodePtr p_account = ((xmlNodePtr)p_newrecord)->parent;
    xmlUnlinkNode((xmlNodePtr)p_newrecord);
    xmlFreeNode((xmlNodePtr)p_newrecord);

    char new_prefix[255];
    if (prefix != NULL)
        sprintf(new_prefix, "%s", prefix);
    else
	sprintf(new_prefix, "%s", xmlGetProp(p_account, "id") );
    //fprintf(stdout, "new prefix: %s\n", new_prefix);

    //copy contents of provenance record
    PrivateProvPtr p_priv = (PrivateProvPtr)(p_prov->private);
    XPathQueryPtr p_xpquery = query_xpath(p_priv->doc, "/prov:container/prov:records");
    int size = (p_xpquery->xpathObj->nodesetval) ? p_xpquery->xpathObj->nodesetval->nodeNr : 0;
    //fprintf(stdout, "xpath[%s]:numnodes[%d]\n", xpathExpr, size);
    if (size==0){
        fprintf(stderr, "Provenance contains no records element.\n");
        // Cleanup
        xmlUnlinkNode(p_account);
        xmlFreeNode(p_account);
        free_xpquery(p_xpquery);
        return(1);
    }
    p_newrecord = (RecordPtr)xmlCopyNode(p_xpquery->xpathObj->nodesetval->nodeTab[0], 1);
    xmlAddChild(p_account, (xmlNodePtr)p_newrecord);

    change_refs((xmlNodePtr)p_newrecord, new_prefix);

    /* Cleanup */
    free_xpquery(p_xpquery);
    return(0);

}

int freeID(IDREF id)
{
    xmlFree((xmlChar*)id);
    return(0);
}
#endif
