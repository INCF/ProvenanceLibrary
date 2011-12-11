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

#include "neuroprovenance.h"

const char* version = "1.0.0";


/* A simple example on using the provenance library
*/
int
main(int argc, char **argv, char** envp)
{
    ProvObjectPtr p_prov = newProvenanceObject("BET");
    ProcessPtr p_proc;
    REFID id, act_id;
    char arg[50];
    int i;

    // Add program information
    p_proc = newProcess(p_prov,
                        "11/30/11 00:13:20.650432 EST",
                        "11/30/11 00:13:20.650550 EST",
                        "ni:brain_extraction");
    addCommandLine(p_prov, p_proc, argc, argv);

    addKeyValuePair(p_prov, p_proc, "program", argv[0]);
    addKeyValuePair(p_prov, p_proc, "version", version);

    //Add all input parameters. if you use getopt this can be refined further
    for(i=1;i<argc; i++){
        sprintf(arg, "arg%d", i);
        newProcessInput(p_prov, p_proc, arg, argv[i], NULL);
    }

    addAllEnvironVariables(p_prov, p_proc, envp);

    // [TODO]: add runtime info such as walltime, cputime, host,

    id = newFile(p_prov, "./testneuroprov.c", "ni:output");
    addKeyValuePair(p_prov, id, "name", "warped_file");
    addType(p_prov, id, "ni:normalized");
    addOutput(p_prov, p_proc, id);

    id = newProcessOutput(p_prov, p_proc, "pearson_correlation_coefficient", ".234", "ni:output");
    addType(p_prov, id, "ni:statistic");

    printProvenance(p_prov, NULL);
    printProvenance(p_prov, "testneuroprov.xml");
    delProvenanceObject(p_prov);
    return(0);
}
