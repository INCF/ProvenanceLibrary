//
//  testprov.c
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
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
    ProvPtr prov_ptr = create_provenance_object(1);
    IDREF id, act_id;
    char arg[50];
    int i;

    //Add all input parameters. if you use getopt this can be refined further
    for(i=1;i<argc; i++){
        id = add_entity(prov_ptr);
        add_attribute(prov_ptr, id, "type", "input");
        sprintf(arg, "arg%d", i);
        add_attribute(prov_ptr, id, arg, argv[i]);
    }

    // Add program information
    act_id = add_activity(prov_ptr, NULL, "11/30/11 00:13:20.650432 EST", "11/30/11 00:13:20.650550 EST");
    add_attribute(prov_ptr, act_id, "type", "prov:program");
    add_attribute(prov_ptr, act_id, "name", argv[0]);
    add_attribute(prov_ptr, act_id, "version", version);
    char * cmdline = get_cmdline(argc, argv);
    add_attribute(prov_ptr, act_id, "cmdline", cmdline);
    free(cmdline);

    id = add_entity(prov_ptr);
    add_attribute(prov_ptr, id, "type", "environment");
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
           add_attribute(prov_ptr, id, name, &thisEnv[pos]);
       free(name);
    }

    id = add_entity(prov_ptr);
    add_attribute(prov_ptr, id, "type", "runtime");
    // add runtime info such as walltime, cputime, host,

    id = add_entity(prov_ptr);
    add_attribute(prov_ptr, id, "type", "output:file");
    add_attribute(prov_ptr, id, "warped_file", "/full/path/to/file");
    add_generationRecord(prov_ptr, id, act_id, NULL);

    id = add_entity(prov_ptr);
    add_attribute(prov_ptr, id, "type", "output:stat");
    add_attribute(prov_ptr, id, "pearson_correlation_coefficient", "23.4");
    add_generationRecord(prov_ptr, id, act_id, NULL);


    print_provenance(prov_ptr, NULL);
    print_provenance(prov_ptr, "testprov.xml");
    destroy_provenance_object(prov_ptr);
    return(0);
}
