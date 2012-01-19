//  Test routine for provenance library
//  Requires:
//     libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "provenance.h"

const char* version = "1.0.0";

char * get_cmdline(int argc, char **argv){
    int total_len = 0, i;
    for(i = 0; i < argc; i++)
        total_len += strlen(argv[i]) + 1;
    char * cmdline = (char *)malloc(total_len*sizeof(char));
    char * p_index = cmdline;
    for(i = 0; i < argc; i++){
     strcpy(p_index, argv[i]);
     if ((i + 1) < argc) strcat(p_index, " ");
     p_index += (strlen(argv[i]) + 1);
    }
    return cmdline;
}

/* A simple example on using the provenance library
*/
int
main(int argc, char **argv, char** envp)
{
    ProvPtr p_prov = newProvenanceFactory("1");
    RecordPtr p_record = p_prov->p_record;
    IDREF id, act_id;
    char arg[50];
    int i;

    // Add program information
    act_id = newActivity(p_record, NULL, "11/30/11 00:13:20.650432 EST", "11/30/11 00:13:20.650550 EST");
    addAttribute(p_record, act_id, "prov", "type", "program");
    addAttribute(p_record, act_id, "ni", "name", argv[0]);
    addAttribute(p_record, act_id, "ni", "version", version);
    char * cmdline = get_cmdline(argc, argv);
    addAttribute(p_record, act_id, "ni", "cmdline", cmdline);
    free(cmdline);

    //Add all input parameters. if you use getopt this can be refined further
    for(i=1;i<argc; i++){
        id = newEntity(p_record);
        addAttribute(p_record, id, "prov", "type", "input");
        sprintf(arg, "arg%d", i);
        addAttribute(p_record, id, NULL, arg, argv[i]);
        newUsedRecord(p_record, act_id, id, NULL);
    }

    id = newEntity(p_record);
    addAttribute(p_record, id, "prov", "type", "environment");
    // add all environment variables
    char** env;
    for (env = envp; *env != 0; env++)
    {
       char* thisEnv = *env;
       char *name;
       char* p_index = thisEnv;
       int pos = 0;
       while (thisEnv[pos++] != '=');
       name = strndup(thisEnv, pos-1);
       if (name[0] != '_')
           addAttribute(p_record, id, NULL, name, &thisEnv[pos]);
       free(name);
    }

    id = newEntity(p_record);
    addAttribute(p_record, id, "prov", "type", "runtime");
    // add runtime info such as walltime, cputime, host,

    id = newEntity(p_record);
    addAttribute(p_record, id, "prov", "type", "output:file");
    addAttribute(p_record, id, "ni", "warped_file", "/full/path/to/file");
    newGeneratedByRecord(p_record, id, act_id, NULL);

    id = newEntity(p_record);
    addAttribute(p_record, id, "prov", "type", "output:stat");
    addAttribute(p_record, id, "ni", "pearson_correlation_coefficient", ".234");
    newGeneratedByRecord(p_record, id, act_id, NULL);

    /* Test i/o manipulations */
    char *buffer;
    int bufsize;
    print_provenance(p_prov, "testprov.xml");
    dumpToMemoryBuffer(p_prov, &buffer, &bufsize);
    delProvenanceFactory(p_prov);
    p_prov = newProvenanceFactoryFromMemoryBuffer(buffer, bufsize);
    freeMemoryBuffer(buffer);
    delProvenanceFactory(p_prov);
    p_prov = newProvenanceFactoryFromFile("testprov.xml");
    ProvPtr p_prov2 = newProvenanceFactory("1");
    addProvAsAccount(p_prov2->p_record, p_prov, NULL);
    print_provenance(p_prov2, NULL);
    print_provenance(p_prov2, "testprov2.xml");
    delProvenanceFactory(p_prov);
    delProvenanceFactory(p_prov2);

    return(0);
}
