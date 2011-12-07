#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "provenance.h"
#include "neuroprovenance.h"

/*Initialization and Cleanup routines*/
ProvObjectPtr newProvenanceObject(const char* id)
{
    ProvPtr p_prov = newProvenanceFactory(id);
    // add new namespace
    addNamespace(p_prov, "https://github.com/INCF/ProvenanceLibrary/wiki/terms", "ni");
    return((ProvObjectPtr)p_prov);
}

int delProvenanceObject(ProvObjectPtr p_prov){
    return(delProvenanceFactory((ProvPtr)p_prov));
}

/*IO routines*/
void printProvenance(ProvObjectPtr p_prov, const char* filename)
{
    print_provenance((ProvPtr)p_prov, filename);
}

/* Record creation routines */

/* Create a new process */
ProcessPtr newProcess(ProvObjectPtr p_prov, const char* startTime, const char* endTime, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF act_id = newActivity(p_record, NULL, startTime, endTime);
    if (type != NULL)
        add_child_element(p_record, act_id, "type", type);
    return((ProcessPtr)act_id);
}

/* Associate an input with a process */
REFID newProcessInput(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* name, const char* value, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    if (type == NULL)
        add_child_element(p_record, id, "type", "ni:input");
    else
        add_child_element(p_record, id, "type", type);
    add_child_element(p_record, id, "name", name);
    add_child_element(p_record, id, "value", value);
    newUsedRecord(p_record, (IDREF)p_proc, id, NULL);
    return((REFID)id);
}

/* Associate an output with a process */
REFID newProcessOutput(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* name, const char* value, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    if (type == NULL)
        add_child_element(p_record, id, "type", "ni:output");
    else
        add_child_element(p_record, id, "type", type);
    add_child_element(p_record, id, "name", name);
    add_child_element(p_record, id, "value", value);
    newGeneratedByRecord(p_record, id, (IDREF)p_proc, NULL);
    return((REFID)id);
}

int addInput(ProvObjectPtr p_prov, ProcessPtr p_proc, REFID input)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    newUsedRecord(p_record, (IDREF)p_proc, input, NULL);
    return(0);
}
int addOutput(ProvObjectPtr p_prov, ProcessPtr p_proc, REFID output)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    newGeneratedByRecord(p_record, output, (IDREF)p_proc, NULL);
    return(0);
}

/* Create a new file record */
REFID newFile(ProvObjectPtr p_prov, const char* filename, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    if (type == NULL)
        add_child_element(p_record, id, "type", "ni:file");
    else
        add_child_element(p_record, id, "type", type);
    add_child_element(p_record, id, "path", filename);
    return((REFID)id);
}

/* Create a new file collection record */
REFID newFileCollection(ProvObjectPtr p_prov, const char** filenames, int n_files, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    int i;
    if (type == NULL)
        add_child_element(p_record, id, "type", "ni:filelist");
    else
        add_child_element(p_record, id, "type", type);
    for(i=0; i<n_files; i++)
        add_child_element(p_record, id, "path", filenames[i]);
    return((REFID)id);
}

REFID addEnvironVariable(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* name)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    add_child_element(p_record, id, "type", "ni:environ");
    add_child_element(p_record, id, name, getenv(name));
    return((REFID)id);
}

REFID addAllEnvironVariables(ProvObjectPtr p_prov, ProcessPtr p_proc, char **envp)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = newEntity(p_record);
    char buffer[255];
    int i;
    add_child_element(p_record, id, "type", "ni:environ");
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
           add_child_element(p_record, id, name, &thisEnv[pos]);
       free(name);
    }
    return((REFID)id);
}

int addKeyValuePair(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* key, const char* value)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    add_child_element(p_record, (IDREF)p_proc, key, value);
    return(0);
}

static char * get_cmdline(int argc, char **argv){
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

int addCommandLine(ProvObjectPtr p_prov, ProcessPtr p_proc, int argc, char** argv)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    IDREF id = (IDREF)p_proc;
    char *cmdline = get_cmdline(argc, argv);
    add_child_element(p_record, id, "cmdline", cmdline);
    free(cmdline);
    return(0);
}

int addDependency(ProvObjectPtr p_prov, ProcessPtr parent, ProcessPtr child)
{
    return(0);
}

int addType(ProvObjectPtr p_prov, REFID id, const char* type)
{
    RecordPtr p_record = ((ProvPtr)p_prov)->p_record;
    add_child_element(p_record, id, "type", type);
    return(0);
}
