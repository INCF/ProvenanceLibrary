//
//  provenance.h
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#ifndef libprov_provenance_h
#define libprov_provenance_h

typedef void *RecordPtr;
typedef struct {
  char* id;
  RecordPtr p_record;
  void* private;
}  Provenance;

typedef Provenance Prov, *ProvPtr;
typedef const char *IDREF;

/*Initialization and Cleanup routines*/
ProvPtr newProvenanceFactory(const char*);
ProvPtr newProvenanceFactoryFromFile(const char*);
ProvPtr newProvenanceFactoryFromMemoryBuffer(const char* buffer, int bufferSize);
int delProvenanceFactory(ProvPtr);
int addNamespace(ProvPtr, const char* href, const char* prefix);

/*IO routines*/
void print_provenance(ProvPtr, const char*);
void dumpToMemoryBuffer(ProvPtr p_prov, char** buffer, int* buffer_size);
void freeMemoryBuffer(char* buffer);

/* Record creation routines */
RecordPtr newAccount(RecordPtr, const char*);
IDREF newEntity(RecordPtr);
IDREF newActivity(RecordPtr, const char* recipeLink, const char* startTime, const char* endTime);
IDREF newAgent(RecordPtr);
IDREF newNote(RecordPtr);

/* Dependency routines */
IDREF newUsedRecord(RecordPtr p_rec, IDREF activity, IDREF entity, const char* time);
IDREF newGeneratedByRecord(RecordPtr p_rec, IDREF entity, IDREF activity, const char* time);
IDREF newAssociatedWithRecord(RecordPtr p_record, IDREF activity, IDREF agent, const char* startTime, const char* endTime);
IDREF newControlledByRecord(RecordPtr p_rec, IDREF activity, IDREF agent, const char* startTime, const char* endTime);
IDREF newDerivedFromRecord(RecordPtr p_rec, IDREF entity_effect, IDREF entity_cause);
IDREF newInformedByRecord(RecordPtr p_rec, IDREF activity_effect, IDREF activity_cause, const char* time);
IDREF newAlternateOfRecord(RecordPtr p_record, IDREF entity1, IDREF entity2);
IDREF newSpecializationOfRecord(RecordPtr p_record, IDREF entity1, IDREF entity2);
IDREF newHasAnnotationRecord(RecordPtr p_rec, IDREF thing, IDREF note);

/* Record manipulation routines */
int addAttribute(RecordPtr p_record, IDREF id, const char * prefix, const char * type, const char* localName, const char* value);
int changeID(RecordPtr, IDREF, const char*);
int addProvAsAccount(RecordPtr p_record, const ProvPtr p_prov, const char *prefix);
int freeID(IDREF id);

#endif
