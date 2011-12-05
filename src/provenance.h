//
//  provenance.h
//  libprov
//
//  Created by Satrajit Ghosh on 11/25/11.
//  Copyright (c) 2011 TankThink Labs LLC. All rights reserved.
//

#ifndef libprov_provenance_h
#define libprov_provenance_h

typedef struct {
  char* id;
  void* private;
}  Provenance;

typedef Provenance Prov, *ProvPtr;
typedef const char *IDREF;
typedef void *RecordPtr;

/*Initialization and Cleanup routines*/
ProvPtr newProvenanceFactory(const char*);
int delProvenanceFactory(ProvPtr);

/*IO routines*/
void print_provenance(ProvPtr, const char*);

/* Record creation routines */
RecordPtr newRecord(ProvPtr);
IDREF newEntity(RecordPtr);
IDREF newActivity(RecordPtr, const char* recipeLink, const char* startTime, const char* endTime);
IDREF newAgent(RecordPtr);
IDREF newNote(RecordPtr);

/* Dependency routines */
IDREF newUsedRecord(RecordPtr p_rec, IDREF activity, IDREF entity, const char* time);
IDREF newGeneratedByRecord(RecordPtr p_rec, IDREF entity, IDREF activity, const char* time);
IDREF newControlledByRecord(RecordPtr p_rec, IDREF activity, IDREF agent, const char* startTime, const char* endTime);
IDREF newDerivedFromRecord(RecordPtr p_rec, IDREF entity_effect, IDREF entity_cause);
IDREF newComplementOfRecord(RecordPtr p_rec, IDREF entity_effect, IDREF entity_cause);
IDREF newAssociatedWithRecord(RecordPtr p_record, IDREF activity, IDREF agent, const char* startTime, const char* endTime);
IDREF newInformedByRecord(RecordPtr p_rec, IDREF activity_effect, IDREF activity_cause, const char* time);
IDREF newHasAnnotationRecord(RecordPtr p_rec, IDREF thing, IDREF note);

/* Record manipulation routines */
int add_attribute(RecordPtr, IDREF, const char* key, const char* value);
int set_property(RecordPtr, IDREF, const char* name, const char* value);

#endif
