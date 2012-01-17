//
//  provenance.h
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#ifndef libneuroprov_provenance_h
#define libneuroprov_provenance_h

typedef void *ProcessPtr;
typedef void *ProvObjectPtr;
typedef char *REFID;

/*Initialization and Cleanup routines*/
ProvObjectPtr newProvenanceObject(const char*);
ProvObjectPtr newProvenanceObjectFromFile(const char*);
ProvObjectPtr newProvenanceObjectFromBuffer(const char* buffer, int bufferSize);
int delProvenanceObject(ProvObjectPtr);

/*IO routines*/
void printProvenance(ProvObjectPtr, const char*);
void toBuffer(ProvObjectPtr p_prov, char** buffer, int* buffer_size);
void freeBuffer(char* buffer);

/* Record creation routines */
ProcessPtr newProcess(ProvObjectPtr, const char* startTime, const char* endTime, const char* type);

REFID newProcessInput(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* name, const char* value, const char* type);
REFID newProcessOutput(ProvObjectPtr p_prov, ProcessPtr, const char* name, const char* value, const char* type);
int addKeyValuePair(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* key, const char* value);
int addCommandLine(ProvObjectPtr p_prov, ProcessPtr p_proc, int argc, char** argv);

int addInput(ProvObjectPtr p_prov, ProcessPtr p_proc, REFID input);
int addOutput(ProvObjectPtr p_prov, ProcessPtr p_proc, REFID output);
REFID newFile(ProvObjectPtr p_prov, const char* filename, const char* type);
REFID newFileCollection(ProvObjectPtr p_prov, const char** filenames, int n_files, const char* type);
REFID addEnvironVariable(ProvObjectPtr p_prov, ProcessPtr p_proc, const char* name);
REFID addAllEnvironVariables(ProvObjectPtr p_prov, ProcessPtr p_proc, char **envp);

int addDependency(ProvObjectPtr p_prov, ProcessPtr parent, ProcessPtr child);
int addType(ProvObjectPtr p_prov, REFID id, const char* type);

int changeREFID(ProvObjectPtr p_prov, REFID id, const char*);
int addProvenanceRecord(ProvObjectPtr p_curprov, const ProvObjectPtr p_otherprov, const char *prefix);

#endif
