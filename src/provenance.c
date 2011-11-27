//
//  provenance.c
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "provenance.h"

typedef struct {
    xmlDocPtr doc;
    xmlNodePtr entity_node, activity_node, agent_node, note_node;
    xmlNodePtr generation_node, usage_node, derivation_node, activityAssociation_node,
               responsibility_node, start_node, end_node, complement_node, annotation_node;
} PrivateProv;

typedef PrivateProv *PrivateProvPtr;

ProvPtr create_provenance_object(int id)
{
    printf("Creating provenance object [BEGIN]\n");
    LIBXML_TEST_VERSION;
    ProvPtr prov_ptr = (ProvPtr)malloc(sizeof(Provenance));
    if (prov_ptr != NULL){
        prov_ptr->id = id;
        prov_ptr->private = (void *)malloc(sizeof(PrivateProv));

        PrivateProvPtr priv_ptr = (PrivateProvPtr)(prov_ptr->private);
        priv_ptr->doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "opmx");
        xmlDocSetRootElement(priv_ptr->doc, root_node);

        // Create child nodes
        xmlNodePtr element_node = xmlNewChild(root_node, NULL, BAD_CAST "element", NULL);
        xmlNodePtr relation_node = xmlNewChild(root_node, NULL, BAD_CAST "relation", NULL);
        xmlNodePtr account_node = xmlNewChild(root_node, NULL, BAD_CAST "account", NULL);

        // Create record subtypes
        priv_ptr->entity_node = xmlNewChild(element_node, NULL, BAD_CAST "entities", NULL);
        priv_ptr->activity_node = xmlNewChild(element_node, NULL, BAD_CAST "activities", NULL);
        priv_ptr->agent_node = xmlNewChild(element_node, NULL, BAD_CAST "agents", NULL);
        priv_ptr->note_node = xmlNewChild(element_node, NULL, BAD_CAST "notes", NULL);
        printf("Creating provenance object [END]\n");
    }
    return prov_ptr;
}

int destroy_provenance_object(ProvPtr prov_ptr)
{
    if (prov_ptr == NULL) return(0);
    printf("Destroying provenance object [BEGIN]\n");
    PrivateProvPtr priv_ptr = (PrivateProvPtr)(prov_ptr->private);
    
    /*free the document */
    xmlFreeDoc(priv_ptr->doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    free(priv_ptr);
    free(prov_ptr);
    printf("Destroying provenance object [END]\n");

    return(0);
}

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}


void print_provenance(ProvPtr prov_ptr)
{
    if (prov_ptr == NULL) return;
    printf("Printing provenance\n");
    printf("ID: %d\n", prov_ptr->id);

    /*Get the root element node */
    PrivateProvPtr priv_ptr = (PrivateProvPtr)(prov_ptr->private);
    //print_element_names(xmlDocGetRootElement(priv_ptr->doc));
    xmlSaveFormatFileEnc("-", priv_ptr->doc, "UTF-8", 1);
}


int
testapi()
{
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;/* node pointers */
    xmlDtdPtr dtd = NULL;       /* DTD pointer */
    char buff[256];
    int i, j;
    
    LIBXML_TEST_VERSION;
    
    /* 
     * Creates a new document, a node and set it as a root node
     */
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    
    /*
     * Creates a DTD declaration. Isn't mandatory. 
     */
    dtd = xmlCreateIntSubset(doc, BAD_CAST "root", NULL, BAD_CAST "tree2.dtd");
    
    /* 
     * xmlNewChild() creates a new node, which is "attached" as child node
     * of root_node node. 
     */
    xmlNewChild(root_node, NULL, BAD_CAST "node1",
                BAD_CAST "content of node 1");
    /* 
     * The same as above, but the new child node doesn't have a content 
     */
    xmlNewChild(root_node, NULL, BAD_CAST "node2", NULL);
    
    /* 
     * xmlNewProp() creates attributes, which is "attached" to an node.
     * It returns xmlAttrPtr, which isn't used here.
     */
    node =
    xmlNewChild(root_node, NULL, BAD_CAST "node3",
                BAD_CAST "this node has attributes");
    xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");
    xmlNewProp(node, BAD_CAST "foo", BAD_CAST "bar");
    
    /*
     * Here goes another way to create nodes. xmlNewNode() and xmlNewText
     * creates a node and a text node separately. They are "attached"
     * by xmlAddChild() 
     */
    node = xmlNewNode(NULL, BAD_CAST "node4");
    node1 = xmlNewText(BAD_CAST
                       "other way to create content (which is also a node)");
    xmlAddChild(node, node1);
    xmlAddChild(root_node, node);
    
    /* 
     * A simple loop that "automates" nodes creation 
     */
    for (i = 5; i < 7; i++) {
        sprintf(buff, "node%d", i);
        node = xmlNewChild(root_node, NULL, BAD_CAST buff, NULL);
        for (j = 1; j < 4; j++) {
            sprintf(buff, "node%d%d", i, j);
            node1 = xmlNewChild(node, NULL, BAD_CAST buff, NULL);
            xmlNewProp(node1, BAD_CAST "odd", BAD_CAST((j % 2) ? "no" : "yes"));
        }
    }
    
    /* 
     * Dumping document to stdio or file
     */
    xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    
    /*free the document */
    xmlFreeDoc(doc);
    
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
    return(0);
}
