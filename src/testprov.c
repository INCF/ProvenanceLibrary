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
    add_attribute(p_record, act_id, "type", "program");
    add_attribute(p_record, act_id, "name", argv[0]);
    add_attribute(p_record, act_id, "version", version);
    char * cmdline = get_cmdline(argc, argv);
    add_attribute(p_record, act_id, "cmdline", cmdline);
    free(cmdline);

    //Add all input parameters. if you use getopt this can be refined further
    for(i=1;i<argc; i++){
        id = newEntity(p_record);
        add_attribute(p_record, id, "type", "input");
        sprintf(arg, "arg%d", i);
        add_attribute(p_record, id, arg, argv[i]);
        newUsedRecord(p_record, act_id, id, NULL);
    }

    id = newEntity(p_record);
    add_attribute(p_record, id, "type", "environment");
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
           add_attribute(p_record, id, name, &thisEnv[pos]);
       free(name);
    }

    id = newEntity(p_record);
    add_attribute(p_record, id, "type", "runtime");
    // add runtime info such as walltime, cputime, host,

    id = newEntity(p_record);
    add_attribute(p_record, id, "type", "output:file");
    add_attribute(p_record, id, "warped_file", "/full/path/to/file");
    newGeneratedByRecord(p_record, id, act_id, NULL);

    id = newEntity(p_record);
    add_attribute(p_record, id, "type", "output:stat");
    add_attribute(p_record, id, "pearson_correlation_coefficient", ".234");
    newGeneratedByRecord(p_record, id, act_id, NULL);


    print_provenance(p_prov, NULL);
    print_provenance(p_prov, "testprov.xml");
    delProvenanceFactory(p_prov);
    return(0);
}
